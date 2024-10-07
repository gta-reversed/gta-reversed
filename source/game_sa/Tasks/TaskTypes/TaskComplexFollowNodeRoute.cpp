#include "StdInc.h"

#include "Interior/InteriorManager_c.h"

#include "TaskComplexFollowNodeRoute.h"

#include "Tasks/TaskTypes/TaskSimpleGoToPointFine.h"
#include "Tasks/TaskTypes/TaskComplexGoToPointAndStandStill.h"
#include "Tasks/TaskTypes/TaskComplexLeaveCar.h"
#include "Tasks/TaskTypes/TaskSimpleGoToPoint.h"
#include "Tasks/TaskTypes/TaskSimpleStandStill.h"

void CTaskComplexFollowNodeRoute::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexFollowNodeRoute, 0x8700a8, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x66EA30);
    RH_ScopedInstall(Destructor, 0x66EB70);

    RH_ScopedInstall(GetSubTaskType, 0x669650);

    RH_ScopedInstall(CalcGoToTaskType, 0x66EBE0);
    RH_ScopedInstall(SetTarget, 0x671750);
    RH_ScopedInstall(CreateSubTask, 0x669690);
    RH_ScopedInstall(GetLastWaypoint, 0x6698E0);
    RH_ScopedInstall(GetNextWaypoint, 0x669980);
    RH_ScopedInstall(ComputeRoute, 0x6699E0);
    RH_ScopedInstall(CalcBlendRatio, 0x66EDC0);
    RH_ScopedInstall(CanGoStraightThere, 0x66EF20);
    RH_ScopedInstall(ComputePathNodes, 0x66EFA0);

    RH_ScopedVMTInstall(Clone, 0x6713E0);
    RH_ScopedVMTInstall(GetTaskType, 0x66EB60);
    RH_ScopedVMTInstall(StopTimer, 0x6694E0);
    RH_ScopedVMTInstall(MakeAbortable, 0x669520);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x6718D0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x671800);
    RH_ScopedVMTInstall(ControlSubTask, 0x671AB0);
}

// 0x66EA30
CTaskComplexFollowNodeRoute::CTaskComplexFollowNodeRoute(
    eMoveState     moveState,
    const CVector& target,
    float          targetTolerance,
    float          slowDownDist,
    float          followNodeThresholdHeightChange,
    bool           bKeepNodesHeadingAwayFromTarget,
    int32          timeToFinishMs,
    bool           bUseBlending
) :
    m_MoveState{moveState},
    m_TargetPt{target},
    m_TargetTolerance{targetTolerance},
    m_SlowDownDist{slowDownDist},
    m_FollowNodeThresholdHeightChange{followNodeThresholdHeightChange},
    m_bKeepNodesHeadingAwayFromTarget{bKeepNodesHeadingAwayFromTarget},
    m_TimeToFinishMs{timeToFinishMs},
    m_bUseBlending{bUseBlending},
    m_NodeRoute{new CNodeRoute{}},
    m_PtRoute{new CPointRoute{}}
{
}

// 0x6713E0
CTaskComplexFollowNodeRoute::CTaskComplexFollowNodeRoute(const CTaskComplexFollowNodeRoute& o) :
    CTaskComplexFollowNodeRoute{
        o.m_MoveState,
        o.m_TargetPt,
        o.m_TargetTolerance,
        o.m_SlowDownDist,
        o.m_FollowNodeThresholdHeightChange,
        o.m_bKeepNodesHeadingAwayFromTarget,
        o.m_TimeToFinishMs,
        o.m_bUseBlending
    }
{
    m_StartNode = o.m_StartNode;
}

// 0x66EB70
CTaskComplexFollowNodeRoute::~CTaskComplexFollowNodeRoute() {
    delete m_PtRoute;
    delete m_NodeRoute;
}

// 0x669650
eTaskType CTaskComplexFollowNodeRoute::GetSubTaskType(uint32 progress, bool bLastRoutePointIsTarget, const CPointRoute& route) {
    if (!bLastRoutePointIsTarget) {
        return TASK_SIMPLE_GO_TO_POINT;
    }
    if (progress == route.GetSize()) {
        return TASK_FINISHED;
    }
    return route.GetSize() != progress + 1
        ? TASK_SIMPLE_GO_TO_POINT
        : TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL;
}

