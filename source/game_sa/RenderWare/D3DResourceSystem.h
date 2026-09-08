/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "D3DTextureBuffer.h"
#include "D3DIndexDataBuffer.h"
#ifdef _DX9_SDK_INSTALLED
#include "d3d9.h"
#endif

class D3DResourceSystem {
public:
    static bool &UseD3DResourceBuffering;
    static uint32 &FreeTextureBufferIndex;
    static D3DTextureBuffer &TextureBuffer;
    static D3DIndexDataBuffer &IndexDataBuffer;

public:
    static void InjectHooks();

public:
    static void CancelBuffering();
    static uint32 GetTotalIndexDataSize();
    static uint32 GetTotalPixelsSize();
    static void Init();
    static void SetUseD3DResourceBuffering(bool bUse);
    static void Shutdown();
    static void TidyUpD3DIndexBuffers(uint32 count);
    static void TidyUpD3DTextures(uint32 count);
    static int32 CreateIndexBuffer(uint32 numIndices, uint32 format, void** ppIndexBuffer);
    static int32 CreateTexture(int32 width, int32 height, uint32 format, void** ppTexture);
    static void DestroyIndexBuffer(void* pIndexBuffer);
    static void DestroyTexture(void* texture);
};
