#include "StdInc.h"

#include "PedGeometryAnalyser.h"

// 0x8D22B0: Bounding-box expansion/clipping tolerance (0.35). Writable; temporarily halved by `ComputeRouteRoundEntityBoundingBox`.
static auto& s_fBBoxEpsilon = StaticRef<float>(0x8D22B0);

void CPedGeometryAnalyser::InjectHooks() {
    RH_ScopedClass(CPedGeometryAnalyser);
    RH_ScopedCategoryGlobal();

    RH_ScopedOverloadedInstall(CanPedJumpObstacle, "", 0x5F1B00, bool(*)(const CPed&,const CEntity&));
    RH_ScopedOverloadedInstall(CanPedJumpObstacle, "contacted", 0x5F32D0, bool(*)(const CPed&,const CEntity&,const CVector&,const CVector&), { .reversed = false });
    RH_ScopedInstall(CanPedTargetPed, 0x5F1C40);
    RH_ScopedInstall(CanPedTargetPoint, 0x5F1B70);
    RH_ScopedInstall(ComputeBuildingHitPoints, 0x5F1E30);
    RH_ScopedInstall(ComputeClearTarget, 0x5F5D80);
    RH_ScopedOverloadedInstall(ComputeClosestSurfacePoint, "ped", 0x5F3B70, bool (*)(const CPed& ped, CEntity& entity, CVector& point));
    RH_ScopedOverloadedInstall(ComputeClosestSurfacePoint, "posn", 0x5F36F0, bool(*)(const CVector&,CEntity&,CVector&));
    RH_ScopedOverloadedInstall(ComputeClosestSurfacePoint, "rect", 0x5F2C10, bool(*)(const CVector&,const CVector*,CVector&));
    RH_ScopedInstall(ComputeEntityBoundingBoxCentreUncached, 0x5F1600);
    RH_ScopedInstall(ComputeEntityBoundingBoxCentreUncachedAll, 0x5F3B40);
    RH_ScopedInstall(ComputeEntityBoundingBoxCorners, 0x5F3650);
    RH_ScopedInstall(ComputeEntityBoundingBoxCornersUncached, 0x5F1FA0, { .reversed = false });
    RH_ScopedInstall(ComputeEntityBoundingBoxPlanes, 0x5F3660);
    RH_ScopedInstall(ComputeEntityBoundingBoxPlanesUncached, 0x5F1670);
    RH_ScopedInstall(ComputeEntityBoundingBoxPlanesUncachedAll, 0x5F2B80);
    RH_ScopedInstall(ComputeEntityBoundingBoxSegmentPlanes, 0x5F36A0);
    RH_ScopedInstall(ComputeEntityBoundingBoxSegmentPlanesUncached, 0x5F1750);
    RH_ScopedInstall(ComputeEntityBoundingBoxSegmentPlanesUncachedAll, 0x5F2BC0);
    RH_ScopedInstall(ComputeEntityBoundingSphere, 0x5F3C20);
    RH_ScopedInstall(ComputeMoveDirToAvoidEntity, 0x5F3730);
    RH_ScopedInstall(ComputeEntityDirs, 0x5F1500);
    RH_ScopedOverloadedInstall(ComputeEntityHitSide, "1", 0x5F3BC0, int32 (*)(const CPed& ped, CEntity& entity));
    RH_ScopedOverloadedInstall(ComputeEntityHitSide, "2", 0x5F1450, int32 (*)(const CVector& point1, const CVector* point2, const float* x));
    RH_ScopedOverloadedInstall(ComputeEntityHitSide, "3", 0x5F3AC0, int32 (*)(const CVector& point, CEntity& entity));
    RH_ScopedOverloadedInstall(ComputePedHitSide, "physical", 0x5F3640, int32(*)(const CPed&,const CPhysical&));
    RH_ScopedOverloadedInstall(ComputePedHitSide, "posn", 0x5F1E70, int32(*)(const CPed&,const CVector&));
    RH_ScopedInstall(ComputePedShotSide, 0x5F13F0);
    RH_ScopedOverloadedInstall(ComputeRouteRoundEntityBoundingBox, "1", 0x5F6110, int32(*)(const CPed&,CEntity&,const CVector&,CPointRoute&,int32));
    RH_ScopedOverloadedInstall(ComputeRouteRoundEntityBoundingBox, "2", 0x5F3DD0, int32(*)(const CPed&,const CVector&,CEntity&,const CVector&,CPointRoute&,int32), { .reversed = false });
    RH_ScopedInstall(ComputeRouteRoundSphere, 0x5F1890);
    RH_ScopedOverloadedInstall(GetIsLineOfSightClear, "ped", 0x5F5A30, bool(*)(const CPed&,const CVector&,CEntity&,float&));
    RH_ScopedOverloadedInstall(GetIsLineOfSightClear, "v3d", 0x5F2F00, bool(*)(const CVector&,const CVector&,CEntity&));
    RH_ScopedInstall(GetNearestPed, 0x5F3590);
    RH_ScopedInstall(IsEntityBlockingTarget, 0x5F3970);
    RH_ScopedInstall(IsInAir, 0x5F1CB0);
    RH_ScopedInstall(IsWanderPathClear, 0x5F2F70);
    RH_ScopedInstall(LiesInsideBoundingBox, 0x5F3880);
}

