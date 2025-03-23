/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#include "StdInc.h"

#include "Renderer.h"

#include "Occlusion.h"
#include "PostEffects.h"
#include "Shadows.h"
#include "CarFXRenderer.h"

#include "toolsmenu/UIRenderer.h"

bool& CRenderer::ms_bRenderTunnels = *(bool*)0xB745C0;
bool& CRenderer::ms_bRenderOutsideTunnels = *(bool*)0xB745C1;
tRenderListEntry*& CRenderer::ms_pLodDontRenderList = *(tRenderListEntry**)0xB745CC;
tRenderListEntry*& CRenderer::ms_pLodRenderList = *(tRenderListEntry**)0xB745D0;
CVehicle*& CRenderer::m_pFirstPersonVehicle = *(CVehicle**)0xB745D4;
CEntity* (&CRenderer::ms_aInVisibleEntityPtrs)[MAX_INVISIBLE_ENTITY_PTRS] = *(CEntity*(*)[MAX_INVISIBLE_ENTITY_PTRS])0xB745D8;
CEntity* (&CRenderer::ms_aVisibleSuperLodPtrs)[MAX_VISIBLE_SUPERLOD_PTRS] = *(CEntity*(*)[MAX_VISIBLE_SUPERLOD_PTRS])0xB74830;
CEntity* (&CRenderer::ms_aVisibleLodPtrs)[MAX_VISIBLE_LOD_PTRS] = *(CEntity*(*)[MAX_VISIBLE_LOD_PTRS])0xB748F8;
CEntity* (&CRenderer::ms_aVisibleEntityPtrs)[MAX_VISIBLE_ENTITY_PTRS] = *(CEntity*(*)[MAX_VISIBLE_ENTITY_PTRS])0xB75898;
int32& CRenderer::ms_nNoOfVisibleSuperLods = *(int32*)0xB76838;
int32& CRenderer::ms_nNoOfInVisibleEntities = *(int32*)0xB7683C;
int32& CRenderer::ms_nNoOfVisibleLods = *(int32*)0xB76840;
int32& CRenderer::ms_nNoOfVisibleEntities = *(int32*)0xB76844;
float& CRenderer::ms_fFarClipPlane = *(float*)0xB76848;
float& CRenderer::ms_fCameraHeading = *(float*)0xB7684C;
bool& CRenderer::m_loadingPriority = *(bool*)0xB76850;
bool& CRenderer::ms_bInTheSky = *(bool*)0xB76851;
CVector& CRenderer::ms_vecCameraPosition = *(CVector*)0xB76870;
float& CRenderer::ms_lodDistScale = *(float*)0x8CD800;
float& CRenderer::ms_lowLodDistScale = *(float*)0x8CD804;
uint32& gnRendererModelRequestFlags = *(uint32*)0xB745C4;
CEntity**& gpOutEntitiesForGetObjectsInFrustum = *(CEntity***)0xB76854;

void CRenderer::InjectHooks()
{
    RH_ScopedClass(CRenderer);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(RenderFadingInEntities, 0x5531E0);
    RH_ScopedInstall(RenderFadingInUnderwaterEntities, 0x553220);
    RH_ScopedInstall(RenderOneRoad, 0x553230);
    RH_ScopedInstall(RenderOneNonRoad, 0x553260);
    RH_ScopedInstall(RemoveVehiclePedLights, 0x553390);
    RH_ScopedInstall(AddEntityToRenderList, 0x5534B0);
    RH_ScopedInstall(GetLodRenderListBase, 0x5536D0);
    RH_ScopedInstall(GetLodDontRenderListBase, 0x5536E0);
    RH_ScopedInstall(ResetLodRenderLists, 0x5536F0);
    RH_ScopedInstall(AddToLodRenderList, 0x553710);
    RH_ScopedInstall(AddToLodDontRenderList, 0x553740);
    RH_ScopedInstall(ProcessLodRenderLists, 0x553770);
    RH_ScopedInstall(PreRender, 0x553910);
    RH_ScopedInstall(RenderRoads, 0x553A10);
    RH_ScopedInstall(RenderEverythingBarRoads, 0x553AA0);
    RH_ScopedInstall(RenderFirstPersonVehicle, 0x553D00);
    RH_ScopedInstall(SetupLightingForEntity, 0x553E40);
    RH_ScopedInstall(SetupEntityVisibility, 0x554230);
    RH_ScopedInstall(SetupMapEntityVisibility, 0x553F60);
    RH_ScopedInstall(SetupBigBuildingVisibility, 0x554650);
    RH_ScopedInstall(SetupScanLists, 0x553540);
    RH_ScopedInstall(ScanSectorList_ListModels, 0x5535D0);
    RH_ScopedInstall(ScanSectorList_ListModelsVisible, 0x553650);
    RH_ScopedInstall(ScanSectorList, 0x554840);
    RH_ScopedInstall(ScanBigBuildingList, 0x554B10);
    RH_ScopedInstall(ShouldModelBeStreamed, 0x554EB0);
    RH_ScopedInstall(ScanPtrList_RequestModels, 0x555680);
    RH_ScopedInstall(ConstructRenderList, 0x5556E0);
    RH_ScopedInstall(ScanSectorList_RequestModels, 0x555900);
    RH_ScopedInstall(ScanWorld, 0x554FE0);
    RH_ScopedInstall(GetObjectsInFrustum, 0x554C60);
    RH_ScopedInstall(RequestObjectsInFrustum, 0x555960);
    RH_ScopedInstall(RequestObjectsInDirection, 0x555CB0);

}

void CWorldScan::ScanWorld(CVector2D *points, int32 pointsCount, tScanFunction scanFunction)
{
    plugin::Call<0x72CAE0,CVector2D*, int32, tScanFunction>(points,pointsCount, scanFunction);
}

void CWorldScan::SetExtraRectangleToScan(float minX, float maxX, float minY, float maxY)
{
    plugin::Call<0x72D5E0, float, float, float, float>(minX, maxX, minY, maxY);
}

void CRenderer::Init() {
    // empty
}

void CRenderer::Shutdown() {
    // empty
}

// 0x5531E0
void CRenderer::RenderFadingInEntities() {
    RwRenderStateSet(rwRENDERSTATEFOGENABLE,         RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATECULLMODE,          RWRSTATE(rwCULLMODECULLBACK));
    DeActivateDirectional();
    SetAmbientColours();
    CVisibilityPlugins::RenderFadingEntities();
}

// 0x553220
void CRenderer::RenderFadingInUnderwaterEntities() {
    DeActivateDirectional();
    SetAmbientColours();
    CVisibilityPlugins::RenderFadingUnderwaterEntities();
}

// 0x553230
void CRenderer::RenderOneRoad(CEntity* entity) {
    if (CPostEffects::IsVisionFXActive()) {
        CPostEffects::FilterFX_StoreAndSetDayNightBalance();
        entity->Render();
        CPostEffects::FilterFX_RestoreDayNightBalance();
        return;
    }
    entity->Render();
}

