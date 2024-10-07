#include "StdInc.h"
#include "TaskComplexUseEffectSprinting.h"

void CTaskComplexUseEffectSprinting::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexUseEffectSprinting, 0x86e5c0, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedVMTInstall(Clone, 0x6368A0);
    RH_ScopedVMTInstall(GetTaskType, 0x636890);
}

// 0x636780
CTaskComplexUseEffectSprinting::CTaskComplexUseEffectSprinting(C2dEffectBase* fx, CEntity* e) :
    CTaskComplexUseEffect{ fx, e }
{
    m_MoveState = PEDMOVE_RUN;
}

// 0x6367C0
CTaskComplexUseEffectSprinting::CTaskComplexUseEffectSprinting(const CTaskComplexUseEffectSprinting& o) :
    CTaskComplexUseEffectSprinting{ o.m_2dFx, o.m_Entity }
{
}
