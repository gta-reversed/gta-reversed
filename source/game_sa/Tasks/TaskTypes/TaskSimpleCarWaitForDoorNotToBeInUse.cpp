#include "StdInc.h"
#include "TaskSimpleCarWaitForDoorNotToBeInUse.h"

void CTaskSimpleCarWaitForDoorNotToBeInUse::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleCarWaitForDoorNotToBeInUse, 0x86edbc, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x646B70);

    RH_ScopedInstall(Destructor, 0x646C00);

    RH_ScopedVMTInstall(Clone, 0x649D20);
    RH_ScopedVMTInstall(GetTaskType, 0x646BE0);
    RH_ScopedVMTInstall(MakeAbortable, 0x646BF0);
    RH_ScopedVMTInstall(ProcessPed, 0x646C90);
    RH_ScopedVMTInstall(SetPedPosition, 0x646CC0);
}

/*!
* @addr 0x646B70
* @brief Wait until either of the 2 doors are ready
* @param veh            The vehicle whose door we have to wait for
* @param doorToWaitFor1 Main door to wait for
* @param doorToWaitFor2 Alternative door
*/
CTaskSimpleCarWaitForDoorNotToBeInUse::CTaskSimpleCarWaitForDoorNotToBeInUse(CVehicle* veh, uint32 doorToWaitFor1, uint32 doorToWaitFor2) :
    m_veh{veh},
    m_doorsToWaitFor{doorToWaitFor1, doorToWaitFor2}
{
    CEntity::SafeRegisterRef(m_veh);
}

CTaskSimpleCarWaitForDoorNotToBeInUse::CTaskSimpleCarWaitForDoorNotToBeInUse(const CTaskSimpleCarWaitForDoorNotToBeInUse& o)
    : CTaskSimpleCarWaitForDoorNotToBeInUse{o.m_veh, o.m_doorsToWaitFor[0], o.m_doorsToWaitFor[1]} {}

// 0x646C00
CTaskSimpleCarWaitForDoorNotToBeInUse::~CTaskSimpleCarWaitForDoorNotToBeInUse() {
    CEntity::SafeCleanUpRef(m_veh);
}

// 0x646BF0
bool CTaskSimpleCarWaitForDoorNotToBeInUse::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
    return true;
}

// 0x646C90
bool CTaskSimpleCarWaitForDoorNotToBeInUse::ProcessPed(CPed* ped) {
    return !m_veh || !CCarEnterExit::IsCarDoorInUse(m_veh, m_doorsToWaitFor[0], m_doorsToWaitFor[1]);
}

// 0x646CC0
bool CTaskSimpleCarWaitForDoorNotToBeInUse::SetPedPosition(CPed* ped) {
    if (ped->IsInVehicle(m_veh)) {
        ped->SetPedPositionInCar();
    }
    return true;
}
