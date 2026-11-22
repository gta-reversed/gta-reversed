#pragma once

#include "Event.h"
#include "Vehicle.h"

class NOTSA_EXPORT_VTABLE CEventSexyVehicle : public CEvent {
public:
    CEventSexyVehicle(CVehicle* vehicle);
    ~CEventSexyVehicle() override = default;

    eEventType         GetEventType() const override { return EVENT_SEXY_VEHICLE; }
    int32              GetEventPriority() const override { return 3; }
    int32              GetLifeTime() override { return 0; }
    CEventSexyVehicle* Clone() const noexcept override { return new CEventSexyVehicle(m_SexyVehicle); }
    bool               AffectsPed(CPed* ped) override;

    CVehicle* GetSexyVehicle() const { return m_SexyVehicle; }

public:
    static void InjectHooks();

private:
    CVehicle::Ref m_SexyVehicle{};
};
VALIDATE_SIZE(CEventSexyVehicle, 0x10);
