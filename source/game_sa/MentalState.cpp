#include "StdInc.h"

#include "MentalState.h"

#include "Events/EventHealthReallyLow.h"
#include "Events/EventHealthLow.h"
#include "Events/EventHighAngerAtPlayer.h"
#include "Events/EventLowAngerAtPlayer.h"

// 0x421050
void CMentalState::IncrementAngerAtPlayer(uint8 anger) {
    if (m_AngerTimer.IsOutOfTime()) {
        m_AngerTimer.Start(3'000);
        m_AngerAtPlayer += anger;
    }
}

// 0x6008A0
void CMentalState::Process(const CPed& ped) {
    auto* const eg = &ped.GetIntelligence()->GetEventGroup();

    m_PedHealth = (uint8)ped.m_fHealth;
    if (ped.bInVehicle) {
        if (ped.m_pVehicle) {
            m_VehHealth = (uint8)ped.m_pVehicle->m_fHealth;
        }
    }

    if (!ped.bInVehicle) {
        if (m_PrevPedHealth < 50) {
            if (m_PrevPedHealth >= 10 && m_PedHealth < 10) {
                eg->Add(CEventHealthReallyLow{});
            }
        } else if (m_PedHealth < 10) {
            eg->Add(CEventHealthReallyLow{});
        } else if (m_PedHealth <= 50) {
            eg->Add(CEventHealthLow{});
        }
    }

    if (m_LastAngerAtPlayer > 3) {
        if (m_LastAngerAtPlayer <= 6 && m_AngerAtPlayer > 6) {
            eg->Add(CEventHighAngerAtPlayer{});
        }
    } else if (m_AngerAtPlayer > 6) {
        eg->Add(CEventHighAngerAtPlayer{});
    } else if (m_AngerAtPlayer > 3) {
        eg->Add(CEventLowAngerAtPlayer{});
    }

    m_LastAngerAtPlayer = m_AngerAtPlayer;
    m_PrevPedHealth     = m_PedHealth;
    m_PrevVehHealth     = m_VehHealth;
}


void CMentalState::InjectHooks() {
    RH_ScopedClass(CMentalState);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(IncrementAngerAtPlayer, 0x421050, { .Reversed = false });
    RH_ScopedInstall(Process, 0x6008A0, { .Reversed = false });
}
