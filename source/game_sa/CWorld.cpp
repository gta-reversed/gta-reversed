/*
Plugin-SDK (Grand Theft Auto San Andreas) source file
Authors: GTA Community. See more here
https://github.com/DK22Pac/plugin-sdk
Do not delete this comment block. Respect others' work!
*/
#include "StdInc.h"

int32 CWorld::TOTAL_PLAYERS = 2;
int32 &CWorld::ms_iProcessLineNumCrossings = *(int32 *)0xB7CD60;
float &CWorld::fWeaponSpreadRate = *(float *)0xB7CD64;
CEntity *&CWorld::pIgnoreEntity = *(CEntity **)0xB7CD68;
bool &CWorld::bSecondShift = *(bool *)0xB7CD6C;
bool &CWorld::bProcessCutsceneOnly = *(bool *)0xB7CD6D;
bool &CWorld::bForceProcessControl = *(bool *)0xB7CD6E;
bool &CWorld::bIncludeBikers = *(bool *)0xB7CD6F;
bool &CWorld::bIncludeCarTyres = *(bool *)0xB7CD70;
bool &CWorld::bIncludeDeadPeds = *(bool *)0xB7CD71;
bool &CWorld::bNoMoreCollisionTorque = *(bool *)0xB7CD72;
bool &CWorld::bDoingCarCollisions = *(bool *)0xB7CD73;
char &CWorld::PlayerInFocus = *(char *)0xB7CD74;
uint16&CWorld::ms_nCurrentScanCode = *(uint16*)0xB7CD78;
CPlayerInfo *CWorld::Players = (CPlayerInfo *)0xB7CD98;
CSector *CWorld::ms_aSectors = (CSector *)0xB7D0B8;
CRepeatSector(&CWorld::ms_aRepeatSectors)[MAX_REPEAT_SECTORS] = *(CRepeatSector(*)[MAX_REPEAT_SECTORS])0xB992B8;
CPtrListSingleLink(&CWorld::ms_aLodPtrLists)[MAX_LOD_PTR_LISTS_Y][MAX_LOD_PTR_LISTS_X] = *(CPtrListSingleLink(*)[MAX_LOD_PTR_LISTS_Y][MAX_LOD_PTR_LISTS_X])0xB99EB8;
CPtrListDoubleLink &CWorld::ms_listMovingEntityPtrs = *(CPtrListDoubleLink *)0xB9ACC8;
CPtrListDoubleLink &CWorld::ms_listObjectsWithControlCode = *(CPtrListDoubleLink *)0xB9ACCC;
CColPoint *CWorld::m_aTempColPts = (CColPoint *)0xB9ACD0;
CVector &CWorld::SnookerTableMax = *(CVector *)0x8CDEF4;
CVector &CWorld::SnookerTableMin = *(CVector *)0x8CDF00;
uint32 &FilledColPointIndex = *(uint32*)0xB7CD7C;
CColPoint *gaTempSphereColPoints = (CColPoint *)0xB9B250;
int16 &TAG_SPRAYING_INCREMENT_VAL = *(int16 *)0x8CDEF0;

void CWorld::InjectHooks() {
    ReversibleHooks::Install("CWorld", "RemoveFallenPeds", 0x565CB0, &RemoveFallenPeds);
    ReversibleHooks::Install("CWorld", "RemoveFallenCars", 0x565E80, &RemoveFallenCars);
    ReversibleHooks::Install("CWorld", "Remove", 0x563280, &CWorld::Remove);
    ReversibleHooks::Install("CWorld", "ClearForRestart", 0x564360, &CWorld::ClearForRestart);
}

// 0x5631C0
void CWorld::ResetLineTestOptions() {
    plugin::Call<0x5631C0>();
}

// 0x5631E0
void CWorld::Initialise() {
    plugin::Call<0x5631E0>();
}

// 0x563220
void CWorld::Add(CEntity* entity) {
    plugin::Call<0x563220, CEntity*>(entity);
}

