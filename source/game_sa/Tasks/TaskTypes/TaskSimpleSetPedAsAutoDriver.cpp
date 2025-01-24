#include "StdInc.h"
#include "TaskSimpleSetPedAsAutoDriver.h"

void CTaskSimpleSetPedAsAutoDriver::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleSetPedAsAutoDriver, 0x86eff4, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x648F40);
    RH_ScopedInstall(Destructor, 0x648FC0);

    RH_ScopedVMTInstall(Clone, 0x64A2F0);
    RH_ScopedVMTInstall(GetTaskType, 0x648FA0);
    RH_ScopedVMTInstall(MakeAbortable, 0x648FB0);
    RH_ScopedVMTInstall(ProcessPed, 0x649020);
}

// 0x648F40
CTaskSimpleSetPedAsAutoDriver::CTaskSimpleSetPedAsAutoDriver(CVehicle* veh) :
    m_veh{veh}
{
    CEntity::SafeRegisterRef(m_veh);
}

CTaskSimpleSetPedAsAutoDriver::CTaskSimpleSetPedAsAutoDriver(const CTaskSimpleSetPedAsAutoDriver& o) :
    CTaskSimpleSetPedAsAutoDriver{o.m_veh}
{
}

// 0x648FC0
CTaskSimpleSetPedAsAutoDriver::~CTaskSimpleSetPedAsAutoDriver() {
    CEntity::SafeCleanUpRef(m_veh);
}

// 0x649020
bool CTaskSimpleSetPedAsAutoDriver::ProcessPed(CPed* ped) {
    CCarCtrl::JoinCarWithRoadSystem(m_veh);
    m_veh->m_nStatus = STATUS_PHYSICS;
    m_veh->m_autoPilot.SetCarMission(MISSION_CRUISE);
    m_veh->m_autoPilot.SetCruiseSpeed(10);
    m_veh->vehicleFlags.bEngineOn = m_veh->vehicleFlags.bEngineBroken != 0;
    return true;
}
