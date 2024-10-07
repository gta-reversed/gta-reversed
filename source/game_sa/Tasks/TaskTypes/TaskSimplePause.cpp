#include "StdInc.h"

#include "TaskSimplePause.h"

void CTaskSimplePause::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimplePause, 0x85A278, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x48E750);
    RH_ScopedVMTInstall(ProcessPed, 0x48E830);
    RH_ScopedVMTInstall(Clone, 0x48E780);
    RH_ScopedVMTInstall(MakeAbortable, 0x48E810);
}

// 0x48E750
CTaskSimplePause::CTaskSimplePause(int32 pauseIntervalMs)  :
    m_PauseIntervalMs{pauseIntervalMs}
{
}

// For 0x48E780
CTaskSimplePause::CTaskSimplePause(const CTaskSimplePause& o) :
    CTaskSimplePause{o.m_PauseIntervalMs}
{
}

CTaskSimplePause* CTaskSimplePause::Constructor(int32 time) {
    this->CTaskSimplePause::CTaskSimplePause(time);
    return this;
}

// 0x48E810
bool CTaskSimplePause::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    m_Timer.SetAsOutOfTime();
    return true;
}

// 0x48E830
bool CTaskSimplePause::ProcessPed(CPed* ped) {
    if (!m_Timer.IsStarted() && !m_Timer.Start(m_PauseIntervalMs)) {
        return false;
    }
    m_Timer.Reset();
    return m_Timer.IsOutOfTime();
}
