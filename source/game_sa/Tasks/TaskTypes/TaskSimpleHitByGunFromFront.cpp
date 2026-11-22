#include "StdInc.h"
#include "TaskSimpleHitByGunFromFront.h"

void CTaskSimpleHitByGunFromFront::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleHitByGunFromFront, 0x86dc10, 10);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x62EE70);

    RH_ScopedVMTInstall(Clone, 0x62EEA0);
    RH_ScopedVMTInstall(IsInterruptable, 0x62EF10);
}

// 0x62EE70
CTaskSimpleHitByGunFromFront::CTaskSimpleHitByGunFromFront() :
    CTaskSimpleRunAnim::CTaskSimpleRunAnim{
        ANIM_GROUP_DEFAULT,
        ANIM_ID_SHOT_PARTIAL,
        4.0f,
        Type,
        "HitByGunFromFront",
        false
    }
{
}

CTaskSimpleHitByGunFromFront::CTaskSimpleHitByGunFromFront(const CTaskSimpleHitByGunFromFront&) :
    CTaskSimpleHitByGunFromFront{}
{
}
