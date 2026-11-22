#include "StdInc.h"
#include "TaskSimpleHitFromBehind.h"

void CTaskSimpleHitFromBehind::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleHitFromBehind, 0x86f168, 10);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x652EE0);

    RH_ScopedVMTInstall(Clone, 0x652F10);
    RH_ScopedVMTInstall(IsInterruptable, 0x652F80);
}

// 0x652EE0
CTaskSimpleHitFromBehind::CTaskSimpleHitFromBehind() :
    CTaskSimpleRunAnim{
        ANIM_GROUP_DEFAULT,
        ANIM_ID_HIT_BEHIND,
        4.0f,
        Type,
        "HitFromBehind",
        false
    }
{
}

CTaskSimpleHitFromBehind::CTaskSimpleHitFromBehind(const CTaskSimpleHitFromBehind& o) :
    CTaskSimpleHitFromBehind{}
{
}
