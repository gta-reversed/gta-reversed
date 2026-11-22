#include "StdInc.h"
#include "EventChatPartner.h"

// 0x4AECD0
CEventChatPartner::CEventChatPartner(bool isLeadSpeaker, CPed* partner) :
    m_IsLeadSpeaker{ isLeadSpeaker },
    m_Partner{ partner } {
}

// 0x4AEDC0
bool CEventChatPartner::AffectsPed(CPed* ped) {
    return ped->IsAlive() && m_Partner;
}

void CEventChatPartner::InjectHooks() {
    RH_ScopedVirtualClass(CEventChatPartner, 0x85b070, 16);
    RH_ScopedCategory("Events");

    RH_ScopedConstructorInstall(0x4AECD0, "1", {}, bool, CPed*);
    RH_ScopedDestructorInstall(0x4AED60);

    RH_ScopedVMTInstall(GetEventType, 0x4AED30);
    RH_ScopedVMTInstall(GetEventPriority, 0x4AED50);
    RH_ScopedVMTInstall(GetLifeTime, 0x4AED40);
    RH_ScopedVMTInstall(Clone, 0x4B7210);
    RH_ScopedVMTInstall(AffectsPed, 0x4AEDC0);
}
