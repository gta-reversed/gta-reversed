#include "StdInc.h"

#include "TaskComplexCarSlowBeDraggedOut.h"

void CTaskComplexCarSlowBeDraggedOut::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexCarSlowBeDraggedOut, 0x86EF54, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x648490);
    RH_ScopedInstall(Destructor, 0x64C580);
    RH_ScopedInstall(CreateTaskUtilityLineUpPedWithCar, 0x648520, { .reversed = false });
    RH_ScopedInstall(CreateSubTask, 0x64C6A0, { .reversed = false });
    RH_ScopedInstall(Clone, 0x64A120, { .reversed = false });
    RH_ScopedInstall(GetTaskType, 0x648500);
    RH_ScopedInstall(MakeAbortable, 0x64C600, { .reversed = false });
    RH_ScopedInstall(CreateNextSubTask, 0x64C810, { .reversed = false });
    RH_ScopedInstall(CreateFirstSubTask, 0x64C8B0, { .reversed = false });
    RH_ScopedInstall(ControlSubTask, 0x648510, { .reversed = false });
}

// 0x648490
CTaskComplexCarSlowBeDraggedOut::CTaskComplexCarSlowBeDraggedOut(CVehicle* vehicle, int32 door, bool a4) : CTaskComplex() {
    m_Door = door;
    m_Vehicle = vehicle;
    field_14 = a4;
    m_TaskUtilityLineUpPedWithCar = nullptr;
    CEntity::SafeRegisterRef(m_Vehicle);
}

// 0x64C580
CTaskComplexCarSlowBeDraggedOut::~CTaskComplexCarSlowBeDraggedOut() {
    CEntity::SafeCleanUpRef(m_Vehicle);
    delete m_TaskUtilityLineUpPedWithCar;
}

// 0x648520
CTaskUtilityLineUpPedWithCar* CTaskComplexCarSlowBeDraggedOut::CreateTaskUtilityLineUpPedWithCar(int32 a2) {
    return plugin::CallMethodAndReturn<CTaskUtilityLineUpPedWithCar*, 0x648520, CTaskComplexCarSlowBeDraggedOut*, int32>(this, a2);
}

// 0x64C6A0
CTask* CTaskComplexCarSlowBeDraggedOut::CreateSubTask(eTaskType taskType) {
    return plugin::CallMethodAndReturn<CTask*, 0x64C6A0, CTaskComplexCarSlowBeDraggedOut*, eTaskType>(this, taskType);
}

// 0x64C600
bool CTaskComplexCarSlowBeDraggedOut::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
    return plugin::CallMethodAndReturn<bool, 0x64C600, CTaskComplexCarSlowBeDraggedOut*, CPed*, eAbortPriority, CEvent const*>(this, ped, priority, event);
}

// 0x64C810
CTask* CTaskComplexCarSlowBeDraggedOut::CreateNextSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x64C810, CTaskComplexCarSlowBeDraggedOut*, CPed*>(this, ped);
}

// 0x64C8B0
CTask* CTaskComplexCarSlowBeDraggedOut::CreateFirstSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x64C8B0, CTaskComplexCarSlowBeDraggedOut*, CPed*>(this, ped);
}

// 0x648510
CTask* CTaskComplexCarSlowBeDraggedOut::ControlSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x648510, CTaskComplexCarSlowBeDraggedOut*, CPed*>(this, ped);
}
