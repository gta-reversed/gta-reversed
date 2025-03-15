/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#include "StdInc.h"

void CTaskTimer::InjectHooks() {
    RH_ScopedClass(CTaskTimer);
    RH_ScopedCategory("Tasks");

    RH_ScopedInstall(IsOutOfTime, 0x420E30);
}

// 0x420E30
bool CTaskTimer::IsOutOfTime() {
    if (!m_bStarted) {
        return false;
    }

    if (m_bStopped) {
        m_nStartTime = CTimer::GetTimeInMS();
        m_bStopped = false;
    }

    return CTimer::GetTimeInMS() >= (m_nStartTime + m_nInterval);
}
