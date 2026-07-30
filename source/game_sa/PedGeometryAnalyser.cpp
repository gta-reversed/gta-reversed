#include "StdInc.h"
#include <Interior/InteriorManager_c.h>
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
    RH_ScopedOverloadedInstall(ComputeClosestSurfacePoint, "rect", 0x5F2C10, bool(*)(const CVector&,const std::array<CVector, 4>&,CVector&));
    RH_ScopedInstall(ComputeEntityBoundingBoxCentreUncached, 0x5F1600);
    RH_ScopedInstall(ComputeEntityBoundingBoxCentreUncachedAll, 0x5F3B40);
    RH_ScopedInstall(ComputeEntityBoundingBoxCorners, 0x5F3650);
    RH_ScopedInstall(ComputeEntityBoundingBoxCornersUncached, 0x5F1FA0);
    RH_ScopedInstall(ComputeEntityBoundingBoxPlanes, 0x5F3660);
    RH_ScopedInstall(ComputeEntityBoundingBoxPlanesUncached, 0x5F1670);
    RH_ScopedInstall(ComputeEntityBoundingBoxPlanesUncachedAll, 0x5F2B80);
    RH_ScopedInstall(ComputeEntityBoundingBoxSegmentPlanes, 0x5F36A0);
    RH_ScopedInstall(ComputeEntityBoundingBoxSegmentPlanesUncached, 0x5F1750);
    RH_ScopedInstall(ComputeEntityBoundingBoxSegmentPlanesUncachedAll, 0x5F2BC0);
    RH_ScopedInstall(ComputeEntityBoundingSphere, 0x5F3C20);
    RH_ScopedOverloadedInstall(ComputeMoveDirToAvoidEntity, "OG", 0x5F3730, void(*)(const CPed&, CEntity&, CVector&));
    RH_ScopedInstall(ComputeEntityDirs, 0x5F1500);
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
    //
    // NOTE:
    // The code below is adapted to use `CCollision::GetClosestPtOnLine` instead of duplicating it
    // which also clamps the point to the line segment, so the redudant code at the end of the function
    // was eliminated.
    //

    float closestPtDist3DSq = FLT_MAX;
    for (uint32 i = 0; i < corners.size(); i++) {
        const auto& curr = corners[i];
        const auto& next = corners[(i + 1) % corners.size()];
        const auto  closest = CCollision::GetClosestPtOnLine(curr, next, posn);
        const auto  distSq  = (closest - posn).SquaredMagnitude();
        if (distSq < closestPtDist3DSq) {
            closestPtDist3DSq = distSq;
            point             = closest;
        }
    }
    return closestPtDist3DSq != FLT_MAX; // This is same as the original code
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
bool CPedGeometryAnalyser::ComputeEntityBoundingBoxCornersUncached(float zPos, CEntity& entity, std::array<CVector, 4>& corners) {
    if (entity.GetIsTypeBuilding() && entity.m_bIsTempBuilding) {
        if (g_interiorMan.GetBoundingBox(entity, corners.data())) {
            for (auto& corner : corners) {
                corner.z = zPos;
            }
            return true;
        }
    }

    CVector min{ FLT_MAX }, max{ FLT_MIN };
    const auto&       matrix   = entity.GetMatrix();
    const auto* const entityCM = entity.GetColModel();
    if (!entityCM) {
        return false; // BUGFIX
    }
    const auto* const entityCD = entityCM->GetData();
    if (!entityCD) {
        return false; // BUGFIX
    }
    if (entity.GetIsTypeObject() && entityCM && entityCM->GetBoundingBox().GetHeight() > 6.f && (entityCD->GetBoxes().size() || entityCD->GetSpheres().size())) {
        const auto ProcessBoxForBB = [
            &matrix,
            &min,
            &max,
            minAllowedZ = zPos - 1.f,
            maxAllowedZ = zPos + 1.f
        ](const CBox& box) {
            const auto bmin = box.m_vecMin,
                       bmax = box.m_vecMax;

            // Check intersection on Z axis in world-space
            // Can't do this the other way around, because the
            // I've simplified this, because there's no reason to transpose anything other than the Z axis
            {
                const auto TransformObjecToWorldSpaceZ = [&matrix](float z) -> float {
                    return (z + matrix.GetPosition().z) * matrix.GetUp().z;
                };
                const auto bminZ = TransformObjecToWorldSpaceZ(bmin.z);
                const auto bmaxZ = TransformObjecToWorldSpaceZ(bmax.z);

                //
                // This is the original code, it makes no sense, so I've simplified
                // 
                //if (bminZ < minAllowedZ && bmaxZ < minAllowedZ) { // NB: What's the point of testing min.z too, if max.z is supposed to be higher?
                //    continue;
                //}
                //
                //if (bminZ > maxAllowedZ && bmaxZ > maxAllowedZ) { // NB: What's the point of testing max.z too, if min.z is supposed to be lower?
                //    continue;
                //}

                assert(bmax.z >= bmin.z);
                if (bmax.z < minAllowedZ || bmin.z > maxAllowedZ) {
                    return;
                }
            }

            min.x = std::min(min.x, bmin.x);
            min.y = std::min(min.y, bmin.y);
            min.z = std::min(min.z, bmin.z);

            max.x = std::max(max.x, bmax.x);
            max.y = std::max(max.y, bmax.y);
            max.z = std::max(max.z, bmax.z);
        };

        for (const auto& box : entityCD->GetBoxes()) {
            ProcessBoxForBB(box);
        }
        for (const auto& sphere : entityCD->GetSpheres()) {
            ProcessBoxForBB(sphere.GetBoundingBox());
        }
    } else {
        const auto& bb = entityCM->GetBoundingBox();
        min            = bb.m_vecMin;
        max            = bb.m_vecMax;
    }

    min -= CVector{ ms_fPedNominalRadius }; // 0x5F23D7
    max += CVector{ ms_fPedNominalRadius }; // 0x5F23FB

    const auto halfExtent   = (max - min) * 0.5f; // 0x5F246C

    // Code below is combined code of all the `if` branches found below
    // And after 0x5F28F8
    const auto CalculateBB  = [&](
        CVector   principal, float extP,
        CVector2D axisA,     float extA,
        CVector2D axisB,     float extB
    ) {
        //
        // We're using the Separating Axis Theorem (SAT) to calculate the bounding box corners
        // It works by projecting the two axes of the bounding box onto a new 2D grid,
        // which is defined by the principal axis and its perpendicular vector.
        // This approach differs a bit from the original code, as they've instead used the bb's two axes as the new grid,
        // but the result is the same.
        // I've spent a whole day on figuring this shit out, and I'm mad. Good night. (P)
        // 
        // ▲ Perpendicular Vector (V)
        // │                      * 
        // │                     / \
        // │               av   /   \    bv 
        // │                   /     \  
        // │                  /       \
        // │                 *         * 
        // │                  \       /  
        // │               au  \     /   bu
        // │                    \   /    
        // │                     \ /
        // └──────────────────────*──────────────────────► Principal Axis (U)
        //

        // Our grid's vectors (Originally axisA and axisB, but it's simpler like this)
        const auto u = CVector2D{ principal }.Normalized();
        const auto v = u.GetPerpRight();

        // Project Axis A (Width) onto our new 2D U/V grid
        const auto au = std::abs(u.Dot(axisA) * extA);
        const auto av = std::abs(v.Dot(axisA) * extA);

        // Project Axis B (Length) onto our new 2D U/V grid
        const auto bu = std::abs(u.Dot(axisB) * extB);
        const auto bv = std::abs(v.Dot(axisB) * extB);

        // Calculate Separating Axis Theorem (SAT)
        const auto extentU = std::abs(au) + std::abs(bu);
        const auto extentV = std::abs(av) + std::abs(bv);

        // Center of the bounding box in world-space
        const CVector2D center  = matrix.TransformPoint((min + max) * 0.5f);

        // Calculate the offsets along the U and V axes
        const CVector2D offsetU = u * extentU,
                        offsetV = v * extentV;

        // Calculate the two extreme points along the principal axis
        const CVector2D ptA     = center + (principal * extP),
                        ptB     = center - (principal * extP);

        // Calculate the four corners of the bounding box in world-space
        corners[0]              = CVector{ ptA + offsetU - offsetV, zPos }; // Top Left
        corners[1]              = CVector{ ptB - offsetU - offsetV, zPos }; // Bottom Left
        corners[2]              = CVector{ ptB - offsetU + offsetV, zPos }; // Bottom Right
        corners[3]              = CVector{ ptA + offsetU + offsetV, zPos }; // Top Right
    };

    // 0x5F24E5 - Weights used to select the dominant axis for the bounding box calculation 
    const auto weightX = 2.f * halfExtent.x * CVector2D{ matrix.GetRight() }.SquaredMagnitude(),
               weightY = 2.f * halfExtent.y * CVector2D{ matrix.GetForward() }.SquaredMagnitude(),
               weightZ = 2.f * halfExtent.z * CVector2D{ matrix.GetUp() }.SquaredMagnitude();

    // Based on the weights, select the axis we want to work on
    if (weightY > weightX && weightY > weightZ) { // 0x5F2523 (Inverted) - Dominant Y Axis
        CalculateBB(
            matrix.GetForward(), halfExtent.y,
            matrix.GetRight(), halfExtent.x,
            matrix.GetUp(), halfExtent.z
        );
    } else if (weightX <= weightZ) { // 0x5F268A - Dominant Z Axis
        CalculateBB(
            matrix.GetUp(), halfExtent.z,
            matrix.GetRight(), halfExtent.x,
            matrix.GetForward(), halfExtent.y
        );
    } else { // 0x5F26A8 - Dominant X Axis
        CalculateBB(
            matrix.GetRight(), halfExtent.x,
            matrix.GetForward(), halfExtent.y,
            matrix.GetUp(), halfExtent.z
        );
    }

    return true; // 0x5F1FEF
}

