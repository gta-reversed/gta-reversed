#include "StdInc.h"

#include "PedGeometryAnalyser.h"

void CPedGeometryAnalyser::InjectHooks() {
    RH_ScopedClass(CPedGeometryAnalyser);
    RH_ScopedCategoryGlobal();

    RH_ScopedOverloadedInstall(CanPedJumpObstacle, "LoS", 0x5F1B00, bool(*)(const CPed&,const CEntity&));
    RH_ScopedOverloadedInstall(CanPedJumpObstacle, "Contacted", 0x5F32D0, bool(*)(const CPed&,const CEntity&,const CVector&,const CVector&));
    RH_ScopedInstall(CanPedTargetPed, 0x5F1C40);
    RH_ScopedInstall(CanPedTargetPoint, 0x5F1B70);
    RH_ScopedInstall(ComputeBuildingHitPoints, 0x5F1E30);
    RH_ScopedInstall(ComputeClearTarget, 0x5F5D80);
    RH_ScopedOverloadedInstall(ComputeClosestSurfacePoint, "ped", 0x5F3B70, bool (*)(const CPed& ped, CEntity& entity, CVector& point));
    RH_ScopedOverloadedInstall(ComputeClosestSurfacePoint, "posn", 0x5F36F0, bool(*)(const CVector&,CEntity&,CVector&));
    RH_ScopedOverloadedInstall(ComputeClosestSurfacePoint, "rect", 0x5F2C10, bool(*)(const CVector&,const std::array<CVector, 4>&,CVector&), { .reversed = false });
    RH_ScopedInstall(ComputeEntityBoundingBoxCentreUncached, 0x5F1600);
    RH_ScopedInstall(ComputeEntityBoundingBoxCentreUncachedAll, 0x5F3B40);
    RH_ScopedInstall(ComputeEntityBoundingBoxCorners, 0x5F3650);
    RH_ScopedInstall(ComputeEntityBoundingBoxCornersUncached, 0x5F1FA0, { .reversed = false });
    RH_ScopedInstall(ComputeEntityBoundingBoxPlanes, 0x5F3660, { .reversed = false });
    RH_ScopedInstall(ComputeEntityBoundingBoxPlanesUncached, 0x5F1670);
    RH_ScopedInstall(ComputeEntityBoundingBoxPlanesUncachedAll, 0x5F2B80);
    RH_ScopedInstall(ComputeEntityBoundingBoxSegmentPlanes, 0x5F36A0);
    RH_ScopedInstall(ComputeEntityBoundingBoxSegmentPlanesUncached, 0x5F1750, { .reversed = false });
    RH_ScopedInstall(ComputeEntityBoundingBoxSegmentPlanesUncachedAll, 0x5F2BC0);
    RH_ScopedInstall(ComputeEntityBoundingSphere, 0x5F3C20, { .reversed = false });
    RH_ScopedInstall(ComputeMoveDirToAvoidEntity, 0x5F3730, { .reversed = false });
    RH_ScopedInstall(ComputeEntityDirs, 0x5F1500, { .reversed = false });
    RH_ScopedOverloadedInstall(ComputeEntityHitSide, "1", 0x5F3BC0, int32 (*)(const CPed& ped, CEntity& entity), {.reversed = false});
    RH_ScopedOverloadedInstall(ComputeEntityHitSide, "2", 0x5F1450, int32 (*)(const CVector& point1, const std::array<CVector, 4>& point2, const float* x), {.reversed = false});
    RH_ScopedOverloadedInstall(ComputeEntityHitSide, "3", 0x5F3AC0, int32 (*)(const CVector& point, CEntity& entity), {.reversed = false});
    RH_ScopedOverloadedInstall(ComputePedHitSide, "physical", 0x5F3640, int32(*)(const CPed&,const CPhysical&), { .reversed = false });
    RH_ScopedOverloadedInstall(ComputePedHitSide, "posn", 0x5F1E70, int32(*)(const CPed&,const CVector&), { .reversed = false });
    RH_ScopedInstall(ComputePedShotSide, 0x5F13F0, { .reversed = false });
    RH_ScopedOverloadedInstall(ComputeRouteRoundEntityBoundingBox, "1", 0x5F6110, int32(*)(const CPed&,CEntity&,const CVector&,CPointRoute&,int32), { .reversed = false });
    RH_ScopedOverloadedInstall(ComputeRouteRoundEntityBoundingBox, "2", 0x5F3DD0, int32(*)(const CPed&,const CVector&,CEntity&,const CVector&,CPointRoute&,int32), { .reversed = false });
    RH_ScopedInstall(ComputeRouteRoundSphere, 0x5F1890, { .reversed = false });
    RH_ScopedOverloadedInstall(GetIsLineOfSightClear, "ped", 0x5F5A30, bool(*)(const CPed&,const CVector&,CEntity&,float&), { .reversed = false });
    RH_ScopedOverloadedInstall(GetIsLineOfSightClear, "v3d", 0x5F2F00, bool(*)(const CVector&,const CVector&,CEntity&), { .reversed = false });
    RH_ScopedInstall(GetNearestPed, 0x5F3590, { .reversed = false });
    RH_ScopedInstall(IsEntityBlockingTarget, 0x5F3970, { .reversed = false });
    RH_ScopedInstall(IsInAir, 0x5F1CB0, { .reversed = false });
    RH_ScopedInstall(IsWanderPathClear, 0x5F2F70, { .reversed = false });
    RH_ScopedInstall(LiesInsideBoundingBox, 0x5F3880, { .reversed = false });
}

