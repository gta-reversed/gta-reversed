#pragma once

#include <array>
#include <span>

#include <Base.h>

#include "Vector.h"
#include "Entity.h"
#include "Rect.h"

struct RpAtomic;

struct tTagDesc {
    CEntity* Entity; //!< The physical tag entity (Pretty much just a model with a single atomic and material)
    uint8    Alpha; //!< The alpha value of the tag (0-255)
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
    static void           Init();
    static void           ShutdownForRestart();
    static const CVector& GetTagPos(int32 idx);
    static void           AddTag(CEntity& entity);
    static bool           IsTag(const CEntity& entity);
    static int32          GetPercentageTaggedInArea(const CRect& area);
    static int32          GetPercentageTagged();
    static void           UpdateNumTagged();
    static uint8          GetAlpha(const CEntity& entity);
    static void           SetAlpha(CEntity& entity, uint8 alphaToSet);
    static void           SetAlphaInArea(const CRect& area, uint8 alphaToSet);
    static void           ResetAlpha(const CEntity& entity);
    static CEntity*       GetNearestTag(const CVector& nearestToPoint);
    static void           SetupAtomic(RpAtomic& atomic);
    static void           RenderTagForPC(RpAtomic& atomic);
    static void           Save();
    static void           Load();

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
        return GetTags() | std::views::filter([area] (const tTagDesc& tag) {
            return area.IsPointInside(tag.Entity->GetPosition2D());
        });
    }

private:
    static void      SetAlpha(RpAtomic& atomic, uint8 alphaToSet);
    static uint8     GetAlpha(const RpAtomic& atomic);
    static tTagDesc* FindTagDesc(const CEntity& entity);
};
