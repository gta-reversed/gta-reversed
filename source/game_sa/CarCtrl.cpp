/*
    Plugin-SDK (Grand Theft Auto San Andreas) source file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/

#include "StdInc.h"

uint32& CCarCtrl::NumLawEnforcerCars = *(uint32*)0x969098;
uint32& CCarCtrl::NumParkedCars = *(uint32*)0x9690A0;
uint32& CCarCtrl::NumAmbulancesOnDuty = *(uint32*)0x9690A8;
uint32& CCarCtrl::NumFireTrucksOnDuty = *(uint32*)0x9690AC;
uint32& CCarCtrl::MaxNumberOfCarsInUse = *(uint32*)0x8A5B24;
float& CCarCtrl::CarDensityMultiplier = *(float*)0x8A5B20;
int32& CCarCtrl::NumRandomCars = *(int32*)0x969094;
int32& CCarCtrl::NumMissionCars = *(int32*)0x96909C;
int32& CCarCtrl::NumPermanentVehicles = *(int32*)0x9690A4;
int32& CCarCtrl::LastTimeAmbulanceCreated = *(int32*)0x9690B0;
int32& CCarCtrl::LastTimeFireTruckCreated = *(int32*)0x9690B4;
bool& CCarCtrl::bAllowEmergencyServicesToBeCreated = *(bool*)0x8A5B28;
bool& CCarCtrl::bCarsGeneratedAroundCamera = *(bool*)0x9690C1;
int8& CCarCtrl::CountDownToCarsAtStart = *(int8*)0x9690C0;
float& CCarCtrl::flt_9690BC = *(float*)0x9690BC;
int32& CCarCtrl::SequenceElements = *(int32*)0x969078;
int32& CCarCtrl::SequenceRandomOffset = *(int32*)0x969074;
bool& CCarCtrl::bSequenceOtherWay = *(bool*)0x969070;
int32& CCarCtrl::dword_9690B8 = *(int32*)0x9690B8;

CVehicle* (&apCarsToKeep)[2] = *(CVehicle*(*)[2])0x969084;
uint32(&aCarsToKeepTime)[2] = *(uint32(*)[2])0x96907C;

void CCarCtrl::InjectHooks()
{
    using namespace ReversibleHooks;
    Install("CCarCtrl", "ChooseGangCarModel", 0x421A40, &CCarCtrl::ChooseGangCarModel, false, 7);
    Install("CCarCtrl", "ChoosePoliceCarModel", 0x421980, &CCarCtrl::ChoosePoliceCarModel, false, 7);
    Install("CCarCtrl", "CreateCarForScript", 0x431F80, &CCarCtrl::CreateCarForScript);
    Install("CCarCtrl", "ChooseBoatModel", 0x421970, &CCarCtrl::ChooseBoatModel);
    Install("CCarCtrl", "ChooseCarModelToLoad", 0x421900, &CCarCtrl::ChooseCarModelToLoad);
    Install("CCarCtrl", "GetNewVehicleDependingOnCarModel", 0x421440, &CCarCtrl::GetNewVehicleDependingOnCarModel);
    Install("CCarCtrl", "Init", 0x4212E0, &CCarCtrl::Init);
    Install("CCarCtrl", "ReInit", 0x4213B0, &CCarCtrl::ReInit);
    Install("CCarCtrl", "InitSequence", 0x421740, &CCarCtrl::InitSequence);
    Install("CCarCtrl", "IsAnyoneParking", 0x42C250, &CCarCtrl::IsAnyoneParking);
    Install("CCarCtrl", "IsThisVehicleInteresting", 0x423EA0, &CCarCtrl::IsThisVehicleInteresting);
    Install("CCarCtrl", "JoinCarWithRoadAccordingToMission", 0x432CB0, &CCarCtrl::JoinCarWithRoadAccordingToMission);
    Install("CCarCtrl", "PossiblyFireHSMissile", 0x429600, &CCarCtrl::PossiblyFireHSMissile);
    Install("CCarCtrl", "PruneVehiclesOfInterest", 0x423F10, &CCarCtrl::PruneVehiclesOfInterest);
    Install("CCarCtrl", "RemoveCarsIfThePoolGetsFull", 0x4322B0, &CCarCtrl::RemoveCarsIfThePoolGetsFull);
    Install("CCarCtrl", "RemoveDistantCars", 0x42CD10, &CCarCtrl::RemoveDistantCars);
    Install("CCarCtrl", "RemoveFromInterestingVehicleList", 0x423ED0, &CCarCtrl::RemoveFromInterestingVehicleList);
    Install("CCarCtrl", "ScriptGenerateOneEmergencyServicesCar", 0x42FBC0, &CCarCtrl::ScriptGenerateOneEmergencyServicesCar);
    Install("CCarCtrl", "SlowCarDownForObject", 0x426220, &CCarCtrl::SlowCarDownForObject);
    Install("CCarCtrl", "SlowCarOnRailsDownForTrafficAndLights", 0x434790, &CCarCtrl::SlowCarOnRailsDownForTrafficAndLights);
}

// 0x421970
int32 CCarCtrl::ChooseBoatModel() {
    return CPopulation::m_LoadedBoats.PickLeastUsedModel(1);
}

// 0x421900
int32 CCarCtrl::ChooseCarModelToLoad(int32 arg1) {
    for (auto i = 0; i < 16; i++) {
        const short model = CPopulation::m_CarGroups[i][CGeneral::GetRandomNumberInRange(0, CPopulation::m_nNumCarsInGroup[i])];
        if (!CStreaming::IsModelLoaded(model))
            return model;
    }
    return -1;
}

int32 CCarCtrl::ChooseGangCarModel(int32 loadedCarGroupId)
{
    return CPopulation::PickGangCar(loadedCarGroupId);
}

// 0x424CE0
int32 CCarCtrl::ChooseModel(int32* arg1) {
    return plugin::CallAndReturn<int32, 0x424CE0, int32*>(arg1);
}

int32 CCarCtrl::ChoosePoliceCarModel(uint32 ignoreLvpd1Model) {
    CWanted* playerWanted = FindPlayerWanted(-1);
    if (playerWanted->AreSwatRequired() 
        && CStreaming::GetInfo(MODEL_ENFORCER).IsLoaded() 
        && CStreaming::GetInfo(MODEL_SWAT).IsLoaded())
    {
        if (CGeneral::GetRandomNumberInRange(0, 3) == 2)
            return MODEL_ENFORCER; 
    }
    else
    {
        if (playerWanted->AreFbiRequired() 
            && CStreaming::GetInfo(MODEL_FBIRANCH).IsLoaded()
            && CStreaming::GetInfo(MODEL_FBI).IsLoaded())
            return MODEL_FBIRANCH; 
        if (playerWanted->AreArmyRequired() 
            && CStreaming::GetInfo(MODEL_RHINO).IsLoaded()
            && CStreaming::GetInfo(MODEL_BARRACKS).IsLoaded()
            && CStreaming::GetInfo(MODEL_ARMY).IsLoaded())
            return (rand() < 0x3FFF) + MODEL_RHINO;
    }
    return CStreaming::GetDefaultCopCarModel(ignoreLvpd1Model);
}

// 0x423F00
void CCarCtrl::ClearInterestingVehicleList() {
    plugin::Call<0x423F00>();
}

// 0x422760
void CCarCtrl::ClipTargetOrientationToLink(CVehicle* vehicle, CCarPathLinkAddress arg2, int8 arg3, float* arg4, float arg5, float arg6) {
    plugin::Call<0x422760, CVehicle*, CCarPathLinkAddress, int8, float*, float, float>(vehicle, arg2, arg3, arg4, arg5, arg6);
}

// 0x431F80
CVehicle* CCarCtrl::CreateCarForScript(int32 modelid, CVector posn, bool doMissionCleanup) {
    if (CModelInfo::IsBoatModel(modelid))
    {
        auto* pBoat = new CBoat(modelid, eVehicleCreatedBy::MISSION_VEHICLE);
        if (posn.z <= -100.0F)
            posn.z = CWorld::FindGroundZForCoord(posn.x, posn.y);

        posn.z += pBoat->GetDistanceFromCentreOfMassToBaseOfModel();
        pBoat->SetPosn(posn);

        CTheScripts::ClearSpaceForMissionEntity(posn, pBoat);
        pBoat->vehicleFlags.bEngineOn = false;
        pBoat->vehicleFlags.bIsLocked = true;
        pBoat->m_nStatus = eEntityStatus::STATUS_ABANDONED;
        CCarCtrl::JoinCarWithRoadSystem(pBoat);

        pBoat->m_autoPilot.m_nCarMission = eCarMission::MISSION_NONE;
        pBoat->m_autoPilot.m_nTempAction = 0;
        pBoat->m_autoPilot.m_speed = 20.0F;
        pBoat->m_autoPilot.m_nCruiseSpeed = 20;

        if (doMissionCleanup)
            pBoat->m_bIsStaticWaitingForCollision = true;

        pBoat->m_autoPilot.movementFlags.bIsStopped = true;
        CWorld::Add(pBoat);

        if (doMissionCleanup)
            CTheScripts::MissionCleanUp.AddEntityToList(CPools::ms_pVehiclePool->GetRef(pBoat), MissionCleanUpEntityType::MISSION_CLEANUP_ENTITY_TYPE_VEHICLE);

        return pBoat;
    }

    auto* vehicle = CCarCtrl::GetNewVehicleDependingOnCarModel(modelid, eVehicleCreatedBy::MISSION_VEHICLE);
    if (posn.z <= -100.0F)
        posn.z = CWorld::FindGroundZForCoord(posn.x, posn.y);

    posn.z += vehicle->GetDistanceFromCentreOfMassToBaseOfModel();
    vehicle->SetPosn(posn);

    if (!doMissionCleanup)
    {
        if (vehicle->IsAutomobile())
            vehicle->AsAutomobile()->PlaceOnRoadProperly();
        else if (vehicle->IsBike())
            vehicle->AsBike()->PlaceOnRoadProperly();
    }

    if (vehicle->IsTrain())
        vehicle->AsTrain()->trainFlags.bNotOnARailRoad = true;

    CTheScripts::ClearSpaceForMissionEntity(posn, vehicle);
    vehicle->vehicleFlags.bIsLocked = true;
    vehicle->m_nStatus = eEntityStatus::STATUS_ABANDONED;
    CCarCtrl::JoinCarWithRoadSystem(vehicle);
    vehicle->vehicleFlags.bEngineOn = false;
    vehicle->vehicleFlags.bHasBeenOwnedByPlayer = true;

    vehicle->m_autoPilot.m_nCarMission = eCarMission::MISSION_NONE;
    vehicle->m_autoPilot.m_nTempAction = 0;
    vehicle->m_autoPilot.m_nCarDrivingStyle = eCarDrivingStyle::DRIVINGSTYLE_STOP_FOR_CARS;
    vehicle->m_autoPilot.m_speed = 13.0F;
    vehicle->m_autoPilot.m_nCruiseSpeed = 13;
    vehicle->m_autoPilot.m_nCurrentLane = 0;
    vehicle->m_autoPilot.m_nNextLane = 0;

    if (doMissionCleanup)
        vehicle->m_bIsStaticWaitingForCollision = true;

    CWorld::Add(vehicle);
    if (doMissionCleanup)
        CTheScripts::MissionCleanUp.AddEntityToList(CPools::ms_pVehiclePool->GetRef(vehicle), MissionCleanUpEntityType::MISSION_CLEANUP_ENTITY_TYPE_VEHICLE);

    if (vehicle->IsRoadVehicle())
        vehicle->m_autoPilot.movementFlags.bIsStopped = true;

    return vehicle;
}

// 0x42C740
bool CCarCtrl::CreateConvoy(CVehicle* vehicle, int32 arg2) {
    return plugin::CallAndReturn<bool, 0x42C740, CVehicle*, int32>(vehicle, arg2);
}

// 0x42C2B0
bool CCarCtrl::CreatePoliceChase(CVehicle* vehicle, int32 arg2, CNodeAddress NodeAddress) {
    return plugin::CallAndReturn<bool, 0x42C2B0, CVehicle*, int32, CNodeAddress>(vehicle, arg2, NodeAddress);
}

// 0x428040
bool CCarCtrl::DealWithBend_Racing(CVehicle* vehicle, CCarPathLinkAddress LinkAddress1, CCarPathLinkAddress LinkAddress2, CCarPathLinkAddress LinkAddress3, CCarPathLinkAddress LinkAddress4, char arg6, char arg7, char arg8, char arg9, float arg10, float* arg11, float* arg12, float* arg13, float* arg14, CVector* pos) {
    return plugin::CallAndReturn<bool, 0x428040, CVehicle*, CCarPathLinkAddress, CCarPathLinkAddress, CCarPathLinkAddress, CCarPathLinkAddress, int8, int8, int8, int8, float, float*, float*, float*, float*, CVector*>(vehicle, LinkAddress1, LinkAddress2, LinkAddress3, LinkAddress4, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, pos);
}

// 0x42EC90
void CCarCtrl::DragCarToPoint(CVehicle* vehicle, CVector* pVector) {
    plugin::Call<0x42EC90, CVehicle*, CVector*>(vehicle, pVector);
}

// 0x4325C0
float CCarCtrl::FindAngleToWeaveThroughTraffic(CVehicle* vehicle, CPhysical* pPhysical, float arg3, float arg4, float arg5) {
    return plugin::CallAndReturn<float, 0x4325C0, CVehicle*, CPhysical*, float, float, float>(vehicle, pPhysical, arg3, arg4, arg5);
}

// 0x4226F0
void CCarCtrl::FindIntersection2Lines(float arg1, float arg2, float arg3, float arg4, float arg5, float arg6, float arg7, float arg8, float* arg9, float* arg10) {
    plugin::Call<0x4226F0, float, float, float, float, float, float, float, float, float*, float*>(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);
}

// 0x42B470
void CCarCtrl::FindLinksToGoWithTheseNodes(CVehicle* vehicle) {
    plugin::Call<0x42B470, CVehicle*>(vehicle);
}

// 0x434400
float CCarCtrl::FindMaximumSpeedForThisCarInTraffic(CVehicle* vehicle) {
    return plugin::CallAndReturn<float, 0x434400, CVehicle*>(vehicle);
}

// 0x42BD20
void CCarCtrl::FindNodesThisCarIsNearestTo(CVehicle* vehicle, CNodeAddress& NodeAddress1, CNodeAddress& NodeAddress2) {
    plugin::Call<0x42BD20, CVehicle*, CNodeAddress&, CNodeAddress&>(vehicle, NodeAddress1, NodeAddress2);
}

// 0x422090
int8 CCarCtrl::FindPathDirection(CNodeAddress NodeAddress1, CNodeAddress NodeAddress2, CNodeAddress NodeAddress3, bool* arg4) {
    return plugin::CallAndReturn<int8, 0x422090, CNodeAddress, CNodeAddress, CNodeAddress, bool*>(NodeAddress1, NodeAddress2, NodeAddress3, arg4);
}

// 0x422620
float CCarCtrl::FindPercDependingOnDistToLink(CVehicle* vehicle, CCarPathLinkAddress LinkAddress) {
    return plugin::CallAndReturn<float, 0x422620, CVehicle*, CCarPathLinkAddress>(vehicle, LinkAddress);
}

// 0x421770
int32 CCarCtrl::FindSequenceElement(int32 arg1) {
    return plugin::CallAndReturn<int32, 0x421770, int32>(arg1);
}

// 0x4224E0
float CCarCtrl::FindSpeedMultiplier(float arg1, float arg2, float arg3, float arg4) {
    return plugin::CallAndReturn<float, 0x4224E0, float, float, float, float>(arg1, arg2, arg3, arg4);
}

// 0x424130
float CCarCtrl::FindSpeedMultiplierWithSpeedFromNodes(int8 arg1) {
    return plugin::CallAndReturn<float, 0x424130, int8>(arg1);
}

float CCarCtrl::FindGhostRoadHeight(CVehicle* vehicle) {
    return plugin::CallAndReturn<float, 0x422370, CVehicle*>(vehicle);
}

// 0x42B270
void CCarCtrl::FireHeliRocketsAtTarget(CAutomobile* pEntityLauncher, CEntity* pEntity) {
    plugin::Call<0x42B270, CAutomobile*, CEntity*>(pEntityLauncher, pEntity);
}

// 0x429A70
void CCarCtrl::FlyAIHeliInCertainDirection(CHeli* pHeli, float arg2, float arg3, bool arg4) {
    plugin::Call<0x429A70, CHeli*, float, float, bool>(pHeli, arg2, arg3, arg4);
}

// 0x423940
void CCarCtrl::FlyAIHeliToTarget_FixedOrientation(CHeli* pHeli, float Orientation, CVector posn) {
    plugin::Call<0x423940, CHeli*, float, CVector>(pHeli, Orientation, posn);
}

// 0x423000
void CCarCtrl::FlyAIPlaneInCertainDirection(CPlane* pPlane) {
    plugin::Call<0x423000, CPlane*>(pPlane);
}

// 0x424210
bool CCarCtrl::GenerateCarCreationCoors2(CVector posn, float radius, float arg3, float arg4, bool arg5, float arg6, float arg7, CVector* pOrigin, CNodeAddress* pNodeAddress1, CNodeAddress* pNodeAddress12, float* arg11, bool arg12, bool arg13) {
    return plugin::CallAndReturn<bool, 0x424210, CVector, float, float, float, bool, float, float, CVector*, CNodeAddress*, CNodeAddress*, float*, bool, bool>(posn, radius, arg3, arg4, arg5, arg6, arg7, pOrigin, pNodeAddress1, pNodeAddress12, arg11, arg12, arg13);
}

// 0x42F9C0
void CCarCtrl::GenerateEmergencyServicesCar() {
    plugin::Call<0x42F9C0>();
}

// 0x42B7D0
CAutomobile* CCarCtrl::GenerateOneEmergencyServicesCar(uint32 modelId, CVector posn) {
    return plugin::CallAndReturn<CAutomobile*, 0x42B7D0, uint32, CVector>(modelId, posn);
}

// 0x430050
void CCarCtrl::GenerateOneRandomCar() {
    plugin::Call<0x430050>();
}

// 0x4341C0
void CCarCtrl::GenerateRandomCars() {
    plugin::Call<0x4341C0>();
}

// 0x42F3C0
void CCarCtrl::GetAIHeliToAttackPlayer(CAutomobile* pAutomobile) {
    plugin::Call<0x42F3C0, CAutomobile*>(pAutomobile);
}

// 0x42A730
void CCarCtrl::GetAIHeliToFlyInDirection(CAutomobile* pAutomobile) {
    plugin::Call<0x42A730, CAutomobile*>(pAutomobile);
}

// 0x429780
void CCarCtrl::GetAIPlaneToAttackPlayer(CAutomobile* pAutomobile) {
    plugin::Call<0x429780, CAutomobile*>(pAutomobile);
}

// 0x429890
void CCarCtrl::GetAIPlaneToDoDogFight(CAutomobile* pAutomobile) {
    plugin::Call<0x429890, CAutomobile*>(pAutomobile);
}

// 0x42F370
void CCarCtrl::GetAIPlaneToDoDogFightAgainstPlayer(CAutomobile* pAutomobile) {
    plugin::Call<0x42F370, CAutomobile*>(pAutomobile);
}

// 0x421440
CVehicle* CCarCtrl::GetNewVehicleDependingOnCarModel(int32 modelId, uint8 createdBy) {
    return plugin::CallAndReturn<CVehicle*, 0x421440, int32, uint8>(modelId, createdBy);
    /*
    switch (CModelInfo::GetModelInfo(modelId)->AsVehicleModelInfoPtr()->m_nVehicleType) {
    case eVehicleType::VEHICLE_MTRUCK:
        return new CMonsterTruck(modelId, createdBy);
    case eVehicleType::VEHICLE_QUAD:
        return new CQuadBike(modelId, createdBy);
    case eVehicleType::VEHICLE_HELI:
        return new CHeli(modelId, createdBy);
    case eVehicleType::VEHICLE_PLANE:
        return new CPlane(modelId, createdBy);
    case eVehicleType::VEHICLE_BOAT:
        return new CBoat(modelId, createdBy);
    case eVehicleType::VEHICLE_TRAIN:
        return new CTrain(modelId, createdBy);
    case eVehicleType::VEHICLE_BIKE:
        return new CBike(modelId, createdBy);
    case eVehicleType::VEHICLE_BMX:
        return new CBmx(modelId, createdBy);
    case eVehicleType::VEHICLE_TRAILER:
        return new CTrailer(modelId, createdBy);
    case eVehicleType::VEHICLE_AUTOMOBILE:
        return new CAutomobile(modelId, createdBy, 1);
    }
    return nullptr;
    */
}

