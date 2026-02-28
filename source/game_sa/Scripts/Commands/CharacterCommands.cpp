#include <StdInc.h>

#include "Commands.hpp"
#include <CommandParser/Parser.hpp>
#include <extensions/Shapes/AngledRect.hpp>
#include "Utility.h"
#include <reversiblebugfixes/Bugs.hpp>

#include <TaskTypes/TaskComplexDie.h>
#include <TaskTypes/TaskSimpleCarSetPedInAsDriver.h>
#include <TaskTypes/TaskSimpleCarSetPedInAsPassenger.h>
#include <TaskTypes/TaskSimpleCarDrive.h>
#include <TaskTypes/TaskComplexKillPedOnFoot.h>
#include <TaskTypes/TaskSimpleStandStill.h>
#include <TaskTypes/TaskComplexWanderCriminal.h>
#include <TaskTypes/TaskComplexWanderStandard.h>
#include <TaskTypes/TaskComplexTurnToFaceEntityOrCoord.h>
#include <TaskTypes/TaskComplexKillPedOnFoot.h>
#include <TaskTypes/TaskComplexEnterCarAsDriver.h>
#include <TaskTypes/TaskComplexEnterCarAsPassenger.h>
#include <TaskTypes/TaskSimpleCarSetPedOut.h>

#include <Attractors/PedAttractorPedPlacer.h>

#include "DecisionMakers/DecisionMakerTypesFileLoader.h"

#include <FireManager.h>
#include <World.h>
#include <EntryExitManager.h>
#include <TimeCycle.h>
#include <eBoneTag.h>
#include <SearchLight.h>

using namespace notsa::script;
/*!
* Various character (ped) commands
*/

template<typename T>
void HandleEntityMissionCleanup(CRunningScript& S, T& entity) {
    if (S.m_UsesMissionCleanup) {
        CTheScripts::MissionCleanUp.AddEntityToList(entity);
    }
}

//
// Flags getters/setters
//

