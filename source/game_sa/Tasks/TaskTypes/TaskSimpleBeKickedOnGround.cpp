#include "StdInc.h"
#include "TaskSimpleBeKickedOnGround.h"

void CTaskSimpleBeKickedOnGround::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleBeKickedOnGround, 0x86d7d8, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedConstructorInstall(0x61FC20, "1", {});
    RH_ScopedDestructorInstall(0x61FC50);

    RH_ScopedGlobalInstall(FinishAnimBeKickedOnGroundCB, 0x61FD10);
    RH_ScopedInstall(StartAnim, 0x61FD20);
    RH_ScopedVMTInstall(Clone, 0x623120);
    RH_ScopedVMTInstall(GetTaskType, 0x61FC40);
    RH_ScopedVMTInstall(MakeAbortable, 0x61FCC0);
    RH_ScopedVMTInstall(ProcessPed, 0x61FE00);
}

// 0x61FD10
void CTaskSimpleBeKickedOnGround::FinishAnimBeKickedOnGroundCB(CAnimBlendAssociation* anim, void* data) {
    auto* const self   = static_cast<CTaskSimpleBeKickedOnGround*>(data);
    self->m_Anim       = nullptr;
    self->m_IsFinished = true;
}

// 0x61FD20
void CTaskSimpleBeKickedOnGround::StartAnim(CPed* ped) {
    if (!CLocalisation::KickingWhenDown()) {
        m_IsFinished = true;
        return;
    }

    const auto onFront = RpAnimBlendClumpGetFirstAssociation(ped->GetRpClump(), ANIMATION_IS_FRONT) != nullptr;
    m_Anim             = CAnimManager::BlendAnimation(ped->GetRpClump(), ANIM_GROUP_DEFAULT, onFront ? ANIM_ID_FLOOR_HIT_F : ANIM_ID_FLOOR_HIT, 8.f);
    m_Anim->SetCurrentTime(0.f);
    m_Anim->m_Flags |= ANIMATION_IS_PLAYING;
    m_Anim->m_Flags &= ~ANIMATION_IS_FINISH_AUTO_REMOVE;

    // NOTE: Here the original transforms a bone position (if `KickingWhenDown()`) and then never uses it (cut blood effect?), skipped
    m_Anim->SetFinishCallback(FinishAnimBeKickedOnGroundCB, this);
}

// 0x61FCC0
bool CTaskSimpleBeKickedOnGround::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    switch (priority) {
    case ABORT_PRIORITY_URGENT:
    case ABORT_PRIORITY_IMMEDIATE: {
        if (m_Anim) {
            m_Anim->SetDefaultFinishCallback();
            m_Anim = nullptr;
        }
        return true;
    }
    default: {
        if (m_Anim) {
            m_Anim->m_Flags |= ANIMATION_IS_BLEND_AUTO_REMOVE;
            m_Anim->m_BlendDelta = -4.f;
        }
        return false;
    }
    }
}

// 0x61FE00
bool CTaskSimpleBeKickedOnGround::ProcessPed(CPed* ped) {
    if (m_IsFinished) {
        return true;
    }
    if (!m_Anim) {
        StartAnim(ped);
    }
    return false;
}
