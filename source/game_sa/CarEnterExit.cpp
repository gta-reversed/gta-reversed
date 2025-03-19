#include "StdInc.h"

#include "PedStats.h"
#include "CarEnterExit.h"
#include "TaskSimpleCarSetPedInAsDriver.h"
#include "TaskComplexDriveWander.h"
#include "TaskSimpleCarSetPedInAsPassenger.h"

/*
const float& CCarEnterExit::ms_fMaxSpeed_CanDragPedOut = *(float*)0x0;
const float& CCarEnterExit::ms_fMaxSpeed_PlayerCanDragPedOut = *(float*)0x0;
*/

void CCarEnterExit::InjectHooks() {
    RH_ScopedClass(CCarEnterExit);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(AddInCarAnim, 0x64F720);
    RH_ScopedInstall(CarHasDoorToClose, 0x64EE10);
    RH_ScopedInstall(CarHasDoorToOpen, 0x64EDD0);
    RH_ScopedInstall(CarHasOpenableDoor, 0x64EE50);
    RH_ScopedInstall(CarHasPartiallyOpenDoor, 0x64EE70);
    RH_ScopedInstall(ComputeDoorFlag, 0x64E550);
    RH_ScopedInstall(ComputeOppositeDoorFlag, 0x64E610);
    RH_ScopedInstall(ComputePassengerIndexFromCarDoor, 0x64F1E0);
    RH_ScopedInstall(ComputeSlowJackedPed, 0x64F070);
    RH_ScopedInstall(ComputeTargetDoorToEnterAsPassenger, 0x64F190);
    RH_ScopedInstall(ComputeTargetDoorToExit, 0x64F110);
    RH_ScopedInstall(GetNearestCarDoor, 0x6528F0);
    RH_ScopedInstall(GetNearestCarPassengerDoor, 0x650BB0, { .reversed = false });
    RH_ScopedInstall(GetPositionToOpenCarDoor, 0x64E740, { .reversed = false });
    RH_ScopedInstall(IsCarDoorInUse, 0x64EC90);
    RH_ScopedInstall(IsCarDoorReady, 0x64ED90);
    RH_ScopedInstall(IsCarQuickJackPossible, 0x64EF00);
    RH_ScopedInstall(IsCarSlowJackRequired, 0x64EF70);
    RH_ScopedInstall(IsClearToDriveAway, 0x6509B0);
    RH_ScopedInstall(IsPathToDoorBlockedByVehicleCollisionModel, 0x651210);
    RH_ScopedInstall(IsPedHealthy, 0x64EEE0);
    RH_ScopedInstall(IsPlayerToQuitCarEnter, 0x64F240, { .reversed = false });
    RH_ScopedInstall(IsRoomForPedToLeaveCar, 0x6504C0, { .reversed = false });
    RH_ScopedInstall(IsVehicleHealthy, 0x64EEC0);
    RH_ScopedInstall(IsVehicleStealable, 0x6510D0);
    RH_ScopedInstall(MakeUndraggedDriverPedLeaveCar, 0x64F600);
    RH_ScopedInstall(MakeUndraggedPassengerPedsLeaveCar, 0x64F540, { .reversed = false });
    RH_ScopedInstall(QuitEnteringCar, 0x650130);
    RH_ScopedInstall(RemoveCarSitAnim, 0x64F680);
    RH_ScopedInstall(RemoveGetInAnims, 0x64F6E0);
    RH_ScopedInstall(SetAnimOffsetForEnterOrExitVehicle, 0x64F860);
    RH_ScopedInstall(SetPedInCarDirect, 0x650280);
}

