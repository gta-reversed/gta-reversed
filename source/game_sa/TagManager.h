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

    static inline auto& ms_tagDesc = StaticRef<std::array<tTagDesc, MAX_TAGS>>(0xA9A8C0);
    static inline auto& ms_numTags = StaticRef<int32>(0xA9AD70);
    static inline auto& ms_numTagged = StaticRef<int32>(0xA9AD74);
    static inline auto& ms_pPipeline = StaticRef<RxPipeline*>(0xA9AD78);

public:
    static void InjectHooks();

public:
    static const CVector& GetTagPos(int32 idx);
    static int32          GetPercentageTaggedInArea(const CRect& area);
    static void           SetAlphaInArea(const CRect& area, uint8 ucAlpha);

    /*!
     * @return A span of all initialized tags
     */
    static auto GetTags() {
        return std::span(std::begin(ms_tagDesc), ms_numTags);
    }

    /*!
     * @param area Area in which the tags should be in 
     * @return A view of all tags that are in the given area (With valid entity pointers)
     */
    static auto GetTagsInArea(const CRect& area) {
        return GetTags() | std::views::filter([&area] (const tTagDesc& tag) {
            return area.IsPointInside(tag.m_pEntity->GetPosition2D());
        });
    }

private:
    static void      SetAlpha(RpAtomic* atomic, uint8 ucAlpha);
    static uint8     GetAlpha(RpAtomic* atomic);
    static tTagDesc* FindTagDesc(CEntity* entity);
};
