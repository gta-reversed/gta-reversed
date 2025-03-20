#include "StdInc.h"

#include "Cover.h"

void CCover::InjectHooks() {
    RH_ScopedClass(CCover);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x698710);
    RH_ScopedInstall(RemoveCoverPointIfEntityLost, 0x698DB0);
    RH_ScopedInstall(RemoveCoverPointsForThisEntity, 0x698740, {.reversed = false});
    RH_ScopedInstall(ShouldThisBuildingHaveItsCoverPointsCreated, 0x699230);
    RH_ScopedInstall(Update, 0x6997E0, {.reversed = false});

    RH_ScopedInstall(AddCoverPoint, 0x698F30, {.reversed = false});
    RH_ScopedInstall(CalculateHorizontalSize, 0x6987F0, {.reversed = false});
    RH_ScopedInstall(DoLineCheckWithinObject, 0x698990, {.reversed = false});
    RH_ScopedInstall(DoesCoverPointStillProvideCover, 0x698DD0, {.reversed = false});
    RH_ScopedInstall(Find2HighestPoints, 0x6988E0, {.reversed = false});
    RH_ScopedInstall(FindAndReserveCoverPoint, 0x6992B0, {.reversed = false});
    RH_ScopedInstall(FindCoordinatesCoverPoint, 0x699570, {.reversed = false});
    RH_ScopedInstall(FindCoverPointsForThisBuilding, 0x699120);
    RH_ScopedInstall(FindDirFromVector, 0x698D40);
    RH_ScopedInstall(FindVectorFromDir, 0x698D60);
    RH_ScopedInstall(FindVectorFromFirstToMissingVertex, 0x698790);
}

// 0x698710
void CCover::Init() {
    ZoneScoped;

    m_NumPoints = 0;
    m_ListOfProcessedBuildings.Flush();
    rng::fill(m_aPoints, CCoverPoint{});
}

// unused
// 0x698DB0
void CCover::RemoveCoverPointIfEntityLost(CCoverPoint* point) {
    if (0 < point->m_nMaxPedsInCover && point->m_nMaxPedsInCover < 4 && !point->m_pCoverEntity) {
        point->m_nMaxPedsInCover = 0;
        m_NumPoints--;
    }
}

// 0x698740
void CCover::RemoveCoverPointsForThisEntity(CEntity* entity) {
    plugin::Call<0x698740>();
}

// 0x699230
bool CCover::ShouldThisBuildingHaveItsCoverPointsCreated(CBuilding* building) {
    CVector vecCenter;
    building->GetBoundCentre(vecCenter);
    auto* mi = CModelInfo::GetModelInfo(building->m_nModelIndex);
    return IsPointInSphere(FindPlayerCoors(), vecCenter, mi->GetColModel()->GetBoundRadius() + 30.0f);
}

// 0x6997E0
void CCover::Update() {
    ZoneScoped;

    plugin::Call<0x6997E0>();
}

// 0x698F30
void CCover::AddCoverPoint(int32 maxPeds, CEntity* coverEntity, CVector* position, char coverType, uint8 direction) {
    plugin::Call<0x698F30, int32, CEntity*, CVector*, char, uint8>(maxPeds, coverEntity, position, coverType, direction);
}

// unused
// 0x6987F0
float CCover::CalculateHorizontalSize(CColTriangle* triangle, CVector* vertPositions) {
    return plugin::CallAndReturn<float, 0x6987F0, CColTriangle*, CVector*>(triangle, vertPositions);
}

// unused
// 0x698990
bool CCover::DoLineCheckWithinObject(CColTriangle* triangle, int32 a2, CVector* a3, CVector* a4, CVector a5, CVector a6) {
    return plugin::CallAndReturn<bool, 0x698990, CColTriangle*, int32, CVector*, CVector*, CVector, CVector>(triangle, a2, a3, a4, a5, a6);
}

