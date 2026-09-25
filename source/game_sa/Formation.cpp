#include "StdInc.h"
#include "Formation.h"

#include <reversiblebugfixes/Bugs.hpp>

// 0x699F50
void CFormation::ReturnTargetPedForPed(CPed* ped, CPed** outTargetPed) {
    for (int32 i = 0; i < (int32)m_Peds.m_count; i++) {
        if (m_Peds.m_peds[i] == ped && m_aFinalPedLinkToDestinations[i] >= 0) {
            *outTargetPed = m_DestinationPeds.m_peds[m_aFinalPedLinkToDestinations[i]];
            return;
        }
    }
}

// 0x699FA0
bool CFormation::ReturnDestinationForPed(CPed* ped, CVector* outDestination) {
    // BUG: The original iterates 24 times (the size of `CPointList`) instead of `m_Peds.m_count` (Compare with `ReturnTargetPedForPed`, which uses the correct bound).
    //      `m_aFinalPedLinkToDestinations` only has 8 entries (Verified from the Android ELF symbol sizes), so it reads past it, into `m_aPedLinkToDestinations`, then unrelated globals.
    //      In practice the OOB read is unreachable: the link array is only read when `m_Peds.m_peds[i] == ped`, but the list builders null all unused slots and a group has at most 8 peds, so no valid ped is ever at `i >= 8`.
    const size_t numToCheck = notsa::bugfixes::GenericOOB
        ? std::min<size_t>(m_Peds.m_count, m_aFinalPedLinkToDestinations.size())
        : 24;
    for (size_t i = 0; i < numToCheck; i++) {
        if (m_Peds.m_peds[i] != ped) {
            continue;
        }
        const auto destIdx = m_aFinalPedLinkToDestinations.data()[i]; // Not using `operator[]`, as it'd assert with the bugfix disabled
        if (destIdx >= 0) {
            *outDestination = m_Destinations.m_Points[destIdx];
            return true;
        }
    }
    return false;
}

// 0x699FF0
void CFormation::FindCoverPointsBehindBox(
    CPointList*    outPoints,
    CVector        target,
    CMatrix*       mat,
    const CVector& center, //!< Unused
    const CVector& bbMin,
    const CVector& bbMax,
    float          cutoffDist
) {
    const auto h = std::abs(target.z - mat->GetPosition().z);
    if (h >= 7.f) {
        return;
    }

    // 0x69A02E - Calculate the 2D position of the corners in world space
    const auto GetCorner2D = [&](float x, float y) {
        return mat->TransformPoint(CVector{ x, y, 0.f });
    };
    const CVector corners[4]{
        GetCorner2D(bbMin.x, bbMin.y), // bottom left
        GetCorner2D(bbMin.x, bbMax.y), // top left
        GetCorner2D(bbMax.x, bbMax.y), // top right
        GetCorner2D(bbMax.x, bbMin.y)  // bottom right
    };

    // 0x69A128 - Calculate side's lengths
    float sides[4]{}; // left, top, right, bottom
    for (auto&& [i, corner] : rngv::enumerate(corners)) {
        sides[i] = CVector2D::DistSqr(corners[i], corners[(i + 1) % 4]);
    }

    // 0x69A160
    auto shortestSideIdx = std::distance(sides, rng::min_element(sides));

    // 0x69A325
    for (auto&& [i, corner] : rngv::enumerate(corners)) {
        if (i == shortestSideIdx) {
            continue;
        }
        const auto otherCornerIdx = (i - 2) % 4; // The other corner of the side
        if (otherCornerIdx == shortestSideIdx) {
            continue;
        }
        if (CVector2D::DistSqr(target, corner) > sq(cutoffDist)) {
            continue;
        }
        const auto sideDir = (corners[otherCornerIdx] - corner).Normalized();
        const auto pt = corner + sideDir;
        outPoints->AddPoint(pt + (pt - target).Normalized() * 0.75f);
    }
}

// 0x69A620
void CFormation::GenerateGatherDestinations(CPedList& pedList, CPed* ped) {
    m_Destinations.m_Count = 0;
    rng::fill(m_Destinations.m_PointHasBeenClaimed, false);

    const auto count = (int32)pedList.m_count;
    float radius;
    switch (count) {
    case 1:  radius = 1.25f;  break;
    case 2:  radius = 1.5f;   break;
    case 3:  radius = 1.75f;  break;
    case 4:  radius = 2.125f; break;
    default: radius = 2.5f;   break;
    }

    const auto& pos = ped->GetPosition();
    for (int32 i = 0; i < count; i++) {
        const float angle = count < 2
            ? ped->m_fCurrentRotation + HALF_PI
            : PI / (float)count + (float)i / (float)count * TWO_PI - ped->m_fCurrentRotation;
        m_Destinations.AddPoint({
            std::sin(angle) * radius + pos.x,
            std::cos(angle) * radius + pos.y,
            pos.z,
        });
    }
}

