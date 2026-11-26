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

// 0x62F1A0
CTaskComplexLeaveCar::CTaskComplexLeaveCar(CVehicle* targetVehicle, int32 nTargetDoor, int32 nDelayTime) :
    CTaskComplexLeaveCar(targetVehicle, nTargetDoor, nDelayTime, false, true) {
    m_bDie = true;
}

// 0x63B8C0
CTaskComplexLeaveCar::CTaskComplexLeaveCar(CVehicle* targetVehicle, int32 nTargetDoor, int32 nDelayTime, bool bSensibleLeaveCar, bool bForceGetOut) :
    m_nTargetDoor{ nTargetDoor },
    m_nDelayTime{ nDelayTime },
    m_bSensibleLeaveCar{ bSensibleLeaveCar },
    m_pTargetVehicle{ targetVehicle },
    m_bForceGetOut{ bForceGetOut } {
}

// 0x63D9E0
CTaskComplexLeaveCar::CTaskComplexLeaveCar(const CTaskComplexLeaveCar& o) :
    CTaskComplexLeaveCar{ o.m_pTargetVehicle, o.m_nTargetDoor, o.m_nDelayTime, o.m_bSensibleLeaveCar, o.m_bForceGetOut } {
}

// 0x63B970
CTaskComplexLeaveCar::~CTaskComplexLeaveCar() {
    if (m_pTargetVehicle) {
        m_pTargetVehicle->ClearGettingOutFlags(m_nDoorFlagsSet);
        m_pTargetVehicle->m_nNumGettingIn -= m_nNumGettingInSet;
    }
}

// 0x63BAB0
void CTaskComplexLeaveCar::ComputeTargetDoor(CPed* ped) {
    if (!m_nTargetDoor) {
        m_nTargetDoor = CCarEnterExit::ComputeTargetDoorToExit(m_pTargetVehicle, ped);
    }
}

// 0x63BA00
void CTaskComplexLeaveCar::SetVehicleFlags(CPed* ped) {
    m_nDoorFlagsSet = (uint8)CCarEnterExit::ComputeDoorFlag(m_pTargetVehicle, m_nTargetDoor, true);
    m_pTargetVehicle->SetGettingOutFlags(m_nDoorFlagsSet);
    m_nNumGettingInSet = 1;
    m_pTargetVehicle->m_nNumGettingIn++;

    auto* const driver = m_pTargetVehicle->m_pDriver;
    if (driver && !driver->IsPlayer() && ped == driver && m_bSensibleLeaveCar) { // Stop the car
        m_pTargetVehicle->m_autoPilot.m_nCruiseSpeed = 0;
        m_pTargetVehicle->m_autoPilot.m_nCarMission  = MISSION_NONE;
    }
    if (ped->IsPlayer() && ped == m_pTargetVehicle->m_pDriver) {
        m_pTargetVehicle->SetStatus(STATUS_FORCED_STOP);
    }
}

// 0x63BAE0
void CTaskComplexLeaveCar::CreateTaskUtilityLineUpPedWithCar(CPed* ped) {
    // NOTE: The original leaks the previous line-up task (if any)
    m_pTaskUtilityLineUpPedWithCar = std::make_unique<CTaskUtilityLineUpPedWithCar>(CVector{}, 0, 0, m_nTargetDoor);
}