// 0x64F720
void CCarEnterExit::AddInCarAnim(const CVehicle* vehicle, CPed* ped, bool bAsDriver) {
    const auto [grpId, animId] = [&]() -> std::pair<AssocGroupId, AnimationId> {
        if (bAsDriver) { // Inverted
            if (const auto data = const_cast<CVehicle*>(vehicle)->GetRideAnimData()) {
                return { data->m_nAnimGroup, ANIM_ID_BIKE_RIDE };
            } else if (vehicle->IsBoat()) {
                if (vehicle->m_pHandlingData->m_bSitInBoat) {
                    return { ANIM_GROUP_DEFAULT, ANIM_ID_DRIVE_BOAT };
                }
            } else if (vehicle->vehicleFlags.bLowVehicle) {
                return { ANIM_GROUP_DEFAULT, ANIM_ID_CAR_LSIT };
            }

            return { ANIM_GROUP_DEFAULT, ANIM_ID_CAR_SIT };
        } else {
            if (const auto data = const_cast<CVehicle*>(vehicle)->GetRideAnimData()) {
                return { data->m_nAnimGroup, ANIM_ID_BIKE_RIDE };
            } else if (vehicle->vehicleFlags.bLowVehicle) {
                return { ANIM_GROUP_DEFAULT, ANIM_ID_CAR_SITPLO };
            }

            return { ANIM_GROUP_DEFAULT, ANIM_ID_CAR_SITP };
        }
    }();
    CAnimManager::BlendAnimation(ped->m_pRwClump, grpId, animId, 1000.f);
    ped->StopNonPartialAnims();
}

// 0x64EE10
bool CCarEnterExit::CarHasDoorToClose(const CVehicle* vehicle, int32 doorId) {
    auto& veh = const_cast<CVehicle&>(*vehicle);
    return !veh.IsDoorMissingU32(doorId) && !veh.IsDoorClosedU32(doorId);
}

// 0x64EDD0
bool CCarEnterExit::CarHasDoorToOpen(const CVehicle* vehicle, int32 doorId) {
    auto& veh = const_cast<CVehicle&>(*vehicle);
    return !veh.IsDoorMissingU32((uint32)doorId) && !veh.IsDoorFullyOpenU32((uint32)doorId);
}

// 0x64EE50
bool CCarEnterExit::CarHasOpenableDoor(const CVehicle* vehicle, int32 doorId_UnusedArg, const CPed* ped) {
    return vehicle->CanPedOpenLocks(ped);
}

// 0x64EE70
bool CCarEnterExit::CarHasPartiallyOpenDoor(const CVehicle* vehicle, int32 doorId) {
    auto& veh = const_cast<CVehicle&>(*vehicle); // TODO: Fix
    return !veh.IsDoorMissingU32((uint32)doorId)
        && !veh.IsDoorFullyOpenU32((uint32)doorId)
        && !veh.IsDoorClosedU32((uint32)doorId);
}

// 0x64E550
int32 CCarEnterExit::ComputeDoorFlag(const CVehicle* vehicle, int32 doorId, bool bSettingFlags) {
    if (bSettingFlags && (vehicle->IsBike() || vehicle->m_pHandlingData->m_bTandemSeats)) {
        switch (doorId) {
        case 8:
        case 10:
        case 18: return 5;
        case 9:
        case 11: return 10;
        default: NOTSA_UNREACHABLE(); // Originally `return 0`
        }
    } else {
        switch (doorId) {
        case 8:  return 4;
        case 9:  return 8;
        case 10:
        case 18: return 1;
        case 11: return 2;
        default: NOTSA_UNREACHABLE(); // Originally `return 0`
        }
    }
}

// 0x64E610
int32 CCarEnterExit::ComputeOppositeDoorFlag(const CVehicle* vehicle, int32 doorId, bool bCheckVehicleType) {
    if (bCheckVehicleType && (vehicle->IsBike() || vehicle->m_pHandlingData->m_bTandemSeats)) {
        switch (doorId) {
        case 8:
        case 10:
        case 18: return 5;
        case 9:
        case 11: return 10;
        default: NOTSA_UNREACHABLE(); // Originally `return 0`
        }
    } else {
        switch (doorId) {
        case 8: return 1;
        case 9: return 2;
        case 10:
        case 18: return 4;
        case 11: return 8;
        default: NOTSA_UNREACHABLE(); // Originally `return 0`
        }
    }
}

// 0x64F1E0
int32 CCarEnterExit::ComputePassengerIndexFromCarDoor(const CVehicle* vehicle, int32 doorId) {
    if (vehicle->IsBike() || vehicle->m_pHandlingData->m_bTandemSeats) {
        switch (doorId) {
        case 9:
        case 11:
            return 0;
        default:
            return -1;
        }
    }

    switch (doorId) {
    case 8:
        return 0;
    case 9:
        return 2;
    case 11:
        return 1;
    default:
        return -1;
    }
}

