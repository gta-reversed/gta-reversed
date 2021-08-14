/*
    Plugin-SDK (Grand Theft Auto San Andreas) source file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#include "StdInc.h"

#include "World.h"

int32 CWorld::TOTAL_PLAYERS = 2;
int32& CWorld::ms_iProcessLineNumCrossings = *(int32*)0xB7CD60;
float& CWorld::fWeaponSpreadRate = *(float*)0xB7CD64;
CEntity*& CWorld::pIgnoreEntity = *(CEntity**)0xB7CD68;
bool& CWorld::bSecondShift = *(bool*)0xB7CD6C;
bool& CWorld::bProcessCutsceneOnly = *(bool*)0xB7CD6D;
bool& CWorld::bForceProcessControl = *(bool*)0xB7CD6E;
bool& CWorld::bIncludeBikers = *(bool*)0xB7CD6F;
bool& CWorld::bIncludeCarTyres = *(bool*)0xB7CD70;
bool& CWorld::bIncludeDeadPeds = *(bool*)0xB7CD71;
bool& CWorld::bNoMoreCollisionTorque = *(bool*)0xB7CD72;
bool& CWorld::bDoingCarCollisions = *(bool*)0xB7CD73;
int8& CWorld::PlayerInFocus = *(int8*)0xB7CD74;
uint16& CWorld::ms_nCurrentScanCode = *(uint16*)0xB7CD78;
CPlayerInfo* CWorld::Players = (CPlayerInfo*)0xB7CD98;
CSector* CWorld::ms_aSectors = (CSector*)0xB7D0B8;
CRepeatSector (&CWorld::ms_aRepeatSectors)[MAX_REPEAT_SECTORS] = *(CRepeatSector(*)[MAX_REPEAT_SECTORS])0xB992B8;
CPtrListSingleLink (&CWorld::ms_aLodPtrLists)[MAX_LOD_PTR_LISTS_Y][MAX_LOD_PTR_LISTS_X] = *(CPtrListSingleLink(*)[MAX_LOD_PTR_LISTS_Y][MAX_LOD_PTR_LISTS_X])0xB99EB8;
CPtrListDoubleLink& CWorld::ms_listMovingEntityPtrs = *(CPtrListDoubleLink*)0xB9ACC8;
CPtrListDoubleLink& CWorld::ms_listObjectsWithControlCode = *(CPtrListDoubleLink*)0xB9ACCC;
CColPoint* CWorld::m_aTempColPts = (CColPoint*)0xB9ACD0;
CVector& CWorld::SnookerTableMax = *(CVector*)0x8CDEF4;
CVector& CWorld::SnookerTableMin = *(CVector*)0x8CDF00;
uint32& FilledColPointIndex = *(uint32*)0xB7CD7C;
CColPoint* gaTempSphereColPoints = (CColPoint*)0xB9B250;
int16& TAG_SPRAYING_INCREMENT_VAL = *(int16*)0x8CDEF0;
int8& gCurCamColVars = *(int8*)0x8CCB80;

void CWorld::InjectHooks() {
    HookInstall(0x565CB0, RemoveFallenPeds);
    HookInstall(0x565E80, RemoveFallenCars);
    using namespace ReversibleHooks;
    Install("CWorld", "Remove", 0x563280, &CWorld::Remove);
    Install("CWorld", "ClearForRestart", 0x564360, &CWorld::ClearForRestart);
    Install("CWorld", "CastShadowSectorList", 0x563390, &CWorld::CastShadowSectorList);
    Install("CWorld", "ResetLineTestOptions", 0x5631C0, &CWorld::ResetLineTestOptions);
    Install("CWorld", "Initialise", 0x5631E0, &CWorld::Initialise);
    Install("CWorld", "ProcessForAnimViewer", 0x5633D0, &CWorld::ProcessForAnimViewer);
    Install("CWorld", "ProcessPedsAfterPreRender", 0x563430, &CWorld::ProcessPedsAfterPreRender);
    Install("CWorld", "FindObjectsInRangeSectorList", 0x563500, &CWorld::FindObjectsInRangeSectorList);
    Install("CWorld", "FindObjectsOfTypeInRangeSectorList", 0x5635C0, &CWorld::FindObjectsOfTypeInRangeSectorList);
    Install("CWorld", "TestForBuildingsOnTopOfEachOther", 0x563950, static_cast<void(*)(CPtrList&)>(&CWorld::TestForBuildingsOnTopOfEachOther));
    Install("CWorld", "TestForUnusedModels", 0x5639D0, static_cast<void(*)(CPtrList&, int32*)>(&CWorld::TestForUnusedModels));
    Install("CWorld", "CameraToIgnoreThisObject", 0x563F40, &CWorld::CameraToIgnoreThisObject);
    Install("CWorld", "FindPlayerSlotWithPedPointer", 0x563FA0, &CWorld::FindPlayerSlotWithPedPointer);
    Install("CWorld", "FindPlayerSlotWithVehiclePointer", 0x563FD0, &CWorld::FindPlayerSlotWithVehiclePointer);
    Install("CWorld", "ProcessVerticalLineSector_FillGlobeColPoints", 0x564420, &CWorld::ProcessVerticalLineSector_FillGlobeColPoints);
    Install("CWorld", "ProcessVerticalLineSector", 0x564500, &CWorld::ProcessVerticalLineSector);
    Install("CWorld", "CastShadow", 0x564600, &CWorld::CastShadow);
    Install("CWorld", "GetIsLineOfSightSectorListClear", 0x564600, &CWorld::GetIsLineOfSightSectorListClear);
    Install("CWorld", "FindObjectsKindaCollidingSectorList", 0x565000, &CWorld::FindObjectsKindaCollidingSectorList);
    Install("CWorld", "FindObjectsIntersectingCubeSectorList", 0x5650E0, &CWorld::FindObjectsIntersectingCubeSectorList);
    Install("CWorld", "FindObjectsIntersectingAngledCollisionBoxSectorList", 0x565200, &CWorld::FindObjectsIntersectingAngledCollisionBoxSectorList);
    Install("CWorld", "ProcessLineOfSightSectorList", 0x566EE0, &CWorld::ProcessLineOfSightSectorList);
    // Install("CWorld", "FindUnsuspectingTargetPed", 0x566DA0, &CWorld::FindUnsuspectingTargetPed);
    Install("CWorld", "ProcessVerticalLine", 0x5674E0, &CWorld::ProcessVerticalLine);
    Install("CWorld", "ProcessVerticalLine_FillGlobeColPoints", 0x567620, &CWorld::ProcessVerticalLine_FillGlobeColPoints);
    Install("CWorld", "GetIsLineOfSightSectorClear", 0x568AD0, &CWorld::GetIsLineOfSightSectorClear);
    Install("CWorld", "FindRoofZFor3DCoord", 0x569750, &CWorld::FindRoofZFor3DCoord);
    Install("CWorld", "FindLowestZForCoord", 0x5697F0, &CWorld::FindLowestZForCoord);
    Install("CWorld", "FindGroundZFor3DCoord", 0x5696C0, &CWorld::FindGroundZFor3DCoord);
}

// 0x5631C0
void CWorld::ResetLineTestOptions() {
    bIncludeDeadPeds  = false;
    bIncludeCarTyres  = false;
    bIncludeBikers    = false;
    pIgnoreEntity     = nullptr;
    fWeaponSpreadRate = 0.0f;
}

// 0x5631E0
void CWorld::Initialise() {
    bDoingCarCollisions    = false;
    bNoMoreCollisionTorque = false;
    bIncludeDeadPeds       = false;
    bIncludeCarTyres       = false;
    bIncludeBikers         = false;
    bForceProcessControl   = false;
    bProcessCutsceneOnly   = false;
    bSecondShift           = false;
    pIgnoreEntity          = nullptr;
    fWeaponSpreadRate      = 0.0f;
    CIplStore::Initialise();
}

// 0x563220
void CWorld::Add(CEntity* entity) {
    plugin::Call<0x563220, CEntity*>(entity);
}

// 0x563280
void CWorld::Remove(CEntity* entity) {
    entity->Remove();
    if (entity->IsPhysical())
        static_cast<CPhysical*>(entity)->RemoveFromMovingList();
}

// 0x5632B0
bool CWorld::ProcessVerticalLineSectorList(CPtrList& ptrList, const CColLine& colLine, CColPoint& colPoint, float& maxTouchDistance, CEntity*& outEntity, bool doSeeThroughCheck, CStoredCollPoly* collPoly) {
    float maxTouchDistanceLocal{};

    for (CPtrNode* it = ptrList.m_node; it; it = it->m_next) {
        auto entity = static_cast<CEntity*>(it->m_item);

        if (entity->m_nScanCode == ms_nCurrentScanCode || !entity->m_bUsesCollision || entity == pIgnoreEntity)
            continue;

        entity->m_nScanCode = ms_nCurrentScanCode;

        if (CCollision::ProcessVerticalLine(
            colLine,
            entity->GetMatrix(),
            *entity->GetColModel(),
            colPoint,
            maxTouchDistanceLocal,
            doSeeThroughCheck,
            false,
            collPoly
            )
        ) {
            outEntity = entity;
            ms_iProcessLineNumCrossings += CCollision::ms_iProcessLineNumCrossings;
        }
    }

    if (maxTouchDistanceLocal < maxTouchDistance) {
        maxTouchDistance = maxTouchDistanceLocal;
        return true;
    }

    return false;
}

// 0x563390
void CWorld::CastShadowSectorList(CPtrList& ptrList, float, float, float, float) {
    for (CPtrNode* it = ptrList.m_node; it; it = it->m_next) {
        auto entity = static_cast<CEntity*>(it->m_item);
        if (entity->m_nScanCode != ms_nCurrentScanCode && entity->m_bUsesCollision) {
            entity->m_nScanCode = ms_nCurrentScanCode;
        }
    }
}

// 0x5633D0
void CWorld::ProcessForAnimViewer() {
    for (CPtrNode* it = ms_listMovingEntityPtrs.m_node; it; it = it->m_next) {
        auto entity = static_cast<CEntity*>(it->m_item);
        if (!entity->m_bRemoveFromWorld) {
            entity->UpdateAnim();
            entity->UpdateRW();
        }
    }
}

// 0x563430
void CWorld::ProcessPedsAfterPreRender() {
    if (!CTimer::bSkipProcessThisFrame)
        return;

    for (CPtrNode* it = ms_listMovingEntityPtrs.m_node; it; it = it->m_next) {
        auto entity = static_cast<CEntity*>(it->m_item);
        if (!entity->m_bRemoveFromWorld) {
            if (entity->IsPed()) {
                static_cast<CPed*>(entity)->GetIntelligence()->ProcessAfterPreRender();
            }
        }
    }
}

// 0x563470
void CWorld::ClearScanCodes() {
    plugin::Call<0x563470>();
}

// 0x563500
void CWorld::FindObjectsInRangeSectorList(CPtrList& arg0, const CVector& point, float radius, bool b2D, int16* outCount, int16 maxCount, CEntity** outEntities) {
    for (CPtrNode* it = ms_listMovingEntityPtrs.m_node; it; it = it->m_next) {
        auto entity = static_cast<CEntity*>(it->m_item);
        if (entity->m_nScanCode == ms_nCurrentScanCode)
            continue;

        entity->m_nScanCode = ms_nCurrentScanCode;

        if (b2D) {
            if (DistanceBetweenPointsSquared2D(CVector2D{ point }, entity->GetPosition()) > radius)
                continue;
        } else {
            if (DistanceBetweenPointsSquared(point, entity->GetPosition()) > radius)
                continue;
        }

        /* Don't stop if reached max count, because of entity scan code update */
        if (*outCount < maxCount) {
            if (outEntities)
                outEntities[*outCount] = entity;
            outCount++;
        }
    }
}