// NOTSA
void CTaskComplexLeaveCar::ClearVehicleFlags() {
    m_pTargetVehicle->ClearGettingOutFlags(m_nDoorFlagsSet);
    m_nDoorFlagsSet = 0;
    m_pTargetVehicle->m_nNumGettingIn -= m_nNumGettingInSet;
    m_nNumGettingInSet = 0;
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
    if (!m_pTargetVehicle) {
        return true;
    }

    switch (priority) {
    case ABORT_PRIORITY_IMMEDIATE: { // 0x641145
        m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_IMMEDIATE, event);
        if (ped->bInVehicle) { // Warp the ped out
            ComputeTargetDoor(ped);
            CTaskSimpleCarCloseDoorFromOutside tCloseDoor{ m_pTargetVehicle, (uint32)m_nTargetDoor, nullptr };
            tCloseDoor.MakeAbortable(ped, ABORT_PRIORITY_IMMEDIATE, event);
            CTaskSimpleCarSetPedOut tSetPedOut{ m_pTargetVehicle, (eTargetDoor)m_nTargetDoor, m_bSensibleLeaveCar };
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
            const auto aborted = AbortSubTask();
            if (aborted) {
                ClearVehicleFlags();
            }
            return aborted;
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
                m_bIsInAir = true;
            }
            if (!AbortSubTask()) {
                return false;
            }

            float blendDelta;
            if (ped->GetActiveWeapon().m_Type == WEAPON_PARACHUTE && eventType == EVENT_IN_AIR && m_pTargetVehicle->GetNumContactWheels() == 0) { // 0x6413fe
                // NOTE: Using MagSq here
                if (!m_pTargetVehicle->IsSubPlane() || m_pTargetVehicle->GetMoveSpeed().SquaredMagnitude() <= sq(0.2f)) {
                    if (!ped->IsPlayer()) {
                        return false;
                    }
                    CColPoint cp{};
                    CEntity*  hitEntity{};
                    if (CWorld::ProcessVerticalLine(ped->GetPosition(), -10.f, cp, hitEntity, true, false, false, false, true)) { // Too close to the ground
                        return false;
                    }
                }
                blendDelta = 8.f;
            } else { // 0x6414a7
                if (eventType != EVENT_IN_WATER || ped->GetMoveSpeed().z >= -0.3f) {
                    return false;
                }
                blendDelta = 16.f;
            }
            CAnimManager::BlendAnimation(ped->GetRpClump(), ANIM_GROUP_DEFAULT, ANIM_ID_FALL_FRONT, blendDelta);
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
        return CreateSubTask(ped->bInVehicle ? TASK_SIMPLE_CAR_SET_PED_OUT : TASK_FINISHED, ped);
    case TASK_COMPLEX_LEAVE_BOAT:     // 0x641a68
    case TASK_SIMPLE_CAR_SET_PED_OUT: // 0x641f49
        return CreateSubTask(m_bDie ? TASK_SIMPLE_DIE : TASK_FINISHED, ped);
    case TASK_SIMPLE_CAR_CLOSE_DOOR_FROM_OUTSIDE: // 0x641a97
        m_nDieAnimID = ANIM_ID_KO_SHOT_FRONT_0;
        return CreateSubTask(TASK_SIMPLE_CAR_SET_PED_OUT, ped);
    case TASK_SIMPLE_CAR_DRIVE_TIMED: { // 0x641ab8
        if (!ped->bInVehicle) {
            return CreateSubTask(TASK_FINISHED, ped);
        }
        if (!ped->m_pVehicle->IsPassenger(ped) && !ped->m_pVehicle->IsDriver(ped)) {
            ped->bInVehicle = false;
            return CreateSubTask(TASK_FINISHED, ped);
        }
        ComputeTargetDoor(ped);
        bool isGettingIn{}, isGettingOut{};
        GetDoorUsage(m_pTargetVehicle, m_nTargetDoor, isGettingIn, isGettingOut);
        if (m_bForceGetOut) {
            return CreateSubTask(TASK_SIMPLE_CAR_WAIT_TO_SLOW_DOWN, ped);
        }
        if (isGettingOut) {
            return CreateSubTask(TASK_SIMPLE_CAR_WAIT_FOR_DOOR_NOT_TO_BE_IN_USE, ped);
        }
        return CreateSubTask(isGettingIn ? TASK_FINISHED : TASK_SIMPLE_CAR_WAIT_TO_SLOW_DOWN, ped);
    }
    case TASK_SIMPLE_CAR_WAIT_TO_SLOW_DOWN: {             // 0x641b68
        if (!m_pTargetVehicle->CanPedStepOutCar(false)) { // 0x641d10
            SetVehicleFlags(ped);
            ped->SetPedState(PEDSTATE_NONE);
            return CreateSubTask(TASK_SIMPLE_CAR_JUMP_OUT, ped);
        }
        if (!CCarEnterExit::IsRoomForPedToLeaveCar(m_pTargetVehicle, m_nTargetDoor)) { // Try the other door on the same side
            const auto ForcePedOut = [&] {                                             // 0x641c23
                SetVehicleFlags(ped);
                ped->SetPedState(PEDSTATE_NONE);
                CreateTaskUtilityLineUpPedWithCar(ped);
                return CreateSubTask(TASK_SIMPLE_CAR_FORCE_PED_OUT, ped);
            };
            auto* const veh        = m_pTargetVehicle.Get();
            const auto  IsSeatFree = [veh](const CPed* occupant, uint8 gettingInFlag) {
                return !occupant && !(veh->m_nGettingInFlags & gettingInFlag);
            };
            const auto CanUseAnyDoor = [veh] {
                return veh->IsBike() || veh->m_pHandlingData->m_bTandemSeats;
            };
            // NOTE: Not using `GetPassengers()`: The original checks these seats even if the vehicle has fewer
            int32 otherDoor = 0;
            switch (m_nTargetDoor) {
            case TARGET_DOOR_FRONT_RIGHT: // 0x641c55
                if (!IsSeatFree(veh->m_pDriver, 1)) {
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
            m_nTargetDoor = otherDoor;
        }
        SetVehicleFlags(ped); // 0x641cc6
        ped->SetPedState(PEDSTATE_NONE);
        if (m_bDie) {
            return CreateSubTask(TASK_COMPLEX_CAR_SLOW_BE_DRAGGED_OUT, ped);
        }
        CreateTaskUtilityLineUpPedWithCar(ped);
        return CreateSubTask(TASK_SIMPLE_CAR_GET_OUT, ped);
    }
    case TASK_SIMPLE_CAR_WAIT_FOR_DOOR_NOT_TO_BE_IN_USE: // 0x641d56
        return CreateSubTask(TASK_SIMPLE_CAR_WAIT_TO_SLOW_DOWN, ped);
    case TASK_SIMPLE_CAR_GET_OUT: { // 0x641d70
        m_pTaskUtilityLineUpPedWithCar->m_nDoorOpenPosType = 2;
        if ((!m_bForceGetOut || m_pTargetVehicle->m_nDoorLock != CARLOCK_UNLOCKED) && CCarEnterExit::CarHasDoorToClose(m_pTargetVehicle, m_nTargetDoor)) {
            return CreateSubTask(TASK_SIMPLE_CAR_CLOSE_DOOR_FROM_OUTSIDE, ped);
        }
        if (m_pTargetVehicle->IsAutomobile() && CCarEnterExit::CarHasDoorToClose(m_pTargetVehicle, m_nTargetDoor)) { // 0x641db9 - Leave the door open
            auto&      dm     = m_pTargetVehicle->AsAutomobile()->GetDamageManager();
            const auto status = dm.GetDoorStatus_Component((tComponent)m_nTargetDoor);
            if (status == DAMSTATE_OK || status == DAMSTATE_DAMAGED) {
                dm.SetDoorStatus_Component((tComponent)m_nTargetDoor, (eDoorStatus)(status + 1));
            }
        }
        return CreateSubTask(TASK_SIMPLE_CAR_SET_PED_OUT, ped);
    }
    case TASK_SIMPLE_CAR_FORCE_PED_OUT: // 0x641e28
        return CreateSubTask(TASK_SIMPLE_CAR_SET_PED_OUT, ped);
    case TASK_SIMPLE_CAR_JUMP_OUT: { // 0x641e42
        if (m_bDie) {
            m_nDieAnimID         = ANIM_ID_KO_SHOT_FRONT_0;
            m_fDieAnimBlendDelta = 1000.f;
            m_fDieAnimSpeed      = 0.5f;
            return CreateSubTask(TASK_SIMPLE_DIE, ped);
        }
        if (!m_pTargetVehicle || m_pTargetVehicle->IsBike() || m_pTargetVehicle->IsSubQuad()) {
            return CreateSubTask(TASK_FINISHED, ped);
        }
        if (m_bIsInAir || ped->bIsDrowning) {
            return CreateSubTask(TASK_SIMPLE_CAR_SET_PED_OUT, ped);
        }
        ClearVehicleFlags();
        return CreateSubTask(TASK_COMPLEX_GET_UP_AND_STAND_STILL, ped);
    }
    case TASK_COMPLEX_CAR_SLOW_BE_DRAGGED_OUT: { // 0x641f00
        m_nDieAnimID         = ANIM_ID_FLOOR_HIT;
        m_fDieAnimBlendDelta = 1000.f;
        m_fDieAnimSpeed      = 0.5f;
        if (m_pTargetVehicle && m_pTargetVehicle->m_nDoorLock == CARLOCK_COP_CAR) {
            m_pTargetVehicle->m_nDoorLock = CARLOCK_UNLOCKED;
        }
        return CreateSubTask(TASK_SIMPLE_DIE, ped);
    }
    default:
        NOTSA_UNREACHABLE_CASE(tt);
    }
}

// 0x641FC0
CTask* CTaskComplexLeaveCar::CreateFirstSubTask(CPed* ped) {
    if ((ped->m_nPedState == PEDSTATE_ARRESTED || ped->bIsBeingArrested) && ped->IsPlayer()) {
        return new CTaskSimplePause{ -1 };
    }
    if (!ped->bInVehicle) { // NOTE: The original prints a debug message here
        return new CTaskSimplePause{ -1 };
    }
    if (m_pTargetVehicle->m_pDriver == ped) { // 0x642073
        if (auto* const grp = CPedGroups::GetPedsGroup(ped); grp && grp->GetMembership().IsLeader(ped)) {
            CEventGroupEvent groupEvent{ ped, new CEventLeaderExitedCarAsDriver{} };
            grp->GetIntelligence().AddEvent(&groupEvent);
        }
        if (!ped->IsPlayer()) {
            ped->SetRadioStation();
        } else if (ped->m_pVehicle) {
            ped->m_pVehicle->m_vehicleAudio.PlayerAboutToExitVehicleAsDriver();
        }
    }
    return m_pTargetVehicle->IsBoat()
        ? static_cast<CTask*>(new CTaskComplexLeaveBoat{ m_pTargetVehicle, 0 })
        : new CTaskSimpleCarDriveTimed{ m_pTargetVehicle, m_nDelayTime };
}

// 0x6421B0
CTask* CTaskComplexLeaveCar::ControlSubTask(CPed* ped) {
    if (!m_pTargetVehicle) {
        return nullptr;
    }
    if (!ped->bInVehicle) {
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
    if (!m_bSensibleLeaveCar && notsa::isa<CTaskSimpleCarWaitToSlowDown>(m_pSubTask)) {
        m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_LEISURE);
    }
    return m_pSubTask;
}

// 0x641530
CTask* CTaskComplexLeaveCar::CreateSubTask(eTaskType taskType, CPed* ped) {
    switch (taskType) {
    case TASK_SIMPLE_CAR_DRIVE_TIMED:
        return new CTaskSimpleCarDriveTimed{ m_pTargetVehicle, m_nDelayTime };
    case TASK_SIMPLE_CAR_WAIT_TO_SLOW_DOWN: {
        using enum CTaskSimpleCarWaitToSlowDown::SlowDownType;
        return new CTaskSimpleCarWaitToSlowDown{
            m_pTargetVehicle,
            m_bDie || !m_bSensibleLeaveCar
                ? DONT_WAIT
                : ped->IsPlayer() ? SLOW_ENOUGH_TO_STEP_OR_JUMP
                                  : SLOW_ENOUGH_TO_STEP
        };
    }
    case TASK_SIMPLE_CAR_WAIT_FOR_DOOR_NOT_TO_BE_IN_USE:
        return new CTaskSimpleCarWaitForDoorNotToBeInUse{ m_pTargetVehicle, (uint32)m_nTargetDoor };
    case TASK_SIMPLE_CAR_FORCE_PED_OUT:
        return new CTaskSimpleCarForcePedOut{ m_pTargetVehicle, m_nTargetDoor };
    case TASK_SIMPLE_CAR_GET_OUT:
        return new CTaskSimpleCarGetOut{ m_pTargetVehicle, (uint32)m_nTargetDoor, m_pTaskUtilityLineUpPedWithCar.get() };
    case TASK_SIMPLE_CAR_JUMP_OUT:
        return new CTaskSimpleCarJumpOut{ m_pTargetVehicle, (uint32)m_nTargetDoor, m_pTaskUtilityLineUpPedWithCar.get() };
    case TASK_SIMPLE_CAR_CLOSE_DOOR_FROM_OUTSIDE:
        return new CTaskSimpleCarCloseDoorFromOutside{ m_pTargetVehicle, (uint32)m_nTargetDoor, m_pTaskUtilityLineUpPedWithCar.get() };
    case TASK_SIMPLE_CAR_SET_PED_OUT:
        return new CTaskSimpleCarSetPedOut{ m_pTargetVehicle, (eTargetDoor)m_nTargetDoor, m_bSensibleLeaveCar };
    case TASK_COMPLEX_CAR_SLOW_BE_DRAGGED_OUT:
        return new CTaskComplexCarSlowBeDraggedOut{ m_pTargetVehicle, (eTargetDoor)m_nTargetDoor, true };
    case TASK_SIMPLE_DIE:
        return new CTaskSimpleDie{ ANIM_GROUP_DEFAULT, (AnimationId)m_nDieAnimID, m_fDieAnimBlendDelta, m_fDieAnimSpeed };
    case TASK_COMPLEX_GET_UP_AND_STAND_STILL:
        return new CTaskComplexGetUpAndStandStill{};
    case TASK_COMPLEX_LEAVE_BOAT:
        return new CTaskComplexLeaveBoat{ m_pTargetVehicle, 0 };
    case TASK_SIMPLE_PAUSE:
        return new CTaskSimplePause{ -1 };
    case TASK_FINISHED:
        return nullptr;
    default:
        NOTSA_UNREACHABLE_CASE(taskType);
    }
}
