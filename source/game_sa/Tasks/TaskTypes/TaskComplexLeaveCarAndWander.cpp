#include "StdInc.h"
#include "TaskComplexLeaveCarAndWander.h"
#include "TaskSimpleNone.h"
#include "TaskComplexLeaveCar.h"
#include "TaskComplexWander.h"

void CTaskComplexLeaveCarAndWander::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexLeaveCarAndWander, 0x86e880, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x63BDD0);

    RH_ScopedInstall(Destructor, 0x63BE50);

    RH_ScopedInstall(CreateSubTask, 0x63BEC0, {.reversed = false});

    RH_ScopedVMTInstall(Clone, 0x63DAD0, {.reversed = false});
    RH_ScopedVMTInstall(GetTaskType, 0x63BE40, {.reversed = false});
    RH_ScopedVMTInstall(CreateNextSubTask, 0x6422C0, {.reversed = false});
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x642340, {.reversed = false});
    RH_ScopedVMTInstall(ControlSubTask, 0x63BEB0, {.reversed = false});
}

// 0x63BDD0
CTaskComplexLeaveCarAndWander::CTaskComplexLeaveCarAndWander(CVehicle* veh, eTargetDoor door, int32 leaveDelayMs, bool sensibleLeaveVeh) :
    m_veh{veh},
    m_door{door},
    m_bSensibleLeaveCar{sensibleLeaveVeh},
    m_leaveDelayMs{ leaveDelayMs }
{
    CEntity::SafeRegisterRef(m_veh);
}

CTaskComplexLeaveCarAndWander::CTaskComplexLeaveCarAndWander(const CTaskComplexLeaveCarAndWander& o) :
    CTaskComplexLeaveCarAndWander{ o.m_veh, o.m_door, o.m_leaveDelayMs, o.m_bSensibleLeaveCar }
{
}

// 0x63BE50
CTaskComplexLeaveCarAndWander::~CTaskComplexLeaveCarAndWander() {
    CEntity::SafeCleanUpRef(m_veh);
}

// 0x63BEC0
CTask* CTaskComplexLeaveCarAndWander::CreateSubTask(eTaskType taskType) {
    switch (taskType) {
    case TASK_COMPLEX_LEAVE_CAR:
        return new CTaskComplexLeaveCar{
            m_veh,
            (int32)m_door,
            m_leaveDelayMs,
            m_bSensibleLeaveCar,
            false
        };
    case TASK_NONE:
        return new CTaskSimpleNone{};
    case TASK_FINISHED:
        return nullptr;
    default:
        NOTSA_UNREACHABLE();
    }
}

// 0x6422C0
CTask* CTaskComplexLeaveCarAndWander::CreateNextSubTask(CPed* ped) {
    switch (m_pSubTask->GetTaskType()) {
    case TASK_NONE:
    case TASK_COMPLEX_LEAVE_CAR:
        break;
    default:
        NOTSA_UNREACHABLE();
    }

    // Make sure ped's default task is `ComplexWander`
    if (const auto defaultPrimary = ped->GetTaskManager().GetTaskPrimary(TASK_PRIMARY_DEFAULT)) {
        if (!CTask::IsA<TASK_COMPLEX_WANDER>(defaultPrimary)) {
            ped->GetTaskManager().SetTask(CTaskComplexWander::GetWanderTaskByPedType(ped), TASK_PRIMARY_DEFAULT);
        }
    }

    return CreateSubTask(TASK_FINISHED);
}

// 0x642340
CTask* CTaskComplexLeaveCarAndWander::CreateFirstSubTask(CPed* ped) {
    return CreateSubTask(
        ped->bInVehicle
            ? TASK_COMPLEX_LEAVE_CAR
            : TASK_NONE
    );
}
