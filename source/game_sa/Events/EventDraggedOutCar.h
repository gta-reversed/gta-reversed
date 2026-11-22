#pragma once

#include "EventEditableResponse.h"

#include "Ped.h"
#include "Vehicle.h"

class CEntity;

class NOTSA_EXPORT_VTABLE CEventDraggedOutCar : public CEventEditableResponse {
public:
    CEventDraggedOutCar(CVehicle* vehicle, CPed* carjacker, bool IsDriverSeat);
    ~CEventDraggedOutCar() override = default;

    eEventType              GetEventType() const override { return EVENT_DRAGGED_OUT_CAR; }
    int32                   GetEventPriority() const override { return 40; }
    int32                   GetLifeTime() override { return 0; }
    bool                    AffectsPed(CPed* ped) override;
    bool                    AffectsPedGroup(CPedGroup* pedGroup) override;
    CEntity*                GetSourceEntity() const override { return m_CarJacker; }
    float                   GetLocalSoundLevel() override { return 100.0f; }
    CEventEditableResponse* CloneEditable() const noexcept override;

    CPed*     GetCarJacker() const { return m_CarJacker; }
    CVehicle* GetVehicle() const { return m_Vehicle; }
    auto      IsDriverSeat() const { return m_IsDriverSeat; }

public:
    static void InjectHooks();

private:
    CPed::Ref     m_CarJacker{};
    CVehicle::Ref m_Vehicle{};
    bool          m_IsDriverSeat{};
};
VALIDATE_SIZE(CEventDraggedOutCar, 0x20);
