#pragma once

#include "Event.h"

#include "Event.h"
#include "TaskComplexEnterCar.h"
#include "TheScripts.h"


class NOTSA_EXPORT_VTABLE CEventVehicleToSteal : public CEvent {
public:
    CVehicle* m_vehicle;

public:
    CEventVehicleToSteal(CVehicle* vehicle);
    ~CEventVehicleToSteal() override;

    eEventType GetEventType() const override { return EVENT_VEHICLE_TO_STEAL; }
    int32 GetEventPriority() const override { return 7; }
    int32 GetLifeTime() override { return 0; }
    CEvent* Clone() override { return new CEventVehicleToSteal(m_vehicle); }
    bool AffectsPed(CPed* ped) override;

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CEventVehicleToSteal* Constructor(CVehicle* vehicle);

};
VALIDATE_SIZE(CEventVehicleToSteal, 0x10);
