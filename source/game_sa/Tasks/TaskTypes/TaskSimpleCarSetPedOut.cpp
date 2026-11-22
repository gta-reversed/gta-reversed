#include "StdInc.h"

#include "TaskSimpleCarSetPedOut.h"

// 0x6478B0
CTaskSimpleCarSetPedOut::CTaskSimpleCarSetPedOut(CVehicle* targetVehicle, eTargetDoor nTargetDoor, bool bSwitchOffEngine, bool warpingOutOfCar) :
    m_nTargetDoor{ nTargetDoor },
    m_pTargetVehicle{ targetVehicle },
    m_bSwitchOffEngine{ bSwitchOffEngine },
    m_bWarpingOutOfCar{ warpingOutOfCar } {
}

CTaskSimpleCarSetPedOut::CTaskSimpleCarSetPedOut(CVehicle* targetVehicle, eTargetDoor nTargetDoor, bool bSwitchOffEngine) :
    CTaskSimpleCarSetPedOut{ targetVehicle, nTargetDoor, bSwitchOffEngine, false } {
}

// 0x649F50
CTaskSimpleCarSetPedOut::CTaskSimpleCarSetPedOut(const CTaskSimpleCarSetPedOut& o) :
    CTaskSimpleCarSetPedOut{ o.m_pTargetVehicle, o.m_nTargetDoor, o.m_bSwitchOffEngine, o.m_bWarpingOutOfCar } {
    m_bWarpingOutOfCar     = o.m_bWarpingOutOfCar;
    m_bFallingOutOfCar     = o.m_bFallingOutOfCar;
    m_bKnockedOffBike      = o.m_bKnockedOffBike;
    m_nDoorFlagsToClear    = o.m_nDoorFlagsToClear;
    m_nNumGettingInToClear = o.m_nNumGettingInToClear;
}

// 0x6479B0
void CTaskSimpleCarSetPedOut::PositionPedOutOfCollision(CPed* ped, CVehicle* veh, int32 door) {
    return plugin::Call<0x6479B0>(ped, veh, door);
}

// 0x647D10
bool CTaskSimpleCarSetPedOut::ProcessPed(CPed* ped) {
    return plugin::CallMethodAndReturn<bool, 0x647D10, CTask*, CPed*>(this, ped);
}

void CTaskSimpleCarSetPedOut::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleCarSetPedOut, 0x86eeb8, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedConstructorInstall(0x6478B0, "1", {}, CVehicle*, eTargetDoor, bool);
    RH_ScopedDestructorInstall(0x647950);

    RH_ScopedGlobalInstall(PositionPedOutOfCollision, 0x6479B0, { .Reversed = false });

    RH_ScopedVMTInstall(Clone, 0x649F50);
    RH_ScopedVMTInstall(GetTaskType, 0x647930);
    RH_ScopedVMTInstall(MakeAbortable, 0x647940);
    RH_ScopedVMTInstall(ProcessPed, 0x647D10, { .Reversed = false });
}
