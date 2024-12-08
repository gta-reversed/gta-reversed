/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Fire.h"

class CEntity;

#define MAX_NUM_FIRES 60

class CFireManager {
public:
    std::array<CFire, MAX_NUM_FIRES> m_aFires;
    uint32 m_nMaxFireGenerationsAllowed;

public:
    CFireManager();
    ~CFireManager() = default; // 0x538BB0

    void Init();
    void Shutdown();

    uint32 GetNumOfNonScriptFires();
    CFire* FindNearestFire(const CVector& point, bool bCheckWasExtinguished = false, bool bCheckWasCreatedByScript = false);
    bool PlentyFiresAvailable();

    void ExtinguishPoint(CVector point, float fRadiusSq);
    bool ExtinguishPointWithWater(CVector point, float fRadiusSq, float fFireSize);

    bool IsScriptFireExtinguished(int16 id);

    void RemoveScriptFire(uint16 fireId);
    void RemoveAllScriptFires();
    void ClearAllScriptFireFlags();

    void SetScriptFireAudio(int16 fireId, bool bFlag);

    const CVector& GetScriptFireCoords(int16 fireId);
    uint32 GetNumFiresInRange(const CVector& point, float fRadiusSq);
    uint32 GetNumFiresInArea(float minX, float minY, float minZ, float maxX, float maxY, float maxZ);
    CFire* GetNextFreeFire(bool bMayExtinguish); // bAllowDeletingOldFire - allow deleting old fire if no free slots available

    void CreateAllFxSystems();
    void DestroyAllFxSystems();

    CFire* StartFire(CVector pos, float size, uint8 unused, CEntity* creator, uint32 nTimeToBurn, int8 nGenerations, uint8 unused_);
    CFire* StartFire(CEntity* target, CEntity* creator, _IGNORED_ float size = 0.8f, _IGNORED_ uint8 arg3 = 1, uint32 time = 7000, int8 numGenerations = 0);
    int32 StartScriptFire(const CVector& point, CEntity* target, _IGNORED_ float arg2, _IGNORED_ uint8 arg3, int8 numGenerations, int32 size);

    void Update();

    // NOTSA funcs
    uint32 GetNumOfFires();
    CFire& GetRandomFire();

    // NOTSA
    CFire& Get(size_t idx)
    {
        assert(m_aFires[idx].IsActive());
        return m_aFires[idx];
    }
    auto GetIndexOf(const CFire* fire) const { return std::distance(m_aFires.data(), fire); }
private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CFireManager* Constructor();
    CFireManager* Destructor();
};

VALIDATE_SIZE(CFireManager, 0x964);

inline static CFireManager& gFireManager = *reinterpret_cast<CFireManager*>(0xB71F80);