// 0x6694E0
void CTaskComplexFollowNodeRoute::StopTimer(const CEvent* event) {
    if (!CEventHandler::IsTemporaryEvent(*event)) {
        m_FinishedTimer.Stop();
    }
}

// 0x66EBE0
eTaskType CTaskComplexFollowNodeRoute::CalcGoToTaskType(CPed* ped, eTaskType subTaskType) {
    if (subTaskType != TASK_SIMPLE_GO_TO_POINT) {
        return subTaskType;
    }

    m_SpeedIncreaseAmt = m_SpeedDecreaseAmt;
    m_bWillSlowDown    = false;

    if (m_MoveState >= PEDMOVE_JOG) {
        if (m_PtRoute->IsEmpty()) { // [Code moved from 0x66EC22]: The only way the value could be negative is for `m_PtRoute->GetSize()` to be `0`
            return TASK_SIMPLE_GO_TO_POINT;
        }

        assert(m_CurrPtIdx + 1 <= m_PtRoute->GetSize()); // Original code just clamped the value [But that's a quite error imo]

        const CVector currPt                     = GetCurrentPt();
        const auto    cosAngleBetweenRoutePoints = (currPt - GetLastWaypoint(ped)).Normalized().Dot((GetNextWaypoint(ped) - currPt).Normalized()); // I'm pretty sure they're missing an `abs` here
        if (m_bWillSlowDown = cosAngleBetweenRoutePoints <= COS_45) { // Keep in mind that the angles are inverted, so `<= COS_45` means "angle is more than 45"
            const auto sprint   = m_MoveState == PEDMOVE_SPRINT;
            m_SpeedIncreaseDist = m_SpeedDecreaseDist = sprint ? 5.f : 4.f;
            m_SpeedDecreaseAmt  = std::clamp(1.f - (cosAngleBetweenRoutePoints + 1.f) / (COS_45 + 1.f), 0.f, 1.f) * (sprint ? 3.f : 1.5f); 
        }
    }

    if (m_bSlowingDown) {
        m_bSlowingDown = false;
        m_bSpeedingUp  = true;
        return TASK_SIMPLE_GO_TO_POINT_FINE;
    } else {
        m_bSpeedingUp  = false;
        m_bSlowingDown = false;
        return TASK_SIMPLE_GO_TO_POINT;
    }
}

// 0x66EDC0
float CTaskComplexFollowNodeRoute::CalcBlendRatio(CPed* ped, bool slowing) const {
    const auto spdChangeDistSq = sq(slowing ? m_SpeedDecreaseDist : m_SpeedIncreaseDist);
    const auto distToPtSq      = ((slowing ? GetCurrentPt() : GetLastWaypoint(ped)) - ped->GetPosition()).SquaredMagnitude();
    if (distToPtSq >= spdChangeDistSq) {
        return -1.f;
    }
    const auto spdChangeAmt   = slowing ? m_SpeedDecreaseAmt : m_SpeedIncreaseAmt;
    const auto spdChangeRatio = std::cos(std::clamp(distToPtSq / spdChangeDistSq, 0.f, 1.f) * PI) * 0.5f + 0.5f; // NOTE/BUG: Not sure how well diving squared values will work out :D
    return std::max(
        CTaskSimpleGoToPointFine::BaseRatio(PEDMOVE_WALK) + 0.75f,
        CTaskSimpleGoToPointFine::BaseRatio(m_MoveState) - spdChangeRatio * spdChangeAmt
    );
}

// 0x66EF20
bool CTaskComplexFollowNodeRoute::CanGoStraightThere(const CPed* ped, const CVector& from, const CVector& to, float maxDist) const {
    if ((from - to).SquaredMagnitude() >= sq(maxDist)) {
        return false;
    }
    if (ped->bIgnoreHeightDifferenceFollowingNodes) {
        return true;
    }
    return CPedGeometryAnalyser::IsWanderPathClear(from, to, m_FollowNodeThresholdHeightChange, 4) == CPedGeometryAnalyser::WanderPathClearness::CLEAR;
}

