/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/

#include "StdInc.h"

// 0x730900
void D3DResourceSystem::CancelBuffering() {
    plugin::Call<0x730900>();
}

// 0x7307F0
uint32 D3DResourceSystem::GetTotalIndexDataSize() {
    return plugin::CallAndReturn<uint32, 0x7307F0>();
}

// 0x730660
uint32 D3DResourceSystem::GetTotalPixelsSize() {
    return plugin::CallAndReturn<uint32, 0x730660>();
}

// 0x730830
void D3DResourceSystem::Init() {
    plugin::Call<0x730830>();
}

// 0x730AC0
void D3DResourceSystem::SetUseD3DResourceBuffering(bool bUse) {
    ZoneScoped;

    plugin::Call<0x730AC0, bool>(bUse);
}

// 0x730A00
void D3DResourceSystem::Shutdown() {
    plugin::Call<0x730A00>();
}

// 0x730740
void D3DResourceSystem::TidyUpD3DIndexBuffers(uint32 count) {
    plugin::Call<0x730740, uint32>(count);
}

// 0x7305E0
void D3DResourceSystem::TidyUpD3DTextures(uint32 count) {
    plugin::Call<0x7305E0, uint32>(count);
}

// 0x7306A0
int32 D3DResourceSystem::CreateIndexBuffer(uint32 numIndices, uint32 format, void** ppIndexBuffer) {
    return plugin::CallAndReturn<int32, 0x7306A0, uint32, uint32, void**>(numIndices, format, ppIndexBuffer);
}

// 0x730510
int32 D3DResourceSystem::CreateTexture(int32 width, int32 height, uint32 format, void** ppTexture) {
    return plugin::CallAndReturn<int32, 0x730510, int32, int32, uint32, void**>(width, height, format, ppTexture);
}

// 0x730D30
void D3DResourceSystem::DestroyIndexBuffer(void* pIndexBuffer) {
    plugin::Call<0x730D30, void*>(pIndexBuffer);
}

// 0x730B70
void D3DResourceSystem::DestroyTexture(void* texture) {
    plugin::Call<0x730B70, void*>(texture);
}
