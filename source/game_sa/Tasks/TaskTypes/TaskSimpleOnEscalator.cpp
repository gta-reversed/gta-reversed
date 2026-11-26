#include "StdInc.h"
#include "TaskSimpleOnEscalator.h"

// 0x4B8670
CTaskSimpleOnEscalator::CTaskSimpleOnEscalator() :
    CTaskSimpleStandStill{ 0, true }
{
}

// 0x4B86C0
CTaskSimpleOnEscalator::CTaskSimpleOnEscalator(const CTaskSimpleOnEscalator&) :
    CTaskSimpleOnEscalator{}
{
}

// 0x62F520
bool CTaskSimpleOnEscalator::ProcessPed(CPed* ped) {
    return plugin::CallMethodAndReturn<bool, 0x62F520, CTaskSimpleOnEscalator*, CPed*>(this, ped);
}

void CTaskSimpleOnEscalator::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleOnEscalator, 0x85b8dc, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedConstructorInstall(0x4B8670, "1", {});
    RH_ScopedDestructorInstall(0x4B8750);

    RH_ScopedVMTInstall(Clone, 0x4B86C0);
    RH_ScopedVMTInstall(GetTaskType, 0x4B8720);
    RH_ScopedVMTInstall(ProcessPed, 0x62F520, { .Reversed = false });
}
