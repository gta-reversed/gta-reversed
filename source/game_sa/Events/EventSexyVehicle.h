#pragma once

#include "Event.h"

#include "Event.h"

class NOTSA_EXPORT_VTABLE CEventSexyVehicle : public CEvent {
public:
    CVehicle* m_vehicle;

public:
    CEventSexyVehicle(CVehicle* vehicle);
    ~CEventSexyVehicle() override;

    eEventType GetEventType() const override { return EVENT_SEXY_VEHICLE; }
    int32 GetEventPriority() const override { return 3; }
    int32 GetLifeTime() override { return 0; }
    CEventSexyVehicle* Clone() override { return new CEventSexyVehicle(m_vehicle); }
    bool AffectsPed(CPed* ped) override;
};
VALIDATE_SIZE(CEventSexyVehicle, 0x10);
