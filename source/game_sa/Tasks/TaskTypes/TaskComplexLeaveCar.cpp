#include "StdInc.h"

#include "TaskComplexLeaveCar.h"
#include "TaskSimpleCarWaitToSlowDown.h"
#include "TaskSimplePause.h"
#include "TaskComplexLeaveBoat.h"
#include "TaskSimpleCarDriveTimed.h"
#include "TaskSimpleCarWaitForDoorNotToBeInUse.h"
#include "TaskSimpleCarForcePedOut.h"
#include "TaskSimpleCarGetOut.h"
#include "TaskSimpleCarJumpOut.h"
#include "TaskSimpleCarCloseDoorFromOutside.h"
#include "TaskSimpleCarSetPedOut.h"
#include "TaskComplexCarSlowBeDraggedOut.h"
#include "TaskSimpleDie.h"
#include "TaskComplexGetUpAndStandStill.h"
#include "EventGroupEvent.h"
#include "EventLeaderExitedCarAsDriver.h"
#include "EventDamage.h"

// 0x62F1A0
CTaskComplexLeaveCar::CTaskComplexLeaveCar(CVehicle* targetVehicle, int32 targetDoor, int32 delayTime) :
    CTaskComplexLeaveCar{ targetVehicle, targetDoor, delayTime, false, true } {
    m_Die = true;
}

// 0x63B8C0
CTaskComplexLeaveCar::CTaskComplexLeaveCar(CVehicle* targetVehicle, int32 targetDoor, int32 delayTime, bool sensibleLeaveCar, bool forceGetOut) :
    m_TargetDoor{ targetDoor },
    m_DelayTime{ delayTime },
    m_SensibleLeaveCar{ sensibleLeaveCar },
    m_TargetVehicle{ targetVehicle },
    m_ForceGetOut{ forceGetOut } {
}

// NOTSA
CTaskComplexLeaveCar::CTaskComplexLeaveCar(const CTaskComplexLeaveCar& o) :
    CTaskComplexLeaveCar{ o.m_TargetVehicle, o.m_TargetDoor, o.m_DelayTime, o.m_SensibleLeaveCar, o.m_ForceGetOut } {
}

// 0x63B970
CTaskComplexLeaveCar::~CTaskComplexLeaveCar() {
    if (m_TargetVehicle) {
        ClearVehicleFlags();
    }
}

// 0x63BAB0
void CTaskComplexLeaveCar::ComputeTargetDoor(CPed* ped) {
    if (!m_TargetDoor) {
        m_TargetDoor = CCarEnterExit::ComputeTargetDoorToExit(m_TargetVehicle, ped);
    }
}

// 0x63BA00
void CTaskComplexLeaveCar::SetVehicleFlags(CPed* ped) {
    m_DoorFlagsSet = (uint8)CCarEnterExit::ComputeDoorFlag(m_TargetVehicle, m_TargetDoor, true);
    m_TargetVehicle->SetGettingOutFlags(m_DoorFlagsSet);
    m_NumGettingInSet = 1;
    m_TargetVehicle->m_nNumGettingIn++;

    if (m_TargetVehicle->GetDriver() != ped) {
        return;
    }
    if (ped->IsPlayer()) { // 0x63ba75
        m_TargetVehicle->SetStatus(STATUS_FORCED_STOP);
    } else if (m_SensibleLeaveCar) { // 0x63ba5d - Stop the car
        m_TargetVehicle->m_autoPilot.SetCruiseSpeed(0);
        m_TargetVehicle->m_autoPilot.ClearCarMission();
    }
}

// 0x63BAE0
void CTaskComplexLeaveCar::CreateTaskUtilityLineUpPedWithCar(CPed* ped) {
    // NOTE: The original leaks the previous line-up task (if any)
    m_LineUpUtility = std::make_unique<CTaskUtilityLineUpPedWithCar>(CVector{}, 0, 0, m_TargetDoor);
}

// NOTSA
void CTaskComplexLeaveCar::ClearVehicleFlags() {
    m_TargetVehicle->ClearGettingOutFlags(m_DoorFlagsSet);
    m_DoorFlagsSet = 0;
    m_TargetVehicle->m_nNumGettingIn -= m_NumGettingInSet;
    m_NumGettingInSet = 0;
}

