#include "StdInc.h"

#include "TaskComplexEvasiveStep.h"
#include "TaskSimpleEvasiveStep.h"
#include "TaskSimpleAchieveHeading.h"

void CTaskComplexEvasiveStep::InjectHooks() {
    RH_ScopedClass(CTaskComplexEvasiveStep);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x6532D0);
    RH_ScopedInstall(Destructor, 0x653350);

    RH_ScopedInstall(CreateSubTask, 0x6533E0);

    RH_ScopedVMTInstall(Clone, 0x655870);
    RH_ScopedVMTInstall(GetTaskType, 0x653340);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x6534F0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x653540);
    RH_ScopedVMTInstall(ControlSubTask, 0x653550);
}

// 0x6532D0
CTaskComplexEvasiveStep::CTaskComplexEvasiveStep(CEntity* entity, const CVector& pos) : CTaskComplex() {
    m_Pos = pos;
    m_Entity = entity;
    CEntity::SafeRegisterRef(m_Entity);
}

// 0x653350
CTaskComplexEvasiveStep::~CTaskComplexEvasiveStep() {
    CEntity::SafeCleanUpRef(m_Entity);
}

// 0x6533E0
CTask* CTaskComplexEvasiveStep::CreateSubTask(eTaskType taskType) {
    return plugin::CallMethodAndReturn<CTask*, 0x6533E0, CTaskComplexEvasiveStep*, eTaskType>(this, taskType);

    switch (taskType) {
    case TASK_SIMPLE_EVASIVE_STEP:
        return new CTaskSimpleEvasiveStep(m_Entity);
    case TASK_SIMPLE_ACHIEVE_HEADING: {
        auto angle = CGeneral::GetRadianAngleBetweenPoints(-m_Pos.x, -m_Pos.y, 0.0f, 0.0f);
        return new CTaskSimpleAchieveHeading(angle, 2.0f, 0.2f);
    }
    case TASK_FINISHED:
        return nullptr;
    default:
        return nullptr;
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
