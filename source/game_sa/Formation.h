/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

class CPedList;
class CPointList;
class CEntity;
class CPed;
class CMatrix;

class CFormation {
public:
    static inline CPointList& m_Destinations    = StaticRef<CPointList>(0xC1A318);
    static inline CPedList&   m_DestinationPeds = StaticRef<CPedList>(0xC1A458);
    static inline CPedList&   m_Peds            = StaticRef<CPedList>(0xC1A4D8);

    static void InjectHooks();

    static void DistributeDestinations(CPedList* pedlist);
    static void DistributeDestinations_CoverPoints(CPedList* pedlist);
    static void DistributeDestinations_PedsToAttack(CPedList* pedlist);
    static void FindCoverPoints(CVector pos, float radius);
    static void FindCoverPointsBehindBox(CPointList* pointlist, CVector Pos, CMatrix* coverEntityMatrix, const CVector* vecCenter, const CVector* vecMin, const CVector* vecMax, float radius);
    static int32 FindNearestAvailableDestination(CVector pos, float& outDistance);
    static void GenerateGatherDestinations(CPedList* pedList, CPed* ped);
    static void GenerateGatherDestinations_AroundCar(CPedList* pedlist, CVehicle* vehicle);
    static bool ReturnDestinationForPed(CPed* ped, CVector& pos);
    static void ReturnTargetPedForPed(CPed* ped, CPed*& outTargetPed);
};

static inline auto& m_aFinalPedLinkToDestinations = StaticRef<std::array<int32, 8>>(0xC1A2C0);
static inline auto& m_aPedLinkToDestinations      = StaticRef<std::array<int32, 8>>(0xC1A2E0);
