#include "StdInc.h"
#include "TaskSimpleCower.h"

void CTaskSimpleCower::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleCower, 0x85a0d0, 10);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x48DE70);
    RH_ScopedInstall(Destructor, 0x48E900);

    RH_ScopedVMTInstall(Clone, 0x48DEB0);
    RH_ScopedVMTInstall(IsInterruptable, 0x48DF20);
}
