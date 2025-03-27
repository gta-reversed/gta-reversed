#include "StdInc.h"
#include "TaskComplexUseClosestFreeScriptedAttractorRun.h"

void CTaskComplexUseClosestFreeScriptedAttractorRun::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexUseClosestFreeScriptedAttractorRun, 0x85b904, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x4B8790);
    RH_ScopedInstall(Destructor, 0x4B8840);

    RH_ScopedVMTInstall(Clone, 0x4B87B0);
    RH_ScopedVMTInstall(GetTaskType, 0x4B8810);
}

// 0x4B8790
CTaskComplexUseClosestFreeScriptedAttractorRun::CTaskComplexUseClosestFreeScriptedAttractorRun() :
    CTaskComplexUseClosestFreeScriptedAttractor{PEDMOVE_RUN}
{
}
