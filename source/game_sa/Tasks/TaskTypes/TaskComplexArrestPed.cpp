#include "StdInc.h"

#include "TaskComplexArrestPed.h"

void CTaskComplexArrestPed::InjectHooks() {
    using namespace ReversibleHooks;
    Install("CTaskComplexArrestPed", "CTaskComplexArrestPed", 0x68B990, &CTaskComplexArrestPed::Constructor);
}

// 0x68B990
CTaskComplexArrestPed::CTaskComplexArrestPed(CPed* ped) : CTaskComplex() {
    m_pedToArrest = ped;
    m_vehicle = nullptr;
    if (ped)
        ped->RegisterReference(reinterpret_cast<CEntity**>(&m_pedToArrest));
}

CTaskComplexArrestPed::~CTaskComplexArrestPed() {
    if (m_pedToArrest)
        m_pedToArrest->CleanUpOldReference(reinterpret_cast<CEntity**>(&m_pedToArrest));
}

CTaskComplexArrestPed* CTaskComplexArrestPed::Constructor(CPed* ped) {
    this->CTaskComplexArrestPed::CTaskComplexArrestPed(ped);
    return this;
}

bool CTaskComplexArrestPed::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    return plugin::CallMethodAndReturn<bool, 0x68BA60, CTaskComplexArrestPed*, CPed*, eAbortPriority, const CEvent*>(this, ped, priority, event);
}

CTask* CTaskComplexArrestPed::CreateNextSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x690220, CTaskComplexArrestPed*, CPed*>(this, ped);
}

CTask* CTaskComplexArrestPed::CreateFirstSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x6907A0, CTaskComplexArrestPed*, CPed*>(this, ped);
}

CTask* CTaskComplexArrestPed::ControlSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x68D350, CTaskComplexArrestPed*, CPed*>(this, ped);
}

CTask* CTaskComplexArrestPed::CreateSubTask(int32 taskId, CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x68CF80, CTaskComplexArrestPed*, int32, CPed*>(this, taskId, ped);
}
