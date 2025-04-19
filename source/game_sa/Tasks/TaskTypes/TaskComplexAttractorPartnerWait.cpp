#include "StdInc.h"

#include "TaskComplexAttractorPartnerWait.h"
#include "Scripted2dEffects.h"

void CTaskComplexAttractorPartnerWait::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexAttractorPartnerWait, 0x86e284, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedVMTInstall(Clone, 0x636CF0);
    RH_ScopedVMTInstall(GetTaskType, 0x633280);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x6332A0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x638F20);
    RH_ScopedVMTInstall(ControlSubTask, 0x6332B0);
}

// 0x633250
CTaskComplexAttractorPartnerWait::CTaskComplexAttractorPartnerWait(bool isLeader, const CScriptedEffectPair& pair) :
    m_IsLeader{ isLeader },
    m_CurrentFxPair{ &pair }
{
}

// 0x638F20
CTask* CTaskComplexAttractorPartnerWait::CreateFirstSubTask(CPed* ped) {
    return new CTaskComplexUseSequence{ m_CurrentFxPair->Effects[m_IsLeader ? 0 : 1].WaitingTask };
}
