#include "StdInc.h"

#include "TaskComplexGoToCarDoorAndStandStill.h"
#include "TaskSimpleCarGoToPointNearDoorUntilDoorNotInUse.h"
#include "TaskSimplePause.h"
#include "TaskSimpleStandStill.h"
#include "TaskSimpleCarWaitForDoorNotToBeInUse.h"
#include "TaskComplexFollowPointRoute.h"
#include "TaskSimpleGoToPoint.h"

void CTaskComplexGoToCarDoorAndStandStill::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexGoToCarDoorAndStandStill, 0x86ec4c, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x645780);
    RH_ScopedInstall(Destructor, 0x64A580);

    RH_ScopedInstall(IsVehicleInRange, 0x6458A0);
    RH_ScopedInstall(ComputeRouteToDoor, 0x645910);
    RH_ScopedInstall(CreateSubTask, 0x64A5F0);

    RH_ScopedVMTInstall(Clone, 0x6498B0);
    RH_ScopedVMTInstall(GetTaskType, 0x645830);
    RH_ScopedVMTInstall(MakeAbortable, 0x645840);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x64D2B0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x64D440);
    RH_ScopedVMTInstall(ControlSubTask, 0x64A820);
}

// 0x645780
CTaskComplexGoToCarDoorAndStandStill::CTaskComplexGoToCarDoorAndStandStill(
    CVehicle*  vehicle,
    eMoveState moveState,
    bool       bIsDriver,
    int32      targetSeat,
    float      targetRadius,
    float      slowDownDist,
    float      maxSeekDist,
    int32      maxSeekTime
) :
    m_Vehicle{vehicle},
    m_MoveState{moveState},
    m_bIsDriver{bIsDriver},
    m_TargetRadius{targetRadius},
    m_SlowDownDist{slowDownDist},
    m_MaxSeekDist{maxSeekDist},
    m_MaxSeekTime{maxSeekTime},
    m_TargetSeat{targetSeat}
{
    CEntity::SafeRegisterRef(m_Vehicle);
}

// 0x6498B0
CTaskComplexGoToCarDoorAndStandStill::CTaskComplexGoToCarDoorAndStandStill(const CTaskComplexGoToCarDoorAndStandStill& o) :
    CTaskComplexGoToCarDoorAndStandStill{ o.m_Vehicle, o.m_MoveState, o.m_bIsDriver, o.m_TargetSeat, o.m_TargetRadius, o.m_SlowDownDist, o.m_MaxSeekDist, o.m_MaxSeekTime }
{
}

// 0x64A580
CTaskComplexGoToCarDoorAndStandStill::~CTaskComplexGoToCarDoorAndStandStill() {
    CEntity::SafeCleanUpRef(m_Vehicle);
    delete m_RouteToDoor;
}

// 0x645840
bool CTaskComplexGoToCarDoorAndStandStill::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    if (m_bTryingToEnterInWater) {
        if (m_pSubTask && m_pSubTask->GetTaskType() == TASK_SIMPLE_PAUSE) {
            if (event && notsa::contains({ EVENT_IN_WATER, EVENT_IN_AIR }, event->GetEventType())) {
                return false;
            }
        }
    }
    return m_pSubTask->MakeAbortable(ped, priority, event);
}

// 0x64A5F0
CTask* CTaskComplexGoToCarDoorAndStandStill::CreateSubTask(eTaskType taskType, CPed* ped) {
    switch (taskType) {
    case TASK_SIMPLE_GO_TO_POINT_NEAR_CAR_DOOR_UNTIL_DOOR_NOT_IN_USE: {
        ped->bHasJustLeftCar = true;
        return new CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse{ m_Vehicle, m_TargetDoor, m_TargetPt, m_MoveState };
    }
    case TASK_SIMPLE_PAUSE:
        return new CTaskSimplePause{ 1 };
    case TASK_SIMPLE_STAND_STILL:
        return new CTaskSimpleStandStill{ 1 };
    case TASK_SIMPLE_CAR_WAIT_FOR_DOOR_NOT_TO_BE_IN_USE:
        return new CTaskSimpleCarWaitForDoorNotToBeInUse{ m_Vehicle, (uint32)m_TargetDoor };
    case TASK_COMPLEX_FOLLOW_POINT_ROUTE:
        return new CTaskComplexFollowPointRoute{ m_MoveState, *m_RouteToDoor, CTaskComplexFollowPointRoute::Mode::ONE_WAY, 0.5f, 5.f, false, false, false };
    case TASK_SIMPLE_GO_TO_POINT: {
        ped->bHasJustLeftCar = true;
        return new CTaskSimpleGoToPoint{ m_MoveState, m_TargetPt, m_TargetRadius, true };
    }
    case TASK_FINISHED:
        return nullptr;
    default:
        NOTSA_UNREACHABLE();
    }
}

