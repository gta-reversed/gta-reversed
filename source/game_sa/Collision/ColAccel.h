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
    bool         m_bColModelNotEmpty;
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

// If you ever see a cast like: `(PackedModelStartEnd)modelId`
// That's (in theory) the same as `PackedModelStartEnd{.start = modelId, .end = 0}`, as
// no model has the MSB 16 bits set (as no model's ID is higher than 65535)
union PackedModelStartEnd {
    struct {
        int16 wModelStart;
        int16 wModelEnd;
    };
    int32 modelId;
};
VALIDATE_SIZE(PackedModelStartEnd, 0x4);

class CColAccel {
public:
    static inline auto& m_colBounds = StaticRef<CColAccelColBound*>(0xBC4090);
    static inline auto& m_iplDefs = StaticRef<IplDef*>(0xBC4094);
    static inline auto& m_iSectionSize = StaticRef<int32*>(0xBC4098);
    static inline auto& m_iCachingColSize = StaticRef<int32>(0xBC409C);
    static inline auto& m_iCacheState = StaticRef<eColAccelState>(0xBC40A0);
    static inline auto& mp_caccColItems = StaticRef<CColAccelColEntry*>(0xBC40A4);
    static inline auto& m_iNumColItems = StaticRef<int32>(0xBC40A8);
    static inline auto& mp_caccIPLItems = StaticRef<CColAccelIPLEntry*>(0xBC40AC);
    static inline auto& m_iNumIPLItems = StaticRef<int32>(0xBC40B0);
    static inline auto& m_iNumSections = StaticRef<int32>(0xBC40B4);
    static inline auto& m_iNumColBounds = StaticRef<int32>(0xBC40B8);
    static const char*         mp_cCacheName;

public:
    static void InjectHooks();

    static bool   isCacheLoading();
    static void   startCache();
    static void   endCache();
    static void   addCacheCol(PackedModelStartEnd startEnd, const CColModel& colModel);
    static void   cacheLoadCol();
    static void   addColDef(ColDef colDef);
    static void   getColDef(ColDef& colDef);
    static void   setIplDef(int32 iplIndex, IplDef iplDef);
    static IplDef getIplDef(int32 iplIndex);
    static void   cacheIPLSection(CEntity** ppEntities, int32 entitiesCount);
    static void   addIPLEntity(CEntity** ppEntities, int32 entitiesCount, int32 entityIndex);
};
