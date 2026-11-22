#include "StdInc.h"
#include "TaskSimpleHitFromBack.h"

void CTaskSimpleHitFromBack::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleHitFromBack, 0x86db70, 10);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x62EC60);

    RH_ScopedVMTInstall(Clone, 0x62EC90);
    RH_ScopedVMTInstall(IsInterruptable, 0x62ED00);
}

// 0x62EC60
CTaskSimpleHitFromBack::CTaskSimpleHitFromBack() :
    CTaskSimpleRunAnim{
        ANIM_GROUP_DEFAULT,
        ANIM_ID_HIT_BACK,
        4.0f,
        Type,
        "HitFromBack",
        false
    }
{
}

CTaskSimpleHitFromBack::CTaskSimpleHitFromBack(const CTaskSimpleHitFromBack& o) :
    CTaskSimpleHitFromBack{}
{
}
