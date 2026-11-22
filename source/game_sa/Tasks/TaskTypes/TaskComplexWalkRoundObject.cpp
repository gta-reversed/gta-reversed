#include "StdInc.h"

#include "TaskComplexFollowPointRoute.h"
#include "TaskComplexWalkRoundObject.h"
#include "TaskComplexGoToPointAndStandStill.h"
#include "TaskSimpleAchieveHeading.h"
#include "TaskSimpleStandStill.h"
#include "PedGeometryAnalyser.h"

void CTaskComplexWalkRoundObject::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexWalkRoundObject, 0x86f364, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedConstructorInstall(0x655020, "1", {}, int32, CVector const&, CEntity *);
    RH_ScopedDestructorInstall(0x6550D0);

    RH_ScopedInstall(CreateRouteTask, 0x655140);
    RH_ScopedInstall(CreateSubTask, 0x655290);
    RH_ScopedInstall(ComputeRouteAndDistToHitSide, 0x6551D0);

    RH_ScopedVMTInstall(Clone, 0x655C10);
    RH_ScopedVMTInstall(GetTaskType, 0x6550C0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x657220);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x657380);
    RH_ScopedVMTInstall(ControlSubTask, 0x6575F0);
}

// 0x655020
CTaskComplexWalkRoundObject::CTaskComplexWalkRoundObject(int32 moveState, const CVector& targetPoint, CEntity* object) :
    CTaskComplex() {
    m_moveState   = moveState;
    m_targetPoint = targetPoint;
    m_object      = object;

    CEntity::SafeRegisterRef(m_object);

    m_pointRoute = new CPointRoute();
}

CTaskComplexWalkRoundObject::~CTaskComplexWalkRoundObject() {
    CEntity::SafeCleanUpRef(m_object);

    delete std::exchange(m_pointRoute, nullptr);
}

// 0x6575F0
CTask* CTaskComplexWalkRoundObject::ControlSubTask(CPed* ped) {
    const auto ShouldKeepSubTask = [&] {
        if (!m_object) {
            return false;
        }
        if (m_pSubTask->GetTaskType() == TASK_COMPLEX_FOLLOW_POINT_ROUTE && m_timer.IsOutOfTime()) {
            return false; // Took too long to walk round it
        }
        if (ped->GetIntelligence()->m_AnotherStaticCounter > 30) {
            return false; // Ped is stuck
        }
        if (m_pSubTask->GetTaskType() == TASK_SIMPLE_STAND_STILL) {
            return true;
        }
        // The route is only valid as long as the object hasn't moved/rotated
        const auto& mat = m_object->GetMatrix();
        return (m_objectPos - m_object->GetPosition()).SquaredMagnitude() <= sq(0.25f)
            && m_objectForward.Dot(mat.GetForward()) >= 0.9f
            && m_objectRight.Dot(mat.GetRight()) >= 0.9f;
    };
    if (ShouldKeepSubTask()) {
        return m_pSubTask;
    }
    if (m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_URGENT, nullptr)) {
        return CreateSubTask(TASK_SIMPLE_STAND_STILL, ped);
    }
    return m_pSubTask;
}

// 0x657220
CTask* CTaskComplexWalkRoundObject::CreateNextSubTask(CPed* ped) {
    if (!m_object) {
        return nullptr;
    }
    switch (m_pSubTask->GetTaskType()) {
    case TASK_SIMPLE_ACHIEVE_HEADING: {
        if (!m_pointRoute->IsEmpty()) {
            return CreateRouteTask(ped);
        }
        // No route: go to the closest point on the object's surface first
        CVector closestPt;
        CPedGeometryAnalyser::ComputeClosestSurfacePoint(*ped, *m_object, closestPt);
        if (CPedGeometryAnalyser::ComputeEntityHitSide(closestPt, *m_object) != CPedGeometryAnalyser::ComputeEntityHitSide(m_targetPoint, *m_object)) {
            return new CTaskComplexGoToPointAndStandStill(PEDMOVE_WALK, closestPt, 0.5f, 2.f, false, false);
        }
        // Same side as the target, so just walk along that side
        m_pointRoute->AddUnlessFull(closestPt);
        CPedGeometryAnalyser::ComputeClosestSurfacePoint(m_targetPoint, *m_object, closestPt);
        m_pointRoute->AddUnlessFull(closestPt);
        return CreateRouteTask(ped);
    }
    case TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL:
        return CreateRouteTask(ped);
    case TASK_SIMPLE_STAND_STILL:
    case TASK_COMPLEX_FOLLOW_POINT_ROUTE:
    default:
        return nullptr;
    }
}

