#include "StdInc.h"
#include "TaskSimpleCarForcePedOut.h"

void CTaskSimpleCarForcePedOut::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleCarForcePedOut, 0x86ee94, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedConstructorInstall(0x647710, "1", {}, CVehicle*, int32);
    RH_ScopedDestructorInstall(0x647790);

    RH_ScopedVMTInstall(Clone, 0x649EE0);
    RH_ScopedVMTInstall(GetTaskType, 0x647770);
    RH_ScopedVMTInstall(MakeAbortable, 0x647780);
    RH_ScopedVMTInstall(ProcessPed, 0x6477F0);
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
    if (!m_Veh) {
        return true;
    }

    // Wait while someone is getting out through the opposite door (unless we're the driver)
    if (!m_Veh->IsDriver(ped) && (CCarEnterExit::ComputeOppositeDoorFlag(m_Veh, m_Door, false) & m_Veh->m_nGettingOutFlags)) {
        if (m_Veh->m_pDriver) {
            return false;
        }
        // NOTE: Only checks the passenger seats before the one of our door
        const auto passengerIdx = CCarEnterExit::ComputePassengerIndexFromCarDoor(m_Veh, m_Door);
        for (auto i = 0; i < passengerIdx; i++) {
            if (m_Veh->m_apPassengers[i]) {
                return false;
            }
        }
    }

    ped->PositionPedOutOfCollision(m_Door, m_Veh, true);
    return true;
}