// 0x4212E0
void CCarCtrl::Init() {
    CarDensityMultiplier = 1.0f;
    NumRandomCars = 0;
    NumLawEnforcerCars = 0;
    NumMissionCars = 0;
    NumParkedCars = 0;
    NumPermanentVehicles = 0;
    NumAmbulancesOnDuty = 0;
    NumFireTrucksOnDuty = 0;
    LastTimeAmbulanceCreated = 0;
    LastTimeFireTruckCreated = 0;
    bAllowEmergencyServicesToBeCreated = true;
    bCarsGeneratedAroundCamera = false;
    CountDownToCarsAtStart = 2;
    for (CVehicle*& v : apCarsToKeep)
        v = nullptr; /* null array out */
    flt_9690BC = CGeneral::GetRandomNumberInRange(600.0f, 1200.0f);
    for (auto& group : CPopulation::m_LoadedGangCars) {
        group.Clear();
    }
    CPopulation::m_AppropriateLoadedCars.Clear();
    CPopulation::m_InAppropriateLoadedCars.Clear();
    CPopulation::m_LoadedBoats.Clear();
}

// 0x421740
void CCarCtrl::InitSequence(int32 numSequenceElements) {
    SequenceElements = numSequenceElements;
    SequenceRandomOffset = rand() % numSequenceElements;
    bSequenceOtherWay = (rand() >> 4) & 1; // >> 4 is div. by 16, not sure about `& 1` part though
}

