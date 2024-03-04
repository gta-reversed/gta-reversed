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

int32 CFormation::FindNearestAvailableDestination(CVector pos, float* pOutDistance) {
    return int32();
}

void CFormation::GenerateGatherDestinations(CPedList* pedList, CPed* ped) {
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
