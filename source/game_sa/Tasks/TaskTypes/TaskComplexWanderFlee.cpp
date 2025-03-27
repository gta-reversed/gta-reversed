#include "StdInc.h"
#include "TaskComplexWanderFlee.h"

void CTaskComplexWanderFlee::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexWanderFlee, 0x86f690, 15);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x65B320);
    RH_ScopedInstall(Destructor, 0x65B370);

    RH_ScopedVMTInstall(Clone, 0x65CD70);
    RH_ScopedVMTInstall(ControlSubTask, 0x65B380);
    RH_ScopedVMTInstall(GetWanderType, 0x65B350);
    RH_ScopedVMTInstall(ScanForStuff, 0x65B360);
}

// 0x65B320
CTaskComplexWanderFlee::CTaskComplexWanderFlee(eMoveState moveState, uint8 dir) :
    CTaskComplexWander{moveState, dir, false}
{
}

CTaskComplexWanderFlee::CTaskComplexWanderFlee(const CTaskComplexWanderFlee& o) :
    CTaskComplexWanderFlee{o.m_nMoveState, o.m_nDir}
{
}
