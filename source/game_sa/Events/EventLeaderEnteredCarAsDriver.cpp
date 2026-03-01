#include "StdInc.h"
#include "EventLeaderEnteredCarAsDriver.h"


void CEventLeaderEnteredCarAsDriver::InjectHooks()
{
    RH_ScopedVirtualClass(CEventLeaderEnteredCarAsDriver, 0x85A198, 17);
    RH_ScopedCategory("Events");

    RH_ScopedInstall(Constructor, 0x48E1C0);
    RH_ScopedVMTInstall(AffectsPedGroup, 0x4B0EF0);
}

// 0x48E1C0
CEventLeaderEnteredCarAsDriver::CEventLeaderEnteredCarAsDriver(CVehicle* vehicle)
{
    m_vehicle = vehicle;
    CEntity::SafeRegisterRef(m_vehicle);
}

CEventLeaderEnteredCarAsDriver::~CEventLeaderEnteredCarAsDriver()
{
    CEntity::SafeCleanUpRef(m_vehicle);
}

// 0x48E1C0
CEventLeaderEnteredCarAsDriver* CEventLeaderEnteredCarAsDriver::Constructor(CVehicle* vehicle)
{
    this->CEventLeaderEnteredCarAsDriver::CEventLeaderEnteredCarAsDriver(vehicle);
    return this;
}

// 0x4B0EF0
bool CEventLeaderEnteredCarAsDriver::AffectsPedGroup(CPedGroup* pedGroup)
{
    if (m_vehicle && pedGroup->m_bMembersEnterLeadersVehicle) {
        for (int32 i = 0; i < TOTAL_PED_GROUP_FOLLOWERS; i++) {
            CPed* member = pedGroup->GetMembership().GetMember(i);
            if (!member)
                continue;

            if (!member->bInVehicle
                || member->m_pVehicle != m_vehicle
                || member->GetTaskManager().Find<TASK_COMPLEX_LEAVE_CAR>())
            {
                return true;
            }
        }
    }
    return false;
}