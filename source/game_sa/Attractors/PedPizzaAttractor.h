#pragma once

#include "PedAttractor.h"

class NOTSA_EXPORT_VTABLE CPedPizzaAttractor final : public CPedAttractor {
public:
    static constexpr auto Type = PED_ATTRACTOR_PIZZA;

    // 0x5EE6E0
    CPedPizzaAttractor(C2dEffectPedAttractor* effect, CEntity* entity, eMoveState moveState) :
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

    // 0x5EE740
    ePedAttractorType GetType() const override { return Type; }

    // 0x5EE750
    float GetHeadOfQueueWaitTime() const { return 2000.0f; }
};