// 0x64F070
CPed* CCarEnterExit::ComputeSlowJackedPed(const CVehicle* vehicle, int32 doorId) {
    if (vehicle->IsBike() || vehicle->m_pHandlingData->m_bTandemSeats) {
        switch (doorId) {
        case 8:
        case 10:
        case 18:
            return vehicle->m_pDriver;
        case 9:
        case 11:
            return vehicle->m_apPassengers[0];
        default:
            return nullptr;
        }
    }

    switch (doorId) {
    case 8:
        return vehicle->m_apPassengers[0];
    case 9:
        return vehicle->m_apPassengers[2];
    case 10:
        return vehicle->m_pDriver;
    case 11:
        return vehicle->m_apPassengers[1];
    default:
        return nullptr;
    }
}

// 0x64F190
int32 CCarEnterExit::ComputeTargetDoorToEnterAsPassenger(const CVehicle* vehicle, int32 psgrIdx) {
    if (vehicle->vehicleFlags.bIsBus) {
        return 8;
    }

    switch (psgrIdx) {
    case 0:
        return (vehicle->IsBike() || vehicle->m_pHandlingData->m_bTandemSeats) ? 11 : 8; // Inverted condition
    case 1:
        return 11;
    case 2:
        return 9;
    default:
        return -1;
    }
}

// 0x64F110
int32 CCarEnterExit::ComputeTargetDoorToExit(const CVehicle* vehicle, const CPed* ped) {
    if (vehicle->m_pDriver == ped) {
        return 10;
    }

    // Theoritically the rest here is the same as `ComputeTargetDoorToEnterAsPassenger`
    // but I'm not quite sure, as in that function they just check `bIsBus`, while here they check the anim groups
    // So, using the below switch I make sure the theory is right.
    switch (vehicle->GetAnimGroupId()) {
    case ANIM_GROUP_COACHCARANIMS:
    case ANIM_GROUP_BUSCARANIMS:
        assert(vehicle->vehicleFlags.bIsBus);
    }

    if (const auto optIndex = vehicle->GetPassengerIndex(ped)) {
        return ComputeTargetDoorToEnterAsPassenger(vehicle, *optIndex);
    }

    return -1;
}

