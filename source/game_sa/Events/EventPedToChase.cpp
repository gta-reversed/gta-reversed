#include "StdInc.h"

#include "EventPedToChase.h"

// 0x4AF130
CEventPedToChase::CEventPedToChase(CPed* pedToChase) :
    m_PedToChase{ pedToChase } {
}

void CEventPedToChase::InjectHooks() {
    RH_ScopedVirtualClass(CEventPedToChase, 0x85b138, 16);
    RH_ScopedCategory("Events");

    RH_ScopedConstructorInstall(0x4AF130, "1", {}, CPed*);
    RH_ScopedDestructorInstall(0x4AF1C0);

    RH_ScopedVMTInstall(GetEventType, 0x4AF190);
    RH_ScopedVMTInstall(GetEventPriority, 0x4AF1B0);
    RH_ScopedVMTInstall(GetLifeTime, 0x4AF1A0);
    RH_ScopedVMTInstall(Clone, 0x4B7360);
    RH_ScopedVMTInstall(AffectsPed, 0x4AF220);
}
