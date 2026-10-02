#include "StdInc.h"

#include "TaskComplexWanderMedic.h"

// 0x658770
CTaskComplexWanderMedic::CTaskComplexWanderMedic(eMoveState MoveState, uint8 Dir, bool bWanderSensibly) :
    CTaskComplexWander(MoveState, Dir, bWanderSensibly) 
{
    /* nop */
}

void CTaskComplexWanderMedic::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexWanderMedic, 0x86f48c, 15);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x658770);
    RH_ScopedInstall(Destructor, 0x658850);

    RH_ScopedVMTInstall(Clone, 0x6587A0);
    RH_ScopedVMTInstall(GetWanderType, 0x658810);
    RH_ScopedVMTInstall(ScanForStuff, 0x658820);
}
