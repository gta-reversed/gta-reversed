/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

//! A free list of released D3D textures of one format and size (or of any, see `D3DResourceSystem`)
class D3DTextureBuffer {
public:
    void   Setup(D3DFORMAT format, uint32 width, int32 levels, uint32 capacity);
    void Clear();
    void Destroy();
    void Resize(uint32 newCapacity);
    uint32 GetTotalDataSize() const;

    IDirect3DTexture9* Pop(D3DFORMAT format, uint32 width, uint32 height, int32 levels);
    bool Push(IDirect3DTexture9* texture);
    bool PushWithoutIncreasingCounter(IDirect3DTexture9* texture);

    IDirect3DTexture9* PopLast();
    void               PushOrGrow(IDirect3DTexture9* texture);
    bool               Matches(D3DFORMAT format, uint32 width, int32 levels) const;

private:
    D3DFORMAT           m_Format{};
    uint32              m_Width{};
    int32               m_Levels{}; //!< 0 - full chain, 1 - one level, -1 - any (the mixed buffer)
    uint32              m_Capacity{};
    uint32              m_NumTexturesCounter{}; //!< Increased by every push, decreased only by `PopLast` - not the real count
    uint32              m_NumTextures{};
    IDirect3DTexture9** m_Textures{};
};
VALIDATE_SIZE(D3DTextureBuffer, 0x1C);
