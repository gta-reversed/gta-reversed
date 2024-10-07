#pragma once

#include "EventEditableResponse.h"

#include "EventEditableResponse.h"

class NOTSA_EXPORT_VTABLE CEventPedEnteredMyVehicle : public CEventEditableResponse {
public:
    CEventPedEnteredMyVehicle(/*const*/ CPed* ped, /*const*/ CVehicle* vehicle, eTargetDoor targetDoor); // todo: make const as OG
    ~CEventPedEnteredMyVehicle() override;

    eEventType GetEventType() const override { return EVENT_PED_ENTERED_MY_VEHICLE; }
    int32 GetEventPriority() const override { return 39; }
    int32 GetLifeTime() override { return 0; }
    bool AffectsPed(CPed* ped) override;
    CEntity* GetSourceEntity() const override { return m_PedThatEntered; }
    float GetLocalSoundLevel() override { return 60.0f; }
    CEventEditableResponse* CloneEditable() override { return new CEventPedEnteredMyVehicle(m_PedThatEntered, m_Vehicle, m_TargetDoor); }

public:
    CPed*       m_PedThatEntered;
    CVehicle*   m_Vehicle;
    eTargetDoor m_TargetDoor;
};
VALIDATE_SIZE(CEventPedEnteredMyVehicle, 0x20);
