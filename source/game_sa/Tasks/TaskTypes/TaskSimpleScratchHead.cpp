#include "StdInc.h"

#include "TaskSimpleScratchHead.h"

// 0x48DF30
CTaskSimpleScratchHead::CTaskSimpleScratchHead() :
    CTaskSimpleRunAnim(ANIM_GROUP_DEFAULT, ANIM_ID_XPRESSSCRATCH, 4.0, TASK_SIMPLE_SCRATCH_HEAD, "ScratchHead", false) {
}

// 0x48DF60
CTaskSimpleScratchHead::CTaskSimpleScratchHead(const CTaskSimpleScratchHead&) :
    CTaskSimpleScratchHead{}
{}

void CTaskSimpleScratchHead::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleScratchHead, 0x85a100, 10);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedConstructorInstall(0x48DF30, "1", {});
    RH_ScopedDestructorInstall(0x48E930);

    RH_ScopedVMTInstall(Clone, 0x48DF60);
    RH_ScopedVMTInstall(IsInterruptable, 0x48DFD0);
}
