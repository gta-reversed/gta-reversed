#include "StdInc.h"

#include <reversiblehooks/ReversibleHooks.h>
#include <PedGeometryAnalyser.h>

#include "TaskComplexFollowPointRoute.h"
#include "TaskComplexWalkRoundObject.h"
#include "TaskComplexGoToPointAndStandStill.h"
#include "TaskSimpleAchieveHeading.h"
#include "TaskSimpleStandStill.h"

void CTaskComplexWalkRoundObject::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexWalkRoundObject, 0x86f364, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedConstructorInstall(0x655020, "1", {}, int32, const CVector&, CEntity*);
    RH_ScopedDestructorInstall(0x6550D0);

    RH_ScopedInstall(CreateSubTask, 0x655290);
    RH_ScopedInstall(CreateRouteTask, 0x655140);
    RH_ScopedInstall(ComputeRoute, 0x6551D0, { .Reversed = false });
    RH_ScopedVMTInstall(Clone, 0x655C10);
    RH_ScopedVMTInstall(GetTaskType, 0x6550C0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x657220);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x657380);
    RH_ScopedVMTInstall(ControlSubTask, 0x6575F0);
}

// 0x655020
CTaskComplexWalkRoundObject::CTaskComplexWalkRoundObject(int32 moveState, const CVector& targetPoint, CEntity* object) :
    m_moveState{ moveState },
    m_targetPoint{ targetPoint },
    m_object{ object },
    m_pointRoute{ new CPointRoute() } {
}

// 0x6550D0
CTaskComplexWalkRoundObject::~CTaskComplexWalkRoundObject() {
    delete std::exchange(m_pointRoute, nullptr);
}

// 0x6575F0
CTask* CTaskComplexWalkRoundObject::ControlSubTask(CPed* ped) {
    if (m_object) {
        if (!notsa::isa<CTaskComplexFollowPointRoute>(m_pSubTask) || !m_Timer.IsOutOfTime()) {
            if (ped->GetIntelligence()->m_AnotherStaticCounter <= 30) {
                if (notsa::isa<CTaskSimpleStandStill>(m_pSubTask)) {
                    return m_pSubTask;
                }
                if (CVector::DistSqr(m_ObjectPos, ped->GetPosition()) <= sq(0.25f)) {
                    if (m_ObjectForwardsVec.Dot(ped->GetForward()) >= 0.9f) {
                        if (m_ObjectRightVec.Dot(ped->GetRight()) >= 0.9f) {
                            return m_pSubTask;
                        }
                    }
                }
            }
        }
    }
    if (!m_pSubTask->MakeAbortable(ped)) {
        return m_pSubTask;
    }
    return CreateSubTask(TASK_SIMPLE_STAND_STILL, ped);
}

// 0x657220
CTask* CTaskComplexWalkRoundObject::CreateNextSubTask(CPed* ped) {
    switch (const auto stt = m_pSubTask->GetTaskType()) {
    case TASK_SIMPLE_STAND_STILL:
    case TASK_COMPLEX_FOLLOW_POINT_ROUTE:
        return nullptr;
    case TASK_SIMPLE_ACHIEVE_HEADING: {
        if (m_pointRoute->IsEmpty()) {
            if (CVector from; CPedGeometryAnalyser::ComputeClosestSurfacePoint(*ped, *m_object, from) || notsa::bugfixes::GenericUB) {
                if (CPedGeometryAnalyser::ComputeEntityHitSide(from, *m_object) == CPedGeometryAnalyser::ComputeEntityHitSide(m_targetPoint, *m_object)) { // 0x6572cd
                    if (CVector target; CPedGeometryAnalyser::ComputeClosestSurfacePoint(m_targetPoint, *m_object, target) || notsa::bugfixes::GenericUB) {
                        m_pointRoute->Add(from);
                        m_pointRoute->Add(target);
                        return CreateSubTask(TASK_COMPLEX_FOLLOW_POINT_ROUTE, ped); // 0x657309
                    }
                }
                return new CTaskComplexGoToPointAndStandStill{ PEDMOVE_WALK, from, 0.5f };
            }
            return nullptr; /* bugfix */
        }
        return CreateSubTask(TASK_COMPLEX_FOLLOW_POINT_ROUTE, ped); // 0x657295
    }
    case TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL:
        return CreateSubTask(TASK_COMPLEX_FOLLOW_POINT_ROUTE, ped); // 0x657295
    default:
        NOTSA_UNREACHABLE_CASE(stt);
    }
}

// 0x657380
CTask* CTaskComplexWalkRoundObject::CreateFirstSubTask(CPed* ped) {
    if (!m_object) {
        return nullptr;
    }

    const auto routeLen = ComputeRoute(ped);
    if (m_pointRoute->IsEmpty()) {
        return nullptr;
    }

    m_Timer.Start(m_moveState == PEDMOVE_WALK ? 8'000 : 4'000);

    m_ObjectForwardsVec                          = m_object->GetForward();
    m_ObjectRightVec                             = m_object->GetRight();
    m_ObjectPos                                  = m_object->GetPosition();
    const auto [startToObjDir, startToObjDirLen] = ((*m_pointRoute)[0] - m_ObjectPos).NormalizedAndMag();
    const auto maxDistInInterval =
        m_moveState == PEDMOVE_WALK ? 2.f : m_moveState == PEDMOVE_RUN ? 4.f
                                                                       : 8.f;
    if (startToObjDirLen > maxDistInInterval && routeLen > maxDistInInterval && startToObjDir.Dot(ped->GetForward()) > 0.f) {
        return CreateSubTask(TASK_COMPLEX_FOLLOW_POINT_ROUTE, ped);
    }

    return new CTaskSimpleAchieveHeading{
        ((*m_pointRoute)[0] - ped->GetPosition2D()).Heading() // Using Heading() instead of `CGeneral::GetRadianAngleBetweenPoints`
    };
}

// 0x655290
CTask* CTaskComplexWalkRoundObject::CreateSubTask(eTaskType taskType, CPed* ped) const {
    switch (taskType) {
    case TASK_SIMPLE_STAND_STILL:
        return new CTaskSimpleStandStill{};
    case TASK_COMPLEX_FOLLOW_POINT_ROUTE:
        return CreateRouteTask(ped);
    case TASK_FINISHED:
        return nullptr;
    default:
        NOTSA_UNREACHABLE_CASE(taskType);
    }
}

// 0x655140
CTask* CTaskComplexWalkRoundObject::CreateRouteTask(CPed*) const {
    if (m_pointRoute->IsEmpty()) {
        return nullptr;
    }
    return new CTaskComplexFollowPointRoute{ (eMoveState)m_moveState, *m_pointRoute };
}

// 0x6551D0
float CTaskComplexWalkRoundObject::ComputeRoute(CPed* ped) const {
    return plugin::CallMethodAndReturn<float, 0x6551D0>(this, ped);
}
