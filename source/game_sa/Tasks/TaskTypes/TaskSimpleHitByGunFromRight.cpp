#include "StdInc.h"
#include "TaskSimpleHitByGunFromRight.h"

void CTaskSimpleHitByGunFromRight::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleHitByGunFromRight, 0x86dcc4, 10);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x62F080);

    RH_ScopedVMTInstall(Clone, 0x62F0B0);
    RH_ScopedVMTInstall(IsInterruptable, 0x62F120);
}

// 0x62F080
CTaskSimpleHitByGunFromRight::CTaskSimpleHitByGunFromRight() :
    CTaskSimpleRunAnim::CTaskSimpleRunAnim{
        ANIM_GROUP_DEFAULT,
        ANIM_ID_SHOT_RIGHTP,
        4.0f,
        Type,
        "HitByGunFromRight",
        false
    }
{
}

CTaskSimpleHitByGunFromRight::CTaskSimpleHitByGunFromRight(const CTaskSimpleHitByGunFromRight&) :
    CTaskSimpleHitByGunFromRight{}
{
}
