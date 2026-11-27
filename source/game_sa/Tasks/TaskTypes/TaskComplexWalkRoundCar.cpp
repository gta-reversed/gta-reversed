#include "StdInc.h"

#include "TaskComplexWalkRoundCar.h"
#include "TaskComplexFollowPointRoute.h"
#include "TaskComplexEnterCarAsDriver.h"
#include "TaskSimpleAchieveHeading.h"
#include "TaskSimpleStandStill.h"
#include "PedGeometryAnalyser.h"
#include "CarEnterExit.h"

void CTaskComplexWalkRoundCar::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexWalkRoundCar, 0x86f308, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedConstructorInstall(0x6541B0, "1", {}, eMoveState, const CVector&, CVehicle*, bool, uint8);
    RH_ScopedDestructorInstall(0x656B00);

    RH_ScopedInstall(SetNewVehicle, 0x654290);
    RH_ScopedInstall(CreateRouteTask, 0x6542E0);
    RH_ScopedInstall(ComputeRouteRoundSmallCar, 0x6544F0);
    RH_ScopedInstall(GoingForDoor, 0x654720);
    RH_ScopedInstall(ComputeRouteRoundBigCar, 0x656BB0);
    RH_ScopedInstall(ComputeRoute, 0x657B80);

    RH_ScopedVMTInstall(Clone, 0x655B00);
    RH_ScopedVMTInstall(GetTaskType, 0x654280);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x656B70);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x658200);
    RH_ScopedVMTInstall(ControlSubTask, 0x654370);
}

// 0x6541B0
CTaskComplexWalkRoundCar::CTaskComplexWalkRoundCar(eMoveState moveState, const CVector& targetPt, CVehicle* vehicle, bool isPedGoingForCarDoor, uint8 forceThisDirectionRoundCar) :
    m_MoveState{ (uint8)(moveState) },
    m_IsPedGoingForCarDoor{ isPedGoingForCarDoor },
    m_ForceThisDirectionRoundCar{ forceThisDirectionRoundCar },
    m_TargetPt{ targetPt },
    m_Veh{ vehicle },
    m_Route{ new CPointRoute{} } {
}

// 0x655B00
CTaskComplexWalkRoundCar::CTaskComplexWalkRoundCar(const CTaskComplexWalkRoundCar& o) :
    CTaskComplexWalkRoundCar{
        (eMoveState)(o.m_MoveState),
        o.m_TargetPt,
        o.m_Veh,
        o.m_IsPedGoingForCarDoor,
        o.m_ForceThisDirectionRoundCar
    } {
}

// 0x656B00
CTaskComplexWalkRoundCar::~CTaskComplexWalkRoundCar() {
    delete m_Route;
}

// 0x654290
void CTaskComplexWalkRoundCar::SetNewVehicle(CVehicle* vehicle, uint8 forceThisDirectionRoundCar) {
    if (notsa::IsFixBugs()) {
        if (vehicle == m_Veh) {
            return;
        }
    }
    m_Veh                          = vehicle;
    m_ForceThisDirectionRoundCar   = forceThisDirectionRoundCar;
    m_FirstSubTaskNeedsToBeCreated = true;
    m_Route->Clear();
}

// 0x6542E0
CTask* CTaskComplexWalkRoundCar::CreateRouteTask(CPed*) const {
    if (m_Route->IsEmpty()) {
        return nullptr;
    }
    return new CTaskComplexFollowPointRoute{ (eMoveState)(m_MoveState), *m_Route };
}

