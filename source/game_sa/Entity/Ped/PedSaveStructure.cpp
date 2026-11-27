#include "StdInc.h"
#include "PedSaveStructure.h"
#include "EntryExitManager.h"

// 0x5D43D0
void CPedSaveStructure::Construct(CPed* ped) {
    // NOTE: Raw `CPed` members where it has no getter (health, active weapon slot, fighting style, attack moves, enex)
    m_Pos                = ped->GetPosition();
    m_Health             = ped->m_fHealth;
    m_Armour             = ped->m_fArmour;
    m_CreatedBy          = (ePedCreatedBy)(ped->GetCreatedBy());
    m_ActiveWeaponSlot   = (int8)(ped->m_nActiveWeaponSlot);
    m_AreaCode           = ped->GetAreaCode();
    m_FightingStyle      = ped->m_nFightingStyle;
    m_AllowedAttackMoves = (uint8)(ped->m_nAllowedAttackMoves);

    // NOTE: The area is checked on the linked enex (if any), but the index saved is of the ped's own enex
    const auto* const enex = ped->m_pEnex;
    m_EnexIndex            = enex && enex->GetMyOrLinkedArea() != AREA_CODE_NORMAL_WORLD
        ? (int32)(CEntryExitManager::GetPool()->GetIndex(enex))
        : -1;

    m_Weapons              = ped->m_aWeapons;
}

// 0x5D44B0
void CPedSaveStructure::Extract(CPed* ped) {
    // NOTE: Raw `CPed` members where it has no setter
    ped->GetPosition() = m_Pos;
    ped->m_fHealth     = m_Health;
    ped->SetArmour(m_Armour);
    // The saved slot, weapon types and enex index are used unchecked by the original
    const auto activeSlot    = notsa::bugfixes::GenericOOB && (m_ActiveWeaponSlot < 0 || m_ActiveWeaponSlot >= (int8)(NUM_WEAPON_SLOTS))
        ? (int8)(0)
        : m_ActiveWeaponSlot;
    ped->m_nActiveWeaponSlot = (uint8)(activeSlot);
    ped->SetCharCreatedBy(m_CreatedBy);
    ped->m_nFightingStyle      = m_FightingStyle;
    ped->m_nAllowedAttackMoves = (char)(m_AllowedAttackMoves);

    for (auto&& [slot, weapon] : rngv::enumerate(m_Weapons)) {
        if (weapon.GetType() == WEAPON_UNARMED) {
            continue;
        }
        if (notsa::bugfixes::GenericOOB && weapon.GetType() >= NUM_WEAPONS) {
            continue;
        }
        for (const auto model : weapon.GetWeaponInfo().GetModels()) {
            if (model != MODEL_INVALID) {
                CStreaming::RequestModel(model, STREAMING_KEEP_IN_MEMORY);
                CStreaming::LoadAllRequestedModels(false);
            }
        }
        ped->GiveWeapon(weapon, false);
        ped->GetWeaponInSlot(slot).m_AmmoInClip = weapon.GetAmmoInClip();
    }
    ped->SetCurrentWeapon(activeSlot);
    ped->SetAreaCode(m_AreaCode);
    if (notsa::bugfixes::GenericOOB) {
        auto* const pool = CEntryExitManager::GetPool();
        ped->m_pEnex     = pool->IsIndexInBounds(m_EnexIndex)
            ? pool->GetAt(m_EnexIndex)
            : nullptr;
    } else {
        ped->m_pEnex = m_EnexIndex != -1
            ? CEntryExitManager::GetPool()->GetAt(m_EnexIndex)
            : nullptr;
    }
}

void CPedSaveStructure::InjectHooks() {
    RH_ScopedClass(CPedSaveStructure);
    RH_ScopedCategory("Entity/Ped");

    RH_ScopedInstall(Construct, 0x5D43D0);
    RH_ScopedInstall(Extract, 0x5D44B0);
}
