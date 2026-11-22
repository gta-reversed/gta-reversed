#include "StdInc.h"
#include "TaskSimpleHitFromRight.h"

void CTaskSimpleHitFromRight::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleHitFromRight, 0x86dbd8, 10);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x62EDC0);

    RH_ScopedVMTInstall(Clone, 0x62EDF0);
    RH_ScopedVMTInstall(IsInterruptable, 0x62EE60);
}

// 0x62EDC0
CTaskSimpleHitFromRight::CTaskSimpleHitFromRight() :
    CTaskSimpleRunAnim{
        ANIM_GROUP_DEFAULT,
        ANIM_ID_HIT_R,
        4.0f,
        Type,
        "HitFromRight",
        false
    }
{
}

CTaskSimpleHitFromRight::CTaskSimpleHitFromRight(const CTaskSimpleHitFromRight& o) :
    CTaskSimpleHitFromRight{}
{
}
