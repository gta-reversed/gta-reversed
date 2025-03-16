#pragma once

#include "EventEditableResponse.h"

#include "EventEditableResponse.h"
#include "Ped.h"


class CEventHealthLow : public CEventEditableResponse {
public:
    CEventHealthLow() = default;
    ~CEventHealthLow() override = default;

    eEventType GetEventType() const override { return EVENT_HEALTH_LOW; }
    int32 GetEventPriority() const override { return 68; }
    int32 GetLifeTime() override { return 0; }
    bool AffectsPed(CPed* ped) override { return ped->IsAlive(); };
    CEventEditableResponse* CloneEditable() override { return new CEventHealthLow(); }
};
VALIDATE_SIZE(CEventHealthLow, 0x14);
