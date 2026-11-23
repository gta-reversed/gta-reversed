#pragma once

#include "Event.h"
#include "Ped.h"

class NOTSA_EXPORT_VTABLE CEventCreatePartnerTask : public CEvent {
public:
    CEventCreatePartnerTask(int32 type, CPed* partner, bool isLeadSpeaker, float meetDist);
    ~CEventCreatePartnerTask() override = default;

    eEventType GetEventType() const override { return EVENT_CREATE_PARTNER_TASK; }
    int32      GetEventPriority() const override { return 6; }
    int32      GetLifeTime() override { return 0; }
    CEvent*    Clone() const noexcept override { return new CEventCreatePartnerTask(m_PartnerType, m_Partner, m_IsLeadSpeaker, m_MeetDist); }
    bool       AffectsPed(CPed* ped) override { return true; }

    CPed* GetPartner() const { return m_Partner; }
    auto  IsLeadSpeaker() const { return m_IsLeadSpeaker; }
    auto  GetPartnerType() const { return m_PartnerType; }
    auto  GetMeetDist() const { return m_MeetDist; }

public:
    static void InjectHooks();

private:
    int32     m_PartnerType{};
    CPed::Ref m_Partner{};
    bool      m_IsLeadSpeaker{};
    float     m_MeetDist{};
};
VALIDATE_SIZE(CEventCreatePartnerTask, 0x1C);
