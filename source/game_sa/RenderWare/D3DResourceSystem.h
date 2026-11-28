/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "D3DTextureBuffer.h"
#include "D3DIndexDataBuffer.h"

//! Keeps released D3D textures and index buffers to hand them out again instead of creating new ones
class D3DResourceSystem {
public:
    static void InjectHooks();

    static void Init();
    static void Shutdown();
    static void CancelBuffering();
    static void SetUseD3DResourceBuffering(bool use);

    static uint32 GetTotalPixelsSize();
    static uint32 GetTotalIndexDataSize();
    static void TidyUpD3DTextures(uint32 count);
    static void   TidyUpD3DIndexBuffers(uint32 count);

    static HRESULT CreateTexture(int32 width, int32 height, int32 levels, D3DFORMAT format, IDirect3DTexture9** outTexture);
    static void    DestroyTexture(IDirect3DTexture9* texture);
    static HRESULT CreateIndexBuffer(uint32 numIndices, D3DFORMAT format, IDirect3DIndexBuffer9** outIndexBuffer);
    static void    DestroyIndexBuffer(IDirect3DIndexBuffer9* indexBuffer);

private:
    static auto GetTextureBuffers() { return std::span{ ms_TextureBuffers.data(), ms_NumTextureBuffers }; }

    static constexpr uint32 MAX_NUM_TEXTURE_BUFFERS      = 17; //!< The first one holds the textures of any format/size
    static constexpr uint32 NUM_SIZED_INDEX_DATA_BUFFERS = 16; //!< Buffer `i` holds the ones of up to `(i + 1) * NUM_INDICES_PER_SIZE_CLASS` indices, the one after them the bigger ones
    static constexpr uint32 NUM_INDICES_PER_SIZE_CLASS   = 100;
    static constexpr uint32 INITIAL_BUFFER_CAPACITY      = 16;
    static constexpr uint32 MIN_BUFFERED_TEXTURE_SIZE    = 32;  //!< Smaller square textures go to the mixed buffer
    static constexpr uint32 MAX_BUFFERED_TEXTURE_SIZE    = 256; //!< Bigger textures are released

    static inline auto& ms_UseD3DResourceBuffering       = StaticRef<bool>(0x8D6084);
    static inline auto& ms_NumTextureBuffers             = StaticRef<uint32>(0xC87C60);
    static inline auto& ms_TextureBuffers                = StaticRef<std::array<D3DTextureBuffer, MAX_NUM_TEXTURE_BUFFERS>>(0xC87C68);
    static inline auto& ms_IndexDataBuffers              = StaticRef<std::array<D3DIndexDataBuffer, NUM_SIZED_INDEX_DATA_BUFFERS + 1>>(0xC87E48);
    static inline auto& ms_TidyUpTextureBufferIdx        = StaticRef<uint32>(0xC87FE0);
    static inline auto& ms_TidyUpIndexDataBufferIdx      = StaticRef<uint32>(0xC87FE4);
};