// 0x6544F0
float CTaskComplexWalkRoundCar::ComputeRouteRoundSmallCar(CPed* ped) {
    // NOTE: The original also computes the corners of the car's bounding box here (0x65451D), but never uses them
    std::array<CVector, 4> planeNormals;
    std::array<float, 4>   planeDs;
    CPedGeometryAnalyser::ComputeEntityBoundingBoxPlanes(ped->GetPosition().z, *m_Veh, planeNormals, planeDs);

    // Distance of the ped from the side of the car it's at
    // NOTE: The original returns 0 if the side is -1 (0x65455B), but `ComputeEntityHitSide` never returns that
    const auto pedSide       = +CPedGeometryAnalyser::ComputeEntityHitSide(*ped, *m_Veh);
    const auto distToHitSide = planeNormals[pedSide].Dot(ped->GetPosition()) + planeDs[pedSide];

    if (m_IsPedGoingForCarDoor) { // 0x6545A4
        // The target (a door) is inside the bounding box: move it just outside of it, to the side it's at
        const auto GetTargetDistToSide = [&](size_t side) {
            return planeNormals[side].Dot(m_TargetPt) + planeDs[side];
        };
        const auto IsTargetBehindSide = [&](size_t side) {
            return GetTargetDistToSide(side) < 0.f;
        };
        if (rng::all_of(rngv::iota(0u, planeNormals.size()), IsTargetBehindSide)) {
            // NOTE: The original checks this side against -1 too (0x654689)
            const auto targetSide = +CPedGeometryAnalyser::ComputeEntityHitSide(m_TargetPt, *m_Veh);
            m_TargetPt += planeNormals[targetSide] * (0.05f - GetTargetDistToSide(targetSide));
        }
    }

    m_DirectionGoingRoundCar = CPedGeometryAnalyser::ComputeRouteRoundEntityBoundingBox(*ped, *m_Veh, m_TargetPt, *m_Route, m_ForceThisDirectionRoundCar);

    return distToHitSide;
}

// 0x656BB0
float CTaskComplexWalkRoundCar::ComputeRouteRoundBigCar(CPed* ped) {
    // Closest point on the car's surface, computed with a bigger ped radius than usual
    const auto GetClosestSurfacePoint = [this](const auto& pedOrPoint) {
        const auto oldPedRadius = std::exchange(CPedGeometryAnalyser::ms_fPedNominalRadius, 0.7f);
        CVector    surfacePt;
        CPedGeometryAnalyser::ComputeClosestSurfacePoint(pedOrPoint, *m_Veh, surfacePt);
        CPedGeometryAnalyser::ms_fPedNominalRadius = oldPedRadius;
        return surfacePt;
    };

    if (CPedGeometryAnalyser::ComputeEntityHitSide(*ped, *m_Veh) == CPedGeometryAnalyser::ComputeEntityHitSide(m_TargetPt, *m_Veh)) { // 0x656BE3
        // Same side as the target, so just walk along that side
        m_Route->AddUnlessFull(GetClosestSurfacePoint(*ped));
    } else { // 0x656C52
        const CVector pedSurfacePt    = GetClosestSurfacePoint(*ped);
        const CVector targetSurfacePt = GetClosestSurfacePoint(m_TargetPt);

        // Route round the car as if the ped and the target were on its surface
        const CVector pedPos   = ped->GetPosition();
        const CVector targetPt = std::exchange(m_TargetPt, targetSurfacePt);
        ped->SetPosn(pedSurfacePt);
        ComputeRouteRoundSmallCar(ped);
        m_TargetPt = targetPt;
        ped->SetPosn(pedPos);

        // Walk to the car's surface first
        const CPointRoute routeRoundCar = *m_Route;
        m_Route->Clear();
        m_Route->AddUnlessFull(pedSurfacePt);
        for (const auto& pt : routeRoundCar.GetAll()) {
            m_Route->AddUnlessFull(pt);
        }
    }
    m_Route->AddUnlessFull(GetClosestSurfacePoint(m_TargetPt));

    return 0.f;
}

// 0x657B80
float CTaskComplexWalkRoundCar::ComputeRoute(CPed* ped) {
    return m_Veh->vehicleFlags.bIsBig
        ? ComputeRouteRoundBigCar(ped)
        : ComputeRouteRoundSmallCar(ped);
}

// 0x656B70
CTask* CTaskComplexWalkRoundCar::CreateNextSubTask(CPed* ped) {
    switch (const auto tt = m_pSubTask->GetTaskType()) {
    case TASK_SIMPLE_ACHIEVE_HEADING: // 0x656B91
        return CreateRouteTask(ped);
    case TASK_COMPLEX_FOLLOW_POINT_ROUTE: // 0x656B8A
        return nullptr;
    default:
        NOTSA_UNREACHABLE_CASE(tt);
    }
}

