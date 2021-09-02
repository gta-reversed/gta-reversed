#include "StdInc.h"

void CPedGeometryAnalyser::InjectHooks() {
//    ReversibleHooks::Install("CPedGeometryAnalyser", "CanPedJumpObstacle", 0x0, &CPedGeometryAnalyser::CanPedJumpObstacle);
//    ReversibleHooks::Install("CPedGeometryAnalyser", "CanPedJumpObstacle", 0x0, &CPedGeometryAnalyser::CanPedJumpObstacle);
//    ReversibleHooks::Install("CPedGeometryAnalyser", "CanPedTargetPed", 0x5F1C40, &CPedGeometryAnalyser::CanPedTargetPed);
//    ReversibleHooks::Install("CPedGeometryAnalyser", "CanPedTargetPoint", 0x5F1B70, &CPedGeometryAnalyser::CanPedTargetPoint);
//    ReversibleHooks::Install("CPedGeometryAnalyser", "ComputeBuildingHitPoints", 0x5F1E30, &CPedGeometryAnalyser::ComputeBuildingHitPoints);
//    ReversibleHooks::Install("CPedGeometryAnalyser", "ComputeClearTarget", 0x0, &CPedGeometryAnalyser::ComputeClearTarget);
    ReversibleHooks::Install("CPedGeometryAnalyser", "ComputeClosestSurfacePoint", 0x5F3B70, static_cast<bool (*)(const CPed& ped, CEntity& entity, CVector& point)>(CPedGeometryAnalyser::ComputeClosestSurfacePoint));
//    ReversibleHooks::Install("CPedGeometryAnalyser", "ComputeClosestSurfacePoint", 0x0, &CPedGeometryAnalyser::ComputeClosestSurfacePoint);
//    ReversibleHooks::Install("CPedGeometryAnalyser", "ComputeClosestSurfacePoint", 0x0, &CPedGeometryAnalyser::ComputeClosestSurfacePoint);
//    ReversibleHooks::Install("CPedGeometryAnalyser", "ComputeEntityBoundingBoxCentre", 0x0, &CPedGeometryAnalyser::ComputeEntityBoundingBoxCentre);
    ReversibleHooks::Install("CPedGeometryAnalyser", "ComputeEntityBoundingBoxCentreUncached", 0x5F1600, &CPedGeometryAnalyser::ComputeEntityBoundingBoxCentreUncached);
    ReversibleHooks::Install("CPedGeometryAnalyser", "ComputeEntityBoundingBoxCentreUncachedAll", 0x5F3B40, &CPedGeometryAnalyser::ComputeEntityBoundingBoxCentreUncachedAll);
//    ReversibleHooks::Install("CPedGeometryAnalyser", "ComputeEntityBoundingBoxCorners", 0x0, &CPedGeometryAnalyser::ComputeEntityBoundingBoxCorners);
//    ReversibleHooks::Install("CPedGeometryAnalyser", "ComputeEntityBoundingBoxCornersUncached", 0x5F1FA0, &CPedGeometryAnalyser::ComputeEntityBoundingBoxCornersUncached);
//    ReversibleHooks::Install("CPedGeometryAnalyser", "ComputeEntityBoundingBoxPlanes", 0x5F3660, &CPedGeometryAnalyser::ComputeEntityBoundingBoxPlanes);
    ReversibleHooks::Install("CPedGeometryAnalyser", "ComputeEntityBoundingBoxPlanesUncached", 0x5F1670, &CPedGeometryAnalyser::ComputeEntityBoundingBoxPlanesUncached);
//    ReversibleHooks::Install("CPedGeometryAnalyser", "ComputeEntityBoundingBoxPlanesUncachedAll", 0x0, &CPedGeometryAnalyser::ComputeEntityBoundingBoxPlanesUncachedAll);
//    ReversibleHooks::Install("CPedGeometryAnalyser", "ComputeEntityBoundingBoxSegmentPlanes", 0x0, &CPedGeometryAnalyser::ComputeEntityBoundingBoxSegmentPlanes);
//    ReversibleHooks::Install("CPedGeometryAnalyser", "ComputeEntityBoundingBoxSegmentPlanesUncached", 0x5F1750, &CPedGeometryAnalyser::ComputeEntityBoundingBoxSegmentPlanesUncached);
    ReversibleHooks::Install("CPedGeometryAnalyser", "ComputeEntityBoundingBoxSegmentPlanesUncachedAll", 0x5F2BC0, &CPedGeometryAnalyser::ComputeEntityBoundingBoxSegmentPlanesUncachedAll);
//    ReversibleHooks::Install("CPedGeometryAnalyser", "ComputeEntityBoundingSphere", 0x5F3C20, &CPedGeometryAnalyser::ComputeEntityBoundingSphere);
//    ReversibleHooks::Install("CPedGeometryAnalyser", "ComputeMoveDirToAvoidEntity", 0x0, &CPedGeometryAnalyser::ComputeMoveDirToAvoidEntity);
//    ReversibleHooks::Install("CPedGeometryAnalyser", "ComputeEntityDirs", 0x5F1500, &CPedGeometryAnalyser::ComputeEntityDirs);
//    ReversibleHooks::Install("CPedGeometryAnalyser", "ComputeEntityHitSide", 0x0, static_cast<int32 (*)()>(&CPedGeometryAnalyser::ComputeEntityHitSide));
//    ReversibleHooks::Install("CPedGeometryAnalyser", "ComputeEntityHitSide", 0x0, static_cast<int32 (*)()>(&CPedGeometryAnalyser::ComputeEntityHitSide));
//    ReversibleHooks::Install("CPedGeometryAnalyser", "ComputeEntityHitSide", 0x0, static_cast<int32 (*)()>(&CPedGeometryAnalyser::ComputeEntityHitSide));
//    ReversibleHooks::Install("CPedGeometryAnalyser", "ComputePedHitSide", 0x0, &CPedGeometryAnalyser::ComputePedHitSide);
//    ReversibleHooks::Install("CPedGeometryAnalyser", "ComputePedHitSide", 0x0, &CPedGeometryAnalyser::ComputePedHitSide);
//    ReversibleHooks::Install("CPedGeometryAnalyser", "ComputePedShotSide", 0x5F13F0, &CPedGeometryAnalyser::ComputePedShotSide);
//    ReversibleHooks::Install("CPedGeometryAnalyser", "ComputeRouteRoundEntityBoundingBox", 0x0, &CPedGeometryAnalyser::ComputeRouteRoundEntityBoundingBox);
//    ReversibleHooks::Install("CPedGeometryAnalyser", "ComputeRouteRoundEntityBoundingBox", 0x0, &CPedGeometryAnalyser::ComputeRouteRoundEntityBoundingBox);
//    ReversibleHooks::Install("CPedGeometryAnalyser", "ComputeRouteRoundSphere", 0x5F1890, &CPedGeometryAnalyser::ComputeRouteRoundSphere);
//    ReversibleHooks::Install("CPedGeometryAnalyser", "GetIsLineOfSightClear", 0x0, &CPedGeometryAnalyser::GetIsLineOfSightClear);
//    ReversibleHooks::Install("CPedGeometryAnalyser", "GetIsLineOfSightClear", 0x0, &CPedGeometryAnalyser::GetIsLineOfSightClear);
//    ReversibleHooks::Install("CPedGeometryAnalyser", "GetNearestPed", 0x5F3590, &CPedGeometryAnalyser::GetNearestPed);
//    ReversibleHooks::Install("CPedGeometryAnalyser", "IsEntityBlockingTarget", 0x5F3970, &CPedGeometryAnalyser::IsEntityBlockingTarget);
//    ReversibleHooks::Install("CPedGeometryAnalyser", "IsInAir", 0x5F1CB0, &CPedGeometryAnalyser::IsInAir);
//    ReversibleHooks::Install("CPedGeometryAnalyser", "IsWanderPathClear", 0x5F2F70, &CPedGeometryAnalyser::IsWanderPathClear);
//    ReversibleHooks::Install("CPedGeometryAnalyser", "LiesInsideBoundingBox", 0x5F3880, &CPedGeometryAnalyser::LiesInsideBoundingBox);
}