// 0x553260
void CRenderer::RenderOneNonRoad(CEntity* entity) {
    if (entity->IsPed() && entity->AsPed()->m_nPedState == PEDSTATE_DRIVING)
        return;

    bool bSetupLighting = entity->SetupLighting();
    auto* vehicle = entity->AsVehicle();
    if (entity->IsVehicle()) {
        CVisibilityPlugins::SetupVehicleVariables(entity->m_pRwClump);
        CVisibilityPlugins::InitAlphaAtomicList();
        vehicle->RenderDriverAndPassengers();
        vehicle->SetupRender();
    } else if (!entity->m_bBackfaceCulled) {
        RwRenderStateSet(rwRENDERSTATECULLMODE, RWRSTATE(rwCULLMODECULLNONE));
    }

    if (CPostEffects::IsVisionFXActive()) {
        if (CPostEffects::m_bNightVision)
            CPostEffects::NightVisionSetLights();
        if (CPostEffects::m_bInfraredVision)
            CPostEffects::InfraredVisionSetLightsForDefaultObjects();
        CPostEffects::FilterFX_StoreAndSetDayNightBalance();
        entity->Render();
        CPostEffects::FilterFX_RestoreDayNightBalance();
    } else {
        entity->Render();
    }

    if (entity->IsVehicle()) {
        vehicle->m_bImBeingRendered = true;
        CVisibilityPlugins::RenderAlphaAtomics();
        vehicle->m_bImBeingRendered = false;
        vehicle->ResetAfterRender();
        vehicle->RemoveLighting(bSetupLighting);
    } else {
        if (!entity->m_bBackfaceCulled)
            RwRenderStateSet(rwRENDERSTATECULLMODE, RWRSTATE(rwCULLMODECULLBACK));
        entity->RemoveLighting(bSetupLighting);
    }
}

// 0x553390
void CRenderer::RemoveVehiclePedLights(CPhysical* entity) {
    if (!entity->physicalFlags.bRenderScorched)
        CPointLights::RemoveLightsAffectingObject();
}

// 0x05534B0
void CRenderer::AddEntityToRenderList(CEntity* entity, float fDistance)
{
    CBaseModelInfo* mi = entity->GetModelInfo();
    mi->SetHasBeenPreRendered(false);
    if (!entity->m_bDistanceFade) {
        if (entity->m_bDrawLast && CVisibilityPlugins::InsertEntityIntoSortedList(entity, fDistance)) {
            entity->m_bDistanceFade = false;
            return;
        }
    }
    else if (CVisibilityPlugins::InsertEntityIntoSortedList(entity, fDistance)) {
        return;
    }
    if (entity->m_nNumLodChildren && !entity->m_bUnderwater) {
        ms_aVisibleLodPtrs[ms_nNoOfVisibleLods] = entity;
        ms_nNoOfVisibleLods++;
        assert(ms_nNoOfVisibleLods <= MAX_VISIBLE_LOD_PTRS);
    }
    else {
        ms_aVisibleEntityPtrs[ms_nNoOfVisibleEntities] = entity;
        ms_nNoOfVisibleEntities++;
        assert(ms_nNoOfVisibleEntities <= MAX_VISIBLE_ENTITY_PTRS);
    }
}

// 0x5536D0
tRenderListEntry* CRenderer::GetLodRenderListBase() {
    return reinterpret_cast<tRenderListEntry*>(&PC_Scratch[24]);
}

// 0x5536E0
tRenderListEntry* CRenderer::GetLodDontRenderListBase() {
    return reinterpret_cast<tRenderListEntry*>(&PC_Scratch[0x2000]);
}

// 0x5536F0
void CRenderer::ResetLodRenderLists() {
    ms_pLodRenderList = GetLodRenderListBase();
    ms_pLodDontRenderList = GetLodDontRenderListBase();
}

// 0x553710
void CRenderer::AddToLodRenderList(CEntity* entity, float distance) {
    ms_pLodRenderList->entity = entity;
    ms_pLodRenderList->distance = distance;
    ++ms_pLodRenderList;
}

// unused
// 0x553740
void CRenderer::AddToLodDontRenderList(CEntity* entity, float distance) {
    ms_pLodDontRenderList->entity = entity;
    ms_pLodDontRenderList->distance = distance;
    ++ms_pLodDontRenderList;
}

// 0x553770
void CRenderer::ProcessLodRenderLists() {
    for (auto renderListEntry = GetLodRenderListBase(); renderListEntry != ms_pLodRenderList; renderListEntry++) {
        CEntity* entity = renderListEntry->entity;
        if (entity && !entity->m_bIsVisible) {
            entity->m_nNumLodChildrenRendered = 0;
            renderListEntry->entity = nullptr;
        }
    }

    const uint8 displaySuperLowLodFlag = 0x80u; // yes, this is very hacky. Blame R*
    bool bAllLodsRendered = false;
    while (bAllLodsRendered) {
        for (auto renderListEntry = GetLodRenderListBase(); renderListEntry != ms_pLodRenderList; renderListEntry++) {
            CEntity* entity = renderListEntry->entity;
            if (entity) {
                if (entity->m_nNumLodChildren > 0 && entity->m_nNumLodChildrenRendered == entity->m_nNumLodChildren) {
                    entity->m_nNumLodChildrenRendered = 0;
                    renderListEntry->entity = nullptr;
                    bAllLodsRendered = true;
                }
                else if (entity->m_pLod) {
                    auto modelInfo = CModelInfo::GetModelInfo(entity->m_nModelIndex);
                    if (modelInfo->m_nAlpha < 255u
                        && entity->m_pLod->m_nNumLodChildrenRendered != displaySuperLowLodFlag
                        && entity->m_pLod->m_bDisplayedSuperLowLOD
                    ) {
                        entity->m_pLod->m_nNumLodChildrenRendered = 0;
                    }
                    if (!entity->m_pRwObject) {
                        if (entity->m_pLod->m_bDisplayedSuperLowLOD)
                            entity->m_pLod->m_nNumLodChildrenRendered = displaySuperLowLodFlag;
                        renderListEntry->entity = nullptr;
                        entity->m_nNumLodChildrenRendered = 0;
                        CStreaming::RequestModel(entity->m_nModelIndex, 0);
                    }
                }
            }
        }
    }
    for (auto renderListEntry = GetLodRenderListBase(); renderListEntry != ms_pLodRenderList; renderListEntry++) {
        CEntity* entity = renderListEntry->entity;
        if (entity && entity->m_nNumLodChildrenRendered > 0) {
            entity->m_bDisplayedSuperLowLOD = false;
            entity->m_nNumLodChildrenRendered = 0;
            renderListEntry->entity = nullptr;
        }
    }
    for (auto renderListEntry = GetLodRenderListBase(); renderListEntry != ms_pLodRenderList; renderListEntry++) {
        CEntity* entity = renderListEntry->entity;
        if (entity) {
            if (entity->m_nNumLodChildrenRendered == displaySuperLowLodFlag || !entity->m_nNumLodChildrenRendered)
            {
                entity->m_bDisplayedSuperLowLOD = true;
                auto modelInfo = CModelInfo::GetModelInfo(entity->m_nModelIndex);
                if (modelInfo->m_nAlpha != 255)
                    entity->m_bDistanceFade = true;
                AddEntityToRenderList(entity, renderListEntry->distance);
            }
            entity->m_nNumLodChildrenRendered = 0;
        }
    }
}

