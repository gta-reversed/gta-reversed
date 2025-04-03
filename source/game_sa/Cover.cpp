#include "StdInc.h"

#include "Cover.h"

void CCover::InjectHooks() {
    RH_ScopedClass(CCover);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x698710);
    RH_ScopedInstall(RemoveCoverPointIfEntityLost, 0x698DB0);
    RH_ScopedInstall(RemoveCoverPointsForThisEntity, 0x698740);
    RH_ScopedInstall(ShouldThisBuildingHaveItsCoverPointsCreated, 0x699230);
    RH_ScopedInstall(Update, 0x6997E0);
    RH_ScopedInstall(AddCoverPoint, 0x698F30);
    RH_ScopedInstall(CalculateHorizontalSize, 0x6987F0);
    RH_ScopedInstall(DoLineCheckWithinObject, 0x698990);
    RH_ScopedInstall(DoesCoverPointStillProvideCover, 0x698DD0);
    RH_ScopedInstall(Find2HighestPoints, 0x6988E0);
    RH_ScopedInstall(FindAndReserveCoverPoint, 0x6992B0);
    RH_ScopedInstall(FindCoordinatesCoverPoint, 0x699570);
    RH_ScopedInstall(FindCoverPointsForThisBuilding, 0x699120);
    RH_ScopedInstall(FindDirFromVector, 0x698D40, {.locked = true});
    RH_ScopedInstall(FindVectorFromDir, 0x698D60, {.locked = true});
    RH_ScopedInstall(FindVectorFromFirstToMissingVertex, 0x698790);
}

// 0x698710
void CCover::Init() {
    ZoneScoped;

    m_NumPoints = 0;
    m_ListOfProcessedBuildings.Flush();
    rng::fill(m_Points, CCoverPoint{});
}

// 0x698DB0
void CCover::RemoveCoverPointIfEntityLost(CCoverPoint& cpt) {
    if (cpt.GetType() > CCoverPoint::eType::NONE && cpt.GetType() < CCoverPoint::eType::NUM) {
        if (!cpt.GetCoverEntity()) {
            cpt.Remove();
        }
    }
}

// 0x698740
void CCover::RemoveCoverPointsForThisEntity(CEntity* entity) {
    for (auto& cpt : m_Points) {
        if (cpt.GetType() == CCoverPoint::eType::NONE) {
            continue;
        }
        if (cpt.GetCoverEntity() != entity) {
            continue;
        }
        cpt.Remove();
    }
}

// 0x699230
bool CCover::ShouldThisBuildingHaveItsCoverPointsCreated(CBuilding* building) {
    CVector pos;
    building->GetBoundCentre(pos);
    return IsPointInSphere(
        FindPlayerCoors(),
        pos,
        building->GetColModel()->GetBoundRadius() + 30.0f
    );
}