// 0x5F1B00
bool CPedGeometryAnalyser::CanPedJumpObstacle(const CPed& ped, const CEntity& entity) {
    if (entity.m_bIsTempBuilding) {
        return false;
    }
    const auto& posn = ped.GetPosition();
    return CWorld::GetIsLineOfSightClear(posn, posn + ped.GetForward(), true, false, false, true, false, false, false);
}

// 0x5F32D0
bool CPedGeometryAnalyser::CanPedJumpObstacle(const CPed& ped, const CEntity& entity, const CVector& contactNormal, const CVector& contactPos) {
    return plugin::CallAndReturn<bool, 0x5F32D0, CPed const&, CEntity const&, CVector const&, CVector const&>(ped, entity, contactNormal, contactPos);
}

// 0x5F1C40
bool CPedGeometryAnalyser::CanPedTargetPed(CPed& ped, CPed& targetPed, bool checkDirection) {
    return CanPedTargetPoint(
        ped,
        targetPed.GetPosition() + CVector{ 0.f, 0.f, targetPed.GetTaskManager().GetTaskSecondary(TASK_SECONDARY_DUCK) ? -0.25f : 0.75f }, // 0.75f - 1.f = -0.25f
        checkDirection
    );
}

// 0x5F1B70
bool CPedGeometryAnalyser::CanPedTargetPoint(const CPed& ped, const CVector& a2, bool a3) {
    const auto& posn        = ped.GetPosition();
    const auto  dirToTarget = a2 - posn;
    if (a3 && DotProduct(dirToTarget, ped.GetForward()) < 0.f) {
        return false;
    }
    if (dirToTarget.SquaredMagnitude() > 40.f * 40.f) {
        return false;
    }
    return CWorld::GetIsLineOfSightClear(posn + CVector{ 0.f, 0.f, 0.75f }, a2, true, false, false, true, false, true, false);
}

// 0x5F1E30
// unused
int32 CPedGeometryAnalyser::ComputeBuildingHitPoints(const CVector& a1, const CVector& a2) {
    CEntity*  outEntity;
    CColPoint v4;

    CWorld::ProcessLineOfSight(a1, a2, v4, outEntity, true, false, false, false, true, false, false, false);
    return CWorld::ms_iProcessLineNumCrossings;
}

