#include "StdInc.h"
#include "TaskSimpleLeaveGroup.h"

void CTaskSimpleLeaveGroup::InjectHooks() {
    RH_ScopedClass(CTaskSimpleLeaveGroup);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x463540);
    RH_ScopedInstall(Destructor, 0x463560);
}
