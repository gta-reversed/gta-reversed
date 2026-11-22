#include "StdInc.h"

#include "TaskComplexEnterCarAsPassenger.h"

// 0x640340
CTaskComplexEnterCarAsPassenger::CTaskComplexEnterCarAsPassenger(CVehicle* targetVehicle, int32 nTargetSeat, bool bCarryOnAfterFallingOff)
    : CTaskComplexEnterCar(targetVehicle, false, false, false, bCarryOnAfterFallingOff)
{
    m_TargetSeat = nTargetSeat;
}

// For 0x6437F0
CTaskComplexEnterCarAsPassenger::CTaskComplexEnterCarAsPassenger(const CTaskComplexEnterCarAsPassenger& o) :
    CTaskComplexEnterCarAsPassenger{
        o.m_Car,
        o.m_TargetSeat,
        o.m_bCarryOnAfterFallingOff
    }
{
    m_MoveState = o.m_MoveState;
}

void CTaskComplexEnterCarAsPassenger::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexEnterCarAsPassenger, 0x86eadc, 12);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x640340);
    RH_ScopedInstall(Destructor, 0x640380);

    RH_ScopedVMTInstall(Clone, 0x6437F0);
    RH_ScopedVMTInstall(GetTaskType, 0x640370);
}