// 0x658200
CTask* CTaskComplexWalkRoundCar::CreateFirstSubTask(CPed* ped) {
    if (notsa::bugfixes::GenericCrashing) {
        if (!m_Veh) {
            return nullptr;
        }
    }

    m_Route->Clear();
    const auto distToHitSide = ComputeRoute(ped);
    if (m_Route->IsEmpty()) {
        return nullptr;
    }

    // A car that tows or is towed (a longer one) takes longer to walk round
    const auto walkRoundTimeMs = m_MoveState == PEDMOVE_WALK ? 20'000 : 15'000;
    m_Timer.Start(m_Veh->m_pVehicleBeingTowed || m_Veh->m_pTowingVehicle ? (int32)((float)(walkRoundTimeMs) * 4.f) : walkRoundTimeMs);

    // Remember the car's position/orientation, if it changes the route is invalid (See `ControlSubTask`)
    m_VehPos                  = m_Veh->GetPosition();
    m_VehMatFwd               = m_Veh->GetForward();
    m_VehMatRight             = m_Veh->GetRight();

    const auto& pedPos        = ped->GetPosition();
    CVector     dirToFirstPt  = (*m_Route)[0] - pedPos;
    const auto  distToFirstPt = dirToFirstPt.NormaliseAndMag();
    const auto  minDist       = m_MoveState == PEDMOVE_WALK ? 2.f : (m_MoveState == PEDMOVE_RUN ? 4.f : 6.f);

    if (ped->IsPlayer()) {
        // NOTE: Not `Find<CTaskComplexEnterCarAsDriver>`, the original only looks in the primary task (0x6583A4)
        if (auto* const tEnterCar = ped->GetTaskManager().FindTaskByType(TASK_PRIMARY_PRIMARY, TASK_COMPLEX_ENTER_CAR_AS_DRIVER)) {
            m_EnterCarStartTime = notsa::cast<CTaskComplexEnterCarAsDriver>(tEnterCar)->GetEnterCarStartTime();
        }
    }

    if (!m_Veh->vehicleFlags.bIsBig && distToFirstPt > minDist && distToHitSide > minDist && dirToFirstPt.Dot(ped->GetForward()) > 0.f) {
        return CreateRouteTask(ped);
    }

    // Too close (or facing away), stop and turn towards the first point of the route first
    CTaskSimpleStandStill{}.ProcessPed(ped);

    // NOTE: Not `.Heading()`, it differs from the original for a zero `y`
    return new CTaskSimpleAchieveHeading{
        CGeneral::LimitRadianAngle(CGeneral::GetRadianAngleBetweenPoints((*m_Route)[0].x - pedPos.x, (*m_Route)[0].y - pedPos.y, 0.f, 0.f)),
        1.f,
        0.1f
    };
}

// 0x654370
CTask* CTaskComplexWalkRoundCar::ControlSubTask(CPed* ped) {
    // NOTE: Not `std::exchange`, it's a bit field
    if (m_FirstSubTaskNeedsToBeCreated) { // 0x65437A
        m_FirstSubTaskNeedsToBeCreated = false;
        return CreateFirstSubTask(ped);
    }

    const auto IsFollowingRoute = [this] {
        return notsa::isa<CTaskComplexFollowPointRoute>(m_pSubTask);
    };
    const auto playerQuitsEnteringCar = ped->IsPlayer()
        && m_Veh
        && m_EnterCarStartTime != -1
        && CCarEnterExit::IsPlayerToQuitCarEnter(ped, m_Veh, m_EnterCarStartTime, m_pSubTask)
        && IsFollowingRoute();
    const auto isTimeUp = !playerQuitsEnteringCar && IsFollowingRoute() && m_Timer.IsOutOfTime(); // Took too long to walk round it

    // The route is only valid as long as the car hasn't moved/rotated
    // NOTE: Not `CVector::DistSqr`, that's 2D on master, the original's distance is 3D (0x654419)
    if (!playerQuitsEnteringCar && !isTimeUp && m_Veh) { // 0x6543FD
        if ((m_VehPos - m_Veh->GetPosition()).SquaredMagnitude() <= sq(0.25f) && m_VehMatFwd.Dot(m_Veh->GetForward()) >= 0.9f && m_VehMatRight.Dot(m_Veh->GetRight()) >= 0.9f) {
            return m_pSubTask;
        }
    }

    if (!m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_URGENT, nullptr)) {
        return m_pSubTask;
    }
    if (playerQuitsEnteringCar) { // 0x6544AB
        if (auto* const tEnterCar = notsa::dyn_cast_if_present<CTaskComplexEnterCarAsDriver>(ped->GetTaskManager().GetTaskPrimary(TASK_PRIMARY_PRIMARY))) {
            tEnterCar->MakeAbortable(ped, ABORT_PRIORITY_LEISURE, nullptr);
        }
    }
    return nullptr;
}
