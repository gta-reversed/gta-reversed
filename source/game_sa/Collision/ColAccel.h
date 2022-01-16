#pragma once

#include "IplDef.h"

struct CColAccelColBound {
    CRect m_Area;
    int16 m_wModelStart;
    int16 m_wModelEnd;
    bool  m_bProcedural;
    bool  m_bInterior;
};
VALIDATE_SIZE(CColAccelColBound, 0x18);

struct CColAccelColEntry {
    CBoundingBox m_boundBox;
    CSphere      m_boundSphere;
    int16        m_wModelStart;
    int16        m_wModelEnd;
    uint8        m_nColSlot;
    bool         m_bUnkn;
};
VALIDATE_SIZE(CColAccelColEntry, 0x30);

struct CColAccelIPLEntry {
    int32 m_nLodIndex;
    int32 m_nEntityIndex;
    int32 m_nModelId;
    int32 m_nLodModelId;
    union {
        uint32 m_nFlags;
        struct {
            uint32 m_bIsUnderwater : 1;
            uint32 m_bHasColModel : 1;
            uint32 m_bIsFarDrawDist : 1;
            uint32 m_bOwnsColModel : 1;
        };
    };
};
VALIDATE_SIZE(CColAccelIPLEntry, 0x14);

enum eColAccelState : int32 {
    COLACCEL_ENDED = 0,
    COLACCEL_STARTED = 1,
    COLACCEL_LOADING = 2,
};

struct PackedModelStartEnd {
    int16 wModelStart;
    int16 wModelEnd;
};
VALIDATE_SIZE(PackedModelStartEnd, 0x4);

class CColAccel {
public:
    static CColAccelColBound*& m_colBounds;
    static IplDef*&            m_iplDefs;
    static int32*&             m_iSectionSize;
    static int32&              m_iCachingColSize;
    static eColAccelState&     m_iCacheState;
    static CColAccelColEntry*& mp_caccColItems;
    static int32&              m_iNumColItems;
    static CColAccelIPLEntry*& mp_caccIPLItems;
    static int32&              m_iNumIPLItems;
    static int32&              m_iNumSections;
    static int32&              m_iNumColBounds;
    static char const*         mp_cCacheName; // MODELS\CINFO.BIN

public:
    static void InjectHooks();

    static bool   isCacheLoading();
    static void   endCache();
    static void   addCacheCol(PackedModelStartEnd startEnd, CColModel const& colModel);
    static void   cacheLoadCol();
    static void   addColDef(ColDef colDef);
    static void   getColDef(ColDef& colDef);
    static void   setIplDef(int32 iplIndex, IplDef iplDef);
    static IplDef getIplDef(int32 iplIndex);
    static void   cacheIPLSection(CEntity** ppEntities, int32 entitiesCount);
    static void   addIPLEntity(CEntity** ppEntities, int32 entitiesCount, int32 entityIndex);
    static void   startCache();
};
