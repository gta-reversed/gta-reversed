#pragma once

#include "Event.h"

#include "Event.h"

class NOTSA_EXPORT_VTABLE CEventCarUpsideDown : public CEvent {
public:
    CVehicle* m_vehicle;

public:
    explicit CEventCarUpsideDown(CVehicle* vehicle);
    ~CEventCarUpsideDown() override;

    eEventType GetEventType() const override { return EVENT_CAR_UPSIDE_DOWN; }
    int32 GetEventPriority() const override { return 18; }
    int32 GetLifeTime() override { return 0; }
    CEventCarUpsideDown* Clone() override { return new CEventCarUpsideDown(m_vehicle); }
    bool AffectsPed(CPed* ped) override;

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CEventCarUpsideDown* Constructor(CVehicle* vehicle);
    CEventCarUpsideDown* Destructor();

};
VALIDATE_SIZE(CEventCarUpsideDown, 0x10);
