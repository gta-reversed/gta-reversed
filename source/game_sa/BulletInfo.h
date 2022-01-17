/*
    Plugin-SDK (Grand Theft Auto San Andreas) header file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Vector.h"
#include "eWeaponType.h"
#include "Entity.h"

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

    static CBulletInfo(&aBulletInfos)[MAX_BULLET_INFOS];
    static CVector& PlayerSniperBulletStart;
    static CVector& PlayerSniperBulletEnd;

public:
    static void InjectHooks();

    static void Initialise();
    static void Shutdown();
    static void AddBullet(CEntity* creator, eWeaponType weaponType, CVector position, CVector velocity);
    static void Update();

private:
    // NOTSA funcs
    static CBulletInfo* GetFree();
    bool IsTimeToBeDestroyed() const noexcept { return (float)CTimer::GetTimeInMS() > m_nDestroyTime; }
};

VALIDATE_SIZE(CBulletInfo, 0x2C);