// 0x6528F0
bool CCarEnterExit::GetNearestCarDoor(const CPed* ped, const CVehicle* vehicle, CVector& outPos, int32& doorId) {
    auto driverDraggedOutOffset = vehicle->m_pDriver ? &ms_vecPedQuickDraggedOutCarAnimOffset : nullptr;
    auto psgrDraggedOutOffset   = vehicle->HasPassengerAtSeat(0) ? &ms_vecPedQuickDraggedOutCarAnimOffset : nullptr;

    if ((vehicle->IsBike() && !vehicle->IsSubBMX()) || vehicle->IsSubQuad()) {
        driverDraggedOutOffset = nullptr;
        psgrDraggedOutOffset = nullptr;

        if (ped->GetTaskManager().GetActiveTask()->GetTaskType() != TASK_COMPLEX_ENTER_CAR_AS_PASSENGER) {
            if (std::abs(vehicle->GetRight().z) < 0.1f) { // Isn't on it's side
                // Check if ped is 30 degrees to the left of the vehicle
                // Original code used atan and whatnot, but this achieves the same result
                if (DotProduct2D(vehicle->GetRight(), ped->GetForward()) > 0 // On the left
                 && DotProduct2D(vehicle->GetForward(), ped->GetForward()) > std::cos(PI / 6.f)
                ) {
                    if ((ped->IsPlayer() && ped->m_pPlayerData->m_fMoveBlendRatio > 1.5f && doorId == 0) 
                    || (!ped->IsPlayer() && ped->m_nPedType != PEDTYPE_COP && ped->m_nMoveState == PEDMOVE_RUN && ped->m_pStats->m_nTemper > 65 && doorId == 0)
                    ) {
                        // 18 here is probably either from eBikeNodes or eQuadNodes, not sure?
                        if (IsRoomForPedToLeaveCar(vehicle, 18)) {
                            doorId = 18;
                            outPos = GetPositionToOpenCarDoor(vehicle, 18);
                            return true;
                        }
                    }
                }
            }
        }
    } else if (vehicle->vehicleFlags.bIsBus || vehicle->vehicleFlags.bLowVehicle) {
        driverDraggedOutOffset = nullptr;
        psgrDraggedOutOffset = nullptr;
    }


    const auto posDoorFLeft = GetPositionToOpenCarDoor(vehicle, CAR_DOOR_LF);
    const auto posDoorFRight = GetPositionToOpenCarDoor(vehicle, CAR_DOOR_RF);

    CVector2D pedPos2D = ped->GetPosition();
    CVector2D dir2DToDoorFLeft = posDoorFLeft - pedPos2D, dir2DToDoorFRight = posDoorFRight - pedPos2D;

    if (vehicle->m_pVehicleBeingTowed) {
        if (dir2DToDoorFLeft.SquaredMagnitude() < dir2DToDoorFRight.SquaredMagnitude()) {
            if (IsPathToDoorBlockedByVehicleCollisionModel(ped, vehicle, posDoorFRight)) {
                dir2DToDoorFRight = { 999.90002f, 999.90002f };
            } else if (IsPathToDoorBlockedByVehicleCollisionModel(ped, vehicle, posDoorFLeft)) {
                dir2DToDoorFLeft = { 999.90002f, 999.90002f };
            }
        }
    }

    if (vehicle->m_pHandlingData->m_bForceDoorCheck && vehicle->IsAutomobile()) {
        const auto aut = static_cast<const CAutomobile*>(vehicle);

        if (aut->m_aCarNodes[CAR_DOOR_RF]) { // Inverted
            if (!aut->m_aCarNodes[CAR_DOOR_LF]) {
                if (IsRoomForPedToLeaveCar(vehicle, CAR_DOOR_RF, driverDraggedOutOffset)) {
                    doorId = CAR_DOOR_RF;
                    outPos = posDoorFRight;
                    return true;
                }
            }
        } else {
            if (IsRoomForPedToLeaveCar(vehicle, CAR_DOOR_LF, driverDraggedOutOffset)) {
                doorId = CAR_DOOR_LF;
                outPos = posDoorFLeft;
                return true;
            }
        }

        return false;
    }

    if (doorId != CAR_NODE_NONE && IsRoomForPedToLeaveCar(vehicle, doorId, driverDraggedOutOffset)) {
        switch (doorId) {
        case CAR_DOOR_LF: {
            doorId = CAR_DOOR_LF;
            outPos = posDoorFLeft;
            return true;
        }
        case CAR_DOOR_RF: {
            doorId = CAR_DOOR_RF;
            outPos = posDoorFRight;
            return true;
        }
        default:
            return false;
        }
    }

    if (!vehicle->m_pDriver
    || (!CPedGroups::AreInSameGroup(ped, vehicle->m_pDriver) && !vehicle->m_pDriver->bDontDragMeOutCar)
    ) {
        if (vehicle->vehicleFlags.bIsBus
         || dir2DToDoorFRight.SquaredMagnitude() > dir2DToDoorFLeft.SquaredMagnitude()
        ) {
            if (IsRoomForPedToLeaveCar(vehicle, CAR_DOOR_LF, driverDraggedOutOffset)) {
                doorId = CAR_DOOR_LF;
                outPos = posDoorFLeft;
                return true;
            }
            if (IsRoomForPedToLeaveCar(vehicle, CAR_DOOR_RF, driverDraggedOutOffset)) {
                doorId = CAR_DOOR_RF;
                outPos = posDoorFRight;
                return true;
            }
        } else {
            if (IsRoomForPedToLeaveCar(vehicle, CAR_DOOR_RF, driverDraggedOutOffset)) {
                if ((
                        vehicle->HasPassengerAtSeat(0)
                        && !vehicle->IsBike()
                        && !vehicle->m_pHandlingData->m_bTandemSeats
                        && (CPedGroups::AreInSameGroup(vehicle->m_apPassengers[0], ped) || vehicle->m_apPassengers[0]->bDontDragMeOutCar || vehicle->IsMissionVehicle())
                        && IsRoomForPedToLeaveCar(vehicle, CAR_DOOR_LF, driverDraggedOutOffset)
                    ) || (
                        vehicle->m_nGettingInFlags & 4
                        && IsRoomForPedToLeaveCar(vehicle, CAR_DOOR_LF, driverDraggedOutOffset)
                    )
                ) {
                    doorId = CAR_DOOR_LF;
                    outPos = posDoorFLeft;
                    return true;
                } else {
                    doorId = CAR_DOOR_RF;
                    outPos = posDoorFRight;
                    return true;
                }
            }

            if (IsRoomForPedToLeaveCar(vehicle, CAR_DOOR_LF, driverDraggedOutOffset)) {
                doorId = CAR_DOOR_LF;
                outPos = posDoorFLeft;
                return true;
            }
        }
    }
    return false;
}

