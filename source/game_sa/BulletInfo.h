/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Base.h"

#include "Vector.h"
#include "eWeaponType.h"

class CEntity;

class CBulletInfo {
public:
    eWeaponType m_nWeaponType;
    CEntity*    m_pCreator;
    float       m_nDestroyTime;
    bool        m_bExists;
    CVector     m_vecPosition;
    CVector     m_vecVelocity;
    int16       m_nDamage;

    static constexpr auto MAX_BULLET_INFOS{8u};

    static inline auto& aBulletInfos = StaticRef<std::array<CBulletInfo, 8>>(0xC88740);
    static inline auto& PlayerSniperBulletStart = StaticRef<CVector>(0xC888A0);
    static inline auto& PlayerSniperBulletEnd = StaticRef<CVector>(0xC888AC);

public:
    static void InjectHooks();

    static void Initialise();
    static void Shutdown();
    static void AddBullet(CEntity* creator, eWeaponType weaponType, CVector position, CVector velocity);
    static void Update();

    // NOTSA funcs:
private:
    static CBulletInfo* GetFree();
    bool IsTimeToBeDestroyed() const noexcept;
};

VALIDATE_SIZE(CBulletInfo, 0x2C);