// 0x5F5D80
void CPedGeometryAnalyser::ComputeClearTarget(const CPed& ped, const CVector& a2, CVector& a3) {
    a3                  = a2;

    const auto& pedPosn = ped.GetPosition();

    // If the target is inside a nearby ped/vehicle move it towards the ped until it's out of it
    const auto moveOutOfEntity = [&](CEntity** entities) {
        for (auto i = 0; i < 16; i++) {
            CEntity* entity = entities[i];
            if (!entity) {
                continue;
            }
            if ((entity->GetPosition() - a3).SquaredMagnitude() >= 5.f * 5.f) {
                continue;
            }
            if (!LiesInsideBoundingBox(ped, a3, *entity)) {
                continue;
            }
            float dist;
            if (GetIsLineOfSightClear(ped, a3, *entity, dist)) {
                continue;
            }
            a3 -= (a3 - pedPosn).Normalized() * (0.35f + dist);
        }
    };

    moveOutOfEntity(ped.GetIntelligence()->GetVehicleEntities());
    moveOutOfEntity(ped.GetIntelligence()->GetPedEntities());

    const auto step = (pedPosn - a3).Normalized() * 0.35f;
    for (auto i = 0; i < (int32)(5.f / 0.35f) + 1; i++) {
        if ((pedPosn - a3).SquaredMagnitude() >= 5.f * 5.f) {
            return;
        }
        if (DotProduct(pedPosn - a3, step) < 0.f) {
            return;
        }

        CColPoint colPoint;
        CEntity*  hitEntity;
        CWorld::ProcessLineOfSight(pedPosn, a3, colPoint, hitEntity, true, false, false, false, true, false, false, false);
        if (CWorld::ms_iProcessLineNumCrossings % 2 != 1) {
            break;
        }
        a3 += step;
    }
}

// 0x5F3B70
bool CPedGeometryAnalyser::ComputeClosestSurfacePoint(const CPed& ped, CEntity& entity, CVector& point) {
    CVector     corners[4];
    const auto& posn = ped.GetPosition();
    ComputeEntityBoundingBoxCornersUncached(posn.z, entity, corners);
    return ComputeClosestSurfacePoint(posn, corners, point);
}

// 0x5F36F0
bool CPedGeometryAnalyser::ComputeClosestSurfacePoint(const CVector& posn, CEntity& entity, CVector& point) {
    CVector corners[4];
    ComputeEntityBoundingBoxCornersUncached(posn.z, entity, corners);
    return ComputeClosestSurfacePoint(posn, corners, point);
}

// 0x5F2C10
bool CPedGeometryAnalyser::ComputeClosestSurfacePoint(const CVector& posn, const CVector* corners, CVector& point) {
    bool found     = false;
    auto minDistSq = FLT_MAX;

    // Try to project the point onto each of the (4) segments
    for (auto i = 0; i < 4; i++) {
        const auto diff   = corners[(i + 1) % 4] - corners[i];
        const auto segLen = diff.Magnitude();
        const auto segDir = diff * (1.f / segLen);
        const auto t      = DotProduct(posn - corners[i], segDir);
        if (t < 0.f || t > segLen) {
            continue;
        }
        const auto proj   = corners[i] + segDir * t;
        const auto distSq = (posn - proj).SquaredMagnitude();
        if (distSq < minDistSq) {
            minDistSq = distSq;
            point     = proj;
            found     = true;
        }
    }

    // Otherwise the closest point is one of the corners
    if (!found) {
        for (auto i = 0; i < 4; i++) {
            const auto distSq = (posn - corners[i]).SquaredMagnitude();
            if (distSq < minDistSq) {
                minDistSq = distSq;
                point     = corners[i];
                found     = true;
            }
        }
    }
    return found;
}

// inlined into CPedGeometryAnalyser::ComputeEntityBoundingSphere
void CPedGeometryAnalyser::ComputeEntityBoundingBoxCentre(float zPos, CEntity& entity, CVector& center) {
    ComputeEntityBoundingBoxCentreUncachedAll(zPos, entity, center);
}

// 0x5F1600
void CPedGeometryAnalyser::ComputeEntityBoundingBoxCentreUncached(float zPos, const CVector* corners, CVector& center) {
    center.Set(0.0f, 0.0f, zPos);

    center.x = corners[0].x;
    center.y = corners[0].y;

    center.x += corners[1].x;
    center.y += corners[1].y;

    center.x += corners[2].x;
    center.y += corners[2].y;

    center.x += corners[3].x;
    center.y += corners[3].y;

    center.x *= 0.25f;
    center.y *= 0.25f;
}

// 0x5F3B40
void CPedGeometryAnalyser::ComputeEntityBoundingBoxCentreUncachedAll(float zPos, CEntity& entity, CVector& center) {
    CVector corners[4];
    ComputeEntityBoundingBoxCornersUncached(zPos, entity, corners);
    ComputeEntityBoundingBoxCentreUncached(zPos, corners, center);
}

