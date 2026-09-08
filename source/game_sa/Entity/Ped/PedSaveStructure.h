#pragma once

#include "Vector.h"
#include "Weapon.h"
#include "Ped.h"

class CPedSaveStructure {
public:
    static void InjectHooks();

public:
    CVector       m_pos;
    float         m_health;
    float         m_armor;
    CWeapon       m_weapons[13];
    ePedCreatedBy m_createdBy;
    int8          m_activeWeaponSlot;
    int8          m_areaCode;
    int32         m_nExitIndex;
    uint8         m_nFightingStyle;
    uint8         m_nAllowedAttackMoves;

    void Extract(CPed* ped);
    void Construct(CPed* ped);
};
VALIDATE_SIZE(CPedSaveStructure, 0x18C);
