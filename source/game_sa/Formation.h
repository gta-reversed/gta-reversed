/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include <Vector.h>
#include "PedGroupMembership.h"

class CPedList;
class CPointList;
class CEntity;
class CPed;
class CMatrix;

class CFormation {
public:
    static inline auto& m_aFinalPedLinkToDestinations = StaticRef<std::array<int32, TOTAL_PED_GROUP_MEMBERS>>(0xC1A4DC);
    static inline auto& m_aPedLinkToDestinations      = StaticRef<std::array<int32, TOTAL_PED_GROUP_MEMBERS>>(0xC1A2C0);
    static inline auto& m_Destinations                = StaticRef<CPointList>(0xC1A318);
    static inline auto& m_DestinationPeds             = StaticRef<CPedList>(0xC1A458);
    static inline auto& m_Peds                        = StaticRef<CPedList>(0xC1A4D8);

    static void InjectHooks();

    static void DistributeDestinations(CPedList& pedlist);
    static void DistributeDestinations_CoverPoints(const CPedList& pedlist, CVector pos);
    static void DistributeDestinations_PedsToAttack(const CPedList& pedlist);
    static void FindCoverPoints(CVector pos, float radius);
    static void FindCoverPointsBehindBox(
        CPointList*    outPoints,
        CVector        target,
        CMatrix*       mat,
        const CVector& center, //!< Unused
        const CVector& bbMin,
        const CVector& bbMax,
        float          cutoffDist
    );
    static void GenerateGatherDestinations(CPedList& pedList, CPed* ped);
    static void GenerateGatherDestinations_AroundCar(CPedList& pedlist, CVehicle* vehicle);
    static bool ReturnDestinationForPed(CPed* ped, CVector* out);
    static void ReturnTargetPedForPed(CPed* ped, CPed** pOutTargetPed);
};