// 0x5F3650
void CPedGeometryAnalyser::ComputeEntityBoundingBoxCorners(float zPos, CEntity& entity, CVector* corners) {
    ComputeEntityBoundingBoxCornersUncached(zPos, entity, corners);
}

// 0x5F1FA0
void CPedGeometryAnalyser::ComputeEntityBoundingBoxCornersUncached(float zPos, CEntity& entity, CVector* corners) {
    plugin::Call<0x5F1FA0, float, CEntity&, void*>(zPos, entity, corners);
}

// 0x5F3660
void CPedGeometryAnalyser::ComputeEntityBoundingBoxPlanes(float zPos, CEntity& entity, CVector (*outPlanes)[4], float* outPlanesDot) {
    ComputeEntityBoundingBoxPlanesUncachedAll(zPos, entity, outPlanes, outPlanesDot);
}

// 0x5F1670
void CPedGeometryAnalyser::ComputeEntityBoundingBoxPlanesUncached(float zPos, const CVector* corners, CVector (*outPlanes)[4], float* outPlanesDot) {
    const CVector* corner2 = &corners[3];
    for (auto i = 0; i < 4; i++) {
        const CVector& corner    = corners[i];
        CVector&       plane     = (*outPlanes)[i];
        CVector        direction = corner - *corner2;
        direction.Normalise();
        plane.x = direction.y;
        plane.y = -direction.x;
        plane.z = 0.0f;
        // point-normal plane equation:
        // ax + by + cz + d = 0
        // d = - n . P
        outPlanesDot[i] = -DotProduct(plane, *corner2);

        corner2         = &corner;
    }
}

// 0x5F2B80
void CPedGeometryAnalyser::ComputeEntityBoundingBoxPlanesUncachedAll(float zPos, CEntity& entity, CVector (*outPlanes)[4], float* outPlanesDot) {
    CVector corners[4];
    CPedGeometryAnalyser::ComputeEntityBoundingBoxCornersUncached(zPos, entity, corners);
    CPedGeometryAnalyser::ComputeEntityBoundingBoxPlanesUncached(zPos, corners, outPlanes, outPlanesDot);
}

// 0x5F36A0
void CPedGeometryAnalyser::ComputeEntityBoundingBoxSegmentPlanes(float zPos, CEntity& entity, CVector* normals, float* dots) {
    ComputeEntityBoundingBoxSegmentPlanesUncachedAll(zPos, entity, normals, dots);
}

// 0x5F1750
CVector* CPedGeometryAnalyser::ComputeEntityBoundingBoxSegmentPlanesUncached(const CVector* corners, CVector& center, CVector* a3, float* a4) {
    for (auto i = 0; i < 4; i++) {
        const CVector& corner = corners[i];
        CVector&       plane  = a3[i];

        plane.x               = -(corner.y - center.y);
        plane.y               = corner.x - center.x;
        plane.z               = 0.f;

        a4[i]                 = -DotProduct(plane, corner);
    }
    return const_cast<CVector*>(corners);
}

// 0x5F2BC0
CVector* CPedGeometryAnalyser::ComputeEntityBoundingBoxSegmentPlanesUncachedAll(float zPos, CEntity& entity, CVector* a3, float* a4) {
    CVector corners[4];
    CVector center;

    ComputeEntityBoundingBoxCornersUncached(zPos, entity, corners);
    ComputeEntityBoundingBoxCentreUncached(zPos, corners, center);
    return ComputeEntityBoundingBoxSegmentPlanesUncached(corners, center, a3, a4);
}

// 0x5F3C20
void CPedGeometryAnalyser::ComputeEntityBoundingSphere(const CPed& ped, CEntity& entity, CColSphere& a3) {
    const auto zPos = ped.GetPosition().z;

    CVector corners[4];
    ComputeEntityBoundingBoxCornersUncached(zPos, entity, corners);

    CVector center;
    ComputeEntityBoundingBoxCentreUncachedAll(zPos, entity, center);

    auto maxDistSq = 0.f;
    for (const auto& corner : corners) {
        maxDistSq = std::max(maxDistSq, (corner - center).SquaredMagnitude());
    }
    a3.Set(std::sqrt(maxDistSq) * 1.1f, center, SURFACE_DEFAULT, 0, tColLighting{ 0xFF });
}

