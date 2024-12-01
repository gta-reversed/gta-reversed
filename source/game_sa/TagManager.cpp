#include "StdInc.h"

#include "TagManager.h"
#include "Garages.h"

tTagDesc (&CTagManager::ms_tagDesc)[MAX_TAGS] = *(tTagDesc(*)[MAX_TAGS])0xA9A8C0;
int32& CTagManager::ms_numTags = *(int32*)0xA9AD70;
int32& CTagManager::ms_numTagged = *(int32*)0xA9AD74;
RxPipeline* &CTagManager::ms_pPipeline = *(RxPipeline**)0xA9AD78;

void CTagManager::InjectHooks()
{
    RH_ScopedClass(CTagManager);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x49CC50);
    RH_ScopedInstall(ShutdownForRestart, 0x49CC60);
    RH_ScopedInstall(GetTagPos, 0x49CEA0);
    RH_ScopedInstall(AddTag, 0x49CC90);
    RH_ScopedInstall(FindTagDesc, 0x49CCB0);
    RH_ScopedInstall(IsTag, 0x49CCE0);
    RH_ScopedOverloadedInstall(SetAlpha, "RpAtomic", 0x49CD30, void(*)(RpAtomic*, uint8));
    RH_ScopedOverloadedInstall(SetAlpha, "Entity", 0x49CEC0, void (*)(CEntity*, uint8));
    RH_ScopedOverloadedInstall(GetAlpha, "RpAtomic", 0x49CD40, uint8(*)(RpAtomic*));
    RH_ScopedOverloadedInstall(GetAlpha, "Entity", 0x49CF90, uint8(*)(CEntity*));
    RH_ScopedInstall(GetPercentageTagged, 0x49CDA0);
    RH_ScopedInstall(GetPercentageTaggedInArea, 0x49D0B0);
    RH_ScopedInstall(UpdateNumTagged, 0x49CDE0);
    RH_ScopedInstall(SetAlphaInArea, 0x49CFE0);
    RH_ScopedInstall(GetNearestTag, 0x49D160);
    RH_ScopedInstall(SetupAtomic, 0x49CE10);
    RH_ScopedInstall(RenderTagForPC, 0x49CE40);
    RH_ScopedInstall(Save, 0x5D3D60, {.enabled = SAVE_HOOKS_ENABLED });
    RH_ScopedInstall(Load, 0x5D3DA0, {.enabled = LOAD_HOOKS_ENABLED });
}

// 0x49CC50
void CTagManager::Init() {
    ZoneScoped;

    ms_numTags = 0;
    ms_numTagged = 0;
}

// 0x49CC60
void CTagManager::ShutdownForRestart()
{
    for (int32 i = ms_numTags; i > 0; --i)
        ms_tagDesc[i].m_nAlpha = 0;

    ms_numTagged = 0;
}

// 0x49CEA0
CVector& CTagManager::GetTagPos(int32 iTag)
{
    return ms_tagDesc[iTag].m_pEntity->GetPosition();
}

// 0x49CC90
void CTagManager::AddTag(CEntity* entity)
{
    auto& tag = ms_tagDesc[ms_numTags];
    tag.m_pEntity = entity;
    tag.m_nAlpha = 0;

    ms_numTags++;
}

// 0x49CCB0
tTagDesc* CTagManager::FindTagDesc(CEntity* entity)
{
    if (!ms_numTags)
        return nullptr;

    for (int32 i = ms_numTags - 1; i >= 0; --i) {
        auto tagDesc = &ms_tagDesc[i];
        if (tagDesc->m_pEntity == entity)
            return tagDesc;
    }

    return nullptr;
}

// 0x49CCE0
bool CTagManager::IsTag(const CEntity* entity)
{
    auto mi = CModelInfo::GetModelInfo(entity->m_nModelIndex);
    if (mi->GetRwModelType() != rpATOMIC)
        return false;

    return mi->IsTagModel() && !mi->AsAtomicModelInfoPtr()->bTagDisabled;
}

// 0x49CDA0
int32 CTagManager::GetPercentageTagged()
{
    return static_cast<int32>(static_cast<float>(ms_numTagged) / static_cast<float>(ms_numTags) * 100.0F);
}

// 0x49D0B0
int32 CTagManager::GetPercentageTaggedInArea(CRect* area)
{
    int32 iTotalTags = 0;
    int32 iTagged = 0;
    for (int32 i = ms_numTags - 1; i >= 0; --i) {
        auto& tagDesc = ms_tagDesc[i];
        auto vecPos = CVector2D(tagDesc.m_pEntity->GetPosition());
        if (area->IsPointInside(vecPos)) {
            ++iTotalTags;
            if (tagDesc.m_nAlpha > ALPHA_TAGGED)
                ++iTagged;
        }
    }

    return static_cast<int32>(static_cast<float>(iTagged) / static_cast<float>(iTotalTags) * 100.0F);
}

// 0x49CDE0
void CTagManager::UpdateNumTagged()
{
    ms_numTagged = 0;
    if (!ms_numTags)
        return;

    for (int32 i = ms_numTags - 1; i >= 0; --i) {
        auto& tagDesc = ms_tagDesc[i];
        if (tagDesc.m_nAlpha > ALPHA_TAGGED)
            ms_numTagged++;
    }
}

