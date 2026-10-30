#include "StdInc.h"
#include "TaskComplexSmartFleePoint.h"
#include "Ragdoll/IKChainManager.h"
#include "TaskComplexWander.h"

void CTaskComplexSmartFleePoint::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexSmartFleePoint, 0x86f744, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x65BD20);
    RH_ScopedInstall(Destructor, 0x65BDB0);

    RH_ScopedInstall(SetDefaultTaskWanderDir, 0x65BE00, {.Reversed = false});
    RH_ScopedInstall(ComputeFleeDir, 0x65BE40, {.Reversed = false});
    RH_ScopedInstall(CreateSubTask, 0x65BE80, {.Reversed = false});
    RH_ScopedInstall(SetFleePosition, 0x65C3C0, {.Reversed = false});

    RH_ScopedVMTInstall(Clone, 0x65CED0, {.Reversed = false});
    RH_ScopedVMTInstall(GetTaskType, 0x65BDA0, {.Reversed = false});
    RH_ScopedVMTInstall(MakeAbortable, 0x65BDC0, {.Reversed = false});
    RH_ScopedVMTInstall(CreateNextSubTask, 0x65C0C0, {.Reversed = false});
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
int8 CTaskComplexSmartFleePoint::SetDefaultTaskWanderDir(CPed* ped) {
    return plugin::CallMethodAndReturn<int8, 0x65BE00, CTaskComplexSmartFleePoint*, CPed*>(this, ped);
}

// 0x65BE40
uint32 CTaskComplexSmartFleePoint::ComputeFleeDir(CPed* ped) {
    return plugin::CallMethodAndReturn<uint32, 0x65BE40, CTaskComplexSmartFleePoint*, CPed*>(this, ped);
}

// 0x65BE80
CTask* CTaskComplexSmartFleePoint::CreateSubTask(eTaskType taskType, CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x65BE80, CTaskComplexSmartFleePoint*, eTaskType, CPed*>(this, taskType, ped);
}

// 0x65C3C0
int8 CTaskComplexSmartFleePoint::SetFleePosition(CVector const& a2, float a3, bool a4) {
    return plugin::CallMethodAndReturn<int8, 0x65C3C0, CTaskComplexSmartFleePoint*, CVector const&, float, bool>(this, a2, a3, a4);
}

// 0x65BDC0
bool CTaskComplexSmartFleePoint::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
    return plugin::CallMethodAndReturn<bool, 0x65BDC0, CTaskComplexSmartFleePoint*, CPed*, eAbortPriority, CEvent const*>(this, ped, priority, event);
}

// 0x65C0C0
CTask* CTaskComplexSmartFleePoint::CreateNextSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x65C0C0, CTaskComplexSmartFleePoint*, CPed*>(this, ped);
}

// 0x65C140
CTask* CTaskComplexSmartFleePoint::CreateFirstSubTask(CPed* ped) {
    m_initalPos = ped->GetPosition();

    if (ped->IsInVehicle()) {
        return CreateSubTask(TASK_COMPLEX_LEAVE_ANY_CAR, ped);
    }

    m_fleeDir = (uint8)ComputeFleeDir(ped); // Original only keeps the low byte
    return CreateSubTask(TASK_COMPLEX_WANDER, ped);
}

// 0x65C1E0
CTask* CTaskComplexSmartFleePoint::ControlSubTask(CPed* ped) {
    const auto subTaskType = m_pSubTask->GetTaskType();
    if (subTaskType != TASK_COMPLEX_WANDER) {
        if (g_ikChainMan.IsLooking(ped)) {
            g_ikChainMan.AbortLookAt(ped, 250);
        }
        return m_pSubTask;
    }

    auto* nextSubTask = m_pSubTask;

    if (m_doScream) {
        ped->Say(CTX_GLOBAL_PAIN_PANIC, 0, 0.1f);
    }

    static_cast<CTaskComplexWander*>(m_pSubTask)->m_nMoveState = m_moveState;

    if (m_hasFleePointChanged) {
        m_hasFleePointChanged = false;
        if (m_timer.m_bStarted) {
            m_timer.m_nStartTime = CTimer::GetTimeInMS();
            m_timer.m_nInterval  = m_fleeTimeMs;
            m_timer.m_bStarted   = true;
        }
        if (const auto dir = (uint8)ComputeFleeDir(ped); m_fleeDir != dir) {
            m_fleeDir = dir;
            plugin::CallMethod<0x669D30>(m_pSubTask, uint32(dir)); // Original: ECX = wander subtask; callee pops the direction argument
        }
    } else {
        if (m_timer.m_bStarted && m_timer.IsOutOfTime()) {
            nextSubTask = CreateSubTask(TASK_SIMPLE_STAND_STILL, ped);
        } else {
            const auto& pedPos = ped->GetPosition();
            const auto  sqDist = m_safeDist * m_safeDist;
            const auto squaredDistance = [&pedPos](const CVector& point) {
                const auto delta = point - pedPos;
                // The original adds y/z first, which matters at the radius boundary.
                return delta.x * delta.x + (delta.y * delta.y + delta.z * delta.z);
            };
            if (sqDist < squaredDistance(m_fleePoint)
                && sqDist < squaredDistance(m_initalPos)) {
                nextSubTask = CreateSubTask(TASK_SIMPLE_STAND_STILL, ped);
            }
        }
    }

    if (m_moveState == PEDMOVE_RUN && !g_ikChainMan.IsLooking(ped)) {
        if (CGeneral::RandomBool(6.f)) {
            g_ikChainMan.LookAt("TaskSmartFleePoint", ped, nullptr, 2000, eBoneTag::BONE_UNKNOWN, &m_fleePoint, false, 0.25f, 500, 3, false);
        }
    }

    return nextSubTask;
}