// 0x5F3730
int32 CPedGeometryAnalyser::ComputeMoveDirToAvoidEntity(const CPed& ped, CEntity& entity, CVector& outDirToAvoidEntity) {
    const auto zPos = ped.GetPosition().z;

    CVector corners[4], planes[4];
    float   planeDots[4];
    ComputeEntityBoundingBoxCornersUncached(zPos, entity, corners);
    ComputeEntityBoundingBoxPlanesUncached(zPos, corners, &planes, planeDots);

    const auto& pedPosn   = ped.GetPosition();
    const auto  plane1Dot = DotProduct(planes[1], pedPosn) + planeDots[1];
    const auto  plane3Dot = DotProduct(planes[3], pedPosn) + planeDots[3];

    if (plane1Dot > 0.f) {
        outDirToAvoidEntity = planes[1];
    } else if (plane3Dot > 0.f) {
        outDirToAvoidEntity = planes[3];
    } else if (plane1Dot > plane3Dot) {
        outDirToAvoidEntity = planes[1];
    } else {
        outDirToAvoidEntity = planes[3];
    }

    // NOTE: The original doesn't return anything (EAX is just a leftover)
    return 0;
}

//! @notsa
CVector CPedGeometryAnalyser::ComputeEntityDir(const CEntity& entity, eDirection dir) {
    switch (dir) {
    case eDirection::FORWARD:  return entity.GetForward();
    case eDirection::LEFT:     return -entity.GetRight();
    case eDirection::BACKWARD: return -entity.GetForward();
    case eDirection::RIGHT:    return entity.GetRight();
    default:                   NOTSA_UNREACHABLE();
    }
}

// 0x5F1500
CVector* CPedGeometryAnalyser::ComputeEntityDirs(const CEntity& entity, CVector* posn) {
    posn[0] = ComputeEntityDir(entity, eDirection::FORWARD);
    posn[1] = ComputeEntityDir(entity, eDirection::LEFT);
    posn[2] = ComputeEntityDir(entity, eDirection::BACKWARD);
    return &(posn[3] = ComputeEntityDir(entity, eDirection::RIGHT));
}

// 0x5F3BC0
int32 CPedGeometryAnalyser::ComputeEntityHitSide(const CPed& ped, CEntity& entity) {
    return ComputeEntityHitSide(ped.GetPosition(), entity);
}

// 0x5F1450
int32 CPedGeometryAnalyser::ComputeEntityHitSide(const CVector& point1, const CVector* point2, const float* x) {
    for (auto i = 4; i < 8; i++) {
        const auto a = (i - 1) % 4;
        const auto b = i % 4;
        if (DotProduct(point1, point2[a]) + x[a] < 0.f) {
            continue;
        }
        if (DotProduct(point1, point2[b]) + x[b] < 0.f) {
            return b;
        }
    }
    return 0;
}

// 0x5F3AC0
int32 CPedGeometryAnalyser::ComputeEntityHitSide(const CVector& point, CEntity& entity) {
    CVector corners[4], planes[4];
    float   planeDots[4];

    ComputeEntityBoundingBoxCornersUncached(point.z, entity, corners);

    CVector center;
    ComputeEntityBoundingBoxCentreUncached(point.z, corners, center);

    ComputeEntityBoundingBoxSegmentPlanesUncached(corners, center, planes, planeDots);

    return ComputeEntityHitSide(point, planes, planeDots);
}

// 0x5F3640
int32 CPedGeometryAnalyser::ComputePedHitSide(const CPed& ped, const CPhysical& physical) {
    return ComputePedHitSide(ped, physical.m_vecMoveSpeed);
}

// 0x5F1E70
int32 CPedGeometryAnalyser::ComputePedHitSide(const CPed& ped, const CVector& posn) {
    CVector dir = -posn;
    dir.Normalise();

    CVector dirs[4];
    ComputeEntityDirs(ped, dirs);

    auto side   = 0;
    auto maxDot = -1.f;
    for (auto i = 0; i < 4; i++) {
        const auto dot = DotProduct(dirs[i], dir);
        if (maxDot <= dot) {
            maxDot = dot;
            side   = i;
        }
    }
    return side;
}

