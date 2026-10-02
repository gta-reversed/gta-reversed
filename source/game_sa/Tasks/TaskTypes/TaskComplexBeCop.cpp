#include "StdInc.h"

#include "TaskComplexBeCop.h"

void CTaskComplexBeCop::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexBeCop, 0x85a428, 15);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x4993D0);
    RH_ScopedInstall(Destructor, 0x4994E0);

    RH_ScopedVMTInstall(Clone, 0x499440);
    RH_ScopedVMTInstall(GetTaskType, 0x499430);
}
