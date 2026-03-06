#pragma once

#include "RenderWare.h"

class CEntity;
class CRect;

struct tTagDesc {
    CEntity* m_pEntity;
    uint8    m_nAlpha;
};

class CTagManager {
public:
    static constexpr uint8  ALPHA_TAGGED = 228;
    static constexpr uint32 MAX_TAGS = 150;

    static inline auto& ms_tagDesc = StaticRef<tTagDesc[MAX_TAGS]>(0xA9A8C0);
    static inline auto& ms_numTags = StaticRef<int32>(0xA9AD70);
    static inline auto& ms_numTagged = StaticRef<int32>(0xA9AD74);
    static inline auto& ms_pPipeline = StaticRef<RxPipeline*>(0xA9AD78);

public:
    static void InjectHooks();

public:
    static void     Init();
    static void     ShutdownForRestart();
    static CVector& GetTagPos(int32 iTag);
    static void     AddTag(CEntity* entity);
    static bool     IsTag(const CEntity* entity);
    static int32    GetPercentageTaggedInArea(CRect* area);
    static int32    GetPercentageTagged();
    static void     UpdateNumTagged();
    static uint8    GetAlpha(CEntity* entity);
    static void     SetAlpha(CEntity* entity, uint8 ucAlpha);
    static void     SetAlphaInArea(CRect* area, uint8 ucAlpha);
    static void     ResetAlpha(CEntity* entity);
    static CEntity* GetNearestTag(const CVector& vecPos);
    static void     SetupAtomic(RpAtomic* atomic);
    static void     RenderTagForPC(RpAtomic* atomic);
    static void     Save();
    static void     Load();

private:
    static void      SetAlpha(RpAtomic* atomic, uint8 ucAlpha);
    static uint8     GetAlpha(RpAtomic* atomic);
    static tTagDesc* FindTagDesc(CEntity* entity);
};