// 0x5635C0
void CWorld::FindObjectsOfTypeInRangeSectorList(uint32 modelId, CPtrList& ptrList, const CVector& point, float fRadiusSq, bool b2D, int16* outCount, int16 maxCount, CEntity** outEntities) {
    for (CPtrNode* it = ms_listMovingEntityPtrs.m_node; it; it = it->m_next) {
        auto entity = static_cast<CEntity*>(it->m_item);
        if (entity->m_nScanCode == ms_nCurrentScanCode)
            continue;

        entity->m_nScanCode = ms_nCurrentScanCode;

        if (entity->m_nModelIndex != modelId)
            continue;

        if (b2D) {
            if (DistanceBetweenPointsSquared2D(CVector2D{ point }, entity->GetPosition()) > fRadiusSq)
                continue;
        }
        else {
            if (DistanceBetweenPoints(point, entity->GetPosition()) > fRadiusSq)
                continue;
        }

        /* Don't stop if reached max count, because of entity scan code update */
        if (*outCount < maxCount) {
            if (outEntities)
                outEntities[*outCount] = entity;
            outCount++;
        }
    }
}

// 0x5636A0
bool CWorld::ProcessVerticalLineSectorList_FillGlobeColPoints(CPtrList& ptrList, const CColLine& colLine, CEntity*& outEntity, bool doSeeThroughCheck, CStoredCollPoly* outCollPoly) {
    return plugin::CallAndReturn<bool, 0x5636A0, CPtrList&, const CColLine&, CEntity*&, bool, CStoredCollPoly*>(ptrList, colLine, outEntity, doSeeThroughCheck, outCollPoly);
}