// 0x657380
CTask* CTaskComplexWalkRoundObject::CreateFirstSubTask(CPed* ped) {
    if (!m_object) {
        return nullptr;
    }

    m_pointRoute->Clear();
    const auto distToHitSide = ComputeRouteAndDistToHitSide(ped);
    if (m_pointRoute->IsEmpty()) {
        return nullptr;
    }

    m_timer.Start(m_moveState == PEDMOVE_WALK ? 8'000 : 4'000);

    // Remember the object's position/orientation, if it changes the route is invalid (See `ControlSubTask`)
    m_objectPos               = m_object->GetPosition();
    m_objectForward           = m_object->GetMatrix().GetForward();
    m_objectRight             = m_object->GetMatrix().GetRight();

    const auto& pedPos        = ped->GetPosition();
    const auto  toFirstPt     = (*m_pointRoute)[0] - pedPos;
    const auto  distToFirstPt = CVector{ toFirstPt }.NormaliseAndMag();
    const auto  minDist       = [this] {
        switch (m_moveState) {
        case PEDMOVE_WALK: return 2.f;
        case PEDMOVE_RUN:  return 4.f;
        default:           return 6.f;
        }
    }();
    if (distToFirstPt > minDist && distToHitSide > minDist && toFirstPt.Dot(ped->GetForward()) > 0.f) {
        return CreateRouteTask(ped);
    }

    // Too close (or facing away), turn towards the first point of the route first
    return new CTaskSimpleAchieveHeading(
        CGeneral::LimitRadianAngle(CGeneral::GetRadianAngleBetweenPoints(toFirstPt.x, toFirstPt.y, 0.f, 0.f)),
        1.f,
        0.2f
    );
}

// 0x655140
CTask* CTaskComplexWalkRoundObject::CreateRouteTask(CPed*) const {
    if (m_pointRoute->IsEmpty()) {
        return nullptr;
    }
    return new CTaskComplexFollowPointRoute{ (eMoveState)m_moveState, *m_pointRoute };
}

// 0x655290
CTask* CTaskComplexWalkRoundObject::CreateSubTask(eTaskType taskType, CPed* ped) const {
    switch (taskType) {
    case TASK_SIMPLE_STAND_STILL:
        return new CTaskSimpleStandStill(500, false, false, 8.f);
    case TASK_COMPLEX_FOLLOW_POINT_ROUTE:
        return CreateRouteTask(ped);
    default:
        return nullptr;
    }
}

// 0x6551D0
float CTaskComplexWalkRoundObject::ComputeRouteAndDistToHitSide(CPed* ped) {
    // Compute the route with a bigger ped radius than usual
    const auto oldPedRadius = std::exchange(CPedGeometryAnalyser::ms_fPedNominalRadius, 0.7f);
    CPedGeometryAnalyser::ComputeRouteRoundEntityBoundingBox(*ped, *m_object, m_targetPoint, *m_pointRoute, 0);
    CPedGeometryAnalyser::ms_fPedNominalRadius = oldPedRadius;

    // Distance of the ped from the side of the object it's at
    const auto&            pedPos = ped->GetPosition();
    std::array<CVector, 4> planeNormals;
    std::array<float, 4>   planeDs;
    CPedGeometryAnalyser::ComputeEntityBoundingBoxPlanes(pedPos.z, *m_object, planeNormals, planeDs);
    const auto side = +CPedGeometryAnalyser::ComputeEntityHitSide(pedPos, *m_object);
    return planeNormals[side].Dot(pedPos) + planeDs[side];
}