// notsa, common code
bool CanPedJumpObstacleLoSCheck(CVector pedPos, CVector jmpDir, const CEntity& entity) {
    return CWorld::GetIsLineOfSightClear(
        pedPos,
        pedPos + jmpDir,
        true,
        false,
        false,
        true,
        false,
        false,
        false
    );
}

// 0x5F1B00
bool CPedGeometryAnalyser::CanPedJumpObstacle(const CPed& ped, const CEntity& entity) {
    if (entity.m_bIsTempBuilding) {
        return false;
    }
    return CanPedJumpObstacleLoSCheck(ped.GetPosition(), ped.GetForward(), entity);
}

// 0x5F32D0
bool CPedGeometryAnalyser::CanPedJumpObstacle(const CPed& ped, const CEntity& entity, const CVector& contactNormal, const CVector& contactPos) {
    if (entity.m_bIsTempBuilding) {
        return false;
    }
    if (g_surfaceInfos.IsShallowWater(ped.m_nContactSurface)) {
        return true;
    }

    const auto CheckCanJumpFrom = [&ped, &entity](const CVector& jumpFrom, CVector dir) -> bool {
        if (!CanPedJumpObstacleLoSCheck(jumpFrom, dir, entity)) { // 0x5F34EA
            return false;
        }
        bool onGround = false;
        const auto groundZ = CWorld::FindGroundZFor3DCoord(jumpFrom + dir * 3.f, &onGround, nullptr);
        return onGround && (jumpFrom.z - groundZ) < 3.0f; // 0x5F3555
    };

    if (contactNormal.z <= 0.17f) { // 0x5F34A0
        if (!CPedGroups::IsInPlayersGroup(&ped)) { // 0x5F3494
            return CheckCanJumpFrom(
                ped.GetPosition() + CVector{ 0.f, 0.f, 0.15f },
                ped.GetForward()
            );
        }
        return CheckCanJumpFrom(ped.GetPosition(), ped.GetForward()); // 0x5F34BE
    }

    if (contactNormal.z > 0.9f) {
        return false; // 0x5F3361
    }

    const auto* const pedCM = ped.GetColModel();
    if (!pedCM) { // BUGFIX
        return false;
    }
    CVector from = ped.GetPosition();
    from.z += pedCM->GetBoundCenter().z - pedCM->GetBoundRadius() * contactNormal.z;
    const auto normMag = contactNormal.Magnitude();
    if (contactNormal.z <= 0.5f) {
        return CheckCanJumpFrom( // 0x5F3465
            from,
            ped.GetForward() * (normMag + pedCM->GetBoundRadius() + 1.f) // 0x5F346B, 0x5F347E, 0x5F3488 (+1.f for addition)
        );
    }
    return CheckCanJumpFrom( // 0x5F33BC
        from,
        -CVector{ contactNormal, 0.f } * (              // Yes I simplified this quite a bit, but the effect is the same
                (1.f / normMag)                         // 0x5F33CA - Normalize vector
            * (normMag + pedCM->GetBoundRadius() + 1.f) // 0x5F33F3, 0x5F3401, 0x5F340E (+1.f for addition)
            * std::min(2.f / normMag, 4.f)              // 0x5F3419
        )
    );
}

// 0x5F1C40
bool CPedGeometryAnalyser::CanPedTargetPed(CPed& ped, CPed& targetPed, bool useDirectionTest) {
    return CanPedTargetPoint(
        ped,
        targetPed.GetPosition() + CVector{ 0.f, 0.f, targetPed.GetTaskManager().GetTaskSecondary(TASK_SECONDARY_DUCK) ? -0.25f : 0.75f }, // 0.75f - 1.f = -0.25f
        useDirectionTest
    );
}

