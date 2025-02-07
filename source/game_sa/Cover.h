/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Vector.h"

class CEntity;
class CPtrListDoubleLink;
class CCoverPoint;
class CColTriangle;
class CBuilding;
class CPed;

class CCover {
public:
    inline static uint32& m_NumPoints = *reinterpret_cast<uint32*>(0xC197A4);
    inline static std::array<CCoverPoint, 100>& m_aPoints = *reinterpret_cast<std::array<CCoverPoint, 100>*>(0xC197C8);
    inline static CPtrListDoubleLink& m_ListOfProcessedBuildings = *reinterpret_cast<CPtrListDoubleLink*>(0xC1A2B8);

public:
    static void InjectHooks();

    static void Init();
    static void RemoveCoverPointIfEntityLost(CCoverPoint* point);
    static void RemoveCoverPointsForThisEntity(CEntity* entity);
    static bool ShouldThisBuildingHaveItsCoverPointsCreated(CBuilding* building);
    static void Update();

    static void AddCoverPoint(int32 maxPeds, CEntity* coverEntity, CVector* position, char coverType, uint8 direction);
    static float CalculateHorizontalSize(CColTriangle* triangle, CVector* vertPositions);
    static bool DoLineCheckWithinObject(CColTriangle* triangle, int32 a2, CVector* a3, CVector* a4, CVector a5, CVector a6);
    static bool DoesCoverPointStillProvideCover(CCoverPoint* point, CVector position);
    static void Find2HighestPoints(CColTriangle* triangle, CVector* vertPositions, int32& outPoint1, int32& outPoint2);
    static CCoverPoint* FindAndReserveCoverPoint(CPed* ped, const CVector& position, bool a3);
    static bool FindCoordinatesCoverPoint(CCoverPoint* point, CPed* ped, const CVector& position, CVector& outCoordinates);
    static void FindCoverPointsForThisBuilding(CBuilding* building);
    static uint8 FindDirFromVector(CVector dir);
    static CVector FindVectorFromDir(uint8 direction);
    static CVector FindVectorFromFirstToMissingVertex(CColTriangle* triangle, int32* a3, CVector* vertPositions);
};