namespace {
/*
 * @opcode 02AB
 * @command SET_CHAR_PROOFS
 * @class Char
 * @method SetProofs
 * 
 * @brief Sets the character's immunities
 * 
 * @param {Char} self
 * @param {bool} bulletProof
 * @param {bool} fireProof
 * @param {bool} explosionProof
 * @param {bool} collisionProof
 * @param {bool} meleeProof
 */
void SetCharProofs(CPed& ped, bool bullet, bool fire, bool explosion, bool collision, bool melee) {
    auto& flags = ped.physicalFlags;
    flags.bBulletProof = bullet;
    flags.bFireProof = fire;
    flags.bExplosionProof = explosion;
    flags.bCollisionProof = collision;
    flags.bMeleeProof = melee;
}

/*
 * @opcode 087E
 * @command SET_CHAR_DROPS_WEAPONS_WHEN_DEAD
 * @class Char
 * @method SetDropsWeaponsWhenDead
 * 
 * @brief Sets whether the character will drop any of their weapons when they die
 * 
 * @param {Char} self
 * @param {bool} state
 */
auto SetCharDropsWeaponsWhenDead(CPed& ped, bool dropsWepsWhenDead) {
    ped.bDoesntDropWeaponsWhenDead = !dropsWepsWhenDead;
}

/*
 * @opcode 087F
 * @command SET_CHAR_NEVER_LEAVES_GROUP
 * @class Char
 * @method SetNeverLeavesGroup
 * 
 * @brief Prevents the character from leaving their group
 * 
 * @param {Char} self
 * @param {bool} state
 */
auto SetCharNeverLeavesGroup(CPed& ped, bool bNeverLeavesGroup) {
    ped.bNeverLeavesGroup = bNeverLeavesGroup;
}

/*
 * @opcode 0332
 * @command SET_CHAR_BLEEDING
 * @class Char
 * @method SetBleeding
 * 
 * @brief Makes a character bleed
 * 
 * @param {Char} self
 * @param {bool} state
 */
auto SetCharBleeding(CPed& ped, bool isBleeding) {
    ped.bPedIsBleeding = isBleeding;
}

/*
 * @opcode 039E
 * @command SET_CHAR_CANT_BE_DRAGGED_OUT
 * @class Char
 * @method SetCantBeDraggedOut
 * 
 * @brief Locks the character while in a car
 * 
 * @param {Char} self
 * @param {bool} state
 */
auto SetCharCantBeDraggedOut(CPed& ped, bool bDontDrag) {
    ped.bDontDragMeOutCar = bDontDrag;
}

/*
 * @opcode 0433
 * @command SET_CHAR_IS_CHRIS_CRIMINAL
 * @class Char
 * @method SetIsChrisCriminal
 * 
 * @brief Sets whether the character is a psychotic killer or not
 * 
 * @param {Char} self
 * @param {bool} state
 */
auto SetCharIsChrisCriminal(CPed& ped, bool value) {
    ped.bChrisCriminal = value;
}

/*
 * @opcode 0446
 * @command SET_CHAR_SUFFERS_CRITICAL_HITS
 * @class Char
 * @method SetSuffersCriticalHits
 * 
 * @brief Sets whether the specified character is immune to headshots
 * 
 * @param {Char} self
 * @param {bool} state
 */
auto SetCharSuffersCriticalHits(CPed& ped, bool value) {
    ped.bNoCriticalHits = !value;
}

/*
 * @opcode 04D7
 * @command FREEZE_CHAR_POSITION
 * @class Char
 * @method FreezePosition
 * 
 * @brief Sets whether the character's position remains unchanged
 * 
 * @param {Char} self
 * @param {bool} state
 */
auto FreezeCharPosition(CPed& ped, bool freeze) {
    ped.physicalFlags.bDontApplySpeed = freeze;
}

/*
 * @opcode 04D8
 * @command SET_CHAR_DROWNS_IN_WATER
 * @class Char
 * @method SetDrownsInWater
 * 
 * @brief Controls whether the character can drown in water
 * 
 * @param {Char} self
 * @param {bool} state
 */
auto SetCharDrownsInWater(CPed& ped, bool bDrownsInWater) {
    ped.bDrownsInWater = bDrownsInWater;
}

/*
 * @opcode 0526
 * @command SET_CHAR_STAY_IN_CAR_WHEN_JACKED
 * @class Char
 * @method SetStayInCarWhenJacked
 * 
 * @brief Makes the character stay in the vehicle when it is jacked (characters let themselves get "kidnapped")
 * 
 * @param {Char} self
 * @param {bool} state
 */
auto SetCharStayInCarWhenJacked(CPed& ped, bool bStayInCarOnJack) {
    ped.bStayInCarOnJack = bStayInCarOnJack;
}

/*
 * @opcode 054A
 * @command SET_CHAR_CAN_BE_SHOT_IN_VEHICLE
 * @class Char
 * @method SetCanBeShotInVehicle
 * 
 * @brief Makes the character immune to a damage while in a vehicle
 * 
 * @param {Char} self
 * @param {bool} state
 */
auto SetCharCanBeShotInVehicle(CPed& ped, bool bCanBeShotInVehicle) {
    ped.bCanBeShotInVehicle = bCanBeShotInVehicle;
}

/*
 * @opcode 0568
 * @command SET_CHAR_NEVER_TARGETTED
 * @class Char
 * @method SetNeverTargeted
 * 
 * @brief Sets whether the character won't be targeted by the autoaim system
 * 
 * @param {Char} self
 * @param {bool} state
 */
auto SetCharNeverTargetted(CPed& ped, bool bNeverEverTargetThisPed) {
    ped.bNeverEverTargetThisPed = bNeverEverTargetThisPed;
}

/*
 * @opcode 0619
 * @command SET_CHAR_COLLISION
 * @class Char
 * @method SetCollision
 * 
 * @brief Sets whether collision detection is enabled for the character
 * 
 * @param {Char} self
 * @param {bool} state
 */
auto SetCharCollision(CPed& ped, bool bUsesCollision) {
    ped.SetUsesCollision(bUsesCollision);
}

/*
 * @opcode 02E0
 * @command IS_CHAR_SHOOTING
 * @class Char
 * @method IsShooting
 * 
 * @brief Returns true if the character is firing a weapon
 * 
 * @param {Char} self
 */
auto IsCharShooting(CPed& ped) {
    return ped.bFiringWeapon;
}

/*
 * @opcode 04F0
 * @command IS_CHAR_WAITING_FOR_WORLD_COLLISION
 * @class Char
 * @method IsWaitingForWorldCollision
 * 
 * @brief 
 * 
 * @param {Char} self
 */
auto IsCharWaitingForWorldCollision(CPed& ped) {
    return ped.m_bIsStaticWaitingForCollision;
}

/*
 * @opcode 0597
 * @command IS_CHAR_DUCKING
 * @class Char
 * @method IsDucking
 * 
 * @brief Returns true if the specified character is crouching
 * 
 * @param {Char} self
 */
auto IsCharDucking(CPed& ped) {
    return ped.bIsDucking;
}

/*
 * @opcode 083D
 * @command GET_CHAR_VELOCITY
 * @class Char
 * @method GetVelocity
 * 
 * @brief Gets the characters velocity
 * 
 * @param {Char} self
 * 
 * @returns {Vector} 
 */
auto GetCharVelocity(CPed& ped) {
    return ped.GetMoveSpeed();
}

/*
 * @opcode 083C
 * @command SET_CHAR_VELOCITY
 * @class Char
 * @method SetVelocity
 * 
 * @brief Sets the characters velocity
 * 
 * @param {Char} self
 * @param {Vector} 
 */
auto SetCharVelocity(CPed& ped, CVector velocity) {
    ped.SetVelocity(velocity / 50.f);
}

/*
 * @opcode 083E
 * @command SET_CHAR_ROTATION
 * @class Char
 * @method SetRotation
 * 
 * @brief Sets the characters rotation
 * 
 * @param {Char} self
 * @param {Vector} 
 */
auto SetCharRotation(CPed& ped, CVector angles) {
    ped.SetOrientation(angles * DegreesToRadians(1.0f)); // make euler angles from degrees
    CWorld::Add(&ped);
}

/*
 * @opcode 0856
 * @command SET_CHAR_ALLOWED_TO_DUCK
 * @class Char
 * @method SetAllowedToDuck
 * 
 * @brief Sets whether the character can crouch
 * 
 * @param {Char} self
 * @param {bool} state
 */
auto SetCharAllowedToDuck(CPed& ped, bool bCanDuck) {
    ped.bNotAllowedToDuck = !bCanDuck;
}

/*
 * @opcode 0860
 * @command SET_CHAR_AREA_VISIBLE
 * @class Char
 * @method SetAreaVisible
 * 
 * @brief Sets the interior that the char is in
 * 
 * @param {Char} self
 * @param {int} areaId
 */
auto SetCharAreaVisible(CPed& ped, eAreaCodes area) {
    ped.SetAreaCode(area);
    if (area != AREA_CODE_NORMAL_WORLD) {
        return;
    }
    ped.m_pEnex = nullptr;
    if (!ped.IsPlayer()) {
        return;
    }
    CEntryExitManager::ms_entryExitStackPosn = 0;
    CTimeCycle::StopExtraColour(0);
}

/*
 * @opcode 0883
 * @command ATTACH_FX_SYSTEM_TO_CHAR_BONE
 * @class Particle
 * @method AttachToCharBone
 * 
 * @brief Attaches the specified particle to the specified character
 * 
 * @param {Particle} self
 * @param {Char} handle
 * @param {PedBone} pedBone
 */
auto AttachFxSystemToCharBone(tScriptEffectSystem& fx, CPed& ped, eBoneTag bone) {
    fx.m_pFxSystem->AttachToBone(&ped, bone);
}

/*
 * @opcode 0889
 * @command GET_DEAD_CHAR_COORDINATES
 * @class Char
 * @method GetCoordinatesOfDied
 * 
 * @brief 
 * 
 * @param {Char} self
 * 
 * @returns {Vector} 
 */
auto GetDeadCharCoordinates(CPed& ped) {
    return ped.IsInVehicle()
        ? ped.m_pVehicle->GetPosition()
        : ped.GetBonePosition(BONE_PELVIS);
}

/*
 * @opcode 00A0
 * @command GET_CHAR_COORDINATES
 * @class Char
 * @method GetCoordinates
 * 
 * @brief Returns the character's coordinates
 * 
 * @param {Char} self
 * 
 * @returns {Vector} 
 */
auto GetCharCoordinates(CPed& ped) {
    return ped.IsInVehicle()
        ? ped.m_pVehicle->GetPosition()
        : ped.GetPosition();
}

/*
 * @opcode 00A1
 * @command SET_CHAR_COORDINATES
 * @class Char
 * @method SetCoordinates
 * 
 * @brief Puts the character at the specified location
 * 
 * @param {Char} self
 * @param {Vector} 
 */
auto SetCharCoordinates(CRunningScript& S, CPed& ped, CVector coords) {
    S.SetCharCoordinates(ped, coords, true, true);
}

/*
 * @opcode 00A3
 * @command IS_CHAR_IN_AREA_2D
 * @class Char
 * @method IsInArea2D
 * 
 * @brief Returns true if the character is within the specified 2D area
 * 
 * @param {Char} self
 * @param {Vector2D} leftBottom
 * @param {Vector2D} rightTop
 * @param {bool} drawSphere
 */
auto IsCharInArea2D(CRunningScript& S, CPed& ped, CVector2D a, CVector2D b, bool highlightArea) {
    if (highlightArea) {
        S.HighlightImportantArea(a, b);
    }

    const auto Check = [&](const auto& e) { return e.IsWithinArea(a.x, a.y, b.x, b.y); };
    return ped.IsInVehicle()
        ? Check(*ped.m_pVehicle)
        : Check(ped);
}

/*
 * @opcode 00A4
 * @command IS_CHAR_IN_AREA_3D
 * @class Char
 * @method IsInArea3D
 * 
 * @brief Returns true if the character is within the specified 3D area
 * 
 * @param {Char} self
 * @param {Vector} leftBottom
 * @param {Vector} rightTop
 * @param {bool} drawSphere
 */
auto IsCharInArea3D(CRunningScript& S, CPed& ped, CVector a, CVector b, bool highlightArea) {
    if (highlightArea) {
        S.HighlightImportantArea(a, b);
    }

    const auto Check = [&](const auto& e) { return e.IsWithinArea(a.x, a.y, a.z, b.x, b.y, b.z); };
    return ped.IsInVehicle()
        ? Check(*ped.m_pVehicle)
        : Check(ped);
}

/*
 * @opcode 00D9
 * @command STORE_CAR_CHAR_IS_IN
 * @class Char
 * @method StoreCarIsIn
 * 
 * @brief Returns the current vehicle of the character and adds it to the mission cleanup list (alts:03C0,0811,0484)
 * 
 * @param {Char} self
 * 
 * @returns {Car} handle
 */
auto StoreCarCharIsIn(CRunningScript& S, CPed& ped) { // 0x469481
    const auto veh = ped.GetVehicleIfInOne();

    if (notsa::bugfixes::GenericCrashing && !veh) {
        NOTSA_LOG_WARN("Tried to store vehicle of a ped that isn't in a vehicle!");
        return -1; // TODO: Raise exception here for the error
    }

    if (GetVehiclePool()->GetRef(veh) != CTheScripts::StoreVehicleIndex && S.m_UsesMissionCleanup) {
        // Unstore previous (If it still exists)
        if (CTheScripts::StoreVehicleIndex != -1) { // NOTSA: Bugfix
            if (const auto stored = GetVehiclePool()->GetAt(CTheScripts::StoreVehicleIndex)) {
                CCarCtrl::RemoveFromInterestingVehicleList(stored);
                if (stored->IsMissionVehicle() && CTheScripts::StoreVehicleWasRandom) {
                    stored->SetVehicleCreatedBy(RANDOM_VEHICLE);
                    stored->vehicleFlags.bIsLocked = false;
                    CTheScripts::MissionCleanUp.RemoveEntityFromList(CTheScripts::StoreVehicleIndex, MISSION_CLEANUP_ENTITY_TYPE_VEHICLE);
                }
            }
        }

        // Now store this vehicle
        CTheScripts::StoreVehicleIndex = GetVehiclePool()->GetRef(veh);
        switch (veh->GetCreatedBy()) {
        case RANDOM_VEHICLE:
        case PARKED_VEHICLE: {
            veh->SetVehicleCreatedBy(MISSION_VEHICLE);
            CTheScripts::StoreVehicleWasRandom = true;
            CTheScripts::MissionCleanUp.AddEntityToList(CTheScripts::StoreVehicleIndex, MISSION_CLEANUP_ENTITY_TYPE_VEHICLE);
            break;
        }
        case MISSION_VEHICLE:
        case PERMANENT_VEHICLE: {
            CTheScripts::StoreVehicleWasRandom = false;
            break;
        }
        }
    }

    return CTheScripts::StoreVehicleIndex;
}

/*
 * @opcode 00DB
 * @command IS_CHAR_IN_CAR
 * @class Char
 * @method IsInCar
 * 
 * @brief Returns true if the character is in the specified vehicle
 * 
 * @param {Char} self
 * @param {Car} vehicle
 */
auto IsCharInCar(CPed& ped, CVehicle& veh) {
    return ped.IsInVehicle() && ped.m_pVehicle == &veh;
}

/*
 * @opcode 00DD
 * @command IS_CHAR_IN_MODEL
 * @class Char
 * @method IsInModel
 * 
 * @brief Returns true if the character is driving a vehicle with the specified model
 * 
 * @param {Char} self
 * @param {model_vehicle} modelId
 */
auto IsCharInModel(CPed& ped, eModelID model) {
    return ped.IsInVehicle() && ped.m_pVehicle->m_nModelIndex == model;
}

/*
 * @opcode 00DF
 * @command IS_CHAR_IN_ANY_CAR
 * @class Char
 * @method IsInAnyCar
 * 
 * @brief Returns true if the character has a vehicle, even if they are not actually sat inside it (opening and closing the door)
 * 
 * @param {Char} self
 */
auto IsCharInAnyCar(CPed& ped) {
    return ped.IsInVehicle();
}

//
// Locate[Stopped]Character(Any Means/On Foot/In Car)
//

//! Does the usual checks
bool DoLocateCharChecks(CPed& ped, bool mustBeInCar, bool mustBeOnFoot, bool mustBeStopped) {
    if (mustBeInCar && !ped.IsInVehicle()) {
        return false;
    }
    if (mustBeOnFoot && ped.IsInVehicle()) {
        return false;
    }
    if (mustBeStopped && !CTheScripts::IsPedStopped(&ped)) {
        return false;
    }
    return true;
}

//! Check is char within 3D area (with some restrictions)
bool LocateChar3D(CRunningScript& S, CPed& ped, CVector pos, CVector radius, bool highlightArea, bool mustBeInCar, bool mustBeOnFoot, bool mustBeStopped) {
    const CBox bb{ pos - radius, pos + radius };
    if (highlightArea) { // Highlight area every time
        S.HighlightImportantArea(bb.m_vecMin, bb.m_vecMax);
    }
    if (!DoLocateCharChecks(ped, mustBeInCar, mustBeOnFoot, mustBeStopped)) {
        return false;
    }
    return bb.IsPointInside(GetCharCoordinates(ped));
}

bool LocateCharEntity3D(CRunningScript& S, CPed& ped, CEntity& entity, CVector radius, bool highlightArea, bool mustBeInCar, bool mustBeOnFoot) {
    return LocateChar3D(S, ped, entity.GetPosition(), radius, highlightArea, mustBeInCar, mustBeOnFoot, false);
}

bool LocateChar2D(CRunningScript& S, CPed& ped, CVector2D pos, CVector2D radius, bool highlightArea, bool mustBeInCar, bool mustBeOnFoot, bool mustBeStopped) {
    const CRect rect{ pos - radius, pos + radius };
    if (highlightArea) { // Highlight area every time
        S.HighlightImportantArea(rect.GetTopLeft(), rect.GetBottomRight());
    }
    if (!DoLocateCharChecks(ped, mustBeInCar, mustBeOnFoot, mustBeStopped)) {
        return false;
    }
    if (CTheScripts::DbgFlag) {
        CTheScripts::DrawDebugSquare(rect);
    }
    return rect.IsPointInside(GetCharCoordinates(ped));
}

bool LocateCharEntity2D(CRunningScript& S, CPed& ped, CEntity& entity, CVector2D radius, bool highlightArea, bool mustBeInCar, bool mustBeOnFoot) {
    return LocateChar2D(S, ped, entity.GetPosition(), radius, highlightArea, mustBeInCar, mustBeOnFoot, false);
}

//
// Char, Pos
//

// => 2D

/*
 * @opcode 00EC
 * @command LOCATE_CHAR_ANY_MEANS_2D
 * @class Char
 * @method LocateAnyMeans2D
 * 
 * @brief Returns true if the character is within the 2D radius of the coordinates point
 * 
 * @param {Char} self
 * @param {Vector2D} 
 * @param {Vector2D} radius
 * @param {bool} drawSphere
 */
auto LocateCharAnyMeans2D(CRunningScript& S, CPed& ped, CVector2D pos, CVector2D radius, bool highlightArea) {
    return LocateChar2D(S, ped, pos, radius, highlightArea, false, false, false);
}

/*
 * @opcode 00ED
 * @command LOCATE_CHAR_ON_FOOT_2D
 * @class Char
 * @method LocateOnFoot2D
 * 
 * @brief Returns true if the character is within the 2D radius of the coordinates point on foot
 * 
 * @param {Char} self
 * @param {Vector2D} 
 * @param {Vector2D} radius
 * @param {bool} drawSphere
 */
auto LocateCharOnFoot2D(CRunningScript& S, CPed& ped, CVector2D pos, CVector2D radius, bool highlightArea) {
    return LocateChar2D(S, ped, pos, radius, highlightArea, false, true, false);
}

/*
 * @opcode 00EE
 * @command LOCATE_CHAR_IN_CAR_2D
 * @class Char
 * @method LocateInCar2D
 * 
 * @brief Returns true if the character is within the 2D radius of the coordinates point in a vehicle
 * 
 * @param {Char} self
 * @param {Vector2D} 
 * @param {Vector2D} radius
 * @param {bool} drawSphere
 */
auto LocateCharInCar2D(CRunningScript& S, CPed& ped, CVector2D pos, CVector2D radius, bool highlightArea) {
    return LocateChar2D(S, ped, pos, radius, highlightArea, true, false, false);
}

/*
 * @opcode 00EF
 * @command LOCATE_STOPPED_CHAR_ANY_MEANS_2D
 * @class Char
 * @method LocateStoppedAnyMeans2D
 * 
 * @brief Returns true if the character stopped within the 2D radius of the coordinates point
 * 
 * @param {Char} self
 * @param {Vector2D} 
 * @param {Vector2D} radius
 * @param {bool} drawSphere
 */
auto LocateStoppedCharAnyMeans2D(CRunningScript& S, CPed& ped, CVector2D pos, CVector2D radius, bool highlightArea) {
    return LocateChar2D(S, ped, pos, radius, highlightArea, false, false, true);
}

/*
 * @opcode 00F0
 * @command LOCATE_STOPPED_CHAR_ON_FOOT_2D
 * @class Char
 * @method LocateStoppedOnFoot2D
 * 
 * @brief Returns true if the character stopped within the 2D radius of the coordinates point on foot
 * 
 * @param {Char} self
 * @param {Vector2D} 
 * @param {Vector2D} radius
 * @param {bool} drawSphere
 */
auto LocateStoppedCharOnFoot2D(CRunningScript& S, CPed& ped, CVector2D pos, CVector2D radius, bool highlightArea) {
    return LocateChar2D(S, ped, pos, radius, highlightArea, false, true, true);
}

/*
 * @opcode 00F1
 * @command LOCATE_STOPPED_CHAR_IN_CAR_2D
 * @class Char
 * @method LocateStoppedInCar2D
 * 
 * @brief Returns true if the character stopped within the 2D radius of the coordinates point in a vehicle
 * 
 * @param {Char} self
 * @param {Vector2D} 
 * @param {Vector2D} radius
 * @param {bool} drawSphere
 */
auto LocateStoppedCharInCar2D(CRunningScript& S, CPed& ped, CVector2D pos, CVector2D radius, bool highlightArea) {
    return LocateChar2D(S, ped, pos, radius, highlightArea, true, false, true);
}

// => 3D

/*
 * @opcode 00FE
 * @command LOCATE_CHAR_ANY_MEANS_3D
 * @class Char
 * @method LocateAnyMeans3D
 * 
 * @brief Returns true if the character is within the 3D radius of the coordinates point
 * 
 * @param {Char} self
 * @param {Vector} 
 * @param {Vector} radius
 * @param {bool} drawSphere
 */
auto LocateCharAnyMeans3D(CRunningScript& S, CPed& ped, CVector pos, CVector radius, bool highlightArea) {
    return LocateChar3D(S, ped, pos, radius, highlightArea, false, false, false);
}

/*
 * @opcode 00FF
 * @command LOCATE_CHAR_ON_FOOT_3D
 * @class Char
 * @method LocateOnFoot3D
 * 
 * @brief Returns true if the character is within the 3D radius of the coordinates point on foot
 * 
 * @param {Char} self
 * @param {Vector} 
 * @param {Vector} radius
 * @param {bool} drawSphere
 */
auto LocateCharOnFoot3D(CRunningScript& S, CPed& ped, CVector pos, CVector radius, bool highlightArea) {
    return LocateChar3D(S, ped, pos, radius, highlightArea, false, true, false);
}

/*
 * @opcode 0100
 * @command LOCATE_CHAR_IN_CAR_3D
 * @class Char
 * @method LocateInCar3D
 * 
 * @brief Returns true if the character is within the 3D radius of the coordinates point in a vehicle
 * 
 * @param {Char} self
 * @param {Vector} 
 * @param {Vector} radius
 * @param {bool} drawSphere
 */
auto LocateCharInCar3D(CRunningScript& S, CPed& ped, CVector pos, CVector radius, bool highlightArea) {
    return LocateChar3D(S, ped, pos, radius, highlightArea, true, false, false);
}

/*
 * @opcode 0101
 * @command LOCATE_STOPPED_CHAR_ANY_MEANS_3D
 * @class Char
 * @method LocateStoppedAnyMeans3D
 * 
 * @brief Returns true if the character stopped within the 3D radius of the coordinates point
 * 
 * @param {Char} self
 * @param {Vector} 
 * @param {Vector} radius
 * @param {bool} drawSphere
 */
auto LocateStoppedCharAnyMeans3D(CRunningScript& S, CPed& ped, CVector pos, CVector radius, bool highlightArea) {
    return LocateChar3D(S, ped, pos, radius, highlightArea, false, false, true);
}

/*
 * @opcode 0102
 * @command LOCATE_STOPPED_CHAR_ON_FOOT_3D
 * @class Char
 * @method LocateStoppedOnFoot3D
 * 
 * @brief Returns true if the character stopped within the 3D radius of the coordinates point on foot
 * 
 * @param {Char} self
 * @param {Vector} 
 * @param {Vector} radius
 * @param {bool} drawSphere
 */
auto LocateStoppedCharOnFoot3D(CRunningScript& S, CPed& ped, CVector pos, CVector radius, bool highlightArea) {
    return LocateChar3D(S, ped, pos, radius, highlightArea, false, true, true);
}

/*
 * @opcode 0103
 * @command LOCATE_STOPPED_CHAR_IN_CAR_3D
 * @class Char
 * @method LocateStoppedInCar3D
 * 
 * @brief Returns true if the character stopped within the 3D radius of the coordinates point in a vehicle
 * 
 * @param {Char} self
 * @param {Vector} 
 * @param {Vector} radius
 * @param {bool} drawSphere
 */
auto LocateStoppedCharInCar3D(CRunningScript& S, CPed& ped, CVector pos, CVector radius, bool highlightArea) {
    return LocateChar3D(S, ped, pos, radius, highlightArea, true, false, true);
}

//
// Char, Char
//

// => 2D

/*
 * @opcode 00F2
 * @command LOCATE_CHAR_ANY_MEANS_CHAR_2D
 * @class Char
 * @method LocateAnyMeansChar2D
 * 
 * @brief Returns true if the character is within the 2D radius of the other character
 * 
 * @param {Char} self
 * @param {Char} target
 * @param {Vector2D} radius
 * @param {bool} drawSphere
 */
auto LocateCharAnyMeansChar2D(CRunningScript& S, CPed& ped1, CPed& ped2, CVector2D radius, bool highlightArea) {
    return LocateCharEntity2D(S, ped1, ped2, radius, highlightArea, false, false);
}

/*
 * @opcode 00F3
 * @command LOCATE_CHAR_ON_FOOT_CHAR_2D
 * @class Char
 * @method LocateOnFootChar2D
 * 
 * @brief Returns true if the character is within the 2D radius of the other character on foot
 * 
 * @param {Char} self
 * @param {Char} target
 * @param {Vector2D} radius
 * @param {bool} drawSphere
 */
auto LocateCharOnFootChar2D(CRunningScript& S, CPed& ped1, CPed& ped2, CVector2D radius, bool highlightArea) {
    return LocateCharEntity2D(S, ped1, ped2, radius, highlightArea, false, true);
}

/*
 * @opcode 00F4
 * @command LOCATE_CHAR_IN_CAR_CHAR_2D
 * @class Char
 * @method LocateInCarChar2D
 * 
 * @brief Returns true if the character is within the 2D radius of the other character in a vehicle
 * 
 * @param {Char} self
 * @param {Char} otherChar
 * @param {Vector2D} radius
 * @param {bool} drawSphere
 */
auto LocateCharInCarChar2D(CRunningScript& S, CPed& ped1, CPed& ped2, CVector2D radius, bool highlightArea) {
    return LocateCharEntity2D(S, ped1, ped2, radius, highlightArea, true, false);
}

// => 3D

/*
 * @opcode 0104
 * @command LOCATE_CHAR_ANY_MEANS_CHAR_3D
 * @class Char
 * @method LocateAnyMeansChar3D
 * 
 * @brief Returns true if the character is within the 3D radius of the other character
 * 
 * @param {Char} self
 * @param {Char} target
 * @param {Vector} radius
 * @param {bool} drawSphere
 */
auto LocateCharAnyMeansChar3D(CRunningScript& S, CPed& ped1, CPed& ped2, CVector radius, bool highlightArea) {
    return LocateCharEntity3D(S, ped1, ped2, radius, highlightArea, false, false);
}

/*
 * @opcode 0105
 * @command LOCATE_CHAR_ON_FOOT_CHAR_3D
 * @class Char
 * @method LocateOnFootChar3D
 * 
 * @brief Returns true if the character is within the 3D radius of the other character on foot
 * 
 * @param {Char} self
 * @param {Char} target
 * @param {Vector} radius
 * @param {bool} drawSphere
 */
auto LocateCharOnFootChar3D(CRunningScript& S, CPed& ped1, CPed& ped2, CVector radius, bool highlightArea) {
    return LocateCharEntity3D(S, ped1, ped2, radius, highlightArea, false, true);
}

/*
 * @opcode 0106
 * @command LOCATE_CHAR_IN_CAR_CHAR_3D
 * @class Char
 * @method LocateInCarChar3D
 * 
 * @brief Returns true if the character is within the 3D radius of the other character in a vehicle
 * 
 * @param {Char} self
 * @param {Char} target
 * @param {Vector} radius
 * @param {bool} drawSphere
 */
auto LocateCharInCarChar3D(CRunningScript& S, CPed& ped1, CPed& ped2, CVector radius, bool highlightArea) {
    return LocateCharEntity3D(S, ped1, ped2, radius, highlightArea, true, false);
}

//
// Char, Car
//

// => 2D

/*
 * @opcode 0202
 * @command LOCATE_CHAR_ANY_MEANS_CAR_2D
 * @class Char
 * @method LocateAnyMeansCar2D
 * 
 * @brief Returns true if the character is within the 2D radius of the vehicle
 * 
 * @param {Char} self
 * @param {Car} vehicle
 * @param {Vector2D} radius
 * @param {bool} drawSphere
 */
auto LocateCharAnyMeansCar2D(CRunningScript& S, CPed& ped, CVehicle& veh, CVector2D radius, bool highlightArea) {
    return LocateCharEntity2D(S, ped, veh, radius, highlightArea, false, false);
}

/*
 * @opcode 0203
 * @command LOCATE_CHAR_ON_FOOT_CAR_2D
 * @class Char
 * @method LocateOnFootCar2D
 * 
 * @brief Returns true if the character is within the 2D radius of the vehicle on foot
 * 
 * @param {Char} self
 * @param {Car} vehicle
 * @param {Vector2D} radius
 * @param {bool} drawSphere
 */
auto LocateCharOnFootCar2D(CRunningScript& S, CPed& ped, CVehicle& veh, CVector2D radius, bool highlightArea) {
    return LocateCharEntity2D(S, ped, veh, radius, highlightArea, false, true);
}

/*
 * @opcode 0204
 * @command LOCATE_CHAR_IN_CAR_CAR_2D
 * @class Char
 * @method LocateInCarCar2D
 * 
 * @brief Returns true if the character is within the 2D radius of the vehicle in a vehicle
 * 
 * @param {Char} self
 * @param {Car} handle
 * @param {Vector2D} radius
 * @param {bool} drawSphere
 */
auto LocateCharInCarCar2D(CRunningScript& S, CPed& ped, CVehicle& veh, CVector2D radius, bool highlightArea) {
    return LocateCharEntity2D(S, ped, veh, radius, highlightArea, true, false);
}

// => 3D

/*
 * @opcode 0205
 * @command LOCATE_CHAR_ANY_MEANS_CAR_3D
 * @class Char
 * @method LocateAnyMeansCar3D
 * 
 * @brief Returns true if the character is within the 3D radius of the vehicle
 * 
 * @param {Char} self
 * @param {Car} vehicle
 * @param {Vector} radius
 * @param {bool} drawSphere
 */
auto LocateCharAnyMeansCar3D(CRunningScript& S, CPed& ped, CVehicle& veh, CVector radius, bool highlightArea) {
    return LocateCharEntity3D(S, ped, veh, radius, highlightArea, false, false);
}

/*
 * @opcode 0206
 * @command LOCATE_CHAR_ON_FOOT_CAR_3D
 * @class Char
 * @method LocateOnFootCar3D
 * 
 * @brief Returns true if the character is within the 3D radius of the vehicle on foot
 * 
 * @param {Char} self
 * @param {Car} vehicle
 * @param {Vector} radius
 * @param {bool} drawSphere
 */
auto LocateCharOnFootCar3D(CRunningScript& S, CPed& ped, CVehicle& veh, CVector radius, bool highlightArea) {
    return LocateCharEntity3D(S, ped, veh, radius, highlightArea, false, true);
}

/*
 * @opcode 0207
 * @command LOCATE_CHAR_IN_CAR_CAR_3D
 * @class Char
 * @method LocateInCarCar3D
 * 
 * @brief Returns true if the character is within the 3D radius of the vehicle in a vehicle
 * 
 * @param {Char} self
 * @param {Car} vehicle
 * @param {Vector} radius
 * @param {bool} drawSphere
 */
auto LocateCharInCarCar3D(CRunningScript& S, CPed& ped, CVehicle& veh, CVector radius, bool highlightArea) {
    return LocateCharEntity3D(S, ped, veh, radius, highlightArea, true, false);
}

//
// Char, Obj
//

// => 2D

/*
 * @opcode 0471
 * @command LOCATE_CHAR_ANY_MEANS_OBJECT_2D
 * @class Char
 * @method LocateAnyMeansObject2D
 * 
 * @brief Returns true if the character is within the 2D radius of the object
 * 
 * @param {Char} self
 * @param {Object} object
 * @param {Vector2D} radius
 * @param {bool} drawSphere
 */
auto LocateCharAnyMeansObject2D(CRunningScript& S, CPed& ped, CObject& obj, CVector2D radius, bool highlightArea) {
    return LocateCharEntity2D(S, ped, obj, radius, highlightArea, false, false);
}

/*
 * @opcode 0472
 * @command LOCATE_CHAR_ON_FOOT_OBJECT_2D
 * @class Char
 * @method LocateOnFootObject2D
 * 
 * @brief Returns true if the character is within the 2D radius of the object on foot
 * 
 * @param {Char} self
 * @param {Object} object
 * @param {Vector2D} radius
 * @param {bool} drawSphere
 */
auto LocateCharOnFootObject2D(CRunningScript& S, CPed& ped, CObject& obj, CVector2D radius, bool highlightArea) {
    return LocateCharEntity2D(S, ped, obj, radius, highlightArea, false, true);
}

/*
 * @opcode 0473
 * @command LOCATE_CHAR_IN_CAR_OBJECT_2D
 * @class Char
 * @method LocateInCarObject2D
 * 
 * @brief Returns true if the character is within the 2D radius of the object in a vehicle
 * 
 * @param {Char} self
 * @param {Object} object
 * @param {Vector2D} radius
 * @param {bool} drawSphere
 */
auto LocateCharInCarObject2D(CRunningScript& S, CPed& ped, CObject& obj, CVector2D radius, bool highlightArea) {
    return LocateCharEntity2D(S, ped, obj, radius, highlightArea, true, false);
}

// => 3D

/*
 * @opcode 0474
 * @command LOCATE_CHAR_ANY_MEANS_OBJECT_3D
 * @class Char
 * @method LocateAnyMeansObject3D
 * 
 * @brief Returns true if the character is within the 3D radius of the object
 * 
 * @param {Char} self
 * @param {Object} object
 * @param {Vector} radius
 * @param {bool} drawSphere
 */
auto LocateCharAnyMeansObject3D(CRunningScript& S, CPed& ped, CObject& obj, CVector radius, bool highlightArea) {
    return LocateCharEntity3D(S, ped, obj, radius, highlightArea, false, false);
}

/*
 * @opcode 0475
 * @command LOCATE_CHAR_ON_FOOT_OBJECT_3D
 * @class Char
 * @method LocateOnFootObject3D
 * 
 * @brief Returns true if the character is within the 3D radius of the object on foot
 * 
 * @param {Char} self
 * @param {Object} object
 * @param {Vector} radius
 * @param {bool} drawSphere
 */
auto LocateCharOnFootObject3D(CRunningScript& S, CPed& ped, CObject& obj, CVector radius, bool highlightArea) {
    return LocateCharEntity3D(S, ped, obj, radius, highlightArea, false, true);
}

/*
 * @opcode 0476
 * @command LOCATE_CHAR_IN_CAR_OBJECT_3D
 * @class Char
 * @method LocateInCarObject3D
 * 
 * @brief Returns true if the character is within the 3D radius of the object in a vehicle
 * 
 * @param {Char} self
 * @param {Object} object
 * @param {Vector} radius
 * @param {bool} drawSphere
 */
auto LocateCharInCarObject3D(CRunningScript& S, CPed& ped, CObject& obj, CVector radius, bool highlightArea) {
    return LocateCharEntity3D(S, ped, obj, radius, highlightArea, true, false);
}

//
// Char, Angled Area
//

// => 2D

bool IsPositionWitihinAngledArea2D_Impl(CVector2D pos, CVector2D a, CVector2D b, float widthAndDir, bool highlightArea, bool otherChecksFailed) {
    if (otherChecksFailed) {
        if (highlightArea) {
            notsa::shapes::AngledRect{ a, b, widthAndDir }.HighlightWithMarkers();
        }
        return false;
    }
    else {
        notsa::shapes::AngledRect area{ a, b, widthAndDir };
        if (highlightArea) {
            area.HighlightWithMarkers();
        }
        return area.IsPointWithin(pos);
    }
}

bool IsPedInAngledArea2D(CPed& ped, CVector2D a, CVector2D b, float widthAndDir, bool highlightArea, bool mustBeInCar, bool mustBeOnFoot, bool mustBeStopped) {
    return IsPositionWitihinAngledArea2D_Impl(
        ped.GetPosition2D(),
        a, b, widthAndDir,
        highlightArea,
        !DoLocateCharChecks(ped, mustBeInCar, mustBeOnFoot, mustBeStopped)
    );
}

/*
 * @opcode 05F6
 * @command IS_CHAR_IN_ANGLED_AREA_2D
 * @class Char
 * @method IsInAngledArea2D
 * 
 * @brief Checks if the character is within the angled 2D area
 * 
 * @param {Char} self
 * @param {Vector2D} leftBottom
 * @param {Vector2D} rightTop
 * @param {float} angle
 * @param {bool} drawSphere
 */
auto IsCharInAngledArea2D(CPed& ped, CVector2D a, CVector2D b, float widthAndDir, bool highlightArea) {
    return IsPedInAngledArea2D(ped, a, b, widthAndDir, highlightArea, false, false, false);
}

/*
 * @opcode 05F7
 * @command IS_CHAR_IN_ANGLED_AREA_ON_FOOT_2D
 * @class Char
 * @method IsInAngledAreaOnFoot2D
 * 
 * @brief Checks if the character is within the angled 2D area
 * 
 * @param {Char} self
 * @param {Vector2D} leftBottom
 * @param {Vector2D} rightTop
 * @param {float} angle
 * @param {bool} drawSphere
 */
auto IsCharInAngledAreaOnFoot2D(CPed& ped, CVector2D a, CVector2D b, float widthAndDir, bool highlightArea) {
    return IsPedInAngledArea2D(ped, a, b, widthAndDir, highlightArea, false, true, false);
}

/*
 * @opcode 05F8
 * @command IS_CHAR_IN_ANGLED_AREA_IN_CAR_2D
 * @class Char
 * @method IsInAngledAreaInCar2D
 * 
 * @brief Checks if the character is in a car which is within the angled 2D area
 * 
 * @param {Char} self
 * @param {Vector2D} leftBottom
 * @param {Vector2D} rightTop
 * @param {float} angle
 * @param {bool} drawSphere
 */
auto IsCharInAngledAreaInCar2D(CPed& ped, CVector2D a, CVector2D b, float widthAndDir, bool highlightArea) {
    return IsPedInAngledArea2D(ped, a, b, widthAndDir, highlightArea, true, false, false);
}

/*
 * @opcode 05F9
 * @command IS_CHAR_STOPPED_IN_ANGLED_AREA_2D
 * @class Char
 * @method IsStoppedInAngledArea2D
 * 
 * @brief Checks if the character is within the angled 2D area and is motionless
 * 
 * @param {Char} self
 * @param {Vector2D} leftBottom
 * @param {Vector2D} rightTop
 * @param {float} angle
 * @param {bool} drawSphere
 */
auto IsCharStoppedInAngledArea2D(CPed& ped, CVector2D a, CVector2D b, float widthAndDir, bool highlightArea) {
    return IsPedInAngledArea2D(ped, a, b, widthAndDir, highlightArea, false, false, true);
}

/*
 * @opcode 05FA
 * @command IS_CHAR_STOPPED_IN_ANGLED_AREA_ON_FOOT_2D
 * @class Char
 * @method IsStoppedInAngledAreaOnFoot2D
 * 
 * @brief Checks if the character is within the angled 2D area
 * 
 * @param {Char} self
 * @param {Vector2D} leftBottom
 * @param {Vector2D} rightTop
 * @param {float} angle
 * @param {bool} drawSphere
 */
auto IsCharStoppedInAngledAreaOnFoot2D(CPed& ped, CVector2D a, CVector2D b, float widthAndDir, bool highlightArea) {
    return IsPedInAngledArea2D(ped, a, b, widthAndDir, highlightArea, false, true, true);
}

// IS_CHAR_STOPPED_IN_ANGLED_AREA_IN_CAR_2D
/*
 * @opcode 05FB
 * @command IS_CHAR_STOPPED_IN_ANGLED_AREA_IN_CAR_2D
 * @class Char
 * @method IsStoppedInAngledAreaInCar2D
 * 
 * @brief Checks if the character is in a motionless car within the angled 2D area
 * 
 * @param {Char} self
 * @param {Vector2D} leftBottom
 * @param {Vector2D} rightTop
 * @param {float} angle
 * @param {bool} drawSphere
 */
auto IsCharStoppedInAngledAreaInCar2D(CPed& ped, CVector2D a, CVector2D b, float widthAndDir, bool highlightArea) {
    return IsPedInAngledArea2D(ped, a, b, widthAndDir, highlightArea, true, false, true);
}

// => 3D

bool IsPedInAngledArea3D(CPed& ped, CVector a, CVector b, float widthAndDir, bool highlightArea, bool mustBeInCar, bool mustBeOnFoot, bool mustBeStopped) {
    const auto& pos = ped.GetPosition();
    const auto [minZ, maxZ] = std::minmax(a.z, b.z);
    return IsPositionWitihinAngledArea2D_Impl(
        pos,
        a, b, widthAndDir,
        highlightArea,
        (pos.z < minZ || pos.z > maxZ) || !DoLocateCharChecks(ped, mustBeInCar, mustBeOnFoot, mustBeStopped)
    );
}

/*
 * @opcode 05FC
 * @command IS_CHAR_IN_ANGLED_AREA_3D
 * @class Char
 * @method IsInAngledArea3D
 * 
 * @brief Checks if the character is within the angled 3D area
 * 
 * @param {Char} self
 * @param {Vector} leftBottom
 * @param {Vector} rightTop
 * @param {float} angle
 * @param {bool} drawSphere
 */
auto IsCharInAngledArea3D(CPed& ped, CVector a, CVector b, float widthAndDir, bool highlightArea) {
    return IsPedInAngledArea3D(ped, a, b, widthAndDir, highlightArea, false, false, false);
}

/*
 * @opcode 05FD
 * @command IS_CHAR_IN_ANGLED_AREA_ON_FOOT_3D
 * @class Char
 * @method IsInAngledAreaOnFoot3D
 * 
 * @brief Checks if the character is within the angled 3D area
 * 
 * @param {Char} self
 * @param {Vector} leftBottom
 * @param {Vector} rightTop
 * @param {float} angle
 * @param {bool} drawSphere
 */
auto IsCharInAngledAreaOnFoot3D(CPed& ped, CVector a, CVector b, float widthAndDir, bool highlightArea) {
    return IsPedInAngledArea3D(ped, a, b, widthAndDir, highlightArea, false, true, false);
}

/*
 * @opcode 05FE
 * @command IS_CHAR_IN_ANGLED_AREA_IN_CAR_3D
 * @class Char
 * @method IsInAngledAreaInCar3D
 * 
 * @brief Checks if the character is in a car which is within the angled 3D area
 * 
 * @param {Char} self
 * @param {Vector} leftBottom
 * @param {Vector} rightTop
 * @param {float} angle
 * @param {bool} drawSphere
 */
auto IsCharInAngledAreaInCar3D(CPed& ped, CVector a, CVector b, float widthAndDir, bool highlightArea) {
    return IsPedInAngledArea3D(ped, a, b, widthAndDir, highlightArea, true, false, false);
}

/*
 * @opcode 05FF
 * @command IS_CHAR_STOPPED_IN_ANGLED_AREA_3D
 * @class Char
 * @method IsStoppedInAngledArea3D
 * 
 * @brief Checks if the character is within the angled 3D area and is motionless
 * 
 * @param {Char} self
 * @param {Vector} leftBottom
 * @param {Vector} rightTop
 * @param {float} angle
 * @param {bool} drawSphere
 */
auto IsCharStoppedInAngledArea3D(CPed& ped, CVector a, CVector b, float widthAndDir, bool highlightArea) {
    return IsPedInAngledArea3D(ped, a, b, widthAndDir, highlightArea, false, false, true);
}

/*
 * @opcode 0600
 * @command IS_CHAR_STOPPED_IN_ANGLED_AREA_ON_FOOT_3D
 * @class Char
 * @method IsStoppedInAngledAreaOnFoot3D
 * 
 * @brief Checks if the character is on foot within the angled 3D area and is motionless
 * 
 * @param {Char} self
 * @param {Vector} leftBottom
 * @param {Vector} rightTop
 * @param {float} angle
 * @param {bool} drawSphere
 */
auto IsCharStoppedInAngledAreaOnFoot3D(CPed& ped, CVector a, CVector b, float widthAndDir, bool highlightArea) {
    return IsPedInAngledArea3D(ped, a, b, widthAndDir, highlightArea, false, true, true);
}

// IS_CHAR_STOPPED_IN_ANGLED_AREA_IN_CAR_3D
/*
 * @opcode 0601
 * @command IS_CHAR_STOPPED_IN_ANGLED_AREA_IN_CAR_3D
 * @class Char
 * @method IsStoppedInAngledAreaInCar3D
 * 
 * @brief Checks if the character is in a motionless car within the angled 3D area
 * 
 * @param {Char} self
 * @param {Vector} leftBottom
 * @param {Vector} rightTop
 * @param {float} angle
 * @param {bool} drawSphere
 */
auto IsCharStoppedInAngledAreaInCar3D(CPed& ped, CVector a, CVector b, float widthAndDir, bool highlightArea) {
    return IsPedInAngledArea3D(ped, a, b, widthAndDir, highlightArea, true, false, true);
}

/*
 * @opcode 0118
 * @command IS_CHAR_DEAD
 * @class Char
 * @method IsDead
 * @static
 * 
 * @brief Returns true if the handle is an invalid character handle or the character is dead (wasted)
 * 
 * @param {Char} char_
 */
auto IsCharDead(CPed* ped) {
    return !ped || ped->IsStateDeadForScript();
}

/*
 * @opcode 0154
 * @command IS_CHAR_IN_ZONE
 * @class Char
 * @method IsInZone
 * 
 * @brief Returns true if the character is in the specified map zone
 * 
 * @param {Char} self
 * @param {zone_key} zone
 */
auto IsCharInZone(CPed& ped, std::string_view zoneName) {
    return CTheZones::FindZone(GetCharCoordinates(ped), zoneName, ZONE_TYPE_NAVI);
}

/*
 * @opcode 0172
 * @command GET_CHAR_HEADING
 * @class Char
 * @method GetHeading
 * 
 * @brief Returns the character's heading (z-angle)
 * 
 * @param {Char} self
 * 
 * @returns {float} heading
 */
auto GetCharHeading(CPed& ped) {
    return FixAngleDegrees(RadiansToDegrees(GetPedOrItsVehicle(ped).GetHeading()));
}

/*
 * @opcode 0173
 * @command SET_CHAR_HEADING
 * @class Char
 * @method SetHeading
 * 
 * @brief Sets the character's heading (z-angle)
 * 
 * @param {Char} self
 * @param {float} heading
 */
auto SetCharHeading(CPed& ped, float deg) {
    if (ped.IsInVehicle()) {
        return;
    }

    const auto rad = DegreesToRadians(FixAngleDegrees(deg));
    ped.m_fAimingRotation = ped.m_fCurrentRotation = rad;
    ped.SetHeading(rad);
    ped.UpdateRwMatrix();
}

/*
 * @opcode 0179
 * @command IS_CHAR_TOUCHING_OBJECT
 * @class Char
 * @method IsTouchingObject
 * 
 * @brief Returns true if the character is colliding with the specified object
 * 
 * @param {Char} self
 * @param {Object} object
 */
auto IsCharTouchingObject(CPed& ped, CObject& obj) {
    return GetPedOrItsVehicle(ped).GetHasCollidedWith(&obj);
}

/*
 * @opcode 017B
 * @command SET_CHAR_AMMO
 * @class Char
 * @method SetAmmo
 * 
 * @brief Sets the amount of ammo the character has in the specified weapon
 * 
 * @param {Char} self
 * @param {WeaponType} weaponType
 * @param {int} ammo
 */
auto SetCharAmmo(CPed& ped, eWeaponType wep, int32 nammo) {
    ped.SetAmmo(wep, nammo);
}

/*
 * @opcode 0184
 * @command IS_CHAR_HEALTH_GREATER
 * @class Char
 * @method IsHealthGreater
 * 
 * @brief Returns true if the character's health is over the specified value
 * 
 * @param {Char} self
 * @param {int} health
 */
auto IsCharHealthGreater(CPed& ped, float health) {
    return ped.m_fHealth >= health;
}

auto CreatePed(CRunningScript& S, ePedType pedType, eModelID pedModel) -> CPed& {
    const auto ped = [&]() -> CPed* {
        uint32 typeSpecificModelId = (uint32)(pedModel);
        S.GetCorrectPedModelIndexForEmergencyServiceType(pedType, &typeSpecificModelId);
        switch (pedType) {
        case PED_TYPE_COP:      return new CCopPed{ typeSpecificModelId };
        case PED_TYPE_MEDIC:
        case PED_TYPE_FIREMAN:  return new CEmergencyPed{ pedType, typeSpecificModelId };
        default:                return new CCivilianPed{ pedType, typeSpecificModelId };
        }
    }();
    ped->SetCharCreatedBy(PED_MISSION);
    ped->bAllowMedicsToReviveMe = false;
    CPopulation::ms_nTotalMissionPeds++;
    if (S.m_UsesMissionCleanup) {
        CTheScripts::MissionCleanUp.AddEntityToList(*ped);
    }
    return *ped;
}

//! Creates a character in the driver's seat of the vehicle
CPed& CreateCharInsideCar(CRunningScript& S, CVehicle& veh, ePedType pedType, eModelID pedModel) {
    const auto ped = &CreatePed(S, pedType, pedModel);
    CTaskSimpleCarSetPedInAsDriver{ &veh, true, nullptr}.ProcessPed(ped); // Make ped get into da car
    CWorld::Add(ped);
    return *ped;
}

/// SET_CHAR_HEALTH(0223)
/*
 * @opcode 0223
 * @command SET_CHAR_HEALTH
 * @class Char
 * @method SetHealth
 * 
 * @brief Sets the character's health
 * 
 * @param {Char} self
 * @param {int} health
 */
auto SetCharHealth(CPed& ped, float health) {
    if (health != 0.f) {
        if (ped.IsPlayer()) {
            const auto slot = CWorld::FindPlayerSlotWithPedPointer(&ped);
            ped.m_fHealth = std::min(health, (float)CWorld::Players[slot].m_nMaxHealth);
        } else {
            ped.m_fHealth = health;
            if (ped.m_fMaxHealth == 100.f) {
                ped.m_fMaxHealth = health;
            }
        }
    } else { // Otherwise kill ped
        ped.GetIntelligence()->m_eventGroup.Add(
            CEventScriptCommand{
                TASK_PRIMARY_PRIMARY,
                new CTaskComplexDie{
                    WEAPON_UNARMED,
                    ANIM_GROUP_DEFAULT,
                    ANIM_ID_KO_SHOT_FRONT_0
                },
                false
            }
        );
    }
}

/*
 * @opcode 0226
 * @command GET_CHAR_HEALTH
 * @class Char
 * @method GetHealth
 * 
 * @brief Returns the character's health
 * 
 * @param {Char} self
 * 
 * @returns {int} health
 */
auto GetCharHealth(CPed& ped) {
    return ped.m_fHealth;
}

/*
 * @opcode 023B
 * @command IS_CHAR_TOUCHING_OBJECT_ON_FOOT
 * @class Char
 * @method IsTouchingObjectOnFoot
 * 
 * @brief Returns true if the character is colliding with the specified object on foot
 * 
 * @param {Char} self
 * @param {Object} object
 */
auto IsCharTouchingObjectOnFoot(CPed& ped, CObject& obj) {
    return !ped.IsInVehicle() && ped.GetHasCollidedWith(&obj);
}

/*
 * @opcode 02A0
 * @command IS_CHAR_STOPPED
 * @class Char
 * @method IsStopped
 * 
 * @brief Returns true if the character is not moving
 * 
 * @param {Char} self
 */
auto IsCharStopped(CPed& ped) {
    return CTheScripts::IsPedStopped(&ped);
}

/*
 * @opcode 02A9
 * @command SET_CHAR_ONLY_DAMAGED_BY_PLAYER
 * @class Char
 * @method SetOnlyDamagedByPlayer
 * 
 * @brief Makes a character immune to everything except the player
 * 
 * @param {Char} self
 * @param {bool} state
 */
auto SetCharOnlyDamagedByPlayer(CPed& ped, bool enabled) {
    ped.physicalFlags.bInvulnerable = enabled;
}

/*
 * @opcode 02C0
 * @command GET_CLOSEST_CHAR_NODE
 * @class Path
 * @method GetClosestCharNode
 * @static
 * 
 * @brief Returns the nearest path node from the specified coordinates that a pedestrian can walk on
 * 
 * @param {Vector} 
 * 
 * @returns {Vector} node
 */
auto GetClosestCharNode(CVector pos) -> CVector {
    CWorld::PutToGroundIfTooLow(pos);
    if (const auto node = ThePaths.FindNodeClosestToCoors(pos)) {
        return ThePaths.GetPathNode(node)->GetPosition();
    }
    return {}; // Can't find anything nearby
}

/*
 * @opcode 02CB
 * @command IS_CHAR_ON_SCREEN
 * @class Char
 * @method IsOnScreen
 * 
 * @brief Returns true if the character is visible
 * 
 * @param {Char} self
 */
auto IsCharOnScreen(CPed& ped) {
    return ped.GetIsOnScreen();
}

/*
 * @opcode 02D6
 * @command IS_CHAR_SHOOTING_IN_AREA
 * @class Char
 * @method IsShootingInArea
 * 
 * @brief Returns true if the character fired a weapon within the specified 2D area
 * 
 * @param {Char} self
 * @param {Vector2D} leftBottom
 * @param {Vector2D} topRight
 * @param {bool} drawSphere
 */
auto IsCharShootingInArea(CRunningScript& S, CPed& ped, CRect area, bool highlightArea) {
    if (highlightArea) {
        S.HighlightImportantArea(area);
    }

    if (CTheScripts::DbgFlag) {
        CTheScripts::DrawDebugSquare(area);
    }

    return ped.bFiringWeapon && area.IsPointInside(ped.GetPosition2D());
}

/*
 * @opcode 02D8
 * @command IS_CURRENT_CHAR_WEAPON
 * @class Char
 * @method IsCurrentWeapon
 * 
 * @brief Returns true if the character is holding the given type of weapon
 * 
 * @param {Char} self
 * @param {WeaponType} weaponType
 */
auto IsCurrentCharWeapon(CPed& ped, eWeaponType wep) {
    if (wep == WEAPON_ANYMELEE && ped.GetActiveWeapon().IsTypeMelee()) {
        return true;
    }
    return ped.GetActiveWeapon().m_Type == wep;
}

/*
 * @opcode 02DD
 * @command GET_RANDOM_CHAR_IN_ZONE
 * @class Zone
 * @method GetRandomChar
 * 
 * @brief Gets a random character in the specified zone whose pedtype matches the specified values
 * 
 * @param {zone_key} zone
 * @param {bool} civilian
 * @param {bool} gang
 * @param {bool} criminalOrProstitute
 * 
 * @returns {Char} handle
 */
auto GetRandomCharInZone(CRunningScript& S, std::string_view zoneName, bool civilian, bool gang, bool criminal) -> CPed* { // 0x04802D0
    const auto playerPosZ = FindPlayerCoors().z;
    for (auto& ped : GetPedPool()->GetAllValid()) {
        const auto pedHandle = GetPedPool()->GetRef(&ped);
        if (   pedHandle == CTheScripts::LastRandomPedId
            || !ped.IsCreatedBy(PED_GAME)
            || ped.m_bRemoveFromWorld
            || ped.bFadeOut
            || ped.IsStateDeadForScript()
            || ped.IsInVehicle()
            || !S.ThisIsAValidRandomPed(ped.m_nPedType, civilian, gang, criminal)
            || ped.GetGroup()
        ) {
            continue;
        }
        const auto pos = ped.GetPosition();
        if (!CTheZones::FindZone(ped.GetPosition(), zoneName, ZONE_TYPE_NAVI)) {
            continue;
        }
        if (playerPosZ - 5.f > pos.z || playerPosZ + 5.f < pos.z) {
            continue;
        }
        bool roofZFound{};
        (void)(CWorld::FindRoofZFor3DCoord(pos.x, pos.y, pos.z, &roofZFound));
        if (roofZFound) {
            continue;
        }
        CTheScripts::LastRandomPedId = pedHandle;
        ped.SetCharCreatedBy(PED_MISSION);
        CPopulation::ms_nTotalMissionPeds++;
        if (S.m_UsesMissionCleanup) {
            CTheScripts::MissionCleanUp.AddEntityToList(ped);
        }

        // They forgot to return here (did it instead after the loop), so let's do that
        return &ped;
    }
    return nullptr; // No suitable ped found
}

/*
 * @opcode 02F2
 * @command IS_CHAR_MODEL
 * @class Char
 * @method IsModel
 * 
 * @brief Returns true if the character's model ID is equivalent to the model ID passed
 * 
 * @param {Char} self
 * @param {model_char} modelId
 */
auto IsCharModel(CPed& ped, eModelID modelId) {
    return ped.m_nModelIndex == modelId;
}

/*
 * @opcode 031D
 * @command HAS_CHAR_BEEN_DAMAGED_BY_WEAPON
 * @class Char
 * @method HasBeenDamagedByWeapon
 * 
 * @brief Returns true if the character has been hit by the specified weapon
 * 
 * @param {Char} self
 * @param {WeaponType} weaponType
 */
auto HasCharBeenDamagedByWeapon(CPed* ped, eWeaponType byWepType) {
    if (!ped) {
        return false;
    }
    switch (byWepType) {
    case WEAPON_ANYWEAPON:
    case WEAPON_ANYMELEE:
        return CDarkel::CheckDamagedWeaponType((eWeaponType)(ped->m_nLastWeaponDamage), byWepType);
    }
    return (eWeaponType)(ped->m_nLastWeaponDamage) == byWepType;
}

/*
 * @opcode 0321
 * @command EXPLODE_CHAR_HEAD
 * @class Char
 * @method ExplodeHead
 * 
 * @brief Dismembers the character
 * 
 * @param {Char} self
 */
auto ExplodeCharHead(CPed& ped) {
    CEventDamage dmgEvent{
        nullptr,
        CTimer::GetTimeInMS(),
        WEAPON_SNIPERRIFLE,
        PED_PIECE_HEAD,
        0,
        false,
        (bool)(ped.bInVehicle)
    };
    CPedDamageResponseCalculator dmgCalc{
        nullptr,
        1000.f,
        WEAPON_SNIPERRIFLE,
        PED_PIECE_HEAD,
        false
    };
    dmgEvent.ComputeDamageResponseIfAffectsPed(
        &ped,
        dmgCalc,
        true
    );
    ped.GetEventGroup().Add(dmgEvent);
}

/*
 * @opcode 0326
 * @command START_CHAR_FIRE
 * @class ScriptFire
 * @method CreateCharFire
 * 
 * @brief Creates a script fire on the character
 * 
 * @param {Char} char_
 * 
 * @returns {ScriptFire} handle
 */
auto StartCharFire(CPed& ped) { // TODO: return type: ScriptThing<CFire>
    return gFireManager.StartScriptFire(ped.GetPosition(), &ped, 0.8f, true, 0, 1);
}

/*
 * @opcode 0337
 * @command SET_CHAR_VISIBLE
 * @class Char
 * @method SetVisible
 * 
 * @brief Sets whether the character is visible or not
 * 
 * @param {Char} self
 * @param {bool} state
 */
auto SetCharVisible(CPed& ped, bool isVisible) {
    if (&ped == FindPlayerPed()) {
        gPlayerPedVisible = isVisible;
    }
    ped.SetIsVisible(isVisible);
}

/*
 * @opcode 034F
 * @command REMOVE_CHAR_ELEGANTLY
 * @class Char
 * @method RemoveElegantly
 * 
 * @brief Removes the character with a fade, freeing game memory
 * 
 * @param {Char} self
 */
auto RemoveCharElegantly(CRunningScript& S, CPed* ped) {
    if (ped && ped->IsCreatedBy(PED_MISSION)) {
        if (ped->IsInVehicle()) {
            CTheScripts::RemoveThisPed(ped);
        } else {
            ped->SetCharCreatedBy(PED_GAME);
            if (const auto grp = ped->GetGroup()) {
                if (grp->GetMembership().IsFollower(ped)) { // TODO: Most likely inlined, this check makes no sense otherwise
                    grp->GetMembership().RemoveMember(ped);
                }
            }
            CPopulation::ms_nTotalMissionPeds--;
            ped->bFadeOut = true;
            CWorld::RemoveReferencesToDeletedObject(ped);
        }
    }
    if (S.m_UsesMissionCleanup) {
        CTheScripts::MissionCleanUp.RemoveEntityFromList(*ped);
    }
}

/*
 * @opcode 0350
 * @command SET_CHAR_STAY_IN_SAME_PLACE
 * @class Char
 * @method SetStayInSamePlace
 * 
 * @brief Makes the character maintain their position when attacked
 * 
 * @param {Char} self
 * @param {bool} state
 */
auto SetCharStayInSamePlace(CPed& ped, bool bStay) {
    ped.SetStayInSamePlace(bStay);
}

/*
 * @opcode 0362
 * @command WARP_CHAR_FROM_CAR_TO_COORD
 * @class Char
 * @method WarpFromCarToCoord
 * 
 * @brief Pulls the character out of their car and places at the location
 * 
 * @param {Char} self
 * @param {Vector} 
 */
auto WarpCharFromCarToCoord(CPed& ped, CVector pos) {
    CWorld::PutToGroundIfTooLow(pos);
    ped.GetIntelligence()->FlushImmediately(true);
    pos.z += ped.GetDistanceFromCentreOfMassToBaseOfModel();
    ped.Teleport(
        pos,
        false
    );
    CTheScripts::ClearSpaceForMissionEntity(pos, &ped);
}

/*
 * @opcode 0364
 * @command HAS_CHAR_SPOTTED_CHAR
 * @class Char
 * @method HasSpottedChar
 * 
 * @brief Returns true if the character can see the target character
 * 
 * @param {Char} self
 * @param {Char} target
 */
auto HasCharSpottedChar(CPed& ped, CPed& target) {
    return ped.OurPedCanSeeThisEntity(&target, true);
}

/*
 * @opcode 036A
 * @command WARP_CHAR_INTO_CAR
 * @class Char
 * @method WarpIntoCar
 * 
 * @brief Puts the character in the specified vehicle
 * 
 * @param {Char} self
 * @param {Car} vehicle
 */
auto WarpCharIntoCar(CPed& ped, CVehicle& veh) {
    ped.GetIntelligence()->FlushImmediately(false);
    CTaskSimpleCarSetPedInAsDriver{ &veh, true, nullptr }.ProcessPed(&ped);
}

/*
 * @opcode 0393
 * @command SET_CHAR_ANIM_SPEED
 * @class Char
 * @method SetAnimSpeed
 * 
 * @brief Makes an char perform an animation at the specified speed
 * 
 * @param {Char} self
 * @param {string} animName
 * @param {float} animSpeed
 */
auto SetCharAnimSpeed(CPed& ped, const char* animName, float speed) {
    if (const auto anim = RpAnimBlendClumpGetAssociation(ped.m_pRwClump, animName)) {
        anim->SetSpeed(speed);
    }
}

/*
 * @opcode 03A3
 * @command IS_CHAR_MALE
 * @class Char
 * @method IsMale
 * 
 * @brief Returns true if the character is male
 * 
 * @param {Char} self
 */
auto IsCharMale(CPed& ped) {
    return !IsPedTypeFemale(ped.m_nPedType);
}

/*
 * @opcode 03C0
 * @command STORE_CAR_CHAR_IS_IN_NO_SAVE
 * @class Char
 * @method StoreCarIsInNoSave
 * 
 * @brief Returns the character's vehicle handle without marking it as used by the script, therefore allowing it to be deleted by the game at any time (alts:00D9,0811,0484)
 * 
 * @param {Char} self
 * 
 * @returns {Car} handle
 */
auto StoreCarCharIsInNoSave(CPed& ped) -> CVehicle& { 
    assert(ped.bInVehicle && ped.m_pVehicle); // Original code had a bug (namely, calling VehiclePool::GetRef() with nullptr, very bad)
    return *ped.m_pVehicle;
}

/*
 * @opcode 03FE
 * @command SET_CHAR_MONEY
 * @class Char
 * @method SetMoney
 * 
 * @brief Sets the character's cash sum, setting how much cash they will drop when dead
 * 
 * @param {Char} self
 * @param {int} amount
 */
auto SetCharMoney(CPed& ped, int16 money) {
    ped.bMoneyHasBeenGivenByScript = true;
    ped.m_nMoneyCount = money;
}

/*
 * @opcode 041A
 * @command GET_AMMO_IN_CHAR_WEAPON
 * @class Char
 * @method GetAmmoInWeapon
 * 
 * @brief Gets the amount of ammo in the specified weapon of the character
 * 
 * @param {Char} self
 * @param {WeaponType} weaponType
 * 
 * @returns {int} ammo
 */
auto GetAmmoInCharWeapon(CPed& ped, eWeaponType wtype) -> uint32 {
    for (auto& wep : ped.m_aWeapons) { 
        if (wep.m_Type == wtype) { // Originally they continued looping, but that doesn't make sense (A ped can't have the same weapon _twice_)
            return wep.m_TotalAmmo;
        }
    }
    return 0;
}

/*
 * @opcode 0430
 * @command WARP_CHAR_INTO_CAR_AS_PASSENGER
 * @class Char
 * @method WarpIntoCarAsPassenger
 * 
 * @brief Puts the character into a vehicle's passenger seat
 * 
 * @param {Char} self
 * @param {Car} handle
 * @param {SeatId} seat
 */
auto WarpCharIntoCarAsPassenger(CPed& ped, CVehicle& veh, int32 psgrSeatIdx) {
    if (psgrSeatIdx >= 0) {
        psgrSeatIdx = CCarEnterExit::ComputeTargetDoorToEnterAsPassenger(&veh, psgrSeatIdx);
    }
    ped.GetIntelligence()->FlushImmediately(false);
    CTaskSimpleCarSetPedInAsPassenger{ &veh, (eTargetDoor)(psgrSeatIdx), true }.ProcessPed(&ped); // Warp ped into car
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
auto GetCharInCarPassengerSeat(CVehicle& veh, uint32 psgrSeatIdx) -> CPed& {
    return *veh.m_apPassengers[psgrSeatIdx];
}

bool HasPedSittingInCarTask(CPed& ped) {
    return ped.GetTaskManager().IsSimplestActiveTaskOfType({ TASK_SIMPLE_CAR_DRIVE, TASK_SIMPLE_GANG_DRIVEBY });
}

/*
 * @opcode 0448
 * @command IS_CHAR_SITTING_IN_CAR
 * @class Char
 * @method IsSittingInCar
 * 
 * @brief Returns true if the character is sitting in the specified vehicle
 * 
 * @param {Char} self
 * @param {Car} vehicle
 */
auto IsCharSittingInCar(CPed& ped, CVehicle& veh) {
    return ped.bInVehicle && ped.m_pVehicle == &veh && HasPedSittingInCarTask(ped);
}

/*
 * @opcode 0449
 * @command IS_CHAR_SITTING_IN_ANY_CAR
 * @class Char
 * @method IsSittingInAnyCar
 * 
 * @brief Returns true if the character is sitting in any vehicle
 * 
 * @param {Char} self
 */
auto IsCharSittingInAnyCar(CPed& ped) {
    return ped.bInVehicle && HasPedSittingInCarTask(ped);
}

/*
 * @opcode 044B
 * @command IS_CHAR_ON_FOOT
 * @class Char
 * @method IsOnFoot
 * 
 * @brief Returns true if the character is on foot, and not occupying a vehicle
 * 
 * @param {Char} self
 */
auto IsCharOnFoot(CPed& ped) {
    return !ped.bInVehicle && !ped.GetTaskManager().HasAnyOf<TASK_COMPLEX_ENTER_CAR_AS_PASSENGER, TASK_COMPLEX_ENTER_CAR_AS_DRIVER>();
}

/*
 * @opcode 0464
 * @command ATTACH_CHAR_TO_CAR
 * @class Char
 * @method AttachToCar
 * 
 * @brief Puts character into a turret on the vehicle, allowing them to shoot
 * 
 * @param {Char} self
 * @param {Car} vehicle
 * @param {Vector} offset
 * @param {Facing} heading
 * @param {float} headingRange
 * @param {WeaponType} weaponType
 */
auto AttachCharToCar(CPed& ped, CVehicle& veh, CVector offset, int32 pos, float angleLimitDeg, eWeaponType wtype) {
    ped.AttachPedToEntity(
        &veh,
        offset,
        pos,
        DegreesToRadians(angleLimitDeg),
        wtype
    );
}

/*
 * @opcode 0465
 * @command DETACH_CHAR_FROM_CAR
 * @class Char
 * @method DetachFromCar
 * 
 * @brief Takes the character out of turret mode (0464)
 * 
 * @param {Char} self
 */
auto DetachCharFromCar(CPed* ped) {
    if (ped && ped->m_pAttachedTo) {
        ped->DettachPedFromEntity();
    }
}

/*
 * @opcode 0467
 * @command CLEAR_CHAR_LAST_WEAPON_DAMAGE
 * @class Char
 * @method ClearLastWeaponDamage
 * 
 * @brief Clears the character's last weapon damage (see 031D)
 * 
 * @param {Char} self
 */
auto ClearCharLastWeaponDamage(CPed& ped) {
    ped.m_nLastWeaponDamage = -1;
}

/*
 * @opcode 0470
 * @command GET_CURRENT_CHAR_WEAPON
 * @class Char
 * @method GetCurrentWeapon
 * 
 * @brief Returns the type of weapon that the character is currently holding
 * 
 * @param {Char} self
 * 
 * @returns {WeaponType} weaponType
 */
auto GetCurrentCharWeapon(CPed& ped) {
    return ped.GetActiveWeapon().m_Type;
}

/*
 * @opcode 0480
 * @command CAN_CHAR_SEE_DEAD_CHAR
 * @class Char
 * @method CanSeeDeadChar
 * 
 * @brief Returns true if the character sees a dead body of the given type
 * 
 * @param {Char} self
 * @param {PedType} pedType
 */
auto CanCharSeeDeadChar(CPed& ped) {
    for (auto& nearbyPed : ped.GetIntelligence()->GetPedScanner().GetEntities<CPed>()) {
        if (!nearbyPed.IsAlive() && CPedGeometryAnalyser::CanPedTargetPed(ped, nearbyPed, true)) {
            return true;
        }
    }
    return false;
}

/*
 * @opcode 0489
 * @command SHUT_CHAR_UP
 * @class Char
 * @method ShutUp
 * 
 * @brief Sets the character's ability to talk
 * 
 * @param {Char} self
 * @param {bool} state
 */
auto ShutCharUp(CPed& ped, bool stfu) {
    if (stfu) {
        ped.DisablePedSpeech(false);
    } else {
        ped.EnablePedSpeech();
    }
}

/*
 * @opcode 048F
 * @command REMOVE_ALL_CHAR_WEAPONS
 * @class Char
 * @method RemoveAllWeapons
 * 
 * @brief Removes the characters weapons
 * 
 * @param {Char} self
 */
auto RemoveAllCharWeapons(CPed& ped) {
    ped.ClearWeapons();
}

/*
 * @opcode 0491
 * @command HAS_CHAR_GOT_WEAPON
 * @class Char
 * @method HasGotWeapon
 * 
 * @brief Returns true if the character has the specified weapon
 * 
 * @param {Char} self
 * @param {WeaponType} weaponType
 */
auto HasCharGotWeapon(CPed& ped, eWeaponType wtype) {
    return notsa::contains(ped.m_aWeapons, wtype, [](CWeapon& w) { return w.m_Type; });
}

/*
 * @opcode 04A5
 * @command GET_DEAD_CHAR_PICKUP_COORDS
 * @class World
 * @method GetDeadCharPickupCoords
 * @static
 * 
 * @brief Returns appropriate coordinates for creating a pickup by a dead character
 * 
 * @param {Char} char_
 * 
 * @returns {Vector} 
 */
auto GetDeadCharPickupCoords(CPed& ped) {
    CVector pos;
    ped.CreateDeadPedPickupCoors(pos);
    return pos;
}

auto IsPedInVehicleOfAppearance(CPed& ped, eVehicleAppearance appr) {
    return ped.IsInVehicle() && ped.m_pVehicle->GetVehicleAppearance() == appr;
}

/*
 * @opcode 047A
 * @command IS_CHAR_ON_ANY_BIKE
 * @class Char
 * @method IsOnAnyBike
 * 
 * @brief Returns true if the character is riding a bike
 * 
 * @param {Char} self
 */
auto IsCharOnAnyBike(CPed& ped) {
    return IsPedInVehicleOfAppearance(ped, VEHICLE_APPEARANCE_BIKE);
}

/*
 * @opcode 04A7
 * @command IS_CHAR_IN_ANY_BOAT
 * @class Char
 * @method IsInAnyBoat
 * 
 * @brief Returns true if the character is driving a boat
 * 
 * @param {Char} self
 */
auto IsCharInAnyBoat(CPed& ped) {
    return IsPedInVehicleOfAppearance(ped, VEHICLE_APPEARANCE_BOAT);
}

/*
 * @opcode 04A9
 * @command IS_CHAR_IN_ANY_HELI
 * @class Char
 * @method IsInAnyHeli
 * 
 * @brief Returns true if the character is flying a helicopter
 * 
 * @param {Char} self
 */
auto IsCharInAnyHeli(CPed& ped) {
    return IsPedInVehicleOfAppearance(ped, VEHICLE_APPEARANCE_HELI);
}

/*
 * @opcode 04AB
 * @command IS_CHAR_IN_ANY_PLANE
 * @class Char
 * @method IsInAnyPlane
 * 
 * @brief Returns true if the character is in a plane
 * 
 * @param {Char} self
 */
auto IsCharInAnyPlane(CPed& ped) {
    return IsPedInVehicleOfAppearance(ped, VEHICLE_APPEARANCE_PLANE);
}

/*
 * @opcode 04AD
 * @command IS_CHAR_IN_WATER
 * @class Char
 * @method IsInWater
 * 
 * @brief Returns true if the character is in water
 * 
 * @param {Char} self
 */
auto IsCharInWater(CPed* ped) {
    return ped && ped->physicalFlags.bSubmergedInWater;
}

/*
 * @opcode 04B8
 * @command GET_CHAR_WEAPON_IN_SLOT
 * @class Char
 * @method GetWeaponInSlot
 * 
 * @brief Returns the weapon type, ammo and model from the specified slot
 * 
 * @param {Char} self
 * @param {int} slot
 * 
 * @returns {WeaponType} weaponType, {int} weaponAmmo, {model_object} weaponModel
 */
auto GetCharWeaponInSlot(CPed& ped, int slot) {
    const auto& wep = ped.GetWeaponInSlot(slot - 1); // 1-based slot index
    return notsa::script::return_multiple(wep.m_Type, wep.m_TotalAmmo, CPickups::ModelForWeapon(wep.m_Type));
}

/*
 * @opcode 04C4
 * @command GET_OFFSET_FROM_CHAR_IN_WORLD_COORDS
 * @class Char
 * @method GetOffsetInWorldCoords
 * 
 * @brief Returns the coordinates of the character, with an offset
 * 
 * @param {Char} self
 * @param {Vector} offset
 * 
 * @returns {Vector} 
 */
auto GetOffsetFromCharInWorldCoords(CPed& ped, CVector offset) {
    return ped.GetMatrix().TransformPoint(offset);
}

/*
 * @opcode 04C5
 * @command HAS_CHAR_BEEN_PHOTOGRAPHED
 * @class Char
 * @method HasBeenPhotographed
 * 
 * @brief Returns true if the character has been photographed
 * 
 * @param {Char} self
 */
auto HasCharBeenPhotographed(CPed* ped) {
    if (ped) {
        if (ped->bHasBeenPhotographed) {
            ped->bHasBeenPhotographed = false;
            return true;
        }
    }
    return false;
}

/*
 * @opcode 04C8
 * @command IS_CHAR_IN_FLYING_VEHICLE
 * @class Char
 * @method IsInFlyingVehicle
 * 
 * @brief Returns true if the character is in a flying vehicle
 * 
 * @param {Char} self
 */
auto IsCharInFlyingVehicle(CPed& ped) {
    return IsCharInAnyHeli(ped) || IsCharInAnyPlane(ped);
}

/*
 * @opcode 04DD
 * @command GET_CHAR_ARMOUR
 * @class Char
 * @method GetArmor
 * 
 * @brief Returns the character's armor amount
 * 
 * @param {Char} self
 * 
 * @returns {int} armor
 */
auto GetCharArmour(CPed& ped) {
    return ped.m_fArmour;
}

// 0x48C12E - COMMAND_GET_CHAR_ARMOUR
/*
 * @opcode 04DE
 * @command SET_CHAR_ARMOUR
 */
void SetCharArmour(CPed& ped, float value) {
    ped.m_fArmour = value;
}

/*
 * @opcode 04F4
 * @command ATTACH_CHAR_TO_OBJECT
 * @class Char
 * @method AttachToObject
 * 
 * @brief Attaches the character to the specified object, in turret mode
 * 
 * @param {Char} self
 * @param {Object} handle
 * @param {Vector} offset
 * @param {Facing} heading
 * @param {float} headingRange
 * @param {WeaponType} weaponType
 */
auto AttachCharToObject(CPed& ped, CObject& obj, CVector offset, int32 orientation, float angleLimitDeg, eWeaponType wtype) {
    ped.AttachPedToEntity(&obj, offset, orientation, DegreesToRadians(angleLimitDeg), wtype);
}

/*
 * @opcode 051A
 * @command HAS_CHAR_BEEN_DAMAGED_BY_CHAR
 * @class Char
 * @method HasBeenDamagedByChar
 * 
 * @brief Returns true if the character has been hurt by the other character
 * 
 * @param {Char} self
 * @param {Char} handle
 */
auto HasCharBeenDamagedByChar(CPed* ped, CPed& byPed) {
    return ped && ped->m_pLastEntityDamage
        && (ped->m_pLastEntityDamage == &byPed || byPed.bInVehicle && ped->m_pLastEntityDamage == byPed.m_pVehicle);
}

/*
 * @opcode 051B
 * @command HAS_CHAR_BEEN_DAMAGED_BY_CAR
 * @class Char
 * @method HasBeenDamagedByCar
 * 
 * @brief Returns true if the char has been hurt by the specified vehicle
 * 
 * @param {Char} self
 * @param {Car} handle
 */
auto HasCharBeenDamagedByCar(CPed* ped, CVehicle& veh) {
    return ped && ped->m_pLastEntityDamage == &veh;
}

/*
 * @opcode 0547
 * @command IS_CHAR_TOUCHING_VEHICLE
 * @class Char
 * @method IsTouchingVehicle
 * 
 * @brief Returns true if the character is colliding with a car
 * 
 * @param {Char} self
 * @param {Car} handle
 */
auto IsCharTouchingVehicle(CPed& ped, CVehicle& withVeh) {
    return ped.bInVehicle
        ? ped.m_pVehicle->GetHasCollidedWith(&withVeh)
        : ped.GetHasCollidedWith(&withVeh);
}

/*
 * @opcode 054E
 * @command CLEAR_CHAR_LAST_DAMAGE_ENTITY
 * @class Char
 * @method ClearLastDamageEntity
 * 
 * @brief 
 * 
 * @param {Char} self
 */
auto ClearCharLastDamageEntity(CPed* ped) {
    if (ped) {
        ped->m_pLastEntityDamage = nullptr;
    }
}

auto CreateRandomCharInVehicle(CRunningScript& S, CVehicle& veh, bool asDriver, int32 psgrSeatIdx = -1) -> CPed& {
    const auto ped = CPopulation::AddPedInCar(&veh, asDriver, -1, asDriver ? 0 : psgrSeatIdx, false, false);
    ped->SetCharCreatedBy(PED_MISSION);
    if (!asDriver) {
        ped->GetTaskManager().SetTask(nullptr, TASK_PRIMARY_PRIMARY);
    }
    ped->bAllowMedicsToReviveMe = false;
    CPopulation::ms_nTotalMissionPeds++;
    if (S.m_UsesMissionCleanup) {
        CTheScripts::MissionCleanUp.AddEntityToList(*ped);
    }
    return *ped;
}

/*
 * @opcode 0560
 * @command CREATE_RANDOM_CHAR_AS_DRIVER
 * @class Char
 * @method CreateRandomAsDriver
 * 
 * @brief Creates a driver in the vehicle
 * 
 * @param {Car} vehicle
 * 
 * @returns {Char} handle
 */
auto CreateRandomCharAsDriver(CRunningScript& S, CVehicle& veh) -> CPed& {
    return CreateRandomCharInVehicle(S, veh, true);
}

/*
 * @opcode 0561
 * @command CREATE_RANDOM_CHAR_AS_PASSENGER
 * @class Char
 * @method CreateRandomAsPassenger
 * 
 * @brief Creates a random character in the passenger seat of the vehicle
 * 
 * @param {Car} vehicle
 * @param {SeatId} seat
 * 
 * @returns {Char} handle
 */
auto CreateRandomCharAsPassenger(CRunningScript& S, CVehicle& veh, int32 psgrSeatIdx) -> CPed& {
    return CreateRandomCharInVehicle(S, veh, false, psgrSeatIdx);
}

/*
 * @opcode 056C
 * @command IS_CHAR_IN_ANY_POLICE_VEHICLE
 * @class Char
 * @method IsInAnyPoliceVehicle
 * 
 * @brief Returns true if the character is driving a police vehicle
 * 
 * @param {Char} self
 */
auto IsCharInAnyPoliceVehicle(CPed& ped) {
    if (!ped.IsInVehicle()) {
        return false;
    }
    const auto veh = ped.m_pVehicle;
    return veh->IsLawEnforcementVehicle() && veh->m_nModelIndex != eModelID::MODEL_PREDATOR;
}

/*
 * @opcode 056D
 * @command DOES_CHAR_EXIST
 * @class Char
 * @method DoesExist
 * @static
 * 
 * @brief Returns true if the handle is a valid character handle
 * 
 * @param {Char} char_
 */
auto DoesCharExist(CPed* ped) {
    return ped != nullptr;
}

/*
 * @opcode 02E2
 * @command SET_CHAR_ACCURACY
 * @class Char
 * @method SetAccuracy
 * 
 * @brief Affects how often the character will hit the target when attacking with a weapon
 * 
 * @param {Char} self
 * @param {int} accuracy
 */
auto SetCharAccuracy(CPed& ped, uint8 accuracy) {
    ped.m_nWeaponAccuracy = accuracy;
}

void DoSetPedIsWaitingForCollision(CRunningScript& S, CPed& ped) {
    if (S.m_UsesMissionCleanup) {
        CWorld::Remove(&ped);
        ped.m_bIsStaticWaitingForCollision = true;
        CWorld::Add(&ped);
    }
}

/*
 * @opcode 0575
 * @command FREEZE_CHAR_POSITION_AND_DONT_LOAD_COLLISION
 * @class Char
 * @method FreezePositionAndDontLoadCollision
 * 
 * @brief 
 * 
 * @param {Char} self
 * @param {bool} state
 */
auto FreezeCharPositionAndDontLoadCollision(CRunningScript& S, CPed& ped, bool freeze) {
    ped.physicalFlags.bDontApplySpeed = freeze;
    if (freeze) {
        DoSetPedIsWaitingForCollision(S, ped);
    }
}

/*
 * @opcode 0588
 * @command SET_LOAD_COLLISION_FOR_CHAR_FLAG
 * @class Char
 * @method SetLoadCollisionFlag
 * 
 * @brief 
 * 
 * @param {Char} self
 * @param {bool} state
 */
auto SetLoadCollisionForCharFlag(CRunningScript& S, CPed& ped, bool loadCol) {
    ped.physicalFlags.b15 = !loadCol;
    if (loadCol) {
        DoSetPedIsWaitingForCollision(S, ped);
    } else if (ped.m_bIsStaticWaitingForCollision) {
        ped.m_bIsStaticWaitingForCollision = false;
        if (!ped.GetIsStatic()) {
            ped.AddToMovingList();
        }
    }
}

/*
 * @opcode 05E2
 * @command TASK_KILL_CHAR_ON_FOOT
 * @class Task
 * @method KillCharOnFoot
 * @static
 * 
 * @brief Makes a character attack another character on foot
 * 
 * @param {Char} killer
 * @param {Char} target
 */
auto TaskKillCharOnFoot(CRunningScript& S, eScriptCommands opcode, CPed& ped, CPed& target) {
    S.GivePedScriptedTask(
        &ped,
        new CTaskComplexKillPedOnFoot{ &target, -1, 0, 0, 0, true },
        (int32)(opcode)
    );
}

/*
 * @opcode 0602
 * @command IS_CHAR_IN_TAXI
 * @class Char
 * @method IsInTaxi
 * 
 * @brief Returns true if the character is driving a taxi
 * 
 * @param {Char} self
 */
auto IsCharInTaxi(CPed& ped) {
    if (ped.IsInVehicle()) {
        switch ((eModelID)(ped.m_pVehicle->m_nModelIndex)) {
        case MODEL_CABBIE:
        case MODEL_TAXI:
            return true;
        }
    }
    return false;
}

/*
 * @opcode 060A
 * @command LOAD_CHAR_DECISION_MAKER
 * @class DecisionMakerChar
 * @method Load
 * 
 * @brief Creates a decision maker with the specified type and adds it to mission cleanup list. Otherwise should be released with REMOVE_DECISION_MAKER
 * 
 * @param {DecisionMakerType} type
 * 
 * @returns {DecisionMakerChar} handle
 */
auto LoadCharDecisionMaker(CRunningScript& S, int32 type) { // TODO: return ScriptThing<CDecisionMaker>
    char pedDMName[1024];
    CDecisionMakerTypesFileLoader::GetPedDMName(type, pedDMName);
    const auto id = CDecisionMakerTypesFileLoader::LoadDecisionMaker(pedDMName, DEFAULT_DECISION_MAKER, S.m_UsesMissionCleanup);
    const auto handle = CTheScripts::GetNewUniqueScriptThingIndex(id, SCRIPT_THING_DECISION_MAKER);
    if (S.m_UsesMissionCleanup) {
        CTheScripts::MissionCleanUp.AddEntityToList(handle, MISSION_CLEANUP_ENTITY_TYPE_DECISION_MAKER);
    }
    return handle;
}

/*
 * @opcode 060B
 * @command SET_CHAR_DECISION_MAKER
 * @class Char
 * @method SetDecisionMaker
 * 
 * @brief Sets the decision maker for the character
 * 
 * @param {Char} self
 * @param {DecisionMakerCharTemplate} handleOrTemplate
 */
auto SetCharDecisionMaker(CPed& ped, int32 scriptHandleOfDM) { // TODO: Use `ScriptThing<CDecisionMaker>` instead of `int32` for `scriptHandleOfDM`
    ped.GetIntelligence()->SetPedDecisionMakerType(
        scriptHandleOfDM == -1
            ? -1
            : CTheScripts::GetActualScriptThingIndex(scriptHandleOfDM, SCRIPT_THING_DECISION_MAKER)
    );
}

/*
 * @opcode 0611
 * @command IS_CHAR_PLAYING_ANIM
 * @class Char
 * @method IsPlayingAnim
 * 
 * @brief Returns true if character is performing the specified animation
 * 
 * @param {Char} self
 * @param {string} animationName
 */
auto IsCharPlayingAnim(CPed& ped, const char* animName) {
    return RpAnimBlendClumpGetAssociation(ped.m_pRwClump, animName) != nullptr;
}

/*
 * @opcode 0612
 * @command SET_CHAR_ANIM_PLAYING_FLAG
 * @class Char
 * @method SetAnimPlayingFlag
 * 
 * @brief Sets whether the animation is playing
 * 
 * @param {Char} self
 * @param {string} animationName
 * @param {bool} flag
 */
auto SetCharAnimPlayingFlag(CPed& ped, const char* animName, bool started) {
    if (const auto anim = RpAnimBlendClumpGetAssociation(ped.m_pRwClump, animName)) {
        anim->SetFlag(ANIMATION_IS_PLAYING, started);
    }
}

/*
 * @opcode 0613
 * @command GET_CHAR_ANIM_CURRENT_TIME
 * @class Char
 * @method GetAnimCurrentTime
 * 
 * @brief Returns the progress of the animation on the char, ranging from 0.0 to 1.0
 * 
 * @param {Char} self
 * @param {string} animationName
 * 
 * @returns {float} time
 */
auto GetCharAnimCurrentTime(CPed& ped, const char* animName) {
    if (const auto anim = RpAnimBlendClumpGetAssociation(ped.m_pRwClump, animName)) {
        return anim->m_CurrentTime / anim->m_BlendHier->m_fTotalTime;
    }
    return 0.f;
}

/*
 * @opcode 0614
 * @command SET_CHAR_ANIM_CURRENT_TIME
 * @class Char
 * @method SetAnimCurrentTime
 * 
 * @brief Sets how far through the animation the character is, with 1
 * 
 * @param {Char} self
 * @param {string} animationName
 * @param {float} time
 */
auto SetCharAnimCurrentTime(CPed& ped, const char* animName, float progress) {
    if (const auto anim = RpAnimBlendClumpGetAssociation(ped.m_pRwClump, animName)) {
        anim->SetCurrentTime(progress * anim->m_BlendHier->m_fTotalTime);
    }
}

// GET_CHAR_ANIM_TOTAL_TIME (In seconds)
/*
 * @opcode 061A
 * @command GET_CHAR_ANIM_TOTAL_TIME
 * @class Char
 * @method GetAnimTotalTime
 * 
 * @brief Returns a float of the length of the animation in milliseconds
 * 
 * @param {Char} self
 * @param {string} animationName
 * 
 * @returns {float} totalTime
 */
auto GetCharAnimTotalTime(CPed& ped, const char* animName) {
    if (const auto anim = RpAnimBlendClumpGetAssociation(ped.m_pRwClump, animName)) {
        return anim->m_BlendHier->m_fTotalTime * 1000.f;
    }
    return 0.f;
}

/*
 * @opcode 0621
 * @command CREATE_CHAR_AT_ATTRACTOR
 * @class Char
 * @method CreateAtAttractor
 * 
 * @brief 
 * 
 * @param {PedType} pedType
 * @param {model_char} modelId
 * @param {Attractor} attractor
 * @param {TaskCommand} task
 * 
 * @returns {Char} handle
 */
//auto CreateCharAtAttractor(CRunningScript& S, ePedType pedType, eModelID pedModel, int32 taskType, C2dEffectPedAttractor* attractor) -> CPed* {
    /*
    if (!attractor) {
        return nullptr;
    }

    auto& ped = CreatePed(S, pedType, pedModel);

    CPedAttractorPedPlacer::PlacePedAtEffect(*attractor, nullptr, &ped, 0.01f);
    CTheScripts::ClearSpaceForMissionEntity(ped.GetPosition(), &ped);
    if (S.m_bUseMissionCleanup) {
        ped.m_bIsStaticWaitingForCollision = true;
    }
    CWorld::Add(&ped);

    // Set the task specified 
    
    if (const auto task = [&] {
        switch ((eScriptCommands)(taskType)) { // Can't take this as an input argument, as it will always put in the current command instead of reading it off the stack
        case COMMAND_TASK_STAND_STILL:
            return new CTaskSimpleStandStill{}
        }
    }()) {

    }

    // TODO: Code is so far correct, but we're missing the stub for `CTaskComplexUseEffect`
    */
//}

/*
 * @opcode 0634
 * @command TASK_KILL_CHAR_ON_FOOT_WHILE_DUCKING
 * @class Task
 * @method KillCharOnFootWhileDucking
 * @static
 * 
 * @brief 
 * 
 * @param {Char} char_
 * @param {Char} target
 * @param {int} flags
 * @param {int} actionDelay
 * @param {int} actionChance
 */
auto TaskKillCharOnFootWhileDucking(eScriptCommands command, CRunningScript& S, CPed& ped, CPed& target, int32 flags, int32 actionDelay, int32 actionChance) {
    S.GivePedScriptedTask(
        &ped,
        new CTaskComplexKillPedOnFoot{
            &target,
            -1,
            flags,
            actionDelay,
            actionChance,
            true
        },
        command
    );
}

/*
 * @opcode 0639
 * @command TASK_TURN_CHAR_TO_FACE_CHAR
 * @class Task
 * @method TurnCharToFaceChar
 * @static
 * 
 * @brief Makes a character face another character
 * 
 * @param {Char} char_
 * @param {Char} target
 */
auto TaskTurnCharToFaceChar(eScriptCommands command, CRunningScript& S, int32 pedHandle, CPed& target) {
    S.GivePedScriptedTask(
        pedHandle,
        new CTaskComplexTurnToFaceEntityOrCoord{ &target },
        command
    );
}

/*
 * @opcode 0642
 * @command IS_CHAR_AT_SCRIPTED_ATTRACTOR
 * @class Char
 * @method IsAtScriptedAttractor
 * 
 * @brief 
 * 
 * @param {Char} self
 * @param {Attractor} handle
 */
auto IsCharAtScriptedAttractor(CPed* ped, C2dEffectPedAttractor* attractor) {
    if (!attractor) {
        return false;
    }
    const auto pedUsingAttractor = GetPedAttractorManager()->GetPedUsingEffect(attractor);
    return ped
        ? pedUsingAttractor == ped
        : pedUsingAttractor != nullptr;
}

/*
 * @opcode 0665
 * @command GET_CHAR_MODEL
 * @class Char
 * @method GetModel
 * 
 * @brief Returns the characters model
 * 
 * @param {Char} self
 * 
 * @returns {int} modelId
 */
auto GetCharModel(CPed& ped) {
    return (eModelID)(ped.m_nModelIndex);
}

/*
 * @opcode 01B9
 * @command SET_CURRENT_CHAR_WEAPON
 * @class Char
 * @method SetCurrentWeapon
 * 
 * @brief Sets the character's currently held weapon
 * 
 * @param {Char} self
 * @param {WeaponType} weaponType
 */
void SetCurrentCharWeapon(CPed& ped, eWeaponType weaponType) {
    for (auto&& [slot, weapon] : rngv::enumerate(ped.m_aWeapons)) {
        if (weapon.m_Type != weaponType)
            continue;

        if (ped.IsPlayer()) {
            ped.AsPlayer()->GetPlayerData()->m_nChosenWeapon = slot;
        } else {
            ped.SetCurrentWeapon(slot);
        }
    }
}

/*
 * @opcode 01C2
 * @command MARK_CHAR_AS_NO_LONGER_NEEDED
 * @class Char
 * @method MarkAsNoLongerNeeded
 * 
 * @brief Allows the character to be deleted by the game if necessary, and also removes them from the mission cleanup list, if applicable
 * 
 * @param {Char} self
 */
void MarkCharAsNoLongerNeeded(CRunningScript& S, int32 handle) { // TODO: Some way to get a CPed* and it's handle too (As this function seems to be called even if the handle is not pointing to a ped anymore)
    const auto ped = GetPedPool()->GetAtRef(handle); // This might be null, but we need the handle even if it is, so we can't take `CPed*` either...
    CTheScripts::CleanUpThisPed(ped);
    if (S.m_UsesMissionCleanup) {
        CTheScripts::MissionCleanUp.RemoveEntityFromList(handle, MISSION_CLEANUP_ENTITY_TYPE_PED);
    }
}

/*
 * @opcode 06AC
 * @command GET_CHAR_SPEED
 * @class Char
 * @method GetSpeed
 * 
 * @brief Returns the char's movement speed
 * 
 * @param {Char} self
 * 
 * @returns {float} speed
 */
float GetCharSpeed(CPed& ped) {
    return ped.GetMoveSpeed().Magnitude() * 50.0f;
}

// IS_CHAR_IN_SEARCH_LIGHT
/*
 * @opcode 06B7
 * @command IS_CHAR_IN_SEARCHLIGHT
 * @class Searchlight
 * @method IsCharIn
 * 
 * @brief Returns true if the searchlight has spotted the char
 * 
 * @param {Searchlight} self
 * @param {Char} handle
 */
bool IsCharInSearchlight(uint32 searchLightIdx, CPed& ped) {
    return CSearchLight::IsSpottedEntity(searchLightIdx, ped);
}

/*
 * @opcode 06C9
 * @command REMOVE_CHAR_FROM_GROUP
 * @class Char
 * @method RemoveFromGroup
 * 
 * @brief Removes the character from their current group
 * 
 * @param {Char} self
 */
void RemoveCharFromGroup(CPed& ped) {
    if (auto pedGroup = ped.GetGroup(); pedGroup && !pedGroup->GetMembership().IsLeader(&ped)) {
        pedGroup->GetMembership().RemoveMember(&ped);
        pedGroup->Process();
    }
}

/*
 * @opcode 06A7
 * @command ATTACH_CHAR_TO_BIKE
 * @class Char
 * @method AttachToBike
 * 
 * @brief 
 * 
 * @param {Char} self
 * @param {Car} vehicle
 * @param {Vector} offset
 * @param {Facing} heading
 * @param {float} headingRange
 * @param {float} pitchRange
 * @param {WeaponType} weaponType
 */
void AttachCharToBike(CPed& ped, CVehicle& bike, CVector posn, uint16 heading, float headingLimit, float verticalLimit, eWeaponType weapon) {
    // turret on car?
    ped.AttachPedToBike(&bike, posn, heading, DegreesToRadians(headingLimit), DegreesToRadians(verticalLimit), weapon);
}

/*
 * @opcode 0811
 * @command GET_CAR_CHAR_IS_USING
 * @class Char
 * @method GetCarIsUsing
 * 
 * @brief Stores a handle for the vehicle the character is in or entering (alts: 00D9,03C0,0484)
 * 
 * @param {Char} self
 * 
 * @returns {Car} handle
 */
CVehicle* GetCarCharIsUsing(CPed& ped) {
    if (ped.bInVehicle) {
        return ped.m_pVehicle;
    }
    if (const auto task = ped.GetTaskManager().Find<CTaskComplexEnterCarAsDriver>(false)) {
        return task->GetTargetCar();
    }
    if (const auto task = ped.GetTaskManager().Find<CTaskComplexEnterCarAsPassenger>(false)) {
        return task->GetTargetCar();
    }
    return nullptr;
}

/*
 * @opcode 094F
 * @command ENABLE_CHAR_SPEECH
 * @class Char
 * @method EnableSpeech
 * 
 * @brief Enables pain audio if it was disabled using 094E
 * 
 * @param {Char} self
 */
void EnableCharSpeech(CPed& ped) {
    ped.EnablePedSpeech();
}

/*
 * @opcode 094E
 * @command DISABLE_CHAR_SPEECH
 * @class Char
 * @method DisableSpeech
 * 
 * @brief Prevents any character speech from playing
 * 
 * @param {Char} self
 * @param {bool} stopNow
 */
void DisableCharSpeech(CPed& ped, bool stopCurrentSpeech) {
    ped.DisablePedSpeech(stopCurrentSpeech);
}

/*
 * @opcode 09B6
 * @command SET_CHAR_WANTED_BY_POLICE
 * @class Char
 * @method SetWantedByPolice
 * 
 * @brief Sets whether police should chase the character
 * 
 * @param {Char} self
 * @param {bool} state
 */
void SetCharWantedByPolice(CPed& ped, bool state) {
    ped.bWantedByPolice = state;
}

/*
 * @opcode 09B5
 * @command SET_CHAR_SIGNAL_AFTER_KILL
 * @class Char
 * @method SetSignalAfterKill
 * 
 * @brief Sets whether the character signals after killing
 * 
 * @param {Char} self
 * @param {bool} state
 */
void SetCharSignalAfterKill(CPed& ped, bool state) {
    ped.bSignalAfterKill = state;
}

/*
 * @opcode 09BC
 * @command SET_CHAR_COORDINATES_DONT_WARP_GANG_NO_OFFSET
 * @class Char
 * @method SetCoordinatesDontWarpGangNoOffset
 * 
 * @brief This command is a combination of 0972 and 08C7
 * 
 * @param {Char} self
 * @param {Vector} 
 */
void SetCharCoordinatesDontWarpGangNoOffset(CRunningScript& S, CPed& ped, CVector posn) {
    S.SetCharCoordinates(ped, posn, false, false);
}

/*
 * @opcode 09C5
 * @command IS_CHAR_USING_MAP_ATTRACTOR
 * @class Char
 * @method IsUsingMapAttractor
 * 
 * @brief Returns true if the character is using a map attractor
 * 
 * @param {Char} self
 */
bool IsCharUsingMapAttractor(CPed& ped) {
    return GetPedAttractorManager()->IsPedRegisteredWithEffect(&ped);
}

// todo: move that to somewhere else
eTargetDoor ComputeTargetDoorToExit(const CVehicle& vehicle, const CPed& ped) {
    return plugin::CallAndReturn<eTargetDoor, 0x64F110, const CVehicle&, const CPed&>(vehicle, ped);
}

/*
 * @opcode 09C9
 * @command REMOVE_CHAR_FROM_CAR_MAINTAIN_POSITION
 * @class Char
 * @method RemoveFromCarMaintainPosition
 * 
 * @brief Removes the character from the vehicle
 * 
 * @param {Char} self
 * @param {Car} vehicle
 */
void RemoveCharFromCarMaintainPosition(CPed& ped, CVehicle& vehicle) {
    const auto pedPos = ped.GetPosition();
    CTaskSimpleCarSetPedOut task(&vehicle, ComputeTargetDoorToExit(vehicle, ped), false);
    task.ProcessPed(&ped);
    ped.SetPosn(pedPos); // ?
}

/*
 * @opcode 09D5
 * @command SET_CHAR_SAY_CONTEXT_IMPORTANT
 * @class Char
 * @method SetSayContextImportant
 * 
 * @brief 
 * 
 * @param {Char} self
 * @param {SpeechId} phrase
 * @param {bool} overrideSilence
 * @param {bool} ignoreMute
 * @param {bool} frontEnd
 * 
 * @returns {int} saidVariant
 */
int16 SetCharSayContextImportant(CPed& ped, uint16 phraseId, bool overrideSilence, bool isForceAudible, bool isFrontEnd) {
    return ped.Say((eGlobalSpeechContext)phraseId, 0u, 1.0f, overrideSilence, isForceAudible, isFrontEnd);
}

/*
 * @opcode 09D6
 * @command SET_CHAR_SAY_SCRIPT
 * @class Char
 * @method SetSayScript
 * 
 * @brief 
 * 
 * @param {Char} self
 * @param {int} _p2
 * @param {bool} _p3
 * @param {bool} _p4
 * @param {bool} _p5
 */
void SetCharSayScript(CPed& ped, eAudioEvents scriptID, bool overrideSilence, bool isForceAudible, bool isFrontEnd) {
    ped.SayScript(scriptID, overrideSilence, isForceAudible, isFrontEnd);
}

// IS_CHAR_GETTING_IN_INTO_A_CAR
/*
 * @opcode 09DE
 * @command IS_CHAR_GETTING_IN_TO_A_CAR
 * @class Char
 * @method IsGettingInToACar
 * 
 * @brief Returns true if the character is entering a car, but is not in the car
 * 
 * @param {Char} self
 */
bool IsCharGettingInToACar(CPed& ped) {
    return ped.GetTaskManager().FindActiveTaskFromList({
        TASK_COMPLEX_ENTER_CAR_AS_DRIVER,
        TASK_COMPLEX_ENTER_CAR_AS_PASSENGER,
        TASK_COMPLEX_GO_TO_CAR_DOOR_AND_STAND_STILL
    });
}

/*
 * @opcode 09E8
 * @command GET_CHAR_AREA_VISIBLE
 * @class Char
 * @method GetAreaVisible
 * 
 * @brief Returns the interior ID that the character is in
 * 
 * @param {Char} self
 * 
 * @returns {int} areaId
 */
uint32 GetCharAreaVisible(CPed& ped) {
    return ped.GetAreaCode() != eAreaCodes::AREA_CODE_NORMAL_WORLD;
}

/*
 * @opcode 09ED
 * @command HAS_CHAR_SPOTTED_CHAR_IN_FRONT
 * @class Char
 * @method HasSpottedCharInFront
 * 
 * @brief Returns true if the character can see the other character in front of them
 * 
 * @param {Char} self
 * @param {Char} handle
 */
bool HasCharSpottedCharInFront(CPed& ped, CPed& other) {
    return ped.OurPedCanSeeThisEntity(&other, true);
}

/*
 * @opcode 0A09
 * @command SHUT_CHAR_UP_FOR_SCRIPTED_SPEECH
 * @class Char
 * @method ShutUpForScriptedSpeech
 * 
 * @brief Works similar to 0489, but mutes more things, including ambient speeches (needs confirming)
 * 
 * @param {Char} self
 * @param {bool} state
 */
void ShutCharUpForScriptedSpeech(CPed* ped, bool disable) {
    if (!ped)
        return;

    ped->DisablePedSpeechForScriptSpeech(disable);
}

/*
 * @opcode 0A1B
 * @command IS_CHAR_TOUCHING_CHAR
 * @class Char
 * @method IsTouchingChar
 * 
 * @brief Returns true if the character is touching the other character
 * 
 * @param {Char} self
 * @param {Char} other
 */
bool IsCharTouchingChar(CPed& ped, CPed& other) {
    const auto GetRelevantEntity = [](CPed& p) -> CPhysical* {
        return p.IsInVehicle()
            ? p.m_pVehicle->AsPhysical()
            : p.AsPhysical();
    };
    return GetRelevantEntity(ped)->GetHasCollidedWith(GetRelevantEntity(other));
}

/*
 * @opcode 0A32
 * @command IS_CHAR_ATTACHED_TO_ANY_CAR
 * @class Char
 * @method IsAttachedToAnyCar
 * 
 * @brief Returns true if the char is turreted on any vehicle
 * 
 * @param {Char} self
 */
bool IsCharAttachedToAnyCar(CPed* ped) {
    return ped && ped->GetIsTypeVehicle();
}

/*
 * @opcode 0A33
 * @command STORE_CAR_CHAR_IS_ATTACHED_TO_NO_SAVE
 * @class Char
 * @method StoreCarIsAttachedToNoSave
 * 
 * @brief Returns the vehicle the character is attached to
 * 
 * @param {Char} self
 * 
 * @returns {Car} handle
 */
CVehicle* StoreCarCharIsAttachedToNoSave(CPed* ped) {
    if (!ped || !ped->GetIsTypeVehicle() || !ped->m_pAttachedTo) {
        return nullptr;
    }
    return ped->m_pAttachedTo->AsVehicle();
}
};

/*
 * @opcode 0792
 * @command CLEAR_CHAR_TASKS_IMMEDIATELY
 * @class Char
 * @method ClearTasksImmediately
 * 
 * @brief Clears all the characters tasks immediately, resetting the character to an idle state
 * 
 * @param {Char} self
 */
void ClearCharTasksImmediately(CPed& ped) {
    ped.GetIntelligence()->FlushImmediately(true);
}

/*
 * @opcode 07A0
 * @command PERFORM_SEQUENCE_TASK_FROM_PROGRESS
 * @class Char
 * @method PerformSequenceFromProgress
 * 
 * @brief 
 * 
 * @param {Char} self
 * @param {Sequence} sequence
 * @param {int} startTaskIndex
 * @param {int} endTaskIndex
 */
//void PerformSequenceTaskFromProgress(CPed& self, CSequence& sequence, int32 startTaskIndex, int32 endTaskIndex) {
    //NOTSA_UNREACHABLE("Not implemented");
//}

/*
 * @opcode 009A
 * @command CREATE_CHAR
 * @class Char
 * @method Create
 * 
 * @brief Creates a character at the specified location, with the specified model and pedtype
 * 
 * @param {PedType} pedType
 * @param {model_char} modelId
 * @param {Vector} 
 * 
 * @returns {Char} handle
 */
//auto CreateChar(ePedType pedType, eModelID modelId, CVector& ) {
    //NOTSA_UNREACHABLE("Not implemented");
//}

/*
 * @opcode 01B2
 * @command GIVE_WEAPON_TO_CHAR
 * @class Char
 * @method GiveWeapon
 * 
 * @brief Gives the character the weapon with the specified amount of ammo
 * 
 * @param {Char} self
 * @param {WeaponType} weaponType
 * @param {int} ammo
 */
//void GiveWeaponToChar(CPed& self, eWeaponType weaponType, int32 ammo) {
    //NOTSA_UNREACHABLE("Not implemented");
//}

/*
 * @opcode 01C8
 * @command CREATE_CHAR_AS_PASSENGER
 * @class Char
 * @method CreateAsPassenger
 * 
 * @brief Creates a character with the specified model in the passenger seat of the vehicle
 * 
 * @param {Car} vehicle
 * @param {PedType} pedType
 * @param {model_char} modelId
 * @param {SeatId} seat
 * 
 * @returns {Char} handle
 */
//auto CreateCharAsPassenger(CVehicle& vehicle, ePedType pedType, eModelID modelId, eSeatId seat) {
    //NOTSA_UNREACHABLE("Not implemented");
//}

/*
 * @opcode 062E
 * @command GET_SCRIPT_TASK_STATUS
 * @class Char
 * @method GetScriptTaskStatus
 * 
 * @brief Returns the status of the specified script task of the character
 * 
 * @param {Char} self
 * @param {TaskCommand} task
 * 
 * @returns {TaskStatus} status
 */
//auto GetScriptTaskStatus(CPed& self, eTaskCommand task) {
    //NOTSA_UNREACHABLE("Not implemented");
//}

/*
 * @opcode 07CB
 * @command LISTEN_TO_PLAYER_GROUP_COMMANDS
 * @class Char
 * @method ListenToPlayerGroupCommands
 * 
 * @brief 
 * 
 * @param {Char} self
 * @param {bool} state
 */
//void ListenToPlayerGroupCommands(CPed& self, bool state) {
    //NOTSA_UNREACHABLE("Not implemented");
//}

/*
 * @opcode 0376
 * @command CREATE_RANDOM_CHAR
 * @class Char
 * @method CreateRandom
 * 
 * @brief Creates a character with a randomised model and pedtype at the specified coordinates
 * 
 * @param {Vector} 
 * 
 * @returns {Char} handle
 */
//auto CreateRandomChar(CVector& ) {
    //NOTSA_UNREACHABLE("Not implemented");
//}

/*
 * @opcode 06EF
 * @command IS_GROUP_LEADER
 * @class Char
 * @method IsGroupLeader
 * 
 * @brief Returns true if the character is the leader of the specified group
 * 
 * @param {Char} self
 * @param {Group} handle
 */
//bool IsGroupLeader(CPed& self, CGroup& handle) {
    //NOTSA_UNREACHABLE("Not implemented");
//}

/*
 * @opcode 07A4
 * @command GET_SEQUENCE_PROGRESS_RECURSIVE
 * @class Char
 * @method GetSequenceProgressRecursive
 * 
 * @brief 
 * 
 * @param {Char} self
 * 
 * @returns {int} _p2, {int} _p3
 */
//auto GetSequenceProgressRecursive(CPed& self) {
    //NOTSA_UNREACHABLE("Not implemented");
//}

/*
 * @opcode 060F
 * @command SET_SENSE_RANGE
 * @class Char
 * @method SetSenseRange
 * @static
 * 
 * @brief Sets the seeing and hearing range for the specified character or for all mission characters when handle is -1
 * 
 * @param {Char} char_
 * @param {float} range
 */
//void SetSenseRange(CPed* char_, float range) {
    //NOTSA_UNREACHABLE("Not implemented");
//}

/*
 * @opcode 06FF
 * @command ARE_ANY_CHARS_NEAR_CHAR
 * @class Char
 * @method IsNearAnyChars
 * 
 * @brief Returns true if any characters are within range of the character
 * 
 * @param {Char} self
 * @param {float} radius
 */
//bool AreAnyCharsNearChar(CPed& self, float radius) {
    //NOTSA_UNREACHABLE("Not implemented");
//}

/*
 * @opcode 07A1
 * @command SET_NEXT_DESIRED_MOVE_STATE
 * @class Char
 * @method SetNextDesiredMoveState
 * @static
 * 
 * @brief Sets how the character chooses to go to their destination in the next task without a parameter specifying this
 * 
 * @param {MoveState} moveState
 */
//void SetNextDesiredMoveState(eMoveState moveState) {
    //NOTSA_UNREACHABLE("Not implemented");
//}

/*
 * @opcode 089F
 * @command GET_PED_TYPE
 * @class Char
 * @method GetPedType
 * 
 * @brief Gets the ped type of the character
 * 
 * @param {Char} self
 * 
 * @returns {PedType} pedType
 */
//auto GetPedType(CPed& self) {
    //NOTSA_UNREACHABLE("Not implemented");
//}

/*
 * @opcode 01C5
 * @command DONT_REMOVE_CHAR
 * @class Char
 * @method DontRemove
 * 
 * @brief Removes the character from the mission cleanup list, preventing it from being deleted when the mission ends
 * 
 * @param {Char} self
 */
//void DontRemoveChar(CPed& self) {
    //NOTSA_UNREACHABLE("Not implemented");
//}

/*
 * @opcode 07FE
 * @command GIVE_MELEE_ATTACK_TO_CHAR
 * @class Char
 * @method GiveMeleeAttack
 * 
 * @brief Sets the specified characters fighting style and moves
 * 
 * @param {Char} self
 * @param {FightStyle} style
 * @param {FightMoves} move
 */
//void GiveMeleeAttackToChar(CPed& self, eFightStyle style, eFightMoves move) {
    //NOTSA_UNREACHABLE("Not implemented");
//}

/*
 * @opcode 009B
 * @command DELETE_CHAR
 * @class Char
 * @method Delete
 * 
 * @brief Removes the character from the game and mission cleanup list, freeing game memory
 * 
 * @param {Char} self
 */
//void DeleteChar(CPed& self) {
    //NOTSA_UNREACHABLE("Not implemented");
//}

/*
 * @opcode 06EE
 * @command IS_GROUP_MEMBER
 * @class Char
 * @method IsGroupMember
 * 
 * @brief Returns true if the character is a member of the specified group
 * 
 * @param {Char} self
 * @param {Group} handle
 */
//bool IsGroupMember(CPed& self, CGroup& handle) {
    //NOTSA_UNREACHABLE("Not implemented");
//}

/*
 * @opcode 0646
 * @command GET_SEQUENCE_PROGRESS
 * @class Char
 * @method GetSequenceProgress
 * 
 * @brief Gets the characters task sequence progress, as started by 0618
 * 
 * @param {Char} self
 * 
 * @returns {int} progress
 */
//auto GetSequenceProgress(CPed& self) {
    //NOTSA_UNREACHABLE("Not implemented");
//}

/*
 * @opcode 09A1
 * @command DROP_SECOND_OBJECT
 * @class Char
 * @method DropSecondObject
 * 
 * @brief 
 * 
 * @param {Char} self
 * @param {bool} state
 */
//void DropSecondObject(CPed& self, bool state) {
    //NOTSA_UNREACHABLE("Not implemented");
//}

/*
 * @opcode 0245
 * @command SET_ANIM_GROUP_FOR_CHAR
 * @class Char
 * @method SetAnimGroup
 * 
 * @brief Sets the animation group for the character
 * 
 * @param {Char} self
 * @param {AnimGroup} animGroup
 */
//void SetAnimGroupForChar(CPed& self, eAnimGroup animGroup) {
    //NOTSA_UNREACHABLE("Not implemented");
//}

/*
 * @opcode 0618
 * @command PERFORM_SEQUENCE_TASK
 * @class Char
 * @method PerformSequence
 * 
 * @brief Assigns the character to the specified action sequence
 * 
 * @param {Char} self
 * @param {Sequence} sequence
 */
//void PerformSequenceTask(CPed& self, CSequence& sequence) {
    //NOTSA_UNREACHABLE("Not implemented");
//}

/*
 * @opcode 0A28
 * @command SET_SWIM_SPEED
 * @class Char
 * @method SetSwimSpeed
 * 
 * @brief Sets the speed that the character swims at, changing their swimming animation speed
 * 
 * @param {Char} self
 * @param {float} speed
 */
//void SetSwimSpeed(CPed& self, float speed) {
    //NOTSA_UNREACHABLE("Not implemented");
//}

/*
 * @opcode 09F4
 * @command IGNORE_HEIGHT_DIFFERENCE_FOLLOWING_NODES
 * @class Char
 * @method IgnoreHeightDifferenceFollowingNodes
 * 
 * @brief 
 * 
 * @param {Char} self
 * @param {bool} state
 */
//void IgnoreHeightDifferenceFollowingNodes(CPed& self, bool state) {
    //NOTSA_UNREACHABLE("Not implemented");
//}

/*
 * @opcode 0114
 * @command ADD_AMMO_TO_CHAR
 * @class Char
 * @method AddAmmo
 * 
 * @brief Adds the specified amount of ammo to the character's weapon, if the character has the weapon
 * 
 * @param {Char} self
 * @param {WeaponType} weaponType
 * @param {int} ammo
 */
//void AddAmmoToChar(CPed& self, eWeaponType weaponType, int32 ammo) {
    //NOTSA_UNREACHABLE("Not implemented");
//}

/*
 * @opcode 09F6
 * @command SET_CHAR_GET_OUT_UPSIDE_DOWN_CAR
 * @class Char
 * @method SetGetOutUpsideDownCar
 * 
 * @brief Controls whether the character will try to exit an upside-down car until it is on fire
 * 
 * @param {Char} self
 * @param {bool} state
 */
//void SetCharGetOutUpsideDownCar(CPed& self, bool state) {
    //NOTSA_UNREACHABLE("Not implemented");
//}

/*
 * @opcode 070B
 * @command DROP_OBJECT
 * @class Char
 * @method DropObject
 * 
 * @brief 
 * 
 * @param {Char} self
 * @param {bool} state
 */
//void DropObject(CPed& self, bool state) {
    //NOTSA_UNREACHABLE("Not implemented");
//}

/*
 * @opcode 035F
 * @command ADD_ARMOUR_TO_CHAR
 * @class Char
 * @method AddArmor
 * 
 * @brief Increases the character's armor by the specified value to the maximum of 100
 * 
 * @param {Char} self
 * @param {int} amount
 */
//void AddArmourToChar(CPed& self, int32 amount) {
    //NOTSA_UNREACHABLE("Not implemented");
//}

/*
 * @opcode 0A27
 * @command SET_DEATH_WEAPONS_PERSIST
 * @class Char
 * @method SetDeathWeaponsPersist
 * 
 * @brief Prevents pickups, which are created when this character dies, from disappearing until picked up by the player
 * 
 * @param {Char} self
 * @param {bool} state
 */
//void SetDeathWeaponsPersist(CPed& self, bool state) {
    //NOTSA_UNREACHABLE("Not implemented");
//}

/*
 * @opcode 0887
 * @command SET_HEADING_LIMIT_FOR_ATTACHED_CHAR
 * @class Char
 * @method SetHeadingLimitForAttached
 * 
 * @brief Sets the heading limit for a character attached to an object or vehicle
 * 
 * @param {Char} self
 * @param {Facing} heading
 * @param {float} headingRange
 */
//void SetHeadingLimitForAttachedChar(CPed& self, eFacing heading, float headingRange) {
    //NOTSA_UNREACHABLE("Not implemented");
//}

/*
 * @opcode 0503
 * @command CREATE_SWAT_ROPE
 * @class Char
 * @method CreateSwatRope
 * 
 * @brief Creates a character descending from a rope
 * 
 * @param {PedType} pedType
 * @param {model_char} modelId
 * @param {Vector} 
 * 
 * @returns {Char} handle
 */
//auto CreateSwatRope(ePedType pedType, eModelID modelId, CVector& ) {
    //NOTSA_UNREACHABLE("Not implemented");
//}

/*
 * @opcode 0555
 * @command REMOVE_WEAPON_FROM_CHAR
 * @class Char
 * @method RemoveWeapon
 * 
 * @brief Removes the weapon from the character
 * 
 * @param {Char} self
 * @param {WeaponType} weaponType
 */
//void RemoveWeaponFromChar(CPed& self, eWeaponType weaponType) {
    //NOTSA_UNREACHABLE("Not implemented");
//}

/*
 * @opcode 0648
 * @command SET_FOLLOW_NODE_THRESHOLD_DISTANCE
 * @class Char
 * @method SetFollowNodeThresholdDistance
 * 
 * @brief Sets the range within which the char responds to events
 * 
 * @param {Char} self
 * @param {float} range
 */
//void SetFollowNodeThresholdDistance(CPed& self, float range) {
    //NOTSA_UNREACHABLE("Not implemented");
//}

/*
 * @opcode 046D
 * @command GET_NUMBER_OF_FOLLOWERS
 * @class Char
 * @method GetNumberOfFollowers
 * 
 * @brief Returns the number of members which are in a group of the character (01DE)
 * 
 * @param {Char} self
 * 
 * @returns {int} number
 */
//auto GetNumberOfFollowers(CPed& self) {
    //NOTSA_UNREACHABLE("Not implemented");
//}

/*
 * @opcode 0851
 * @command DAMAGE_CHAR
 * @class Char
 * @method Damage
 * 
 * @brief Decreases the characters health
 * 
 * @param {Char} self
 * @param {int} amount
 * @param {bool} damageArmour
 */
//void DamageChar(CPed& self, int32 amount, bool damageArmour) {
    //NOTSA_UNREACHABLE("Not implemented");
//}

/*
 * @opcode 074E
 * @command SET_INFORM_RESPECTED_FRIENDS
 * @class Char
 * @method SetInformRespectedFriends
 * 
 * @brief 
 * 
 * @param {Char} self
 * @param {float} radius
 * @param {int} _p3
 */
//void SetInformRespectedFriends(CPed& self, float radius, int32 _p3) {
    //NOTSA_UNREACHABLE("Not implemented");
//}

/*
 * @opcode 0647
 * @command CLEAR_LOOK_AT
 * @class Char
 * @method ClearLookAt
 * 
 * @brief Clears the char's look task, making them stop looking at whatever they were assigned to look at
 * 
 * @param {Char} self
 */
//void ClearLookAt(CPed& self) {
    //NOTSA_UNREACHABLE("Not implemented");
//}

void notsa::script::commands::character::RegisterHandlers() {
    REGISTER_COMMAND_HANDLER_BEGIN("Char");
    //REGISTER_COMMAND_HANDLER(COMMAND_ADD_AMMO_TO_CHAR, AddAmmoToChar);
    //REGISTER_COMMAND_HANDLER(COMMAND_ADD_ARMOUR_TO_CHAR, AddArmourToChar);
    //REGISTER_COMMAND_HANDLER(COMMAND_ARE_ANY_CHARS_NEAR_CHAR, AreAnyCharsNearChar);
    //REGISTER_COMMAND_HANDLER(COMMAND_CLEAR_LOOK_AT, ClearLookAt);
    //REGISTER_COMMAND_HANDLER(COMMAND_CREATE_CHAR, CreateChar);
    //REGISTER_COMMAND_HANDLER(COMMAND_CREATE_CHAR_AS_PASSENGER, CreateCharAsPassenger);
    //REGISTER_COMMAND_HANDLER(COMMAND_CREATE_RANDOM_CHAR, CreateRandomChar);
    //REGISTER_COMMAND_HANDLER(COMMAND_CREATE_SWAT_ROPE, CreateSwatRope);
    //REGISTER_COMMAND_HANDLER(COMMAND_DAMAGE_CHAR, DamageChar);
    //REGISTER_COMMAND_HANDLER(COMMAND_DELETE_CHAR, DeleteChar);
    //REGISTER_COMMAND_HANDLER(COMMAND_DONT_REMOVE_CHAR, DontRemoveChar);
    //REGISTER_COMMAND_HANDLER(COMMAND_DROP_OBJECT, DropObject);
    //REGISTER_COMMAND_HANDLER(COMMAND_DROP_SECOND_OBJECT, DropSecondObject);
    //REGISTER_COMMAND_HANDLER(COMMAND_GET_NUMBER_OF_FOLLOWERS, GetNumberOfFollowers);
    //REGISTER_COMMAND_HANDLER(COMMAND_GET_PED_TYPE, GetPedType);
    //REGISTER_COMMAND_HANDLER(COMMAND_GET_SCRIPT_TASK_STATUS, GetScriptTaskStatus);
    //REGISTER_COMMAND_HANDLER(COMMAND_GET_SEQUENCE_PROGRESS, GetSequenceProgress);
    //REGISTER_COMMAND_HANDLER(COMMAND_GET_SEQUENCE_PROGRESS_RECURSIVE, GetSequenceProgressRecursive);
    //REGISTER_COMMAND_HANDLER(COMMAND_GIVE_MELEE_ATTACK_TO_CHAR, GiveMeleeAttackToChar);
    //REGISTER_COMMAND_HANDLER(COMMAND_GIVE_WEAPON_TO_CHAR, GiveWeaponToChar);
    //REGISTER_COMMAND_HANDLER(COMMAND_IGNORE_HEIGHT_DIFFERENCE_FOLLOWING_NODES, IgnoreHeightDifferenceFollowingNodes);
    //REGISTER_COMMAND_HANDLER(COMMAND_IS_CHAR_STOPPED_IN_ANGLED_AREA_IN_CAR_2D, IsCharStoppedInAngledAreaInCar2d);
    //REGISTER_COMMAND_HANDLER(COMMAND_IS_CHAR_STOPPED_IN_ANGLED_AREA_IN_CAR_3D, IsCharStoppedInAngledAreaInCar3d);
    //REGISTER_COMMAND_HANDLER(COMMAND_IS_GROUP_LEADER, IsGroupLeader);
    //REGISTER_COMMAND_HANDLER(COMMAND_IS_GROUP_MEMBER, IsGroupMember);
    //REGISTER_COMMAND_HANDLER(COMMAND_LISTEN_TO_PLAYER_GROUP_COMMANDS, ListenToPlayerGroupCommands);
    //REGISTER_COMMAND_HANDLER(COMMAND_LOCATE_CHAR_IN_CAR_OBJECT_2D, LocateCharInCarObject2d);
    //REGISTER_COMMAND_HANDLER(COMMAND_LOCATE_CHAR_IN_CAR_OBJECT_3D, LocateCharInCarObject3d);
    //REGISTER_COMMAND_HANDLER(COMMAND_PERFORM_SEQUENCE_TASK, PerformSequenceTask);
    //REGISTER_COMMAND_HANDLER(COMMAND_PERFORM_SEQUENCE_TASK_FROM_PROGRESS, PerformSequenceTaskFromProgress);
    //REGISTER_COMMAND_HANDLER(COMMAND_REMOVE_WEAPON_FROM_CHAR, RemoveWeaponFromChar);
    //REGISTER_COMMAND_HANDLER(COMMAND_SET_ANIM_GROUP_FOR_CHAR, SetAnimGroupForChar);
    //REGISTER_COMMAND_HANDLER(COMMAND_SET_CHAR_GET_OUT_UPSIDE_DOWN_CAR, SetCharGetOutUpsideDownCar);
    //REGISTER_COMMAND_HANDLER(COMMAND_SET_DEATH_WEAPONS_PERSIST, SetDeathWeaponsPersist);
    //REGISTER_COMMAND_HANDLER(COMMAND_SET_FOLLOW_NODE_THRESHOLD_DISTANCE, SetFollowNodeThresholdDistance);
    //REGISTER_COMMAND_HANDLER(COMMAND_SET_HEADING_LIMIT_FOR_ATTACHED_CHAR, SetHeadingLimitForAttachedChar);
    //REGISTER_COMMAND_HANDLER(COMMAND_SET_INFORM_RESPECTED_FRIENDS, SetInformRespectedFriends);
    //REGISTER_COMMAND_HANDLER(COMMAND_SET_NEXT_DESIRED_MOVE_STATE, SetNextDesiredMoveState);
    //REGISTER_COMMAND_HANDLER(COMMAND_SET_SENSE_RANGE, SetSenseRange);
    //REGISTER_COMMAND_HANDLER(COMMAND_SET_SWIM_SPEED, SetSwimSpeed);

    REGISTER_COMMAND_HANDLER(COMMAND_SET_CHAR_PROOFS, SetCharProofs);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CHAR_VELOCITY, SetCharVelocity);
    REGISTER_COMMAND_HANDLER(COMMAND_GET_CHAR_VELOCITY, GetCharVelocity);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CHAR_ROTATION, SetCharRotation);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CHAR_ALLOWED_TO_DUCK, SetCharAllowedToDuck);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CHAR_AREA_VISIBLE, SetCharAreaVisible);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CHAR_DROPS_WEAPONS_WHEN_DEAD, SetCharDropsWeaponsWhenDead);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CHAR_NEVER_LEAVES_GROUP, SetCharNeverLeavesGroup);
    REGISTER_COMMAND_HANDLER(COMMAND_ATTACH_FX_SYSTEM_TO_CHAR_BONE, AttachFxSystemToCharBone);
    REGISTER_COMMAND_HANDLER(COMMAND_GET_DEAD_CHAR_COORDINATES, GetDeadCharCoordinates);
    REGISTER_COMMAND_HANDLER(COMMAND_GET_CHAR_COORDINATES, GetCharCoordinates);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CHAR_COORDINATES, SetCharCoordinates);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_CHAR_IN_AREA_2D, IsCharInArea2D);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_CHAR_IN_AREA_3D, IsCharInArea3D);
    REGISTER_COMMAND_HANDLER(COMMAND_STORE_CAR_CHAR_IS_IN, StoreCarCharIsIn);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_CHAR_IN_CAR, IsCharInCar);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_CHAR_IN_MODEL, IsCharInModel);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_CHAR_IN_ANY_CAR, IsCharInAnyCar);
    REGISTER_COMMAND_HANDLER(COMMAND_LOCATE_CHAR_ANY_MEANS_2D, LocateCharAnyMeans2D);
    REGISTER_COMMAND_HANDLER(COMMAND_LOCATE_CHAR_ON_FOOT_2D, LocateCharOnFoot2D);
    REGISTER_COMMAND_HANDLER(COMMAND_LOCATE_CHAR_IN_CAR_2D, LocateCharInCar2D);
    REGISTER_COMMAND_HANDLER(COMMAND_LOCATE_STOPPED_CHAR_ANY_MEANS_2D, LocateStoppedCharAnyMeans2D);
    REGISTER_COMMAND_HANDLER(COMMAND_LOCATE_STOPPED_CHAR_ON_FOOT_2D, LocateStoppedCharOnFoot2D);
    REGISTER_COMMAND_HANDLER(COMMAND_LOCATE_STOPPED_CHAR_IN_CAR_2D, LocateStoppedCharInCar2D);
    REGISTER_COMMAND_HANDLER(COMMAND_LOCATE_CHAR_ANY_MEANS_3D, LocateCharAnyMeans3D);
    REGISTER_COMMAND_HANDLER(COMMAND_LOCATE_CHAR_ON_FOOT_3D, LocateCharOnFoot3D);
    REGISTER_COMMAND_HANDLER(COMMAND_LOCATE_CHAR_IN_CAR_3D, LocateCharInCar3D);
    REGISTER_COMMAND_HANDLER(COMMAND_LOCATE_STOPPED_CHAR_ANY_MEANS_3D, LocateStoppedCharAnyMeans3D);
    REGISTER_COMMAND_HANDLER(COMMAND_LOCATE_STOPPED_CHAR_ON_FOOT_3D, LocateStoppedCharOnFoot3D);
    REGISTER_COMMAND_HANDLER(COMMAND_LOCATE_STOPPED_CHAR_IN_CAR_3D, LocateStoppedCharInCar3D);
    REGISTER_COMMAND_HANDLER(COMMAND_LOCATE_CHAR_ANY_MEANS_CHAR_2D, LocateCharAnyMeansChar2D);
    REGISTER_COMMAND_HANDLER(COMMAND_LOCATE_CHAR_ON_FOOT_CHAR_2D, LocateCharOnFootChar2D);
    REGISTER_COMMAND_HANDLER(COMMAND_LOCATE_CHAR_IN_CAR_CHAR_2D, LocateCharInCarChar2D);
    REGISTER_COMMAND_HANDLER(COMMAND_LOCATE_CHAR_ANY_MEANS_CHAR_3D, LocateCharAnyMeansChar3D);
    REGISTER_COMMAND_HANDLER(COMMAND_LOCATE_CHAR_ON_FOOT_CHAR_3D, LocateCharOnFootChar3D);
    REGISTER_COMMAND_HANDLER(COMMAND_LOCATE_CHAR_IN_CAR_CHAR_3D, LocateCharInCarChar3D);
    REGISTER_COMMAND_HANDLER(COMMAND_LOCATE_CHAR_ANY_MEANS_CAR_2D, LocateCharAnyMeansCar2D);
    REGISTER_COMMAND_HANDLER(COMMAND_LOCATE_CHAR_ON_FOOT_CAR_2D, LocateCharOnFootCar2D);
    REGISTER_COMMAND_HANDLER(COMMAND_LOCATE_CHAR_IN_CAR_CAR_2D, LocateCharInCarCar2D);
    REGISTER_COMMAND_HANDLER(COMMAND_LOCATE_CHAR_ANY_MEANS_CAR_3D, LocateCharAnyMeansCar3D);
    REGISTER_COMMAND_HANDLER(COMMAND_LOCATE_CHAR_ON_FOOT_CAR_3D, LocateCharOnFootCar3D);
    REGISTER_COMMAND_HANDLER(COMMAND_LOCATE_CHAR_IN_CAR_CAR_3D, LocateCharInCarCar3D);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_CHAR_DEAD, IsCharDead);
    REGISTER_COMMAND_HANDLER(COMMAND_CREATE_CHAR_INSIDE_CAR, CreateCharInsideCar);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_CHAR_IN_ZONE, IsCharInZone);
    REGISTER_COMMAND_HANDLER(COMMAND_GET_CHAR_HEADING, GetCharHeading);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CHAR_HEADING, SetCharHeading);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_CHAR_TOUCHING_OBJECT, IsCharTouchingObject);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CHAR_AMMO, SetCharAmmo);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_CHAR_HEALTH_GREATER, IsCharHealthGreater);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CHAR_HEALTH, SetCharHealth);
    REGISTER_COMMAND_HANDLER(COMMAND_GET_CHAR_HEALTH, GetCharHealth);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_CHAR_TOUCHING_OBJECT_ON_FOOT, IsCharTouchingObjectOnFoot);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_CHAR_STOPPED, IsCharStopped);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CHAR_ONLY_DAMAGED_BY_PLAYER, SetCharOnlyDamagedByPlayer);
    REGISTER_COMMAND_HANDLER(COMMAND_GET_CLOSEST_CHAR_NODE, GetClosestCharNode);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_CHAR_ON_SCREEN, IsCharOnScreen);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_CHAR_SHOOTING_IN_AREA, IsCharShootingInArea);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_CURRENT_CHAR_WEAPON, IsCurrentCharWeapon);
    REGISTER_COMMAND_HANDLER(COMMAND_GET_RANDOM_CHAR_IN_ZONE, GetRandomCharInZone);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_CHAR_SHOOTING, IsCharShooting); // bad
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CHAR_ACCURACY, SetCharAccuracy);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_CHAR_MODEL, IsCharModel);
    REGISTER_COMMAND_HANDLER(COMMAND_HAS_CHAR_BEEN_DAMAGED_BY_WEAPON, HasCharBeenDamagedByWeapon);
    REGISTER_COMMAND_HANDLER(COMMAND_EXPLODE_CHAR_HEAD, ExplodeCharHead);
    REGISTER_COMMAND_HANDLER(COMMAND_START_CHAR_FIRE, StartCharFire);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CHAR_BLEEDING, SetCharBleeding);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CHAR_VISIBLE, SetCharVisible);
    //REGISTER_COMMAND_HANDLER(COMMAND_REMOVE_CHAR_ELEGANTLY, RemoveCharElegantly); // TODO(FIXME): It also needs the pool ref, regardless of whenever the ped was deleted from the pool or not, so taking a simple CPed* won't work...
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CHAR_STAY_IN_SAME_PLACE, SetCharStayInSamePlace);
    REGISTER_COMMAND_HANDLER(COMMAND_WARP_CHAR_FROM_CAR_TO_COORD, WarpCharFromCarToCoord);
    REGISTER_COMMAND_HANDLER(COMMAND_HAS_CHAR_SPOTTED_CHAR, HasCharSpottedChar);
    REGISTER_COMMAND_HANDLER(COMMAND_WARP_CHAR_INTO_CAR, WarpCharIntoCar);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CHAR_ANIM_SPEED, SetCharAnimSpeed);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CHAR_CANT_BE_DRAGGED_OUT, SetCharCantBeDraggedOut);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_CHAR_MALE, IsCharMale);
    REGISTER_COMMAND_HANDLER(COMMAND_STORE_CAR_CHAR_IS_IN_NO_SAVE, StoreCarCharIsInNoSave);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CHAR_MONEY, SetCharMoney);
    REGISTER_COMMAND_HANDLER(COMMAND_GET_AMMO_IN_CHAR_WEAPON, GetAmmoInCharWeapon);
    REGISTER_COMMAND_HANDLER(COMMAND_WARP_CHAR_INTO_CAR_AS_PASSENGER, WarpCharIntoCarAsPassenger);
    REGISTER_COMMAND_HANDLER(COMMAND_GET_CHAR_IN_CAR_PASSENGER_SEAT, GetCharInCarPassengerSeat);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CHAR_IS_CHRIS_CRIMINAL, SetCharIsChrisCriminal);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CHAR_SUFFERS_CRITICAL_HITS, SetCharSuffersCriticalHits);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_CHAR_SITTING_IN_CAR, IsCharSittingInCar);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_CHAR_SITTING_IN_ANY_CAR, IsCharSittingInAnyCar);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_CHAR_ON_FOOT, IsCharOnFoot);
    REGISTER_COMMAND_HANDLER(COMMAND_ATTACH_CHAR_TO_CAR, AttachCharToCar);
    REGISTER_COMMAND_HANDLER(COMMAND_DETACH_CHAR_FROM_CAR, DetachCharFromCar);
    REGISTER_COMMAND_HANDLER(COMMAND_CLEAR_CHAR_LAST_WEAPON_DAMAGE, ClearCharLastWeaponDamage);
    REGISTER_COMMAND_HANDLER(COMMAND_GET_CURRENT_CHAR_WEAPON, GetCurrentCharWeapon);
    REGISTER_COMMAND_HANDLER(COMMAND_LOCATE_CHAR_ANY_MEANS_OBJECT_2D, LocateCharAnyMeansObject2D);
    REGISTER_COMMAND_HANDLER(COMMAND_LOCATE_CHAR_ON_FOOT_OBJECT_2D, LocateCharOnFootObject2D);
    REGISTER_COMMAND_HANDLER(COMMAND_LOCATE_CHAR_ANY_MEANS_OBJECT_3D, LocateCharAnyMeansObject3D);
    REGISTER_COMMAND_HANDLER(COMMAND_LOCATE_CHAR_ON_FOOT_OBJECT_3D, LocateCharOnFootObject3D);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_CHAR_ON_ANY_BIKE, IsCharOnAnyBike);
    REGISTER_COMMAND_HANDLER(COMMAND_CAN_CHAR_SEE_DEAD_CHAR, CanCharSeeDeadChar);
    REGISTER_COMMAND_HANDLER(COMMAND_SHUT_CHAR_UP, ShutCharUp);
    REGISTER_COMMAND_HANDLER(COMMAND_REMOVE_ALL_CHAR_WEAPONS, RemoveAllCharWeapons);
    REGISTER_COMMAND_HANDLER(COMMAND_HAS_CHAR_GOT_WEAPON, HasCharGotWeapon);
    REGISTER_COMMAND_HANDLER(COMMAND_GET_DEAD_CHAR_PICKUP_COORDS, GetDeadCharPickupCoords);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_CHAR_IN_ANY_BOAT, IsCharInAnyBoat);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_CHAR_IN_ANY_HELI, IsCharInAnyHeli);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_CHAR_IN_ANY_PLANE, IsCharInAnyPlane);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_CHAR_IN_WATER, IsCharInWater);
    REGISTER_COMMAND_HANDLER(COMMAND_GET_CHAR_WEAPON_IN_SLOT, GetCharWeaponInSlot);
    REGISTER_COMMAND_HANDLER(COMMAND_GET_OFFSET_FROM_CHAR_IN_WORLD_COORDS, GetOffsetFromCharInWorldCoords);
    REGISTER_COMMAND_HANDLER(COMMAND_HAS_CHAR_BEEN_PHOTOGRAPHED, HasCharBeenPhotographed);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_CHAR_IN_FLYING_VEHICLE, IsCharInFlyingVehicle);
    REGISTER_COMMAND_HANDLER(COMMAND_FREEZE_CHAR_POSITION, FreezeCharPosition);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CHAR_DROWNS_IN_WATER, SetCharDrownsInWater);
    REGISTER_COMMAND_HANDLER(COMMAND_GET_CHAR_ARMOUR, GetCharArmour);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CHAR_ARMOUR, SetCharArmour);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_CHAR_WAITING_FOR_WORLD_COLLISION, IsCharWaitingForWorldCollision);
    REGISTER_COMMAND_HANDLER(COMMAND_ATTACH_CHAR_TO_OBJECT, AttachCharToObject);
    REGISTER_COMMAND_HANDLER(COMMAND_HAS_CHAR_BEEN_DAMAGED_BY_CHAR, HasCharBeenDamagedByChar);
    REGISTER_COMMAND_HANDLER(COMMAND_HAS_CHAR_BEEN_DAMAGED_BY_CAR, HasCharBeenDamagedByCar);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CHAR_STAY_IN_CAR_WHEN_JACKED, SetCharStayInCarWhenJacked);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_CHAR_TOUCHING_VEHICLE, IsCharTouchingVehicle);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CHAR_CAN_BE_SHOT_IN_VEHICLE, SetCharCanBeShotInVehicle);
    REGISTER_COMMAND_HANDLER(COMMAND_CLEAR_CHAR_LAST_DAMAGE_ENTITY, ClearCharLastDamageEntity);
    REGISTER_COMMAND_HANDLER(COMMAND_CREATE_RANDOM_CHAR_AS_DRIVER, CreateRandomCharAsDriver);
    REGISTER_COMMAND_HANDLER(COMMAND_CREATE_RANDOM_CHAR_AS_PASSENGER, CreateRandomCharAsPassenger);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CHAR_NEVER_TARGETTED, SetCharNeverTargetted);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_CHAR_IN_ANY_POLICE_VEHICLE, IsCharInAnyPoliceVehicle);
    REGISTER_COMMAND_HANDLER(COMMAND_DOES_CHAR_EXIST, DoesCharExist);
    REGISTER_COMMAND_HANDLER(COMMAND_FREEZE_CHAR_POSITION_AND_DONT_LOAD_COLLISION, FreezeCharPositionAndDontLoadCollision);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_LOAD_COLLISION_FOR_CHAR_FLAG, SetLoadCollisionForCharFlag);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_CHAR_DUCKING, IsCharDucking);
    REGISTER_COMMAND_HANDLER(COMMAND_TASK_KILL_CHAR_ON_FOOT, TaskKillCharOnFoot);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_CHAR_IN_ANGLED_AREA_2D, IsCharInAngledArea2D);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_CHAR_IN_ANGLED_AREA_IN_CAR_2D, IsCharInAngledAreaInCar2D);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_CHAR_IN_ANGLED_AREA_3D, IsCharInAngledArea3D);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_CHAR_IN_ANGLED_AREA_ON_FOOT_3D, IsCharInAngledAreaOnFoot3D);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_CHAR_IN_ANGLED_AREA_IN_CAR_3D, IsCharInAngledAreaInCar3D);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_CHAR_IN_TAXI, IsCharInTaxi);
    REGISTER_COMMAND_HANDLER(COMMAND_LOAD_CHAR_DECISION_MAKER, LoadCharDecisionMaker);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CHAR_DECISION_MAKER, SetCharDecisionMaker);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_CHAR_PLAYING_ANIM, IsCharPlayingAnim);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CHAR_ANIM_PLAYING_FLAG, SetCharAnimPlayingFlag);
    REGISTER_COMMAND_HANDLER(COMMAND_GET_CHAR_ANIM_CURRENT_TIME, GetCharAnimCurrentTime);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CHAR_ANIM_CURRENT_TIME, SetCharAnimCurrentTime);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CHAR_COLLISION, SetCharCollision);
    REGISTER_COMMAND_HANDLER(COMMAND_GET_CHAR_ANIM_TOTAL_TIME, GetCharAnimTotalTime);
    // REGISTER_COMMAND_HANDLER(COMMAND_CREATE_CHAR_AT_ATTRACTOR, CreateCharAtAttractor);
    REGISTER_COMMAND_HANDLER(COMMAND_TASK_KILL_CHAR_ON_FOOT_WHILE_DUCKING, TaskKillCharOnFootWhileDucking);
    REGISTER_COMMAND_HANDLER(COMMAND_TASK_TURN_CHAR_TO_FACE_CHAR, TaskTurnCharToFaceChar);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_CHAR_AT_SCRIPTED_ATTRACTOR, IsCharAtScriptedAttractor);
    REGISTER_COMMAND_HANDLER(COMMAND_GET_CHAR_MODEL, GetCharModel);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CURRENT_CHAR_WEAPON, SetCurrentCharWeapon);
    REGISTER_COMMAND_HANDLER(COMMAND_MARK_CHAR_AS_NO_LONGER_NEEDED, MarkCharAsNoLongerNeeded);
    REGISTER_COMMAND_HANDLER(COMMAND_GET_CHAR_SPEED, GetCharSpeed);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_CHAR_IN_SEARCHLIGHT, IsCharInSearchlight);
    REGISTER_COMMAND_HANDLER(COMMAND_REMOVE_CHAR_FROM_GROUP, RemoveCharFromGroup);
    REGISTER_COMMAND_HANDLER(COMMAND_ATTACH_CHAR_TO_BIKE, AttachCharToBike);
    REGISTER_COMMAND_HANDLER(COMMAND_GET_CAR_CHAR_IS_USING, GetCarCharIsUsing);
    REGISTER_COMMAND_HANDLER(COMMAND_DISABLE_CHAR_SPEECH, DisableCharSpeech);
    REGISTER_COMMAND_HANDLER(COMMAND_ENABLE_CHAR_SPEECH, EnableCharSpeech);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CHAR_SIGNAL_AFTER_KILL, SetCharSignalAfterKill);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CHAR_WANTED_BY_POLICE, SetCharWantedByPolice);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CHAR_COORDINATES_DONT_WARP_GANG_NO_OFFSET, SetCharCoordinatesDontWarpGangNoOffset);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_CHAR_USING_MAP_ATTRACTOR, IsCharUsingMapAttractor);
    REGISTER_COMMAND_HANDLER(COMMAND_REMOVE_CHAR_FROM_CAR_MAINTAIN_POSITION, RemoveCharFromCarMaintainPosition);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CHAR_SAY_CONTEXT_IMPORTANT, SetCharSayContextImportant);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CHAR_SAY_SCRIPT, SetCharSayScript);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_CHAR_GETTING_IN_TO_A_CAR, IsCharGettingInToACar);
    REGISTER_COMMAND_HANDLER(COMMAND_GET_CHAR_AREA_VISIBLE, GetCharAreaVisible);
    REGISTER_COMMAND_HANDLER(COMMAND_HAS_CHAR_SPOTTED_CHAR_IN_FRONT, HasCharSpottedCharInFront);
    REGISTER_COMMAND_HANDLER(COMMAND_SHUT_CHAR_UP_FOR_SCRIPTED_SPEECH, ShutCharUpForScriptedSpeech);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_CHAR_TOUCHING_CHAR, IsCharTouchingChar);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_CHAR_ATTACHED_TO_ANY_CAR, IsCharAttachedToAnyCar);
    REGISTER_COMMAND_HANDLER(COMMAND_STORE_CAR_CHAR_IS_ATTACHED_TO_NO_SAVE, StoreCarCharIsAttachedToNoSave);
    //REGISTER_COMMAND_HANDLER(COMMAND_CREATE_FX_SYSTEM_ON_CHAR_WITH_DIRECTION, CreateFxSystemOnCharWithDirection);
    //REGISTER_COMMAND_HANDLER(COMMAND_ATTACH_CAMERA_TO_CHAR_LOOK_AT_CHAR, AttachCameraToCharLookAtChar);
    //REGISTER_COMMAND_HANDLER(COMMAND_CLEAR_CHAR_TASKS, ClearCharTasks);
    //REGISTER_COMMAND_HANDLER(COMMAND_TASK_GOTO_CHAR_OFFSET, TaskGotoCharOffset);
    //REGISTER_COMMAND_HANDLER(COMMAND_HIDE_CHAR_WEAPON_FOR_SCRIPTED_CUTSCENE, HideCharWeaponForScriptedCutscene);
    //REGISTER_COMMAND_HANDLER(COMMAND_TASK_TURN_CHAR_TO_FACE_COORD, TaskTurnCharToFaceCoord);
    //REGISTER_COMMAND_HANDLER(COMMAND_TASK_CHAR_ARREST_CHAR, TaskCharArrestChar);
    //REGISTER_COMMAND_HANDLER(COMMAND_CLEAR_CHAR_DECISION_MAKER_EVENT_RESPONSE, ClearCharDecisionMakerEventResponse);
    //REGISTER_COMMAND_HANDLER(COMMAND_ADD_CHAR_DECISION_MAKER_EVENT_RESPONSE, AddCharDecisionMakerEventResponse);
    //REGISTER_COMMAND_HANDLER(COMMAND_TASK_WARP_CHAR_INTO_CAR_AS_DRIVER, TaskWarpCharIntoCarAsDriver);
    //REGISTER_COMMAND_HANDLER(COMMAND_TASK_WARP_CHAR_INTO_CAR_AS_PASSENGER, TaskWarpCharIntoCarAsPassenger);
    //REGISTER_COMMAND_HANDLER(COMMAND_IS_CHAR_HOLDING_OBJECT, IsCharHoldingObject);
    //REGISTER_COMMAND_HANDLER(COMMAND_GET_RANDOM_CHAR_IN_SPHERE, GetRandomCharInSphere);
    //REGISTER_COMMAND_HANDLER(COMMAND_HAS_CHAR_BEEN_ARRESTED, HasCharBeenArrested);
    //REGISTER_COMMAND_HANDLER(COMMAND_IS_CHAR_RESPONDING_TO_EVENT, IsCharRespondingToEvent);
    //REGISTER_COMMAND_HANDLER(COMMAND_TASK_FLEE_CHAR_ANY_MEANS, TaskFleeCharAnyMeans);
    //REGISTER_COMMAND_HANDLER(COMMAND_SET_CHAR_IS_TARGET_PRIORITY, SetCharIsTargetPriority);
    //REGISTER_COMMAND_HANDLER(COMMAND_SET_CHAR_RELATIONSHIP, SetCharRelationship);
    //REGISTER_COMMAND_HANDLER(COMMAND_CLEAR_CHAR_RELATIONSHIP, ClearCharRelationship);
    //REGISTER_COMMAND_HANDLER(COMMAND_CLEAR_ALL_CHAR_RELATIONSHIPS, ClearAllCharRelationships);
    REGISTER_COMMAND_HANDLER(COMMAND_CLEAR_CHAR_TASKS_IMMEDIATELY, ClearCharTasksImmediately);
    //REGISTER_COMMAND_HANDLER(COMMAND_TASK_GOTO_CHAR_AIMING, TaskGotoCharAiming);
    //REGISTER_COMMAND_HANDLER(COMMAND_TASK_KILL_CHAR_ON_FOOT_TIMED, TaskKillCharOnFootTimed);
    //REGISTER_COMMAND_HANDLER(COMMAND_IS_CHAR_IN_ANY_SEARCHLIGHT, IsCharInAnySearchlight);
    //REGISTER_COMMAND_HANDLER(COMMAND_TASK_SET_CHAR_DECISION_MAKER, TaskSetCharDecisionMaker);
    //REGISTER_COMMAND_HANDLER(COMMAND_TASK_CHAR_SLIDE_TO_COORD, TaskCharSlideToCoord);
    //REGISTER_COMMAND_HANDLER(COMMAND_SET_CHAR_SHOOT_RATE, SetCharShootRate);
    //REGISTER_COMMAND_HANDLER(COMMAND_COPY_CHAR_DECISION_MAKER, CopyCharDecisionMaker);
    //REGISTER_COMMAND_HANDLER(COMMAND_TASK_CHAR_SLIDE_TO_COORD_AND_PLAY_ANIM, TaskCharSlideToCoordAndPlayAnim);
    //REGISTER_COMMAND_HANDLER(COMMAND_GET_CHAR_HIGHEST_PRIORITY_EVENT, GetCharHighestPriorityEvent);
    //REGISTER_COMMAND_HANDLER(COMMAND_SET_CHAR_KINDA_STAY_IN_SAME_PLACE, SetCharKindaStayInSamePlace);
    //REGISTER_COMMAND_HANDLER(COMMAND_IS_CHAR_IN_AIR, IsCharInAir);
    //REGISTER_COMMAND_HANDLER(COMMAND_GET_CHAR_HEIGHT_ABOVE_GROUND, GetCharHeightAboveGround);
    //REGISTER_COMMAND_HANDLER(COMMAND_SET_CHAR_WEAPON_SKILL, SetCharWeaponSkill);
    //REGISTER_COMMAND_HANDLER(COMMAND_GET_RANDOM_CHAR_IN_SPHERE_ONLY_DRUGS_BUYERS, GetRandomCharInSphereOnlyDrugsBuyers);
    //REGISTER_COMMAND_HANDLER(COMMAND_SET_CHAR_HAS_USED_ENTRY_EXIT, SetCharHasUsedEntryExit);
    //REGISTER_COMMAND_HANDLER(COMMAND_SET_CHAR_MAX_HEALTH, SetCharMaxHealth);
    //REGISTER_COMMAND_HANDLER(COMMAND_SET_CHAR_CAN_BE_KNOCKED_OFF_BIKE, SetCharCanBeKnockedOffBike);
    //REGISTER_COMMAND_HANDLER(COMMAND_SET_CHAR_COORDINATES_DONT_WARP_GANG, SetCharCoordinatesDontWarpGang);
    //REGISTER_COMMAND_HANDLER(COMMAND_GET_RANDOM_CHAR_IN_SPHERE_NO_BRAIN, GetRandomCharInSphereNoBrain);
    //REGISTER_COMMAND_HANDLER(COMMAND_SET_CHAR_BULLETPROOF_VEST, SetCharBulletproofVest);
    //REGISTER_COMMAND_HANDLER(COMMAND_SET_CHAR_FIRE_DAMAGE_MULTIPLIER, SetCharFireDamageMultiplier);
    //REGISTER_COMMAND_HANDLER(COMMAND_SET_CHAR_USES_UPPERBODY_DAMAGE_ANIMS_ONLY, SetCharUsesUpperbodyDamageAnimsOnly);
    //REGISTER_COMMAND_HANDLER(COMMAND_SET_CHAR_SAY_CONTEXT, SetCharSayContext);
    //REGISTER_COMMAND_HANDLER(COMMAND_GET_NAME_OF_ENTRY_EXIT_CHAR_USED, GetNameOfEntryExitCharUsed);
    //REGISTER_COMMAND_HANDLER(COMMAND_GET_POSITION_OF_ENTRY_EXIT_CHAR_USED, GetPositionOfEntryExitCharUsed);
    //REGISTER_COMMAND_HANDLER(COMMAND_IS_CHAR_TALKING, IsCharTalking);
    //REGISTER_COMMAND_HANDLER(COMMAND_IS_CHAR_STUCK_UNDER_CAR, IsCharStuckUnderCar);
    //REGISTER_COMMAND_HANDLER(COMMAND_SET_CHAR_KEEP_TASK, SetCharKeepTask);
    //REGISTER_COMMAND_HANDLER(COMMAND_IS_CHAR_SWIMMING, IsCharSwimming);
    //REGISTER_COMMAND_HANDLER(COMMAND_GET_CHAR_SWIM_STATE, GetCharSwimState);
    //REGISTER_COMMAND_HANDLER(COMMAND_START_CHAR_FACIAL_TALK, StartCharFacialTalk);
    //REGISTER_COMMAND_HANDLER(COMMAND_STOP_CHAR_FACIAL_TALK, StopCharFacialTalk);
    //REGISTER_COMMAND_HANDLER(COMMAND_SET_CHAR_COORDINATES_NO_OFFSET, SetCharCoordinatesNoOffset);
    //REGISTER_COMMAND_HANDLER(COMMAND_COPY_SHARED_CHAR_DECISION_MAKER, CopySharedCharDecisionMaker);
    //REGISTER_COMMAND_HANDLER(COMMAND_SET_CHAR_FORCE_DIE_IN_CAR, SetCharForceDieInCar);
    //REGISTER_COMMAND_HANDLER(COMMAND_SET_CHAR_USES_COLLISION_CLOSEST_OBJECT_OF_TYPE, SetCharUsesCollisionClosestObjectOfType);
    //REGISTER_COMMAND_HANDLER(COMMAND_SET_CHAR_DRUGGED_UP, SetCharDruggedUp);
    //REGISTER_COMMAND_HANDLER(COMMAND_IS_CHAR_HEAD_MISSING, IsCharHeadMissing);
    //REGISTER_COMMAND_HANDLER(COMMAND_IS_CHAR_IN_ANY_TRAIN, IsCharInAnyTrain);
    //REGISTER_COMMAND_HANDLER(COMMAND_GET_RANDOM_CHAR_IN_AREA_OFFSET_NO_SAVE, GetRandomCharInAreaOffsetNoSave);

    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_CHAR_GRAPHIC_TYPE);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_STOP_CHAR_DRIVING);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_CHAR_OCCUPATION);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_CHAR_IN_GANG_ZONE);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_ADD_BLIP_FOR_CHAR_OLD);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_LOCATE_PLAYER_ANY_MEANS_CHAR_2D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_CHAR_STILL_ALIVE);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_LOCATE_PLAYER_ON_FOOT_CHAR_2D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_LOCATE_PLAYER_IN_CAR_CHAR_2D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_LOCATE_PLAYER_ANY_MEANS_CHAR_3D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_LOCATE_PLAYER_ON_FOOT_CHAR_3D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_LOCATE_PLAYER_IN_CAR_CHAR_3D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_CHAR_THREAT_SEARCH);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_CHAR_THREAT_REACTION);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_CHAR_OBJ_NO_OBJ);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_ORDER_CHAR_TO_DRIVE_CAR);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_HAS_CHAR_SPOTTED_PLAYER);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_ORDER_CHAR_TO_BACKDOOR);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_ADD_CHAR_TO_GANG);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_CHAR_OBJECTIVE_PASSED);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_CHAR_DRIVE_AGGRESSION);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_CHAR_MAX_DRIVESPEED);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_MAKE_CHAR_DO_NOTHING);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_CHAR_INVINCIBLE);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_CHAR_OBJ_WAIT_ON_FOOT);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_CHAR_OBJ_FLEE_ON_FOOT_TILL_SAFE);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_CHAR_OBJ_GUARD_SPOT);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_CHAR_OBJ_GUARD_AREA);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_CHAR_OBJ_WAIT_IN_CAR);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_CHAR_IN_AREA_ON_FOOT_2D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_CHAR_IN_AREA_IN_CAR_2D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_CHAR_STOPPED_IN_AREA_2D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_CHAR_STOPPED_IN_AREA_ON_FOOT_2D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_CHAR_STOPPED_IN_AREA_IN_CAR_2D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_CHAR_IN_AREA_ON_FOOT_3D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_CHAR_IN_AREA_IN_CAR_3D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_CHAR_STOPPED_IN_AREA_3D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_CHAR_STOPPED_IN_AREA_ON_FOOT_3D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_CHAR_STOPPED_IN_AREA_IN_CAR_3D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_TURN_CHAR_TO_FACE_COORD);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_CHAR_OBJ_KILL_CHAR_ON_FOOT);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_CHAR_OBJ_KILL_PLAYER_ON_FOOT);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_CHAR_OBJ_KILL_CHAR_ANY_MEANS);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_CHAR_OBJ_KILL_PLAYER_ANY_MEANS);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_CHAR_OBJ_FLEE_CHAR_ON_FOOT_TILL_SAFE);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_CHAR_OBJ_FLEE_PLAYER_ON_FOOT_TILL_SAFE);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_CHAR_OBJ_FLEE_CHAR_ON_FOOT_ALWAYS);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_CHAR_OBJ_FLEE_PLAYER_ON_FOOT_ALWAYS);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_CHAR_OBJ_GOTO_CHAR_ON_FOOT);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_CHAR_OBJ_GOTO_PLAYER_ON_FOOT);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_CHAR_OBJ_LEAVE_CAR);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_CHAR_OBJ_ENTER_CAR_AS_PASSENGER);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_CHAR_OBJ_ENTER_CAR_AS_DRIVER);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_CHAR_OBJ_FOLLOW_CAR_IN_CAR);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_CHAR_OBJ_FIRE_AT_OBJECT_FROM_VEHICLE);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_CHAR_OBJ_DESTROY_OBJECT);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_CHAR_OBJ_DESTROY_CAR);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_CHAR_OBJ_GOTO_AREA_ON_FOOT);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_CHAR_OBJ_GOTO_AREA_IN_CAR);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_CHAR_OBJ_FOLLOW_CAR_ON_FOOT_WITH_OFFSET);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_CHAR_OBJ_GUARD_ATTACK);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_CHAR_AS_LEADER);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_CHAR_OBJ_FOLLOW_ROUTE);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_CLEAR_CHAR_THREAT_SEARCH);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_TURN_CHAR_TO_FACE_CHAR);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_TURN_CHAR_TO_FACE_PLAYER);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_CHAR_OBJ_GOTO_COORD_ON_FOOT);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_CHAR_OBJ_GOTO_COORD_IN_CAR);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_CHAR_LOOK_AT_CHAR_ALWAYS);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_PLAYER_LOOK_AT_CHAR_ALWAYS);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_STOP_CHAR_LOOKING);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_CHAR_OBJ_RUN_TO_AREA);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_CHAR_OBJ_RUN_TO_COORD);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_CHAR_PERSONALITY);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_CHAR_HEED_THREATS);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_CHAR_OBJ_GOTO_AREA_ANY_MEANS);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_GET_RANDOM_CHAR_IN_AREA);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_CHAR_WANDER_DIR);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_CHAR_WANDER_RANGE);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_CHAR_FOLLOW_PATH);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_CHAR_SET_IDLE);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_CHAR_LOOK_AT_PLAYER_ALWAYS);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_CHAR_OBJ_FOLLOW_CHAR_IN_FORMATION);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_CHAR_RUNNING);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_CHAR_IN_CHARS_GROUP);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_CHAR_IN_PLAYERS_GROUP);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_CHAR_OBJ_CATCH_TRAIN);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_CHAR_OBJ_HAIL_TAXI);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_CHAR_WAIT_STATE);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_CHAR_OBJ_STEAL_ANY_CAR);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_CHAR_OBJ_LEAVE_ANY_CAR);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_CHAR_IN_CONTROL);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_CHAR_STAYS_IN_CURRENT_LEVEL);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_INCREASE_CHAR_MONEY);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_CHAR_USE_PEDNODE_SEEK);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_CHAR_SAY);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_CHAR_AVOID_LEVEL_TRANSITIONS);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_CHAR_IGNORE_LEVEL_TRANSITIONS);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_CHAR_OBJ_FLEE_CAR);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_CHAR_LYING_DOWN);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_CHAR_CEASE_ATTACK_TIMER);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_CHAR_FOOT_DOWN);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_CHAR_OBJ_WALK_TO_CHAR);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_CHAR_OBJ_AIM_GUN_AT_CHAR);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_GET_NTH_CLOSEST_CHAR_NODE);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_CHAR_SHUFFLE_INTO_DRIVERS_SEAT);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_CHAR_AS_PLAYER_FRIEND);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_CHAR_OBJ_NO_OBJ);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_CHAR_OBJ_SPRINT_TO_COORD);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_CHAR_LEAVING_VEHICLE_TO_DIE);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_CHAR_WANDER_PATH_CLEAR);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_CHAR_CAN_BE_DAMAGED_BY_MEMBERS_OF_GANG);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_CHAR_DROWNING_IN_WATER);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_CHAR_ANSWERING_MOBILE);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_CHAR_IN_PLAYERS_GROUP_CAN_FIGHT);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_CLEAR_CHAR_WAIT_STATE);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_CLEAR_CHAR_FOLLOW_PATH);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_CHAR_IGNORE_THREATS_BEHIND_OBJECTS);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_CHAR_CROUCH_WHEN_THREATENED);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_CHAR_STUCK);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_CHAR_STOP_SHOOT_DONT_SEEK_ENTITY);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_CLEAR_ALL_CHAR_ANIMS);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_CHAR_OBJ_BUY_ICE_CREAM);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_CLEAR_CHAR_ICE_CREAM_PURCHASE);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_HAS_CHAR_ATTEMPTED_ATTRACTOR);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_HAS_CHAR_BOUGHT_ICE_CREAM);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_CHAR_FRIGHTENED_IN_JACKED_CAR);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_CHAR_LOOK_AT_OBJECT_ALWAYS);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_CHAR_IN_DISGUISE);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_CHAR_IN_ANGLED_AREA_ON_FOOT_2D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_CHAR_STOPPED_IN_ANGLED_AREA_2D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_CHAR_STOPPED_IN_ANGLED_AREA_ON_FOOT_2D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_CHAR_STOPPED_IN_ANGLED_AREA_3D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_CHAR_STOPPED_IN_ANGLED_AREA_ON_FOOT_3D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_CHAR_THREAT_LIST);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_CLEAR_CHAR_THREATS);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_CHAR_ZONE_DISTANCE);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_PLACE_CHAR_AT_ATTRACTOR);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_CHAR_TOUCHING_ANY_OBJECT);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_GET_CHAR_AT_SCRIPTED_ATTRACTOR);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_ATTACH_CAMERA_TO_CHAR_LOOK_AT_VEHICLE);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_CLEAR_CHAR_FRIENDS);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_CHAR_FRIEND_LIST);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_ATTACH_CAMERA_TO_CHAR_LOOK_AT_OBJECT);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_GET_CHAR_LIGHTING);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_CHAR_SPECIAL_EVENT);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_CHAR_TYRES_CAN_BE_BURST);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_TASK_DRAG_CHAR_FROM_CAR);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_ATTACH_CHAR_TO_ROPE_FOR_OBJECT);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_TASK_KILL_CHAR_ON_FOOT_PATROL);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_HAS_CHAR_SPOTTED_CAR);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_GET_CHAR_BREATH);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_CHAR_BREATH);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_GET_CHAR_MAP_ATTRACTOR_STATUS);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_PICK_UP_CHAR_WITH_WINCH);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_CHAR_BEEN_PHOTOGRAPHED_FLAG);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_LOAD_SHARED_CHAR_DECISION_MAKER);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_CHAR_AIR_RESISTANCE_MULTIPLIER);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_CHAR_CAN_CLIMB_OUT_WATER);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_FORCE_RANDOM_PED_TYPE);
}
