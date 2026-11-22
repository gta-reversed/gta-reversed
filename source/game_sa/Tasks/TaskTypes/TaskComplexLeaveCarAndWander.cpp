#include "StdInc.h"
#include "TaskComplexLeaveCarAndWander.h"
#include "TaskSimpleNone.h"
#include "TaskComplexLeaveCar.h"
#include "TaskComplexWander.h"

// 0x63BDD0
CTaskComplexLeaveCarAndWander::CTaskComplexLeaveCarAndWander(CVehicle* veh, eTargetDoor door, int32 leaveDelayMs, bool sensibleLeaveVeh) :
    m_CarToLeave{ veh },
    m_DoorToUse{ door },
    m_SensibleLeaveCar{ sensibleLeaveVeh },
    m_LeaveDelayMs{ leaveDelayMs } {
}

// 0x63DAD0
CTaskComplexLeaveCarAndWander::CTaskComplexLeaveCarAndWander(const CTaskComplexLeaveCarAndWander& o) :
    CTaskComplexLeaveCarAndWander{ o.m_CarToLeave, o.m_DoorToUse, o.m_LeaveDelayMs, o.m_SensibleLeaveCar } {
}

// 0x63BEC0
CTask* CTaskComplexLeaveCarAndWander::CreateSubTask(eTaskType taskType) {
    switch (taskType) {
    case TASK_COMPLEX_LEAVE_CAR:
        return new CTaskComplexLeaveCar{
            m_CarToLeave,
            (int32)m_DoorToUse,
            m_LeaveDelayMs,
            m_SensibleLeaveCar,
            false
        };
    case TASK_NONE:
        return new CTaskSimpleNone{};
    case TASK_FINISHED:
        return nullptr;
    default:
        NOTSA_UNREACHABLE_CASE(taskType);
    }
}

// 0x6422C0
CTask* CTaskComplexLeaveCarAndWander::CreateNextSubTask(CPed* ped) {
    switch (const auto tt = m_pSubTask->GetTaskType()) {
    case TASK_NONE:
    case TASK_COMPLEX_LEAVE_CAR: {
        if (!notsa::isa_and_present<CTaskComplexWander>(ped->GetTaskManager().GetTaskPrimary(TASK_PRIMARY_DEFAULT))) {
            ped->GetTaskManager().SetTask(CTaskComplexWander::GetWanderTaskByPedType(ped), TASK_PRIMARY_DEFAULT);
        }
        return CreateSubTask(TASK_FINISHED);
    }
    default:
        NOTSA_UNREACHABLE_CASE(tt);
    }
}

// 0x642340
CTask* CTaskComplexLeaveCarAndWander::CreateFirstSubTask(CPed* ped) {
    return CreateSubTask(
        ped->bInVehicle
            ? TASK_COMPLEX_LEAVE_CAR
            : TASK_NONE
    );
}

void CTaskComplexLeaveCarAndWander::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexLeaveCarAndWander, 0x86e880, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedConstructorInstall(0x63BDD0, "0", {}, CVehicle*, eTargetDoor, int32, bool);
    RH_ScopedDestructorInstall(0x63BE50);

    RH_ScopedInstall(CreateSubTask, 0x63BEC0);
    RH_ScopedVMTInstall(Clone, 0x63DAD0);
    RH_ScopedVMTInstall(GetTaskType, 0x63BE40);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x6422C0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x642340);
    RH_ScopedVMTInstall(ControlSubTask, 0x63BEB0);
}
