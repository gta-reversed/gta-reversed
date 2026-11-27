#include "StdInc.h"

#include "TaskSimpleChoking.h"

// 0x6202C0
CTaskSimpleChoking::CTaskSimpleChoking(CPed* attacker, bool isTeargas) :
    m_Attacker{ attacker },
    m_TimeRemaining{ CGeneral::GetRandomNumberInRange<uint32>(1'000, 2'000) },
    m_TimeStarted{ CTimer::GetTimeInMS() },
    m_IsTeargas{ isTeargas } {
}

// NOTSA
CTaskSimpleChoking::CTaskSimpleChoking(const CTaskSimpleChoking& o) :
    CTaskSimpleChoking{ o.m_Attacker, o.m_IsTeargas } {
}

// 0x620370
CTaskSimpleChoking::~CTaskSimpleChoking() {
    if (m_Anim) {
        m_Anim->SetBlendDelta(-4.f);
        m_Anim->SetDefaultFinishCallback();
    }
}

// 0x620480
void CTaskSimpleChoking::DeleteAnimChokeCB(CAnimBlendAssociation* anim, void* data) {
    auto* const self = notsa::cast<CTaskSimpleChoking>(static_cast<CTask*>(data));

    self->m_Anim       = nullptr;
    self->m_IsFinished = true;
}

// 0x6203F0
bool CTaskSimpleChoking::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    switch (priority) {
    case ABORT_PRIORITY_URGENT:
    case ABORT_PRIORITY_IMMEDIATE: {
        if (event && event->GetEventPriority() < 57) {
            return false;
        }
        m_IsFinished = true;
        break;
    }
    case ABORT_PRIORITY_LEISURE: {
        if (m_Anim) {
            m_Anim->SetFlag(ANIMATION_IS_BLEND_AUTO_REMOVE);
        }
        break;
    }
    default: {
        NOTSA_UNREACHABLE_CASE(priority);
    }
    }

    if (auto* const anim = std::exchange(m_Anim, nullptr)) {
        anim->SetBlendDelta(-4.f);
        anim->SetDefaultFinishCallback();
    }

    return true;
}

// 0x620490
bool CTaskSimpleChoking::ProcessPed(CPed* ped) {
    if (m_IsFinished) {
        return true;
    }

    if (m_Attacker && !ped->IsPlayer() && !m_IsTeargas) { // 0x6204AD
        const CVector pedToAttacker = m_Attacker->GetPosition() - ped->GetPosition();
        if (pedToAttacker.Dot(ped->GetForward()) > 0.f) {
            ped->m_fAimingRotation = CVector2D{ pedToAttacker }.Heading();
        }
    }

    const auto timeStepMs = (uint32)CTimer::GetTimeStepInMS();
    m_TimeRemaining = m_TimeRemaining > timeStepMs
        ? m_TimeRemaining - timeStepMs
        : 0;

    if (!m_Anim) { // 0x620553
        m_Anim = CAnimManager::BlendAnimation(ped->GetRpClump(), ANIM_GROUP_DEFAULT, ANIM_ID_GAS_CWR, 4.f);
        m_Anim->SetFinishCallback(DeleteAnimChokeCB, this);
        m_Anim->SetSpeed(CGeneral::GetRandomNumberInRange(0.75f, 1.f));
    } else if (!m_TimeRemaining) { // 0x6205A1
        if (m_Anim->GetAnimId() == ANIM_ID_GAS_CWR) { // 0x6205B3
            m_Anim->SetDefaultFinishCallback();
            m_Anim = CAnimManager::BlendAnimation(ped->GetRpClump(), ANIM_GROUP_DEFAULT, ANIM_ID_IDLE_TIRED, 4.f);
            m_Anim->SetFinishCallback(DeleteAnimChokeCB, this);

            // NOTE: The original evaluates the random number twice (`std::min` macro)
            const auto chokedForMs = CTimer::GetTimeInMS() - m_TimeStarted;
            m_TimeRemaining = chokedForMs < CGeneral::GetRandomNumberInRange<uint32>(8'000, 12'000)
                ? chokedForMs
                : CGeneral::GetRandomNumberInRange<uint32>(8'000, 12'000);
        } else { // 0x620625
            m_Anim->SetBlendDelta(-4.f);
            m_Anim->SetFlag(ANIMATION_IS_BLEND_AUTO_REMOVE);
        }
    }

    ped->Say(CTX_GLOBAL_PAIN_COUGH);

    return false;
}

// 0x620660
void CTaskSimpleChoking::UpdateChoke(CPed* victim, CPed* attacker, bool isTeargas) {
    m_IsTeargas = isTeargas;
    if (m_Attacker != attacker) {
        m_Attacker = attacker;
    }
    m_IsFinished = false;

    if (m_Anim && m_Anim->GetAnimId() != ANIM_ID_GAS_CWR) { // 0x6206B3
        m_Anim->SetDefaultFinishCallback();
        m_Anim = CAnimManager::BlendAnimation(victim->GetRpClump(), ANIM_GROUP_DEFAULT, ANIM_ID_GAS_CWR, 4.f);
        m_Anim->SetFinishCallback(DeleteAnimChokeCB, this);
        m_Anim->SetSpeed(CGeneral::GetRandomNumberInRange(0.8f, 1.1f));
        m_TimeStarted = CTimer::GetTimeInMS() - m_TimeRemaining;
    }

    m_TimeRemaining = CGeneral::GetRandomNumberInRange<uint32>(1'000, 2'000);
}

void CTaskSimpleChoking::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleChoking, 0x86d820, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedConstructorInstall(0x6202C0, "1", {}, CPed*, uint8);
    RH_ScopedDestructorInstall(0x620370);

    RH_ScopedGlobalInstall(DeleteAnimChokeCB, 0x620480);
    RH_ScopedInstall(UpdateChoke, 0x620660);
    RH_ScopedVMTInstall(Clone, 0x623220);
    RH_ScopedVMTInstall(GetTaskType, 0x620360);
    RH_ScopedVMTInstall(MakeAbortable, 0x6203F0);
    RH_ScopedVMTInstall(ProcessPed, 0x620490);
}
