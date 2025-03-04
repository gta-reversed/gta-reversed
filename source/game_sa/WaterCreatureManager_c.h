#pragma once

#include "WaterCreature_c.h"

struct WaterCreatureInfo {
    uint16* m_pModelId;
    uint8 m_nMinCreated;
    uint8 m_nMaxCreated;
    uint16 m_wUnkn;
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
VALIDATE_SIZE(WaterCreatureInfo, 0x2C);

class WaterCreatureManager_c {
public:
    WaterCreatureManager_c();

public:
    static constexpr int32 NUM_WATER_CREATURES = 128;
    WaterCreature_c m_aCreatures[NUM_WATER_CREATURES];
    TList_c<WaterCreature_c> m_freeList;
    TList_c<WaterCreature_c> m_createdList;
    int32 m_nLastCreationCheckTime;

public:
    static void InjectHooks();

    bool Init();
    void Exit();
    int32 GetRandomWaterCreatureId();
    void TryToFreeUpWaterCreatures(int32 numToFree);
    bool CanAddWaterCreatureAtPos(int32 nCreatureType, CVector vecPos);
    void TryToExitGroup(WaterCreature_c* pCreature);
    void Update(float fTimestep);

public:
    static const WaterCreatureInfo& GetCreatureInfo(int32 nType) { return ms_waterCreatureInfos[nType]; }

public:
    static constexpr int32 NUM_WATER_CREATURE_INFOS = 7;
    static WaterCreatureInfo(&ms_waterCreatureInfos)[NUM_WATER_CREATURE_INFOS]; // Access using GetCreatureInfo()
    static constexpr float ms_fMaxWaterCreaturesDrawDistance = 60.0F;
    static constexpr float ms_fMaxWaterCreaturesDrawDistanceSquared = ms_fMaxWaterCreaturesDrawDistance * ms_fMaxWaterCreaturesDrawDistance;
};

extern WaterCreatureManager_c& g_waterCreatureMan;
