#include "StdInc.h"

#include "PedStuckChecker.h"


void CPedStuckChecker::InjectHooks() {
    RH_ScopedClass(CPedStuckChecker);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(TestPedStuck, 0x602C00, { .reversed = false });
}

bool CPedStuckChecker::TestPedStuck(CPed* ped, CEventGroup* eventGroup)
{
    return plugin::CallMethodAndReturn<bool, 0x602C00, CPedStuckChecker*, CPed*, CEventGroup*>(this, ped, eventGroup);
}