// 0x563840
void CWorld::RemoveStaticObjects() {
    plugin::Call<0x563840>();
}

// 0x563950
void CWorld::TestForBuildingsOnTopOfEachOther(CPtrList& ptrList) {
    /* nothing meaningful */
}

// 0x5639D0
void CWorld::TestForUnusedModels(CPtrList& ptrList, int32* models) {
    for (CPtrNode* it = ms_listMovingEntityPtrs.m_node; it; it = it->m_next) {
        auto entity = static_cast<CEntity*>(it->m_item);
        if (entity->m_nScanCode == ms_nCurrentScanCode)
            continue;
        entity->m_nScanCode = ms_nCurrentScanCode;
        models[entity->m_nModelIndex]++;
    }
}

// 0x563A10
void CWorld::RemoveEntityInsteadOfProcessingIt(CEntity* entity) {
    plugin::Call<0x563A10, CEntity*>(entity);
}

// 0x563A80
void CWorld::CallOffChaseForAreaSectorListVehicles(CPtrList& ptrList, float x1, float y1, float x2, float y2, float arg5, float arg6, float arg7, float arg8) {
    plugin::Call<0x563A80, CPtrList&, float, float, float, float, float, float, float, float>(ptrList, x1, y1, x2, y2, arg5, arg6, arg7, arg8);
}

// 0x563D00
void CWorld::CallOffChaseForAreaSectorListPeds(CPtrList& ptrList, float x1, float y1, float x2, float y2, float arg5, float arg6, float arg7, float arg8) {
    plugin::Call<0x563D00, CPtrList&, float, float, float, float, float, float, float, float>(ptrList, x1, y1, x2, y2, arg5, arg6, arg7, arg8);
}

// 0x563F40
bool CWorld::CameraToIgnoreThisObject(CEntity* entity) {
    if (CGarages::IsModelIndexADoor(entity->m_nModelIndex))
        return false;

    switch (entity->AsObject()->m_pObjectInfo->m_nCameraAvoidObject) {
    case 1:
        return gCurCamColVars >= 10u;
    case 2:
        return false;
    case 3:
        return gCurCamColVars < 10u;
    default:
        return true;
    }
}

// 0x563FA0
int32 CWorld::FindPlayerSlotWithPedPointer(void* ped) {
    for (int32 i = 0; i < MAX_PLAYERS; i++) {
        if (Players[i].m_pPed == ped)
            return i;
    }
    return -1;
}

// 0x563FD0
int32 CWorld::FindPlayerSlotWithRemoteVehiclePointer(void* vehicle) {
    for (int32 i = 0; i < MAX_PLAYERS; i++) {
        if (Players[i].m_pRemoteVehicle == vehicle)
            return i;
    }
    return -1;
}

// 0x564000
void CWorld::FindPlayerSlotWithVehiclePointer(CEntity* vehiclePtr) {
    plugin::Call<0x564000, CEntity*>(vehiclePtr);
}

// 0x564050
void CWorld::ShutDown() {
    plugin::Call<0x564050>();
}

// 0x564360
void CWorld::ClearForRestart() {
    if (CCutsceneMgr::ms_cutsceneLoadStatus == 2)
        CCutsceneMgr::DeleteCutsceneData();

    CProjectileInfo::RemoveAllProjectiles();
    CObject::DeleteAllTempObjects();
    CObject::DeleteAllMissionObjects();
    for (auto& pSector : CWorld::ms_aRepeatSectors) {
        auto pedNode = pSector.m_lists[eRepeatSectorList::REPEATSECTOR_PEDS].GetNode();
        while (pedNode) {
            auto ped = reinterpret_cast<CPed*>(pedNode->m_item);
            pedNode = pedNode->m_next;

            ped->Remove();
            if (ped->IsPhysical())
                ped->RemoveFromMovingList();

            delete ped;
        }

        auto vehicleNode = pSector.m_lists[eRepeatSectorList::REPEATSECTOR_VEHICLES].GetNode();
        while (vehicleNode) {
            auto vehicle = reinterpret_cast<CVehicle*>(vehicleNode->m_item);
            vehicleNode = vehicleNode->m_next;

            vehicle->Remove();
            if (vehicle->IsPhysical())
                vehicle->RemoveFromMovingList();

            delete vehicle;
        }
    }

    CPickups::ReInit();
    CPools::CheckPoolsEmpty();
}

// 0x564420
bool CWorld::ProcessVerticalLineSector_FillGlobeColPoints(CSector& sector, CRepeatSector& repeatSector, const CColLine& colLine, CEntity*& outEntity, bool buildings, bool vehicles, bool peds, bool objects, bool dummies, bool doSeeThroughCheck, CStoredCollPoly* outCollPoly) {
    bool bSuccess{};

    bSuccess |= buildings && ProcessVerticalLineSectorList_FillGlobeColPoints(sector.m_buildings, colLine, outEntity, doSeeThroughCheck, outCollPoly);
    bSuccess |= vehicles && ProcessVerticalLineSectorList_FillGlobeColPoints(repeatSector.m_lists[0], colLine, outEntity, doSeeThroughCheck, outCollPoly);
    bSuccess |= peds && ProcessVerticalLineSectorList_FillGlobeColPoints(repeatSector.m_lists[1], colLine, outEntity, doSeeThroughCheck, outCollPoly);
    bSuccess |= objects && ProcessVerticalLineSectorList_FillGlobeColPoints(repeatSector.m_lists[2], colLine, outEntity, doSeeThroughCheck, outCollPoly);
    bSuccess |= dummies && ProcessVerticalLineSectorList_FillGlobeColPoints(sector.m_dummies, colLine, outEntity, doSeeThroughCheck, outCollPoly);

    return bSuccess;
}