// 0x6997E0
void CCover::Update() {
    ZoneScoped;

    if (CReplay::Mode == MODE_PLAYBACK) {
        return;
    }

    rng::for_each(m_Points, &CCoverPoint::Update);

    /* Add new cover points for vehicles/objects */
    switch (CTimer::GetFrameCounter() % 32) {
    case 26: { // 0x6998B5
        for (auto& veh : GetVehiclePool()->GetAllValid()) {
            if (!veh.IsAutomobile()) {
                continue;
            }
            if (veh.GetMoveSpeed().SquaredMagnitude() > sq(0.05f)) {
                continue;
            }
            if (!veh.vehicleFlags.bDoesProvideCover) {
                continue;
            }
            if ((FindPlayerCoors() - veh.GetPosition()).SquaredMagnitude2D() >= sq(25.f)) {
                continue;
            }
            AddCoverPoint(CCoverPoint::eType::VEHICLE, &veh, nullptr, CCoverPoint::eUsage::LOWCOVER, TWO_PI);
        }

        return;
    }
    case 28: { // 0x6999B5
        for (auto& obj : GetObjectPool()->GetAllValid()) {
            if (obj.GetUp().z <= 0.95f) {
                continue;
            }
            if (!obj.CanBeUsedToTakeCoverBehind()) {
                continue;
            }
            if ((FindPlayerCoors() - obj.GetPosition()).SquaredMagnitude2D() >= sq(25.f)) {
                continue;
            }
            AddCoverPoint(CCoverPoint::eType::OBJECT, &obj, nullptr, CCoverPoint::eUsage::LOWCOVER, TWO_PI);
        }

        return;
    }
    }

    /* Process buildings too */
    if (CTimer::GetFrameCounter() % 8 == 5) { // 0x699A71
        // BUG:
        // This implementation may've accessed objects after they've been destroyed (use-after-free
        // I've modified it so that it uses references that get null'd automatically
        for (CPtrNodeDoubleLink<CBuilding*> *it = m_ListOfProcessedBuildings.GetNode(), *next{}; it; it = next) {
            next            = it->Next;
            auto* const obj = it->Item;

            if (!notsa::IsFixBugs() || obj) { // If fixbugs the reference may've got cleared
                if (ShouldThisBuildingHaveItsCoverPointsCreated(obj)) {
                    continue;
                }
                RemoveCoverPointsForThisEntity(obj);
                if (notsa::IsFixBugs()) { // FIXBUGS: Use-after-free
                    CEntity::SafeCleanUpRef(reinterpret_cast<CEntity*&>(it->Item));
                }
            }
            m_ListOfProcessedBuildings.DeleteNode(it);
        }

        if (!FindPlayerVehicle()) { // 0x699AE1
            CWorld::IncrementCurrentScanCode();
            CWorld::IterateSectorsOverlappedByRect(CRect{ FindPlayerCoors(), 30.f }, [&](int32 x, int32 y) {
                for (auto* const obj : GetSector(x, y)->m_buildings) {
                    if (obj->IsScanCodeCurrent()) {
                        continue;
                    }
                    obj->SetCurrentScanCode();
                    if (!ShouldThisBuildingHaveItsCoverPointsCreated(obj)) {
                        continue;
                    }
                    if (m_ListOfProcessedBuildings.IsMemberOfList(obj)) {
                        continue;
                    }
                    FindCoverPointsForThisBuilding(obj);
                    auto* const link = m_ListOfProcessedBuildings.AddItem(obj);
                    if (notsa::IsFixBugs()) { // FIXBUGS: Use-after-free
                        CEntity::SafeRegisterRef(reinterpret_cast<CEntity*&>(link->Item));
                    }
                }
                return true;
            });
        }
    }
}

// notsa - allocate new point, increments `NumPoints`
CCoverPoint* CCover::GetFree() {
    for (auto& cpt : m_Points) {
        if (cpt.GetType() == CCoverPoint::eType::NONE) {
            m_NumPoints++;
            return &cpt;
        }
    }
    return nullptr;
}

// 0x698F30
// NOTE: Original function didn't return the (possibly) created point, we do
CCoverPoint* CCover::AddCoverPoint(CCoverPoint::eType type, CEntity* coverEntity, const CVector* pos, CCoverPoint::eUsage usage, CCoverPoint::Dir dir) {
    // `POINTONMAP` is tied to a building, thus both variables are supposed to be set
    assert(type == CCoverPoint::eType::POINTONMAP || !!pos != !!coverEntity && "Only either `position` or `coverEntity` must be set"); 

    if (m_NumPoints >= m_Points.size()) {
        return nullptr;
    }

    // Check if there's already a cover point nearby
    // If so, we might not need to allocate a new one
    for (auto& cpt : m_Points) {
        // 0x698F5D
        if (cpt.GetType() == type) {
            switch (cpt.GetType()) {
            case CCoverPoint::eType::OBJECT:
            case CCoverPoint::eType::VEHICLE: {
                if (coverEntity == cpt.GetCoverEntity()) {
                    return &cpt;
                }
                break;
            }
            case CCoverPoint::eType::POINTONMAP: {
                if (*pos == cpt.GetPointOnMap()) {
                    return &cpt;
                }
                break;
            }
            }
        }

        // 0x698F9F - BUG: `GetPointOnMap()` only works if the type is `POINTONMAP`
        if (notsa::IsFixBugs() ? cpt.GetType() != CCoverPoint::eType::POINTONMAP : cpt.GetType() == CCoverPoint::eType::NONE) {
            continue;
        }

        const CVector atPos = coverEntity
            ? coverEntity->GetPosition()
            : *pos;
        const auto distToCptSq = (atPos - cpt.GetPointOnMap()).SquaredMagnitude();

        // 0x69901E
        if (distToCptSq < sq(0.8f)) {
            return &cpt;
        }

        // 0x69903C
        if (distToCptSq < sq(2.f)) {
            const auto d = cpt.GetDir() - dir;
            //if (d < 32 || d > 223) { // 45=32/255*360, 315=223/255*360
            //    return;
            //}
            if (d < DegreesToRadians(45.f) || DegreesToRadians(315.f) > d) {
                return &cpt;
            }
        }
    }

    // 0x699071
    return new (GetFree()) CCoverPoint{ // May return null
        type,
        usage,
        dir,
        coverEntity,
        pos
    };
}

