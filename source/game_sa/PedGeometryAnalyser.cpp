#include "StdInc.h"

#include "PedGeometryAnalyser.h"
#include "Interior/InteriorManager_c.h"

void CPedGeometryAnalyser::InjectHooks() {
    RH_ScopedClass(CPedGeometryAnalyser);
    RH_ScopedCategoryGlobal();

    RH_ScopedOverloadedInstall(CanPedJumpObstacle, "", 0x5F1B00, bool(*)(const CPed&,const CEntity&), { .Reversed = false });
    RH_ScopedOverloadedInstall(CanPedJumpObstacle, "contacted", 0x5F32D0, bool(*)(const CPed&,const CEntity&,const CVector&,const CVector&), { .Reversed = false });
    RH_ScopedInstall(CanPedTargetPed, 0x5F1C40);
    RH_ScopedInstall(CanPedTargetPoint, 0x5F1B70, { .Reversed = false });
    RH_ScopedInstall(ComputeBuildingHitPoints, 0x5F1E30, { .Reversed = false });
    RH_ScopedInstall(ComputeClearTarget, 0x5F5D80, { .Reversed = false });
    RH_ScopedOverloadedInstall(ComputeClosestSurfacePoint, "ped", 0x5F3B70, bool (*)(const CPed& ped, CEntity& entity, CVector& point));
    RH_ScopedOverloadedInstall(ComputeClosestSurfacePoint, "posn", 0x5F36F0, bool(*)(const CVector&,CEntity&,CVector&));
    RH_ScopedOverloadedInstall(ComputeClosestSurfacePoint, "rect", 0x5F2C10, bool(*)(const CVector&,const CVector*,CVector&));
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
    RH_ScopedInstall(ComputeMoveDirToAvoidEntity, 0x5F3730);
    RH_ScopedInstall(ComputeEntityDirs, 0x5F1500);
    RH_ScopedOverloadedInstall(ComputeEntityHitSide, "1", 0x5F3BC0, int32 (*)(const CPed& ped, CEntity& entity));
    RH_ScopedOverloadedInstall(ComputeEntityHitSide, "2", 0x5F1450, int32 (*)(const CVector& point1, const CVector* point2, const float* x));
    RH_ScopedOverloadedInstall(ComputeEntityHitSide, "3", 0x5F3AC0, int32 (*)(const CVector& point, CEntity& entity));
    RH_ScopedOverloadedInstall(ComputePedHitSide, "physical", 0x5F3640, int32(*)(const CPed&,const CPhysical&));
    RH_ScopedOverloadedInstall(ComputePedHitSide, "posn", 0x5F1E70, int32(*)(const CPed&,const CVector&));
    RH_ScopedInstall(ComputePedShotSide, 0x5F13F0);
    RH_ScopedOverloadedInstall(ComputeRouteRoundEntityBoundingBox, "1", 0x5F6110, int32(*)(const CPed&,CEntity&,const CVector&,CPointRoute&,int32), { .Reversed = false });
    RH_ScopedOverloadedInstall(ComputeRouteRoundEntityBoundingBox, "2", 0x5F3DD0, int32(*)(const CPed&,const CVector&,CEntity&,const CVector&,CPointRoute&,int32), { .Reversed = false });
    RH_ScopedInstall(ComputeRouteRoundSphere, 0x5F1890, { .Reversed = false });
    RH_ScopedOverloadedInstall(GetIsLineOfSightClear, "ped", 0x5F5A30, bool(*)(const CPed&,const CVector&,CEntity&,float&), { .Reversed = false });
    RH_ScopedOverloadedInstall(GetIsLineOfSightClear, "v3d", 0x5F2F00, bool(*)(const CVector&,const CVector&,CEntity&), { .Reversed = false });
    RH_ScopedInstall(GetNearestPed, 0x5F3590, { .Reversed = false });
    RH_ScopedInstall(IsEntityBlockingTarget, 0x5F3970);
    RH_ScopedInstall(IsInAir, 0x5F1CB0, { .Reversed = false });
    RH_ScopedInstall(IsWanderPathClear, 0x5F2F70, { .Reversed = false });
    RH_ScopedInstall(LiesInsideBoundingBox, 0x5F3880);
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

// 0x5F5D80
void CPedGeometryAnalyser::ComputeClearTarget(const CPed& ped, const CVector& a2, CVector& a3) {
    return plugin::Call<0x5F5D80, const CPed&, const CVector&, CVector&>(ped, a2, a3);
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
    CVector corners[4];
    ComputeEntityBoundingBoxCornersUncached(posn.z, entity, corners);
    return ComputeClosestSurfacePoint(posn, corners, point);
}

// Closest point of the edges of the rectangle (or if none is found, the closest corner) to `posn`
// 0x5F2C10
bool CPedGeometryAnalyser::ComputeClosestSurfacePoint(const CVector& posn, const CVector* corners, CVector& point) {
    auto minDistSq = FLT_MAX;
    auto found     = false;

    for (auto i = 0; i < 4; i++) {
        const auto& a = corners[i];
        const auto& b = corners[(i + 1) % 4];

        auto       dir = b - a;
        const auto len = std::sqrt(dir.z * dir.z + dir.y * dir.y + dir.x * dir.x);
        const auto invLen = 1.0f / len;
        dir.x *= invLen;
        dir.y *= invLen;
        dir.z *= invLen;

        // Projection of `posn` onto the edge
        const auto t = (posn.z - a.z) * dir.z + (posn.y - a.y) * dir.y + (posn.x - a.x) * dir.x;
        if (t < 0.0f || !(t <= len)) {
            continue;
        }
        const CVector proj{
            dir.x * t + a.x,
            dir.y * t + a.y,
            dir.z * t + a.z
        };

        const auto d      = posn - proj;
        const auto distSq = d.z * d.z + d.y * d.y + d.x * d.x;
        if (distSq < minDistSq) {
            point     = proj;
            minDistSq = distSq;
            found     = true;
        }
    }
    if (found) {
        return true;
    }

    for (auto i = 0; i < 4; i++) {
        const auto d      = corners[i] - posn;
        const auto distSq = d.z * d.z + d.y * d.y + d.x * d.x;
        if (distSq < minDistSq) {
            point     = corners[i];
            minDistSq = distSq;
            found     = true;
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

// Rounds the value like the x87 FPU does with 24 bit precision: To 24 bit mantissa, but with a larger exponent range than `float`.
// Used to get the same results for values (temporaries) that'd overflow a `float`, but are kept on the FPU stack in the original code.
static double RoundLikeX87(double value) {
    int32 exp;
    const auto mantissa = std::frexp(value, &exp);
    return std::ldexp((double)(float)(mantissa), exp);
}

// Corners of the entity's bounding box (expanded by the ped's radius) on the XY plane, at height `zPos`.
// The box is oriented along the (XY projection of the) entity's longest axis.
// 0x5F1FA0
void CPedGeometryAnalyser::ComputeEntityBoundingBoxCornersUncached(float zPos, CEntity& entity, CVector* corners) {
    const auto SetCornersZ = [&] {
        for (auto i = 0; i < 4; i++) {
            corners[i].z = zPos;
        }
    };

    // Furniture of interiors
    if (entity.GetIsTypeBuilding() && entity.m_bIsTempBuilding) {
        if (g_interiorMan.GetBoundingBox(reinterpret_cast<FurnitureEntity_c*>(&entity), corners)) {
            SetCornersZ();
            return;
        }
    }

    const auto& mat     = entity.GetMatrix();
    const auto* cm      = entity.GetModelInfo()->GetColModel();
    const auto* cd      = cm->m_pColData;
    const auto  isTall  = cm->m_boundBox.m_vecMax.z - cm->m_boundBox.m_vecMin.z > 6.0f;

    CVector bbMin, bbMax;
    if (entity.GetIsTypeObject() && cd && isTall && (cd->m_nNumBoxes || cd->m_nNumSpheres)) {
        // Tall objects: Bounding box of the collision volumes intersecting the [zPos - 1, zPos + 1] slice
        const auto zMin = zPos - 1.0f, zMax = zPos + 1.0f;
        bbMin = CVector{ FLT_MAX, FLT_MAX, FLT_MAX };
        bbMax = CVector{ -FLT_MAX, -FLT_MAX, -FLT_MAX };

        // `min`, `max` are in model space, but the slice is checked in world space
        const auto AddVolume = [&](const CVector& min, const CVector& max) {
            CVector worldMin, worldMax;
            worldMin.FromMultiply(mat, min);
            worldMax.FromMultiply(mat, max);
            if ((worldMin.z < zMin && worldMax.z < zMin) || (worldMin.z > zMax && worldMax.z > zMax)) {
                return;
            }
            bbMin.x = bbMin.x < min.x ? bbMin.x : min.x;
            bbMin.y = bbMin.y < min.y ? bbMin.y : min.y;
            bbMin.z = bbMin.z < min.z ? bbMin.z : min.z;
            bbMax.x = bbMax.x > max.x ? bbMax.x : max.x;
            bbMax.y = bbMax.y > max.y ? bbMax.y : max.y;
            bbMax.z = bbMax.z > max.z ? bbMax.z : max.z;
        };
        for (auto i = 0; i < (int16)(cd->m_nNumBoxes); i++) {
            const auto& box = cd->m_pBoxes[i];
            AddVolume(box.m_vecMin, box.m_vecMax);
        }
        for (auto i = 0; i < (int16)(cd->m_nNumSpheres); i++) {
            const auto& c = cd->m_pSpheres[i].m_vecCenter;
            const auto  r = cd->m_pSpheres[i].m_fRadius;
            AddVolume({ c.x - r, c.y - r, c.z - r }, { r + c.x, r + c.y, r + c.z });
        }
    } else {
        bbMin = cm->m_boundBox.m_vecMin;
        bbMax = cm->m_boundBox.m_vecMax;
    }

    // Expand by the ped's radius
    bbMin.x -= ms_fPedNominalRadius;
    bbMin.y -= ms_fPedNominalRadius;
    bbMin.z -= ms_fPedNominalRadius;
    bbMax.x = ms_fPedNominalRadius + bbMax.x;
    bbMax.y = ms_fPedNominalRadius + bbMax.y;
    bbMax.z = ms_fPedNominalRadius + bbMax.z;

    const auto& right = mat.GetRight(), &fwd = mat.GetForward(), &up = mat.GetUp();
    const auto rightLenSqXY = right.x * right.x + right.y * right.y;
    const auto fwdLenSqXY   = fwd.x * fwd.x + fwd.y * fwd.y;
    const auto upLenSqXY    = up.x * up.x + up.y * up.y;

    // NOTE: If no collision volumes were found the box is [FLT_MAX, -FLT_MAX], the original code (using the x87 FPU)
    //       calculates the size (`-2 * FLT_MAX`) without overflowing, so `double` is used here for the same result
    const auto Half = [](double v) { return (float)(v * 0.5); };
    const CVector halfSize{ Half((double)bbMax.x - bbMin.x), Half((double)bbMax.y - bbMin.y), Half((double)bbMax.z - bbMin.z) };
    const CVector centre{ Half((double)bbMax.x + bbMin.x), Half((double)bbMax.y + bbMin.y), Half((double)bbMax.z + bbMin.z) };

    CVector worldCentre;
    worldCentre.FromMultiply(mat, centre);

    // Length of the axes projected onto the XY plane (Not exactly, as it's multiplied by the squared length)
    // NOTE: These may overflow `float` (See above), so they're calculated like the x87 FPU does (24 bit mantissa, but larger exponent range)
    const auto Doubled = [](float halfLen, float lenSq) {
        return RoundLikeX87((double)halfLen * lenSq) * 2.0;
    };
    const auto rightLen = Doubled(halfSize.x, rightLenSqXY);        // Kept on the FPU stack in the original code
    const auto fwdLen   = (float)Doubled(halfSize.y, fwdLenSqXY); // Stored as `float`s in the original code (so they may overflow)
    const auto upLen    = (float)Doubled(halfSize.z, upLenSqXY);

    // `dir` is the longest axis (on the XY plane), the other 2 axes are projected onto it, and onto its perpendicular
    CVector dir;
    CVector maxPt, minPt; // Ends of the longest axis
    double  dirProj1; // Kept on the FPU stack in the original code (See `RoundLikeX87`)
    float   perpProj1, dirProj2, perpProj2;
    const auto ProcessAxes = [&](const CVector& axis, float halfLen, const CVector& other1, float other1HalfLen, const CVector& other2, float other2HalfLen) {
        dir = CVector{ axis.x, axis.y, 0.0f };

        const CVector halfAxis{ axis.x * halfLen, axis.y * halfLen, halfLen * 0.0f };
        maxPt = CVector{ halfAxis.x + worldCentre.x, worldCentre.y + halfAxis.y, worldCentre.z + halfAxis.z };
        minPt = CVector{ worldCentre.x - halfAxis.x, worldCentre.y - halfAxis.y, worldCentre.z - halfAxis.z };

        dir.Normalise();
        dirProj1  = RoundLikeX87((double)(dir.x * other1.x + dir.y * other1.y) * other1HalfLen);
        perpProj1 = (dir.x * other1.y - dir.y * other1.x) * other1HalfLen;
        dirProj2  = (dir.x * other2.x + dir.y * other2.y) * other2HalfLen;
        perpProj2 = (dir.x * other2.y - dir.y * other2.x) * other2HalfLen;
    };
    if (rightLen > fwdLen && rightLen > upLen) {
        ProcessAxes(right, halfSize.x, fwd, halfSize.y, up, halfSize.z);
    } else if (fwdLen > upLen) {
        // NOTE: The dot product is calculated in the reverse order here (which may matter due to rounding)
        ProcessAxes(fwd, halfSize.y, right, halfSize.x, up, halfSize.z);
        dirProj1 = RoundLikeX87((double)(dir.y * right.y + dir.x * right.x) * halfSize.x);
    } else {
        ProcessAxes(up, halfSize.z, right, halfSize.x, fwd, halfSize.y);
        dirProj1 = RoundLikeX87((double)(dir.y * right.y + dir.x * right.x) * halfSize.x);
    }

    const auto Abs = [](double x) { return x < 0.0 ? -x : x; };
    const auto dirExtent  = (float)RoundLikeX87(Abs(dirProj2) + Abs(dirProj1)); // Extent of the other axes along `dir`
    const auto perpExtent = (float)(Abs(perpProj2) + Abs(perpProj1));           // ...and along its perpendicular

    const CVector2D dirOffset{ dirExtent * dir.x, dirExtent * dir.y };
    const CVector2D perpOffset{ perpExtent * dir.y, -dir.x * perpExtent }; // Perpendicular is (dir.y, -dir.x)

    // NOTE: The `y` coordinates of the ends are kept on the x87 FPU stack in the original code, so they don't overflow (See above)
    const auto maxEndX = dirOffset.x + maxPt.x, minEndX = minPt.x - dirOffset.x;
    const auto maxEndY = RoundLikeX87((double)maxPt.y + dirOffset.y), minEndY = RoundLikeX87((double)minPt.y - dirOffset.y);
    corners[0] = CVector{ maxEndX - perpOffset.x, (float)RoundLikeX87(maxEndY - perpOffset.y), zPos };
    corners[1] = CVector{ minEndX - perpOffset.x, (float)RoundLikeX87(minEndY - perpOffset.y), zPos };
    corners[2] = CVector{ minEndX + perpOffset.x, (float)RoundLikeX87(minEndY + perpOffset.y), zPos };
    corners[3] = CVector{ maxEndX + perpOffset.x, (float)RoundLikeX87(maxEndY + perpOffset.y), zPos };
}

// 0x5F3660
void CPedGeometryAnalyser::ComputeEntityBoundingBoxPlanes(float zPos, CEntity& entity, CVector(*outPlanes)[4], float* outPlanesDot) {
    ComputeEntityBoundingBoxPlanesUncachedAll(zPos, entity, outPlanes, outPlanesDot);
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

// Planes going through the corners and the centre (the normals are perpendicular to the centre->corner direction)
// 0x5F1750
CVector* CPedGeometryAnalyser::ComputeEntityBoundingBoxSegmentPlanesUncached(const CVector* corners, CVector& center, CVector* normals, float* dots) {
    for (auto i = 0; i < 4; i++) {
        const auto& corner = corners[i];
        auto&       normal = normals[i];
        normal = CVector{ -(corner.y - center.y), corner.x - center.x, 0.0f };
        dots[i] = -(corner.z * normal.z + normal.y * corner.y + corner.x * normal.x);
    }
    return const_cast<CVector*>(corners); // NOTE: Returned by the original code, likely by accident
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
void CPedGeometryAnalyser::ComputeEntityBoundingSphere(const CPed& ped, CEntity& entity, CColSphere& sphere) {
    const auto zPos = ped.GetPosition().z;

    CVector corners[4];
    ComputeEntityBoundingBoxCornersUncached(zPos, entity, corners);

    CVector center;
    ComputeEntityBoundingBoxCentre(zPos, entity, center); // NOTE: Calculates the corners again

    auto maxDistSq = 0.0f;
    for (const auto& corner : corners) {
        const auto d      = corner - center;
        const auto distSq = d.z * d.z + d.y * d.y + d.x * d.x;
        if (distSq > maxDistSq) {
            maxDistSq = distSq;
        }
    }
    sphere.Set(std::sqrt(maxDistSq) * 1.1f, center, SURFACE_DEFAULT, 0, tColLighting{ 0xFF });
}

// Normal of the left or right side of the entity's bounding box, whichever the ped is on (or closer to)
// 0x5F3730
void CPedGeometryAnalyser::ComputeMoveDirToAvoidEntity(const CPed& ped, CEntity& entity, CVector& outDirToAvoidEntity) {
    const auto zPos = ped.GetPosition().z;

    CVector corners[4];
    ComputeEntityBoundingBoxCornersUncached(zPos, entity, corners);

    CVector planes[4];
    float   planeDots[4];
    ComputeEntityBoundingBoxPlanesUncached(zPos, corners, &planes, planeDots);

    const auto& pos  = ped.GetPosition();
    const auto  dist1 = planes[1].y * pos.y + planes[1].z * pos.z + planes[1].x * pos.x + planeDots[1];
    const auto  dist3 = planes[3].y * pos.y + planes[3].z * pos.z + planes[3].x * pos.x + planeDots[3];
    if (dist1 > 0.0f) {
        outDirToAvoidEntity = planes[1];
    } else if (dist3 > 0.0f) {
        outDirToAvoidEntity = planes[3];
    } else {
        outDirToAvoidEntity = dist1 > dist3 ? planes[1] : planes[3];
    }
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

// Directions of the entity in the order of `eDirection` (forward, left, backward, right)
// 0x5F1500
CVector* CPedGeometryAnalyser::ComputeEntityDirs(const CEntity& entity, CVector* dirs) {
    const auto  heading = entity.m_placement.m_fHeading;
    const auto* mat     = entity.m_matrix;

    const auto fwd   = mat ? mat->GetForward() : CVector{ -std::sin(heading), std::cos(heading), 0.0f };
    const auto right = mat ? mat->GetRight() : CVector{ std::cos(heading), std::sin(heading), 0.0f };

    dirs[0] = fwd;
    dirs[1] = right * -1.0f;
    dirs[2] = fwd * -1.0f;
    dirs[3] = right;
    return &dirs[3]; // NOTE: Returned by the original code, likely by accident
}

// 0x5F3BC0
int32 CPedGeometryAnalyser::ComputeEntityHitSide(const CPed& ped, CEntity& entity) {
    return ComputeEntityHitSide(ped.GetPosition(), entity);
}

// Side (segment) of the bounding box the point is in (Planes are from `ComputeEntityBoundingBoxSegmentPlanesUncached`)
// 0x5F1450
int32 CPedGeometryAnalyser::ComputeEntityHitSide(const CVector& point, const CVector* planes, const float* planeDots) {
    const auto DistToPlane = [&](int32 i) {
        return planes[i].y * point.y + planes[i].z * point.z + point.x * planes[i].x + planeDots[i];
    };
    for (auto i = 0; i < 4; i++) {
        if (DistToPlane((i + 3) % 4) >= 0.0f && DistToPlane(i) < 0.0f) {
            return i;
        }
    }
    return 0;
}

// 0x5F3AC0
int32 CPedGeometryAnalyser::ComputeEntityHitSide(const CVector& point, CEntity& entity) {
    CVector corners[4];
    ComputeEntityBoundingBoxCornersUncached(point.z, entity, corners);

    CVector center;
    ComputeEntityBoundingBoxCentreUncached(point.z, corners, center);

    CVector planes[4];
    float   planeDots[4];
    ComputeEntityBoundingBoxSegmentPlanesUncached(corners, center, planes, planeDots);

    return ComputeEntityHitSide(point, planes, planeDots);
}

// 0x5F3640
int32 CPedGeometryAnalyser::ComputePedHitSide(const CPed& ped, const CPhysical& physical) {
    return ComputePedHitSide(ped, physical.m_vecMoveSpeed);
}

// Side of the ped (`eDirection`) facing the direction opposite to `dir`
// 0x5F1E70
int32 CPedGeometryAnalyser::ComputePedHitSide(const CPed& ped, const CVector& dir) {
    auto hitDir = dir * -1.0f;
    hitDir.Normalise();

    CVector pedDirs[4];
    ComputeEntityDirs(ped, pedDirs);

    auto  maxDot = -1.0f;
    int32 side{};
    for (auto i = 0; i < 4; i++) {
        const auto dot = pedDirs[i].z * hitDir.z + pedDirs[i].y * hitDir.y + pedDirs[i].x * hitDir.x;
        if (dot >= maxDot) {
            maxDot = dot;
            side   = i;
        } else if (i == 0) {
            side = std::bit_cast<int32>(dot); // NOTE: Uninitialized in the original code, it's a leftover value of `dot`
        }
    }
    return side;
}

// Side of the ped (`eDirection`) the point is at
// 0x5F13F0
int32 CPedGeometryAnalyser::ComputePedShotSide(const CPed& ped, const CVector& posn) {
    const auto& pedPos = ped.GetPosition();

    // NOTE: The constants are more precise than `TWO_PI`, etc
    auto angle = std::atan2(-(posn.x - pedPos.x), posn.y - pedPos.y) - ped.m_fCurrentRotation + 0.78539819f; // + PI / 4
    if (angle < 0.0f) {
        angle += 6.2831855f; // 2 * PI
    }
    return (int32)(angle * 0.63661975f); // 4 / (2 * PI)
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
    const auto& entityPos = entity->GetPosition();
    const auto  dx = entityPos.x - point.x, dy = entityPos.y - point.y;
    if (std::abs(entityPos.z - point.z) > 3.0f) {
        return false;
    }

    // NOTE: Compares the distance with squared values
    const auto radius = entity->GetModelInfo()->GetColModel()->GetBoundRadius();
    if (radius * radius + distance * distance < std::sqrt(dy * dy + dx * dx)) {
        return false;
    }

    const auto halfDist = distance * 0.5f;
    const auto zPos     = entity->GetPosition().z;

    CVector corners[4];
    ComputeEntityBoundingBoxCornersUncached(zPos, *entity, corners);

    CVector planes[4];
    float   planeDots[4];
    ComputeEntityBoundingBoxPlanesUncached(zPos, corners, &planes, planeDots);

    for (auto i = 0; i < 4; i++) {
        if (planes[i].x * point.x + planes[i].z * point.z + point.y * planes[i].y + planeDots[i] + halfDist > 0.0f) {
            return false;
        }
    }
    return true;
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
    const auto d      = posn - entity.GetPosition();
    const auto radius = entity.GetModelInfo()->GetColModel()->GetBoundRadius();
    if (!(radius * radius > d.z * d.z + d.y * d.y + d.x * d.x)) {
        return false;
    }

    const auto zPos = ped.GetPosition().z;

    CVector corners[4];
    ComputeEntityBoundingBoxCornersUncached(zPos, entity, corners);

    CVector planes[4];
    float   planeDots[4];
    ComputeEntityBoundingBoxPlanesUncached(zPos, corners, &planes, planeDots);

    // NOTE: True if the point is behind any of the planes
    for (auto i = 0; i < 4; i++) {
        if (planes[i].x * posn.x + planes[i].z * posn.z + posn.y * planes[i].y + planeDots[i] < 0.0f) {
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
