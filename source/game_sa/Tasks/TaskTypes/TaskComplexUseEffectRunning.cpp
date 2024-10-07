#include "StdInc.h"
#include "TaskComplexUseEffectRunning.h"

void CTaskComplexUseEffectRunning::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexUseEffectRunning, 0x86e594, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedVMTInstall(Clone, 0x6367C0);
    RH_ScopedVMTInstall(GetTaskType, 0x6367B0);
}

// 0x636780
CTaskComplexUseEffectRunning::CTaskComplexUseEffectRunning(C2dEffectBase* fx, CEntity* e) :
    CTaskComplexUseEffect{ fx, e }
{
    m_MoveState = PEDMOVE_RUN;
}

// 0x6367C0
CTaskComplexUseEffectRunning::CTaskComplexUseEffectRunning(const CTaskComplexUseEffectRunning& o) :
    CTaskComplexUseEffectRunning{ o.m_2dFx, o.m_Entity }
{
}
