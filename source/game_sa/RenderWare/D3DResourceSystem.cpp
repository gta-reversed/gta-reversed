/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/

#include "StdInc.h"

#include "D3DResourceSystem.h"

// 0x730830
void D3DResourceSystem::Init() {
    ms_NumTextureBuffers       = 1;
    ms_UseD3DResourceBuffering = true;
    ms_TextureBuffers[0].Setup(D3DFMT_UNKNOWN, 0, -1, INITIAL_BUFFER_CAPACITY);
    for (auto i = 0u; i < NUM_SIZED_INDEX_DATA_BUFFERS; i++) {
        ms_IndexDataBuffers[i].Setup(D3DFMT_INDEX16, (i + 1) * NUM_INDICES_PER_SIZE_CLASS, INITIAL_BUFFER_CAPACITY);
    }
    ms_IndexDataBuffers.back().Setup(D3DFMT_INDEX16, 0, INITIAL_BUFFER_CAPACITY);
}

// 0x730A00
void D3DResourceSystem::Shutdown() {
    for (auto& buffer : GetTextureBuffers()) {
        buffer.Clear();
    }
    ms_NumTextureBuffers = 0;
    for (auto& buffer : ms_IndexDataBuffers) {
        buffer.Clear();
    }
}

// 0x730900
void D3DResourceSystem::CancelBuffering() {
    for (auto& buffer : GetTextureBuffers()) {
        buffer.Destroy();
    }
    ms_NumTextureBuffers = 0;
    for (auto& buffer : ms_IndexDataBuffers) {
        buffer.Destroy();
    }
    ms_UseD3DResourceBuffering = false;
}

// 0x730AC0
void D3DResourceSystem::SetUseD3DResourceBuffering(bool use) {
    ZoneScoped;

    ms_UseD3DResourceBuffering = use;
}

// 0x730660
uint32 D3DResourceSystem::GetTotalPixelsSize() {
    uint32 total = 0;
    for (const auto& buffer : GetTextureBuffers()) {
        total += buffer.GetTotalDataSize();
    }
    return total;
}

// 0x7307F0
uint32 D3DResourceSystem::GetTotalIndexDataSize() {
    uint32 total = 0;
    for (const auto& buffer : ms_IndexDataBuffers) {
        total += buffer.GetTotalDataSize();
    }
    return total;
}

// 0x7305E0
void D3DResourceSystem::TidyUpD3DTextures(uint32 count) {
    // Release `count` textures, taking one from each buffer in turn
    auto numToRelease = count;
    for (auto numEmptyInARow = 0u; numToRelease && numEmptyInARow < ms_NumTextureBuffers; ms_TidyUpTextureBufferIdx = (ms_TidyUpTextureBufferIdx + 1) % ms_NumTextureBuffers) {
        if (auto* const texture = ms_TextureBuffers[ms_TidyUpTextureBufferIdx].PopLast()) {
            texture->Release();
            numEmptyInARow = 0;
            numToRelease--;
        } else {
            numEmptyInARow++;
        }
    }
}

// 0x730740
void D3DResourceSystem::TidyUpD3DIndexBuffers(uint32 count) {
    // First the big ones, then one from each of the others in turn
    auto numToRelease = count;
    for (; numToRelease; numToRelease--) {
        auto* const indexBuffer = ms_IndexDataBuffers.back().PopLast();
        if (!indexBuffer) {
            break;
        }
        indexBuffer->Release();
    }
    for (auto numEmptyInARow = 0u; numToRelease && numEmptyInARow < NUM_SIZED_INDEX_DATA_BUFFERS; ms_TidyUpIndexDataBufferIdx = (ms_TidyUpIndexDataBufferIdx + 1) % NUM_SIZED_INDEX_DATA_BUFFERS) {
        if (auto* const indexBuffer = ms_IndexDataBuffers[ms_TidyUpIndexDataBufferIdx].PopLast()) {
            indexBuffer->Release();
            numEmptyInARow = 0;
            numToRelease--;
        } else {
            numEmptyInARow++;
        }
    }
}

// 0x730510
HRESULT D3DResourceSystem::CreateTexture(int32 width, int32 height, int32 levels, D3DFORMAT format, IDirect3DTexture9** outTexture) {
    const auto numLevels = levels > 1 ? 0 : levels; // Only single level textures and full chains are buffered
    *outTexture          = [&]() -> IDirect3DTexture9* {
        if (width == height) {
            for (auto& buffer : GetTextureBuffers() | rngv::drop(1)) {
                if (buffer.Matches(format, width, numLevels)) {
                    return buffer.PopLast();
                }
            }
        }
        return ms_TextureBuffers[0].Pop(format, width, height, numLevels);
    }();
    if (*outTexture) {
        return D3D_OK;
    }
    return _RwD3DDevice->CreateTexture(width, height, numLevels, 0, format, D3DPOOL_MANAGED, outTexture, nullptr);
}

