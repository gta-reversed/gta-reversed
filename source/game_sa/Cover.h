/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Vector.h"
#include "PtrListDoubleLink.h"

class CEntity;
class CCoverPoint;
class CColTriangle;
class CBuilding;
class CPed;

class CCover {
public:
    inline static uint32&                        m_NumPoints                = *reinterpret_cast<uint32*>(0xC197A4);
    inline static std::array<CCoverPoint, 0x64>& m_Points                   = *reinterpret_cast<std::array<CCoverPoint, 100>*>(0xC197C8);
    inline static auto&                          m_ListOfProcessedBuildings = StaticRef<CPtrListDoubleLink<CBuilding*>>(0xC1A2B8);

public:
    static void InjectHooks();

    static void Init();
    static void RemoveCoverPointIfEntityLost(CCoverPoint& point);
    static void RemoveCoverPointsForThisEntity(CEntity* entity);
    static bool ShouldThisBuildingHaveItsCoverPointsCreated(CBuilding* building);
    static void Update();

    static CCoverPoint* GetFree();

    static CCoverPoint* AddCoverPoint(CCoverPoint::eType type, CEntity* coverEntity, const CVector* position, CCoverPoint::eUsage usage, CCoverPoint::Dir dir);
    static float CalculateHorizontalSize(CColTriangle* triangle, CVector* vertPositions);
    static bool DoLineCheckWithinObject(CColTriangle* triangle, int32 a2, CVector* a3, CVector* a4, CVector a5, CVector a6);
    static bool DoesCoverPointStillProvideCover(CCoverPoint* point, CVector position);
    static void Find2HighestPoints(CColTriangle* triangle, CVector* vertPositions, int32& outPoint1, int32& outPoint2);
    static CCoverPoint* FindAndReserveCoverPoint(CPed* ped, const CVector& position, bool isForAttack);
    static bool FindCoordinatesCoverPoint(const CCoverPoint& point, CPed* ped, const CVector& position, CVector& outCoordinates);
    static void FindCoverPointsForThisBuilding(CBuilding* building);
    static uint8 FindDirFromVector(CVector dir);
    static CVector FindVectorFromDir(uint8 direction);
    static CVector FindVectorFromFirstToMissingVertex(CColTriangle* triangle, int32* a3, CVector* vertPositions);

    static auto& GetCoverPoints() { return m_Points; }
};
