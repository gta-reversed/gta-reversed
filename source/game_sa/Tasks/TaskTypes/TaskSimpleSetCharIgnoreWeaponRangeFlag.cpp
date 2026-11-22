#include "StdInc.h"
#include "TaskSimpleSetCharIgnoreWeaponRangeFlag.h"

void CTaskSimpleSetCharIgnoreWeaponRangeFlag::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleSetCharIgnoreWeaponRangeFlag, 0x859ebc, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x474620);
    RH_ScopedInstall(Destructor, 0x4748F0);

    RH_ScopedVMTInstall(Clone, 0x474640);
    RH_ScopedVMTInstall(GetTaskType, 0x4746A0);
    RH_ScopedVMTInstall(MakeAbortable, 0x4746B0);
    RH_ScopedVMTInstall(ProcessPed, 0x622B50);
}

// 0x474620
CTaskSimpleSetCharIgnoreWeaponRangeFlag::CTaskSimpleSetCharIgnoreWeaponRangeFlag(bool bIgnoreWeaponRange) :
    m_bIgnoreWeaponRange{ bIgnoreWeaponRange } {
}

// 0x474640
CTaskSimpleSetCharIgnoreWeaponRangeFlag::CTaskSimpleSetCharIgnoreWeaponRangeFlag(const CTaskSimpleSetCharIgnoreWeaponRangeFlag& o) :
    CTaskSimpleSetCharIgnoreWeaponRangeFlag{ o.m_bIgnoreWeaponRange } {
}
