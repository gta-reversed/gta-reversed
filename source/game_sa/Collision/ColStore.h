#pragma once

#include "Rect.h"
#include "Vector.h"
#include "Pool.h"

// thanks to jte for reversing this
struct ColDef {
    CRect  m_Area;
    char   name[18]{};
    int16  m_nModelIdStart;
    int16  m_nModelIdEnd;
    uint16 m_nRefCount;
    bool   m_bActive;
    bool   m_bCollisionIsRequired;
    bool   m_bProcedural;
    bool   m_bInterior;

    static void* operator new(unsigned size);
    static void  operator delete(void* data);
};
VALIDATE_SIZE(ColDef, 0x2C);

using CColPool = CPool<ColDef>;
class CColStore {
public:
    static CVector& ms_vecCollisionNeeded;
    static bool&    ms_bCollisionNeeded;
    static int32    ms_nRequiredCollisionArea;

public:
    static void InjectHooks();

    static void Initialise();
    static void Shutdown();
    static int32 AddColSlot(const char* name);
    static void AddCollisionNeededAtPosn(const CVector& pos);
    static void AddRef(int32 colNum);
    static int32 FindColSlot() { return -1; }
    static int32 FindColSlot(const char*);
    static void BoundingBoxesPostProcess();
    static void EnsureCollisionIsInMemory(const CVector& pos);
    static CRect* GetBoundingBox(int32 colSlot);
    static void IncludeModelIndex(int32 colSlot, int32 modelId);
    static bool HasCollisionLoaded(const CVector& pos, int32 areaCode);
    static void LoadAllBoundingBoxes();
    static void LoadAllCollision();
    static void LoadCol(int32 colSlot, const char* filename);
    static bool LoadCol(int32 colSlot, uint8* data, int32 dataSize);
    static void LoadCollision(CVector pos, bool bIgnorePlayerVeh);
    static void RemoveAllCollision();
    static void RemoveCol(int32 colSlot);
    static void RemoveColSlot(int32 colSlot);
    static void RemoveRef(int32 colNum);
    static void RequestCollision(const CVector& pos, int32 areaCode);
    static void SetCollisionRequired(const CVector& pos, int32 areaCode);

    static ColDef* GetInSlot(int32 slot);
    static CColPool* GetPool();
};
