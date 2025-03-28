#include "StdInc.h"

#include "TaskSimpleBeHit.h"
#include "TaskSimpleFight.h"

void CTaskSimpleBeHit::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleBeHit, 0x86D844, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x620780);
    RH_ScopedInstall(Destructor, 0x620810);
    RH_ScopedGlobalInstall(FinishAnimBeHitCB, 0x620900);
    RH_ScopedInstall(StartAnim, 0x620910);
    RH_ScopedVMTInstall(Clone, 0x623290);
    RH_ScopedVMTInstall(GetTaskType, 0x620800);
    RH_ScopedVMTInstall(MakeAbortable, 0x620890);
    RH_ScopedVMTInstall(ProcessPed, 0x620A20);
}

/*!
* @addr 0x620780
* @param [opt] attacker  The ped that hit us
* @param       pieceType The piece we got hit at
* @param       direction
* @param       dmgHealth The amount of damage to our health
*/
CTaskSimpleBeHit::CTaskSimpleBeHit(CPed* attacker, ePedPieceTypes pieceType, int32 direction, int32 dmgHealth) :
      CTaskSimple(),
      m_eHitZone{ pieceType },
      m_nHitPower{ dmgHealth },
      m_Attacker{ attacker },
      m_bIsFinished{ false },
      m_bAnimAdded{ false },
      m_nAnimId{ ANIM_ID_NO_ANIMATION_SET },
      m_nAnimGroup{ ANIM_GROUP_DEFAULT },
      m_nDirn{ direction },
      m_Anim{ nullptr }
{
    CEntity::SafeRegisterRef(attacker);
}

// 0x620810
CTaskSimpleBeHit::~CTaskSimpleBeHit() {
    if (m_Anim) {
        m_Anim->m_BlendDelta = -4.0f;
        m_Anim->SetFinishCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
    }
    CEntity::SafeCleanUpRef(m_Attacker);
}

// 0x623290
CTask* CTaskSimpleBeHit::Clone() const {
    auto task = new CTaskSimpleBeHit(m_Attacker, m_eHitZone, m_nDirn, m_nHitPower);
    task->m_bAnimAdded = m_bAnimAdded;
    task->m_nAnimId = m_nAnimId;
    task->m_nAnimGroup = m_nAnimGroup;
    return task;
}

/*!
* @addr 0x620900
* @brief The finish callback of `m_anim`
*/
void CTaskSimpleBeHit::FinishAnimBeHitCB(CAnimBlendAssociation* anim, void* data) {
    const auto self = static_cast<CTaskSimpleBeHit*>(data);
    self->m_Anim = nullptr;
    self->m_bIsFinished = true;
}

/*!
* @addr 0x620910
* @brief Start the animation / handle check if it's still running. If not `m_finishedOrAborted` is set.
*
* @param ped The ped to who this task belongs to
*/
void CTaskSimpleBeHit::StartAnim(CPed* ped) {
    // If no animId is set yet, calculate it
    if (m_nAnimId == ANIM_ID_NO_ANIMATION_SET) {
        std::tie(m_nAnimGroup, m_nAnimId) = [this]() -> std::tuple<AssocGroupId, AnimationId> {
            if (m_nDirn == 0 && m_Attacker) {
                const auto fighting = m_Attacker->GetIntelligence()->GetTaskFighting();
                if (   fighting
                    && fighting->GetCurrentComboSet() >= eMeleeCombo::UNARMED_1
                    && fighting->GetMove() <= eMeleeMove::ATTACK3
                ) {
                    return { fighting->GetComboAnimGroupID(), fighting->GetMoveAnimID() };
                }
            }
            return {
                m_nAnimGroup,
                [this] {
                    if (m_eHitZone == PED_PIECE_TORSO) {
                         return ANIM_ID_HIT_FRONT;
                    }
                    switch (m_nDirn) {
                    case 1:  return ANIM_ID_HIT_L;
                    case 2:  return ANIM_ID_HIT_BACK;
                    case 3:  return ANIM_ID_HIT_R;
                    default: return m_nDirn <= 5
                        ? ANIM_ID_HIT_FRONT
                        : ANIM_ID_HIT_WALK;
                    }
                }()
            };
        }();
    }

    // Check if anim exists/create it
    if (m_bAnimAdded) { // Inverted
        m_Anim = RpAnimBlendClumpGetAssociation(ped->m_pRwClump, m_nAnimId);
        if (!m_Anim) {
            m_bIsFinished = true;
            return;
        }
    } else {
        m_Anim = CAnimManager::BlendAnimation(ped->m_pRwClump, m_nAnimGroup, m_nAnimId, 8.f);
        m_Anim->SetCurrentTime(0.f);
        m_bAnimAdded = true;
    }
    m_Anim->SetFinishCallback(FinishAnimBeHitCB, this);
}

/*!
* @addr 0x620890
* @brief Try aborting the task
*
* @param       ped      The ped to who this task belongs to
* @param       priority The abort priority
* @param [opt] event    The event that caused the abortion
*
* @return If it was aborted
*/
bool CTaskSimpleBeHit::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    switch (priority) {
    case ABORT_PRIORITY_LEISURE: {
        if (m_Anim) {
            if ((m_Anim->m_Flags & ANIMATION_IS_PLAYING) == 0) {
                m_Anim->m_Flags |= ANIMATION_IS_BLEND_AUTO_REMOVE;
                m_Anim->m_BlendDelta = -4.f;
            }
        }
        return false;
    }
    case ABORT_PRIORITY_URGENT:
    case ABORT_PRIORITY_IMMEDIATE: {
        if (event && event->GetEventPriority() < 62) {
            return false;
        }

        if (m_Anim) {
            m_Anim->SetFinishCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
            m_Anim = nullptr;
        }

        m_bIsFinished = true;

        return true;
    }
    default:
        NOTSA_UNREACHABLE();
        return false;
    }
}

/*!
* @addr 0x620A20
* @brief Process the ped
*
* @param ped The ped to who this task belongs to
*/
bool CTaskSimpleBeHit::ProcessPed(CPed* ped) {
    if (m_bIsFinished) {
        return true;
    }

    if (m_nDirn == 0 && m_Attacker && !m_Attacker->IsPlayer()) { // todo: m_nDirn == 0
        ped->m_fAimingRotation = (m_Attacker->GetPosition() - ped->GetPosition()).Heading();
    }

    if (m_Anim) {
        return false;
    }

    if (   !m_Attacker
        || !m_Attacker->IsPlayer()
        || m_Attacker->m_nPedType != PED_TYPE_GANG2
        || (ped->DisablePedSpeech(true), ped->EnablePedSpeech(), ped->Say(CTX_GLOBAL_ATTACK_BY_PLAYER_LIKE, 1000) < 0)
    ) {
        ped->Say(CTX_GLOBAL_PAIN_LOW);
    }

    StartAnim(ped);
    return false;
}
