#include "StdInc.h"
#include "TaskSimpleFinishBrain.h"

void CTaskSimpleFinishBrain::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleFinishBrain, 0x859d20, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedVMTInstall(Clone, 0x463660);
    RH_ScopedVMTInstall(GetTaskType, 0x4636B0);
}

// 0x463660
CTaskSimpleFinishBrain::CTaskSimpleFinishBrain(const CTaskSimpleFinishBrain&) :
    CTaskSimpleFinishBrain{/* nothing to copy */}
{
}
