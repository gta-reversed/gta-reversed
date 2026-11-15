#include "StdInc.h"

#include "TaskComplexBeCop.h"

void CTaskComplexBeCop::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexBeCop, 0x85a428, 15);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedConstructorInstall(0x4993D0, "1", {}, eMoveState, bool, CTask*);
    RH_ScopedDestructorInstall(0x4994E0);


    RH_ScopedVMTInstall(Clone, 0x499440);
    RH_ScopedVMTInstall(GetTaskType, 0x499430);
}
