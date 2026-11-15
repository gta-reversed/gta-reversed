#include "StdInc.h"

#include "TaskSimpleDead.h"

// NOTSA: *deathTime* originally int32
// 0x630590
CTaskSimpleDead::CTaskSimpleDead(uint32 deathTime, bool hasDrowned) :
    m_TimeOfDeathMs{ deathTime },
    m_bHasDrowned{ hasDrowned } {
}

// 0x636100
CTaskSimpleDead::CTaskSimpleDead(const CTaskSimpleDead& o) :
    CTaskSimpleDead{ o.m_TimeOfDeathMs, o.m_bHasDrowned } {
}

// 0x630600
bool CTaskSimpleDead::ProcessPed(CPed* ped) {
    return plugin::CallMethodAndReturn<bool, 0x630600, CTaskSimpleDead*, CPed*>(this, ped);
}

void CTaskSimpleDead::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleDead, 0x86dea4, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedConstructorInstall(0x630590, "1", {}, uint32, bool);
    RH_ScopedDestructorInstall(0x6305F0);

    RH_ScopedVMTInstall(Clone, 0x636100);
    RH_ScopedVMTInstall(GetTaskType, 0x6305D0);
    RH_ScopedVMTInstall(MakeAbortable, 0x6305E0);
    RH_ScopedVMTInstall(ProcessPed, 0x630600, { .Reversed = false });
}