// 0x563280
void CWorld::Remove(CEntity* entity) 
{
    entity->Remove();
    if (entity->IsPhysical())
        static_cast<CPhysical*>(entity)->RemoveFromMovingList();
}

// 0x5632B0
bool CWorld::ProcessVerticalLineSectorList(CPtrList& ptrList, CColLine const& colLine, CColPoint& colPoint, float& maxTouchDistance, CEntity*& outEntity, bool doSeeThroughCheck, CStoredCollPoly* collPoly) {
    return plugin::CallAndReturn<bool, 0x5632B0, CPtrList&, CColLine const&, CColPoint&, float&, CEntity*&, bool, CStoredCollPoly*>(ptrList, colLine, colPoint, maxTouchDistance, outEntity, doSeeThroughCheck, collPoly);
}

// 0x563390
void CWorld::CastShadowSectorList(CPtrList& ptrList, float arg1, float arg2, float arg3, float arg4) {
    plugin::Call<0x563390, CPtrList&, float, float, float, float>(ptrList, arg1, arg2, arg3, arg4);
}

// 0x5633D0
void CWorld::ProcessForAnimViewer() {
    plugin::Call<0x5633D0>();
}

// 0x563430
void CWorld::ProcessPedsAfterPreRender() {
    plugin::Call<0x563430>();
}

// 0x563470
void CWorld::ClearScanCodes() {
    plugin::Call<0x563470>();
}

// 0x563500
void CWorld::FindObjectsInRangeSectorList(CPtrList& arg0, CVector const& point, float radius, bool b2D, int16* outCount, int16 maxCount, CEntity** outEntities) {
    plugin::Call<0x563500, CPtrList&, CVector const&, float, bool, int16*, int16, CEntity**>(arg0, point, radius, b2D, outCount, maxCount, outEntities);
}

// 0x5635C0
void CWorld::FindObjectsOfTypeInRangeSectorList(uint32 modelId, CPtrList& ptrList, CVector const& point, float radius, bool b2D, int16* outCount, int16 maxCount, CEntity** outEntities) {
    plugin::Call<0x5635C0, uint32, CPtrList&, CVector const&, float, bool, int16*, int16, CEntity**>(modelId, ptrList, point, radius, b2D, outCount, maxCount, outEntities);
}

// 0x5636A0
bool CWorld::ProcessVerticalLineSectorList_FillGlobeColPoints(CPtrList& ptrList, CColLine const& colLine, CEntity*& outEntity, bool doSeeThroughCheck, CStoredCollPoly* outCollPoly) {
    return plugin::CallAndReturn<bool, 0x5636A0, CPtrList&, CColLine const&, CEntity*&, bool, CStoredCollPoly*>(ptrList, colLine, outEntity, doSeeThroughCheck, outCollPoly);
}

// 0x563840
void CWorld::RemoveStaticObjects() {
    plugin::Call<0x563840>();
}

// 0x563950
void CWorld::TestForBuildingsOnTopOfEachOther(CPtrList& ptrList) {
    plugin::Call<0x563950, CPtrList&>(ptrList);
}

// 0x5639D0
void CWorld::TestForUnusedModels(CPtrList& ptrList, int32* models) {
    plugin::Call<0x5639D0, CPtrList&, int32*>(ptrList, models);
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
    return plugin::CallAndReturn<bool, 0x563F40, CEntity*>(entity);
}

// 0x563FA0
int32 CWorld::FindPlayerSlotWithPedPointer(void* ptr) {
    return plugin::CallAndReturn<int32, 0x563FA0, void*>(ptr);
}