// 0x5F3660
void CPedGeometryAnalyser::ComputeEntityBoundingBoxPlanes(float zPos, CEntity& entity, std::array<CVector, 4>& outPlanes, std::array<float, 4>& outPlanesDot) {
    ComputeEntityBoundingBoxPlanesUncachedAll(zPos, entity, outPlanes, outPlanesDot);
}

// 0x5F1670
void CPedGeometryAnalyser::ComputeEntityBoundingBoxPlanesUncached(float zPos, const std::array<CVector, 4>& corners, std::array<CVector, 4>& outPlanes, std::array<float, 4>& outPlanesDot) {
    const auto N = corners.size();

    UNUSED(zPos);

    for (size_t i = 0; i < corners.size(); i++) {
        const CVector2D prev = corners[(i + N - 1) % N];
        const CVector2D curr = corners[i];

        // Normal vector of the plane is perpendicular to the edge of the bounding box, pointing outwards
        const CVector2D normal = (curr - prev).GetPerpRight().Normalized();

        // Store the normal vector of the plane
        outPlanes[i] = CVector{ normal, 0.f };

        // point-normal plane equation:
        // ax + by + d = 0
        // d = - n . P
        outPlanesDot[i] = -normal.Dot(prev);
    }
}

// 0x5F2B80
void CPedGeometryAnalyser::ComputeEntityBoundingBoxPlanesUncachedAll(float zPos, CEntity& entity, std::array<CVector, 4>& outPlanes, std::array<float, 4>& outPlanesDot) {
    std::array<CVector, 4> corners{};
    CPedGeometryAnalyser::ComputeEntityBoundingBoxCornersUncached(zPos, entity, corners);
    CPedGeometryAnalyser::ComputeEntityBoundingBoxPlanesUncached(zPos, corners, outPlanes, outPlanesDot);
}