// 0x564500
bool CWorld::ProcessVerticalLineSector(CSector& sector, CRepeatSector& repeatSector, const CColLine& colLine, CColPoint& outColPoint, CEntity*& outEntity, bool buildings, bool vehicles, bool peds, bool objects, bool dummies, bool doSeeThroughCheck, CStoredCollPoly* outCollPoly) {
    float max{};

    if (buildings)
        ProcessVerticalLineSectorList(sector.m_buildings, colLine,  outColPoint, max, outEntity, doSeeThroughCheck, outCollPoly);
    if (vehicles)
        ProcessVerticalLineSectorList(repeatSector.m_lists[0], colLine, outColPoint, max,  outEntity, doSeeThroughCheck, outCollPoly);
    if (peds)
        ProcessVerticalLineSectorList(repeatSector.m_lists[1], colLine, outColPoint, max,  outEntity, doSeeThroughCheck, outCollPoly);
    if (objects)
        ProcessVerticalLineSectorList(repeatSector.m_lists[2], colLine, outColPoint, max,  outEntity, doSeeThroughCheck, outCollPoly);
    if (dummies)
        ProcessVerticalLineSectorList(sector.m_dummies, colLine,  outColPoint, max, outEntity, doSeeThroughCheck, outCollPoly);

    return max < 1.0f;
}

// 0x564600
void CWorld::CastShadow(float x1, float y1, float x2, float y2) {
    const size_t fromSecX = std::max(0, GetSectorX(x1)), fromSecY = std::max(0, GetSectorY(y1));
    const size_t toSexX = std::min(120, GetSectorX(x2)), toSecY = std::min(120, GetSectorY(y2));

    IncrementCurrentScanCode();

    for (size_t x = fromSecX; x <= toSexX; x++) {
        for (size_t y = fromSecY; y <= toSecY; y++) {
            CastShadowSectorList(GetSector(x, y)->m_buildings, 0.0f, 0.0f, 0.0f, 0.0f); // todo (Izzotop): Add a comment why these 4 floats are useless
        }
    }
}

// 0x5647F0
void CWorld::ProcessAttachedEntities() {
    plugin::Call<0x5647F0>();
}

// 0x564970
bool CWorld::GetIsLineOfSightSectorListClear(CPtrList& ptrList, const CColLine& colLine, bool doSeeThroughCheck, bool doCameraIgnoreCheck) {
    for (CPtrNode* it = ms_listMovingEntityPtrs.m_node; it; it = it->m_next) {
        auto obj = static_cast<CObject*>(it->m_item);
        if (obj->m_nScanCode == ms_nCurrentScanCode || !obj->m_bUsesCollision)
            continue;

        obj->m_nScanCode = ms_nCurrentScanCode;

        if (obj == pIgnoreEntity)
            continue;

        if (doCameraIgnoreCheck && CameraToIgnoreThisObject(obj))
            continue;

        if (CCollision::TestLineOfSight(colLine, obj->GetMatrix(), *obj->GetColModel(), doSeeThroughCheck, false))
            return false;
    }
    return true;
}

// 0x564A20
void CWorld::FindObjectsInRange(const CVector& point, float radius, bool b2D, int16* outCount, int16 maxCount, CEntity** outEntities, bool buildings, bool vehicles, bool peds, bool objects, bool dummies) {
    plugin::Call<0x564A20, const CVector&, float, bool, int16*, int16, CEntity**, bool, bool, bool, bool, bool>(point, radius, b2D, outCount, maxCount, outEntities, buildings, vehicles, peds, objects, dummies);
}

// 0x564C70
void CWorld::FindObjectsOfTypeInRange(uint32 modelId, const CVector& point, float radius, bool b2D, int16* outCount, int16 maxCount, CEntity** outEntities, bool buildings, bool vehicles, bool peds, bool objects, bool dummies) {
    plugin::Call<0x564C70, uint32, const CVector&, float, bool, int16*, int16, CEntity**, bool, bool, bool, bool, bool>(modelId, point, radius, b2D, outCount, maxCount, outEntities, buildings, vehicles, peds, objects, dummies);
}

// 0x564ED0
void CWorld::FindLodOfTypeInRange(uint32 modelId, const CVector& point, float radius, bool b2D, int16* outCount, int16 maxCount, CEntity** outEntities) {
    plugin::Call<0x564ED0, uint32, const CVector&, float, bool, int16*, int16, CEntity**>(modelId, point, radius, b2D, outCount, maxCount, outEntities);
}

// 0x565000
void CWorld::FindObjectsKindaCollidingSectorList(CPtrList& ptrList, const CVector& point, float radius, bool b2D, int16* outCount, int16 maxCount, CEntity** outEntities) {
    for (CPtrNode* it = ms_listMovingEntityPtrs.m_node; it; it = it->m_next) {
        auto entity = static_cast<CEntity*>(it->m_item);
        if (entity->m_nScanCode == ms_nCurrentScanCode)
            continue;

        entity->m_nScanCode = ms_nCurrentScanCode;

        const float fRadiusToCheck = CModelInfo::GetModelInfo(entity->m_nModelIndex)->GetColModel()->GetBoundRadius() + radius;
        if (b2D) {
            if (DistanceBetweenPoints2D(CVector2D{ point }, entity->GetBoundCentre()) >= fRadiusToCheck)
                continue;
        } else {
            if (DistanceBetweenPoints(point, entity->GetBoundCentre()) >= fRadiusToCheck)
                continue;
        }

        if (*outCount < maxCount) {
            if (outEntities)
                outEntities[*outCount] = entity;
            outCount++;
        }
    }
}

