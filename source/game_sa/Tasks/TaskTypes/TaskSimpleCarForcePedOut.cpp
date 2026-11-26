#include "StdInc.h"
#include "TaskSimpleCarForcePedOut.h"

void CTaskSimpleCarForcePedOut::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleCarForcePedOut, 0x86ee94, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedConstructorInstall(0x647710, "1", {}, CVehicle*, int32);
    RH_ScopedDestructorInstall(0x647790);

    RH_ScopedVMTInstall(Clone, 0x649EE0, { .Reversed = false });
    RH_ScopedVMTInstall(GetTaskType, 0x647770);
    RH_ScopedVMTInstall(MakeAbortable, 0x647780);
    RH_ScopedVMTInstall(ProcessPed, 0x6477F0, { .Reversed = false });
}

// 0x647710
CTaskSimpleCarForcePedOut::CTaskSimpleCarForcePedOut(CVehicle* veh, int32 door) :
    m_Veh{ veh },
    m_Door{ door } {
}

// 0x649EE0
CTaskSimpleCarForcePedOut::CTaskSimpleCarForcePedOut(const CTaskSimpleCarForcePedOut& o) :
    CTaskSimpleCarForcePedOut{ o.m_Veh, o.m_Door } {
}

// 0x6477F0
bool CTaskSimpleCarForcePedOut::ProcessPed(CPed* ped) {
    return plugin::CallMethodAndReturn<bool, 0x6477F0, CTaskSimpleCarForcePedOut*, CPed*>(this, ped);
}