// 0x69A770
void CFormation::GenerateGatherDestinations_AroundCar(CPedList& pedList, CVehicle* vehicle) {
    const auto& boundingBox = vehicle->GetModelInfo()->GetColModel()->GetBoundingBox();

    m_Destinations.m_Count = 0;
    rng::fill(m_Destinations.m_PointHasBeenClaimed, false);

    const auto AddDestinations = [
        &,
        forward = vehicle->GetForward().Normalized(),
        length  = boundingBox.GetLength(),
        center  = vehicle->GetPosition()
    ](uint32 numPeds, const CVector& sideOffset) {
        for (uint32 i = 0; i < numPeds; i++) {
            m_Destinations.AddPoint(center + sideOffset + forward * length * (0.5f - (float)i / (float)numPeds));
        }
    };

    // Offset to either side of the car
    const CVector sideOffset = vehicle->GetRight().Normalized() * (boundingBox.m_vecMax.x + 1.5f);

    // Put half of the peds on the left side...
    const auto leftCount = pedList.m_count / 2;
    AddDestinations(leftCount, -sideOffset);

    // ...and the rest on the right side
    AddDestinations(pedList.m_count - leftCount, sideOffset);
}

// 0x69B1B0
static int32 FindNearestUnclaimedDestination(const CVector& pt, float& totalCost) {
    int32 bestIdx    = -1;
    float bestDistSq = FLT_MAX;
    for (auto&& [i, destPt] : rngv::enumerate(CFormation::m_Destinations.GetPoints())) {
        if (CFormation::m_Destinations.m_PointHasBeenClaimed[i]) {
            continue;
        }
        if (const auto distSq = CVector::DistSqr(destPt, pt); distSq < bestDistSq) {
            bestIdx    = (int32)i;
            bestDistSq = distSq;
        }
    }
    totalCost += std::sqrt(bestDistSq);
    return bestIdx;
}

// 0x69B240
void CFormation::DistributeDestinations(CPedList& pedList) {
    m_Peds = pedList;
    if (m_Peds.m_count == 0) {
        return;
    }
    assert(m_Peds.m_count <= m_aPedLinkToDestinations.size());

    CPointList pedPoints;
    for (const auto ped : m_Peds.m_peds | rngv::take(m_Peds.m_count)) {
        pedPoints.AddPoint(ped->GetPosition());
    }

    const auto GetAverageDistanceTo = [](const CPointList& points, const CVector& centre) {
        float total = 0.0f;
        for (const auto& pt : points.GetPoints()) {
            total += CVector::Dist(pt, centre);
        }
        return total / (float)points.m_Count;
    };

    // Map the peds' positions onto the destinations, keeping their relative placement (scaled to match the spread of the destinations)
    const auto destCentre = m_Destinations.GetCentroid();
    const auto pedCentre  = pedPoints.GetCentroid();
    const auto scale      = std::max(GetAverageDistanceTo(m_Destinations, destCentre), 1.0f)
                          / std::max(GetAverageDistanceTo(pedPoints, pedCentre), 1.0f);
    for (auto& pt : pedPoints.GetPoints()) {
        pt = (pt - pedCentre) * scale + destCentre;
    }

    float bestCost = FLT_MAX;
    for (uint32 trial = 0; trial < m_Peds.m_count; trial++) {
        // PC only clears 7 entries here (Android clears all 8), but every entry that's used is overwritten below anyway
        rng::fill_n(m_aPedLinkToDestinations.begin(), TOTAL_PED_GROUP_FOLLOWERS, -1);
        rng::fill(m_Destinations.m_PointHasBeenClaimed, false);

        float cost = 0.0f;
        for (auto&& [i, pt] : rngv::enumerate(pedPoints.GetPoints())) {
            const auto destIdx = FindNearestUnclaimedDestination(pt, cost);
            m_aPedLinkToDestinations[i] = destIdx;
            m_Destinations.m_PointHasBeenClaimed[destIdx] = true;
        }
        if (cost < bestCost) {
            rng::copy_n(m_aPedLinkToDestinations.begin(), m_Peds.m_count, m_aFinalPedLinkToDestinations.begin());
            bestCost = cost;
        }
    }
}

// 0x69B5B0
void CFormation::DistributeDestinations_CoverPoints(const CPedList& pedList, CVector pos) {
    m_Peds = pedList;
    if (m_Peds.m_count == 0) {
        return;
    }
    assert(m_Peds.m_count <= m_aFinalPedLinkToDestinations.size());

    rng::fill(m_aFinalPedLinkToDestinations, -1);
    for (auto&& [destIdx, destPt] : rngv::enumerate(m_Destinations.GetPoints())) {
        int32 bestPedIdx = -1;
        float bestScore  = 0.4f;

        const auto destToTarget = CVector2D::Dist(destPt, pos);
        for (auto&& [pedIdx, ped] : rngv::enumerate(m_Peds.m_peds | rngv::take(m_Peds.m_count))) {
            if (m_aFinalPedLinkToDestinations[pedIdx] >= 0) {
                continue;
            }
            const auto pedToTarget = CVector2D::Dist(ped->GetPosition(), pos);
            if (destToTarget > pedToTarget + 1.0f) { // Destination is further away from the target than the ped
                continue;
            }
            // How much of a detour going to the destination is, compared to going straight to the target
            const auto score = 1.0f - (CVector2D::Dist(ped->GetPosition(), destPt) + destToTarget - pedToTarget) / pedToTarget;
            if (score > bestScore) {
                bestPedIdx = (int32)pedIdx;
                bestScore  = score;
            }
        }
        if (bestPedIdx >= 0) {
            m_aFinalPedLinkToDestinations[bestPedIdx] = (int32)destIdx;
        }
    }
}