// 0x42C250
bool CCarCtrl::IsAnyoneParking() {
    for (auto i = 0; i < CPools::ms_pVehiclePool->GetSize(); i++) {
        if (auto vehicle = CPools::ms_pVehiclePool->GetAt(i)) {
            switch (vehicle->m_autoPilot.m_nCarMission) {
            case eCarMission::MISSION_PARK_PARALLEL_0:
            case eCarMission::MISSION_PARK_PARALLEL_1:
            case eCarMission::MISSION_PARK_PERPENDICULAR_0:
            case eCarMission::MISSION_PARK_PERPENDICULAR_1:
                return true;
            }
        }
    }
    return false;
}

// 0x42DAB0
bool CCarCtrl::IsThisAnAppropriateNode(CVehicle* vehicle, CNodeAddress NodeAddress1, CNodeAddress NodeAddress2, CNodeAddress NodeAddress3, bool arg5) {
    return plugin::CallAndReturn<bool, 0x42DAB0, CVehicle*, CNodeAddress, CNodeAddress, CNodeAddress, bool>(vehicle, NodeAddress1, NodeAddress2, NodeAddress3, arg5);
}

// 0x423EA0
bool CCarCtrl::IsThisVehicleInteresting(CVehicle* vehicle) {
    for (auto& vehicle : apCarsToKeep) {
        if (vehicle == vehicle) {
            return true;
        }
    }
    return false;
}

