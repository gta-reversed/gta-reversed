#pragma once

#include "EventEditableResponse.h"
#include "Vehicle.h"


class NOTSA_EXPORT_VTABLE CEventPotentialGetRunOver : public CEventEditableResponse {
public:
    CEventPotentialGetRunOver(CVehicle* vehicle);
    ~CEventPotentialGetRunOver() override = default;

    eEventType GetEventType() const override { return EVENT_POTENTIAL_GET_RUN_OVER; }
    int32 GetEventPriority() const override { return 51; }
    int32 GetLifeTime() override { return 0; }
    bool AffectsPed(CPed* ped) override;
    CEntity* GetSourceEntity() const override { return m_RunOverBy ? m_RunOverBy->m_pDriver : nullptr; }
    float GetLocalSoundLevel() override { return 100.0f; }
    CEventEditableResponse* CloneEditable() const noexcept override { return new CEventPotentialGetRunOver(m_RunOverBy); }

    CVehicle* GetRunOverBy() const { return m_RunOverBy; }

public:
    static void InjectHooks();

private:
    CVehicle::Ref m_RunOverBy{};
};
VALIDATE_SIZE(CEventPotentialGetRunOver, 0x18);
