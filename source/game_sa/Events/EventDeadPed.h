#pragma once

#include "Event.h"

class CPed;

class NOTSA_EXPORT_VTABLE CEventDeadPed : public CEventEditableResponse {
public:
    CEventDeadPed(CPed* ped, bool bUnknown, uint32 deathTimeInMs);
    ~CEventDeadPed() override;

    eEventType              GetEventType() const override { return EVENT_DEAD_PED; }
    int32                   GetEventPriority() const override { return 15; }
    int32                   GetLifeTime() override { return 0; }
    bool                    AffectsPed(CPed* ped) override;
    CEntity*                GetSourceEntity() const override { return m_ped; }
    float                   GetLocalSoundLevel() override { return 60.0f; }
    CEventEditableResponse* CloneEditable() const noexcept override;

    auto GetDeadPed() const { return m_ped; }

public:
    static void InjectHooks();

private:
    CPed*  m_ped{};
    bool   field_18{};
    uint32 m_deathTimeInMs{};
};
VALIDATE_SIZE(CEventDeadPed, 0x20);
