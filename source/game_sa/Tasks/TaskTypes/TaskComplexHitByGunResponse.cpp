#include "StdInc.h"

#include "TaskComplexHitByGunResponse.h"
#include "TaskSimpleHitByGunFromFront.h"
#include "TaskSimpleHitByGunFromLeft.h"
#include "TaskSimpleHitByGunFromRear.h"
#include "TaskSimpleHitByGunFromRight.h"

void CTaskComplexHitByGunResponse::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexHitByGunResponse, 0x86e06c, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x631DD0);
    RH_ScopedInstall(Destructor, 0x631E30);

    RH_ScopedVMTInstall(Clone, 0x636640);
    RH_ScopedVMTInstall(GetTaskType, 0x631DF0);
    RH_ScopedVMTInstall(MakeAbortable, 0x631E00);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x631E40);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x631E50);
    RH_ScopedVMTInstall(ControlSubTask, 0x631F70);
}

// 0x631DD0
CTaskComplexHitByGunResponse::CTaskComplexHitByGunResponse(eDirection hs) :
    m_HitSide{hs}
{
}

//! @notsa
CTaskComplexHitByGunResponse::CTaskComplexHitByGunResponse(const CTaskComplexHitByGunResponse& o) :
    CTaskComplexHitByGunResponse{ o.m_HitSide }
{
}

// 0x631E50
CTask* CTaskComplexHitByGunResponse::CreateFirstSubTask(CPed* ped) {
    switch (m_HitSide) {
    case eDirection::FORWARD:  return new CTaskSimpleHitByGunFromFront{};
    case eDirection::LEFT:     return new CTaskSimpleHitByGunFromLeft{};
    case eDirection::BACKWARD: return new CTaskSimpleHitByGunFromRear{};
    case eDirection::RIGHT:    return new CTaskSimpleHitByGunFromRight{};
    default:                   NOTSA_UNREACHABLE();
    }
}
