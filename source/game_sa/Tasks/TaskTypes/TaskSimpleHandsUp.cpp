#include "StdInc.h"
#include "TaskSimpleHandsUp.h"

void CTaskSimpleHandsUp::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleHandsUp, 0x85a29c, 10);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x48E970);
    RH_ScopedInstall(Destructor, 0x48EA60);

    RH_ScopedVMTInstall(Clone, 0x48E9C0);
    RH_ScopedVMTInstall(IsInterruptable, 0x48E9B0);
}