// 0x671750
void CTaskComplexFollowNodeRoute::SetTarget(CPed* ped, const CVector& targetPt, float targetTolerance, float slowDownDistance, float followNodeThresholdHeightChange, bool bForce) {
    if (!bForce) {
        if (m_TargetPt == targetPt && m_TargetTolerance == targetTolerance && m_SlowDownDist == slowDownDistance && m_FollowNodeThresholdHeightChange == followNodeThresholdHeightChange) {
            return; // Nothing changed
        }
    }

    m_TargetPt                        = targetPt;
    m_TargetTolerance                 = targetTolerance;
    m_SlowDownDist                    = slowDownDistance;
    m_FollowNodeThresholdHeightChange = followNodeThresholdHeightChange;

    if (m_bUseBlending) {
        m_SpeedDecreaseDist = m_SpeedIncreaseDist = 0.0;
        m_SpeedDecreaseAmt  = m_SpeedIncreaseAmt  = 0.f;
        m_bSlowingDown      = m_bSpeedingUp       = false;
    }

    ComputePathNodes(ped);
    ComputeRoute();

    m_bNewTarget = true;
}

// 0x669690
CTask* CTaskComplexFollowNodeRoute::CreateSubTask(eTaskType taskType, CPed* ped) const {
    switch (taskType) {
    case TASK_SIMPLE_GO_TO_POINT_FINE:
        return new CTaskSimpleGoToPointFine{ CTaskSimpleGoToPointFine::BaseRatio(m_MoveState), GetCurrentPt() };
    case TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL:
        return new CTaskComplexGoToPointAndStandStill{ m_MoveState, GetCurrentPt(), m_TargetTolerance, m_SlowDownDist };
    case TASK_COMPLEX_LEAVE_CAR:
        return new CTaskComplexLeaveCar{ ped->m_pVehicle, TARGET_DOOR_FRONT_LEFT, 0, true, false };
    case TASK_SIMPLE_GO_TO_POINT:
        return new CTaskSimpleGoToPoint{ m_MoveState, GetCurrentPt() };
    case TASK_SIMPLE_STAND_STILL: {
        ped->Teleport(m_TargetPt, false);
        return new CTaskSimpleStandStill{};
    }
    case TASK_FINISHED:
        return nullptr;
    default:
        NOTSA_UNREACHABLE();
    }
}

// 0x6698E0
CVector CTaskComplexFollowNodeRoute::GetLastWaypoint(CPed* ped) const {
    if (m_PtRoute->GetSize() == 1) {
        return (*m_PtRoute)[0];
    }
    return m_CurrPtIdx != 0
        ? (*m_PtRoute)[m_CurrPtIdx - 1]
        : ped->GetPosition();
}

// 0x669980
CVector CTaskComplexFollowNodeRoute::GetNextWaypoint(CPed* ped) const {
    if (m_PtRoute->GetSize() == 1) {
        return (*m_PtRoute)[0];
    }
    const auto nextPt = m_CurrPtIdx + 1;
    return (*m_PtRoute)[nextPt < m_PtRoute->GetSize() ? nextPt : m_CurrPtIdx - 1];
}