// 0x646CF0
void CTaskComplexLeaveCar::GetDoorUsage(const CVehicle* veh, int32 door, bool& isGettingIn, bool& isGettingOut) {
    const auto flag = [door]() -> uint8 {
        switch (door) {
        case TARGET_DOOR_FRONT_RIGHT: return 4;
        case TARGET_DOOR_REAR_RIGHT:  return 8;
        case TARGET_DOOR_DRIVER:      return 1;
        case TARGET_DOOR_REAR_LEFT:   return 2;
        default:                      return 0;
        }
    }();
    if (veh->m_nGettingInFlags & flag) {
        isGettingIn = true;
    }
    if (veh->m_nGettingOutFlags & flag) {
        isGettingOut = true;
    }
}

// 0x641100
bool CTaskComplexLeaveCar::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    if (!m_TargetVehicle) {
        return true;
    }

    switch (priority) {
    case ABORT_PRIORITY_IMMEDIATE: { // 0x641145
        m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_IMMEDIATE, event);
        if (ped->IsInVehicle()) { // Warp the ped out
            ComputeTargetDoor(ped);
            CTaskSimpleCarCloseDoorFromOutside tCloseDoor{ m_TargetVehicle, (uint32)m_TargetDoor, nullptr };
            tCloseDoor.MakeAbortable(ped, ABORT_PRIORITY_IMMEDIATE, event);
            CTaskSimpleCarSetPedOut tSetPedOut{ m_TargetVehicle, (eTargetDoor)m_TargetDoor, m_SensibleLeaveCar };
            tSetPedOut.ProcessPed(ped);
        }
        ClearVehicleFlags();
        return true;
    }
    case ABORT_PRIORITY_URGENT: { // 0x64122f
        const auto AbortSubTask = [&] {
            return m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_URGENT, event);
        };
        const auto AbortSubTaskAndClearVehicleFlags = [&] {
            if (!AbortSubTask()) {
                return false;
            }
            ClearVehicleFlags();
            return true;
        };
        const auto* const eDamage      = notsa::dyn_cast_if_present<CEventDamage>(event);
        const auto        hasKilledPed = eDamage && eDamage->HasKilledPed();

        switch (m_pSubTask->GetTaskType()) {
        case TASK_SIMPLE_DIE: // 0x641251
            return AbortSubTaskAndClearVehicleFlags();
        case TASK_SIMPLE_CAR_WAIT_TO_SLOW_DOWN:   // 0x6412ba
        case TASK_COMPLEX_GET_UP_AND_STAND_STILL: // 0x641332
            return hasKilledPed && AbortSubTaskAndClearVehicleFlags();
        case TASK_SIMPLE_CAR_JUMP_OUT: { // 0x6413b2
            if (!event) {
                return false;
            }
            const auto eventType = event->GetEventType();
            if (eventType != EVENT_IN_AIR && eventType != EVENT_IN_WATER) {
                return false;
            }
            if (eventType == EVENT_IN_AIR) {
                m_IsInAir = true;
            }
            if (!AbortSubTask()) {
                return false;
            }

            if (ped->GetActiveWeapon().GetType() == WEAPON_PARACHUTE && eventType == EVENT_IN_AIR && m_TargetVehicle->GetNumContactWheels() == 0) { // 0x6413fe
                // NOTE: Using MagSq here
                if (!m_TargetVehicle->IsSubPlane() || m_TargetVehicle->GetMoveSpeed().SquaredMagnitude() <= sq(0.2f)) {
                    if (!ped->IsPlayer()) {
                        return false;
                    }
                    CColPoint cp{};
                    CEntity*  hitEntity{};
                    if (CWorld::ProcessVerticalLine(ped->GetPosition(), -10.f, cp, hitEntity, true, false, false, false, true)) { // Too close to the ground
                        return false;
                    }
                }
                CAnimManager::BlendAnimation(ped->GetRpClump(), ANIM_GROUP_DEFAULT, ANIM_ID_FALL_FRONT, 8.f);
            } else if (eventType == EVENT_IN_WATER && ped->GetMoveSpeed().z < -0.3f) { // 0x6414a7
                CAnimManager::BlendAnimation(ped->GetRpClump(), ANIM_GROUP_DEFAULT, ANIM_ID_FALL_FRONT, 16.f);
            }
            return false;
        }
        case TASK_SIMPLE_CAR_DRIVE_TIMED: // 0x6414fa
            return event && AbortSubTask();
        default:
            return false;
        }
    }
    default:
        return false;
    }
}

