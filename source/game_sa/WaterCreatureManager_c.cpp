#include "StdInc.h"

#include "WaterCreatureManager_c.h"

auto& g_waterCreatureMan = StaticRef<WaterCreatureManager_c>(0xC1DF30);

void WaterCreatureManager_c::InjectHooks() {
    RH_ScopedClass(WaterCreatureManager_c);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x6E3F90);
    RH_ScopedInstall(Exit, 0x6E3FD0);
    RH_ScopedInstall(Update, 0x6E4F10);
    RH_ScopedInstall(GetRandomWaterCreatureId, 0x6E4040);
    RH_ScopedInstall(CanAddWaterCreatureAtPos, 0x6E4510);
    RH_ScopedInstall(TryToFreeUpWaterCreatures, 0x6E40E0);
    RH_ScopedInstall(TryToExitGroup, 0x6E45B0);
}

WaterCreatureManager_c::WaterCreatureManager_c() : m_waterCreatureItems(), m_waterCreaturePool(), m_waterCreatureList()
{}

// 0x6E3F90
bool WaterCreatureManager_c::Init() {
    for (auto i = NUM_WATER_CREATURES; i --> 0;) {
        m_waterCreaturePool.AddItem(&m_waterCreatureItems[i]);
    }

    m_lastUpdateTime = 0;
    return true;
}

// 0x6E3FD0
void WaterCreatureManager_c::Exit() {
    auto* creature = m_waterCreatureList.GetHead();
    while (creature) {
        auto* nextCreature = m_waterCreatureList.GetNext(creature);
        creature->Exit();
        creature = nextCreature;
    }

    m_waterCreaturePool.RemoveAll();
}

// 0x6E4040
int32 WaterCreatureManager_c::GetRandomWaterCreatureId() {
    auto randNum = CGeneral::GetRandomNumberInRange(0, 100);
    if (randNum < 80) {
        return CGeneral::GetRandomNumberInRange(0, 3); // Small fish
    }

    if (randNum < 90) {
        return CGeneral::GetRandomNumberInRange(3, 5); // Jelly fish
    }

    if (randNum < 97) {
        return eWaterCreatureType::TURTLE;
    }

    return eWaterCreatureType::DOLPHIN;
}

// 0x6E40E0
int32 WaterCreatureManager_c::TryToFreeUpWaterCreatures(int32 numToFree) {
    auto* creature = m_waterCreatureList.GetHead();
    int32 numFreed = 0;
    while (numFreed < numToFree && creature) {
        auto* nextCreature = m_waterCreatureList.GetNext(creature);
        if (creature->m_pObject->m_bOffscreen && creature->m_pFollowedCreature) {
            creature->Exit();
            ++numFreed;
        }
        creature = nextCreature;
    }
    return numFreed;
}

// 0x6E4510
bool WaterCreatureManager_c::CanAddWaterCreatureAtPos(int32 id, CVector pos) {
    auto* creature = m_waterCreatureList.GetHead();
    if (!creature) {
        return true;
    }

    const auto& creatureInfo = GetCreatureInfo(id); // Originally not assigned by reference, but copied to stack
    while (creature) {
        if (creature->m_nCreatureType == id && !creature->m_pFollowedCreature) {
            const auto& objPos = creature->GetObject()->GetPosition();
            if (DistanceBetweenPointsSquared(objPos, pos) <= creatureInfo.m_fMinDistFromSameCreature) {
                return false;
            }
        }
        creature = m_waterCreatureList.GetNext(creature);
    }
    return true;
}

// 0x6E45B0
void WaterCreatureManager_c::TryToExitGroup(WaterCreature_c* waterCreature) {
    auto* leader = waterCreature->m_pFollowedCreature ? waterCreature->m_pFollowedCreature : waterCreature;
    int32 numCreaturesInGroup = 0;
    WaterCreature_c* creaturesInGroup[32];
    auto* creature = m_waterCreatureList.GetHead();
    while (creature) {
        if (creature == leader || creature->m_pFollowedCreature == leader) {
            creaturesInGroup[numCreaturesInGroup] = creature;
            ++numCreaturesInGroup;
        }

        creature = m_waterCreatureList.GetNext(creature);
    }

    const auto& camPos = TheCamera.GetPosition();
    for (int32 i = 0; i < numCreaturesInGroup; ++i) {
        if (DistanceBetweenPointsSquared(camPos, creaturesInGroup[i]->GetObject()->GetPosition()) < ms_fMaxWaterCreaturesDrawDistanceSquared) {
            return; // Jump out of function if any of the creatures in group aren't out of camera reach.
                    // All fishes in group have to be destroyed at once as no to leave any of them with dangling pointers
        }
    }

    for (int32 i = 0; i < numCreaturesInGroup; ++i) {
        creaturesInGroup[i]->m_bShouldBeDeleted = true;
    }
}

// 0x6E4F10
void WaterCreatureManager_c::Update(float deltaTime) {
    ZoneScoped;

    if (FindPlayerPed(0)->GetPlayerData()->m_nWaterCoverPerc > 50) {
        const auto currTime = CTimer::GetTimeInMS();

        if (currTime - m_lastUpdateTime > 1'000 && m_waterCreaturePool.GetNumItems() > 0) {
            m_lastUpdateTime = currTime;

            CVector direction;
            direction.x = CGeneral::GetRandomNumberInRange(-1.0F, 1.0F);
            direction.y = CGeneral::GetRandomNumberInRange(-1.0F, 1.0F);
            direction.z = 0.0F;
            direction.Normalise();

            const auto dist = CGeneral::GetRandomNumberInRange(25.0F, 40.0F);

            auto creaturePos = TheCamera.GetPosition() + direction * dist;

            float waterZ, waterDepth;
            if (!TheCamera.IsSphereVisible(creaturePos, 3.0F)
                && CWaterLevel::GetWaterDepth(creaturePos, &waterDepth, &waterZ, nullptr)
                && waterDepth > 4.5F) {
                const auto id = GetRandomWaterCreatureId();

                if (CanAddWaterCreatureAtPos(id, creaturePos)) {
                    const auto& creatureInfo = GetCreatureInfo(id);

                    uint32 randNumToCreate = CGeneral::GetRandomNumberInRange(creatureInfo.m_nMinCreated, creatureInfo.m_nMaxCreated);
                    uint32 numToCreate = std::min(randNumToCreate, m_waterCreaturePool.GetNumItems());

                    WaterCreature_c* leader = nullptr;

                    for (uint32 i = 0; i < numToCreate; ++i) {
                        auto* creature = m_waterCreaturePool.RemoveHead();

                        if (creature->Init(id, &creaturePos, leader, waterZ, waterDepth)) {
                            m_waterCreatureList.AddItem(creature);
                        } else {
                            m_waterCreaturePool.AddItem(creature);
                        }

                        if (!leader) {
                            leader = creature;
                        }
                    }
                }
            }
        }
    }

    // Trigger updates
    auto* creature = m_waterCreatureList.GetHead();
    while (creature) {
        auto* nextCreature = m_waterCreatureList.GetNext(creature);
        creature->Update(deltaTime);
        creature = nextCreature;
    }

    // Remove creatures queued for removal
    creature = m_waterCreatureList.GetHead();
    while (creature) {
        auto* nextCreature = m_waterCreatureList.GetNext(creature);
        if (creature->m_bShouldBeDeleted) {
            creature->Exit();
        }
        creature = nextCreature;
    }
}
