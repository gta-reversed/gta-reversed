#include "StdInc.h"
#include "EventPlayerCommandToGroupGather.h"


void CEventPlayerCommandToGroupGather::InjectHooks()
{
    RH_ScopedVirtualClass(CEventPlayerCommandToGroupGather, 0x86D120, 17);
    RH_ScopedCategory("Events");

    RH_ScopedInstall(Constructor, 0x609250);
}

// 0x609250
CEventPlayerCommandToGroupGather* CEventPlayerCommandToGroupGather::Constructor(CPed* target)
{
    this->CEventPlayerCommandToGroupGather::CEventPlayerCommandToGroupGather(target);
    return this;
}

