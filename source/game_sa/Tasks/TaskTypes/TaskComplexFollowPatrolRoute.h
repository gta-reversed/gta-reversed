#pragma once

#include "Vector.h"
#include "TaskComplex.h"
#include "PatrolRoute.h"
#include <Enums/eMoveState.h>

class CPed;
class CEvent;
class CPatrolRoute;

class NOTSA_EXPORT_VTABLE CTaskComplexFollowPatrolRoute : public CTaskComplex {
public:
    static constexpr auto Type = TASK_COMPLEX_FOLLOW_PATROL_ROUTE;

    static void InjectHooks();

    CTaskComplexFollowPatrolRoute(eMoveStateS32 moveState, const CPatrolRoute& route, int32 mode, float targetRadius, float slowDownDistance);
    CTaskComplexFollowPatrolRoute(const CTaskComplexFollowPatrolRoute&);
    ~CTaskComplexFollowPatrolRoute();

    CTask*    Clone() const override { return new CTaskComplexFollowPatrolRoute(*this); }
    eTaskType GetTaskType() const override { return Type; }
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override;

private:
    void CreateSubTask(int32 moveStateRadius);

public:
    int16         m_Mode{};
    int16         m_RouteTraversals{};
    eMoveStateS16 m_MoveState{};
    int16         m_Progress{};
    float         m_TargetRadius{};
    float         m_SlowDownDistance{};
    CPatrolRoute* m_Route{};
    bool          m_IsNewRoute : 1 {};
    bool          m_IsAborting : 1 {};
    CVector       m_PedPositionWhenAborted{};
};
