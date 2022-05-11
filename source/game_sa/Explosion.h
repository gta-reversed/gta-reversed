/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Vector.h"

class CAEExplosionAudioEntity;

enum eExplosionType : uint32 {
    EXPLOSION_GRENADE,
    EXPLOSION_MOLOTOV,
    EXPLOSION_ROCKET,
    EXPLOSION_WEAK_ROCKET,
    EXPLOSION_CAR,
    EXPLOSION_QUICK_CAR,
    EXPLOSION_BOAT,
    EXPLOSION_AIRCRAFT,
    EXPLOSION_MINE,
    EXPLOSION_OBJECT,
    EXPLOSION_TANK_FIRE,
    EXPLOSION_SMALL,
    EXPLOSION_RC_VEHICLE
};

class CEntity;

class CExplosion {
public:
    eExplosionType m_nType;
    CVector        m_vecPosition;
    float          m_fRadius{1.0f};
    float          m_fPropagationRate;
    CEntity*       m_pCreator;
    CEntity*       m_pVictim;
    float          m_nExpireTime;
    float          m_fDamagePercentage;
    uint8          m_nActiveCounter;
    bool           m_bMakeSound;
    float          m_nCreatedTime;
    uint32         m_nParticlesExpireTime;
    float          m_fVisibleDistance;
    float          m_fGroundZ;
    int32          m_nFuelTimer;
    CVector        m_vecFuelDirection[3];
    float          m_fFuelOffsetDistance[3];
    float          m_fFuelSpeed[3];

    static CAEExplosionAudioEntity& m_ExplosionAudioEntity;
    static CExplosion (&aExplosions)[16];

public:
    static void InjectHooks();

    static void Initialise();
    static void Shutdown();
    static void ClearAllExplosions();

    static int8 GetExplosionActiveCounter(uint8 id);
    static void ResetExplosionActiveCounter(uint8 id);
    static bool DoesExplosionMakeSound(uint8 id);
    static int32 GetExplosionType(uint8 id);
    static const CVector& GetExplosionPosition(uint8 id);

    static bool TestForExplosionInArea(eExplosionType type, float minX, float maxX, float minY, float maxY, float minZ, float maxZ);
    static void RemoveAllExplosionsInArea(CVector pos, float r);
    static void AddExplosion(CEntity* victim, CEntity* creator, eExplosionType type, CVector pos, uint32 lifetime, uint8 usesSound, float cameraShake, uint8 isVisible);
    static void Update();

private:
    // NOTSA functions:
    static CExplosion* GetFree();
    void SetCreator(CEntity* newCreator) noexcept;
    void SetVictim(CEntity* newVictim) noexcept;
};

VALIDATE_SIZE(CExplosion, 0x7C);