// 0x5650E0
void CWorld::FindObjectsIntersectingCubeSectorList(CPtrList& ptrList, const CVector& min, const CVector& max, int16* outCount, int16 maxCount, CEntity** outEntities) {
    for (CPtrNode* it = ms_listMovingEntityPtrs.m_node; it; it = it->m_next) {
        auto entity = static_cast<CEntity*>(it->m_item);
        if (entity->m_nScanCode == ms_nCurrentScanCode)
            continue;

        entity->m_nScanCode = ms_nCurrentScanCode;

        const float fBoundRadius = entity->GetColModel()->GetBoundRadius();
        const CVector pos = entity->GetPosition();

        // TODO:
        // The check below can be done using a bounding box.
        // const CVector vecOfRadius{fBoundRadius, fBoundRadius, fBoundRadius};
        // CBoundingBox bb{min - vecOfRadius, max + vecOfRadius};
        // bb.IsPointWithin(pos);

        if (pos.x + fBoundRadius >= min.x &&
            pos.x - fBoundRadius <= max.x && 
            pos.y + fBoundRadius >= min.y && 
            pos.y - fBoundRadius <= max.y && 
            pos.z + fBoundRadius >= min.z && 
            pos.z - fBoundRadius <= max.z 
        ) {
            if (*outCount < maxCount) {
                if (outEntities)
                    outEntities[*outCount] = entity;
                outCount++;
            }
        }
    }
}

// 0x565200
void CWorld::FindObjectsIntersectingAngledCollisionBoxSectorList(CPtrList& ptrList, CBox const& box, const CMatrix& transform, const CVector& point, int16* outCount, int16 maxCount, CEntity** outEntities) {
    for (CPtrNode* it = ms_listMovingEntityPtrs.m_node; it; it = it->m_next) {
        auto entity = static_cast<CEntity*>(it->m_item);
        if (entity->m_nScanCode == ms_nCurrentScanCode)
            continue;
        entity->m_nScanCode = ms_nCurrentScanCode;

        CColSphere sphere{};
        sphere.Set(
            entity->GetColModel()->GetBoundRadius(),
            Multiply3x3(entity->GetPosition() - point, transform)
        );

        if (CCollision::TestSphereBox(sphere, box)) {
            if (*outCount < maxCount) {
                if (outEntities)
                    outEntities[*outCount] = entity;
                outCount++;
            }
        }
    }
}

// 0x565300
void CWorld::FindMissionEntitiesIntersectingCubeSectorList(CPtrList& ptrList, const CVector& cornerA, const CVector& cornerB, int16* outCount, int16 maxCount, CEntity** outEntities, bool vehiclesList, bool pedsList, bool objectsList) {
    plugin::Call<0x565300, CPtrList&, const CVector&, const CVector&, int16*, int16, CEntity**, bool, bool, bool>(ptrList, cornerA, cornerB, outCount, maxCount, outEntities, vehiclesList, pedsList, objectsList);
}

// 0x565450
void CWorld::FindNearestObjectOfTypeSectorList(int32 modelId, CPtrList& ptrList, const CVector& point, float radius, bool b2D, CEntity** outEntities, float* outDistance) {
    plugin::Call<0x565450, int32, CPtrList&, const CVector&, float, bool, CEntity**, float*>(modelId, ptrList, point, radius, b2D, outEntities, outDistance);
}

// 0x565510
void CWorld::RemoveReferencesToDeletedObject(CEntity* entity) {
    plugin::Call<0x565510, CEntity*>(entity);
}

// 0x565610
void CWorld::SetPedsOnFire(float x1, float y1, float x2, float y2, CEntity* fireCreator) {
    plugin::Call<0x565610, float, float, float, float, CEntity*>(x1, y1, x2, y2, fireCreator);
}

// 0x565800
void CWorld::SetPedsChoking(float x1, float y1, float x2, float y2, CEntity* gasCreator) {
    plugin::Call<0x565800, float, float, float, float, CEntity*>(x1, y1, x2, y2, gasCreator);
}

// 0x5659F0
void CWorld::SetCarsOnFire(float x1, float y1, float x2, float y2, CEntity* fireCreator) {
    plugin::Call<0x5659F0, float, float, float, float, CEntity*>(x1, y1, x2, y2, fireCreator);
}

// 0x565B70
bool CWorld::SprayPaintWorld(CVector& posn, CVector& outDir, float radius, bool processTagAlphaState) {
    return plugin::CallAndReturn<bool, 0x565B70, CVector&, CVector&, float, bool>(posn, outDir, radius, processTagAlphaState);
}

// 0x565CB0
void CWorld::RemoveFallenPeds() {
    for (int32 i = CPools::ms_pPedPool->GetSize(); i; i--) {
        CPed* ped = CPools::ms_pPedPool->GetAt(i - 1);
        if (!ped)
            continue;

        const CVector& vecPedPos = ped->GetPosition();
        if (vecPedPos.z > -100.0f)
            continue;
        if (!ped->IsCreatedBy(ePedCreatedBy::PED_GAME) || ped->IsPlayer()) {
            CNodeAddress pathNodeAddress;
            ThePaths.FindNodeClosestToCoors(&pathNodeAddress, vecPedPos.x, vecPedPos.y, vecPedPos.z, 1, 1000000.0f, 0, 0, 0, 0, 0);
            if (pathNodeAddress.m_wAreaId != -1) {
                CVector pathNodePos = ThePaths.GetPathNode(pathNodeAddress)->GetNodeCoors();
                pathNodePos.z += 2.0f;
                ped->Teleport(pathNodePos, false);
            } else
                ped->SetPosn(vecPedPos.x, vecPedPos.y, -95);
            ped->ResetMoveSpeed();
        } else
            CPopulation::RemovePed(ped);
    }
}

// 0x565E80
void CWorld::RemoveFallenCars() {
    for (int32 i = CPools::ms_pVehiclePool->GetSize(); i; i--) {
        CVehicle* vehicle = CPools::ms_pVehiclePool->GetAt(i - 1);
        if (!vehicle)
            continue;

        const CVector& vecPos = vehicle->GetPosition();
        if (vecPos.z > -100.0f)
            continue;

        const auto ShouldWeKeepIt = [vehicle]() {
            if (vehicle->IsCreatedBy(eVehicleCreatedBy::MISSION_VEHICLE) && !vehicle->physicalFlags.bDestroyed)
                return true;

            if (vehicle == FindPlayerVehicle(-1, false))
                return true;

            return vehicle->m_pDriver && vehicle->m_pDriver->IsPlayer();
        };

        if (ShouldWeKeepIt()) {
            CNodeAddress pathNodeAddress;
            ThePaths.FindNodeClosestToCoors(&pathNodeAddress, vecPos.x, vecPos.y, vecPos.z, 1, 1000000.0f, 0, 0, 0, 0, 0);
            if (pathNodeAddress.m_wAreaId != -1) {
                const auto pathNodePos = ThePaths.GetPathNode(pathNodeAddress)->GetNodeCoors();
                vehicle->Teleport(pathNodePos + CVector(0, 0, 3), true);
            } else
                vehicle->Teleport(CVector(vecPos.x, vecPos.y, 0), true);
            vehicle->ResetMoveSpeed();
        } else {
            if (!vehicle->IsCreatedBy(eVehicleCreatedBy::RANDOM_VEHICLE))
                if (!vehicle->IsCreatedBy(eVehicleCreatedBy::PARKED_VEHICLE))
                    continue;

            vehicle->Remove();
            if (vehicle->IsPhysical())
                vehicle->RemoveFromMovingList();

            delete vehicle;
        }
    }
}

