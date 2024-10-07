#pragma once

#include "TaskComplexUseEffect.h"

class NOTSA_EXPORT_VTABLE CTaskComplexUseEffectSprinting final : public CTaskComplexUseEffect {
public:
    static constexpr auto Type = eTaskType::TASK_COMPLEX_USE_EFFECT_SPRINTING;

    static void InjectHooks();

    CTaskComplexUseEffectSprinting(C2dEffectBase* fx, CEntity* e);
    CTaskComplexUseEffectSprinting(const CTaskComplexUseEffectSprinting&);
    ~CTaskComplexUseEffectSprinting() override = default;

    CTask*    Clone() const override { return new CTaskComplexUseEffectSprinting{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
};
