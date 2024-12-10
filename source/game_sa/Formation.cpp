#include "StdInc.h"

void CFormation::InjectHooks() {
    RH_ScopedClass(CFormation);
    RH_ScopedCategory("Ped Groups");

    // RH_ScopedInstall(DistributeDestinations, 0x0);
    RH_ScopedInstall(DistributeDestinations_CoverPoints, 0x69B240, {.reversed=false});
    RH_ScopedInstall(DistributeDestinations_PedsToAttack, 0x69B700, {.reversed=false});
    RH_ScopedInstall(FindCoverPoints, 0x69B860, {.reversed=false});
    RH_ScopedInstall(FindCoverPointsBehindBox, 0x699FF0, {.reversed=false});
    RH_ScopedInstall(FindNearestAvailableDestination, 0x69B1B0, {.reversed=false});
    RH_ScopedInstall(GenerateGatherDestinations, 0x69A620, {.reversed=false});
    RH_ScopedInstall(GenerateGatherDestinations_AroundCar, 0x69A770, {.reversed=false});
    RH_ScopedInstall(ReturnDestinationForPed, 0x699FA0, {.reversed=false});
    RH_ScopedInstall(ReturnTargetPedForPed, 0x699F50, {.reversed=false});
}

void CFormation::DistributeDestinations(CPedList* pedlist) {
    NOTSA_UNREACHABLE();
}

void CFormation::DistributeDestinations_CoverPoints(CPedList* pedlist) {
}

void CFormation::DistributeDestinations_PedsToAttack(CPedList* pedlist) {
}

void CFormation::FindCoverPoints(CVector pos, float radius) {
}

void CFormation::FindCoverPointsBehindBox(CPointList* pointlist, CVector Pos, CMatrix* coverEntityMatrix, const CVector* vecCenter, const CVector* vecMin, const CVector* vecMax, float radius) {
}

// 0x69B1B0
int32 CFormation::FindNearestAvailableDestination(CVector pos, float& pOutDistance) {
    auto minDist = FLT_MAX;
    auto minIdx  = -1;
    for (auto&& [i, dst] : notsa::enumerate(m_Destinations.GetPoints())) {
        if (const auto d = DistanceBetweenPoints(pos, dst); d < minDist) {
            minDist = d;
            minIdx  = i;
        }
    }
    return minIdx;
}

// 0x69A620
void CFormation::GenerateGatherDestinations(CPedList* pedList, CPed* ped) {
    m_Destinations.Empty();

    const auto radius = [pedList] {
        switch (pedList->m_count) {
        case 1:  return 1.25f;
        case 2:  return 1.5f;
        case 3:  return 1.75f;
        case 4:  return 2.125f;
        default: return 2.5f;
        }
    }();

    const auto numPeds = pedList->m_count;
    for (auto&& [i, member] : notsa::enumerate(pedList->GetPeds())) {
        const auto angle = (!numPeds) ? ped->m_fCurrentRotation + HALF_PI
            : (float)i / (float)numPeds * TWO_PI - ped->m_fCurrentRotation + PI / (float)numPeds;

        if (i < NUM_POINTLIST_POINTS) {
            m_Destinations.Get(i) = CVector{ std::sin(angle) * radius, std::cos(angle) * radius, 0.0f } + ped->GetPosition();
        }
    }
}

void CFormation::GenerateGatherDestinations_AroundCar(CPedList* pedlist, CVehicle* vehicle) {
}

// 0x699FA0
bool CFormation::ReturnDestinationForPed(CPed* ped, CVector& pos) {
    for (auto&& [i, p] : notsa::enumerate(m_Peds.GetPeds())) {
        if (ped != p || m_aFinalPedLinkToDestinations[i] < 0) {
            continue;
        }
        pos = m_Destinations.Get(i);
        return true;
    }
    return false;
}

// 0x699F50
void CFormation::ReturnTargetPedForPed(CPed* ped, CPed*& outTargetPed) {
    for (auto&& [i, p] : notsa::enumerate(m_Peds.GetPeds())) {
        if (ped != p || m_aFinalPedLinkToDestinations[i] < 0) {
            continue;
        }
        outTargetPed = m_DestinationPeds.Get(m_aFinalPedLinkToDestinations[i]);
        return;
    }
}
