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
#include "MissionCleanup.h"
#include <cHandlingDataMgr.h>

#include "Tasks/TaskTypes/TaskComplexLeaveAnyCar.h"

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

enum class eModSlot {
    HOOD           = 0,
    VENTS          = 1,
    SPOILERS       = 2,
    SIDE_SKIRTS    = 3,
    FRONT_BULLBARS = 4,
    REAR_BULLBARS  = 5,
    LIGHTS         = 6,
    ROOF           = 7,
    NITRO          = 8,
    HYDRAULICS     = 9,
    CAR_STEREO     = 10,
    P11            = 11,
    WHEELS         = 12,
    EXHAUSTS       = 13,
    FRONT_BUMPER   = 14,
    REAR_BUMPER    = 15,
    P16            = 16,
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

void ConvertScriptTextToCarPlate(char (&out)[9], std::string_view in) {
    for (auto [i, chr] : rngv::enumerate(in)) {
        out[i] = chr == '_' || !chr ? ' ' : chr;
    }
    out[8] = '\0';
}
}; // namespace

/*!
 * Various vehicle commands
 */
namespace {
/*
 * @opcode 04D1
 * @command CLEAR_HELI_ORIENTATION
 * @class Heli
 * @method ClearOrientation
 * 
 * @brief Resets the heli rotation set with 04D0
 * 
 * @param {Heli} self
 */
void ClearHeliOrientation(CHeli& heli) {
    heli.ClearHeliOrientation();
}

/*
 * @opcode 04DB
 * @command REMOVE_RC_BUGGY
 * @class Rc
 * @method RemoveBuggy
 * @static
 * 
 * @brief Exits remote-control mode
 */
void RemoveRCBuggy() {
    FindPlayerInfo().BlowUpRCBuggy(false);
}

/*
 * @opcode 02AC
 * @command SET_CAR_PROOFS
 * @class Car
 * @method SetProofs
 * 
 * @brief Sets the vehicle's immunities
 * 
 * @param {Car} self
 * @param {bool} bulletProof
 * @param {bool} fireProof
 * @param {bool} explosionProof
 * @param {bool} collisionProof
 * @param {bool} meleeProof
 */
void SetCarProofs(CVehicle& veh, bool bullet, bool fire, bool explosion, bool collision, bool melee) {
    auto& flags           = veh.physicalFlags;
    flags.bBulletProof    = bullet;
    flags.bFireProof      = fire;
    flags.bExplosionProof = explosion;
    flags.bCollisionProof = collision;
    flags.bMeleeProof     = melee;
}

/*
 * @opcode 014C
 * @command SWITCH_CAR_GENERATOR
 * @class CarGenerator
 * @method Switch
 * 
 * @brief Specifies the number of times the car generator spawns a car (101 - infinite)
 * 
 * @param {CarGenerator} self
 * @param {int} amount
 */
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

/*
 * @opcode 0A17
 * @command SET_HAS_BEEN_OWNED_FOR_CAR_GENERATOR
 * @class CarGenerator
 * @method SetHasBeenOwned
 * 
 * @brief Sets whether the player will not receive a wanted level when entering a vehicle
 * @brief from this generator when the police is around
 * 
 * @param {CarGenerator} self
 * @param {bool} state
 */
void SetHasBeenOwnedForCarGenerator(int32 generatorId, bool alreadyOwned) {
    CTheCarGenerators::Get(generatorId)->bPlayerHasAlreadyOwnedCar = alreadyOwned;
}

/*
 * @opcode 02E3
 * @command GET_CAR_SPEED
 * @class Car
 * @method GetSpeed
 * 
 * @brief Gets the car's speed
 * 
 * @param {Car} self
 * 
 * @returns {float} speed
 */
float GetCarSpeed(CVehicle& veh) {
    return veh.m_vecMoveSpeed.Magnitude() * 50.f;
}

/*
 * @opcode 00AE
 * @command SET_CAR_DRIVING_STYLE
 * @class Car
 * @method SetDrivingStyle
 * 
 * @brief Sets the behavior of the vehicle's AI driver
 * 
 * @param {Car} self
 * @param {DrivingMode} drivingStyle
 */
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

/*
 * @opcode 0A07
 * @command SKIP_TO_NEXT_ALLOWED_STATION
 * @class Train
 * @method SkipToNextAllowedStation
 * 
 * @brief Puts the script created train at the next allowed station
 * 
 * @param {Train} self
 */
void SkipToNextAllowedStation(CTrain& train) {
    CTrain::SkipToNextAllowedStation(&train);
}

/*
 * @opcode 0A45
 * @command SET_RAILTRACK_RESISTANCE_MULT
 * @class World
 * @method SetRailtrackResistanceMult
 * @static
 * 
 * @brief Sets the friction/slowdown rate on all rail tracks
 * 
 * @param {float} mult
 */
void SetRailTrackResistanceMult(float value) {
    CVehicle::ms_fRailTrackResistance = CVehicle::ms_fRailTrackResistanceDefault * (value > 0.0f ? value : 1.0f);
}

/*
 * @opcode 0A1C
 * @command DISABLE_HELI_AUDIO
 * @class Heli
 * @method DisableAudio
 * 
 * @brief Sets whether the helicopter sound is muted
 * 
 * @param {Heli} self
 * @param {bool} state
 */
void DisableHeliAudio(CVehicle& self, bool enable) {
    if (enable) {
        self.m_vehicleAudio.EnableHelicoptor();
    } else {
        self.m_vehicleAudio.DisableHelicoptor();
    }
}

/*
 * @opcode 01F3
 * @command IS_CAR_IN_AIR_PROPER
 * @class Car
 * @method IsInAirProper
 * 
 * @brief Returns true if the vehicle is in the air
 * 
 * @param {Car} self
 */
bool IsCarInAirProper(CRunningScript& S, CVehicle& self) {
    for (auto* const e : self.GetCollidingEntities()) {
        if (e && (e->GetIsTypeBuilding() || e->GetIsTypeVehicle())) {
            return false;
        }
    }
    return true;
}

/*
 * @opcode 03CE
 * @command IS_CAR_STUCK
 * @class StuckCarCheck
 * @method IsCarStuck
 * @static
 * 
 * @brief Returns true if the car is stuck
 * 
 * @param {Car} vehicle
 */
bool IsCarStuck(CVehicle& self) {
    return CTheScripts::StuckCars.HasCarBeenStuckForAWhile(GetVehiclePool()->GetRef(&self));
}

/*
 * @opcode 03CC
 * @command ADD_STUCK_CAR_CHECK
 * @class StuckCarCheck
 * @method Add
 * @static
 * 
 * @brief Adds the vehicle to the stuck cars array
 * 
 * @param {Car} vehicle
 * @param {float} distance
 * @param {int} time
 */
void AddStuckCarCheck(CVehicle& self, float stuckRadius, uint32 time) {
    CTheScripts::StuckCars.AddCarToCheck(GetVehiclePool()->GetRef(&self), stuckRadius, time, false, false, false, false, 0);
}

/*
 * @opcode 03CD
 * @command REMOVE_STUCK_CAR_CHECK
 * @class StuckCarCheck
 * @method Remove
 * @static
 * 
 * @brief Removes the vehicle from the stuck cars array
 * 
 * @param {Car} vehicle
 */
void RemoveStuckCarCheck(CVehicle& self) {
    CTheScripts::StuckCars.RemoveCarFromCheck(GetVehiclePool()->GetRef(&self));
}

/*
 * @opcode 072F
 * @command ADD_STUCK_CAR_CHECK_WITH_WARP
 * @class StuckCarCheck
 * @method AddWithWarp
 * @static
 * 
 * @brief Attempts to automatically restore vehicles that get stuck or flipped
 * 
 * @param {Car} vehicle
 * @param {float} distance
 * @param {int} time
 * @param {bool} stuck
 * @param {bool} flipped
 * @param {bool} inWater
 * @param {int} numNodesToCheck
 */
void AddStuckCarCheckWithWarp(CVehicle& self, float stuckRadius, uint32 time, bool stuck, bool flipped, bool inWater, int8 numberOfNodesToCheck) {
    CTheScripts::StuckCars.AddCarToCheck(GetVehiclePool()->GetRef(&self), stuckRadius, time, true, stuck, flipped, inWater, numberOfNodesToCheck);
}

/*
 * @opcode 08A2
 * @command PLANE_ATTACK_PLAYER_USING_DOG_FIGHT
 * @class Plane
 * @method AttackPlayerUsingDogFight
 * 
 * @brief Sets the plane mission to attack the player while maintaining the minimum
 * @brief altitude
 * 
 * @param {Plane} self
 * @param {Player} player
 * @param {float} altitude
 */
void PlaneAttackPlayerUsingDogFight(CPlane& plane, CPlayerPed& player, float altitude) {
    if (plane.m_autoPilot.m_nCarMission != eCarMission::MISSION_PLANE_CRASH_AND_BURN && plane.m_autoPilot.m_nCarMission != eCarMission::MISSION_HELI_CRASH_AND_BURN) {
        plane.m_autoPilot.SetCarMission(eCarMission::MISSION_PLANE_DOG_FIGHT_PLAYER);
    }
    plane.m_minAltitude = altitude;
}

/// SET_CAR_ALWAYS_CREATE_SKIDS(07EE)
/*
 * @opcode 07EE
 * @command SET_CAR_ALWAYS_CREATE_SKIDS
 * @class Car
 * @method SetAlwaysCreateSkids
 * 
 * @param {Car} self
 * @param {bool} state
 */
void SetCarAlwaysCreateSkids(CVehicle& self, bool enable) {
    self.vehicleFlags.bAlwaysSkidMarks = enable;
}

/*
 * @opcode 00A5
 * @command CREATE_CAR
 * @class Car
 * @method Create
 * 
 * @brief Creates a vehicle at the specified location, with the specified model
 * 
 * @param {model_vehicle} modelId
 * @param {Vector} 
 * 
 * @returns {Car} handle
 */
CVehicle* CreateCar(CRunningScript& S, eModelID modelId, CVector pos) {
    return CCarCtrl::CreateCarForScript(modelId, pos, S.m_UsesMissionCleanup);
}

/*
 * @opcode 00A6
 * @command DELETE_CAR
 * @class Car
 * @method Delete
 * 
 * @brief Removes the vehicle from the game
 * 
 * @param {Car} self
 */
void DeleteCar(notsa::script::ScriptEntity<CVehicle> entity) {
    const auto [self, handle] = entity;
    if (self) {
        CWorld::Remove(self);
        CWorld::RemoveReferencesToDeletedObject(self);
        delete self;
    } else {
        CTheScripts::MissionCleanUp.RemoveEntityFromList(handle, MISSION_CLEANUP_ENTITY_TYPE_VEHICLE);
    }
}

/// Not a script command, but a shared implementation
void CarGotoCoordinatesUsingMission(CVehicle& self, CVector where, eCarMission mission, eCarMission missionStraightLine) {
    auto& ap = self.m_autoPilot;

    if (where.z <= MAP_Z_LOW_LIMIT) {
        where.z = CWorld::FindGroundZForCoord(where.x, where.y);
    }
    where.z += self.GetDistanceFromCentreOfMassToBaseOfModel();

    const auto joined = CCarCtrl::JoinCarWithRoadSystemGotoCoors(&self, where, false);

    self.SetStatus(STATUS_PHYSICS);
    self.SetEngineOn(true);

    if (!notsa::contains({ MISSION_PLANE_CRASH_AND_BURN, MISSION_HELI_CRASH_AND_BURN }, self.m_autoPilot.m_nCarMission)) {
        ap.m_nCarMission = !joined
            ? mission
            : missionStraightLine;
    }
    if (ap.m_nCruiseSpeed <= 1) {
        ap.SetCruiseSpeed(1);
    }
    ap.StartCarMissionNow();
}

/*
 * @opcode 00A7
 * @command CAR_GOTO_COORDINATES
 * @class Car
 * @method GotoCoordinates
 * 
 * @brief Makes the AI drive to the specified location by any means
 * 
 * @param {Car} self
 * @param {Vector} 
 */
void CarGotoCoordinates(CVehicle& self, CVector where) {
    CarGotoCoordinatesUsingMission(self, where, MISSION_GOTOCOORDINATES, MISSION_GOTOCOORDINATES_STRAIGHTLINE);
}

/*
 * @opcode 02C2
 * @command CAR_GOTO_COORDINATES_ACCURATE
 * @class Car
 * @method GotoCoordinatesAccurate
 * 
 * @brief Makes the AI drive to the specified location obeying the traffic rules
 * 
 * @param {Car} self
 * @param {Vector} 
 */
auto CarGotoCoordinatesAccurate(CVehicle& self, CVector where) {
    CarGotoCoordinatesUsingMission(self, where, MISSION_GOTOCOORDINATES_ACCURATE, MISSION_GOTOCOORDINATES_STRAIGHTLINE_ACCURATE);
}

/*
 * @opcode 0704
 * @command CAR_GOTO_COORDINATES_RACING
 * @class Car
 * @method GotoCoordinatesRacing
 * 
 * @brief Makes the AI drive to the destination as fast as possible, trying to overtake
 * @brief other vehicles
 * 
 * @param {Car} self
 * @param {Vector} 
 */
void CarGotoCoordinatesRacing(CVehicle& self, CVector where) {
    CarGotoCoordinatesUsingMission(self, where, MISSION_GOTOCOORDINATES_RACING, MISSION_GOTOCOORDINATES_STRAIGHTLINE);
}

/*
 * @opcode 00A8
 * @command CAR_WANDER_RANDOMLY
 * @class Car
 * @method WanderRandomly
 * 
 * @brief Clears any current tasks the vehicle has and makes it drive around aimlessly
 * 
 * @param {Car} self
 */
void CarWanderRandomly(CVehicle& self) {
    auto& ap = self.m_autoPilot;

    CCarCtrl::JoinCarWithRoadSystem(&self);

    self.SetStatus(STATUS_PHYSICS);
    self.SetEngineOn(true);

    if (!notsa::contains({ MISSION_PLANE_CRASH_AND_BURN, MISSION_HELI_CRASH_AND_BURN }, self.m_autoPilot.m_nCarMission)) {
        ap.m_nCarMission = MISSION_CRUISE;
    }
    if (ap.m_nCruiseSpeed <= 1) {
        ap.SetCruiseSpeed(1);
    }
    ap.m_nTimeToStartMission = CTimer::GetTimeInMS();
}

/*
 * @opcode 00A9
 * @command CAR_SET_IDLE
 * @class Car
 * @method SetIdle
 * 
 * @brief Sets the car's mission to idle (MISSION_NONE), stopping any driving activity
 * 
 * @param {Car} self
 */
void CarSetIdle(CVehicle& self) {
    auto& ap = self.m_autoPilot;

    if (!notsa::contains({ MISSION_PLANE_CRASH_AND_BURN, MISSION_HELI_CRASH_AND_BURN }, self.m_autoPilot.m_nCarMission)) {
        ap.m_nCarMission = MISSION_NONE;
    }
}

/*
 * @opcode 00AA
 * @command GET_CAR_COORDINATES
 * @class Car
 * @method GetCoordinates
 * 
 * @brief Returns the vehicle's coordinates
 * 
 * @param {Car} self
 * 
 * @returns {Vector} 
 */
CVector GetCarCoordinates(CVehicle& self) {
    return self.GetPosition();
}

/*
 * @opcode 00AB
 * @command SET_CAR_COORDINATES
 * @class Car
 * @method SetCoordinates
 * 
 * @brief Puts the vehicle at the specified location
 * 
 * @param {Car} self
 * @param {Vector} 
 */
void SetCarCoordinates(CVehicle& self, float x, float y, float z) {
    CCarCtrl::SetCoordsOfScriptCar(&self, x, y, z, false, true);
}

/*
 * @opcode 00AD
 * @command SET_CAR_CRUISE_SPEED
 * @class Car
 * @method SetCruiseSpeed
 * 
 * @brief Sets the vehicle's max speed
 * 
 * @param {Car} self
 * @param {float} maxSpeed
 */
void SetCarCruiseSpeed(CVehicle& self, float speed) {
    self.m_autoPilot.SetCruiseSpeed((uint32)(std::min(speed, self.m_pHandlingData->m_transmissionData.m_MaxFlatVelocity * 60.f)));
}

/*
 * @opcode 00AF
 * @command SET_CAR_MISSION
 * @class Car
 * @method SetMission
 * 
 * @brief Sets the mission of the vehicle's AI driver
 * 
 * @param {Car} self
 * @param {CarMission} carMission
 */
void SetCarMission(CVehicle& self, eCarMission mission) {
    auto& ap = self.m_autoPilot;

    ap.SetCarMissionFromScript(mission);
    ap.StartCarMissionNow();
    self.SetEngineOn(true);
}

/*
 * @opcode 0119
 * @command IS_CAR_DEAD
 * @class Car
 * @method IsDead
 * @static
 * 
 * @brief Returns true if the handle is an invalid vehicle handle or the vehicle has been
 * @brief destroyed (wrecked)
 * 
 * @param {Car} car
 */
bool IsCarDead(CVehicle* car) {
    return !car || car->GetStatus() == STATUS_WRECKED || !!car->vehicleFlags.bIsDrowning;
}

/*
 * @opcode 0137
 * @command IS_CAR_MODEL
 * @class Car
 * @method IsModel
 * 
 * @brief Returns true if the vehicle has the specified model
 * 
 * @param {Car} self
 * @param {model_vehicle} modelId
 */
bool IsCarModel(CVehicle& self, eModelID modelId) {
    return self.GetModelId() == modelId;
}

/*
 * @opcode 014B
 * @command CREATE_CAR_GENERATOR
 * @class CarGenerator
 * @method Create
 * 
 * @brief Initializes a parked car generator (modelId -1 selects a random vehicle from the
 * @brief local popcycle)
 * 
 * @param {Vector} 
 * @param {float} heading
 * @param {model_vehicle} modelId
 * @param {int} primaryColor
 * @param {int} secondaryColor
 * @param {bool} forceSpawn
 * @param {int} alarmChance
 * @param {int} doorLockChance
 * @param {int} minDelay
 * @param {int} maxDelay
 * 
 * @returns {CarGenerator} handle
 */
auto CreateCarGenerator(
    CVehicle& self,
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

/*
 * @opcode 0161
 * @command ADD_BLIP_FOR_CAR_OLD
 * @class Blip
 * @method AddForCarOld
 * 
 * @brief Adds a blip with properties to the vehicle
 * 
 * @param {Car} vehicle
 * @param {BlipColor} color
 * @param {BlipDisplay} display
 * 
 * @returns {Blip} handle
 */
auto AddBlipForCarOld(CRunningScript& S, int32 handle, int32 color, eBlipDisplay display) { // TODO: use `notsa::ScriptEntity<CVehicle>` instead of `int32 handle`
    return CRadar::SetEntityBlip(BLIP_CAR, handle, color, display);
}

/*
 * @opcode 0174
 * @command GET_CAR_HEADING
 * @class Car
 * @method GetHeading
 * 
 * @brief Returns the vehicle's heading (z-angle)
 * 
 * @param {Car} self
 * 
 * @returns {float} heading
 */
auto GetCarHeading(CVehicle& self) {
    return ClampDegreesForScript(DegreesToRadians(self.GetHeading()));
}

/*
 * @opcode 0175
 * @command SET_CAR_HEADING
 * @class Car
 * @method SetHeading
 * 
 * @brief Sets the vehicle's heading (z-angle)
 * 
 * @param {Car} self
 * @param {float} heading
 */
auto SetCarHeading(CVehicle& self, float deg) {
    self.SetHeading(DegreesToRadians(ClampDegreesForScript(deg)));
    self.UpdateRwMatrix();
}

/*
 * @opcode 0185
 * @command IS_CAR_HEALTH_GREATER
 * @class Car
 * @method IsHealthGreater
 * 
 * @brief Returns true if the car's health is over the specified value
 * 
 * @param {Car} self
 * @param {int} health
 */
auto IsCarHealthGreater(CVehicle& self, float value) {
    return self.GetHealth() >= value;
}

/*
 * @opcode 0186
 * @command ADD_BLIP_FOR_CAR
 * @class Blip
 * @method AddForCar
 * 
 * @brief Adds a blip and a marker to the vehicle
 * 
 * @param {Car} vehicle
 * 
 * @returns {Blip} handle
 */
auto AddBlipForCar(int32 handle) { // TODO: use `notsa::ScriptEntity<CVehicle>` instead of `int32 handle`
    const auto blip = CRadar::SetEntityBlip(BLIP_CAR, handle, 0, BLIP_DISPLAY_BOTH);
    CRadar::ChangeBlipScale(blip, 3);
    return blip;
}

/*
 * @opcode 018F
 * @command IS_CAR_STUCK_ON_ROOF
 * @class Car
 * @method IsStuckOnRoof
 * 
 * @brief Returns true if the car has been upside down for more than 2 seconds (requires
 * @brief 0190)
 * 
 * @param {Car} self
 */
auto IsCarStuckOnRoof(int32 handle) { // TODO: use `notsa::ScriptEntity<CVehicle>` instead of `int32 handle`
    return CTheScripts::UpsideDownCars.HasCarBeenUpsideDownForAWhile(handle);
}

/*
 * @opcode 0190
 * @command ADD_UPSIDEDOWN_CAR_CHECK
 * @class Car
 * @method AddUpsidedownCheck
 * 
 * @brief Activates upside-down car check for the car
 * 
 * @param {Car} self
 */
auto AddUpsidedownCarCheck(int32 handle) { // TODO: use `notsa::ScriptEntity<CVehicle>` instead of `int32 handle`
    CTheScripts::UpsideDownCars.AddCarToCheck(handle);
}

/*
 * @opcode 0191
 * @command REMOVE_UPSIDEDOWN_CAR_CHECK
 * @class Car
 * @method RemoveUpsidedownCheck
 * 
 * @brief Deactivates upside-down car check (0190) for the car
 * 
 * @param {Car} self
 */
auto RemoveUpsidedownCarCheck(int32 handle) { // TODO: use `notsa::ScriptEntity<CVehicle>` instead of `int32 handle`
    CTheScripts::UpsideDownCars.RemoveCarFromCheck(handle);
}

/*
 * @opcode 01C1
 * @command IS_CAR_STOPPED
 * @class Car
 * @method IsStopped
 * 
 * @brief Returns true if the vehicle is not moving
 * 
 * @param {Car} self
 */
auto IsCarStopped(CVehicle& self) {
    return CTheScripts::IsVehicleStopped(&self);
}

/// IS_CAR_IN_AREA_2D
template<bool WithDebugSquare = false>
/*
 * @opcode 00B0
 * @command IS_CAR_IN_AREA_2D
 * @class Car
 * @method IsInArea2D
 * 
 * @brief Returns true if the vehicle is located within the specified 2D area
 * 
 * @param {Car} self
 * @param {Vector2D} leftBottom
 * @param {Vector2D} rightTop
 * @param {bool} drawSphere
 */
bool IsCarInArea2D(CRunningScript& S, CVehicle& self, CVector2D p1, CVector2D p2, bool highlight) {
    if (highlight) {
        CTheScripts::HighlightImportantArea(
            (int32)(&S) + (int32)(S.m_IP),
            p1.x,
            p1.y,
            p2.x,
            p2.y,
            -100.f
        );
    }
    if constexpr (WithDebugSquare) {
        if (CTheScripts::DbgFlag) {
            const auto [minX, maxX] = std::minmax(p1.x, p2.x);
            const auto [minY, maxY] = std::minmax(p1.y, p2.y);
            CTheScripts::DrawDebugSquare(
                minX, maxX, minY, maxY
            );
        }
    }
    return self.IsWithinArea(p1.x, p1.y, p2.x, p2.y);
}

/*
 * @opcode 00B1
 * @command IS_CAR_IN_AREA_3D
 * @class Car
 * @method IsInArea3D
 * 
 * @brief Returns true if the vehicle is located within the specified 3D area
 * 
 * @param {Car} self
 * @param {Vector} leftBottom
 * @param {Vector} rightTop
 * @param {bool} drawSphere
 */
bool IsCarInArea3D(CRunningScript& S, CVehicle& self, CVector p1, CVector p2, bool highlight) {
    if (highlight) {
        CTheScripts::HighlightImportantArea(
            (int32)(&S) + (int32)(S.m_IP),
            p1.x,
            p1.y,
            p2.x,
            p2.y,
            (p2.z - p1.z) / 2.f
        );
    }
    return self.IsWithinArea(
        p1.x, p1.y, p1.z, p2.x, p2.y, p2.z
    );
}

/*
 * @opcode 01AD
 * @command LOCATE_CAR_2D
 * @class Car
 * @method Locate2D
 * 
 * @brief Returns true if the car is within the 2D radius of the point
 * 
 * @param {Car} self
 * @param {Vector2D} 
 * @param {Vector2D} radius
 * @param {bool} drawSphere
 */
auto LocateCar2D(CRunningScript& S, CVehicle& self, CVector2D pt, CVector2D radius, bool highlight) {
    return IsCarInArea2D<true>(
        S,
        self,
        pt - radius,
        pt + radius,
        highlight
    );
}

/*
 * @opcode 01AB
 * @command IS_CAR_STOPPED_IN_AREA_2D
 * @class Car
 * @method IsStoppedInArea2D
 * 
 * @brief Returns true if the car stopped within the specified 2D area
 * 
 * @param {Car} self
 * @param {Vector2D} leftBottom
 * @param {Vector2D} rightTop
 * @param {bool} drawSphere
 */
auto IsCarStoppedInArea2D(CRunningScript& S, CVehicle& self, CVector2D p1, CVector2D p2, bool highlight) {
    return IsCarInArea2D<true>(S, self, p1, p2, highlight) && IsCarStopped(self); // Make sure `IsCarInArea2D` check is first, as it also does highlighting
}

/*
 * @opcode 01AE
 * @command LOCATE_STOPPED_CAR_2D
 * @class Car
 * @method LocateStopped2D
 * 
 * @brief Returns true if the car is stopped within the 2D radius of the point
 * 
 * @param {Car} self
 * @param {Vector2D} 
 * @param {Vector2D} radius
 * @param {bool} drawSphere
 */
auto LocateStoppedCar2D(CRunningScript& S, CVehicle& self, CVector2D pt, CVector2D radius, bool highlight) {
    return IsCarStoppedInArea2D(
        S,
        self,
        pt - radius,
        pt + radius,
        highlight
    );
}

/*
 * @opcode 01AF
 * @command LOCATE_CAR_3D
 * @class Car
 * @method Locate3D
 * 
 * @brief Returns true if the car is within the 3D radius of the point
 * 
 * @param {Car} self
 * @param {Vector} 
 * @param {Vector} radius
 * @param {bool} drawSphere
 */
auto LocateCar3D(CRunningScript& S, CVehicle& self, CVector pt, CVector radius, bool highlight) {
    return IsCarInArea3D(
        S,
        self,
        pt - radius,
        pt + radius,
        highlight
    );
}

/*
 * @opcode 01AC
 * @command IS_CAR_STOPPED_IN_AREA_3D
 * @class Car
 * @method IsStoppedInArea3D
 * 
 * @brief Returns true if the car stopped within the specified 3D area
 * 
 * @param {Car} self
 * @param {Vector} leftBottom
 * @param {Vector} rightTop
 * @param {bool} drawSphere
 */
auto IsCarStoppedInArea3D(CRunningScript& S, CVehicle& self, CVector p1, CVector p2, bool highlight) {
    return IsCarInArea3D(S, self, p1, p2, highlight) && IsCarStopped(self); // Make sure `IsCarInArea3D` check is first, as it also does highlighting
}

/*
 * @opcode 01B0
 * @command LOCATE_STOPPED_CAR_3D
 * @class Car
 * @method LocateStopped3D
 * 
 * @brief Returns true if the car is stopped in the radius of the specified point
 * 
 * @param {Car} self
 * @param {Vector} 
 * @param {Vector} radius
 * @param {bool} drawSphere
 */
auto LocateStoppedCar3D(CRunningScript& S, CVehicle& self, CVector pt, CVector radius, bool highlight) {
    return IsCarStoppedInArea3D(
        S,
        self,
        pt - radius,
        pt + radius,
        highlight
    );
}

/*
 * @opcode 01C3
 * @command MARK_CAR_AS_NO_LONGER_NEEDED
 * @class Car
 * @method MarkAsNoLongerNeeded
 * 
 * @brief Allows the vehicle to be deleted by the game if necessary, and also removes it
 * @brief from the mission cleanup list, if applicable
 * 
 * @param {Car} self
 */
auto MarkCarAsNoLongerNeeded(CRunningScript& S, CVehicle& self) {
    CTheScripts::CleanUpThisVehicle(&self);
    if (S.m_UsesMissionCleanup) {
        CTheScripts::MissionCleanUp.RemoveEntityFromList(self);
    }
}

/*
 * @opcode 01EB
 * @command SET_CAR_DENSITY_MULTIPLIER
 * @class World
 * @method SetCarDensityMultiplier
 * @static
 * 
 * @brief Sets the quantity of traffic that will spawn in the game
 * 
 * @param {float} multiplier
 */
auto SetCarDensityMultiplier(CVehicle& self, float mult) {
    CCarCtrl::CarDensityMultiplier = mult;
}

/*
 * @opcode 01EC
 * @command SET_CAR_HEAVY
 * @class Car
 * @method SetHeavy
 * 
 * @brief Sets whether the car is heavy
 * 
 * @param {Car} self
 * @param {bool} state
 */
auto SetCarHeavy(CVehicle* car, bool isHeavy) {
    if (!car) {
        return;
    }

    car->physicalFlags.bMakeMassTwiceAsBig = isHeavy;

    const auto* const handling             = car->m_pHandlingData;
    car->m_fMass                           = handling->m_fMass;
    car->m_fTurnMass                       = handling->m_fTurnMass;
    car->m_fBuoyancyConstant               = handling->m_fBuoyancyConstant;
    if (isHeavy) {
        car->m_fMass *= 3.0f;
        car->m_fTurnMass *= 5.0f;
        car->m_fBuoyancyConstant *= 2.0f;
    }
}

/*
 * @opcode 01F4
 * @command IS_CAR_UPSIDEDOWN
 * @class Car
 * @method IsUpsidedown
 * 
 * @brief Returns true if the car is upside down
 * 
 * @param {Car} self
 */
auto IsCarUpsidedown(CVehicle& self) {
    return self.GetUp().z <= 0.3f;
}

/*
 * @opcode 020A
 * @command LOCK_CAR_DOORS
 * @class Car
 * @method LockDoors
 * 
 * @brief Sets the locked status of the car's doors
 * 
 * @param {Car} self
 * @param {CarLock} lockStatus
 */
auto LockCarDoors(CVehicle* car, eCarLock lock) {
    if (car) {
        car->m_nDoorLock = lock;
    }
}

/*
 * @opcode 020B
 * @command EXPLODE_CAR
 * @class Car
 * @method Explode
 * 
 * @brief Makes the vehicle explode
 * 
 * @param {Car} self
 */
auto ExplodeCar(CVehicle& self) {
    self.BlowUpCar(nullptr, false);
}

/*
 * @opcode 020D
 * @command IS_CAR_UPRIGHT
 * @class Car
 * @method IsUpright
 * 
 * @brief Returns true if the vehicle is in the normal position (upright)
 * 
 * @param {Car} self
 */
auto IsCarUpright(CVehicle& self) {
    return self.GetUp().z > 0.f;
}

/*
 * @opcode 021B
 * @command SET_TARGET_CAR_FOR_MISSION_GARAGE
 * @class Garage
 * @method SetTargetCarForMission
 * @static
 * 
 * @brief Sets the specified garage to only accept the specified vehicle
 * 
 * @param {GarageName} garageName
 * @param {Car} vehicle
 */
auto SetTargetCarForMissionGarage(const char* garageName, CVehicle* car) {
    const auto garage = CGarages::FindGarageIndex(garageName);
    if (garage == -1) {
        return;
    }
    CGarages::SetTargetCarForMissionGarage(garage, car);
}

/*
 * @opcode 0224
 * @command SET_CAR_HEALTH
 * @class Car
 * @method SetHealth
 * 
 * @brief Sets the vehicle's health
 * 
 * @param {Car} self
 * @param {int} health
 */
auto SetCarHealth(CVehicle& self, float health) {
    self.m_fHealth = health;
}

/*
 * @opcode 0227
 * @command GET_CAR_HEALTH
 * @class Car
 * @method GetHealth
 * 
 * @brief Returns the vehicle's health
 * 
 * @param {Car} self
 * 
 * @returns {int} health
 */
auto GetCarHealth(CVehicle& self) {
    return self.m_fHealth;
}

/*
 * @opcode 0229
 * @command CHANGE_CAR_COLOUR
 * @class Car
 * @method ChangeColor
 * 
 * @brief Sets the car's primary and secondary colors
 * 
 * @param {Car} self
 * @param {int} color1
 * @param {int} color2
 */
auto ChangeCarColour(CVehicle& self, uint32 primaryColor, uint32 secondaryColor) {
    self.m_nPrimaryColor   = primaryColor;
    self.m_nSecondaryColor = secondaryColor;
}

/*
 * @opcode 0294
 * @command SET_CAN_RESPRAY_CAR
 * @class Car
 * @method SetCanRespray
 * 
 * @brief Makes car keep current colors when Pay'n'Spray is used
 * 
 * @param {Car} self
 * @param {bool} changeColors
 */
auto SetCanResprayCar(CAutomobile& automobile, bool enabled) {
    automobile.autoFlags.bShouldNotChangeColour = enabled;
}

/*
 * @opcode 02AA
 * @command SET_CAR_ONLY_DAMAGED_BY_PLAYER
 * @class Car
 * @method SetOnlyDamagedByPlayer
 * 
 * @brief Makes a vehicle immune to everything except the player
 * 
 * @param {Car} self
 * @param {bool} state
 */
auto SetCarOnlyDamagedByPlayer(CVehicle& self, bool enabled) {
    self.physicalFlags.bInvulnerable = enabled;
}

/*
 * @opcode 02BF
 * @command IS_CAR_IN_WATER
 * @class Car
 * @method IsInWater
 * 
 * @brief Returns true if the vehicle is submerged in water
 * 
 * @param {Car} self
 */
auto IsCarInWater(CVehicle* car) {
    if (!car) {
        return false;
    }
    if (car->physicalFlags.bSubmergedInWater) {
        return true;
    }
    if (car->GetModelId() == MODEL_VORTEX) {
        const auto* const vortex = car->AsAutomobile();
        if (vortex->m_fWheelsSuspensionCompression[0] < 1.f) {
            return g_surfaceInfos.IsShallowWater(vortex->m_wheelColPoint[0].m_nSurfaceTypeB);
        }
    }
    return false;
}

/*
 * @opcode 02CA
 * @command IS_CAR_ON_SCREEN
 * @class Car
 * @method IsOnScreen
 * 
 * @brief Returns true if the car is visible
 * 
 * @param {Car} self
 */
auto IsCarOnScreen(CVehicle& self) {
    return self.GetIsOnScreen();
}

/*
 * @opcode 02F8
 * @command GET_CAR_FORWARD_X
 * @class Car
 * @method GetForwardX
 * 
 * @brief Returns the X coord of the vehicle's angle
 * 
 * @param {Car} self
 * 
 * @returns {float} x
 */
auto GetCarForwardX(CVehicle& self) {
    const CVector2D forward{ self.GetForward() };
    return forward.x / forward.Magnitude();
}

/*
 * @opcode 02F9
 * @command GET_CAR_FORWARD_Y
 * @class Car
 * @method GetForwardY
 * 
 * @brief Returns the Y coord of the vehicle's angle
 * 
 * @param {Car} self
 * 
 * @returns {float} y
 */
auto GetCarForwardY(CVehicle& self) {
    const CVector2D forward{ self.GetForward() };
    return forward.y / forward.Magnitude();
}

/*
 * @opcode 031E
 * @command HAS_CAR_BEEN_DAMAGED_BY_WEAPON
 * @class Car
 * @method HasBeenDamagedByWeapon
 * 
 * @brief Returns true if the vehicle has been hit by the specified weapon
 * 
 * @param {Car} self
 * @param {WeaponType} weaponType
 */
auto HasCarBeenDamagedByWeapon(CVehicle* car, eWeaponType weaponType) {
    if (!car) {
        return false;
    }
    if (weaponType == WEAPON_ANYMELEE || weaponType == WEAPON_ANYWEAPON) {
        return CDarkel::CheckDamagedWeaponType(car->GetLastWeaponDamageType(), weaponType);
    }
    return car->GetLastWeaponDamageType() == weaponType;
}

/*
 * @opcode 0325
 * @command START_CAR_FIRE
 * @class ScriptFire
 * @method CreateCarFire
 * 
 * @brief Creates a script fire on the vehicle
 * 
 * @param {Car} vehicle
 * 
 * @returns {ScriptFire} handle
 */
auto StartCarFire(CVehicle& self) {
    return gFireManager.StartScriptFire(
        self.GetPosition(),
        &self,
        0.8f,
        1,
        0,
        1
    );
}

/*
 * @opcode 0327
 * @command GET_RANDOM_CAR_OF_TYPE_IN_AREA
 * @class World
 * @method GetRandomCarOfTypeInArea
 * 
 * @brief Returns the handle of a random car with the specified model in the specified 2D
 * @brief area, or -1 otherwise
 * 
 * @param {Vector2D} leftBottom
 * @param {Vector2D} rightTop
 * @param {model_vehicle} modelId
 * 
 * @returns {Car} handle
 */
auto GetRandomCarOfTypeInArea(CRunningScript& S, CVehicle& self, float minX, float minY, float maxX, float maxY, eModelID modelId) {
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

/*
 * @opcode 0338
 * @command SET_CAR_VISIBLE
 * @class Car
 * @method SetVisible
 * 
 * @brief Sets whether the vehicle is visible or not
 * 
 * @param {Car} self
 * @param {bool} state
 */
auto SetCarVisible(CVehicle& self, bool visible) {
    self.SetIsVisible(visible);
}

/*
 * @opcode 035C
 * @command PLACE_OBJECT_RELATIVE_TO_CAR
 * @class Object
 * @method PlaceRelativeToCar
 * 
 * @brief Places the object at an offset from the car
 * 
 * @param {Object} self
 * @param {Car} vehicle
 * @param {Vector} offset
 */
auto PlaceObjectRelativeToCar(CObject& obj, CVehicle& self, CVector offset) {
    CPhysical::PlacePhysicalRelativeToOtherPhysical(&obj, &self, offset);
}

/*
 * @opcode 0397
 * @command SWITCH_CAR_SIREN
 * @class Car
 * @method SwitchSiren
 * 
 * @brief Sets whether the car's alarm can be activated
 * 
 * @param {Car} self
 * @param {bool} state
 */
auto SwitchCarSiren(CVehicle& self, bool state) {
    self.vehicleFlags.bSirenOrAlarm = state;
}

/*
 * @opcode 039C
 * @command SET_CAR_WATERTIGHT
 * @class Car
 * @method SetWatertight
 * 
 * @brief Makes the vehicle watertight, meaning characters inside will not be harmed if
 * @brief the vehicle is submerged in water
 * 
 * @param {Car} self
 * @param {bool} state
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
 * @param {Car} self
 * @param {float} x
 * @param {float} y
 */
void TurnCarToFaceCoord(CVehicle& self, CVector2D point) {
    const auto& pos   = self.GetPosition();
    const auto  angle = CGeneral::GetATanOf(pos - point) + HALF_PI;
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
 * @param {Car} self
 * @param {EntityStatus} status
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
 * @param {Car} self
 * @param {bool} state
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
 * @param {Car} self
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
 * @brief Disables the car from exploding when it is upside down, as long as the player is
 * @brief not in the vehicle
 * 
 * @param {Car} self
 * @param {bool} state
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
 * @param {Car} self
 * 
 * @returns {int} color1, {int} color2
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
 * @param {Car} self
 * @param {bool} state
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
 * @brief Returns the coordinates of an offset of the vehicle's position, depending on the
 * @brief vehicle's rotation
 * 
 * @param {Car} self
 * @param {Vector} offset
 * 
 * @returns {Vector} 
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
 * @param {Car} self
 * @param {float} traction
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
 * @param {Car} self
 * @param {bool} state
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
 * @param {Car} self
 * @param {bool} state
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
 * @param {Car} self
 * @param {SeatId} seat
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
 * @param {Car} self
 * 
 * @returns {model_vehicle} modelId
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
 * @param {Car} self
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
 * @param {Car} self
 * 
 * @returns {Char} handle
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
 * @brief Makes the AI driver perform the action in the vehicle for the specified period
 * @brief of time
 * 
 * @param {Car} self
 * @param {TempAction} actionId
 * @param {int} time
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
 * @param {Car} self
 * @param {int} routeSeed
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
 * @param {Car} self
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
 * @param {Car} self
 * @param {WheelId} tireId
 */
bool IsCarTyreBurst(CVehicle& self, eWheelId tire) {
    if (self.IsBike()) {
        auto* const bike = self.AsBike();
        switch (tire) {
        case eWheelId::ANY:
            return rng::all_of(bike->m_nWheelStatus, [](auto status) {
                return status == 1;
            });
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
 * @opcode 04FE
 * @command BURST_CAR_TYRE
 * @class Car
 * @method BurstTire
 * 
 * @brief Deflates the car's tire
 * 
 * @param {Car} self
 * @param {WheelId} tireId
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
 * @opcode 0699
 * @command FIX_CAR_TYRE
 * @class Car
 * @method FixTire
 * 
 * @brief Repairs a car's tire
 * 
 * @param {Car} self
 * @param {WheelId} tireId
 */
void FixCarTyre(CVehicle& self, eCarWheel tire) {
    self.AsAutomobile()->FixTyre(static_cast<eCarWheel>(tire));
}

/*
 * @opcode 053F
 * @command SET_CAN_BURST_CAR_TYRES
 * @class Car
 * @method SetCanBurstTires
 * 
 * @brief Sets whether the car's tires can be deflated
 * 
 * @param {Car} self
 * @param {bool} state
 */
void SetCanBurstCarTyres(CVehicle& self, bool state) {
    self.vehicleFlags.bTyresDontBurst = state;
}

/*
 * @opcode 04BA
 * @command SET_CAR_FORWARD_SPEED
 * @class Car
 * @method SetForwardSpeed
 * 
 * @brief Sets the speed of the car
 * 
 * @param {Car} self
 * @param {float} forwardSpeed
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
 * @brief Marks the car as being part of a convoy, which seems to follow a path set by
 * @brief 0994
 * 
 * @param {Car} self
 * @param {bool} state
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
 * @brief Sets the minimum distance for the AI driver to start ignoring car paths and go
 * @brief straight to the target
 * 
 * @param {Car} self
 * @param {int} distance
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
 * @param {Car} self
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
 * @param {Car} self
 */
bool IsCarWaitingForWorldCollision(CVehicle& self) {
    return self.m_bIsStaticWaitingForCollision;
}

/*
 * @opcode 0506
 * @command SET_CAR_MODEL_COMPONENTS
 * @class Car
 * @method SetModelComponents
 * @static
 * 
 * @brief Sets the variation of the next car to be created
 * 
 * @param {model_vehicle} _unused
 * @param {int} component1
 * @param {int} component2
 */
void SetCarModelComponents(eModelID _unused, int component1, int component2) {
    CVehicleModelInfo::ms_compsToUse[0] = component1;
    CVehicleModelInfo::ms_compsToUse[1] = component2;
}

/*
 * @opcode 0519
 * @command FREEZE_CAR_POSITION
 * @class Car
 * @method FreezePosition
 * 
 * @brief Locks the vehicle's position
 * 
 * @param {Car} self
 * @param {bool} state
 */
void FreezeCarPosition(CVehicle& self, bool state) {
    self.physicalFlags.bDontApplySpeed        = state;
    self.physicalFlags.bCollidable            = state;
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
 * @param {Car} self
 * @param {Char} handle
 */
bool HasCarBeenDamagedByChar(CVehicle* car, CPed* ped) {
    if (!car || !car->m_pLastDamageEntity) {
        return false;
    }
    if (!ped) {
        return car->m_pLastDamageEntity->GetIsTypePed();
    }
    if (car->m_pLastDamageEntity == ped) {
        return true;
    }
    if (ped->bInVehicle) {
        return car->m_pLastDamageEntity == ped->m_pVehicle;
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
 * @param {Car} self
 * @param {Car} other
 */
bool HasCarBeenDamagedByCar(CVehicle* car, CVehicle* other) {
    if (!car || !car->m_pLastDamageEntity) {
        return false;
    }
    if (!other) {
        return car->m_pLastDamageEntity->GetIsTypeVehicle();
    }
    return car->m_pLastDamageEntity == other;
}

/*
 * @opcode 054F
 * @command CLEAR_CAR_LAST_DAMAGE_ENTITY
 * @class Car
 * @method ClearLastDamageEntity
 * 
 * @brief Clears the car's last damage entity
 * 
 * @param {Car} self
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
 * @param {Car} self
 * @param {bool} state
 */
void FreezeCarPositionAndDontLoadCollision(CRunningScript& S, CVehicle& self, bool state) {
    self.physicalFlags.bDontApplySpeed        = state;
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
 * @param {Car} self
 * @param {bool} state
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
 * @param {Car} self
 * @param {int} path
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
 * @param {Car} self
 */
void StopPlaybackRecordedCar(CVehicle* car) {
    if (car) {
        CVehicleRecording::StopPlaybackRecordedCar(car);
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
 * @param {Car} self
 */
void PausePlaybackRecordedCar(CVehicle& self) {
    CVehicleRecording::PausePlaybackRecordedCar(&self);
}

/*
* @opcode 06FD
* @command SET_PLAYBACK_SPEED
* @class Car
* @method SetPlaybackSpeed
* 
* @brief Sets the playback speed of the car playing a car recording
* 
* @param {Car} self
* @param {float} speed
*/
void SetPlaybackSpeed(CVehicle& self, float speed) {
    CVehicleRecording::SetPlaybackSpeed(&self, speed);
}

/*
 * @opcode 060E
 * @command IS_PLAYBACK_GOING_ON_FOR_CAR
 * @class Car
 * @method IsPlaybackGoingOn
 * 
 * @brief Returns true if the car is assigned to a path
 * 
 * @param {Car} self
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
 * @param {Car} self
 */
void UnpausePlaybackRecordedCar(CVehicle& self) {
    CVehicleRecording::UnpausePlaybackRecordedCar(&self);
}

/*
 * @opcode 06C5
 * @command SKIP_TO_END_AND_STOP_PLAYBACK_RECORDED_CAR
 * @class Car
 * @method SkipToEndAndStopPlayback
 * 
 * @param {Car} self
 */
void SkipToEndAndStopPlaybackRecordedCar(CVehicle& self) {
    CVehicleRecording::SkipToEndAndStopPlaybackRecordedCar(&self);
}

/*
 * @opcode 0705
 * @command START_PLAYBACK_RECORDED_CAR_USING_AI
 * @class Car
 * @method StartPlaybackUsingAi
 * 
 * @brief Starts the playback of a recorded car with driver AI enabled
 * 
 * @param {Car} self
 * @param {int} pathId
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
 * @param {Car} self
 * @param {float} amount
 */
void SkipInPlaybackRecordedCar(CVehicle& self, float amount) {
    CVehicleRecording::SkipForwardInRecording(&self, amount);
}

/*
 * @opcode 085E
 * @command START_PLAYBACK_RECORDED_CAR_LOOPED
 * @class Car
 * @method StartPlaybackLooped
 * 
 * @brief Starts looped playback of a recorded car path
 * 
 * @param {Car} self
 * @param {int} pathId
 */
void StartPlaybackRecordedCarLooped(CVehicle& self, int32 pathId) {
    CVehicleRecording::StartPlaybackRecordedCar(&self, pathId, false, true);
}

/*
* @opcode 099B
* @command CHANGE_PLAYBACK_TO_USE_AI
* @class Car
* @method ChangePlaybackToUseAi
* 
* @brief Changes vehicle control from playback to AI driven
* 
* @param {Car} self
*/
void ChangePlaybackToUseAi(CVehicle& self) {
    CVehicleRecording::ChangeCarPlaybackToUseAI(&self);
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
 * @param {Car} self
 * @param {Car} handle
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
 * @param {Car} self
 * @param {Car} handle
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
 * @param {Car} self
 * @param {Car} handle
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
 * @param {Car} self
 * @param {Car} handle
 */
void SetCarEscortCarFront(CVehicle& self, CVehicle& other) {
    SetCarEscortCarUsingMission(self, other, MISSION_ESCORT_FRONT);
}

/*
 * @opcode 08A6
 * @command OPEN_CAR_DOOR_A_BIT
 * @class Car
 * @method OpenDoorABit
 * 
 * @brief Sets the angle of a car door
 * 
 * @param {Car} self
 * @param {CarDoor} door
 * @param {float} value
 */
void OpenCarDoorABit(CVehicle& self, eDoors door, float value) {
    auto* const automobile = self.AsAutomobile();
    if (automobile->IsDoorMissing(door)) {
        return;
    }
    const auto node = CDamageManager::GetCarNodeIndexFromDoor(door);
    if (!automobile->m_aCarNodes[node]) {
        return;
    }
    automobile->OpenDoor(nullptr, node, door, value, true);
}

/*
 * @opcode 0657
 * @command OPEN_CAR_DOOR
 * @class Car
 * @method OpenDoor
 * 
 * @brief Opens the specified car door
 * 
 * @param {Car} self
 * @param {CarDoor} door
 */
void OpenCarDoor(CVehicle& self, eDoors door) {
    OpenCarDoorABit(self, door, 1.f);
}

/*
 * @opcode 0508
 * @command CLOSE_ALL_CAR_DOORS
 * @class Car
 * @method CloseAllDoors
 * 
 * @brief Closes all car doors, hoods and boots
 * 
 * @param {Car} self
 */
void CloseAllCarDoors(CVehicle& self) {
    self.AsAutomobile()->CloseAllDoors();
}

/*
 * @opcode 073C
 * @command DAMAGE_CAR_DOOR
 * @class Car
 * @method DamageDoor
 * 
 * @brief Damages a component on the vehicle
 * 
 * @param {Car} self
 * @param {CarDoor} door
 */
void DamageCarDoor(CVehicle& self, eDoors door) {
    assert(door <= MAX_DOORS);

    auto* const automobile = self.AsAutomobile();
    automobile->GetDamageManager().ApplyDamage(
        automobile,
        (tComponent)(+COMPONENT_BONNET + +door),
        150.f,
        1.f
    );
}

/*
 * @opcode 08A7
 * @command IS_CAR_DOOR_FULLY_OPEN
 * @class Car
 * @method IsDoorFullyOpen
 * 
 * @param {Car} self
 * @param {CarDoor} door
 */
bool IsCarDoorFullyOpen(CVehicle& self, eDoors door) {
    auto* const automobile = self.AsAutomobile();
    return automobile->IsDoorFullyOpenU32(automobile->GetDamageManager().GetCarNodeIndexFromDoor(door));
}

/*
 * @opcode 0689
 * @command POP_CAR_DOOR
 * @class Car
 * @method PopDoor
 * 
 * @brief Removes the specified car door component from the car
 * 
 * @param {Car} self
 * @param {CarDoor} door
 * @param {bool} visibility
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
 * @param {Car} self
 * @param {CarDoor} door
 */
void FixCarDoor(CVehicle& self, eDoors door) {
    auto* const automobile = self.AsAutomobile();
    automobile->FixDoor(CDamageManager::GetCarNodeIndexFromDoor(door), door);
}

/*
 * @opcode 095E
 * @command CONTROL_CAR_DOOR
 * @class Car
 * @method ControlDoor
 * 
 * @brief Sets the car's door angle and latch state
 * 
 * @param {Car} self
 * @param {CarDoor} door
 * @param {CarDoorState} state
 * @param {float} angle
 */
void ControlCarDoor(CVehicle& self, eDoors door, eDoorStatus state, float angle) {
    auto* const automobile = self.AsAutomobile();

    if (door >= MAX_DOORS) {
        auto* const chassis = &automobile->m_swingingChassis;
        if (angle >= 0.f) {
            chassis->Open(angle);
        }
        chassis->m_doorState = (eDoorState)(state); // TODO
    } else {
        if (angle >= 0.f) {
            automobile->m_doors[door].Open(angle);
        }
        automobile->GetDamageManager().SetDoorStatus(door, state);
        automobile->SetDoorDamage(door);
    }
}

/*
 * @opcode 09BB
 * @command IS_CAR_DOOR_DAMAGED
 * @class Car
 * @method IsDoorDamaged
 * 
 * @brief Returns true if the specified vehicle part is visibly damaged
 * 
 * @param {Car} self
 * @param {CarDoor} door
 */
bool IsCarDoorDamaged(CVehicle& self, eDoors door) {
    return self.AsAutomobile()->GetDamageManager().GetDoorStatus(door) != eDoorStatus::DAMSTATE_OK;
}

/*
 * @opcode 0674
 * @command CUSTOM_PLATE_FOR_NEXT_CAR
 * @class Car
 * @method CustomPlateForNextCar
 * @static
 * 
 * @brief Sets the numberplate of the next car to be spawned with the specified model
 * 
 * @param {model_vehicle} modelId
 * @param {string} text
 */
void CustomPlateForNextCar(eModelID modelId, std::string_view text) {
    assert(text.length() <= 8);

    auto* const mi = CModelInfo::GetVehicleModelInfo(modelId); /* NOTE: Not sure if this is correct, it uses `AsVehicleModelInfoPtr` without checking */
    if (!mi || mi->GetModelType() != MODEL_INFO_VEHICLE || !mi->m_pPlateMaterial) {
        return;
    }

    char plate[8 + 1]{ 0 };
    ConvertScriptTextToCarPlate(plate, text);
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
 * @param {Car} self
 * @param {CarLights} lightMode
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
 * @param {Car} self
 * @param {Car} handle
 * @param {Vector} offset
 * @param {Vector} rotation
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
 * @brief Detaches the car with optional rotation and force
 * 
 * @param {Car} self
 * @param {float} pitch
 * @param {float} heading
 * @param {float} strength
 * @param {bool} applyTurnForce
 */
void DetachCar(CVehicle* car, CVector2D dir, float strength, bool collisionDetection) {
    if (!car || !car->m_pAttachedTo) {
        return;
    }
    car->DettachEntityFromEntity(
        DegreesToRadians(dir.x),
        DegreesToRadians(dir.y),
        strength,
        collisionDetection
    );
}

/*
 * @opcode 068B
 * @command TASK_EVERYONE_LEAVE_CAR
 * @class Car
 * @method TaskEveryoneLeave
 * 
 * @brief Makes all passengers of the car leave it
 * 
 * @param {Car} self
 */
void TaskEveryoneLeaveCar(CRunningScript& S, CVehicle& self) {
    const auto ProcessPed = [&](CPed* ped, int32 delay) {
        S.GivePedScriptedTask(
            ped,
            new CTaskComplexLeaveAnyCar{ delay, false, false },
            COMMAND_TASK_EVERYONE_LEAVE_CAR
        );
    };

    if (auto* const driver = self.m_pDriver) {
        ProcessPed(driver, 0);
    }

    int32 delay = 0;
    for (auto* const passenger : self.GetPassengers()) {
        if (passenger) {
            ProcessPed(passenger, CGeneral::GetRandomNumberInRange(-250, 250) + 500 + delay);
            delay += 500;
        }
    }
}

/*
 * @opcode 0697
 * @command POP_CAR_PANEL
 * @class Car
 * @method PopPanel
 * 
 * @brief Detatches or deletes car's body part
 * 
 * @param {Car} self
 * @param {CarPanel} panelId
 * @param {bool} drop
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
 * @param {Car} self
 * @param {CarPanel} panelId
 */
void FixCarPanel(CVehicle& self, ePanels panel) {
    auto* const automobile = self.AsAutomobile();
    automobile->FixPanel(CDamageManager::GetCarNodeIndexFromPanel(panel), panel);
}

/*
 * @opcode 0730
 * @command DAMAGE_CAR_PANEL
 * @class Car
 * @method DamagePanel
 * 
 * @brief Damages a panel on the car
 * 
 * @param {Car} self
 * @param {int} panelId
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
 * @opcode 06A2
 * @command GET_CAR_SPEED_VECTOR
 * @class Car
 * @method GetSpeedVector
 * 
 * @param {Car} self
 * 
 * @returns {Vector} 
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
 * @param {Car} self
 * 
 * @returns {float} mass
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
 * @param {Car} self
 * 
 * @returns {float} angle
 */
float GetCarRoll(CVehicle& self) {
    return self.GetRoll();
}

/*
 * @opcode 0731
 * @command SET_CAR_ROLL
 * @class Car
 * @method SetRoll
 * 
 * @brief Sets the Y Angle of the vehicle to the specified value
 * 
 * @param {Car} self
 * @param {float} yAngle
 */
void SetCarRoll(CVehicle& self, float roll) {
    self.SetRoll(roll);
}

/*
 * @opcode 077D
 * @command GET_CAR_PITCH
 * @class Car
 * @method GetPitch
 * 
 * @brief Returns the X Angle of the vehicle
 * 
 * @param {Car} self
 * 
 * @returns {float} angle
 */
float GetCarPitch(CVehicle& self) {
    return ClampDegreesForScript(DegreesToRadians(self.AsAutomobile()->GetCarPitch()));
}

/*
 * @opcode 06FC
 * @command DOES_CAR_HAVE_STUCK_CAR_CHECK
 * @class Car
 * @method DoesHaveStuckCarCheck
 * 
 * @brief Returns true if the car has car stuck check enabled
 * 
 * @param {Car} self
 */
bool DoesCarHaveStuckCarCheck(notsa::script::ScriptEntity<CVehicle> self) {
    return CTheScripts::StuckCars.IsCarInStuckCarArray(self.h);
}

/*
 * @opcode 070C
 * @command EXPLODE_CAR_IN_CUTSCENE
 * @class Car
 * @method ExplodeInCutscene
 * 
 * @brief Makes the vehicle explode without affecting its surroundings
 * 
 * @param {Car} self
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
 * @param {Car} self
 * @param {bool} state
 */
void SetCarStayInSlowLane(CVehicle& self, bool state) {
    self.m_autoPilot.carCtrlFlags.bStayInSlowLane = state;
}

/*
 * @opcode 0732
 * @command SUPPRESS_CAR_MODEL
 * @class CarGenerator
 * @method SuppressCarModel
 * @static
 * 
 * @brief Prevents the specified car model from spawning for car generators
 * 
 * @param {model_vehicle} model
 */
void SuppressCarModel(eModelID model) {
    CTheScripts::AddToSuppressedCarModelArray(model);
}

/*
 * @opcode 0733
 * @command DONT_SUPPRESS_CAR_MODEL
 * @class CarGenerator
 * @method DontSuppressCarModel
 * @static
 * 
 * @brief Allows the specified car model to spawn for car generators
 * 
 * @param {model_vehicle} modelId
 */
void DontSuppressCarModel(eModelID modelId) {
    CTheScripts::RemoveFromSuppressedCarModelArray(modelId);
}

/*
 * @opcode 0734
 * @command DONT_SUPPRESS_ANY_CAR_MODELS
 * @class CarGenerator
 * @method DontSuppressAnyCarModels
 * @static
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
 * @param {Car} self
 * @param {bool} state
 */
void SetCarCanGoAgainstTraffic(CVehicle& self, bool state) {
    self.m_autoPilot.carCtrlFlags.bCantGoAgainstTraffic = state;
}

/*
 * @opcode 0763
 * @command SET_CAR_AS_MISSION_CAR
 * @class Car
 * @method SetAsMissionCar
 * 
 * @brief Sets the script as the owner of the vehicle and adds it to the mission cleanup
 * @brief list
 * 
 * @param {Car} self
 */
void SetCarAsMissionCar(CRunningScript& S, CVehicle& self) {
    if (S.m_UsesMissionCleanup && (self.IsCreatedBy(RANDOM_VEHICLE) || self.IsCreatedBy(PARKED_VEHICLE))) {
        self.SetVehicleCreatedBy(MISSION_VEHICLE);
        CTheScripts::MissionCleanUp.AddEntityToList(self);
    }
}

/*
 * @opcode 07D5
 * @command APPLY_FORCE_TO_CAR
 * @class Car
 * @method ApplyForce
 * 
 * @brief Applies force to car with offset from its center of mass
 * 
 * @param {Car} self
 * @param {Vector} dir
 * @param {Vector} offset
 */
void ApplyForceToCar(CVehicle& self, CVector force, CVector offset) {
    self.ApplyForce(
        force * self.GetMass(offset, force.Normalized()),
        offset + self.GetMatrix().TransformVector(self.m_vecCentreOfMass),
        true
    );
}

/*
 * @opcode 07DA
 * @command ADD_TO_CAR_ROTATION_VELOCITY
 * @class Car
 * @method AddToRotationVelocity
 * 
 * @param {Car} self
 * @param {Vector} 
 */
void AddToCarRotationVelocity(CVehicle& self, CVector velocity) {
    if (self.m_bIsStatic) {
        self.SetIsStatic(false);
        self.AddToMovingList();
    }
    self.SetTurnSpeed(self.GetTurnSpeed() + self.GetMatrix().TransformVector(velocity / 50.f));
}

/*
 * @opcode 07DB
 * @command SET_CAR_ROTATION_VELOCITY
 * @class Car
 * @method SetRotationVelocity
 * 
 * @param {Car} self
 * @param {Vector} 
 */
void SetCarRotationVelocity(CVehicle& self, CVector velocity) {
    if (self.m_bIsStatic) {
        self.SetIsStatic(false);
        self.AddToMovingList();
    }
    self.SetTurnSpeed(self.GetMatrix().TransformVector(velocity) / 50.f);
}

/*
 * @opcode 07F5
 * @command CONTROL_CAR_HYDRAULICS
 * @class Car
 * @method ControlHydraulics
 * 
 * @brief Changes the car wheels' suspension level
 * 
 * @param {Car} self
 * @param {float} frontLeftWheelSuspension
 * @param {float} rearLeftWheelSuspension
 * @param {float} frontRightWheelSuspension
 * @param {float} rearRightWheelSuspension
 */
void ControlCarHydraulics(CVehicle& self, float frontLeftWheelSuspension, float rearLeftWheelSuspension, float frontRightWheelSuspension, float rearRightWheelSuspension) {
    if (self.m_vehicleSpecialColIndex < 0) {
        if (!self.GetSpecialColModel()) {
            NOTSA_LOG_WARN("Failed to allocate SpecialColModel for vehicle {:p}", (void*)(&self));
            return;
        }
    }
    auto& suspension                  = self.m_aSpecialHydraulicData[self.m_vehicleSpecialColIndex].m_aWheelSuspension;
    suspension[CAR_WHEEL_FRONT_LEFT]  = frontLeftWheelSuspension;
    suspension[CAR_WHEEL_REAR_LEFT]   = rearLeftWheelSuspension;
    suspension[CAR_WHEEL_FRONT_RIGHT] = frontRightWheelSuspension;
    suspension[CAR_WHEEL_REAR_RIGHT]  = rearRightWheelSuspension;
}

/*
 * @opcode 07F8
 * @command SET_CAR_FOLLOW_CAR
 * @class Car
 * @method SetFollowCar
 * 
 * @param {Car} self
 * @param {Car} handle
 * @param {float} radius
 */
void SetCarFollowCar(CVehicle& self, CVehicle& other, float radius) {
    CCarAI::TellCarToFollowOtherCar(&self, &other, radius);
}

/*
 * @opcode 07FF
 * @command SET_CAR_HYDRAULICS
 * @class Car
 * @method SetHydraulics
 * 
 * @brief Enables hydraulic suspension on the car
 * 
 * @param {Car} self
 * @param {bool} state
 */
void SetCarHydraulics(CVehicle& self, bool state) {
    if (state) {
        self.AddVehicleUpgrade(ModelIndices::MI_HYDRAULICS);
    } else {
        self.RemoveVehicleUpgrade(ModelIndices::MI_HYDRAULICS);
    }
}

/*
 * @opcode 0803
 * @command DOES_CAR_HAVE_HYDRAULICS
 * @class Car
 * @method DoesHaveHydraulics
 * 
 * @brief Returns true if the car has hydraulics installed
 * 
 * @param {Car} self
 */
bool DoesCarHaveHydraulics(CVehicle& self) {
    return self.IsSubAutomobile() && self.handlingFlags.bHydraulicInst;
}

/*
 * @opcode 081D
 * @command SET_CAR_ENGINE_BROKEN
 * @class Car
 * @method SetEngineBroken
 * 
 * @brief Sets whether the car's engine is broken
 * 
 * @param {Car} self
 * @param {bool} state
 */
void SetCarEngineBroken(CVehicle& self, bool broken) {
    self.vehicleFlags.bEngineBroken = broken;
    if (broken) {
        self.vehicleFlags.bEngineOn = false;
    }
}

/*
 * @opcode 083F
 * @command GET_CAR_UPRIGHT_VALUE
 * @class Car
 * @method GetUprightValue
 * 
 * @brief Gets the car's vertical angle
 * 
 * @param {Car} self
 * 
 * @returns {float} value
 */
float GetCarUprightValue(CVehicle& self) {
    return self.GetUpVector().z;
}

/*
 * @opcode 0852
 * @command SET_CAR_CAN_BE_VISIBLY_DAMAGED
 * @class Car
 * @method SetCanBeVisiblyDamaged
 * 
 * @brief Sets whether the vehicle can be visibly damaged
 * 
 * @param {Car} self
 * @param {bool} state
 */
void SetCarCanBeVisiblyDamaged(CVehicle& self, bool state) {
    self.AsAutomobile()->autoFlags.bCanBeVisiblyDamaged = state;
}

/*
 * @opcode 088C
 * @command SET_CAR_COORDINATES_NO_OFFSET
 * @class Car
 * @method SetCoordinatesNoOffset
 * 
 * @brief Sets the vehicle coordinates without applying offsets to account for the height
 * @brief of the vehicle
 * 
 * @param {Car} self
 * @param {Vector} 
 */
void SetCarCoordinatesNoOffset(CVehicle& self, CVector pos) {
    CCarCtrl::SetCoordsOfScriptCar(&self, pos.x, pos.y, pos.z, false, false);
}

/*
 * @opcode 08CB
 * @command EXPLODE_CAR_IN_CUTSCENE_SHAKE_AND_BITS
 * @class Car
 * @method ExplodeInCutsceneShakeAndBits
 * 
 * @brief Causes the vehicle to explode, without damage to surrounding entities
 * 
 * @param {Car} self
 * @param {bool} shake
 * @param {bool} effect
 * @param {bool} sound
 */
void ExplodeCarInCutsceneShakeAndBits(CVehicle& self, bool shake, bool effect, bool sound) {
    self.BlowUpCarCutSceneNoExtras(!shake, !effect, false, sound);
}

/*
 * @opcode 0918
 * @command SET_CAR_ENGINE_ON
 * @class Car
 * @method SetEngineOn
 * 
 * @brief Sets whether the vehicle's engine is turned on or off
 * 
 * @param {Car} self
 * @param {bool} state
 */
void SetCarEngineOn(CVehicle& self, bool state) {
    self.SetEngineOn(state);
}

/*
 * @opcode 0919
 * @command SET_CAR_LIGHTS_ON
 * @class Car
 * @method SetLightsOn
 * 
 * @brief Sets whether the vehicle's lights are on
 * 
 * @param {Car} self
 * @param {bool} state
 */
void SetCarLightsOn(CVehicle& self, bool state) {
    self.vehicleFlags.bLightsOn = state;
}

/*
 * @opcode 0939
 * @command ATTACH_CAR_TO_OBJECT
 * @class Car
 * @method AttachToObject
 * 
 * @brief Attaches the car to object with offset and rotation
 * 
 * @param {Car} self
 * @param {Object} handle
 * @param {Vector} offset
 * @param {Vector} rotation
 */
void AttachCarToObject(CVehicle& self, CObject& object, CVector offset, CVector rotation) {
    self.AttachEntityToEntity(&object, offset, rotation * DEG_TO_RAD);
}

/*
 * @opcode 096B
 * @command STORE_CAR_MOD_STATE
 * @class Car
 * @method StoreModState
 * @static
 */
void StoreCarModState() {
    CShopping::StoreVehicleMods();
}

/*
 * @opcode 096C
 * @command RESTORE_CAR_MOD_STATE
 * @class Car
 * @method RestoreModState
 * @static
 */
void RestoreCarModState() {
    CShopping::RestoreVehicleMods();
}

/*
 * @opcode 096D
 * @command GET_CURRENT_CAR_MOD
 * @class Car
 * @method GetCurrentMod
 * 
 * @brief Returns the model of the component installed on the specified slot of the
 * @brief vehicle, or -1 otherwise
 * 
 * @param {Car} self
 * @param {ModSlot} slot
 * 
 * @returns {model_object} modelId
 */
eModelID GetCurrentCarMod(CVehicle& self, eModSlot slot) {
    using enum eModSlot;

    const auto GetDependentUpgrade = [&self](int32 upgradeA, int32 upgradeB) {
        if (self.GetUpgrade(upgradeA) == -1) {
            return MODEL_INVALID;
        }
        return self.GetUpgrade(upgradeB);
    };

    switch (slot) {
    case HOOD:           return self.GetUpgrade(0);
    case VENTS:          return GetDependentUpgrade(1, 2);
    case SPOILERS:       return self.GetUpgrade(6);
    case SIDE_SKIRTS:    return GetDependentUpgrade(8, 9);
    case FRONT_BULLBARS: return self.GetUpgrade(10);
    case REAR_BULLBARS:  return self.GetUpgrade(11);
    case LIGHTS:         return self.GetUpgrade(12);
    case ROOF:           return self.GetUpgrade(14);
    case NITRO:          return self.GetUpgrade(15);
    case HYDRAULICS:     return self.GetUpgrade(16);
    case CAR_STEREO:     return self.GetUpgrade(17);
    case WHEELS:         return self.GetReplacementUpgrade(2);
    case EXHAUSTS:       return self.GetReplacementUpgrade(19);
    case FRONT_BUMPER:   return self.GetReplacementUpgrade(12);
    case REAR_BUMPER:    return self.GetReplacementUpgrade(13);
    case P16:            return self.GetReplacementUpgrade(20);
    }
    return MODEL_INVALID;
}

/*
 * @opcode 096E
 * @command IS_CAR_LOW_RIDER
 * @class Car
 * @method IsLowRider
 * 
 * @brief Returns true if the vehicle is a low rider
 * 
 * @param {Car} self
 */
bool IsCarLowRider(CVehicle& self) {
    return gHandlingDataMgr.m_aVehicleHandling[self.GetVehicleModelInfo()->m_nHandlingId].m_bLowRider;
}

/*
 * @opcode 096F
 * @command IS_CAR_STREET_RACER
 * @class Car
 * @method IsStreetRacer
 * 
 * @brief Returns true if the vehicle is a street racer
 * 
 * @param {Car} self
 */
bool IsCarStreetRacer(CVehicle& self) {
    return gHandlingDataMgr.m_aVehicleHandling[self.GetVehicleModelInfo()->m_nHandlingId].m_bStreetRacer;
}

/*
 * @opcode 097D
 * @command GET_NUM_CAR_COLOURS
 * @class Car
 * @method GetNumColors
 * 
 * @brief Returns number of color variations defined for the model of this car in
 * @brief carcols.dat
 * 
 * @param {Car} self
 * 
 * @returns {int} count
 */
int GetNumCarColours(CVehicle& self) {
    return self.GetVehicleModelInfo()->m_nNumColorVariations;
}

/*
 * @opcode 0987
 * @command GET_CAR_BLOCKING_CAR
 * @class Car
 * @method GetBlockingCar
 * 
 * @brief Returns a handle of the vehicle preventing this car from getting to its
 * @brief destination
 * 
 * @param {Car} self
 * 
 * @returns {Car} handle
 */
CVehicle* GetCarBlockingCar(CVehicle& self) {
    if (auto* const entity = self.m_autoPilot.m_ObstructingEntity) {
        if (entity->GetIsTypeVehicle()) {
            return entity->AsVehicle();
        }
    }
    return nullptr;
}

/*
 * @opcode 098D
 * @command GET_CAR_MOVING_COMPONENT_OFFSET
 * @class Car
 * @method GetMovingComponentOffset
 * 
 * @brief Sets the angle of a vehicle's extra
 * 
 * @param {Car} self
 * 
 * @returns {float} offset
 */
float GetCarMovingComponentOffset(CVehicle& self) {
    return self.AsAutomobile()->GetMovingCollisionOffset();
}

/*
 * @opcode 099A
 * @command SET_CAR_COLLISION
 * @class Car
 * @method SetCollision
 * 
 * @param {Car} self
 * @param {bool} state
 */
void SetCarCollision(CVehicle& self, bool state) {
    self.m_bUsesCollision            = state;
    self.physicalFlags.bApplyGravity = state;
}

/*
 * @opcode 09B3
 * @command GET_CAR_DOOR_LOCK_STATUS
 * @class Car
 * @method GetDoorLockStatus
 * 
 * @brief Returns the door lock mode of the vehicle
 * 
 * @param {Car} self
 * 
 * @returns {CarLock} lockStatus
 */
eCarLock GetCarDoorLockStatus(CVehicle& self) {
    return self.m_nDoorLock;
}

/*
 * @opcode 09CB
 * @command IS_CAR_TOUCHING_CAR
 * @class Car
 * @method IsTouchingCar
 * 
 * @brief Returns true if the car is touching the other car
 * 
 * @param {Car} self
 * @param {Car} handle
 */
bool IsCarTouchingCar(CVehicle& self, CVehicle& other) {
    return self.GetHasCollidedWith(&other);
}

/*
 * @opcode 09E1
 * @command GET_CAR_MODEL_VALUE
 * @class Car
 * @method GetModelValue
 * @static
 * 
 * @brief Returns the value of the specified car model
 * 
 * @param {model_vehicle} model
 * 
 * @returns {int} value
 */
int GetCarModelValue(eModelID model) {
    return gHandlingDataMgr.m_aVehicleHandling[CModelInfo::GetModelInfo(model)->AsVehicleModelInfoPtr()->m_nHandlingId].m_nMonetaryValue;
}

/*
 * @opcode 09E2
 * @command CREATE_CAR_GENERATOR_WITH_PLATE
 * @class CarGenerator
 * @method CreateWithPlate
 * 
 * @brief Creates a parked car generator with a number plate (modelId -1 selects a random
 * @brief vehicle from the local popcycle)
 * 
 * @param {Vector} 
 * @param {float} heading
 * @param {model_vehicle} modelId
 * @param {int} primaryColor
 * @param {int} secondaryColor
 * @param {bool} forceSpawn
 * @param {int} alarmChance
 * @param {int} doorLockChance
 * @param {int} minDelay
 * @param {int} maxDelay
 * @param {string} plateName
 * 
 * @returns {CarGenerator} handle
 */
auto CreateCarGeneratorWithPlate(
    CVector          pos,
    float            heading,
    eModelID         modelId,
    int              primaryColor,
    int              secondaryColor,
    bool             forceSpawn,
    int              alarmChance,
    int              doorLockChance,
    int              minDelay,
    int              maxDelay,
    std::string_view plateName
) {
    char plate[8 + 1]{ 0 };
    ConvertScriptTextToCarPlate(plate, plateName);

    auto generator = CTheCarGenerators::CreateCarGenerator(
        pos,
        heading,
        modelId,
        primaryColor,
        secondaryColor,
        forceSpawn,
        alarmChance,
        doorLockChance,
        minDelay,
        maxDelay,
        0,
        true
    );
    CTheCarGenerators::m_SpecialPlateHandler.Add(generator, plate);

    return generator;
}

/*
 * @opcode 09E9
 * @command GIVE_NON_PLAYER_CAR_NITRO
 * @class Car
 * @method GiveNonPlayerNitro
 * 
 * @brief Makes the car have one nitro
 * 
 * @param {Car} self
 */
void GiveNonPlayerCarNitro(CVehicle& self) {
    self.AsAutomobile()->NitrousControl(1);
}

/*
 * @opcode 0A11
 * @command SET_EXTRA_CAR_COLOURS
 * @class Car
 * @method SetExtraColors
 * 
 * @brief Sets the car's ternary and quaternary colors. See also 0229
 * 
 * @param {Car} self
 * @param {int} color3
 * @param {int} color4
 */
void SetExtraCarColours(CVehicle& self, int color3, int color4) {
    self.m_nTertiaryColor   = color3;
    self.m_nQuaternaryColor = color4;
}

/*
 * @opcode 0A15
 * @command HAS_CAR_BEEN_RESPRAYED
 * @class Car
 * @method HasBeenResprayed
 * 
 * @brief Returns true if the vehicle was resprayed in the last frame AND resets the
 * @brief resprayed state to false
 * 
 * @param {Car} self
 */
bool HasCarBeenResprayed(CVehicle& self) {
    const bool resprayed                = self.vehicleFlags.bHasBeenResprayed;
    self.vehicleFlags.bHasBeenResprayed = false;
    return resprayed;
}

/*
 * @opcode 0A21
 * @command IMPROVE_CAR_BY_CHEATING
 * @class Car
 * @method ImproveByCheating
 * 
 * @brief Sets whether a ped driven vehicle's handling is affected by the 'perfect
 * @brief handling' cheat
 * 
 * @param {Car} self
 * @param {bool} state
 */
void ImproveCarByCheating(CVehicle& self, bool state) {
    self.vehicleFlags.bUseCarCheats = state;
}

/*
 * @opcode 0A30
 * @command FIX_CAR
 * @class Car
 * @method Fix
 * 
 * @brief Restores the vehicle to full health and removes the damage
 * 
 * @param {Car} self
 */
void FixCar(CVehicle& self) {
    self.Fix();
    self.m_fHealth = 1000.f;
}

/*
 * @opcode 08EC
 * @command GET_VEHICLE_CLASS
 * @class Car
 * @method GetClass
 * 
 * @brief Returns the vehicle's class as defined in vehicles.ide
 * 
 * @param {Car} self
 * 
 * @returns {VehicleClass} class
 */
auto GetVehicleClass(CVehicle& self) {
    return self.GetVehicleModelInfo()->m_nVehicleClass;
}

/*
 * @opcode 084E
 * @command SET_VEHICLE_CAN_BE_TARGETTED
 * @class Car
 * @method SetCanBeTargeted
 * 
 * @brief Sets whether the vehicle can be targeted
 * 
 * @param {Car} self
 * @param {bool} state
 */
void SetVehicleCanBeTargetted(CVehicle& self, bool state) {
    self.vehicleFlags.bVehicleCanBeTargetted = state;
}

/*
 * @opcode 06EC
 * @command GET_NUM_AVAILABLE_PAINTJOBS
 * @class Car
 * @method GetNumAvailablePaintjobs
 * 
 * @brief Gets the number of possible paintjobs that can be applied to the car
 * 
 * @param {Car} self
 * 
 * @returns {int} numPaintjobs
 */
auto GetNumAvailablePaintjobs(CVehicle& self) {
    return self.GetVehicleModelInfo()->GetNumRemaps();
}

/*
 * @opcode 0841
 * @command SELECT_WEAPONS_FOR_VEHICLE
 * @class Car
 * @method SelectWeapons
 * 
 * @brief Sets the vehicle to use its secondary guns
 * 
 * @param {Car} self
 * @param {CarWeapon} weapon
 */
void SelectWeaponsForVehicle(CVehicle& self, eCarWeapon weapon) {
    self.m_nVehicleWeaponInUse = weapon;
}

/*
 * @opcode 06E5
 * @command GET_AVAILABLE_VEHICLE_MOD
 * @class Car
 * @method GetAvailableMod
 * 
 * @brief Returns a model id available for the vehicle's mod slot, or -1 otherwise
 * 
 * @param {Car} self
 * @param {ModSlot} slotId
 * 
 * @returns {model_object} modelId
 */
auto GetAvailableVehicleMod(CVehicle& self, eModSlot slotId) {
    return self.GetVehicleModelInfo()->m_anUpgrades[+slotId];
}

/*
 * @opcode 0686
 * @command IS_VEHICLE_ATTACHED
 * @class Car
 * @method IsAttached
 * @static
 * 
 * @param {Car} car
 */
bool IsVehicleAttached(CVehicle* car) {
    return car && car->m_pAttachedTo;
}

/*
 * @opcode 0878
 * @command SET_VEHICLE_DIRT_LEVEL
 * @class Car
 * @method SetDirtLevel
 * 
 * @brief Sets the dirt level of the car
 * 
 * @param {Car} self
 * @param {float} level
 */
void SetVehicleDirtLevel(CVehicle& self, float level) {
    self.m_fDirtLevel = level;
}

/*
 * @opcode 056E
 * @command DOES_VEHICLE_EXIST
 * @class Car
 * @method DoesExist
 * @static
 * 
 * @brief Returns true if the handle is a valid vehicle handle
 * 
 * @param {Car} car
 */
bool DoesVehicleExist(CVehicle* car) {
    return car != nullptr;
}

/*
 * @opcode 07C5
 * @command GET_VEHICLE_QUATERNION
 * @class Car
 * @method GetQuaternion
 * 
 * @brief Gets the quaternion values of the car
 * 
 * @param {Car} self
 * 
 * @returns {float} x, {float} y, {float} z, {float} w
 */
auto GetVehicleQuaternion(CVehicle& self) {
    const auto mm = self.GetModellingMatrix();
    CQuaternion q;
    q.Set(*self.GetModellingMatrix());
    return q;
}

/*
 * @opcode 0432
 * @command GET_CHAR_IN_CAR_PASSENGER_SEAT
 * @class Car
 * @method GetCharInPassengerSeat
 * 
 * @brief Returns the handle of a character sitting in the specified car seat
 * 
 * @param {Car} self
 * @param {SeatId} seat
 * 
 * @returns {Char} handle
 */
auto GetCharInCarPassengerSeat(CVehicle& self, eSeatId seat) {
    return self.GetPassengers()[+seat];
}

/*
 * @opcode 09D0
 * @command IS_VEHICLE_ON_ALL_WHEELS
 * @class Car
 * @method IsOnAllWheels
 * 
 * @brief Returns true if all the vehicle's wheels are touching the ground
 * 
 * @param {Car} self
 */
bool IsVehicleOnAllWheels(CVehicle& self) {
    if (self.IsBike()) {
        return self.AsBike()->m_nNoOfContactWheels == 4; // ?????????
    }
    if (self.IsAutomobile()) {
        return self.AsAutomobile()->m_nNumContactWheels == 4;
    }
    return false;
}

/*
 * @opcode 07C6
 * @command SET_VEHICLE_QUATERNION
 * @class Car
 * @method SetQuaternion
 * 
 * @brief Sets the rotation of a vehicle using quaternion values
 * 
 * @param {Car} self
 * @param {Quaternion} quaternion
 */
void SetVehicleQuaternion(CVehicle& self, CQuaternion quaternion) {
    const CVector pos = self.GetPosition();
    self.SetMatrix(CMatrix{quaternion.GetAs<CMatrix>()});
    self.GetMatrix().SetTranslate(pos);
}

/*
 * @opcode 0840
 * @command SET_VEHICLE_AREA_VISIBLE
 * @class Car
 * @method SetAreaVisible
 * 
 * @param {Car} self
 * @param {AreaCode} areaCode
 */
void SetVehicleAreaVisible(CVehicle& self, eAreaCodes areaCode) {
    self.SetAreaCode(areaCode);
}

/*
 * @opcode 01EA
 * @command GET_MAXIMUM_NUMBER_OF_PASSENGERS
 * @class Car
 * @method GetMaximumNumberOfPassengers
 * 
 * @brief Returns the maximum number of passengers that could sit in the car
 * 
 * @param {Car} self
 * 
 * @returns {int} count
 */
auto GetMaximumNumberOfPassengers(CVehicle& self) {
    return self.m_nMaxPassengers;
}

/*
 * @opcode 088B
 * @command SET_VEHICLE_AIR_RESISTANCE_MULTIPLIER
 * @class Car
 * @method SetAirResistanceMultiplier
 * 
 * @brief Sets the air resistance for the vehicle
 * 
 * @param {Car} self
 * @param {float} multiplier
 */
void SetVehicleAirResistanceMultiplier(CVehicle& self, float multiplier) {
    auto* const hd = self.m_pHandlingData;
    if (!hd) {
        return;
    }
    self.m_fAirResistance = hd->m_fDragMult <= 0.01f
        ? hd->m_fDragMult
        : hd->m_fDragMult / 1000.f * 0.5f;
    self.m_fAirResistance *= multiplier;
}

/*
 * @opcode 0216
 * @command SET_TAXI_LIGHTS
 * @class Car
 * @method SetTaxiLights
 * 
 * @brief Sets whether the taxi's roof light is on
 * 
 * @param {Car} self
 * @param {bool} state
 */
void SetTaxiLights(CVehicle& self, bool state) {
    self.AsAutomobile()->SetTaxiLight(state);
}

/*
 * @opcode 08F3
 * @command SET_FREEBIES_IN_VEHICLE
 * @class Car
 * @method SetFreebies
 * 
 * @brief Sets whether the player can receive items from this vehicle, such as shotgun
 * @brief ammo from a police car and cash from a taxi
 * 
 * @param {Car} self
 * @param {bool} state
 */
void SetFreebiesInVehicle(CVehicle& self, bool state) {
    self.vehicleFlags.bFreebies = state;
}

/*
 * @opcode 06E7
 * @command ADD_VEHICLE_MOD
 * @class Car
 * @method AddMod
 * 
 * @brief Adds a new mod with the model to the vehicle
 * 
 * @param {Car} self
 * @param {model_object} modelId
 * 
 * @returns {int} handle
 */
auto AddVehicleMod(CVehicle& self, Model modelId) {
    self.AddVehicleUpgrade(modelId);
}

/*
 * @opcode 06E8
 * @command REMOVE_VEHICLE_MOD
 * @class Car
 * @method RemoveMod
 * 
 * @brief Removes the vehicle's mod with the specified model
 * 
 * @param {Car} self
 * @param {model_object} modelId
 */
void RemoveVehicleMod(CVehicle& self, Model modelId) {
    self.RemoveVehicleUpgrade(modelId);
}

/*
 * @opcode 09AB
 * @command RANDOM_PASSENGER_SAY
 * @class Car
 * @method RandomPassengerSay
 * 
 * @brief Makes a passenger in the vehicle speak from an ambient speech ID, if one exists
 * @brief for the character
 * 
 * @param {Car} self
 * @param {SpeechId} phrase
 */
void RandomPassengerSay(CVehicle& self, eGlobalSpeechContext phrase) {
    if (auto* passenger = self.PickRandomPassenger()) {
        passenger->Say(phrase);
    }
}

/*
 * @opcode 0969
 * @command IS_BIG_VEHICLE
 * @class Car
 * @method IsBig
 * 
 * @brief Returns true if the specified vehicle has the 'is big' flag set in vehicles
 * 
 * @param {Car} self
 */
bool IsBigVehicle(CVehicle& self) {
    return self.vehicleFlags.bIsBig;
}

/*
 * @opcode 0957
 * @command VEHICLE_DOES_PROVIDE_COVER
 * @class Car
 * @method DoesProvideCover
 * 
 * @brief Sets whether characters in combat will choose to use the vehicle as cover from
 * @brief gunfire
 * 
 * @param {Car} self
 * @param {bool} state
 */
void VehicleDoesProvideCover(CVehicle& self, bool state) {
    self.vehicleFlags.bDoesProvideCover = state;
}

/*
 * @opcode 0988
 * @command GET_CURRENT_VEHICLE_PAINTJOB
 * @class Car
 * @method GetCurrentPaintjob
 * 
 * @brief Gets the car's paintjob
 * 
 * @param {Car} self
 * 
 * @returns {int} paintjobNumber
 */
auto GetCurrentVehiclePaintjob(CVehicle& self) {
    return self.GetRemapIndex();
}

/*
 * @opcode 09C4
 * @command SET_PETROL_TANK_WEAKPOINT
 * @class Car
 * @method SetPetrolTankWeakpoint
 * 
 * @brief Sets whether the car can be blown up by shooting at the petrol tank
 * 
 * @param {Car} self
 * @param {bool} state
 */
void SetPetrolTankWeakpoint(CVehicle& self, bool state) {
    self.vehicleFlags.bPetrolTankIsWeakPoint = state;
}

/*
 * @opcode 06ED
 * @command GIVE_VEHICLE_PAINTJOB
 * @class Car
 * @method GivePaintjob
 * 
 * @brief Sets the car's paintjob
 * 
 * @param {Car} self
 * @param {int} paintjobId
 */
void GiveVehiclePaintjob(CVehicle& self, int32 paintjobId) {
    self.SetRemap(paintjobId);
}

/*
 * @opcode 01E9
 * @command GET_NUMBER_OF_PASSENGERS
 * @class Car
 * @method GetNumberOfPassengers
 * 
 * @brief Returns the number of passengers sitting in the car
 * 
 * @param {Car} self
 * 
 * @returns {int} count
 */
auto GetNumberOfPassengers(CVehicle& self) {
    return self.m_nNumPassengers;
}

/*
 * @opcode 0897
 * @command IS_VEHICLE_TOUCHING_OBJECT
 * @class Car
 * @method IsTouchingObject
 * @static
 * 
 * @brief Returns true if the vehicle is in contact with the object
 * 
 * @param {Car} self
 * @param {Object} handle
 */
bool IsVehicleTouchingObject(CVehicle* car, CObject& obj) {
    return car && car->GetHasCollidedWith(&obj);
}

/*
 * @opcode 08F2
 * @command VEHICLE_CAN_BE_TARGETTED_BY_HS_MISSILE
 * @class Car
 * @method CanBeTargetedByHsMissile
 * 
 * @brief Sets whether the player can target this vehicle with a heatseeking rocket
 * @brief launcher
 * 
 * @param {Car} self
 * @param {bool} state
 */
void VehicleCanBeTargettedByHsMissile(CVehicle& self, bool state) {
    self.vehicleFlags.bVehicleCanBeTargettedByHS = state;
}

/*
 * @opcode 08A4
 * @command CONTROL_MOVABLE_VEHICLE_PART
 * @class Car
 * @method ControlMovablePart
 * 
 * @brief Sets the angle of a vehicle's extra
 * 
 * @param {Car} self
 * @param {float} range
 */
void ControlMovableVehiclePart(CVehicle& self, float range) {
    self.AsAutomobile()->UpdateMovingCollision(range);
}

/*
 * @opcode 09B0
 * @command SET_VEHICLE_IS_CONSIDERED_BY_PLAYER
 * @class Car
 * @method SetIsConsideredByPlayer
 * 
 * @brief Makes player character ignore the car when enter vehicle key is used
 * 
 * @param {Car} self
 * @param {bool} state
 */
void SetVehicleIsConsideredByPlayer(CVehicle& self, bool state) {
    self.vehicleFlags.bConsideredByPlayer = state;
}

/*
 * @opcode 0594
 * @command SET_VEHICLE_TO_FADE_IN
 * @class Car
 * @method SetToFadeIn
 * 
 * @brief Sets the alpha transparency of a distant vehicle
 * 
 * @param {Car} self
 * @param {int} alpha
 */
void SetVehicleToFadeIn(CVehicle& self, int32 alpha) {
    CVisibilityPlugins::SetClumpAlpha(self.m_pRwClump, alpha);
}

/*
 * @opcode 095F
 * @command GET_DOOR_ANGLE_RATIO
 * @class Car
 * @method GetDoorAngleRatio
 * 
 * @brief Gets the specified car doors angle, relative to the hinge
 * 
 * @param {Car} self
 * @param {CarDoor} door
 * 
 * @returns {float} ratio
 */
auto GetDoorAngleRatio(CVehicle& self, eDoors door) {
    auto* const automobile = self.AsAutomobile();
    if (door >= MAX_DOORS) {
        return automobile->m_swingingChassis.m_angle;
    }
    return automobile->m_doors[+door].m_angle;
}

/*
 * @opcode 09FE
 * @command RESET_VEHICLE_HYDRAULICS
 * @class Car
 * @method ResetHydraulics
 * 
 * @brief This resets all the hydraulics on the car, making it "sit"
 * 
 * @param {Car} self
 */
//void ResetVehicleHydraulics(CVehicle& self) {
    //NOTSA_UNREACHABLE("Not implemented");
//}

/*
 * @opcode 08A5
 * @command WINCH_CAN_PICK_VEHICLE_UP
 * @class Car
 * @method WinchCanPickUp
 * 
 * @brief Sets whether the vehicle can be picked up using the magnocrane
 * 
 * @param {Car} self
 * @param {bool} state
 */
//void WinchCanPickVehicleUp(CVehicle& self, bool state) {
    //NOTSA_UNREACHABLE("Not implemented");
//}

/*
 * @opcode 0975
 * @command IS_EMERGENCY_SERVICES_VEHICLE
 * @class Car
 * @method IsEmergencyServices
 * 
 * @brief Returns true if the vehicle is an emergency vehicle
 * 
 * @param {Car} self
 */
//bool IsEmergencyServicesVehicle(CVehicle& self) {
    //NOTSA_UNREACHABLE("Not implemented");
//}
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
    REGISTER_COMMAND_HANDLER(COMMAND_CREATE_CAR, CreateCar);
    REGISTER_COMMAND_HANDLER(COMMAND_DELETE_CAR, DeleteCar);
    REGISTER_COMMAND_HANDLER(COMMAND_CAR_GOTO_COORDINATES, CarGotoCoordinates);
    REGISTER_COMMAND_HANDLER(COMMAND_CAR_GOTO_COORDINATES_ACCURATE, CarGotoCoordinatesAccurate);
    REGISTER_COMMAND_HANDLER(COMMAND_CAR_GOTO_COORDINATES_RACING, CarGotoCoordinatesRacing);
    REGISTER_COMMAND_HANDLER(COMMAND_CAR_WANDER_RANDOMLY, CarWanderRandomly);
    REGISTER_COMMAND_HANDLER(COMMAND_CAR_SET_IDLE, CarSetIdle);
    REGISTER_COMMAND_HANDLER(COMMAND_GET_CAR_COORDINATES, GetCarCoordinates);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_COORDINATES, SetCarCoordinates);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_CRUISE_SPEED, SetCarCruiseSpeed);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_MISSION, SetCarMission);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_CAR_DEAD, IsCarDead);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_CAR_MODEL, IsCarModel);
    REGISTER_COMMAND_HANDLER(COMMAND_CREATE_CAR_GENERATOR, CreateCarGenerator);
    REGISTER_COMMAND_HANDLER(COMMAND_ADD_BLIP_FOR_CAR_OLD, AddBlipForCarOld);

    //
    // Heading
    //
    REGISTER_COMMAND_HANDLER(COMMAND_GET_CAR_HEADING, GetCarHeading);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_HEADING, SetCarHeading);

    REGISTER_COMMAND_HANDLER(COMMAND_IS_CAR_HEALTH_GREATER, IsCarHealthGreater);
    REGISTER_COMMAND_HANDLER(COMMAND_ADD_BLIP_FOR_CAR, AddBlipForCar);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_CAR_STUCK_ON_ROOF, IsCarStuckOnRoof);

    //
    // Upsidedown Car Check
    //
    REGISTER_COMMAND_HANDLER(COMMAND_ADD_UPSIDEDOWN_CAR_CHECK, AddUpsidedownCarCheck);
    REGISTER_COMMAND_HANDLER(COMMAND_REMOVE_UPSIDEDOWN_CAR_CHECK, RemoveUpsidedownCarCheck);

    //
    // IsInArea checks
    //
    REGISTER_COMMAND_HANDLER(COMMAND_IS_CAR_IN_AREA_2D, IsCarInArea2D<false>);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_CAR_IN_AREA_3D, IsCarInArea3D);
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
    REGISTER_COMMAND_HANDLER(COMMAND_SET_TARGET_CAR_FOR_MISSION_GARAGE, SetTargetCarForMissionGarage); // TODO: This is Garages stuff, move it out from here
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

    //
    // Tyres
    //
    REGISTER_COMMAND_HANDLER(COMMAND_IS_CAR_TYRE_BURST, IsCarTyreBurst);
    REGISTER_COMMAND_HANDLER(COMMAND_BURST_CAR_TYRE, BurstCarTyre);
    REGISTER_COMMAND_HANDLER(COMMAND_FIX_CAR_TYRE, FixCarTyre);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CAN_BURST_CAR_TYRES, SetCanBurstCarTyres);

    REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_FORWARD_SPEED, SetCarForwardSpeed);
    REGISTER_COMMAND_HANDLER(COMMAND_MARK_CAR_AS_CONVOY_CAR, MarkCarAsConvoyCar);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_STRAIGHT_LINE_DISTANCE, SetCarStraightLineDistance);
    REGISTER_COMMAND_HANDLER(COMMAND_POP_CAR_BOOT, PopCarBoot);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_CAR_WAITING_FOR_WORLD_COLLISION, IsCarWaitingForWorldCollision);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_MODEL_COMPONENTS, SetCarModelComponents);
    REGISTER_COMMAND_HANDLER(COMMAND_FREEZE_CAR_POSITION, FreezeCarPosition);
    REGISTER_COMMAND_HANDLER(COMMAND_HAS_CAR_BEEN_DAMAGED_BY_CHAR, HasCarBeenDamagedByChar);
    REGISTER_COMMAND_HANDLER(COMMAND_HAS_CAR_BEEN_DAMAGED_BY_CAR, HasCarBeenDamagedByCar);
    REGISTER_COMMAND_HANDLER(COMMAND_CLEAR_CAR_LAST_DAMAGE_ENTITY, ClearCarLastDamageEntity);
    REGISTER_COMMAND_HANDLER(COMMAND_FREEZE_CAR_POSITION_AND_DONT_LOAD_COLLISION, FreezeCarPositionAndDontLoadCollision);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_LOAD_COLLISION_FOR_CAR_FLAG, SetLoadCollisionForCarFlag);