// 0x5F1B70
bool CPedGeometryAnalyser::CanPedTargetPoint(const CPed& ped, const CVector& targetPt, bool useDirectionTest) {
    const auto dir = targetPt - ped.GetPosition();
    if (useDirectionTest && dir.Dot(ped.GetForward()) < 0.f) {
        return false;
    }
    if (dir.SquaredMagnitude() > sq(40.f)) {
        return false;
    }
    return CWorld::GetIsLineOfSightClear(
        ped.GetPosition() + CVector{ 0.f, 0.f, 0.75f },
        targetPt,
        true,
        false,
        false,
        true,
        false,
        true,
        false
    );
}

// 0x5F1E30
// unused
int32 CPedGeometryAnalyser::ComputeBuildingHitPoints(const CVector& start, const CVector& target) {
    CEntity *e;
    CColPoint cp;
    CWorld::ProcessLineOfSight(start, target, cp, e, true, false, false, false, true, false, false, false);
    return CWorld::ms_iProcessLineNumCrossings;
}

// 0x5F5D80
void CPedGeometryAnalyser::ComputeClearTarget(const CPed& ped, const CVector& target, CVector& outTargetClear) {
    constexpr auto MAX_DIST_SQ = sq(5.f);

    // Start from the target point
    outTargetClear = target;

    // Check if any entities are in the way and adjust the target point accordingly
    const auto ProcessLineOfSightForEntity = [&](CEntity& e){
        if (CVector::DistSqr(e.GetPosition(), outTargetClear) >= MAX_DIST_SQ) {
            return;
        }
        if (!LiesInsideBoundingBox(ped, outTargetClear, e)) {
            return;
        }
        float depth;
        if (!GetIsLineOfSightClear(ped, outTargetClear, e, depth)) {
            return;
        }
        outTargetClear -= (outTargetClear - ped.GetPosition()).Normalized() * (ms_fPedNominalRadius + depth);
    };
    rng::for_each(ped.GetIntelligence()->GetVehicleScanner().GetEntities<CVehicle>(), ProcessLineOfSightForEntity); // 0x5F5DD0
    rng::for_each(ped.GetIntelligence()->GetPedScanner().GetEntities<CPed>(), ProcessLineOfSightForEntity); // 0x5F5EE0

    // Step away from the target point until we are clear of any entities
    // But never change the direction relative to the ped
    const auto steppingDir = (ped.GetPosition() - outTargetClear).Normalized() * ms_fPedNominalRadius;
    const auto steps       = (int32)(5.f / ms_fPedNominalRadius) + 1;
    for (auto step = 0; step < steps; step++) { // 0x5F5FDD
        const auto stepDir = (ped.GetPosition() - outTargetClear);
        if (stepDir.SquaredMagnitude() >= MAX_DIST_SQ) { // Too far?
            break;
        }
        if (stepDir.Dot(steppingDir) < 0.f) { // Direction changed relative to the ped?
            break;
        }
        if (ComputeBuildingHitPoints(ped.GetPosition(), outTargetClear) % 2 != 1) { // Clear?
            break;
        }
        outTargetClear += steppingDir; // Still not clear, so go further back
    }
    /* we really could return false here, but okay */
}

// 0x5F3B70
bool CPedGeometryAnalyser::ComputeClosestSurfacePoint(const CPed& ped, CEntity& entity, CVector& point) {
    std::array<CVector, 4> corners;
    const auto& posn = ped.GetPosition();
    ComputeEntityBoundingBoxCornersUncached(posn.z, entity, corners);
    return ComputeClosestSurfacePoint(posn, corners, point);
}

// 0x5F36F0
bool CPedGeometryAnalyser::ComputeClosestSurfacePoint(const CVector& posn, CEntity& entity, CVector& point) {
    std::array<CVector, 4> corners;
    CPedGeometryAnalyser::ComputeEntityBoundingBoxCornersUncached(posn.z, entity, corners);
    return CPedGeometryAnalyser::ComputeClosestSurfacePoint(posn, corners, point);
}

// 0x5F2C10
bool CPedGeometryAnalyser::ComputeClosestSurfacePoint(const CVector& posn, const std::array<CVector, 4>& corners, CVector& point) {
    //float closestPtDist3DSq = FLT_MAX;
    //for ()

    return plugin::CallAndReturn<bool, 0x5F2C10, const CVector&, const std::array<CVector, 4>&, CVector&>(posn, corners, point);
}

// inlined into CPedGeometryAnalyser::ComputeEntityBoundingSphere
void CPedGeometryAnalyser::ComputeEntityBoundingBoxCentre(float zPos, CEntity& entity, CVector& center) {
    ComputeEntityBoundingBoxCentreUncachedAll(zPos, entity, center);
}

