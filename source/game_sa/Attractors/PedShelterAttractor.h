#pragma once

#include "PedAttractor.h"

class NOTSA_EXPORT_VTABLE CPedShelterAttractor : public CPedAttractor {
public:
    // SArray& ms_displacements;

public:
    // 0x5EE7C0
    CPedShelterAttractor(C2dEffect* effect, CEntity* entity, int32 a4)
        : CPedAttractor(effect, entity, a4, 5, 1.0f, 30000.0f, 3000.0f, 0.5f, 6.28f, 0.1f, 0.1f, 0) {
    }

    // 0x5EE820
    ePedAttractorType GetType() const override { return PED_ATTRACTOR_SHELTER; }

    // 0x5E9680
    bool IsAtHeadOfQueue(CPed* ped) override { return true; }

    static CVector GetDisplacement(int32 pedId);
    void ComputeAttractPos(int32 pedId, CVector& outPos) override;
    void ComputeAttractHeading(int32 bQueue, float& heading) override;
    void BroadcastDeparture(CPed* ped) override;

private:
    friend void InjectHooksMain();
    static void InjectHooks();

};
