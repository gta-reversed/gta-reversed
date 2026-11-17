#include "StdInc.h"

#include "TaskComplexWanderStandard.h"

// 0x48E4F0
CTaskComplexWanderStandard::CTaskComplexWanderStandard(eMoveState MoveState, uint8 Dir, bool bWanderSensibly) :
    CTaskComplexWander(MoveState, Dir, bWanderSensibly),
    m_nMinNextScanTime{ 0 } {
}

// 0x672600
void CTaskComplexWanderStandard::ScanForStuff(CPed* ped) {
    if (!m_TaskTimer.m_bStarted) {
        m_TaskTimer.m_nStartTime = CTimer::GetTimeInMS();
        m_TaskTimer.m_nInterval  = 50;
        m_TaskTimer.m_bStarted   = true;
    }

    if (CTimer::GetTimeInMS() < m_nMinNextScanTime) {
        return;
    }

    if (m_TaskTimer.m_bStarted) { // V547 Expression 'm_TaskTimer.m_bStarted' is always true.
        if (m_TaskTimer.m_bStopped) {
            m_TaskTimer.m_nStartTime = CTimer::GetTimeInMS();
            m_TaskTimer.m_bStopped   = false;
        }

        if (CTimer::GetTimeInMS() >= m_TaskTimer.m_nStartTime + m_TaskTimer.m_nInterval) {
            m_TaskTimer.m_nInterval  = 50;
            m_TaskTimer.m_nStartTime = CTimer::GetTimeInMS();
            m_TaskTimer.m_bStarted   = true;
            if (!LookForGangMembers(ped) && !LookForChatPartners(ped)) {
                CTaskComplexWanderStandard::LookForSexyCars(ped);
            }
        }
    }
}

// 0x66AED0
bool CTaskComplexWanderStandard::WillChat(const CPed& first, const CPed& second) {
    return plugin::CallMethodAndReturn<bool, 0x66AED0, CTaskComplexWanderStandard*, const CPed&, const CPed&>(this, first, second);
}

// 0x66AF60
void CTaskComplexWanderStandard::SetNextMinScanTime(CPed* ped) {
    plugin::CallMethod<0x66AF60, CTaskComplexWanderStandard*, CPed*>(this, ped);
}

// 0x66AFD0
bool CTaskComplexWanderStandard::LookForSexyCars(CPed* ped) {
    return plugin::CallMethodAndReturn<bool, 0x66AFD0, CTaskComplexWanderStandard*, CPed*>(this, ped);
}

// 0x66FDA0
bool CTaskComplexWanderStandard::LookForChatPartners(CPed* ped) {
    return plugin::CallMethodAndReturn<bool, 0x66FDA0, CTaskComplexWanderStandard*, CPed*>(this, ped);
}

// 0x670100
bool CTaskComplexWanderStandard::LookForGangMembers(CPed* ped) {
    return plugin::CallMethodAndReturn<bool, 0x670100, CTaskComplexWanderStandard*, CPed*>(this, ped);
}

void CTaskComplexWanderStandard::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexWanderStandard, 0x85a200, 15);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedConstructorInstall(0x48E4F0, "1", {}, eMoveState, uint8, bool);
    RH_ScopedDestructorInstall(0x48E600);

    RH_ScopedInstall(WillChat, 0x66AED0, { .Reversed = false });
    RH_ScopedInstall(SetNextMinScanTime, 0x66AF60, { .Reversed = false });
    RH_ScopedInstall(LookForSexyCars, 0x66AFD0, { .Reversed = false });
    RH_ScopedInstall(LookForChatPartners, 0x66FDA0, { .Reversed = false });
    RH_ScopedInstall(LookForGangMembers, 0x670100, { .Reversed = false });
    RH_ScopedVMTInstall(Clone, 0x48E530);
    RH_ScopedVMTInstall(GetWanderType, 0x48E5D0);
    RH_ScopedVMTInstall(ScanForStuff, 0x672600);
}