// 0x69B700
void CFormation::DistributeDestinations_PedsToAttack(const CPedList& pedList) {
    m_Peds = pedList;
    if (m_Peds.m_count == 0) {
        return;
    }
    const auto numPeds    = m_Peds.m_count;
    const auto numTargets = m_DestinationPeds.m_count;

    std::array<int32, std::tuple_size_v<decltype(CPedList::m_peds)>> remainingForTarget;
    assert(numPeds <= m_aFinalPedLinkToDestinations.size());
    assert(numTargets <= remainingForTarget.size());

    rng::fill(m_aFinalPedLinkToDestinations, -1);

    // Each target can be attacked by at most this many peds
    const auto maxPerTarget = std::max(2, (int32)std::ceil((double)(int32)numPeds / (double)(int32)numTargets));
    rng::fill(remainingForTarget | rngv::take(numTargets), maxPerTarget);

    size_t bestTargetIdx = 0, bestPedIdx = 0; // Not reset per iteration, same as the original
    for (uint32 assigned = 0; assigned < numPeds; assigned++) {
        float bestDistSq = FLT_MAX;
        for (auto&& [pedIdx, ped] : rngv::enumerate(m_Peds.m_peds | rngv::take(numPeds))) {
            if (m_aFinalPedLinkToDestinations[pedIdx] >= 0) {
                continue;
            }
            for (auto&& [targetIdx, target] : rngv::enumerate(m_DestinationPeds.m_peds | rngv::take(numTargets))) {
                if (remainingForTarget[targetIdx] <= 0) {
                    continue;
                }
                if (const auto distSq = CVector2D::DistSqr(ped->GetPosition(), target->GetPosition()); distSq < bestDistSq) {
                    bestTargetIdx = (size_t)targetIdx;
                    bestPedIdx    = (size_t)pedIdx;
                    bestDistSq    = distSq;
                }
            }
        }
        m_aFinalPedLinkToDestinations[bestPedIdx] = (int32)bestTargetIdx;
        remainingForTarget[bestTargetIdx]--;
    }
}

// 0x69B860
void CFormation::FindCoverPoints(CVector pos, float radius) {
    m_Destinations.m_Count = 0;
    rng::fill(m_Destinations.m_PointHasBeenClaimed, false);

    for (auto& vehicle : GetVehiclePool()->GetAllValid()) {
        if (!vehicle.IsAutomobile() || vehicle.GetMoveSpeed().Magnitude() >= 0.005f) {
            continue;
        }
        const auto* colModel = vehicle.GetModelInfo()->GetColModel();
        const auto& bb       = colModel->m_boundBox;
        if (bb.m_vecMax.z >= 1.5f) { // Too tall to take cover behind
            continue;
        }
        CPointList points;
        FindCoverPointsBehindBox(
            &points,
            pos,
            vehicle.m_matrix,
            colModel->m_boundSphere.m_vecCenter,
            bb.m_vecMin,
            bb.m_vecMax,
            radius
        );
        for (const auto& pt : points.GetPoints()) {
            m_Destinations.AddPoint(pt);
        }
    }

    for (auto& object : GetObjectPool()->GetAllValid()) {
        if (object.m_matrix->GetUp().z <= 0.95f || !object.CanBeUsedToTakeCoverBehind()) {
            continue;
        }
        const auto& objPos = object.GetPosition();
        if (CVector2D::Dist(objPos, pos) < radius) {
            m_Destinations.AddPoint(objPos + (objPos - pos).Normalized());
        }
    }
}

void CFormation::InjectHooks() {
    RH_ScopedClass(CFormation);
    RH_ScopedCategoryGlobal();

    RH_ScopedGlobalInstall(ReturnTargetPedForPed, 0x699F50);
    RH_ScopedGlobalInstall(ReturnDestinationForPed, 0x699FA0);
    RH_ScopedGlobalInstall(FindCoverPointsBehindBox, 0x699FF0);
    RH_ScopedGlobalInstall(GenerateGatherDestinations, 0x69A620);
    RH_ScopedGlobalInstall(GenerateGatherDestinations_AroundCar, 0x69A770);
    RH_ScopedGlobalInstall(DistributeDestinations, 0x69B240);
    RH_ScopedGlobalInstall(DistributeDestinations_CoverPoints, 0x69B5B0);
    RH_ScopedGlobalInstall(DistributeDestinations_PedsToAttack, 0x69B700);
    RH_ScopedGlobalInstall(FindCoverPoints, 0x69B860);
}
