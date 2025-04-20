#include "StdInc.h"

#include "TaskComplexUsePairedAttractor.h"
#include "TaskComplexUseScriptedAttractor.h"
#include "TaskComplexAttractorPartnerWait.h"
#include "TaskComplexUseAttractorPartner.h"
#include "Scripted2dEffects.h"

void CTaskComplexUsePairedAttractor::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexUsePairedAttractor, 0x86e258, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x6331A0);
    RH_ScopedInstall(Destructor, 0x6331E0);

    RH_ScopedInstall(CreateSubTask, 0x638D30);
    RH_ScopedInstall(SetPartnership, 0x6331F0);
    RH_ScopedInstall(ClearPartnership, 0x633220);
    RH_ScopedInstall(SeekPartnership, 0x638AE0);
    RH_ScopedVMTInstall(Clone, 0x636C70);
    RH_ScopedVMTInstall(GetTaskType, 0x6331D0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x639E40);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x639F70);
    RH_ScopedVMTInstall(ControlSubTask, 0x638BD0);
}

// 0x6331A0
CTaskComplexUsePairedAttractor::CTaskComplexUsePairedAttractor(CPedAttractor* attractor) :
    m_Attractor{ attractor }
{
    assert(attractor);
}

// 0x636C70
CTaskComplexUsePairedAttractor::CTaskComplexUsePairedAttractor(const CTaskComplexUsePairedAttractor& o) :
    CTaskComplexUsePairedAttractor{o.m_Attractor}
{
}

// 0x638D30
CTask* CTaskComplexUsePairedAttractor::CreateSubTask(eTaskType taskType, CPed* ped) {
    switch (taskType) {
    case TASK_FINISHED: { // 0x638ED0
        GetPedAttractorManager()->BroadcastDeparture(ped, m_Attractor);
        return nullptr;
    }
    case TASK_COMPLEX_USE_SCRIPTED_ATTRACTOR: { // 0x638E65
        ClearPartnership();
        return new CTaskComplexUseScriptedAttractor{ *m_Attractor };
    }
    case TASK_COMPLEX_ATTRACTOR_PARTNER_WAIT: { // 0x638E65
        // ...some dead code here I won't bother with... //
        return new CTaskComplexAttractorPartnerWait{ m_IsLeader, *m_CurrentFxPair };
    }
    case TASK_COMPLEX_USE_ATTRACTOR_PARTNER: { // 0x638DE7
        // ...some dead code here I won't bother with... //
        return new CTaskComplexUseAttractorPartner{ m_IsLeader, *m_CurrentFxPair };
    }
    default:
        NOTSA_UNREACHABLE("Unhandled task type ({})", taskType);
    }
}

// 0x639E40
CTask* CTaskComplexUsePairedAttractor::CreateNextSubTask(CPed* ped) {
    switch (const auto tt = m_pSubTask->GetTaskType()) {
    case TASK_COMPLEX_USE_ATTRACTOR_PARTNER: { // 0x639E53
        if (m_IsLeader) {
            m_PartnerProgress = (m_PartnerProgress + 5) % CScriptedEffectPairs::MAX_NUM_EFFECT_PAIRS;
        }
        CTask* ret;
        switch (m_CurrentFxPair->Effects[m_IsLeader ? 0 : 1].PartnerUseMode) {
        case CScriptedEffectPair::eMode::USE_PARTNER_ONCE :         ret = CreateSubTask(TASK_FINISHED, ped); break;                       // 0x639F24
        case CScriptedEffectPair::eMode::LOOK_FOR_ANOTHER_PARTNER : ret = CreateSubTask(TASK_COMPLEX_USE_SCRIPTED_ATTRACTOR, ped); break; // 0x639F37
        default: NOTSA_UNREACHABLE();
        }
        ClearPartnership();
        return ret;
    }
    case TASK_COMPLEX_USE_SCRIPTED_ATTRACTOR: { // 0x639E5D
        m_SoloProgress++;
        if (m_Partner) {
            return CreateSubTask(TASK_COMPLEX_ATTRACTOR_PARTNER_WAIT, ped);
        }
        return CreateSubTask(
            CScripted2dEffects::ms_useAgainFlags[CScripted2dEffects::GetIndex(m_Attractor->m_Fx)]
                ? TASK_COMPLEX_USE_SCRIPTED_ATTRACTOR
                : TASK_FINISHED,
            ped
        );
    }
    case TASK_COMPLEX_ATTRACTOR_PARTNER_WAIT: {
        return CreateSubTask(
            m_Partner
                ? TASK_COMPLEX_USE_ATTRACTOR_PARTNER
                : TASK_COMPLEX_USE_SCRIPTED_ATTRACTOR,
            ped
        );
    }
    default:
        NOTSA_UNREACHABLE("Unhandled task type ({})", tt);
    }
}

