#include "StdInc.h"

#include <reversiblebugfixes/Bugs.hpp>

#include "TagManager.h"
#include "Garages.h"

void CTagManager::InjectHooks() {
    RH_ScopedClass(CTagManager);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x49CC50);
    RH_ScopedInstall(ShutdownForRestart, 0x49CC60);
    RH_ScopedInstall(GetTagPos, 0x49CEA0);
    RH_ScopedInstall(AddTag, 0x49CC90);
    RH_ScopedInstall(FindTagDesc, 0x49CCB0);
    RH_ScopedInstall(IsTag, 0x49CCE0);
    RH_ScopedOverloadedInstall(SetAlpha, "RpAtomic", 0x49CD30, void(*)(RpAtomic&, uint8));
    RH_ScopedOverloadedInstall(SetAlpha, "Entity", 0x49CEC0, void (*)(CEntity&, uint8));
    RH_ScopedOverloadedInstall(GetAlpha, "RpAtomic", 0x49CD40, uint8(*)(const RpAtomic&));
    RH_ScopedOverloadedInstall(GetAlpha, "Entity", 0x49CF90, uint8(*)(const CEntity&));
    RH_ScopedInstall(GetPercentageTagged, 0x49CDA0);
    RH_ScopedInstall(GetPercentageTaggedInArea, 0x49D0B0);
    RH_ScopedInstall(UpdateNumTagged, 0x49CDE0);
    RH_ScopedInstall(SetAlphaInArea, 0x49CFE0);
    RH_ScopedInstall(GetNearestTag, 0x49D160);
    RH_ScopedInstall(SetupAtomic, 0x49CE10);
    RH_ScopedInstall(RenderTagForPC, 0x49CE40);
    RH_ScopedInstall(Save, 0x5D3D60);
    RH_ScopedInstall(Load, 0x5D3DA0);
}

// 0x49CC50
void CTagManager::Init() {
    ZoneScoped;

    ms_numTags   = 0;
    ms_numTagged = 0;
}

// 0x49CC60
void CTagManager::ShutdownForRestart() {
#if 0
    // Doing this here breaks the game, so i'll just leave it as it was originally...
    rng::fill(ms_tagDesc, tTagDesc{});
    if (notsa::bugfixes::CTagManager_MissingTagCountResetOnShutdown) {
        ms_numTags = 0;
    }
#else
    for (auto& tag : ms_tagDesc) {
        tag.Alpha = 0;
    }
#endif
    ms_numTagged = 0;
}

// 0x49CEA0
const CVector& CTagManager::GetTagPos(int32 idx) {
    return GetTags()[idx].Entity->GetPosition();
}

// 0x49CC90
void CTagManager::AddTag(CEntity& entity) {
    assert(IsTag(entity) && "Must be a tag entity");

    ms_tagDesc[ms_numTags++] = tTagDesc{ &entity, 0 };
}

// 0x49CCB0
tTagDesc* CTagManager::FindTagDesc(const CEntity& entity) {
    for (auto& tag : GetTags()) {
        if (tag.Entity == &entity) {
            return &tag;
        }
    }
    NOTSA_LOG_WARN("CTagManager::FindTagDesc: Couldn't find tag desc for entity %p - This shouldn't happen! Perhaps corrupted savefile?", LOG_PTR(&entity));
    return nullptr;
}

// 0x49CCE0
bool CTagManager::IsTag(const CEntity& entity) {
    auto mi = entity.GetModelInfo();
    if (mi->GetRwModelType() != rpATOMIC) {
        return false;
    }

    return mi->IsTagModel() && !mi->AsAtomicModelInfoPtr()->bTagDisabled;
}

// 0x49CDA0
int32 CTagManager::GetPercentageTagged() {
    return static_cast<int32>(static_cast<float>(ms_numTagged) / static_cast<float>(ms_numTags) * 100.0F);
}

// 0x49D0B0
int32 CTagManager::GetPercentageTaggedInArea(const CRect& area) {
    int32 numTotalTaggable = 0, numTagged = 0;
    for (auto& tag : GetTagsInArea(area)) {
        ++numTotalTaggable;
        if (tag.Alpha > ALPHA_TAGGED) {
            ++numTagged;
        }
    }
    return numTotalTaggable != 0
        ? (numTagged * 100) / numTotalTaggable /* Do division without converting to float at all, this isn't how it was done, but it's the same logic... */
        : 0;
}

// 0x49CDE0
void CTagManager::UpdateNumTagged() {
    ms_numTagged = rng::count_if(GetTags(), [](const tTagDesc& tag) {
        return tag.Alpha > ALPHA_TAGGED;
    });
}

uint8 CTagManager::GetAlpha(const RpAtomic& atomic) {
    return static_cast<uint8>(CVisibilityPlugins::GetUserValue(&atomic));
}

uint8 CTagManager::GetAlpha(const CEntity& entity) {
    assert(IsTag(entity));

    if (entity.GetRpAtomic()) {
        return static_cast<uint8>(CVisibilityPlugins::GetUserValue(entity.GetRpAtomic()));
    }

    auto* const desc = FindTagDesc(entity);
    if (notsa::bugfixes::GenericCrashing) {
        if (!desc) {
            return 0;
        }
    }
    return desc->Alpha;
}

