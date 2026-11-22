#include "StdInc.h"

#include "TaskSimpleCarSetPedInAsPassenger.h"
#include "TaskUtilityLineUpPedWithCar.h"

// OG constructor was at 0x646FE0
CTaskSimpleCarSetPedInAsPassenger::CTaskSimpleCarSetPedInAsPassenger(CVehicle* targetVehicle, eTargetDoor nTargetDoor, bool warpingInToCar, CTaskUtilityLineUpPedWithCar* utility) :
    m_nTargetDoor{ nTargetDoor },
    m_pTargetVehicle{ targetVehicle },
    m_pUtility{ utility },
    m_bWarpingInToCar{ warpingInToCar } {
    CEntity::SafeRegisterRef(m_pTargetVehicle);
}

CTaskSimpleCarSetPedInAsPassenger::CTaskSimpleCarSetPedInAsPassenger(CVehicle* targetVehicle, eTargetDoor nTargetDoor, CTaskUtilityLineUpPedWithCar* utility) :
    CTaskSimpleCarSetPedInAsPassenger{
        targetVehicle,
        nTargetDoor,
        false,
        utility
    } {
}

// For 0x649D90
CTaskSimpleCarSetPedInAsPassenger::CTaskSimpleCarSetPedInAsPassenger(const CTaskSimpleCarSetPedInAsPassenger& o) :
    CTaskSimpleCarSetPedInAsPassenger{
        o.m_pTargetVehicle,
        o.m_nTargetDoor,
        o.m_bWarpingInToCar,
        o.m_pUtility
    } {
    m_bWarpingInToCar      = o.m_bWarpingInToCar;
    m_nDoorFlagsToClear    = o.m_nDoorFlagsToClear;
    m_nNumGettingInToClear = o.m_nNumGettingInToClear;
}

// 0x647080
CTaskSimpleCarSetPedInAsPassenger::~CTaskSimpleCarSetPedInAsPassenger() {
    CEntity::SafeCleanUpRef(m_pTargetVehicle);
}

// 0x64B5D0
bool CTaskSimpleCarSetPedInAsPassenger::ProcessPed(CPed* ped) {
    return plugin::CallMethodAndReturn<bool, 0x64B5D0, CTask*, CPed*>(this, ped);
}

void CTaskSimpleCarSetPedInAsPassenger::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleCarSetPedInAsPassenger, 0x86ee04, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedConstructorInstall(0x646FE0, "1", {}, CVehicle*, eTargetDoor, CTaskUtilityLineUpPedWithCar*);
    RH_ScopedDestructorInstall(0x647080);

    RH_ScopedVMTInstall(Clone, 0x649D90);
    RH_ScopedVMTInstall(GetTaskType, 0x647060);
    RH_ScopedVMTInstall(MakeAbortable, 0x647070);
    RH_ScopedVMTInstall(ProcessPed, 0x64B5D0, { .Reversed = false });
}
