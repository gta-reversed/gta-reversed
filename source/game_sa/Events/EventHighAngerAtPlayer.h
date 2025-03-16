#pragma once

#include "EventEditableResponse.h"

#include "EventEditableResponse.h"
#include "Ped.h"


class CEventHighAngerAtPlayer : public CEventEditableResponse {
public:
    CEventHighAngerAtPlayer() = default;
    ~CEventHighAngerAtPlayer() override = default;

    eEventType GetEventType() const override { return EVENT_HIGH_ANGER_AT_PLAYER; }
    int32 GetEventPriority() const override { return 34; }
    int32 GetLifeTime() override { return 0; }
    bool AffectsPed(CPed* ped) override { return ped->IsAlive(); };
    CEventEditableResponse* CloneEditable() override { return new CEventHighAngerAtPlayer(); }
};
VALIDATE_SIZE(CEventHighAngerAtPlayer, 0x14);
