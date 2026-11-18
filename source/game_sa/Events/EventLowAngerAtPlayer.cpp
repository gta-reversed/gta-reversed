#include "StdInc.h"

#include "EventLowAngerAtPlayer.h"

void CEventLowAngerAtPlayer::InjectHooks() {
    RH_ScopedVirtualClass(CEventLowAngerAtPlayer, 0x86cc30, 17);
    RH_ScopedCategory("Events");

    //RH_ScopedDestructorInstall(0x5FF5E0); // Doesn't compile for some reason

    RH_ScopedVMTInstall(GetEventType, 0x5FF5F0);
    RH_ScopedVMTInstall(GetEventPriority, 0x5FF910);
    RH_ScopedVMTInstall(GetLifeTime, 0x5FF600);
    RH_ScopedVMTInstall(AffectsPed, 0x4B0EA0);
    RH_ScopedVMTInstall(CloneEditable, 0x5FF610);
}
