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
    /*!
     * @addr 0x41B7C0
     * @brief Allocate a route object from the point-route pool.
     * @param size Requested allocation size.
     * @return Allocated route object memory.
     */
    static void* operator new(uint32 size);

    /*!
     * @addr 0x41B7D0
     * @brief Return a route object to the point-route pool.
     * @param ptr Pointer to memory previously allocated by operator new.
     * @param sz Allocation size (unused by implementation).
     */
    static void operator delete(void* ptr, size_t sz);

    /*!
     * @brief Compute the squared polyline length of this route.
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
    /*! @brief Register hooks for all analyser methods. */
    static void InjectHooks();

    /*!
     * @addr 0x5F1B00
     * @brief Check whether a ped can jump over a blocking entity.
     * @param ped Ped attempting the jump.
     * @param entity Blocking entity.
     * @return Whenever the jump path is acceptable.
     */
    static bool CanPedJumpObstacle(const CPed& ped, const CEntity& entity);

    /*!
     * @addr 0x5F32D0
     * @brief Check whether a ped can jump after a collision contact.
     * @param ped Ped attempting the jump.
     * @param entity Blocking entity.
     * @param contactNormal Contact normal from collision.
     * @param contactPos Contact point from collision.
     * @return Whenever a jump route is feasible from the contact state.
     */
    static bool CanPedJumpObstacle(const CPed& ped, const CEntity& entity, const CVector& contactNormal, const CVector& contactPos);

    /*!
     * @addr 0x5F1C40
     * @brief Check whether a target ped can be aimed at.
     * @param ped Shooter ped.
     * @param targetPed Candidate target ped.
     * @param useDirectionTest If true, reject targets behind the shooter.
     * @return Whenever targetPed is a valid target point.
     */
    static bool CanPedTargetPed(CPed& ped, CPed& targetPed, bool useDirectionTest);

    /*!
     * @addr 0x5F1B70
     * @brief Check whether a world-space point can be targeted.
     * @param ped Shooter ped.
     * @param targetPt Candidate target point.
     * @param useDirectionTest If true, reject points behind the shooter.
     * @return Whenever the point is close enough and visible.
     */
    static bool CanPedTargetPoint(const CPed& ped, const CVector& targetPt, bool useDirectionTest);

    /*!
     * @addr 0x5F1E30
     * @brief Count line-of-sight crossings against buildings.
     * @param start Line start position.
     * @param target Line end position.
     * @return Number of line crossings recorded by ProcessLineOfSight.
     * @deprecated Use `CWorld::ProcessLineOfSight` directly instead
     */
    static int32 ComputeBuildingHitPoints(const CVector& start, const CVector& target);

    /*!
     * @addr 0x5F5D80
     * @brief Computes a clear target point for the given ped and target point.
     * @brief That is a point that is not blocked by any entities and is within a certain distance from the target point.
     * @param ped The ped for which to compute the clear target point.
     * @param target The target point to compute the clear target point for.
     * @param [out] outTargetClear The computed clear target point.
     */
    static void ComputeClearTarget(const CPed& ped, const CVector& target, CVector& outTargetClear);

    /*!
     * @addr 0x5F3B70
     * @brief Computes the closest point on the surface of an entity to a given ped.
     * @param ped Ped for which to compute the closest surface point.
     * @param entity Entity to compute the closest surface point on.
     * @param [out] outPoint Computed closest surface point.
     * @return Whenever the closest surface point is successfully computed.
     */
    static bool ComputeClosestSurfacePoint(const CPed& ped, CEntity& entity, CVector& outPoint);

    /*!
     * @addr 0x5F36F0
     * @brief Computes the closest point on the surface of an entity to a given point.
     * @param pos Position for which to compute the closest surface point.
     * @param entity Entity to compute the closest surface point on.
     * @param [out] outPoint Computed closest surface point.
     * @return Whenever the closest surface point is successfully computed.
     */
    static bool ComputeClosestSurfacePoint(const CVector& pos, CEntity& entity, CVector& outPoint);

    /*!
     * @addr 0x5F2C10
     * @brief Get the closest point to `pos` on the side of a polygon defined by `corners`.
     * @param pos The point to find the closest point to.
     * @param corners The corners of the polygon, in order
     * @param [out] outPoint The closest point on the polygon to `pos`.
     * @return Whenever the closest surface point is successfully computed.
     */
    static bool ComputeClosestSurfacePoint(const CVector& pos, const std::array<CVector, 4>& corners, CVector& outPoint);

    /*! 
     * @brief Compute the center point of an entity bounding box at a specified Z.
     * @param zPos Z coordinate to apply to the computed center.
     * @param entity Entity whose bounds are used.
     * @param [out] center Computed center point.
     */
    static void ComputeEntityBoundingBoxCentre(float zPos, CEntity& entity, CVector& center);

    /*!
     * @addr 0x5F1600
     * @brief Compute the center of a 4-corner bounding polygon at a specified Z.
     * @param zPos Z coordinate to apply to the computed center.
     * @param corners Bounding corners in winding order.
     * @param [out] center Computed center point.
     */
    static void ComputeEntityBoundingBoxCentreUncached(float zPos, const std::array<CVector, 4>& corners, CVector& center);

    /*!
     * @addr 0x5F3B40
     * @brief Compute an entity bounding-box center without using cached data.
     * @param zPos Z coordinate to apply to the computed center.
     * @param entity Entity whose bounds are used.
     * @param [out] center Computed center point.
     */
    static void ComputeEntityBoundingBoxCentreUncachedAll(float zPos, CEntity& entity, CVector& center);

    /*!
     * @addr 0x5F3650
     * @brief Compute entity bounding-box corners at the specified Z.
     * @param zPos Z coordinate applied to all corners.
     * @param entity Entity whose bounds are used.
     * @param [out] corners Computed bounding-box corners.
     */
    static void ComputeEntityBoundingBoxCorners(float zPos, CEntity& entity, std::array<CVector, 4>& corners);

    /*!
     * @addr 0x5F1FA0
     * @brief Compute entity bounding-box corners from current model data.
     * @param zPos Z coordinate applied to all corners.
     * @param entity Entity whose bounds are used.
     * @param [out] corners Computed bounding-box corners.
     * @return Whenever corners can be computed.
     */
    static bool ComputeEntityBoundingBoxCornersUncached(float zPos, CEntity& entity, std::array<CVector, 4>& corners);

    /*!
     * @addr 0x5F3660
     * @brief Compute outward planes of an entity bounding box.
     * @param zPos Z coordinate used for corner generation.
     * @param entity Entity whose bounds are used.
     * @param [out] outPlaneNormals Plane normals.
     * @param [out] outPlaneDs Plane dot constants.
     */
    static void ComputeEntityBoundingBoxPlanes(float zPos, CEntity& entity, std::array<CVector, 4>& outPlaneNormals, std::array<float, 4>& outPlaneDs);

    /*!
     * @addr 0x5F1670
     * @brief Compute outward planes for provided bounding corners.
     * @param zPos Unused in this implementation.
     * @param corners Bounding corners in winding order.
     * @param [out] outPlaneNormals Plane normals.
     * @param [out] outPlaneDs Plane dot constants.
     */
    static void ComputeEntityBoundingBoxPlanesUncached(float zPos, const std::array<CVector, 4>& corners, std::array<CVector, 4>& outPlaneNormals, std::array<float, 4>& outPlaneDs);

    /*!
     * @addr 0x5F2B80
     * @brief Compute bounding planes directly from entity model data.
     * @param zPos Z coordinate used for corner generation.
     * @param entity Entity whose bounds are used.
     * @param [out] outPlaneNormals Plane normals.
     * @param [out] outPlaneDs Plane dot constants.
     */
    static void ComputeEntityBoundingBoxPlanesUncachedAll(float zPos, CEntity& entity, std::array<CVector, 4>& outPlaneNormals, std::array<float, 4>& outPlaneDs);

    /*!
     * @addr 0x5F36A0
     * @brief Compute segment planes from entity bounds, using center-to-corner lines.
     * @param zPos Z coordinate used for corner generation.
     * @param entity Entity whose bounds are used.
     * @param [out] outNormals Segment-plane normals.
     * @param [out] outPlaneDs Plane dot constants.
     */
    static void ComputeEntityBoundingBoxSegmentPlanes(float zPos, CEntity& entity, std::array<CVector, 4>& outSegPlaneNormals, std::array<float, 4>& outPlaneDs);

    /*!
     * @addr 0x5F1750
     * @brief Compute segment planes from explicit corners and center.
     * @param corners Bounding corners in winding order.
     * @param center Bounding-box center.
     * @param [out] outNormals Segment-plane normals.
     * @param [out] outPlaneDs Plane dot constants.
     */
    static void ComputeEntityBoundingBoxSegmentPlanesUncached(const std::array<CVector, 4>& corners, CVector& center, std::array<CVector, 4>& outSegPlaneNormals, std::array<float, 4>& outPlaneDs);

    /*!
     * @addr 0x5F2BC0
     * @brief Compute segment planes directly from entity model data.
     * @param zPos Z coordinate used for corner generation.
     * @param entity Entity whose bounds are used.
     * @param [out] outNormals Segment-plane normals.
     * @param [out] outPlaneDs Plane dot constants.
     */
    static void ComputeEntityBoundingBoxSegmentPlanesUncachedAll(float zPos, CEntity& entity, std::array<CVector, 4>& outSegPlaneNormals, std::array<float, 4>& outPlaneDs);

    /*!
     * @addr 0x5F3C20
     * @brief Compute a 2D bounding sphere around an entity at ped Z.
     * @param ped Ped providing the Z plane.
     * @param entity Entity whose bounds are used.
     * @param [out] out Computed sphere.
     */
    static void ComputeEntityBoundingSphere(const CPed& ped, CEntity& entity, CColSphere& out);

    /*!
     * @addr 0x5F3730
     * @brief Compute an avoidance direction around an entity.
     * @param ped Ped that needs to avoid the entity.
     * @param entity Blocking entity.
     * @param [out] outDirToAvoidEntity Chosen avoidance direction.
     */
    static void ComputeMoveDirToAvoidEntity(const CPed& ped, CEntity& entity, CVector& outDirToAvoidEntity);

    /*!
     * @addr 0x5F3730
     * @brief Compute and return an avoidance direction around an entity.
     * @param ped Ped that needs to avoid the entity.
     * @param entity Blocking entity.
     * @return Chosen avoidance direction.
     */
    static CVector ComputeMoveDirToAvoidEntity(const CPed& ped, CEntity& entity);

    /*!
     * @notsa
     * @brief Get a unit-like direction vector for a logical entity side.
     * @param entity Entity that provides orientation axes.
     * @param dir Requested side direction.
     * @return Direction vector for dir.
     */
    static CVector ComputeEntityDir(const CEntity& entity, eDirection dir);

    /*!
     * @addr 0x5F1500
     * @brief Compute the four cardinal direction vectors for an entity.
     * @param entity Entity that provides orientation axes.
     * @param [out] outDirs Directions indexed by eDirection.
     */
    static void ComputeEntityDirs(const CEntity& entity, std::array<CVector, 4>& outDirs);

    /*!
     * @addr 0x5F3BC0
     * @brief Determine which side of an entity a ped is on.
     * @param ped Ped used as query point.
     * @param entity Entity whose bounds are tested.
     * @return Side classification in entity space.
     */
    static eDirection ComputeEntityHitSide(const CPed& ped, CEntity& entity);

    /*!
     * @addr 0x5F1450
     * @brief Determine hit side from precomputed segment planes.
     * @param point1 The final point after the hit, outside the bounding box of the entity.
     * @param segmentPlaneNormals The normals of the planes of the bounding box segments of the entity.
     * @param segmentPlaneDots The dot products of the planes of the bounding box segments of the entity.
     * @return The side where the hit came from to end up at point1 after passing the entity bounding box.
     */
    static eDirection ComputeEntityHitSide(const CVector& point1, const std::array<CVector, 4>& segmentPlaneNormals, const std::array<float, 4>& segmentPlaneDots);

    /*!
     * @addr 0x5F3AC0
     * @brief Determine which side of an entity a point lies beyond.
     * @param point Query point.
     * @param entity Entity whose bounds are tested.
     * @return Side classification in entity space.
     */
    static eDirection ComputeEntityHitSide(const CVector& point, CEntity& entity);

    /*!
     * @addr 0x5F3640
     * @brief Determine ped hit side from another physical's move speed.
     * @param ped Ped that was hit.
     * @param physical Physical object whose move speed is used as impact direction.
     * @return Side classification on ped.
     */
    static eDirection ComputePedHitSide(const CPed& ped, const CPhysical& physical);

    /*!
     * @addr 0x5F1E70
     * @brief Determine ped hit side from an explicit hit direction.
     * @param ped The ped that was hit.
     * @param hitDir The direction of the hit.
     * @return The side of the ped that was most likely hit by the given velocity vector, or `eDirection::FORWARD` as a fallback.
     */
    static eDirection ComputePedHitSide(const CPed& ped, const CVector& hitDir);

    /*!
     * @addr 0x5F13F0
     * @brief Determine shot side from a world-space position.
     * @param ped Ped that was shot.
     * @param pos World-space shot source position.
     * @return Side classification on ped.
     */
    static eDirection ComputePedShotSide(const CPed& ped, const CVector& pos);

    /*!
     * @addr 0x5F6110
     * @brief Calculate `route` around the bounding box of `entity` from `ped` to `target`
     * @param ped Ped that is trying to reach the target
     * @param entity Entity to go around the bounding box of
     * @param target Target point to reach
     * @param [out] outRoute Calculated route around the bounding box of the entity
     * @param forceDirection Direction to use, `0` => no force, `1` => force left, `2` => force right
     * @return The route used, 0 => none, no viable route, 1 => left, 2 => right
     */
    static int32 ComputeRouteRoundEntityBoundingBox(const CPed& ped, CEntity& entity, const CVector& target, CPointRoute& outRoute, int32 forceDirection);

    /*!
     * @addr 0x5F3DD0
     * @brief Calculate `route` around the bounding box of `entity` from `start` to `target`.
     * @param ped Ped context for dimensions and collision checks.
     * @param start Starting point of the route.
     * @param entity Entity to go around the bounding box of.
     * @param target Target point to reach.
     * @param [out] outRoute Calculated route around the bounding box of the entity.
     * @param forceDirection Direction to use, `0` => no force, `1` => force left, `2` => force right.
     * @return The route used, 0 => none, no viable route, 1 => left, 2 => right.
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
     * @param [out] outNewStart Updated target used for intersection checks (moved to sphere edge if needed)
     * @param [out] outDetourTarget Detour target position to go around the sphere
     * @return Whenever a detour is needed.
     */
    static bool ComputeRouteRoundSphere(const CPed& ped, const CColSphere& sphere, const CVector& start, const CVector& target, CVector& outNewStart, CVector& outDetourTarget);

    /*!
     * @addr 0x5F5A30
     * @brief Check if line of sight is blocked by `entity` between `ped` and `target`.
     * @note Does a pretty basic bounding box check, so it may return false positives, but never false negatives.
     * @param ped Ped from which the line of sight is checked
     * @param target Target position to check the line of sight to
     * @param entity Entity we want to check if it blocks the line of sight
     * @param [out] outIntersectionLength Length of the intersection if blocked
     * @return Whenever the line of sight is clear.
     */
    static bool GetIsLineOfSightClear(const CPed& ped, const CVector& target, CEntity& entity, float& outIntersectionLength);

    /*!
     * @addr 0x5F2F00
     * @brief Check if line of sight is blocked by `entity` between `start` and `target`.
     * @note Does a pretty accurate collision mesh check
     * @param start LoS start position
     * @param target LoS target position
     * @param entity Entity we want to check if it blocks the line of sight
     * @return Whenever the line of sight is clear.
     */
    static bool GetIsLineOfSightClear(const CVector& start, const CVector& target, CEntity& entity);

    /*!
     * @addr 0x5F3590
     * @brief Find the nearest ped in the ped pool to a point.
     * @param point Query position.
     * @return Pointer to the nearest ped to `point`, or nullptr if no ped is found
     */
    static CPed* GetNearestPed(const CVector& point);

    /*!
     * @addr 0x5F3970
     * @brief Check whether an entity blocks a target point within a radius.
     * @param entity Entity to test.
     * @param point Target point to test.
     * @param distance Extra radius margin around point.
     * @return Whenever the point is blocked by entity bounds.
     */
    static bool IsEntityBlockingTarget(CEntity& entity, const CVector& point, float distance);

    /*!
     * @addr 0x5F1CB0
     * @brief Determine whether a ped is in the air
     * @param ped Ped to test.
     * @return Whenver the ped is considered to be in the air
     */
    static bool IsInAir(const CPed& ped);

    /*!
     * @brief Clearness status of a wander path, used for @ref IsWanderPathClear
     */
    enum class WanderPathClearness : uint32 {
        BLOCKED_HEIGHT,
        BLOCKED_LOS,
        BLOCKED_WATER,
        BLOCKED_SHARP_DROP,
        CLEAR,
    };

    /*!
     * @addr 0x5F2F70
     * @brief Walk along the path and check for water and sharp drops
     * @param start Starting position of the path
     * @param target Target position of the path
     * @param maxHeightChange Maximum allowed height change between samples
     * @param maxSamples Maximum number of samples to check along the path (This right now translates to raw distance) (0, 1 - does very basic checks, 2 or more - does more thorough checks, like drops and water)
     * @return The clearness status of the path, see @ref WanderPathClearness for more details
     */
    static auto IsWanderPathClear(const CVector& start, const CVector& target, float maxHeightChange, int32 maxSamples) -> WanderPathClearness;

    /*!
     * @addr 0x5F3880
    * @brief Check if a position lies within an entity's 2D bounding box planes.
     * @param ped The ped to check with
     * @param pos Position to check if it's inside the entity's bounding box
     * @param entity Entity to check against
     * @return If `ped` is within `entity`'s bounding box at `pos`
     */
    static bool LiesInsideBoundingBox(const CPed& ped, const CVector& pos, CEntity& entity);

private:
    static inline auto& ms_fPedNominalRadius = StaticRef<float>(0x8D22B0);
};
