#include "StdInc.h"

#include "TaskComplexGoToPointAndStandStillAndAchieveHeading.h"
#include "TaskSimpleAchieveHeading.h"
#include "TaskSimpleStandStill.h"
#include "TaskSimpleGoToPoint.h"

void CTaskComplexGoToPointAndStandStillAndAchieveHeading::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexGoToPointAndStandStillAndAchieveHeading, 0x86FE58, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedConstructorInstall(0x668CD0, "1", {}, eMoveState, const CVector&, float, float, float, float);
    RH_ScopedDestructorInstall(0x668D40);

    RH_ScopedInstall(CreateSubTask, 0x668D50);
    RH_ScopedVMTInstall(Clone, 0x66CFD0);
    RH_ScopedVMTInstall(GetTaskType, 0x668D30);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x66DFD0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x66E030);
    RH_ScopedVMTInstall(ControlSubTask, 0x668E80);
}

// 0x668CD0
CTaskComplexGoToPointAndStandStillAndAchieveHeading::CTaskComplexGoToPointAndStandStillAndAchieveHeading(
    eMoveState     moveState,
    const CVector& targetPos,
    float          desirdHeading,
    float          targetRadius,
    float          changeRateMult,
    float          headingTolerance
) :
    m_MoveState{ moveState },
    m_TargetPos{ targetPos },
    m_TargetRadius{ targetRadius },
    m_DesiredHeading{ desirdHeading },
    m_HeadingChangeRateMult{ changeRateMult },
    m_HeadingTolerance{ headingTolerance } {
}

// 0x66CFD0
CTaskComplexGoToPointAndStandStillAndAchieveHeading::CTaskComplexGoToPointAndStandStillAndAchieveHeading(const CTaskComplexGoToPointAndStandStillAndAchieveHeading& o) :
    CTaskComplexGoToPointAndStandStillAndAchieveHeading{
        o.m_MoveState,
        o.m_TargetPos,
        o.m_DesiredHeading,
        o.m_TargetRadius,
        o.m_HeadingChangeRateMult,
        o.m_HeadingTolerance
    } {
}

// 0x66DFD0
CTask* CTaskComplexGoToPointAndStandStillAndAchieveHeading::CreateNextSubTask(CPed* ped) {
    switch (const auto subtt = m_pSubTask->GetTaskType()) {
    case TASK_SIMPLE_GO_TO_POINT:
        return CreateSubTask(TASK_SIMPLE_STAND_STILL);
    case TASK_SIMPLE_ACHIEVE_HEADING:
        return CreateSubTask(TASK_FINISHED);
    case TASK_SIMPLE_STAND_STILL:
        return CreateSubTask(TASK_SIMPLE_ACHIEVE_HEADING);
    default:
        NOTSA_UNREACHABLE_CASE(subtt);
    }
}

// 0x668D50
CTask* CTaskComplexGoToPointAndStandStillAndAchieveHeading::CreateSubTask(eTaskType taskType) {
    switch (taskType) {
    case TASK_SIMPLE_ACHIEVE_HEADING:
        return new CTaskSimpleAchieveHeading{ m_DesiredHeading, m_HeadingChangeRateMult, m_HeadingTolerance };
    case TASK_SIMPLE_STAND_STILL:
        return new CTaskSimpleStandStill{};
    case TASK_SIMPLE_GO_TO_POINT:
        return new CTaskSimpleGoToPoint{ m_MoveState, m_TargetPos, m_TargetRadius, false, false };
    default:
        NOTSA_UNREACHABLE_CASE(taskType);
    }
}

// 0x66E030
CTask* CTaskComplexGoToPointAndStandStillAndAchieveHeading::CreateFirstSubTask(CPed* ped) {
    m_HasNewTarget = false;
    return CreateSubTask(TASK_SIMPLE_GO_TO_POINT);
}

// 0x668E80
CTask* CTaskComplexGoToPointAndStandStillAndAchieveHeading::ControlSubTask(CPed* ped) {
    if (m_HasNewTarget) {
        return CreateFirstSubTask(ped);
    }
    if (auto* const tGoToPoint = notsa::dyn_cast<CTaskSimpleGoToPoint>(m_pSubTask)) {
        tGoToPoint->m_moveState = m_MoveState;
    }
    return m_pSubTask;
}
