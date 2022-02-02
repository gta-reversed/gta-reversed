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
    RH_ScopedClass(CTaskComplexStuckInAir);
    RH_ScopedCategory("Tasks/TaskTypes");
    // RH_ScopedInstall(ControlSubTask_Reversed, 0x67BE50);
    // RH_ScopedInstall(CreateFirstSubTask_Reversed, 0x67BE20);
    // RH_ScopedInstall(CreateNextSubTask_Reversed, 0x67BD10);
    // RH_ScopedInstall(CreateSubTask_Reversed, 0x67BA80);
}

CTask* CTaskComplexStuckInAir::ControlSubTask_Reversed(CPed* ped) {
    return CTaskComplexStuckInAir::ControlSubTask(ped);
}

CTask* CTaskComplexStuckInAir::CreateFirstSubTask_Reversed(CPed* ped) {
    return CTaskComplexStuckInAir::CreateFirstSubTask(ped);
}

CTask* CTaskComplexStuckInAir::CreateNextSubTask_Reversed(CPed* ped) {
    return CTaskComplexStuckInAir::CreateNextSubTask(ped);
}

CTask* CTaskComplexStuckInAir::CreateSubTask_Reversed(eTaskType taskType, CPed* ped) {
    return CTaskComplexStuckInAir::CreateSubTask(taskType, ped);
}