// 0x64D2B0
CTask* CTaskComplexGoToCarDoorAndStandStill::CreateNextSubTask(CPed* ped) {
    switch (const auto tt = m_pSubTask->GetTaskType()) {
    case TASK_COMPLEX_FOLLOW_POINT_ROUTE: { // 0x64D3B8
        m_TargetPt = CCarEnterExit::GetPositionToOpenCarDoor(m_Vehicle, m_TargetDoor);
        return CreateSubTask(TASK_SIMPLE_GO_TO_POINT, ped);
    }
    case TASK_SIMPLE_GO_TO_POINT: { // 0x64D3E9
        m_bAchievedTargetDoor |= static_cast<CTaskSimpleGoToPoint*>(m_pSubTask)->WasTaskSuccessful(ped);
        return CreateSubTask(TASK_FINISHED, ped);
    }
    case TASK_SIMPLE_GO_TO_POINT_NEAR_CAR_DOOR_UNTIL_DOOR_NOT_IN_USE: // 0x64D387
        return CreateSubTask(TASK_SIMPLE_CAR_WAIT_FOR_DOOR_NOT_TO_BE_IN_USE, ped);
    case TASK_SIMPLE_CAR_WAIT_FOR_DOOR_NOT_TO_BE_IN_USE: // 0x64D2EB
        return CreateFirstSubTask(ped); 
    case TASK_SIMPLE_STAND_STILL: // 0x64D420
        return CreateSubTask(TASK_FINISHED, ped);
    case TASK_SIMPLE_PAUSE: { // 0x64D314
        if (m_bTryingToEnterInWater) {
            m_bAchievedTargetDoor |= (ped->GetPosition() - m_TargetPt).SquaredMagnitude2D() <= sq(0.5f);
        }
        return CreateSubTask(TASK_FINISHED, ped);
    }
    default:
        NOTSA_UNREACHABLE("SubTaskType: {}", (int)tt);
    }
}

// 0x64D440
CTask* CTaskComplexGoToCarDoorAndStandStill::CreateFirstSubTask(CPed* ped) {
    return CreateSubTask([this, ped] {
        if (!IsVehicleInRange(*ped)) {
            return TASK_SIMPLE_STAND_STILL;
        }

        if (m_bTryingToEnterInWater) {
            return TASK_SIMPLE_PAUSE;
        }

        if (m_TargetSeat != 0 && !m_bIsDriver) {
            if (const auto psgrAtDoor = m_Vehicle->GetPassengers()[CCarEnterExit::ComputePassengerIndexFromCarDoor(m_Vehicle, m_TargetSeat)]) {
                if (psgrAtDoor->bThisPedIsATargetPriority) {
                    return TASK_SIMPLE_STAND_STILL;
                }
            }
        }

        // Calculate the door to use
        bool bCanWaitForDoorToBeFree;
        if (!CalculateTargetDoor(ped, bCanWaitForDoorToBeFree)) {
            if (!bCanWaitForDoorToBeFree) {
                return TASK_SIMPLE_STAND_STILL;
            }
        }

        // If it's blocked by something we first have to get into a position where it's not blocked
        if (CCarEnterExit::IsPathToDoorBlockedByVehicleCollisionModel(ped, m_Vehicle, m_TargetPt)) {
            ComputeRouteToDoor(*ped);
            return TASK_COMPLEX_FOLLOW_POINT_ROUTE; // 0x64D4E7
        }

        // Otherwise we can just go to the door straight
        return bCanWaitForDoorToBeFree
            ? TASK_SIMPLE_GO_TO_POINT_NEAR_CAR_DOOR_UNTIL_DOOR_NOT_IN_USE
            : TASK_SIMPLE_GO_TO_POINT;
    }(), ped);
}

