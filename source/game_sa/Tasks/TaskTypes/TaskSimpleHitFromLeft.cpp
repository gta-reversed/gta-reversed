#include "StdInc.h"
#include "TaskSimpleHitFromLeft.h"

void CTaskSimpleHitFromLeft::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleHitFromLeft, 0x86dba4, 10);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x62ED10);

    RH_ScopedVMTInstall(Clone, 0x62ED40);
    RH_ScopedVMTInstall(IsInterruptable, 0x62EDB0);
}

// 0x62ED10
CTaskSimpleHitFromLeft::CTaskSimpleHitFromLeft() :
    CTaskSimpleRunAnim{
        ANIM_GROUP_DEFAULT,
        ANIM_ID_HIT_L,
        4.0f,
        Type,
        "HitFromLeft",
        false
    }
{
}

CTaskSimpleHitFromLeft::CTaskSimpleHitFromLeft(const CTaskSimpleHitFromLeft& o) :
    CTaskSimpleHitFromLeft{}
{
}
