#pragma once

#include "TaskComplexUseClosestFreeScriptedAttractor.h"

class NOTSA_EXPORT_VTABLE CTaskComplexUseClosestFreeScriptedAttractorSprint final : public CTaskComplexUseClosestFreeScriptedAttractor {
public:
    static constexpr auto Type = eTaskType::TASK_COMPLEX_USE_CLOSEST_FREE_SCRIPTED_ATTRACTOR_SPRINT;

    static void InjectHooks();

    CTaskComplexUseClosestFreeScriptedAttractorSprint();
    ~CTaskComplexUseClosestFreeScriptedAttractorSprint() override = default;

    CTask*    Clone()       const override { return new CTaskComplexUseClosestFreeScriptedAttractorSprint{ *this }; }
    eTaskType GetTaskType() const override { return Type; }

private: // Wrappers for hooks
    // 0x4B8850
    CTaskComplexUseClosestFreeScriptedAttractorSprint* Constructor() {
        this->CTaskComplexUseClosestFreeScriptedAttractorSprint::CTaskComplexUseClosestFreeScriptedAttractorSprint();
        return this;
    }

    // 0x4B8900
    CTaskComplexUseClosestFreeScriptedAttractorSprint* Destructor() {
        this->CTaskComplexUseClosestFreeScriptedAttractorSprint::~CTaskComplexUseClosestFreeScriptedAttractorSprint();
        return this;
    }
};
