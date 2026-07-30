#pragma once

#include <array>
#include <span>

#include "Enums/eDirection.h"
#include "Vector.h"
#include "Base.h"
#include "Route.hpp"

class CVector;
class CEntity;
class CPed;
class CColSphere;
class CPhysical;

class CPointRoute : public notsa::Route<CVector> {
public:
    static void* operator new(uint32 size);
    static void operator delete(void* ptr, size_t sz);
};
VALIDATE_SIZE(CPointRoute, 0x64);

class CPedGeometryAnalyser {
public:
    static void InjectHooks();
    
    static bool CanPedJumpObstacle(const CPed& ped, const CEntity& entity);
    static bool CanPedJumpObstacle(const CPed& ped, const CEntity& entity, const CVector& contactNormal, const CVector& contactPos);

    static bool CanPedTargetPed(CPed& ped, CPed& targetPed, bool useDirectionTest);
    static bool CanPedTargetPoint(const CPed& ped, const CVector& targetPt, bool useDirectionTest);

    static int32 ComputeBuildingHitPoints(const CVector& start, const CVector& target);

    /*!
     * @addr 0x5F5D80
     * @brief Computes a clear target point for the given ped and target point.
     * @brief That is a point that is not blocked by any entities and is within a certain distance from the target point.
     * @param ped The ped for which to compute the clear target point.
     * @param target The target point to compute the clear target point for.
     * @param outTargetClear The computed clear target point.
     */
    static void ComputeClearTarget(const CPed& ped, const CVector& target, CVector& outTarget);

    static bool ComputeClosestSurfacePoint(const CPed& ped, CEntity& entity, CVector& point);
    static bool ComputeClosestSurfacePoint(const CVector& posn, CEntity& entity, CVector& point);

    /*!
     * @addr 0x5F2C10
     * @brief Get the closest point to `posn` on the side of a polygon defined by `corners`.
     * @param posn The point to find the closest point to.
     * @param corners The corners of the polygon, in order
     * @param point The closest point on the polygon to `posn`.
     * @return True if there was a point within 
     */
    static bool ComputeClosestSurfacePoint(const CVector& posn, const std::array<CVector, 4>& corners, CVector& point);

    static void ComputeEntityBoundingBoxCentre(float zPos, CEntity& entity, CVector& center);
    static void ComputeEntityBoundingBoxCentreUncached(float zPos, const std::array<CVector, 4>& corners, CVector& center);
    static void ComputeEntityBoundingBoxCentreUncachedAll(float zPos, CEntity& entity, CVector& center);
    static void ComputeEntityBoundingBoxCorners(float zPos, CEntity& entity, std::array<CVector, 4>& corners);
    static bool ComputeEntityBoundingBoxCornersUncached(float zPos, CEntity& entity, std::array<CVector, 4>& corners);
    static void ComputeEntityBoundingBoxPlanes(float zPos, CEntity& entity, std::array<CVector, 4>& outPlanes, std::array<float, 4>& outPlanesDot);
    static void ComputeEntityBoundingBoxPlanesUncached(float zPos, const std::array<CVector, 4>& corners, std::array<CVector, 4>& outPlanes, std::array<float, 4>& outPlanesDot);
    static void ComputeEntityBoundingBoxPlanesUncachedAll(float zPos, CEntity& entity, std::array<CVector, 4>& outPlanes, std::array<float, 4>& outPlanesDot);
    static void ComputeEntityBoundingBoxSegmentPlanes(float zPos, CEntity& entity, std::array<CVector, 4>& outNormals, std::array<float, 4>& outPlanesDot);
    static void ComputeEntityBoundingBoxSegmentPlanesUncached(const std::array<CVector, 4>& corners, CVector& center, std::array<CVector, 4>& outNormals, std::array<float, 4>& outPlanesDot);
    static void ComputeEntityBoundingBoxSegmentPlanesUncachedAll(float zPos, CEntity& entity, std::array<CVector, 4>& outNormals, std::array<float, 4>& outPlanesDot);
    static void ComputeEntityBoundingSphere(const CPed& ped, CEntity& entity, CColSphere& out);

    static void ComputeMoveDirToAvoidEntity(const CPed& ped, CEntity& entity, CVector& outDirToAvoidEntity);
    static CVector ComputeMoveDirToAvoidEntity(const CPed& ped, CEntity& entity);

    static CVector ComputeEntityDir(const CEntity& entity, eDirection dir);
    static void ComputeEntityDirs(const CEntity& entity, std::array<CVector, 4>& outDirs);
    /*!
     * @addr 0x5F1450
     * @param point The final point after the hit, outside the bounging box of the entity
     * @param segmentPlaneNormals The normals of the planes of the bounding box segments of the entity
     * @param segmentPlaneDots The dot products of the planes of the bounding box segments of the entity
     * @return The side where the hit came from to end up at point after passing the bounding box of the entity (so, for example, if the point is in front, then the value returned will be `eDirection::LEFT`).
     */
    static eDirection ComputeEntityHitSide(const CPed& ped, CEntity& entity);
    static eDirection ComputeEntityHitSide(const CVector& point1, const std::array<CVector, 4>& segmentPlaneNormals, const std::array<float, 4>& segmentPlaneDots);
    static eDirection ComputeEntityHitSide(const CVector& point, CEntity& entity);
    static eDirection ComputePedHitSide(const CPed& ped, const CPhysical& physical);

    /*!
     * @param ped The ped that was hit.
     * @param hitDir The direction of the hit.
     * @return The side of the ped that was most likely hit by the given velocity vector, or `eDirection::FORWARD` as a fallback.
     */
    static eDirection ComputePedHitSide(const CPed& ped, const CVector& hitDir);

    static eDirection ComputePedShotSide(const CPed& ped, const CVector& posn);

    static int32 ComputeRouteRoundEntityBoundingBox(const CPed& ped, CEntity& entity, const CVector& target, CPointRoute& route, int32 forceDirection);
    static int32 ComputeRouteRoundEntityBoundingBox(const CPed& ped, const CVector& a2, CEntity& entity, const CVector& a4, CPointRoute& pointRoute, int32 a6);

    static bool ComputeRouteRoundSphere(const CPed& ped, const CColSphere& sphere, const CVector& a3, const CVector& a4, CVector& a5, CVector& a6);

    static bool GetIsLineOfSightClear(const CPed& ped, const CVector& a2, CEntity& entity, float& a4);
    static bool GetIsLineOfSightClear(const CVector& a1, const CVector& a2, CEntity& a3);
    static CPed* GetNearestPed(const CVector& point);

    static bool   IsEntityBlockingTarget(CEntity* entity, const CVector& point, float distance);
    static bool   IsInAir(const CPed& ped);

    enum class WanderPathClearness : uint32 {
        BLOCKED_HEIGHT,
        BLOCKED_LOS,
        BLOCKED_WATER,
        BLOCKED_SHARP_DROP,
        CLEAR,
    };
    static WanderPathClearness IsWanderPathClear(const CVector& from, const CVector& to, float maxHeightChange, int32 maxSamples);

    static bool LiesInsideBoundingBox(const CPed& ped, const CVector& posn, CEntity& entity);

private:
    static inline auto& ms_fPedNominalRadius = StaticRef<float>(0x8D22B0);
};