// 0x5660B0
void CWorld::UseDetonator(CEntity* creator) {
    plugin::Call<0x5660B0, CEntity*>(creator);
}

// 0x566140
CEntity* CWorld::TestSphereAgainstSectorList(CPtrList& ptrList, CVector sphereCenter, float sphereRadius, CEntity* ignoreEntity, bool doCameraIgnoreCheck) {
    return plugin::CallAndReturn<CEntity*, 0x566140, CPtrList&, CVector, float, CEntity*, bool>(ptrList, sphereCenter, sphereRadius, ignoreEntity, doCameraIgnoreCheck);
}

// 0x566420
void CWorld::PrintCarChanges() {
    plugin::Call<0x566420>();
}

// 0x5664A0
void CWorld::TestForBuildingsOnTopOfEachOther() {
    plugin::Call<0x5664A0>();
}

// 0x566510
void CWorld::TestForUnusedModels() {
    plugin::Call<0x566510>();
}

// 0x566610
void CWorld::ClearCarsFromArea(float x1, float y1, float z1, float x2, float y2, float z2) {
    plugin::Call<0x566610, float, float, float, float, float, float>(x1, y1, z1, x2, y2, z2);
}

// 0x5667F0
void CWorld::ClearPedsFromArea(float x1, float y1, float z1, float x2, float y2, float z2) {
    plugin::Call<0x5667F0, float, float, float, float, float, float>(x1, y1, z1, x2, y2, z2);
}

// 0x5668F0
void CWorld::SetAllCarsCanBeDamaged(bool enable) {
    plugin::Call<0x5668F0, bool>(enable);
}

// 0x566950
void CWorld::ExtinguishAllCarFiresInArea(CVector point, float radius) {
    plugin::Call<0x566950, CVector, float>(point, radius);
}

// 0x566A60
void CWorld::CallOffChaseForArea(float x1, float y1, float x2, float y2) {
    plugin::Call<0x566A60, float, float, float, float>(x1, y1, x2, y2);
}

// 0x566C10
void CWorld::StopAllLawEnforcersInTheirTracks() {
    plugin::Call<0x566C10>();
}

// 0x566C90
CVehicle* CWorld::FindUnsuspectingTargetCar(CVector point, CVector playerPosn) {
    return plugin::CallAndReturn<CVehicle*, 0x566C90, CVector, CVector>(point, playerPosn);
}

// 0x566DA0
CPed* CWorld::FindUnsuspectingTargetPed(CVector point, CVector playerPosn) {
    return plugin::CallAndReturn<CPed*, 0x566DA0, CVector, CVector>(point, playerPosn);

    float fNearest2D = std::numeric_limits<float>::max();
    CPed* nearestPed{};
    for (int32 i = 0; i < CPools::ms_pPedPool->GetSize(); i++) {
        CPed* ped = CPools::ms_pPedPool->GetAt(i);
        if (!ped)
            continue;

        if (!ped->IsCreatedBy(ePedCreatedBy::PED_GAME) || !ped->IsAlive())
            continue;

        if (ped->m_nPedType <= ePedType::PED_TYPE_GANG1 || ped->m_nPedType >= ePedType::PED_TYPE_GANG10)
            if (ped->m_nPedType != ePedType::PED_TYPE_CIVMALE)
                continue;

        if (CTask* pActive = ped->GetTaskManager().GetActiveTask())
            if (pActive->GetTaskType() != eTaskType::TASK_COMPLEX_WANDER)
                continue;

        const CVector pedPos = ped->GetPosition();
        const float fDist2D = DistanceBetweenPoints2D(CVector2D{ point }, pedPos);
        if (fDist2D >= fNearest2D)
            continue;

        if (DotProduct(playerPosn - pedPos, ped->GetForward()) <= 0.0f)
            continue;

        nearestPed = ped;
        fNearest2D = fDist2D;
    }
}

// 0x566EE0
bool CWorld::ProcessLineOfSightSectorList(CPtrList& ptrList, const CColLine& colLine, CColPoint& outColPoint, float& maxTouchDistance, CEntity*& outEntity, bool doSeeThroughCheck, bool doIgnoreCameraCheck, bool doShootThroughCheck) {
    return plugin::CallAndReturn<bool, 0x566EE0, CPtrList&, const CColLine&, CColPoint&, float&, CEntity*&, bool, bool, bool>(ptrList, colLine, outColPoint, maxTouchDistance, outEntity, doSeeThroughCheck, doIgnoreCameraCheck, doShootThroughCheck);
}

// 0x5674E0
bool CWorld::ProcessVerticalLine(const CVector& origin, float distance, CColPoint& outColPoint, CEntity*& outEntity, bool buildings, bool vehicles, bool peds, bool objects, bool dummies, bool doSeeThroughCheck, CStoredCollPoly* outCollPoly) {
    IncrementCurrentScanCode();
    ms_iProcessLineNumCrossings = 0;

    const int32 secX = GetSectorX(origin.x), secY = GetSectorY(origin.y);
    CColLine colLine{ origin, CVector{origin.x, origin.y, distance} };
    return ProcessVerticalLineSector(
        *GetSector(secX, secY),
        *GetRepeatSector(secX % MAX_REPEAT_SECTORS_X, secY % MAX_REPEAT_SECTORS_Y),
        colLine, outColPoint, outEntity, buildings, vehicles, peds, objects, dummies, doSeeThroughCheck, outCollPoly
    );
}

