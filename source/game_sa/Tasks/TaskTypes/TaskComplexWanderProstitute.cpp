#include "StdInc.h"

#include "TaskComplexWanderProstitute.h"

// 0x672690
CTaskComplexWanderProstitute::CTaskComplexWanderProstitute(eMoveState MoveState, uint8 Dir, bool bWanderSensibly) :
    CTaskComplexWanderStandard(MoveState, Dir, bWanderSensibly),
    m_nStartTimeInMs{ 0 }
{
}

// 0x672700
void CTaskComplexWanderProstitute::ScanForStuff(CPed* ped) {
    plugin::CallMethod<0x672700, CTaskComplexWanderProstitute*, CPed*>(this, ped);
}

void CTaskComplexWanderProstitute::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexWanderProstitute, 0x870148, 15);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x672690);

    RH_ScopedVMTInstall(Clone, 0x673C80);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x674920);
    RH_ScopedVMTInstall(GetWanderType, 0x6726C0);
    RH_ScopedVMTInstall(ScanForStuff, 0x672700, { .reversed = false });
}
