#include "StdInc.h"

#include "EntityScanner.h"

void CEntityScanner::InjectHooks() {
    RH_ScopedVirtualClass(CEntityScanner, 0x86CD40, 1);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Constructor, 0x5FF990);
    RH_ScopedInstall(Destructor, 0x603480);

    RH_ScopedInstall(Clear, 0x5FF9D0);
    RH_ScopedVMTInstall(ScanForEntitiesInRange, 0x5FFA20);
}

// 0x5FF990
CEntityScanner::CEntityScanner() {
    m_timer = {};

    rng::fill(m_apEntities, nullptr);
    m_pClosestEntityInRange = nullptr;

    m_timer.SetPeriod(MAX_NUM_ENTITIES);
}

// 0x603480
CEntityScanner::~CEntityScanner() {
    Clear();
}

// 0x5FF9D0
void CEntityScanner::Clear() {
    for (auto& entity : m_apEntities) {
        CEntity::ClearReference(entity);
    }

    CEntity::ClearReference(m_pClosestEntityInRange);
}

// 0x5FFA20
void CEntityScanner::ScanForEntitiesInRange(eRepeatSectorList sectorList, CPed& ped) {
    if (!m_timer.Tick()) {
        return;
    }

    Clear();

    const auto        isPlayer = FindPlayerPed() == &ped;
    const auto* const intel    = ped.GetIntelligence();
    const auto        range    = std::max(intel->m_fHearingRange, intel->m_fSeeingRange);
    const auto&       pos      = ped.GetPosition();

    size_t numEntities{};
    std::array<float, MAX_NUM_ENTITIES> distancesSq;
    rng::fill(distancesSq, FLT_MAX);

    CWorld::AdvanceCurrentScanCode();
    ped.SetCurrentScanCode();

    const auto ProcessList = [&](auto& list) {
        for (CEntity* const entity : list) {
            if (!entity->ProcessScan()) {
                continue;
            }
            if (sectorList == REPEATSECTOR_PEDS && !isPlayer && entity->AsPed()->IsStateDead()) {
                continue;
            }

            const auto distSq = (entity->GetPosition() - pos).SquaredMagnitude();
            if (distSq >= sq(range)) {
                continue;
            }

            // Insert it so the array stays sorted by distance (closer to further)
            for (auto i = 0u; i < MAX_NUM_ENTITIES; i++) {
                if (!m_apEntities[i]) {
                    distancesSq[i] = distSq;
                    numEntities++;
                    m_apEntities[i] = entity;
                    break;
                }
                if (distSq < distancesSq[i]) {
                    // Make room for it (If the array is full the furthest one is dropped)
                    const auto last = std::min(numEntities + 1, MAX_NUM_ENTITIES);
                    rng::shift_right(m_apEntities.begin() + i, m_apEntities.begin() + last, 1);
                    rng::shift_right(distancesSq.begin() + i, distancesSq.begin() + last, 1);
                    distancesSq[i]  = distSq;
                    m_apEntities[i] = entity;
                    if (numEntities < MAX_NUM_ENTITIES) {
                        numEntities++;
                    }
                    break;
                }
            }
        }
    };
    CWorld::IterateSectorsOverlappedByRect(CRect{ pos, range }, [&](int32 x, int32 y) {
        if (x < 0 || y < 0 || x >= MAX_SECTORS_X || y >= MAX_SECTORS_Y) {
            return true; // The original only scans the sectors inside the map (`GetRepeatSector` would wrap the others onto other tiles)
        }
        auto& sector = CWorld::GetRepeatSector(x, y);
        switch (sectorList) {
        case REPEATSECTOR_VEHICLES: ProcessList(sector.Vehicles); break;
        case REPEATSECTOR_PEDS:     ProcessList(sector.Peds);     break;
        case REPEATSECTOR_OBJECTS:  ProcessList(sector.Objects);  break;
        default:                    NOTSA_UNREACHABLE_CASE(sectorList);
        }
        return true;
    });

    for (auto& entity : m_apEntities | rngv::take(numEntities)) {
        if (entity) {
            CEntity::RegisterReference(entity);
        }
    }

    if (const auto closest = m_apEntities.front()) {
        CEntity::SetEntityReference(m_pClosestEntityInRange, closest);
    }
}
