#include "StdInc.h"

#include "TaskSimpleCarSetPedInAsDriver.h"

// 0x6470E0
CTaskSimpleCarSetPedInAsDriver::CTaskSimpleCarSetPedInAsDriver(CVehicle* targetVehicle, CTaskUtilityLineUpPedWithCar* utility) :
    m_pTargetVehicle{ targetVehicle },
    m_pUtility{ utility } {
}

// 0x649E00
CTaskSimpleCarSetPedInAsDriver::CTaskSimpleCarSetPedInAsDriver(const CTaskSimpleCarSetPedInAsDriver& o) :
    CTaskSimpleCarSetPedInAsDriver{ o.m_pTargetVehicle, o.m_pUtility } {
    m_bWarpingInToCar      = o.m_bWarpingInToCar;
    m_nDoorFlagsToClear    = o.m_nDoorFlagsToClear;
    m_nNumGettingInToClear = o.m_nNumGettingInToClear;
}

// notsa
CTaskSimpleCarSetPedInAsDriver::CTaskSimpleCarSetPedInAsDriver(CVehicle* targetVehicle, bool warpingInToCar, CTaskUtilityLineUpPedWithCar* utility) : // NOTSA
    CTaskSimpleCarSetPedInAsDriver{ targetVehicle, utility } {
    m_bWarpingInToCar = warpingInToCar;
}

// 0x64B950
bool CTaskSimpleCarSetPedInAsDriver::ProcessPed(CPed* ped) {
    return plugin::CallMethodAndReturn<bool, 0x64B950, CTask*, CPed*>(this, ped);
}

void CTaskSimpleCarSetPedInAsDriver::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleCarSetPedInAsDriver, 0x86ee28, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedConstructorInstall(0x6470E0, "1", {}, CVehicle*, CTaskUtilityLineUpPedWithCar*);
    RH_ScopedDestructorInstall(0x647170);

    RH_ScopedVMTInstall(Clone, 0x649E00);
    RH_ScopedVMTInstall(GetTaskType, 0x647150);
    RH_ScopedVMTInstall(MakeAbortable, 0x647160);
    RH_ScopedVMTInstall(ProcessPed, 0x64B950, { .Reversed = false });
}