// 0x432CB0
void CCarCtrl::JoinCarWithRoadAccordingToMission(CVehicle* vehicle) {
    switch (vehicle->m_autoPilot.m_nCarMission) {
    case MISSION_NONE:
    case MISSION_CRUISE:
    case MISSION_WAITFORDELETION:
    case MISSION_EMERGENCYVEHICLE_STOP:
    case MISSION_STOP_FOREVER:
    case MISSION_FOLLOW_PRE_RECORDED_PATH:
    case MISSION_PARK_PERPENDICULAR_0:
    case MISSION_PARK_PARALLEL_0:
    case MISSION_PARK_PERPENDICULAR_1:
    case MISSION_PARK_PARALLEL_1:
        return JoinCarWithRoadSystem(vehicle);
    case MISSION_RAMPLAYER_FARAWAY:
    case MISSION_RAMPLAYER_CLOSE:
    case MISSION_BLOCKPLAYER_FARAWAY:
    case MISSION_BLOCKPLAYER_CLOSE:
    case MISSION_BLOCKPLAYER_HANDBRAKESTOP:
    case MISSION_ATTACKPLAYER:
    case MISSION_SLOWLY_DRIVE_TOWARDS_PLAYER_1:
    case MISSION_SLOWLY_DRIVE_TOWARDS_PLAYER_2:
    case MISSION_BLOCKPLAYER_FORWARDANDBACK:
    case MISSION_POLICE_BIKE:
    case MISSION_2C:
    case MISSION_BOAT_CIRCLING_PLAYER: {
        JoinCarWithRoadSystemGotoCoors(vehicle, FindPlayerCoors(-1), true, vehicle->m_vehicleSubType == eVehicleType::VEHICLE_BOAT);
        break;
    }
    case MISSION_GOTOCOORDS:
    case MISSION_GOTOCOORDS_STRAIGHT:
    case MISSION_GOTOCOORDS_ACCURATE:
    case MISSION_GOTOCOORDS_STRAIGHT_ACCURATE:
    case MISSION_GOTOCOORDS_ASTHECROWSWIMS:
    case MISSION_FOLLOW_PATH_RACING: {
        JoinCarWithRoadSystemGotoCoors(vehicle, vehicle->m_autoPilot.m_vecDestinationCoors, true, vehicle->m_vehicleSubType == eVehicleType::VEHICLE_BOAT);
        break;
    }
    case MISSION_RAMCAR_FARAWAY:
    case MISSION_RAMCAR_CLOSE:
    case MISSION_BLOCKCAR_FARAWAY:
    case MISSION_BLOCKCAR_CLOSE:
    case MISSION_BLOCKCAR_HANDBRAKESTOP:
    case MISSION_1B:
    case MISSION_1C:
    case MISSION_GOTO_ESCORTPOINT_0:
    case MISSION_GOTO_ESCORTPOINT_1:
    case MISSION_GOTO_ESCORTPOINT_2:
    case MISSION_GOTO_ESCORTPOINT_3:
    case MISSION_34:
    case MISSION_35:
    case MISSION_36:
    case MISSION_37:
    case MISSION_3C:
    case MISSION_3D:
    case MISSION_41:
    case MISSION_42:
    case MISSION_43:
    case MISSION_44: {
        JoinCarWithRoadSystemGotoCoors(vehicle, vehicle->m_autoPilot.m_pTargetCar->GetPosition(), true, vehicle->m_vehicleSubType == eVehicleType::VEHICLE_BOAT);
        break;
    }
    }
}

