#include "StdInc.h"
#include "TaskComplexSeekEntityStandard.h"

void CTaskComplexSeekEntityStandard::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexSeekEntityStandard, 0x00859DF8, 11);
    RH_ScopedCategory("Tasks/TaskTypes/SeekEntity");

    RH_ScopedInstall(Constructor, 0x46AC10);
    RH_ScopedInstall(Destructor, 0x46AD70);

    RH_ScopedInstall(CreateSubTask, 0x46F1E0);

    RH_ScopedVMTInstall(Clone, 0x46EF50);
    RH_ScopedVMTInstall(GetTaskType, 0x46ACE0);
    RH_ScopedVMTInstall(MakeAbortable, 0x46ACF0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x46EFF0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x46F6A0);
    RH_ScopedVMTInstall(ControlSubTask, 0x46F800);
}
