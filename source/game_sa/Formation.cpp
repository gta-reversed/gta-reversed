#include "StdInc.h"
#include "Formation.h"

// 0x699F50
void CFormation::ReturnTargetPedForPed(CPed* ped, CPed** pOutTargetPed) {
    // NOTSA: m_aFinalPedLinkToDestinations aliases m_Peds.m_peds (0xC1A4DC); using the latter avoids UB for large lists
    for (uint32 i = 0; i < m_Peds.m_count; ++i) {
        if (ped != m_Peds.m_peds[i] || m_aPedLinkToDestinations[i] < 0) {
            continue;
        }
        *pOutTargetPed = m_DestinationPeds.m_peds[m_aPedLinkToDestinations[i]];
        return;
    }
}

// 0x699FA0
bool CFormation::ReturnDestinationForPed(CPed* ped, CVector* out) {
    // NOTE: The original loops 24 times, scanning past the 8 link entries into the following globals. Replicated faithfully.
    for (int32 i = 0; i < 24; ++i) {
        if (ped != reinterpret_cast<CPed*>(m_aFinalPedLinkToDestinations[i]) || m_aPedLinkToDestinations[i] < 0) {
            continue;
        }
        *out = m_Destinations.m_Points[m_aPedLinkToDestinations[i]];
        return true;
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

    const auto heading = ped->m_fCurrentRotation;
    float radius;
    switch (pedList.m_count) {
    case 1:  radius = 1.25f;  break;
    case 2:  radius = 1.5f;   break;
    case 3:  radius = 1.75f;  break;
    case 4:  radius = 2.125f; break;
    default: radius = 2.5f;   break;
    }

    const auto count = (int32)pedList.m_count;
    for (int32 i = 0; i < count; ++i) {
        float angle;
        if (count < 2) {
            angle = heading + HALF_PI;
        } else {
            angle = PI / (float)count + (float)i / (float)count * TWO_PI - heading;
        }
        const auto& posn = ped->GetPosition();
        m_Destinations.AddPoint({ posn.x + std::sin(angle) * radius, posn.y + std::cos(angle) * radius, posn.z });
    }
}
// 0x69A770
void CFormation::GenerateGatherDestinations_AroundCar(CPedList& pedList, CVehicle* veh) {
    auto* const colModel = CModelInfo::GetModelInfo(veh->m_nModelIndex)->GetColModel();
    const float sideExtend = colModel->GetBoundingBox().m_vecMax.x + 1.5f;
    const float length     = colModel->GetBoundingBox().m_vecMax.y - colModel->GetBoundingBox().m_vecMin.y;

    CVector sideDir{ veh->GetMatrix().GetRight() };
    sideDir.Normalise();
    CVector fwdDir{ veh->GetMatrix().GetForward() };
    fwdDir.Normalise();

    m_Destinations.m_Count = 0;
    rng::fill(m_Destinations.m_PointHasBeenClaimed, false);

    const auto AddSideDestinations = [&](int32 numOnSide, float sideSign) {
        if (numOnSide <= 3) {
            return;
        }
        const CVector sideOffset = sideDir * (sideExtend * sideSign);
        int32 unrolledIdx = 0;
        for (int32 i = 3; i < numOnSide; unrolledIdx += 4, i += 4) {
            for (int32 k = 0; k < 4; ++k) {
                const auto& posn = veh->GetPosition();
                CVector dest{ posn.x + sideOffset.x, posn.y + sideOffset.y, posn.z + sideOffset.z };
                if (numOnSide != 0) {
                    const float t = 0.5f - (float)(unrolledIdx + k) / (float)numOnSide;
                    dest += fwdDir * (length * t);
                }
                m_Destinations.AddPoint(dest);
            }
        }
        for (; unrolledIdx < numOnSide; ++unrolledIdx) {
            const auto& posn = veh->GetPosition();
            CVector dest{ posn.x + sideOffset.x, posn.y + sideOffset.y, posn.z + sideOffset.z };
            if (numOnSide != 0) {
                const float t = 0.5f - (float)unrolledIdx / (float)numOnSide;
                dest += fwdDir * (length * t);
            }
            m_Destinations.AddPoint(dest);
        }
    };

    const int32 count    = (int32)pedList.m_count;
    const int32 numLeft  = count / 2;
    const int32 numRight = count - numLeft;
    AddSideDestinations(numLeft, -1.f);
    AddSideDestinations(numRight, 1.f);
}

// 0x69B1B0 - NOTSA: original name unknown; nearest-unclaimed-destination helper for DistributeDestinations
static int32 FindNearestUnclaimedDestination(CVector pos, float* totalDist) {
    int32 nearestIdx = -1;
    float nearestDist = 10000000.0f;
    for (uint32 i = 0; i < CFormation::m_Destinations.m_Count; ++i) {
        if (CFormation::m_Destinations.m_PointHasBeenClaimed[i]) {
            continue;
        }
        const float dist = (CFormation::m_Destinations.m_Points[i] - pos).Magnitude();
        if (dist < nearestDist) {
            nearestIdx = i;
            nearestDist = dist;
        }
    }
    *totalDist += nearestDist;
    return nearestIdx;
}

// 0x69B240
void CFormation::DistributeDestinations(CPedList& pedList) {
    // NOTSA: The original keeps 7 scratch link slots at 0xC1A2E0 (gap before m_Destinations) and the claimed
    // flags in m_Destinations.m_PointHasBeenClaimed (0xC1A43C); only the flags are observable state, so only they use the global.
    // Sized for the largest possible CPedList; the original overflows its 7-slot scratch for large lists.
    std::array<int32, 30> tempLinks{};

    m_Peds = pedList;
    if (m_Peds.m_count == 0) {
        return;
    }

    // Centroid of the gather destinations, scaled by 1 / count
    CVector destCentroid{};
    for (const auto& dest : std::span{ m_Destinations.m_Points.data(), m_Destinations.m_Count }) {
        destCentroid += dest;
    }
    destCentroid *= 1.f / (float)m_Destinations.m_Count;

    // Snapshot ped positions and compute their centroid
    CVector pedPositions[24]{};
    int32   numPeds = 0;
    CVector pedCentroid{};
    for (uint32 i = 0; i < m_Peds.m_count; ++i) {
        const auto& posn = m_Peds.m_peds[i]->GetPosition();
        if (numPeds < 24) {
            pedPositions[numPeds] = posn;
            pedCentroid += posn;
            numPeds = numPeds + 1;
        } else {
            pedCentroid += posn;
        }
    }
    pedCentroid *= 1.f / (float)m_Peds.m_count;

    // Mean spread of each cloud around its centroid
    float destSpread = 0.f;
    for (uint32 i = 0; i < m_Destinations.m_Count; ++i) {
        destSpread += (m_Destinations.m_Points[i] - destCentroid).Magnitude();
    }
    destSpread /= (float)m_Destinations.m_Count;
    float pedSpread = 0.f;
    for (int32 i = 0; i < numPeds; ++i) {
        pedSpread += (pedPositions[i] - pedCentroid).Magnitude();
    }
    pedSpread /= (float)m_Peds.m_count;
    if (destSpread < 1.f) {
        destSpread = 1.f;
    }
    if (pedSpread < 1.f) {
        pedSpread = 1.f;
    }

    // Rescale the ped cloud onto the destination cloud
    const float scale = destSpread / pedSpread;
    for (int32 i = 0; i < numPeds; ++i) {
        pedPositions[i] = destCentroid + (pedPositions[i] - pedCentroid) * scale;
    }

    // Try every ped as the starting seed, keep the assignment with the lowest total distance
    float bestScore = 999999.9f;
    for (uint32 iter = 0; iter < m_Peds.m_count; ++iter) {
        rng::fill(tempLinks, -1);
        rng::fill(m_Destinations.m_PointHasBeenClaimed, false);
        float totalDist = 0.f;
        for (int32 i = 0; i < numPeds; ++i) {
            const int32 destIdx = FindNearestUnclaimedDestination(pedPositions[i], &totalDist);
            tempLinks[i] = destIdx;
            m_Destinations.m_PointHasBeenClaimed[destIdx] = true;
        }
        if (totalDist < bestScore) {
            for (uint32 i = 0; i < m_Peds.m_count; ++i) {
                m_aPedLinkToDestinations[i] = tempLinks[i];
            }
            bestScore = totalDist;
        }
    }
}
// 0x69B5B0
void CFormation::DistributeDestinations_CoverPoints(const CPedList& pedlist, CVector pos) {
    m_Peds = pedlist;
    if (m_Peds.m_count == 0) {
        return;
    }
    rng::fill(m_aPedLinkToDestinations, -1);

    for (uint32 destIdx = 0; destIdx < m_Destinations.m_Count; ++destIdx) {
        const CVector& dest = m_Destinations.m_Points[destIdx];
        const float destDist = CVector2D::Dist(dest, pos);
        int32 bestPed = -1;
        float bestScore = 0.4f;
        for (uint32 pedIdx = 0; pedIdx < m_Peds.m_count; ++pedIdx) {
            if (m_aPedLinkToDestinations[pedIdx] >= 0) {
                continue;
            }
            const auto& pedPosn = m_Peds.m_peds[pedIdx]->GetPosition(); // NOTSA: Original reads the same ped pointer via m_aFinalPedLinkToDestinations (aliases m_Peds)
            const float pedDist = CVector2D::Dist(pedPosn, pos);
            if (destDist > 1.f + pedDist) { // Destination further out than the ped: ped would have to run past the threat
                continue;
            }
            const float score = 1.f - ((dest - pedPosn).Magnitude2D() + destDist - pedDist) / pedDist;
            if (score > bestScore) {
                bestScore = score;
                bestPed = (int32)pedIdx;
            }
        }
        if (bestPed >= 0) {
            m_aPedLinkToDestinations[bestPed] = (int32)destIdx;
        }
    }
}

// 0x69B700
void CFormation::DistributeDestinations_PedsToAttack(const CPedList& pedList) {
    m_Peds = pedList;
    if (m_Peds.m_count == 0) {
        return;
    }
    // Slots per target = floor(attackers / targets), i.e. plain integer division for positive counts.
    // (Original: FILD/FIDIV + floor + double-to-int helpers; exact for these magnitudes. NOTSA: guards the
    // div-by-zero the original resolves to INT_MIN via x87 floor(+inf) -> ftol)
    int32 slotsPerTarget = m_DestinationPeds.m_count ? (int32)(m_Peds.m_count / m_DestinationPeds.m_count) : INT32_MIN;
    if (slotsPerTarget < 2) {
        slotsPerTarget = 2;
    }
    int32 slotsLeft[30]{}; // NOTSA: Sized for CPedList (30); only the first m_DestinationPeds.m_count entries are used
    rng::fill(std::span{ slotsLeft, m_DestinationPeds.m_count }, slotsPerTarget);

    // NOTSA: bestAttacker/bestTarget live on the stack across iterations in the original (stale = previous
    // iteration's pair when nothing is found); hoisted here to match without reading uninitialized memory
    float bestDist = 999999.9f;
    int32 bestAttacker = 0;
    int32 bestTarget = 0;
    for (uint32 assigned = 0; assigned < pedList.m_count; ++assigned) {
        bestDist = 999999.9f;
        for (uint32 attackerIdx = 0; attackerIdx < pedList.m_count; ++attackerIdx) {
            if (m_aPedLinkToDestinations[attackerIdx] >= 0) {
                continue; // Already has a target
            }
            for (int32 targetIdx = 0; targetIdx < (int32)m_DestinationPeds.m_count; ++targetIdx) {
                if (slotsLeft[targetIdx] <= 0) {
                    continue;
                }
                const auto& targetPosn   = m_DestinationPeds.m_peds[targetIdx]->GetPosition();
                const auto& attackerPosn = pedList.m_peds[attackerIdx]->GetPosition();
                const float dist = CVector2D::Dist(targetPosn, attackerPosn);
                if (dist < bestDist) {
                    bestDist     = dist;
                    bestAttacker = (int32)attackerIdx;
                    bestTarget   = targetIdx;
                }
            }
        }
        m_aPedLinkToDestinations[bestAttacker] = bestTarget;
        slotsLeft[bestTarget] -= 1;
    }
}

// 0x69B860
void CFormation::FindCoverPoints(CVector pos, float radius) {
    m_Destinations.m_Count = 0;
    rng::fill(m_Destinations.m_PointHasBeenClaimed, false);

    // Parked automobiles with a low speed (original reads the speed off CPhysical, not CVehicle/CPlaceable)
    auto* const vehiclePool = GetVehiclePool();
    for (auto i = 0u; i < vehiclePool->GetSize(); ++i) {
        if (vehiclePool->IsFreeSlotAtIndex(i)) {
            continue;
        }
        auto* const veh = vehiclePool->GetAt(i);
        if (!veh || !veh->IsAutomobile()) {
            continue;
        }
        if (veh->GetMoveSpeed().Magnitude() >= 0.005f) {
            continue;
        }
        const float coverHeight = CModelInfo::GetModelInfo(veh->m_nModelIndex)->GetColModel()->GetBoundingBox().GetHeight();
        if (coverHeight >= 1.5f) {
            continue;
        }
        CPointList behindBox{};
        FindCoverPointsBehindBox(
            &behindBox,
            pos,
            &veh->GetMatrix(),
            veh->GetColModel()->GetBoundCenter(),
            veh->GetColModel()->GetBoundingBox().m_vecMin,
            veh->GetColModel()->GetBoundingBox().m_vecMax,
            radius
        );
        for (uint32 j = 0; j < behindBox.m_Count; ++j) {
            m_Destinations.AddPoint(behindBox.m_Points[j]);
        }
    }

    // Upright objects tall enough to hide behind, close enough to the threat
    auto* const objectPool = GetObjectPool();
    for (auto i = 0u; i < objectPool->GetSize(); ++i) {
        if (objectPool->IsFreeSlotAtIndex(i)) {
            continue;
        }
        auto* const obj = objectPool->GetAt(i);
        if (!obj) {
            continue;
        }
        if (obj->GetColModel()->GetBoundingBox().GetHeight() <= 0.95f) {
            continue;
        }
        if (!obj->CanBeUsedToTakeCoverBehind()) {
            continue;
        }
        const auto& objPosn = obj->GetPosition();
        const CVector2D toObj{ objPosn.x - pos.x, objPosn.y - pos.y };
        if (toObj.Magnitude() >= radius) {
            continue;
        }
        CVector away{ toObj.x, toObj.y, objPosn.z - pos.z };
        away.Normalise();
        m_Destinations.AddPoint(objPosn + away);
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
    RH_ScopedGlobalInstall(FindNearestUnclaimedDestination, 0x69B1B0);
    RH_ScopedGlobalInstall(DistributeDestinations, 0x69B240);
    RH_ScopedGlobalInstall(DistributeDestinations_CoverPoints, 0x69B5B0);
    RH_ScopedGlobalInstall(DistributeDestinations_PedsToAttack, 0x69B700);
    RH_ScopedGlobalInstall(FindCoverPoints, 0x69B860);
}