// 0x563FD0
int32 CWorld::FindPlayerSlotWithRemoteVehiclePointer(void* ptr) {
    return plugin::CallAndReturn<int32, 0x563FD0, void*>(ptr);
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
        auto pPedNode = pSector.m_lists[eRepeatSectorList::REPEATSECTOR_PEDS].pNode;
        while (pPedNode) {
            auto pPed = reinterpret_cast<CPed*>(pPedNode->pItem);
            pPedNode = pPedNode->pNext;

            pPed->Remove();
            if (pPed->IsPhysical())
                pPed->RemoveFromMovingList();

            if (pPed)
                delete pPed;
        }

        auto pVehNode = pSector.m_lists[eRepeatSectorList::REPEATSECTOR_VEHICLES].pNode;
        while (pVehNode) {
            auto pVeh = reinterpret_cast<CVehicle*>(pVehNode->pItem);
            pVehNode = pVehNode->pNext;

            pVeh->Remove();
            if (pVeh->IsPhysical())
                pVeh->RemoveFromMovingList();

            if (pVeh)
                delete pVeh;
        }
    }

    CPickups::ReInit();
    CPools::CheckPoolsEmpty();
}

// 0x564420
bool CWorld::ProcessVerticalLineSector_FillGlobeColPoints(CSector& sector, CRepeatSector& repeatSector, CColLine const& colLine, CEntity*& outEntity, bool buildings, bool vehicles, bool peds, bool objects, bool dummies, bool doSeeThroughCheck, CStoredCollPoly* outCollPoly) {
    return plugin::CallAndReturn<bool, 0x564420, CSector&, CRepeatSector&, CColLine const&, CEntity*&, bool, bool, bool, bool, bool, bool, CStoredCollPoly*>(sector, repeatSector, colLine, outEntity, buildings, vehicles, peds, objects, dummies, doSeeThroughCheck, outCollPoly);
}

// 0x564500
bool CWorld::ProcessVerticalLineSector(CSector& sector, CRepeatSector& repeatSector, CColLine const& colLine, CColPoint& outColPoint, CEntity*& outEntity, bool buildings, bool vehicles, bool peds, bool objects, bool dummies, bool doSeeThroughCheck, CStoredCollPoly* outCollPoly) {
    return plugin::CallAndReturn<bool, 0x564500, CSector&, CRepeatSector&, CColLine const&, CColPoint&, CEntity*&, bool, bool, bool, bool, bool, bool, CStoredCollPoly*>(sector, repeatSector, colLine, outColPoint, outEntity, buildings, vehicles, peds, objects, dummies, doSeeThroughCheck, outCollPoly);
}

// 0x564600
void CWorld::CastShadow(float x1, float y1, float x2, float y2) {
    plugin::Call<0x564600, float, float, float, float>(x1, y1, x2, y2);
}

// 0x5647F0
void CWorld::ProcessAttachedEntities() {
    plugin::Call<0x5647F0>();
}

// 0x564970
bool CWorld::GetIsLineOfSightSectorListClear(CPtrList& ptrList, CColLine const& colLine, bool doSeeThroughCheck, bool doCameraIgnoreCheck) {
    return plugin::CallAndReturn<bool, 0x564970, CPtrList&, CColLine const&, bool, bool>(ptrList, colLine, doSeeThroughCheck, doCameraIgnoreCheck);
}

// 0x564A20
void CWorld::FindObjectsInRange(CVector const& point, float radius, bool b2D, int16* outCount, int16 maxCount, CEntity** outEntities, bool buildings, bool vehicles, bool peds, bool objects, bool dummies) {
    plugin::Call<0x564A20, CVector const&, float, bool, int16*, int16, CEntity**, bool, bool, bool, bool, bool>(point, radius, b2D, outCount, maxCount, outEntities, buildings, vehicles, peds, objects, dummies);
}

// 0x564C70
void CWorld::FindObjectsOfTypeInRange(uint32 modelId, CVector const& point, float radius, bool b2D, int16* outCount, int16 maxCount, CEntity** outEntities, bool buildings, bool vehicles, bool peds, bool objects, bool dummies) {
    plugin::Call<0x564C70, uint32, CVector const&, float, bool, int16*, int16, CEntity**, bool, bool, bool, bool, bool>(modelId, point, radius, b2D, outCount, maxCount, outEntities, buildings, vehicles, peds, objects, dummies);
}

