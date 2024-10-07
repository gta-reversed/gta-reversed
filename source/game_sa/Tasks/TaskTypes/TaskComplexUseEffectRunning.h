#pragma once

#include "TaskComplexUseEffect.h"

class NOTSA_EXPORT_VTABLE CTaskComplexUseEffectRunning final : public CTaskComplexUseEffect {
public:
    static constexpr auto Type = eTaskType::TASK_COMPLEX_USE_EFFECT_RUNNING;

    static void InjectHooks();

    CTaskComplexUseEffectRunning(C2dEffectBase* fx, CEntity* e);
    CTaskComplexUseEffectRunning(const CTaskComplexUseEffectRunning&);
    ~CTaskComplexUseEffectRunning() override = default;

    CTask*    Clone() const override { return new CTaskComplexUseEffectRunning{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
};