// 0x553910
void CRenderer::PreRender() {
    ZoneScoped;

    assert(ms_nNoOfVisibleLods <= MAX_VISIBLE_LOD_PTRS);
    std::ranges::for_each(GetVisibleLodPtrs(), [](auto& entity) { entity->PreRender(); });

    assert(ms_nNoOfVisibleEntities <= MAX_VISIBLE_ENTITY_PTRS);
    std::ranges::for_each(GetVisibleEntityPtrs(), [](auto& entity) { entity->PreRender(); });

    assert(ms_nNoOfVisibleSuperLods <= MAX_VISIBLE_SUPERLOD_PTRS);
    std::ranges::for_each(GetVisibleSuperLodPtrs(), [](auto& entity) { entity->PreRender(); });

    assert(ms_nNoOfInVisibleEntities <= MAX_INVISIBLE_ENTITY_PTRS);
    std::ranges::for_each(GetInVisibleEntityPtrs(), [](auto& entity) { entity->PreRender(); });

    for (auto* link = CVisibilityPlugins::m_alphaEntityList.usedListHead.next;
        link != &CVisibilityPlugins::m_alphaEntityList.usedListTail;
        link = link->next
    ) {
        // NOTSA: HACK: We compare function pointers, and want it to work with reversible hooks,
        // we need to check for both original function and our one
        if (link->data.m_pCallback == CVisibilityPlugins::RenderEntity || link->data.m_pCallback == (void*)0x732B40) {
            link->data.m_entity->m_bOffscreen = false;
            link->data.m_entity->PreRender();
        }
    }

    for (auto* link = CVisibilityPlugins::m_alphaUnderwaterEntityList.usedListHead.next;
        link != &CVisibilityPlugins::m_alphaUnderwaterEntityList.usedListTail;
        link = link->next
    ) {
        // todo: NOTSA: HACK: We compare function pointers, and want it to work with reversible hooks,
        // we need to check for both original function and our one
        if (link->data.m_pCallback == CVisibilityPlugins::RenderEntity || link->data.m_pCallback == (void*)0x732B40) {
            link->data.m_entity->m_bOffscreen = false;
            link->data.m_entity->PreRender();
        }
    }
    CHeli::SpecialHeliPreRender();
    CShadows::RenderExtraPlayerShadows();
}

// 0x553A10
void CRenderer::RenderRoads() {
    assert(ms_nNoOfVisibleEntities <= MAX_VISIBLE_ENTITY_PTRS);

    RwRenderStateSet(rwRENDERSTATEFOGENABLE,         RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATECULLMODE,          RWRSTATE(rwCULLMODECULLBACK));

    DeActivateDirectional();
    SetAmbientColours();

    for (auto& entity : GetVisibleEntityPtrs()) {
        if (entity->IsBuilding() && CModelInfo::GetModelInfo(entity->m_nModelIndex)->IsRoad()) {
            if (CPostEffects::IsVisionFXActive()) {
                CPostEffects::FilterFX_StoreAndSetDayNightBalance();
                entity->Render();
                CPostEffects::FilterFX_RestoreDayNightBalance();
            }
            else {
                entity->Render();
            }
        }
    }
}

// 0x553AA0
void CRenderer::RenderEverythingBarRoads() {
    RwRenderStateSet(rwRENDERSTATEFOGENABLE,         RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATECULLMODE,          RWRSTATE(rwCULLMODECULLBACK));
    if (!CGame::currArea)
        RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, RWRSTATE(140u));

    assert(ms_nNoOfVisibleEntities <= MAX_VISIBLE_ENTITY_PTRS);
    for (auto& entity : GetVisibleEntityPtrs()) {
        auto* vehicle = entity->AsVehicle();
        if (entity->IsBuilding() && CModelInfo::GetModelInfo(entity->m_nModelIndex)->IsRoad())
            continue;

        bool bInserted = false;
        if (entity->IsVehicle() || (entity->IsPed() && CVisibilityPlugins::GetClumpAlpha(entity->m_pRwClump) != 255)) {
            // todo: R* nice check | or we missed smth here?
            if (entity->IsVehicle()) {
                bool bInsertIntoSortedList = false;
                if (vehicle->IsBoat()) {
                    const auto& camMode = CCamera::GetActiveCamera().m_nMode;
                    const auto& lookDirection = TheCamera.GetLookDirection();
                    if (camMode == MODE_WHEELCAM || camMode == MODE_1STPERSON &&
                        lookDirection != LOOKING_DIRECTION_FORWARD && lookDirection != LOOKING_DIRECTION_UNKNOWN_1 ||
                        CVisibilityPlugins::GetClumpAlpha(entity->m_pRwClump) != 255
                    )
                    {
                        bInsertIntoSortedList = true;
                    }
                }
                else if (!vehicle->physicalFlags.bTouchingWater) {
                    bInsertIntoSortedList = true;
                }
                const float fMagnitude = DistanceBetweenPoints(entity->GetPosition(), ms_vecCameraPosition);
                if (bInsertIntoSortedList)
                    bInserted = CVisibilityPlugins::InsertEntityIntoSortedList(entity, fMagnitude);
                else
                    bInserted = CVisibilityPlugins::InsertEntityIntoUnderwaterEntities(entity, fMagnitude);
            }
        }
        if (!bInserted)
            RenderOneNonRoad(entity);
    }
    float oldzShift = Scene.m_pRwCamera->zShift;
    RwCameraEndUpdate(Scene.m_pRwCamera);

    Scene.m_pRwCamera->zShift -= 100.0f;
    RwCameraBeginUpdate(Scene.m_pRwCamera);
    for (auto& entity : GetVisibleLodPtrs()) {
        RenderOneNonRoad(entity);
    }
    RwCameraEndUpdate(Scene.m_pRwCamera);

    Scene.m_pRwCamera->zShift = oldzShift;
    RwCameraBeginUpdate(Scene.m_pRwCamera);
}

// 0x553D00
void CRenderer::RenderFirstPersonVehicle() {
    ZoneScoped;

    if (m_pFirstPersonVehicle) {
        bool bRestoreAlphaTest = false;
        if (FindPlayerPed(0)->GetActiveWeapon().m_Type == WEAPON_MICRO_UZI) {
            bRestoreAlphaTest = true;
            RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, RWRSTATE(80u));
        }
        RwRenderStateSet(rwRENDERSTATEFOGENABLE,         RWRSTATE(TRUE));
        RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,      RWRSTATE(TRUE));
        RwRenderStateSet(rwRENDERSTATEZTESTENABLE,       RWRSTATE(TRUE));
        RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(TRUE));
        RwRenderStateSet(rwRENDERSTATESRCBLEND,          RWRSTATE(rwBLENDSRCALPHA));
        RwRenderStateSet(rwRENDERSTATEDESTBLEND,         RWRSTATE(rwBLENDINVSRCALPHA));
        RenderOneNonRoad(m_pFirstPersonVehicle);
        RwRenderStateSet(rwRENDERSTATEFOGENABLE,         RWRSTATE(FALSE));
        if (bRestoreAlphaTest)
            RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, RWRSTATE(NULL));
    }
}

// 0x553E40
bool CRenderer::SetupLightingForEntity(CPhysical* entity) {
    CCarFXRenderer::SetFxEnvMapLightMult(1.0f);
    if (entity->physicalFlags.bRenderScorched) {
        WorldReplaceNormalLightsWithScorched(Scene.m_pRpWorld, 0.18f);
        return false;
    }
    entity->m_fDynamicLighting = 0.0f;
    const CVector& point = entity->GetPosition();
    float generatedLightings = CPointLights::GenerateLightsAffectingObject(&point, &entity->m_fDynamicLighting, entity);
    float lightingMultiplier = (entity->GetLightingFromCol(true) * (1.0f - 0.05f) + 0.05f) * generatedLightings;
    CCarFXRenderer::SetFxEnvMapLightMult(lightingMultiplier);
    SetLightColoursForPedsCarsAndObjects(lightingMultiplier);
    return true;
}

