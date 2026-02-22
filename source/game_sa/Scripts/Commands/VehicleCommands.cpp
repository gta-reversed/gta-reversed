#include <StdInc.h>

#include "Commands.hpp"
#include <CommandParser/Parser.hpp>
#include <cassert>

#include "Garages.h"
#include "PlayerInfo.h"
#include "World.h"
#include "CarGenerator.h"
#include "TheCarGenerators.h"
#include "VehicleRecording.h"
#include "CommandParser/Parser.hpp"

using namespace notsa::script;

enum class eSeatId {
    FRONT_RIGHT,
    REAR_LEFT,
    REAR_RIGHT,
    PASSENGER4,
    PASSENGER5,
    PASSENGER6,
    PASSENGER7,
    PASSENGER8,

    BIKE_REAR = 0,
};

enum class eWheelId {
    FRONT_LEFT,
    REAR_LEFT,
    FRONT_RIGHT,
    REAR_RIGHT,
    ANY,

    BIKE_FRONT = 0,
    BIKE_REAR  = 1,
};

namespace {
auto ClampDegreesForScript(float deg) {
    return deg < 0.f
        ? deg + 360.f
        : deg > 360.f
            ? deg - 360.f
            : deg;
}

void DoSetCarIsWaitingForCollision(CRunningScript& S, CVehicle& self) {
    if (S.m_UsesMissionCleanup) {
        CWorld::Remove(&self);
        self.m_bIsStaticWaitingForCollision = true;
        CWorld::Add(&self);
    }
}
}; // namespace


