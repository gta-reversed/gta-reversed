#pragma once

#include "EventEditableResponse.h"

class CVehicle;

class NOTSA_EXPORT_VTABLE CEventVehicleOnFire : public CEventEditableResponse {
public:
    CVehicle* m_vehicle;

public:
    static void InjectHooks();

    CEventVehicleOnFire(CVehicle* vehicle);
    ~CEventVehicleOnFire() override;

    eEventType GetEventType() const override { return EVENT_VEHICLE_ON_FIRE; }
    int32 GetEventPriority() const override { return 54; }
    int32 GetLifeTime() override { return 0; }
    bool AffectsPed(CPed* ped) override;
    float GetLocalSoundLevel() override { return 60.0f; }
    CEventEditableResponse* CloneEditable() override;

private:
    CEventVehicleOnFire* Constructor(CVehicle* vehicle);
};

VALIDATE_SIZE(CEventVehicleOnFire, 0x18);
