#include "StdInc.h"
#include "TaskComplexUseClosestFreeScriptedAttractorSprint.h"

void CTaskComplexUseClosestFreeScriptedAttractorSprint::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexUseClosestFreeScriptedAttractorSprint, 0x85b930, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x4B8850);
    RH_ScopedInstall(Destructor, 0x4B8900);

    RH_ScopedVMTInstall(Clone, 0x4B8870, { .reversed = false });
    RH_ScopedVMTInstall(GetTaskType, 0x4B88D0, { .reversed = false });
}

// 0x4B8850
CTaskComplexUseClosestFreeScriptedAttractorSprint::CTaskComplexUseClosestFreeScriptedAttractorSprint() :
    CTaskComplexUseClosestFreeScriptedAttractor{PEDMOVE_SPRINT}
{
}
