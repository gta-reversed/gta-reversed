#include "StdInc.h"
#include "TaskComplexFollowPatrolRoute.h"
#include "TaskComplexFollowNodeRoute.h"
#include "TaskComplexGoToPointAndStandStill.h"
#include "TaskSimpleGoToPoint.h"
#include "TaskSimpleNone.h"
#include "TaskSimpleRunNamedAnim.h"
#include "PatrolRoute.h"

// 0x674930
CTaskComplexFollowPatrolRoute::CTaskComplexFollowPatrolRoute(eMoveStateS32 moveState, const CPatrolRoute& route, ePatrolRouteModeS32 mode, float targetRadius, float slowDownDistance) :
    m_Mode{ mode },
    m_MoveState{ moveState },
    m_TargetRadius{ targetRadius },
    m_SlowDownDistance{ slowDownDistance },
    m_Route{ new CPatrolRoute(route) },
    m_IsNewRoute{ true } {
}

// NOTSA (0x674BB0 is `Clone`)
CTaskComplexFollowPatrolRoute::CTaskComplexFollowPatrolRoute(const CTaskComplexFollowPatrolRoute& o) :
    CTaskComplexFollowPatrolRoute{
        o.m_MoveState,
        *o.m_Route,
        o.m_Mode,
        o.m_TargetRadius,
        o.m_SlowDownDistance
    } {
}

// 0x6709F0
CTaskComplexFollowPatrolRoute::~CTaskComplexFollowPatrolRoute() {
    delete m_Route;
}

// 0x66BA90
bool CTaskComplexFollowPatrolRoute::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    // NOTE: `Flush` only zeroes the count, `m_Progress` is kept. A later `GetInfo(m_Progress)` (0x670DC0, 0x670BA8) reads the
    //       old (still intact, in bounds) entry; `GetNextSubTaskType` then returns `TASK_FINISHED` for the empty route.
    if (priority == ABORT_PRIORITY_LEISURE) {
        m_Route->Flush();
    }
    m_PedPositionWhenAborted = ped->GetPosition();
    // NOTE: No null check, same as the original (0x66BAD4): `CreateFirstSubTask` never returns null, a null next sub task ends the task
    m_IsAborting = m_pSubTask->MakeAbortable(ped, priority, event);
    return m_IsAborting;
}

// 0x670D80
CTask* CTaskComplexFollowPatrolRoute::CreateNextSubTask(CPed* ped) {
    switch (const auto tt = m_pSubTask->GetTaskType()) {
    case TASK_COMPLEX_FOLLOW_NODE_ROUTE: // 0x670DFF
        return CreateSubTask(GetNextSubTaskType());
    case TASK_NONE: // 0x670DA1
        return CreateSubTask(TASK_FINISHED);
    case TASK_SIMPLE_NAMED_ANIM: // 0x670DFB
        ++m_Progress;
        return CreateSubTask(GetNextSubTaskType());
    case TASK_SIMPLE_GO_TO_POINT:
    case TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL: { // 0x670DC0
        // NOTE: No bounds check, same as the original - these sub tasks are only made for `m_Progress + 1 <= count`, so it's at most 7
        if (m_Route->GetInfo(m_Progress).AnimationName[0] != '\0') {
            return CreateSubTask(TASK_SIMPLE_NAMED_ANIM);
        }
        ++m_Progress;
        return CreateSubTask(GetNextSubTaskType());
    }
    default:
        NOTSA_UNREACHABLE_CASE(tt);
    }
}

// 0x670E20
CTask* CTaskComplexFollowPatrolRoute::CreateFirstSubTask(CPed* ped) {
    m_IsNewRoute         = false;

    const auto numPoints = (int32)(m_Route->GetCount());
    if (numPoints == 0) {
        return CreateSubTask(TASK_NONE);
    }

    const CVector& pedPos = ped->GetPosition();

    // Closest point on any segment of the (closed) route, the point the segment goes to is the next one
    int32 closest{ -1 };
    float closestDist{ FLT_MAX };
    for (auto i = 0; i < numPoints; i++) {
        const auto    next = (i + 1) % numPoints;
        const auto&   from = m_Route->GetPosition(i % numPoints);
        const CVector seg  = m_Route->GetPosition(next) - from;
        const CVector dir  = seg.Normalized();
        const auto    proj = (pedPos - from).Dot(dir);
        if (proj <= 0.f || std::sqrt(seg.SquaredMagnitude()) <= proj) {
            continue;
        }
        // NOTE: `closestDist` holds a squared distance, yet it's squared again for the comparison
        if (const auto distSq = (pedPos - (from + dir * proj)).SquaredMagnitude(); distSq < sq(closestDist)) {
            closestDist = distSq;
            closest     = next;
        }
    }

    // Not beside any segment, take the closest point instead
    if (closest == -1) {
        float closestPtDist{ FLT_MAX };
        for (auto i = 0; i < numPoints; i++) {
            // NOTE: Same squared-twice comparison as above
            if (const auto distSq = CVector::DistSqr(pedPos, m_Route->GetPosition(i)); distSq < sq(closestPtDist)) {
                closestPtDist = distSq;
                closest       = i;
            }
        }
    }
    // NOTE: `closest` stays -1 only if every distance is NaN (NaN positions), the original (0x6711AC) doesn't check it either
    m_Progress = (int16)(closest);

    // NOTE: `m_IsAborting` is a bit field, `std::exchange` can't take it
    const bool wasAborting = m_IsAborting;
    m_IsAborting           = false;
    return CreateSubTask(wasAborting ? TASK_COMPLEX_FOLLOW_NODE_ROUTE : GetNextSubTaskType());
}