// 0x567620
bool CWorld::ProcessVerticalLine_FillGlobeColPoints(const CVector& origin, float distance, CEntity*& outEntity, bool buildings, bool vehicles, bool peds, bool objects, bool dummies, bool doSeeThroughCheck, CStoredCollPoly* outCollPoly) {
    FilledColPointIndex = 0;

    const int32 secX = GetSectorX(origin.x), secY = GetSectorY(origin.y);
    CColLine colLine{ origin, CVector{origin.x, origin.y, distance} };
    return ProcessVerticalLineSector_FillGlobeColPoints(
        *GetSector(secX, secY),
        *GetRepeatSector(secX % MAX_REPEAT_SECTORS_X, secY % MAX_REPEAT_SECTORS_Y),
        colLine, outEntity, buildings, vehicles, peds, objects, dummies, doSeeThroughCheck, outCollPoly
    );
}

// 0x567750
void CWorld::TriggerExplosionSectorList(CPtrList& ptrList, const CVector& point, float radius, float visibleDistance, CEntity* victim, CEntity* creator, bool processVehicleBombTimer, float damage) {
    plugin::Call<0x567750, CPtrList&, const CVector&, float, float, CEntity*, CEntity*, bool, float>(ptrList, point, radius, visibleDistance, victim, creator, processVehicleBombTimer, damage);
}

// 0x5684A0
void CWorld::Process() {
    plugin::Call<0x5684A0>();
}

// 0x568AD0
bool CWorld::GetIsLineOfSightSectorClear(CSector& sector, CRepeatSector& repeatSector, const CColLine& colLine, bool buildings, bool vehicles, bool peds, bool objects, bool dummies, bool doSeeThroughCheck, bool doIgnoreCameraCheck){
    /* No need for `if`'s because of int16 circuit eval */
    return (!buildings || GetIsLineOfSightSectorListClear(sector.m_buildings, colLine, doSeeThroughCheck, false))
        && (!vehicles  || GetIsLineOfSightSectorListClear(repeatSector.m_lists[0], colLine, doSeeThroughCheck, false))
        && (!peds      || GetIsLineOfSightSectorListClear(repeatSector.m_lists[1], colLine, doSeeThroughCheck, false))
        && (!objects   || GetIsLineOfSightSectorListClear(repeatSector.m_lists[2], colLine, doSeeThroughCheck, false))
        && (!dummies   || GetIsLineOfSightSectorListClear(sector.m_dummies, colLine, doSeeThroughCheck, false));
}

// 0x568B80
void CWorld::FindObjectsKindaColliding(const CVector& point, float radius, bool b2D, int16* outCount, int16 maxCount, CEntity** outEntities, bool buildings, bool vehicles, bool peds, bool objects, bool dummies) {
    plugin::Call<0x568B80, const CVector&, float, bool, int16*, int16, CEntity**, bool, bool, bool, bool, bool>(point, radius, b2D, outCount, maxCount, outEntities, buildings, vehicles, peds, objects, dummies);
}

// 0x568DD0
void CWorld::FindObjectsIntersectingCube(const CVector& cornerA, const CVector& cornerB, int16* outCount, int16 maxCount, CEntity** outEntities, bool buildings, bool vehicles, bool peds, bool objects, bool dummies) {
    plugin::Call<0x568DD0, const CVector&, const CVector&, int16*, int16, CEntity**, bool, bool, bool, bool, bool>(cornerA, cornerB, outCount, maxCount, outEntities, buildings, vehicles, peds, objects, dummies);
}

// 0x568FF0
void CWorld::FindObjectsIntersectingAngledCollisionBox(CBox const& box, const CMatrix& transform, const CVector& point, float x1, float y1, float x2, float y2, int16* outCount, int16 maxCount, CEntity** outEntities, bool buildings, bool vehicles, bool peds, bool objects, bool dummies) {
    plugin::Call<0x568FF0, CBox const&, CMatrix const&, const CVector&, float, float, float, float, int16*, int16, CEntity**, bool, bool, bool, bool, bool>(box, transform, point, x1, y1, x2, y2, outCount, maxCount, outEntities, buildings, vehicles, peds, objects, dummies);
}

// 0x569240
void CWorld::FindMissionEntitiesIntersectingCube(const CVector& cornerA, const CVector& cornerB, int16* outCount, int16 maxCount, CEntity** outEntities, bool vehicles, bool peds, bool objects) {
    plugin::Call<0x569240, const CVector&, const CVector&, int16*, int16, CEntity**, bool, bool, bool>(cornerA, cornerB, outCount, maxCount, outEntities, vehicles, peds, objects);
}

// 0x5693F0
void CWorld::FindNearestObjectOfType(int32 modelId, const CVector& point, float radius, bool b2D, bool buildings, bool vehicles, bool peds, bool objects, bool dummies) {
    plugin::Call<0x5693F0, int32, const CVector&, float, bool, bool, bool, bool, bool, bool>(modelId, point, radius, b2D, buildings, vehicles, peds, objects, dummies);
}

// 0x569660
float CWorld::FindGroundZForCoord(float x, float y) {
    return plugin::CallAndReturn<float, 0x569660, float, float>(x, y);
}

// 0x5696C0
float CWorld::FindGroundZFor3DCoord(float x, float y, float z, bool* outResult, CEntity** outEntity) {
    CEntity* localOutEntity{};
    CColPoint colPoint;
    if (ProcessVerticalLine({ x, y, z }, 1000.0f, colPoint, localOutEntity, true, false, false, false, true, false, nullptr)) {
        if (outResult)
            *outResult = true;
        if (outEntity)
            *outEntity = localOutEntity;
        return colPoint.m_vecPoint.z;
    }
    else {
        if (outResult)
            *outResult = false;
        if (outEntity)
            *outEntity = nullptr;
        return 0.0f;
    }
}

// 0x569750
float CWorld::FindRoofZFor3DCoord(float x, float y, float z, bool* outResult) {
    CEntity* outEntity{};
    CColPoint colPoint;
    if (ProcessVerticalLine({ x, y, z }, 1000.0f, colPoint, outEntity, true, false, false, false, true, false, nullptr)) {
        if (outResult)
            *outResult = true;
        return colPoint.m_vecPoint.z;
    } else {
        if (outResult)
            *outResult = false;
        else
            printf("THERE IS NO MAP BELOW THE FOLLOWING COORS:%f %f %f. (FindGroundZFor3DCoord)\n", x, y, z); // R* triggered
        return 20.0f;
    }
}

