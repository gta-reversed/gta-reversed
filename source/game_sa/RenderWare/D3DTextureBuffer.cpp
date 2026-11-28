#include "StdInc.h"

#include "D3DTextureBuffer.h"

// 0x72FE80
void D3DTextureBuffer::Setup(D3DFORMAT format, uint32 width, int32 levels, uint32 capacity) {
    m_Format             = format;
    m_Width              = width;
    m_Levels             = levels;
    m_Textures           = new IDirect3DTexture9*[capacity];
    m_Capacity           = capacity;
    m_NumTextures        = 0;
    m_NumTexturesCounter = 0;
}

// 0x72FEC0
void D3DTextureBuffer::Clear() {
    for (auto* const texture : std::span{ m_Textures, m_NumTextures }) {
        texture->Release();
    }
    m_NumTextures        = 0;
    m_NumTexturesCounter = 0;
}

// 0x72FEF0
void D3DTextureBuffer::Destroy() {
    Clear();
    delete[] std::exchange(m_Textures, nullptr);
}

// 0x730020
void D3DTextureBuffer::Resize(uint32 newCapacity) {
    if (newCapacity == m_Capacity) {
        return;
    }
    auto* const textures = new IDirect3DTexture9*[newCapacity];
    for (auto i = 0u; i < m_NumTextures && i < newCapacity; i++) {
        textures[i] = m_Textures[i];
    }
    for (auto i = newCapacity; i < m_NumTextures; i++) {
        m_Textures[i]->Release();
    }
    // NOTE: The original keeps `m_NumTextures` when shrinking - it's only ever called to grow
    delete[] m_Textures;
    m_Textures = textures;
    m_Capacity = newCapacity;
}

// 0x7300A0
uint32 D3DTextureBuffer::GetTotalDataSize() const {
    const auto GetTopLevelDesc = [](IDirect3DTexture9* texture) {
        D3DSURFACE_DESC desc;
        texture->GetLevelDesc(0, &desc);
        return desc;
    };
    // NOTE: The original checks for 0 levels (never the case) to add the mip chain (4/3 of the top level)
    if (m_Format != D3DFMT_UNKNOWN) { // All textures are the same, use the first one's size
        if (!m_NumTextures) {
            return 0;
        }
        const auto   desc = GetTopLevelDesc(m_Textures[0]);
        const uint32 size = m_NumTextures * desc.Height * desc.Width;
        return m_Textures[0]->GetLevelCount() == 0
            ? (uint32)((float)size * (4.f / 3.f))
            : size;
    }
    uint32 total = 0;
    for (auto* const texture : std::span{ m_Textures, m_NumTextures }) {
        const auto   desc   = GetTopLevelDesc(texture);
        const uint32 pixels = desc.Height * desc.Width;
        total               = texture->GetLevelCount() == 0
            ? (uint32)((float)pixels * (4.f / 3.f) + (float)total)
            : total + pixels;
    }
    return total;
}

// 0x72FF60
IDirect3DTexture9* D3DTextureBuffer::Pop(D3DFORMAT format, uint32 width, uint32 height, int32 levels) {
    for (auto i = m_NumTextures; i != 0;) {
        auto* const texture = m_Textures[--i];
        if ((int32)(texture->GetLevelCount() == 1) != levels) {
            continue;
        }
        D3DSURFACE_DESC desc;
        texture->GetLevelDesc(0, &desc);
        if (desc.Format != format || desc.Width != width || desc.Height != height) {
            continue;
        }
        m_Textures[i] = m_Textures[--m_NumTextures];
        return texture;
    }
    return nullptr;
}

// 0x72FFF0
bool D3DTextureBuffer::Push(IDirect3DTexture9* texture) {
    m_NumTexturesCounter++;
    return PushWithoutIncreasingCounter(texture);
}

// 0x730AD0
bool D3DTextureBuffer::PushWithoutIncreasingCounter(IDirect3DTexture9* texture) {
    if (m_NumTextures >= m_Capacity) {
        return false;
    }
    m_Textures[m_NumTextures++] = texture;
    return true;
}

// NOTSA
IDirect3DTexture9* D3DTextureBuffer::PopLast() {
    if (!m_NumTextures) {
        return nullptr;
    }
    m_NumTexturesCounter--;
    return m_Textures[--m_NumTextures];
}

// NOTSA
void D3DTextureBuffer::PushOrGrow(IDirect3DTexture9* texture) {
    if (!Push(texture)) {
        Resize(m_Capacity * 2);
        PushWithoutIncreasingCounter(texture); // NOTE: Fails only with a capacity of 0 (never the case), the original drops (leaks) the texture then
    }
}

// NOTSA
bool D3DTextureBuffer::Matches(D3DFORMAT format, uint32 width, int32 levels) const {
    return m_Width == width && m_Format == format && m_Levels == levels;
}