// 0x553F60
int32 CRenderer::SetupMapEntityVisibility(CEntity* entity, CBaseModelInfo* baseModelInfo, float fDistance, bool bIsTimeInRange) {
    if (!entity->m_bTunnelTransition) {
        if (!ms_bRenderTunnels && entity->m_bTunnel || (!ms_bRenderOutsideTunnels && !entity->m_bTunnel)) {
            return RENDERER_INVISIBLE;
        }
    }

    const float fFarClipRadius = baseModelInfo->GetColModel()->GetBoundRadius() + ms_fFarClipPlane;
    float fDrawDistanceRadius = std::min(TheCamera.m_fLODDistMultiplier * baseModelInfo->m_fDrawDistance, fFarClipRadius);
    float fFadingDistance = MAX_FADING_DISTANCE;
    if (!entity->m_pLod) {
        float fDrawDistance = std::min(baseModelInfo->m_fDrawDistance, fDrawDistanceRadius);
        if (fDrawDistance > MAX_LOWLOD_DISTANCE)
            fFadingDistance = fDrawDistance / 15.0f + 10.0f;
        if (entity->m_bIsBIGBuilding)
            fDrawDistanceRadius *= ms_lowLodDistScale;
    }

    if (!baseModelInfo->m_pRwObject) {
        if (entity->m_pLod && entity->m_pLod->m_nNumLodChildren > 1u &&
            fFadingDistance + fDistance - MAX_FADING_DISTANCE < fDrawDistanceRadius)
        {
            AddToLodRenderList(entity, fDistance);
            return RENDERER_STREAMME;
        }
    }

    if (!baseModelInfo->m_pRwObject || (fFadingDistance + fDistance - MAX_FADING_DISTANCE >= fDrawDistanceRadius)) {
        if (entity->m_bDontStream)
            return RENDERER_INVISIBLE;
        if (baseModelInfo->m_pRwObject && fDistance - MAX_FADING_DISTANCE < fDrawDistanceRadius) {
            if (!entity->m_pRwObject) {
                entity->CreateRwObject();
                if (!entity->m_pRwObject)
                    return RENDERER_INVISIBLE;
            }
            if (!entity->m_bIsVisible)
                return RENDERER_INVISIBLE;
            if (!entity->GetIsOnScreen() || entity->IsEntityOccluded()) {
                if (!baseModelInfo->HasBeenPreRendered()) {
                    baseModelInfo->m_nAlpha = 255;
                }
                baseModelInfo->SetHasBeenPreRendered(false);
                return RENDERER_INVISIBLE;
            }
            entity->m_bDistanceFade = true;
            if (entity->m_pLod && entity->m_pLod->m_nNumLodChildren > 1u)
                AddToLodRenderList(entity, fDistance);
            else
                AddEntityToRenderList(entity, fDistance);
            return RENDERER_INVISIBLE;
        }
        if (fDistance - MAX_STREAMING_DISTANCE >= fDrawDistanceRadius || !bIsTimeInRange || !entity->m_bIsVisible)
            return RENDERER_INVISIBLE;
        if (!entity->m_pRwObject)
            entity->CreateRwObject();
        return RENDERER_STREAMME;
    }

    if (!entity->m_pRwObject) {
        entity->CreateRwObject();
        if (!entity->m_pRwObject)
            return RENDERER_INVISIBLE;
    }

    if (!entity->m_bIsVisible)
        return RENDERER_INVISIBLE;

    if (entity->GetIsOnScreen() && !entity->IsEntityOccluded()) {
        if (baseModelInfo->m_nAlpha == 255)
            entity->m_bDistanceFade = false;
        else
            entity->m_bDistanceFade = true;
        if (!entity->m_pLod)
            return RENDERER_VISIBLE;
        if (baseModelInfo->m_nAlpha == 255)
            entity->m_pLod->m_nNumLodChildrenRendered++;
        if (entity->m_pLod->m_nNumLodChildren <= 1u)
            return RENDERER_VISIBLE;
        AddToLodRenderList(entity, fDistance);
        return RENDERER_INVISIBLE;
    }
    if (!baseModelInfo->HasBeenPreRendered())
        baseModelInfo->m_nAlpha = 255;
    baseModelInfo->SetHasBeenPreRendered(false);
    return RENDERER_CULLED;
}

// 0x554230
int32 CRenderer::SetupEntityVisibility(CEntity* entity, float& outDistance) {
    const int32& modelId = entity->m_nModelIndex;
    CBaseModelInfo* baseModelInfo = CModelInfo::GetModelInfo(modelId);
    CBaseModelInfo* baseAtomicModelInfo = baseModelInfo->AsAtomicModelInfoPtr();
    if (entity->IsVehicle() && !entity->m_bTunnelTransition) {
        if (!ms_bRenderTunnels && entity->m_bTunnel || !ms_bRenderOutsideTunnels && !entity->m_bTunnel)
            return RENDERER_INVISIBLE;
    }

    bool bIsTimeInRange = true;
    if (!baseAtomicModelInfo)
    {
        if (baseModelInfo->GetModelType() != MODEL_INFO_CLUMP && baseModelInfo->GetModelType() != MODEL_INFO_WEAPON)
        {
            if (FindPlayerVehicle() == entity && gbFirstPersonRunThisFrame && CReplay::Mode != MODE_PLAYBACK) {
                uint32 dwDirectionWasLooking = CCamera::GetActiveCamera().m_nDirectionWasLooking;
                CVehicle* vehicle = FindPlayerVehicle();
                if (!vehicle->IsBike() || !(vehicle->AsBike()->bikeFlags.bWheelieForCamera))
                {
                    if (dwDirectionWasLooking == 3)
                        return RENDERER_CULLED;

                    if (modelId == MODEL_RHINO || modelId == MODEL_COACH || TheCamera.m_bInATunnelAndABigVehicle)
                        return RENDERER_CULLED;

                    if (dwDirectionWasLooking) {
                        m_pFirstPersonVehicle = entity->AsVehicle();
                        return RENDERER_CULLED;
                    }

                    if (vehicle->m_pHandlingData->m_bNo1fpsLookBehind)
                        return RENDERER_CULLED;

                    if (!vehicle->IsBoat()
                        || modelId == MODEL_REEFER || modelId == MODEL_TROPIC || modelId == MODEL_PREDATOR
                        || modelId == MODEL_SKIMMER
                    ) {
                        m_pFirstPersonVehicle = entity->AsVehicle();
                        return RENDERER_CULLED;
                    }
                }
            }

            if (!entity->m_pRwObject
                || !entity->m_bIsVisible && (!CMirrors::TypeOfMirror || entity->m_nModelIndex)
                || !entity->IsInCurrentAreaOrBarberShopInterior() && entity->IsVehicle()
            ) {
                return RENDERER_INVISIBLE;
            }

            if (!entity->GetIsOnScreen() || entity->IsEntityOccluded()) {
                return RENDERER_CULLED;
            }

            if (entity->m_bWasPostponed) {
                entity->m_bDistanceFade = false;
                AddEntityToRenderList(entity, DistanceBetweenPoints(ms_vecCameraPosition, entity->GetPosition()));
                return RENDERER_INVISIBLE;
            }
            return RENDERER_VISIBLE;
        }
    }
    else
    {
        if (baseModelInfo->GetModelType() == MODEL_INFO_TIME)
        {
            CTimeInfo* modelTimeInfo = baseModelInfo->GetTimeInfo();
            int32 otherTimeModel = modelTimeInfo->GetOtherTimeModel();
            if (CClock::GetIsTimeInRange(modelTimeInfo->GetTimeOn(), modelTimeInfo->GetTimeOff()))
            {
                if (otherTimeModel != -1 && CModelInfo::GetModelInfo(otherTimeModel)->m_pRwObject) {
                    baseModelInfo->m_nAlpha = 255;
                }
            }
            else
            {
                if (otherTimeModel == -1 || CModelInfo::GetModelInfo(otherTimeModel)->m_pRwObject)
                {
                    entity->DeleteRwObject();
                    return RENDERER_INVISIBLE;
                }
                bIsTimeInRange = false;
            }
        }
        else
        {
            if (entity->m_bDontStream)
            {
                if (!entity->m_pRwObject || !entity->m_bIsVisible && (!CMirrors::TypeOfMirror || entity->m_nModelIndex))
                {
                    return RENDERER_INVISIBLE;
                }

                if (!entity->GetIsOnScreen() || entity->IsEntityOccluded())
                {
                    return RENDERER_CULLED;
                }

                if (!entity->m_bWasPostponed)
                {
                    return RENDERER_VISIBLE;
                }

                CVisibilityPlugins::InsertEntityIntoSortedList(entity, (entity->GetPosition() - ms_vecCameraPosition).Magnitude());
                entity->m_bDistanceFade = false;
                return RENDERER_INVISIBLE;
            }
        }
    }

    if (entity->m_nAreaCode == CGame::currArea || entity->m_nAreaCode == AREA_CODE_13) {
        CVector position = entity->GetPosition();
        if (entity->m_pLod) {
            position = entity->m_pLod->GetPosition();
        }

        outDistance = DistanceBetweenPoints(ms_vecCameraPosition, position);
        if (outDistance > MAX_LOD_DISTANCE) {
            const float fDrawDistanceRadius = TheCamera.m_fLODDistMultiplier * baseModelInfo->m_fDrawDistance;
            if (fDrawDistanceRadius > MAX_LOD_DISTANCE &&
                fDrawDistanceRadius + MAX_FADING_DISTANCE > outDistance
            ) {
                outDistance += fDrawDistanceRadius - MAX_LOD_DISTANCE;
            }
        }
        return SetupMapEntityVisibility(entity, baseModelInfo, outDistance, bIsTimeInRange);
    }

    return RENDERER_INVISIBLE;
}

