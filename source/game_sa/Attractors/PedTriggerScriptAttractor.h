#pragma once

#include "PedAttractor.h"

class NOTSA_EXPORT_VTABLE CPedTriggerScriptAttractor final : public CPedAttractor {
public:
    static constexpr auto Type = PED_ATTRACTOR_TRIGGER_SCRIPT;

    // 0x5EE890
    CPedTriggerScriptAttractor(C2dEffectPedAttractor* effect, CEntity* entity, eMoveState moveState) :
        CPedAttractor(
            effect,
            entity,
            moveState,
            1,
            1.f,
            30000.0f,
            3000.0f,
            0.125f,
            0.1f,
            0.1f,
            0.1f
        )
    {
    }

    // 0x5EE8F0
    ePedAttractorType GetType() const override { return Type; }
};
