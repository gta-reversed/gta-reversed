#pragma once

#include "Event.h"

#include "Event.h"

class NOTSA_EXPORT_VTABLE CEventCreatePartnerTask : public CEvent {
public:
    int32 m_partnerType;
    CPed* m_partner;
    bool  m_isLeadSpeaker;
    float m_meetDist;

public:
    CEventCreatePartnerTask(int32 type, CPed* partner, bool isLeadSpeaker, float meetDist);
    ~CEventCreatePartnerTask() override;

    eEventType GetEventType()        const override { return EVENT_CREATE_PARTNER_TASK; }
    int32      GetEventPriority()    const override { return 6; }
    int32      GetLifeTime()               override { return 0; }
    CEvent*    Clone()                     override { return new CEventCreatePartnerTask(m_partnerType, m_partner, m_isLeadSpeaker, m_meetDist); }
    bool       AffectsPed(CPed* ped)       override { return true; }
};
VALIDATE_SIZE(CEventCreatePartnerTask, 0x1C);
