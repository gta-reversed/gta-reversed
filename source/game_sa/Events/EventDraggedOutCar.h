#pragma once

#include "EventEditableResponse.h"

class CVehicle;
class CEntity;
class CPed;

class NOTSA_EXPORT_VTABLE CEventDraggedOutCar : public CEventEditableResponse {
public:
    CPed*     m_CarJacker;
    CVehicle* m_Vehicle;
    bool      m_IsDriverSeat;

public:
    CEventDraggedOutCar(CVehicle* vehicle, CPed* carjacker, bool IsDriverSeat);
    ~CEventDraggedOutCar() override;

    eEventType GetEventType() const override { return EVENT_DRAGGED_OUT_CAR; }
    int32 GetEventPriority() const override { return 40; }
    int32 GetLifeTime() override { return 0; }
    bool AffectsPed(CPed* ped) override;
    bool AffectsPedGroup(CPedGroup* pedGroup) override;
    CEntity* GetSourceEntity() const override { return m_CarJacker; }
    float GetLocalSoundLevel() override { return 100.0f; }
    CEventEditableResponse* CloneEditable() override;

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CEventDraggedOutCar* Constructor(CVehicle* vehicle, CPed* carjacker, bool IsDriverSeat);
};
VALIDATE_SIZE(CEventDraggedOutCar, 0x20);
