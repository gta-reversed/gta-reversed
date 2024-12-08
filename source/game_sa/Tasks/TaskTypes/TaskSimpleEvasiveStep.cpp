#include "StdInc.h"

#include "TaskSimpleEvasiveStep.h"

// 0x653160
CTaskSimpleEvasiveStep::CTaskSimpleEvasiveStep(CEntity* entity) : CTaskSimple() {
    m_Entity = entity;
    m_bFinished = false;
    m_Assoc = nullptr;
    CEntity::SafeRegisterRef(m_Entity);
}

// 0x6531D0
CTaskSimpleEvasiveStep::~CTaskSimpleEvasiveStep() {
    if (m_Assoc) {
        m_Assoc->SetFinishCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
    }
    CEntity::SafeCleanUpRef(m_Entity);
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
    if (m_bFinished)
        return true;

    if (!m_Entity)
        return true;

    if (!m_Assoc) {
        if (m_Entity->m_nStatus == STATUS_SIMPLE) {
            ped->Say(CTX_GLOBAL_DODGE);
        }
        StartAnim(ped);
    }
    return false;
}

// 0x655EA0
void CTaskSimpleEvasiveStep::StartAnim(CPed* ped) {
    m_Assoc = CAnimManager::BlendAnimation(ped->m_pRwClump, ANIM_GROUP_DEFAULT, ANIM_ID_EV_STEP, 8.0f);
    m_Assoc->SetFlag(ANIMATION_IS_BLEND_AUTO_REMOVE, false);
    m_Assoc->SetFinishCallback(FinishAnimEvasiveStepCB, this);
}

// 0x653290
void CTaskSimpleEvasiveStep::FinishAnimEvasiveStepCB(CAnimBlendAssociation* assoc, void* data) {
    auto task = static_cast<CTaskSimpleEvasiveStep*>(data);
    task->m_Assoc->SetFlag(ANIMATION_IS_BLEND_AUTO_REMOVE, true);
    if (task->m_Assoc->m_BlendDelta >= 0.0f) {
        task->m_Assoc->m_BlendDelta = -4.0f;
    }
    task->m_bFinished = true;
    task->m_Assoc = nullptr;
}
