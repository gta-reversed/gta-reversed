#include "StdInc.h"

#include "TaskComplexWanderProstitute.h"

// 0x672690
CTaskComplexWanderProstitute::CTaskComplexWanderProstitute(eMoveState MoveState, uint8 Dir, bool bWanderSensibly) :
    CTaskComplexWanderStandard(MoveState, Dir, bWanderSensibly),
    m_nStartTimeInMs{ 0 }
{
}

// 0x672700
void CTaskComplexWanderProstitute::ScanForStuff(CPed* ped) {
    plugin::CallMethod<0x672700, CTaskComplexWanderProstitute*, CPed*>(this, ped);
}
