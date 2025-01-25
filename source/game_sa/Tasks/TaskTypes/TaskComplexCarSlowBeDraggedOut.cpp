#include "StdInc.h"

#include "./TaskComplexCarSlowBeDraggedOut.h"
#include "./TaskSimpleCarSetPedSlowDraggedOut.h"
#include "./TaskSimpleCarSlowBeDraggedOut.h"
#include "./TaskSimpleCarSetPedOut.h"
#include "./TaskSimpleCarFallOut.h"

void CTaskComplexCarSlowBeDraggedOut::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexCarSlowBeDraggedOut, 0x86EF54, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x648490);
    RH_ScopedInstall(Destructor, 0x64C580);

    RH_ScopedInstall(CreateTaskUtilityLineUpPedWithCar, 0x648520);
    RH_ScopedInstall(CreateSubTask, 0x64C6A0);
    RH_ScopedInstall(PrepareVehicleForPedExit, 0x6485A0);

    RH_ScopedVMTInstall(Clone, 0x64A120);
    RH_ScopedVMTInstall(GetTaskType, 0x648500);
    RH_ScopedVMTInstall(MakeAbortable, 0x64C600);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x64C810);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x64C8B0);
    RH_ScopedVMTInstall(ControlSubTask, 0x648510);
}

// 0x648490
CTaskComplexCarSlowBeDraggedOut::CTaskComplexCarSlowBeDraggedOut(CVehicle* vehicle, eTargetDoor door, bool isDraggedBySelf) :
    m_Door{ door },
    m_Vehicle{ vehicle },
    m_IsDraggedBySelf{ isDraggedBySelf }
{ }

// 0x648520
void CTaskComplexCarSlowBeDraggedOut::CreateTaskUtilityLineUpPedWithCar(CPed* ped) {
    UNUSED(ped);

    m_LineUpUtility = std::make_unique<CTaskUtilityLineUpPedWithCar>(CVector{}, 0, 0, m_Door);
}

// 0x64C6A0
CTask* CTaskComplexCarSlowBeDraggedOut::CreateSubTask(eTaskType taskType) {
    switch (taskType) {
    case TASK_SIMPLE_CAR_FALL_OUT: { // 0x64C7A9
        return new CTaskSimpleCarFallOut{m_Vehicle, m_Door, m_LineUpUtility.get()};
    }
    case TASK_FINISHED: { // 0x64C718
        return nullptr;
    }
    case TASK_SIMPLE_CAR_SET_PED_SLOW_DRAGGED_OUT: { // 0x64C770
        return new CTaskSimpleCarSetPedSlowDraggedOut{ m_Vehicle, m_Door };
    }
    case TASK_SIMPLE_CAR_SET_PED_OUT: { // 0x64C72C
        return new CTaskSimpleCarSetPedOut{ m_Vehicle, m_Door, m_IsDraggedBySelf };
    }
    case TASK_SIMPLE_CAR_SLOW_BE_DRAGGED_OUT: { // 0x64C6E3
        return new CTaskSimpleCarSlowBeDraggedOut{ m_Vehicle, m_Door, m_LineUpUtility.get(), m_IsDraggedBySelf };
    }
    default: NOTSA_UNREACHABLE();
    }
}

// 0x64C600
bool CTaskComplexCarSlowBeDraggedOut::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    if (priority == ABORT_PRIORITY_IMMEDIATE) {
        return false;
    }
    m_pSubTask->MakeAbortable(ped, priority, event);
    CTaskSimpleCarSetPedSlowDraggedOut{ m_Vehicle, m_Door }.ProcessPed(ped);
    return true;
}

// 0x64C810
CTask* CTaskComplexCarSlowBeDraggedOut::CreateNextSubTask(CPed* ped) {
    if (!ped->IsInVehicle()) {
        return nullptr;
    }
    return CreateSubTask([this, ped] {
        switch (m_pSubTask->GetTaskType()) {
        case TASK_SIMPLE_CAR_FALL_OUT:                 return TASK_SIMPLE_CAR_SET_PED_OUT;
        case TASK_SIMPLE_CAR_SET_PED_OUT:
        case TASK_SIMPLE_CAR_SLOW_BE_DRAGGED_OUT:      return TASK_SIMPLE_CAR_SET_PED_SLOW_DRAGGED_OUT;
        case TASK_SIMPLE_CAR_SET_PED_SLOW_DRAGGED_OUT: return TASK_FINISHED;
        default:                                       NOTSA_UNREACHABLE();
        }
    }());
}

// 0x6485A0
void CTaskComplexCarSlowBeDraggedOut::PrepareVehicleForPedExit(CPed* ped) {
    if (m_Door == TARGET_DOOR_DRIVER) {
        m_Vehicle->SetStatus(m_Vehicle->IsDriverAPlayer() ? STATUS_FORCED_STOP : STATUS_ABANDONED);
    }
}

// 0x??????
void CTaskComplexCarSlowBeDraggedOut::PreparePedForVehicleExit(CPed* ped) {
    ped->SetOrientation(0.f, 0.f, ped->m_fCurrentRotation);
    ped->SetPedState(PEDSTATE_NONE);
}

// 0x64C8B0
CTask* CTaskComplexCarSlowBeDraggedOut::CreateFirstSubTask(CPed* ped) {
    PrepareVehicleForPedExit(ped);
    PreparePedForVehicleExit(ped);

    CreateTaskUtilityLineUpPedWithCar(ped);

    return CreateSubTask(m_IsDraggedBySelf
        ? TASK_SIMPLE_CAR_FALL_OUT
        : TASK_SIMPLE_CAR_SLOW_BE_DRAGGED_OUT
    );
}