// 0x564ED0
void CWorld::FindLodOfTypeInRange(uint32 modelId, CVector const& point, float radius, bool b2D, int16* outCount, int16 maxCount, CEntity** outEntities) {
    plugin::Call<0x564ED0, uint32, CVector const&, float, bool, int16*, int16, CEntity**>(modelId, point, radius, b2D, outCount, maxCount, outEntities);
}

// 0x565000
void CWorld::FindObjectsKindaCollidingSectorList(CPtrList& ptrList, CVector const& point, float radius, bool b2D, int16* outCount, int16 maxCount, CEntity** outEntities) {
    plugin::Call<0x565000, CPtrList&, CVector const&, float, bool, int16*, int16, CEntity**>(ptrList, point, radius, b2D, outCount, maxCount, outEntities);
}

// 0x5650E0
void CWorld::FindObjectsIntersectingCubeSectorList(CPtrList& ptrList, CVector const& cornerA, CVector const& cornerB, int16* outCount, int16 maxCount, CEntity** outEntities) {
    plugin::Call<0x5650E0, CPtrList&, CVector const&, CVector const&, int16*, int16, CEntity**>(ptrList, cornerA, cornerB, outCount, maxCount, outEntities);
}

// 0x565200
void CWorld::FindObjectsIntersectingAngledCollisionBoxSectorList(CPtrList& ptrList, CBox const& box, CMatrix const& transform, CVector const& point, int16* outCount, int16 maxCount, CEntity** outEntities) {
    plugin::Call<0x565200, CPtrList&, CBox const&, CMatrix const&, CVector const&, int16*, int16, CEntity**>(ptrList, box, transform, point, outCount, maxCount, outEntities);
}

// 0x565300
void CWorld::FindMissionEntitiesIntersectingCubeSectorList(CPtrList& ptrList, CVector const& cornerA, CVector const& cornerB, int16* outCount, int16 maxCount, CEntity** outEntities, bool vehiclesList, bool pedsList, bool objectsList) {
    plugin::Call<0x565300, CPtrList&, CVector const&, CVector const&, int16*, int16, CEntity**, bool, bool, bool>(ptrList, cornerA, cornerB, outCount, maxCount, outEntities, vehiclesList, pedsList, objectsList);
}

// 0x565450
void CWorld::FindNearestObjectOfTypeSectorList(int32 modelId, CPtrList& ptrList, CVector const& point, float radius, bool b2D, CEntity** outEntities, float* outDistance) {
    plugin::Call<0x565450, int32, CPtrList&, CVector const&, float, bool, CEntity**, float*>(modelId, ptrList, point, radius, b2D, outEntities, outDistance);
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
#ifdef USE_DEFAULT_FUNCTIONS
    plugin::Call<0x565CB0>();
#else
    for (int32 i = CPools::ms_pPedPool->GetSize(); i; i--)
    {
        CPed* pPed = CPools::ms_pPedPool->GetAt(i - 1);
        if (!pPed)
            continue;
        const CVector& vecPedPos = pPed->GetPosition();
        if (vecPedPos.z > -100.0f)
            continue;
        if (!pPed->IsCreatedBy(ePedCreatedBy::PED_GAME) || pPed->IsPlayer())
        {
            CNodeAddress pathNodeAddress;
            ThePaths.FindNodeClosestToCoors(&pathNodeAddress, vecPedPos.x, vecPedPos.y, vecPedPos.z, 1, 1000000.0f, 0, 0, 0, 0, 0);
            if (pathNodeAddress.m_wAreaId != -1)
            {
                CVector pathNodePos = ThePaths.GetPathNode(pathNodeAddress)->GetNodeCoors();
                pathNodePos.z += 2.0f;
                pPed->Teleport(pathNodePos, false); 
            }
            else
                pPed->SetPosn(vecPedPos.x, vecPedPos.y, -95);
            pPed->ResetMoveSpeed();
        }
        else
            CPopulation::RemovePed(pPed);
    }
#endif
}

