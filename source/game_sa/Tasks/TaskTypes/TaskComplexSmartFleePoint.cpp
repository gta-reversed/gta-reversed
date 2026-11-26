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
void CTaskComplexSmartFleePoint::SetDefaultTaskWanderDir(CPed* ped) {
    if (auto* const tComplexWander = notsa::dyn_cast<CTaskComplexWander>(ped->GetTaskManager().GetTaskPrimary(TASK_PRIMARY_DEFAULT))) {
        tComplexWander->SetPathNodesFrom(*notsa::cast<CTaskComplexWander>(m_pSubTask));
    }
}

// 0x65BE40
uint32 CTaskComplexSmartFleePoint::ComputeFleeDir(CPed* ped) {
    return (ped->GetPosition2D() - CVector2D{ m_fleePoint }).NodeHeading(); // Away from the flee point
}

// 0x65BE80
CTask* CTaskComplexSmartFleePoint::CreateSubTask(eTaskType taskType, CPed* ped) {
    switch (taskType) {
    case TASK_COMPLEX_LEAVE_ANY_CAR: // 0x65bffd
        return new CTaskComplexLeaveAnyCar{ 0, false, true };
    case TASK_COMPLEX_WANDER: // 0x65c099
        return new CTaskComplexWanderFlee{ m_moveState, m_fleeDir };
    case TASK_SIMPLE_STAND_STILL: { // 0x65bfae
        SetDefaultTaskWanderDir(ped);
        return new CTaskSimpleStandStill{ 0, false, false, 8.f };
    }
    case TASK_COMPLEX_SEQUENCE: { // 0x65bec6
        if (m_moveState != PEDMOVE_RUN) {
            return nullptr;
        }
        return new CTaskComplexSequence{
            new CTaskSimpleRunAnim{ ANIM_GROUP_DEFAULT, ANIM_ID_FLEE_LKAROUND_01 },
            new CTaskSimpleTired{ 2'000 }
        };
    }
    case TASK_FINISHED: // 0x65c051
        return nullptr;
    default:
        NOTSA_UNREACHABLE_CASE(taskType);
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
        m_timer.SetOutOfTime();
    }
    return m_pSubTask->MakeAbortable(ped, priority, event);
}

// 0x65C0C0
CTask* CTaskComplexSmartFleePoint::CreateNextSubTask(CPed* ped) {
    switch (const auto tt = m_pSubTask->GetTaskType()) {
    case TASK_SIMPLE_STAND_STILL:
        return CreateSubTask(TASK_COMPLEX_SEQUENCE, ped);
    case TASK_COMPLEX_SEQUENCE:
        return CreateSubTask(TASK_FINISHED, ped);
    case TASK_COMPLEX_LEAVE_ANY_CAR: {
        m_fleeDir = (uint8)ComputeFleeDir(ped);
        return CreateSubTask(TASK_COMPLEX_WANDER, ped);
    }
    case TASK_COMPLEX_WANDER: // 0x65c12e - the flee wander finished
        return nullptr;
    default:
        NOTSA_UNREACHABLE_CASE(tt);
    }
}

// 0x65C140
CTask* CTaskComplexSmartFleePoint::CreateFirstSubTask(CPed* ped) {
    m_initalPos = ped->GetPosition();
    if (ped->IsInVehicle()) {
        return CreateSubTask(TASK_COMPLEX_LEAVE_ANY_CAR, ped);
    }
    m_fleeDir = (uint8)ComputeFleeDir(ped);
    return CreateSubTask(TASK_COMPLEX_WANDER, ped);
}

// 0x65C1E0
CTask* CTaskComplexSmartFleePoint::ControlSubTask(CPed* ped) {
    if (auto* const tSubWander = notsa::dyn_cast<CTaskComplexWander>(m_pSubTask)) {
        if (m_doScream) {
            ped->Say(CTX_GLOBAL_PAIN_PANIC, 0, 0.1f);
        }
        
        tSubWander->m_nMoveState = m_moveState;

        if (m_moveState == PEDMOVE_RUN && !g_ikChainMan.IsLooking(ped) && CGeneral::GetRandomNumberInRange(0, 100) <= 5) { // 0x65c26f - moved up here for convinience
            g_ikChainMan.LookAt("TaskSmartFleePoint", ped, nullptr, 2'000, BONE_UNKNOWN, &m_fleePoint, false, 0.25f, 500, 3, false);
        }
        
        if (m_hasFleePointChanged) { // 0x65c238
            m_hasFleePointChanged = false;
            if (m_timer.m_bStarted) { // Restart with raw fields (`m_fleeTimeMs` may be -1, see `MakeAbortable`)
                m_timer.m_nStartTime = CTimer::GetTimeInMS();
                m_timer.m_nInterval  = m_fleeTimeMs;
                m_timer.m_bStarted   = true;
            }
            if (const auto dir = (uint8)ComputeFleeDir(ped); std::exchange(m_fleeDir, dir) != dir) {
                tSubWander->SetDirection(dir);
            }
        } else if (m_timer.IsOutOfTime()) {
            return CreateSubTask(TASK_SIMPLE_STAND_STILL, ped); // 0x65c2e3
        } else {
            const auto DistSqToPed = [from = ped->GetPosition()](CVector pt) {
                return CVector::DistSqr(pt, from);
            };
            if (DistSqToPed(m_fleePoint) > sq(m_safeDist) && DistSqToPed(m_initalPos) > sq(m_safeDist)) {
                return CreateSubTask(TASK_SIMPLE_STAND_STILL, ped);
            }
        }  
    } else {
        if (g_ikChainMan.IsLooking(ped)) { // 0x65c39a
            g_ikChainMan.AbortLookAt(ped, 250);
        }
    }
    return m_pSubTask;
}
