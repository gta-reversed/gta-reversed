#pragma once

#include "PedAttractor.h"

class NOTSA_EXPORT_VTABLE CPedSeatAttractor final : public CPedAttractor {
public:
    static constexpr auto Type = PED_ATTRACTOR_SEAT;

    // 0x5EE540
    CPedSeatAttractor(C2dEffectPedAttractor* effect, CEntity* entity, eMoveState moveState) :
        CPedAttractor(
            effect,
            entity,
            moveState,
            1,
            1.0f,
            30000.0f,
            3000.0f,
            0.125f,
            0.1f,
            0.1f,
            0.1f
        )
    {
    }

    // 0x5EE5A0
    ePedAttractorType GetType() const override { return Type; }
};
