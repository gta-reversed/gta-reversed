#pragma once

#include "EventEditableResponse.h"

class CEventLowAngerAtPlayer : public CEventEditableResponse {
    CEventLowAngerAtPlayer() = default;
    ~CEventLowAngerAtPlayer() override = default;

    eEventType GetEventType() const override { return EVENT_LOW_ANGER_AT_PLAYER; }
    int32 GetEventPriority() const override { return 33; }
    int32 GetLifeTime() override { return 0; }
    bool AffectsPed(CPed* ped) override { return ped->IsAlive(); }
    CEventEditableResponse* CloneEditable() override { return new CEventLowAngerAtPlayer(); }
};
