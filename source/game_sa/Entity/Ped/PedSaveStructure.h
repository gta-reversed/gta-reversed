#pragma once

#include "Vector.h"
#include "Weapon.h"
#include "Ped.h"

class CPedSaveStructure {
public:
    void Construct(CPed* ped);
    void Extract(CPed* ped);

    static void InjectHooks();

private:
    CVector                               m_Pos{};
    float                                 m_Health{};
    float                                 m_Armour{};
    std::array<CWeapon, NUM_WEAPON_SLOTS> m_Weapons{};
    ePedCreatedBy                         m_CreatedBy{};
    int8                                  m_ActiveWeaponSlot{};
    eAreaCodesS8                          m_AreaCode{};
    int32                                 m_EnexIndex{}; //!< Index of the ped's enex in the enex pool, -1 if none
    eFightingStyle                        m_FightingStyle{};
    uint8                                 m_AllowedAttackMoves{};
};
VALIDATE_SIZE(CPedSaveStructure, 0x18C);
