/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "RenderWare.h"
#include "Vector.h"

class FxMemoryPool_c {
public:
    uint8* m_pData;
    size_t m_nSize;
    size_t m_nPosition; // offset, current byte

public:
    static void InjectHooks();

    FxMemoryPool_c() = default; // 0x4A9C10
    ~FxMemoryPool_c() = default; // 0x4A9C20

    void  Init();
    void  Exit();
    void  Reset();
    void* GetMem(int32 size, int32 align = sizeof(void*));
    void  Optimise();
};

VALIDATE_SIZE(FxMemoryPool_c, 0xC);