// 0x5F1600
void CPedGeometryAnalyser::ComputeEntityBoundingBoxCentreUncached(float zPos, const std::array<CVector, 4>& corners, CVector& center) {
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
    std::array<CVector, 4> corners{};
    ComputeEntityBoundingBoxCornersUncached(zPos, entity, corners);
    ComputeEntityBoundingBoxCentreUncached(zPos, corners, center);
}

// 0x5F3650
void CPedGeometryAnalyser::ComputeEntityBoundingBoxCorners(float zPos, CEntity& entity, std::array<CVector, 4>& corners) {
    ComputeEntityBoundingBoxCornersUncached(zPos, entity, corners);
}

// 0x5F1FA0
void CPedGeometryAnalyser::ComputeEntityBoundingBoxCornersUncached(float zPos, CEntity& entity, std::array<CVector, 4>& corners) {
    plugin::Call<0x5F1FA0>(zPos, &entity, &corners);
}

// 0x5F3660
void CPedGeometryAnalyser::ComputeEntityBoundingBoxPlanes(float zPos, CEntity& entity, CVector(*outPlanes)[4], float* outPlanesDot) {
    ComputeEntityBoundingBoxPlanesUncachedAll(zPos, entity, outPlanes, outPlanesDot);
}

// 0x5F1670
void CPedGeometryAnalyser::ComputeEntityBoundingBoxPlanesUncached(float zPos, const std::array<CVector, 4>& corners, CVector(*outPlanes)[4], float* outPlanesDot) {
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

// 0x5F2B80
void CPedGeometryAnalyser::ComputeEntityBoundingBoxPlanesUncachedAll(float zPos, CEntity& entity, CVector (*outPlanes)[4], float* outPlanesDot) {
    std::array<CVector, 4> corners{};
    CPedGeometryAnalyser::ComputeEntityBoundingBoxCornersUncached(zPos, entity, corners);
    CPedGeometryAnalyser::ComputeEntityBoundingBoxPlanesUncached(zPos, corners, outPlanes, outPlanesDot);
}

// 0x5F36A0
void CPedGeometryAnalyser::ComputeEntityBoundingBoxSegmentPlanes(float zPos, CEntity& entity, CVector* normals, float* dots) {
    ComputeEntityBoundingBoxSegmentPlanesUncachedAll(zPos, entity, normals, dots);
}

// 0x5F1750
CVector* CPedGeometryAnalyser::ComputeEntityBoundingBoxSegmentPlanesUncached(const std::array<CVector, 4>& corners, CVector& center, CVector* a3, float* a4) {
    return plugin::CallAndReturn<CVector*, 0x5F1750>(&corners, &center, a3, a4);
}

// 0x5F2BC0
CVector* CPedGeometryAnalyser::ComputeEntityBoundingBoxSegmentPlanesUncachedAll(float zPos, CEntity& entity, CVector* a3, float* a4) {
    std::array<CVector, 4> corners{};
    CVector center;

    ComputeEntityBoundingBoxCornersUncached(zPos, entity, corners);
    ComputeEntityBoundingBoxCentreUncached(zPos, corners, center);
    return ComputeEntityBoundingBoxSegmentPlanesUncached(corners, center, a3, a4);
}

// 0x5F3C20
void CPedGeometryAnalyser::ComputeEntityBoundingSphere(const CPed& ped, CEntity& entity, CColSphere& a3) {
    return plugin::Call<0x5F3C20, const CPed&, CEntity&, CColSphere&>(ped, entity, a3);
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
int32 CPedGeometryAnalyser::ComputeEntityHitSide(const CVector& point1, const std::array<CVector, 4>& point2, const float* x) {
    return plugin::CallAndReturn<int32, 0x5F1450, const CVector&, const std::array<CVector, 4>&, const float*>(point1, point2, x);
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
CPedGeometryAnalyser::WanderPathClearness CPedGeometryAnalyser::IsWanderPathClear(const CVector& from, const CVector& to, float maxHeightChange, int32 maxSamples) {
    return plugin::CallAndReturn<WanderPathClearness, 0x5F2F70, const CVector&, const CVector&, float, int32>(from, to, maxHeightChange, maxSamples);
}

// 0x5F3880
bool CPedGeometryAnalyser::LiesInsideBoundingBox(const CPed& ped, const CVector& posn, CEntity& entity) {
    return plugin::CallAndReturn<bool, 0x5F3880, const CPed&, const CVector&, CEntity&>(ped, posn, entity);
}

// 0x41B7C0
void* CPointRoute::operator new(uint32 size) {
    return GetPointRoutePool()->New();
}

// 0x41B7D0
void CPointRoute::operator delete(void* ptr, size_t sz) {
    GetPointRoutePool()->Delete(reinterpret_cast<CPointRoute*>(ptr));
}