// 0x5F36A0
void CPedGeometryAnalyser::ComputeEntityBoundingBoxSegmentPlanes(float zPos, CEntity& entity, std::array<CVector, 4>& outNormals, std::array<float, 4>& outPlanesDot) {
    ComputeEntityBoundingBoxSegmentPlanesUncachedAll(zPos, entity, outNormals, outPlanesDot);
}

// 0x5F1750
void CPedGeometryAnalyser::ComputeEntityBoundingBoxSegmentPlanesUncached(const std::array<CVector, 4>& corners, CVector& center, std::array<CVector, 4>& outNormals, std::array<float, 4>& outPlanesDot) {
    const auto center2D = CVector2D{ center };
    for (size_t i = 0; i < corners.size(); i++) {
        const auto corner2D = CVector2D{ corners[i] };
        const auto normal2D = (corner2D - center2D).GetPerpLeft();
        outNormals[i]       = CVector{ normal2D, 0.f };

        // point-normal plane equation:
        // ax + by + d = 0
        // d = - n . P
        outPlanesDot[i] = -normal2D.Dot(corner2D);
    }
}

// 0x5F2BC0
void CPedGeometryAnalyser::ComputeEntityBoundingBoxSegmentPlanesUncachedAll(float zPos, CEntity& entity, std::array<CVector, 4>& outNormals, std::array<float, 4>& outPlanesDot) {
    std::array<CVector, 4> corners{};
    CVector center;
    ComputeEntityBoundingBoxCornersUncached(zPos, entity, corners);
    ComputeEntityBoundingBoxCentreUncached(zPos, corners, center);
    ComputeEntityBoundingBoxSegmentPlanesUncached(corners, center, outNormals, outPlanesDot);
}

