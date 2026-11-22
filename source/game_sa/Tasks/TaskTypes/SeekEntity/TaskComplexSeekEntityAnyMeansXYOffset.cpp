#include "StdInc.h"
#include "TaskComplexSeekEntityAnyMeansXYOffset.h"

void CTaskComplexSeekEntityAnyMeansXYOffset::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexSeekEntityAnyMeansXYOffset, 0x870CF0, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedVMTInstall(Clone, 0x696030);
    RH_ScopedVMTInstall(GetTaskType, 0x695690);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x6956A0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x6960B0);
    RH_ScopedVMTInstall(ControlSubTask, 0x696160);
}
