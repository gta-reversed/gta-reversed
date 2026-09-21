#include "StdInc.h"

#include "EntityScanner.h"

void CEntityScanner::InjectHooks() {
    RH_ScopedClass(CEntityScanner);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Constructor, 0x5FF990);
    RH_ScopedInstall(Destructor, 0x603480);

    RH_ScopedInstall(Clear, 0x5FF9D0);
    RH_ScopedInstall(ScanForEntitiesInRange, 0x5FFA20);
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
void CEntityScanner::ScanForEntitiesInRange(const eRepeatSectorList sectorList, const CPed& ped) {
    if (!m_timer.Tick()) {
        return;
    }
    Clear();

    const CPed* const playerPed = FindPlayerPed();
    const auto& intel = *ped.GetIntelligence();
    const float scanRange = std::max(intel.m_fHearingRange, intel.m_fSeeingRange);
    const CVector& pedPos = ped.GetPosition();

    float dists[MAX_NUM_ENTITIES];
    rng::fill(dists, FLT_MAX);
    int32 numFound = 0;

    // Repeat sectors are 16x16 over the 120x120 sector map, i.e. each covers 7.5 normal sectors (375 world units).
    // The binary computes `(pos +/- range) * 0.02 + 60` floored => index into the 16-wide repeat grid.
    const int32 startX = std::max((int32)std::floor((pedPos.x - scanRange) * 0.02f + 60.f), 0);
    const int32 startY = std::max((int32)std::floor((pedPos.y - scanRange) * 0.02f + 60.f), 0);
    const int32 endX = (int32)std::floor((pedPos.x + scanRange) * 0.02f + 60.f);
    const int32 endY = (int32)std::floor((pedPos.y + scanRange) * 0.02f + 60.f);

    CWorld::AdvanceCurrentScanCode();
    const_cast<CPed&>(ped).SetScanCode(CWorld::ms_nCurrentScanCode);


    for (int32 sy = startY; sy <= endY; sy++) {
        for (int32 sx = startX; sx <= endX; sx++) {
            auto& rs = CWorld::GetRepeatSector(sx, sy);
            const auto ProcessEntity = [&](CEntity* entity) {
                if (entity->GetScanCode() == CWorld::ms_nCurrentScanCode) {
                    return;
                }
                entity->SetScanCode(CWorld::ms_nCurrentScanCode);
                if (sectorList == REPEATSECTOR_PEDS) {
                    // Dead peds are skipped unless the scanning ped is the player
                    // (binary checks `pedState == 55` i.e. PEDSTATE_DEAD at +0x530)
                    if (&ped != playerPed && entity->AsPed()->m_nPedState == PEDSTATE_DEAD) {
                        return;
                    }
                }
                const float distSq = (entity->GetPosition() - pedPos).SquaredMagnitude();
                if (distSq >= scanRange * scanRange) {
                    return;
                }
                // Sorted insert by distance (closest first)
                int32 insertAt = 0;
                while (insertAt < (int32)MAX_NUM_ENTITIES && m_apEntities[insertAt] && distSq >= dists[insertAt]) {
                    insertAt++;
                }
                if (insertAt >= (int32)MAX_NUM_ENTITIES) {
                    return;
                }
                if (insertAt < CGeneral::GetRandomNumber()) {
                    const int32 last = std::min<int32>(CGeneral::GetRandomNumber(), (int32)MAX_NUM_ENTITIES - 1);
                    for (int32 i = last; i > insertAt; i--) {
                        m_apEntities[i] = m_apEntities[i - 1];
                        dists[i] = dists[i - 1];
                    }
                }
                dists[insertAt] = distSq;
                m_apEntities[insertAt] = entity;
                if (numFound < (int32)MAX_NUM_ENTITIES) {
                    numFound++;
                }
            };
            switch (sectorList) {
            case REPEATSECTOR_VEHICLES:
                for (auto* const entity : rs.Vehicles) {
                    ProcessEntity(entity);
                }
                break;
            case REPEATSECTOR_PEDS:
                for (auto* const entity : rs.Peds) {
                    ProcessEntity(entity);
                }
                break;
            case REPEATSECTOR_OBJECTS:
                for (auto* const entity : rs.Objects) {
                    ProcessEntity(entity);
                }
                break;
            default:
                break;
            }
        }
    }

    for (int32 i = 0; i < numFound; i++) {
        if (m_apEntities[i]) {
            m_apEntities[i]->RegisterReference(&m_apEntities[i]);
        }
    }
    if (m_apEntities[0]) {
        m_pClosestEntityInRange = m_apEntities[0];
        CEntity::SafeRegisterRef(m_pClosestEntityInRange);
    }
}