// 0x565E80
void CWorld::RemoveFallenCars() {

#ifdef USE_DEFAULT_FUNCTIONS
    plugin::Call<0x565E80>();
#else
    for (int32 i = CPools::ms_pVehiclePool->GetSize(); i; i--)
    {
        CVehicle* pVeh = CPools::ms_pVehiclePool->GetAt(i - 1);
        if (!pVeh)
            continue;

        const CVector& vecPos = pVeh->GetPosition();
        if (vecPos.z > -100.0f)
            continue;

        const auto ShouldWeKeepIt = [pVeh]() {
            if (pVeh->IsCreatedBy(eVehicleCreatedBy::MISSION_VEHICLE) && !pVeh->physicalFlags.bDestroyed)
                return true;

            if (pVeh == FindPlayerVehicle(-1, false))
                return true;

            return pVeh->m_pDriver && pVeh->m_pDriver->IsPlayer();
        };

        if (ShouldWeKeepIt())
        {
            CNodeAddress pathNodeAddress;
            ThePaths.FindNodeClosestToCoors(&pathNodeAddress, vecPos.x, vecPos.y, vecPos.z, 1, 1000000.0f, 0, 0, 0, 0, 0);
            if (pathNodeAddress.m_wAreaId != -1)
            {
                const auto pathNodePos = ThePaths.GetPathNode(pathNodeAddress)->GetNodeCoors();
                pVeh->Teleport(pathNodePos + CVector(0, 0, 3), true);
            }
            else
                pVeh->Teleport(CVector(vecPos.x, vecPos.y, 0), true);
            pVeh->ResetMoveSpeed();
        }
        else
        {
            if (!pVeh->IsCreatedBy(eVehicleCreatedBy::RANDOM_VEHICLE))
                if (!pVeh->IsCreatedBy(eVehicleCreatedBy::PARKED_VEHICLE))
                    continue;
            pVeh->Remove();
            if (pVeh->IsPhysical())
                pVeh->RemoveFromMovingList();

            delete pVeh;
        }
    }
#endif
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
}

// 0x566EE0
bool CWorld::ProcessLineOfSightSectorList(CPtrList& ptrList, CColLine const& colLine, CColPoint& outColPoint, float& maxTouchDistance, CEntity*& outEntity, bool doSeeThroughCheck, bool doIgnoreCameraCheck, bool doShootThroughCheck) {
    return plugin::CallAndReturn<bool, 0x566EE0, CPtrList&, CColLine const&, CColPoint&, float&, CEntity*&, bool, bool, bool>(ptrList, colLine, outColPoint, maxTouchDistance, outEntity, doSeeThroughCheck, doIgnoreCameraCheck, doShootThroughCheck);
}

// 0x5674E0
bool CWorld::ProcessVerticalLine(CVector const& origin, float distance, CColPoint& outColPoint, CEntity*& outEntity, bool buildings, bool vehicles, bool peds, bool objects, bool dummies, bool doSeeThroughCheck, CStoredCollPoly* outCollPoly) {
    return plugin::CallAndReturn<bool, 0x5674E0, CVector const&, float, CColPoint&, CEntity*&, bool, bool, bool, bool, bool, bool, CStoredCollPoly*>(origin, distance, outColPoint, outEntity, buildings, vehicles, peds, objects, dummies, doSeeThroughCheck, outCollPoly);
}

// 0x567620
bool CWorld::ProcessVerticalLine_FillGlobeColPoints(CVector const& origin, float distance, CEntity*& outEntity, bool buildings, bool vehicles, bool peds, bool objects, bool dummies, bool doSeeThroughCheck, CStoredCollPoly* outCollPoly) {
    return plugin::CallAndReturn<bool, 0x567620, CVector const&, float, CEntity*&, bool, bool, bool, bool, bool, bool, CStoredCollPoly*>(origin, distance, outEntity, buildings, vehicles, peds, objects, dummies, doSeeThroughCheck, outCollPoly);
}

