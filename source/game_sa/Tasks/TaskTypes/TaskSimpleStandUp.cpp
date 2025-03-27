#include "StdInc.h"
#include "TaskSimpleStandUp.h"

void CTaskSimpleStandUp::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleStandUp, 0x86df3c, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x6312E0);
    RH_ScopedInstall(Destructor, 0x631320);

    RH_ScopedInstall(FinishAnimStandUpCB, 0x631400);

    RH_ScopedInstall(StartAnim, 0x631410);

    RH_ScopedVMTInstall(Clone, 0x636310);
    RH_ScopedVMTInstall(GetTaskType, 0x631310);
    RH_ScopedVMTInstall(MakeAbortable, 0x631380);
    RH_ScopedVMTInstall(ProcessPed, 0x637D60);
}

// 0x6312E0
CTaskSimpleStandUp::CTaskSimpleStandUp(bool sitAfterStep) :
    m_sitAfterStep{sitAfterStep}
{
}

CTaskSimpleStandUp::CTaskSimpleStandUp(const CTaskSimpleStandUp& o) :
    CTaskSimpleStandUp{o.m_sitAfterStep}
{
}

// 0x631320
CTaskSimpleStandUp::~CTaskSimpleStandUp() {
    if (m_anim) {
        m_anim->SetDefaultFinishCallback();
    }
}

// 0x631400
void CTaskSimpleStandUp::FinishAnimStandUpCB(CAnimBlendAssociation* anim, void* data) {
    const auto self = static_cast<CTaskSimpleStandUp*>(data);
    self->m_hasAnimFinished = true;
    self->m_anim = nullptr;
}

// 0x631410
void CTaskSimpleStandUp::StartAnim(CPed* ped) {
    const auto SetAnim = [this, ped](auto grpId, auto animId) { m_anim = CAnimManager::BlendAnimation(ped->m_pRwClump, grpId, animId, 4.f); };
    if (m_sitAfterStep) {
        SetAnim(ANIM_GROUP_ATTRACTORS, ANIM_ID_STEPSIT_OUT);
    } else {
        SetAnim(ANIM_GROUP_DEFAULT, ANIM_ID_SEAT_UP);
    }
    m_anim->SetFinishCallback(FinishAnimStandUpCB, this);
}

// 0x631380
bool CTaskSimpleStandUp::MakeAbortable(CPed*, eAbortPriority priority, CEvent const*) {
    switch (priority) {
    case ABORT_PRIORITY_IMMEDIATE: {
        if (m_anim) {
            m_anim->m_BlendDelta = -1000.f;
            m_anim->SetDefaultFinishCallback();
            m_anim = nullptr;
        }
        return true;
    }
    case ABORT_PRIORITY_URGENT: {
        if (m_anim) {
            m_anim->SetDefaultFinishCallback();
            m_anim = nullptr;
        }
        return true;
    }
    case ABORT_PRIORITY_LEISURE: {
        if (m_anim) {
            m_anim->m_Flags |= ANIMATION_IS_BLEND_AUTO_REMOVE;
            m_anim->m_BlendDelta = -4.f;
        }
        return false;
    }
    default:
        NOTSA_UNREACHABLE();
    }
}

// 0x637D60
bool CTaskSimpleStandUp::ProcessPed(CPed* ped) {
    if (m_hasAnimFinished) {
        return true;
    }
    if (!m_anim) {
        StartAnim(ped);
    }
    return false;
}
