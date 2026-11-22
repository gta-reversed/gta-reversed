#include "StdInc.h"
#include "TaskSimpleUseAtm.h"


void CTaskSimpleUseAtm::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleUseAtm, 0x85a134, 10);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x48DFE0);
    RH_ScopedInstall(Destructor, 0x48E960);

    RH_ScopedVMTInstall(Clone, 0x48E010);
    RH_ScopedVMTInstall(IsInterruptable, 0x48E080);
}

// 0x48DFE0
CTaskSimpleUseAtm::CTaskSimpleUseAtm() :
    CTaskSimpleRunAnim { ANIM_GROUP_DEFAULT, ANIM_ID_ATM, 4.f, Type, "UseAtm", false }
{
}

// NOTSA
CTaskSimpleUseAtm::CTaskSimpleUseAtm(const CTaskSimpleUseAtm&) :
    CTaskSimpleUseAtm{}
{
}