// 0x5F13F0
int32 CPedGeometryAnalyser::ComputePedShotSide(const CPed& ped, const CVector& posn) {
    const auto& pedPosn = ped.GetPosition();

    // Bearing of `posn` relative to the ped's current rotation, normalized into [0, 2PI)
    auto angle = std::atan2(-(posn.x - pedPosn.x), posn.y - pedPosn.y) - ped.m_fCurrentRotation + PI / 4.f;
    if (angle < 0.f) {
        angle += TWO_PI;
    }
    return (int32)(angle * (2.f / PI));
}

// 0x5F6110
int32 CPedGeometryAnalyser::ComputeRouteRoundEntityBoundingBox(const CPed& ped, CEntity& entity, const CVector& posn, CPointRoute& pointRoute, int32 a5) {
    return ComputeRouteRoundEntityBoundingBox(ped, ped.GetPosition(), entity, posn, pointRoute, a5);
}

// 0x5F3DD0
int32 CPedGeometryAnalyser::ComputeRouteRoundEntityBoundingBox(const CPed& ped, const CVector& a2, CEntity& entity, const CVector& a4, CPointRoute& pointRoute, int32 a6) {
    return plugin::CallAndReturn<int32, 0x5F3DD0, const CPed&, const CVector&, CEntity&, const CVector&, CPointRoute&, int32>(ped, a2, entity, a4, pointRoute, a6);
}

// 0x5F1890
bool CPedGeometryAnalyser::ComputeRouteRoundSphere(const CPed& ped, const CColSphere& sphere, const CVector& a3, const CVector& a4, CVector& a5, CVector& a6) {
    const auto& pedPosn = ped.GetPosition();

    // NOTE: `IntersectPoint`/`IntersectRay` aren't const-correct
    auto& sphereNC = const_cast<CColSphere&>(sphere);

    a5             = a4;
    if (sphereNC.IntersectPoint(a4)) {
        auto dir = (a4 - a3).Normalized();

        CVector p1, p2;
        if (sphereNC.IntersectRay(pedPosn, dir, p1, p2)) {
            a5 = p2;
        }
    }

    CVector p1, p2;
    auto    dir = (a5 - pedPosn).Normalized();
    if (!sphereNC.IntersectRay(a5, dir, p1, p2)) {
        a6 = a5;
        return false;
    }
    if ((a5 - pedPosn).SquaredMagnitude() > (p1 - pedPosn).SquaredMagnitude()) {
        a6 = a5;
        return false;
    }

    if (sphereNC.IntersectRay(pedPosn, dir, p1, p2)) {
        const auto proj = pedPosn + dir * DotProduct(sphere.m_vecCenter - pedPosn, dir);
        a6              = sphere.m_vecCenter + (proj - sphere.m_vecCenter).Normalized() * sphere.m_fRadius;
    }
    return true;
}

// 0x5F5A30
bool CPedGeometryAnalyser::GetIsLineOfSightClear(const CPed& ped, const CVector& target, CEntity& entity, float& outDist) {
    CVector pedPosn  = ped.GetPosition();
    CVector targetPt = target;

    CColSphere sphere;
    ComputeEntityBoundingSphere(ped, entity, sphere);

    const CVector dir = (targetPt - pedPosn).Normalized();

    CVector p1, p2;
    if (!sphere.IntersectRay(pedPosn, dir, p1, p2)) {
        return true;
    }

    CVector corners[4], planes[4];
    float   planeDots[4];
    ComputeEntityBoundingBoxCornersUncached(pedPosn.z, entity, corners);
    ComputeEntityBoundingBoxPlanesUncached(pedPosn.z, corners, &planes, planeDots);

    outDist         = 0.0f;
    const float eps = s_fBBoxEpsilon;
    for (auto i = 0; i < 4; i++) {
        const float pedD    = DotProduct(pedPosn, planes[i]) + planeDots[i];
        const float tgtD    = DotProduct(targetPt, planes[i]) + planeDots[i];
        const auto  pedSide = pedD > eps ? 1 : (pedD < -eps ? -1 : 0);
        const auto  tgtSide = tgtD > eps ? 1 : (tgtD < -eps ? -1 : 0);
        if (pedSide < 0) {
            if (tgtSide > 0) {
                const auto denom = DotProduct(dir, planes[i]);
                if (denom > 0.001f) {
                    targetPt = pedPosn + dir * ((-1.0f / denom) * pedD);
                }
            }
        } else {
            if (tgtSide >= 0) {
                return true;
            }
            if (pedSide > 0) {
                const auto denom = DotProduct(dir, planes[i]);
                if (denom > 0.001f) {
                    pedPosn = pedPosn + dir * ((-1.0f / denom) * pedD);
                }
            }
        }
    }
    outDist = (targetPt - pedPosn).Magnitude();
    return false;
}

