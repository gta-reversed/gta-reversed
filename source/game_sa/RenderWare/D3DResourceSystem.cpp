/*
    Plugin-SDK (Grand Theft Auto San Andreas) source file
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
unsigned int D3DResourceSystem::GetTotalIndexDataSize() {
    return plugin::CallAndReturn<unsigned int, 0x7307F0>();
}

// 0x730660
unsigned int D3DResourceSystem::GetTotalPixelsSize() {
    return plugin::CallAndReturn<unsigned int, 0x730660>();
}

// 0x730830
void D3DResourceSystem::Init() {
    plugin::Call<0x730830>();
}

// 0x730AC0
void D3DResourceSystem::SetUseD3DResourceBuffering(bool bUse) {
    plugin::Call<0x730AC0, bool>(bUse);
}

// 0x730A00
void D3DResourceSystem::Shutdown() {
    plugin::Call<0x730A00>();
}

// 0x730740
void D3DResourceSystem::TidyUpD3DIndexBuffers(unsigned int count) {
    plugin::Call<0x730740, unsigned int>(count);
}

// 0x7305E0
void D3DResourceSystem::TidyUpD3DTextures(unsigned int count) {
    plugin::Call<0x7305E0, unsigned int>(count);
}

// 0x7306A0
int D3DResourceSystem::CreateIndexBuffer(unsigned int numIndices, unsigned int format, void** ppIndexBuffer) {
    return plugin::CallAndReturn<int, 0x7306A0, unsigned int, unsigned int, void**>(numIndices, format, ppIndexBuffer);
}

// 0x730510
int D3DResourceSystem::CreateTexture(int width, int height, unsigned int format, void** ppTexture) {
    return plugin::CallAndReturn<int, 0x730510, int, int, unsigned int, void**>(width, height, format, ppTexture);
}

// 0x730D30
void D3DResourceSystem::DestroyIndexBuffer(void* pIndexBuffer) {
    plugin::Call<0x730D30, void*>(pIndexBuffer);
}

// 0x730B70
void D3DResourceSystem::DestroyTexture(void* pTexture) {
    plugin::Call<0x730B70, void*>(pTexture);
}
