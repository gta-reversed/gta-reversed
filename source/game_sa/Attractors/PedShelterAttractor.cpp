#include "StdInc.h"

#include "PedShelterAttractor.h"
#include "Tasks/TaskTypes/TaskComplexGoToAttractor.h"

// 0x5EF420
CVector CPedShelterAttractor::GetDisplacement(int32 pedId) {
    // Grow the shared pool to 5 slots on first use, each new slot placed
    // at least 1.0 unit away (squared) from the existing ones.
    if (ms_displacements.empty()) {
        for (int32 i = 0; i < 5; i++) {
            CVector candidate{};
            bool    tooClose;
            do {
                tooClose = false;
                // Random angle + squared-random radius (matches the binary's
                // `rand * TWO_PI` / `rand + rand` polar sampling).
                const float angle  = (float)rand() * (1.0f / (float)RAND_MAX) * TWO_PI;
                const float radius = (float)rand() * (1.0f / (float)RAND_MAX) + (float)rand() * (1.0f / (float)RAND_MAX);
                candidate.x = std::cos(angle) * radius;
                candidate.y = std::sin(angle) * radius;
                candidate.z = 0.0f;
                for (const auto& existing : ms_displacements) {
                    if ((existing - candidate).SquaredMagnitude() < 1.0f) {
                        tooClose = true;
                        break;
                    }
                }
                if (!tooClose) {
                    ms_displacements.emplace_back(candidate);
                }
            } while (tooClose);
        }
    }
    return ms_displacements[pedId];
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
    const auto it = rng::find(m_AttractPeds, ped);
    if (it == m_AttractPeds.end()) {
        return false;
    }
    m_PedTaskPairs.erase(rng::find(m_PedTaskPairs, ped, &CPedTaskPair::Ped));
    m_AttractPeds.erase(it);
    for (auto* const attractedPed : m_AttractPeds) {
        const auto slot = (int32)rng::distance(m_AttractPeds.begin(), rng::find(m_AttractPeds, attractedPed));
        SetTaskForPed(attractedPed, new CTaskComplexGoToAttractor(
            this,
            CPedAttractor::ComputeAttractPos(slot),
            CPedAttractor::ComputeAttractHeading(slot),
            m_AchieveQueueTime,
            slot,
            PEDMOVE_WALK
        ));
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