// 0x5F2F00
bool CPedGeometryAnalyser::GetIsLineOfSightClear(const CVector& a1, const CVector& a2, CEntity& a3) {
    const CColLine line{ a1, a2 };

    if (!a3.m_matrix) {
        a3.AllocateMatrix();
        a3.m_placement.UpdateMatrix(a3.m_matrix);
    }
    return !CCollision::TestLineOfSight(line, *a3.m_matrix, *a3.GetColModel(), false, false);
}

// 0x5F3590
CPed* CPedGeometryAnalyser::GetNearestPed(const CVector& point) {
    CPed* nearestPed    = nullptr;
    auto  nearestDistSq = FLT_MAX;

    for (int32 i = GetPedPool()->GetSize() - 1; i >= 0; i--) {
        CPed* ped = GetPedPool()->GetAt(i);
        if (!ped) {
            continue;
        }
        const auto distSq = (point - ped->GetPosition()).SquaredMagnitude();
        if (distSq < nearestDistSq) {
            nearestDistSq = distSq;
            nearestPed    = ped;
        }
    }
    return nearestPed;
}

// 0x5F3970
bool CPedGeometryAnalyser::IsEntityBlockingTarget(CEntity* entity, const CVector& point, float distance) {
    const auto& entityPosn = entity->GetPosition();
    if (std::abs(entityPosn.z - point.z) > 3.f) {
        return false;
    }

    const auto radius = entity->GetColModel()->GetBoundRadius();
    if (radius * radius + distance * distance < (entityPosn - point).Magnitude2D()) {
        return false;
    }

    CVector corners[4], planes[4];
    float   planeDots[4];
    ComputeEntityBoundingBoxCornersUncached(entityPosn.z, *entity, corners);
    ComputeEntityBoundingBoxPlanesUncached(entityPosn.z, corners, &planes, planeDots);

    const auto halfDistance = distance * 0.5f;
    for (auto i = 0; i < 4; i++) {
        if (DotProduct(planes[i], point) + planeDots[i] + halfDistance > 0.f) {
            return false;
        }
    }
    return true;
}

// 0x5F1CB0
bool CPedGeometryAnalyser::IsInAir(const CPed& ped) {
    if (ped.bInVehicle) {
        return false;
    }

    auto& taskMgr = ped.GetTaskManager();
    if (taskMgr.GetActiveTask()) {
        if (ped.GetIntelligence()->GetTaskSwim() || ped.GetIntelligence()->GetTaskJetPack()) {
            return false;
        }
        if (taskMgr.GetSimplestActiveTask()->GetTaskType() == TASK_SIMPLE_CLIMB) {
            return false;
        }
    }

    bool isJumping = false;
    if (const auto task = taskMgr.GetActiveTask()) {
        isJumping = task->GetTaskType() == TASK_COMPLEX_JUMP;
    }

    const auto& posn = ped.GetPosition();

    CColPoint colPoint;
    CEntity*  hitEntity;
    bool      hit = CWorld::ProcessVerticalLine(posn, posn.z - 1.5f, colPoint, hitEntity, true, true, false, true);
    if (!hit && !isJumping) {
        hit = CWorld::TestSphereAgainstWorld(CVector{ posn.x, posn.y, posn.z - 1.f }, 0.15f, const_cast<CPed*>(&ped), true, false, false, false, false, false) != nullptr;
    }
    return !hit;
}