// 0x5F3C20
void CPedGeometryAnalyser::ComputeEntityBoundingSphere(const CPed& ped, CEntity& entity, CColSphere& out) {
    CVector                center{};
    std::array<CVector, 4> corners{};
    const auto             zPos = ped.GetPosition().z;
    ComputeEntityBoundingBoxCornersUncached(zPos, entity, corners);
    //ComputeEntityBoundingBoxCornersUncached(zPos, entity, corners); // Why?
    ComputeEntityBoundingBoxCentreUncached(zPos, corners, center);

    float radiusSq = 0.f;
    for (const auto& corner : corners) {
        radiusSq = std::max(radiusSq, (corner - center).SquaredMagnitude2D()); // z is same for all, so we can just ignore it
    }

    out.Set(std::sqrt(radiusSq), center);
}

// 0x5F3730
void CPedGeometryAnalyser::ComputeMoveDirToAvoidEntity(const CPed& ped, CEntity& entity, CVector& outDirToAvoidEntity) {
    outDirToAvoidEntity = ComputeMoveDirToAvoidEntity(ped, entity);
}

// notsa, code from 0x5F3730
CVector CPedGeometryAnalyser::ComputeMoveDirToAvoidEntity(const CPed& ped, CEntity& entity) {
    std::array<CVector, 4> planes{};
    std::array<float, 4>   planesDot{};
    ComputeEntityBoundingBoxPlanes(ped.GetPosition().z, entity, planes, planesDot);
    const auto GetDotProductOnPlane = [&] (int32 i) {
        return planes[i].Dot(ped.GetPosition()) + planesDot[i];
    };

    const auto planeA = GetDotProductOnPlane(1);
    if (planeA > 0.f) {
        return planes[1];
    }

    const auto planeB = GetDotProductOnPlane(3);
    if (planeB > 0.f) {
        return planes[3];
    }

    return planeA <= planeB
        ? planes[1]
        : planes[3];
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
void CPedGeometryAnalyser::ComputeEntityDirs(const CEntity& entity, std::array<CVector, 4>& outDirs) {
    const auto fwd                 = entity.GetForwardVector();
    const auto right               = entity.GetRightVector();
    outDirs[+eDirection::FORWARD]  = fwd;
    outDirs[+eDirection::LEFT]     = -right;
    outDirs[+eDirection::BACKWARD] = -fwd;
    outDirs[+eDirection::RIGHT]    = right;
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
