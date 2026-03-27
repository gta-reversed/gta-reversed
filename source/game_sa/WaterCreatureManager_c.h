#pragma once

#include "WaterCreature_c.h"

// WaterCreatureInfo_t
struct tWaterCreatureInfo {
    uint16* m_pModelId;

    uint8 m_nMinCreated;
    uint8 m_nMaxCreated;

    float m_fMinSpawnDepth;
    float m_fMinDistFromSameCreature;

    float m_fMinDistFromFollowed;
    float m_fMaxDistFromFollowed;

    float m_fMinSpeed;
    float m_fMaxSpeed;

    float m_fMaxHeadingChange;
    float m_fChanceToRandomizeRotation;

    float m_Speed;
};
VALIDATE_SIZE(tWaterCreatureInfo, 0x2C);

class WaterCreatureManager_c {
public:
    static constexpr int32 NUM_WATER_CREATURES = 128;
    static constexpr int32 NUM_WATER_CREATURE_INFOS = 7;

    static constexpr float ms_fMaxWaterCreaturesDrawDistance        = 60.0F;
    static constexpr float ms_fMaxWaterCreaturesDrawDistanceSquared = sq(ms_fMaxWaterCreaturesDrawDistance);

private:
    static inline auto& ms_waterCreatureInfos = StaticRef<tWaterCreatureInfo[NUM_WATER_CREATURE_INFOS]>(0x8D3698); // Access using GetCreatureInfo()

    WaterCreature_c m_waterCreatureItems[NUM_WATER_CREATURES];
    TList_c<WaterCreature_c> m_waterCreaturePool;
    TList_c<WaterCreature_c> m_waterCreatureList;

    int32 m_lastUpdateTime;

public:
    WaterCreatureManager_c();
    ~WaterCreatureManager_c();

    bool Init();
    void Exit();

    void Update(float deltaTime);

    int32 TryToFreeUpWaterCreatures(int32 numToFree);

private:
    int32 GetRandomWaterCreatureId(); // eWaterCreatureType
    bool CanAddWaterCreatureAtPos(int32 id, CVector pos);
    void TryToExitGroup(WaterCreature_c* waterCreature);

public: // NOTSA
    static const tWaterCreatureInfo& GetCreatureInfo(uint32/*eWaterCreatureType*/ type) { return ms_waterCreatureInfos[type]; }

private:
    friend WaterCreature_c;
    friend void InjectHooksMain();
    static void InjectHooks();
};

extern WaterCreatureManager_c& g_waterCreatureMan;
