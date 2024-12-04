/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Vector.h"
#include "eWeaponType.h"

class CEntity;

class CShotInfo {
public:
    int32    m_nWeaponType;
    CVector  m_vecOrigin;
    CVector  m_vecTargetOffset;
    float    m_fRange;
    CEntity* m_pCreator;
    float    m_DestroyTime;
    bool     m_bExist;
    bool     m_bExecuted;
    char     _pad2A[2];

    static float *ms_afRandTable; // static float ms_afRandTable[20]

public:
    static void InjectHooks();

    static void Initialise();
    static void Shutdown();
    static bool AddShot(CEntity* creator, eWeaponType weaponType, CVector origin, CVector target);
    static bool GetFlameThrowerShotPosn(uint8 shotId, CVector* outPos);
    static void Update();
};

VALIDATE_SIZE(CShotInfo, 0x2C);
