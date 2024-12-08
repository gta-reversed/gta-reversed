#include "StdInc.h"
#include "TaskSimpleSitDown.h"

void CTaskSimpleSitDown::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleSitDown, 0x86def4, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x631030);
    RH_ScopedInstall(Destructor, 0x631070);

    RH_ScopedInstall(FinishAnimSitDownCB, 0x631150);
    RH_ScopedInstall(StartAnim, 0x6379F0);

    RH_ScopedVMTInstall(Clone, 0x636200);
    RH_ScopedVMTInstall(GetTaskType, 0x631060);
    RH_ScopedVMTInstall(MakeAbortable, 0x6310D0);
    RH_ScopedVMTInstall(ProcessPed, 0x639960);
}

// 0x631030
CTaskSimpleSitDown::CTaskSimpleSitDown(bool sitOnStep) :
    m_sitOnStep{ sitOnStep }
{
}

CTaskSimpleSitDown::CTaskSimpleSitDown(const CTaskSimpleSitDown& o) :
    CTaskSimpleSitDown{o.m_sitOnStep}
{
}

// 0x631070
CTaskSimpleSitDown::~CTaskSimpleSitDown() {
    if (m_anim) {
        m_anim->SetDefaultFinishCallback();
    }
}

// 0x631150
void CTaskSimpleSitDown::FinishAnimSitDownCB(CAnimBlendAssociation* anim, void* data) {
    const auto self = static_cast<CTaskSimpleSitDown*>(data);
    assert(!self->m_anim || self->m_anim == anim);
    self->m_animFinished = true;
    self->m_anim = nullptr;
}

// 0x6379F0
void CTaskSimpleSitDown::StartAnim(CPed* ped) {
    assert(!m_anim);

    const auto SetAnim = [this, ped](auto grpId, auto animId) {
        m_anim = CAnimManager::BlendAnimation(ped->m_pRwClump, grpId, animId, 4.f);
    };
    if (m_sitOnStep) {
        SetAnim(ANIM_GROUP_ATTRACTORS, ANIM_ID_STEPSIT_IN);
    } else {
        SetAnim(ANIM_GROUP_DEFAULT, ANIM_ID_SEAT_DOWN);
    }
    m_anim->SetFinishCallback(FinishAnimSitDownCB, this);
}

// 0x6310D0
bool CTaskSimpleSitDown::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
    switch (priority) {
    case ABORT_PRIORITY_IMMEDIATE: {
        if (m_anim) {
            m_anim->SetDefaultFinishCallback();
            m_anim->m_BlendDelta = -1000.f;
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
            m_anim->SetFlag(ANIMATION_IS_BLEND_AUTO_REMOVE, true);
            m_anim->m_BlendDelta = -4.f;
        }
        return false;
    }
    default:
        NOTSA_UNREACHABLE();
    }
}

// 0x639960
bool CTaskSimpleSitDown::ProcessPed(CPed* ped) {
    if (m_animFinished) {
        return true;
    }
    if (!m_anim) {
        StartAnim(ped);
    }
    return false;
}