// 0x650BB0
bool CCarEnterExit::GetNearestCarPassengerDoor(const CPed* ped, const CVehicle* vehicle, CVector* outVec, int32* doorId, bool CheckIfOccupiedTandemSeat, bool CheckIfDoorIsEnterable, bool CheckIfRoomToGetIn) {
    return plugin::CallAndReturn<bool, 0x650BB0, const CPed*, const CVehicle*, CVector*, int32*, bool, bool, bool>(ped, vehicle, outVec, doorId, CheckIfOccupiedTandemSeat, CheckIfDoorIsEnterable, CheckIfRoomToGetIn);
}

// 0x64E740
// Originally RVO'd
CVector CCarEnterExit::GetPositionToOpenCarDoor(const CVehicle* vehicle, int32 doorId) {
    CVector out;
    plugin::CallAndReturn<CVector*, 0x64E740, CVector*, const CVehicle*, int32>(&out, vehicle, doorId);
    return out;
}

// 0x64EC90
bool CCarEnterExit::IsCarDoorInUse(const CVehicle* vehicle, int32 firstDoorId, int32 secondDoorId) {
    const auto CheckIsDoorInUse = [vehicle](int32 door) {
        const auto CheckInOutFlags = [vehicle](uint32 n) {
            const auto flag = 1 << n;
            return (flag & vehicle->m_nGettingInFlags) || (flag & vehicle->m_nGettingOutFlags);
        };
        switch (door) {
        case 8: return CheckInOutFlags(2);
        case 9: return CheckInOutFlags(3);
        case 10:
        case 18: return CheckInOutFlags(0);
        case 11: return CheckInOutFlags(1);
        default: return false;
        }
    };
    return CheckIsDoorInUse(firstDoorId) || CheckIsDoorInUse(secondDoorId);
}

// 0x64ED90
bool CCarEnterExit::IsCarDoorReady(const CVehicle* vehicle, int32 doorId) {
    // TODO: Make IsDoorReadyU32 a const member function to avoid const_cast
    auto& veh = const_cast<CVehicle&>(*vehicle); // TODO: Fix
    return veh.IsDoorReadyU32((uint32)doorId)
        || veh.IsDoorFullyOpenU32((uint32)doorId);
}

// 0x64EF00
bool CCarEnterExit::IsCarQuickJackPossible(CVehicle* vehicle, int32 doorId, const CPed* ped) {
    // I think doorId 10 is the driver's door
    //if (doorId == 10 && vehicle->IsAutomobile() && !vehicle->IsDoorMissingU32(doorId) && vehicle->IsDoorClosedU32(doorId)) {
    //    // This does *nothing* - I tried `return vehicle->CanPedOpenLocks(ped);` but that just breaks everything.
    //    // Basically, returning anything but `false` from here breaks the code (in `CTaskComplexEnterCar`)
    //    vehicle->CanPedOpenLocks(ped); 
    //}
    return false;
}

// 0x64EF70
bool CCarEnterExit::IsCarSlowJackRequired(const CVehicle* vehicle, int32 doorId) {
    if (vehicle->IsBike() || (vehicle->m_pHandlingData->m_bTandemSeats)) {
        switch (doorId) {
        case 8:
        case 10:
        case 18:
            return vehicle->HasDriver();
        case 9:
        case 11:
            return vehicle->HasPassengerAtSeat(0);
        default:
            return false;
        }
    }

    int group = vehicle->GetAnimGroupId();
    if (group == ANIM_GROUP_COACHCARANIMS || group == ANIM_GROUP_BUSCARANIMS) {
        switch (doorId) {
        case 8:
            return false;
        case 10:
            return vehicle->HasDriver();
        }
    } else {
        switch (doorId) {
        case 8:
            return vehicle->HasPassengerAtSeat(0);
        case 9:
            return vehicle->HasPassengerAtSeat(2);
        case 10:
            return vehicle->HasDriver();
        case 11:
            return vehicle->HasPassengerAtSeat(1);
        default:
            return false;
        }
    }
    return false;
}

