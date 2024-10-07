#include "StdInc.h"

#include "TaskComplexGoToPointAndStandStill.h"

#include "TaskSimpleGoToPoint.h"
#include "TaskComplexLeaveCar.h"
#include "TaskSimplePause.h"
#include "TaskSimpleStandStill.h"

void CTaskComplexGoToPointAndStandStill::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexGoToPointAndStandStill, 0x86fda8, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x668120);

    RH_ScopedInstall(GoToPoint, 0x46FE60);
    RH_ScopedVMTInstall(Clone, 0x66CEA0);
    RH_ScopedVMTInstall(GetTaskType, 0x668230);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x66DBA0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x66DC40);
    RH_ScopedVMTInstall(ControlSubTask, 0x668570);
}

// 0x668120
CTaskComplexGoToPointAndStandStill::CTaskComplexGoToPointAndStandStill(
    eMoveState moveState,
    const CVector& targetPoint,
    float fRadius,
    float fMoveStateRadius,
    bool bMustOverShootTarget,
    bool bStopExactly,
    /* \/\/\/ NOTSA ARGS \/\/\/ */
    bool bTrackingEntity
) :
    m_moveState{ moveState },
    m_bMustOverShootTarget{ bMustOverShootTarget },
    m_bStopExactly{ bStopExactly },
    m_bTrackingEntity{bTrackingEntity}
{
    GoToPoint(targetPoint, fRadius, fMoveStateRadius);

    if (m_bStopExactly) {
        m_bMustOverShootTarget = false;
        m_fMoveStateRadius     = 0.0f;
        m_fRadius              = std::max(moveState <= PEDMOVE_WALK ? 0.5f : 1.f, m_fRadius);
    }
}

CTaskComplexGoToPointAndStandStill::~CTaskComplexGoToPointAndStandStill() {
    // NOP
}

// 0x66CEA0
CTask* CTaskComplexGoToPointAndStandStill::Clone() const {
    return new CTaskComplexGoToPointAndStandStill(m_moveState, m_vecTargetPoint, m_fRadius, m_fMoveStateRadius, m_bMustOverShootTarget, m_bStopExactly);
}

// 0x66DBA0
CTask* CTaskComplexGoToPointAndStandStill::CreateNextSubTask(CPed* ped) {
    switch (const auto subtt = m_pSubTask->GetTaskType()) {
    case TASK_SIMPLE_STAND_STILL:
        return CreateSubTask(TASK_FINISHED, ped);
    case TASK_COMPLEX_LEAVE_CAR: {
        return CreateSubTask(ped->bInVehicle ? TASK_FINISHED : TASK_SIMPLE_GO_TO_POINT, ped);
    }
    case TASK_SIMPLE_GO_TO_POINT: {
        m_bWasSuccessful = CTask::Cast<CTaskSimpleGoToPoint>(m_pSubTask)->gotoPointFlags.m_b03;
        return CreateSubTask(TASK_SIMPLE_STAND_STILL, ped);
    }
    default:
        NOTSA_UNREACHABLE("Invalid Sub-Task Type ({})", subtt);
    }
}

// 0x66DC40
CTask* CTaskComplexGoToPointAndStandStill::CreateFirstSubTask(CPed* ped) {
    m_bNewTarget = false;
    return CreateSubTask(ped->bInVehicle ? TASK_COMPLEX_LEAVE_CAR : TASK_SIMPLE_GO_TO_POINT, ped);
}

// 0x668570
CTask* CTaskComplexGoToPointAndStandStill::ControlSubTask(CPed* ped) {
    if (m_bNewTarget) {
        if (m_pSubTask->MakeAbortable(ped))
            return CreateFirstSubTask(ped);

    } else if (m_pSubTask->GetTaskType() == TASK_SIMPLE_GO_TO_POINT) {
        auto gotoPointTask = static_cast<CTaskSimpleGoToPoint*>(m_pSubTask);
        if (m_moveState == PEDMOVE_RUN) {
            SelectMoveState(gotoPointTask, ped, m_fMoveStateRadius, 100'000'000.0f);
        } else if (m_moveState == PEDMOVE_SPRINT) {
            SelectMoveState(gotoPointTask, ped, m_fMoveStateRadius, 10.0f);
        }
        return m_pSubTask;
    }
    return m_pSubTask;
}

