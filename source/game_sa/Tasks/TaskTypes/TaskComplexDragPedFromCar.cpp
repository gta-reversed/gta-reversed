#include "StdInc.h"

#include "TaskComplexDragPedFromCar.h"

void CTaskComplexDragPedFromCar__InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexDragPedFromCar, 0x86EB6C, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedVMTInstall(ControlSubTask, 0x640530);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x643D00);
}

// 0x640430
CTaskComplexDragPedFromCar::CTaskComplexDragPedFromCar(CPed* ped, int32 draggedPedDownTime) : CTaskComplexEnterCar(nullptr, false, false, true, false) {
    m_Ped = ped;
    CEntity::SafeRegisterRef(m_Ped);
    m_DraggedPedDownTime = draggedPedDownTime;
}

// 0x6404D0
CTaskComplexDragPedFromCar::~CTaskComplexDragPedFromCar() {
    CEntity::SafeCleanUpRef(m_Ped);
}

// 0x640530


CTask* CTaskComplexDragPedFromCar::ControlSubTask(CPed* ped) {
    if (m_NumGettingInSet)
        return CTaskComplexEnterCar::ControlSubTask(ped);

    if (!m_Ped || m_Ped->bInVehicle || !m_pSubTask->MakeAbortable(ped))
        return CTaskComplexEnterCar::ControlSubTask(ped);

    return CTaskComplexEnterCar::CreateSubTask(TASK_NONE, nullptr);
}

// 0x643D00
CTask* CTaskComplexDragPedFromCar::CreateFirstSubTask(CPed* ped) {
    if (!m_Ped) {
        return CreateSubTask(TASK_NONE, ped);
    }

    auto* const pedVeh = m_Ped->m_pVehicle;
    if (!pedVeh || !m_Ped->bInVehicle || (!pedVeh->IsPassenger(m_Ped) && !pedVeh->IsDriver(m_Ped))) {
        return CreateSubTask(TASK_NONE, ped);
    }

    // The vehicle we're working with is the one that the dragged ped is in
    CEntity::SafeCleanUpRef(m_Car);
    m_Car = pedVeh;
    CEntity::SafeRegisterRef(m_Car);

    m_bAsDriver                = m_Car->m_pDriver == m_Ped;
    m_bQuitAfterOpeningDoor    = false;
    m_bQuitAfterDraggingPedOut = true;

    // NOTE: A door is stored into the seat field here (same as the original)
    // TODO: verify this for other vehicle types / subtypes.
    if (!m_Car->m_pHandlingData->m_bTandemSeats
        && !m_Car->IsBike()
        && !m_Car->IsSubQuad())
    {
        m_TargetSeat = CCarEnterExit::ComputeTargetDoorToExit(m_Car, m_Ped);
    } else {
        m_TargetSeat = 0;
    }

    return CTaskComplexEnterCar::CreateFirstSubTask(ped);
}
