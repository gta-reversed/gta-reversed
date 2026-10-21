#include "StdInc.h"

#include "PedShelterAttractor.h"

#include "Tasks/TaskTypes/TaskComplexGoToAttractor.h"

// 0x5EF420
const CVector& CPedShelterAttractor::GetDisplacement(int32 pedId) {
    if (ms_displacements.empty()) {
        for (auto i = 0; i < 5; i++) { // One spot per ped, see the constructor
            CVector displacement;
            do {
                const auto rads   = CGeneral::GetRandomNumberInRange(0.f, TWO_PI);
                const auto radius = CGeneral::GetRandomNumberInRange(0.f, 2.f);
                displacement      = CVector{ std::cos(rads) * radius, std::sin(rads) * radius, 0.f };
            } while (rng::any_of(ms_displacements, [&](const CVector& d) { return DistanceBetweenPointsSquared(d, displacement) < sq(1.f); }));
            ms_displacements.emplace_back(displacement);
        }
    }
    return ms_displacements.begin()[pedId];
}

// 0x5EFC40
void CPedShelterAttractor::ComputeAttractPos(int32 pedId, CVector& outPos) {
    if (m_Fx) {
        CVector displacement = GetDisplacement(pedId);
        outPos = displacement + m_Pos;
    }
}

// 0x5E9690
void CPedShelterAttractor::ComputeAttractHeading(int32 bQueue, float& heading) {
    heading = CGeneral::GetRandomNumberInRange(0.0f, TWO_PI);
}

// 0x5EF570
bool CPedShelterAttractor::BroadcastDeparture(CPed* ped) {
    const auto it = rng::find(m_ArrivedPeds, ped);
    if (it == m_ArrivedPeds.end()) {
        return false;
    }
    if (const auto pair = rng::find(m_PedTaskPairs, ped, &CPedTaskPair::Ped); pair != m_PedTaskPairs.end()) {
        m_PedTaskPairs.erase(pair);
    }
    m_ArrivedPeds.erase(it);
    for (auto* const attractedPed : m_AttractPeds) {
        const auto slot = (int32)m_ArrivedPeds.size();
        CVector pos;
        ComputeAttractPos(slot, pos);
        float heading;
        ComputeAttractHeading(slot, heading);
        SetTaskForPed(attractedPed, new CTaskComplexGoToAttractor{ this, pos, heading, m_AchieveQueueTime, slot, PEDMOVE_WALK });
    }
    return true;
}

void CPedShelterAttractor::InjectHooks() {
    RH_ScopedVirtualClass(CPedShelterAttractor, 0x86C5B4, 6);
    RH_ScopedCategory("Attractors");

    RH_ScopedInstall(GetDisplacement, 0x5EF420);
    RH_ScopedVMTInstall(ComputeAttractPos, 0x5EFC40);
    RH_ScopedVMTInstall(ComputeAttractHeading, 0x5E9690);
    RH_ScopedVMTInstall(BroadcastDeparture, 0x5EF570);
}