// 0x6419F0
CTask* CTaskComplexLeaveCar::CreateNextSubTask(CPed* ped) {
    switch (const auto tt = m_pSubTask->GetTaskType()) {
    case TASK_SIMPLE_PAUSE: // 0x641ef9
    case TASK_SIMPLE_DIE:   // 0x641a35
        return CreateSubTask(TASK_FINISHED, ped);
    case TASK_COMPLEX_GET_UP_AND_STAND_STILL: // 0x641a3f
        return CreateSubTask(ped->IsInVehicle() ? TASK_SIMPLE_CAR_SET_PED_OUT : TASK_FINISHED, ped);
    case TASK_COMPLEX_LEAVE_BOAT:     // 0x641a68
    case TASK_SIMPLE_CAR_SET_PED_OUT: // 0x641f49
        return CreateSubTask(m_Die ? TASK_SIMPLE_DIE : TASK_FINISHED, ped);
    case TASK_SIMPLE_CAR_CLOSE_DOOR_FROM_OUTSIDE: // 0x641a97
        m_DieAnimID = ANIM_ID_KO_SHOT_FRONT_0;
        return CreateSubTask(TASK_SIMPLE_CAR_SET_PED_OUT, ped);
    case TASK_SIMPLE_CAR_DRIVE_TIMED: { // 0x641ab8
        auto* const pedVeh = ped->GetVehicleIfInOne();
        if (!pedVeh) {
            return CreateSubTask(TASK_FINISHED, ped);
        }
        if (!pedVeh->IsPassenger(ped) && !pedVeh->IsDriver(ped)) {
            ped->bInVehicle = false;
            return CreateSubTask(TASK_FINISHED, ped);
        }
        ComputeTargetDoor(ped);
        bool isGettingIn{};
        bool isGettingOut{};
        GetDoorUsage(m_TargetVehicle, m_TargetDoor, isGettingIn, isGettingOut);
        if (m_ForceGetOut) {
            return CreateSubTask(TASK_SIMPLE_CAR_WAIT_TO_SLOW_DOWN, ped);
        }
        if (isGettingOut) {
            return CreateSubTask(TASK_SIMPLE_CAR_WAIT_FOR_DOOR_NOT_TO_BE_IN_USE, ped);
        }
        return CreateSubTask(isGettingIn ? TASK_FINISHED : TASK_SIMPLE_CAR_WAIT_TO_SLOW_DOWN, ped);
    }
    case TASK_SIMPLE_CAR_WAIT_TO_SLOW_DOWN: {            // 0x641b68
        if (!m_TargetVehicle->CanPedStepOutCar(false)) { // 0x641d10
            SetVehicleFlags(ped);
            ped->SetPedState(PEDSTATE_NONE);
            return CreateSubTask(TASK_SIMPLE_CAR_JUMP_OUT, ped);
        }
        if (!CCarEnterExit::IsRoomForPedToLeaveCar(m_TargetVehicle, m_TargetDoor)) { // Try the other door on the same side
            const auto ForcePedOut = [&] {                                           // 0x641c23
                SetVehicleFlags(ped);
                ped->SetPedState(PEDSTATE_NONE);
                CreateTaskUtilityLineUpPedWithCar(ped);
                return CreateSubTask(TASK_SIMPLE_CAR_FORCE_PED_OUT, ped);
            };
            auto* const veh        = m_TargetVehicle.Get();
            const auto  IsSeatFree = [veh](const CPed* occupant, uint8 gettingInFlag) {
                return !occupant && !(veh->m_nGettingInFlags & gettingInFlag);
            };
            const auto CanUseAnyDoor = [veh] {
                return veh->IsBike() || veh->m_pHandlingData->m_bTandemSeats;
            };
            int32 otherDoor = 0;
            // NOTE: Not using `GetPassengers()`: The original checks these seats even if the vehicle has fewer
            switch (m_TargetDoor) {
            case TARGET_DOOR_FRONT_RIGHT: // 0x641c55
                if (!IsSeatFree(veh->GetDriver(), 1)) {
                    return ForcePedOut();
                }
                otherDoor = TARGET_DOOR_DRIVER;
                break;
            case TARGET_DOOR_REAR_RIGHT: // 0x641c70
                if (!IsSeatFree(veh->m_apPassengers[1], 2)) {
                    return ForcePedOut();
                }
                otherDoor = TARGET_DOOR_REAR_LEFT;
                break;
            case TARGET_DOOR_DRIVER: // 0x641bad
                if (!CanUseAnyDoor() && !IsSeatFree(veh->m_apPassengers[0], 4)) {
                    return ForcePedOut();
                }
                otherDoor = TARGET_DOOR_FRONT_RIGHT;
                break;
            case TARGET_DOOR_REAR_LEFT: // 0x641be8
                if (!CanUseAnyDoor() && !IsSeatFree(veh->m_apPassengers[2], 8)) {
                    return ForcePedOut();
                }
                otherDoor = TARGET_DOOR_REAR_RIGHT;
                break;
            }
            if (!CCarEnterExit::IsRoomForPedToLeaveCar(veh, otherDoor)) {
                return ForcePedOut();
            }
            if (veh->m_pHandlingData->m_bForceDoorCheck && veh->IsAutomobile() && !veh->AsAutomobile()->m_aCarNodes[otherDoor]) { // 0x641c9b
                return ForcePedOut();
            }
            m_TargetDoor = otherDoor;
        }
        SetVehicleFlags(ped); // 0x641cc6
        ped->SetPedState(PEDSTATE_NONE);
        if (m_Die) {
            return CreateSubTask(TASK_COMPLEX_CAR_SLOW_BE_DRAGGED_OUT, ped);
        }
        CreateTaskUtilityLineUpPedWithCar(ped);
        return CreateSubTask(TASK_SIMPLE_CAR_GET_OUT, ped);
    }
    case TASK_SIMPLE_CAR_WAIT_FOR_DOOR_NOT_TO_BE_IN_USE: // 0x641d56
        return CreateSubTask(TASK_SIMPLE_CAR_WAIT_TO_SLOW_DOWN, ped);
    case TASK_SIMPLE_CAR_GET_OUT: { // 0x641d70
        m_LineUpUtility->m_nDoorOpenPosType = 2;
        if ((!m_ForceGetOut || m_TargetVehicle->m_nDoorLock != CARLOCK_UNLOCKED) && CCarEnterExit::CarHasDoorToClose(m_TargetVehicle, m_TargetDoor)) {
            return CreateSubTask(TASK_SIMPLE_CAR_CLOSE_DOOR_FROM_OUTSIDE, ped);
        }
        if (m_TargetVehicle->IsAutomobile() && CCarEnterExit::CarHasDoorToClose(m_TargetVehicle, m_TargetDoor)) { // 0x641db9 - Leave the door open
            auto&      dm   = m_TargetVehicle->AsAutomobile()->GetDamageManager();
            const auto door = (tComponent)m_TargetDoor;
            switch (dm.GetDoorStatus_Component(door)) {
            case DAMSTATE_OK:      dm.SetDoorStatus_Component(door, DAMSTATE_OPENED); break;
            case DAMSTATE_DAMAGED: dm.SetDoorStatus_Component(door, DAMSTATE_OPENED_DAMAGED); break;
            default:               break;
            }
        }
        return CreateSubTask(TASK_SIMPLE_CAR_SET_PED_OUT, ped);
    }
    case TASK_SIMPLE_CAR_FORCE_PED_OUT: // 0x641e28
        return CreateSubTask(TASK_SIMPLE_CAR_SET_PED_OUT, ped);
    case TASK_SIMPLE_CAR_JUMP_OUT: { // 0x641e42
        if (m_Die) {
            m_DieAnimID         = ANIM_ID_KO_SHOT_FRONT_0;
            m_DieAnimBlendDelta = 1000.f;
            m_DieAnimSpeed      = 0.5f;
            return CreateSubTask(TASK_SIMPLE_DIE, ped);
        }
        if (!m_TargetVehicle || m_TargetVehicle->IsBike() || m_TargetVehicle->IsSubQuad()) {
            return CreateSubTask(TASK_FINISHED, ped);
        }
        if (m_IsInAir || ped->bIsDrowning) {
            return CreateSubTask(TASK_SIMPLE_CAR_SET_PED_OUT, ped);
        }
        ClearVehicleFlags();
        return CreateSubTask(TASK_COMPLEX_GET_UP_AND_STAND_STILL, ped);
    }
    case TASK_COMPLEX_CAR_SLOW_BE_DRAGGED_OUT: { // 0x641f00
        m_DieAnimID         = ANIM_ID_FLOOR_HIT;
        m_DieAnimBlendDelta = 1000.f;
        m_DieAnimSpeed      = 0.5f;
        if (m_TargetVehicle && m_TargetVehicle->m_nDoorLock == CARLOCK_COP_CAR) {
            m_TargetVehicle->m_nDoorLock = CARLOCK_UNLOCKED;
        }
        return CreateSubTask(TASK_SIMPLE_DIE, ped);
    }
    default:
        NOTSA_UNREACHABLE_CASE(tt);
    }
}

