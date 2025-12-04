#include <StdInc.h>

#include "Commands.hpp"
#include <CommandParser/Parser.hpp>
#include <cassert>

#include "PlayerInfo.h"
#include "World.h"
#include "CarGenerator.h"
#include "TheCarGenerators.h"
#include "CommandParser/Parser.hpp"
using namespace notsa::script;

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

/// CAR_GOTO_COORDINATES
void CarGotoCoordinates(CVehicle& vehicle, CVector where) {
    auto& ap = vehicle.m_autoPilot;

    if (where.z <= MAP_Z_LOW_LIMIT) {
        where.z = CWorld::FindGroundZForCoord(where.x, where.y);
    }
    where.z += vehicle.GetDistanceFromCentreOfMassToBaseOfModel();

    const auto joined = CCarCtrl::JoinCarWithRoadSystemGotoCoors(&vehicle, where, false);

    vehicle.SetStatus(STATUS_PHYSICS);
    vehicle.SetEngineOn(true);

    if (!notsa::contains({ MISSION_PLANE_CRASH_AND_BURN, MISSION_HELI_CRASH_AND_BURN }, vehicle.m_autoPilot.m_nCarMission)) {
        ap.m_nCarMission = joined
            ? MISSION_GOTOCOORDINATES
            : MISSION_GOTOCOORDINATES_STRAIGHTLINE;
    }
    if (ap.m_nCruiseSpeed <= 1) {
        ap.SetCruiseSpeed(1);
    }
    ap.m_nTimeToStartMission = CTimer::GetTimeInMS();
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

/// IS_CAR_IN_AREA_2D
//ReturnType IsCarInArea2D(CVehicle& vehicle) {
//}

/// IS_CAR_IN_AREA_3D
//ReturnType IsCarInArea3D(CVehicle& vehicle) {
//}

/// IS_CAR_DEAD
//ReturnType IsCarDead(CVehicle& vehicle) {
//}

/// IS_CAR_MODEL
//ReturnType IsCarModel(CVehicle& vehicle) {
//}

/// CREATE_CAR_GENERATOR
//ReturnType CreateCarGenerator(CVehicle& vehicle) {
//}

/// ADD_BLIP_FOR_CAR_OLD
//ReturnType AddBlipForCarOld(CVehicle& vehicle) {
//}

/// GET_CAR_HEADING
//ReturnType GetCarHeading(CVehicle& vehicle) {
//}

/// SET_CAR_HEADING
//ReturnType SetCarHeading(CVehicle& vehicle) {
//}

/// IS_CAR_HEALTH_GREATER
//ReturnType IsCarHealthGreater(CVehicle& vehicle) {
//}

/// ADD_BLIP_FOR_CAR
//ReturnType AddBlipForCar(CVehicle& vehicle) {
//}

/// IS_CAR_STUCK_ON_ROOF
//ReturnType IsCarStuckOnRoof(CVehicle& vehicle) {
//}

/// ADD_UPSIDEDOWN_CAR_CHECK
//ReturnType AddUpsidedownCarCheck(CVehicle& vehicle) {
//}

/// REMOVE_UPSIDEDOWN_CAR_CHECK
//ReturnType RemoveUpsidedownCarCheck(CVehicle& vehicle) {
//}

/// IS_CAR_STOPPED_IN_AREA_2D
//ReturnType IsCarStoppedInArea2D(CVehicle& vehicle) {
//}

/// IS_CAR_STOPPED_IN_AREA_3D
//ReturnType IsCarStoppedInArea3D(CVehicle& vehicle) {
//}

/// LOCATE_CAR_2D
//ReturnType LocateCar2D(CVehicle& vehicle) {
//}

/// LOCATE_STOPPED_CAR_2D
//ReturnType LocateStoppedCar2D(CVehicle& vehicle) {
//}

/// LOCATE_CAR_3D
//ReturnType LocateCar3D(CVehicle& vehicle) {
//}

/// LOCATE_STOPPED_CAR_3D
//ReturnType LocateStoppedCar3D(CVehicle& vehicle) {
//}

/// IS_CAR_STOPPED
//ReturnType IsCarStopped(CVehicle& vehicle) {
//}

/// MARK_CAR_AS_NO_LONGER_NEEDED
//ReturnType MarkCarAsNoLongerNeeded(CVehicle& vehicle) {
//}

/// SET_CAR_DENSITY_MULTIPLIER
//ReturnType SetCarDensityMultiplier(CVehicle& vehicle) {
//}

/// SET_CAR_HEAVY
//ReturnType SetCarHeavy(CVehicle& vehicle) {
//}

/// IS_CAR_UPSIDEDOWN
//ReturnType IsCarUpsidedown(CVehicle& vehicle) {
//}

/// LOCK_CAR_DOORS
//ReturnType LockCarDoors(CVehicle& vehicle) {
//}

/// EXPLODE_CAR
//ReturnType ExplodeCar(CVehicle& vehicle) {
//}

/// IS_CAR_UPRIGHT
//ReturnType IsCarUpright(CVehicle& vehicle) {
//}

/// SET_TARGET_CAR_FOR_MISSION_GARAGE
//ReturnType SetTargetCarForMissionGarage(CVehicle& vehicle) {
//}

/// SET_CAR_HEALTH
//ReturnType SetCarHealth(CVehicle& vehicle) {
//}

/// GET_CAR_HEALTH
//ReturnType GetCarHealth(CVehicle& vehicle) {
//}

/// CHANGE_CAR_COLOUR
//ReturnType ChangeCarColour(CVehicle& vehicle) {
//}

/// SET_CAN_RESPRAY_CAR
//ReturnType SetCanResprayCar(CVehicle& vehicle) {
//}

/// SET_CAR_ONLY_DAMAGED_BY_PLAYER
//ReturnType SetCarOnlyDamagedByPlayer(CVehicle& vehicle) {
//}

/// IS_CAR_IN_WATER
//ReturnType IsCarInWater(CVehicle& vehicle) {
//}

/// GET_CLOSEST_CAR_NODE
//ReturnType GetClosestCarNode(CVehicle& vehicle) {
//}

/// CAR_GOTO_COORDINATES_ACCURATE
//ReturnType CarGotoCoordinatesAccurate(CVehicle& vehicle) {
//}

/// IS_CAR_ON_SCREEN
//ReturnType IsCarOnScreen(CVehicle& vehicle) {
//}

/// GET_CAR_FORWARD_X
//ReturnType GetCarForwardX(CVehicle& vehicle) {
//}

/// GET_CAR_FORWARD_Y
//ReturnType getCarForwardY(CVehicle& vehicle) {
//}

/// HAS_CAR_BEEN_DAMAGED_BY_WEAPON
//ReturnType hasCarBeenDamagedByWeapon(CVehicle& vehicle) {
//}

/// START_CAR_FIRE
//ReturnType StartCarFire(CVehicle& vehicle) {
//}

/// GET_RANDOM_CAR_OF_TYPE_IN_AREA
//ReturnType GetRandomCarOfTypeInArea(CVehicle& vehicle) {
//}

/// SET_CAR_VISIBLE
//ReturnType SetCarVisible(CVehicle& vehicle) {
//}

/// PLACE_OBJECT_RELATIVE_TO_CAR
//ReturnType PlaceObjectRelativeToCar(CVehicle& vehicle) {
//}

/// SWITCH_CAR_SIREN
//ReturnType SwitchCarSiren(CVehicle& vehicle) {
//}

/// SET_CAR_WATERTIGHT
//ReturnType SetCarWatertight(CVehicle& vehicle) {
//}

/// TURN_CAR_TO_FACE_COORD
//ReturnType TurnCarToFaceCoord(CVehicle& vehicle) {
//}

/// SET_CAR_STATUS
//ReturnType SetCarStatus(CVehicle& vehicle) {
//}

/// SET_CAR_STRONG
//ReturnType SetCarStrong(CVehicle& vehicle) {
//}

/// CLEAR_AREA_OF_CARS
//ReturnType ClearAreaOfCars(CVehicle& vehicle) {
//}

/// CREATE_RANDOM_CAR_FOR_CAR_PARK
//ReturnType CreateRandomCarForCarPark(CVehicle& vehicle) {
//}

/// IS_CAR_VISIBLY_DAMAGED
//ReturnType IsCarVisiblyDamaged(CVehicle& vehicle) {
//}

/// GET_CLOSEST_CAR_NODE_WITH_HEADING
//ReturnType GetClosestCarNodeWithHeading(CVehicle& vehicle) {
//}

/// SET_UPSIDEDOWN_CAR_NOT_DAMAGED
//ReturnType SetUpsidedownCarNotDamaged(CVehicle& vehicle) {
//}

/// GET_CAR_COLOURS
//ReturnType GetCarColours(CVehicle& vehicle) {
//}

/// SET_ALL_CARS_CAN_BE_DAMAGED
//ReturnType SetAllCarsCanBeDamaged(CVehicle& vehicle) {
//}

/// SET_CAR_CAN_BE_DAMAGED
//ReturnType SetCarCanBeDamaged(CVehicle& vehicle) {
//}

/// GET_OFFSET_FROM_CAR_IN_WORLD_COORDS
//ReturnType GetOffsetFromCarInWorldCoords(CVehicle& vehicle) {
//}

/// SET_FREE_HEALTH_CARE
//ReturnType SetFreeHealthCare(CVehicle& vehicle) {
//}

/// SET_CAR_TRACTION
//ReturnType SetCarTraction(CVehicle& vehicle) {
//}

/// SET_CAR_AVOID_LEVEL_TRANSITIONS
//ReturnType SetCarAvoidLevelTransitions(CVehicle& vehicle) {
//}

/// IS_CAR_PASSENGER_SEAT_FREE
//ReturnType IsCarPassengerSeatFree(CVehicle& vehicle) {
//}

/// GET_CAR_MODEL
//ReturnType GetCarModel(CVehicle& vehicle) {
//}

/// SET_CAR_STAY_IN_FAST_LANE
//ReturnType SetCarStayInFastLane(CVehicle& vehicle) {
//}

/// CLEAR_CAR_LAST_WEAPON_DAMAGE
//ReturnType ClearCarLastWeaponDamage(CVehicle& vehicle) {
//}

/// GET_DRIVER_OF_CAR
//ReturnType GetDriverOfCar(CVehicle& vehicle) {
//}

/// LOCATE_CHAR_IN_CAR_OBJECT_2D
//ReturnType LocateCharInCarObject2D(CVehicle& vehicle) {
//}

/// LOCATE_CHAR_IN_CAR_OBJECT_3D
//ReturnType LocateCharInCarObject3D(CVehicle& vehicle) {
//}

/// SET_CAR_TEMP_ACTION
//ReturnType SetCarTempAction(CVehicle& vehicle) {
//}

/// GET_REMOTE_CONTROLLED_CAR
//ReturnType GetRemoteControlledCar(CVehicle& vehicle) {
//}

/// SET_CAR_RANDOM_ROUTE_SEED
//ReturnType SetCarRandomRouteSeed(CVehicle& vehicle) {
//}

/// IS_CAR_ON_FIRE
//ReturnType IsCarOnFire(CVehicle& vehicle) {
//}

/// IS_CAR_TYRE_BURST
//ReturnType IsCarTyreBurst(CVehicle& vehicle) {
//}

/// SET_CAR_FORWARD_SPEED
//ReturnType SetCarForwardSpeed(CVehicle& vehicle) {
//}

/// MARK_CAR_AS_CONVOY_CAR
//ReturnType MarkCarAsConvoyCar(CVehicle& vehicle) {
//}

/// GET_NTH_CLOSEST_CAR_NODE
//ReturnType GetNthClosestCarNode(CVehicle& vehicle) {
//}

/// SET_CAR_STRAIGHT_LINE_DISTANCE
//ReturnType SetCarStraightLineDistance(CVehicle& vehicle) {
//}

/// POP_CAR_BOOT
//ReturnType PopCarBoot(CVehicle& vehicle) {
//}

/// IS_CAR_WAITING_FOR_WORLD_COLLISION
//ReturnType IsCarWaitingForWorldCollision(CVehicle& vehicle) {
//}

/// BURST_CAR_TYRE
//ReturnType BurstCarTyre(CVehicle& vehicle) {
//}

/// SET_CAR_MODEL_COMPONENTS
//ReturnType SetCarModelComponents(CVehicle& vehicle) {
//}

/// CLOSE_ALL_CAR_DOORS
//ReturnType CloseAllCarDoors(CVehicle& vehicle) {
//}

/// SORT_OUT_OBJECT_COLLISION_WITH_CAR
//ReturnType SortOutObjectCollisionWithCar(CVehicle& vehicle) {
//}

/// FREEZE_CAR_POSITION
//ReturnType FreezeCarPosition(CVehicle& vehicle) {
//}

/// HAS_CAR_BEEN_DAMAGED_BY_CHAR
//ReturnType HasCarBeenDamagedByChar(CVehicle& vehicle) {
//}

/// HAS_CAR_BEEN_DAMAGED_BY_CAR
//ReturnType HasCarBeenDamagedByCar(CVehicle& vehicle) {
//}

/// GET_RANDOM_CAR_OF_TYPE_IN_AREA_NO_SAVE
//ReturnType GetRandomCarOfTypeInAreaNoSave(CVehicle& vehicle) {
//}

/// SET_CAN_BURST_CAR_TYRES
//ReturnType SetCanBurstCarTyres(CVehicle& vehicle) {
//}

/// CLEAR_CAR_LAST_DAMAGE_ENTITY
//ReturnType ClearCarLastDamageEntity(CVehicle& vehicle) {
//}

/// FREEZE_CAR_POSITION_AND_DONT_LOAD_COLLISION
//ReturnType FreezeCarPositionAndDontLoadCollision(CVehicle& vehicle) {
//}

/// SET_LOAD_COLLISION_FOR_CAR_FLAG
//ReturnType SetLoadCollisionForCarFlag(CVehicle& vehicle) {
//}

/// SHUFFLE_CARD_DECKS
//ReturnType ShuffleCardDecks(CVehicle& vehicle) {
//}

/// FETCH_NEXT_CARD
//ReturnType FetchNextCard(CVehicle& vehicle) {
//}

/// TASK_ENTER_CAR_AS_PASSENGER
//ReturnType TaskEnterCarAsPassenger(CVehicle& vehicle) {
//}

/// TASK_ENTER_CAR_AS_DRIVER
//ReturnType TaskEnterCarAsDriver(CVehicle& vehicle) {
//}

/// TASK_LEAVE_CAR
//ReturnType TaskLeaveCar(CVehicle& vehicle) {
//}

/// TASK_LEAVE_CAR_AND_FLEE
//ReturnType TaskLeaveCarAndFlee(CVehicle& vehicle) {
//}

/// TASK_CAR_DRIVE_TO_COORD
//ReturnType TaskCarDriveToCoord(CVehicle& vehicle) {
//}

/// TASK_CAR_DRIVE_WANDER
//ReturnType TaskCarDriveWander(CVehicle& vehicle) {
//}

/// START_PLAYBACK_RECORDED_CAR
//ReturnType StartPlaybackRecordedCar(CVehicle& vehicle) {
//}

/// STOP_PLAYBACK_RECORDED_CAR
//ReturnType StopPlaybackRecordedCar(CVehicle& vehicle) {
//}

/// PAUSE_PLAYBACK_RECORDED_CAR
//ReturnType PausePlaybackRecordedCar(CVehicle& vehicle) {
//}

/// UNPAUSE_PLAYBACK_RECORDED_CAR
//ReturnType UnpausePlaybackRecordedCar(CVehicle& vehicle) {
//}

/// SET_CAR_ESCORT_CAR_LEFT
//ReturnType SetCarEscortCarLeft(CVehicle& vehicle) {
//}

/// SET_CAR_ESCORT_CAR_RIGHT
//ReturnType SetCarEscortCarRight(CVehicle& vehicle) {
//}

/// SET_CAR_ESCORT_CAR_REAR
//ReturnType SetCarEscortCarRear(CVehicle& vehicle) {
//}

/// SET_CAR_ESCORT_CAR_FRONT
//ReturnType SetCarEscortCarFront(CVehicle& vehicle) {
//}

/// IS_CHAR_STOPPED_IN_ANGLED_AREA_IN_CAR_2D
//ReturnType IsCharStoppedInAngledAreaInCar2D(CVehicle& vehicle) {
//}

/// IS_CHAR_STOPPED_IN_ANGLED_AREA_IN_CAR_3D
//ReturnType IsCharStoppedInAngledAreaInCar3D(CVehicle& vehicle) {
//}

/// IS_PLAYBACK_GOING_ON_FOR_CAR
//ReturnType IsPlaybackGoingOnForCar(CVehicle& vehicle) {
//}

/// TASK_LEAVE_CAR_IMMEDIATELY
//ReturnType TaskLeaveCarImmediately(CVehicle& vehicle) {
//}

/// TASK_LEAVE_ANY_CAR
//ReturnType TaskLeaveAnyCar(CVehicle& vehicle) {
//}

/// OPEN_CAR_DOOR
//ReturnType OpenCarDoor(CVehicle& vehicle) {
//}

/// CREATE_FX_SYSTEM_ON_CAR
//ReturnType CreateFxSystemOnCar(CVehicle& vehicle) {
//}

/// CREATE_FX_SYSTEM_ON_CAR_WITH_DIRECTION
//ReturnType CreateFxSystemOnCarWithDirection(CVehicle& vehicle) {
//}

/// TASK_DESTROY_CAR
//ReturnType TaskDestroyCar(CVehicle& vehicle) {
//}

/// CUSTOM_PLATE_FOR_NEXT_CAR
//ReturnType CustomPlateForNextCar(CVehicle& vehicle) {
//}

/// TASK_SHUFFLE_TO_NEXT_CAR_SEAT
//ReturnType TaskShuffleToNextCarSeat(CVehicle& vehicle) {
//}

/// FORCE_CAR_LIGHTS
//ReturnType ForceCarLights(CVehicle& vehicle) {
//}

/// ATTACH_OBJECT_TO_CAR
//ReturnType AttachObjectToCar(CVehicle& vehicle) {
//}

/// ATTACH_CAR_TO_CAR
//ReturnType AttachCarToCar(CVehicle& vehicle) {
//}

/// DETACH_CAR
//ReturnType DetachCar(CVehicle& vehicle) {
//}

/// POP_CAR_DOOR
//ReturnType PopCarDoor(CVehicle& vehicle) {
//}

/// FIX_CAR_DOOR
//ReturnType FixCarDoor(CVehicle& vehicle) {
//}

/// TASK_EVERYONE_LEAVE_CAR
//ReturnType TaskEveryoneLeaveCar(CVehicle& vehicle) {
//}

/// POP_CAR_PANEL
//ReturnType PopCarPanel(CVehicle& vehicle) {
//}

/// FIX_CAR_PANEL
//ReturnType FixCarPanel(CVehicle& vehicle) {
//}

/// FIX_CAR_TYRE
//ReturnType FixCarTyre(CVehicle& vehicle) {
//}

/// GET_CAR_SPEED_VECTOR
//ReturnType GetCarSpeedVector(CVehicle& vehicle) {
//}

/// GET_CAR_MASS
//ReturnType GetCarMass(CVehicle& vehicle) {
//}

/// GET_CAR_ROLL
//ReturnType GetCarRoll(CVehicle& vehicle) {
//}

/// SKIP_TO_END_AND_STOP_PLAYBACK_RECORDED_CAR
//ReturnType SkipToEndAndStopPlaybackRecordedCar(CVehicle& vehicle) {
//}

/// TASK_CAR_TEMP_ACTION
//ReturnType TaskCarTempAction(CVehicle& vehicle) {
//}

/// TASK_CAR_MISSION
//ReturnType TaskCarMission(CVehicle& vehicle) {
//}

/// GET_NTH_CLOSEST_CAR_NODE_WITH_HEADING
//ReturnType GetNthClosestCarNodeWithHeading(CVehicle& vehicle) {
//}

/// DOES_CAR_HAVE_STUCK_CAR_CHECK
//ReturnType DoesCarHaveStuckCarCheck(CVehicle& vehicle) {
//}

/// GET_CAR_VALUE
//ReturnType GetCarValue(CVehicle& vehicle) {
//}

/// CAR_GOTO_COORDINATES_RACING
//ReturnType CarGotoCoordinatesRacing(CVehicle& vehicle) {
//}

/// START_PLAYBACK_RECORDED_CAR_USING_AI
//ReturnType StartPlaybackRecordedCarUsingAi(CVehicle& vehicle) {
//}

/// SKIP_IN_PLAYBACK_RECORDED_CAR
//ReturnType SkipInPlaybackRecordedCar(CVehicle& vehicle) {
//}

/// EXPLODE_CAR_IN_CUTSCENE
//ReturnType ExplodeCarInCutscene(CVehicle& vehicle) {
//}

/// SET_CAR_STAY_IN_SLOW_LANE
//ReturnType SetCarStayInSlowLane(CVehicle& vehicle) {
//}

/// TAKE_REMOTE_CONTROL_OF_CAR
//ReturnType TakeRemoteControlOfCar(CVehicle& vehicle) {
//}

/// TASK_WARP_CHAR_INTO_CAR_AS_DRIVER
//ReturnType TaskWarpCharIntoCarAsDriver(CVehicle& vehicle) {
//}

/// TASK_WARP_CHAR_INTO_CAR_AS_PASSENGER
//ReturnType TaskWarpCharIntoCarAsPassenger(CVehicle& vehicle) {
//}

/// DAMAGE_CAR_PANEL
//ReturnType DamageCarPanel(CVehicle& vehicle) {
//}

/// SET_CAR_ROLL
//ReturnType SetCarRoll(CVehicle& vehicle) {
//}

/// SUPPRESS_CAR_MODEL
//ReturnType SuppressCarModel(CVehicle& vehicle) {
//}

/// DONT_SUPPRESS_CAR_MODEL
//ReturnType DontSuppressCarModel(CVehicle& vehicle) {
//}

/// DONT_SUPPRESS_ANY_CAR_MODELS
//ReturnType DontSuppressAnyCarModels(CVehicle& vehicle) {
//}

/// SET_CAR_CAN_GO_AGAINST_TRAFFIC
//ReturnType SetCarCanGoAgainstTraffic(CVehicle& vehicle) {
//}

/// DAMAGE_CAR_DOOR
//ReturnType DamageCarDoor(CVehicle& vehicle) {
//}

/// GET_RANDOM_CAR_IN_SPHERE_NO_SAVE
//ReturnType GetRandomCarInSphereNoSave(CVehicle& vehicle) {
//}

/// SET_CAR_AS_MISSION_CAR
//ReturnType SetCarAsMissionCar(CVehicle& vehicle) {
//}

/// CUSTOM_PLATE_DESIGN_FOR_NEXT_CAR
//ReturnType CustomPlateDesignForNextCar(CVehicle& vehicle) {
//}

/// TASK_GOTO_CAR
//ReturnType TaskGotoCar(CVehicle& vehicle) {
//}

/// GET_CAR_PITCH
//ReturnType GetCarPitch(CVehicle& vehicle) {
//}

/// GET_TRAIN_CARRIAGE
//ReturnType GetTrainCarriage(CVehicle& vehicle) {
//}

/// REQUEST_CAR_RECORDING
//ReturnType RequestCarRecording(CVehicle& vehicle) {
//}

/// HAS_CAR_RECORDING_BEEN_LOADED
//ReturnType HasCarRecordingBeenLoaded(CVehicle& vehicle) {
//}

/// APPLY_FORCE_TO_CAR
//ReturnType ApplyForceToCar(CVehicle& vehicle) {
//}

/// ADD_TO_CAR_ROTATION_VELOCITY
//ReturnType AddToCarRotationVelocity(CVehicle& vehicle) {
//}

/// SET_CAR_ROTATION_VELOCITY
//ReturnType SetCarRotationVelocity(CVehicle& vehicle) {
//}

/// CONTROL_CAR_HYDRAULICS
//ReturnType ControlCarHydraulics(CVehicle& vehicle) {
//}

/// SET_CAR_FOLLOW_CAR
//ReturnType SetCarFollowCar(CVehicle& vehicle) {
//}

/// SET_CAR_HYDRAULICS
//ReturnType SetCarHydraulics(CVehicle& vehicle) {
//}

/// DOES_CAR_HAVE_HYDRAULICS
//ReturnType DoesCarHaveHydraulics(CVehicle& vehicle) {
//}

/// SET_CAR_ENGINE_BROKEN
//ReturnType SetCarEngineBroken(CVehicle& vehicle) {
//}

/// GET_CAR_UPRIGHT_VALUE
//ReturnType GetCarUprightValue(CVehicle& vehicle) {
//}

/// SET_CAR_CAN_BE_VISIBLY_DAMAGED
//ReturnType SetCarCanBeVisiblyDamaged(CVehicle& vehicle) {
//}

/// CREATE_EMERGENCY_SERVICES_CAR
//ReturnType CreateEmergencyServicesCar(CVehicle& vehicle) {
//}

/// START_PLAYBACK_RECORDED_CAR_LOOPED
//ReturnType StartPlaybackRecordedCarLooped(CVehicle& vehicle) {
//}

/// REMOVE_CAR_RECORDING
//ReturnType RemoveCarRecording(CVehicle& vehicle) {
//}

/// SET_CAR_COORDINATES_NO_OFFSET
//ReturnType SetCarCoordinatesNoOffset(CVehicle& vehicle) {
//}

/// OPEN_CAR_DOOR_A_BIT
//ReturnType OpenCarDoorAbit(CVehicle& vehicle) {
//}

/// IS_CAR_DOOR_FULLY_OPEN
//ReturnType IsCarDoorFullyOpen(CVehicle& vehicle) {
//}

/// EXPLODE_CAR_IN_CUTSCENE_SHAKE_AND_BITS
//ReturnType ExplodeCarInCutsceneShakeAndBits(CVehicle& vehicle) {
//}

/// SET_PLANE_UNDERCARRIAGE_UP
//ReturnType SetPlaneUndercarriageUp(CVehicle& vehicle) {
//}

/// SET_CAR_ENGINE_ON
//ReturnType SetCarEngineOn(CVehicle& vehicle) {
//}

/// SET_CAR_LIGHTS_ON
//ReturnType SetCarLightsOn(CVehicle& vehicle) {
//}

/// GET_PLANE_UNDERCARRIAGE_POSITION
//ReturnType GetPlaneUndercarriagePosition(CVehicle& vehicle) {
//}

/// ATTACH_CAR_TO_OBJECT
//ReturnType AttachCarToObject(CVehicle& vehicle) {
//}

/// IS_CHAR_STUCK_UNDER_CAR
//ReturnType IsCharStuckUnderCar(CVehicle& vehicle) {
//}

/// CONTROL_CAR_DOOR
//ReturnType ControlCarDoor(CVehicle& vehicle) {
//}

/// STORE_CAR_MOD_STATE
//ReturnType StoreCarModState(CVehicle& vehicle) {
//}

/// RESTORE_CAR_MOD_STATE
//ReturnType RestoreCarModState(CVehicle& vehicle) {
//}

/// GET_CURRENT_CAR_MOD
//ReturnType GetCurrentCarMod(CVehicle& vehicle) {
//}

/// IS_CAR_LOW_RIDER
//ReturnType IsCarLowRider(CVehicle& vehicle) {
//}

/// IS_CAR_STREET_RACER
//ReturnType IsCarStreetRacer(CVehicle& vehicle) {
//}

/// GET_NUM_CAR_COLOURS
//ReturnType GetNumCarColours(CVehicle& vehicle) {
//}

/// SET_CHAR_FORCE_DIE_IN_CAR
//ReturnType SetCharForceDieInCar(CVehicle& vehicle) {
//}

/// GET_CAR_BLOCKING_CAR
//ReturnType GetCarBlockingCar(CVehicle& vehicle) {
//}

/// GET_CAR_MOVING_COMPONENT_OFFSET
//ReturnType GetCarMovingComponentOffset(CVehicle& vehicle) {
//}

/// SET_CAR_COLLISION
//ReturnType SetCarCollision(CVehicle& vehicle) {
//}

/// GET_RANDOM_CAR_MODEL_IN_MEMORY
//ReturnType GetRandomCarModelInMemory(CVehicle& vehicle) {
//}

/// GET_CAR_DOOR_LOCK_STATUS
//ReturnType GetCarDoorLockStatus(CVehicle& vehicle) {
//}

/// DISPLAY_CAR_NAMES
//ReturnType DisplayCarNames(CVehicle& vehicle) {
//}

/// IS_CAR_DOOR_DAMAGED
//ReturnType IsCarDoorDamaged(CVehicle& vehicle) {
//}

/// SET_FORCE_RANDOM_CAR_MODEL
//ReturnType SetForceRandomCarModel(CVehicle& vehicle) {
//}

/// GET_RANDOM_CAR_OF_TYPE_IN_ANGLED_AREA_NO_SAVE
//ReturnType GetRandomCarOfTypeInAngledAreaNoSave(CVehicle& vehicle) {
//}

/// IS_CAR_TOUCHING_CAR
//ReturnType IsCarTouchingCar(CVehicle& vehicle) {
//}

/// GET_CAR_MODEL_VALUE
//ReturnType GetCarModelValue(CVehicle& vehicle) {
//}

/// CREATE_CAR_GENERATOR_WITH_PLATE
//ReturnType CreateCarGeneratorWithPlate(CVehicle& vehicle) {
//}

/// SET_AIRCRAFT_CARRIER_SAM_SITE
//ReturnType SetAircraftCarrierSamSite(CVehicle& vehicle) {
//}

/// GIVE_NON_PLAYER_CAR_NITRO
//ReturnType GiveNonPlayerCarNitro(CVehicle& vehicle) {
//}

/// IS_THIS_MODEL_A_CAR
//ReturnType IsThisModelAcar(CVehicle& vehicle) {
//}

/// SET_EXTRA_CAR_COLOURS
//ReturnType SetExtraCarColours(CVehicle& vehicle) {
//}

/// HAS_CAR_BEEN_RESPRAYED
//ReturnType HasCarBeenResprayed(CVehicle& vehicle) {
//}

/// IMPROVE_CAR_BY_CHEATING
//ReturnType ImproveCarByCheating(CVehicle& vehicle) {
//}

/// CHANGE_CAR_COLOUR_FROM_MENU
//ReturnType ChangeCarColourFromMenu(CVehicle& vehicle) {
//}

/// FIX_CAR
//ReturnType FixCar(CVehicle& vehicle) {
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
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_AS_MISSION_CAR, SetCarAsMissionCar);
    REGISTER_COMMAND_HANDLER(COMMAND_CREATE_CAR, CreateCar);
    // REGISTER_COMMAND_HANDLER(COMMAND_DELETE_CAR, DeleteCar);
    REGISTER_COMMAND_HANDLER(COMMAND_CAR_GOTO_COORDINATES, CarGotoCoordinates);
    REGISTER_COMMAND_HANDLER(COMMAND_CAR_WANDER_RANDOMLY, CarWanderRandomly);
    REGISTER_COMMAND_HANDLER(COMMAND_CAR_SET_IDLE, CarSetIdle);
    REGISTER_COMMAND_HANDLER(COMMAND_GET_CAR_COORDINATES, GetCarCoordinates);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_COORDINATES, SetCarCoordinates);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_CRUISE_SPEED, SetCarCruiseSpeed);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_MISSION, SetCarMission);
    // REGISTER_COMMAND_HANDLER(COMMAND_IS_CAR_IN_AREA_2D, IsCarInArea2D);
    // REGISTER_COMMAND_HANDLER(COMMAND_IS_CAR_IN_AREA_3D, IsCarInArea3D);
    // REGISTER_COMMAND_HANDLER(COMMAND_IS_CAR_DEAD, IsCarDead);
    // REGISTER_COMMAND_HANDLER(COMMAND_IS_CAR_MODEL, IsCarModel);
    // REGISTER_COMMAND_HANDLER(COMMAND_CREATE_CAR_GENERATOR, CreateCarGenerator);
    // REGISTER_COMMAND_HANDLER(COMMAND_ADD_BLIP_FOR_CAR_OLD, AddBlipForCarOld);
    // REGISTER_COMMAND_HANDLER(COMMAND_GET_CAR_HEADING, GetCarHeading);
    // REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_HEADING, SetCarHeading);
    // REGISTER_COMMAND_HANDLER(COMMAND_IS_CAR_HEALTH_GREATER, IsCarHealthGreater);
    // REGISTER_COMMAND_HANDLER(COMMAND_ADD_BLIP_FOR_CAR, AddBlipForCar);
    // REGISTER_COMMAND_HANDLER(COMMAND_IS_CAR_STUCK_ON_ROOF, IsCarStuckOnRoof);
    // REGISTER_COMMAND_HANDLER(COMMAND_ADD_UPSIDEDOWN_CAR_CHECK, AddUpsidedownCarCheck);
    // REGISTER_COMMAND_HANDLER(COMMAND_REMOVE_UPSIDEDOWN_CAR_CHECK, RemoveUpsidedownCarCheck);
    // REGISTER_COMMAND_HANDLER(COMMAND_IS_CAR_STOPPED_IN_AREA_2D, IsCarStoppedInArea2D);
    // REGISTER_COMMAND_HANDLER(COMMAND_IS_CAR_STOPPED_IN_AREA_3D, IsCarStoppedInArea3D);
    // REGISTER_COMMAND_HANDLER(COMMAND_LOCATE_CAR_2D, LocateCar2D);
    // REGISTER_COMMAND_HANDLER(COMMAND_LOCATE_STOPPED_CAR_2D, LocateStoppedCar2D);
    // REGISTER_COMMAND_HANDLER(COMMAND_LOCATE_CAR_3D, LocateCar3D);
    // REGISTER_COMMAND_HANDLER(COMMAND_LOCATE_STOPPED_CAR_3D, LocateStoppedCar3D);
    // REGISTER_COMMAND_HANDLER(COMMAND_IS_CAR_STOPPED, IsCarStopped);
    // REGISTER_COMMAND_HANDLER(COMMAND_MARK_CAR_AS_NO_LONGER_NEEDED, MarkCarAsNoLongerNeeded);
    // REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_DENSITY_MULTIPLIER, SetCarDensityMultiplier);
    // REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_HEAVY, SetCarHeavy);
    // REGISTER_COMMAND_HANDLER(COMMAND_IS_CAR_UPSIDEDOWN, IsCarUpsidedown);
    // REGISTER_COMMAND_HANDLER(COMMAND_LOCK_CAR_DOORS, LockCarDoors);
    // REGISTER_COMMAND_HANDLER(COMMAND_EXPLODE_CAR, ExplodeCar);
    // REGISTER_COMMAND_HANDLER(COMMAND_IS_CAR_UPRIGHT, IsCarUpright);
    // REGISTER_COMMAND_HANDLER(COMMAND_SET_TARGET_CAR_FOR_MISSION_GARAGE, SetTargetCarForMissionGarage);
    // REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_HEALTH, SetCarHealth);
    // REGISTER_COMMAND_HANDLER(COMMAND_GET_CAR_HEALTH, GetCarHealth);
    // REGISTER_COMMAND_HANDLER(COMMAND_CHANGE_CAR_COLOUR, ChangeCarColour);
    // REGISTER_COMMAND_HANDLER(COMMAND_SET_CAN_RESPRAY_CAR, SetCanResprayCar);
    // REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_ONLY_DAMAGED_BY_PLAYER, SetCarOnlyDamagedByPlayer);
    // REGISTER_COMMAND_HANDLER(COMMAND_IS_CAR_IN_WATER, IsCarInWater);
    // REGISTER_COMMAND_HANDLER(COMMAND_GET_CLOSEST_CAR_NODE, GetClosestCarNode);
    // REGISTER_COMMAND_HANDLER(COMMAND_CAR_GOTO_COORDINATES_ACCURATE, CarGotoCoordinatesAccurate);
    // REGISTER_COMMAND_HANDLER(COMMAND_IS_CAR_ON_SCREEN, IsCarOnScreen);
    // REGISTER_COMMAND_HANDLER(COMMAND_GET_CAR_FORWARD_X, GetCarForwardX);
    // REGISTER_COMMAND_HANDLER(COMMAND_GET_CAR_FORWARD_Y, getCarForwardY);
    // REGISTER_COMMAND_HANDLER(COMMAND_HAS_CAR_BEEN_DAMAGED_BY_WEAPON, hasCarBeenDamagedByWeapon);
    // REGISTER_COMMAND_HANDLER(COMMAND_START_CAR_FIRE, StartCarFire);
    // REGISTER_COMMAND_HANDLER(COMMAND_GET_RANDOM_CAR_OF_TYPE_IN_AREA, GetRandomCarOfTypeInArea);
    // REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_VISIBLE, SetCarVisible);
    // REGISTER_COMMAND_HANDLER(COMMAND_PLACE_OBJECT_RELATIVE_TO_CAR, PlaceObjectRelativeToCar);
    // REGISTER_COMMAND_HANDLER(COMMAND_SWITCH_CAR_SIREN, SwitchCarSiren);
    // REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_WATERTIGHT, SetCarWatertight);
    // REGISTER_COMMAND_HANDLER(COMMAND_TURN_CAR_TO_FACE_COORD, TurnCarToFaceCoord);
    // REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_STATUS, SetCarStatus);
    // REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_STRONG, SetCarStrong);
    // REGISTER_COMMAND_HANDLER(COMMAND_CLEAR_AREA_OF_CARS, ClearAreaOfCars);
    // REGISTER_COMMAND_HANDLER(COMMAND_CREATE_RANDOM_CAR_FOR_CAR_PARK, CreateRandomCarForCarPark);
    // REGISTER_COMMAND_HANDLER(COMMAND_IS_CAR_VISIBLY_DAMAGED, IsCarVisiblyDamaged);
    // REGISTER_COMMAND_HANDLER(COMMAND_GET_CLOSEST_CAR_NODE_WITH_HEADING, GetClosestCarNodeWithHeading);
    // REGISTER_COMMAND_HANDLER(COMMAND_SET_UPSIDEDOWN_CAR_NOT_DAMAGED, SetUpsidedownCarNotDamaged);
    // REGISTER_COMMAND_HANDLER(COMMAND_GET_CAR_COLOURS, GetCarColours);
    // REGISTER_COMMAND_HANDLER(COMMAND_SET_ALL_CARS_CAN_BE_DAMAGED, SetAllCarsCanBeDamaged);
    // REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_CAN_BE_DAMAGED, SetCarCanBeDamaged);
    // REGISTER_COMMAND_HANDLER(COMMAND_GET_OFFSET_FROM_CAR_IN_WORLD_COORDS, GetOffsetFromCarInWorldCoords);
    // REGISTER_COMMAND_HANDLER(COMMAND_SET_FREE_HEALTH_CARE, SetFreeHealthCare);
    // REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_TRACTION, SetCarTraction);
    // REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_AVOID_LEVEL_TRANSITIONS, SetCarAvoidLevelTransitions);
    // REGISTER_COMMAND_HANDLER(COMMAND_IS_CAR_PASSENGER_SEAT_FREE, IsCarPassengerSeatFree);
    // REGISTER_COMMAND_HANDLER(COMMAND_GET_CAR_MODEL, GetCarModel);
    // REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_STAY_IN_FAST_LANE, SetCarStayInFastLane);
    // REGISTER_COMMAND_HANDLER(COMMAND_CLEAR_CAR_LAST_WEAPON_DAMAGE, ClearCarLastWeaponDamage);
    // REGISTER_COMMAND_HANDLER(COMMAND_GET_DRIVER_OF_CAR, GetDriverOfCar);
    // REGISTER_COMMAND_HANDLER(COMMAND_LOCATE_CHAR_IN_CAR_OBJECT_2D, LocateCharInCarObject2D);
    // REGISTER_COMMAND_HANDLER(COMMAND_LOCATE_CHAR_IN_CAR_OBJECT_3D, LocateCharInCarObject3D);
    // REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_TEMP_ACTION, SetCarTempAction);
    // REGISTER_COMMAND_HANDLER(COMMAND_GET_REMOTE_CONTROLLED_CAR, GetRemoteControlledCar);
    // REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_RANDOM_ROUTE_SEED, SetCarRandomRouteSeed);
    // REGISTER_COMMAND_HANDLER(COMMAND_IS_CAR_ON_FIRE, IsCarOnFire);
    // REGISTER_COMMAND_HANDLER(COMMAND_IS_CAR_TYRE_BURST, IsCarTyreBurst);
    // REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_FORWARD_SPEED, SetCarForwardSpeed);
    // REGISTER_COMMAND_HANDLER(COMMAND_MARK_CAR_AS_CONVOY_CAR, MarkCarAsConvoyCar);
    // REGISTER_COMMAND_HANDLER(COMMAND_GET_NTH_CLOSEST_CAR_NODE, GetNthClosestCarNode);
    // REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_STRAIGHT_LINE_DISTANCE, SetCarStraightLineDistance);
    // REGISTER_COMMAND_HANDLER(COMMAND_POP_CAR_BOOT, PopCarBoot);
    // REGISTER_COMMAND_HANDLER(COMMAND_IS_CAR_WAITING_FOR_WORLD_COLLISION, IsCarWaitingForWorldCollision);
    // REGISTER_COMMAND_HANDLER(COMMAND_BURST_CAR_TYRE, BurstCarTyre);
    // REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_MODEL_COMPONENTS, SetCarModelComponents);
    // REGISTER_COMMAND_HANDLER(COMMAND_CLOSE_ALL_CAR_DOORS, CloseAllCarDoors);
    // REGISTER_COMMAND_HANDLER(COMMAND_SORT_OUT_OBJECT_COLLISION_WITH_CAR, SortOutObjectCollisionWithCar);
    // REGISTER_COMMAND_HANDLER(COMMAND_FREEZE_CAR_POSITION, FreezeCarPosition);
    // REGISTER_COMMAND_HANDLER(COMMAND_HAS_CAR_BEEN_DAMAGED_BY_CHAR, HasCarBeenDamagedByChar);
    // REGISTER_COMMAND_HANDLER(COMMAND_HAS_CAR_BEEN_DAMAGED_BY_CAR, HasCarBeenDamagedByCar);
    // REGISTER_COMMAND_HANDLER(COMMAND_GET_RANDOM_CAR_OF_TYPE_IN_AREA_NO_SAVE, GetRandomCarOfTypeInAreaNoSave);
    // REGISTER_COMMAND_HANDLER(COMMAND_SET_CAN_BURST_CAR_TYRES, SetCanBurstCarTyres);
    // REGISTER_COMMAND_HANDLER(COMMAND_CLEAR_CAR_LAST_DAMAGE_ENTITY, ClearCarLastDamageEntity);
    // REGISTER_COMMAND_HANDLER(COMMAND_FREEZE_CAR_POSITION_AND_DONT_LOAD_COLLISION, FreezeCarPositionAndDontLoadCollision);
    // REGISTER_COMMAND_HANDLER(COMMAND_SET_LOAD_COLLISION_FOR_CAR_FLAG, SetLoadCollisionForCarFlag);
    // REGISTER_COMMAND_HANDLER(COMMAND_SHUFFLE_CARD_DECKS, ShuffleCardDecks);
    // REGISTER_COMMAND_HANDLER(COMMAND_FETCH_NEXT_CARD, FetchNextCard);
    // REGISTER_COMMAND_HANDLER(COMMAND_TASK_ENTER_CAR_AS_PASSENGER, TaskEnterCarAsPassenger);
    // REGISTER_COMMAND_HANDLER(COMMAND_TASK_ENTER_CAR_AS_DRIVER, TaskEnterCarAsDriver);
    // REGISTER_COMMAND_HANDLER(COMMAND_TASK_LEAVE_CAR, TaskLeaveCar);
    // REGISTER_COMMAND_HANDLER(COMMAND_TASK_LEAVE_CAR_AND_FLEE, TaskLeaveCarAndFlee);
    // REGISTER_COMMAND_HANDLER(COMMAND_TASK_CAR_DRIVE_TO_COORD, TaskCarDriveToCoord);
    // REGISTER_COMMAND_HANDLER(COMMAND_TASK_CAR_DRIVE_WANDER, TaskCarDriveWander);
    // REGISTER_COMMAND_HANDLER(COMMAND_START_PLAYBACK_RECORDED_CAR, StartPlaybackRecordedCar);
    // REGISTER_COMMAND_HANDLER(COMMAND_STOP_PLAYBACK_RECORDED_CAR, StopPlaybackRecordedCar);
    // REGISTER_COMMAND_HANDLER(COMMAND_PAUSE_PLAYBACK_RECORDED_CAR, PausePlaybackRecordedCar);
    // REGISTER_COMMAND_HANDLER(COMMAND_UNPAUSE_PLAYBACK_RECORDED_CAR, UnpausePlaybackRecordedCar);
    // REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_ESCORT_CAR_LEFT, SetCarEscortCarLeft);
    // REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_ESCORT_CAR_RIGHT, SetCarEscortCarRight);
    // REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_ESCORT_CAR_REAR, SetCarEscortCarRear);
    // REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_ESCORT_CAR_FRONT, SetCarEscortCarFront);
    // REGISTER_COMMAND_HANDLER(COMMAND_IS_CHAR_STOPPED_IN_ANGLED_AREA_IN_CAR_2D, IsCharStoppedInAngledAreaInCar2D);
    // REGISTER_COMMAND_HANDLER(COMMAND_IS_CHAR_STOPPED_IN_ANGLED_AREA_IN_CAR_3D, IsCharStoppedInAngledAreaInCar3D);
    // REGISTER_COMMAND_HANDLER(COMMAND_IS_PLAYBACK_GOING_ON_FOR_CAR, IsPlaybackGoingOnForCar);
    // REGISTER_COMMAND_HANDLER(COMMAND_TASK_LEAVE_CAR_IMMEDIATELY, TaskLeaveCarImmediately);
    // REGISTER_COMMAND_HANDLER(COMMAND_TASK_LEAVE_ANY_CAR, TaskLeaveAnyCar);
    // REGISTER_COMMAND_HANDLER(COMMAND_OPEN_CAR_DOOR, OpenCarDoor);
    // REGISTER_COMMAND_HANDLER(COMMAND_CREATE_FX_SYSTEM_ON_CAR, CreateFxSystemOnCar);
    // REGISTER_COMMAND_HANDLER(COMMAND_CREATE_FX_SYSTEM_ON_CAR_WITH_DIRECTION, CreateFxSystemOnCarWithDirection);
    // REGISTER_COMMAND_HANDLER(COMMAND_TASK_DESTROY_CAR, TaskDestroyCar);
    // REGISTER_COMMAND_HANDLER(COMMAND_CUSTOM_PLATE_FOR_NEXT_CAR, CustomPlateForNextCar);
    // REGISTER_COMMAND_HANDLER(COMMAND_TASK_SHUFFLE_TO_NEXT_CAR_SEAT, TaskShuffleToNextCarSeat);
    // REGISTER_COMMAND_HANDLER(COMMAND_FORCE_CAR_LIGHTS, ForceCarLights);
    // REGISTER_COMMAND_HANDLER(COMMAND_ATTACH_OBJECT_TO_CAR, AttachObjectToCar);
    // REGISTER_COMMAND_HANDLER(COMMAND_ATTACH_CAR_TO_CAR, AttachCarToCar);
    // REGISTER_COMMAND_HANDLER(COMMAND_DETACH_CAR, DetachCar);
    // REGISTER_COMMAND_HANDLER(COMMAND_POP_CAR_DOOR, PopCarDoor);
    // REGISTER_COMMAND_HANDLER(COMMAND_FIX_CAR_DOOR, FixCarDoor);
    // REGISTER_COMMAND_HANDLER(COMMAND_TASK_EVERYONE_LEAVE_CAR, TaskEveryoneLeaveCar);
    // REGISTER_COMMAND_HANDLER(COMMAND_POP_CAR_PANEL, PopCarPanel);
    // REGISTER_COMMAND_HANDLER(COMMAND_FIX_CAR_PANEL, FixCarPanel);
    // REGISTER_COMMAND_HANDLER(COMMAND_FIX_CAR_TYRE, FixCarTyre);
    // REGISTER_COMMAND_HANDLER(COMMAND_GET_CAR_SPEED_VECTOR, GetCarSpeedVector);
    // REGISTER_COMMAND_HANDLER(COMMAND_GET_CAR_MASS, GetCarMass);
    // REGISTER_COMMAND_HANDLER(COMMAND_GET_CAR_ROLL, GetCarRoll);
    // REGISTER_COMMAND_HANDLER(COMMAND_SKIP_TO_END_AND_STOP_PLAYBACK_RECORDED_CAR, SkipToEndAndStopPlaybackRecordedCar);
    // REGISTER_COMMAND_HANDLER(COMMAND_TASK_CAR_TEMP_ACTION, TaskCarTempAction);
    // REGISTER_COMMAND_HANDLER(COMMAND_TASK_CAR_MISSION, TaskCarMission);
    // REGISTER_COMMAND_HANDLER(COMMAND_GET_NTH_CLOSEST_CAR_NODE_WITH_HEADING, GetNthClosestCarNodeWithHeading);
    // REGISTER_COMMAND_HANDLER(COMMAND_DOES_CAR_HAVE_STUCK_CAR_CHECK, DoesCarHaveStuckCarCheck);
    // REGISTER_COMMAND_HANDLER(COMMAND_GET_CAR_VALUE, GetCarValue);
    // REGISTER_COMMAND_HANDLER(COMMAND_CAR_GOTO_COORDINATES_RACING, CarGotoCoordinatesRacing);
    // REGISTER_COMMAND_HANDLER(COMMAND_START_PLAYBACK_RECORDED_CAR_USING_AI, StartPlaybackRecordedCarUsingAi);
    // REGISTER_COMMAND_HANDLER(COMMAND_SKIP_IN_PLAYBACK_RECORDED_CAR, SkipInPlaybackRecordedCar);
    // REGISTER_COMMAND_HANDLER(COMMAND_EXPLODE_CAR_IN_CUTSCENE, ExplodeCarInCutscene);
    // REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_STAY_IN_SLOW_LANE, SetCarStayInSlowLane);
    // REGISTER_COMMAND_HANDLER(COMMAND_TAKE_REMOTE_CONTROL_OF_CAR, TakeRemoteControlOfCar);
    // REGISTER_COMMAND_HANDLER(COMMAND_TASK_WARP_CHAR_INTO_CAR_AS_DRIVER, TaskWarpCharIntoCarAsDriver);
    // REGISTER_COMMAND_HANDLER(COMMAND_TASK_WARP_CHAR_INTO_CAR_AS_PASSENGER, TaskWarpCharIntoCarAsPassenger);
    // REGISTER_COMMAND_HANDLER(COMMAND_DAMAGE_CAR_PANEL, DamageCarPanel);
    // REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_ROLL, SetCarRoll);
    // REGISTER_COMMAND_HANDLER(COMMAND_SUPPRESS_CAR_MODEL, SuppressCarModel);
    // REGISTER_COMMAND_HANDLER(COMMAND_DONT_SUPPRESS_CAR_MODEL, DontSuppressCarModel);
    // REGISTER_COMMAND_HANDLER(COMMAND_DONT_SUPPRESS_ANY_CAR_MODELS, DontSuppressAnyCarModels);
    // REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_CAN_GO_AGAINST_TRAFFIC, SetCarCanGoAgainstTraffic);
    // REGISTER_COMMAND_HANDLER(COMMAND_DAMAGE_CAR_DOOR, DamageCarDoor);
    // REGISTER_COMMAND_HANDLER(COMMAND_GET_RANDOM_CAR_IN_SPHERE_NO_SAVE, GetRandomCarInSphereNoSave);
    // REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_AS_MISSION_CAR, SetCarAsMissionCar);
    // REGISTER_COMMAND_HANDLER(COMMAND_CUSTOM_PLATE_DESIGN_FOR_NEXT_CAR, CustomPlateDesignForNextCar);
    // REGISTER_COMMAND_HANDLER(COMMAND_TASK_GOTO_CAR, TaskGotoCar);
    // REGISTER_COMMAND_HANDLER(COMMAND_GET_CAR_PITCH, GetCarPitch);
    // REGISTER_COMMAND_HANDLER(COMMAND_GET_TRAIN_CARRIAGE, GetTrainCarriage);
    // REGISTER_COMMAND_HANDLER(COMMAND_REQUEST_CAR_RECORDING, RequestCarRecording);
    // REGISTER_COMMAND_HANDLER(COMMAND_HAS_CAR_RECORDING_BEEN_LOADED, HasCarRecordingBeenLoaded);
    // REGISTER_COMMAND_HANDLER(COMMAND_APPLY_FORCE_TO_CAR, ApplyForceToCar);
    // REGISTER_COMMAND_HANDLER(COMMAND_ADD_TO_CAR_ROTATION_VELOCITY, AddToCarRotationVelocity);
    // REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_ROTATION_VELOCITY, SetCarRotationVelocity);
    // REGISTER_COMMAND_HANDLER(COMMAND_CONTROL_CAR_HYDRAULICS, ControlCarHydraulics);
    // REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_FOLLOW_CAR, SetCarFollowCar);
    // REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_HYDRAULICS, SetCarHydraulics);
    // REGISTER_COMMAND_HANDLER(COMMAND_DOES_CAR_HAVE_HYDRAULICS, DoesCarHaveHydraulics);
    // REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_ENGINE_BROKEN, SetCarEngineBroken);
    // REGISTER_COMMAND_HANDLER(COMMAND_GET_CAR_UPRIGHT_VALUE, GetCarUprightValue);
    // REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_CAN_BE_VISIBLY_DAMAGED, SetCarCanBeVisiblyDamaged);
    // REGISTER_COMMAND_HANDLER(COMMAND_CREATE_EMERGENCY_SERVICES_CAR, CreateEmergencyServicesCar);
    // REGISTER_COMMAND_HANDLER(COMMAND_START_PLAYBACK_RECORDED_CAR_LOOPED, StartPlaybackRecordedCarLooped);
    // REGISTER_COMMAND_HANDLER(COMMAND_REMOVE_CAR_RECORDING, RemoveCarRecording);
    // REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_COORDINATES_NO_OFFSET, SetCarCoordinatesNoOffset);
    // REGISTER_COMMAND_HANDLER(COMMAND_OPEN_CAR_DOOR_A_BIT, OpenCarDoorAbit);
    // REGISTER_COMMAND_HANDLER(COMMAND_IS_CAR_DOOR_FULLY_OPEN, IsCarDoorFullyOpen);
    // REGISTER_COMMAND_HANDLER(COMMAND_EXPLODE_CAR_IN_CUTSCENE_SHAKE_AND_BITS, ExplodeCarInCutsceneShakeAndBits);
    // REGISTER_COMMAND_HANDLER(COMMAND_SET_PLANE_UNDERCARRIAGE_UP, SetPlaneUndercarriageUp);
    // REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_ENGINE_ON, SetCarEngineOn);
    // REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_LIGHTS_ON, SetCarLightsOn);
    // REGISTER_COMMAND_HANDLER(COMMAND_GET_PLANE_UNDERCARRIAGE_POSITION, GetPlaneUndercarriagePosition);
    // REGISTER_COMMAND_HANDLER(COMMAND_ATTACH_CAR_TO_OBJECT, AttachCarToObject);
    // REGISTER_COMMAND_HANDLER(COMMAND_IS_CHAR_STUCK_UNDER_CAR, IsCharStuckUnderCar);
    // REGISTER_COMMAND_HANDLER(COMMAND_CONTROL_CAR_DOOR, ControlCarDoor);
    // REGISTER_COMMAND_HANDLER(COMMAND_STORE_CAR_MOD_STATE, StoreCarModState);
    // REGISTER_COMMAND_HANDLER(COMMAND_RESTORE_CAR_MOD_STATE, RestoreCarModState);
    // REGISTER_COMMAND_HANDLER(COMMAND_GET_CURRENT_CAR_MOD, GetCurrentCarMod);
    // REGISTER_COMMAND_HANDLER(COMMAND_IS_CAR_LOW_RIDER, IsCarLowRider);
    // REGISTER_COMMAND_HANDLER(COMMAND_IS_CAR_STREET_RACER, IsCarStreetRacer);
    // REGISTER_COMMAND_HANDLER(COMMAND_GET_NUM_CAR_COLOURS, GetNumCarColours);
    // REGISTER_COMMAND_HANDLER(COMMAND_SET_CHAR_FORCE_DIE_IN_CAR, SetCharForceDieInCar);
    // REGISTER_COMMAND_HANDLER(COMMAND_GET_CAR_BLOCKING_CAR, GetCarBlockingCar);
    // REGISTER_COMMAND_HANDLER(COMMAND_GET_CAR_MOVING_COMPONENT_OFFSET, GetCarMovingComponentOffset);
    // REGISTER_COMMAND_HANDLER(COMMAND_SET_CAR_COLLISION, SetCarCollision);
    // REGISTER_COMMAND_HANDLER(COMMAND_GET_RANDOM_CAR_MODEL_IN_MEMORY, GetRandomCarModelInMemory);
    // REGISTER_COMMAND_HANDLER(COMMAND_GET_CAR_DOOR_LOCK_STATUS, GetCarDoorLockStatus);
    // REGISTER_COMMAND_HANDLER(COMMAND_DISPLAY_CAR_NAMES, DisplayCarNames);
    // REGISTER_COMMAND_HANDLER(COMMAND_IS_CAR_DOOR_DAMAGED, IsCarDoorDamaged);
    // REGISTER_COMMAND_HANDLER(COMMAND_SET_FORCE_RANDOM_CAR_MODEL, SetForceRandomCarModel);
    // REGISTER_COMMAND_HANDLER(COMMAND_GET_RANDOM_CAR_OF_TYPE_IN_ANGLED_AREA_NO_SAVE, GetRandomCarOfTypeInAngledAreaNoSave);
    // REGISTER_COMMAND_HANDLER(COMMAND_IS_CAR_TOUCHING_CAR, IsCarTouchingCar);
    // REGISTER_COMMAND_HANDLER(COMMAND_GET_CAR_MODEL_VALUE, GetCarModelValue);
    // REGISTER_COMMAND_HANDLER(COMMAND_CREATE_CAR_GENERATOR_WITH_PLATE, CreateCarGeneratorWithPlate);
    // REGISTER_COMMAND_HANDLER(COMMAND_SET_AIRCRAFT_CARRIER_SAM_SITE, SetAircraftCarrierSamSite);
    // REGISTER_COMMAND_HANDLER(COMMAND_GIVE_NON_PLAYER_CAR_NITRO, GiveNonPlayerCarNitro);
    // REGISTER_COMMAND_HANDLER(COMMAND_IS_THIS_MODEL_A_CAR, IsThisModelAcar);
    // REGISTER_COMMAND_HANDLER(COMMAND_SET_EXTRA_CAR_COLOURS, SetExtraCarColours);
    // REGISTER_COMMAND_HANDLER(COMMAND_HAS_CAR_BEEN_RESPRAYED, HasCarBeenResprayed);
    // REGISTER_COMMAND_HANDLER(COMMAND_IMPROVE_CAR_BY_CHEATING, ImproveCarByCheating);
    // REGISTER_COMMAND_HANDLER(COMMAND_CHANGE_CAR_COLOUR_FROM_MENU, ChangeCarColourFromMenu);
    // REGISTER_COMMAND_HANDLER(COMMAND_FIX_CAR, FixCar);
    
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
