/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "eWeaponType.h"
#include "Base.h"

class CProjectile;
class FxSystem_c;

constexpr uint32 MAX_PROJECTILES = 32;

class CProjectileInfo {
public:
    uint32      m_nWeaponType; // see eWeaponType
    CEntity*    m_pCreator;
    CEntity*    m_pVictim;
    int32       m_nDestroyTime;
    bool        m_bActive;
    char        _pad11[3];
    CVector     m_vecLastPosn;
    FxSystem_c* m_pFxSystem;

public:
    static inline auto& ms_apProjectile = StaticRef<std::array<CProjectile*, MAX_PROJECTILES>>(0xC89110);

    static void Initialise();
    static void Shutdown();
    void RemoveFXSystem(bool bInstantly);
    static CProjectileInfo* GetProjectileInfo(int32 infoId);
    static void RemoveNotAdd(CEntity* creator, eWeaponType weaponType, CVector posn);
    static bool AddProjectile(CEntity* creator, eWeaponType eWeaponType, CVector posn, float force, const CVector* direction, CEntity* victim);
    static void RemoveDetonatorProjectiles();
    static void RemoveProjectile(CProjectileInfo* projectileInfo, CProjectile* projectileObject);
    static void Update();
    static bool IsProjectileInRange(float x1, float x2, float y1, float y2, float z1, float z2, bool bDestroy);
    static void RemoveAllProjectiles();
    static bool RemoveIfThisIsAProjectile(CObject* object);

private:
    friend void InjectHooksMain();
    static void InjectHooks();
};
VALIDATE_SIZE(CProjectileInfo, 0x24);

constexpr uint32 MAX_PROJECTILE_INFOS = 32;
inline auto& gaProjectileInfo = StaticRef<std::array<CProjectileInfo, MAX_PROJECTILE_INFOS>>(0xC891A8);