// 0x66EFA0
void CTaskComplexFollowNodeRoute::ComputePathNodes(CPed const* ped) {
    // Make sure TargetPt.z is on the ground
    {
        bool hasTargetPtGroundZ;
        float targetPtGroundZ = CWorld::FindGroundZFor3DCoord(m_TargetPt + CVector{0.f, 0.f, 1.f}, &hasTargetPtGroundZ);
        if (hasTargetPtGroundZ) {
            m_TargetPt.z = targetPtGroundZ + 1.f;
        }
    }

    const CVector pedPos = ped->GetPosition();

    if (CanGoStraightThere(ped, pedPos, m_TargetPt, ped->GetIntelligence()->m_FollowNodeThresholdDistance)) {
        ThePaths.ComputeRoute(
            PATH_TYPE_PED,
            ped->GetPosition(),
            m_TargetPt,
            m_StartNode,
            *m_NodeRoute
        );
        m_StartNode = {};
    }

    if (std::abs(pedPos.z - m_TargetPt.z) >= m_FollowNodeThresholdHeightChange || m_NodeRoute->IsEmpty() || m_bKeepNodesHeadingAwayFromTarget) { // 0x66F147
        if (!m_NodeRoute->IsEmpty() && !m_bKeepNodesHeadingAwayFromTarget && m_NodeRoute->GetAll()[0]) {
            const auto routeFirstNode = m_NodeRoute->GetAll()[0];
            if (ThePaths.IsAreaNodesAvailable(routeFirstNode)) {
                if ((m_TargetPt - pedPos).Magnitude2D() + 50.f < (ThePaths.GetPathNode(routeFirstNode)->GetPosition() - pedPos).Magnitude2D()) {
                    m_NodeRoute->Clear(); // 0x66F209
                }
            }
        }
    } else if (m_NodeRoute->GetAll()[0]) {
        const auto routeFirstNode = m_NodeRoute->GetAll()[0];
        if (ThePaths.IsAreaNodesAvailable(routeFirstNode)) {
            if ((m_TargetPt - pedPos).Magnitude2D() + 3.f < (ThePaths.GetPathNode(routeFirstNode)->GetPosition() - pedPos).Magnitude2D()) {
                m_NodeRoute->Clear(); // 0x66F139
            }
        }
    }

    if (g_interiorMan.GetPedsInteriorGroup(ped)) { // 0x66F216
        if (!m_NodeRoute->IsEmpty()) {
            const auto routeFirstNode = m_NodeRoute->GetAll()[0];
            if (routeFirstNode && ThePaths.IsAreaNodesAvailable(routeFirstNode)) {
                const auto pedInt = g_interiorMan.GetPedsInterior(ped); // 0x66F2EE

                auto routeFirstNodeCoors = ThePaths.GetPathNode(routeFirstNode)->GetPosition();
                routeFirstNodeCoors.z += 1.f;
                const auto routeFirstNodeInt = g_interiorMan.GetVectorsInterior(routeFirstNodeCoors); // 0x66F330

                if (pedInt) {
                    if (pedInt != routeFirstNodeInt) {
                        if (!ThePaths.These2NodesAreAdjacent(routeFirstNode, pedInt->GetNodeAddress())) {
                            m_NodeRoute->Clear();
                            ThePaths.ComputeRoute( // 0x66F38D
                                PATH_TYPE_PED,
                                ped->GetPosition(),
                                m_TargetPt,
                                pedInt->GetNodeAddress(),
                                *m_NodeRoute
                            );
                        }
                    }
                } else if (routeFirstNodeInt) {
                    if (!ThePaths.These2NodesAreAdjacent(routeFirstNode, routeFirstNodeInt->GetNodeAddress())) {
                        m_NodeRoute->Clear();
                        ThePaths.ComputeRoute( // 0x66F3E0
                            PATH_TYPE_PED,
                            ped->GetPosition(),
                            m_TargetPt,
                            routeFirstNodeInt->GetNodeAddress(),
                            *m_NodeRoute
                        );
                        m_NodeRoute->Reverse();
                    }
                }
            }
        }
    } else {
        // Make the route as short as possible by "cutting" down excess nodes
        for (const auto& [i, node] : notsa::enumerate(m_NodeRoute->GetAll())) {
            assert(node); // Original code checks for this too, but I don't think this is possible

            CVector nodePos;
            if (ThePaths.IsAreaNodesAvailable(node)) {
                nodePos = ThePaths.GetPathNode(node)->GetPosition();
            } else if (notsa::IsFixBugs()) {
                continue; 
            }
            nodePos.z += 1.f;
            if (CanGoStraightThere(ped, nodePos, m_TargetPt, ped->GetIntelligence()->m_FollowNodeThresholdDistance)) {
                m_NodeRoute->ResizeTo(i + 1);
                break;
            }
        }
    }

    // 0x66F3F2
    m_CurrPtIdx = m_CurrNode
        ? notsa::indexof(m_NodeRoute->GetAll(), m_CurrNode, 0)
        : 0;
    if (!m_NodeRoute->IsEmpty()) {
        m_CurrNode = (*m_NodeRoute)[m_CurrPtIdx];
    }
}

