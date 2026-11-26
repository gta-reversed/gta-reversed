#include "StdInc.h"

#include "TaskLeanOnVehicle.h"

// 0x660F90
CTaskLeanOnVehicle::CTaskLeanOnVehicle(CVehicle* vehicle, int32 leanAnimDurationInMs, bool skipLeanIn) :
    m_Vehicle{ vehicle },
    m_LeanAnimDurationInMs{ leanAnimDurationInMs },
    m_SkipLeanIn{ skipLeanIn } {
}

// 0x661030
CTaskLeanOnVehicle::~CTaskLeanOnVehicle() {
    if (m_LeanAnim) {
        m_LeanAnim->SetDefaultDeleteCallback();
    }
}

// 0x661160
void CTaskLeanOnVehicle::FinishAnimCB(CAnimBlendAssociation* assoc, void* data) {
    auto* const self   = notsa::cast<CTaskLeanOnVehicle>(static_cast<CTask*>(data));

    self->m_LastAnimId = assoc->GetAnimId();

    if (assoc->GetAnimId() == ANIM_ID_LEANOUT) {
        assoc->SetBlendDelta(-1000.0f);
        self->m_IsFinished = true;
    }

    if (self->m_IsLeanOutRequested && assoc->GetAnimId() == ANIM_ID_LEANIN) {
        assoc->SetBlendDelta(-1000.0f);
        self->m_IsFinished = true;
    }

    self->m_LeanAnim = nullptr;
}

// 0x661110
bool CTaskLeanOnVehicle::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    if (priority == ABORT_PRIORITY_IMMEDIATE) {
        if (m_LeanAnim) {
            m_LeanAnim->SetBlendDelta(-1000.0f);
            m_LeanAnim->SetDefaultDeleteCallback();
            m_LeanAnim = nullptr;
        }
        m_StopLeaning = true;
        ped->m_pEntityIgnoredCollision = nullptr;
        return true;
    }

    m_IsLeanOutRequested = true;
    return false;
}

// 0x6648C0
bool CTaskLeanOnVehicle::ProcessPed(CPed* ped) {
    if (!m_Vehicle) {
        m_StopLeaning = true;
    }
    ped->m_pEntityIgnoredCollision = m_Vehicle;

    const auto StopLeaning         = [ped] {
        ped->m_pEntityIgnoredCollision = nullptr;
        return true;
    };

    const auto animId = m_LeanAnim
        ? m_LeanAnim->GetAnimId()
        : ANIM_ID_UNDEFINED;

    if (m_StopLeaning) {
        return StopLeaning();
    }

    ped->SetMoveState(PEDMOVE_STILL);

    if (m_IsFinished && !RpAnimBlendClumpGetAssociation(ped->GetRpClump(), ANIM_ID_LEANOUT)) { // 0x6648F8
        return StopLeaning();
    }

    const auto BlendLeanAnim = [&](AnimationId id, float blendDelta) {
        m_LeanAnim = CAnimManager::BlendAnimation(ped->GetRpClump(), ANIM_GROUP_GANGS, id, blendDelta);
        m_LeanAnim->SetFinishCallback(FinishAnimCB, this);
    };

    if (m_IsLeanOutRequested) { // 0x664926
        switch (animId) {
        case ANIM_ID_LEANIN: // 0x664935
            m_LeanAnim->SetBlendDelta(-8.0f);
            break;
        case ANIM_ID_LEANIDLE: // 0x664949
            m_LeanAnim->SetDefaultDeleteCallback();
            BlendLeanAnim(ANIM_ID_LEANOUT, 1000.0f);
            return false;
        case ANIM_ID_LEANOUT: // 0x664975
            m_LeanAnim->SetSpeed(3.0f);
            break;
        }
    }

    if (m_LeanAnim) { // 0x6649D1
        if (m_LeanTimer.IsOutOfTime() && m_LeanAnim->GetAnimId() != ANIM_ID_LEANOUT) {
            m_LeanAnim->SetDefaultDeleteCallback();
            BlendLeanAnim(ANIM_ID_LEANOUT, 1000.0f);
        }
        return false;
    }

    switch (m_LastAnimId) {
    case ANIM_ID_UNDEFINED: { // 0x66498E
        if (!m_SkipLeanIn) {
            BlendLeanAnim(ANIM_ID_LEANIN, 4.0f);
            return false;
        }
        break;
    }
    case ANIM_ID_LEANIN: // 0x6649A8
        break;
    default:
        return false;
    }

    m_LeanTimer.Start(m_LeanAnimDurationInMs);
    BlendLeanAnim(ANIM_ID_LEANIDLE, 1000.0f);
    return false;
}

void CTaskLeanOnVehicle::InjectHooks() {
    RH_ScopedVirtualClass(CTaskLeanOnVehicle, 0x86FAF4, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedConstructorInstall(0x660F90, "", {}, CVehicle*, int32, bool);
    RH_ScopedDestructorInstall(0x661030);

    RH_ScopedInstall(FinishAnimCB, 0x661160);

    RH_ScopedVMTInstall(MakeAbortable, 0x661110);
    RH_ScopedVMTInstall(ProcessPed, 0x6648C0);
}