// 0x6509B0
bool CCarEnterExit::IsClearToDriveAway(const CVehicle* vehicle) {
    const auto& vehPos = vehicle->GetPosition();
    const auto  bbSizeY = vehicle->GetColModel()->GetBoundingBox().GetSize().y;
    CEntity* hitEntity{};
    CColPoint hitCP{};
    return !CWorld::ProcessLineOfSight(vehPos + vehicle->GetForward() * bbSizeY, vehPos, hitCP, hitEntity, true, true, false, false, false, true, true, false) || hitEntity == vehicle;
}

// 0x651210
bool CCarEnterExit::IsPathToDoorBlockedByVehicleCollisionModel(const CPed* ped, const CVehicle* vehicle, const CVector& pos) {
    if (vehicle->m_nModelIndex == eModelID::MODEL_AT400) {
        return false;
    }

    const auto vehMatInv = Invert(*vehicle->m_matrix);
    const CColLine line{
        vehMatInv.TransformPoint(ped->GetPosition()),
        vehMatInv.TransformPoint(pos)
    };

    for (const auto& sp : vehicle->GetColModel()->GetData()->GetSpheres()) {
        if (CCollision::TestLineSphere(line, sp)) {
            return false;
        }
    }

    return true;
}

// 0x64EEE0
bool CCarEnterExit::IsPedHealthy(CPed* ped) {
    return ped->m_fHealth > 0.f;
}

// 0x64F240
bool CCarEnterExit::IsPlayerToQuitCarEnter(const CPed* ped, const CVehicle* vehicle, int32 startTime, CTask* task) {
    return plugin::CallAndReturn<bool, 0x64F240, const CPed*, const CVehicle*, int32, CTask*>(ped, vehicle, startTime, task);
}

// 0x6504C0
bool CCarEnterExit::IsRoomForPedToLeaveCar(const CVehicle* vehicle, int32 doorId, const CVector* pos) {
    return plugin::CallAndReturn<bool, 0x6504C0>(vehicle, doorId, pos);
}

// 0x64EEC0
bool CCarEnterExit::IsVehicleHealthy(const CVehicle* vehicle) {
    return vehicle->m_nStatus != STATUS_WRECKED;
}

// 0x6510D0
bool CCarEnterExit::IsVehicleStealable(const CVehicle* vehicle, const CPed* ped) {
    switch (vehicle->m_nVehicleSubType) {
    case VEHICLE_TYPE_PLANE:
    case VEHICLE_TYPE_HELI:
        return false;
    }

    switch (vehicle->m_nVehicleType) {
    case VEHICLE_TYPE_AUTOMOBILE:
    case VEHICLE_TYPE_BIKE:
        break;
    default:
        return false;
    }

    if (ped->m_pVehicle != vehicle) {
        switch (vehicle->m_nCreatedBy) {
        case RANDOM_VEHICLE:
        case PARKED_VEHICLE:
            break;
        default:
            return false;
        }
    }

    if (CUpsideDownCarCheck{}.IsCarUpsideDown(vehicle)) {
        return false;
    }

    if (!vehicle->CanBeDriven()) {
        return false;
    }

    if (vehicle->IsLawEnforcementVehicle()) {
        return false;
    }

    if (const auto drvr = vehicle->m_pDriver) {
        if (   drvr->IsCreatedByMission()
            || drvr->IsPlayer()
            || ped->GetIntelligence()->IsFriendlyWith(*drvr)
            || CPedGroups::AreInSameGroup(ped, drvr)
        ) {
            return false;
        }
    }

    if (const auto grp = ped->GetGroup()) {
        if (grp->IsAnyoneUsingCar(vehicle)) {
            return false;
        }
    }

    if (vehicle->m_pFire) {
        return false;
    }

    if (vehicle->m_fHealth <= 600.f) {
        return false;
    }

    if (vehicle->IsUpsideDown() || vehicle->IsOnItsSide()) {
        return false;
    }

    if (!IsClearToDriveAway(vehicle)) {
        return false;
    }

    return true;
}

