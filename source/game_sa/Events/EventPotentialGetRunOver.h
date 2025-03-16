#pragma once

#include "EventEditableResponse.h"

#include "EventEditableResponse.h"

class NOTSA_EXPORT_VTABLE CEventPotentialGetRunOver : public CEventEditableResponse {
public:
    CVehicle* m_Vehicle;

public:
    CEventPotentialGetRunOver(CVehicle* vehicle);
    ~CEventPotentialGetRunOver() override;

    eEventType GetEventType() const override { return EVENT_POTENTIAL_GET_RUN_OVER; }
    int32 GetEventPriority() const override { return 51; }
    int32 GetLifeTime() override { return 0; }
    bool AffectsPed(CPed* ped) override;
    CEntity* GetSourceEntity() const override { return m_Vehicle ? m_Vehicle->m_pDriver : nullptr; }
    float GetLocalSoundLevel() override { return 100.0f; }
    CEventEditableResponse* CloneEditable() override { return new CEventPotentialGetRunOver(m_Vehicle); }
};
VALIDATE_SIZE(CEventPotentialGetRunOver, 0x18);
