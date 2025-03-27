#pragma once

#include "Event.h"
#include "Ped.h"
#include "Vehicle.h"

class CVehicle;

class NOTSA_EXPORT_VTABLE CEventVehicleHitAndRun : public CEvent {
public:
    CPed*     m_victim;
    CVehicle* m_vehicle;

public:
    CEventVehicleHitAndRun(CPed* victim, CVehicle* vehicle);
    ~CEventVehicleHitAndRun() override;

    eEventType GetEventType() const override { return EVENT_VEHICLE_HIT_AND_RUN; }
    int32 GetEventPriority() const override { return 14; }
    int32 GetLifeTime() override { return 0; }
    CEvent* Clone() override;
    bool AffectsPed(CPed* ped) override { return ped->IsAlive(); }
    bool IsCriminalEvent() override { return m_vehicle->m_pDriver && m_vehicle->m_pDriver->IsPlayer(); }
    void ReportCriminalEvent(CPed* ped) override;
    float GetLocalSoundLevel() override { return 100.0f; }

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CEventVehicleHitAndRun* Constructor(CPed* victim, CVehicle* vehicle);

};

VALIDATE_SIZE(CEventVehicleHitAndRun, 0x14);