// 0x42F5A0
void CCarCtrl::JoinCarWithRoadSystem(CVehicle* vehicle) {
    plugin::Call<0x42F5A0, CVehicle*>(vehicle);
}

// 0x42F870
bool CCarCtrl::JoinCarWithRoadSystemGotoCoors(CVehicle* vehicle, CVector const& posn, bool unused, bool bIsBoat) {
    return plugin::CallAndReturn<bool, 0x42F870, CVehicle*, CVector const&, bool, bool>(vehicle, posn, unused, bIsBoat);
}

// 0x432B10
bool CCarCtrl::PickNextNodeAccordingStrategy(CVehicle* vehicle) {
    return plugin::CallAndReturn<bool, 0x432B10, CVehicle*>(vehicle);
}

// 0x42DE80
void CCarCtrl::PickNextNodeRandomly(CVehicle* vehicle) {
    plugin::Call<0x42DE80, CVehicle*>(vehicle);
}

// 0x426EF0
bool CCarCtrl::PickNextNodeToChaseCar(CVehicle* vehicle, float x_dest, float y_dest, float z_dest) {
    return plugin::CallAndReturn<bool, 0x426EF0, CVehicle*, float, float, float>(vehicle, x_dest, y_dest, z_dest);
}

// 0x427740
bool CCarCtrl::PickNextNodeToFollowPath(CVehicle* vehicle) {
    return plugin::CallAndReturn<bool, 0x427740, CVehicle*>(vehicle);
}

// 0x429600
void CCarCtrl::PossiblyFireHSMissile(CVehicle* pEntityLauncher, CEntity* pTargetEntity) {
    if (!pTargetEntity) {
        return;
    }
    if (CTimer::GetTimeInMS() / 2000u == CTimer::GetPreviousTimeInMS() / 2000u) {
        return;
    }

    const CVector launcherPos = pEntityLauncher->GetPosition();
    const CVector targetPos = pTargetEntity->GetPosition();
    CVector dir = targetPos - launcherPos;
    const float dist = dir.Magnitude();
    if (dist < 160.0f && dist > 30.0f) {
        CMatrix launcherMat = pEntityLauncher->GetMatrix();
        CVector dirNormalized = dir;
        dir.Normalise();
        if (DotProduct(launcherMat.GetForward(), dirNormalized) > 0.8f) {
            CProjectileInfo::AddProjectile(
                pEntityLauncher,
                eWeaponType::WEAPON_ROCKET_HS,
                launcherPos + launcherMat.GetForward() * 4.0f - launcherMat.GetUp() * 3.0f,
                1.0f,
                &pEntityLauncher->GetMatrix().GetForward(),
                pTargetEntity
            );
        }
    }
}

// 0x424F80
void CCarCtrl::PossiblyRemoveVehicle(CVehicle* vehicle) {
    plugin::Call<0x424F80, CVehicle*>(vehicle);
}

// 0x423F10
void CCarCtrl::PruneVehiclesOfInterest() {
    if ((CTimer::GetFrameCounter() & 0x3F) == 19 && FindPlayerCoors(-1).z < 950.0f) {
        for (size_t i = 0; i < std::size(apCarsToKeep); i++) {
            if (apCarsToKeep[i]) {
                if (CTimer::GetTimeInMS() > aCarsToKeepTime[i] + 180000) {
                    apCarsToKeep[i] = nullptr;
                }
            }
        }
    }
}

// 0x4213B0
void CCarCtrl::ReInit() {
    NumRandomCars = 0;
    NumLawEnforcerCars = 0;
    NumMissionCars = 0;
    NumParkedCars = 0;
    NumPermanentVehicles = 0;
    NumAmbulancesOnDuty = 0;
    NumFireTrucksOnDuty = 0;
    dword_9690B8 = 0;
    CarDensityMultiplier = 1.0;
    bAllowEmergencyServicesToBeCreated = true;
    CountDownToCarsAtStart = 2;
    for (CVehicle*& v : apCarsToKeep)
        v = nullptr; /* null array out */
    for (auto& group : CPopulation::m_LoadedGangCars) {
        group.Clear();
    }
    CPopulation::m_AppropriateLoadedCars.Clear();
    CPopulation::m_InAppropriateLoadedCars.Clear();
    CPopulation::m_LoadedBoats.Clear();
}

// 0x42FC40
void CCarCtrl::ReconsiderRoute(CVehicle* vehicle) {
    plugin::Call<0x42FC40, CVehicle*>(vehicle);
}

// 0x423DE0
void CCarCtrl::RegisterVehicleOfInterest(CVehicle* vehicle) {
    plugin::Call<0x423DE0, CVehicle*>(vehicle);
}