// 0x554650
int32 CRenderer::SetupBigBuildingVisibility(CEntity* entity, float& outDistance) {
    CBaseModelInfo* baseModelInfo = CModelInfo::GetModelInfo(entity->m_nModelIndex);
    bool bIsTimeInRange = true;
    if (entity->m_nAreaCode != CGame::currArea && entity->m_nAreaCode != AREA_CODE_13)
        return RENDERER_INVISIBLE;

    if (baseModelInfo->GetModelType() == MODEL_INFO_TIME)
    {
        CTimeInfo* timeInfo = baseModelInfo->GetTimeInfo();
        int32 otherTimeModel = timeInfo->GetOtherTimeModel();
        if (CClock::GetIsTimeInRange(timeInfo->GetTimeOn(), timeInfo->GetTimeOff()))
        {
            if (otherTimeModel != -1 && CModelInfo::GetModelInfo(otherTimeModel)->m_pRwObject)
                baseModelInfo->m_nAlpha = 255;
        }
        else
        {
            if (otherTimeModel == -1 || CModelInfo::GetModelInfo(otherTimeModel)->m_pRwObject) {
                entity->DeleteRwObject();
                return RENDERER_INVISIBLE;
            }
            bIsTimeInRange = false;
        }
    }
    else if (baseModelInfo->GetModelType() == MODEL_INFO_VEHICLE) {
        return entity->IsVisible() && !entity->IsEntityOccluded() ? RENDERER_VISIBLE : RENDERER_INVISIBLE;
    }

    CVector entityPos = entity->GetPosition();
    if (entity->m_pLod) {
        entityPos = entity->m_pLod->GetPosition();
    }

    outDistance = DistanceBetweenPoints(ms_vecCameraPosition, entityPos);
    if (entity->m_nNumLodChildrenRendered <= 0) {
        int32 visibility = SetupMapEntityVisibility(entity, baseModelInfo, outDistance, bIsTimeInRange);
        if (visibility != RENDERER_VISIBLE || entity->m_nNumLodChildren <= 1u) {
            return visibility;
        }
        if (entity->m_pLod && baseModelInfo->m_nAlpha == 255) {
            ++entity->m_pLod->m_nNumLodChildrenRendered;
        }
        AddToLodRenderList(entity, outDistance);
        return RENDERER_INVISIBLE;
    }

    if (entity->m_pLod)
        ++entity->m_pLod->m_nNumLodChildrenRendered;

    if (entity->m_nNumLodChildren <= 1u) {
        entity->m_nNumLodChildrenRendered = 0;
    } else {
        ms_pLodRenderList->entity = entity;
        ms_pLodRenderList->distance = outDistance;
        ms_pLodRenderList++;
    }

    if (!entity->m_pRwObject)
        entity->CreateRwObject();

    return RENDERER_STREAMME;
}

// 0x5535D0
void CRenderer::ScanSectorList_ListModels(int32 sectorX, int32 sectorY) {
    SetupScanLists(sectorX, sectorY);
    auto** scanLists = reinterpret_cast<CPtrListDoubleLink**>(&PC_Scratch);
    for (int32 scanListIndex = 0; scanListIndex < TOTAL_ENTITY_SCAN_LISTS; scanListIndex++) {
        CPtrListDoubleLink* doubleLinkList = scanLists[scanListIndex];
        if (!doubleLinkList)
            continue;

        for (auto node = doubleLinkList->GetNode(); node; node = node->m_next) {
            auto* entity = reinterpret_cast<CEntity*>(node->m_item);
            if (!entity->IsScanCodeCurrent()) {
                entity->SetCurrentScanCode() ;
                if (entity->m_nAreaCode == CGame::currArea || entity->m_nAreaCode == AREA_CODE_13) {
                    *gpOutEntitiesForGetObjectsInFrustum = entity;
                    gpOutEntitiesForGetObjectsInFrustum++;
                }
            }
        }
    }
}

// 0x553650
void CRenderer::ScanSectorList_ListModelsVisible(int32 sectorX, int32 sectorY) {
    SetupScanLists(sectorX, sectorY);
    auto** scanLists = reinterpret_cast<CPtrListDoubleLink**>(&PC_Scratch);
    for (int32 scanListIndex = 0; scanListIndex < TOTAL_ENTITY_SCAN_LISTS; scanListIndex++) {
        CPtrListDoubleLink* doubleLinkList = scanLists[scanListIndex];
        if (!doubleLinkList)
            continue;

        for (auto node = doubleLinkList->GetNode(); node; node = node->m_next) {
            auto* entity = reinterpret_cast<CEntity*>(node->m_item);
            if (!entity->IsScanCodeCurrent()) {
                entity->SetCurrentScanCode() ;
                if (entity->m_nAreaCode == CGame::currArea || entity->m_nAreaCode == AREA_CODE_13) {
                    if (entity->IsVisible()) {
                        *gpOutEntitiesForGetObjectsInFrustum = entity;
                        gpOutEntitiesForGetObjectsInFrustum++;
                    }
                }
            }
        }
    }
}

