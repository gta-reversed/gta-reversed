#include "StdInc.h"

#include "TaskComplexEvasiveStep.h"
#include "TaskSimpleEvasiveStep.h"
#include "TaskSimpleAchieveHeading.h"

// 0x6532D0
CTaskComplexEvasiveStep::CTaskComplexEvasiveStep(CEntity* entity, const CVector& moveDir) :
    m_MoveDir{ moveDir },
    m_Entity{ entity } {
}

// NOTSA
CTaskComplexEvasiveStep::CTaskComplexEvasiveStep(const CTaskComplexEvasiveStep& o) :
    CTaskComplexEvasiveStep{ o.m_Entity, o.m_MoveDir } {
}

// 0x6533E0
CTask* CTaskComplexEvasiveStep::CreateSubTask(eTaskType taskType) {
    switch (taskType) {
    case TASK_SIMPLE_EVASIVE_STEP: // 0x6534AF
        return new CTaskSimpleEvasiveStep{ m_Entity };
    case TASK_SIMPLE_ACHIEVE_HEADING: // 0x65342F
        // NOTE: Not `Heading()`, the original's angle function replaces a zero y with 0.0001
        return new CTaskSimpleAchieveHeading{ CGeneral::GetRadianAngleBetweenPoints(-m_MoveDir.x, -m_MoveDir.y, 0.f, 0.f), 2.f, 0.2f };
    case TASK_FINISHED: // 0x65341B
        return nullptr;
    default:
        NOTSA_UNREACHABLE_CASE(taskType);
    }
}

// 0x6534F0
CTask* CTaskComplexEvasiveStep::CreateNextSubTask(CPed* ped) {
    switch (m_pSubTask->GetTaskType()) {
    case TASK_SIMPLE_EVASIVE_STEP:
        return CreateSubTask(TASK_FINISHED);
    case TASK_SIMPLE_ACHIEVE_HEADING:
        return CreateSubTask(TASK_SIMPLE_EVASIVE_STEP);
    default:
        return nullptr;
    }
}

// 0x653540
CTask* CTaskComplexEvasiveStep::CreateFirstSubTask(CPed* ped) {
    return CreateSubTask(TASK_SIMPLE_ACHIEVE_HEADING);
}

void CTaskComplexEvasiveStep::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexEvasiveStep, 0x86f218, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedConstructorInstall(0x6532D0, "1", {}, CEntity*, const CVector&);
    RH_ScopedDestructorInstall(0x653350);

    RH_ScopedInstall(CreateSubTask, 0x6533E0);

    RH_ScopedVMTInstall(Clone, 0x655870);
    RH_ScopedVMTInstall(GetTaskType, 0x653340);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x6534F0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x653540);
    RH_ScopedVMTInstall(ControlSubTask, 0x653550);
}