// 0x6987F0
float CCover::CalculateHorizontalSize(CColTriangle* triangle, CVector* vertPositions) {
    NOTSA_UNREACHABLE("Unused"); //return plugin::CallAndReturn<float, 0x6987F0, CColTriangle*, CVector*>(triangle, vertPositions);
}

// 0x698990
bool CCover::DoLineCheckWithinObject(CColTriangle* triangle, int32 a2, CVector* a3, CVector* a4, CVector a5, CVector a6) {
    NOTSA_UNREACHABLE("Unused"); // return plugin::CallAndReturn<bool, 0x698990, CColTriangle*, int32, CVector*, CVector*, CVector, CVector>(triangle, a2, a3, a4, a5, a6);
}

// 0x698DD0
bool CCover::DoesCoverPointStillProvideCover(CCoverPoint* cpt, CVector pos) {
    if (!cpt) {
        return false;
    }
    RemoveCoverPointIfEntityLost(*cpt);
    switch (cpt->GetType()) {
    case CCoverPoint::eType::NONE:
        return false;
    case CCoverPoint::eType::POINTONMAP: {
        if (FindVectorFromDir(cpt->GetDir()).Dot(pos - cpt->GetPointOnMap()) > 0.f) {
            return true;
        }
        return false;
    }
    }
    return true;
}

// 0x6988E0
void CCover::Find2HighestPoints(CColTriangle* triangle, CVector* vertPositions, int32& outPoint1, int32& outPoint2) {
    NOTSA_UNREACHABLE("Unused"); // plugin::Call<0x6988E0, CColTriangle*, CVector*, int32&, int32&>(triangle, vertPositions, outPoint1, outPoint2);
}

// 0x6992B0
CCoverPoint* CCover::FindAndReserveCoverPoint(CPed* ped, const CVector& targetPos, bool isForAttack) {
    rng::for_each(m_Points, RemoveCoverPointIfEntityLost);

    auto points = m_Points |
        rng::views::filter(&CCoverPoint::CanAccommodateAnotherPed) |
        rng::views::filter([&](const CCoverPoint& cpt) { // 0x69936B (Yes, I had to move this up here)
            return cpt.GetType() != CCoverPoint::eType::POINTONMAP
                || (targetPos - cpt.GetPointOnMap()).Dot(cpt.GetDirVector()) > 0.f;
        }) |
        rng::views::transform([&](CCoverPoint& cpt) -> std::pair<CCoverPoint*, CVector> { // 0x699316
            return { &cpt, cpt.GetPos() };
        }) |
        rng::views::filter([&](auto&& pair) { // 0x6994A1
            return !isForAttack || CVector2D::Dist(ped->GetPosition2D(), pair.second) < CVector2D::Dist(ped->GetPosition2D(), targetPos) + 4.f;
        }) |
        rng::views::transform([&](auto&& pair) -> std::pair<CCoverPoint*, float> { 
            return { pair.first, CVector2D::DistSqr(ped->GetPosition2D(), pair.second) };
        });
    if (points.empty()) {
        return nullptr;
    }
    const auto [cpt, dist] = *rng::max_element(points, [](auto&& a, auto&& b) { return a.second < b.second; });;
    if (!rng::contains(cpt->GetCoveredPeds(), ped)) {
        return nullptr;
    }
    cpt->ReserveCoverPointForPed(ped);
    return cpt;
}

// 0x699570
bool CCover::FindCoordinatesCoverPoint(const CCoverPoint& cpt, CPed* ped, const CVector& position, CVector& outCoordinates) {
    return cpt.FindCoordinatesCoverPoint(ped, position, outCoordinates);
}

// 0x699120
void CCover::FindCoverPointsForThisBuilding(CBuilding* building) {
    auto* mi = CModelInfo::GetModelInfo(building->m_nModelIndex);
    for (int32 i = 0; i < mi->m_n2dfxCount; ++i) {
        if (auto* const fx = notsa::dyn_cast<C2dEffectCoverPoint>(mi->Get2dEffect(i))) {
            const auto dir = building->GetMatrix().TransformVector(CVector{fx->m_DirOfCover, 0.f});
            const auto pos = building->GetMatrix().TransformPoint(fx->m_Pos);
            CCover::AddCoverPoint(
                CCoverPoint::eType::POINTONMAP,
                building,
                &pos,
                fx->m_Usage,
                std::atan2(-dir.x, dir.y)
            );
        }
    }
}

// 0x698D40 - unused
CCoverPoint::Dir CCover::FindDirFromVector(CVector dir) {
    return atan2(-dir.x, dir.y);
}

// 0x698D60
CVector CCover::FindVectorFromDir(CCoverPoint::Dir direction) {
    return CVector{ std::sin(direction), std::cos(direction), 0.f };
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
