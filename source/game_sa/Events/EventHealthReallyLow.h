#pragma once

#include "EventEditableResponse.h"

#include "EventEditableResponse.h"
#include "Ped.h"


class CEventHealthReallyLow : public CEventEditableResponse {
public:
    CEventHealthReallyLow() = default;
    ~CEventHealthReallyLow() override = default;

    eEventType GetEventType() const override { return EVENT_HEALTH_REALLY_LOW; }
    int32 GetEventPriority() const override { return 69; }
    int32 GetLifeTime() override { return 0; }
    bool AffectsPed(CPed* ped) override { return ped->IsAlive(); };
    CEventEditableResponse* CloneEditable() override { return new CEventHealthReallyLow(); }
};
VALIDATE_SIZE(CEventHealthReallyLow, 0x14);