// 0x64F600
void CCarEnterExit::MakeUndraggedDriverPedLeaveCar(const CVehicle* vehicle, const CPed* pedGettingIn) {
    auto& veh = const_cast<CVehicle&>(*vehicle); // TODO: Fix
    auto& ped = const_cast<CPed&>(*pedGettingIn); // TODO: Fix
    veh.m_pDriver->GetEventGroup().Add(CEventDraggedOutCar{ &veh, &ped, true });
}

// 0x64F540
void CCarEnterExit::MakeUndraggedPassengerPedsLeaveCar(const CVehicle* targetVehicle, const CPed* draggedPed, const CPed* ped) {
    plugin::Call<0x64F540, const CVehicle*, const CPed*, const CPed*>(targetVehicle, draggedPed, ped);
}

// unused
// 0x650130
void CCarEnterExit::QuitEnteringCar(CPed* ped, CVehicle* vehicle, int32 doorId, bool bCarWasBeingJacked) {
    RemoveGetInAnims(ped);
    ped->RestartNonPartialAnims();
    if (!RpAnimBlendClumpGetAssociation(ped->m_pRwClump, ANIM_ID_IDLE)) {
        CAnimManager::BlendAnimation(ped->m_pRwClump, ped->m_nAnimGroup, ANIM_ID_IDLE, 1000.0f);
    }

    if (bCarWasBeingJacked) {
        vehicle->vehicleFlags.bIsBeingCarJacked = true;
    }
    vehicle->m_nNumGettingIn--;

    if (vehicle->IsBike() || vehicle->m_pHandlingData->m_bTandemSeats) {
        switch (doorId) {
        case 8:
        case 10:
            vehicle->SetGettingInFlags(5);
            break;
        case 9:
        case 11:
            vehicle->SetGettingInFlags(10);
            break;
        }
        vehicle->vehicleFlags.bIsBig = false;
    } else {
        switch (doorId) {
        case 8:
            vehicle->SetGettingInFlags(4);
            break;
        case 9:
            vehicle->SetGettingInFlags(8);
            break;
        case 10:
            vehicle->SetGettingInFlags(vehicle->m_nMaxPassengers ? 1 : 3);
            break;
        case 11:
            vehicle->SetGettingInFlags(vehicle->m_nMaxPassengers ? 2 : 3);
            break;
        }
    }
    ped->m_bUsesCollision = false;
}

// 0x64F680
void CCarEnterExit::RemoveCarSitAnim(const CPed* ped) {
    for (auto anim = RpAnimBlendClumpGetFirstAssociation(ped->m_pRwClump, ANIMATION_SECONDARY_TASK_ANIM); anim; anim = RpAnimBlendGetNextAssociation(anim, ANIMATION_SECONDARY_TASK_ANIM)) {
        anim->SetFlag(ANIMATION_IS_BLEND_AUTO_REMOVE);
        anim->m_BlendDelta = -1000.f;
    }
    CAnimManager::BlendAnimation(ped->m_pRwClump, ped->m_nAnimGroup, ANIM_ID_IDLE, 1000.0);
}

// 0x64F6E0
void CCarEnterExit::RemoveGetInAnims(const CPed* ped) {
    for (auto anim = RpAnimBlendClumpGetFirstAssociation(ped->m_pRwClump, ANIMATION_IS_PARTIAL); anim; anim = RpAnimBlendGetNextAssociation(anim, ANIMATION_IS_PARTIAL)) {
        anim->SetFlag(ANIMATION_IS_BLEND_AUTO_REMOVE);
        anim->m_BlendDelta = -1000.f;
    }
}

