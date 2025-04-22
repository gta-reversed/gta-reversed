#pragma once

#include "PedAttractor.h"

class NOTSA_EXPORT_VTABLE CPedShelterAttractor final : public CPedAttractor {
public:
    // SArray& ms_displacements;

public:
    static constexpr auto Type = PED_ATTRACTOR_SHELTER;

    // 0x5EE7C0
    CPedShelterAttractor(C2dEffectPedAttractor* effect, CEntity* entity, eMoveState moveState) :
        CPedAttractor(
            effect,
            entity,
            moveState,
            5,
            1.0f,
            30000.0f,
            3000.0f,
            0.5f,
            6.28f,
            0.1f,
            0.1f
        )
    {
    }

    // 0x5EE820
    ePedAttractorType GetType() const override { return Type; }

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
