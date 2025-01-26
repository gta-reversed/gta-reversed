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

public:
    static void InjectHooks();

    static void ReturnTargetPedForPed(CPed* a1, CPed** a2);
    static bool ReturnDestinationForPed(CPed* ped, CVector& dest);
    static void FindCoverPointsBehindBox(CPointList& points, CVector target, const CMatrix& mat, const CVector& bbOffset, const CVector& bbMin, const CVector& bbMax, float radius);
    static void GenerateGatherDestinations(CPedList& pedList, CPed* ped);
    static void GenerateGatherDestinations_AroundCar(CPedList& pedList, CVehicle* veh);
    static void DistributeDestinations(CPedList& pedList);
    static void DistributeDestinations_CoverPoints(const CPedList& pedlist, CVector pos);
    static void DistributeDestinations_PedsToAttack(const CPedList& pedList);
    static void FindCoverPoints(CVector pos, float radius);
};
