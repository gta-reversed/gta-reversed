#pragma once

#include "Event.h"

class CVehicle;

class NOTSA_EXPORT_VTABLE CEventVehicleDied : public CEvent {
public:
    CVehicle* m_vehicle;
    bool      m_IsOccupantLeavingCar;

public:
    CEventVehicleDied (CVehicle* vehicle);
    ~CEventVehicleDied () override;

    eEventType GetEventType() const override { return EVENT_VEHICLE_DIED; }
    int32 GetEventPriority() const override { return 72; }
    int32 GetLifeTime() override { return 0; }
    CEvent* Clone() override;
    bool AffectsPed(CPed* ped) override;

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CEventVehicleDied* Constructor(CVehicle* vehicle);

};

VALIDATE_SIZE(CEventVehicleDied, 0x14);
