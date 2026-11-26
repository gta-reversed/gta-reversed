#pragma once

#include "EventEditableResponse.h"
#include <Enums/eTaskType.h>

class NOTSA_EXPORT_VTABLE CEventSexyPed : public CEventEditableResponse {
public:
    CEventSexyPed(CPed* ped);
    CEventSexyPed(CPed* sexyPed, eTaskType taskType);
    ~CEventSexyPed() override = default;

    eEventType              GetEventType() const override { return EVENT_SEXY_PED; }
    int32                   GetEventPriority() const override { return 4; }
    int32                   GetLifeTime() override { return 0; }
    bool                    AffectsPed(CPed* ped) override;
    CEntity*                GetSourceEntity() const override { return m_SexyPed; }
    CEventEditableResponse* CloneEditable() const noexcept override { return new CEventSexyPed(m_SexyPed); }

    CPed* GetSexyPed() const { return m_SexyPed; }

public:
    static void InjectHooks();

private:
    CPed::Ref m_SexyPed{};
};
VALIDATE_SIZE(CEventSexyPed, 0x18);
