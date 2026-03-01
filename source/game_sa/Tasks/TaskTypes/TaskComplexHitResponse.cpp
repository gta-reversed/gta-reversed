#include "StdInc.h"

#include "TaskComplexHitResponse.h"
#include "TaskSimpleHitFromFront.h"
#include "TaskSimpleHitFromLeft.h"
#include "TaskSimpleHitFromBack.h"
#include "TaskSimpleHitFromRight.h"

void CTaskComplexHitResponse::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexHitResponse, 0x86e040, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x631D70);
    RH_ScopedInstall(Destructor, 0x631DA0);

    RH_ScopedVMTInstall(Clone, 0x6365E0);
    RH_ScopedVMTInstall(GetTaskType, 0x631D90);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x631DB0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x638040);
    RH_ScopedVMTInstall(ControlSubTask, 0x631DC0);
}

// 0x631D70
CTaskComplexHitResponse::CTaskComplexHitResponse(eDirection hs) :
    m_HitSide{ hs }
{
}

CTaskComplexHitResponse::CTaskComplexHitResponse(const CTaskComplexHitResponse& o) :
    CTaskComplexHitResponse{ o.m_HitSide }
{
}

// 0x638040
CTask* CTaskComplexHitResponse::CreateFirstSubTask(CPed* ped) {
    switch (m_HitSide) {
    case eDirection::FORWARD:  return new CTaskSimpleHitFromFront{};
    case eDirection::LEFT:     return new CTaskSimpleHitFromLeft{};
    case eDirection::BACKWARD: return new CTaskSimpleHitFromBack{};
    case eDirection::RIGHT:    return new CTaskSimpleHitFromRight{};
    default:                   NOTSA_UNREACHABLE();
    }
}
