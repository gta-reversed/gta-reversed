#include <StdInc.h>

#include "./Commands.hpp"
#include <CommandParser/Parser.hpp>
#include <extensions/Shapes/AngledRect.hpp>
#include "Utility.h"

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
    if (S.m_bUseMissionCleanup) {
        CTheScripts::MissionCleanUp.AddEntityToList(entity);
    }
}

//
// Flags getters/setters
//

void SetCharProofs(CPed& ped, bool bullet, bool fire, bool explosion, bool collision, bool melee) {
    auto& flags = ped.physicalFlags;
    flags.bBulletProof = bullet;
    flags.bFireProof = fire;
    flags.bExplosionProof = explosion;
    flags.bCollisionProof = collision;
    flags.bMeleeProof = melee;
}

auto SetCharDropsWeaponsWhenDead(CPed& ped, bool dropsWepsWhenDead) {
    ped.bDoesntDropWeaponsWhenDead = !dropsWepsWhenDead;
}

auto SetCharNeverLeavesGroup(CPed& ped, bool bNeverLeavesGroup) {
    ped.bNeverLeavesGroup = bNeverLeavesGroup;
}

// SET_CHAR_BLEEDING
auto SetCharBleeding(CPed& ped, bool isBleeding) {
    ped.bPedIsBleeding = isBleeding;
}

// SET_CHAR_CANT_BE_DRAGGED_OUT
auto SetCharCantBeDraggedOut(CPed& ped, bool bDontDrag) {
    ped.bDontDragMeOutCar = bDontDrag;
}

// SET_CHAR_IS_CHRIS_CRIMINAL
auto SetCharIsChrisCriminal(CPed& ped, bool value) {
    ped.bChrisCriminal = value;
}

// SET_CHAR_SUFFERS_CRITICAL_HITS
auto SetCharSuffersCriticalHits(CPed& ped, bool value) {
    ped.bNoCriticalHits = !value;
}

// FREEZE_CHAR_POSITION
auto FreezeCharPosition(CPed& ped, bool freeze) {
    ped.physicalFlags.bDontApplySpeed = freeze;
}

// SET_CHAR_DROWNS_IN_WATER
auto SetCharDrownsInWater(CPed& ped, bool bDrownsInWater) {
    ped.bDrownsInWater = bDrownsInWater;
}

// SET_CHAR_STAY_IN_CAR_WHEN_JACKED
auto SetCharStayInCarWhenJacked(CPed& ped, bool bStayInCarOnJack) {
    ped.bStayInCarOnJack = bStayInCarOnJack;
}

// SET_CHAR_CAN_BE_SHOT_IN_VEHICLE
auto SetCharCanBeShotInVehicle(CPed& ped, bool bCanBeShotInVehicle) {
    ped.bCanBeShotInVehicle = bCanBeShotInVehicle;
}

// SET_CHAR_NEVER_TARGETTED
auto SetCharNeverTargetted(CPed& ped, bool bNeverEverTargetThisPed) {
    ped.bNeverEverTargetThisPed = bNeverEverTargetThisPed;
}

// SET_CHAR_COLLISION
auto SetCharCollision(CPed& ped, bool bUsesCollision) {
    ped.m_bUsesCollision = bUsesCollision;
}

// IS_CHAR_SHOOTING
auto IsCharShooting(CPed& ped) {
    return ped.bFiringWeapon;
}

// IS_CHAR_WAITING_FOR_WORLD_COLLISION
auto IsCharWaitingForWorldCollision(CPed& ped) {
    return ped.m_bIsStaticWaitingForCollision;
}

// IS_CHAR_DUCKING
auto IsCharDucking(CPed& ped) {
    return ped.bIsDucking;
}

auto GetCharVelocity(CPed& ped) {
    return ped.GetMoveSpeed();
}

auto SetCharVelocity(CPed& ped, CVector velocity) {
    ped.SetVelocity(velocity / 50.f);
}

auto SetCharRotation(CPed& ped, CVector angles) {
    ped.SetOrientation(angles * DegreesToRadians(1.0f)); // make euler angles from degrees
    CWorld::Add(&ped);
}

auto SetCharAllowedToDuck(CPed& ped, CVector rotdeg) {
    CWorld::Remove(&ped);
    ped.SetOrientation(rotdeg * DegreesToRadians(1.f)); // degrees => radians
    CWorld::Add(&ped);
}

