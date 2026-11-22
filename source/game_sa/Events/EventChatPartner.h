#pragma once

#include "Event.h"
#include "Ped.h"

class NOTSA_EXPORT_VTABLE CEventChatPartner : public CEvent {
public:
    CEventChatPartner(bool isLeadSpeaker, CPed* partner);
    ~CEventChatPartner() override = default;

    eEventType         GetEventType() const override { return EVENT_CHAT_PARTNER; }
    int32              GetEventPriority() const override { return 5; }
    int32              GetLifeTime() override { return 0; }
    CEventChatPartner* Clone() const noexcept override { return new CEventChatPartner(m_IsLeadSpeaker, m_Partner); }
    bool               AffectsPed(CPed* ped) override;

    CPed* GetPartner() const { return m_Partner; }
    bool  IsLeadSpeaker() const { return m_IsLeadSpeaker; }

public:
    static void InjectHooks();

private:
    bool      m_IsLeadSpeaker{};
    CPed::Ref m_Partner{};
};
VALIDATE_SIZE(CEventChatPartner, 0x14);
