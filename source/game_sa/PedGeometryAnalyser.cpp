#include "StdInc.h"

#include "PedGeometryAnalyser.h"

void CPedGeometryAnalyser::InjectHooks() {
    RH_ScopedClass(CPedGeometryAnalyser);
    RH_ScopedCategoryGlobal();

    //RH_ScopedInstall(CanPedJumpObstacle, 0x0, { .reversed = false });
    //RH_ScopedInstall(CanPedJumpObstacle, 0x0, { .reversed = false });
    RH_ScopedInstall(CanPedTargetPed, 0x5F1C40);
    RH_ScopedInstall(CanPedTargetPoint, 0x5F1B70, { .reversed = false });
    RH_ScopedInstall(ComputeBuildingHitPoints, 0x5F1E30, { .reversed = false });
    //RH_ScopedInstall(ComputeClearTarget, 0x0, { .reversed = false });
    RH_ScopedOverloadedInstall(ComputeClosestSurfacePoint, "", 0x5F3B70, bool (*)(const CPed& ped, CEntity& entity, CVector& point));
    //RH_ScopedInstall(ComputeClosestSurfacePoint, 0x0, { .reversed = false });
    //RH_ScopedInstall(ComputeClosestSurfacePoint, 0x0, { .reversed = false });
    //RH_ScopedInstall(ComputeEntityBoundingBoxCentre, 0x0, { .reversed = false });
    RH_ScopedInstall(ComputeEntityBoundingBoxCentreUncached, 0x5F1600);
    RH_ScopedInstall(ComputeEntityBoundingBoxCentreUncachedAll, 0x5F3B40);
    //RH_ScopedInstall(ComputeEntityBoundingBoxCorners, 0x0, { .reversed = false });
    RH_ScopedInstall(ComputeEntityBoundingBoxCornersUncached, 0x5F1FA0, { .reversed = false });
    RH_ScopedInstall(ComputeEntityBoundingBoxPlanes, 0x5F3660, { .reversed = false });
    RH_ScopedInstall(ComputeEntityBoundingBoxPlanesUncached, 0x5F1670);
    //RH_ScopedInstall(ComputeEntityBoundingBoxPlanesUncachedAll, 0x0, { .reversed = false });
    //RH_ScopedInstall(ComputeEntityBoundingBoxSegmentPlanes, 0x0, { .reversed = false });
    RH_ScopedInstall(ComputeEntityBoundingBoxSegmentPlanesUncached, 0x5F1750, { .reversed = false });
    RH_ScopedInstall(ComputeEntityBoundingBoxSegmentPlanesUncachedAll, 0x5F2BC0);
    RH_ScopedInstall(ComputeEntityBoundingSphere, 0x5F3C20, { .reversed = false });
    //RH_ScopedInstall(ComputeMoveDirToAvoidEntity, 0x0, { .reversed = false });
    RH_ScopedInstall(ComputeEntityDirs, 0x5F1500, { .reversed = false });
    //    RH_ScopedOverloadedInstall(ComputeEntityHitSide, "", 0x0, int32 (*)());
    //    RH_ScopedOverloadedInstall(ComputeEntityHitSide, "", 0x0, int32 (*)());
    //    RH_ScopedOverloadedInstall(ComputeEntityHitSide, "", 0x0, int32 (*)());
    //RH_ScopedInstall(ComputePedHitSide, 0x0, { .reversed = false });
    //RH_ScopedInstall(ComputePedHitSide, 0x0, { .reversed = false });
    RH_ScopedInstall(ComputePedShotSide, 0x5F13F0, { .reversed = false });
    //RH_ScopedInstall(ComputeRouteRoundEntityBoundingBox, 0x0, { .reversed = false });
    //RH_ScopedInstall(ComputeRouteRoundEntityBoundingBox, 0x0, { .reversed = false });
    RH_ScopedInstall(ComputeRouteRoundSphere, 0x5F1890, { .reversed = false });
    //RH_ScopedInstall(GetIsLineOfSightClear, 0x0, { .reversed = false });
    //RH_ScopedInstall(GetIsLineOfSightClear, 0x0, { .reversed = false });
    RH_ScopedInstall(GetNearestPed, 0x5F3590, { .reversed = false });
    RH_ScopedInstall(IsEntityBlockingTarget, 0x5F3970, { .reversed = false });
    RH_ScopedInstall(IsInAir, 0x5F1CB0, { .reversed = false });
    RH_ScopedInstall(IsWanderPathClear, 0x5F2F70, { .reversed = false });
    RH_ScopedInstall(LiesInsideBoundingBox, 0x5F3880, { .reversed = false });
}