// 0x4322B0
void CCarCtrl::RemoveCarsIfThePoolGetsFull() {
    if ((CTimer::GetFrameCounter() & 7) != 3) {
        return;
    }
    if (CPools::ms_pVehiclePool->GetNoOfFreeSpaces() >= 8) {
        return;
    }

    // Find closest deletable vehicle
    const CVector camPos = TheCamera.GetPosition();
    float fClosestDist = std::numeric_limits<float>::max();
    CVehicle* pClosestVeh = nullptr;
    for (auto i = 0; i < CPools::ms_pVehiclePool->GetSize(); i++) {
        if (auto vehicle = CPools::ms_pVehiclePool->GetAt(i)) {
            if (IsThisVehicleInteresting(vehicle))
                continue;
            if (vehicle->vehicleFlags.bIsLocked)
                continue;
            if (!vehicle->CanBeDeleted())
                continue;
            if (CCranes::IsThisCarBeingTargettedByAnyCrane())
                continue;

            const float fCamVehDist = (camPos - vehicle->GetPosition()).Magnitude();
            if (fClosestDist > fCamVehDist) {
                fClosestDist = fCamVehDist;
                pClosestVeh = vehicle;
            }
        }
    }
    if (pClosestVeh) {
        CWorld::Remove(pClosestVeh);
        delete pClosestVeh;
    }
}

// 0x42CD10
void CCarCtrl::RemoveDistantCars() {
    for (auto i = 0; i < CPools::ms_pVehiclePool->GetSize(); i++) {
        if (auto vehicle = CPools::ms_pVehiclePool->GetAt(i)) {
            PossiblyRemoveVehicle(vehicle);
            if (!vehicle->vehicleFlags.bCreateRoadBlockPeds)
                continue;

            CVector centreOfWorld = FindPlayerCentreOfWorld();
            if (DistanceBetweenPoints(centreOfWorld, vehicle->GetPosition()) < 54.5f) {
                CRoadBlocks::GenerateRoadBlockCopsForCar(
                    vehicle,
                    vehicle->m_nPedsPositionForRoadBlock,
                    vehicle->IsLawEnforcementVehicle() ? ePedType::PED_TYPE_COP : ePedType::PED_TYPE_GANG1
                );
            }
        }
    }
}

// 0x423ED0
void CCarCtrl::RemoveFromInterestingVehicleList(CVehicle* vehicle) {
    for (auto& v : apCarsToKeep) {
        if (v == vehicle) {
            v = nullptr;
            break;
        }
    }
}

// 0x42CE40
void CCarCtrl::ScanForPedDanger(CVehicle* vehicle) {
    plugin::Call<0x42CE40, CVehicle*>(vehicle);
}

// 0x42FBC0
bool CCarCtrl::ScriptGenerateOneEmergencyServicesCar(uint32 modelId, CVector posn) {
    if (CStreaming::IsModelLoaded(modelId)) {
        if (auto pAuto = GenerateOneEmergencyServicesCar(modelId, posn)) {
            pAuto->m_autoPilot.m_vecDestinationCoors = posn;
            pAuto->m_autoPilot.m_nCarMission = JoinCarWithRoadSystemGotoCoors(pAuto, posn, false, false) ? MISSION_GOTOCOORDS_STRAIGHT : MISSION_GOTOCOORDS;
            return true;
        }
    }
    return false;
}

// 0x4342A0
void CCarCtrl::SetCoordsOfScriptCar(CVehicle* vehicle, float x, float y, float z, uint8 arg5, uint8 arg6) {
    plugin::Call<0x4342A0, CVehicle*, float, float, float, uint8, uint8>(vehicle, x, y, z, arg5, arg6);
}

// 0x4217C0
void CCarCtrl::SetUpDriverAndPassengersForVehicle(CVehicle* vehicle, int32 arg2, int32 arg3, bool arg4, bool arg5, int32 passengersNum) {
    plugin::Call<0x4217C0, CVehicle*, int32, int32, bool, bool, int32>(vehicle, arg2, arg3, arg4, arg5, passengersNum);
}

// 0x432420
void CCarCtrl::SlowCarDownForCarsSectorList(CPtrList& PtrList, CVehicle* vehicle, float arg3, float arg4, float arg5, float arg6, float* arg7, float arg8) {
    plugin::Call<0x432420, CPtrList&, CVehicle*, float, float, float, float, float*, float>(PtrList, vehicle, arg3, arg4, arg5, arg6, arg7, arg8);
}

// 0x426220
void CCarCtrl::SlowCarDownForObject(CEntity* pEntity, CVehicle* vehicle, float* arg3, float arg4) {
    const CVector entityDir = pEntity->GetPosition() - vehicle->GetPosition();
    const float entityHeading = DotProduct(entityDir, vehicle->GetMatrix().GetForward());
    if (entityHeading > 0.0f && entityHeading < 20.0f) {
        if (pEntity->GetColModel()->GetBoundRadius() + vehicle->GetColModel()->GetBoundingBox().m_vecMax.x > fabs(DotProduct(entityDir, vehicle->GetMatrix().GetRight()))) {
            if (entityHeading >= 7.0f) {
                *arg3 = std::min(*arg3, (1.0f - (entityHeading - 7.0f) / 13.0f)) * arg4; // Original code multiplies by 0.07692308, which is the recp. of 13
            } else {
                *arg3 = 0.0f;
            }
        }
    }
}

// 0x42D4F0
void CCarCtrl::SlowCarDownForObjectsSectorList(CPtrList& PtrList, CVehicle* vehicle, float arg3, float arg4, float arg5, float arg6, float* arg7, float arg8) {
    plugin::Call<0x42D4F0, CPtrList&, CVehicle*, float, float, float, float, float*, float>(PtrList, vehicle, arg3, arg4, arg5, arg6, arg7, arg8);
}

// 0x42D0E0
void CCarCtrl::SlowCarDownForOtherCar(CEntity* pCar1, CVehicle* pCar2, float* arg3, float arg4) {
    plugin::Call<0x42D0E0, CEntity*, CVehicle*, float*, float>(pCar1, pCar2, arg3, arg4);
}

// 0x425440
void CCarCtrl::SlowCarDownForPedsSectorList(CPtrList& PtrList, CVehicle* vehicle, float arg3, float arg4, float arg5, float arg6, float* arg7, float arg8) {
    plugin::Call<0x425440, CPtrList&, CVehicle*, float, float, float, float, float*, float>(PtrList, vehicle, arg3, arg4, arg5, arg6, arg7, arg8);
}

