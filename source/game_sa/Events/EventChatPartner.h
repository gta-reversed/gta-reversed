#pragma once

#include "Event.h"

#include "Event.h"

class NOTSA_EXPORT_VTABLE CEventChatPartner : public CEvent {
public:
    bool  m_leadSpeaker;
    CPed* m_partner;

public:
    CEventChatPartner(bool leadSpeaker, CPed* partner);
    ~CEventChatPartner() override;

    eEventType GetEventType() const override { return EVENT_CHAT_PARTNER; }
    int32 GetEventPriority() const override { return 5; }
    int32 GetLifeTime() override { return 0; }
    CEventChatPartner* Clone() override { return new CEventChatPartner(m_leadSpeaker, m_partner); }
    bool AffectsPed(CPed* ped)  override;
};
VALIDATE_SIZE(CEventChatPartner, 0x14);
