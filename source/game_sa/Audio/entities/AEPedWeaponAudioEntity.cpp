#include "StdInc.h"

#include "AEPedWeaponAudioEntity.h"

// 0x5DE990
CAEPedWeaponAudioEntity::CAEPedWeaponAudioEntity() : CAEWeaponAudioEntity() {
    m_Ped = nullptr;
    m_bInitialised = false;
}

// 0x4E6AA0
void CAEPedWeaponAudioEntity::Initialise(CPed* ped) {
    m_Ped = ped;
    CAEWeaponAudioEntity::Initialise();
    m_bInitialised = true;
}

// 0x4E6AC0
void CAEPedWeaponAudioEntity::Terminate() {
    m_bInitialised = false;
    m_Ped = nullptr;
    CAEWeaponAudioEntity::Terminate();
}

// 0x4E69F0
void CAEPedWeaponAudioEntity::AddAudioEvent(eAudioEvents event) {
    if (!m_bInitialised || !m_Ped)
        return;

    const auto& weapon = m_Ped->GetActiveWeapon();
    if (weapon.m_Type == WEAPON_UNARMED)
        return;

    switch (event) {
    case AE_WEAPON_FIRE:
    case AE_WEAPON_FIRE_MINIGUN_AMMO:
    case AE_WEAPON_FIRE_MINIGUN_NO_AMMO:
        WeaponFire(weapon.m_Type, m_Ped, event);
        break;
    case AE_WEAPON_RELOAD_A:
    case AE_WEAPON_RELOAD_B:
        WeaponReload(weapon.m_Type, m_Ped, event);
        break;
    case AE_WEAPON_CHAINSAW_IDLE:
    case AE_WEAPON_CHAINSAW_ACTIVE:
    case AE_WEAPON_CHAINSAW_CUTTING:
        ReportChainsawEvent(m_Ped, event);
        break;
    case AE_WEAPON_STEALTH_KILL:
        ReportStealthKill(weapon.m_Type, m_Ped, event);
        break;
    default:
        return;
    }
}

// 0x4E6AE0
void CAEPedWeaponAudioEntity::Service() {
    if (  !m_Ped
        || m_Ped->bCollidedWithMyVehicle
        || m_Ped->GetIntelligence()->GetTaskSwim()
        || m_Ped->GetActiveWeapon().m_Type != WEAPON_FLAMETHROWER
    ) {
        StopFlameThrowerIdleGasLoop();
    } else {
        PlayFlameThrowerIdleGasLoop(m_Ped);
    }
}