// 0x46FE60
void CTaskComplexGoToPointAndStandStill::GoToPoint(const CVector& targetPoint, float fRadius, float fMoveStateRadius, bool bUpdateTargetEvenIfItsTheSame) {
    if (bUpdateTargetEvenIfItsTheSame || m_vecTargetPoint != targetPoint || m_fMoveStateRadius != fMoveStateRadius) {
        m_vecTargetPoint = targetPoint;
        m_fRadius = fRadius;
        m_fMoveStateRadius = fMoveStateRadius;
        m_bNewTarget = true;
    }
}

// 0x668250
void CTaskComplexGoToPointAndStandStill::SelectMoveState(CTaskSimpleGoToPoint* gotoPointTask, CPed* ped, float fMoveStateRadius, float fRunOrSprintRadius) {
    const float dist2DSq = DistanceBetweenPointsSquared2D(gotoPointTask->m_vecTargetPoint, ped->GetPosition());
    gotoPointTask->m_moveState = dist2DSq >= sq(fMoveStateRadius)
        ? dist2DSq >= sq(fRunOrSprintRadius) ? PEDMOVE_SPRINT : PEDMOVE_RUN
        : PEDMOVE_WALK;
}

// 0x6682D0
CTask* CTaskComplexGoToPointAndStandStill::CreateSubTask(eTaskType taskType, CPed* ped) {
    switch (taskType) {
    case TASK_FINISHED:
        return nullptr;
    case TASK_SIMPLE_GO_TO_POINT: {
        const auto tGoToPoint = new CTaskSimpleGoToPoint(m_moveState, m_vecTargetPoint, m_fRadius, false, m_bMustOverShootTarget);

        if (m_bTrackingEntity)
            tGoToPoint->gotoPointFlags.m_b05 = true;

        if (m_moveState == PEDMOVE_RUN)
            SelectMoveState(tGoToPoint, ped, m_fMoveStateRadius, 100'000'000.0f);

        else if (m_moveState == PEDMOVE_SPRINT)
            SelectMoveState(tGoToPoint, ped, m_fMoveStateRadius, 10.0f);

        return tGoToPoint;
    }
    case TASK_COMPLEX_LEAVE_CAR:
        return new CTaskComplexLeaveCar(ped->m_pVehicle, 0, 0, true, false);
    case TASK_SIMPLE_PAUSE:
        return new CTaskSimplePause(1);
    case TASK_SIMPLE_STAND_STILL: {
        if (m_bStopExactly && m_pSubTask && m_pSubTask->GetTaskType() == TASK_SIMPLE_GO_TO_POINT) {
            CVector vecDistance = m_vecTargetPoint - ped->GetPosition();
            float   fDotProduct = DotProduct(&vecDistance, &ped->GetForward());
            float   fBlendDelta = 8.0f;
            if (ped->m_vecAnimMovingShiftLocal.y >= 0.01f) {
                fBlendDelta = 50.0f / ((fDotProduct / (ped->m_vecAnimMovingShiftLocal.y * 0.5f) - 1.0f) * CTimer::GetTimeStep());
                if (fDotProduct <= 0.01f || fBlendDelta > 16.0f)
                    fBlendDelta = 16.0f;
            }
            return new CTaskSimpleStandStill(2000, false, true, fBlendDelta);
        }
        return new CTaskSimpleStandStill(1, false, false, 8.0f);
    }
    default:
        NOTSA_UNREACHABLE("Invalid TaskType ({})", taskType);
    }
}