// 0x730B70
void D3DResourceSystem::DestroyTexture(IDirect3DTexture9* texture) {
    if (!ms_UseD3DResourceBuffering) {
        texture->Release();
        return;
    }
    const int32     levels = texture->GetLevelCount() <= 1 ? 1 : 0;
    D3DSURFACE_DESC desc;
    texture->GetLevelDesc(0, &desc);
    const auto isSquare = desc.Width == desc.Height;
    if (isSquare) {
        for (auto& buffer : GetTextureBuffers() | rngv::drop(1)) {
            if (buffer.Matches(desc.Format, desc.Width, levels)) {
                buffer.PushOrGrow(texture);
                return;
            }
        }
    }
    if (ms_NumTextureBuffers < MAX_NUM_TEXTURE_BUFFERS && isSquare && desc.Width >= MIN_BUFFERED_TEXTURE_SIZE) { // 0x730C07
        if (desc.Width <= MAX_BUFFERED_TEXTURE_SIZE) {                                                           // Start a new buffer for this kind
            auto& buffer = ms_TextureBuffers[ms_NumTextureBuffers++];
            buffer.Setup(desc.Format, desc.Width, levels, INITIAL_BUFFER_CAPACITY);
            buffer.Push(texture);
            return;
        }
    } else if (desc.Width <= MAX_BUFFERED_TEXTURE_SIZE && desc.Height <= MAX_BUFFERED_TEXTURE_SIZE) { // 0x730CAB
        ms_TextureBuffers[0].PushOrGrow(texture);
        return;
    }
    texture->Release();
}

// 0x7306A0
HRESULT D3DResourceSystem::CreateIndexBuffer(uint32 numIndices, D3DFORMAT format, IDirect3DIndexBuffer9** outIndexBuffer) {
    const auto bufferIdx = ((int32)numIndices - 1) / (int32)NUM_INDICES_PER_SIZE_CLASS;
    if (notsa::bugfixes::GenericOOB && bufferIdx < 0) { // More than `INT32_MAX` indices
        *outIndexBuffer = nullptr;
        return D3DERR_INVALIDCALL;
    }
    const auto numIndicesInBuffer = (bufferIdx + 1) * NUM_INDICES_PER_SIZE_CLASS;
    // NOTE: The original also calls `GetDesc` of a reused buffer and throws the result away
    *outIndexBuffer = bufferIdx < (int32)NUM_SIZED_INDEX_DATA_BUFFERS
        ? ms_IndexDataBuffers[bufferIdx].PopLast()
        : ms_IndexDataBuffers.back().Pop(numIndicesInBuffer);
    if (*outIndexBuffer) {
        return D3D_OK;
    }
    // NOTE: `format` is ignored, it's always 16 bit indices
    return _RwD3DDevice->CreateIndexBuffer(numIndicesInBuffer * sizeof(uint16), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, outIndexBuffer, nullptr);
}

// 0x730D30
void D3DResourceSystem::DestroyIndexBuffer(IDirect3DIndexBuffer9* indexBuffer) {
    if (!ms_UseD3DResourceBuffering) {
        indexBuffer->Release();
        return;
    }
    D3DINDEXBUFFER_DESC desc;
    indexBuffer->GetDesc(&desc);
    const auto bufferIdx = (desc.Size / sizeof(uint16) - 1) / NUM_INDICES_PER_SIZE_CLASS;
    (bufferIdx < NUM_SIZED_INDEX_DATA_BUFFERS ? ms_IndexDataBuffers[bufferIdx] : ms_IndexDataBuffers.back()).PushOrGrow(indexBuffer);
}

void D3DResourceSystem::InjectHooks() {
    RH_ScopedClass(D3DResourceSystem);
    RH_ScopedCategory("RenderWare");

    RH_ScopedInstall(CancelBuffering, 0x730900);
    RH_ScopedInstall(GetTotalIndexDataSize, 0x7307F0);
    RH_ScopedInstall(GetTotalPixelsSize, 0x730660);
    RH_ScopedInstall(Init, 0x730830);
    RH_ScopedInstall(SetUseD3DResourceBuffering, 0x730AC0);
    RH_ScopedInstall(Shutdown, 0x730A00);
    RH_ScopedInstall(TidyUpD3DIndexBuffers, 0x730740);
    RH_ScopedInstall(TidyUpD3DTextures, 0x7305E0);
    RH_ScopedInstall(CreateIndexBuffer, 0x7306A0);
    RH_ScopedInstall(CreateTexture, 0x730510);
    RH_ScopedInstall(DestroyIndexBuffer, 0x730D30);
    RH_ScopedInstall(DestroyTexture, 0x730B70);

    RH_ScopedGlobalInstall(D3DTextureBuffer::Setup, 0x72FE80);
    RH_ScopedGlobalInstall(D3DTextureBuffer::Clear, 0x72FEC0);
    RH_ScopedGlobalInstall(D3DTextureBuffer::Destroy, 0x72FEF0);
    RH_ScopedGlobalInstall(D3DTextureBuffer::Pop, 0x72FF60);
    RH_ScopedGlobalInstall(D3DTextureBuffer::Push, 0x72FFF0);
    RH_ScopedGlobalInstall(D3DTextureBuffer::Resize, 0x730020);
    RH_ScopedGlobalInstall(D3DTextureBuffer::GetTotalDataSize, 0x7300A0);
    RH_ScopedGlobalInstall(D3DTextureBuffer::PushWithoutIncreasingCounter, 0x730AD0);

    RH_ScopedGlobalInstall(D3DIndexDataBuffer::Clear, 0x7301D0);
    RH_ScopedGlobalInstall(D3DIndexDataBuffer::Destroy, 0x730200);
    RH_ScopedGlobalInstall(D3DIndexDataBuffer::Pop, 0x730270);
    RH_ScopedGlobalInstall(D3DIndexDataBuffer::Resize, 0x730330);
    RH_ScopedGlobalInstall(D3DIndexDataBuffer::GetTotalDataSize, 0x7303B0);
}