// 0x554840
void CRenderer::ScanSectorList(int32 sectorX, int32 sectorY) {
    bool bRequestModel = false;
    float fDistanceX = CWorld::GetSectorPosX(sectorX) - ms_vecCameraPosition.x;
    float fDistanceY = CWorld::GetSectorPosY(sectorY) - ms_vecCameraPosition.y;
    float fAngleInRadians = std::atan2(-fDistanceX, fDistanceY) - ms_fCameraHeading;
    if (CVector2D(fDistanceX, fDistanceY).SquaredMagnitude() < MAX_STREAMING_RADIUS_SQUARED ||
        std::fabs(CGeneral::LimitRadianAngle(fAngleInRadians)) < STREAMING_ANGLE_THRESHOLD_RAD
    ) {
        bRequestModel = true;
    }

    SetupScanLists(sectorX, sectorY);
    auto* scanLists = reinterpret_cast<tScanLists*>(&PC_Scratch);
    for (int32 scanListIndex = 0; scanListIndex < TOTAL_ENTITY_SCAN_LISTS; scanListIndex++) {
        CPtrListDoubleLink* doubleLinkList = scanLists->GetList(scanListIndex);
        if (!doubleLinkList)
            continue;

        CPtrNodeDoubleLink* doubleLinkNode = doubleLinkList->GetNode();
        while (doubleLinkNode) {
            auto* entity = reinterpret_cast<CEntity*>(doubleLinkNode->m_item);
            doubleLinkNode = doubleLinkNode->m_next;
            if (entity->IsScanCodeCurrent())
                continue;

            entity->SetCurrentScanCode() ;
            entity->m_bOffscreen = false;
            bool bInvisibleEntity = false;
            float fDistance = 0.0f;
            switch (SetupEntityVisibility(entity, fDistance)) {
            case RENDERER_INVISIBLE: {
                if (entity->IsObject()) {
                    auto* atomicModelInfo = CModelInfo::GetModelInfo(entity->m_nModelIndex)->AsAtomicModelInfoPtr();
                    if (atomicModelInfo && atomicModelInfo->IsGlass()) {
                        bInvisibleEntity = true;
                    }
                }
                break;
            }
            case RENDERER_VISIBLE: {
                AddEntityToRenderList(entity, fDistance);
                break;
            }
            case RENDERER_CULLED: {
                bInvisibleEntity = true;
                break;
            }
            case RENDERER_STREAMME: {
                if (CStreaming::ms_disableStreaming || !entity->GetIsOnScreen() || ms_bInTheSky)
                    break;

                if (bRequestModel) {
                    if (CStreaming::GetInfo(entity->m_nModelIndex).IsLoaded()) {
                        CStreaming::RequestModel(entity->m_nModelIndex, 0);
                        break;
                    } else if (!entity->IsEntityOccluded()) {
                        SetLoadingPriority(1);
                        CStreaming::RequestModel(entity->m_nModelIndex, 0);
                        break;
                    }
                }

                if (!m_loadingPriority || CStreaming::ms_numModelsRequested < 1)
                    CStreaming::RequestModel(entity->m_nModelIndex, 0);

                break;
            }
            default:
                break;
            }

            if (!bInvisibleEntity)
                continue;

            entity->m_bOffscreen = true;
            if (entity->m_bHasPreRenderEffects) {
                float fDrawDistance = MAX_INVISIBLE_ENTITY_DISTANCE;
                CVector2D distance = ms_vecCameraPosition - entity->GetPosition();
                if (entity->IsVehicle()) {
                    if (entity->AsVehicle()->vehicleFlags.bAlwaysSkidMarks) {
                        fDrawDistance = MAX_INVISIBLE_VEHICLE_DISTANCE;
                    }
                }
                if (distance.x > -fDrawDistance && distance.x < fDrawDistance &&
                    distance.y > -fDrawDistance && distance.y < fDrawDistance
                ) {
                    if (ms_nNoOfInVisibleEntities < MAX_INVISIBLE_ENTITY_PTRS - 1) {
                        ms_aInVisibleEntityPtrs[ms_nNoOfInVisibleEntities] = entity;
                        ms_nNoOfInVisibleEntities++;
                    }
                }
            }
        }
    }
}

// 0x554B10
void CRenderer::ScanBigBuildingList(int32 sectorX, int32 sectorY) {
    if (sectorX < 0 || sectorY < 0 || sectorX >= MAX_LOD_PTR_LISTS_X || sectorY >= MAX_LOD_PTR_LISTS_Y)
        return;

    bool bRequestModel = false;
    float fDistanceX = CWorld::GetLodSectorPosX(sectorX) - ms_vecCameraPosition.x;
    float fDistanceY = CWorld::GetLodSectorPosY(sectorY) - ms_vecCameraPosition.y;
    float fAngleInRadians = std::atan2(-fDistanceX, fDistanceY) - ms_fCameraHeading;
    if (CVector2D(fDistanceX, fDistanceY).SquaredMagnitude() < MAX_BIGBUILDING_STREAMING_RADIUS_SQUARED ||
        std::fabs(CGeneral::LimitRadianAngle(fAngleInRadians)) <= BIGBUILDING_STREAMING_ANGLE_THRESHOLD_RAD
    ) {
        bRequestModel = true;
    }

    CPtrList& list = CWorld::GetLodPtrList(sectorX, sectorY);
    auto* node = list.GetNode();
    while (node) {
        auto* entity = reinterpret_cast<CEntity*>(node->m_item);
        node = node->GetNext();

        if (entity->IsScanCodeCurrent())
            continue;

        entity->SetCurrentScanCode() ;

        float fDistance = 0.0f;
        switch (SetupBigBuildingVisibility(entity, fDistance)) {
        case RENDERER_CULLED:
        case RENDERER_INVISIBLE:
            break;
        case RENDERER_STREAMME:
            if (!CStreaming::ms_disableStreaming && bRequestModel) {
                CStreaming::RequestModel(entity->m_nModelIndex, 0);
            }
            break;
        case RENDERER_VISIBLE:
            AddEntityToRenderList(entity, fDistance + 0.01f);
            entity->m_bOffscreen = false;
            break;
        default:
            assert(false);
            break;
        }
    }
}

// 0x554EB0
bool CRenderer::ShouldModelBeStreamed(CEntity* entity, const CVector& point, float farClip) {
    if (entity->m_nAreaCode != CGame::currArea && entity->m_nAreaCode != AREA_CODE_13)
        return false;

    CBaseModelInfo* modelInfo = CModelInfo::GetModelInfo(entity->m_nModelIndex);
    CTimeInfo* timeInfo = modelInfo->GetTimeInfo();
    if (timeInfo && !CClock::GetIsTimeInRange(timeInfo->GetTimeOn(), timeInfo->GetTimeOff()))
        return false;

    const float fMagnitude = DistanceBetweenPoints(point, entity->GetPosition());
    const float fDrawDistanceRadius = TheCamera.m_fLODDistMultiplier * modelInfo->m_fDrawDistance;
    if (fMagnitude <= modelInfo->GetColModel()->GetBoundRadius() + farClip && fDrawDistanceRadius > fMagnitude)
        return true;

    return false;
}

// 0x555680
void CRenderer::ScanPtrList_RequestModels(CPtrList& list) {
    for (auto node = list.GetNode(); node; node = node->m_next) {
        auto* entity = reinterpret_cast<CEntity*>(node->m_item);
        if (!entity->IsScanCodeCurrent()) {
            entity->SetCurrentScanCode() ;
            if (ShouldModelBeStreamed(entity, ms_vecCameraPosition, ms_fFarClipPlane))
                CStreaming::RequestModel(entity->m_nModelIndex, gnRendererModelRequestFlags);
        }
    }
}