// 0x638BD0
CTask* CTaskComplexUsePairedAttractor::ControlSubTask(CPed* ped) {
    switch (const auto tt = m_pSubTask->GetTaskType()) {
    case TASK_COMPLEX_USE_ATTRACTOR_PARTNER: { // 0x638BE4
        if (!m_Partner || !m_Partner->GetTaskManager().Find<CTaskComplexUsePairedAttractor, CTaskComplexUseAttractorPartner, CTaskComplexAttractorPartnerWait>()) {
            m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_LEISURE);
        }
        return m_pSubTask;
    }
    case TASK_COMPLEX_ATTRACTOR_PARTNER_WAIT: { // 0x638C35
        if (m_Partner) {
            if (m_Partner->GetTaskManager().Find<CTaskComplexUsePairedAttractor>()) {
                if (m_IsLeader ? m_Partner->GetTaskManager().Has<CTaskComplexAttractorPartnerWait>() : m_Partner->GetTaskManager().Has<CTaskComplexUseAttractorPartner>()) {
                    m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_LEISURE);
                }
                return m_pSubTask;
            }
        } else {
            m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_LEISURE);
        }
        ClearPartnership();
        return m_pSubTask;
    }
    case TASK_COMPLEX_USE_SCRIPTED_ATTRACTOR: { // 0x638BF4
        if (!m_Partner) {
            SeekPartnership(ped);
        }
        if (m_Partner && m_CurrentFxPair->UsePartnerImmediately) {
            m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_LEISURE);
        }
        return m_pSubTask;
    }
    }
    NOTSA_UNREACHABLE();
}

// 0x6331F0
void CTaskComplexUsePairedAttractor::SetPartnership(CPed* partner, bool isLeader, const CScriptedEffectPair* pair) {
    m_Partner       = partner;
    m_IsLeader      = isLeader;
    m_CurrentFxPair = pair;
}

// 0x633220
void CTaskComplexUsePairedAttractor::ClearPartnership() {
    m_Partner       = nullptr;
    m_IsLeader      = false;
    m_CurrentFxPair = nullptr;
}

// 0x638AE0
void CTaskComplexUsePairedAttractor::SeekPartnership(CPed* us) {
    auto* const pairs = CScripted2dEffects::GetEffectPairs(m_Attractor->m_Fx);
    if (pairs->NumPairs <= 0) {
        return;
    }
    for (auto k = 0; k < CScriptedEffectPairs::MAX_NUM_EFFECT_PAIRS; k++) {
        const auto        i    = (m_PartnerProgress + k + 4) % CScriptedEffectPairs::MAX_NUM_EFFECT_PAIRS;
        const auto* const pair = &pairs->Pairs[i];
        if (pair->Effects[0].Effect == -1 || pair->Effects[1].Effect == -1) {
            continue;
        }
        auto* const partner = GetPedAttractorManager()->GetPedUsingEffect(CScripted2dEffects::GetEffect(pair->Effects[1].Effect));
        if (!partner) {
            continue;
        }
        auto* const tPartnerUsePairedAttractor = partner->GetTaskManager().Find<CTaskComplexUsePairedAttractor>();
        if (!tPartnerUsePairedAttractor || tPartnerUsePairedAttractor->m_Partner || m_Partner) {
            continue;
        }
        m_PartnerProgress = i;
        SetPartnership(partner, true, pair);
        tPartnerUsePairedAttractor->SetPartnership(us, false, pair);
        // TODO/NOTE: Why not break here?
    }
}
