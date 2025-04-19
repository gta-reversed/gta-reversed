#include "StdInc.h"

#include "TaskComplexUsePairedAttractor.h"
#include "TaskComplexUseScriptedAttractor.h"
#include "TaskComplexAttractorPartnerWait.h"
#include "TaskComplexUseAttractorPartner.h"

void CTaskComplexUsePairedAttractor::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexUsePairedAttractor, 0x86e258, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x6331A0);
    RH_ScopedInstall(Destructor, 0x6331E0);

    RH_ScopedInstall(CreateSubTask, 0x638D30, { .reversed = false });
    RH_ScopedVMTInstall(Clone, 0x636C70);
    RH_ScopedVMTInstall(GetTaskType, 0x6331D0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x639E40, { .reversed = false });
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x639F70, { .reversed = false });
    RH_ScopedVMTInstall(ControlSubTask, 0x638BD0, { .reversed = false });
}

// 0x6331A0
CTaskComplexUsePairedAttractor::CTaskComplexUsePairedAttractor(CPedAttractor* attractor) :
    m_Attractor{attractor}
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
        m_Partner       = nullptr;
        m_IsLeader      = false;
        m_CurrentFxPair = nullptr;
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
    return plugin::CallMethodAndReturn<CTask*, 0x639E40, CTaskComplexUsePairedAttractor*, CPed*>(this, ped);
}

// 0x639F70
CTask* CTaskComplexUsePairedAttractor::CreateFirstSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x639F70, CTaskComplexUsePairedAttractor*, CPed*>(this, ped);
}

// 0x638BD0
CTask* CTaskComplexUsePairedAttractor::ControlSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x638BD0, CTaskComplexUsePairedAttractor*, CPed*>(this, ped);
}