// 0x567750
void CWorld::TriggerExplosionSectorList(CPtrList& ptrList, CVector const& point, float radius, float visibleDistance, CEntity* victim, CEntity* creator, bool processVehicleBombTimer, float damage) {
    plugin::Call<0x567750, CPtrList&, CVector const&, float, float, CEntity*, CEntity*, bool, float>(ptrList, point, radius, visibleDistance, victim, creator, processVehicleBombTimer, damage);
}

// 0x5684A0
void CWorld::Process() {
    plugin::Call<0x5684A0>();
}

// 0x568AD0
bool CWorld::GetIsLineOfSightSectorClear(CSector& sector, CRepeatSector& repeatSector, CColLine const& colLine, bool buildings, bool vehicles, bool peds, bool objects, bool dummies, bool doSeeThroughCheck, bool doIgnoreCameraCheck) {
    return plugin::CallAndReturn<bool, 0x568AD0, CSector&, CRepeatSector&, CColLine const&, bool, bool, bool, bool, bool, bool, bool>(sector, repeatSector, colLine, buildings, vehicles, peds, objects, dummies, doSeeThroughCheck, doIgnoreCameraCheck);
}

// 0x568B80
void CWorld::FindObjectsKindaColliding(CVector const& point, float radius, bool b2D, int16* outCount, int16 maxCount, CEntity** outEntities, bool buildings, bool vehicles, bool peds, bool objects, bool dummies) {
    plugin::Call<0x568B80, CVector const&, float, bool, int16*, int16, CEntity**, bool, bool, bool, bool, bool>(point, radius, b2D, outCount, maxCount, outEntities, buildings, vehicles, peds, objects, dummies);
}

// 0x568DD0
void CWorld::FindObjectsIntersectingCube(CVector const& cornerA, CVector const& cornerB, int16* outCount, int16 maxCount, CEntity** outEntities, bool buildings, bool vehicles, bool peds, bool objects, bool dummies) {
    plugin::Call<0x568DD0, CVector const&, CVector const&, int16*, int16, CEntity**, bool, bool, bool, bool, bool>(cornerA, cornerB, outCount, maxCount, outEntities, buildings, vehicles, peds, objects, dummies);
}

// 0x568FF0
void CWorld::FindObjectsIntersectingAngledCollisionBox(CBox const& box, CMatrix const& transform, CVector const& point, float x1, float y1, float x2, float y2, int16* outCount, int16 maxCount, CEntity** outEntities, bool buildings, bool vehicles, bool peds, bool objects, bool dummies) {
    plugin::Call<0x568FF0, CBox const&, CMatrix const&, CVector const&, float, float, float, float, int16*, int16, CEntity**, bool, bool, bool, bool, bool>(box, transform, point, x1, y1, x2, y2, outCount, maxCount, outEntities, buildings, vehicles, peds, objects, dummies);
}

// 0x569240
void CWorld::FindMissionEntitiesIntersectingCube(CVector const& cornerA, CVector const& cornerB, int16* outCount, int16 maxCount, CEntity** outEntities, bool vehicles, bool peds, bool objects) {
    plugin::Call<0x569240, CVector const&, CVector const&, int16*, int16, CEntity**, bool, bool, bool>(cornerA, cornerB, outCount, maxCount, outEntities, vehicles, peds, objects);
}

// 0x5693F0
void CWorld::FindNearestObjectOfType(int32 modelId, CVector const& point, float radius, bool b2D, bool buildings, bool vehicles, bool peds, bool objects, bool dummies) {
    plugin::Call<0x5693F0, int32, CVector const&, float, bool, bool, bool, bool, bool, bool>(modelId, point, radius, b2D, buildings, vehicles, peds, objects, dummies);
}

// 0x569660
float CWorld::FindGroundZForCoord(float x, float y) {
    return plugin::CallAndReturn<float, 0x569660, float, float>(x, y);
}

