#include "StdInc.h"
#include "TaskSimpleHitByGunFromRear.h"

void CTaskSimpleHitByGunFromRear::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleHitByGunFromRear, 0x86dc4c, 10);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x62EF20);

    RH_ScopedVMTInstall(Clone, 0x62EF50);
    RH_ScopedVMTInstall(IsInterruptable, 0x62EFC0);
}

// 0x62EF20
CTaskSimpleHitByGunFromRear::CTaskSimpleHitByGunFromRear() :
    CTaskSimpleRunAnim::CTaskSimpleRunAnim{
        ANIM_GROUP_DEFAULT,
        ANIM_ID_SHOT_PARTIAL_B,
        4.0f,
        Type,
        "HitByGunFromRear",
        false
}
{
}

CTaskSimpleHitByGunFromRear::CTaskSimpleHitByGunFromRear(const CTaskSimpleHitByGunFromRear&) :
    CTaskSimpleHitByGunFromRear{}
{
}
