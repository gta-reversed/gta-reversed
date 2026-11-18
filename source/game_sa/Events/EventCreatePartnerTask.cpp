#include "StdInc.h"
#include "EventCreatePartnerTask.h"

CEventCreatePartnerTask::CEventCreatePartnerTask(int32 type, CPed* partner, bool isLeadSpeaker, float meetDist) :
    m_PartnerType{ type },
    m_Partner{ partner },
    m_IsLeadSpeaker{ isLeadSpeaker },
    m_MeetDist{ meetDist } {
}

void CEventCreatePartnerTask::InjectHooks() {
    RH_ScopedVirtualClass(CEventCreatePartnerTask, 0x86c6d0, 16);
    RH_ScopedCategory("Events");

    RH_ScopedConstructorInstall(0x5F6190, "1", {}, int32, CPed*, int8, float);
    RH_ScopedDestructorInstall(0x5F6200);

    RH_ScopedVMTInstall(GetEventType, 0x5F6260);
    RH_ScopedVMTInstall(GetEventPriority, 0x5F6410);
    RH_ScopedVMTInstall(GetLifeTime, 0x5F6270);
    RH_ScopedVMTInstall(Clone, 0x5F6280);
    RH_ScopedVMTInstall(AffectsPed, 0x5F62F0);
}