// 0x66BB00
CTask* CTaskComplexFollowPatrolRoute::ControlSubTask(CPed* ped) {
    if (m_IsNewRoute || m_IsAborting) { // Start over
        m_Progress = 0;
        return CreateFirstSubTask(ped);
    }
    return m_pSubTask;
}

// 0x670A70
eTaskType CTaskComplexFollowPatrolRoute::GetNextSubTaskType() {
    const auto numPoints = (int32)(m_Route->GetCount());
    if (numPoints == 0) {
        return TASK_FINISHED;
    }
    if (m_Progress + 1 < numPoints) {
        return TASK_SIMPLE_GO_TO_POINT;
    }
    if (m_Progress + 1 == numPoints) {
        return TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL;
    }
    if (m_Progress != numPoints) {
        return TASK_NONE;
    }

    // Reached the end of the route
    ++m_RouteTraversals;
    switch (m_Mode) {
    case ePatrolRouteMode::ONE_WAY: // 0x670AC5
        return TASK_FINISHED;
    case ePatrolRouteMode::RETURN: // 0x670ACE
        if (m_RouteTraversals != 1) {
            return TASK_FINISHED;
        }
        [[fallthrough]];
    case ePatrolRouteMode::BACK_AND_FORTH: // 0x670AD4
        m_Route->Reverse();
        [[fallthrough]];
    case ePatrolRouteMode::LOOP: // 0x670AD9
        m_Progress = 0;
        return GetNextSubTaskType();
    default:
        return TASK_NONE;
    }
}

// 0x670B00
CTask* CTaskComplexFollowPatrolRoute::CreateSubTask(eTaskType taskType) {
    switch (taskType) {
    case TASK_SIMPLE_GO_TO_POINT: // 0x670B53
        return new CTaskSimpleGoToPoint{ m_MoveState, m_Route->GetPosition(m_Progress), m_TargetRadius };
    case TASK_SIMPLE_NAMED_ANIM: { // 0x670BA8
        const auto& info = m_Route->GetInfo(m_Progress);
        return new CTaskSimpleRunNamedAnim{
            info.AnimationName,
            info.AnimationFile,
            ANIMATION_IS_FINISH_AUTO_REMOVE | ANIMATION_IS_PARTIAL | ANIMATION_CAN_EXTRACT_VELOCITY,
            4.f,
            (uint32)(-1),
            false,
            false,
            false,
            false
        };
    }
    case TASK_NONE: // 0x670C03
        return new CTaskSimpleNone{};
    case TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL: { // 0x670C44
        const auto keepsGoing = (m_Mode == ePatrolRouteMode::RETURN && m_RouteTraversals == 0) || m_Mode == ePatrolRouteMode::BACK_AND_FORTH || m_Mode == ePatrolRouteMode::LOOP;
        return new CTaskComplexGoToPointAndStandStill{
            m_MoveState,
            m_Route->GetPosition(m_Progress),
            m_TargetRadius,
            keepsGoing ? 0.f : m_SlowDownDistance
        };
    }
    case TASK_COMPLEX_FOLLOW_NODE_ROUTE: { // 0x670CE1
        const auto numPoints = (int32)(m_Route->GetCount());
        if (numPoints == 0) {
            return nullptr;
        }
        if (m_Progress == numPoints) {
            --m_Progress;
        }
        return new CTaskComplexFollowNodeRoute{ m_MoveState, m_Route->GetPosition(m_Progress) };
    }
    case TASK_FINISHED: // 0x670CCD
        return nullptr;
    default:
        NOTSA_UNREACHABLE_CASE(taskType);
    }
}

void CTaskComplexFollowPatrolRoute::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexFollowPatrolRoute, 0x870114, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedConstructorInstall(0x674930, "1", {}, eMoveStateS32, const CPatrolRoute&, ePatrolRouteModeS32, float, float);
    RH_ScopedDestructorInstall(0x6709F0);

    RH_ScopedInstall(GetNextSubTaskType, 0x670A70);
    RH_ScopedInstall(CreateSubTask, 0x670B00);

    RH_ScopedVMTInstall(Clone, 0x674BB0);
    RH_ScopedVMTInstall(GetTaskType, 0x670A60);
    RH_ScopedVMTInstall(MakeAbortable, 0x66BA90);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x670D80);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x670E20);
    RH_ScopedVMTInstall(ControlSubTask, 0x66BB00);
}