auto SetCharAreaVisible(CPed& ped, eAreaCodes area) {
    ped.m_nAreaCode = area;
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

auto AttachFxSystemToCharBone(tScriptEffectSystem& fx, CPed& ped, eBoneTag bone) {
    fx.m_pFxSystem->AttachToBone(&ped, bone);
}

auto GetDeadCharCoordinates(CPed& ped) {
    if (ped.IsInVehicle()) {
        return ped.m_pVehicle->GetPosition();
    } else {
        return ped.GetBonePosition(BONE_PELVIS);
    }
}

auto GetCharCoordinates(CPed& ped) {
    if (ped.IsInVehicle()) {
        return ped.m_pVehicle->GetPosition();
    } else {
        return ped.GetPosition();
    }
}

auto SetCharCoordinates(CRunningScript& S, CPed& ped, CVector coords) {
    S.SetCharCoordinates(ped, coords, true, true);
}

auto IsCharInArea2D(CRunningScript& S, CPed& ped, CVector2D a, CVector2D b, bool highlightArea) {
    if (highlightArea) {
        S.HighlightImportantArea(a, b);
    }

    const auto Check = [&](const auto& e) { return e.IsWithinArea(a.x, a.y, b.x, b.y); };
    return ped.IsInVehicle()
        ? Check(*ped.m_pVehicle)
        : Check(ped);
}

auto IsCharInArea3D(CRunningScript& S, CPed& ped, CVector a, CVector b, bool highlightArea) {
    if (highlightArea) {
        S.HighlightImportantArea(a, b);
    }

    const auto Check = [&](const auto& e) { return e.IsWithinArea(a.x, a.y, a.z, b.x, b.y, b.z); };
    return ped.IsInVehicle()
        ? Check(*ped.m_pVehicle)
        : Check(ped);
}

auto StoreCarCharIsIn(CRunningScript& S, CPed& ped) { // 0x469481
    const auto veh = ped.GetVehicleIfInOne();

    if (GetVehiclePool()->GetRef(veh) != CTheScripts::StoreVehicleIndex && S.m_bUseMissionCleanup) {
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

auto IsCharInCar(CPed& ped, CVehicle& veh) {
    return ped.IsInVehicle() && ped.m_pVehicle == &veh;
}

auto IsCharInModel(CPed& ped, eModelID model) {
    return ped.IsInVehicle() && ped.m_pVehicle->m_nModelIndex == model;
}

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

auto LocateCharAnyMeans2D(CRunningScript& S, CPed& ped, CVector2D pos, CVector2D radius, bool highlightArea) {
    return LocateChar2D(S, ped, pos, radius, highlightArea, false, false, false);
}

auto LocateCharOnFoot2D(CRunningScript& S, CPed& ped, CVector2D pos, CVector2D radius, bool highlightArea) {
    return LocateChar2D(S, ped, pos, radius, highlightArea, false, true, false);
}

auto LocateCharInCar2D(CRunningScript& S, CPed& ped, CVector2D pos, CVector2D radius, bool highlightArea) {
    return LocateChar2D(S, ped, pos, radius, highlightArea, true, false, false);
}

auto LocateStoppedCharAnyMeans2D(CRunningScript& S, CPed& ped, CVector2D pos, CVector2D radius, bool highlightArea) {
    return LocateChar2D(S, ped, pos, radius, highlightArea, false, false, true);
}

auto LocateStoppedCharOnFoot2D(CRunningScript& S, CPed& ped, CVector2D pos, CVector2D radius, bool highlightArea) {
    return LocateChar2D(S, ped, pos, radius, highlightArea, false, true, true);
}

auto LocateStoppedCharInCar2D(CRunningScript& S, CPed& ped, CVector2D pos, CVector2D radius, bool highlightArea) {
    return LocateChar2D(S, ped, pos, radius, highlightArea, true, false, true);
}

// => 3D

auto LocateCharAnyMeans3D(CRunningScript& S, CPed& ped, CVector pos, CVector radius, bool highlightArea) {
    return LocateChar3D(S, ped, pos, radius, highlightArea, false, false, false);
}

auto LocateCharOnFoot3D(CRunningScript& S, CPed& ped, CVector pos, CVector radius, bool highlightArea) {
    return LocateChar3D(S, ped, pos, radius, highlightArea, false, true, false);
}

auto LocateCharInCar3D(CRunningScript& S, CPed& ped, CVector pos, CVector radius, bool highlightArea) {
    return LocateChar3D(S, ped, pos, radius, highlightArea, true, false, false);
}

auto LocateStoppedCharAnyMeans3D(CRunningScript& S, CPed& ped, CVector pos, CVector radius, bool highlightArea) {
    return LocateChar3D(S, ped, pos, radius, highlightArea, false, false, true);
}

auto LocateStoppedCharOnFoot3D(CRunningScript& S, CPed& ped, CVector pos, CVector radius, bool highlightArea) {
    return LocateChar3D(S, ped, pos, radius, highlightArea, false, true, true);
}

auto LocateStoppedCharInCar3D(CRunningScript& S, CPed& ped, CVector pos, CVector radius, bool highlightArea) {
    return LocateChar3D(S, ped, pos, radius, highlightArea, true, false, true);
}

//
// Char, Char
//

// => 2D

auto LocateCharAnyMeansChar2D(CRunningScript& S, CPed& ped1, CPed& ped2, CVector2D radius, bool highlightArea) {
    return LocateCharEntity2D(S, ped1, ped2, radius, highlightArea, false, false);
}

auto LocateCharOnFootChar2D(CRunningScript& S, CPed& ped1, CPed& ped2, CVector2D radius, bool highlightArea) {
    return LocateCharEntity2D(S, ped1, ped2, radius, highlightArea, false, true);
}

auto LocateCharInCarChar2D(CRunningScript& S, CPed& ped1, CPed& ped2, CVector2D radius, bool highlightArea) {
    return LocateCharEntity2D(S, ped1, ped2, radius, highlightArea, true, false);
}

// => 3D

auto LocateCharAnyMeansChar3D(CRunningScript& S, CPed& ped1, CPed& ped2, CVector radius, bool highlightArea) {
    return LocateCharEntity3D(S, ped1, ped2, radius, highlightArea, false, false);
}

auto LocateCharOnFootChar3D(CRunningScript& S, CPed& ped1, CPed& ped2, CVector radius, bool highlightArea) {
    return LocateCharEntity3D(S, ped1, ped2, radius, highlightArea, false, true);
}

auto LocateCharInCarChar3D(CRunningScript& S, CPed& ped1, CPed& ped2, CVector radius, bool highlightArea) {
    return LocateCharEntity3D(S, ped1, ped2, radius, highlightArea, true, false);
}

//
// Char, Car
//

// => 2D

auto LocateCharAnyMeansCar2D(CRunningScript& S, CPed& ped, CVehicle& veh, CVector2D radius, bool highlightArea) {
    return LocateCharEntity2D(S, ped, veh, radius, highlightArea, false, false);
}

auto LocateCharOnFootCar2D(CRunningScript& S, CPed& ped, CVehicle& veh, CVector2D radius, bool highlightArea) {
    return LocateCharEntity2D(S, ped, veh, radius, highlightArea, false, true);
}

auto LocateCharInCarCar2D(CRunningScript& S, CPed& ped, CVehicle& veh, CVector2D radius, bool highlightArea) {
    return LocateCharEntity2D(S, ped, veh, radius, highlightArea, true, false);
}

// => 3D

auto LocateCharAnyMeansCar3D(CRunningScript& S, CPed& ped, CVehicle& veh, CVector radius, bool highlightArea) {
    return LocateCharEntity3D(S, ped, veh, radius, highlightArea, false, false);
}

auto LocateCharOnFootCar3D(CRunningScript& S, CPed& ped, CVehicle& veh, CVector radius, bool highlightArea) {
    return LocateCharEntity3D(S, ped, veh, radius, highlightArea, false, true);
}

auto LocateCharInCarCar3D(CRunningScript& S, CPed& ped, CVehicle& veh, CVector radius, bool highlightArea) {
    return LocateCharEntity3D(S, ped, veh, radius, highlightArea, true, false);
}

//
// Char, Obj
//

// => 2D

auto LocateCharAnyMeansObject2D(CRunningScript& S, CPed& ped, CObject& obj, CVector2D radius, bool highlightArea) {
    return LocateCharEntity2D(S, ped, obj, radius, highlightArea, false, false);
}

auto LocateCharOnFootObject2D(CRunningScript& S, CPed& ped, CObject& obj, CVector2D radius, bool highlightArea) {
    return LocateCharEntity2D(S, ped, obj, radius, highlightArea, false, true);
}

auto LocateCharInCarObject2D(CRunningScript& S, CPed& ped, CObject& obj, CVector2D radius, bool highlightArea) {
    return LocateCharEntity2D(S, ped, obj, radius, highlightArea, true, false);
}

// => 3D

auto LocateCharAnyMeansObject3D(CRunningScript& S, CPed& ped, CObject& obj, CVector radius, bool highlightArea) {
    return LocateCharEntity3D(S, ped, obj, radius, highlightArea, false, false);
}

auto LocateCharOnFootObject3D(CRunningScript& S, CPed& ped, CObject& obj, CVector radius, bool highlightArea) {
    return LocateCharEntity3D(S, ped, obj, radius, highlightArea, false, true);
}

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

// IS_CHAR_IN_ANGLED_AREA_2D
auto IsCharInAngledArea2D(CPed& ped, CVector2D a, CVector2D b, float widthAndDir, bool highlightArea) {
    return IsPedInAngledArea2D(ped, a, b, widthAndDir, highlightArea, false, false, false);
}

// IS_CHAR_IN_ANGLED_AREA_ON_FOOT_2D
auto IsCharInAngledAreaOnFoot2D(CPed& ped, CVector2D a, CVector2D b, float widthAndDir, bool highlightArea) {
    return IsPedInAngledArea2D(ped, a, b, widthAndDir, highlightArea, false, true, false);
}

// IS_CHAR_IN_ANGLED_AREA_IN_CAR_2D
auto IsCharInAngledAreaInCar2D(CPed& ped, CVector2D a, CVector2D b, float widthAndDir, bool highlightArea) {
    return IsPedInAngledArea2D(ped, a, b, widthAndDir, highlightArea, true, false, false);
}

// IS_CHAR_STOPPED_IN_ANGLED_AREA_2D
auto IsCharStoppedInAngledArea2D(CPed& ped, CVector2D a, CVector2D b, float widthAndDir, bool highlightArea) {
    return IsPedInAngledArea2D(ped, a, b, widthAndDir, highlightArea, false, false, true);
}

// IS_CHAR_STOPPED_IN_ANGLED_AREA_ON_FOOT_2D
auto IsCharStoppedInAngledAreaOnFoot2D(CPed& ped, CVector2D a, CVector2D b, float widthAndDir, bool highlightArea) {
    return IsPedInAngledArea2D(ped, a, b, widthAndDir, highlightArea, false, true, true);
}

// IS_CHAR_STOPPED_IN_ANGLED_AREA_IN_CAR_2D
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

// IS_CHAR_IN_ANGLED_AREA_3D
auto IsCharInAngledArea3D(CPed& ped, CVector a, CVector b, float widthAndDir, bool highlightArea) {
    return IsPedInAngledArea3D(ped, a, b, widthAndDir, highlightArea, false, false, false);
}

// IS_CHAR_IN_ANGLED_AREA_ON_FOOT_3D
auto IsCharInAngledAreaOnFoot3D(CPed& ped, CVector a, CVector b, float widthAndDir, bool highlightArea) {
    return IsPedInAngledArea3D(ped, a, b, widthAndDir, highlightArea, false, true, false);
}

// IS_CHAR_IN_ANGLED_AREA_IN_CAR_3D
auto IsCharInAngledAreaInCar3D(CPed& ped, CVector a, CVector b, float widthAndDir, bool highlightArea) {
    return IsPedInAngledArea3D(ped, a, b, widthAndDir, highlightArea, true, false, false);
}

// IS_CHAR_STOPPED_IN_ANGLED_AREA_3D
auto IsCharStoppedInAngledArea3D(CPed& ped, CVector a, CVector b, float widthAndDir, bool highlightArea) {
    return IsPedInAngledArea3D(ped, a, b, widthAndDir, highlightArea, false, false, true);
}

// IS_CHAR_STOPPED_IN_ANGLED_AREA_ON_FOOT_3D
auto IsCharStoppedInAngledAreaOnFoot3D(CPed& ped, CVector a, CVector b, float widthAndDir, bool highlightArea) {
    return IsPedInAngledArea3D(ped, a, b, widthAndDir, highlightArea, false, true, true);
}

// IS_CHAR_STOPPED_IN_ANGLED_AREA_IN_CAR_3D
auto IsCharStoppedInAngledAreaInCar3D(CPed& ped, CVector a, CVector b, float widthAndDir, bool highlightArea) {
    return IsPedInAngledArea3D(ped, a, b, widthAndDir, highlightArea, true, false, true);
}

auto IsCharDead(CPed* ped) {
    return !ped || ped->IsStateDeadForScript();
}

// IS_CHAR_IN_ZONE
auto IsCharInZone(CPed& ped, std::string_view zoneName) {
    return CTheZones::FindZone(GetCharCoordinates(ped), zoneName, ZONE_TYPE_NAVI);
}

// GET_CHAR_HEADING
auto GetCharHeading(CPed& ped) {
    return FixAngleDegrees(RadiansToDegrees(GetPedOrItsVehicle(ped).GetHeading()));
}

// SET_CHAR_HEADING
auto SetCharHeading(CPed& ped, float deg) {
    if (ped.IsInVehicle()) {
        return;
    }

    const auto rad = DegreesToRadians(FixAngleDegrees(deg));
    ped.m_fAimingRotation = ped.m_fCurrentRotation = rad;
    ped.SetHeading(rad);
    ped.UpdateRW();
}

// IS_CHAR_TOUCHING_OBJECT
auto IsCharTouchingObject(CPed& ped, CObject& obj) {
    return GetPedOrItsVehicle(ped).GetHasCollidedWith(&obj);
}

// SET_CHAR_AMMO
auto SetCharAmmo(CPed& ped, eWeaponType wep, int32 nammo) {
    ped.SetAmmo(wep, nammo);
}

// IS_CHAR_HEALTH_GREATER
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
    if (S.m_bUseMissionCleanup) {
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

// SET_CHAR_HEALTH
auto SetCharHealth(CPed& ped, float health) {
    if (health != 0.f) {
        if (ped.IsPlayer()) {
            ped.m_fHealth = std::min(health, ped.m_fMaxHealth);
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

// GET_CHAR_HEALTH
auto GetCharHealth(CPed& ped) {
    return ped.m_fHealth;
}

// IS_CHAR_TOUCHING_OBJECT_ON_FOOT
auto IsCharTouchingObjectOnFoot(CPed& ped, CObject& obj) {
    return !ped.IsInVehicle() && ped.GetHasCollidedWith(&obj);
}

// IS_CHAR_STOPPED
auto IsCharStopped(CPed& ped) {
    return CTheScripts::IsPedStopped(&ped);
}

// SET_CHAR_ONLY_DAMAGED_BY_PLAYER
auto SetCharOnlyDamagedByPlayer(CPed& ped, bool enabled) {
    ped.physicalFlags.bInvulnerable = enabled;
}

// GET_CLOSEST_CHAR_NODE
auto GetClosestCharNode(CVector pos) -> CVector {
    CWorld::PutToGroundIfTooLow(pos);
    if (const auto node = ThePaths.FindNodeClosestToCoors(pos)) {
        return ThePaths.GetPathNode(node)->GetPosition();
    }
    return {}; // Can't find anything nearby
}

// IS_CHAR_ON_SCREEN
auto IsCharOnScreen(CPed& ped) {
    return ped.GetIsOnScreen();
}

// IS_CHAR_SHOOTING_IN_AREA
auto IsCharShootingInArea(CRunningScript& S, CPed& ped, CRect area, bool highlightArea) {
    if (highlightArea) {
        S.HighlightImportantArea(area);
    }

    if (CTheScripts::DbgFlag) {
        CTheScripts::DrawDebugSquare(area);
    }

    return ped.bFiringWeapon && area.IsPointInside(ped.GetPosition2D());
}

// IS_CURRENT_CHAR_WEAPON
auto IsCurrentCharWeapon(CPed& ped, eWeaponType wep) {
    if (wep == WEAPON_ANYMELEE && ped.GetActiveWeapon().IsTypeMelee()) {
        return true;
    }
    return ped.GetActiveWeapon().m_Type == wep;
}

// GET_RANDOM_CHAR_IN_ZONE
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
        if (S.m_bUseMissionCleanup) {
            CTheScripts::MissionCleanUp.AddEntityToList(ped);
        }

        // They forgot to return here (did it instead after the loop), so let's do that
        return &ped;
    }
    return nullptr; // No suitable ped found
}

// IS_CHAR_MODEL
auto IsCharModel(CPed& ped, int8 accuracy) {
    ped.m_nWeaponAccuracy = accuracy;
}

// HAS_CHAR_BEEN_DAMAGED_BY_WEAPON
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

// EXPLODE_CHAR_HEAD
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

// START_CHAR_FIRE
auto StartCharFire(CPed& ped) { // TODO: return type: ScriptThing<CFire>
    return gFireManager.StartScriptFire(ped.GetPosition(), &ped, 0.8f, true, 0, 1);
}

// SET_CHAR_VISIBLE
auto SetCharVisible(CPed& ped, bool isVisible) {
    if (&ped == FindPlayerPed()) {
        gPlayerPedVisible = isVisible;
    }
    ped.m_bIsVisible = isVisible;
}

// REMOVE_CHAR_ELEGANTLY
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
    if (S.m_bUseMissionCleanup) {
        CTheScripts::MissionCleanUp.RemoveEntityFromList(*ped);
    }
}

// SET_CHAR_STAY_IN_SAME_PLACE
auto SetCharStayInSamePlace(CPed& ped, bool bStay) {
    ped.SetStayInSamePlace(bStay);
}

// WARP_CHAR_FROM_CAR_TO_COORD
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

// HAS_CHAR_SPOTTED_CHAR
auto HasCharSpottedChar(CPed& ped, CPed& target) {
    return ped.OurPedCanSeeThisEntity(&target, true);
}

// WARP_CHAR_INTO_CAR
auto WarpCharIntoCar(CPed& ped, CVehicle& veh) {
    ped.GetIntelligence()->FlushImmediately(false);
    CTaskSimpleCarSetPedInAsDriver{ &veh, true, nullptr }.ProcessPed(&ped);
}

// SET_CHAR_ANIM_SPEED
auto SetCharAnimSpeed(CPed& ped, const char* animName, float speed) {
    if (const auto anim = RpAnimBlendClumpGetAssociation(ped.m_pRwClump, animName)) {
        anim->SetSpeed(speed);
    }
}

// IS_CHAR_MALE
auto IsCharMale(CPed& ped) {
    return !IsPedTypeFemale(ped.m_nPedType);
}

// STORE_CAR_CHAR_IS_IN_NO_SAVE
auto StoreCarCharIsInNoSave(CPed& ped) -> CVehicle& { 
    assert(ped.bInVehicle && ped.m_pVehicle); // Original code had a bug (namely, calling VehiclePool::GetRef() with nullptr, very bad)
    return *ped.m_pVehicle;
}

// SET_CHAR_MONEY
auto SetCharMoney(CPed& ped, int16 money) {
    ped.bMoneyHasBeenGivenByScript = true;
    ped.m_nMoneyCount = money;
}

// GET_AMMO_IN_CHAR_WEAPON
auto GetAmmoInCharWeapon(CPed& ped, eWeaponType wtype) -> uint32 {
    for (auto& wep : ped.m_aWeapons) { 
        if (wep.m_Type == wtype) { // Originally they continued looping, but that doesn't make sense (A ped can't have the same weapon _twice_)
            return wep.m_TotalAmmo;
        }
    }
    return 0;
}

// WARP_CHAR_INTO_CAR_AS_PASSENGER
auto WarpCharIntoCarAsPassenger(CPed& ped, CVehicle& veh, int32 psgrSeatIdx) {
    if (psgrSeatIdx >= 0) {
        psgrSeatIdx = CCarEnterExit::ComputeTargetDoorToEnterAsPassenger(&veh, psgrSeatIdx);
    }
    ped.GetIntelligence()->FlushImmediately(false);
    CTaskSimpleCarSetPedInAsPassenger{ &veh, (eTargetDoor)(psgrSeatIdx), true }.ProcessPed(&ped); // Warp ped into car
}

// GET_CHAR_IN_CAR_PASSENGER_SEAT - TODO: Move to `Vehicle`
auto GetCharInCarPassengerSeat(CVehicle& veh, uint32 psgrSeatIdx) -> CPed& {
    return *veh.m_apPassengers[psgrSeatIdx];
}

bool HasPedSittingInCarTask(CPed& ped) {
    return ped.GetTaskManager().IsSimplestActiveTaskOfType({ TASK_SIMPLE_CAR_DRIVE, TASK_SIMPLE_GANG_DRIVEBY });
}

// IS_CHAR_SITTING_IN_CAR
auto IsCharSittingInCar(CPed& ped, CVehicle& veh) {
    return ped.bInVehicle && ped.m_pVehicle == &veh && HasPedSittingInCarTask(ped);
}

// IS_CHAR_SITTING_IN_ANY_CAR
auto IsCharSittingInAnyCar(CPed& ped) {
    return ped.bInVehicle && HasPedSittingInCarTask(ped);
}

// IS_CHAR_ON_FOOT
auto IsCharOnFoot(CPed& ped) {
    return !ped.bInVehicle && !ped.GetTaskManager().HasAnyOf<TASK_COMPLEX_ENTER_CAR_AS_PASSENGER, TASK_COMPLEX_ENTER_CAR_AS_DRIVER>();
}

// ATTACH_CHAR_TO_CAR
auto AttachCharToCar(CPed& ped, CVehicle& veh, CVector offset, int32 pos, float angleLimitDeg, eWeaponType wtype) {
    ped.AttachPedToEntity(
        &veh,
        offset,
        pos,
        DegreesToRadians(angleLimitDeg),
        wtype
    );
}

// DETACH_CHAR_FROM_CAR
auto DetachCharFromCar(CPed* ped) {
    if (ped && ped->m_pAttachedTo) {
        ped->DettachPedFromEntity();
    }
}

// CLEAR_CHAR_LAST_WEAPON_DAMAGE
auto ClearCharLastWeaponDamage(CPed& ped) {
    ped.m_nLastWeaponDamage = -1;
}

// GET_CURRENT_CHAR_WEAPON
auto GetCurrentCharWeapon(CPed& ped) {
    return ped.GetActiveWeapon().m_Type;
}

// CAN_CHAR_SEE_DEAD_CHAR
auto CanCharSeeDeadChar(CPed& ped) {
    for (auto& nearbyPed : ped.GetIntelligence()->GetPedScanner().GetEntities<CPed>()) {
        if (!nearbyPed.IsAlive() && CPedGeometryAnalyser::CanPedTargetPed(ped, nearbyPed, true)) {
            return true;
        }
    }
    return false;
}

// SHUT_CHAR_UP
auto ShutCharUp(CPed& ped, bool stfu) {
    if (stfu) {
        ped.DisablePedSpeech(false);
    } else {
        ped.EnablePedSpeech();
    }
}

// REMOVE_ALL_CHAR_WEAPONS
auto RemoveAllCharWeapons(CPed& ped) {
    ped.ClearWeapons();
}

// HAS_CHAR_GOT_WEAPON
auto HasCharGotWeapon(CPed& ped, eWeaponType wtype) {
    return notsa::contains(ped.m_aWeapons, wtype, [](CWeapon& w) { return w.m_Type; });
}

// GET_DEAD_CHAR_PICKUP_COORDS
auto GetDeadCharPickupCoords(CPed& ped) {
    CVector pos;
    ped.CreateDeadPedPickupCoors(pos);
    return pos;
}

auto IsPedInVehicleOfAppearance(CPed& ped, eVehicleAppearance appr) {
    return ped.IsInVehicle() && ped.m_pVehicle->GetVehicleAppearance() == appr;
}

// IS_CHAR_ON_ANY_BIKE
auto IsCharOnAnyBike(CPed& ped) {
    return IsPedInVehicleOfAppearance(ped, VEHICLE_APPEARANCE_BIKE);
}

// IS_CHAR_IN_ANY_BOAT
auto IsCharInAnyBoat(CPed& ped) {
    return IsPedInVehicleOfAppearance(ped, VEHICLE_APPEARANCE_BOAT);
}

// IS_CHAR_IN_ANY_HELI
auto IsCharInAnyHeli(CPed& ped) {
    return IsPedInVehicleOfAppearance(ped, VEHICLE_APPEARANCE_HELI);
}

// IS_CHAR_IN_ANY_PLANE
auto IsCharInAnyPlane(CPed& ped) {
    return IsPedInVehicleOfAppearance(ped, VEHICLE_APPEARANCE_PLANE);
}

// IS_CHAR_IN_WATER
auto IsCharInWater(CPed* ped) {
    return ped && ped->physicalFlags.bSubmergedInWater;
}

// GET_CHAR_WEAPON_IN_SLOT
auto GetCharWeaponInSlot(CPed& ped, eWeaponSlot slut) {
    const auto& wep = ped.GetWeaponInSlot(slut);
    return notsa::script::return_multiple(wep.m_Type, wep.m_TotalAmmo, CPickups::ModelForWeapon(wep.m_Type));
}

// GET_OFFSET_FROM_CHAR_IN_WORLD_COORDS
auto GetOffsetFromCharInWorldCoords(CPed& ped, CVector offset) {
    return ped.GetMatrix().TransformPoint(offset);
}

// HAS_CHAR_BEEN_PHOTOGRAPHED
auto HasCharBeenPhotographed(CPed* ped) {
    if (ped) {
        if (ped->bHasBeenPhotographed) {
            ped->bHasBeenPhotographed = false;
            return true;
        }
    }
    return false;
}

// IS_CHAR_IN_FLYING_VEHICLE
auto IsCharInFlyingVehicle(CPed& ped) {
    return IsCharInAnyHeli(ped) || IsCharInAnyPlane(ped);
}

// GET_CHAR_ARMOUR
auto GetCharArmour(CPed& ped) {
    return ped.m_fArmour;
}

// ATTACH_CHAR_TO_OBJECT
auto AttachCharToObject(CPed& ped, CObject& obj, CVector offset, int32 orientation, float angleLimitDeg, eWeaponType wtype) {
    ped.AttachPedToEntity(&obj, offset, orientation, DegreesToRadians(angleLimitDeg), wtype);
}

// HAS_CHAR_BEEN_DAMAGED_BY_CHAR
auto HasCharBeenDamagedByChar(CPed* ped, CPed& byPed) {
    if (!ped) {
        return false;
    }
    if (!ped->m_pLastEntityDamage) {
        return false;
    }
    if (ped->m_pLastEntityDamage == &byPed) {
        return true;
    }
    if (byPed.bInVehicle && ped->m_pLastEntityDamage == byPed.m_pVehicle) {
        return true;
    }
    return false;
}

// HAS_CHAR_BEEN_DAMAGED_BY_CAR
auto HasCharBeenDamagedByCar(CPed* ped, CVehicle& veh) {
    return ped && ped->m_pLastEntityDamage == &veh;
}

// IS_CHAR_TOUCHING_VEHICLE
auto IsCharTouchingVehicle(CPed& ped, CVehicle& withVeh) {
    return ped.bInVehicle
        ? ped.m_pVehicle->GetHasCollidedWith(&withVeh)
        : ped.GetHasCollidedWith(&withVeh);
}

// CLEAR_CHAR_LAST_DAMAGE_ENTITY
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
    if (S.m_bUseMissionCleanup) {
        CTheScripts::MissionCleanUp.AddEntityToList(*ped);
    }
    return *ped;
}

// CREATE_RANDOM_CHAR_AS_DRIVER
auto CreateRandomCharAsDriver(CRunningScript& S, CVehicle& veh) -> CPed& {
    return CreateRandomCharInVehicle(S, veh, true);
}

// CREATE_RANDOM_CHAR_AS_PASSENGER
auto CreateRandomCharAsPassenger(CRunningScript& S, CVehicle& veh, int32 psgrSeatIdx) -> CPed& {
    return CreateRandomCharInVehicle(S, veh, false, psgrSeatIdx);
}

// IS_CHAR_IN_ANY_POLICE_VEHICLE
auto IsCharInAnyPoliceVehicle(CPed& ped) {
    if (!ped.IsInVehicle()) {
        return false;
    }
    const auto veh = ped.m_pVehicle;
    return veh->IsLawEnforcementVehicle() && veh->m_nModelIndex != eModelID::MODEL_PREDATOR;
}

// DOES_CHAR_EXIST
auto DoesCharExist(CPed* ped) {
    return ped != nullptr;
}

auto SetCharAccuracy(CPed& ped, uint8 accuracy) {
    ped.m_nWeaponAccuracy = accuracy;
}

void DoSetPedIsWaitingForCollision(CRunningScript& S, CPed& ped) {
    if (S.m_bUseMissionCleanup) {
        CWorld::Remove(&ped);
        ped.m_bIsStaticWaitingForCollision = true;
        CWorld::Add(&ped);
    }
}

// FREEZE_CHAR_POSITION_AND_DONT_LOAD_COLLISION
auto FreezeCharPositionAndDontLoadCollision(CRunningScript& S, CPed& ped, bool freeze) {
    ped.physicalFlags.bDontApplySpeed = freeze;
    if (freeze) {
        DoSetPedIsWaitingForCollision(S, ped);
    }
}

// SET_LOAD_COLLISION_FOR_CHAR_FLAG
auto SetLoadCollisionForCharFlag(CRunningScript& S, CPed& ped, bool loadCol) {
    ped.physicalFlags.b15 = !loadCol;
    if (loadCol) {
        DoSetPedIsWaitingForCollision(S, ped);
    } else if (ped.m_bIsStaticWaitingForCollision) {
        ped.m_bIsStaticWaitingForCollision = false;
        if (!ped.IsStatic()) { // TODO: I think this is inlined
            ped.AddToMovingList();
        }
    }
}

// TASK_KILL_CHAR_ON_FOOT
auto TaskKillCharOnFoot(CRunningScript& S, eScriptCommands opcode, CPed& ped, CPed& target) {
    S.GivePedScriptedTask(
        &ped,
        new CTaskComplexKillPedOnFoot{ &target, -1, 0, 0, 0, true },
        (int32)(opcode)
    );
}

// IS_CHAR_IN_TAXI
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

// LOAD_CHAR_DECISION_MAKER
auto LoadCharDecisionMaker(CRunningScript& S, int32 type) { // TODO: return ScriptThing<CDecisionMaker>
    char pedDMName[1024];
    CDecisionMakerTypesFileLoader::GetPedDMName(type, pedDMName);
    const auto id = CDecisionMakerTypesFileLoader::LoadDecisionMaker(pedDMName, DECISION_ON_FOOT, S.m_bUseMissionCleanup);
    const auto handle = CTheScripts::GetNewUniqueScriptThingIndex(id, SCRIPT_THING_DECISION_MAKER);
    if (S.m_bUseMissionCleanup) {
        CTheScripts::MissionCleanUp.AddEntityToList(handle, MISSION_CLEANUP_ENTITY_TYPE_DECISION_MAKER);
    }
    return handle;
}

// SET_CHAR_DECISION_MAKER
auto SetCharDecisionMaker(CPed& ped, int32 scriptHandleOfDM) { // TODO: Use `ScriptThing<CDecisionMaker>` instead of `int32` for `scriptHandleOfDM`
    ped.GetIntelligence()->SetPedDecisionMakerType(
        scriptHandleOfDM == -1
            ? -1
            : CTheScripts::GetActualScriptThingIndex(scriptHandleOfDM, SCRIPT_THING_DECISION_MAKER)
    );
}

// IS_CHAR_PLAYING_ANIM
auto IsCharPlayingAnim(CPed& ped, const char* animName) {
    return RpAnimBlendClumpGetAssociation(ped.m_pRwClump, animName) != nullptr;
}

// SET_CHAR_ANIM_PLAYING_FLAG
auto SetCharAnimPlayingFlag(CPed& ped, const char* animName, bool started) {
    if (const auto anim = RpAnimBlendClumpGetAssociation(ped.m_pRwClump, animName)) {
        anim->SetFlag(ANIMATION_IS_PLAYING, started);
    }
}

// GET_CHAR_ANIM_CURRENT_TIME
auto GetCharAnimCurrentTime(CPed& ped, const char* animName) {
    if (const auto anim = RpAnimBlendClumpGetAssociation(ped.m_pRwClump, animName)) {
        return anim->m_CurrentTime / anim->m_BlendHier->m_fTotalTime;
    }
    return 0.f;
}

// SET_CHAR_ANIM_CURRENT_TIME
auto SetCharAnimCurrentTime(CPed& ped, const char* animName, float progress) {
    if (const auto anim = RpAnimBlendClumpGetAssociation(ped.m_pRwClump, animName)) {
        anim->SetCurrentTime(progress * anim->m_BlendHier->m_fTotalTime);
    }
}

// GET_CHAR_ANIM_TOTAL_TIME (In seconds)
auto GetCharAnimTotalTime(CPed& ped, const char* animName) {
    if (const auto anim = RpAnimBlendClumpGetAssociation(ped.m_pRwClump, animName)) {
        return anim->m_BlendHier->m_fTotalTime * 1000.f;
    }
    return 0.f;
}

// CREATE_CHAR_AT_ATTRACTOR
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

// TASK_KILL_CHAR_ON_FOOT_WHILE_DUCKING
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

// TASK_TURN_CHAR_TO_FACE_CHAR
auto TaskTurnCharToFaceChar(eScriptCommands command, CRunningScript& S, CPed& ped, CPed& target) {
    S.GivePedScriptedTask(
        &ped,
        new CTaskComplexTurnToFaceEntityOrCoord{ &ped },
        command
    );
}

// IS_CHAR_AT_SCRIPTED_ATTRACTOR
auto IsCharAtScriptedAttractor(CPed* ped, C2dEffectPedAttractor* attractor) {
    if (!attractor) {
        return false;
    }
    const auto pedUsingAttractor = GetPedAttractorManager()->GetPedUsingEffect(reinterpret_cast<C2dEffect*>(attractor));
    return ped
        ? pedUsingAttractor == ped
        : pedUsingAttractor != nullptr;
}

// GET_CHAR_MODEL
auto GetCharModel(CPed& ped) {
    return (eModelID)(ped.m_nModelIndex);
}

// SET_CURRENT_CHAR_WEAPON
void SetCurrentCharWeapon(CPed& ped, eWeaponType weaponType) {
    for (auto&& [slot, weapon] : notsa::enumerate(ped.m_aWeapons)) {
        if (weapon.m_Type != weaponType)
            continue;

        if (ped.IsPlayer()) {
            ped.AsPlayer()->m_pPlayerData->m_nChosenWeapon = slot;
        } else {
            ped.SetCurrentWeapon(slot);
        }
    }
}

// MARK_CHAR_AS_NO_LONGER_NEEDED
void MarkCharAsNoLongerNeeded(CRunningScript& S, int32 handle) { // TODO: Some way to get a CPed* and it's handle too (As this function seems to be called even if the handle is not pointing to a ped anymore)
    const auto ped = GetPedPool()->GetAtRef(handle); // This might be null, but we need the handle even if it is, so we can't take `CPed*` either...
    CTheScripts::CleanUpThisPed(ped);
    if (S.m_bUseMissionCleanup) {
        CTheScripts::MissionCleanUp.RemoveEntityFromList(handle, MISSION_CLEANUP_ENTITY_TYPE_PED);
    }
}

// GET_CHAR_SPEED
float GetCharSpeed(CPed& ped) {
    return ped.GetMoveSpeed().Magnitude() * 50.0f;
}

// IS_CHAR_IN_SEARCH_LIGHT
bool IsCharInSearchlight(uint32 searchLightIdx, CPed& ped) {
    return CSearchLight::IsSpottedEntity(searchLightIdx, ped);
}

// REMOVE_CHAR_FROM_GROUP
void RemoveCharFromGroup(CPed& ped) {
    if (auto pedGroup = ped.GetGroup(); pedGroup && !pedGroup->GetMembership().IsLeader(&ped)) {
        pedGroup->GetMembership().RemoveMember(&ped);
        pedGroup->Process();
    }
}

// ATTACH_CHAR_TO_BIKE
void AttachCharToBike(CPed& ped, CVehicle& bike, CVector posn, uint16 heading, float headingLimit, float verticalLimit, eWeaponType weapon) {
    // turret on car?
    ped.AttachPedToBike(&bike, posn, heading, DegreesToRadians(headingLimit), DegreesToRadians(verticalLimit), weapon);
}

// GET_CAR_CHAR_IS_USING
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

// ENABLE_CHAR_SPEECH
void EnableCharSpeech(CPed& ped) {
    ped.EnablePedSpeech();
}

// DISABLE_CHAR_SPEECH
void DisableCharSpeech(CPed& ped, bool stopCurrentSpeech) {
    ped.DisablePedSpeech(stopCurrentSpeech);
}

// SET_CHAR_WANTED_BY_POLICE
void SetCharWantedByPolice(CPed& ped, bool state) {
    ped.bWantedByPolice = state;
}

// SET_CHAR_SIGNAL_AFTER_KILL
void SetCharSignalAfterKill(CPed& ped, bool state) {
    ped.bSignalAfterKill = state;
}

// SET_CHAR_COORDINATES_DONT_WARP_GANG_NO_OFFSET
void SetCharCoordinatesDontWarpGangNoOffset(CRunningScript& S, CPed& ped, CVector posn) {
    S.SetCharCoordinates(ped, posn, false, false);
}

// IS_CHAR_USING_MAP_ATTRACTOR
bool IsCharUsingMapAttractor(CPed& ped) {
    return GetPedAttractorManager()->IsPedRegisteredWithEffect(&ped);
}

// todo: move that to somewhere else
eTargetDoor ComputeTargetDoorToExit(const CVehicle& vehicle, const CPed& ped) {
    return plugin::CallAndReturn<eTargetDoor, 0x64F110, const CVehicle&, const CPed&>(vehicle, ped);
}

// REMOVE_CHAR_FROM_CAR_MAINTAIN_POSITION
void RemoveCharFromCarMaintainPosition(CPed& ped, CVehicle& vehicle) {
    const auto pedPos = ped.GetPosition();
    CTaskSimpleCarSetPedOut task(&vehicle, ComputeTargetDoorToExit(vehicle, ped), false);
    task.ProcessPed(&ped);
    ped.SetPosn(pedPos); // ?
}

// SET_CHAR_SAY_CONTEXT_IMPORTANT
int16 SetCharSayContextImportant(CPed& ped, uint16 phraseId, bool overrideSilence, bool isForceAudible, bool isFrontEnd) {
    return ped.Say((eGlobalSpeechContext)phraseId, 0u, 1.0f, overrideSilence, isForceAudible, isFrontEnd);
}

// SET_CHAR_SAY_SCRIPT - 0x47ABC0
void SetCharSayScript(CPed& ped, eAudioEvents scriptID, bool overrideSilence, bool isForceAudible, bool isFrontEnd) {
    ped.SayScript(scriptID, overrideSilence, isForceAudible, isFrontEnd);
}

// IS_CHAR_GETTING_IN_INTO_A_CAR
bool IsCharGettingInToACar(CPed& ped) {
    return ped.GetTaskManager().FindActiveTaskFromList({
        TASK_COMPLEX_ENTER_CAR_AS_DRIVER,
        TASK_COMPLEX_ENTER_CAR_AS_PASSENGER,
        TASK_COMPLEX_GO_TO_CAR_DOOR_AND_STAND_STILL
    });
}

// GET_CHAR_AREA_VISIBLE
uint32 GetCharAreaVisible(CPed& ped) {
    return ped.m_nAreaCode != eAreaCodes::AREA_CODE_NORMAL_WORLD;
}

// HAS_CHAR_SPOTTED_CHAR_IN_FRONT
bool HasCharSpottedCharInFront(CPed& ped, CPed& other) {
    return ped.OurPedCanSeeThisEntity(&other, true);
}

// SHUT_CHAR_UP_FOR_SCRIPTED_SPEECH
void ShutCharUpForScriptedSpeech(CPed* ped, bool disable) {
    if (!ped)
        return;

    ped->DisablePedSpeechForScriptSpeech(disable);
}

// IS_CHAR_TOUCHING_CHAR
bool IsCharTouchingChar(CPed& ped, CPed& other) {
    const auto GetRelevantEntity = [](CPed& _ped) -> CPhysical* {
        return _ped.IsInVehicle() ? _ped.m_pVehicle->AsPhysical() : _ped.AsPhysical();
    };
    return GetRelevantEntity(ped)->GetHasCollidedWith(GetRelevantEntity(other));
}

// IS_CHAR_ATTACHED_TO_ANY_CAR
bool IsCharAttachedToAnyCar(CPed* ped) {
    return ped && ped->m_nType == ENTITY_TYPE_VEHICLE;
}

// STORE_CAR_CHAR_IS_ATTACHED_TO_NO_SAVE
CVehicle* StoreCarCharIsAttachedToNoSave(CPed* ped) {
    if (ped->m_nType != ENTITY_TYPE_VEHICLE) {
        return nullptr;
    }
    return ped->m_pAttachedTo->AsVehicle();
}

void notsa::script::commands::character::RegisterHandlers() {
    REGISTER_COMMAND_HANDLER_BEGIN("Char");

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
    //REGISTER_COMMAND_HANDLER(COMMAND_CLEAR_CHAR_TASKS_IMMEDIATELY, ClearCharTasksImmediately);
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
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_CHAR_ARMOUR);
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
