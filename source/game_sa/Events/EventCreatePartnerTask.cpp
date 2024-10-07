#include "StdInc.h"
#include "EventCreatePartnerTask.h"


CEventCreatePartnerTask::CEventCreatePartnerTask(int32 type, CPed* partner, bool isLeadSpeaker, float meetDist) :
    m_partnerType{type},
    m_partner{partner},
    m_isLeadSpeaker{isLeadSpeaker},
    m_meetDist{meetDist}
{
    CEntity::SafeRegisterRef(m_partner);
}

CEventCreatePartnerTask::~CEventCreatePartnerTask() {
    CEntity::SafeCleanUpRef(m_partner);
}

