#pragma once

#include "EventEditableResponse.h"
#include "Ped.h"

class NOTSA_EXPORT_VTABLE CEventHealthReallyLow : public CEventEditableResponse {
public:
    CEventHealthReallyLow() = default;
    ~CEventHealthReallyLow() override = default;

    eEventType GetEventType() const override { return EVENT_HEALTH_REALLY_LOW; }
    int32 GetEventPriority() const override { return 69; }
    int32 GetLifeTime() override { return 0; }
    bool AffectsPed(CPed* ped) override { return ped->IsAlive(); };
    CEventEditableResponse* CloneEditable() const noexcept override { return new CEventHealthReallyLow(); }

public:
    static void InjectHooks();
};
VALIDATE_SIZE(CEventHealthReallyLow, 0x14);
