#include "StdInc.h"

#include "PedShelterAttractor.h"

// SArray& CPedShelterAttractor::ms_displacements = *(SArray*)0xC0986C;

// 0x5EF420
CVector CPedShelterAttractor::GetDisplacement(int32 pedId) {
    return plugin::CallAndReturn<CVector, 0x5EF420, int32>(pedId);
}

// 0x5EFC40


void CPedShelterAttractor::ComputeAttractPos(int32 pedId, CVector& outPos) {
    if (m_pEffect) {
        CVector displacement = GetDisplacement(pedId);
        outPos = displacement + m_vecAttractorPosn;
    }
}

// 0x5E9690


void CPedShelterAttractor::ComputeAttractHeading(int32 bQueue, float& heading) {
    heading = CGeneral::GetRandomNumberInRange(0.0f, TWO_PI);
}

// 0x5EF570

// 0x0
void CPedShelterAttractor::BroadcastDeparture(CPed* ped) {
    plugin::CallMethod<0x5EF570, CPedShelterAttractor*, CPed*>(this, ped);
}

void CPedShelterAttractor::InjectHooks() {
    RH_ScopedVirtualClass(CPedShelterAttractor, 0x86C5B4, 6);
    RH_ScopedCategory("Attractors");

    RH_ScopedInstall(GetDisplacement, 0x5EF420, { .reversed = false });
    RH_ScopedVMTInstall(ComputeAttractPos, 0x5EFC40);
    RH_ScopedVMTInstall(ComputeAttractHeading, 0x5E9690);
    RH_ScopedVMTInstall(BroadcastDeparture, 0x5EF570, { .reversed = false });
}