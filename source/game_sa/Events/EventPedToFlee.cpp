#include "StdInc.h"

#include "EventPedToFlee.h"

// 0x4AF240
CEventPedToFlee::CEventPedToFlee(CPed* ped) : m_PedToFlee{ ped } {
}

void CEventPedToFlee::InjectHooks() {
    RH_ScopedVirtualClass(CEventPedToFlee, 0x85b178, 16);
    RH_ScopedCategory("Events");

    RH_ScopedConstructorInstall(0x4AF240, "1", {}, CPed *);
    RH_ScopedDestructorInstall(0x4AF2D0);

    RH_ScopedVMTInstall(GetEventType, 0x4AF2A0);
    RH_ScopedVMTInstall(GetEventPriority, 0x4AF2C0);
    RH_ScopedVMTInstall(GetLifeTime, 0x4AF2B0);
    RH_ScopedVMTInstall(Clone, 0x4B73D0);
    RH_ScopedVMTInstall(AffectsPed, 0x4AF330);
}
