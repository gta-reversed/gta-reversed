#include "StdInc.h"
#include "EventLeaderQuitEnteringCarAsDriver.h"


void CEventLeaderQuitEnteringCarAsDriver::InjectHooks()
{
    RH_ScopedVirtualClass(CEventLeaderQuitEnteringCarAsDriver, 0x86E6B0, 16);
    RH_ScopedCategory("Events");

    RH_ScopedInstall(Constructor, 0x63A110);
    RH_ScopedVMTInstall(AffectsPedGroup, 0x4B1010);
}

// 0x63A110
CEventLeaderQuitEnteringCarAsDriver* CEventLeaderQuitEnteringCarAsDriver::Constructor()
{
    this->CEventLeaderQuitEnteringCarAsDriver::CEventLeaderQuitEnteringCarAsDriver();
    return this;
}

// 0x4B1010
bool CEventLeaderQuitEnteringCarAsDriver::AffectsPedGroup(CPedGroup* pedGroup) {
    const auto oe = pedGroup->GetIntelligence().GetOldEvent();
    return oe && oe->GetEvent().GetEventType() == EVENT_LEADER_ENTERED_CAR_AS_DRIVER;
}