/*!
* Various vehicle commands
*/
namespace {
void ClearHeliOrientation(CHeli& heli) {
    heli.ClearHeliOrientation();
}

void RemoveRCBuggy() {
    FindPlayerInfo().BlowUpRCBuggy(false);
}

void SetCarProofs(CVehicle& veh, bool bullet, bool fire, bool explosion, bool collision, bool melee) {
    auto& flags           = veh.physicalFlags;
    flags.bBulletProof    = bullet;
    flags.bFireProof      = fire;
    flags.bExplosionProof = explosion;
    flags.bCollisionProof = collision;
    flags.bMeleeProof     = melee;
}

void SwitchCarGenerator(int32 generatorId, int32 count) {
    const auto generator = CTheCarGenerators::Get(generatorId);
    if (count) {
        generator->SwitchOn();
        if (count <= 100) {
            generator->m_nGenerateCount = count;
        }
    } else {
        generator->SwitchOff();
    }
}

void SetHasBeenOwnedForCarGenerator(int32 generatorId, bool alreadyOwned) {
    CTheCarGenerators::Get(generatorId)->bPlayerHasAlreadyOwnedCar = alreadyOwned;
}

float GetCarSpeed(CVehicle& veh) {
    return veh.m_vecMoveSpeed.Magnitude() * 50.f;
}

void SetCarDrivingStyle(CVehicle& veh, eCarDrivingStyle style) {
    veh.m_autoPilot.m_nCarDrivingStyle = style;
}

bool IsFirstCarColor(CVehicle& veh, int32 color) {
    return veh.m_nPrimaryColor == color;
}

bool IsSecondCarColor(CVehicle& veh, int32 color) {
    return veh.m_nSecondaryColor == color;
}

MultiRet<uint8, uint8> GetExtraCarColors(CVehicle& veh) {
    return { veh.m_nTertiaryColor, veh.m_nQuaternaryColor };
}

/*
* @opcode 050B
* @command POP_CAR_BOOT_USING_PHYSICS
* 
* @brief Opens the trunk/boot door component of the vehicle
*/
void PopCarBootUsingPhysics(CAutomobile& automobile) {
    automobile.PopBootUsingPhysics();
}

void SkipToNextAllowedStation(CTrain& train) {
    CTrain::SkipToNextAllowedStation(&train);
}

void SetRailTrackResistanceMult(float value) {
    CVehicle::ms_fRailTrackResistance = CVehicle::ms_fRailTrackResistanceDefault * (value > 0.0f ? value : 1.0f);
}

void DisableHeliAudio(CVehicle& vehicle, bool enable) {
    if (enable) {
        vehicle.m_vehicleAudio.EnableHelicoptor();
    } else {
        vehicle.m_vehicleAudio.DisableHelicoptor();
    }
}

bool IsCarInAirProper(CRunningScript& S, CVehicle& vehicle) {
    for (auto* const e : vehicle.GetCollidingEntities()) {
        if (e && (e->GetIsTypeBuilding() || e->GetIsTypeVehicle())) {
            return false;
        }
    }
    return true;
}

bool IsCarStuck(CVehicle& vehicle) {
    return CTheScripts::StuckCars.HasCarBeenStuckForAWhile(GetVehiclePool()->GetRef(&vehicle));
}

void AddStuckCarCheck(CVehicle& vehicle, float stuckRadius, uint32 time) {
    CTheScripts::StuckCars.AddCarToCheck(GetVehiclePool()->GetRef(&vehicle), stuckRadius, time, false, false, false, false, 0);
}

void RemoveStuckCarCheck(CVehicle& vehicle) {
    CTheScripts::StuckCars.RemoveCarFromCheck(GetVehiclePool()->GetRef(&vehicle));
}

void AddStuckCarCheckWithWarp(CVehicle& vehicle, float stuckRadius, uint32 time, bool stuck, bool flipped, bool inWater, int8 numberOfNodesToCheck) {
    CTheScripts::StuckCars.AddCarToCheck(GetVehiclePool()->GetRef(&vehicle), stuckRadius, time, true, stuck, flipped, inWater, numberOfNodesToCheck);
}

void PlaneAttackPlayerUsingDogFight(CPlane& plane, CPlayerPed& player, float altitude) {
    if (plane.m_autoPilot.m_nCarMission != eCarMission::MISSION_PLANE_CRASH_AND_BURN && plane.m_autoPilot.m_nCarMission != eCarMission::MISSION_HELI_CRASH_AND_BURN) {
        plane.m_autoPilot.SetCarMission(eCarMission::MISSION_PLANE_DOG_FIGHT_PLAYER);
    }
    plane.m_minAltitude = altitude;
}

/// SET_CAR_ALWAYS_CREATE_SKIDS(07EE)
void SetCarAlwaysCreateSkids(CVehicle& vehicle, bool enable) {
    vehicle.vehicleFlags.bAlwaysSkidMarks = enable;
}

/// SET_CAR_AS_MISSION_CAR(0763)
void SetCarAsMissionCar(CRunningScript& S, CVehicle& vehicle) {
    if (S.m_UsesMissionCleanup && (vehicle.IsCreatedBy(eVehicleCreatedBy::RANDOM_VEHICLE) || vehicle.IsCreatedBy(eVehicleCreatedBy::PARKED_VEHICLE))) {
        vehicle.SetVehicleCreatedBy(eVehicleCreatedBy::MISSION_VEHICLE);
        CTheScripts::MissionCleanUp.AddEntityToList(vehicle);
    }
}

/// CREATE_CAR
CVehicle* CreateCar(CRunningScript& S, eModelID modelId, CVector pos) {
    return CCarCtrl::CreateCarForScript(modelId, pos, S.m_UsesMissionCleanup);
}

/// DELETE_CAR
// TODO: We need to take the handle here as well so we can delete by it later
//void DeleteCar(CVehicle* car) {
//    if (car) {
//        CWorld::Remove(car);
//        CWorld::RemoveReferencesToDeletedObject(car);
//        delete car;
//    } else {
//        CTheScripts::MissionCleanUp.RemoveEntityFromList(handle); // TODO
//    }
//}

void CarGotoCoordinatesUsingMission(CVehicle& vehicle, CVector where, eCarMission mission, eCarMission missionStraightLine) {
    auto& ap = vehicle.m_autoPilot;

    if (where.z <= MAP_Z_LOW_LIMIT) {
        where.z = CWorld::FindGroundZForCoord(where.x, where.y);
    }
    where.z += vehicle.GetDistanceFromCentreOfMassToBaseOfModel();

    const auto joined = CCarCtrl::JoinCarWithRoadSystemGotoCoors(&vehicle, where, false);

    vehicle.SetStatus(STATUS_PHYSICS);
    vehicle.SetEngineOn(true);

    if (!notsa::contains({ MISSION_PLANE_CRASH_AND_BURN, MISSION_HELI_CRASH_AND_BURN }, vehicle.m_autoPilot.m_nCarMission)) {
        ap.m_nCarMission = !joined
            ? mission
            : missionStraightLine;
    }
    if (ap.m_nCruiseSpeed <= 1) {
        ap.SetCruiseSpeed(1);
    }
    ap.StartCarMissionNow();
}

/// CAR_GOTO_COORDINATES
void CarGotoCoordinates(CVehicle& vehicle, CVector where) {
    CarGotoCoordinatesUsingMission(vehicle, where, MISSION_GOTOCOORDINATES, MISSION_GOTOCOORDINATES_STRAIGHTLINE);
}

/// CAR_GOTO_COORDINATES_ACCURATE
auto CarGotoCoordinatesAccurate(CVehicle& vehicle, CVector where) {
    CarGotoCoordinatesUsingMission(vehicle, where, MISSION_GOTOCOORDINATES_ACCURATE, MISSION_GOTOCOORDINATES_STRAIGHTLINE_ACCURATE);
}

/*
* @opcode 0704
* @command CAR_GOTO_COORDINATES_RACING
* @class Car
* @method GotoCoordinatesRacing
* 
* @brief Makes the AI drive to the destination as fast as possible, trying to overtake other vehicles
* 
* @param self CVehicle&
* @param x float
* @param y float
* @param z float
*/
void CarGotoCoordinatesRacing(CVehicle& self, CVector where) {
    CarGotoCoordinatesUsingMission(self, where, MISSION_GOTOCOORDINATES_RACING, MISSION_GOTOCOORDINATES_STRAIGHTLINE);
}

/// CAR_WANDER_RANDOMLY
void CarWanderRandomly(CVehicle& vehicle) {
    auto& ap = vehicle.m_autoPilot;

    CCarCtrl::JoinCarWithRoadSystem(&vehicle);

    vehicle.SetStatus(STATUS_PHYSICS);
    vehicle.SetEngineOn(true);

    if (!notsa::contains({ MISSION_PLANE_CRASH_AND_BURN, MISSION_HELI_CRASH_AND_BURN }, vehicle.m_autoPilot.m_nCarMission)) {
        ap.m_nCarMission = MISSION_CRUISE;
    }
    if (ap.m_nCruiseSpeed <= 1) {
        ap.SetCruiseSpeed(1);
    }
    ap.m_nTimeToStartMission = CTimer::GetTimeInMS();
}

/// CAR_SET_IDLE
void CarSetIdle(CVehicle& vehicle) {
    auto& ap = vehicle.m_autoPilot;

    if (!notsa::contains({ MISSION_PLANE_CRASH_AND_BURN, MISSION_HELI_CRASH_AND_BURN }, vehicle.m_autoPilot.m_nCarMission)) {
        ap.m_nCarMission = MISSION_NONE;
    }
}

/// GET_CAR_COORDINATES
CVector GetCarCoordinates(CVehicle& vehicle) {
    return vehicle.GetPosition();
}

/// SET_CAR_COORDINATES
void SetCarCoordinates(CVehicle& vehicle, float x, float y, float z) {
    CCarCtrl::SetCoordsOfScriptCar(&vehicle, x, y, z, false, true);
}

/// SET_CAR_CRUISE_SPEED
void SetCarCruiseSpeed(CVehicle& vehicle, float speed) {
    vehicle.m_autoPilot.SetCruiseSpeed((uint32)(std::min(speed, vehicle.m_pHandlingData->m_transmissionData.m_MaxFlatVelocity * 60.f)));
}

/// SET_CAR_MISSION
void SetCarMission(CVehicle& vehicle, eCarMission mission) {
    auto& ap = vehicle.m_autoPilot;

    ap.SetCarMissionFromScript(mission);
    ap.StartCarMissionNow();
    vehicle.SetEngineOn(true);
}

/// IS_CAR_DEAD
bool IsCarDead(CVehicle* vehicle) {
    return !vehicle || vehicle->GetStatus() == STATUS_WRECKED || !!vehicle->vehicleFlags.bIsDrowning;
}

/// IS_CAR_MODEL
bool IsCarModel(CVehicle& vehicle, eModelID modelId) {
    return vehicle.GetModelId() == modelId;
}

/// CREATE_CAR_GENERATOR
auto CreateCarGenerator(
    CVehicle& vehicle,
    CVector   pos,
    float     heading,
    eModelID  modelId,
    uint32    primaryColor,
    uint32    secondaryColor,
    bool      isHighPriority,
    uint8     chanceOfAlarm,
    uint8     chanceOfDoorLock,
    uint16    minDelay,
    uint16    maxDelay
) {
    if (pos.z > -100.f) {
        pos.z += 0.015f;
    }
    return CTheCarGenerators::CreateCarGenerator(
        pos,
        heading,
        modelId,
        primaryColor,
        secondaryColor,
        isHighPriority,
        chanceOfAlarm,
        chanceOfDoorLock,
        minDelay,
        maxDelay,
        0,
        true
    );
}

/// ADD_BLIP_FOR_CAR_OLD
auto AddBlipForCarOld(CRunningScript& S, int32 handle, int32 color, eBlipDisplay display) { // TODO: use `notsa::ScriptEntity<CVehicle>` instead of `int32 handle`
    return CRadar::SetEntityBlip(BLIP_CAR, handle, color, display);
}

/// GET_CAR_HEADING
auto GetCarHeading(CVehicle& vehicle) {
    return ClampDegreesForScript(DegreesToRadians(vehicle.GetHeading()));
}

/// SET_CAR_HEADING
auto SetCarHeading(CVehicle& vehicle, float deg) {
    vehicle.SetHeading(DegreesToRadians(ClampDegreesForScript(deg)));
    vehicle.UpdateRwMatrix();
}

/// IS_CAR_HEALTH_GREATER
auto IsCarHealthGreater(CVehicle& vehicle, float value) {
    return vehicle.GetHealth() >= value;
}

/// ADD_BLIP_FOR_CAR
auto AddBlipForCar(int32 handle) { // TODO: use `notsa::ScriptEntity<CVehicle>` instead of `int32 handle`
    const auto blip = CRadar::SetEntityBlip(BLIP_CAR, handle, 0, BLIP_DISPLAY_BOTH);
    CRadar::ChangeBlipScale(blip, 3);
    return blip;
}

/// IS_CAR_STUCK_ON_ROOF
auto IsCarStuckOnRoof(int32 handle) { // TODO: use `notsa::ScriptEntity<CVehicle>` instead of `int32 handle`
    return CTheScripts::UpsideDownCars.HasCarBeenUpsideDownForAWhile(handle); 
}

/// ADD_UPSIDEDOWN_CAR_CHECK
auto AddUpsidedownCarCheck(int32 handle) { // TODO: use `notsa::ScriptEntity<CVehicle>` instead of `int32 handle`
    CTheScripts::UpsideDownCars.AddCarToCheck(handle);
}

/// REMOVE_UPSIDEDOWN_CAR_CHECK
auto RemoveUpsidedownCarCheck(int32 handle) { // TODO: use `notsa::ScriptEntity<CVehicle>` instead of `int32 handle`
    CTheScripts::UpsideDownCars.RemoveCarFromCheck(handle);
}

/// IS_CAR_STOPPED
auto IsCarStopped(CVehicle& vehicle) {
    return CTheScripts::IsVehicleStopped(&vehicle);
}

/// IS_CAR_IN_AREA_2D
template<bool WithDebugSquare = false>
bool IsCarInArea2D(CRunningScript& S, CVehicle& vehicle, CVector2D p1, CVector2D p2, bool highlight) {
    if (highlight) {
        CTheScripts::HighlightImportantArea(
            (int32)(&S) + (int32)(S.m_IP),
            p1.x, p1.y,
            p2.x, p2.y,
            -100.f
        );
    }
    if constexpr (WithDebugSquare) {
        if (CTheScripts::DbgFlag) {
            const auto [minX, maxX] = std::minmax(p1.x, p2.x);
            const auto [minY, maxY] = std::minmax(p1.y, p2.y);
            CTheScripts::DrawDebugSquare(
                minX, maxX,
                minY, maxY
            );
        }
    }
    return vehicle.IsWithinArea(p1.x, p1.y, p2.x, p2.y);
}

/// LOCATE_CAR_2D
auto LocateCar2D(CRunningScript& S, CVehicle& vehicle, CVector2D pt, CVector2D radius, bool highlight) {
    return IsCarInArea2D<true>(
        S,
        vehicle,
        pt - radius,
        pt + radius,
        highlight
    );
}

/// IS_CAR_STOPPED_IN_AREA_2D
auto IsCarStoppedInArea2D(CRunningScript& S, CVehicle& vehicle, CVector2D p1, CVector2D p2, bool highlight) {
    return IsCarInArea2D<true>(S, vehicle, p1, p2, highlight) && IsCarStopped(vehicle); // Make sure `IsCarInArea2D` check is first, as it also does highlighting
}

/// LOCATE_STOPPED_CAR_2D
auto LocateStoppedCar2D(CRunningScript& S, CVehicle& vehicle, CVector2D pt, CVector2D radius, bool highlight) {
    return IsCarStoppedInArea2D(
        S,
        vehicle,
        pt - radius,
        pt + radius,
        highlight
    );
}

/// IS_CAR_IN_AREA_3D
bool IsCarInArea3D(CRunningScript& S, CVehicle& vehicle, CVector p1, CVector p2, bool highlight) {
    if (highlight) {
        CTheScripts::HighlightImportantArea(
            (int32)(&S) + (int32)(S.m_IP),
            p1.x, p1.y,
            p2.x, p2.y,
            (p2.z - p1.z) / 2.f
        );
    }
    return vehicle.IsWithinArea(
        p1.x, p1.y, p1.z,
        p2.x, p2.y, p2.z
    );
}

/// LOCATE_CAR_3D
auto LocateCar3D(CRunningScript& S, CVehicle& vehicle, CVector pt, CVector radius, bool highlight) {
    return IsCarInArea3D(
        S,
        vehicle,
        pt - radius,
        pt + radius,
        highlight
    );
}

/// IS_CAR_STOPPED_IN_AREA_3D
auto IsCarStoppedInArea3D(CRunningScript& S, CVehicle& vehicle, CVector p1, CVector p2, bool highlight) {
    return IsCarInArea3D(S, vehicle, p1, p2, highlight) && IsCarStopped(vehicle); // Make sure `IsCarInArea3D` check is first, as it also does highlighting
}

/// LOCATE_STOPPED_CAR_3D
auto LocateStoppedCar3D(CRunningScript& S, CVehicle& vehicle, CVector pt, CVector radius, bool highlight) {
    return IsCarStoppedInArea3D(
        S,
        vehicle,
        pt - radius,
        pt + radius,
        highlight
    );
}

/// MARK_CAR_AS_NO_LONGER_NEEDED
auto MarkCarAsNoLongerNeeded(CRunningScript& S, CVehicle& vehicle) {
    CTheScripts::CleanUpThisVehicle(&vehicle);
    if (S.m_UsesMissionCleanup) {
        CTheScripts::MissionCleanUp.RemoveEntityFromList(vehicle);
    }
}

/// SET_CAR_DENSITY_MULTIPLIER
auto SetCarDensityMultiplier(CVehicle& vehicle, float mult) {
    CCarCtrl::CarDensityMultiplier = mult;
}

/// SET_CAR_HEAVY
auto SetCarHeavy(CVehicle* vehicle, bool isHeavy) {
    if (!vehicle) {
        return;
    }

    vehicle->physicalFlags.bMakeMassTwiceAsBig = isHeavy;

    const auto* const handling   = vehicle->m_pHandlingData;
    vehicle->m_fMass             = handling->m_fMass;
    vehicle->m_fTurnMass         = handling->m_fTurnMass;
    vehicle->m_fBuoyancyConstant = handling->m_fBuoyancyConstant;
    if (isHeavy) {
        vehicle->m_fMass *= 3.0f;
        vehicle->m_fTurnMass *= 5.0f;
        vehicle->m_fBuoyancyConstant *= 2.0f;
    }
}

/// IS_CAR_UPSIDEDOWN
auto IsCarUpsidedown(CVehicle& vehicle) {
    return vehicle.GetUp().z <= 0.3f;
}

/// LOCK_CAR_DOORS
auto LockCarDoors(CVehicle* vehicle, eCarLock lock) {
    if (vehicle) {
        vehicle->m_nDoorLock = lock;
    }
}

/// EXPLODE_CAR
auto ExplodeCar(CVehicle& vehicle) {
    vehicle.BlowUpCar(nullptr, false);
}

/// IS_CAR_UPRIGHT
auto IsCarUpright(CVehicle& vehicle) {
    return vehicle.GetUp().z > 0.f;
}

/// SET_TARGET_CAR_FOR_MISSION_GARAGE
auto SetTargetCarForMissionGarage(const char* garageName, CVehicle* vehicle) {
    const auto garage = CGarages::FindGarageIndex(garageName);
    if (garage == -1) {
        return;
    }
    CGarages::SetTargetCarForMissionGarage(garage, vehicle);
}

/// SET_CAR_HEALTH
auto SetCarHealth(CVehicle& vehicle, float health) {
    vehicle.m_fHealth = health;
}

/// GET_CAR_HEALTH
auto GetCarHealth(CVehicle& vehicle) {
    return vehicle.m_fHealth;
}

/// CHANGE_CAR_COLOUR
auto ChangeCarColour(CVehicle& vehicle, uint32 primaryColor, uint32 secondaryColor) {
    vehicle.m_nPrimaryColor = primaryColor;
    vehicle.m_nSecondaryColor = secondaryColor;
}

/// SET_CAN_RESPRAY_CAR
auto SetCanResprayCar(CAutomobile& automobile, bool enabled) {
    automobile.autoFlags.bShouldNotChangeColour = enabled;
}

/// SET_CAR_ONLY_DAMAGED_BY_PLAYER
auto SetCarOnlyDamagedByPlayer(CVehicle& vehicle, bool enabled) {
    vehicle.physicalFlags.bInvulnerable = enabled;
}

/// IS_CAR_IN_WATER
auto IsCarInWater(CVehicle* vehicle) {
    if (!vehicle) {
        return false;
    }
    if (vehicle->physicalFlags.bSubmergedInWater) {
        return true;
    }
    if (vehicle->GetModelId() == MODEL_VORTEX) {
        const auto* const vortex = vehicle->AsAutomobile();
        if (vortex->m_fWheelsSuspensionCompression[0] < 1.f) {
            return g_surfaceInfos.IsShallowWater(vortex->m_wheelColPoint[0].m_nSurfaceTypeB);
        }
    }
    return false;
}

/// IS_CAR_ON_SCREEN
auto IsCarOnScreen(CVehicle& vehicle) {
    return vehicle.GetIsOnScreen();
}

/// GET_CAR_FORWARD_X
auto GetCarForwardX(CVehicle& vehicle) {
    const CVector2D forward { vehicle.GetForward() };
    return forward.x / forward.Magnitude();
}

/// GET_CAR_FORWARD_Y
auto GetCarForwardY(CVehicle& vehicle) {
    const CVector2D forward { vehicle.GetForward() };
    return forward.y / forward.Magnitude();
}

/// HAS_CAR_BEEN_DAMAGED_BY_WEAPON
auto HasCarBeenDamagedByWeapon(CVehicle* vehicle, eWeaponType weaponType) {
    if (!vehicle) {
        return false;
    }
    if (weaponType == WEAPON_ANYMELEE || weaponType == WEAPON_ANYWEAPON) {
        return CDarkel::CheckDamagedWeaponType(vehicle->GetLastWeaponDamageType(), weaponType);
    }
    return vehicle->GetLastWeaponDamageType() == weaponType;
}

/// START_CAR_FIRE
auto StartCarFire(CVehicle& vehicle) {
    return gFireManager.StartScriptFire(
        vehicle.GetPosition(),
        &vehicle,
        0.8f,
        1,
        0,
        1
    );
}

/// GET_RANDOM_CAR_OF_TYPE_IN_AREA
auto GetRandomCarOfTypeInArea(CRunningScript& S, CVehicle& vehicle, float minX, float minY, float maxX, float maxY, eModelID modelId) {
    int32 ret = -1;
    for (auto& veh : GetVehiclePool()->GetAllValid()) {
        switch (veh.GetVehicleAppearance()) {
        case VEHICLE_APPEARANCE_AUTOMOBILE:
        case VEHICLE_APPEARANCE_BIKE:
            break;
        default:
            continue;
        }
        if (!veh.vehicleFlags.bIsLawEnforcer) {
            continue;
        }
        if (modelId != MODEL_INVALID && veh.GetModelId() != modelId) {
            continue;
        }
        if (!veh.CanBeDeleted()) {
            continue;
        }
        if (!veh.IsWithinArea(minX, minY, maxX, maxY)) {
            continue;
        }
        ret = GetVehiclePool()->GetRef(&veh);
        veh.SetVehicleCreatedBy(eVehicleCreatedBy::MISSION_VEHICLE);
        if (S.m_UsesMissionCleanup) {
            CTheScripts::MissionCleanUp.AddEntityToList(veh);
        }
        /* NOTE (Pirulax): Why no `break`, or, better yet, return the handle here? */
    }
    return ret;
}

/// SET_CAR_VISIBLE
auto SetCarVisible(CVehicle& vehicle, bool visible) {
    vehicle.SetIsVisible(visible);
}

/// PLACE_OBJECT_RELATIVE_TO_CAR
auto PlaceObjectRelativeToCar(CObject& obj, CVehicle& vehicle, CVector offset) {
    CPhysical::PlacePhysicalRelativeToOtherPhysical(&obj, &vehicle, offset);
}

/// SWITCH_CAR_SIREN
auto SwitchCarSiren(CVehicle& vehicle, bool state) {
    vehicle.vehicleFlags.bSirenOrAlarm = state;
}

/*
* @opcode 039C
* @command SET_CAR_WATERTIGHT
* @class Car
* @method SetWatertight
* 
* @brief Makes the vehicle watertight, meaning characters inside will not be harmed if the vehicle is submerged in water
* 
* @param self CVehicle&
* @param state bool
*/
void SetCarWatertight(CVehicle& self, bool state) {
    if (self.IsBike()) { // 0x484440
        self.AsBike()->bikeFlags.bWaterTight = state;
    } else if (self.IsAutomobile()) { // 0x484407
        self.AsAutomobile()->autoFlags.bWaterTight = state;
    }
}

/*
* @opcode 039F
* @command TURN_CAR_TO_FACE_COORD
* @class Car
* @method TurnToFaceCoord
* 
* @brief Sets the car's heading so that it is facing the 2D coordinate
* 
* @param self CVehicle&
* @param x float
* @param y float
*/
void TurnCarToFaceCoord(CVehicle& self, CVector2D point) {
    const auto& pos = self.GetPosition();
    const auto angle = CGeneral::GetATanOf(pos - point) + HALF_PI;
    self.SetHeading(angle > TWO_PI ? angle - TWO_PI : angle);
}

/*
* @opcode 03A2
* @command SET_CAR_STATUS
* @class Car
* @method SetStatus
* 
* @brief Sets the car's status
* 
* @param self CVehicle&
* @param status eEntityStatus
*/
void SetCarStatus(CVehicle& self, eEntityStatus status) {
    if (self.GetStatus() == STATUS_SIMPLE && status != STATUS_SIMPLE) {
        CCarCtrl::SwitchVehicleToRealPhysics(&self);
    }
    self.SetStatus(status);
}

/*
* @opcode 03AB
* @command SET_CAR_STRONG
* @class Car
* @method SetStrong
* 
* @brief Defines whether the car is more resistant to collisions than normal
* 
* @param self CVehicle&
* @param state bool
*/
void SetCarStrong(CVehicle& self, bool state) {
    self.vehicleFlags.bTakeLessDamage = state;
}

/*
* @opcode 03C9
* @command IS_CAR_VISIBLY_DAMAGED
* @class Car
* @method IsVisiblyDamaged
* 
* @brief Returns true if any of the car components is visibly damaged or lost
* 
* @param self CVehicle&
*/
bool IsCarVisiblyDamaged(CVehicle& self) {
    return self.vehicleFlags.bIsDamaged;
}

/*
* @opcode 03ED
* @command SET_UPSIDEDOWN_CAR_NOT_DAMAGED
* @class Car
* @method SetUpsidedownNotDamaged
* 
* @brief Disables the car from exploding when it is upside down, as long as the player is not in the vehicle
* 
* @param self CVehicle&
* @param state bool
*/
void SetUpsidedownCarNotDamaged(CVehicle& self, bool state) {
    self.AsAutomobile()->autoFlags.bDoesNotGetDamagedUpsideDown = state;
}

/*
* @opcode 03F3
* @command GET_CAR_COLOURS
* @class Car
* @method GetColors
* 
* @brief Gets the car's primary and secondary colors
* 
* @param self CVehicle&
*/
auto GetCarColours(CVehicle& self) {
    return notsa::script::return_multiple(
        self.m_nPrimaryColor,
        self.m_nSecondaryColor
    );
}

/*
* @opcode 03F5
* @command SET_CAR_CAN_BE_DAMAGED
* @class Car
* @method SetCanBeDamaged
* 
* @brief Sets whether the car receives damage
* 
* @param self CVehicle&
* @param state bool
*/
void SetCarCanBeDamaged(CVehicle& self, bool state) {
    self.vehicleFlags.bCanBeDamaged = state;
}

/*
* @opcode 0407
* @command GET_OFFSET_FROM_CAR_IN_WORLD_COORDS
* @class Car
* @method GetOffsetInWorldCoords
* 
* @brief Returns the coordinates of an offset of the vehicle's position, depending on the vehicle's rotation
* 
* @param self CVehicle&
* @param xOffset float
* @param yOffset float
* @param zOffset float
*/
CVector GetOffsetFromCarInWorldCoords(CVehicle& self, CVector offset) {
    return self.GetMatrix().TransformPoint(offset);
}

/*
* @opcode 0423
* @command SET_CAR_TRACTION
* @class Car
* @method SetTraction
* 
* @brief Overrides the default AI controlled vehicle traction value of 1.0
* 
* @param self CVehicle&
* @param traction float
*/
void SetCarTraction(CVehicle& self, float traction) {
    if (self.IsAutomobile()) {
        self.AsAutomobile()->m_fCarTraction = traction;
    } else {
        self.AsBike()->m_fExtraTractionMult = traction;
    }
}

/*
* @opcode 0428
* @command SET_CAR_AVOID_LEVEL_TRANSITIONS
* @class Car
* @method SetAvoidLevelTransitions
* 
* @brief Sets whether the vehicle will avoid paths between levels (0426)
* 
* @param self CVehicle&
* @param state bool
*/
void SetCarAvoidLevelTransitions(CVehicle& self, bool state) {
    self.m_autoPilot.carCtrlFlags.bAvoidLevelTransitions = state;
}

/*
* @opcode 0466
* @command SET_CAR_STAY_IN_FAST_LANE
* @class Car
* @method SetStayInFastLane
* 
* @brief 
* 
* @param self CVehicle&
* @param state bool
*/
void SetCarStayInFastLane(CVehicle& self, bool state) {
    self.m_autoPilot.carCtrlFlags.bStayInFastLane = state;
}

/*
* @opcode 0431
* @command IS_CAR_PASSENGER_SEAT_FREE
* @class Car
* @method IsPassengerSeatFree
* 
* @brief Returns true if the specified car seat is empty
* 
* @param self CVehicle&
* @param seat eSeatId
*/
bool IsCarPassengerSeatFree(CVehicle& self, eSeatId seat) {
    return +seat < self.m_nMaxPassengers && !self.m_apPassengers[+seat];
}

/*
* @opcode 0441
* @command GET_CAR_MODEL
* @class Car
* @method GetModel
* 
* @brief Returns the car's model id
* 
* @param self CVehicle&
*/
eModelID GetCarModel(CVehicle& self) {
    return self.GetModelId();
}

/*
* @opcode 0468
* @command CLEAR_CAR_LAST_WEAPON_DAMAGE
* @class Car
* @method ClearLastWeaponDamage
* 
* @brief Clears the vehicle's last weapon damage (see 031E)
* 
* @param self CVehicle&
*/
void ClearCarLastWeaponDamage(CVehicle& self) {
    self.m_nLastWeaponDamageType = (uint8)(WEAPON_NONE);
    assert(self.m_nLastWeaponDamageType == (uint8)(-1)); // sanity check
}

/*
* @opcode 046C
* @command GET_DRIVER_OF_CAR
* @class Car
* @method GetDriver
* 
* @brief Returns the car's driver handle
* 
* @param self CVehicle&
*/
CPed* GetDriverOfCar(CVehicle& self) {
    return self.m_pDriver; /* null will be pushed as `-1` - as expected */
}

/*
* @opcode 0477
* @command SET_CAR_TEMP_ACTION
* @class Car
* @method SetTempAction
* 
* @brief Makes the AI driver perform the action in the vehicle for the specified period of time
* 
* @param self CVehicle&
* @param actionId eTempAction
* @param duration uint
*/
void SetCarTempAction(CVehicle& self, eAutoPilotTempAction action, uint32 duration) {
    self.m_autoPilot.SetTempAction(action, duration);
}

/*
* @opcode 048B
* @command SET_CAR_RANDOM_ROUTE_SEED
* @class Car
* @method SetRandomRouteSeed
* 
* @brief Sets the car on a specific route
* 
* @param self CVehicle&
* @param routeSeed int
*/
void SetCarRandomRouteSeed(CVehicle& self, int16 routeSeed) {
    self.m_nForcedRandomRouteSeed = routeSeed;
}

/*
* @opcode 0495
* @command IS_CAR_ON_FIRE
* @class Car
* @method IsOnFire
* 
* @brief Returns true if the car is burning
* 
* @param self CVehicle&
*/
bool IsCarOnFire(CVehicle& self) {
    if (self.m_fHealth < 250.f) {
        return true;
    }
    if (self.IsAutomobile()) {
        return self.AsAutomobile()->GetDamageManager().GetEngineStatus() >= 225;
    }
    return self.m_pFire != nullptr;
}

/*
* @opcode 0496
* @command IS_CAR_TYRE_BURST
* @class Car
* @method IsTireBurst
* 
* @brief Returns true if a given tire on the car is deflated
* 
* @param self CVehicle&
* @param tireId eWheelId
*/
bool IsCarTyreBurst(CVehicle& self, eWheelId tire) {
    if (self.IsBike()) {
        auto* const bike = self.AsBike();
        switch (tire) {
        case eWheelId::ANY:
            return rng::all_of(bike->m_nWheelStatus, [](auto status) { return status == 1; });
        case eWheelId::FRONT_RIGHT:
        case eWheelId::BIKE_FRONT:
            return bike->m_nWheelStatus[+eWheelId::BIKE_FRONT] == 1;
        case eWheelId::REAR_RIGHT:
        case eWheelId::BIKE_REAR:
            return bike->m_nWheelStatus[+eWheelId::BIKE_REAR] == 1;
        default:
            NOTSA_UNREACHABLE("Invalid wheel ID for bike: {}", +tire);
        }
    } else if (self.IsAutomobile()) {
        auto* const automobile = self.AsAutomobile();
        switch (tire) {
        case eWheelId::ANY:
            return automobile->GetDamageManager().GetWheelStatus(eCarWheel::CAR_WHEEL_FRONT_LEFT)
                || automobile->GetDamageManager().GetWheelStatus(eCarWheel::CAR_WHEEL_FRONT_RIGHT)
                || automobile->GetDamageManager().GetWheelStatus(eCarWheel::CAR_WHEEL_REAR_LEFT)
                || automobile->GetDamageManager().GetWheelStatus(eCarWheel::CAR_WHEEL_REAR_RIGHT);
        default:
            return automobile->GetDamageManager().GetWheelStatus(static_cast<eCarWheel>(tire));
        }
    } else {
        NOTSA_UNREACHABLE();
    }
}

/*
* @opcode 04BA
* @command SET_CAR_FORWARD_SPEED
* @class Car
* @method SetForwardSpeed
* 
* @brief Sets the speed of the car
* 
* @param self CVehicle&
* @param forwardSpeed float
*/
void SetCarForwardSpeed(CVehicle& self, float forwardSpeed) {
    self.m_vecMoveSpeed = self.GetForward() * (forwardSpeed / 60.f);
    if (self.m_pHandlingData->m_bIsHeli && !self.IsAutomobile()) {
        self.AsHeli()->m_fHeliRotorSpeed = 0.22f;
    }
}

/*
* @opcode 04BD
* @command MARK_CAR_AS_CONVOY_CAR
* @class Car
* @method MarkAsConvoyCar
* 
* @brief Marks the car as being part of a convoy, which seems to follow a path set by 0994
* 
* @param self CVehicle&
* @param state bool
*/
void MarkCarAsConvoyCar(CVehicle& self, bool state) {
    self.vehicleFlags.bPartOfConvoy = state;
}

/*
* @opcode 04E0
* @command SET_CAR_STRAIGHT_LINE_DISTANCE
* @class Car
* @method SetStraightLineDistance
* 
* @brief Sets the minimum distance for the AI driver to start ignoring car paths and go straight to the target
* 
* @param self CVehicle&
* @param distance int
*/
void SetCarStraightLineDistance(CVehicle& self, uint8 distance) {
    self.m_autoPilot.m_nStraightLineDistance = distance;
}

/*
* @opcode 04E1
* @command POP_CAR_BOOT
* @class Car
* @method PopBoot
* 
* @brief Opens the car's trunk and keeps it open
* 
* @param self CVehicle&
*/
void PopCarBoot(CVehicle& self) {
    self.AsAutomobile()->PopBoot();
}

/*
* @opcode 04F1
* @command IS_CAR_WAITING_FOR_WORLD_COLLISION
* @class Car
* @method IsWaitingForWorldCollision
* 
* @param self CVehicle&
*/
bool IsCarWaitingForWorldCollision(CVehicle& self) {
    return self.m_bIsStaticWaitingForCollision;
}

/*
* @opcode 04FE
* @command BURST_CAR_TYRE
* @class Car
* @method BurstTire
* 
* @brief Deflates the car's tire
* 
* @param self CVehicle&
* @param tireId eWheelId
*/
void BurstCarTyre(CVehicle& self, eWheelId tire) {
    if (self.IsBike()) {
        switch (tire) {
        case eWheelId::BIKE_FRONT:
        case eWheelId::FRONT_RIGHT: self.BurstTyre(0, true); break;
        case eWheelId::BIKE_REAR:
        case eWheelId::REAR_RIGHT:  self.BurstTyre(1, true); break;
        default:                    NOTSA_UNREACHABLE("Invalid wheel ID for bike: {}", +tire);
        }
    }
    self.BurstTyre(+tire, true);
}

/*
* @opcode 0506
* @command SET_CAR_MODEL_COMPONENTS
* @class Car
* @method SetModelComponents
* 
* @brief Sets the variation of the next car to be created
* 
* @param _unused eModelID
* @param component1 int
* @param component2 int
*/
void SetCarModelComponents(eModelID _unused, int component1, int component2) {
    CVehicleModelInfo::ms_compsToUse[0] = component1;
    CVehicleModelInfo::ms_compsToUse[1] = component2;
}

/*
* @opcode 0508
* @command CLOSE_ALL_CAR_DOORS
* @class Car
* @method CloseAllDoors
* 
* @brief Closes all car doors, hoods and boots
* 
* @param self CVehicle&
*/
void CloseAllCarDoors(CVehicle& self) {
    self.AsAutomobile()->CloseAllDoors();
}

/*
* @opcode 0519
* @command FREEZE_CAR_POSITION
* @class Car
* @method FreezePosition
* 
* @brief Locks the vehicle's position
* 
* @param self CVehicle&
* @param state bool
*/
void FreezeCarPosition(CVehicle& self, bool state) {
    self.physicalFlags.bDontApplySpeed = state;
    self.physicalFlags.bCollidable = state;
    self.physicalFlags.bDisableCollisionForce = state;
    if (state) {
        self.SkipPhysics();
        self.SetVelocity({ 0.f, 0.f, 0.f });
        self.SetTurnSpeed({ 0.f, 0.f, 0.f });
    }
}

/*
* @opcode 051C
* @command HAS_CAR_BEEN_DAMAGED_BY_CHAR
* @class Car
* @method HasBeenDamagedByChar
* 
* @brief Returns true if the car has been damaged by the specified char
* 
* @param self CVehicle
* @param ped CPed
*/
bool HasCarBeenDamagedByChar(CVehicle* self, CPed* ped) {
    if (!self || !self->m_pLastDamageEntity) {
        return false;
    }
    if (!ped) {
        return self->m_pLastDamageEntity->GetIsTypePed();
    }
    if (self->m_pLastDamageEntity == ped) {
        return true;
    }
    if (ped->bInVehicle) {
        return self->m_pLastDamageEntity == ped->m_pVehicle;
    }
    return false;
}

/*
* @opcode 051D
* @command HAS_CAR_BEEN_DAMAGED_BY_CAR
* @class Car
* @method HasBeenDamagedByCar
* 
* @brief Returns true if the vehicle has been damaged by another specified vehicle
* 
* @param self CVehicle&
* @param other CVehicle&
*/
bool HasCarBeenDamagedByCar(CVehicle* self, CVehicle* other) {
    if (!self || !self->m_pLastDamageEntity) {
        return false;
    }
    if (!other) {
        return self->m_pLastDamageEntity->GetIsTypeVehicle();
    }
    return self->m_pLastDamageEntity == other;
}

/*
* @opcode 053F
* @command SET_CAN_BURST_CAR_TYRES
* @class Car
* @method SetCanBurstTires
* 
* @brief Sets whether the car's tires can be deflated
* 
* @param self CVehicle&
* @param state bool
*/
void SetCanBurstCarTyres(CVehicle& self, bool state) {
    self.vehicleFlags.bTyresDontBurst = state;
}

/*
* @opcode 054F
* @command CLEAR_CAR_LAST_DAMAGE_ENTITY
* @class Car
* @method ClearLastDamageEntity
* 
* @brief Clears the car's last damage entity
* 
* @param self CVehicle&
*/
void ClearCarLastDamageEntity(CVehicle& self) {
    self.m_pLastDamageEntity = nullptr;
}

/*
* @opcode 0574
* @command FREEZE_CAR_POSITION_AND_DONT_LOAD_COLLISION
* @class Car
* @method FreezePositionAndDontLoadCollision
* 
* @brief Makes the car maintain its position
* 
* @param self CVehicle&
* @param state bool
*/
void FreezeCarPositionAndDontLoadCollision(CRunningScript& S, CVehicle& self, bool state) {
    self.physicalFlags.bDontApplySpeed = state;
    self.physicalFlags.bDisableCollisionForce = state;
    if (state) {
        DoSetCarIsWaitingForCollision(S, self);
    } else {
        self.m_nFakePhysics = false;
    }
}

/*
* @opcode 0587
* @command SET_LOAD_COLLISION_FOR_CAR_FLAG
* @class Car
* @method SetLoadCollisionFlag
* 
* @brief 
* 
* @param self CVehicle&
* @param state bool
*/
void SetLoadCollisionForCarFlag(CRunningScript& S, CVehicle& self, bool state) {
    self.physicalFlags.b15 = !state;
    if (state) {
        DoSetCarIsWaitingForCollision(S, self);
    } else if (self.m_bIsStaticWaitingForCollision) {
        self.m_bIsStaticWaitingForCollision = false;
        if (!self.GetIsStatic()) {
            self.AddToMovingList();
        }
    }
}

/*
* @opcode 05EB
* @command START_PLAYBACK_RECORDED_CAR
* @class Car
* @method StartPlayback
* 
* @brief Assigns a car to a path
* 
* @param self CVehicle&
* @param path int
*/
void StartPlaybackRecordedCar(CVehicle& self, int32 path) {
    CVehicleRecording::StartPlaybackRecordedCar(&self, path, false, false);
}

/*
* @opcode 05EC
* @command STOP_PLAYBACK_RECORDED_CAR
* @class Car
* @method StopPlayback
* 
* @brief Stops car from following path
* 
* @param self CVehicle&
*/
void StopPlaybackRecordedCar(CVehicle* self) {
    if (self) {
        CVehicleRecording::StopPlaybackRecordedCar(self);
    }
}

/*
* @opcode 05ED
* @command PAUSE_PLAYBACK_RECORDED_CAR
* @class Car
* @method PausePlayback
* 
* @brief Freezes the car on its path
* 
* @param self CVehicle&
*/
void PausePlaybackRecordedCar(CVehicle& self) {
    CVehicleRecording::PausePlaybackRecordedCar(&self);
}

/*
* @opcode 060E
* @command IS_PLAYBACK_GOING_ON_FOR_CAR
* @class Car
* @method IsPlaybackGoingOn
* 
* @brief Returns true if the car is assigned to a path
* 
* @param self CVehicle&
*/
bool IsPlaybackGoingOnForCar(CVehicle& self) {
    return CVehicleRecording::IsPlaybackGoingOnForCar(&self);
}

/*
* @opcode 05EE
* @command UNPAUSE_PLAYBACK_RECORDED_CAR
* @class Car
* @method UnpausePlayback
* 
* @brief Unfreezes the vehicle on its path
* 
* @param self CVehicle&
*/
void UnpausePlaybackRecordedCar(CVehicle& self) {
    CVehicleRecording::UnpausePlaybackRecordedCar(&self);
}

/*
* @brief Helper function for setting a car to escort another using a specific mission
*/
void SetCarEscortCarUsingMission(CVehicle& self, CVehicle& other, eCarMission mission) {
    auto* const ap = &self.m_autoPilot;

    ap->SetTargetEntity(&other);
    if (!notsa::contains({ MISSION_PLANE_CRASH_AND_BURN, MISSION_HELI_CRASH_AND_BURN }, ap->m_nCarMission)) {
        ap->m_nCarMission = mission;
    }
}

/*
* @opcode 05F1
* @command SET_CAR_ESCORT_CAR_LEFT
* @class Car
* @method SetEscortCarLeft
* 
* @brief Makes the vehicle stay on the other vehicle's left side, keeping parallel
* 
* @param self CVehicle&
* @param handle CVehicle&
*/
void SetCarEscortCarLeft(CVehicle& self, CVehicle& other) {
    SetCarEscortCarUsingMission(self, other, MISSION_ESCORT_LEFT);
}

/*
* @opcode 05F2
* @command SET_CAR_ESCORT_CAR_RIGHT
* @class Car
* @method SetEscortCarRight
* 
* @brief Makes the vehicle stay by the right side of the other vehicle, keeping parallel
* 
* @param self CVehicle&
* @param handle CVehicle&
*/
void SetCarEscortCarRight(CVehicle& self, CVehicle& other) {
    SetCarEscortCarUsingMission(self, other, MISSION_ESCORT_RIGHT);
}

/*
* @opcode 05F3
* @command SET_CAR_ESCORT_CAR_REAR
* @class Car
* @method SetEscortCarRear
* 
* @brief Makes the vehicle stay behind the other car, keeping parallel
* 
* @param self CVehicle&
* @param handle CVehicle&
*/
void SetCarEscortCarRear(CVehicle& self, CVehicle& other) {
    SetCarEscortCarUsingMission(self, other, MISSION_ESCORT_REAR);
}

/*
* @opcode 05F4
* @command SET_CAR_ESCORT_CAR_FRONT
* @class Car
* @method SetEscortCarFront
* 
* @brief Makes the vehicle stay in front of the other, keeping parallel
* 
* @param self CVehicle&
* @param handle CVehicle&
*/
void SetCarEscortCarFront(CVehicle& self, CVehicle& other) {
    SetCarEscortCarUsingMission(self, other, MISSION_ESCORT_FRONT);
}

/*
* @opcode 0657
* @command OPEN_CAR_DOOR
* @class Car
* @method OpenDoor
* 
* @brief Opens the specified car door
* 
* @param self CVehicle&
* @param door eCarDoor
*/
void OpenCarDoor(CVehicle& self, eDoors door) {
    auto* const automobile = self.AsAutomobile();
    if (automobile->IsDoorMissing(door)) {
        return;
    }
    const auto node = CDamageManager::GetCarNodeIndexFromDoor(door);
    if (!automobile->m_aCarNodes[node]) {
        return;
    }
    automobile->OpenDoor(nullptr, node, door, 1.f, true);
}

/*
* @opcode 0674
* @command CUSTOM_PLATE_FOR_NEXT_CAR
* @class Car
* @method CustomPlateForNextCar
* 
* @brief Sets the numberplate of the next car to be spawned with the specified model
* 
* @param modelId eModelID
* @param text std::string_view
*/
void CustomPlateForNextCar(eModelID modelId, std::string_view text) {
    assert(text.length() <= 8);

    auto* const mi = CModelInfo::GetVehicleModelInfo(modelId); /* NOTE: Not sure if this is correct, it uses `AsVehicleModelInfoPtr` without checking */
    if (!mi || mi->GetModelType() != MODEL_INFO_VEHICLE || !mi->m_pPlateMaterial) {
        return;
    }

    char plate[8 + 1]{ 0 };
    for (auto [i, chr] : rngv::enumerate(text)) {
        plate[i] = chr == '_' || !chr ? ' ' : chr;
    }
    plate[8] = '\0';
    mi->SetCustomCarPlateText(plate);
}

/*
* @opcode 067F
* @command FORCE_CAR_LIGHTS
* @class Car
* @method ForceLights
* 
* @brief Sets an override for the car's lights
* 
* @param self CVehicle&
* @param lightMode eCarLights
*/
void ForceCarLights(CVehicle& self, eVehicleOverrideLightsState lightMode) {
    self.m_nOverrideLights = lightMode;
}

/*
* @opcode 0683
* @command ATTACH_CAR_TO_CAR
* @class Car
* @method AttachToCar
* 
* @brief 
* 
* @param self CVehicle&
* @param other CVehicle&
* @param xOffset float
* @param yOffset float
* @param zOffset float
* @param xRotation float
* @param yRotation float
* @param zRotation float
*/
void AttachCarToCar(CVehicle& self, CVehicle& other, CVector offset, CVector rotation) {
    if (offset.x > -999.9f) {
        self.AttachEntityToEntity(&other, offset, rotation * DEG_TO_RAD);
    } else {
        self.AttachEntityToEntity(&other, nullptr, nullptr);
    }
}

/*
* @opcode 0684
* @command DETACH_CAR
* @class Car
* @method Detach
* 
* @brief 
* 
* @param self CVehicle&
* @param dirX float
* @param dirY float
* @param strength float
* @param collisionDetection bool
*/
void DetachCar(CVehicle* vehicle, CVector2D dir, float strength, bool collisionDetection) {
    if (!vehicle || !vehicle->m_pAttachedTo) {
        return;
    }
    vehicle->DettachEntityFromEntity(
        DegreesToRadians(dir.x),
        DegreesToRadians(dir.y),
        strength,
        collisionDetection
    );
}

/*
* @opcode 0689
* @command POP_CAR_DOOR
* @class Car
* @method PopDoor
* 
* @brief Removes the specified car door component from the car
* 
* @param self CVehicle&
* @param door eCarDoor
* @param visibility bool
*/
void PopCarDoor(CVehicle& self, eDoors door, bool visibility) {
    auto* const automobile = self.AsAutomobile();
    automobile->PopDoor(CDamageManager::GetCarNodeIndexFromDoor(door), door, visibility);
}

/*
* @opcode 068A
* @command FIX_CAR_DOOR
* @class Car
* @method FixDoor
* 
* @brief Repairs the car door
* 
* @param self CVehicle&
* @param door eCarDoor
*/
void FixCarDoor(CVehicle& self, eDoors door) {
    auto* const automobile = self.AsAutomobile();
    automobile->FixDoor(CDamageManager::GetCarNodeIndexFromDoor(door), door);
}

/*
* @opcode 068B
* @command TASK_EVERYONE_LEAVE_CAR
* @class Car
* @method TaskEveryoneLeave
* 
* @brief Makes all passengers of the car leave it
* 
* @param self CVehicle&
*/
// void TaskEveryoneLeaveCar(CVehicle& self) {
//     NOTSA_UNREACHABLE("Not implemented");
// }

/*
* @opcode 0697
* @command POP_CAR_PANEL
* @class Car
* @method PopPanel
* 
* @brief Detatches or deletes car's body part
* 
* @param self CVehicle&
* @param panelId eCarPanel
* @param drop bool
*/
void PopCarPanel(CVehicle& self, ePanels panel, bool drop) {
    auto* const automobile = self.AsAutomobile();
    automobile->PopPanel(CDamageManager::GetCarNodeIndexFromPanel(panel), panel, drop);
}

/*
* @opcode 0698
* @command FIX_CAR_PANEL
* @class Car
* @method FixPanel
* 
* @brief Repairs or reinstalls car's body part
* 
* @param self CVehicle&
* @param panelId eCarPanel
*/
void FixCarPanel(CVehicle& self, ePanels panel) {
    auto* const automobile = self.AsAutomobile();
    automobile->FixPanel(CDamageManager::GetCarNodeIndexFromPanel(panel), panel);
}

/*
* @opcode 0699
* @command FIX_CAR_TYRE
* @class Car
* @method FixTire
* 
* @brief Repairs a car's tire
* 
* @param self CVehicle&
* @param tireId eCarWheel
*/
void FixCarTyre(CVehicle& self, eCarWheel tire) {
    self.AsAutomobile()->FixTyre(static_cast<eCarWheel>(tire));
}

/*
* @opcode 06A2
* @command GET_CAR_SPEED_VECTOR
* @class Car
* @method GetSpeedVector
* 
* @brief 
* 
* @param self CVehicle&
*/
auto GetCarSpeedVector(CVehicle& self) {
    return self.GetMoveSpeed() * 50.f;
}

/*
* @opcode 06A3
* @command GET_CAR_MASS
* @class Car
* @method GetMass
* 
* @brief Returns the vehicle's mass
* 
* @param self CVehicle&
*/
float GetCarMass(CVehicle& self) {
    return self.GetMass();
}

/*
* @opcode 06BE
* @command GET_CAR_ROLL
* @class Car
* @method GetRoll
* 
* @brief Returns the Y Angle of the vehicle
* 
* @param self CVehicle&
*/
float GetCarRoll(CVehicle& self) {
    return self.GetRoll();
}

/*
* @opcode 06C5
* @command SKIP_TO_END_AND_STOP_PLAYBACK_RECORDED_CAR
* @class Car
* @method SkipToEndAndStopPlayback
* 
* @brief 
* 
* @param self CVehicle&
*/
void SkipToEndAndStopPlaybackRecordedCar(CVehicle& self) {
    CVehicleRecording::SkipToEndAndStopPlaybackRecordedCar(&self);
}

/*
* @opcode 06FC
* @command DOES_CAR_HAVE_STUCK_CAR_CHECK
* @class Car
* @method DoesHaveStuckCarCheck
* 
* @brief Returns true if the car has car stuck check enabled
* 
* @param self CVehicle&
*/
bool DoesCarHaveStuckCarCheck(notsa::script::ScriptEntity<CVehicle> self) {
    return CTheScripts::StuckCars.IsCarInStuckCarArray(self.h);
}

/*
* @opcode 0705
* @command START_PLAYBACK_RECORDED_CAR_USING_AI
* @class Car
* @method StartPlaybackUsingAi
* 
* @brief Starts the playback of a recorded car with driver AI enabled
* 
* @param self CVehicle&
* @param pathId int
*/
void StartPlaybackRecordedCarUsingAI(CVehicle& self, int pathId) {
    CVehicleRecording::StartPlaybackRecordedCar(&self, pathId, true, false);
}

/*
* @opcode 0706
* @command SKIP_IN_PLAYBACK_RECORDED_CAR
* @class Car
* @method SkipInPlayback
* 
* @brief Advances the recorded car playback by the specified amount
* 
* @param self CVehicle&
* @param amount float
*/
void SkipInPlaybackRecordedCar(CVehicle& self, float amount) {
    CVehicleRecording::SkipForwardInRecording(&self, amount);
}

/*
* @opcode 070C
* @command EXPLODE_CAR_IN_CUTSCENE
* @class Car
* @method ExplodeInCutscene
* 
* @brief Makes the vehicle explode without affecting its surroundings
* 
* @param self CVehicle&
*/
void ExplodeCarInCutscene(CVehicle& self) {
    self.vehicleFlags.bCanBeDamaged = true;
    self.BlowUpCar(nullptr, true);
}

/*
* @opcode 0714
* @command SET_CAR_STAY_IN_SLOW_LANE
* @class Car
* @method SetStayInSlowLane
* 
* @brief 
* 
* @param self CVehicle&
* @param state bool
*/
void SetCarStayInSlowLane(CVehicle& self, bool state) {
    self.m_autoPilot.carCtrlFlags.bStayInSlowLane = state;
}

/*
* @opcode 0730
* @command DAMAGE_CAR_PANEL
* @class Car
* @method DamagePanel
* 
* @brief Damages a panel on the car
* 
* @param self CVehicle&
* @param panel CarPanel
*/
void DamageCarPanel(CVehicle& self, ePanels panel) {
    assert(panel <= MAX_PANELS);

    auto* const automobile = self.AsAutomobile();
    automobile->GetDamageManager().ApplyDamage(
        automobile,
        (tComponent)(+COMPONENT_WING_LF + +panel),
        150.f,
        1.f
    );
}

/*
* @opcode 0731
* @command SET_CAR_ROLL
* @class Car
* @method SetRoll
* 
* @brief Sets the Y Angle of the vehicle to the specified value
* 
* @param self CVehicle&
* @param roll float
*/
void SetCarRoll(CVehicle& self, float roll) {
    self.SetRoll(roll);
}

/*
* @opcode 0732
* @command SUPPRESS_CAR_MODEL
* @class CarGenerator
* @method SuppressCarModel
* 
* @brief Prevents the specified car model from spawning for car generators
* 
* @param model eModelID
*/
void SuppressCarModel(eModelID model) {
    CTheScripts::AddToSuppressedCarModelArray(model);
}

/*
* @opcode 0733
* @command DONT_SUPPRESS_CAR_MODEL
* @class CarGenerator
* @method DontSuppressCarModel
* 
* @brief Allows the specified car model to spawn for car generators
* 
* @param modelId eModelID
*/
void DontSuppressCarModel(eModelID modelId) {
    CTheScripts::RemoveFromSuppressedCarModelArray(modelId);
}

/*
* @opcode 0734
* @command DONT_SUPPRESS_ANY_CAR_MODELS
* @class CarGenerator
* @method DontSuppressAnyCarModels
* 
* @brief Resets the disabled car model list for car generators
*/
void DontSuppressAnyCarModels() {
    CTheScripts::ClearAllSuppressedCarModels();
}

/*
* @opcode 073B
* @command SET_CAR_CAN_GO_AGAINST_TRAFFIC
* @class Car
* @method SetCanGoAgainstTraffic
* 
* @brief Sets whether the vehicle will drive the wrong way on roads
* 
* @param self CVehicle&
* @param state bool
*/
void SetCarCanGoAgainstTraffic(CVehicle& self, bool state) {
    self.m_autoPilot.carCtrlFlags.bCantGoAgainstTraffic = state;
}

/*
* @opcode 073C
* @command DAMAGE_CAR_DOOR
* @class Car
* @method DamageDoor
* 
* @brief Damages a component on the vehicle
* 
* @param self CVehicle&
* @param door eCarDoor
*/
// void DamageCarDoor(CVehicle& self, eCarDoor door) {
//     NOTSA_UNREACHABLE("Not implemented");
// }

/*
* @opcode 0763
* @command SET_CAR_AS_MISSION_CAR
* @class Car
* @method SetAsMissionCar
* 
* @brief Sets the script as the owner of the vehicle and adds it to the mission cleanup list
* 
* @param self CVehicle&
*/
// void SetCarAsMissionCar(CVehicle& self) {
//     NOTSA_UNREACHABLE("Not implemented");
// }

/*
* @opcode 077D
* @command GET_CAR_PITCH
* @class Car
* @method GetPitch
* 
* @brief Returns the X Angle of the vehicle
* 
* @param self CVehicle&
*/
// float GetCarPitch(CVehicle& self) {
//     NOTSA_UNREACHABLE("Not implemented");
// }

/*
* @opcode 07D5
* @command APPLY_FORCE_TO_CAR
* @class Car
* @method ApplyForce
* 
* @brief 
* 
* @param self CVehicle&
* @param xOffset float
* @param yOffset float
* @param zOffset float
* @param xRotation float
* @param yRotation float
* @param zRotation float
*/
// void ApplyForceToCar(CVehicle& self, CVector offset, CVector rotation) {
//     NOTSA_UNREACHABLE("Not implemented");
// }

/*
* @opcode 07DA
* @command ADD_TO_CAR_ROTATION_VELOCITY
* @class Car
* @method AddToRotationVelocity
* 
* @brief 
* 
* @param self CVehicle&
* @param x float
* @param y float
* @param z float
*/
// void AddToCarRotationVelocity(CVehicle& self, CVector vec1) {
//     NOTSA_UNREACHABLE("Not implemented");
// }

/*
* @opcode 07DB
* @command SET_CAR_ROTATION_VELOCITY
* @class Car
* @method SetRotationVelocity
* 
* @brief 
* 
* @param self CVehicle&
* @param x float
* @param y float
* @param z float
*/
// void SetCarRotationVelocity(CVehicle& self, CVector vec1) {
//     NOTSA_UNREACHABLE("Not implemented");
// }

/*
* @opcode 07F5
* @command CONTROL_CAR_HYDRAULICS
* @class Car
* @method ControlHydraulics
* 
* @brief Changes the car wheels' suspension level
* 
* @param self CVehicle&
* @param frontLeftWheelSuspension float
* @param rearLeftWheelSuspension float
* @param frontRightWheelSuspension float
* @param rearRightWheelSuspension float
*/
// void ControlCarHydraulics(CVehicle& self, float frontLeftWheelSuspension, float rearLeftWheelSuspension, float frontRightWheelSuspension, float rearRightWheelSuspension) {
//     NOTSA_UNREACHABLE("Not implemented");
// }

/*
* @opcode 07F8
* @command SET_CAR_FOLLOW_CAR
* @class Car
* @method SetFollowCar
* 
* @brief 
* 
* @param self CVehicle&
* @param handle CVehicle&
* @param radius float
*/
// void SetCarFollowCar(CVehicle& self, CVehicle& handle, float radius) {
//     NOTSA_UNREACHABLE("Not implemented");
// }

/*
* @opcode 07FF
* @command SET_CAR_HYDRAULICS
* @class Car
* @method SetHydraulics
* 
* @brief Enables hydraulic suspension on the car
* 
* @param self CVehicle&
* @param state bool
*/
// void SetCarHydraulics(CVehicle& self, bool state) {
//     NOTSA_UNREACHABLE("Not implemented");
// }

/*
* @opcode 0803
* @command DOES_CAR_HAVE_HYDRAULICS
* @class Car
* @method DoesHaveHydraulics
* 
* @brief Returns true if the car has hydraulics installed
* 
* @param self CVehicle&
*/
// void DoesCarHaveHydraulics(CVehicle& self) {
//     NOTSA_UNREACHABLE("Not implemented");
// }

/*
* @opcode 081D
* @command SET_CAR_ENGINE_BROKEN
* @class Car
* @method SetEngineBroken
* 
* @brief Sets whether the car's engine is broken
* 
* @param self CVehicle&
* @param state bool
*/
// void SetCarEngineBroken(CVehicle& self, bool state) {
//     NOTSA_UNREACHABLE("Not implemented");
// }

/*
* @opcode 083F
* @command GET_CAR_UPRIGHT_VALUE
* @class Car
* @method GetUprightValue
* 
* @brief Gets the car's vertical angle
* 
* @param self CVehicle&
*/
// float GetCarUprightValue(CVehicle& self) {
//     NOTSA_UNREACHABLE("Not implemented");
// }

/*
* @opcode 0852
* @command SET_CAR_CAN_BE_VISIBLY_DAMAGED
* @class Car
* @method SetCanBeVisiblyDamaged
* 
* @brief Sets whether the vehicle can be visibly damaged
* 
* @param self CVehicle&
* @param state bool
*/
// void SetCarCanBeVisiblyDamaged(CVehicle& self, bool state) {
//     NOTSA_UNREACHABLE("Not implemented");
// }

/*
* @opcode 085E
* @command START_PLAYBACK_RECORDED_CAR_LOOPED
* @class Car
* @method StartPlaybackLooped
* 
* @brief Starts looped playback of a recorded car path
* 
* @param self CVehicle&
* @param pathId int
*/
// void StartPlaybackRecordedCarLooped(CVehicle& self, int pathId) {
//     NOTSA_UNREACHABLE("Not implemented");
// }

/*
* @opcode 085F
* @command START_PLAYBACK_RECORDED_CAR_USING_AI_LOOPED
* 
* @brief Does nothing
*/
// void StartPlaybackRecordedCarUsingAiLooped() {
//     /* no-op */
// }

/*
* @opcode 088C
* @command SET_CAR_COORDINATES_NO_OFFSET
* @class Car
* @method SetCoordinatesNoOffset
* 
* @brief Sets the vehicle coordinates without applying offsets to account for the height of the vehicle
* 
* @param self CVehicle&
* @param x float
* @param y float
* @param z float
*/
// void SetCarCoordinatesNoOffset(CVehicle& self, CVector vec1) {
//     NOTSA_UNREACHABLE("Not implemented");
// }

/*
* @opcode 08A6
* @command OPEN_CAR_DOOR_A_BIT
* @class Car
* @method OpenDoorABit
* 
* @brief Sets the angle of a car door
* 
* @param self CVehicle&
* @param door eCarDoor
* @param value float
*/
// void OpenCarDoorABit(CVehicle& self, eCarDoor door, float value) {
//     NOTSA_UNREACHABLE("Not implemented");
// }

/*
* @opcode 08A7
* @command IS_CAR_DOOR_FULLY_OPEN
* @class Car
* @method IsDoorFullyOpen
* 
* @brief 
* 
* @param self CVehicle&
* @param door eCarDoor
*/
// void IsCarDoorFullyOpen(CVehicle& self, eCarDoor door) {
//     NOTSA_UNREACHABLE("Not implemented");
// }

/*
* @opcode 08CB
* @command EXPLODE_CAR_IN_CUTSCENE_SHAKE_AND_BITS
* @class Car
* @method ExplodeInCutsceneShakeAndBits
* 
* @brief Causes the vehicle to explode, without damage to surrounding entities
* 
* @param self CVehicle&
* @param shake bool
* @param effect bool
* @param sound bool
*/
// void ExplodeCarInCutsceneShakeAndBits(CVehicle& self, bool shake, bool effect, bool sound) {
//     NOTSA_UNREACHABLE("Not implemented");
// }

/*
* @opcode 0918
* @command SET_CAR_ENGINE_ON
* @class Car
* @method SetEngineOn
* 
* @brief Sets whether the vehicle's engine is turned on or off
* 
* @param self CVehicle&
* @param state bool
*/
// void SetCarEngineOn(CVehicle& self, bool state) {
//     NOTSA_UNREACHABLE("Not implemented");
// }

/*
* @opcode 0919
* @command SET_CAR_LIGHTS_ON
* @class Car
* @method SetLightsOn
* 
* @brief Sets whether the vehicle's lights are on
* 
* @param self CVehicle&
* @param state bool
*/
// void SetCarLightsOn(CVehicle& self, bool state) {
//     NOTSA_UNREACHABLE("Not implemented");
// }

/*
* @opcode 0939
* @command ATTACH_CAR_TO_OBJECT
* @class Car
* @method AttachToObject
* 
* @brief Attaches the car to object with offset and rotation
* 
* @param self CVehicle&
* @param handle Object
* @param xOffset float
* @param yOffset float
* @param zOffset float
* @param xRotation float
* @param yRotation float
* @param zRotation float
*/
// void AttachCarToObject(CVehicle& self, Object handle, CVector offset, CVector rotation) {
//     NOTSA_UNREACHABLE("Not implemented");
// }

/*
* @opcode 095E
* @command CONTROL_CAR_DOOR
* @class Car
* @method ControlDoor
* 
* @brief Sets the car's door angle and latch state
* 
* @param self CVehicle&
* @param door eCarDoor
* @param state eCarDoorState
* @param angle float
*/
// void ControlCarDoor(CVehicle& self, eCarDoor door, eCarDoorState state, float angle) {
//     NOTSA_UNREACHABLE("Not implemented");
// }

/*
* @opcode 096B
* @command STORE_CAR_MOD_STATE
* @class Car
* @method StoreModState
* 
* @brief 
*/
// void StoreCarModState() {
//     NOTSA_UNREACHABLE("Not implemented");
// }

/*
* @opcode 096C
* @command RESTORE_CAR_MOD_STATE
* @class Car
* @method RestoreModState
* 
* @brief 
*/
// void RestoreCarModState() {
//     NOTSA_UNREACHABLE("Not implemented");
// }

/*
* @opcode 096D
* @command GET_CURRENT_CAR_MOD
* @class Car
* @method GetCurrentMod
* 
* @brief Returns the model of the component installed on the specified slot of the vehicle, or -1 otherwise
* 
* @param self CVehicle&
* @param slot eModSlot
*/
// model_object GetCurrentCarMod(CVehicle& self, eModSlot slot) {
//     NOTSA_UNREACHABLE("Not implemented");
// }

/*
* @opcode 096E
* @command IS_CAR_LOW_RIDER
* @class Car
* @method IsLowRider
* 
* @brief Returns true if the vehicle is a low rider
* 
* @param self CVehicle&
*/
// void IsCarLowRider(CVehicle& self) {
//     NOTSA_UNREACHABLE("Not implemented");
// }

/*
* @opcode 096F
* @command IS_CAR_STREET_RACER
* @class Car
* @method IsStreetRacer
* 
* @brief Returns true if the vehicle is a street racer
* 
* @param self CVehicle&
*/
// void IsCarStreetRacer(CVehicle& self) {
//     NOTSA_UNREACHABLE("Not implemented");
// }

/*
* @opcode 097D
* @command GET_NUM_CAR_COLOURS
* @class Car
* @method GetNumColors
* 
* @brief Returns number of color variations defined for the model of this car in carcols.dat
* 
* @param self CVehicle&
*/
// int GetNumCarColours(CVehicle& self) {
//     NOTSA_UNREACHABLE("Not implemented");
// }

/*
* @opcode 0987
* @command GET_CAR_BLOCKING_CAR
* @class Car
* @method GetBlockingCar
* 
* @brief Returns a handle of the vehicle preventing this car from getting to its destination
* 
* @param self CVehicle&
*/
// CVehicle& GetCarBlockingCar(CVehicle& self) {
//     NOTSA_UNREACHABLE("Not implemented");
// }

/*
* @opcode 098D
* @command GET_CAR_MOVING_COMPONENT_OFFSET
* @class Car
* @method GetMovingComponentOffset
* 
* @brief Sets the angle of a vehicle's extra
* 
* @param self CVehicle&
*/
// float GetCarMovingComponentOffset(CVehicle& self) {
//     NOTSA_UNREACHABLE("Not implemented");
// }

/*
* @opcode 099A
* @command SET_CAR_COLLISION
* @class Car
* @method SetCollision
* 
* @brief 
* 
* @param self CVehicle&
* @param state bool
*/
// void SetCarCollision(CVehicle& self, bool state) {
//     NOTSA_UNREACHABLE("Not implemented");
// }

/*
* @opcode 09B3
* @command GET_CAR_DOOR_LOCK_STATUS
* @class Car
* @method GetDoorLockStatus
* 
* @brief Returns the door lock mode of the vehicle
* 
* @param self CVehicle&
*/
// eCarLock GetCarDoorLockStatus(CVehicle& self) {
//     NOTSA_UNREACHABLE("Not implemented");
// }

/*
* @opcode 09BB
* @command IS_CAR_DOOR_DAMAGED
* @class Car
* @method IsDoorDamaged
* 
* @brief Returns true if the specified vehicle part is visibly damaged
* 
* @param self CVehicle&
* @param door eCarDoor
*/
// void IsCarDoorDamaged(CVehicle& self, eCarDoor door) {
//     NOTSA_UNREACHABLE("Not implemented");
// }

/*
* @opcode 09CB
* @command IS_CAR_TOUCHING_CAR
* @class Car
* @method IsTouchingCar
* 
* @brief Returns true if the car is touching the other car
* 
* @param self CVehicle&
* @param handle CVehicle&
*/
// void IsCarTouchingCar(CVehicle& self, CVehicle& handle) {
//     NOTSA_UNREACHABLE("Not implemented");
// }

/*
* @opcode 09E1
* @command GET_CAR_MODEL_VALUE
* @class Car
* @method GetModelValue
* 
* @brief Returns the value of the specified car model
* 
* @param model eModelID
*/
// int GetCarModelValue(eModelID model) {
//     NOTSA_UNREACHABLE("Not implemented");
// }

/*
* @opcode 09E2
* @command CREATE_CAR_GENERATOR_WITH_PLATE
* @class CarGenerator
* @method CreateWithPlate
* 
* @brief Creates a parked car generator with a number plate (modelId -1 selects a random vehicle from the local popcycle)
* 
* @param x float
* @param y float
* @param z float
* @param heading float
* @param modelId eModelID
* @param primaryColor int
* @param secondaryColor int
* @param forceSpawn bool
* @param alarmChance int
* @param doorLockChance int
* @param minDelay int
* @param maxDelay int
* @param plateName std::string_view
*/
// CarGenerator CreateCarGeneratorWithPlate(CVector vec0, float heading, eModelID modelId, int primaryColor, int secondaryColor, bool forceSpawn, int alarmChance, int doorLockChance, int minDelay, int maxDelay, std::string_view plateName) {
//     NOTSA_UNREACHABLE("Not implemented");
// }

/*
* @opcode 09E9
* @command GIVE_NON_PLAYER_CAR_NITRO
* @class Car
* @method GiveNonPlayerNitro
* 
* @brief Makes the car have one nitro
* 
* @param self CVehicle&
*/
// void GiveNonPlayerCarNitro(CVehicle& self) {
//     NOTSA_UNREACHABLE("Not implemented");
// }

/*
* @opcode 0A11
* @command SET_EXTRA_CAR_COLOURS
* @class Car
* @method SetExtraColors
* 
* @brief Sets the car's ternary and quaternary colors. See also 0229
* 
* @param self CVehicle&
* @param color3 int
* @param color4 int
*/
// void SetExtraCarColours(CVehicle& self, int color3, int color4) {
//     NOTSA_UNREACHABLE("Not implemented");
// }

/*
* @opcode 0A15
* @command HAS_CAR_BEEN_RESPRAYED
* @class Car
* @method HasBeenResprayed
* 
* @brief Returns true if the vehicle was resprayed in the last frame
* 
* @param self CVehicle&
*/
// void HasCarBeenResprayed(CVehicle& self) {
//     NOTSA_UNREACHABLE("Not implemented");
// }

/*
* @opcode 0A21
* @command IMPROVE_CAR_BY_CHEATING
* @class Car
* @method ImproveByCheating
* 
* @brief Sets whether a ped driven vehicle's handling is affected by the 'perfect handling' cheat
* 
* @param self CVehicle&
* @param state bool
*/
// void ImproveCarByCheating(CVehicle& self, bool state) {
//     NOTSA_UNREACHABLE("Not implemented");
// }
}; // namespace

