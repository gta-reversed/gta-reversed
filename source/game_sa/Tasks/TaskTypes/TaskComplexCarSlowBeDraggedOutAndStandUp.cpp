#include "StdInc.h"

#include "TaskComplexCarSlowBeDraggedOutAndStandUp.h"

void CTaskComplexCarSlowBeDraggedOutAndStandUp::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexCarSlowBeDraggedOutAndStandUp, 0x86EF80, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x648620);
    RH_ScopedInstall(Destructor, 0x648690);
    RH_ScopedInstall(CreateSubTask, 0x648710, { .reversed = false });
    RH_ScopedInstall(Clone, 0x64A190, { .reversed = false });
    RH_ScopedInstall(GetTaskType, 0x648680);
    RH_ScopedInstall(MakeAbortable, 0x6486F0, { .reversed = false });
    RH_ScopedInstall(CreateNextSubTask, 0x6488F0, { .reversed = false });
    RH_ScopedInstall(CreateFirstSubTask, 0x648A10, { .reversed = false });
    RH_ScopedInstall(ControlSubTask, 0x648B80, { .reversed = false });
}

// 0x648620
CTaskComplexCarSlowBeDraggedOutAndStandUp::CTaskComplexCarSlowBeDraggedOutAndStandUp(CVehicle* vehicle, int32 a3) : CTaskComplex() {
    m_Vehicle = vehicle;
    dword10 = a3;
    CEntity::SafeRegisterRef(m_Vehicle);
}

// 0x648690
CTaskComplexCarSlowBeDraggedOutAndStandUp::~CTaskComplexCarSlowBeDraggedOutAndStandUp() {
    CEntity::SafeCleanUpRef(m_Vehicle);
}

// 0x648710
CTask* CTaskComplexCarSlowBeDraggedOutAndStandUp::CreateSubTask(eTaskType taskType, CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x648710, CTaskComplexCarSlowBeDraggedOutAndStandUp*, eTaskType, CPed*>(this, taskType, ped);
}

// 0x6486F0
bool CTaskComplexCarSlowBeDraggedOutAndStandUp::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    return plugin::CallMethodAndReturn<bool, 0x6486F0, CTaskComplexCarSlowBeDraggedOutAndStandUp*, CPed*, int32, CEvent const*>(this, ped, priority, event);
}

// 0x6488F0
CTask* CTaskComplexCarSlowBeDraggedOutAndStandUp::CreateNextSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x6488F0, CTaskComplexCarSlowBeDraggedOutAndStandUp*, CPed*>(this, ped);
}

// 0x648A10
CTask* CTaskComplexCarSlowBeDraggedOutAndStandUp::CreateFirstSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x648A10, CTaskComplexCarSlowBeDraggedOutAndStandUp*, CPed*>(this, ped);
}

// 0x648B80
CTask* CTaskComplexCarSlowBeDraggedOutAndStandUp::ControlSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x648B80, CTaskComplexCarSlowBeDraggedOutAndStandUp*, CPed*>(this, ped);
}
