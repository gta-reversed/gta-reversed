#include "StdInc.h"

#include "TaskSimpleDie.h"

// 0x62FA00
CTaskSimpleDie::CTaskSimpleDie(AssocGroupId animGroupId, AnimationId animId, float blendDelta, float animSpeed) : CTaskSimple() {
    m_animId          = animId;
    m_animGroupId     = animGroupId;
    m_animSpeed       = animSpeed;
    m_blendDelta      = blendDelta;
    m_animHierarchy   = nullptr;
    m_animAssociation = nullptr;
    m_animFlags       = ANIM_FLAG_DEFAULT;
    m_nFlags          = 0; // here and below should be &= ~3
}

// 0x62FA60
CTaskSimpleDie::CTaskSimpleDie(const char* animName, const char* animBlock, eAnimationFlags animFlags, float blendDelta, float animSpeed) : CTaskSimple() {
    m_animId          = ANIM_ID_KO_SHOT_FRONT_0;
    m_animGroupId     = ANIM_GROUP_DEFAULT;
    m_animSpeed       = animSpeed;
    m_blendDelta      = blendDelta;
    m_animHierarchy   = CAnimManager::GetAnimation(animName, CAnimManager::GetAnimationBlock(animBlock));
    m_animAssociation = nullptr;
    m_animFlags       = animFlags;
    m_nFlags          = 0;
}

// 0x62FAF0
CTaskSimpleDie::CTaskSimpleDie(CAnimBlendHierarchy* animHierarchy, eAnimationFlags animFlags, float blendDelta, float animSpeed) : CTaskSimple() {
    m_animId          = ANIM_ID_KO_SHOT_FRONT_0;
    m_animGroupId     = ANIM_GROUP_DEFAULT;
    m_animSpeed       = animSpeed;
    m_blendDelta      = blendDelta;
    m_animFlags       = animFlags;
    m_animHierarchy   = animHierarchy;
    m_animAssociation = nullptr;
    m_nFlags          = 0;
}

// 0x62FB40
CTaskSimpleDie::~CTaskSimpleDie() {
    if (m_animAssociation)
        m_animAssociation->SetFinishCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
}

// 0x637520
void CTaskSimpleDie::StartAnim(CPed* ped) {
    if (m_animHierarchy)
        m_animAssociation = CAnimManager::BlendAnimation(ped->m_pRwClump, m_animHierarchy, m_animFlags, m_blendDelta);
    else
        m_animAssociation = CAnimManager::BlendAnimation(ped->m_pRwClump, m_animGroupId, m_animId, m_blendDelta);

    m_animAssociation->SetFinishCallback(FinishAnimDieCB, this);
    m_animAssociation->m_nFlags &=   ANIM_FLAG_TRANLSATE_X | ANIM_FLAG_TRANLSATE_Y
                                   | ANIM_FLAG_MOVEMENT
                                   | ANIM_FLAG_PARTIAL
                                   | ANIM_FLAG_FREEZE_LAST_FRAME
                                   | ANIM_FLAG_LOOPED
                                   | ANIM_FLAG_STARTED;

    if (m_animSpeed > 0.0f)
        m_animAssociation->m_fSpeed = m_animSpeed;

    ped->ClearAll();
    ped->m_fHealth = 0.0f;                     // todo: SetHealth or something ?
    ped->StopNonPartialAnims();
    ped->m_nDeathTime = CTimer::GetTimeInMS(); // todo: m_nDeathTime int32 to uint32?
}

// 0x635DA0
CTask* CTaskSimpleDie::Clone() {
    if (m_animHierarchy) {
        return new CTaskSimpleDie(m_animHierarchy, m_animFlags, m_blendDelta, m_animSpeed);
    } else {
        return new CTaskSimpleDie(m_animGroupId, m_animId, m_blendDelta, m_animSpeed);
    }
}

// 0x62FBA0
bool CTaskSimpleDie::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
    if (priority == ABORT_PRIORITY_IMMEDIATE) {
        if (m_animAssociation)
            m_animAssociation->m_fBlendDelta = -1000.0f;

        ped->SetPedState(PEDSTATE_IDLE);
        ped->SetMoveState(PEDMOVE_STILL);
        ped->SetMoveAnim();
        ped->bKeepTasksAfterCleanUp = false;
        return true;
    } else if (event && event->GetEventType() == EVENT_DEATH) {
        ped->bKeepTasksAfterCleanUp = false;
        return true;
    }

    return false;
}