// 0x5556E0
void CRenderer::ConstructRenderList() {
    ZoneScoped;

    const auto& camPos = TheCamera.GetPosition();

    eZoneAttributes zoneAttributes = CCullZones::FindTunnelAttributesForCoors(camPos);
    ms_bRenderTunnels = (zoneAttributes & (eZoneAttributes::UNKNOWN_2 | eZoneAttributes::UNKNOWN_1)) != 0;
    if ((zoneAttributes & eZoneAttributes::UNKNOWN_1) || !(zoneAttributes & eZoneAttributes::UNKNOWN_2))
        ms_bRenderOutsideTunnels = true;
    else
        ms_bRenderOutsideTunnels = false;

    ms_lowLodDistScale = 1.0f;
    ms_bInTheSky = false;

    CPlayerPed* player = FindPlayerPed();
    if (player && player->m_nAreaCode == AREA_CODE_NORMAL_WORLD) {
        float fGroundHeightZ = TheCamera.CalculateGroundHeight(eGroundHeightType::ENTITY_BB_BOTTOM);
        float fPlayerHeightZ = player->GetPosition().z;

        if (fPlayerHeightZ - fGroundHeightZ > 50.0f) {
            fGroundHeightZ = TheCamera.CalculateGroundHeight(eGroundHeightType::ENTITY_BB_TOP);
            if (fPlayerHeightZ - fGroundHeightZ > 10.0f && FindPlayerVehicle()) {
                ms_bInTheSky = true;
            }
        }

        if (camPos.z > LOWLOD_CAMERA_HEIGHT_THRESHOLD) {
            float fScale = (camPos.z - LOWLOD_CAMERA_HEIGHT_THRESHOLD) / (250.0f - LOWLOD_CAMERA_HEIGHT_THRESHOLD);
            fScale = std::min(1.0f, fScale);
            ms_lowLodDistScale = fScale * (2.2f - 1.0f) + 1.0f;
        }
    }

    ms_lowLodDistScale *= CTimeCycle::m_CurrentColours.m_fLodDistMult;
    CMirrors::BeforeConstructRenderList();
    COcclusion::ProcessBeforeRendering();
    ms_nNoOfVisibleEntities = 0;
    ms_nNoOfVisibleLods = 0;
    ms_nNoOfInVisibleEntities = 0;
    ms_vecCameraPosition = camPos;
    ms_fCameraHeading = TheCamera.GetHeading();
    ms_fFarClipPlane = TheCamera.m_pRwCamera->farPlane;
    ResetLodRenderLists();
    ScanWorld();
    ProcessLodRenderLists();
    CStreaming::StartRenderEntities();
}

// 0x555900
void CRenderer::ScanSectorList_RequestModels(int32 sectorX, int32 sectorY) {
    if (sectorX >= 0 && sectorY >= 0 && sectorX < MAX_SECTORS_X && sectorY < MAX_SECTORS_Y) {
        CSector* sector = GetSector(sectorX, sectorY);
        ScanPtrList_RequestModels(sector->m_buildings);
        ScanPtrList_RequestModels(sector->m_dummies);
        ScanPtrList_RequestModels(GetRepeatSector(sectorX, sectorY)->GetList(REPEATSECTOR_OBJECTS));
    }
}

// 0x554FE0
void CRenderer::ScanWorld() {
    const float& farPlane = TheCamera.m_pRwCamera->farPlane;
    const float& width  = TheCamera.m_pRwCamera->viewWindow.x;
    const float& height = TheCamera.m_pRwCamera->viewWindow.y;

    CVector frustumPoints[13]{};
    frustumPoints[0] = CVector(0.0f, 0.0f, 0.0f);

    frustumPoints[1].x = frustumPoints[4].x = -(farPlane * width);
    frustumPoints[1].y = frustumPoints[2].y = farPlane * height;

    frustumPoints[2].x = frustumPoints[3].x = farPlane * width;

    frustumPoints[3].y = frustumPoints[4].y = -(farPlane * height);

    frustumPoints[1].z = frustumPoints[2].z = frustumPoints[3].z = frustumPoints[4].z = farPlane;

    // Clear the rest of the array
    for (auto i = 5u; i < std::size(frustumPoints); i++) {
        frustumPoints[i] = CVector(0.0f, 0.0f, 0.0f);
    }

    m_pFirstPersonVehicle = nullptr;
    CVisibilityPlugins::InitAlphaEntityList();

    CWorld::IncrementCurrentScanCode();

    static CVector& lastCameraPosition = *(CVector*)0xB76888; //TODO | STATICREF
    static CVector& lastCameraForward = *(CVector*)0xB7687C; //TODO | STATICREF

    CVector distance = TheCamera.GetPosition() - lastCameraPosition;
    static bool bUnusedBool = false;
    if (DotProduct(distance, lastCameraForward) >= 16.0f || DotProduct(TheCamera.m_mCameraMatrix.GetForward(), lastCameraForward) <= 0.98f)
        bUnusedBool = false;
    else
        bUnusedBool = true;

    lastCameraPosition = TheCamera.GetPosition();
    lastCameraForward = TheCamera.m_mCameraMatrix.GetForward();
    frustumPoints[5] = (frustumPoints[1] * MAX_LOD_DISTANCE) / farPlane;
    frustumPoints[7].x = frustumPoints[5].x / 5;
    frustumPoints[7].y = frustumPoints[5].y / 5;
    frustumPoints[7].z = frustumPoints[5].z;

    frustumPoints[6] = (frustumPoints[2] * MAX_LOD_DISTANCE) / farPlane;
    frustumPoints[8].x = frustumPoints[6].x / 5;
    frustumPoints[8].y = frustumPoints[6].y / 5;
    frustumPoints[8].z = frustumPoints[6].z;

    frustumPoints[9] = (frustumPoints[3] * MAX_LOD_DISTANCE) / farPlane;
    frustumPoints[11].x = frustumPoints[9].x / 5;
    frustumPoints[11].y = frustumPoints[9].y / 5;
    frustumPoints[11].z = frustumPoints[9].z;

    frustumPoints[10] = (frustumPoints[4] * MAX_LOD_DISTANCE) / farPlane;
    frustumPoints[12].x = frustumPoints[10].x / 5;
    frustumPoints[12].y = frustumPoints[10].y / 5;
    frustumPoints[12].z = frustumPoints[10].z;
    RwV3dTransformPoints(frustumPoints, frustumPoints, 13, TheCamera.GetRwMatrix());
    m_loadingPriority = 0;

    CVector2D points[5] = {
        { CWorld::GetSectorfX(frustumPoints[0].x),  CWorld::GetSectorfY(frustumPoints[0].y)  },
        { CWorld::GetSectorfX(frustumPoints[5].x),  CWorld::GetSectorfY(frustumPoints[5].y)  },
        { CWorld::GetSectorfX(frustumPoints[6].x),  CWorld::GetSectorfY(frustumPoints[6].y)  },
        { CWorld::GetSectorfX(frustumPoints[9].x),  CWorld::GetSectorfY(frustumPoints[9].y)  },
        { CWorld::GetSectorfX(frustumPoints[10].x), CWorld::GetSectorfY(frustumPoints[10].y) },
    };
    CWorldScan::ScanWorld(points, (int)std::size(points), ScanSectorList);

    points[0].x = CWorld::GetLodSectorfX(frustumPoints[0].x);
    points[0].y = CWorld::GetLodSectorfY(frustumPoints[0].y);

    points[1].x = CWorld::GetLodSectorfX(frustumPoints[1].x);
    points[1].y = CWorld::GetLodSectorfY(frustumPoints[1].y);

    points[2].x = CWorld::GetLodSectorfX(frustumPoints[2].x);
    points[2].y = CWorld::GetLodSectorfY(frustumPoints[2].y);

    points[3].x = CWorld::GetLodSectorfX(frustumPoints[3].x);
    points[3].y = CWorld::GetLodSectorfY(frustumPoints[3].y);

    points[4].x = CWorld::GetLodSectorfX(frustumPoints[4].x);
    points[4].y = CWorld::GetLodSectorfY(frustumPoints[4].y );

    CWorldScan::ScanWorld(points, (int)std::size(points), ScanBigBuildingList);
}