// 0x64A820
CTask* CTaskComplexGoToCarDoorAndStandStill::ControlSubTask(CPed* ped) {
    if (!m_Vehicle) {
        return nullptr;
    }

    if (!ped->IsCop()) {
        if (const auto drvr = m_Vehicle->m_pDriver) {
            if (drvr->IsPlayer() && drvr->m_nPedState == PEDSTATE_ARRESTED) {
                return CreateSubTask(TASK_SIMPLE_STAND_STILL, ped); // 0x64A875
            }
        }
    }

    switch (const auto tt = m_pSubTask->GetTaskType()) {
    case TASK_SIMPLE_GO_TO_POINT_NEAR_CAR_DOOR_UNTIL_DOOR_NOT_IN_USE:
    case TASK_SIMPLE_PAUSE:
    case TASK_SIMPLE_STAND_STILL:
    case TASK_SIMPLE_CAR_WAIT_FOR_DOOR_NOT_TO_BE_IN_USE: { // 0x64A890
        if (m_TargetDoor != 0) { // TODO: Enum
            m_TargetPt = CCarEnterExit::GetPositionToOpenCarDoor(m_Vehicle, m_TargetDoor);
        }
        return m_pSubTask;
    }
    case TASK_SIMPLE_GO_TO_POINT: { // 0x64AA11
        ped->bPushOtherPeds = true;

        if (!IsVehicleInRange(*ped)) { // Inverted
            m_TargetPt = CCarEnterExit::GetPositionToOpenCarDoor(m_Vehicle, m_TargetDoor);
            return CreateSubTask(TASK_SIMPLE_STAND_STILL, ped); // 0x64AA50 
        }

        if (bool bCanWaitForDoorToBeFree; !CalculateTargetDoor(ped, bCanWaitForDoorToBeFree)) {
            return CreateSubTask(bCanWaitForDoorToBeFree ? TASK_SIMPLE_GO_TO_POINT_NEAR_CAR_DOOR_UNTIL_DOOR_NOT_IN_USE : TASK_SIMPLE_STAND_STILL, ped);
        }
        static_cast<CTaskSimpleGoToPoint*>(m_pSubTask)->UpdatePoint(m_TargetPt, m_TargetRadius);

        return m_pSubTask;
    }
    case TASK_COMPLEX_FOLLOW_POINT_ROUTE: { // 0x64A8FA
        const auto doorOpenPos = CCarEnterExit::GetPositionToOpenCarDoor(m_Vehicle, m_TargetDoor);

        if (!IsVehicleInRange(*ped)) {
            m_TargetPt = doorOpenPos;
            return CreateSubTask(TASK_SIMPLE_STAND_STILL, ped); // 0x64A94A
        }

        if (!CCarEnterExit::IsPathToDoorBlockedByVehicleCollisionModel(ped, m_Vehicle, doorOpenPos)) {
            m_TargetPt = doorOpenPos;
            return CreateSubTask(TASK_SIMPLE_GO_TO_POINT, ped); // 0x64A990
        }

        if ((m_TargetPt - doorOpenPos).SquaredMagnitude() >= sq(0.1f)) { // 0x64A9D1
            m_TargetPt = doorOpenPos;
            return CreateFirstSubTask(ped);
        }

        return m_pSubTask;
    }
    default:
        NOTSA_UNREACHABLE("SubTaskType: {}", (int)tt);
    }
}

// 0x6458A0
bool CTaskComplexGoToCarDoorAndStandStill::IsVehicleInRange(const CPed& ped) {
    return (ped.GetPosition() - m_Vehicle->GetPosition()).SquaredMagnitude() <= sq(m_MaxSeekDist);
}

// 0x645910
void CTaskComplexGoToCarDoorAndStandStill::ComputeRouteToDoor(const CPed& ped) {
    if (!m_RouteToDoor) {
        m_RouteToDoor = new CPointRoute{};
    }
    m_RouteToDoor->Clear();

    const CVector pedPos = ped.GetPosition();

    CVector vehBBPlanes[4];
    float   vehBBPlanesDot[4];
    CPedGeometryAnalyser::ComputeEntityBoundingBoxPlanesUncachedAll(pedPos.z, *m_Vehicle, &vehBBPlanes, vehBBPlanesDot);
    const auto CalculatePositionOnPlane = [&](CVector pos, int32 side) {
        return pos - pos.ProjectOnToNormal(vehBBPlanes[side], vehBBPlanesDot[side]);
    };

    const auto pedPosOnBBPlane  = CalculatePositionOnPlane(pedPos, CPedGeometryAnalyser::ComputeEntityHitSide(pedPos, *m_Vehicle));
    const auto doorPosOnBBPlane = CalculatePositionOnPlane(m_TargetPt, CPedGeometryAnalyser::ComputeEntityHitSide(m_TargetPt, *m_Vehicle));

    CPointRoute routeAroundVeh{};
    CPedGeometryAnalyser::ComputeRouteRoundEntityBoundingBox(ped, pedPosOnBBPlane, *m_Vehicle, doorPosOnBBPlane, routeAroundVeh, 0);

    m_RouteToDoor->AddUnlessFull(pedPosOnBBPlane);
    for (const auto& pt : routeAroundVeh.GetAll()) {
        m_RouteToDoor->AddUnlessFull(pt);
    }
    m_RouteToDoor->AddUnlessFull(doorPosOnBBPlane);
}

// Based on SA code
// Calculate door to use and it's position
// Original code is much different, I had to refactor it to get rid of the copy pasta
bool CTaskComplexGoToCarDoorAndStandStill::CalculateTargetDoor(CPed* ped, bool& bCanWaitForDoorToBeFree) {
    bCanWaitForDoorToBeFree = false;
    if (m_TargetSeat != 0) {
        m_TargetDoor = m_TargetSeat;
        m_TargetPt   = CCarEnterExit::GetPositionToOpenCarDoor(m_Vehicle, m_TargetSeat);
    } else {
        if (m_bIsDriver) {
            if (!CCarEnterExit::GetNearestCarDoor(ped, m_Vehicle, m_TargetPt, m_TargetDoor)) {
                return false;
            }
        } else {
            if (!CCarEnterExit::GetNearestCarPassengerDoor(ped, m_Vehicle, &m_TargetPt, &m_TargetDoor, true, true, true)) {
                bCanWaitForDoorToBeFree = CCarEnterExit::GetNearestCarPassengerDoor(ped, m_Vehicle, &m_TargetPt, &m_TargetDoor, true, false, true);
                return false;
            }
        }
    }
    return true;
}
