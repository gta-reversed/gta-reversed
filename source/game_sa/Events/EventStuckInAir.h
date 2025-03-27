#pragma once

#include "Event.h"

#include "Event.h"
#include "EventInAir.h"


class NOTSA_EXPORT_VTABLE CEventStuckInAir : public CEvent {
public:
    CPed* m_ped;

public:
    CEventStuckInAir(CPed* ped);
    ~CEventStuckInAir() override;

    eEventType GetEventType() const override { return EVENT_STUCK_IN_AIR; }
    int32 GetEventPriority() const override;
    int32 GetLifeTime() override { return 0; }
    CEvent* Clone() override { return new CEventStuckInAir(m_ped); }
    bool AffectsPed(CPed* ped) override;
    bool TakesPriorityOver(const CEvent& refEvent) override;
    bool CanBeInterruptedBySameEvent() override { return true; }

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CEventStuckInAir* Constructor(CPed* ped);

};
VALIDATE_SIZE(CEventStuckInAir, 0x10);
