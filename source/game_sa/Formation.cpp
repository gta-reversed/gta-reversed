#include "StdInc.h"
#include "Formation.h"

// 0x699F50
void CFormation::ReturnTargetPedForPed(CPed* a1, CPed** a2) {
    return plugin::Call<0x699F50, CPed*, CPed**>(a1, a2);
}

// 0x699FA0
bool CFormation::ReturnDestinationForPed(CPed* ped, CVector& dest) {
    return plugin::CallAndReturn<bool, 0x699FA0>(ped, &dest);
}

// 0x699FF0
void CFormation::FindCoverPointsBehindBox(CPointList& points, CVector target, const CMatrix& mat, const CVector& bbOffset, const CVector& bbMin, const CVector& bbMax, float radius) {
    plugin::Call<0x699FF0>(points, target, mat, bbOffset, bbMin, bbMax, radius);
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
    RH_ScopedGlobalInstall(FindCoverPointsBehindBox, 0x699FF0, { .reversed = false });
    RH_ScopedGlobalInstall(GenerateGatherDestinations, 0x69A620, { .reversed = false });
    RH_ScopedGlobalInstall(GenerateGatherDestinations_AroundCar, 0x69A770, { .reversed = false });
    RH_ScopedGlobalInstall(DistributeDestinations, 0x69B240, { .reversed = false });
    RH_ScopedGlobalInstall(DistributeDestinations_CoverPoints, 0x69B5B0, { .reversed = false });
    RH_ScopedGlobalInstall(DistributeDestinations_PedsToAttack, 0x69B700, { .reversed = false });
    RH_ScopedGlobalInstall(FindCoverPoints, 0x69B860, { .reversed = false });
}
