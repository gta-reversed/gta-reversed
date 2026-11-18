#include "StdInc.h"

#include "EventDeath.h"

// 0x4ADE50, `time` should be int32
CEventDeath::CEventDeath(bool bDrowning, uint32 deathTimeInMs) :
    m_bDrowning{ bDrowning },
    m_deathTimeInMs{ deathTimeInMs } {
}

// 0x4ADDF0
CEventDeath::CEventDeath(bool bDrowning) :
    m_bDrowning{ bDrowning },
    m_deathTimeInMs{ CTimer::GetTimeInMS() } {
}

void CEventDeath::InjectHooks() {
    RH_ScopedVirtualClass(CEventDeath, 0x85adc0, 16);
    RH_ScopedCategory("Events");

    RH_ScopedConstructorInstall(0x4ADDF0, "1", {}, bool);
    RH_ScopedConstructorInstall(0x4ADE50, "2", {}, bool, int32);
    RH_ScopedDestructorInstall(0x4ADE90);

    RH_ScopedVMTInstall(GetEventType, 0x4ADE20, { .Reversed = false });
    RH_ScopedVMTInstall(GetEventPriority, 0x4ADE40, { .Reversed = false });
    RH_ScopedVMTInstall(GetLifeTime, 0x4ADE30, { .Reversed = false });
    RH_ScopedVMTInstall(Clone, 0x4B6E30, { .Reversed = false });
    RH_ScopedVMTInstall(AffectsPed, 0x4ADE80, { .Reversed = false });
}
