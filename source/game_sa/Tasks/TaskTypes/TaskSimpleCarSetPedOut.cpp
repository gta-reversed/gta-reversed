#include "StdInc.h"

#include "TaskSimpleCarSetPedOut.h"

CTaskSimpleCarSetPedOut::CTaskSimpleCarSetPedOut(CVehicle* targetVehicle, eTargetDoor nTargetDoor, bool bSwitchOffEngine, bool warpingOutOfCar) :
    m_nTargetDoor{ nTargetDoor },
    m_pTargetVehicle{ targetVehicle },
    m_bSwitchOffEngine{ bSwitchOffEngine },
    m_bWarpingOutOfCar{ warpingOutOfCar }
{
    CEntity::SafeRegisterRef(m_pTargetVehicle);
}

CTaskSimpleCarSetPedOut::~CTaskSimpleCarSetPedOut() {
    CEntity::SafeCleanUpRef(m_pTargetVehicle);
}

// 0x6479B0
void CTaskSimpleCarSetPedOut::PositionPedOutOfCollision(CPed* ped, CVehicle* veh, int32 door) {
    return plugin::Call<0x6479B0>(ped, veh, door);
}

CTask* CTaskSimpleCarSetPedOut::Clone() const {
    return plugin::CallMethodAndReturn<CTask*, 0x649F50, const CTask*>(this);
}

bool CTaskSimpleCarSetPedOut::ProcessPed(CPed* ped) {
    return plugin::CallMethodAndReturn<bool, 0x647D10, CTask*, CPed*>(this, ped);
}
