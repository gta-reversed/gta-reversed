#include "StdInc.h"

#include "TaskSimpleStealthKill.h"

void CTaskSimpleStealthKill::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleStealthKill, 0x86da74, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x6225F0);

    RH_ScopedInstall(ManageAnim, 0x6296D0);
    RH_ScopedInstall(FinishAnimStealthKillCB, 0x622790);

    RH_ScopedVMTInstall(ProcessPed, 0x62E540);
    RH_ScopedVMTInstall(Clone, 0x623830);
    RH_ScopedVMTInstall(GetTaskType, 0x622670);
    RH_ScopedVMTInstall(MakeAbortable, 0x6226F0);
}

// 0x6225F0
CTaskSimpleStealthKill::CTaskSimpleStealthKill(bool keepTargetAlive, CPed* target, AssocGroupId groupId) :
    m_bKeepTargetAlive{ keepTargetAlive },
    m_target{ target },
    m_animGrpId{ groupId }
{
    CEntity::SafeRegisterRef(m_target);
}

// NOTSA (For 0x623830)
CTaskSimpleStealthKill::CTaskSimpleStealthKill(const CTaskSimpleStealthKill& o) :
    CTaskSimpleStealthKill{ m_bKeepTargetAlive, m_target, m_animGrpId }
{
}

// 0x62E540
bool CTaskSimpleStealthKill::ProcessPed(CPed* ped) {
    if (m_bIsAborting || !m_target) {
        return true;
    }

    if (!m_anim) {
        if (!m_bKeepTargetAlive) {
            ped->SetPedState(PEDSTATE_DIE_BY_STEALTH);
        }

        ManageAnim(ped);

        if (!m_anim && m_bIsFinished) {
            return false;
        }
    }

    ped->m_pedIK.bSlopePitch = true;

    const auto pedToTarget = m_target->GetPosition() - ped->GetPosition();

    if (!m_bKeepTargetAlive) {
        if (!m_anim || m_anim->m_AnimId == ANIM_ID_KILL_KNIFE_PED_DAMAGE) {
            ped->m_fAimingRotation = -pedToTarget.Heading();
        }
        return false;
    }

    ped->m_fAimingRotation = pedToTarget.Heading();

    if (const auto shift = pedToTarget.Magnitude() - 1.0f; std::abs(shift) > 0.02f) {
        ped->m_vecAnimMovingShiftLocal.y = std::min(CTimer::GetTimeStep() * 0.05f, shift);
        ped->m_vecAnimMovingShiftLocal.x = 0.0f;
    }

    return false;
}

// 0x6226F0
bool CTaskSimpleStealthKill::MakeAbortable(class CPed* ped, eAbortPriority priority, const CEvent* event) {
    switch (priority) {
    case ABORT_PRIORITY_IMMEDIATE: {
        if (m_anim) {
            m_anim->SetDefaultFinishCallback();
            m_anim = nullptr;
        }
        m_bIsAborting = true;
        return true;
    }
    case ABORT_PRIORITY_URGENT: {
        if (m_bKeepTargetAlive) {
            return false;
        }
        if (!m_anim || m_anim->m_AnimId != ANIM_ID_KILL_KNIFE_PED_DIE) {
            return false;
        }
        if (event->GetEventType() != EVENT_DAMAGE || static_cast<const CEventDamage*>(event)->m_pSourceEntity != m_target) {
            return false;
        }
        return true;
    }
    }
    return false;
}

// 0x6296D0
void CTaskSimpleStealthKill::ManageAnim(CPed* ped) {
    CAnimBlock* pAnimBlock = CAnimManager::GetAssocGroups()[m_animGrpId].m_AnimBlock;
    if (!pAnimBlock || !pAnimBlock->IsLoaded) {
        m_spentWaitingMs += (uint32)CTimer::GetTimeStepInMS();
        if (m_spentWaitingMs > 10000) {
            m_bIsAborting = true;
        }
        return;
    }

    m_anim = CAnimManager::BlendAnimation(ped->m_pRwClump, m_animGrpId, [&, this] {
        if (m_bKeepTargetAlive) {
            return ANIM_ID_KILL_KNIFE_PLAYER;
        }

        if (!m_bIsFinished) {
            return ANIM_ID_KILL_KNIFE_PED_DAMAGE;
        }

        CPedDamageResponseCalculator damageCalculator{
            ped,
            CPedDamageResponseCalculator::ms_damageFactor,
            m_target->GetActiveWeapon().m_Type,
            PED_PIECE_TORSO,
            false
        };
        CEventDamage eventDamage{
            m_target,
            CTimer::GetTimeInMS(),
            m_target->GetActiveWeapon().m_Type,
            PED_PIECE_TORSO,
            0,
            false,
            !!ped->bInVehicle
        };
        if (eventDamage.AffectsPed(ped)) {
            damageCalculator.ComputeDamageResponse(ped, eventDamage.m_damageResponse, true);

            eventDamage.m_nAnimGroup = m_animGrpId;
            eventDamage.m_nAnimID = ANIM_ID_KILL_KNIFE_PED_DIE;
            eventDamage.m_fAnimBlend = 8.0f;
            eventDamage.m_fAnimSpeed = 1.0f;
            eventDamage.m_ucDirection |= 4u;
            ped->GetEventGroup().Add(&eventDamage, false);
        }
        return ANIM_ID_KILL_KNIFE_PED_DIE;
    }());
    m_anim->SetFinishCallback(FinishAnimStealthKillCB, this);
    m_bIsFinished = true;
}

// 0x622790
void CTaskSimpleStealthKill::FinishAnimStealthKillCB(CAnimBlendAssociation* pAnimAssoc, void* data) {
    const auto self = static_cast<CTaskSimpleStealthKill*>(data);
    self->m_bIsAborting = notsa::contains<AnimationId>({ ANIM_ID_KILL_KNIFE_PLAYER, ANIM_ID_KILL_KNIFE_PED_DIE }, pAnimAssoc->m_AnimId);
    self->m_anim        = nullptr;
}