void notsa::script::commands::vehicle::RegisterHandlers() {
    REGISTER_COMMAND_HANDLER_BEGIN("Vehicle");

    REGISTER_COMMAND_HANDLER(COMMAND_CLEAR_HELI_ORIENTATION, ClearHeliOrientation);
    REGISTER_COMMAND_HANDLER(COMMAND_REMOVE_RC_BUGGY, RemoveRCBuggy);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_PROOFS, SetCarProofs);
    REGISTER_COMMAND_HANDLER(COMMAND_SWITCH_CAR_GENERATOR, SwitchCarGenerator);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_HAS_BEEN_OWNED_FOR_CAR_GENERATOR, SetHasBeenOwnedForCarGenerator);
    REGISTER_COMMAND_HANDLER(COMMAND_GET_CAR_SPEED, GetCarSpeed);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_DRIVING_STYLE, SetCarDrivingStyle);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_FIRST_CAR_COLOUR, IsFirstCarColor);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_SECOND_CAR_COLOUR, IsSecondCarColor);
    REGISTER_COMMAND_HANDLER(COMMAND_GET_EXTRA_CAR_COLOURS, GetExtraCarColors);
    REGISTER_COMMAND_HANDLER(COMMAND_POP_CAR_BOOT_USING_PHYSICS, PopCarBootUsingPhysics);
    REGISTER_COMMAND_HANDLER(COMMAND_SKIP_TO_NEXT_ALLOWED_STATION, SkipToNextAllowedStation);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_RAILTRACK_RESISTANCE_MULT, SetRailTrackResistanceMult);
    REGISTER_COMMAND_HANDLER(COMMAND_DISABLE_HELI_AUDIO, DisableHeliAudio);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_CAR_IN_AIR_PROPER, IsCarInAirProper);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_CAR_STUCK, IsCarStuck);
    REGISTER_COMMAND_HANDLER(COMMAND_ADD_STUCK_CAR_CHECK, AddStuckCarCheck);
    REGISTER_COMMAND_HANDLER(COMMAND_REMOVE_STUCK_CAR_CHECK, RemoveStuckCarCheck);
    REGISTER_COMMAND_HANDLER(COMMAND_ADD_STUCK_CAR_CHECK_WITH_WARP, AddStuckCarCheckWithWarp);
    REGISTER_COMMAND_HANDLER(COMMAND_PLANE_ATTACK_PLAYER_USING_DOG_FIGHT, PlaneAttackPlayerUsingDogFight);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_ALWAYS_CREATE_SKIDS, SetCarAlwaysCreateSkids);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_AS_MISSION_CAR, SetCarAsMissionCar);
    REGISTER_COMMAND_HANDLER(COMMAND_CREATE_CAR, CreateCar);
    // REGISTER_COMMAND_HANDLER(COMMAND_DELETE_CAR, DeleteCar);
    REGISTER_COMMAND_HANDLER(COMMAND_CAR_GOTO_COORDINATES, CarGotoCoordinates);
    REGISTER_COMMAND_HANDLER(COMMAND_CAR_GOTO_COORDINATES_ACCURATE, CarGotoCoordinatesAccurate);
    REGISTER_COMMAND_HANDLER(COMMAND_CAR_GOTO_COORDINATES_RACING, CarGotoCoordinatesRacing);
    REGISTER_COMMAND_HANDLER(COMMAND_CAR_WANDER_RANDOMLY, CarWanderRandomly);
    REGISTER_COMMAND_HANDLER(COMMAND_CAR_SET_IDLE, CarSetIdle);
    REGISTER_COMMAND_HANDLER(COMMAND_GET_CAR_COORDINATES, GetCarCoordinates);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_COORDINATES, SetCarCoordinates);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_CRUISE_SPEED, SetCarCruiseSpeed);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_MISSION, SetCarMission);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_CAR_IN_AREA_2D, IsCarInArea2D<false>);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_CAR_IN_AREA_3D, IsCarInArea3D);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_CAR_DEAD, IsCarDead);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_CAR_MODEL, IsCarModel);
    REGISTER_COMMAND_HANDLER(COMMAND_CREATE_CAR_GENERATOR, CreateCarGenerator);
    REGISTER_COMMAND_HANDLER(COMMAND_ADD_BLIP_FOR_CAR_OLD, AddBlipForCarOld);
    REGISTER_COMMAND_HANDLER(COMMAND_GET_CAR_HEADING, GetCarHeading);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_HEADING, SetCarHeading);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_CAR_HEALTH_GREATER, IsCarHealthGreater);
    REGISTER_COMMAND_HANDLER(COMMAND_ADD_BLIP_FOR_CAR, AddBlipForCar);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_CAR_STUCK_ON_ROOF, IsCarStuckOnRoof);
    REGISTER_COMMAND_HANDLER(COMMAND_ADD_UPSIDEDOWN_CAR_CHECK, AddUpsidedownCarCheck);
    REGISTER_COMMAND_HANDLER(COMMAND_REMOVE_UPSIDEDOWN_CAR_CHECK, RemoveUpsidedownCarCheck);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_CAR_STOPPED_IN_AREA_2D, IsCarStoppedInArea2D);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_CAR_STOPPED_IN_AREA_3D, IsCarStoppedInArea3D);
    REGISTER_COMMAND_HANDLER(COMMAND_LOCATE_CAR_2D, LocateCar2D);
    REGISTER_COMMAND_HANDLER(COMMAND_LOCATE_STOPPED_CAR_2D, LocateStoppedCar2D);
    REGISTER_COMMAND_HANDLER(COMMAND_LOCATE_CAR_3D, LocateCar3D);
    REGISTER_COMMAND_HANDLER(COMMAND_LOCATE_STOPPED_CAR_3D, LocateStoppedCar3D);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_CAR_STOPPED, IsCarStopped);
    REGISTER_COMMAND_HANDLER(COMMAND_MARK_CAR_AS_NO_LONGER_NEEDED, MarkCarAsNoLongerNeeded);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_DENSITY_MULTIPLIER, SetCarDensityMultiplier);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_HEAVY, SetCarHeavy);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_CAR_UPSIDEDOWN, IsCarUpsidedown);
    REGISTER_COMMAND_HANDLER(COMMAND_LOCK_CAR_DOORS, LockCarDoors);
    REGISTER_COMMAND_HANDLER(COMMAND_EXPLODE_CAR, ExplodeCar);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_CAR_UPRIGHT, IsCarUpright);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_TARGET_CAR_FOR_MISSION_GARAGE, SetTargetCarForMissionGarage);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_HEALTH, SetCarHealth);
    REGISTER_COMMAND_HANDLER(COMMAND_GET_CAR_HEALTH, GetCarHealth);
    REGISTER_COMMAND_HANDLER(COMMAND_CHANGE_CAR_COLOUR, ChangeCarColour);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CAN_RESPRAY_CAR, SetCanResprayCar);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_ONLY_DAMAGED_BY_PLAYER, SetCarOnlyDamagedByPlayer);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_CAR_IN_WATER, IsCarInWater);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_CAR_ON_SCREEN, IsCarOnScreen);
    REGISTER_COMMAND_HANDLER(COMMAND_GET_CAR_FORWARD_X, GetCarForwardX);
    REGISTER_COMMAND_HANDLER(COMMAND_GET_CAR_FORWARD_Y, GetCarForwardY);
    REGISTER_COMMAND_HANDLER(COMMAND_HAS_CAR_BEEN_DAMAGED_BY_WEAPON, HasCarBeenDamagedByWeapon);
    REGISTER_COMMAND_HANDLER(COMMAND_START_CAR_FIRE, StartCarFire);
    REGISTER_COMMAND_HANDLER(COMMAND_GET_RANDOM_CAR_OF_TYPE_IN_AREA, GetRandomCarOfTypeInArea);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_VISIBLE, SetCarVisible);
    REGISTER_COMMAND_HANDLER(COMMAND_PLACE_OBJECT_RELATIVE_TO_CAR, PlaceObjectRelativeToCar);
    REGISTER_COMMAND_HANDLER(COMMAND_SWITCH_CAR_SIREN, SwitchCarSiren);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_WATERTIGHT, SetCarWatertight);
    REGISTER_COMMAND_HANDLER(COMMAND_TURN_CAR_TO_FACE_COORD, TurnCarToFaceCoord);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_STATUS, SetCarStatus);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_STRONG, SetCarStrong);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_CAR_VISIBLY_DAMAGED, IsCarVisiblyDamaged);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_UPSIDEDOWN_CAR_NOT_DAMAGED, SetUpsidedownCarNotDamaged);
    REGISTER_COMMAND_HANDLER(COMMAND_GET_CAR_COLOURS, GetCarColours);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_CAN_BE_DAMAGED, SetCarCanBeDamaged);
    REGISTER_COMMAND_HANDLER(COMMAND_GET_OFFSET_FROM_CAR_IN_WORLD_COORDS, GetOffsetFromCarInWorldCoords);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_TRACTION, SetCarTraction);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_AVOID_LEVEL_TRANSITIONS, SetCarAvoidLevelTransitions);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_STAY_IN_FAST_LANE, SetCarStayInFastLane);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_CAR_PASSENGER_SEAT_FREE, IsCarPassengerSeatFree);
    REGISTER_COMMAND_HANDLER(COMMAND_GET_CAR_MODEL, GetCarModel);
    REGISTER_COMMAND_HANDLER(COMMAND_CLEAR_CAR_LAST_WEAPON_DAMAGE, ClearCarLastWeaponDamage);
    REGISTER_COMMAND_HANDLER(COMMAND_GET_DRIVER_OF_CAR, GetDriverOfCar);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_TEMP_ACTION, SetCarTempAction);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_RANDOM_ROUTE_SEED, SetCarRandomRouteSeed);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_CAR_ON_FIRE, IsCarOnFire);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_CAR_TYRE_BURST, IsCarTyreBurst);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_FORWARD_SPEED, SetCarForwardSpeed);
    REGISTER_COMMAND_HANDLER(COMMAND_MARK_CAR_AS_CONVOY_CAR, MarkCarAsConvoyCar);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_STRAIGHT_LINE_DISTANCE, SetCarStraightLineDistance);
    REGISTER_COMMAND_HANDLER(COMMAND_POP_CAR_BOOT, PopCarBoot);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_CAR_WAITING_FOR_WORLD_COLLISION, IsCarWaitingForWorldCollision);
    REGISTER_COMMAND_HANDLER(COMMAND_BURST_CAR_TYRE, BurstCarTyre);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_MODEL_COMPONENTS, SetCarModelComponents);
    REGISTER_COMMAND_HANDLER(COMMAND_CLOSE_ALL_CAR_DOORS, CloseAllCarDoors);
    REGISTER_COMMAND_HANDLER(COMMAND_FREEZE_CAR_POSITION, FreezeCarPosition);
    REGISTER_COMMAND_HANDLER(COMMAND_HAS_CAR_BEEN_DAMAGED_BY_CHAR, HasCarBeenDamagedByChar);
    REGISTER_COMMAND_HANDLER(COMMAND_HAS_CAR_BEEN_DAMAGED_BY_CAR, HasCarBeenDamagedByCar);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CAN_BURST_CAR_TYRES, SetCanBurstCarTyres);
    REGISTER_COMMAND_HANDLER(COMMAND_CLEAR_CAR_LAST_DAMAGE_ENTITY, ClearCarLastDamageEntity);
    REGISTER_COMMAND_HANDLER(COMMAND_FREEZE_CAR_POSITION_AND_DONT_LOAD_COLLISION, FreezeCarPositionAndDontLoadCollision);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_LOAD_COLLISION_FOR_CAR_FLAG, SetLoadCollisionForCarFlag);

    REGISTER_COMMAND_HANDLER(COMMAND_START_PLAYBACK_RECORDED_CAR, StartPlaybackRecordedCar);
    REGISTER_COMMAND_HANDLER(COMMAND_STOP_PLAYBACK_RECORDED_CAR, StopPlaybackRecordedCar);
    REGISTER_COMMAND_HANDLER(COMMAND_PAUSE_PLAYBACK_RECORDED_CAR, PausePlaybackRecordedCar);
    REGISTER_COMMAND_HANDLER(COMMAND_UNPAUSE_PLAYBACK_RECORDED_CAR, UnpausePlaybackRecordedCar);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_PLAYBACK_GOING_ON_FOR_CAR, IsPlaybackGoingOnForCar);

    REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_ESCORT_CAR_LEFT, SetCarEscortCarLeft);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_ESCORT_CAR_RIGHT, SetCarEscortCarRight);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_ESCORT_CAR_REAR, SetCarEscortCarRear);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_ESCORT_CAR_FRONT, SetCarEscortCarFront);

    REGISTER_COMMAND_HANDLER(COMMAND_OPEN_CAR_DOOR, OpenCarDoor);
    REGISTER_COMMAND_HANDLER(COMMAND_CUSTOM_PLATE_FOR_NEXT_CAR, CustomPlateForNextCar);
    REGISTER_COMMAND_HANDLER(COMMAND_FORCE_CAR_LIGHTS, ForceCarLights);
    REGISTER_COMMAND_HANDLER(COMMAND_ATTACH_CAR_TO_CAR, AttachCarToCar);
    REGISTER_COMMAND_HANDLER(COMMAND_DETACH_CAR, DetachCar);
    REGISTER_COMMAND_HANDLER(COMMAND_POP_CAR_DOOR, PopCarDoor);
    REGISTER_COMMAND_HANDLER(COMMAND_FIX_CAR_DOOR, FixCarDoor);
    //REGISTER_COMMAND_HANDLER(COMMAND_TASK_EVERYONE_LEAVE_CAR, TaskEveryoneLeaveCar);
    REGISTER_COMMAND_HANDLER(COMMAND_POP_CAR_PANEL, PopCarPanel);
    REGISTER_COMMAND_HANDLER(COMMAND_FIX_CAR_PANEL, FixCarPanel);
    REGISTER_COMMAND_HANDLER(COMMAND_FIX_CAR_TYRE, FixCarTyre);
    REGISTER_COMMAND_HANDLER(COMMAND_GET_CAR_SPEED_VECTOR, GetCarSpeedVector);
    REGISTER_COMMAND_HANDLER(COMMAND_GET_CAR_MASS, GetCarMass);
    REGISTER_COMMAND_HANDLER(COMMAND_GET_CAR_ROLL, GetCarRoll);
    REGISTER_COMMAND_HANDLER(COMMAND_SKIP_TO_END_AND_STOP_PLAYBACK_RECORDED_CAR, SkipToEndAndStopPlaybackRecordedCar);
    REGISTER_COMMAND_HANDLER(COMMAND_DOES_CAR_HAVE_STUCK_CAR_CHECK, DoesCarHaveStuckCarCheck);
    REGISTER_COMMAND_HANDLER(COMMAND_START_PLAYBACK_RECORDED_CAR_USING_AI, StartPlaybackRecordedCarUsingAI);
    REGISTER_COMMAND_HANDLER(COMMAND_SKIP_IN_PLAYBACK_RECORDED_CAR, SkipInPlaybackRecordedCar);
    REGISTER_COMMAND_HANDLER(COMMAND_EXPLODE_CAR_IN_CUTSCENE, ExplodeCarInCutscene);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_STAY_IN_SLOW_LANE, SetCarStayInSlowLane);
    REGISTER_COMMAND_HANDLER(COMMAND_DAMAGE_CAR_PANEL, DamageCarPanel);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_ROLL, SetCarRoll);
    REGISTER_COMMAND_HANDLER(COMMAND_SUPPRESS_CAR_MODEL, SuppressCarModel);
    REGISTER_COMMAND_HANDLER(COMMAND_DONT_SUPPRESS_CAR_MODEL, DontSuppressCarModel);
    REGISTER_COMMAND_HANDLER(COMMAND_DONT_SUPPRESS_ANY_CAR_MODELS, DontSuppressAnyCarModels);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_CAN_GO_AGAINST_TRAFFIC, SetCarCanGoAgainstTraffic);
    //REGISTER_COMMAND_HANDLER(COMMAND_DAMAGE_CAR_DOOR, DamageCarDoor);
    //REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_AS_MISSION_CAR, SetCarAsMissionCar);
    //REGISTER_COMMAND_HANDLER(COMMAND_GET_CAR_PITCH, GetCarPitch);
    //REGISTER_COMMAND_HANDLER(COMMAND_APPLY_FORCE_TO_CAR, ApplyForceToCar);
    //REGISTER_COMMAND_HANDLER(COMMAND_ADD_TO_CAR_ROTATION_VELOCITY, AddToCarRotationVelocity);
    //REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_ROTATION_VELOCITY, SetCarRotationVelocity);
    //REGISTER_COMMAND_HANDLER(COMMAND_CONTROL_CAR_HYDRAULICS, ControlCarHydraulics);
    //REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_FOLLOW_CAR, SetCarFollowCar);
    //REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_HYDRAULICS, SetCarHydraulics);
    //REGISTER_COMMAND_HANDLER(COMMAND_DOES_CAR_HAVE_HYDRAULICS, DoesCarHaveHydraulics);
    //REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_ENGINE_BROKEN, SetCarEngineBroken);
    //REGISTER_COMMAND_HANDLER(COMMAND_GET_CAR_UPRIGHT_VALUE, GetCarUprightValue);
    //REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_CAN_BE_VISIBLY_DAMAGED, SetCarCanBeVisiblyDamaged);
    //REGISTER_COMMAND_HANDLER(COMMAND_START_PLAYBACK_RECORDED_CAR_LOOPED, StartPlaybackRecordedCarLooped);
    //REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_COORDINATES_NO_OFFSET, SetCarCoordinatesNoOffset);
    //REGISTER_COMMAND_HANDLER(COMMAND_OPEN_CAR_DOOR_A_BIT, OpenCarDoorABit);
    //REGISTER_COMMAND_HANDLER(COMMAND_IS_CAR_DOOR_FULLY_OPEN, IsCarDoorFullyOpen);
    //REGISTER_COMMAND_HANDLER(COMMAND_EXPLODE_CAR_IN_CUTSCENE_SHAKE_AND_BITS, ExplodeCarInCutsceneShakeAndBits);
    //REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_ENGINE_ON, SetCarEngineOn);
    //REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_LIGHTS_ON, SetCarLightsOn);
    //REGISTER_COMMAND_HANDLER(COMMAND_ATTACH_CAR_TO_OBJECT, AttachCarToObject);
    //REGISTER_COMMAND_HANDLER(COMMAND_CONTROL_CAR_DOOR, ControlCarDoor);
    //REGISTER_COMMAND_HANDLER(COMMAND_STORE_CAR_MOD_STATE, StoreCarModState);
    //REGISTER_COMMAND_HANDLER(COMMAND_RESTORE_CAR_MOD_STATE, RestoreCarModState);
    //REGISTER_COMMAND_HANDLER(COMMAND_GET_CURRENT_CAR_MOD, GetCurrentCarMod);
    //REGISTER_COMMAND_HANDLER(COMMAND_IS_CAR_LOW_RIDER, IsCarLowRider);
    //REGISTER_COMMAND_HANDLER(COMMAND_IS_CAR_STREET_RACER, IsCarStreetRacer);
    //REGISTER_COMMAND_HANDLER(COMMAND_GET_NUM_CAR_COLOURS, GetNumCarColours);
    //REGISTER_COMMAND_HANDLER(COMMAND_GET_CAR_BLOCKING_CAR, GetCarBlockingCar);
    //REGISTER_COMMAND_HANDLER(COMMAND_GET_CAR_MOVING_COMPONENT_OFFSET, GetCarMovingComponentOffset);
    //REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_COLLISION, SetCarCollision);
    //REGISTER_COMMAND_HANDLER(COMMAND_GET_CAR_DOOR_LOCK_STATUS, GetCarDoorLockStatus);
    //REGISTER_COMMAND_HANDLER(COMMAND_IS_CAR_DOOR_DAMAGED, IsCarDoorDamaged);
    //REGISTER_COMMAND_HANDLER(COMMAND_IS_CAR_TOUCHING_CAR, IsCarTouchingCar);
    //REGISTER_COMMAND_HANDLER(COMMAND_GET_CAR_MODEL_VALUE, GetCarModelValue);
    //REGISTER_COMMAND_HANDLER(COMMAND_CREATE_CAR_GENERATOR_WITH_PLATE, CreateCarGeneratorWithPlate);
    //REGISTER_COMMAND_HANDLER(COMMAND_GIVE_NON_PLAYER_CAR_NITRO, GiveNonPlayerCarNitro);
    //REGISTER_COMMAND_HANDLER(COMMAND_SET_EXTRA_CAR_COLOURS, SetExtraCarColours);
    //REGISTER_COMMAND_HANDLER(COMMAND_HAS_CAR_BEEN_RESPRAYED, HasCarBeenResprayed);
    //REGISTER_COMMAND_HANDLER(COMMAND_IMPROVE_CAR_BY_CHEATING, ImproveCarByCheating);
    //REGISTER_COMMAND_HANDLER(COMMAND_FIX_CAR, FixCar);
    
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_TAXI);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SWITCH_TAXI_TIMER);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_BOAT);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_GET_NUMBER_OF_CARS_COLLECTED_BY_GARAGE);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_HAS_CAR_BEEN_TAKEN_TO_GARAGE);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_GET_RANDOM_CAR_OF_TYPE_IN_ZONE);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_HAS_RESPRAY_HAPPENED);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_CAR_RAM_CAR);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_CAR_BLOCK_CAR);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_CAR_FUNNY_SUSPENSION);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_CAR_BIG_WHEELS);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SWITCH_CAR_RADIO);
}