// 0x6397E0
bool CTaskSimpleDie::ProcessPed(CPed* ped) {
    ped->m_pedIK.bSlopePitch = true;

    if (m_dieAnimFinished || m_animId == ANIM_ID_NO_ANIMATION_SET) {
        ped->bKeepTasksAfterCleanUp = true;
        if (!m_alreadyDead) {
            if (
                ped->physicalFlags.bSubmergedInWater
                || !ped->m_bUsesCollision
                || ped->physicalFlags.bDontApplySpeed
                || ped->bIsStanding
                && ( !ped->m_standingOnEntity
                   || ped->IsPlayer()
                   || ped->m_standingOnEntity->AsPhysical()->physicalFlags.bDisableCollisionForce
               )
            ) {
                CEventDeath event(m_animId == ANIM_ID_DROWN);
                ped->GetEventGroup().Add(&event, false);
                m_alreadyDead = true;
                ped->bKeepTasksAfterCleanUp = false;
            }
        }
    } else if (!m_animAssociation) {
        StartAnim(ped);
        ped->SetPedState(PEDSTATE_DIE);
    }

    return false;
}

// 0x62FC10
void CTaskSimpleDie::FinishAnimDieCB(CAnimBlendAssociation* association, void* data) {
    auto task = static_cast<CTaskSimpleDie*>(data);
    task->m_dieAnimFinished = true;
    task->m_animAssociation = nullptr;
}

void CTaskSimpleDie::InjectHooks() {
    using namespace ReversibleHooks;
    Install("CTaskSimpleDie", "CTaskSimpleDie_1", 0x62FA00, static_cast<CTaskSimpleDie*(CTaskSimpleDie::*)(AssocGroupId, AnimationId, float, float)>(&CTaskSimpleDie::Constructor));
    Install("CTaskSimpleDie", "CTaskSimpleDie_2", 0x62FA60, static_cast<CTaskSimpleDie*(CTaskSimpleDie::*)(const char*, const char*, eAnimationFlags, float, float)>(&CTaskSimpleDie::Constructor));
    Install("CTaskSimpleDie", "CTaskSimpleDie_3", 0x62FAF0, static_cast<CTaskSimpleDie*(CTaskSimpleDie::*)(CAnimBlendHierarchy *, eAnimationFlags, float, float)>(&CTaskSimpleDie::Constructor));
    Install("CTaskSimpleDie", "~CTaskSimpleDie", 0x62FB40, &CTaskSimpleDie::Destructor);
    Install("CTaskSimpleDie", "FinishAnimDieCB", 0x62FC10, &CTaskSimpleDie::FinishAnimDieCB);
    Install("CTaskSimpleDie", "StartAnim", 0x637520, &CTaskSimpleDie::StartAnim);
    Install("CTaskSimpleDie", "Clone", 0x635DA0, static_cast< CTask *(CTaskSimpleDie::*)()>(&CTaskSimpleDie::Clone_Reversed));
    Install("CTaskSimpleDie", "GetTaskType", 0x62FA50, &CTaskSimpleDie::GetTaskType_Reversed);
    Install("CTaskSimpleDie", "MakeAbortable", 0x62FBA0, &CTaskSimpleDie::MakeAbortable_Reversed);
    Install("CTaskSimpleDie", "ProcessPed", 0x6397E0, &CTaskSimpleDie::ProcessPed_Reversed);
}

// 0x62FA00
CTaskSimpleDie* CTaskSimpleDie::Constructor(AssocGroupId animGroupId, AnimationId animId, float blendDelta, float animSpeed) {
    this->CTaskSimpleDie::CTaskSimpleDie(animGroupId, animId, blendDelta, animSpeed);
    return this;
}
// 0x62FA60
CTaskSimpleDie* CTaskSimpleDie::Constructor(const char* animName, const char* animBlock, eAnimationFlags animFlags, float blendDelta, float animSpeed) {
    this->CTaskSimpleDie::CTaskSimpleDie(animName, animBlock, animFlags, blendDelta, animSpeed);
    return this;
}

// 0x62FAF0
CTaskSimpleDie* CTaskSimpleDie::Constructor(CAnimBlendHierarchy* animHierarchy, eAnimationFlags animFlags, float blendDelta, float animSpeed) {
    this->CTaskSimpleDie::CTaskSimpleDie(animHierarchy, animFlags, blendDelta, animSpeed);
    return this;
}

CTaskSimpleDie* CTaskSimpleDie::Destructor() {
    CTaskSimpleDie::~CTaskSimpleDie();
    return this;
}

// 0x635DA0
CTask* CTaskSimpleDie::Clone_Reversed() {
    return CTaskSimpleDie::Clone();
}

// 0x62FA50
eTaskType CTaskSimpleDie::GetTaskType_Reversed() {
    return CTaskSimpleDie::GetTaskType();
}

// 0x62FBA0
bool CTaskSimpleDie::MakeAbortable_Reversed(CPed* ped, eAbortPriority priority, CEvent const* event) {
    return CTaskSimpleDie::MakeAbortable(ped, priority, event);
}

// 0x6397E0
bool CTaskSimpleDie::ProcessPed_Reversed(CPed* ped) {
    return CTaskSimpleDie::ProcessPed(ped);
}