// 0x641FC0
CTask* CTaskComplexLeaveCar::CreateFirstSubTask(CPed* ped) {
    if ((ped->GetPedState() == PEDSTATE_ARRESTED || ped->bIsBeingArrested) && ped->IsPlayer()) {
        return new CTaskSimplePause{ -1 };
    }
    if (!ped->IsInVehicle()) { // NOTE: The original prints a debug message here
        return new CTaskSimplePause{ -1 };
    }
    if (m_TargetVehicle->GetDriver() == ped) { // 0x642073
        if (auto* const grp = CPedGroups::GetPedsGroup(ped); grp && grp->GetMembership().IsLeader(ped)) {
            CEventGroupEvent groupEvent{ ped, new CEventLeaderExitedCarAsDriver{} };
            grp->GetIntelligence().AddEvent(&groupEvent);
        }
        if (!ped->IsPlayer()) {
            ped->SetRadioStation();
        } else if (auto* const pedVeh = ped->GetVehicleIfInOne()) {
            pedVeh->m_vehicleAudio.PlayerAboutToExitVehicleAsDriver();
        }
    }
    if (m_TargetVehicle->IsBoat()) {
        return new CTaskComplexLeaveBoat{ m_TargetVehicle, 0 };
    }
    return new CTaskSimpleCarDriveTimed{ m_TargetVehicle, m_DelayTime };
}

