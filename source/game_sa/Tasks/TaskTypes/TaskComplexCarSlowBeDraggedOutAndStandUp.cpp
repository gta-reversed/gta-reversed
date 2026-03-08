#include "StdInc.h"

#include "TaskComplexCarSlowBeDraggedOutAndStandUp.h"
#include "TaskComplexCarSlowBeDraggedOut.h"
#include "TaskSimpleStandUp.h"

void CTaskComplexCarSlowBeDraggedOutAndStandUp::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexCarSlowBeDraggedOutAndStandUp, 0x86EF80, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x648620);
    RH_ScopedInstall(Destructor, 0x648690);
    RH_ScopedInstall(CreateSubTask, 0x648710);
    RH_ScopedInstall(Clone, 0x64A190);
    RH_ScopedInstall(GetTaskType, 0x648680);
    RH_ScopedInstall(MakeAbortable, 0x6486F0);
    RH_ScopedInstall(CreateNextSubTask, 0x6488F0);
    RH_ScopedInstall(CreateFirstSubTask, 0x648A10);
    RH_ScopedInstall(ControlSubTask, 0x648B80);
}

// 0x648620
CTaskComplexCarSlowBeDraggedOutAndStandUp::CTaskComplexCarSlowBeDraggedOutAndStandUp(CVehicle* vehicle, int32 a3) : CTaskComplex() {
    m_Vehicle = vehicle;
    dword10 = a3;
    CEntity::SafeRegisterRef(m_Vehicle);
}

// 0x648690
CTaskComplexCarSlowBeDraggedOutAndStandUp::~CTaskComplexCarSlowBeDraggedOutAndStandUp() {
    CEntity::SafeCleanUpRef(m_Vehicle);
}

// 0x648710
CTask* CTaskComplexCarSlowBeDraggedOutAndStandUp::CreateSubTask(eTaskType taskType, CPed* ped) {
    UNUSED(ped);

    switch (taskType) {
    case TASK_COMPLEX_CAR_SLOW_BE_DRAGGED_OUT:
        if (!m_Vehicle) {
            return nullptr;
        }
        return new CTaskComplexCarSlowBeDraggedOut{ m_Vehicle, (eTargetDoor)dword10, false };
    case TASK_SIMPLE_STAND_UP:
        return new CTaskSimpleStandUp{ false };
    case TASK_FINISHED:
        return nullptr;
    default:
        NOTSA_UNREACHABLE("Invalid task type {}", taskType);
    }
}

// 0x6486F0
bool CTaskComplexCarSlowBeDraggedOutAndStandUp::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    if (!m_pSubTask) {
        return true;
    }

    if (priority == ABORT_PRIORITY_IMMEDIATE && m_pSubTask->GetTaskType() == TASK_COMPLEX_CAR_SLOW_BE_DRAGGED_OUT) {
        return false;
    }

    return m_pSubTask->MakeAbortable(ped, priority, event);
}

// 0x6488F0
CTask* CTaskComplexCarSlowBeDraggedOutAndStandUp::CreateNextSubTask(CPed* ped) {
    UNUSED(ped);

    switch (m_pSubTask->GetTaskType()) {
    case TASK_COMPLEX_CAR_SLOW_BE_DRAGGED_OUT:
        return CreateSubTask(TASK_SIMPLE_STAND_UP, ped);
    case TASK_SIMPLE_STAND_UP:
        return CreateSubTask(TASK_FINISHED, ped);
    default:
        NOTSA_UNREACHABLE("Invalid subtask {}", m_pSubTask->GetTaskType());
    }
}

// 0x648A10
CTask* CTaskComplexCarSlowBeDraggedOutAndStandUp::CreateFirstSubTask(CPed* ped) {
    if (!m_Vehicle) {
        return CreateSubTask(TASK_FINISHED, ped);
    }
    return CreateSubTask(TASK_COMPLEX_CAR_SLOW_BE_DRAGGED_OUT, ped);
}

// 0x648B80
CTask* CTaskComplexCarSlowBeDraggedOutAndStandUp::ControlSubTask(CPed* ped) {
    UNUSED(ped);
    return m_pSubTask;
}
