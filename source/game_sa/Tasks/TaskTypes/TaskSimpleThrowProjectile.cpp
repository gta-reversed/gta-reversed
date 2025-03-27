#include "StdInc.h"

#include "TaskSimpleThrowProjectile.h"

void CTaskSimpleThrowProjectile::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleThrowProjectile, 0x86d790, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x61F660);
    RH_ScopedInstall(Destructor, 0x61F700);

    RH_ScopedInstall(ControlThrow, 0x61F810);
    RH_ScopedInstall(StartAnim, 0x6259E0);
    RH_ScopedInstall(FinishAnimThrowProjectileCB, 0x61F890);

    RH_ScopedVMTInstall(Clone, 0x623030);
    RH_ScopedVMTInstall(GetTaskType, 0x61F6F0);
    RH_ScopedVMTInstall(MakeAbortable, 0x61F780);
    RH_ScopedVMTInstall(ProcessPed, 0x62AF50);
}

// 0x61F660
CTaskSimpleThrowProjectile::CTaskSimpleThrowProjectile(CEntity* targetEntity, CVector targetPos) :
    m_TargetPos{ targetPos },
    m_TargetEntity{ targetEntity },
    m_ButtonCounter{ CTimer::GetTimeInMS() }
{
    CEntity::SafeRegisterRef(m_TargetEntity);
}

// 0x623030
CTaskSimpleThrowProjectile::CTaskSimpleThrowProjectile(const CTaskSimpleThrowProjectile& o) :
    CTaskSimpleThrowProjectile{ o.m_TargetEntity, o.m_TargetPos }
{
}

// 0x61F700
CTaskSimpleThrowProjectile::~CTaskSimpleThrowProjectile() {
    if (m_Anim) {
        m_Anim->SetDefaultFinishCallback();
        m_Anim = nullptr;
    }
    CEntity::SafeCleanUpRef(m_TargetEntity);
}

// 0x61F810
bool CTaskSimpleThrowProjectile::ControlThrow(bool bButtonReleased, CEntity* targetEntity, const CVector* targetPos) {
    if (m_bIsFinished) {
        return true;
    }

    if (bButtonReleased && !m_bButtonReleased) {
        m_ButtonCounter = CTimer::GetTimeInMS() - m_ButtonCounter;
        m_bButtonReleased = true;
    }

    if (targetEntity && m_TargetEntity != targetEntity) {
        CEntity::ChangeEntityReference(m_TargetEntity, targetEntity);
    }

    if (targetPos) {
        m_TargetPos = *targetPos;
    }

    return false;
}

// 0x61F890
void CTaskSimpleThrowProjectile::FinishAnimThrowProjectileCB(CAnimBlendAssociation* anim, void* data) {
    const auto self = static_cast<CTaskSimpleThrowProjectile*>(data);

    self->m_Anim = nullptr;
    if (self->m_bStartThrowFinished) {
        self->m_bIsFinished = true;
    } else {
        self->m_bStartThrowFinished = true;
    }
}

// 0x6259E0
void CTaskSimpleThrowProjectile::StartAnim(CPed* ped) {
    const auto StartAnim = [&, this](AnimationId animId, float blendDelta) {
        VERIFY(m_Anim = CAnimManager::BlendAnimation(ped->m_pRwClump, ped->GetActiveWeapon().GetWeaponInfo().m_eAnimGroup, animId, blendDelta));
        m_Anim->SetFinishCallback(FinishAnimThrowProjectileCB, this);
    };

    if (!m_bStartThrowFinished) {
        StartAnim(ANIM_ID_GRENADE_WEAPON_START_THROW, 16.f);
    } else {
        const auto IsPtCloseToPed = [ped](CVector pos) { return (pos - ped->GetPosition()).SquaredMagnitude() <= sq(10.f); };
        if (   (!m_TargetEntity || IsPtCloseToPed(m_TargetEntity->GetPosition()))
            || (m_TargetPos.IsZero() || IsPtCloseToPed(m_TargetPos))
            || (m_bButtonReleased || ped->GetActiveWeapon().GetType() == WEAPON_REMOTE_SATCHEL_CHARGE)
        ) {
            StartAnim(ANIM_ID_GRENADE_WEAPON_THROW, 1000.f);
        } else {
            StartAnim(ANIM_ID_GRENADE_WEAPON_THROWU, 1000.f); // "U" as in "UP?
        }
    }
}

// 0x61F780
bool CTaskSimpleThrowProjectile::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    if (priority == ABORT_PRIORITY_IMMEDIATE) {
        if (m_Anim) {
            m_Anim->SetDefaultFinishCallback();
            m_Anim->m_BlendDelta = -1000.0f;
            m_Anim                = nullptr;
        }
    } else if (const auto eDmg = notsa::dyn_cast_if_present<const CEventDamage>(event)) {
        if (eDmg->m_damageResponse.m_bHealthZero && eDmg->m_bAddToEventGroup || eDmg->m_bFallDown) {
            if (m_Anim) {
                m_Anim->SetDefaultFinishCallback();
                m_Anim->m_BlendDelta = -4.0f;
                m_Anim                = nullptr;
            }
            m_bIsFinished = true;
        }
    }
    return true;
}

// 0x62AF50
bool CTaskSimpleThrowProjectile::ProcessPed(CPed* ped) {
    if (m_bIsFinished) {
        return true;
    }

    const auto wi = &ped->GetActiveWeapon().GetWeaponInfo(ped);

    if (!wi->flags.bThrow) {
        return true;
    }

    if (!m_Anim) {
        StartAnim(ped);
        return false;
    }

    switch (m_Anim->m_AnimId) {
    case ANIM_ID_GRENADE_WEAPON_THROW:
    case ANIM_ID_GRENADE_WEAPON_THROWU:
        const auto animFireTime = m_Anim->m_AnimId == ANIM_ID_GRENADE_WEAPON_THROW
            ? wi->m_fAnimLoop2Fire
            : wi->m_fAnimLoopFire;
        if (animFireTime < m_Anim->m_CurrentTime && (m_Anim->m_CurrentTime - m_Anim->m_TimeStep) <= animFireTime && m_Anim->IsPlaying()) {
            if (ped->IsPlayer()) {
                if (!m_bButtonReleased) {
                    m_ButtonCounter = CTimer::GetTimeInMS() - m_ButtonCounter;
                }
                m_ButtonCounter = std::min(m_ButtonCounter, 533u);
                ped->m_pPlayerData->m_fAttackButtonCounter = (float)m_ButtonCounter / 20.f;
            }

            if (m_TargetEntity) {
                m_TargetPos = m_TargetEntity->GetPosition();
            }

            CVector firePos;
            RwV3dTransformPoint(&firePos, &wi->m_vecFireOffset, ped->GetBoneMatrix((eBoneTag)ped->m_apBones[PED_NODE_RIGHT_HAND]->BoneTag));
            ped->GetActiveWeapon().Fire(ped, &firePos, &firePos, nullptr, m_TargetPos.IsZero() ? nullptr : &m_TargetPos, nullptr);
        }
    }

    return false;
}