// 0x5F1B00
void CPedGeometryAnalyser::CanPedJumpObstacle(const CPed& ped, const CEntity& entity) {

}

// 0x5F32D0
void CPedGeometryAnalyser::CanPedJumpObstacle(const CPed& ped, const CEntity& entity, const CVector&, const CVector&) {

}

// 0x5F1C40
// unused
void CPedGeometryAnalyser::CanPedTargetPed(CPed& ped, CPed& targetPed, bool a3) {

}

// 0x5F1B70
// unused
void CPedGeometryAnalyser::CanPedTargetPoint(const CPed& ped, const CVector& a2, bool a3) {

}

// 0x5F1E30
// unused
int32 CPedGeometryAnalyser::ComputeBuildingHitPoints(const CVector& a1, const CVector& a2) {
    CEntity *outEntity;
    CColPoint v4;

    CWorld::ProcessLineOfSight(a1, a2, v4, outEntity, true, false, false, false, true, false, false, false);
    return CWorld::ms_iProcessLineNumCrossings;
}

void CPedGeometryAnalyser::ComputeClearTarget(const CPed& ped, const CVector&, CVector&) {

}

// 0x5F3B70
bool CPedGeometryAnalyser::ComputeClosestSurfacePoint(const CPed& ped, CEntity& entity, CVector& point) {
    CVector corners[4];
    auto posn = ped.GetPosition();
    ComputeEntityBoundingBoxCornersUncached(posn.z, entity, corners);
    return ComputeClosestSurfacePoint(posn, corners, point);
}

