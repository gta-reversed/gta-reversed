#include "StdInc.h"
#include "extensions/utility.hpp"
#include "TaskComplexLeaveCarAsPassengerWait.h"
#include "TaskSimpleGoToPoint.h"
#include "TaskComplexLeaveCar.h"
#include "TaskSimpleCarDrive.h"
#include "TaskSimpleCarSetPedOut.h"
#include "TaskSimpleCarSetPedInAsPassenger.h"
 
void CTaskComplexLeaveCarAsPassengerWait::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexLeaveCarAsPassengerWait, 0x86e7a4, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x63B430);
    RH_ScopedInstall(Destructor, 0x63B4A0);

    RH_ScopedInstall(GetNextPassengerToLeave, 0x63B500);
    RH_ScopedInstall(SetPedInZerothPassengerSeat, 0x63B540);
    RH_ScopedInstall(CreateSubTask, 0x640BE0);

    RH_ScopedVMTInstall(Clone, 0x63D8C0);
    RH_ScopedVMTInstall(GetTaskType, 0x63B490);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x643F90);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x644010);
    RH_ScopedVMTInstall(ControlSubTask, 0x644050);
}

// 0x63B430
CTaskComplexLeaveCarAsPassengerWait::CTaskComplexLeaveCarAsPassengerWait(CVehicle* veh) :
    m_veh{veh}
{
    CEntity::SafeRegisterRef(m_veh);
}

CTaskComplexLeaveCarAsPassengerWait::CTaskComplexLeaveCarAsPassengerWait(const CTaskComplexLeaveCarAsPassengerWait& o) :
    CTaskComplexLeaveCarAsPassengerWait{ o.m_veh }
{
}

// 0x63B4A0
CTaskComplexLeaveCarAsPassengerWait::~CTaskComplexLeaveCarAsPassengerWait(){
    CEntity::SafeCleanUpRef(m_veh);
}

/*!
* @addr 0x63B500
* @brief Get the very next passanger (any of the occupants excluding the driver) to leave the vehicle
*/
CPed* CTaskComplexLeaveCarAsPassengerWait::GetNextPassengerToLeave() {
    const auto passengers = m_veh->GetPassengers();
    const auto it = rng::find_if(passengers, notsa::NotIsNull{});
    return it != passengers.end()
        ? *it
        : nullptr;
}

// 0x63B540
void CTaskComplexLeaveCarAsPassengerWait::SetPedInZerothPassengerSeat(CPed* ped) {
    CTaskSimpleCarSetPedOut{ m_veh, TARGET_DOOR_FRONT_RIGHT, true, true }.ProcessPed(ped);
    CTaskSimpleCarSetPedInAsPassenger{ m_veh, TARGET_DOOR_FRONT_RIGHT, true }.ProcessPed(ped);
}

// 0x640BE0
CTask* CTaskComplexLeaveCarAsPassengerWait::CreateSubTask(eTaskType taskType, CPed* ped) {
    switch (taskType) {
    case TASK_SIMPLE_GO_TO_POINT:
        return new CTaskSimpleGoToPoint{
            PEDMOVE_WALK,
            ped->GetPosition()
                + m_veh->GetRight() * CGeneral::GetRandomNumberInRange(1.f, 2.f)
                + m_veh->GetForward() * (CGeneral::GetRandomNumberInRange(1.f, 2.f) * (CGeneral::DoCoinFlip() ? -1.f : 1.f)),
            1.5f
        };
    case TASK_COMPLEX_LEAVE_CAR: {
        SetPedInZerothPassengerSeat(ped);
        return new CTaskComplexLeaveCar{
            m_veh,
            TARGET_DOOR_FRONT_LEFT,
            0,
            true,
            false
        };
    }
    case TASK_SIMPLE_CAR_DRIVE:
        return new CTaskSimpleCarDrive{ m_veh };
    case TASK_FINISHED:
        return nullptr;
    default:
        NOTSA_UNREACHABLE();
    }
}

// 0x643F90
CTask* CTaskComplexLeaveCarAsPassengerWait::CreateNextSubTask(CPed* ped) {
    return CreateSubTask([this, ped] {
        switch (m_pSubTask->GetTaskType()) {
        case TASK_COMPLEX_LEAVE_CAR:
            return m_veh
                ? TASK_SIMPLE_GO_TO_POINT
                : TASK_FINISHED;
        case TASK_SIMPLE_CAR_DRIVE:
        case TASK_SIMPLE_GO_TO_POINT:
            return TASK_FINISHED;
        default:
            NOTSA_UNREACHABLE();
        }
    }(), ped);
}

// 0x644010
CTask* CTaskComplexLeaveCarAsPassengerWait::CreateFirstSubTask(CPed* ped) {
    return CreateSubTask([this, ped] {
        if (!m_veh) {
            return TASK_FINISHED;
        }
        return GetNextPassengerToLeave() == ped
            ? TASK_COMPLEX_LEAVE_CAR
            : TASK_SIMPLE_CAR_DRIVE;
    }(), ped);
}

// 0x644050
CTask* CTaskComplexLeaveCarAsPassengerWait::ControlSubTask(CPed* ped) {
    if (m_pSubTask->GetTaskType() == TASK_SIMPLE_CAR_DRIVE) {
        if (GetNextPassengerToLeave() == ped) {
            if (m_pSubTask->MakeAbortable(ped)) {
                return CreateSubTask(TASK_COMPLEX_LEAVE_CAR, ped);
            }
        }
    }
    return m_pSubTask;
}
