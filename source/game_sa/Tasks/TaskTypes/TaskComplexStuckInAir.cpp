#include "StdInc.h"

#include "TaskComplexStuckInAir.h"

// 0x67BE50
CTask* CTaskComplexStuckInAir::ControlSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x67BE50, CTaskComplexStuckInAir*, CPed*>(this, ped);
}

// 0x67BE20
CTask* CTaskComplexStuckInAir::CreateFirstSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x67BE20, CTaskComplexStuckInAir*, CPed*>(this, ped);
}

// 0x67BD10
CTask* CTaskComplexStuckInAir::CreateNextSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x67BD10, CTaskComplexStuckInAir*, CPed*>(this, ped);
}

// 0x67BA80
CTask* CTaskComplexStuckInAir::CreateSubTask(eTaskType taskType, CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x67BA80, CTaskComplexStuckInAir*, eTaskType, CPed*>(this, taskType, ped);
}

void CTaskComplexStuckInAir::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexStuckInAir, 0x870608, 11);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedVMTInstall(ControlSubTask, 0x67BE50, { .reversed = false });
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x67BE20, { .reversed = false });
    RH_ScopedVMTInstall(CreateNextSubTask, 0x67BD10, { .reversed = false });
    RH_ScopedInstall(CreateSubTask, 0x67BA80, { .reversed = false });
}
