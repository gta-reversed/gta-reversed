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
    const auto C = [this, ped](eTaskType tt) { return CreateSubTask(tt, ped); };

    if (!m_Ped || !m_Ped->bInVehicle || !m_Ped->m_pVehicle) {
        return C(TASK_FINISHED);
    }

    CEntity::ChangeEntityReference(m_Car, m_Ped->m_pVehicle);

    m_bAsDriver = m_Car->IsDriver(m_Ped);
    m_TargetSeat = CCarEnterExit::ComputeTargetDoorToExit(m_Car, m_Ped);
    if (m_TargetSeat < 0) {
        return C(TASK_FINISHED);
    }

    m_TargetDoor = m_TargetSeat;
    return CTaskComplexEnterCar::CreateFirstSubTask(ped);
}
