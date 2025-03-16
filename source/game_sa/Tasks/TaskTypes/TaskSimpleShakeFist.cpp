#include "StdInc.h"
#include "TaskSimpleShakeFist.h"

void CTaskSimpleShakeFist::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleShakeFist, 0x870a94, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x690B80);

    RH_ScopedInstall(Destructor, 0x690BB0);

    RH_ScopedGlobalInstall(FinishAnimShakeFistCB, 0x690C60);

    RH_ScopedInstall(StartAnim, 0x692DF0);

    RH_ScopedVMTInstall(Clone, 0x6927B0);
    RH_ScopedVMTInstall(GetTaskType, 0x690BA0);
    RH_ScopedVMTInstall(MakeAbortable, 0x690C10);
    RH_ScopedVMTInstall(ProcessPed, 0x693BA0);
}

CTaskSimpleShakeFist::CTaskSimpleShakeFist(const CTaskSimpleShakeFist&) :
    CTaskSimpleShakeFist{}
{
}

// 0x690BB0
CTaskSimpleShakeFist::~CTaskSimpleShakeFist() {
    if (m_anim) {
        m_anim->SetDefaultFinishCallback();
    }
}

// 0x690C60
void CTaskSimpleShakeFist::FinishAnimShakeFistCB(CAnimBlendAssociation*, void* data) {
    const auto self = static_cast<CTaskSimpleShakeFist*>(data);
    self->m_anim = nullptr;
    self->m_hasAnimFinished = true;
}

// 0x692DF0
void CTaskSimpleShakeFist::StartAnim(CPed* ped) {
    m_anim = CAnimManager::BlendAnimation(ped->m_pRwClump, ANIM_GROUP_DEFAULT, ANIM_ID_FUCKU, 4.f);
    m_anim->SetFlag(ANIMATION_IS_FINISH_AUTO_REMOVE);
    m_anim->SetFlag(ANIMATION_IS_BLEND_AUTO_REMOVE);
    m_anim->SetDeleteCallback(FinishAnimShakeFistCB, this);
}

// 0x690C10
bool CTaskSimpleShakeFist::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
    switch (priority) {
    case ABORT_PRIORITY_URGENT:
    case ABORT_PRIORITY_IMMEDIATE: {
        if (m_anim) {
            m_anim->m_BlendDelta = -4.f;
            m_anim->SetDefaultFinishCallback();
            m_anim = nullptr;
        }
        return true;
    }
    case ABORT_PRIORITY_LEISURE: {
        if (m_anim) {
            m_anim->m_BlendDelta = -4.f;
        }
        return false;
    }
    default:
        NOTSA_UNREACHABLE();
    }
}

// 0x693BA0
bool CTaskSimpleShakeFist::ProcessPed(CPed* ped) {
    if (m_hasAnimFinished) {
        return true;
    }

    if (!m_anim) {
        StartAnim(ped);
    }

    return false;
}
