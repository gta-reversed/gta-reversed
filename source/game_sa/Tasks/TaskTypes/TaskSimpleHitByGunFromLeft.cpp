#include "StdInc.h"
#include "TaskSimpleHitByGunFromLeft.h"

void CTaskSimpleHitByGunFromLeft::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleHitByGunFromLeft, 0x86dc88, 10);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x62EFD0);

    RH_ScopedVMTInstall(Clone, 0x62F000);
    RH_ScopedVMTInstall(IsInterruptable, 0x62F070);
}

// 0x62EFD0
CTaskSimpleHitByGunFromLeft::CTaskSimpleHitByGunFromLeft() :
    CTaskSimpleRunAnim::CTaskSimpleRunAnim{
        ANIM_GROUP_DEFAULT,
        ANIM_ID_SHOT_LEFTP,
        4.0f,
        Type,
        "HitByGunFromLeft",
        false
}
{
}

CTaskSimpleHitByGunFromLeft::CTaskSimpleHitByGunFromLeft(const CTaskSimpleHitByGunFromLeft&) :
    CTaskSimpleHitByGunFromLeft{}
{
}
