/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Pool.h"

typedef CPool<class COctTree> COctTreePool;

class NOTSA_EXPORT_VTABLE COctTree {
public:
    uint32 m_nLevel;
    bool   m_bLastStep;       // no children
    int16  m_aChildrens[8];   // pool slot IDs,  -1 - empty
    uint32 m_nRedComponent;
    uint32 m_nGreenComponent;
    uint32 m_nBlueComponent;

public:
    static bool&            ms_bFailed;
    static uint32&          ms_level;
    static COctTreePool&    ms_octTreePool;

public:
    COctTree();
    ~COctTree();

    static void* operator new(size_t size);
    static void  operator delete(void* data);

    virtual bool InsertTree(uint8 red, uint8 green, uint8 blue);
    virtual void FillPalette(uint8* colors);

    static void InitPool(void* data, int32 dataSize);
    static void ShutdownPool();
    uint32      FindNearestColour(uint8 red, uint8 green, uint8 blue);
    uint32      NoOfChildren();
    void        ReduceTree();
    void        RemoveChildren();
    void        empty();
};
VALIDATE_SIZE(COctTree, 0x28);

extern COctTree*& gpTmpOctTree;
