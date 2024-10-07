#pragma once

#include "Event.h"

#include "Event.h"

class NOTSA_EXPORT_VTABLE CEventInAir : public CEvent {
public:
    CEventInAir() = default;
    ~CEventInAir() override = default;

    eEventType GetEventType() const override { return EVENT_IN_AIR; }
    int32 GetEventPriority() const override { return 61; }
    int32 GetLifeTime() override { return 0; }
    CEvent* Clone() override { return new CEventInAir(); }
    bool AffectsPed(CPed* ped) override;

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CEventInAir* Constructor();

};
VALIDATE_SIZE(CEventInAir, 0xC);