// 0x5F36F0
bool CPedGeometryAnalyser::ComputeClosestSurfacePoint(const CVector& posn, CEntity& entity, CVector& point) {
    return plugin::CallAndReturn<bool, 0x5F36F0, const CVector&, CEntity&, CVector&>(posn, entity, point);
}

// 0x5F2C10
bool CPedGeometryAnalyser::ComputeClosestSurfacePoint(const CVector& posn, const CVector* corners, CVector& point) {
    return plugin::CallAndReturn<bool, 0x5F2C10, const CVector&, const CVector*, CVector&>(posn, corners, point);
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
void CPedGeometryAnalyser::ComputeEntityBoundingBoxPlanes(float zPos, CEntity& entity, CVector* planes, float* planes_D) {
    CVector corners[4];
    ComputeEntityBoundingBoxCornersUncached(zPos, entity, corners);
    ComputeEntityBoundingBoxPlanesUncached(zPos, corners, planes, planes_D);
}

// 0x5F1670
void CPedGeometryAnalyser::ComputeEntityBoundingBoxPlanesUncached(float zPos, const CVector* corners, CVector* planes, float* planes_D) {
    const CVector* corner2 = &corners[3];
    for (auto i = 0; i < 4; i++) {
        const CVector& corner = corners[i];
        CVector& plane = planes[i];
        CVector direction = corner - *corner2;
        direction.Normalise();
        plane.x = direction.y;
        plane.y = -direction.x;
        plane.z = 0.0f;
        // point-normal plane equation:
        // ax + by + cz + d = 0
        // d = - n . P
        planes_D[i] = -DotProduct(plane, *corner2);
        corner2 = &corner;
    }
}

void CPedGeometryAnalyser::ComputeEntityBoundingBoxPlanesUncachedAll(float zPos, CEntity& entity, CVector* posn, float* a4) {

}

void CPedGeometryAnalyser::ComputeEntityBoundingBoxSegmentPlanes(float zPos, CEntity& entity, CVector*, float*) {

}

// 0x5F1750
CVector* CPedGeometryAnalyser::ComputeEntityBoundingBoxSegmentPlanesUncached(const CVector* corners, CVector& center, CVector* a3, float* a4) {
    return plugin::CallAndReturn<CVector*, 0x5F1750, const CVector*, CVector&, CVector*, float*>(corners, center, a3, a4);
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
void CPedGeometryAnalyser::ComputeEntityBoundingSphere(const CPed& ped, CEntity& entity, CColSphere&) {

}

// 0x5F3730
int32 CPedGeometryAnalyser::ComputeMoveDirToAvoidEntity(const CPed& ped, CEntity& entity, CVector& posn) {
    return plugin::CallAndReturn<int32, 0x5F3730, const CPed&, CEntity&, CVector&>(ped, entity, posn);
}

// 0x5F1500
CVector* CPedGeometryAnalyser::ComputeEntityDirs(const CEntity& entity, CVector* posn) {
    return plugin::CallAndReturn<CVector*, 0x5F1500, const CEntity&, CVector*>(entity, posn);
}

// 0x5F3BC0
int32 CPedGeometryAnalyser::ComputeEntityHitSide(const CPed& ped, CEntity& entity) {
    auto posn = ped.GetPosition();
    return ComputeEntityHitSide(posn, entity);
}

// 0x5F1450
int32 CPedGeometryAnalyser::ComputeEntityHitSide(const CVector& point1, const CVector* point2, const float* x) {
    return plugin::CallAndReturn<int32, 0x5F1450, const CVector&, const CVector*, const float*>(point1, point2, x);
}

// 0x5F3AC0
int32 CPedGeometryAnalyser::ComputeEntityHitSide(const CVector& point, CEntity& entity) {
    return plugin::CallAndReturn<int32, 0x5F3AC0, const CVector&, CEntity&>(point, entity);
}

// 0x5F3640
int32 CPedGeometryAnalyser::ComputePedHitSide(const CPed& ped, const CPhysical& physical) {
    return ComputePedHitSide(ped, physical.m_vecMoveSpeed);
}

// 0x5F1E70
int32 CPedGeometryAnalyser::ComputePedHitSide(const CPed& ped, const CVector& posn) {
    return plugin::CallAndReturn<int32, 0x5F1E70, const CPed&, const CVector&>(ped, posn);
}

// 0x5F13F0
int32 CPedGeometryAnalyser::ComputePedShotSide(const CPed& ped, const CVector& posn) {
    return plugin::CallAndReturn<int32, 0x5F13F0, const CPed&, const CVector&>(ped, posn);
}

// 0x5F6110
int32 CPedGeometryAnalyser::ComputeRouteRoundEntityBoundingBox(const CPed& ped, CEntity& entity, const CVector& posn, CPointRoute& pointRoute, int32 a5) {
    return ComputeRouteRoundEntityBoundingBox(ped, ped.GetPosition(), entity, posn, pointRoute, a5);
}

// 0x5F3DD0
int32 CPedGeometryAnalyser::ComputeRouteRoundEntityBoundingBox(const CPed& ped, const CVector& a2, CEntity& entity, const CVector& a4, CPointRoute& pointRoute, int32 a6){
    return plugin::CallAndReturn<int32, 0x5F3DD0, const CPed&, const CVector&, CEntity&, const CVector&, CPointRoute&, int32>(ped, a2, entity, a4, pointRoute, a6);
}

// 0x5F1890
bool CPedGeometryAnalyser::ComputeRouteRoundSphere(const CPed& ped, const CColSphere& sphere, const CVector& a3, const CVector& a4, CVector& a5, CVector& a6) {
    return plugin::CallAndReturn<bool, 0x5F1890, const CPed&, const CColSphere&, const CVector&, const CVector&, CVector&, CVector&>(ped, sphere, a3, a4, a5, a6);
}

// 0x5F5A30
bool CPedGeometryAnalyser::GetIsLineOfSightClear(const CPed& ped, const CVector& a2, CEntity& entity, float& a4) {
    return plugin::CallAndReturn<bool, 0x5F5A30, const CPed&, const CVector&, CEntity&, float&>(ped, a2, entity, a4);
}

// 0x5F2F00
bool CPedGeometryAnalyser::GetIsLineOfSightClear(const CVector& a1, const CVector& a2, CEntity& a3) {
    return plugin::CallAndReturn<bool, 0x5F2F00, const CVector&, const CVector&, CEntity&>(a1, a2, a3);
}

// 0x5F3590
CPed* CPedGeometryAnalyser::GetNearestPed(const CVector& point) {
    return plugin::CallAndReturn<CPed*, 0x5F3590, const CVector&>(point);
}

// 0x5F3970
bool CPedGeometryAnalyser::IsEntityBlockingTarget(CEntity* entity, const CVector& point, float distance) {
    return plugin::CallAndReturn<bool, 0x5F3970, CEntity*, const CVector&, float>(entity, point, distance);
}

// 0x5F1CB0
bool CPedGeometryAnalyser::IsInAir(const CPed& ped) {
    return plugin::CallAndReturn<bool, 0x5F1CB0, const CPed&>(ped);
}

// 0x5F2F70
bool CPedGeometryAnalyser::IsWanderPathClear(const CVector& a1, const CVector& a2, float a3, int32 a4) {
    return plugin::CallAndReturn<bool, 0x5F2F70, const CVector&, const CVector&, float, int32>(a1, a2, a3, a4);
}

// 0x5F3880
bool CPedGeometryAnalyser::LiesInsideBoundingBox(const CPed& ped, const CVector& posn, CEntity& entity) {
    return plugin::CallAndReturn<bool, 0x5F3880, const CPed&, const CVector&, CEntity&>(ped, posn, entity);
}