// 0x5696C0
float CWorld::FindGroundZFor3DCoord(float x, float y, float z, bool* outResult, CEntity** outEntity) {
    return plugin::CallAndReturn<float, 0x5696C0, float, float, float, bool*, CEntity**>(x, y, z, outResult, outEntity);
}

// 0x569750
float CWorld::FindRoofZFor3DCoord(float x, float y, float z, bool* outResult) {
    return plugin::CallAndReturn<float, 0x569750, float, float, float, bool*>(x, y, z, outResult);
}

// 0x5697F0
float CWorld::FindLowestZForCoord(float x, float y) {
    return plugin::CallAndReturn<float, 0x5697F0, float, float>(x, y);
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
void CWorld::ClearExcitingStuffFromArea(CVector const& point, float radius, uint8 bRemoveProjectilesAndShadows) {
    plugin::Call<0x56A0D0, CVector const&, float, uint8>(point, radius, bRemoveProjectilesAndShadows);
}

// 0x56A490
bool CWorld::GetIsLineOfSightClear(CVector const& origin, CVector const& target, bool buildings, bool vehicles, bool peds, bool objects, bool dummies, bool doSeeThroughCheck, bool doCameraIgnoreCheck) {
    return plugin::CallAndReturn<bool, 0x56A490, CVector const&, CVector const&, bool, bool, bool, bool, bool, bool, bool>(origin, target, buildings, vehicles, peds, objects, dummies, doSeeThroughCheck, doCameraIgnoreCheck);
}

// 0x56B5E0
bool CWorld::ProcessLineOfSightSector(CSector& sector, CRepeatSector& repeatSector, CColLine const& colLine, CColPoint& outColPoint, float& maxTouchDistance, CEntity*& outEntity, bool buildings, bool vehicles, bool peds, bool objects, bool dummies, bool doSeeThroughCheck, bool doCameraIgnoreCheck, bool doShootThroughCheck) {
    return plugin::CallAndReturn<bool, 0x56B5E0, CSector&, CRepeatSector&, CColLine const&, CColPoint&, float&, CEntity*&, bool, bool, bool, bool, bool, bool, bool, bool>(sector, repeatSector, colLine, outColPoint, maxTouchDistance, outEntity, buildings, vehicles, peds, objects, dummies, doSeeThroughCheck, doCameraIgnoreCheck, doShootThroughCheck);
}

// 0x56B790
void CWorld::TriggerExplosion(CVector const& point, float radius, float visibleDistance, CEntity* victim, CEntity* creator, bool processVehicleBombTimer, float damage) {
    plugin::Call<0x56B790, CVector const&, float, float, CEntity*, CEntity*, bool, float>(point, radius, visibleDistance, victim, creator, processVehicleBombTimer, damage);
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
bool CWorld::ProcessLineOfSight(CVector const& origin, CVector const& target, CColPoint& outColPoint, CEntity*& outEntity, bool buildings, bool vehicles, bool peds, bool objects, bool dummies, bool doSeeThroughCheck, bool doCameraIgnoreCheck, bool doShootThroughCheck) {
    return plugin::CallAndReturn<bool, 0x56BA00, CVector const&, CVector const&, CColPoint&, CEntity*&, bool, bool, bool, bool, bool, bool, bool, bool>(origin, target, outColPoint, outEntity, buildings, vehicles, peds, objects, dummies, doSeeThroughCheck, doCameraIgnoreCheck, doShootThroughCheck);
}

void CWorld::IncrementCurrentScanCode()
{
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
    //return plugin::CallAndReturn<CPtrListSingleLink*, 0x4072C0, int32, int32>(x, y);
}

// 0x4072E0
void SetNextScanCode() {
    plugin::Call<0x4072E0>();
}

double ScaleLighting(uint8 lighting, float fScale)
{
    return plugin::CallAndReturn<double, 0x59F0C0, uint8, float> (lighting, fScale);
}
