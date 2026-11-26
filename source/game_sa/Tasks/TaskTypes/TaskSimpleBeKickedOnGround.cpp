#include "StdInc.h"
#include "TaskSimpleBeKickedOnGround.h"

void CTaskSimpleBeKickedOnGround::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleBeKickedOnGround, 0x86d7d8, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedConstructorInstall(0x61FC20, "1", {});
    RH_ScopedDestructorInstall(0x61FC50);

    RH_ScopedGlobalInstall(FinishAnimBeKickedOnGroundCB, 0x61FD10, { .Reversed = false });
    RH_ScopedInstall(StartAnim, 0x61FD20, { .Reversed = false });
    RH_ScopedVMTInstall(Clone, 0x623120);
    RH_ScopedVMTInstall(GetTaskType, 0x61FC40);
    RH_ScopedVMTInstall(MakeAbortable, 0x61FCC0, { .Reversed = false });
    RH_ScopedVMTInstall(ProcessPed, 0x61FE00, { .Reversed = false });
}

// 0x61FD10
CTaskSimpleBeKickedOnGround* CTaskSimpleBeKickedOnGround::FinishAnimBeKickedOnGroundCB(CAnimBlendAssociation* anim, void* data) {
    return plugin::CallAndReturn<CTaskSimpleBeKickedOnGround*, 0x61FD10>(anim, data);
}

// 0x61FD20
void CTaskSimpleBeKickedOnGround::StartAnim(CPed* ped) {
    return plugin::CallMethodAndReturn<void, 0x61FD20, CTaskSimpleBeKickedOnGround*, CPed*>(this, ped);
}

// 0x61FCC0
bool CTaskSimpleBeKickedOnGround::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    return plugin::CallMethodAndReturn<bool, 0x61FCC0, CTaskSimpleBeKickedOnGround*, CPed*, int32, const CEvent*>(this, ped, priority, event);
}

// 0x61FE00
bool CTaskSimpleBeKickedOnGround::ProcessPed(CPed* ped) {
    return plugin::CallMethodAndReturn<bool, 0x61FE00, CTaskSimpleBeKickedOnGround*, CPed*>(this, ped);
}