// returns objects count
// 0x554C60
int32 CRenderer::GetObjectsInFrustum(CEntity** outEntities, float farPlane, RwMatrix* transformMatrix)
{
    CVector frustumPoints[13]{};
    const float& width = TheCamera.m_pRwCamera->viewWindow.x;
    const float& height = TheCamera.m_pRwCamera->viewWindow.y;
    frustumPoints[0] = CVector(0.0f, 0.0f, 0.0f);
    frustumPoints[1].x = frustumPoints[4].x = -(farPlane * width);
    frustumPoints[1].y = frustumPoints[2].y = farPlane * height;
    frustumPoints[2].x = frustumPoints[3].x = farPlane * width;
    frustumPoints[3].y = frustumPoints[4].y = -(farPlane * height);
    frustumPoints[1].z = frustumPoints[2].z = frustumPoints[3].z = frustumPoints[4].z = farPlane;
    for (auto i = 5u; i < std::size(frustumPoints); i++) {
        frustumPoints[i] = CVector(0.0f, 0.0f, 0.0f);
    }

    CWorld::IncrementCurrentScanCode();

    RwMatrix* theTransformMatrix = transformMatrix;
    if (!theTransformMatrix)
        theTransformMatrix = TheCamera.GetRwMatrix();
    ms_vecCameraPosition = TheCamera.GetPosition();
    ms_fFarClipPlane = MAX_LOD_DISTANCE;
    if (theTransformMatrix->at.z > 0.0f) {
        frustumPoints[1] = frustumPoints[4];
        frustumPoints[2] = frustumPoints[3];
    }
    RwV3dTransformPoints(frustumPoints, frustumPoints, 13, theTransformMatrix);
    gpOutEntitiesForGetObjectsInFrustum = outEntities;
    CVector2D points[3];
    for (auto i = 0u; i < std::size(points); i++) {
        points[i].x = CWorld::GetSectorfX(frustumPoints[i].x);
        points[i].y = CWorld::GetSectorfY(frustumPoints[i].y);
    }
    if (transformMatrix)
        CWorldScan::ScanWorld(points, (int)std::size(points), ScanSectorList_ListModels);
    else
        CWorldScan::ScanWorld(points, (int)std::size(points), ScanSectorList_ListModelsVisible);
    return gpOutEntitiesForGetObjectsInFrustum - outEntities;
}

// 0x555960
void CRenderer::RequestObjectsInFrustum(RwMatrix* transformMatrix, int32 modelRequestFlags) {
    const float& farPlane = TheCamera.m_pRwCamera->farPlane;
    const float& width  = TheCamera.m_pRwCamera->viewWindow.x;
    const float& height = TheCamera.m_pRwCamera->viewWindow.y;

    CVector frustumPoints[13]{};
    frustumPoints[0] = CVector(0.0f, 0.0f, 0.0f);
    frustumPoints[1].x = frustumPoints[4].x = -(farPlane * width);
    frustumPoints[1].y = frustumPoints[2].y = farPlane * height;
    frustumPoints[2].x = frustumPoints[3].x = farPlane * width;
    frustumPoints[3].y = frustumPoints[4].y = -(farPlane * height);
    frustumPoints[1].z = frustumPoints[2].z = frustumPoints[3].z = frustumPoints[4].z = farPlane;
    for (auto i = 5u; i < std::size(frustumPoints); i++) {
        frustumPoints[i] = CVector(0.0f, 0.0f, 0.0f);
    }

    CWorld::IncrementCurrentScanCode();

    if (!transformMatrix) {
        transformMatrix = TheCamera.GetRwMatrix();
    }
    ms_vecCameraPosition = transformMatrix->pos;
    ms_fFarClipPlane = MAX_LOD_DISTANCE;
    gnRendererModelRequestFlags = modelRequestFlags;
    if (transformMatrix->at.z > 0.0f) {
        frustumPoints[5] = (frustumPoints[4] * MAX_LOD_DISTANCE) / farPlane;
        frustumPoints[6] = (frustumPoints[3] * MAX_LOD_DISTANCE) / farPlane;
    }
    else {
        frustumPoints[5] = (frustumPoints[1] * MAX_LOD_DISTANCE) / farPlane;
        frustumPoints[6] = (frustumPoints[2] * MAX_LOD_DISTANCE) / farPlane;
    }
    RwV3dTransformPoints(frustumPoints, frustumPoints, (int)std::size(frustumPoints), transformMatrix);
    CVector2D points[3] = {
        { CWorld::GetSectorfX(frustumPoints[0].x), CWorld::GetSectorfY(frustumPoints[0].y) },
        { CWorld::GetSectorfX(frustumPoints[5].x), CWorld::GetSectorfY(frustumPoints[5].y) },
        { CWorld::GetSectorfX(frustumPoints[6].x), CWorld::GetSectorfY(frustumPoints[6].y) },
    };
    CWorldScan::ScanWorld(points, std::size(points), ScanSectorList_RequestModels);
}

// modelRequestFlags is always set to `STREAMING_LOADING_SCENE` when this function is called
// 0x555CB0
void CRenderer::RequestObjectsInDirection(const CVector& posn, float angle, int32 modelRequestFlags)
{
    RwMatrix matrix {
        .right = { 1.0f, 0.0f, 0.0f },
        .flags = rwMATRIXINTERNALIDENTITY | rwMATRIXTYPEORTHONORMAL,
        .up =    { 0.0f, 1.0f, 0.0f },
        .at =    { 0.0f, 0.0f, 1.0f },
        .pos =   { 0.0f, 0.0f, 0.0f },
    };
    RwV3d axis = { 1.0f, 0.0f, 0.0f };
    RwMatrixRotate(&matrix, &axis, 90.0f, rwCOMBINEREPLACE);
    angle = ((angle * 180.0f) / PI) + 180.0f;
    axis = { 0.0f, 0.0f, 1.0f };
    RwMatrixRotate(&matrix, &axis, angle, rwCOMBINEPOSTCONCAT);
    RwMatrixTranslate(&matrix, &posn, rwCOMBINEPOSTCONCAT);
    RequestObjectsInFrustum(&matrix, modelRequestFlags);
}

// 0x553540
void CRenderer::SetupScanLists(int32 sectorX, int32 sectorY)
{
    CRepeatSector* repeatSector = GetRepeatSector(sectorX, sectorY);
    auto* scanLists = reinterpret_cast<tScanLists*>(&PC_Scratch);
    if (sectorX >= 0 && sectorY >= 0 && sectorX < MAX_SECTORS_X && sectorY < MAX_SECTORS_Y) {
        CSector* sector = GetSector(sectorX, sectorY);
        scanLists->buildingsList = &sector->m_buildings;
        scanLists->objectsList = &repeatSector->GetList(REPEATSECTOR_OBJECTS);
        scanLists->vehiclesList = &repeatSector->GetList(REPEATSECTOR_VEHICLES);
        scanLists->pedsList = &repeatSector->GetList(REPEATSECTOR_PEDS);
        scanLists->dummiesList = &sector->m_dummies;
    }
    else {
        // sector x and y are out of bounds
        scanLists->buildingsList = nullptr;
        scanLists->objectsList = &repeatSector->GetList(REPEATSECTOR_OBJECTS);
        scanLists->vehiclesList = &repeatSector->GetList(REPEATSECTOR_VEHICLES);
        scanLists->pedsList = &repeatSector->GetList(REPEATSECTOR_PEDS);
        scanLists->dummiesList = nullptr;
    }
}
