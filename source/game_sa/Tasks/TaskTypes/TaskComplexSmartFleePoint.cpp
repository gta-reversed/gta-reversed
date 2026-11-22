#include "StdInc.h"
#include "TaskComplexSmartFleePoint.h"
#include "TaskComplexWander.h"
#include "TaskComplexWanderFlee.h"
#include "TaskComplexLeaveAnyCar.h"
#include "TaskSimpleStandStill.h"
#include "TaskSimpleRunAnim.h"
#include "TaskSimpleTired.h"
#include "TaskComplexSequence.h"
#include "Ragdoll/IKChainManager.h"

void CTaskComplexSmartFleePoint::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexSmartFleePoint, 0x86f744, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x65BD20);
    RH_ScopedInstall(Destructor, 0x65BDB0);

    RH_ScopedInstall(SetDefaultTaskWanderDir, 0x65BE00);
    RH_ScopedInstall(ComputeFleeDir, 0x65BE40);
    RH_ScopedInstall(CreateSubTask, 0x65BE80);
    RH_ScopedInstall(SetFleePosition, 0x65C3C0);

    RH_ScopedVMTInstall(Clone, 0x65CED0);
    RH_ScopedVMTInstall(GetTaskType, 0x65BDA0, {.Reversed = false});
    RH_ScopedVMTInstall(MakeAbortable, 0x65BDC0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x65C0C0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x65C140);
    RH_ScopedVMTInstall(ControlSubTask, 0x65C1E0);
}

// 0x65BD20
CTaskComplexSmartFleePoint::CTaskComplexSmartFleePoint(CVector const& fleePos, bool doScream, float safeDist, int32 fleeTimeMs) :
    m_fleePoint{fleePos},
    m_doScream{doScream},
    m_safeDist{safeDist},
    m_fleeTimeMs{fleeTimeMs}
{
    m_timer.Start(m_fleeTimeMs);
}

CTaskComplexSmartFleePoint::CTaskComplexSmartFleePoint(const CTaskComplexSmartFleePoint& o)
    : CTaskComplexSmartFleePoint{o.m_fleePoint, o.m_doScream, o.m_safeDist, o.m_fleeTimeMs}
{
}

// 0x65BE00
void CTaskComplexSmartFleePoint::SetDefaultTaskWanderDir(CPed* ped) {
    // Hand our flee-wander's path nodes over to the ped's default wander task, so it continues from where we stopped
    auto* const defaultTask = ped->GetTaskManager().GetTaskPrimary(TASK_PRIMARY_DEFAULT);
    if (defaultTask && defaultTask->GetTaskType() == TASK_COMPLEX_WANDER) {
        static_cast<CTaskComplexWander*>(defaultTask)->SetPathNodesFrom(*static_cast<CTaskComplexWander*>(m_pSubTask));
    }
}

// 0x65BE40
uint32 CTaskComplexSmartFleePoint::ComputeFleeDir(CPed* ped) {
    const auto& pedPos = ped->GetPosition();
    return CGeneral::GetNodeHeadingFromVector(pedPos.x - m_fleePoint.x, pedPos.y - m_fleePoint.y);
}