// 0x64F860
void CCarEnterExit::SetAnimOffsetForEnterOrExitVehicle() {
    if (ms_bPedOffsetsCalculated) {
        return;
    }

    const auto animBlockIdxs = {
        CAnimManager::GetAnimationBlockIndex("int_house"),
        CAnimManager::GetAnimationBlockIndex("int_office")
    };

    for (const auto idx : animBlockIdxs) {
        CStreaming::RequestModel(IFPToModelId(idx), STREAMING_KEEP_IN_MEMORY);
    }
    CStreaming::LoadAllRequestedModels(false);

    for (const auto idx : animBlockIdxs) {
        CAnimManager::AddAnimBlockRef(idx);
    }

    {
        const auto anim = CAnimManager::GetAnimAssociation(ANIM_GROUP_DEFAULT, ANIM_ID_GETUP_0);
        CAnimManager::UncompressAnimation(anim->m_BlendHier);
        const auto& seq = anim->m_BlendHier->GetSequences()[0];
        ms_vecPedGetUpAnimOffset = seq.m_FramesNum ? seq.GetUKeyFrame(0)->Trans : CVector{};
    }

    ms_vecPedQuickDraggedOutCarAnimOffset = CVector{ -1.841797f, -0.3261719f, -0.01269531f };

    const std::tuple<AssocGroupId, AnimationId, CVector*> toProcess[]{
        {ANIM_GROUP_INT_HOUSE,  ANIM_ID_BED_IN_L,   &ms_vecPedBedLAnimOffset  },
        {ANIM_GROUP_INT_HOUSE,  ANIM_ID_BED_IN_R,   &ms_vecPedBedRAnimOffset  },
        {ANIM_GROUP_INT_OFFICE, ANIM_ID_OFF_SIT_IN, &ms_vecPedDeskAnimOffset  },
        {ANIM_GROUP_INT_HOUSE,  ANIM_ID_LOU_IN,     &ms_vecPedChairAnimOffset },
    };
    for (const auto [grpId, animId, out] : toProcess) {
        // Calculate translation delta between first and last sequence frames
        *out = [grpId, animId] {
            const auto anim = CAnimManager::GetAnimAssociation(grpId, animId);
            CAnimManager::UncompressAnimation(anim->m_BlendHier);
            const auto& seq = anim->m_BlendHier->GetSequences()[0];
            if (seq.m_FramesNum > 0) {
                return seq.GetUKeyFrame(seq.m_FramesNum - 1)->Trans - seq.GetUKeyFrame(0)->Trans;
            }
            return CVector{};
        }();
    }

    for (const auto idx : animBlockIdxs) {
        CAnimManager::RemoveAnimBlockRef(idx);
    }

    ms_bPedOffsetsCalculated = true;
}

// 0x650280
bool CCarEnterExit::SetPedInCarDirect(CPed* ped, CVehicle* vehicle, int32 doorId, bool bAsDriver) {
    if (bAsDriver) {
        // Warp ped into vehicle
        CTaskSimpleCarSetPedInAsDriver task{ vehicle };
        task.m_bWarpingInToCar = true;
        task.ProcessPed(ped);

        // And make them drive
        ped->GetTaskManager().SetTask(new CTaskComplexCarDriveWander{ vehicle, vehicle->m_autoPilot.m_nCarDrivingStyle, (float)vehicle->m_autoPilot.m_nCruiseSpeed }, TASK_PRIMARY_PRIMARY);

        return true;
    }

    // Warp ped into vehicle
    {
        CTaskSimpleCarSetPedInAsPassenger task{ vehicle, (eTargetDoor)doorId };
        task.m_bWarpingInToCar = true;
        task.ProcessPed(ped);
    }

    if (vehicle->IsBike()) {
        ped->GetTaskManager().SetTask(new CTaskComplexCarDrive{ vehicle, false }, TASK_PRIMARY_PRIMARY);
    }

    // Set mutal acquaintance respect between the ped and all other occupants up to the ped's seat
    // I assume the function is only ever called with `bAsDriver` if there are no passengers
    // So that's why this code-path is only reachable if `bAsDriver` is false

    const auto SetMutalAcquaintanceWith = [ped](CPed* other) {
        if (other) {
            const auto SetWith = [](CPed* of, CPed* with) {
                if (!of->IsCreatedByMission()) {
                    of->GetAcquaintance().SetAsAcquaintance(ACQUAINTANCE_RESPECT, CPedType::GetPedFlag(with->m_nPedType));
                }
            };
            SetWith(ped, other);
            SetWith(other, ped);
        }
    };

    SetMutalAcquaintanceWith(vehicle->m_pDriver);

    const auto psgrIdx = ComputePassengerIndexFromCarDoor(vehicle, doorId);
    assert(psgrIdx != -1); // I really doubt this can happen, if it does, an `if` has to be added
    rng::for_each(vehicle->GetPassengers() | rng::views::take((size_t)psgrIdx), SetMutalAcquaintanceWith); // Set with all other passengers up to the ped's seat

    return true;
}
