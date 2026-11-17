#include "StdInc.h"

#include "EventSpecial.h"

void CEventSpecial::InjectHooks() {
    RH_ScopedVirtualClass(CEventSpecial, 0x85b5d0, 17);
    RH_ScopedCategory("Events");

    RH_ScopedConstructorInstall(0x4B1AE0, "1", {});

    RH_ScopedVMTInstall(GetEventType, 0x4B1B10);
    RH_ScopedVMTInstall(GetEventPriority, 0x4B1B40);
    RH_ScopedVMTInstall(GetLifeTime, 0x4B1B20);
    RH_ScopedVMTInstall(AffectsPed, 0x4B1B30);
    RH_ScopedVMTInstall(CloneEditable, 0x4B7920);
}
