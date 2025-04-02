#include "StdInc.h"
#include "Formation.h"

// 0x699F50
void CFormation::ReturnTargetPedForPed(CPed* ped, CPed** pOutTargetPed) {
    return plugin::Call<0x699F50, CPed*, CPed**>(ped, pOutTargetPed);
}

// 0x699FA0
bool CFormation::ReturnDestinationForPed(CPed* ped, CVector* out) {
    return plugin::CallAndReturn<bool, 0x699FA0>(ped, out);
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
    for (auto&& [i, corner] : notsa::enumerate(corners)) {
        sides[i] = CVector2D::DistSqr(corners[i], corners[(i + 1) % 4]);
    }

    // 0x69A160
    auto shortestSideIdx = std::distance(sides, rng::min_element(sides));

    // 0x69A325
    for (auto&& [i, corner] : notsa::enumerate(corners)) {
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
    plugin::Call<0x69A620, CPedList&, CPed*>(pedList, ped);
}

// 0x69A770
void CFormation::GenerateGatherDestinations_AroundCar(CPedList& pedList, CVehicle* veh) {
    plugin::Call<0x69A770, CPedList&, CVehicle*>(pedList, veh);
}

// 0x69B240
void CFormation::DistributeDestinations(CPedList& pedList) {
    plugin::Call<0x69B240>(&pedList);
}

// 0x69B5B0
void CFormation::DistributeDestinations_CoverPoints(const CPedList& pedlist, CVector pos) {
    return plugin::Call<0x69B5B0, const CPedList&, CVector>(pedlist, pos);
}

// 0x69B700
void CFormation::DistributeDestinations_PedsToAttack(const CPedList& pedList) {
    plugin::Call<0x69B700>(&pedList);
}

// 0x69B860
void CFormation::FindCoverPoints(CVector pos, float radius) {
    plugin::Call<0x69B860, CVector, float>(pos, radius);
}

void CFormation::InjectHooks() {
    RH_ScopedClass(CFormation);
    RH_ScopedCategoryGlobal();

    RH_ScopedGlobalInstall(ReturnTargetPedForPed, 0x699F50, { .reversed = false });
    RH_ScopedGlobalInstall(ReturnDestinationForPed, 0x699FA0, { .reversed = false });
    RH_ScopedGlobalInstall(FindCoverPointsBehindBox, 0x699FF0);
    RH_ScopedGlobalInstall(GenerateGatherDestinations, 0x69A620, { .reversed = false });
    RH_ScopedGlobalInstall(GenerateGatherDestinations_AroundCar, 0x69A770, { .reversed = false });
    RH_ScopedGlobalInstall(DistributeDestinations, 0x69B240, { .reversed = false });
    RH_ScopedGlobalInstall(DistributeDestinations_CoverPoints, 0x69B5B0, { .reversed = false });
    RH_ScopedGlobalInstall(DistributeDestinations_PedsToAttack, 0x69B700, { .reversed = false });
    RH_ScopedGlobalInstall(FindCoverPoints, 0x69B860, { .reversed = false });
}