// 0x49CD30
void CTagManager::SetAlpha(RpAtomic& atomic, uint8 alphaToSet) {
    CVisibilityPlugins::SetUserValue(&atomic, alphaToSet);
}

// 0x49CEC0
void CTagManager::SetAlpha(CEntity& entity, uint8 alphaToSet) {
    assert(IsTag(entity));

    if (auto* const atomicOfEntity = entity.GetRpAtomic()) {
        SetAlpha(*atomicOfEntity, alphaToSet);
    }

    auto* const tag = FindTagDesc(entity);
    if (notsa::bugfixes::GenericCrashing) {
        if (!tag) {
            return;
        }
    }

    const auto justGotTagged = alphaToSet > ALPHA_TAGGED && tag->Alpha <= ALPHA_TAGGED;
    tag->Alpha               = alphaToSet;
    UpdateNumTagged();

    if (justGotTagged && !TheCamera.m_bWideScreenOn) {
        if (ms_numTagged == ms_numTags) {
            CGarages::TriggerMessage("TAG_ALL", -1, 5'000u, -1);
        } else {
            CGarages::TriggerMessage("TAG_ONE", ms_numTagged, 5'000u, ms_numTags);
        }
    }
}

void CTagManager::ResetAlpha(const CEntity& entity) {
    assert(IsTag(entity));

    auto* const atomicOfEntity = entity.GetRpAtomic();
    if (!atomicOfEntity) {
        return;
    }
    auto* const tagOfEntity = FindTagDesc(entity);
    if (notsa::bugfixes::GenericCrashing) {
        if (!tagOfEntity) {
            return;
        }
    }
    SetAlpha(*atomicOfEntity, tagOfEntity->Alpha);
}

// 0x49CFE0
void CTagManager::SetAlphaInArea(const CRect& area, uint8 alphaToSet) {
    for (auto& tag : GetTagsInArea(area)) {
        auto* const atomicOfTag = tag.Entity->GetRpAtomic();
        if (!atomicOfTag) {
            continue;
        }
        SetAlpha(*atomicOfTag, alphaToSet);
        tag.Alpha = alphaToSet;
    }           
    UpdateNumTagged();
}

// 0x49D160
CEntity* CTagManager::GetNearestTag(const CVector& nearestToPoint) {
    tTagDesc* closest         = nullptr;
    float     closestDist2DSq = RwRealMAXVAL;
    for (auto& tag : GetTags()) {
        if (const auto dist2DSq = CVector2D::DistSqr(nearestToPoint, tag.Entity->GetPosition2D()); dist2DSq < closestDist2DSq) {
            closestDist2DSq = dist2DSq;
            closest         = &tag;
        }
    }
    return closest
        ? closest->Entity
        : nullptr; // BUGFIX(Pirulax): Original caller site does handle nullptr just fine, they just kinda forgot to return null if there are no tags at all xD
}

// 0x49CE10
void CTagManager::SetupAtomic(RpAtomic& atomic) {
    auto geometry      = RpAtomicGetGeometry(&atomic);
    auto material      = RpGeometryGetMaterial(geometry, 1);
    material->pipeline = ms_pPipeline;
    RpGeometrySetFlags(geometry, RpGeometryGetFlags(geometry) | rpGEOMETRYMODULATEMATERIALCOLOR);
    SetAlpha(atomic, 0);
}

// 0x49CE40
void CTagManager::RenderTagForPC(RpAtomic& atomic) {
    auto geometry                       = RpAtomicGetGeometry(&atomic);
    auto material                       = RpGeometryGetMaterial(geometry, 1);
    RpMaterialGetColor(material)->alpha = GetAlpha(atomic);
    RpAtomicRender(&atomic);
}

// 0x5D3D60
void CTagManager::Save() {
    CGenericGameStorage::SaveDataToWorkBuffer(ms_numTags);

    for (const auto& tag : GetTags()) {
        CGenericGameStorage::SaveDataToWorkBuffer(tag.Alpha);
    }
}

// 0x5D3DA0
void CTagManager::Load() {
#if 0 // FIX_BUGS - NOTE(Pirulax): I've tested, and it breaks the tags (Just try the "tagging the turf" mission), so I guess just leave it as-is
    // TODO: Verify if there are no side effects due to this fix - it's done the same way on android but I'm not sure why this worked even without reading the data to ms_numTags
    // The loop below always iterated the proper amount of times, so i assume the tags are loaded at some point before save is loaded, and we already have all the tags initialized
    CGenericGameStorage::LoadDataFromWorkBuffer(ms_numTags);
#else
    CGenericGameStorage::SaveDataToWorkBuffer(ms_numTags); // Yeah, original also saves into buffer instead of loading
#endif

    for (auto& tag : GetTags()) {
        CGenericGameStorage::LoadDataFromWorkBuffer(tag.Alpha);
    }

    NOTSA_LOG_DEBUG("Loaded {} tags", ms_numTags);
}
