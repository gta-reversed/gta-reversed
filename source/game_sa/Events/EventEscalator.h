#pragma once

#include "Event.h"

#include "Event.h"


class NOTSA_EXPORT_VTABLE CEventEscalator : public CEvent {
public:
    CEventEscalator() = default;
    ~CEventEscalator() override = default;

    eEventType GetEventType() const override { return EVENT_ON_ESCALATOR; }
    int32 GetEventPriority() const override { return 56; }
    int32 GetLifeTime() override { return 0; }
    CEventEscalator* Clone() override { return new CEventEscalator(); }
    bool AffectsPed(CPed* ped) override;
    bool TakesPriorityOver(const CEvent& refEvent) override { return GetEventType() != refEvent.GetEventType(); }

private:
    friend void InjectHooksMain();
    static void InjectHooks();

};
VALIDATE_SIZE(CEventEscalator, 0xC);
