#include "StdInc.h"

#include "TaskComplexEvasiveCower.h"
#include "TaskSimpleCower.h"
#include "TaskSimpleAchieveHeading.h"

// 0x655460
CTaskComplexEvasiveCower::CTaskComplexEvasiveCower(CEntity* entity, const CVector& pos) :
    m_Pos{ pos },
    m_Entity{ entity } {
}

// 0x6556A0
CTask* CTaskComplexEvasiveCower::CreateNextSubTask(CPed* ped) {
    switch (const auto tt = m_pSubTask->GetTaskType()) {
    case TASK_SIMPLE_ACHIEVE_HEADING:
        return CreateSubTask(TASK_SIMPLE_COWER);
    case TASK_SIMPLE_COWER:
        return CreateSubTask(TASK_FINISHED);
    default:
        NOTSA_UNREACHABLE_CASE(tt);
    }
}

// 0x6556F0
CTask* CTaskComplexEvasiveCower::CreateFirstSubTask(CPed* ped) {
    return CreateSubTask(TASK_SIMPLE_ACHIEVE_HEADING);
}

// 0x655570
CTask* CTaskComplexEvasiveCower::CreateSubTask(eTaskType taskType) {
    switch (taskType) {
    case TASK_SIMPLE_COWER:
        return new CTaskSimpleCower{};
    case TASK_SIMPLE_ACHIEVE_HEADING:
        return new CTaskSimpleAchieveHeading{ m_Pos.Heading(), 0.0f }; // NOTE: Should check if `.Heading()` is same as `CGeneral::GetRadianAngleBetweenPoints` used in the original code
    case TASK_FINISHED:
        return nullptr;
    default:
        NOTSA_UNREACHABLE_CASE(taskType);
    }
}

void CTaskComplexEvasiveCower::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexEvasiveCower, 0x86f3bc, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedConstructorInstall(0x655460, "1", {}, CEntity*, const CVector&);
    RH_ScopedDestructorInstall(0x6554E0);

    RH_ScopedInstall(CreateSubTask, 0x655570);
    RH_ScopedVMTInstall(Clone, 0x655CD0);
    RH_ScopedVMTInstall(GetTaskType, 0x6554D0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x6556A0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x6556F0);
    RH_ScopedVMTInstall(ControlSubTask, 0x655700);
}