// 0x5697F0
float CWorld::FindLowestZForCoord(float x, float y) {
    CEntity* outEntity{};
    CColPoint colPoint;
    if (ProcessVerticalLine({ x, y, -1000.0f }, 1000.0f, colPoint, outEntity, true, false, false, false, true, false, nullptr))
        return colPoint.m_vecPoint.z;

    return 20.0f;
}

// 0x569850
void CWorld::RepositionOneObject(CEntity* object) {
    plugin::Call<0x569850, CEntity*>(object);
}

// 0x569E20
CEntity* CWorld::TestSphereAgainstWorld(CVector sphereCenter, float sphereRadius, CEntity* arg2, bool buildings, bool vehicles, bool peds, bool objects, bool dummies, bool doCameraIgnoreCheck) {
    return plugin::CallAndReturn<CEntity*, 0x569E20, CVector, float, CEntity*, bool, bool, bool, bool, bool, bool>(sphereCenter, sphereRadius, arg2, buildings, vehicles, peds, objects, dummies, doCameraIgnoreCheck);
}

// 0x56A0D0
void CWorld::ClearExcitingStuffFromArea(const CVector& point, float radius, uint8 bRemoveProjectilesAndShadows) {
    plugin::Call<0x56A0D0, const CVector&, float, uint8>(point, radius, bRemoveProjectilesAndShadows);
}

// 0x56A490
bool CWorld::GetIsLineOfSightClear(const CVector& origin, const CVector& target, bool buildings, bool vehicles, bool peds, bool objects, bool dummies, bool doSeeThroughCheck, bool doCameraIgnoreCheck) {
    return plugin::CallAndReturn<bool, 0x56A490, const CVector&, const CVector&, bool, bool, bool, bool, bool, bool, bool>(origin, target, buildings, vehicles, peds, objects, dummies, doSeeThroughCheck, doCameraIgnoreCheck);
}

// Converted from cdecl bool CWorld::ProcessLineOfSightSector(CSector &sector,CRepeatSector &repeatSector,const CColLine&colLine,CColPoint &outColPoint,float &maxTouchDistance,CEntity *&outEntity,bool buildings, bool vehicles, bool peds, bool objects, bool dummies,bool doSeeThroughCheck,bool doCameraIgnoreCheck,bool doShootThroughCheck) 0x56B5E0
bool CWorld::ProcessLineOfSightSector(CSector& sector, CRepeatSector& repeatSector, const CColLine& colLine, CColPoint& outColPoint, float& maxTouchDistance, CEntity*& outEntity, bool buildings, bool vehicles, bool peds, bool objects, bool dummies, bool doSeeThroughCheck, bool doCameraIgnoreCheck, bool doShootThroughCheck) {
    return plugin::CallAndReturn<bool, 0x56B5E0, CSector&, CRepeatSector&, const CColLine&, CColPoint&, float&, CEntity*&, bool, bool, bool, bool, bool, bool, bool, bool>(sector, repeatSector, colLine, outColPoint, maxTouchDistance, outEntity, buildings, vehicles, peds, objects, dummies, doSeeThroughCheck, doCameraIgnoreCheck, doShootThroughCheck);
}

// 0x56B790
void CWorld::TriggerExplosion(const CVector& point, float radius, float visibleDistance, CEntity* victim, CEntity* creator, bool processVehicleBombTimer, float damage) {
    plugin::Call<0x56B790, const CVector&, float, float, CEntity*, CEntity*, bool, float>(point, radius, visibleDistance, victim, creator, processVehicleBombTimer, damage);
}

// 0x56B910
void CWorld::SetWorldOnFire(float x, float y, float z, float radius, CEntity* fireCreator) {
    plugin::Call<0x56B910, float, float, float, float, CEntity*>(x, y, z, radius, fireCreator);
}

// 0x56B9C0
void CWorld::RepositionCertainDynamicObjects() {
    plugin::Call<0x56B9C0>();
}

// 0x56BA00
bool CWorld::ProcessLineOfSight(const CVector& origin, const CVector& target, CColPoint& outColPoint, CEntity*& outEntity, bool buildings, bool vehicles, bool peds, bool objects, bool dummies, bool doSeeThroughCheck, bool doCameraIgnoreCheck, bool doShootThroughCheck) {
    return plugin::CallAndReturn<bool, 0x56BA00, const CVector&, const CVector&, CColPoint&, CEntity*&, bool, bool, bool, bool, bool, bool, bool, bool>(origin, target, outColPoint, outEntity, buildings, vehicles, peds, objects, dummies, doSeeThroughCheck, doCameraIgnoreCheck, doShootThroughCheck);
}

void CWorld::IncrementCurrentScanCode() {
    if (CWorld::ms_nCurrentScanCode >= 65535u)
    {
        CWorld::ClearScanCodes();
        CWorld::ms_nCurrentScanCode = 1;
    }
    else
    {
        ++CWorld::ms_nCurrentScanCode;
    }
}

// 0x407250
int16 GetCurrentScanCode() {
    return plugin::CallAndReturn<int16, 0x407250>();
}

// 0x407260
CSector* GetSector(int32 x, int32 y) {
    return plugin::CallAndReturn<CSector*, 0x407260, int32, int32>(x, y);
}

// 0x4072A0
CRepeatSector* GetRepeatSector(int32 x, int32 y) {
    return plugin::CallAndReturn<CRepeatSector*, 0x4072A0, int32, int32>(x, y);
}

// 0x4072C0
CPtrListSingleLink& CWorld::GetLodPtrList(int32 x, int32 y) {
    x = clamp<int32>(x, 0, MAX_LOD_PTR_LISTS_X - 1);
    y = clamp<int32>(y, 0, MAX_LOD_PTR_LISTS_Y - 1);
    return ms_aLodPtrLists[y][x];
    // return plugin::CallAndReturn<CPtrListSingleLink*, 0x4072C0, int32, int32>(x, y);
}

// 0x4072E0
void SetNextScanCode() {
    plugin::Call<0x4072E0>();
}

float ScaleLighting(uint8 lighting, float fScale)
{
    return plugin::CallAndReturn<float, 0x59F0C0, uint8, float> (lighting, fScale);
}
