#pragma once

#include "Vector.h"
#include "TaskComplex.h"
#include "PatrolRoute.h"
#include <Enums/eMoveState.h>

class CPed;
class CEvent;
class CPatrolRoute;

enum class ePatrolRouteMode : int32 {
    ONE_WAY        = 0, //!< Follow the route once
    RETURN         = 1, //!< Follow the route once, then once in reverse
    BACK_AND_FORTH = 2, //!< Same as `RETURN`, but forever
    LOOP           = 3, //!< Follow the route forever, always in the same direction
};
NOTSA_WENUM_DEFS_FOR(ePatrolRouteMode);

class NOTSA_EXPORT_VTABLE CTaskComplexFollowPatrolRoute : public CTaskComplex {
public:
    static constexpr auto Type = TASK_COMPLEX_FOLLOW_PATROL_ROUTE;

    CTaskComplexFollowPatrolRoute(eMoveStateS32 moveState, const CPatrolRoute& route, ePatrolRouteModeS32 mode, float targetRadius, float slowDownDistance);
    CTaskComplexFollowPatrolRoute(const CTaskComplexFollowPatrolRoute&);
    ~CTaskComplexFollowPatrolRoute() override;

    CTask*    Clone() const override { return new CTaskComplexFollowPatrolRoute(*this); }
    eTaskType GetTaskType() const override { return Type; }
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override;

    static void InjectHooks();

private:
    eTaskType GetNextSubTaskType();
    CTask*    CreateSubTask(eTaskType taskType);

private:
    ePatrolRouteModeS16 m_Mode{};
    int16         m_RouteTraversals{};
    eMoveStateS16 m_MoveState{};
    int16               m_Progress{}; //!< Index of the route point the ped is going to
    float         m_TargetRadius{};
    float         m_SlowDownDistance{};
    CPatrolRoute* m_Route{};
    bool          m_IsNewRoute : 1 {};
    bool          m_IsAborting : 1 {};
    CVector       m_PedPositionWhenAborted{};
};
VALIDATE_SIZE(CTaskComplexFollowPatrolRoute, 0x30);