// 0x6699E0
void CTaskComplexFollowNodeRoute::ComputeRoute() {
    m_PtRoute->Clear();
    for (const auto& node : m_NodeRoute->GetAll()) {
        if (!ThePaths.IsAreaNodesAvailable(node)) {
            continue;
        }
        m_PtRoute->Add(ThePaths.GetPathNode(node)->GetPosition());
    }
    if (m_LastRoutePointIsTarget = !m_PtRoute->IsFull()) {
        m_PtRoute->Add(m_TargetPt);
    }
}

// 0x669520
bool CTaskComplexFollowNodeRoute::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
    if (event) {
        if (m_CurrPtIdx + 1 != m_PtRoute->GetSize()) {
            const auto vehicle = [&]() -> CVehicle* {
                switch (event->GetEventType()) {
                case EVENT_POTENTIAL_WALK_INTO_VEHICLE:
                    return static_cast<const CEventPotentialWalkIntoVehicle*>(event)->m_vehicle;
                case EVENT_VEHICLE_COLLISION:
                    return static_cast<const CEventVehicleCollision*>(event)->m_vehicle;
                default:
                    return nullptr;
                }
            }();
            if (vehicle && vehicle->GetMoveSpeed().SquaredMagnitude() <= sq(0.125f)) {
                switch (m_pSubTask->GetTaskType()) {
                case TASK_SIMPLE_GO_TO_POINT:
                case TASK_SIMPLE_GO_TO_POINT_FINE: {
                    const auto tGoToSubTask = static_cast<CTaskSimpleGoTo*>(m_pSubTask);
                    if (CPedGeometryAnalyser::IsEntityBlockingTarget(vehicle, tGoToSubTask->GetTargetPt(), tGoToSubTask->GetTargetPtRadius())) {
                        tGoToSubTask->SetTargetPtRadius((ped->GetPosition() - vehicle->GetPosition()).Magnitude2D());
                    }
                }
                }
            }
        }
    }
    return m_pSubTask->MakeAbortable(ped, priority, event);
}

// 0x6718D0
CTask* CTaskComplexFollowNodeRoute::CreateNextSubTask(CPed* ped) {
    const auto subTaskType = m_pSubTask->GetTaskType();

    if (m_FinishedTimer.IsOutOfTime()) {
        if (subTaskType == TASK_SIMPLE_STAND_STILL) {
            ped->Teleport(m_TargetPt, false);
            return CreateSubTask(TASK_FINISHED, ped); // 0x671949
        }
    }

    if (subTaskType == TASK_COMPLEX_LEAVE_CAR) { // 0x671969
        return ped->bInVehicle
            ? CreateSubTask(TASK_FINISHED, ped)
            : CreateFirstSubTask(ped);
    }

    if (m_LastRoutePointIsTarget) { // Inverted
        switch (subTaskType) {
        case TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL: { // 0x6719E8
            m_CurrPtIdx++;
            break;
        }
        case TASK_SIMPLE_STAND_STILL: // 0x6719B6
            return CreateSubTask(TASK_FINISHED, ped);
        case TASK_SIMPLE_GO_TO_POINT: { // 0x6719FC - Go to next node in the route
            m_CurrPtIdx++;
            if (m_CurrPtIdx < m_NodeRoute->GetSize()) { 
                m_CurrNode = (*m_NodeRoute)[m_CurrPtIdx];
            }
            break;
        }
        case TASK_SIMPLE_GO_TO_POINT_FINE: // 0x671AA7
        default:
            return nullptr; // 0x6719C9             
        }
    } else { // 0x671A3A - Proceed to the next point
        if (m_CurrPtIdx + 1 == m_PtRoute->GetSize()) { // Reached the last point in the route, calculate next few
            SetTarget(ped, m_TargetPt, m_TargetTolerance, m_SlowDownDist, m_FollowNodeThresholdHeightChange, true);
            return CreateFirstSubTask(ped);
        } else { // Continue the current route
            assert(!m_PtRoute->IsEmpty());
            assert(m_CurrPtIdx < m_PtRoute->GetSize());

            m_CurrPtIdx++;
            m_CurrNode = (*m_NodeRoute)[m_CurrPtIdx];
        }
    }

    const auto tt = GetSubTaskType(m_CurrPtIdx, m_LastRoutePointIsTarget, *m_PtRoute);
    return CreateSubTask(m_bUseBlending ? tt : CTaskComplexFollowNodeRoute::CalcGoToTaskType(ped, tt), ped);
}