// 0x5F1B00
bool CPedGeometryAnalyser::CanPedJumpObstacle(const CPed& ped, const CEntity& entity) {
    return plugin::CallAndReturn<bool, 0x5F1B00, CPed const&, CEntity const&>(ped, entity);
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
    return plugin::CallMethodAndReturn<bool, 0x5F1B70>(&ped, &a2, a3);
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
    assert(false);
}

// 0x5F3B70
bool CPedGeometryAnalyser::ComputeClosestSurfacePoint(const CPed& ped, CEntity& entity, CVector& point) {
    CVector corners[4];
    const auto& posn = ped.GetPosition();
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
void CPedGeometryAnalyser::ComputeEntityBoundingBoxPlanes(float zPos, CEntity& entity, CVector(*outPlanes)[4], float* outPlanesDot) {
    CVector corners[4];
    ComputeEntityBoundingBoxCornersUncached(zPos, entity, corners);
    ComputeEntityBoundingBoxPlanesUncached(zPos, corners, outPlanes, outPlanesDot);
}

// 0x5F1670
void CPedGeometryAnalyser::ComputeEntityBoundingBoxPlanesUncached(float zPos, const CVector* corners, CVector(*outPlanes)[4], float* outPlanesDot) {
    const CVector* corner2 = &corners[3];
    for (auto i = 0; i < 4; i++) {
        const CVector& corner = corners[i];
        CVector& plane = (*outPlanes)[i];
        CVector direction = corner - *corner2;
        direction.Normalise();
        plane.x = direction.y;
        plane.y = -direction.x;
        plane.z = 0.0f;
        // point-normal plane equation:
        // ax + by + cz + d = 0
        // d = - n . P
        outPlanesDot[i] = -DotProduct(plane, *corner2);

        corner2 = &corner;
    }
}

void CPedGeometryAnalyser::ComputeEntityBoundingBoxPlanesUncachedAll(float zPos, CEntity& entity, CVector (*outPlanes)[4], float* outPlanesDot) {
    CVector corners[4];
    CPedGeometryAnalyser::ComputeEntityBoundingBoxCornersUncached(zPos, entity, corners);
    CPedGeometryAnalyser::ComputeEntityBoundingBoxPlanesUncached(zPos, corners, outPlanes, outPlanesDot);
}

void CPedGeometryAnalyser::ComputeEntityBoundingBoxSegmentPlanes(float zPos, CEntity& entity, CVector*, float*) {
    assert(false);
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
    assert(false);
}

// 0x5F3730
int32 CPedGeometryAnalyser::ComputeMoveDirToAvoidEntity(const CPed& ped, CEntity& entity, CVector& outDirToAvoidEntity) {
    return plugin::CallAndReturn<int32, 0x5F3730, const CPed&, CEntity&, CVector&>(ped, entity, outDirToAvoidEntity);
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
    return plugin::CallAndReturn<CVector*, 0x5F1500, const CEntity&, CVector*>(entity, posn);
}

// 0x5F3BC0
int32 CPedGeometryAnalyser::ComputeEntityHitSide(const CPed& ped, CEntity& entity) {
    return ComputeEntityHitSide(ped.GetPosition(), entity);
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
CPedGeometryAnalyser::WanderPathClearness CPedGeometryAnalyser::IsWanderPathClear(const CVector& a1, const CVector& a2, float a3, int32 a4) {
    return plugin::CallAndReturn<WanderPathClearness, 0x5F2F70, const CVector&, const CVector&, float, int32>(a1, a2, a3, a4);
}

// 0x5F3880
bool CPedGeometryAnalyser::LiesInsideBoundingBox(const CPed& ped, const CVector& posn, CEntity& entity) {
    return plugin::CallAndReturn<bool, 0x5F3880, const CPed&, const CVector&, CEntity&>(ped, posn, entity);
}

// 0x41B7C0
void* CPointRoute::operator new(uint32 size) {
    return CPools::ms_pPointRoutePool->New();
}

// 0x41B7D0
void CPointRoute::operator delete(void* ptr, size_t sz) {
    CPools::ms_pPointRoutePool->Delete(reinterpret_cast<CPointRoute*>(ptr));
}
