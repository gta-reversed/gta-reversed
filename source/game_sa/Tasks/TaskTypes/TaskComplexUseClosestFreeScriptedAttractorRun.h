#pragma once

#include "TaskComplexUseClosestFreeScriptedAttractor.h"

class NOTSA_EXPORT_VTABLE CTaskComplexUseClosestFreeScriptedAttractorRun final : public CTaskComplexUseClosestFreeScriptedAttractor {
public:
    static constexpr auto Type = eTaskType::TASK_COMPLEX_USE_CLOSEST_FREE_SCRIPTED_ATTRACTOR_RUN;

    static void InjectHooks();

    CTaskComplexUseClosestFreeScriptedAttractorRun();
    ~CTaskComplexUseClosestFreeScriptedAttractorRun() override = default;

    CTask*    Clone()       const override { return new CTaskComplexUseClosestFreeScriptedAttractorRun{ *this }; }
    eTaskType GetTaskType() const override { return Type; }

private: // Wrappers for hooks
    // 0x4B8790
    CTaskComplexUseClosestFreeScriptedAttractorRun* Constructor() {
        this->CTaskComplexUseClosestFreeScriptedAttractorRun::CTaskComplexUseClosestFreeScriptedAttractorRun();
        return this;
    }

    // 0x4B8840
    CTaskComplexUseClosestFreeScriptedAttractorRun* Destructor() {
        this->CTaskComplexUseClosestFreeScriptedAttractorRun::~CTaskComplexUseClosestFreeScriptedAttractorRun();
        return this;
    }
};