// 0x671800
CTask* CTaskComplexFollowNodeRoute::CreateFirstSubTask(CPed* ped) {
    if (m_TimeToFinishMs >= 0) {
        m_FinishedTimer.Start(m_TimeToFinishMs);
    }

    if (m_bUseBlending) {
        m_SpeedDecreaseDist = m_SpeedIncreaseDist = 0.0;
        m_SpeedDecreaseAmt  = m_SpeedIncreaseAmt  = 0.f;
        m_bSlowingDown      = m_bSpeedingUp       = false;
    }

    if (ped->bInVehicle) {
        return CreateSubTask(TASK_COMPLEX_LEAVE_CAR, ped);
    }

    SetTarget(ped, m_TargetPt, m_TargetTolerance, m_SlowDownDist, m_FollowNodeThresholdHeightChange, true);
    m_bNewTarget = false;

    const auto tt = GetSubTaskType(m_CurrPtIdx, m_LastRoutePointIsTarget, *m_PtRoute);
    return CreateSubTask(m_bUseBlending ? tt : CTaskComplexFollowNodeRoute::CalcGoToTaskType(ped, tt), ped);
    
}

// 0x671AB0
CTask* CTaskComplexFollowNodeRoute::ControlSubTask(CPed* ped) {
    const auto subTaskType = m_pSubTask->GetTaskType();

    if (m_bNewTarget && subTaskType != TASK_COMPLEX_LEAVE_CAR) {
        return CreateFirstSubTask(ped);
    }

    if (m_FinishedTimer.IsOutOfTime()) {
        if (subTaskType != TASK_SIMPLE_STAND_STILL) {
            return m_pSubTask->MakeAbortable(ped)
                ? CreateSubTask(TASK_SIMPLE_STAND_STILL, ped)
                : m_pSubTask;
        }
    }

    if (m_bUseBlending) {
        // If we're already going fine (Eg.: using `CTaskSimpleGoToPointFine`), adjust move ratio
        if (subTaskType == TASK_SIMPLE_GO_TO_POINT_FINE) {
            const auto tGoToPointFine = static_cast<CTaskSimpleGoToPointFine*>(m_pSubTask);

            const auto br = CalcBlendRatio(ped, true);
            tGoToPointFine->SetMoveRatio(br >= 0.f ? br : CTaskSimpleGoToPointFine::BaseRatio(m_MoveState));

            if (m_bSpeedingUp && br < 0.f) {
                m_bSpeedingUp = false;
                return CreateSubTask(TASK_SIMPLE_GO_TO_POINT, ped);
            }
        }

        // If we're supposed to slow down (use `CTaskSimpleGoToPointFine`), but aren't yet
        if (m_bWillSlowDown && !m_bSlowingDown && subTaskType != TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL) { // 0x671BA8
            if (const auto br = CalcBlendRatio(ped, true); br >= 0.f) { // Blend ratios under 0 use the standard `CTaskSimpleGoToPoint`
                m_bSpeedingUp   = false;
                m_bWillSlowDown = false;
                m_bSlowingDown  = true;

                const auto tGoToPtFine = subTaskType != TASK_SIMPLE_GO_TO_POINT_FINE
                    ? CreateSubTask(TASK_SIMPLE_GO_TO_POINT_FINE, ped)
                    : m_pSubTask;
                static_cast<CTaskSimpleGoToPointFine*>(tGoToPtFine)->SetMoveRatio(br);

                return tGoToPtFine;
            }
        }
    }

    return m_pSubTask;
}

