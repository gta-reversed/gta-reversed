#pragma once

#include "Event.h"

#include "Event.h"


class NOTSA_EXPORT_VTABLE CEventLeanOnVehicle : public CEvent {
public:
    CVehicle* m_vehicle;
    int32 m_leanAnimDurationInMs;

public:
    CEventLeanOnVehicle(CVehicle* vehicle, int32 leanAnimDurationInMs);
    ~CEventLeanOnVehicle() override;

    eEventType GetEventType() const override { return EVENT_LEAN_ON_VEHICLE; }
    int32 GetEventPriority() const override { return 12; }
    int32 GetLifeTime() override { return 0; }
    CEventLeanOnVehicle* Clone() override { return new CEventLeanOnVehicle(m_vehicle, m_leanAnimDurationInMs); }
    bool AffectsPed(CPed* ped) override { return true; }
    bool IsValid(CPed* ped) override;

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CEventLeanOnVehicle* Constructor(CVehicle* vehicle, int32 leanAnimDurationInMs);

};
VALIDATE_SIZE(CEventLeanOnVehicle, 0x14);
