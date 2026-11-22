#include "StdInc.h"
#include "TaskSimpleBeKickedOnGround.h"

// 0x61FC50
CTaskSimpleBeKickedOnGround::~CTaskSimpleBeKickedOnGround() {
    if (m_Anim) {
        m_Anim->SetBlendDelta(-4.f);
        m_Anim->SetDefaultFinishCallback();
    }
}

// 0x61FD10
void CTaskSimpleBeKickedOnGround::FinishAnimBeKickedOnGroundCB(CAnimBlendAssociation* anim, void* data) {
    const auto self = notsa::cast<CTaskSimpleBeKickedOnGround>(data);
    self->m_Anim = nullptr;
    self->m_IsFinished = true;
}

// 0x61FD20
void CTaskSimpleBeKickedOnGround::StartAnim(CPed* ped) {
    if (CLocalisation::KickingWhenDown()) {
        m_Anim = CAnimManager::BlendAnimation(
            ped->GetRpClump(),
            ANIM_GROUP_DEFAULT,
            RpAnimBlendClumpGetFirstAssociation(ped->GetRpClump(), ANIMATION_IS_FRONT) ? ANIM_ID_FLOOR_HIT_F : ANIM_ID_FLOOR_HIT,
            8.f
        );
        m_Anim->Start();
        m_Anim->SetFlag(ANIMATION_IS_PLAYING);
        m_Anim->SetFlag(ANIMATION_IS_FINISH_AUTO_REMOVE, false);
        

        /*
        * Code below is dead,it gets a bone's position, and does nothing with it.
        if (CLocalisation::KickingWhenDown()) { // TODO: Inlined?
        // Dead code 
        }
        */

        m_Anim->SetFinishCallback(FinishAnimBeKickedOnGroundCB, this);
    } else {
        m_IsFinished = true;
    }
}

// 0x61FCC0
bool CTaskSimpleBeKickedOnGround::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
    switch (priority) {
    case ABORT_PRIORITY_URGENT:
    case ABORT_PRIORITY_IMMEDIATE: {
        if (m_Anim) {
            m_Anim->SetFinishCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
            m_Anim = nullptr;
        }
        return true;
    }
    case ABORT_PRIORITY_LEISURE: {
        if (m_Anim) {
            m_Anim->SetFlag(ANIMATION_IS_FINISH_AUTO_REMOVE);
            m_Anim->SetBlendDelta(-4.f);
        }
        return false;
    }
    default:
        NOTSA_UNREACHABLE();
    }
}

// 0x61FE00
bool CTaskSimpleBeKickedOnGround::ProcessPed(CPed* ped) {
    if (!m_IsFinished) {
        return true;
    }
    if (!m_Anim) {
        StartAnim(ped);
    }
    return false;
}

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
