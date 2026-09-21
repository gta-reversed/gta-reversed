#include "StdInc.h"
#include "TaskComplexSmartFleePoint.h"
#include "TaskComplexLeaveAnyCar.h"
#include "TaskComplexSequence.h"
#include "TaskComplexWanderFlee.h"
#include "TaskSimpleRunAnim.h"
#include "TaskSimpleStandStill.h"
#include "TaskSimpleTired.h"
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
    RH_ScopedVMTInstall(GetTaskType, 0x65BDA0);
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
// Sync the default wander task's nodes/dir with our wander subtask's (so the ped keeps wandering sensibly afterwards)
void CTaskComplexSmartFleePoint::SetDefaultTaskWanderDir(CPed* ped) {
    const auto defaultTask = ped->GetTaskManager().m_aPrimaryTasks[TASK_PRIMARY_DEFAULT];
    if (!defaultTask || defaultTask->GetTaskType() != TASK_COMPLEX_WANDER) {
        return;
    }
    const auto wander = static_cast<CTaskComplexWander*>(defaultTask);
    const auto subWander = static_cast<CTaskComplexWander*>(m_pSubTask);
    if (wander->m_LastNode != subWander->m_LastNode || wander->m_NextNode != subWander->m_NextNode) {
        wander->m_LastNode = subWander->m_LastNode;
        wander->m_NextNode = subWander->m_NextNode;
        wander->m_nDir = subWander->m_nDir;
        wander->m_bNewNodes = true;
    }
}

// 0x65BE40
uint32 CTaskComplexSmartFleePoint::ComputeFleeDir(CPed* ped) {
    const auto& pedPos = ped->m_matrix ? ped->GetMatrix().GetPosition() : ped->m_placement.m_vPosn;
    return CGeneral::GetNodeHeadingFromVector(pedPos.x - m_fleePoint.x, pedPos.y - m_fleePoint.y);
}

// 0x65BE80
CTask* CTaskComplexSmartFleePoint::CreateSubTask(eTaskType taskType, CPed* ped) {
    switch (taskType) {
    case TASK_COMPLEX_LEAVE_ANY_CAR:
        return new CTaskComplexLeaveAnyCar{ 0, false, true };
    case TASK_SIMPLE_STAND_STILL: {
        SetDefaultTaskWanderDir(ped);
        return new CTaskSimpleStandStill{ 0, false, false, 8.0f };
    }
    case TASK_COMPLEX_SEQUENCE: {
        if (m_moveState != PEDMOVE_RUN) {
            return nullptr;
        }
        const auto seq = new CTaskComplexSequence{};
        seq->AddTask(new CTaskSimpleRunAnim{ ANIM_GROUP_DEFAULT, ANIM_ID_FLEE_LKAROUND_01, 4.0f, false });
        seq->AddTask(new CTaskSimpleTired{ 2000u });
        return seq;
    }
    case TASK_COMPLEX_WANDER: {
        return new CTaskComplexWanderFlee{ m_moveState, m_fleeDir };
    }
    default:
        return nullptr;
    }
}

// 0x65C3C0
void CTaskComplexSmartFleePoint::SetFleePosition(CVector const& fleePos, float safeDist, bool doScream) {
    if (m_fleePoint == fleePos && m_safeDist == safeDist) {
        m_doScream = doScream;
        return;
    }
    m_fleePoint = fleePos;
    m_safeDist = safeDist;
    m_hasFleePointChanged = true;
    m_doScream = doScream;
}

// 0x65BDC0
bool CTaskComplexSmartFleePoint::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
    if (priority == ABORT_PRIORITY_LEISURE) {
        m_fleeTimeMs = -1;
        m_timer.m_nStartTime = CTimer::GetTimeInMS();
        m_timer.m_nInterval = -1;
        m_timer.m_bStarted = true;
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
    case TASK_COMPLEX_LEAVE_ANY_CAR: {
        m_fleeDir = ComputeFleeDir(ped);
        return CreateSubTask(TASK_COMPLEX_WANDER, ped);
    }
    default:
        return nullptr;
    }
}

// 0x65C140
CTask* CTaskComplexSmartFleePoint::CreateFirstSubTask(CPed* ped) {
    m_initalPos = ped->GetPosition();
    if (ped->IsInVehicle()) {
        return CreateSubTask(TASK_COMPLEX_LEAVE_ANY_CAR, ped);
    }
    m_fleeDir = ComputeFleeDir(ped);
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
    static_cast<CTaskComplexWander*>(m_pSubTask)->m_nMoveState = m_moveState;
    if (!m_hasFleePointChanged) {
        if (m_timer.IsOutOfTime()) {
            return CreateSubTask(TASK_SIMPLE_STAND_STILL, ped);
        }
        const CVector& pedPos = ped->GetPosition();
        const float safeDistSq = sq(m_safeDist);
        if (safeDistSq < (m_fleePoint - pedPos).SquaredMagnitude()) {
            if (safeDistSq < (m_initalPos - pedPos).SquaredMagnitude()) {
                return CreateSubTask(TASK_SIMPLE_STAND_STILL, ped);
            }
        }
    } else {
        m_hasFleePointChanged = false;
        m_timer.m_nStartTime = CTimer::GetTimeInMS();
        m_timer.m_nInterval = m_fleeTimeMs;
        m_timer.m_bStarted = true;
        m_fleeDir = ComputeFleeDir(ped);
        if (m_fleeDir != static_cast<CTaskComplexWander*>(m_pSubTask)->m_nDir) {
            static_cast<CTaskComplexWander*>(m_pSubTask)->m_nDir = m_fleeDir;
            static_cast<CTaskComplexWander*>(m_pSubTask)->m_bNewDir = true;
        }
    }
    if (m_moveState == PEDMOVE_RUN) {
        if (!g_ikChainMan.IsLooking(ped)) {
            if (CGeneral::GetRandomNumberInRange(0, 100) <= 5) {
                g_ikChainMan.LookAt("TaskSmartFleePoint", ped, nullptr, 2000, BONE_UNKNOWN, &m_fleePoint, false, 0.25f, 500, 3, false);
            }
        }
    }
    return m_pSubTask;
}
