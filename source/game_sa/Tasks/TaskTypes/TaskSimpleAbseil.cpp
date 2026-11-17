#include "StdInc.h"
#include "TaskSimpleAbseil.h"

void CTaskSimpleAbseil::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleAbseil, 0x86f45c, 10);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedConstructorInstall(0x6586A0, "1", {});

    RH_ScopedVMTInstall(Clone, 0x6586D0);
    RH_ScopedVMTInstall(IsInterruptable, 0x658740);
}
