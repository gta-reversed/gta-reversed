#include "StdInc.h"

#include "TaskSimpleCarGoToPointNearDoorUntilDoorNotInUse.h"
#include "TaskSimpleGoToPoint.h"
#include "TaskSimpleCarWaitForDoorNotToBeInUse.h"
#include "TaskSimpleStandStill.h"

void CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse, 0x86ede0, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x646D70);
    RH_ScopedInstall(Destructor, 0x646E50);

    RH_ScopedVMTInstall(Clone, 0x646EC0);
    RH_ScopedVMTInstall(GetTaskType, 0x646E30);
    RH_ScopedVMTInstall(MakeAbortable, 0x646E40);
    RH_ScopedVMTInstall(ProcessPed, 0x64B4A0);
}

// 0x646D70
CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse::CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse(
    CVehicle*      vehicle,
    int32          targetDoor,
    const CVector& targetPt,
    eMoveState     moveState
) :
    m_TaskGoToPoint{ new CTaskSimpleGoToPoint{ moveState, targetPt } },
    m_TaskWaitForDoor{ new CTaskSimpleCarWaitForDoorNotToBeInUse{ vehicle, (uint32)targetDoor } }
{
}

// 0x646EC0
CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse::CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse(const CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse&) :
    CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse{
        m_TaskWaitForDoor->m_veh,
        static_cast<eTargetDoor>(m_TaskWaitForDoor->m_doorsToWaitFor[0]),
        m_TaskGoToPoint->m_vecTargetPoint,
        m_TaskGoToPoint->m_moveState
    }
{
}

// 0x646E50
CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse::~CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse() {
    delete m_TaskGoToPoint;
    delete m_TaskWaitForDoor;
}

// 0x64B4A0
bool CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse::ProcessPed(CPed* ped) {
    m_TaskGoToPoint->UpdatePoint(ComputeTarget());
    if (!m_TaskGoToPoint->ProcessPed(ped)) {
        return m_TaskWaitForDoor->ProcessPed(ped);
    }
    CTaskSimpleStandStill{-1}.ProcessPed(ped);
    return true;
}

// 0x646F40
CVector CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse::ComputeTarget() const {
    const auto    veh    = m_TaskWaitForDoor->m_veh;
    const CVector vehPos = veh->GetPosition();
    return vehPos + (CCarEnterExit::GetPositionToOpenCarDoor(veh, m_TaskWaitForDoor->m_doorsToWaitFor[0]) - vehPos) * 2.f;
}