    //
    // Playback (`CVehicleRecording`)
    //
    REGISTER_COMMAND_HANDLER(COMMAND_START_PLAYBACK_RECORDED_CAR, StartPlaybackRecordedCar);
    REGISTER_COMMAND_HANDLER(COMMAND_STOP_PLAYBACK_RECORDED_CAR, StopPlaybackRecordedCar);
    REGISTER_COMMAND_HANDLER(COMMAND_PAUSE_PLAYBACK_RECORDED_CAR, PausePlaybackRecordedCar);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_PLAYBACK_SPEED, SetPlaybackSpeed);
    REGISTER_COMMAND_HANDLER(COMMAND_UNPAUSE_PLAYBACK_RECORDED_CAR, UnpausePlaybackRecordedCar);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_PLAYBACK_GOING_ON_FOR_CAR, IsPlaybackGoingOnForCar);
    REGISTER_COMMAND_HANDLER(COMMAND_SKIP_TO_END_AND_STOP_PLAYBACK_RECORDED_CAR, SkipToEndAndStopPlaybackRecordedCar);
    REGISTER_COMMAND_HANDLER(COMMAND_START_PLAYBACK_RECORDED_CAR_USING_AI, StartPlaybackRecordedCarUsingAI);
    REGISTER_COMMAND_HANDLER(COMMAND_SKIP_IN_PLAYBACK_RECORDED_CAR, SkipInPlaybackRecordedCar);
    REGISTER_COMMAND_HANDLER(COMMAND_START_PLAYBACK_RECORDED_CAR_LOOPED, StartPlaybackRecordedCarLooped);
    REGISTER_COMMAND_HANDLER(COMMAND_CHANGE_PLAYBACK_TO_USE_AI, ChangePlaybackToUseAi);

    REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_ESCORT_CAR_LEFT, SetCarEscortCarLeft);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_ESCORT_CAR_RIGHT, SetCarEscortCarRight);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_ESCORT_CAR_REAR, SetCarEscortCarRear);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_ESCORT_CAR_FRONT, SetCarEscortCarFront);

    //
    // Doors
    //
    REGISTER_COMMAND_HANDLER(COMMAND_OPEN_CAR_DOOR_A_BIT, OpenCarDoorABit);
    REGISTER_COMMAND_HANDLER(COMMAND_OPEN_CAR_DOOR, OpenCarDoor);
    REGISTER_COMMAND_HANDLER(COMMAND_CLOSE_ALL_CAR_DOORS, CloseAllCarDoors);
    REGISTER_COMMAND_HANDLER(COMMAND_DAMAGE_CAR_DOOR, DamageCarDoor);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_CAR_DOOR_FULLY_OPEN, IsCarDoorFullyOpen);
    REGISTER_COMMAND_HANDLER(COMMAND_POP_CAR_DOOR, PopCarDoor);
    REGISTER_COMMAND_HANDLER(COMMAND_FIX_CAR_DOOR, FixCarDoor);
    REGISTER_COMMAND_HANDLER(COMMAND_CONTROL_CAR_DOOR, ControlCarDoor);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_CAR_DOOR_DAMAGED, IsCarDoorDamaged);

    REGISTER_COMMAND_HANDLER(COMMAND_CUSTOM_PLATE_FOR_NEXT_CAR, CustomPlateForNextCar);
    REGISTER_COMMAND_HANDLER(COMMAND_FORCE_CAR_LIGHTS, ForceCarLights);

    //
    // Attach/detach
    //
    REGISTER_COMMAND_HANDLER(COMMAND_ATTACH_CAR_TO_CAR, AttachCarToCar);
    REGISTER_COMMAND_HANDLER(COMMAND_DETACH_CAR, DetachCar);

    REGISTER_COMMAND_HANDLER(COMMAND_TASK_EVERYONE_LEAVE_CAR, TaskEveryoneLeaveCar);

    //
    // Panels
    //
    REGISTER_COMMAND_HANDLER(COMMAND_POP_CAR_PANEL, PopCarPanel);
    REGISTER_COMMAND_HANDLER(COMMAND_FIX_CAR_PANEL, FixCarPanel);
    REGISTER_COMMAND_HANDLER(COMMAND_DAMAGE_CAR_PANEL, DamageCarPanel);

    REGISTER_COMMAND_HANDLER(COMMAND_GET_CAR_SPEED_VECTOR, GetCarSpeedVector);
    REGISTER_COMMAND_HANDLER(COMMAND_GET_CAR_MASS, GetCarMass);

    //
    // Roll
    //
    REGISTER_COMMAND_HANDLER(COMMAND_GET_CAR_ROLL, GetCarRoll);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_ROLL, SetCarRoll);

    // Pitch
    REGISTER_COMMAND_HANDLER(COMMAND_GET_CAR_PITCH, GetCarPitch);

    REGISTER_COMMAND_HANDLER(COMMAND_DOES_CAR_HAVE_STUCK_CAR_CHECK, DoesCarHaveStuckCarCheck);
    REGISTER_COMMAND_HANDLER(COMMAND_EXPLODE_CAR_IN_CUTSCENE, ExplodeCarInCutscene);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_STAY_IN_SLOW_LANE, SetCarStayInSlowLane);
    REGISTER_COMMAND_HANDLER(COMMAND_SUPPRESS_CAR_MODEL, SuppressCarModel);
    REGISTER_COMMAND_HANDLER(COMMAND_DONT_SUPPRESS_CAR_MODEL, DontSuppressCarModel);
    REGISTER_COMMAND_HANDLER(COMMAND_DONT_SUPPRESS_ANY_CAR_MODELS, DontSuppressAnyCarModels);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_CAN_GO_AGAINST_TRAFFIC, SetCarCanGoAgainstTraffic);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_AS_MISSION_CAR, SetCarAsMissionCar);
    REGISTER_COMMAND_HANDLER(COMMAND_APPLY_FORCE_TO_CAR, ApplyForceToCar);
    REGISTER_COMMAND_HANDLER(COMMAND_ADD_TO_CAR_ROTATION_VELOCITY, AddToCarRotationVelocity);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_ROTATION_VELOCITY, SetCarRotationVelocity);
    REGISTER_COMMAND_HANDLER(COMMAND_CONTROL_CAR_HYDRAULICS, ControlCarHydraulics);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_FOLLOW_CAR, SetCarFollowCar);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_HYDRAULICS, SetCarHydraulics);
    REGISTER_COMMAND_HANDLER(COMMAND_DOES_CAR_HAVE_HYDRAULICS, DoesCarHaveHydraulics);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_ENGINE_BROKEN, SetCarEngineBroken);
    REGISTER_COMMAND_HANDLER(COMMAND_GET_CAR_UPRIGHT_VALUE, GetCarUprightValue);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_CAN_BE_VISIBLY_DAMAGED, SetCarCanBeVisiblyDamaged);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_COORDINATES_NO_OFFSET, SetCarCoordinatesNoOffset);
    REGISTER_COMMAND_HANDLER(COMMAND_EXPLODE_CAR_IN_CUTSCENE_SHAKE_AND_BITS, ExplodeCarInCutsceneShakeAndBits);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_ENGINE_ON, SetCarEngineOn);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_LIGHTS_ON, SetCarLightsOn);
    REGISTER_COMMAND_HANDLER(COMMAND_ATTACH_CAR_TO_OBJECT, AttachCarToObject);
    REGISTER_COMMAND_HANDLER(COMMAND_STORE_CAR_MOD_STATE, StoreCarModState);
    REGISTER_COMMAND_HANDLER(COMMAND_RESTORE_CAR_MOD_STATE, RestoreCarModState);
    REGISTER_COMMAND_HANDLER(COMMAND_GET_CURRENT_CAR_MOD, GetCurrentCarMod);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_CAR_LOW_RIDER, IsCarLowRider);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_CAR_STREET_RACER, IsCarStreetRacer);
    REGISTER_COMMAND_HANDLER(COMMAND_GET_NUM_CAR_COLOURS, GetNumCarColours);
    REGISTER_COMMAND_HANDLER(COMMAND_GET_CAR_BLOCKING_CAR, GetCarBlockingCar);
    REGISTER_COMMAND_HANDLER(COMMAND_GET_CAR_MOVING_COMPONENT_OFFSET, GetCarMovingComponentOffset);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_COLLISION, SetCarCollision);
    REGISTER_COMMAND_HANDLER(COMMAND_GET_CAR_DOOR_LOCK_STATUS, GetCarDoorLockStatus);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_CAR_TOUCHING_CAR, IsCarTouchingCar);
    REGISTER_COMMAND_HANDLER(COMMAND_GET_CAR_MODEL_VALUE, GetCarModelValue);
    REGISTER_COMMAND_HANDLER(COMMAND_CREATE_CAR_GENERATOR_WITH_PLATE, CreateCarGeneratorWithPlate);
    REGISTER_COMMAND_HANDLER(COMMAND_GIVE_NON_PLAYER_CAR_NITRO, GiveNonPlayerCarNitro);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_EXTRA_CAR_COLOURS, SetExtraCarColours);
    REGISTER_COMMAND_HANDLER(COMMAND_HAS_CAR_BEEN_RESPRAYED, HasCarBeenResprayed);
    REGISTER_COMMAND_HANDLER(COMMAND_IMPROVE_CAR_BY_CHEATING, ImproveCarByCheating);
    REGISTER_COMMAND_HANDLER(COMMAND_FIX_CAR, FixCar);
    REGISTER_COMMAND_HANDLER(COMMAND_GET_VEHICLE_CLASS, GetVehicleClass);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_VEHICLE_CAN_BE_TARGETTED, SetVehicleCanBeTargetted);
    REGISTER_COMMAND_HANDLER(COMMAND_GET_NUM_AVAILABLE_PAINTJOBS, GetNumAvailablePaintjobs);
    REGISTER_COMMAND_HANDLER(COMMAND_SELECT_WEAPONS_FOR_VEHICLE, SelectWeaponsForVehicle);
    REGISTER_COMMAND_HANDLER(COMMAND_GET_AVAILABLE_VEHICLE_MOD, GetAvailableVehicleMod);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_VEHICLE_ATTACHED, IsVehicleAttached);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_VEHICLE_DIRT_LEVEL, SetVehicleDirtLevel);
    REGISTER_COMMAND_HANDLER(COMMAND_DOES_VEHICLE_EXIST, DoesVehicleExist);
    REGISTER_COMMAND_HANDLER(COMMAND_GET_VEHICLE_QUATERNION, GetVehicleQuaternion);
    REGISTER_COMMAND_HANDLER(COMMAND_GET_CHAR_IN_CAR_PASSENGER_SEAT, GetCharInCarPassengerSeat);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_VEHICLE_ON_ALL_WHEELS, IsVehicleOnAllWheels);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_VEHICLE_QUATERNION, SetVehicleQuaternion);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_VEHICLE_AREA_VISIBLE, SetVehicleAreaVisible);
    REGISTER_COMMAND_HANDLER(COMMAND_GET_MAXIMUM_NUMBER_OF_PASSENGERS, GetMaximumNumberOfPassengers);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_VEHICLE_AIR_RESISTANCE_MULTIPLIER, SetVehicleAirResistanceMultiplier);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_TAXI_LIGHTS, SetTaxiLights);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_FREEBIES_IN_VEHICLE, SetFreebiesInVehicle);
    REGISTER_COMMAND_HANDLER(COMMAND_ADD_VEHICLE_MOD, AddVehicleMod);
    REGISTER_COMMAND_HANDLER(COMMAND_REMOVE_VEHICLE_MOD, RemoveVehicleMod);
    REGISTER_COMMAND_HANDLER(COMMAND_RANDOM_PASSENGER_SAY, RandomPassengerSay);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_BIG_VEHICLE, IsBigVehicle);
    REGISTER_COMMAND_HANDLER(COMMAND_VEHICLE_DOES_PROVIDE_COVER, VehicleDoesProvideCover);
    REGISTER_COMMAND_HANDLER(COMMAND_GET_CURRENT_VEHICLE_PAINTJOB, GetCurrentVehiclePaintjob);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_PETROL_TANK_WEAKPOINT, SetPetrolTankWeakpoint);
    REGISTER_COMMAND_HANDLER(COMMAND_GIVE_VEHICLE_PAINTJOB, GiveVehiclePaintjob);
    REGISTER_COMMAND_HANDLER(COMMAND_GET_NUMBER_OF_PASSENGERS, GetNumberOfPassengers);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_VEHICLE_TOUCHING_OBJECT, IsVehicleTouchingObject);
    REGISTER_COMMAND_HANDLER(COMMAND_VEHICLE_CAN_BE_TARGETTED_BY_HS_MISSILE, VehicleCanBeTargettedByHsMissile);
    REGISTER_COMMAND_HANDLER(COMMAND_CONTROL_MOVABLE_VEHICLE_PART, ControlMovableVehiclePart);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_VEHICLE_IS_CONSIDERED_BY_PLAYER, SetVehicleIsConsideredByPlayer);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_VEHICLE_TO_FADE_IN, SetVehicleToFadeIn);
    REGISTER_COMMAND_HANDLER(COMMAND_GET_DOOR_ANGLE_RATIO, GetDoorAngleRatio);
    //REGISTER_COMMAND_HANDLER(COMMAND_IS_CAR_IN_AREA_2D, IsCarInArea2d);
    //REGISTER_COMMAND_HANDLER(COMMAND_RESET_VEHICLE_HYDRAULICS, ResetVehicleHydraulics);
    //REGISTER_COMMAND_HANDLER(COMMAND_WINCH_CAN_PICK_VEHICLE_UP, WinchCanPickVehicleUp);
    //REGISTER_COMMAND_HANDLER(COMMAND_IS_EMERGENCY_SERVICES_VEHICLE, IsEmergencyServicesVehicle);

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
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_START_PLAYBACK_RECORDED_CAR_USING_AI_LOOPED);
}
