#pragma once

#include "PedAttractor.h"

class NOTSA_EXPORT_VTABLE CPedStopAttractor final : public CPedAttractor {
public:
    static constexpr auto Type = PED_ATTRACTOR_STOP;

    // 0x5EE610
    CPedStopAttractor(C2dEffectPedAttractor* effect, CEntity* entity, eMoveState moveState) :
        CPedAttractor(
            effect,
            entity,
            moveState,
            5,
            1.0f,
            30000.0f,
            3000.0f,
            0.2f,
            0.1f,
            0.1f,
            0.1f
        )
    {
    }

    // 0x5EE670
    ePedAttractorType GetType() const override { return Type; }
};
