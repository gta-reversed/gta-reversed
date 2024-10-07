#include "StdInc.h"
#include "TaskSimpleThrowControl.h"

void CTaskSimpleThrowControl::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleThrowControl, 0x86D7B4, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x61F8B0);
    RH_ScopedInstall(Destructor, 0x61F950);

    RH_ScopedVMTInstall(Clone, 0x6230B0);
    RH_ScopedVMTInstall(GetTaskType, 0x61F940);
    RH_ScopedVMTInstall(MakeAbortable, 0x61F9B0);
    RH_ScopedVMTInstall(ProcessPed, 0x61F9F0, { .enabled = false, .locked = true });
}

// 0x61F8B0
CTaskSimpleThrowControl::CTaskSimpleThrowControl(CEntity* targetEntity, CVector const* pos) :
    m_entity{targetEntity},
    m_pos{pos ? *pos : CVector{}}
{
    assert(m_entity);

    CEntity::SafeRegisterRef(m_entity);
}

// NOTSA
CTaskSimpleThrowControl::CTaskSimpleThrowControl(const CTaskSimpleThrowControl& o) :
    CTaskSimpleThrowControl{o.m_entity, &o.m_pos}
{
}

// 0x61F950
CTaskSimpleThrowControl::~CTaskSimpleThrowControl() {
    CEntity::SafeCleanUpRef(m_entity);
}

// 0x61F9B0
bool CTaskSimpleThrowControl::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
    if (!ped->GetIntelligence()->GetTaskThrow()) {
        return true;
    }
    return ped->GetTaskManager().GetTaskSecondary(TASK_SECONDARY_ATTACK)->MakeAbortable(ped, priority, event);
}

// 0x61F9F0
bool CTaskSimpleThrowControl::ProcessPed(CPed* ped) {
    return plugin::CallMethodAndReturn<bool, 0x61F9F0, CTaskSimpleThrowControl*, CPed*>(this, ped);
}
