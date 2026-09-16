#include "StdInc.h"

#include "TaskComplexDragPedFromCar.h"

void CTaskComplexDragPedFromCar::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexDragPedFromCar, 0x86eb6c, 12);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x640430);
    RH_ScopedInstall(Destructor, 0x6404D0);

    RH_ScopedVMTInstall(Clone, 0x643950, { .reversed = false });
    RH_ScopedVMTInstall(GetTaskType, 0x6404C0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x643D00, { .reversed = false });
    RH_ScopedVMTInstall(ControlSubTask, 0x640530);
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

// 0x0
CTask* CTaskComplexDragPedFromCar::CreateFirstSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x643D00, CTaskComplexDragPedFromCar*, CPed*>(this, ped);
}