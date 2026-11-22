#include "StdInc.h"
#include "TaskSimpleLookAbout.h"

void CTaskSimpleLookAbout::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleLookAbout, 0x0085A164, 10);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x48E0A0);

    RH_ScopedVMTInstall(Clone, 0x48E0F0);
    RH_ScopedVMTInstall(IsInterruptable, 0x48E160);
}

// 0x48E0A0
CTaskSimpleLookAbout::CTaskSimpleLookAbout(uint32 durationMs) :
    CTaskSimpleRunTimedAnim{
        ANIM_GROUP_DEFAULT,
        ANIM_ID_IDLE_HBHB_1,
        4.0f,
        -4.0f,
        durationMs,
        Type,
        "LookAbout",
        false
    }
{
}

CTaskSimpleLookAbout::CTaskSimpleLookAbout(const CTaskSimpleLookAbout& o) :
    CTaskSimpleLookAbout{o.m_durationMs}
{
}
