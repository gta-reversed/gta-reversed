#include "StdInc.h"
#include "TaskSimpleHitWall.h"

void CTaskSimpleHitWall::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleHitWall, 0x86F1A0, 10);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x652F90);

    RH_ScopedVMTInstall(Clone, 0x652FC0);
    RH_ScopedVMTInstall(IsInterruptable, 0x653030);
}

// 0x48E0A0
CTaskSimpleHitWall::CTaskSimpleHitWall() :
    CTaskSimpleRunAnim{
        ANIM_GROUP_DEFAULT,
        ANIM_ID_HIT_WALL,
        4.0f,
        Type,
        "HitWall",
        false
    }
{
}

CTaskSimpleHitWall::CTaskSimpleHitWall(const CTaskSimpleHitWall&) :
    CTaskSimpleHitWall{}
{
}
