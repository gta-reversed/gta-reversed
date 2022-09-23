#include "StdInc.h"

#include "TaskComplexFacial.h"

void CTaskComplexFacial::InjectHooks() {
    RH_ScopedClass(CTaskComplexFacial);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x690D20, { .reversed = false });
    RH_ScopedInstall(StopAll, 0x691250, { .reversed = false });
    RH_ScopedVirtualInstall(Clone, 0x6928B0, { .reversed = false });
    RH_ScopedVirtualInstall(GetTaskType, 0x690D80, { .reversed = false });
    RH_ScopedVirtualInstall(MakeAbortable, 0x690DA0, { .reversed = false });
    RH_ScopedVirtualInstall(CreateNextSubTask, 0x690DC0, { .reversed = false });
    RH_ScopedVirtualInstall(CreateFirstSubTask, 0x690F30, { .reversed = false });
    RH_ScopedVirtualInstall(ControlSubTask, 0x690FC0, { .reversed = false });
}

// 0x690D20
CTaskComplexFacial::CTaskComplexFacial() : CTaskComplex() {
    m_nFacialExpression1 = eFacialExpression::NONE;
    m_nFacialExpression2 = eFacialExpression::NONE;
    m_bNotPlayer = CGeneral::GetRandomNumberInRange(0, 100) > 70;
    m_bStarted = false;
    m_bIsAborting = false;
}

// 0x691250
void CTaskComplexFacial::StopAll() {
    if (m_pSubTask->GetTaskType() == TASK_SIMPLE_FACIAL) {
        m_bIsAborting = true;
    }
}

// 0x690DA0
bool CTaskComplexFacial::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
    return priority == ABORT_PRIORITY_IMMEDIATE && m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_IMMEDIATE, event);
}

// 0x690DC0
CTask* CTaskComplexFacial::CreateNextSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x690DC0, CTaskComplexFacial*, CPed*>(this, ped);
}

// 0x690F30
CTask* CTaskComplexFacial::CreateFirstSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x690F30, CTaskComplexFacial*, CPed*>(this, ped);
}

// 0x690FC0
CTask* CTaskComplexFacial::ControlSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x690FC0, CTaskComplexFacial*, CPed*>(this, ped);
}
