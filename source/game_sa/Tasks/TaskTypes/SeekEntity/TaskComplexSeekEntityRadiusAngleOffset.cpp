#include "StdInc.h"
#include "TaskComplexSeekEntityRadiusAngleOffset.h"

void CTaskComplexSeekEntityRadiusAngleOffset::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexSeekEntityRadiusAngleOffset, 0x0085A388, 11);
    RH_ScopedCategory("Tasks/TaskTypes/SeekEntity");

    RH_ScopedInstall(Constructor, 0x493730);
    RH_ScopedInstall(Destructor, 0x493890);

    RH_ScopedInstall(CreateSubTask, 0x496270);

    RH_ScopedVMTInstall(Clone, 0x495FB0);
    RH_ScopedVMTInstall(GetTaskType, 0x493800);
    RH_ScopedVMTInstall(MakeAbortable, 0x493810);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x496080);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x496600);
    RH_ScopedVMTInstall(ControlSubTask, 0x496760);
}
