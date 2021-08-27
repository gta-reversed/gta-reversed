#include "CEvent.h"

class CVehicle;

class CEventVehicleToSteal : public CEvent
{
public:
    CVehicle* m_vehicle;

    static void InjectHooks();

    CEventVehicleToSteal(CVehicle* vehicle);
    ~CEventVehicleToSteal();
private:
    CEventVehicleToSteal* Constructor(CVehicle* vehicle);
public:
    eEventType GetEventType() const override { return EVENT_VEHICLE_TO_STEAL; }
    int32_t GetEventPriority() const override { return 7; }
    int GetLifeTime() override { return 0; }
    CEvent* Clone() override { return new CEventVehicleToSteal(m_vehicle); }
    bool AffectsPed(CPed* ped) override;

    bool AffectsPed_Reversed(CPed* ped);
};
