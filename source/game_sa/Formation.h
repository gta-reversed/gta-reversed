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
    static int32 (&m_aFinalPedLinkToDestinations)[8]; // static int32 m_aFinalPedLinkToDestinations[8]
    static int32 (&m_aPedLinkToDestinations)[8];      // static int32 m_aPedLinkToDestinations[8]
    static CPointList& m_Destinations;
    static CPedList&   m_DestinationPeds;
    static CPedList&   m_Peds;

    static void InjectHooks();

    static void DistributeDestinations(CPedList* pedlist);
    static void DistributeDestinations_CoverPoints(CPedList* pedlist, CVector pos);
    static void DistributeDestinations_PedsToAttack(CPedList* pedlist);
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
    static int32 FindNearestAvailableDestination(CVector pos, float* pOutDistance);
    static void GenerateGatherDestinations(CPedList* pedList, CPed* ped);
    static void GenerateGatherDestinations_AroundCar(CPedList* pedlist, CVehicle* vehicle);
    static bool ReturnDestinationForPed(CPed* ped, CVector* pos);
    static int32 ReturnTargetPedForPed(CPed* ped, CPed** pOutTargetPed);
};
