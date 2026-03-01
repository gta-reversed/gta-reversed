#include "StdInc.h"

#include "Scripted2dEffects.h"
#include "TaskComplexUseAttractorPartner.h"

void CTaskComplexUseAttractorPartner::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexUseAttractorPartner, 0x86e2b0, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x6332C0);
    RH_ScopedInstall(Destructor, 0x633300);

    RH_ScopedVMTInstall(Clone, 0x636D60);
    RH_ScopedVMTInstall(GetTaskType, 0x6332F0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x633310);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x638FF0);
    RH_ScopedVMTInstall(ControlSubTask, 0x633320);
}

// 0x6332C0
CTaskComplexUseAttractorPartner::CTaskComplexUseAttractorPartner(bool isLeader, const CScriptedEffectPair& pair) :
    m_IsLeader{ isLeader },
    m_CurrentFxPair{ &pair }
{ 
}

// 0x636D60
CTaskComplexUseAttractorPartner::CTaskComplexUseAttractorPartner(const CTaskComplexUseAttractorPartner& o) :
    CTaskComplexUseAttractorPartner{ o.m_IsLeader, *o.m_CurrentFxPair }
{
}

// 0x638FF0
CTask* CTaskComplexUseAttractorPartner::CreateFirstSubTask(CPed* ped) {
    return new CTaskComplexUseSequence{ m_CurrentFxPair->Effects[m_IsLeader ? 0 : 1].PartnerTask };
}
