/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

//! A free list of released D3D index buffers of one size class (see `D3DResourceSystem`)
class D3DIndexDataBuffer {
public:
    void   Setup(D3DFORMAT format, uint32 maxNumIndices, uint32 capacity);
    void Clear();
    void Destroy();
    void Resize(uint32 newCapacity);
    uint32 GetTotalDataSize() const;

    IDirect3DIndexBuffer9* Pop(uint32 numIndices);

    IDirect3DIndexBuffer9* PopLast();
    void                   PushOrGrow(IDirect3DIndexBuffer9* indexBuffer);

private:
    D3DFORMAT               m_Format{};
    uint32                  m_MaxNumIndices{}; //!< Only set, never read
    uint32                  m_Capacity{};
    uint32                  m_NumIndexBuffersCounter{}; //!< Increased by every push, decreased only by `PopLast` - not the real count
    uint32                  m_NumIndexBuffers{};
    IDirect3DIndexBuffer9** m_IndexBuffers{};
};
VALIDATE_SIZE(D3DIndexDataBuffer, 0x18);