// 0x698DD0
bool CCover::DoesCoverPointStillProvideCover(CCoverPoint* point, CVector position) {
    return plugin::CallAndReturn<bool, 0x698DD0, CCoverPoint*, CVector>(point, position);
}

// unused
// 0x6988E0
void CCover::Find2HighestPoints(CColTriangle* triangle, CVector* vertPositions, int32& outPoint1, int32& outPoint2) {
    plugin::Call<0x6988E0, CColTriangle*, CVector*, int32&, int32&>(triangle, vertPositions, outPoint1, outPoint2);
}

// 0x6992B0
CCoverPoint* CCover::FindAndReserveCoverPoint(CPed* ped, const CVector& position, bool a3) {
    return plugin::CallAndReturn<CCoverPoint*, 0x6992B0, CPed*, const CVector&, bool>(ped, position, a3);
}

// 0x699570
bool CCover::FindCoordinatesCoverPoint(CCoverPoint* point, CPed* ped, const CVector& position, CVector& outCoordinates) {
    return plugin::CallAndReturn<bool, 0x699570, CCoverPoint*, CPed*, const CVector&, CVector&>(point, ped, position, outCoordinates);
}

// 0x699120
void CCover::FindCoverPointsForThisBuilding(CBuilding* building) {
    auto* mi = CModelInfo::GetModelInfo(building->m_nModelIndex);
    if (!mi->m_n2dfxCount) {
        return;
    }

    for (int32 iFxInd = 0; iFxInd < mi->m_n2dfxCount; ++iFxInd) {
        auto* effect = mi->Get2dEffect(iFxInd);
        if (effect->m_Type != e2dEffectType::EFFECT_COVER_POINT)
            continue;
        }

        auto vecDir = CVector(effect->coverPoint.m_vecDirection.x, effect->coverPoint.m_vecDirection.y, 0.0F);
        const auto vedTransformed = building->GetMatrix().TransformVector(vecDir);

        const auto fTwoPiToChar = 256.0F / TWO_PI;
        const auto ucAngle = static_cast<uint8>(atan2(vedTransformed.x, vedTransformed.y) * fTwoPiToChar);
        auto vecPoint = building->GetMatrix().TransformPoint(effect->m_Pos);
        CCover::AddCoverPoint(3, building, &vecPoint, effect->coverPoint.m_nType, ucAngle);
    }
}

// 0x698D40
uint8 CCover::FindDirFromVector(CVector dir) {
    NOTSA_UNUSED_FUNCTION();

    //return (uint8)(atan2(-dir.x, dir.y) * 255.f / TWO_PI);
}

// 0x698D60
CVector CCover::FindVectorFromDir(uint8 direction) {
    CVector vector;
    vector.x = (float)sin(direction / (256.f / TWO_PI));
    vector.y = (float)cos(direction / (256.f / TWO_PI));
    vector.z = 0.0;
    return vector;
}

// unused
// 0x698790
CVector CCover::FindVectorFromFirstToMissingVertex(CColTriangle* triangle, int32* a3, CVector* vertPositions) {
    NOTSA_UNUSED_FUNCTION();

    //uint16 vertexIndex;
    //uint16 referenceIndex = *a3;
    //
    //// Is vertex missing ?
    //if ((triangle->vA != referenceIndex && triangle->vA != a3[1])) {
    //    vertexIndex = triangle->vA;
    //} else if (triangle->vB != referenceIndex && triangle->vB != a3[1]) {
    //    vertexIndex = triangle->vB;
    //} else {
    //    vertexIndex = triangle->vC;
    //}
    //
    //CVector vector;
    //vector.x = vertPositions[vertexIndex].x - vertPositions[referenceIndex].x;
    //vector.y = vertPositions[vertexIndex].y - vertPositions[referenceIndex].y;
    //vector.z = vertPositions[vertexIndex].z - vertPositions[referenceIndex].z;
    //
    //return vector;
}
