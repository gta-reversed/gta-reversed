#pragma once

#include "Event.h"

#include "Event.h"


class NOTSA_EXPORT_VTABLE CEventCopCarBeingStolen : public CEvent {
public:
    CPed* m_hijacker;
    CVehicle* m_vehicle;

public:
    CEventCopCarBeingStolen(CPed* hijacker, CVehicle* vehicle);
    ~CEventCopCarBeingStolen() override;

    eEventType GetEventType() const override { return EVENT_COP_CAR_BEING_STOLEN; }
    int32 GetEventPriority() const override { return 38; }
    int32 GetLifeTime() override { return 0; }
    CEventCopCarBeingStolen* Clone() override { return new CEventCopCarBeingStolen(m_hijacker, m_vehicle); }
    bool AffectsPed(CPed* ped) override;

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CEventCopCarBeingStolen* Constructor(CPed* hijacker, CVehicle* vehicle);

};
VALIDATE_SIZE(CEventCopCarBeingStolen, 0x14);
