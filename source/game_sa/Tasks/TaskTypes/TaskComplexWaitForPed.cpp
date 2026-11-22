#include "StdInc.h"
#include "TaskSimpleStandStill.h"
#include "TaskComplexWaitForPed.h"

void CTaskComplexWaitForPed::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexWaitForPed, 0x8706f8, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedConstructorInstall(0x683340, "1", {}, CPed*, float, int32, bool);
    RH_ScopedDestructorInstall(0x6833D0);

    RH_ScopedVMTInstall(Clone, 0x683950);
    RH_ScopedVMTInstall(GetTaskType, 0x6833C0);
    RH_ScopedVMTInstall(MakeAbortable, 0x683430);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x683440);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x683450);
    RH_ScopedVMTInstall(ControlSubTask, 0x6834E0, { .Reversed = false });
}
// 0x683340
CTaskComplexWaitForPed::CTaskComplexWaitForPed(CPed* waitForPed, float targetDist, int32 timeout, bool lookAtPed) :
    m_WaitForPed(waitForPed),
    m_TargetDist(targetDist),
    m_Timeout(timeout),
    m_LookAtPed(lookAtPed) {
}

// 0x683950
CTaskComplexWaitForPed::CTaskComplexWaitForPed(const CTaskComplexWaitForPed& o) :
    CTaskComplexWaitForPed{
        o.m_WaitForPed,
        o.m_TargetDist,
        o.m_Timeout,
        o.m_LookAtPed
    }
{
}

// 0x683450
CTask* CTaskComplexWaitForPed::CreateFirstSubTask(CPed* ped) {
    return new CTaskSimpleStandStill{ 999'999 };
}

// 0x6834E0
CTask* CTaskComplexWaitForPed::ControlSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x6834E0, CTaskComplexWaitForPed*, CPed*>(this, ped);
}
