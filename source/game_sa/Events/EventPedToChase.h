#pragma once

#include "Event.h"

#include "Event.h"
#include "Ped.h"

class NOTSA_EXPORT_VTABLE CEventPedToChase : public CEvent {
public:
    CPed* m_ped;

public:
    CEventPedToChase(CPed* ped);
    ~CEventPedToChase() override;

    eEventType GetEventType() const override { return EVENT_PED_TO_CHASE; }
    int32 GetEventPriority() const override { return 1; }
    int32 GetLifeTime() override { return 0; }
    CEvent* Clone() override;
    bool AffectsPed(CPed* ped) override { return ped->IsAlive(); }
};
VALIDATE_SIZE(CEventPedToChase, 0x10);