// 0x5F2F70
CPedGeometryAnalyser::WanderPathClearness CPedGeometryAnalyser::IsWanderPathClear(const CVector& from, const CVector& to, float maxHeightChange, int32 maxSamples) {
    // NOTSA: binary derives the sample count from `floor(sqrt((to - from).Magnitude()))` (FUN_008219F0/FUN_00821B40),
    // then walks each sample point and checks water/ground/height-drop. CRT `floor` used instead of reimplementing it.
    if (std::fabs(from.z - to.z) > maxHeightChange) {
        return WanderPathClearness::BLOCKED_HEIGHT;
    }
    const auto minZ = std::min(from.z, to.z);
    if (!CWorld::GetIsLineOfSightClear(CVector{ from.x, from.y, minZ }, CVector{ to.x, to.y, minZ }, true, false, false, false, false, false, false)) {
        return WanderPathClearness::BLOCKED_LOS;
    }
    CVector dir = to - from;
    const auto mag = dir.Magnitude();
    int32 numSamples = (int32)std::floor(mag);
    numSamples = std::min(numSamples, maxSamples);
    if (numSamples != 0) {
        dir.Normalise();
        // NOTE: Both loops below run `i` in [1, numSamples), i.e. the endpoint (`to` itself) is excluded.
        for (int32 i = 1; i < numSamples; i++) {
            const CVector samplePt = from + dir * (float)i;
            float waterLevel;
            if (CWaterLevel::GetWaterLevel(samplePt.x, samplePt.y, samplePt.z, waterLevel, 0, nullptr)) {
                const auto maxZ = std::max(from.z, to.z);
                CColPoint colPoint{};
                CEntity* hitEntity{};
                // NOTE: The vertical line starts at the water level, not at the sample point.
                if (!CWorld::ProcessVerticalLine(CVector{ samplePt.x, samplePt.y, waterLevel }, maxZ, colPoint, hitEntity, true, false, false, false, false, false, nullptr)) {
                    return WanderPathClearness::BLOCKED_WATER;
                }
            }
        }
        CColPoint colPoint{};
        CEntity* hitEntity{};
        if (!CWorld::ProcessVerticalLine(from, from.z - 5.f, colPoint, hitEntity, true, false, false, false, false, false, nullptr)) {
            return WanderPathClearness::BLOCKED_SHARP_DROP;
        }
        auto groundZ = colPoint.m_vecPoint.z + 0.5f;
        for (int32 i = 1; i < numSamples; i++) {
            const CVector samplePt{ from.x + dir.x * (float)i, from.y + dir.y * (float)i, groundZ };
            if (!CWorld::ProcessVerticalLine(samplePt, groundZ - 2.f, colPoint, hitEntity, true, false, false, false, false, false, nullptr)) {
                return WanderPathClearness::BLOCKED_SHARP_DROP;
            }
            if (std::fabs(colPoint.m_vecPoint.z - groundZ) > 1.f) {
                return WanderPathClearness::BLOCKED_SHARP_DROP;
            }
            groundZ = colPoint.m_vecPoint.z + 0.5f;
        }
    }
    return WanderPathClearness::CLEAR;
}

// 0x5F3880
bool CPedGeometryAnalyser::LiesInsideBoundingBox(const CPed& ped, const CVector& posn, CEntity& entity) {
    const auto  radius     = entity.GetColModel()->GetBoundRadius();
    const auto& entityPosn = entity.GetPosition();
    if (radius * radius <= (posn - entityPosn).SquaredMagnitude()) {
        return false;
    }

    CVector corners[4], planes[4];
    float   planeDots[4];
    ComputeEntityBoundingBoxCornersUncached(ped.GetPosition().z, entity, corners);
    ComputeEntityBoundingBoxPlanesUncached(ped.GetPosition().z, corners, &planes, planeDots);

    for (auto i = 0; i < 4; i++) {
        if (DotProduct(planes[i], posn) + planeDots[i] < 0.f) {
            return true;
        }
    }
    return false;
}

// 0x41B7C0
void* CPointRoute::operator new(uint32 size) {
    return GetPointRoutePool()->New();
}

// 0x41B7D0
void CPointRoute::operator delete(void* ptr, size_t sz) {
    GetPointRoutePool()->Delete(reinterpret_cast<CPointRoute*>(ptr));
}
