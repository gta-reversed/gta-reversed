#include "StdInc.h"
#include "MentalState.h"
#include "Events/EventHealthLow.h"
#include "Events/EventHealthReallyLow.h"
#include "Events/EventLowAngerAtPlayer.h"
#include "Events/EventHighAngerAtPlayer.h"

void CMentalState::InjectHooks() {
    RH_ScopedClass(CMentalState);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Process, 0x6008A0);
}

// 0x6008A0
void CMentalState::Process(CPed& ped) {
    m_pedHealth = (uint8)(int32)ped.m_fHealth;
    if (ped.bInVehicle && ped.m_pVehicle) {
        m_vehicleHealth = (uint8)(int32)ped.m_pVehicle->m_fHealth;
    }

    if (!ped.bInVehicle) {
        if (m_oldPedHealth >= 50) {
            if (m_pedHealth < 10) {
                ped.GetEventGroup().Add(CEventHealthReallyLow{});
            } else if (m_pedHealth < 50) {
                ped.GetEventGroup().Add(CEventHealthLow{});
            }
        } else if (m_oldPedHealth >= 10 && m_pedHealth < 10) {
            ped.GetEventGroup().Add(CEventHealthReallyLow{});
        }
    } else {
        // NOTE: Vehicle health is stored in a byte, but compared against 300 and 600,
        //       so `m_oldVehicleHealth < 300` is always true and no event is ever generated here.
        const auto vehHealth = (int32)m_vehicleHealth, oldVehHealth = (int32)m_oldVehicleHealth;
        if (oldVehHealth >= 600) {
            if (vehHealth < 300) {
                ped.GetEventGroup().Add(CEventHealthReallyLow{});
            } else if (vehHealth < 600) {
                ped.GetEventGroup().Add(CEventHealthLow{});
            }
        } else if (oldVehHealth >= 300 && vehHealth < 300) {
            ped.GetEventGroup().Add(CEventHealthReallyLow{});
        }
    }

    if (m_LastAngerAtPlayer <= 3) {
        if (m_AngerAtPlayer >= 7) {
            ped.GetEventGroup().Add(CEventHighAngerAtPlayer{});
        } else if (m_AngerAtPlayer >= 4) {
            ped.GetEventGroup().Add(CEventLowAngerAtPlayer{});
        }
    } else if (m_LastAngerAtPlayer <= 6 && m_AngerAtPlayer >= 7) {
        ped.GetEventGroup().Add(CEventHighAngerAtPlayer{});
    }

    m_LastAngerAtPlayer = m_AngerAtPlayer;
    m_oldPedHealth      = m_pedHealth;
    m_oldVehicleHealth  = m_vehicleHealth;
}
