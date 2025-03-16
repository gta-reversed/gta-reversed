#pragma once

#include "EventEditableResponse.h"


class NOTSA_EXPORT_VTABLE CEventSeenPanickedPed : public CEventEditableResponse {
public:
    CPed* m_ped;

public:
    explicit CEventSeenPanickedPed(CPed* ped);
    ~CEventSeenPanickedPed() override;

    eEventType GetEventType() const override { return EVENT_SEEN_PANICKED_PED; }
    int32 GetEventPriority() const override { return 13; }
    int32 GetLifeTime() override { return 0; }
    bool AffectsPed(CPed* ped) override;
    CEntity* GetSourceEntity() const override { return m_ped;}
    CEventSeenPanickedPed* CloneEditable() override { return new CEventSeenPanickedPed(m_ped); }

private:
    friend void InjectHooksMain();
    static void InjectHooks();
    CEventSeenPanickedPed* Constructor(CPed* ped);
};
VALIDATE_SIZE(CEventSeenPanickedPed, 0x18);
