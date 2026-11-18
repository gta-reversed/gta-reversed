#include "StdInc.h"
#include "TaskComplexFollowPatrolRoute.h"
#include "PatrolRoute.h"

void CTaskComplexFollowPatrolRoute::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexFollowPatrolRoute, 0x870114, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedConstructorInstall(0x674930, "1", {}, eMoveStateS32, const CPatrolRoute&, int32, float, float);
    RH_ScopedDestructorInstall(0x6709F0);

    RH_ScopedInstall(CreateSubTask, 0x670B00, { .Reversed = false });

    RH_ScopedVMTInstall(Clone, 0x674BB0);
    RH_ScopedVMTInstall(GetTaskType, 0x670A60);
    RH_ScopedVMTInstall(MakeAbortable, 0x66BA90, { .Reversed = false });
    RH_ScopedVMTInstall(CreateNextSubTask, 0x670D80, { .Reversed = false });
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x670E20, { .Reversed = false });
    RH_ScopedVMTInstall(ControlSubTask, 0x66BB00, { .Reversed = false });
}

// 0x674930
CTaskComplexFollowPatrolRoute::CTaskComplexFollowPatrolRoute(eMoveStateS32 moveState, const CPatrolRoute& route, int32 mode, float targetRadius, float slowDownDistance) :
    m_MoveState(moveState),
    m_Route(new CPatrolRoute(route)),
    m_Mode(mode),
    m_TargetRadius(targetRadius),
    m_SlowDownDistance(slowDownDistance) {
}

// 0x674BB0
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

// 0x670B00
void CTaskComplexFollowPatrolRoute::CreateSubTask(int32 moveStateRadius) {
    return plugin::CallMethodAndReturn<void, 0x670B00, CTaskComplexFollowPatrolRoute*, int32>(this, moveStateRadius);
}

// 0x66BA90
bool CTaskComplexFollowPatrolRoute::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    return plugin::CallMethodAndReturn<bool, 0x66BA90, CTaskComplexFollowPatrolRoute*, CPed*, int32, const CEvent*>(this, ped, priority, event);
}

// 0x670D80
CTask* CTaskComplexFollowPatrolRoute::CreateNextSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x670D80, CTaskComplexFollowPatrolRoute*, CPed*>(this, ped);
}

// 0x670E20
CTask* CTaskComplexFollowPatrolRoute::CreateFirstSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x670E20, CTaskComplexFollowPatrolRoute*, CPed*>(this, ped);
}

// 0x66BB00
CTask* CTaskComplexFollowPatrolRoute::ControlSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x66BB00, CTaskComplexFollowPatrolRoute*, CPed*>(this, ped);
}