// 0x434790
void CCarCtrl::SlowCarOnRailsDownForTrafficAndLights(CVehicle* vehicle) {
    auto& autoPilot = vehicle->m_autoPilot;

    if ((((int8)CTimer::GetFrameCounter() + (int8)(vehicle->m_nRandomSeed)) & 3) == 0) {
        if (CTrafficLights::ShouldCarStopForLight(vehicle, false) || CTrafficLights::ShouldCarStopForBridge(vehicle)) {
            CCarAI::CarHasReasonToStop(vehicle);
            autoPilot.m_fMaxTrafficSpeed = 0.0f;
        } else {
            autoPilot.m_fMaxTrafficSpeed = FindMaximumSpeedForThisCarInTraffic(vehicle);
        }
    }

    if (autoPilot.m_fMaxTrafficSpeed >= autoPilot.m_speed) {
        autoPilot.ModifySpeed(std::min(autoPilot.m_fMaxTrafficSpeed, CTimer::GetTimeStep() * 0.05f + autoPilot.m_speed));
    } else if (autoPilot.m_speed >= 0.1f) {
        autoPilot.ModifySpeed(std::max(autoPilot.m_fMaxTrafficSpeed, autoPilot.m_speed - CTimer::GetTimeStep() * 0.7f));
    } else if (autoPilot.m_speed != 0.0f) {
        autoPilot.ModifySpeed(0.0f);
    }
}

// 0x428DE0
void CCarCtrl::SteerAIBoatWithPhysicsAttackingPlayer(CVehicle* vehicle, float* arg2, float* arg3, float* arg4, bool* arg5) {
    plugin::Call<0x428DE0, CVehicle*, float*, float*, float*, bool*>(vehicle, arg2, arg3, arg4, arg5);
}

// 0x429090
void CCarCtrl::SteerAIBoatWithPhysicsCirclingPlayer(CVehicle* vehicle, float* arg2, float* arg3, float* arg4, bool* arg5) {
    plugin::Call<0x429090, CVehicle*, float*, float*, float*, bool*>(vehicle, arg2, arg3, arg4, arg5);
}

// 0x428BE0
void CCarCtrl::SteerAIBoatWithPhysicsHeadingForTarget(CVehicle* vehicle, float arg2, float arg3, float* arg4, float* arg5, float* arg6) {
    plugin::Call<0x428BE0, CVehicle*, float, float, float*, float*, float*>(vehicle, arg2, arg3, arg4, arg5, arg6);
}

// 0x422B20
void CCarCtrl::SteerAICarBlockingPlayerForwardAndBack(CVehicle* vehicle, float* arg2, float* arg3, float* arg4, bool* arg5) {
    plugin::Call<0x422B20, CVehicle*, float*, float*, float*, bool*>(vehicle, arg2, arg3, arg4, arg5);
}

// 0x433BA0
void CCarCtrl::SteerAICarParkParallel(CVehicle* vehicle, float* arg2, float* arg3, float* arg4, bool* arg5) {
    plugin::Call<0x433BA0, CVehicle*, float*, float*, float*, bool*>(vehicle, arg2, arg3, arg4, arg5);
}

// 0x433EA0
void CCarCtrl::SteerAICarParkPerpendicular(CVehicle* vehicle, float* arg2, float* arg3, float* arg4, bool* arg5) {
    plugin::Call<0x433EA0, CVehicle*, float*, float*, float*, bool*>(vehicle, arg2, arg3, arg4, arg5);
}

// 0x4336D0
void CCarCtrl::SteerAICarTowardsPointInEscort(CVehicle* pVehicle1, CVehicle* pVehicle2, float arg3, float arg4, float* arg5, float* arg6, float* arg7, bool* arg8) {
    plugin::Call<0x4336D0, CVehicle*, CVehicle*, float, float, float*, float*, float*, bool*>(pVehicle1, pVehicle2, arg3, arg4, arg5, arg6, arg7, arg8);
}

// 0x437C20
void CCarCtrl::SteerAICarWithPhysics(CVehicle* vehicle) {
    plugin::Call<0x437C20, CVehicle*>(vehicle);
}

// 0x434900
void CCarCtrl::SteerAICarWithPhysicsFollowPath(CVehicle* vehicle, float* arg2, float* arg3, float* arg4, bool* arg5) {
    plugin::Call<0x434900, CVehicle*, float*, float*, float*, bool*>(vehicle, arg2, arg3, arg4, arg5);
}

// 0x435830
void CCarCtrl::SteerAICarWithPhysicsFollowPath_Racing(CVehicle* vehicle, float* arg2, float* arg3, float* arg4, bool* arg5) {
    plugin::Call<0x435830, CVehicle*, float*, float*, float*, bool*>(vehicle, arg2, arg3, arg4, arg5);
}

// 0x432DD0
void CCarCtrl::SteerAICarWithPhysicsFollowPreRecordedPath(CVehicle* vehicle, float* arg2, float* arg3, float* arg4, bool* arg5) {
    plugin::Call<0x432DD0, CVehicle*, float*, float*, float*, bool*>(vehicle, arg2, arg3, arg4, arg5);
}

// 0x433280
void CCarCtrl::SteerAICarWithPhysicsHeadingForTarget(CVehicle* vehicle, CPhysical* pTarget, float arg3, float arg4, float* arg5, float* arg6, float* arg7, bool* arg8) {
    plugin::Call<0x433280, CVehicle*, CPhysical*, float, float, float*, float*, float*, bool*>(vehicle, pTarget, arg3, arg4, arg5, arg6, arg7, arg8);
}

// 0x4335E0
void CCarCtrl::SteerAICarWithPhysicsTryingToBlockTarget(CVehicle* vehicle, CEntity* Unusued, float arg3, float arg4, float arg5, float arg6, float* arg7, float* arg8, float* arg9, bool* arg10) {
    plugin::Call<0x4335E0, CVehicle*, CEntity*, float, float, float, float, float*, float*, float*, bool*>(vehicle, Unusued, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);
}

// 0x428990
void CCarCtrl::SteerAICarWithPhysicsTryingToBlockTarget_Stop(CVehicle* vehicle, float x, float y, float arg4, float arg5, float* arg6, float* arg7, float* arg8, bool* arg9) {
    plugin::Call<0x428990, CVehicle*, float, float, float, float, float*, float*, float*, bool*>(vehicle, x, y, arg4, arg5, arg6, arg7, arg8, arg9);
}

// 0x436A90
void CCarCtrl::SteerAICarWithPhysics_OnlyMission(CVehicle* vehicle, float* arg2, float* arg3, float* arg4, bool* arg5) {
    plugin::Call<0x436A90, CVehicle*, float*, float*, float*, bool*>(vehicle, arg2, arg3, arg4, arg5);
}

// 0x42AAD0
void CCarCtrl::SteerAIHeliAsPoliceHeli(CAutomobile* pAutomobile) {
    plugin::Call<0x42AAD0, CAutomobile*>(pAutomobile);
}

