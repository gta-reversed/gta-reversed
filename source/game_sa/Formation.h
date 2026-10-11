/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include <Vector.h>
#include "PedList.h"
#include "PointList.h"

class CFormation {
public:
    // Both arrays hold one entry per group member (Android: 8 entries each, `m_aPedLinkToDestinations` placed first).
    // On PC they're laid out the other way around, and the memory after `m_aPedLinkToDestinations` (0xC1A300) holds unrelated globals.
    static inline std::array<int32, TOTAL_PED_GROUP_MEMBERS> m_aPedLinkToDestinations{};      // 0xC1A2E0 - Links of the distribution currently being tried by `DistributeDestinations`
    static inline std::array<int32, TOTAL_PED_GROUP_MEMBERS> m_aFinalPedLinkToDestinations{}; // 0xC1A2C0 - Chosen links, indices into `m_Destinations` or `m_DestinationPeds` (`-1` if none)
    static inline CPointList                                 m_Destinations{};                // 0xC1A318
    static inline CPedList                                   m_DestinationPeds{};             // 0xC1A458
    static inline CPedList                                   m_Peds{};                        // 0xC1A4D8

    static void InjectHooks();

    static void DistributeDestinations(CPedList& pedList);
    static void DistributeDestinations_CoverPoints(const CPedList& pedList, CVector pos);
    static void DistributeDestinations_PedsToAttack(const CPedList& pedList);
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
    static void GenerateGatherDestinations_AroundCar(CPedList& pedList, CVehicle* vehicle);
    static bool ReturnDestinationForPed(CPed* ped, CVector* outDestination);
    static void ReturnTargetPedForPed(CPed* ped, CPed** outTargetPed);
};
