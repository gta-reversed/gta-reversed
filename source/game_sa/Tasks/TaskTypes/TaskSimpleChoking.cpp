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
    switch (priority) {
    case ABORT_PRIORITY_URGENT:
    case ABORT_PRIORITY_IMMEDIATE: {
        if (priority == ABORT_PRIORITY_URGENT && event && event->GetEventPriority() < 62) {
            return false;
        }

        if (m_pAnim) {
            m_pAnim->m_BlendDelta = -4.f;
            m_pAnim->SetFinishCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
            m_pAnim = nullptr;
        }

        m_bIsFinished = true;
        return true;
    }
    case ABORT_PRIORITY_LEISURE: {
        if (m_pAnim) {
            m_pAnim->m_BlendDelta = -4.f;
        }
        return false;
    }
    default:
        NOTSA_UNREACHABLE();
    }
}

// 0x620490
bool CTaskSimpleChoking::ProcessPed(CPed* ped) {
    if (m_bIsFinished) {
        return true;
    }

    if (!m_pAnim) {
        m_pAnim = CAnimManager::BlendAnimation(ped->m_pRwClump, ANIM_GROUP_DEFAULT, ANIM_ID_GAS_CWR, 4.f);
        m_pAnim->SetFinishCallback([](CAnimBlendAssociation*, void* data) {
            const auto self = static_cast<CTaskSimpleChoking*>(data);
            self->m_bIsFinished = true;
            self->m_pAnim = nullptr;
        }, this);
        m_pAnim->SetSpeed(CGeneral::GetRandomNumberInRange(0.8f, 1.1f));
    }

    if (CTimer::GetTimeInMS() - m_nTimeStarted >= m_nTimeRemaining) {
        m_pAnim->m_BlendDelta = -4.f;
    }

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
        m_pAnim = CAnimManager::BlendAnimation(victim->m_pRwClump, ANIM_GROUP_DEFAULT, ANIM_ID_GAS_CWR, 4.f);
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

    RH_ScopedVMTInstall(Clone, 0x623220);
    RH_ScopedVMTInstall(GetTaskType, 0x620360);
    RH_ScopedVMTInstall(MakeAbortable, 0x6203F0);
    RH_ScopedVMTInstall(ProcessPed, 0x620490);
}
