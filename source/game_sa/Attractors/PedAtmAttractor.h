#pragma once

#include "PedAttractor.h"

class NOTSA_EXPORT_VTABLE CPedATMAttractor final : public CPedAttractor {
public:
    static constexpr auto Type = PED_ATTRACTOR_ATM;

    // 0x5EE470
    CPedATMAttractor(C2dEffectPedAttractor* effect, CEntity* entity, eMoveState moveState) :
        CPedAttractor(
            effect,
            entity,
            moveState,
            5u,
            1.0f,
            30000.0f,
            3000.0f,
            0.2f,
            0.15f,
            0.1f,
            0.1f
        )
    {
    }

    ePedAttractorType GetType() const override { return Type; }; // 0x5EE4D0
};