// 0x6421B0
CTask* CTaskComplexLeaveCar::ControlSubTask(CPed* ped) {
    if (!m_TargetVehicle) {
        return nullptr;
    }
    if (!ped->IsInVehicle()) {
        switch (m_pSubTask->GetTaskType()) {
        case TASK_SIMPLE_CAR_SET_PED_OUT:
        case TASK_SIMPLE_CAR_JUMP_OUT:
        case TASK_COMPLEX_GET_UP_AND_STAND_STILL:
        case TASK_SIMPLE_DIE:
            break;
        default: // 0x64220c - Already out of the car
            return CreateSubTask(TASK_SIMPLE_CAR_SET_PED_OUT, ped);
        }
    }
    if (!m_SensibleLeaveCar && notsa::isa<CTaskSimpleCarWaitToSlowDown>(m_pSubTask)) {
        m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_LEISURE);
    }
    return m_pSubTask;
}

// 0x641530
CTask* CTaskComplexLeaveCar::CreateSubTask(eTaskType taskType, CPed* ped) {
    switch (taskType) {
    case TASK_SIMPLE_CAR_DRIVE_TIMED:
        return new CTaskSimpleCarDriveTimed{ m_TargetVehicle, m_DelayTime };
    case TASK_SIMPLE_CAR_WAIT_TO_SLOW_DOWN: {
        using enum CTaskSimpleCarWaitToSlowDown::SlowDownType;
        return new CTaskSimpleCarWaitToSlowDown{
            m_TargetVehicle,
            m_Die || !m_SensibleLeaveCar
                ? DONT_WAIT
                : ped->IsPlayer() ? SLOW_ENOUGH_TO_STEP_OR_JUMP
                                  : SLOW_ENOUGH_TO_STEP
        };
    }
    case TASK_SIMPLE_CAR_WAIT_FOR_DOOR_NOT_TO_BE_IN_USE:
        return new CTaskSimpleCarWaitForDoorNotToBeInUse{ m_TargetVehicle, (uint32)m_TargetDoor };
    case TASK_SIMPLE_CAR_FORCE_PED_OUT:
        return new CTaskSimpleCarForcePedOut{ m_TargetVehicle, m_TargetDoor };
    case TASK_SIMPLE_CAR_GET_OUT:
        return new CTaskSimpleCarGetOut{ m_TargetVehicle, (uint32)m_TargetDoor, m_LineUpUtility.get() };
    case TASK_SIMPLE_CAR_JUMP_OUT:
        return new CTaskSimpleCarJumpOut{ m_TargetVehicle, (uint32)m_TargetDoor, m_LineUpUtility.get() };
    case TASK_SIMPLE_CAR_CLOSE_DOOR_FROM_OUTSIDE:
        return new CTaskSimpleCarCloseDoorFromOutside{ m_TargetVehicle, (uint32)m_TargetDoor, m_LineUpUtility.get() };
    case TASK_SIMPLE_CAR_SET_PED_OUT:
        return new CTaskSimpleCarSetPedOut{ m_TargetVehicle, (eTargetDoor)m_TargetDoor, m_SensibleLeaveCar };
    case TASK_COMPLEX_CAR_SLOW_BE_DRAGGED_OUT:
        return new CTaskComplexCarSlowBeDraggedOut{ m_TargetVehicle, (eTargetDoor)m_TargetDoor, true };
    case TASK_SIMPLE_DIE:
        return new CTaskSimpleDie{ ANIM_GROUP_DEFAULT, m_DieAnimID, m_DieAnimBlendDelta, m_DieAnimSpeed };
    case TASK_COMPLEX_GET_UP_AND_STAND_STILL:
        return new CTaskComplexGetUpAndStandStill{};
    case TASK_COMPLEX_LEAVE_BOAT:
        return new CTaskComplexLeaveBoat{ m_TargetVehicle, 0 };
    case TASK_SIMPLE_PAUSE:
        return new CTaskSimplePause{ -1 };
    case TASK_FINISHED:
        return nullptr;
    default:
        NOTSA_UNREACHABLE_CASE(taskType);
    }
}

void CTaskComplexLeaveCar::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexLeaveCar, 0x86e828, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedConstructorInstall(0x62F1A0, "1", {}, CVehicle*, int32, int32);
    RH_ScopedConstructorInstall(0x63B8C0, "2", {}, CVehicle*, int32, int32, bool, bool);
    RH_ScopedDestructorInstall(0x63B970);

    RH_ScopedInstall(ComputeTargetDoor, 0x63BAB0);
    RH_ScopedInstall(SetVehicleFlags, 0x63BA00);
    RH_ScopedInstall(CreateTaskUtilityLineUpPedWithCar, 0x63BAE0);
    RH_ScopedInstall(GetDoorUsage, 0x646CF0);
    RH_ScopedInstall(CreateSubTask, 0x641530);
    RH_ScopedVMTInstall(Clone, 0x63D9E0);
    RH_ScopedVMTInstall(GetTaskType, 0x63B960);
    RH_ScopedVMTInstall(MakeAbortable, 0x641100);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x6419F0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x641FC0);
    RH_ScopedVMTInstall(ControlSubTask, 0x6421B0);
}
