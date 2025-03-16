#include "StdInc.h"

#include "TaskComplexGoToPointAndStandStillAndAchieveHeading.h"
#include "TaskSimpleAchieveHeading.h"
#include "TaskSimpleStandStill.h"
#include "TaskSimpleGoToPoint.h"

void CTaskComplexGoToPointAndStandStillAndAchieveHeading::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexGoToPointAndStandStillAndAchieveHeading, 0x86FE58, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x668CD0);
    RH_ScopedInstall(Destructor, 0x668D40);

    RH_ScopedInstall(Clone, 0x66CFD0, { .reversed = false });
    RH_ScopedInstall(GetTaskType, 0x668D30);
    RH_ScopedInstall(CreateNextSubTask, 0x66DFD0, { .reversed = false });
    RH_ScopedInstall(CreateFirstSubTask, 0x66E030, { .reversed = false });
    RH_ScopedInstall(ControlSubTask, 0x668E80, { .reversed = false });
}

// 0x668CD0
CTaskComplexGoToPointAndStandStillAndAchieveHeading::CTaskComplexGoToPointAndStandStillAndAchieveHeading(eMoveState moveState, const CVector& targetPos, float angle, float radius, float changeRateMult, float maxHeading)
    : CTaskComplex()
{
    m_MoveState = moveState;
    m_TargetPos = targetPos;
    m_Radius = radius;
    m_MaxHeading = maxHeading;
    m_nFlags = m_nFlags | 1; // todo: flags
    m_Angle = angle;
    m_ChangeRateMult = changeRateMult;
}

// 0x66DFD0
CTask* CTaskComplexGoToPointAndStandStillAndAchieveHeading::CreateNextSubTask(CPed* ped) {
    switch (m_pSubTask->GetTaskType()) {
    case TASK_SIMPLE_GO_TO_POINT:
        return CreateSubTask(TASK_SIMPLE_STAND_STILL);
    case TASK_SIMPLE_ACHIEVE_HEADING:
        return CreateSubTask(TASK_FINISHED);
    case TASK_SIMPLE_STAND_STILL:
        return CreateSubTask(TASK_SIMPLE_ACHIEVE_HEADING);
    default:
        return nullptr;
    }
}

// 0x668D50
CTask* CTaskComplexGoToPointAndStandStillAndAchieveHeading::CreateSubTask(eTaskType taskType) {
    switch (taskType) {
    case TASK_SIMPLE_ACHIEVE_HEADING:
        return new CTaskSimpleAchieveHeading(m_Angle, m_ChangeRateMult, m_MaxHeading);
    case TASK_SIMPLE_STAND_STILL:
        return new CTaskSimpleStandStill{};
    case TASK_SIMPLE_GO_TO_POINT:
        return new CTaskSimpleGoToPoint(m_MoveState, m_TargetPos, m_Radius, false, false);
    default:
        return nullptr;
    }
}

// 0x66E030
CTask* CTaskComplexGoToPointAndStandStillAndAchieveHeading::CreateFirstSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x66E030, CTaskComplexGoToPointAndStandStillAndAchieveHeading*, CPed*>(this, ped);
}

// 0x668E80
CTask* CTaskComplexGoToPointAndStandStillAndAchieveHeading::ControlSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x668E80, CTaskComplexGoToPointAndStandStillAndAchieveHeading*, CPed*>(this, ped);
}
