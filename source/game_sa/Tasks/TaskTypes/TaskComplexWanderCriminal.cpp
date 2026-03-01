#include "StdInc.h"

#include "TaskComplexWanderCriminal.h"

// 0x48E610
CTaskComplexWanderCriminal::CTaskComplexWanderCriminal(eMoveState MoveState, uint8 Dir, bool bWanderSensibly) : CTaskComplexWander(MoveState, Dir, bWanderSensibly) { }

// 0x670350
void CTaskComplexWanderCriminal::ScanForStuff(CPed* ped) {
    plugin::CallMethod<0x670350, CTaskComplexWanderCriminal*, CPed*>(this, ped);
    return;

    // todo: weird
    if (m_TaskTimer.m_bStarted
        || (
               m_TaskTimer.m_nStartTime = CTimer::GetTimeInMS(),
               m_TaskTimer.m_nInterval = 50,
               m_TaskTimer.m_bStarted = true,
               m_nMinNextScanTime = CTimer::GetTimeInMS() + (uint32)CGeneral::GetRandomNumberInRange(0.0f, 30'000.0f),
               m_TaskTimer.m_bStarted
           )
    ) {
        if (m_TaskTimer.m_bStopped) {
            m_TaskTimer.m_nStartTime = CTimer::GetTimeInMS();
            m_TaskTimer.m_bStopped = false;
        }

        if (CTimer::GetTimeInMS() >= m_TaskTimer.m_nStartTime + m_TaskTimer.m_nInterval) {
            m_TaskTimer.m_nStartTime = CTimer::GetTimeInMS();
            m_TaskTimer.m_nInterval = 50;
            m_TaskTimer.m_bStarted = true;
            if (CTimer::GetTimeInMS() >= m_nMinNextScanTime) {
                LookForCarsToSteal(ped);
            }
        }
    }
}

// 0x66B4F0
void CTaskComplexWanderCriminal::LookForCarsToSteal(CPed* ped) {
    plugin::CallMethod<0x66B4F0, CTaskComplexWanderCriminal*, CPed*>(this, ped);
}
