#include "StdInc.h"

#include "TaskSimpleChoking.h"

// 0x6202C0
CTaskSimpleChoking::CTaskSimpleChoking(CPed* attacker, bool bIsTeargas) :
    m_pAttacker{attacker},
    m_bIsTeargas{bIsTeargas}
{
    CEntity::SafeRegisterRef(m_pAttacker);
    m_pAnim          = nullptr;
    m_bIsFinished    = false;
    m_nTimeRemaining = CGeneral::GetRandomNumberInRange(0u, 1000u);
    m_nTimeStarted   = CTimer::GetTimeInMS();
}

// 0x623220
CTaskSimpleChoking::CTaskSimpleChoking(const CTaskSimpleChoking& o) :
    CTaskSimpleChoking{o.m_pAttacker, o.m_bIsTeargas}
{
}

// 0x6203F0
bool CTaskSimpleChoking::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
    if (priority == ABORT_PRIORITY_LEISURE || priority == ABORT_PRIORITY_URGENT) {
        if (event && event->GetEventPriority() < EVENT_LEADER_QUIT_ENTERING_CAR_AS_DRIVER) {
            return false;
        }
        if (m_pAnim) {
            m_pAnim->m_BlendDelta = -4.0f;
            m_pAnim->SetDefaultFinishCallback();
            m_pAnim = nullptr;
        }
        m_bIsFinished = true;
    } else if (m_pAnim) {
        m_pAnim->m_Flags |= ANIMATION_IS_BLEND_AUTO_REMOVE;
        m_pAnim->m_BlendDelta = -4.0f;
        m_pAnim->SetDefaultFinishCallback();
        m_pAnim = nullptr;
        return true;
    }
    return true;
}

// 0x620490
static void FinishAnimChokingCB(CAnimBlendAssociation* anim, void* data) {
    const auto self = static_cast<CTaskSimpleChoking*>(data);
    self->m_pAnim = nullptr;
    self->m_bIsFinished = true;
}

bool CTaskSimpleChoking::ProcessPed(CPed* ped) {
    if (m_bIsFinished) {
        return true;
    }
    if (m_pAttacker && !m_pAttacker->IsPlayer() && !m_bIsTeargas) {
        const CVector& victimPos   = ped->GetMatrix().GetPosition();
        const CVector& attackerPos = m_pAttacker->GetMatrix().GetPosition();
        const CVector  delta       = attackerPos - victimPos;
        const CVector& moveSpeed   = m_pAttacker->GetMoveSpeed();
        if (delta.x * moveSpeed.x + delta.y * moveSpeed.y + delta.z * moveSpeed.z > 0.0f) {
            ped->m_fCurrentRotation = CGeneral::GetATanOfXY(-delta.x, delta.y);
        }
    }
    const uint32 timeStep = (uint32)CTimer::GetTimeStepInMS();
    if (timeStep < m_nTimeRemaining) {
        m_nTimeRemaining -= timeStep;
    } else {
        m_nTimeRemaining = 0;
    }
    if (!m_pAnim) {
        m_pAnim = CAnimManager::BlendAnimation(ped->GetRpClump(), ANIM_GROUP_DEFAULT, ANIM_ID_GAS_CWR, 4.0f);
        m_pAnim->SetFinishCallback(FinishAnimChokingCB, this);
        m_pAnim->SetSpeed(CGeneral::GetRandomNumberInRange(0.75f, 1.0f));
    } else if (!m_nTimeRemaining) {
        if (m_pAnim->m_AnimId == ANIM_ID_GAS_CWR) {
            m_pAnim->SetDefaultFinishCallback();
            m_pAnim = CAnimManager::BlendAnimation(ped->GetRpClump(), ANIM_GROUP_DEFAULT, ANIM_ID_KO_SKID_BACK, 4.0f);
            m_pAnim->SetFinishCallback(FinishAnimChokingCB, this);
            const uint32 elapsed = CTimer::GetTimeInMS() - m_nTimeStarted;
            const uint32 timeout = (uint32)CGeneral::GetRandomNumberInRange(8'000, 12'000);
            m_nTimeRemaining = std::min(elapsed, timeout);
        } else {
            m_pAnim->m_BlendDelta = -4.0f;
            m_pAnim->m_Flags |= ANIMATION_IS_BLEND_AUTO_REMOVE;
        }
    }
    ped->Say(CTX_GLOBAL_PAIN_COUGH);
    return false;
}

// 0x620660
void CTaskSimpleChoking::UpdateChoke(CPed* victim, CPed* attacker, bool bIsTeargas) {
    m_bIsTeargas = bIsTeargas;
    if (m_pAttacker != attacker) {
        CEntity::ChangeEntityReference(m_pAttacker, attacker);
    }
    m_bIsFinished = false;
    if (m_pAnim && m_pAnim->m_AnimId != ANIM_ID_GAS_CWR) {
        m_pAnim->SetDefaultFinishCallback();
        m_pAnim = CAnimManager::BlendAnimation(victim->GetRpClump(), ANIM_GROUP_DEFAULT, ANIM_ID_GAS_CWR, 4.f);
        m_pAnim->SetFinishCallback([](CAnimBlendAssociation* a, void* data) {
            const auto self = static_cast<CTaskSimpleChoking*>(data);
            self->m_bIsFinished = true;
            self->m_pAnim = nullptr;
        });
        m_pAnim->SetSpeed(CGeneral::GetRandomNumberInRange(0.8f, 1.1f));
        m_nTimeStarted = CTimer::GetTimeInMS() - m_nTimeRemaining;
    }
    m_nTimeRemaining = CGeneral::GetRandomNumberInRange<uint32>(1'000, 2'000);
}

void CTaskSimpleChoking::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleChoking, 0x86d820, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x6202C0);
    RH_ScopedInstall(Destructor, 0x620370);

    RH_ScopedInstall(UpdateChoke, 0x620660);
    RH_ScopedGlobalInstall(FinishAnimChokingCB, 0x620480);

    RH_ScopedVMTInstall(Clone, 0x623220);
    RH_ScopedVMTInstall(GetTaskType, 0x620360);
    RH_ScopedVMTInstall(MakeAbortable, 0x6203F0);
    RH_ScopedVMTInstall(ProcessPed, 0x620490);
}