uint8 CTagManager::GetAlpha(RpAtomic* atomic)
{
    return static_cast<uint8>(CVisibilityPlugins::GetUserValue(atomic));
}

uint8 CTagManager::GetAlpha(CEntity* entity)
{
    if (entity->m_pRwAtomic)
        return static_cast<uint8>(CVisibilityPlugins::GetUserValue(entity->m_pRwAtomic));

    auto tag = FindTagDesc(entity);
    assert(tag); // Originally the function would access uninitialized memory, by clearing EAX and dereferencing pointer to [EAX + 0x4] right after that

    return tag->m_nAlpha;
}

// 0x49CD30
void CTagManager::SetAlpha(RpAtomic* atomic, uint8 ucAlpha)
{
    CVisibilityPlugins::SetUserValue(atomic, ucAlpha);
}

// 0x49CEC0
void CTagManager::SetAlpha(CEntity* entity, uint8 ucAlpha)
{
    if (entity->m_pRwAtomic)
        SetAlpha(entity->m_pRwAtomic, ucAlpha);

    auto tagDesc = FindTagDesc(entity);
    auto bChangedState = false;
    if (ucAlpha > ALPHA_TAGGED && tagDesc->m_nAlpha <= ALPHA_TAGGED)
        bChangedState = true;

    tagDesc->m_nAlpha = ucAlpha;
    UpdateNumTagged();

    if (bChangedState && !TheCamera.m_bWideScreenOn) {
        if (ms_numTagged == ms_numTags)
            CGarages::TriggerMessage("TAG_ALL", -1, 5000u, -1);
        else
            CGarages::TriggerMessage("TAG_ONE", ms_numTagged, 5000u, ms_numTags);
    }
}

void CTagManager::ResetAlpha(CEntity* entity)
{
    if (!entity->m_pRwAtomic)
        return;

    auto tagDesc = FindTagDesc(entity);
    SetAlpha(entity->m_pRwAtomic, tagDesc->m_nAlpha);
}

// 0x49CFE0
void CTagManager::SetAlphaInArea(CRect* area, uint8 ucAlpha)
{
    for (int32 i = ms_numTags - 1; i >= 0; --i) {
        auto& tagDesc = ms_tagDesc[i];
        auto vecPos = CVector2D(tagDesc.m_pEntity->GetPosition());
        if (area->IsPointInside(vecPos) && tagDesc.m_pEntity->m_pRwAtomic) {
            SetAlpha(tagDesc.m_pEntity->m_pRwAtomic, ucAlpha);
            tagDesc.m_nAlpha = ucAlpha;
        }
    }

    UpdateNumTagged();
}

// 0x49D160
CEntity* CTagManager::GetNearestTag(const CVector& vecPos)
{
    auto vecPosUsed = CVector2D(vecPos.x, vecPos.y);
    int32 iClosestInd = -1;
    float fMinDist = RwRealMAXVAL;
    for (int32 i = ms_numTags - 1; i >= 0; --i) {
        auto& tagDesc = ms_tagDesc[i];
        auto vecTagPos = CVector2D(tagDesc.m_pEntity->GetPosition());
        auto fDist = (vecPosUsed - vecTagPos).SquaredMagnitude();
        if (fDist < fMinDist) {
            fMinDist = fDist;
            iClosestInd = i;
        }
    }

    assert(iClosestInd != -1);
    return ms_tagDesc[iClosestInd].m_pEntity;
}

// 0x49CE10
void CTagManager::SetupAtomic(RpAtomic* atomic)
{
    auto geometry = RpAtomicGetGeometry(atomic);
    auto material = RpGeometryGetMaterial(geometry, 1);
    material->pipeline = ms_pPipeline;
    RpGeometrySetFlags(geometry, RpGeometryGetFlags(geometry) | rpGEOMETRYMODULATEMATERIALCOLOR);
    SetAlpha(atomic, 0);
}

// 0x49CE40
void CTagManager::RenderTagForPC(RpAtomic* atomic)
{
    auto geometry = RpAtomicGetGeometry(atomic);
    auto material = RpGeometryGetMaterial(geometry, 1);
    material->color.alpha = GetAlpha(atomic);
    RpAtomicRender(atomic);
}

// 0x5D3D60
void CTagManager::Save()
{
    CGenericGameStorage::SaveDataToWorkBuffer(ms_numTags);

    for (auto i = 0; i < ms_numTags; ++i) {
        CGenericGameStorage::SaveDataToWorkBuffer(ms_tagDesc[i].m_nAlpha);
    }
}

// 0x5D3DA0
void CTagManager::Load()
{
#if FIX_BUGS
    // TODO: Verify if there are no side effects due to this fix - it's done the same way on android but I'm not sure why this worked even without reading the data to ms_numTags
    // The loop below always iterated the proper amount of times, so i assume the tags are loaded at some point before save is loaded, and we already have all the tags initialized
    CGenericGameStorage::LoadDataFromWorkBuffer(ms_numTags);
#else
    CGenericGameStorage::SaveDataToWorkBuffer(ms_numTags); // Yeah, original also saves into buffer instead of loading
#endif

    for (auto i = 0; i < ms_numTags; ++i) {
        CGenericGameStorage::LoadDataFromWorkBuffer(ms_tagDesc[i].m_nAlpha);
    }
}