// 0x42ACB0
void CCarCtrl::SteerAIHeliFlyingAwayFromPlayer(CAutomobile* pAutomobile) {
    plugin::Call<0x42ACB0, CAutomobile*>(pAutomobile);
}

// 0x4238E0
void CCarCtrl::SteerAIHeliToCrashAndBurn(CAutomobile* pAutomobile) {
    plugin::Call<0x4238E0, CAutomobile*>(pAutomobile);
}

// 0x42A750
void CCarCtrl::SteerAIHeliToFollowEntity(CAutomobile* pAutomobile) {
    plugin::Call<0x42A750, CAutomobile*>(pAutomobile);
}

// 0x42AEB0
void CCarCtrl::SteerAIHeliToKeepEntityInView(CAutomobile* pAutomobile) {
    plugin::Call<0x42AEB0, CAutomobile*>(pAutomobile);
}

// 0x42AD30
void CCarCtrl::SteerAIHeliToLand(CAutomobile* pAutomobile) {
    plugin::Call<0x42AD30, CAutomobile*>(pAutomobile);
}

// 0x42A630
void CCarCtrl::SteerAIHeliTowardsTargetCoors(CAutomobile* pAutomobile) {
    plugin::Call<0x42A630, CAutomobile*>(pAutomobile);
}

// 0x423880
void CCarCtrl::SteerAIPlaneToCrashAndBurn(CAutomobile* pAutomobile) {
    plugin::Call<0x423880, CAutomobile*>(pAutomobile);
}

// 0x4237F0
void CCarCtrl::SteerAIPlaneToFollowEntity(CAutomobile* pAutomobile) {
    plugin::Call<0x4237F0, CAutomobile*>(pAutomobile);
}

// 0x423790
void CCarCtrl::SteerAIPlaneTowardsTargetCoors(CAutomobile* pAutomobile) {
    plugin::Call<0x423790, CAutomobile*>(pAutomobile);
}

// 0x422590
bool CCarCtrl::StopCarIfNodesAreInvalid(CVehicle* vehicle) {
    return plugin::CallAndReturn<bool, 0x422590, CVehicle*>(vehicle);
}

// 0x4222A0
void CCarCtrl::SwitchBetweenPhysicsAndGhost(CVehicle* vehicle) {
    plugin::Call<0x4222A0, CVehicle*>(vehicle);
}

// 0x423FC0
void CCarCtrl::SwitchVehicleToRealPhysics(CVehicle* vehicle) {
    plugin::Call<0x423FC0, CVehicle*>(vehicle);
}

// 0x425B30
float CCarCtrl::TestCollisionBetween2MovingRects(CVehicle* pVehicle1, CVehicle* pVehicle2, float arg3, float arg4, CVector* pVector1, CVector* pVector2) {
    return plugin::CallAndReturn<float, 0x425B30, CVehicle*, CVehicle*, float, float, CVector*, CVector*>(pVehicle1, pVehicle2, arg3, arg4, pVector1, pVector2);
}

// 0x425F70
float CCarCtrl::TestCollisionBetween2MovingRects_OnlyFrontBumper(CVehicle* pVehicle1, CVehicle* pVehicle2, float arg3, float arg4, CVector* pVector1, CVector* pVector2) {
    return plugin::CallAndReturn<float, 0x425F70, CVehicle*, CVehicle*, float, float, CVector*, CVector*>(pVehicle1, pVehicle2, arg3, arg4, pVector1, pVector2);
}

// 0x429520
void CCarCtrl::TestWhetherToFirePlaneGuns(CVehicle* vehicle, CEntity* pTarget) {
    plugin::Call<0x429520, CVehicle*, CEntity*>(vehicle, pTarget);
}

// 0x421FE0
bool CCarCtrl::ThisVehicleShouldTryNotToTurn(CVehicle* vehicle) {
    return plugin::CallAndReturn<bool, 0x421FE0, CVehicle*>(vehicle);
}

// 0x429300
void CCarCtrl::TriggerDogFightMoves(CVehicle* pVehicle1, CVehicle* pVehicle2) {
    plugin::Call<0x429300, CVehicle*, CVehicle*>(pVehicle1, pVehicle2);
}

// 0x424000
void CCarCtrl::UpdateCarCount(CVehicle* vehicle, uint8 bDecrease) {
    plugin::Call<0x424000, CVehicle*, uint8>(vehicle, bDecrease);
}

// 0x436540
void CCarCtrl::UpdateCarOnRails(CVehicle* vehicle) {
    plugin::Call<0x436540, CVehicle*>(vehicle);
}

// 0x426BC0
void CCarCtrl::WeaveForObject(CEntity* pEntity, CVehicle* vehicle, float* arg3, float* arg4) {
    plugin::Call<0x426BC0, CEntity*, CVehicle*, float*, float*>(pEntity, vehicle, arg3, arg4);
}

// 0x426350
void CCarCtrl::WeaveForOtherCar(CEntity* pEntity, CVehicle* vehicle, float* arg3, float* arg4) {
    plugin::Call<0x426350, CEntity*, CVehicle*, float*, float*>(pEntity, vehicle, arg3, arg4);
}

// 0x42D680
void CCarCtrl::WeaveThroughCarsSectorList(CPtrList& PtrList, CVehicle* vehicle, CPhysical* pPhysical, float arg4, float arg5, float arg6, float arg7, float* arg8, float* arg9) {
    plugin::Call<0x42D680, CPtrList&, CVehicle*, CPhysical*, float, float, float, float, float*, float*>(PtrList, vehicle, pPhysical, arg4, arg5, arg6, arg7, arg8, arg9);
}

// 0x42D950
void CCarCtrl::WeaveThroughObjectsSectorList(CPtrList& PtrList, CVehicle* vehicle, float arg3, float arg4, float arg5, float arg6, float* arg7, float* arg8) {
    plugin::Call<0x42D950, CPtrList&, CVehicle*, float, float, float, float, float*, float*>(PtrList, vehicle, arg3, arg4, arg5, arg6, arg7, arg8);
}

// 0x42D7E0
void CCarCtrl::WeaveThroughPedsSectorList(CPtrList& PtrList, CVehicle* vehicle, CPhysical* pPhysical, float arg4, float arg5, float arg6, float arg7, float* arg8, float* arg9) {
    plugin::Call<0x42D7E0, CPtrList&, CVehicle*, CPhysical*, float, float, float, float, float*, float*>(PtrList, vehicle, pPhysical, arg4, arg5, arg6, arg7, arg8, arg9);
}
