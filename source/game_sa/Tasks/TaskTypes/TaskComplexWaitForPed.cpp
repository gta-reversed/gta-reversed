#include "StdInc.h"
#include "TaskSimpleStandStill.h"
#include "TaskComplexWaitForPed.h"
#include "TaskComplexWalkAlongsidePed.h"
#include "Events/EventDeadPed.h"

void CTaskComplexWaitForPed::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexWaitForPed, 0x8706f8, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedConstructorInstall(0x683340, "1", {}, CPed*, float, int32, bool);
    RH_ScopedDestructorInstall(0x6833D0);

    RH_ScopedVMTInstall(Clone, 0x683950);
    RH_ScopedVMTInstall(GetTaskType, 0x6833C0);
    RH_ScopedVMTInstall(MakeAbortable, 0x683430);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x683440);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x683450);
    RH_ScopedVMTInstall(ControlSubTask, 0x6834E0);
}
// 0x683340
CTaskComplexWaitForPed::CTaskComplexWaitForPed(CPed* waitForPed, float targetDist, int32 timeout, bool lookAtPed) :
    m_WaitForPed(waitForPed),
    m_TargetDist(targetDist),
    m_Timeout(timeout),
    m_LookAtPed(lookAtPed) {
}

// 0x683950
CTaskComplexWaitForPed::CTaskComplexWaitForPed(const CTaskComplexWaitForPed& o) :
    CTaskComplexWaitForPed{
        o.m_WaitForPed,
        o.m_TargetDist,
        o.m_Timeout,
        o.m_LookAtPed
    }
{
}

// 0x683450
CTask* CTaskComplexWaitForPed::CreateFirstSubTask(CPed* ped) {
    if (!m_WaitForPed) {
        return nullptr;
    }
    // NOTE: Not `m_Timer.Start()`, as that does nothing for negative timeouts (the original always starts it)
    m_Timer.m_nStartTime = CTimer::GetTimeInMS();
    m_Timer.m_nInterval  = m_Timeout;
    m_Timer.m_bStarted   = true;
    m_FrameCounter       = 0;
    return new CTaskSimpleStandStill{ 999'999 };
}

// 0x6834E0
CTask* CTaskComplexWaitForPed::ControlSubTask(CPed* ped) {
    if (!m_WaitForPed) {
        return nullptr;
    }

    if (!m_WaitForPed->IsAlive()) { // Stop waiting, and go see what happened
        CEventDeadPed e{ m_WaitForPed, false, CTimer::GetTimeInMS() };
        e.m_TaskId = TASK_COMPLEX_INVESTIGATE_DEAD_PED;
        ped->GetEventGroup().Add(&e, false);
        return nullptr;
    }

    if (m_Timer.IsStarted() && m_Timer.IsOutOfTime()) {
        return nullptr;
    }

    // If the ped we're waiting for walks alongside someone, use the offset position, otherwise our own position
    // NOTE/BUG: Seems like code assumes `ped == m_TargetPed->m_TargetPed`, should assert it
    const auto* const tWalkAlongsidePed = static_cast<CTaskComplexWalkAlongsidePed*>(m_WaitForPed->GetIntelligence()->FindTaskByType(TASK_COMPLEX_WALK_ALONGSIDE_PED));
    const auto        waitForPoint      = tWalkAlongsidePed
        ? ped->GetMatrix().TransformPoint(tWalkAlongsidePed->GetOffset())
        : ped->GetPosition();
    if ((waitForPoint - m_WaitForPed->GetPosition()).SquaredMagnitude() < sq(m_TargetDist)) {
        return nullptr; // They're here
    }
    
    if (m_LookAtPed && m_FrameCounter == 0) {
        ped->m_fAimingRotation = (m_WaitForPed->GetPosition() - ped->GetPosition()).Heading(); // NOTE: Dropped `LimitRadianAngle`
    }
    
    if (++m_FrameCounter > 10) {
        m_FrameCounter = 0;
    }

    return m_pSubTask;
}
