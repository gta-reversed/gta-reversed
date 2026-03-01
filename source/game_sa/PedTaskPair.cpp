#include "StdInc.h"

#include "PedTaskPair.h"

void CPedTaskPair::InjectHooks() {
    RH_ScopedClass(CPedTaskPair);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Flush, 0x5E95B0);
}

// 0x5E95B0
void CPedTaskPair::Flush() {
    Ped = nullptr;
    delete Task;
    Task = nullptr;
    Slot = TASK_SECONDARY_INVALID;
}
