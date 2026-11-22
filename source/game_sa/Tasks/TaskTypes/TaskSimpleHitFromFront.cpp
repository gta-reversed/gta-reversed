#include "StdInc.h"
#include "TaskSimpleHitFromFront.h"

void CTaskSimpleHitFromFront::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleHitFromFront, 0x86e5ec, 10);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x637250);

    RH_ScopedVMTInstall(Clone, 0x637290);
    RH_ScopedVMTInstall(IsInterruptable, 0x637280);
}

// 0x637250
CTaskSimpleHitFromFront::CTaskSimpleHitFromFront() :
    CTaskSimpleRunAnim{
        ANIM_GROUP_DEFAULT,
        ANIM_ID_HIT_FRONT,
        4.0f,
        Type,
        "HitFromFront",
        false
    }
{
}

CTaskSimpleHitFromFront::CTaskSimpleHitFromFront(const CTaskSimpleHitFromFront& o) :
    CTaskSimpleHitFromFront{}
{
}
