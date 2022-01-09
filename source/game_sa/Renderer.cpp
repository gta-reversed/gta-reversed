/*
    Plugin-SDK (Grand Theft Auto San Andreas) source file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#include "StdInc.h"

#include "Renderer.h"

#include "Occlusion.h"

#ifdef EXTRA_DEBUG_FEATURES
#include "toolsmenu\DebugModules\Collision\CollisionDebugModule.h"
#endif

int32 MAX_INVISIBLE_ENTITY_PTRS = 150;
int32 MAX_VISIBLE_ENTITY_PTRS = 1000;
int32 MAX_VISIBLE_LOD_PTRS = 1000;
int32 MAX_VISIBLE_SUPERLOD_PTRS = 50;
bool& CRenderer::ms_bRenderTunnels = *(bool*)0xB745C0;
bool& CRenderer::ms_bRenderOutsideTunnels = *(bool*)0xB745C1;
tRenderListEntry*& CRenderer::ms_pLodDontRenderList = *(tRenderListEntry**)0xB745CC;
tRenderListEntry*& CRenderer::ms_pLodRenderList = *(tRenderListEntry**)0xB745D0;
CVehicle*& CRenderer::m_pFirstPersonVehicle = *(CVehicle**)0xB745D4;
CEntity** CRenderer::ms_aInVisibleEntityPtrs = (CEntity**)0xB745D8;
CEntity** CRenderer::ms_aVisibleSuperLodPtrs = (CEntity**)0xB74830;
CEntity** CRenderer::ms_aVisibleLodPtrs = (CEntity**)0xB748F8;
CEntity** CRenderer::ms_aVisibleEntityPtrs = (CEntity**)0xB75898;
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
    ReversibleHooks::Install("CRenderer", "RenderFadingInEntities", 0x5531E0, &CRenderer::RenderFadingInEntities);
    ReversibleHooks::Install("CRenderer", "RenderFadingInUnderwaterEntities", 0x553220, &CRenderer::RenderFadingInUnderwaterEntities);
    ReversibleHooks::Install("CRenderer", "RenderOneRoad", 0x553230, &CRenderer::RenderOneRoad);
    ReversibleHooks::Install("CRenderer", "RenderOneNonRoad", 0x553260, &CRenderer::RenderOneNonRoad);
    ReversibleHooks::Install("CRenderer", "RemoveVehiclePedLights", 0x553390, &CRenderer::RemoveVehiclePedLights);
    ReversibleHooks::Install("CRenderer", "AddEntityToRenderList", 0x5534B0, &CRenderer::AddEntityToRenderList);
    ReversibleHooks::Install("CRenderer", "GetLodRenderListBase", 0x5536D0, &CRenderer::GetLodRenderListBase);
    ReversibleHooks::Install("CRenderer", "GetLodDontRenderListBase", 0x5536E0, &CRenderer::GetLodDontRenderListBase);
    ReversibleHooks::Install("CRenderer", "ResetLodRenderLists", 0x5536F0, &CRenderer::ResetLodRenderLists);
    ReversibleHooks::Install("CRenderer", "AddToLodRenderList", 0x553710, &CRenderer::AddToLodRenderList);
    ReversibleHooks::Install("CRenderer", "AddToLodDontRenderList", 0x553740, &CRenderer::AddToLodDontRenderList);
    ReversibleHooks::Install("CRenderer", "ProcessLodRenderLists", 0x553770, &CRenderer::ProcessLodRenderLists);
    ReversibleHooks::Install("CRenderer", "PreRender", 0x553910, &CRenderer::PreRender);
    ReversibleHooks::Install("CRenderer", "RenderRoads", 0x553A10, &CRenderer::RenderRoads);
    ReversibleHooks::Install("CRenderer", "RenderEverythingBarRoads", 0x553AA0, &CRenderer::RenderEverythingBarRoads);
    ReversibleHooks::Install("CRenderer", "RenderFirstPersonVehicle", 0x553D00, &CRenderer::RenderFirstPersonVehicle);
    ReversibleHooks::Install("CRenderer", "SetupLightingForEntity", 0x553E40, &CRenderer::SetupLightingForEntity);
    ReversibleHooks::Install("CRenderer", "SetupEntityVisibility", 0x554230, &CRenderer::SetupEntityVisibility);
    ReversibleHooks::Install("CRenderer", "SetupMapEntityVisibility", 0x553F60, &CRenderer::SetupMapEntityVisibility);
    ReversibleHooks::Install("CRenderer", "SetupBigBuildingVisibility", 0x554650, &CRenderer::SetupBigBuildingVisibility);
    ReversibleHooks::Install("CRenderer", "SetuscanLists", 0x553540, &CRenderer::SetupScanLists);
    ReversibleHooks::Install("CRenderer", "ScanSectorList_ListModels", 0x5535D0, &CRenderer::ScanSectorList_ListModels);
    ReversibleHooks::Install("CRenderer", "ScanSectorList_ListModelsVisible);", 0x553650, &CRenderer::ScanSectorList_ListModelsVisible);
    ReversibleHooks::Install("CRenderer", "ScanSectorList", 0x554840, &CRenderer::ScanSectorList);
    ReversibleHooks::Install("CRenderer", "ScanBigBuildingList", 0x554B10, &CRenderer::ScanBigBuildingList);
    ReversibleHooks::Install("CRenderer", "ShouldModelBeStreamed", 0x554EB0, &CRenderer::ShouldModelBeStreamed);
    ReversibleHooks::Install("CRenderer", "ScanPtrList_RequestModels", 0x555680, &CRenderer::ScanPtrList_RequestModels);
    ReversibleHooks::Install("CRenderer", "ConstructRenderList", 0x5556E0, &CRenderer::ConstructRenderList);
    ReversibleHooks::Install("CRenderer", "ScanSectorList_RequestModels", 0x555900, &CRenderer::ScanSectorList_RequestModels);
    ReversibleHooks::Install("CRenderer", "ScanWorld", 0x554FE0, &CRenderer::ScanWorld);
    ReversibleHooks::Install("CRenderer", "GetObjectsInFrustum", 0x554C60, &CRenderer::GetObjectsInFrustum);
    ReversibleHooks::Install("CRenderer", "RequestObjectsInFrustum", 0x555960, &CRenderer::RequestObjectsInFrustum);
    ReversibleHooks::Install("CRenderer", "RequestObjectsInDirection", 0x555CB0, &CRenderer::RequestObjectsInDirection);

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
    RwRenderStateSet(rwRENDERSTATEFOGENABLE, (void*)TRUE);
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)TRUE);
    RwRenderStateSet(rwRENDERSTATECULLMODE, (void*)rwCULLMODECULLBACK);
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
    CPed* ped = static_cast<CPed*>(entity);
    auto* vehicle = static_cast<CVehicle*>(entity);
    if (entity->m_nType == ENTITY_TYPE_PED && ped->m_nPedState == PEDSTATE_DRIVING)
        return;

    bool bSetupLighting = entity->SetupLighting();
    if (entity->m_nType == ENTITY_TYPE_VEHICLE) {
        CVisibilityPlugins::SetupVehicleVariables(entity->m_pRwClump);
        CVisibilityPlugins::InitAlphaAtomicList();
        vehicle->RenderDriverAndPassengers();
        vehicle->SetupRender();
    } else if (!entity->m_bBackfaceCulled) {
        RwRenderStateSet(rwRENDERSTATECULLMODE, (void*)rwCULLMODECULLNONE);
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

    if (entity->m_nType == ENTITY_TYPE_VEHICLE) {
        vehicle->m_bImBeingRendered = true;
        CVisibilityPlugins::RenderAlphaAtomics();
        vehicle->m_bImBeingRendered = false;
        vehicle->ResetAfterRender();
        vehicle->RemoveLighting(bSetupLighting);
    } else {
        if (!entity->m_bBackfaceCulled)
            RwRenderStateSet(rwRENDERSTATECULLMODE, (void*)rwCULLMODECULLBACK);
        entity->RemoveLighting(bSetupLighting);
    }
}

// 0x553390
void CRenderer::RemoveVehiclePedLights(CPhysical* entity) {
    if (!entity->physicalFlags.bDestroyed)
        CPointLights::RemoveLightsAffectingObject();
}

// 0x05534B0
void CRenderer::AddEntityToRenderList(CEntity* entity, float fDistance)
{
    CBaseModelInfo* baseModelInfo = CModelInfo::GetModelInfo(entity->m_nModelIndex);
    baseModelInfo->SetHasBeenPreRendered(false);
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
    }
    else {
        ms_aVisibleEntityPtrs[ms_nNoOfVisibleEntities] = entity;
        ms_nNoOfVisibleEntities++;
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
    ms_pLodRenderList->pEntity = entity;
    ms_pLodRenderList->distance = distance;
    ++ms_pLodRenderList;
}

// unused
// 0x553740
void CRenderer::AddToLodDontRenderList(CEntity* entity, float distance) {
    ms_pLodDontRenderList->pEntity = entity;
    ms_pLodDontRenderList->distance = distance;
    ++ms_pLodDontRenderList;
}

// 0x553770
void CRenderer::ProcessLodRenderLists() {
    for (auto renderListEntry = GetLodRenderListBase(); renderListEntry != ms_pLodRenderList; renderListEntry++) {
        CEntity* entity = renderListEntry->pEntity;
        if (entity && !entity->m_bIsVisible) {
            entity->m_nNumLodChildrenRendered = 0;
            renderListEntry->pEntity = nullptr;
        }
    }
    const uint8 displaySuperLowLodFlag = 0x80u; // yes, this is very hacky. Blame R*
    bool bAllLodsRendered = false;
    while (bAllLodsRendered) {
        for (auto renderListEntry = GetLodRenderListBase(); renderListEntry != ms_pLodRenderList; renderListEntry++) {
            CEntity* entity = renderListEntry->pEntity;
            if (entity) {
                if (entity->m_nNumLodChildren > 0 && entity->m_nNumLodChildrenRendered == entity->m_nNumLodChildren) {
                    entity->m_nNumLodChildrenRendered = 0;
                    renderListEntry->pEntity = nullptr;
                    bAllLodsRendered = true;
                }
                else if (entity->m_pLod) {
                    auto modelInfo = CModelInfo::GetModelInfo(entity->m_nModelIndex);
                    if (modelInfo->m_nAlpha < 255u
                        && entity->m_pLod->m_nNumLodChildrenRendered != displaySuperLowLodFlag
                        && entity->m_pLod->m_bDisplayedSuperLowLOD)
                    {
                        entity->m_pLod->m_nNumLodChildrenRendered = 0;
                    }
                    if (!entity->m_pRwObject) {
                        if (entity->m_pLod->m_bDisplayedSuperLowLOD)
                            entity->m_pLod->m_nNumLodChildrenRendered = displaySuperLowLodFlag;
                        renderListEntry->pEntity = nullptr;
                        entity->m_nNumLodChildrenRendered = 0;
                        CStreaming::RequestModel(entity->m_nModelIndex, 0);
                    }
                }
            }
        }
    }
    for (auto renderListEntry = GetLodRenderListBase(); renderListEntry != ms_pLodRenderList; renderListEntry++) {
        CEntity* entity = renderListEntry->pEntity;
        if (entity && entity->m_nNumLodChildrenRendered > 0) {
            entity->m_bDisplayedSuperLowLOD = false;
            entity->m_nNumLodChildrenRendered = 0;
            renderListEntry->pEntity = nullptr;
        }
    }
    for (auto renderListEntry = GetLodRenderListBase(); renderListEntry != ms_pLodRenderList; renderListEntry++) {
        CEntity* entity = renderListEntry->pEntity;
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
    for (int32 i = 0; i < ms_nNoOfVisibleLods; ++i) {
        ms_aVisibleLodPtrs[i]->PreRender();
    }
    for (int32 i = 0; i < ms_nNoOfVisibleEntities; ++i) {
        ms_aVisibleEntityPtrs[i]->PreRender();
    }
    for (int32 i = 0; i < ms_nNoOfVisibleSuperLods; ++i) {
        ms_aVisibleSuperLodPtrs[i]->PreRender();
    }
    for (int32 i = 0; i < ms_nNoOfInVisibleEntities; ++i) {
        ms_aInVisibleEntityPtrs[i]->PreRender();
    }
    for (auto link = CVisibilityPlugins::m_alphaEntityList.usedListHead.next;
        link != &CVisibilityPlugins::m_alphaEntityList.usedListTail;
        link = link->next)
    {
        if (link->data.m_pCallback == CVisibilityPlugins::RenderEntity) {
            link->data.m_entity->m_bOffscreen = false;
            link->data.m_entity->PreRender();
        }
    }
    for (auto link = CVisibilityPlugins::m_alphaUnderwaterEntityList.usedListHead.next;
        link != &CVisibilityPlugins::m_alphaUnderwaterEntityList.usedListTail;
        link = link->next)
    {
        if (link->data.m_pCallback == CVisibilityPlugins::RenderEntity) {
            link->data.m_entity->m_bOffscreen = false;
            link->data.m_entity->PreRender();
        }
    }
    CHeli::SpecialHeliPreRender();
    CShadows::RenderExtraPlayerShadows();
}

// 0x553A10
void CRenderer::RenderRoads() {
    RwRenderStateSet(rwRENDERSTATEFOGENABLE, (void*)TRUE);
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)TRUE);
    RwRenderStateSet(rwRENDERSTATECULLMODE, (void*)rwCULLMODECULLBACK);
    DeActivateDirectional();
    SetAmbientColours();
    for (int32 i = 0; i < ms_nNoOfVisibleEntities; ++i) {
        CEntity* entity = ms_aVisibleEntityPtrs[i];
        if (entity->m_nType == ENTITY_TYPE_BUILDING && CModelInfo::GetModelInfo(entity->m_nModelIndex)->IsRoad()) {
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
    RwRenderStateSet(rwRENDERSTATEFOGENABLE, (void*)TRUE);
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)TRUE);
    RwRenderStateSet(rwRENDERSTATECULLMODE, (void*)rwCULLMODECULLBACK);
    if (!CGame::currArea)
        RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, (void*)140u);

    for (int32 i = 0; i < ms_nNoOfVisibleEntities; i++) {
        CEntity* entity = ms_aVisibleEntityPtrs[i];
        auto* vehicle = static_cast<CVehicle*>(entity);
        CPed* ped = static_cast<CPed*>(entity);
        if (entity->m_nType == ENTITY_TYPE_BUILDING && CModelInfo::GetModelInfo(entity->m_nModelIndex)->IsRoad())
            continue;

        bool bInserted = false;
        if (entity->m_nType == ENTITY_TYPE_VEHICLE || (entity->m_nType == ENTITY_TYPE_PED && CVisibilityPlugins::GetClumpAlpha(entity->m_pRwClump) != 255)) {
            if (entity->m_nType == ENTITY_TYPE_VEHICLE) {
                bool bInsertIntoSortedList = false;
                if (vehicle->m_vehicleType == VEHICLE_BOAT) {
                    eCamMode camMode = TheCamera.m_aCams[TheCamera.m_nActiveCam].m_nMode;
                    if (camMode == MODE_WHEELCAM || camMode == MODE_1STPERSON &&
                        TheCamera.GetLookDirection() != LOOKING_DIRECTION_FORWARD && TheCamera.GetLookDirection() ||
                        CVisibilityPlugins::GetClumpAlpha(entity->m_pRwClump) != 255)
                    {
                        bInsertIntoSortedList = true;
                    }
                }
                else if (!vehicle->physicalFlags.bTouchingWater) {
                    bInsertIntoSortedList = true;
                }
                const float fMagnitude = (ms_vecCameraPosition - entity->GetPosition()).Magnitude();
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
    Scene.m_pRwCamera->zShift = Scene.m_pRwCamera->zShift - 100.0f;
    RwCameraBeginUpdate(Scene.m_pRwCamera);
    for (int32 i = 0; i < ms_nNoOfVisibleLods; ++i) {
        RenderOneNonRoad(ms_aVisibleLodPtrs[i]);
    }
    RwCameraEndUpdate(Scene.m_pRwCamera);
    Scene.m_pRwCamera->zShift = oldzShift;
    RwCameraBeginUpdate(Scene.m_pRwCamera);
}

// 0x553D00
void CRenderer::RenderFirstPersonVehicle() {
    if (m_pFirstPersonVehicle) {
        bool bRestoreAlphaTest = false;
        if (CWorld::Players[0].m_pPed->GetActiveWeapon().m_nType == WEAPON_MICRO_UZI) {
            bRestoreAlphaTest = true;
            RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, (void*)80u);
        }
        RwRenderStateSet(rwRENDERSTATEFOGENABLE, (void*)TRUE);
        RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)TRUE);
        RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)TRUE);
        RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)TRUE);
        RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA);
        RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDINVSRCALPHA);
        RenderOneNonRoad(m_pFirstPersonVehicle);
        RwRenderStateSet(rwRENDERSTATEFOGENABLE, (void*)FALSE);
        if (bRestoreAlphaTest)
            RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, (void*)nullptr);
    }

#ifdef EXTRA_DEBUG_FEATURES
    CollisionDebugModule::ProcessRender();
#endif
}

// 0x553E40
bool CRenderer::SetupLightingForEntity(CPhysical* entity) {
    CCarFXRenderer::SetFxEnvMapLightMult(1.0f);
    if (entity->physicalFlags.bDestroyed) {
        WorldReplaceNormalLightsWithScorched(Scene.m_pRpWorld, 0.18f);
        return false;
    }
    entity->m_fDynamicLighting = 0.0f;
    CVector point = entity->GetPosition();
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
    const int32 modelId = entity->m_nModelIndex;
    CBaseModelInfo* baseModelInfo = CModelInfo::ms_modelInfoPtrs[modelId];
    CBaseModelInfo* baseAtomicModelInfo = baseModelInfo->AsAtomicModelInfoPtr();
    if (entity->m_nType == ENTITY_TYPE_VEHICLE && !entity->m_bTunnelTransition) {
        if ((!ms_bRenderTunnels && entity->m_bTunnel || !ms_bRenderOutsideTunnels && !entity->m_bTunnel))
            return RENDERER_INVISIBLE;
    }

    bool bIsTimeInRange = true;
    if (!baseAtomicModelInfo)
    {
        if (baseModelInfo->GetModelType() != MODEL_INFO_CLUMP && baseModelInfo->GetModelType() != MODEL_INFO_WEAPON)
        {
            if (FindPlayerVehicle(-1, false) == entity && gbFirstPersonRunThisFrame && CReplay::Mode != REPLAY_MODE_1) {
                uint32 dwDirectionWasLooking = CCamera::GetActiveCamera().m_nDirectionWasLooking;
                CVehicle* vehicle = FindPlayerVehicle(-1, false);
                if (!vehicle->IsBike() || !(vehicle->AsBike()->damageFlags.bDamageFlag8))
                {
                    if (dwDirectionWasLooking == 3)
                        return RENDERER_CULLED;
                    if (modelId == MODEL_RHINO || modelId == MODEL_COACH || TheCamera.m_bInATunnelAndABigVehicle)
                        return RENDERER_CULLED;
                    if (dwDirectionWasLooking) {
                        m_pFirstPersonVehicle = static_cast<CVehicle*>(entity);
                        return RENDERER_CULLED;
                    }
                    if (vehicle->m_pHandlingData->m_bNo1fpsLookBehind)
                        return RENDERER_CULLED;
                    if (!vehicle->IsBoat()
                        || modelId == MODEL_REEFER || modelId == MODEL_TROPIC || modelId == MODEL_PREDATOR
                        || modelId == MODEL_SKIMMER)
                    {
                        m_pFirstPersonVehicle = static_cast<CVehicle*>(entity);
                        return RENDERER_CULLED;
                    }
                }
            }
            if (!entity->m_pRwObject
                || !entity->m_bIsVisible && (!CMirrors::TypeOfMirror || entity->m_nModelIndex)
                || !entity->IsCurrentAreaOrBarberShopInterior() && entity->m_nType == ENTITY_TYPE_VEHICLE)
            {
                return RENDERER_INVISIBLE;
            }
            if (!entity->GetIsOnScreen() || entity->IsEntityOccluded()) {
#ifdef EXTRA_DEBUG_FEATURES
                ++COcclusionDebugModule::NumEntitiesSkipped;
#endif
                return RENDERER_CULLED;
            }
            if (entity->m_bWasPostponed) {
                entity->m_bDistanceFade = false;
                AddEntityToRenderList(entity, (entity->GetPosition() - ms_vecCameraPosition).Magnitude());
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
                if (otherTimeModel != -1 && CModelInfo::GetModelInfo(otherTimeModel)->m_pRwObject)
                    baseModelInfo->m_nAlpha = 255;
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
#ifdef EXTRA_DEBUG_FEATURES
                    ++COcclusionDebugModule::NumEntitiesSkipped;
#endif
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
        CVector* position = &entity->GetPosition();
        if (entity->m_pLod)
            position = &entity->m_pLod->GetPosition();

        const float distance = (*position - ms_vecCameraPosition).Magnitude();
        outDistance = distance;
        if (distance > MAX_LOD_DISTANCE) {
            const float fDrawDistanceRadius = TheCamera.m_fLODDistMultiplier * baseModelInfo->m_fDrawDistance;
            if (fDrawDistanceRadius > MAX_LOD_DISTANCE && fDrawDistanceRadius + MAX_FADING_DISTANCE > distance)
                outDistance = fDrawDistanceRadius - MAX_LOD_DISTANCE + distance;
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
    CVector distance = entityPos - ms_vecCameraPosition;
    outDistance = distance.Magnitude();
    if (entity->m_nNumLodChildrenRendered <= 0) {
        int32 visibility = SetupMapEntityVisibility(entity, baseModelInfo, outDistance, bIsTimeInRange);
        if (visibility != RENDERER_VISIBLE || entity->m_nNumLodChildren <= 1u)
            return visibility;
        if (entity->m_pLod && baseModelInfo->m_nAlpha == 255)
            ++entity->m_pLod->m_nNumLodChildrenRendered;
        AddToLodRenderList(entity, outDistance);
        return RENDERER_INVISIBLE;
    }

    if (entity->m_pLod)
        ++entity->m_pLod->m_nNumLodChildrenRendered;

    if (entity->m_nNumLodChildren <= 1u) {
        entity->m_nNumLodChildrenRendered = 0;
    } else {
        ms_pLodRenderList->pEntity = entity;
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
            if (entity->m_nScanCode != CWorld::ms_nCurrentScanCode) {
                entity->m_nScanCode = CWorld::ms_nCurrentScanCode;
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
    CEntity** pEntity = gpOutEntitiesForGetObjectsInFrustum;
    auto** scanLists = reinterpret_cast<CPtrListDoubleLink**>(&PC_Scratch);
    for (int32 scanListIndex = 0; scanListIndex < TOTAL_ENTITY_SCAN_LISTS; scanListIndex++) {
        CPtrListDoubleLink* doubleLinkList = scanLists[scanListIndex];
        if (!doubleLinkList)
            continue;

        for (auto node = doubleLinkList->GetNode(); node; node = node->m_next) {
            auto* entity = reinterpret_cast<CEntity*>(node->m_item);
            if (entity->m_nScanCode != CWorld::ms_nCurrentScanCode) {
                entity->m_nScanCode = CWorld::ms_nCurrentScanCode;
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
    float fAngleInRadians = atan2(-fDistanceX, fDistanceY) - ms_fCameraHeading;
    if (CVector2D(fDistanceX, fDistanceY).SquaredMagnitude() < MAX_STREAMING_RADIUS ||
        fabs(CGeneral::LimitRadianAngle(fAngleInRadians)) < DegreesToRadians(STREAMING_ANGLE_THRESHOLD)) {
        bRequestModel = true;
    }

    SetupScanLists(sectorX, sectorY);
    auto** scanLists = reinterpret_cast<CPtrListDoubleLink**>(&PC_Scratch);
    for (int32 scanListIndex = 0; scanListIndex < TOTAL_ENTITY_SCAN_LISTS; scanListIndex++) {
        CPtrListDoubleLink* doubleLinkList = scanLists[scanListIndex];
        if (!doubleLinkList)
            continue;

        CPtrNodeDoubleLink* doubleLinkNode = doubleLinkList->GetNode();
        while (doubleLinkNode) {
            auto* entity = reinterpret_cast<CEntity*>(doubleLinkNode->m_item);
            doubleLinkNode = doubleLinkNode->m_next;
            if (entity->m_nScanCode == CWorld::ms_nCurrentScanCode)
                continue;

            entity->m_nScanCode = CWorld::ms_nCurrentScanCode;
            entity->m_bOffscreen = false;
            bool bInvisibleEntity = false;
            float fDistance = 0.0f;
            switch (SetupEntityVisibility(entity, fDistance)) {
            case RENDERER_INVISIBLE: {
                if (entity->m_nType == ENTITY_TYPE_OBJECT) {
                    CBaseModelInfo* baseModelInfo = CModelInfo::GetModelInfo(entity->m_nModelIndex)->AsAtomicModelInfoPtr();
                    if (baseModelInfo) {
                        if (baseModelInfo->IsGlass()) {
                            bInvisibleEntity = true;
                            break;
                        }
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
                    auto streamingInfo = &CStreaming::ms_aInfoForModel[entity->m_nModelIndex];
                    if (streamingInfo->m_nLoadState == LOADSTATE_LOADED) {
                        CStreaming::RequestModel(entity->m_nModelIndex, 0);
                        break;
                    } else {
                        if (!entity->IsEntityOccluded()) {
                            SetLoadingPriority(1);
                            CStreaming::RequestModel(entity->m_nModelIndex, 0);
                            break;
                        }
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
                if (entity->m_nType == ENTITY_TYPE_VEHICLE) {
                    auto* vehicle = static_cast<CVehicle*>(entity);
                    if (vehicle->vehicleFlags.bAlwaysSkidMarks)
                        fDrawDistance = MAX_INVISIBLE_VEHICLE_DISTANCE;
                }
                if (distance.x > -fDrawDistance && distance.x < fDrawDistance && distance.y > -fDrawDistance && distance.y < fDrawDistance) {
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
    if (sectorX >= 0 && sectorY >= 0 && sectorX < MAX_LOD_PTR_LISTS_X && sectorY < MAX_LOD_PTR_LISTS_Y) {
        CPtrList& list = CWorld::GetLodPtrList(sectorX, sectorY);
        bool bRequestModel = false;
        float fDistanceX = CWorld::GetLodSectorPosX(sectorX) - ms_vecCameraPosition.x;
        float fDistanceY = CWorld::GetLodSectorPosY(sectorY) - ms_vecCameraPosition.y;
        float fAngleInRadians = atan2(-fDistanceX, fDistanceY) - ms_fCameraHeading;
        if (CVector2D(fDistanceX, fDistanceY).SquaredMagnitude() < MAX_BIGBUILDING_STREAMING_RADIUS ||
            fabs(CGeneral::LimitRadianAngle(fAngleInRadians)) <= DegreesToRadians(BIGBUILDING_STREAMING_ANGLE_THRESHOLD))
        {
            bRequestModel = true;
        }
        for (CPtrNode* node = list.GetNode(); node; node = node->m_next) {
            auto* entity = reinterpret_cast<CEntity*>(node->m_item);
            if (entity->m_nScanCode != CWorld::ms_nCurrentScanCode) {
                entity->m_nScanCode = CWorld::ms_nCurrentScanCode;
                float fDistance = 0.0f;
                int32 visibility = SetupBigBuildingVisibility(entity, fDistance);
                if (visibility == RENDERER_VISIBLE) {
                    AddEntityToRenderList(entity, fDistance + 0.01f);
                    entity->m_bOffscreen = false;
                }
                else if (visibility == RENDERER_STREAMME && !CStreaming::ms_disableStreaming && bRequestModel) {
                    CStreaming::RequestModel(entity->m_nModelIndex, 0);
                }
            }
        }
    }
}

// 0x554EB0
bool CRenderer::ShouldModelBeStreamed(CEntity* entity, CVector const& point, float farClip) {
    if (entity->m_nAreaCode != CGame::currArea && entity->m_nAreaCode != AREA_CODE_13)
        return false;

    CBaseModelInfo* modelInfo = CModelInfo::GetModelInfo(entity->m_nModelIndex);
    CTimeInfo* timeInfo = modelInfo->GetTimeInfo();
    if (timeInfo && !CClock::GetIsTimeInRange(timeInfo->GetTimeOn(), timeInfo->GetTimeOff()))
        return false;

    const float fMagnitude = (entity->GetPosition() - point).Magnitude();
    const float fDrawDistanceRadius = TheCamera.m_fLODDistMultiplier * modelInfo->m_fDrawDistance;
    if (fMagnitude <= modelInfo->GetColModel()->GetBoundRadius() + farClip && fDrawDistanceRadius > fMagnitude)
        return true;

    return false;
}

// 0x555680
void CRenderer::ScanPtrList_RequestModels(CPtrList& list) {
    for (auto node = list.GetNode(); node; node = node->m_next) {
        auto* entity = reinterpret_cast<CEntity*>(node->m_item);
        if (entity->m_nScanCode != CWorld::ms_nCurrentScanCode) {
            entity->m_nScanCode = CWorld::ms_nCurrentScanCode;
            if (ShouldModelBeStreamed(entity, ms_vecCameraPosition, ms_fFarClipPlane))
                CStreaming::RequestModel(entity->m_nModelIndex, gnRendererModelRequestFlags);
        }
    }
}

// 0x5556E0
void CRenderer::ConstructRenderList() {
    eZoneAttributes zoneAttributes = CCullZones::FindTunnelAttributesForCoors(TheCamera.GetPosition());
    ms_bRenderTunnels = (zoneAttributes & (eZoneAttributes::UNKNOWN_2 | eZoneAttributes::UNKNOWN_1)) != 0;
    if ((zoneAttributes & eZoneAttributes::UNKNOWN_1) || !(zoneAttributes & eZoneAttributes::UNKNOWN_2))
        ms_bRenderOutsideTunnels = true;
    else
        ms_bRenderOutsideTunnels = false;
    ms_lowLodDistScale = 1.0f;
    ms_bInTheSky = false;
    CPlayerPed* player = FindPlayerPed(-1);
    if (player && player->m_nAreaCode == AREA_CODE_NORMAL_WORLD) {
        float fGroundHeightZ = TheCamera.CalculateGroundHeight(eGroundHeightType::ENTITY_BOUNDINGBOX_BOTTOM);
        if (player->GetPosition().z - fGroundHeightZ > 50.0f) {
            float fGroundHeightZ = TheCamera.CalculateGroundHeight(eGroundHeightType::ENTITY_BOUNDINGBOX_TOP);
            if (player->GetPosition().z - fGroundHeightZ > 10.0f && FindPlayerVehicle(-1, false))
                ms_bInTheSky = true;
        }
        const float fCameraZ = TheCamera.GetPosition().z;
        if (fCameraZ > LOWLOD_CAMERA_HEIGHT_THRESHOLD) {
            float fScale = (fCameraZ - LOWLOD_CAMERA_HEIGHT_THRESHOLD) / (250.0f - LOWLOD_CAMERA_HEIGHT_THRESHOLD);
            if (fScale > 1.0f)
                fScale = 1.0f;
            ms_lowLodDistScale = fScale * (2.2f - 1.0f) + 1.0f;
        }
    }
    ms_lowLodDistScale *= CTimeCycle::m_CurrentColours.m_fLodDistMult;
    CMirrors::BeforeConstructRenderList();
    COcclusion::ProcessBeforeRendering();
    ms_nNoOfVisibleEntities = 0;
    ms_nNoOfVisibleLods = 0;
    ms_nNoOfInVisibleEntities = 0;
    ms_vecCameraPosition = TheCamera.GetPosition();
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
        ScanPtrList_RequestModels(CWorld::ms_aRepeatSectors->m_lists[REPEATSECTOR_OBJECTS]);
    }
}

// 0x554FE0
void CRenderer::ScanWorld() {
    CVector frustumPoints[13];
    const float farPlane = TheCamera.m_pRwCamera->farPlane;
    const float width = TheCamera.m_pRwCamera->viewWindow.x;
    const float height = TheCamera.m_pRwCamera->viewWindow.y;
    frustumPoints[0] = CVector(0.0f, 0.0f, 0.0f);
    frustumPoints[1].x = frustumPoints[4].x = -(farPlane * width);
    frustumPoints[1].y = frustumPoints[2].y = farPlane * height;
    frustumPoints[2].x = frustumPoints[3].x = farPlane * width;
    frustumPoints[3].y = frustumPoints[4].y = -(farPlane * height);
    frustumPoints[1].z = frustumPoints[2].z = frustumPoints[3].z = frustumPoints[4].z = farPlane;
    for (int32 i = 5; i < 13; i++) {
        frustumPoints[i] = CVector(0.0f, 0.0f, 0.0f);
    }

    m_pFirstPersonVehicle = nullptr;
    CVisibilityPlugins::InitAlphaEntityList();

    CWorld::IncrementCurrentScanCode();
    static CVector lastCameraPosition;
    static CVector lastCameraForward;
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

    CVector2D points[5];
    points[0].x = CWorld::GetSectorfX(frustumPoints[0].x);
    points[0].y = CWorld::GetSectorfY(frustumPoints[0].y);
    points[1].x = CWorld::GetSectorfX(frustumPoints[5].x);
    points[1].y = CWorld::GetSectorfY(frustumPoints[5].y);
    points[2].x = CWorld::GetSectorfX(frustumPoints[6].x);
    points[2].y = CWorld::GetSectorfY(frustumPoints[6].y);
    points[3].x = CWorld::GetSectorfX(frustumPoints[9].x);
    points[3].y = CWorld::GetSectorfY(frustumPoints[9].y);
    points[4].x = CWorld::GetSectorfX(frustumPoints[10].x);
    points[4].y = CWorld::GetSectorfY(frustumPoints[10].y);
    CWorldScan::ScanWorld(points, 5, CRenderer::ScanSectorList);

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
    CWorldScan::ScanWorld(points, 5, CRenderer::ScanBigBuildingList);
}

// 0x554C60
int32 CRenderer::GetObjectsInFrustum(CEntity** outEntities, float farPlane, RwMatrix* transformMatrix)
{
    CVector frustumPoints[13];
    const float width = TheCamera.m_pRwCamera->viewWindow.x;
    const float height = TheCamera.m_pRwCamera->viewWindow.y;
    frustumPoints[0] = CVector(0.0f, 0.0f, 0.0f);
    frustumPoints[1].x = frustumPoints[4].x = -(farPlane * width);
    frustumPoints[1].y = frustumPoints[2].y = farPlane * height;
    frustumPoints[2].x = frustumPoints[3].x = farPlane * width;
    frustumPoints[3].y = frustumPoints[4].y = -(farPlane * height);
    frustumPoints[1].z = frustumPoints[2].z = frustumPoints[3].z = frustumPoints[4].z = farPlane;
    for (int32 i = 5; i < 13; i++) {
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
    for (int32 i = 0; i < 3; i++) {
        points[i].x = CWorld::GetSectorfX(frustumPoints[i].x);
        points[i].y = CWorld::GetSectorfY(frustumPoints[i].y);
    }
    if (transformMatrix)
        CWorldScan::ScanWorld(points, 3, ScanSectorList_ListModels);
    else
        CWorldScan::ScanWorld(points, 3, ScanSectorList_ListModelsVisible);
    return gpOutEntitiesForGetObjectsInFrustum - outEntities;
}

// 0x555960
void CRenderer::RequestObjectsInFrustum(RwMatrix* transformMatrix, int32 modelRequestFlags) {
    const float farPlane = TheCamera.m_pRwCamera->farPlane;
    const float width = TheCamera.m_pRwCamera->viewWindow.x;
    const float height = TheCamera.m_pRwCamera->viewWindow.y;
    CVector frustumPoints[13];
    frustumPoints[0] = CVector(0.0f, 0.0f, 0.0f);
    frustumPoints[1].x = frustumPoints[4].x = -(farPlane * width);
    frustumPoints[1].y = frustumPoints[2].y = farPlane * height;
    frustumPoints[2].x = frustumPoints[3].x = farPlane * width;
    frustumPoints[3].y = frustumPoints[4].y = -(farPlane * height);
    frustumPoints[1].z = frustumPoints[2].z = frustumPoints[3].z = frustumPoints[4].z = farPlane;
    for (int32 i = 5; i < 13; i++ ) {
        frustumPoints[i] = CVector(0.0f, 0.0f, 0.0f);
    }
    CWorld::IncrementCurrentScanCode();
    if (!transformMatrix)
        transformMatrix = TheCamera.GetRwMatrix();
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
    RwV3dTransformPoints(frustumPoints, frustumPoints, 13, transformMatrix);
    CVector2D points[3];
    points[0].x = CWorld::GetSectorfX(frustumPoints[0].x);
    points[0].y = CWorld::GetSectorfY(frustumPoints[0].y);
    points[1].x = CWorld::GetSectorfX(frustumPoints[5].x);
    points[1].y = CWorld::GetSectorfY(frustumPoints[5].y);
    points[2].x = CWorld::GetSectorfX(frustumPoints[6].x);
    points[2].y = CWorld::GetSectorfY(frustumPoints[6].y);
    CWorldScan::ScanWorld(points, 3, ScanSectorList_RequestModels);
}

// modelRequestFlags is always set to `STREAMING_LOADING_SCENE` when this function is called
// 0x555CB0
void CRenderer::RequestObjectsInDirection(CVector const& posn, float angle, int32 modelRequestFlags)
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
        scanLists->objectsList = &repeatSector->m_lists[REPEATSECTOR_OBJECTS];
        scanLists->vehiclesList = &repeatSector->m_lists[REPEATSECTOR_VEHICLES];
        scanLists->pedsList = &repeatSector->m_lists[REPEATSECTOR_PEDS];
        scanLists->dummiesList = &sector->m_dummies;
    }
    else {
        // sector x and y are out of bounds
        scanLists->buildingsList = nullptr;
        scanLists->objectsList = &repeatSector->m_lists[0];
        scanLists->vehiclesList = &repeatSector->m_lists[1];
        scanLists->pedsList = &repeatSector->m_lists[2];
        scanLists->dummiesList = nullptr;
    }
}
