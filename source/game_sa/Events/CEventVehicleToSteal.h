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
    eEventType GetEventType() override { return EVENT_VEHICLE_TO_STEAL; }
    int GetEventPriority() override { return 7; }
    int GetLifeTime() override { return 0; }
    CEvent* Clone() override { return new CEventVehicleToSteal(m_vehicle); }
    bool AffectsPed(CPed* ped) override;

    bool AffectsPed_Reversed(CPed* ped);
};
