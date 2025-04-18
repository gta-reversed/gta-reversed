#include "StdInc.h"
#include "EventLeaderExitedCarAsDriver.h"


void CEventLeaderExitedCarAsDriver::InjectHooks()
{
    RH_ScopedVirtualClass(CEventLeaderExitedCarAsDriver, 0x85B898, 17);
    RH_ScopedCategory("Events");

    RH_ScopedInstall(Constructor, 0x4B8300);
    RH_ScopedVMTInstall(AffectsPedGroup, 0x4B0F80);
}

// 0x4B8300
CEventLeaderExitedCarAsDriver* CEventLeaderExitedCarAsDriver::Constructor()
{
    this->CEventLeaderExitedCarAsDriver::CEventLeaderExitedCarAsDriver();
    return this;
}

// 0x4B0F80
bool CEventLeaderExitedCarAsDriver::AffectsPedGroup(CPedGroup* pg) {
    const auto leader = pg->GetMembership().GetLeader();
    for (auto& m : pg->GetMembership().GetFollowers()) {
        if (m.m_pVehicle && m.bInVehicle && m.m_pVehicle == leader->m_pVehicle) {
            return true;
        }
        if (m.GetTaskManager().HasAnyOf<TASK_COMPLEX_ENTER_CAR_AS_PASSENGER, TASK_COMPLEX_ENTER_CAR_AS_PASSENGER_WAIT>(false)) {
            return true;
        }
    }
    return false;
}