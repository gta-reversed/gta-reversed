#include "StdInc.h"
#include "Formation.h"

// 0x699F50
void CFormation::ReturnTargetPedForPed(CPed* ped, CPed** pOutTargetPed) {
    for (int32 i = 0; i < (int32)m_Peds.m_count; i++) {
        if (m_Peds.m_peds[i] == ped && m_aPedLinkToDestinations[i] >= 0) {
            *pOutTargetPed = m_DestinationPeds.m_peds[m_aPedLinkToDestinations[i]];
            return;
        }
    }
}

// 0x699FA0
bool CFormation::ReturnDestinationForPed(CPed* ped, CVector* out) {
    for (int32 i = 0; i < 24; i++) {
        if (m_Peds.m_peds[i] == ped && m_aPedLinkToDestinations[i] >= 0) {
            *out = m_Destinations.m_Points[m_aPedLinkToDestinations[i]];
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
            ? ped->m_fCurrentRotation + 1.5707964f
            : 3.1415927f / (float)count + (float)i / (float)count * 6.2831855f - ped->m_fCurrentRotation;
        if (m_Destinations.m_Count < 24) {
            m_Destinations.m_Points[m_Destinations.m_Count++] = {
                std::sin(angle) * radius + pos.x,
                std::cos(angle) * radius + pos.y,
                pos.z,
            };
        }
    }
}

// 0x69A770
void CFormation::GenerateGatherDestinations_AroundCar(CPedList& pedList, CVehicle* veh) {
    const auto* mi = CModelInfo::GetModelInfo(veh->m_nModelIndex)->AsVehicleModelInfoPtr();
    const float sideOffset = mi->m_pVehicleStruct->m_avDummyPos[DUMMY_LIGHT_REAR_MAIN].x + 1.5f;
    const float length     = mi->m_pVehicleStruct->m_avDummyPos[DUMMY_LIGHT_REAR_MAIN].y
                           - mi->m_pVehicleStruct->m_avDummyPos[DUMMY_LIGHT_FRONT_MAIN].y; // Negative for regular cars

    CVector side = veh->m_matrix->m_right;
    side.Normalise();
    CVector fwd = veh->m_matrix->m_forward;
    fwd.Normalise();
    side *= sideOffset;

    m_Destinations.m_Count = 0;
    rng::fill(m_Destinations.m_PointHasBeenClaimed, false);

    const auto count       = (int32)pedList.m_count;
    const int32 backCount  = count / 2;
    const int32 frontCount = count - backCount;

    const auto& center = veh->GetPosition();
    for (int32 i = 0; i < backCount; i++) {
        CVector pt = center - side;
        if (backCount != 0) {
            pt += fwd * length * (0.5f - (float)i / (float)backCount);
        }
        if (m_Destinations.m_Count < 24) {
            m_Destinations.m_Points[m_Destinations.m_Count++] = pt;
        }
    }
    for (int32 i = 0; i < frontCount; i++) {
        CVector pt = center + side;
        if (frontCount != 0) {
            pt += fwd * length * (0.5f - (float)i / (float)frontCount);
        }
        if (m_Destinations.m_Count < 24) {
            m_Destinations.m_Points[m_Destinations.m_Count++] = pt;
        }
    }
}

// 0x69B240
void CFormation::DistributeDestinations(CPedList& pedList) {
    plugin::Call<0x69B240>(&pedList);
}

// 0x69B5B0
void CFormation::DistributeDestinations_CoverPoints(const CPedList& pedlist, CVector pos) {
    m_Peds = pedlist;
    if (m_Peds.m_count == 0) {
        return;
    }
    rng::fill(m_aPedLinkToDestinations, -1);
    for (int32 destIdx = 0; destIdx < (int32)m_Destinations.m_Count; destIdx++) {
        int32 bestPedIdx = -1;
        float bestScore  = 0.4f;
        const auto& pt   = m_Destinations.m_Points[destIdx];
        const float destToPos = DistanceBetweenPoints2D({ pt.x, pt.y }, { pos.x, pos.y });
        for (int32 pedIdx = 0; pedIdx < (int32)m_Peds.m_count; pedIdx++) {
            if (m_aPedLinkToDestinations[pedIdx] >= 0) {
                continue;
            }
            const auto& pp = m_Peds.m_peds[pedIdx]->GetPosition();
            const float pedToPos = DistanceBetweenPoints2D({ pp.x, pp.y }, { pos.x, pos.y });
            if (destToPos <= pedToPos + 1.0f) {
                const float score = 1.0f - ((DistanceBetweenPoints2D({ pp.x, pp.y }, { pt.x, pt.y }) + destToPos) - pedToPos) / pedToPos;
                if (bestScore < score) {
                    bestPedIdx = pedIdx;
                    bestScore  = score;
                }
            }
        }
        if (bestPedIdx >= 0) {
            m_aPedLinkToDestinations[bestPedIdx] = destIdx;
        }
    }
}

// 0x69B700
void CFormation::DistributeDestinations_PedsToAttack(const CPedList& pedList) {
    m_Peds = pedList;
    if (m_Peds.m_count == 0) {
        return;
    }
    rng::fill(m_aPedLinkToDestinations, -1);

    const auto count = (int32)pedList.m_count;
    std::array<int32, 30> remainingForTarget;
    const int32 maxPerTarget = std::max(2, (int32)std::lround((double)count / (double)(int32)m_DestinationPeds.m_count));
    for (int32 enemyIdx = 0; enemyIdx < (int32)m_DestinationPeds.m_count; enemyIdx++) {
        remainingForTarget[enemyIdx] = maxPerTarget;
    }

    for (int32 assigned = 0; assigned < count; assigned++) {
        int32 bestEnemyIdx = 0;
        int32 bestPedIdx   = 0;
        float bestDist     = 999999.9f;
        for (int32 pedIdx = 0; pedIdx < count; pedIdx++) {
            if (m_aPedLinkToDestinations[pedIdx] >= 0) {
                continue;
            }
            for (int32 enemyIdx = 0; enemyIdx < (int32)m_DestinationPeds.m_count; enemyIdx++) {
                if (remainingForTarget[enemyIdx] <= 0) {
                    continue;
                }
                const auto& ep = m_DestinationPeds.m_peds[enemyIdx]->GetPosition();
                const auto& pp = m_Peds.m_peds[pedIdx]->GetPosition();
                const float dist = DistanceBetweenPoints2D({ pp.x, pp.y }, { ep.x, ep.y });
                if (dist < bestDist) {
                    bestEnemyIdx = enemyIdx;
                    bestPedIdx   = pedIdx;
                    bestDist     = dist;
                }
            }
        }
        m_aPedLinkToDestinations[bestPedIdx] = bestEnemyIdx;
        remainingForTarget[bestEnemyIdx]--;
    }
}

// 0x69B860
void CFormation::FindCoverPoints(CVector pos, float radius) {
    plugin::Call<0x69B860, CVector, float>(pos, radius);
}

void CFormation::InjectHooks() {
    RH_ScopedClass(CFormation);
    RH_ScopedCategoryGlobal();

    RH_ScopedGlobalInstall(ReturnTargetPedForPed, 0x699F50);
    RH_ScopedGlobalInstall(ReturnDestinationForPed, 0x699FA0);
    RH_ScopedGlobalInstall(FindCoverPointsBehindBox, 0x699FF0);
    RH_ScopedGlobalInstall(GenerateGatherDestinations, 0x69A620);
    RH_ScopedGlobalInstall(GenerateGatherDestinations_AroundCar, 0x69A770);
    RH_ScopedGlobalInstall(DistributeDestinations, 0x69B240, { .reversed = false });
    RH_ScopedGlobalInstall(DistributeDestinations_CoverPoints, 0x69B5B0);
    RH_ScopedGlobalInstall(DistributeDestinations_PedsToAttack, 0x69B700);
    RH_ScopedGlobalInstall(FindCoverPoints, 0x69B860, { .reversed = false });
}
