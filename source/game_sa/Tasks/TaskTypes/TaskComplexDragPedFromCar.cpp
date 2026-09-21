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
    if (   m_Ped
        && m_Ped->m_pVehicle
        && !m_Ped->bInVehicle // Ped was dragged out already (Likely by another task?)
        && (m_Ped->m_pVehicle->IsDriver(m_Ped) || m_Ped->m_pVehicle->IsPassenger(m_Ped))
    ) {
        if (m_Car) {
            m_Car->CleanUpOldReference(reinterpret_cast<CEntity**>(&m_Car));
        }
        m_Car = m_Ped->m_pVehicle;
        m_Car->RegisterReference(reinterpret_cast<CEntity**>(&m_Car));

        m_bAsDriver = m_Car->m_pDriver == m_Ped;
        m_bQuitAfterDraggingPedOut = true;

        if (m_DraggedPed) {
            const auto draggedPedVeh = m_DraggedPed->m_pVehicle;
            if (!draggedPedVeh->vehicleFlags.bIsBus
                && draggedPedVeh->m_nVehicleType != VEHICLE_TYPE_AUTOMOBILE
                && draggedPedVeh->m_nVehicleSubType != VEHICLE_TYPE_AUTOMOBILE
            ) {
                m_TargetSeat = CCarEnterExit::ComputeTargetDoorToExit(draggedPedVeh, m_Ped);
                m_DraggedPedDownTime = CTimer::GetTimeInMS();
                return CTaskComplexEnterCar::CreateFirstSubTask(ped);
            }
            m_TargetSeat = 0;
        }
        return CTaskComplexEnterCar::CreateFirstSubTask(ped);
    }
    return CTaskComplexEnterCar::CreateSubTask(TASK_FINISHED, ped);
}