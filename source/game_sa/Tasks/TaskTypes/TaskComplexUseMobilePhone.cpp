#include "StdInc.h"
#include "TaskComplexUseMobilePhone.h"

// 0x6348A0
CTaskComplexUseMobilePhone::CTaskComplexUseMobilePhone(int32 duration) :
    m_Duration{ duration } {
}

// 0x636FD0
CTaskComplexUseMobilePhone::CTaskComplexUseMobilePhone(const CTaskComplexUseMobilePhone& o) :
    CTaskComplexUseMobilePhone{ o.m_Duration } {
}

// 0x6348F0
void CTaskComplexUseMobilePhone::RemovePhoneModel(CPed* ped) {
    return plugin::CallAndReturn<void, 0x6348F0, CPed*>(ped);
}

// 0x634A40
int8 CTaskComplexUseMobilePhone::Stop(CPed* ped) {
    return plugin::CallMethodAndReturn<int8, 0x634A40, CTaskComplexUseMobilePhone*, CPed*>(this, ped);
}

// 0x634930
bool CTaskComplexUseMobilePhone::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    return plugin::CallMethodAndReturn<bool, 0x634930, CTaskComplexUseMobilePhone*, CPed*, eAbortPriority, const CEvent*>(this, ped, priority, event);
}

// 0x634A60
CTask* CTaskComplexUseMobilePhone::CreateNextSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x634A60, CTaskComplexUseMobilePhone*, CPed*>(this, ped);
}

// 0x634C10
CTask* CTaskComplexUseMobilePhone::CreateFirstSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x634C10, CTaskComplexUseMobilePhone*, CPed*>(this, ped);
}

// 0x634D60
CTask* CTaskComplexUseMobilePhone::ControlSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x634D60, CTaskComplexUseMobilePhone*, CPed*>(this, ped);
}

void CTaskComplexUseMobilePhone::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexUseMobilePhone, 0x86e454, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedConstructorInstall(0x6348A0, "1", {}, int32);
    RH_ScopedDestructorInstall(0x6348E0);

    RH_ScopedGlobalInstall(RemovePhoneModel, 0x6348F0, { .Reversed = false });
    RH_ScopedInstall(Stop, 0x634A40, { .Reversed = false });
    RH_ScopedVMTInstall(Clone, 0x636FD0);
    RH_ScopedVMTInstall(GetTaskType, 0x6348D0);
    RH_ScopedVMTInstall(MakeAbortable, 0x634930, { .Reversed = false });
    RH_ScopedVMTInstall(CreateNextSubTask, 0x634A60, { .Reversed = false });
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x634C10, { .Reversed = false });
    RH_ScopedVMTInstall(ControlSubTask, 0x634D60, { .Reversed = false });
}
