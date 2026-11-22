#include "StdInc.h"
#include "TaskComplexUseScriptedAttractor.h"
#include "Scripted2dEffects.h"
#include "PedAttractor.h"

void CTaskComplexUseScriptedAttractor::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexUseScriptedAttractor, 0x86e2dc, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x633330);
    RH_ScopedInstall(Destructor, 0x633360);

    RH_ScopedVMTInstall(Clone, 0x636DD0);
    RH_ScopedVMTInstall(GetTaskType, 0x633350);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x633370);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x6390C0);
    RH_ScopedVMTInstall(ControlSubTask, 0x633380);
}

// 0x633330
CTaskComplexUseScriptedAttractor::CTaskComplexUseScriptedAttractor(const CPedAttractor& attractor) :
    m_attractor{ attractor }
{
}

CTaskComplexUseScriptedAttractor::CTaskComplexUseScriptedAttractor(const CTaskComplexUseScriptedAttractor& o) :
    CTaskComplexUseScriptedAttractor{o.m_attractor}
{
}

// 0x6390C0
CTask* CTaskComplexUseScriptedAttractor::CreateFirstSubTask(CPed* ped) {
    if (const auto optEffectIdx = CScripted2dEffects::IndexOfEffect(m_attractor.m_Fx)) {
        return new CTaskComplexUseSequence{
            CScripted2dEffects::ms_effectSequenceTaskIDs[*optEffectIdx]
        };
    }
    return nullptr;
}
