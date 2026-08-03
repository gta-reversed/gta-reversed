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

    /*!
     * @return Squared length of the route.
     */
    auto GetLengthSq() const noexcept {
        float distSq = 0.f;
        for (size_t i = 1; i < GetSize(); i++) {
            distSq += ((*this)[i - 1] - (*this)[i]).SquaredMagnitude();
        }
        return distSq;
    }
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

    /*!
     * @addr 0x5F6110
     * @brief Calculate `route` around the bounding box of `entity` from `ped` to `target`
     * @param ped Ped that is trying to reach the target
     * @param entity Entity to go around the bounding box of
     * @param target Target point to reach
     * @param route Calculated route around the bounding box of the entity
     * @param forceDirection Direction to use, `0` => no force, `1` => force left, `2` => force right
     * @return The route used, 0 => none, no viable route, 1 => left, 2 => right
     */
    static int32 ComputeRouteRoundEntityBoundingBox(const CPed& ped, CEntity& entity, const CVector& target, CPointRoute& outRoute, int32 forceDirection);

    /*!
    * @addr 0x5F3DD0
    * @brief Calculate `route` around the bounding box of `entity` from `ped` to `target`
    * @param ped Ped that is trying to reach the target
    * @param start Starting point of the ped (usually ped's position)
    * @param entity Entity to go around the bounding box of
    * @param target Target point to reach
    * @param route Calculated route around the bounding box of the entity
    * @param forceDirection Direction to use, `0` => no force, `1` => force left, `2` => force right
    * @return The route used, 0 => none, no viable route, 1 => left, 2 => right
    */
    static int32 ComputeRouteRoundEntityBoundingBox(const CPed& ped, const CVector& start, CEntity& entity, const CVector& target, CPointRoute& outRoute, int32 forceDirection);

    /*!
     * @addr 0x5F1890
     * @brief Calculate a route around a sphere from `start` to `target` for the given ped.
     * @brief Usually used by having static `start` and `target` positions, and as the ped moves `outDetourTarget` is updated
     * @param ped Ped that is trying to reach the target
     * @param sphere Sphere to go around
     * @param start Starting position of the route
     * @param target Target position of the route
     * @param outNewStart New starting position after adjusting for the sphere (In case `start` was inside the sphere)
     * @param outDetourTarget Detour target position to go around the sphere
     * @return True if a detour is needed, false otherwise
     */
    static bool ComputeRouteRoundSphere(const CPed& ped, const CColSphere& sphere, const CVector& start, const CVector& target, CVector& outNewStart, CVector& outDetourTarget);

    /*!
     * @addr 0x5F5A30
     * @brief Check if line of sight is blocked by `entity` between `ped` and `target`.
     * @note Does a pretty basic bounding box check, so it may return false positives, but never false negatives.
     * @param ped Ped from which the line of sight is checked
     * @param target Target position to check the line of sight to
     * @param entity Entity we want to check if it blocks the line of sight
     * @param outIntersectionLength Length of the intersection if blocked
     * @return True if the line of sight is clear, false otherwise
     */
    static bool GetIsLineOfSightClear(const CPed& ped, const CVector& target, CEntity& entity, float& outIntersectionLength);

    /*!
     * @addr 0x5F2F00
     * @brief Check if line of sight is blocked by `entity` between `start` and `target`.
     * @note Does a pretty accurate collision mesh check
     * @param start LoS start position
     * @param target LoS target position
     * @param entity Entity we want to check if it blocks the line of sight
     * @return True if the line of sight is clear, false otherwise
     */
    static bool GetIsLineOfSightClear(const CVector& start, const CVector& target, CEntity& entity);
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