// 0x65BE80
CTask* CTaskComplexSmartFleePoint::CreateSubTask(eTaskType taskType, CPed* ped) {
    switch (taskType) {
    case TASK_COMPLEX_LEAVE_ANY_CAR:
        return new CTaskComplexLeaveAnyCar{ 0, false, true };
    case TASK_COMPLEX_WANDER:
        return new CTaskComplexWanderFlee{ m_moveState, m_fleeDir };
    case TASK_SIMPLE_STAND_STILL:
        SetDefaultTaskWanderDir(ped);
        return new CTaskSimpleStandStill{ 0, false, false, 8.f };
    case TASK_COMPLEX_SEQUENCE: {
        if (m_moveState != PEDMOVE_RUN) {
            return nullptr;
        }
        auto* const seq = new CTaskComplexSequence{};
        seq->AddTask(new CTaskSimpleRunAnim{ ANIM_GROUP_DEFAULT, ANIM_ID_FLEE_LKAROUND_01, 4.f, false });
        seq->AddTask(new CTaskSimpleTired{ 2'000 });
        return seq;
    }
    case TASK_FINISHED:
    default:
        return nullptr;
    }
}

// 0x65C3C0
void CTaskComplexSmartFleePoint::SetFleePosition(const CVector& fleePos, float safeDist, bool doScream) {
    if (m_fleePoint != fleePos || m_safeDist != safeDist) {
        m_fleePoint           = fleePos;
        m_safeDist            = safeDist;
        m_hasFleePointChanged = true;
    }
    m_doScream = doScream;
}

// 0x65BDC0
bool CTaskComplexSmartFleePoint::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
    if (priority == ABORT_PRIORITY_LEISURE) {
        m_fleeTimeMs = -1;

        // Started with raw fields, as `CTaskTimer::Start` refuses negative times
        m_timer.m_nStartTime = CTimer::GetTimeInMS();
        m_timer.m_nInterval  = m_fleeTimeMs;
        m_timer.m_bStarted   = true;
    }
    return m_pSubTask->MakeAbortable(ped, priority, event);
}

// 0x65C0C0
CTask* CTaskComplexSmartFleePoint::CreateNextSubTask(CPed* ped) {
    switch (m_pSubTask->GetTaskType()) {
    case TASK_SIMPLE_STAND_STILL:
        return CreateSubTask(TASK_COMPLEX_SEQUENCE, ped);
    case TASK_COMPLEX_SEQUENCE:
        return CreateSubTask(TASK_FINISHED, ped);
    case TASK_COMPLEX_LEAVE_ANY_CAR:
        m_fleeDir = (uint8)ComputeFleeDir(ped);
        return CreateSubTask(TASK_COMPLEX_WANDER, ped);
    default:
        return nullptr;
    }
}

// 0x65C140
CTask* CTaskComplexSmartFleePoint::CreateFirstSubTask(CPed* ped) {
    m_initalPos = ped->GetPosition();
    if (ped->bInVehicle && ped->m_pVehicle) {
        return CreateSubTask(TASK_COMPLEX_LEAVE_ANY_CAR, ped);
    }
    m_fleeDir = (uint8)ComputeFleeDir(ped);
    return CreateSubTask(TASK_COMPLEX_WANDER, ped);
}

// 0x65C1E0
CTask* CTaskComplexSmartFleePoint::ControlSubTask(CPed* ped) {
    if (m_pSubTask->GetTaskType() != TASK_COMPLEX_WANDER) {
        if (g_ikChainMan.IsLooking(ped)) {
            g_ikChainMan.AbortLookAt(ped, 250);
        }
        return m_pSubTask;
    }

    if (m_doScream) {
        ped->Say(CTX_GLOBAL_PAIN_PANIC, 0, 0.1f);
    }

    auto* const wander   = static_cast<CTaskComplexWander*>(m_pSubTask);
    wander->m_nMoveState = m_moveState;

    CTask* subTask       = m_pSubTask;
    if (m_hasFleePointChanged) {
        m_hasFleePointChanged = false;
        if (m_timer.m_bStarted) { // Restart with raw fields (`m_fleeTimeMs` may be -1, see `MakeAbortable`)
            m_timer.m_nStartTime = CTimer::GetTimeInMS();
            m_timer.m_nInterval  = m_fleeTimeMs;
            m_timer.m_bStarted   = true;
        }
        if (const auto dir = (uint8)ComputeFleeDir(ped); m_fleeDir != dir) {
            m_fleeDir = dir;
            wander->SetDirection(dir);
        }
    } else if (m_timer.m_bStarted && m_timer.IsOutOfTime()) {
        subTask = CreateSubTask(TASK_SIMPLE_STAND_STILL, ped);
    } else {
        const auto& pedPos     = ped->GetPosition();
        const auto  safeDistSq = sq(m_safeDist);
        if ((m_fleePoint - pedPos).SquaredMagnitude() > safeDistSq && (m_initalPos - pedPos).SquaredMagnitude() > safeDistSq) {
            subTask = CreateSubTask(TASK_SIMPLE_STAND_STILL, ped);
        }
    }

    if (m_moveState == PEDMOVE_RUN && !g_ikChainMan.IsLooking(ped) && CGeneral::GetRandomNumberInRange(0, 100) <= 5) {
        g_ikChainMan.LookAt("TaskSmartFleePoint", ped, nullptr, 2'000, BONE_UNKNOWN, &m_fleePoint, false, 0.25f, 500, 3, false);
    }

    return subTask;
}
