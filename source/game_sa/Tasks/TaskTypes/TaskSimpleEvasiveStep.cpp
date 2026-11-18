#include "StdInc.h"

#include "TaskSimpleEvasiveStep.h"

// 0x653160
CTaskSimpleEvasiveStep::CTaskSimpleEvasiveStep(CEntity* entity) :
    m_Entity{ entity } {
}

// 0x6531D0
CTaskSimpleEvasiveStep::~CTaskSimpleEvasiveStep() {
    if (m_Assoc) {
        m_Assoc->SetFinishCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
    }
}

// 0x653290
void CTaskSimpleEvasiveStep::FinishAnimEvasiveStepCB(CAnimBlendAssociation* assoc, void* data) {
    auto* const self = static_cast<CTaskSimpleEvasiveStep*>(data);

    self->m_Assoc->SetFlag(ANIMATION_IS_BLEND_AUTO_REMOVE, true);
    if (self->m_Assoc->m_BlendDelta >= 0.0f) {
        self->m_Assoc->m_BlendDelta = -4.0f;
    }
    self->m_bFinished = true;
    self->m_Assoc     = nullptr;
}

// 0x653240
bool CTaskSimpleEvasiveStep::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    switch (priority) {
    case ABORT_PRIORITY_URGENT:
    case ABORT_PRIORITY_IMMEDIATE:
        if (m_Assoc) {
            m_Assoc->m_BlendDelta = -4.0f;
            m_Assoc->SetFinishCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
            m_Assoc = nullptr;
        }
        return true;
    default:
        return false;
    }
}

// 0x657A60
bool CTaskSimpleEvasiveStep::ProcessPed(CPed* ped) {
    if (m_bFinished) {
        return true;
    }

    if (!m_Entity) {
        return true;
    }

    if (!m_Assoc) {
        if (m_Entity->GetStatus() == STATUS_SIMPLE) {
            ped->Say(CTX_GLOBAL_DODGE);
        }
        StartAnim(ped);
    }
    return false;
}

// 0x655EA0
void CTaskSimpleEvasiveStep::StartAnim(CPed* ped) {
    m_Assoc = CAnimManager::BlendAnimation(ped->GetRpClump(), ANIM_GROUP_DEFAULT, ANIM_ID_EV_STEP, 8.0f);
    m_Assoc->SetFlag(ANIMATION_IS_BLEND_AUTO_REMOVE, false);
    m_Assoc->SetFinishCallback(FinishAnimEvasiveStepCB, this);
}

void CTaskSimpleEvasiveStep::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleEvasiveStep, 0x86f1f4, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedConstructorInstall(0x653160, "1", {}, CEntity*);
    RH_ScopedDestructorInstall(0x6531D0);

    RH_ScopedVMTInstall(Clone, 0x655810);
    RH_ScopedVMTInstall(GetTaskType, 0x6531C0);
    RH_ScopedVMTInstall(MakeAbortable, 0x653240);
    RH_ScopedVMTInstall(ProcessPed, 0x657A60);
}
