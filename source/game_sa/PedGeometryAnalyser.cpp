#include "StdInc.h"
#include <Interior/InteriorManager_c.h>
#include "PedGeometryAnalyser.h"
#include <reversiblebugfixes/Bugs.hpp>

#include <Tasks/TaskTypes/TaskSimpleClimb.h>
#include <Tasks/TaskTypes/TaskComplexJump.h>

/* Clarifications *
 *
 * Normals used in this code all point outwards
 * The winding order is counter-clockwise:
 * - Top Left
 * - Bottom Left
 * - Bottom Right
 * - Top Right
 */


void CPedGeometryAnalyser::InjectHooks() {
    RH_ScopedClass(CPedGeometryAnalyser);
    RH_ScopedCategoryGlobal();

    RH_ScopedOverloadedInstall(CanPedJumpObstacle, "LoS", 0x5F1B00, bool(*)(const CPed&,const CEntity&));
    RH_ScopedOverloadedInstall(CanPedJumpObstacle, "Contacted", 0x5F32D0, bool(*)(const CPed&,const CEntity&,const CVector&,const CVector&));
    RH_ScopedInstall(CanPedTargetPed, 0x5F1C40);
    RH_ScopedInstall(CanPedTargetPoint, 0x5F1B70);
    RH_ScopedInstall(ComputeBuildingHitPoints, 0x5F1E30);
    RH_ScopedInstall(ComputeClearTarget, 0x5F5D80);
    RH_ScopedOverloadedInstall(ComputeClosestSurfacePoint, "ped", 0x5F3B70, bool (*)(const CPed&, CEntity&, CVector&));
    RH_ScopedOverloadedInstall(ComputeClosestSurfacePoint, "pos", 0x5F36F0, bool(*)(const CVector&,CEntity&,CVector&));
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
    RH_ScopedOverloadedInstall(ComputeEntityHitSide, "1", 0x5F3BC0, eDirection(*)(const CPed&, CEntity&));
    RH_ScopedOverloadedInstall(ComputeEntityHitSide, "2", 0x5F1450, eDirection(*)(const CVector&, const std::array<CVector, 4>&, const std::array<float, 4>&));
    RH_ScopedOverloadedInstall(ComputeEntityHitSide, "3", 0x5F3AC0, eDirection(*)(const CVector&, CEntity&));
    RH_ScopedOverloadedInstall(ComputePedHitSide, "physical", 0x5F3640, eDirection(*)(const CPed&,const CPhysical&));
    RH_ScopedOverloadedInstall(ComputePedHitSide, "pos", 0x5F1E70, eDirection(*)(const CPed&,const CVector&));
    RH_ScopedInstall(ComputePedShotSide, 0x5F13F0);
    RH_ScopedOverloadedInstall(ComputeRouteRoundEntityBoundingBox, "Entity", 0x5F6110, int32(*)(const CPed&,CEntity&,const CVector&,CPointRoute&,int32));
    RH_ScopedOverloadedInstall(ComputeRouteRoundEntityBoundingBox, "2", 0x5F3DD0, int32(*)(const CPed&,const CVector&,CEntity&,const CVector&,CPointRoute&,int32));
    RH_ScopedInstall(ComputeRouteRoundSphere, 0x5F1890);
    RH_ScopedOverloadedInstall(GetIsLineOfSightClear, "ped", 0x5F5A30, bool(*)(const CPed&,const CVector&,CEntity&,float&));
    RH_ScopedOverloadedInstall(GetIsLineOfSightClear, "v3d", 0x5F2F00, bool(*)(const CVector&,const CVector&,CEntity&));
    RH_ScopedInstall(GetNearestPed, 0x5F3590);
    RH_ScopedInstall(IsEntityBlockingTarget, 0x5F3970);
    RH_ScopedInstall(IsInAir, 0x5F1CB0);
    RH_ScopedInstall(IsWanderPathClear, 0x5F2F70);
    RH_ScopedInstall(LiesInsideBoundingBox, 0x5F3880);
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
bool CPedGeometryAnalyser::ComputeClosestSurfacePoint(const CPed& ped, CEntity& entity, CVector& outPoint) {
    std::array<CVector, 4> corners;
    const auto& pos = ped.GetPosition();
    ComputeEntityBoundingBoxCornersUncached(pos.z, entity, corners);
    return ComputeClosestSurfacePoint(pos, corners, outPoint);
}

// 0x5F36F0
bool CPedGeometryAnalyser::ComputeClosestSurfacePoint(const CVector& pos, CEntity& entity, CVector& outPoint) {
    std::array<CVector, 4> corners;
    CPedGeometryAnalyser::ComputeEntityBoundingBoxCornersUncached(pos.z, entity, corners);
    return CPedGeometryAnalyser::ComputeClosestSurfacePoint(pos, corners, outPoint);
}

// 0x5F2C10
bool CPedGeometryAnalyser::ComputeClosestSurfacePoint(const CVector& pos, const std::array<CVector, 4>& corners, CVector& outPoint) {
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
        const auto  closest = CCollision::GetClosestPtOnLine(curr, next, pos);
        const auto  distSq  = (closest - pos).SquaredMagnitude();
        if (distSq < closestPtDist3DSq) {
            closestPtDist3DSq = distSq;
            outPoint          = closest;
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
void CPedGeometryAnalyser::ComputeEntityBoundingBoxPlanes(float zPos, CEntity& entity, std::array<CVector, 4>& outPlaneNormals, std::array<float, 4>& outPlaneDs) {
    ComputeEntityBoundingBoxPlanesUncachedAll(zPos, entity, outPlaneNormals, outPlaneDs);
}

// 0x5F1670
void CPedGeometryAnalyser::ComputeEntityBoundingBoxPlanesUncached(float zPos, const std::array<CVector, 4>& corners, std::array<CVector, 4>& outPlaneNormals, std::array<float, 4>& outPlaneDs) {
    const auto N = corners.size();

    UNUSED(zPos);

    for (size_t i = 0; i < corners.size(); i++) {
        const CVector2D prev = corners[(i + N - 1) % N];
        const CVector2D curr = corners[i];

        // Normal vector of the plane is perpendicular to the edge of the bounding box, pointing outwards
        const CVector2D normal = (curr - prev).GetPerpRight().Normalized();

        // Store the normal vector of the plane
        outPlaneNormals[i] = CVector{ normal, 0.f };

        // point-normal plane equation:
        // ax + by + d = 0
        // d = - n . P
        outPlaneDs[i] = -normal.Dot(prev);
    }
}

// 0x5F2B80
void CPedGeometryAnalyser::ComputeEntityBoundingBoxPlanesUncachedAll(float zPos, CEntity& entity, std::array<CVector, 4>& outPlaneNormals, std::array<float, 4>& outPlaneDs) {
    std::array<CVector, 4> corners{};
    CPedGeometryAnalyser::ComputeEntityBoundingBoxCornersUncached(zPos, entity, corners);
    CPedGeometryAnalyser::ComputeEntityBoundingBoxPlanesUncached(zPos, corners, outPlaneNormals, outPlaneDs);
}

// 0x5F36A0
void CPedGeometryAnalyser::ComputeEntityBoundingBoxSegmentPlanes(float zPos, CEntity& entity, std::array<CVector, 4>& outSegPlaneNormals, std::array<float, 4>& outPlaneDs) {
    ComputeEntityBoundingBoxSegmentPlanesUncachedAll(zPos, entity, outSegPlaneNormals, outPlaneDs);
}

// 0x5F1750
void CPedGeometryAnalyser::ComputeEntityBoundingBoxSegmentPlanesUncached(const std::array<CVector, 4>& corners, CVector& center, std::array<CVector, 4>& outSegPlaneNormals, std::array<float, 4>& outPlaneDs) {
    const auto center2D = CVector2D{ center };
    for (size_t i = 0; i < corners.size(); i++) {
        const auto corner2D = CVector2D{ corners[i] };
        const auto normal2D = (corner2D - center2D).GetPerpLeft();
        outSegPlaneNormals[i]       = CVector{ normal2D, 0.f };

        // point-normal plane equation:
        // ax + by + d = 0
        // d = - n . P
        outPlaneDs[i] = -normal2D.Dot(corner2D);
    }
}

// 0x5F2BC0
void CPedGeometryAnalyser::ComputeEntityBoundingBoxSegmentPlanesUncachedAll(float zPos, CEntity& entity, std::array<CVector, 4>& outSegPlaneNormals, std::array<float, 4>& outPlaneDs) {
    std::array<CVector, 4> corners{};
    CVector center;
    ComputeEntityBoundingBoxCornersUncached(zPos, entity, corners);
    ComputeEntityBoundingBoxCentreUncached(zPos, corners, center);
    ComputeEntityBoundingBoxSegmentPlanesUncached(corners, center, outSegPlaneNormals, outPlaneDs);
}

// 0x5F3C20
void CPedGeometryAnalyser::ComputeEntityBoundingSphere(const CPed& ped, CEntity& entity, CColSphere& out) {
    CVector                center{};
    std::array<CVector, 4> corners{};
    const auto             zPos = ped.GetPosition().z;
    ComputeEntityBoundingBoxCornersUncached(zPos, entity, corners);
    //ComputeEntityBoundingBoxCornersUncached(zPos, entity, corners); // NOTE: These functions are inlined from another one, that's why it was doing this... I won't be.
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
    std::array<CVector, 4> planeNormals{};
    std::array<float, 4>   planeDs{};
    ComputeEntityBoundingBoxPlanes(ped.GetPosition().z, entity, planeNormals, planeDs);
    const auto GetDotProductOnPlane = [&] (int32 i) {
        return planeNormals[i].Dot(ped.GetPosition()) + planeDs[i];
    };

    const auto planeA = GetDotProductOnPlane(1);
    if (planeA > 0.f) {
        return planeNormals[1];
    }

    const auto planeB = GetDotProductOnPlane(3);
    if (planeB > 0.f) {
        return planeNormals[3];
    }

    return planeA <= planeB
        ? planeNormals[1]
        : planeNormals[3];
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
eDirection CPedGeometryAnalyser::ComputeEntityHitSide(const CPed& ped, CEntity& entity) {
    return ComputeEntityHitSide(ped.GetPosition(), entity);
}

// 0x5F1450
eDirection CPedGeometryAnalyser::ComputeEntityHitSide(const CVector& point, const std::array<CVector, 4>& segmentPlaneNormals, const std::array<float, 4>& segmentPlaneDots) {
    const auto N = segmentPlaneNormals.size();

    for (size_t curr = 0; curr < N; curr++) {
        const auto prev = (curr + N - 1) % N;

        const auto GetDotProductOnPlane = [&](int32 i) {
            return segmentPlaneNormals[i].Dot(point) + segmentPlaneDots[i];
        };

        if (GetDotProductOnPlane(prev) >= 0.f && GetDotProductOnPlane(curr) < 0.f) {
            return (eDirection)(curr);
        }
    }

    return eDirection::FORWARD;
}

// 0x5F3AC0
eDirection CPedGeometryAnalyser::ComputeEntityHitSide(const CVector& point, CEntity& entity) {
    std::array<CVector, 4> planeNormals{};
    std::array<float, 4>   planeDs{};
    ComputeEntityBoundingBoxSegmentPlanes(point.z, entity, planeNormals, planeDs);
    return ComputeEntityHitSide(point, planeNormals, planeDs);
}

// 0x5F3640
eDirection CPedGeometryAnalyser::ComputePedHitSide(const CPed& ped, const CPhysical& physical) {
    return ComputePedHitSide(ped, physical.m_vecMoveSpeed);
}

// 0x5F1E70
eDirection CPedGeometryAnalyser::ComputePedHitSide(const CPed& ped, const CVector& hitDir) {
    std::array<CVector, 4> dirs{};
    ComputeEntityDirs(ped, dirs); // NOTE/BUG: It returns non-normalized vectors, so if the ped is scaled, the hit side may be wrong.

    // Side with the most similar direction to the hit velocity is the side that was hit
    const auto normal = -hitDir.Normalized();
    float      max    = FLT_MIN;
    eDirection dir    = eDirection::FORWARD;
    for (size_t i = 0; i < dirs.size(); i++) {
        const auto dot = dirs[i].Dot(normal);
        if (dot > max) {
            max = dot;
            dir = (eDirection)(i);
        }
    }
    return dir;
}

// 0x5F13F0
eDirection CPedGeometryAnalyser::ComputePedShotSide(const CPed& ped, const CVector& pos) {
    auto angle = (pos - ped.GetPosition()).Heading() - ped.m_fCurrentRotation + PI / 4.f;
    if (angle < 0.f) {
        angle += TWO_PI;
    }
    return (eDirection)(((int32)(angle / (PI / 2.f))) % 4);
}

// 0x5F6110
int32 CPedGeometryAnalyser::ComputeRouteRoundEntityBoundingBox(const CPed& ped, CEntity& entity, const CVector& target, CPointRoute& route, int32 forceDirection) {
    return ComputeRouteRoundEntityBoundingBox(ped, ped.GetPosition(), entity, target, route, forceDirection);
}

// 0x5F3DD0
int32 CPedGeometryAnalyser::ComputeRouteRoundEntityBoundingBox(const CPed& ped, const CVector& from, CEntity& entity, const CVector& to, CPointRoute& outRoute, int32 forceDirection) {
    outRoute.Clear();

    const auto prevPedNominalRadius = std::exchange(ms_fPedNominalRadius, 0.175f);
    const auto zPos                 = ped.GetPosition().z;
     
    std::array<CVector, 4> bbCorners;
    ComputeEntityBoundingBoxCorners(zPos, entity, bbCorners);

    std::array<CVector, 4> bbPlaneNormals;
    std::array<float, 4>   bbPlaneDs;
    ComputeEntityBoundingBoxPlanes(zPos, entity, bbPlaneNormals, bbPlaneDs);

    ms_fPedNominalRadius = prevPedNominalRadius;

    // Calculates `n . P0 + d` - the distance from the point to the plane along the plane's normal
    // If this distance is positive, the point lies above the plane, otherwise below it
    const auto GetPointDistanceToPlane = [&](int32 pl, CVector pos) {
        return bbPlaneNormals[pl].Dot(pos) + bbPlaneDs[pl];
    };

    // Check if we need to go around the entity at all,
    // If both start and target are in front of the same plane then there's no need
    for (size_t pl = 0; pl < bbPlaneNormals.size(); pl++) { // 0x5F3E95
        const auto IsPosInFrontOfPlane = [&, pl](CVector pos) {
            return GetPointDistanceToPlane(pl, pos) > 0.f;
        };
        if (IsPosInFrontOfPlane(from) && IsPosInFrontOfPlane(to)) {
            return 0;
        }
    }

    // Check if start and target can be approached without having to go around the entity
    if (ComputeEntityHitSide(to, bbPlaneNormals, bbPlaneDs) == ComputeEntityHitSide(from, bbPlaneNormals, bbPlaneDs)) { // 0x5F3FDD
        return 0;
    }

    // Calculate the intersection point of a line with a plane
    const auto GetLineIntersectionWithPlane = [&](int32 pl, CVector origin, CVector lineDir) {
        // t = - (n . P0 + d) / (n . dir)
        // we want to find the point on the line that intersects the plane, so we can use the parametric equation of the line:
        // P(t) = P0 + dir * t
        const auto t = -GetPointDistanceToPlane(pl, origin) / bbPlaneNormals[pl].Dot(lineDir);
        return origin + lineDir * t;
    };

    // Calculate points to be on the planes of the entity's bounding box
    CVector fromOnPlane = from,
            toOnPlane   = to;
    {
        const auto GetMoveDirectionIntersectionWithPlane = [&, moveDir = (to - from).Normalized()](int32 pl) {
            return GetLineIntersectionWithPlane(pl, from, moveDir);
        };

        bool hasPlaneWithNoCrossings = false;
        for (int32 pl = 0; pl < 4; pl++) { // 0x5F4118
            constexpr auto EDGE_TRESHOLD = 0.2f;

            //
            // For completeness, the original code:
            //
            //enum {
            //    RELPOS_BELOW = -1,
            //    RELPOS_EDGE  = 0,
            //    RELPOS_ABOVE = 1,
            //};
            //const auto GetRelativePositionToPlane = [&, pl](float dot) { // 0x5F412E, 0x5F4180
            //    if (dot > +EDGE_TRESHOLD) {
            //        return RELPOS_ABOVE;
            //    }
            //    if (dot < -EDGE_TRESHOLD) {
            //        return RELPOS_BELOW;
            //    }
            //    return RELPOS_EDGE;
            //};
            //
            //const auto toDotOnPlane   = GetDotProductOnPlane(pl, to),
            //           fromDotOnPlane = GetDotProductOnPlane(pl, from);
            //
            //const auto toRelPos     = GetRelativePositionToPlane(toDot),
            //           fromRelPos   = GetRelativePositionToPlane(fromDot);
            //
            //// Check if we've crossed the plane
            //if (fromRelPos == RELPOS_BELOW) { // 0x5F41A7
            //    if (toRelPos == RELPOS_ABOVE) {
            //        to = GetIntersectionOnPlane(pl, start, moveDir);
            //    }
            //}
            //else if (toRelPos == RELPOS_BELOW) { // 0x5F41B0
            //    if (fromRelPos == RELPOS_ABOVE) {
            //        from = GetIntersectionOnPlane(pl, start, moveDir);
            //    }
            //} else { // No crossing of the plane
            //    hasPlaneWithNoCrossings = true;
            //
            //    // Adjust points that are currently below to be on exactly on the plane
            //    const auto CalculateOffsetToPlane = [&](float dot) {
            //        return bbPlaneNormals[pl] * (EDGE_TRESHOLD - dot);
            //    };
            //    if (fromRelPos == RELPOS_EDGE) { // 0x5F41BA
            //        from += CalculateOffsetToPlane(fromDot); // Move point
            //    }
            //    if (toRelPos == RELPOS_EDGE) { // 0x5F41FE
            //        to += CalculateOffsetToPlane(toDot);
            //    }
            //}

            const auto IsAbove = [](float dot) {
                return dot > EDGE_TRESHOLD;
            };
            const auto IsBelow = [](float dot) {
                return dot < -EDGE_TRESHOLD;
            };
            const auto IsOnEdge = [](float dot) {
                return std::abs(dot) <= EDGE_TRESHOLD;
            };

            const auto toDot   = GetPointDistanceToPlane(pl, toOnPlane),
                       fromDot = GetPointDistanceToPlane(pl, fromOnPlane);

            if (IsBelow(fromDot) && IsAbove(toDot)) {                    // Crosses plane to -> from
                toOnPlane = GetMoveDirectionIntersectionWithPlane(pl);   // 0x5F428E - Move target to the intersection point on the plane
            } else if (IsAbove(fromDot) && IsBelow(toDot)) {             // Crosses plane from -> to
                fromOnPlane = GetMoveDirectionIntersectionWithPlane(pl); // 0x5F4337 - Move start to the intersection point on the plane
            } else if (!IsBelow(fromDot) && !IsBelow(toDot)) {           // 0x5F41B8
                hasPlaneWithNoCrossings = true;

                // Adjust points that are currently considered to be on the edge to instead be within treshold
                const auto GetOffsetToPlane = [&](float amount) {
                    return bbPlaneNormals[pl] * (EDGE_TRESHOLD - amount);
                };
                if (IsOnEdge(toDot)) {
                    toOnPlane += GetOffsetToPlane(toDot); // 0x5F41C2
                }
                if (IsOnEdge(fromDot)) {
                    fromOnPlane += GetOffsetToPlane(fromDot); // 0x5F420A
                }
            }
        }

        // In case there's a plane where neither start nor target are below the plane
        // we may be able to go straight to the target without having to go around the entity at all (if we don't intersect the bound sphere)
        // Otherwise we calculate the 2 new points where we intersect the entity's bounding sphere (if we intersct the bound sphere)
        // and generate the route for going around that part
        // (We don't intersect any other part of the entity, other than the sphere)
        if (!hasPlaneWithNoCrossings) { // 0x5F43AE
            float      distSq;
            const auto dir2D = (CVector2D{ from } - CVector2D{ to }).Normalized(&distSq);
            if (distSq == 0.f) { // Yeah, just about time to check this lol
                return 0;        // start == end
            }
            CColSphere sp;
            ComputeEntityBoundingSphere(ped, entity, sp);
            if (!sp.IntersectRay(from, CVector{ dir2D, 0.f }, fromOnPlane, toOnPlane)) {
                return 0; // No intersect on the sphere, so we can actually go start -> target without having to go around
            }
        }
    }

    // This is pretty much another pass of the algorithm above but more strict (no edges)
    // This now sticks the positions exactly onto the entity's bounding box,
    // instead of just the planes
    int32 fromPlane = -1,
          toPlane   = -1;
    {
        const auto moveDir                               = (toOnPlane - fromOnPlane).Normalized(); // 0x5F4485
        const auto GetMoveDirectionIntersectionWithPlane = [&, origin = fromOnPlane](int32 pl) {
            return GetLineIntersectionWithPlane(pl, origin, moveDir);
        };
        for (int32 pl = 0; pl < 4; pl++) { // 0x5F4537
            const auto fromDot = GetPointDistanceToPlane(pl, fromOnPlane),
                       toDot   = GetPointDistanceToPlane(pl, toOnPlane);
            if (toDot > 0.f && fromDot < 0.f) { // 0x5F4637 - , `to` is above, `from` is below
                toPlane   = pl;
                toOnPlane = GetMoveDirectionIntersectionWithPlane(pl);
            }
            if (fromDot > 0.f && toDot < 0.f) { // 0x5F4576 - `from` is above, `to` is below
                fromPlane   = pl;
                fromOnPlane = GetMoveDirectionIntersectionWithPlane(pl);
            }
        }
    }
    if (fromPlane == -1 || toPlane == -1) { // 0x5F4DA8
        return 0;
    }

    // Build the route from the corner's of the planes the calculated points lie on
    const auto BuildRoute = [&](int32 planeFrom, int32 planeTo, int32 dir) {
        CPointRoute r{}; // We're using this instead of a simple array for the added functionality
        r.Add(from);
        const int32 a = (planeFrom + 4) % 4,
                    b = (planeTo + 4) % 4;
        for (int32 i = a; i != b; i = (i + 4 + dir) % 4) { // 0x5F4E2A
            r.Add(bbCorners[i]);
        }
        r.Add(to);
        return r;
    };
    const auto routeL     = BuildRoute(fromPlane - 1, toPlane - 1, -1),
               routeR     = BuildRoute(fromPlane, toPlane, 1);

    // Pre-process route before returning
    const auto PreProcess = [&](int32 ret, const CPointRoute& result) { // Code from 0x5F5828
        assert(result.GetSize() > 2);                                   // `from` and `to` are always included, so we need at least 3 points to have a valid route

        outRoute = result;
        if (ComputeEntityHitSide(from, entity) == ComputeEntityHitSide(outRoute[0], entity)) { // Pop start if it's not necessary
            outRoute.PopFront();                                                               // 0x5F59D4
        }
        outRoute.PopBack(); // 0x5F5A11 - `to` always goes
        return ret;
    };

    switch (forceDirection) {
    case 0: { // 0x5F4FE9 - If no direction is forced, calculate both, and use shortest viable route
        CVector bbCenter;
        ComputeEntityBoundingBoxCentreUncached(zPos, bbCorners, bbCenter);
        const auto bbCenter2D   = CVector2D{ bbCenter };

        const auto ProcessRoute = [&](const CPointRoute& route) {
            struct ProcessedRoute {
                size_t   LastProcessedPoint{ 1 };
                CEntity* BlockedByEntity{};
            };
            if (route.GetSize() < 2) {
                return ProcessedRoute{};
            }
            ProcessedRoute res{};
            for (res.LastProcessedPoint = 1; res.LastProcessedPoint < route.GetSize(); res.LastProcessedPoint++) {
                const auto &curr = route[res.LastProcessedPoint],
                           &prev = route[res.LastProcessedPoint - 1];
                const auto dir   = (curr - prev).Normalized();

                // Check if LoS is blocked by entities other than `ped` or `entity` itself
                const auto CheckLoSIsBlocked = [&](const CVector2D& origin, const CVector2D& target) {
                    CColPoint cp{};
                    if (!CWorld::ProcessLineOfSight(
                            CVector{ origin, zPos },
                            CVector{ target, zPos },
                            cp,
                            res.BlockedByEntity,
                            true,
                            true,
                            true,
                            true,
                            false,
                            false,
                            false,
                            false
                        )) {
                        return false;
                    }
                    if (res.BlockedByEntity == &entity || res.BlockedByEntity == &ped) {
                        res.BlockedByEntity = nullptr;
                        return false;
                    }
                    return true;
                };
                if (!CheckLoSIsBlocked(curr, prev)) { // 0x5F51FC, 0x5F54CF
                    // I've re-ordered the code a little, so it's a bit out of order now
                    // this way the shit below is calculated conditionally
                    const CVector2D curr2D = curr, // 0x5F50C3, 0x5F5393
                        prev2D             = prev;

                    CVector2D offset2D     = CVector2D{ dir } * 0.5f;
                    if (curr2D.Dot(curr2D - bbCenter2D) < 0.f) { // 0x5F5137, 0x5F5407
                        offset2D = -offset2D;
                    }

                    if (!CheckLoSIsBlocked(prev + offset2D, curr + offset2D)) {
                        continue; // Not blocked by anything, so off we go
                    }
                }

                // LoS is blocked by an entity other than `ped` or `entity` itself
                if (res.BlockedByEntity->GetIsTypePed()) { // 0x5F5280, 0x5F555B
                    const auto* hitPed = res.BlockedByEntity->AsPed();
                    if (hitPed->m_nMoveState != PEDMOVE_STILL) {
                        if (dir.Dot(hitPed->GetForward()) > 0.f) { // 0x5F52AE, 0x5F558E
                            continue;                              // The ped is going the same direction as us, so they aren't an issue
                        }
                    }
                }

                // The entity we've hit is an issue, we stop now

                break;
            }
            return res;
        };

        const auto routeProcessedA         = ProcessRoute(routeL),
                   routeProcessedB         = ProcessRoute(routeR);

        const auto isRouteClearA           = !routeProcessedA.BlockedByEntity,
                   isRouteClearB           = !routeProcessedB.BlockedByEntity;

        const auto PreProcessShortestRoute = [&]() {
            return routeL.GetLengthSq() < routeR.GetLengthSq() // Calculated at 0x5F562B, 0x5F5673
                ? PreProcess(1, routeL)                        // 0x5F5718
                : PreProcess(2, routeR);                       // 0x5F57B6
        };

        if (isRouteClearA) {
            if (isRouteClearB) {
                return PreProcessShortestRoute(); // 0x5F5711
            }
            return PreProcess(1, routeL); // 0x5F56F9
        }
        if (isRouteClearB) {
            return PreProcess(2, routeR); // 0x5F56C9
        }
        if (routeProcessedA.LastProcessedPoint == 1 && routeProcessedB.LastProcessedPoint > 1) {
            return PreProcess(2, routeR); // 0x5F56E0
        }
        if (routeProcessedA.LastProcessedPoint > 1 && routeProcessedB.LastProcessedPoint == 1) {
            return PreProcess(1, routeL); // 0x5F56F5
        }
        if (routeProcessedA.BlockedByEntity == routeProcessedB.BlockedByEntity) {
            return PreProcessShortestRoute(); // 0x5F5704
        }
        return routeProcessedB.BlockedByEntity->GetBoundRadius() > routeProcessedA.BlockedByEntity->GetBoundRadius()
            ? PreProcess(1, routeL)  // 0x5F574E
            : PreProcess(2, routeR); // 0x5F5747
    }
    case 1:  return PreProcess(1, routeL); // 0x5F575C - Use L route
    case 2:  return PreProcess(2, routeR); // 0x5F57AD - Use R route
    default: NOTSA_UNREACHABLE_CASE(forceDirection);
    }
}

// 0x5F1890
bool CPedGeometryAnalyser::ComputeRouteRoundSphere(const CPed& ped, const CColSphere& sphere, const CVector& start, const CVector& target, CVector& outNewTarget, CVector& outDetourTarget) {
    outNewTarget = target;

    // If the start point is inside the sphere, we first need to move the target to the edge of the sphere, so we can go around it
    if (sphere.IntersectPoint(start)) {
        if (CVector intersectA, intersectB; sphere.IntersectRay(start, (target - start).Normalized(), intersectA, intersectB)) {
            outNewTarget = intersectB;
        }
    }

    float distPedToStart;
    const auto moveDir = (outNewTarget - ped.GetPosition()).Normalized(&distPedToStart);

    // If we now don't intersect the sphere, we can go straight to the target without having to go around it
    // (Ignoring further intersections that may be on the same ray, but not on the line segment from ped to target)
    if (CVector intersectIn, intersectOut; !sphere.IntersectRay(outNewTarget, moveDir, intersectIn, intersectOut) || sq(distPedToStart) < (intersectIn - ped.GetPosition()).SquaredMagnitude()) {
        outDetourTarget = outNewTarget;
        return false;
    }

    // If the move diretion of the ped intersects the sphere,
    // a detour has to be calculated on the edge closest to the ped
    // such that the ped can go around the sphere without intersecting it
    if (CVector a, b; sphere.IntersectRay(ped.GetPosition(), moveDir, a, b)) {
        if (notsa::bugfixes::CPedGeometryAnalyser_ComputeRouteRoundSphere_IncorrectDetourPosition) {
            const auto halfway = (a + b) * 0.5f;
            const auto dir     = (halfway - CVector2D{ sphere.m_vecCenter }).Normalized();
            outDetourTarget    = CVector{ CVector2D{ sphere.m_vecCenter } + dir * sphere.m_fRadius, halfway.z };
        } else {
            const auto pt   = ped.GetPosition() + (sphere.m_vecCenter - ped.GetPosition()).ProjectOnToNormal(moveDir);
            const auto dir  = (pt - sphere.m_vecCenter).Normalized();
            outDetourTarget = sphere.m_vecCenter + dir * sphere.m_fRadius;
        }
        return true;
    }

    // NOTE/BUG (Pirulax):
    // This is kinda ambigous, because `start -> target` may be intersecting the sphere, but
    // since we've modified `outNewTarget` to be on the edge of the sphere,
    // we may not intersect it anymore, but we still need to go around it.
    // So, perhaps here we should do:
    // `outDetourTarget = outNewTarget`;
    return true;
}

// 0x5F5A30
bool CPedGeometryAnalyser::GetIsLineOfSightClear(const CPed& ped, const CVector& target, CEntity& entity, float& outIntersectionLength) {
    const auto moveDir = (target - ped.GetPosition()).Normalized();

    // Check if we intersect the bounding sphere of the entity at all
    {
        CColSphere sp;
        ComputeEntityBoundingSphere(ped, entity, sp);
        if (CVector a, b; !sp.IntersectRay(ped.GetPosition(), moveDir, a, b)) {
        return true;
        }
    }

    outIntersectionLength = 0.f;

    std::array<CVector, 4> bbPlaneNormals{};
    std::array<float, 4> bbPlaneDs{};
    ComputeEntityBoundingBoxPlanes(ped.GetPosition().z, entity, bbPlaneNormals, bbPlaneDs);

    // Calculate intersection points of the line with the planes of the entity's bounding box
    auto onPlaneStart  = ped.GetPosition(),
         onPlaneTarget = target;

    for (const auto& [plane, planeDot] : rngv::zip(bbPlaneNormals, bbPlaneDs)) {
        // Calculate the intersection point of a line with a plane
        const auto GetMoveLinePointOnPlane = [&](float dist) -> std::optional<CVector> {
            if (const auto distOnLine = plane.Dot(moveDir); distOnLine > 0.0001f) {
                // t = - (n . P0 + d) / (n . dir)
                // we want to find the point on the line that intersects the plane, so we can use the parametric equation of the line:
                // P(t) = P0 + dir * t
                const auto t = -dist / distOnLine;
                return onPlaneStart + moveDir * t;
            }
            return std::nullopt;
        };

        // Calculates `n . P0 + d` - the distance from the point to the plane along the plane's normal
        const auto GetPointDistanceToPlane = [&](CVector pos) {
            return plane.Dot(pos) + planeDot;
        };

        const auto startDistToPlane = GetPointDistanceToPlane(onPlaneStart),
                   targetDistToPlane   = GetPointDistanceToPlane(onPlaneTarget);

        // I'll be using this, instead of whatever the fuck they did, because that's just ugly
        const auto IsAbove = [](float sdist) {
            return sdist > ms_fPedNominalRadius;
        };
        const auto IsBelow = [](float sdist) {
            return sdist < -ms_fPedNominalRadius;
        };

        // Both are above or on the edge of the plane, so we can go straight to the target without having to go around
        if (IsAbove(startDistToPlane) && IsAbove(targetDistToPlane)) {
            return true;
        }
        
        // Check if we cross the plane, and if so, move the point that is below the plane to be on the plane
        if (IsBelow(startDistToPlane) && IsAbove(targetDistToPlane)) { // start is below
            if (const auto pt = GetMoveLinePointOnPlane(startDistToPlane)) {
                onPlaneTarget = *pt; // 0x5F5C97
            }
        } else if (IsAbove(startDistToPlane) && IsBelow(targetDistToPlane)) { // target is below
            if (const auto pt = GetMoveLinePointOnPlane(targetDistToPlane)) {
                onPlaneStart = *pt; // 0x5F5C39
            }
        }
    }
    outIntersectionLength = (onPlaneTarget - onPlaneStart).Magnitude();
    return false;
}

// 0x5F2F00
bool CPedGeometryAnalyser::GetIsLineOfSightClear(const CVector& start, const CVector& target, CEntity& entity) {
    auto* const cm = entity.GetColModel(); // BUGFIX: Check for null
    return cm && CCollision::TestLineOfSight(
        CColLine{ start, target },
        entity.GetMatrix(),
        *cm,
        false,
        false
    );
}

// 0x5F3590
CPed* CPedGeometryAnalyser::GetNearestPed(const CVector& point) {
    float minDistSq{ FLT_MAX };
    CPed* nearest{};
    for (auto& ped : GetPedPool()->GetAllValid()) {
        const auto distSq = (ped.GetPosition() - point).SquaredMagnitude();
        if (distSq < minDistSq) {
            minDistSq = distSq;
            nearest   = &ped;
        }
    }
    return nearest;
}

// 0x5F3970
bool CPedGeometryAnalyser::IsEntityBlockingTarget(CEntity& entity, const CVector& point, float radius) {
    std::array<CVector, 4> bbPlaneNormals{};
    std::array<float, 4> bbPlaneDs{};
    ComputeEntityBoundingBoxPlanes(entity.GetPosition().z, entity, bbPlaneNormals, bbPlaneDs);

    const auto dir = point - entity.GetPosition();
    if (std::abs(dir.z) > 3.f) {
        return false;
    }
    if (notsa::bugfixes::CPedGeometryAnalyser_IsEntityBlockingTarget_IncorrectRadiusCheck) {
        if (sq(entity.GetBoundRadius() + radius) < CVector2D{ dir }.SquaredMagnitude()) { 
            return false;
        }
    } else {
        if (sq(entity.GetBoundRadius()) + sq(radius) < CVector2D{ dir }.Magnitude()) {
            return false;
        }
    }

    for (size_t i = 0; i < bbPlaneNormals.size(); i++) {
        if (bbPlaneNormals[i].Dot(point) + bbPlaneDs[i] > 0.f) {
            return false; // Point lies in front of the plane, so it can't be in the bounding box
        }
    }

    return true;
}

// 0x5F1CB0
bool CPedGeometryAnalyser::IsInAir(const CPed& ped) {
    if (ped.bInVehicle) {
        return false;
    }
    if (ped.GetTaskManager().GetActiveTask()) {
        if (ped.GetIntelligence()->GetTaskSwim() || ped.GetIntelligence()->GetTaskJetPack()) {
            return false;
        }
        if (ped.GetTaskManager().GetSimplestActiveTaskAs<CTaskSimpleClimb>()) {
            return false;
        }
    }

    CColPoint  cp;
    CEntity*   e;
    const auto isOnGround = CWorld::ProcessVerticalLine(
        ped.GetPosition(),
        ped.GetPosition().z - 1.5f,
        cp,
        e,
        true,
        true,
        false,
        true,
        false,
        false,
        nullptr
    );

    if (!isOnGround && !ped.GetTaskManager().GetActiveTaskAs<CTaskComplexJump>()) {
        return !!CWorld::TestSphereAgainstWorld(
            ped.GetPosition(),
            0.15f,
            &ped,
            true,
            false,
            false,
            false,
            false,
            false
        );
    }

    return !isOnGround;
}

// 0x5F2F70
auto CPedGeometryAnalyser::IsWanderPathClear(const CVector& start, const CVector& target, float maxHeightChange, int32 maxSamples) -> WanderPathClearness {
    if (std::abs(start.z - target.z) > maxHeightChange) {
        return WanderPathClearness::BLOCKED_HEIGHT;
    }

    const auto [lowestZ, highestZ] = std::minmax(start.z, target.z);
    const auto start2D             = CVector2D{ start },
               target2D            = CVector2D{ target };

    if (!CWorld::GetIsLineOfSightClear(
            CVector{ start2D, lowestZ },
            CVector{ target2D, lowestZ },
            true,
            false,
            false,
            false,
            false,
            false,
            false
        )) {
        return WanderPathClearness::BLOCKED_LOS;
    }

    const auto [dir, dist] = (target - start).NormalizedAndMag();
    const auto numSamples  = std::min((int32)(dist), maxSamples);

    if (numSamples == 0) {
        return WanderPathClearness::CLEAR;
    }

    const auto CheckHasGround = [&](const CVector& origin, float endZ, float* outGroundZ = nullptr) {
        CEntity*   e;
        CColPoint  cp;
        const auto hasHit = CWorld::ProcessVerticalLine(
            origin,
            endZ,
            cp,
            e,
            true
        );
        if (outGroundZ) {
            *outGroundZ = hasHit ? cp.m_vecPoint.z : endZ;
        }
        return hasHit;
    };

    // Walk path in steps, check for water
    if (numSamples >= 2) {
        for (int32 i = 1; i <= numSamples; i++) {
            const auto curr = start + dir * (float)(i);
            if (float waterZ; CWaterLevel::GetWaterLevel(curr, waterZ, false)) {
                if (!CheckHasGround(CVector{ curr, waterZ }, highestZ)) {
                    return WanderPathClearness::BLOCKED_WATER;
                }
            }
        }
    }

    // Check if the start point is on the ground
    if (!CheckHasGround(start, start.z + 5.f)) {
        return WanderPathClearness::BLOCKED_SHARP_DROP;
    }

    // Walk the path, and check for sharp drops
    if (numSamples >= 2) {
        for (int32 i = 1; i <= numSamples; i++) {
            const auto curr = CVector{ start2D + CVector2D{ dir } * (float)(i), lowestZ + 0.5f };
            if (float groundZ; !CheckHasGround(curr, curr.z - 2.f, &groundZ) || std::abs(curr.z - groundZ) > 1.f) { // NOTE: Why not use `maxHeightChange` here for checking the diff?
                return WanderPathClearness::BLOCKED_SHARP_DROP;
            }
        }
    }

    return WanderPathClearness::CLEAR;
}

// 0x5F3880
bool CPedGeometryAnalyser::LiesInsideBoundingBox(const CPed& ped, const CVector& pos, CEntity& entity) {
    if (CVector::DistSqr(entity.GetPosition(), pos) >= sq(entity.GetBoundRadius())) {
        return false;
    }

    std::array<CVector, 4> bbPlaneNormals{};
    std::array<float, 4> bbPlaneDs{};
    ComputeEntityBoundingBoxPlanes(entity.GetPosition().z, entity, bbPlaneNormals, bbPlaneDs);

    for (size_t i = 0; i < bbPlaneNormals.size(); i++) {
        if (bbPlaneNormals[i].Dot(pos) + bbPlaneDs[i] > 0.f) {
            return false; // Point lies in front of the plane, so it can't be in the bounding box
        }
    }

    return true;
}

// 0x41B7C0
void* CPointRoute::operator new(uint32 size) {
    return GetPointRoutePool()->New();
}

// 0x41B7D0
void CPointRoute::operator delete(void* ptr, size_t sz) {
    GetPointRoutePool()->Delete(reinterpret_cast<CPointRoute*>(ptr));
}
