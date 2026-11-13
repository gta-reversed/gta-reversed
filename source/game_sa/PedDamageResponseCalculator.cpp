#include "StdInc.h"

#include <reversiblebugfixes/Bugs.hpp>
#include "PedDamageResponseCalculator.h"
#include "TaskSimpleFight.h"
#include "PedStats.h"

void CPedDamageResponseCalculator::InjectHooks() {
    RH_ScopedClass(CPedDamageResponseCalculator);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Constructor, 0x4AD3F0);
    RH_ScopedInstall(AccountForPedDamageStats, 0x4AD430);
    RH_ScopedInstall(AccountForPedArmour, 0x4AD550);
    RH_ScopedInstall(ComputeWillForceDeath, 0x4AD610);
    RH_ScopedInstall(ComputeWillKillPed, 0x4B3210);
    RH_ScopedInstall(ComputeDamageResponse, 0x4B5AC0);
}

// 0x4AD3F0
CPedDamageResponseCalculator::CPedDamageResponseCalculator(const CEntity* entity, float fDamage, eWeaponType weaponType, ePedPieceTypes bodyPart, bool bSpeak) {
    m_pDamager      = entity;
    m_fDamageFactor = fDamage;
    m_bodyPart      = bodyPart;
    m_weaponType    = weaponType;
    m_bSpeak        = bSpeak;
}

CPedDamageResponseCalculator* CPedDamageResponseCalculator::Constructor(CEntity* entity, float fDamage, eWeaponType weaponType, ePedPieceTypes bodyPart, bool bSpeak) {
    this->CPedDamageResponseCalculator::CPedDamageResponseCalculator(entity, fDamage, weaponType, bodyPart, bSpeak);
    return this;
}

/*!
 *
 * @param ped
 * @param response
 * @addr 0x4AD430
 */
void CPedDamageResponseCalculator::AccountForPedDamageStats(CPed* ped, CPedDamageResponse& response) {
    const auto isStealthKill = m_fDamageFactor == ms_damageFactor;

    m_fDamageFactor *= ped->IsPlayer()
        ? 0.33f
        : ped->m_pStats->m_fDefendWeakness;

    const auto* const damagerPed = m_pDamager && m_pDamager->GetIsTypePed()
        ? m_pDamager->AsPed()
        : nullptr;
    const auto isExplosiveOrFire = notsa::contains({ WEAPON_EXPLOSION, WEAPON_FLAMETHROWER }, m_weaponType);

    if (damagerPed && damagerPed->IsPlayer()) {
        if (!CPedGroups::AreInSameGroup(ped, damagerPed) || isExplosiveOrFire || isStealthKill) {
            return;
        }
    } else if (!ped->IsPlayer() || !damagerPed || !CPedIntelligence::AreFriends(*ped, *damagerPed)) {
        if (!damagerPed || !ped->IsCreatedBy(PED_MISSION) || !CPedGroups::AreInSameGroup(ped, damagerPed) || isExplosiveOrFire) {
            return;
        }
    }

    m_fDamageFactor = std::min(12.5f, m_fDamageFactor * 0.1f);
}

/*!
 *
 * @param ped
 * @param response
 * @addr 0x4AD550
 */
void CPedDamageResponseCalculator::AccountForPedArmour(CPed* ped, CPedDamageResponse& response) {
    if (ped->m_fArmour == 0.0f)
        return;

    if (m_weaponType == WEAPON_DROWNING || m_weaponType == WEAPON_FALL)
        return;

    if (FindPlayerPed() == ped)
        CWorld::Players[CWorld::PlayerInFocus].m_nLastTimeArmourLost = CTimer::GetTimeInMS();

    if (m_fDamageFactor > ped->m_fArmour) {
        m_fDamageFactor = m_fDamageFactor - ped->m_fArmour;
        response.m_fDamageArmor = ped->m_fArmour;
        ped->m_fArmour = 0.0f;
    } else {
        response.m_fDamageArmor = m_fDamageFactor;
        ped->m_fArmour = ped->m_fArmour - m_fDamageFactor;
        m_fDamageFactor = 0.0f;
    }
}

/*!
 *
 * @param ped
 * @addr Added in Android
 */
void CPedDamageResponseCalculator::AdjustPedDamage(CPed* ped) {
    // TODO: Reverted function from GTA SA 2.10 version ABI : arm64-v8a (x64)
    // offset: 0x444370

    if (!ped) {
        return;
    }

    const auto plyr = FindPlayerPed(-1);
    if (!plyr) {
        return;
    }

    const bool isUnarmedBrassOrParachute = m_weaponType == WEAPON_UNARMED
        || m_weaponType == WEAPON_BRASSKNUCKLE
        || m_weaponType == WEAPON_PARACHUTE;

    if (CCheat::m_aCheatsActive[CHEAT_COUNTRY_TRAFFIC] && m_weaponType <= eWeaponType::WEAPON_PARACHUTE && isUnarmedBrassOrParachute) {
        m_fDamageFactor = ped->m_fHealth;
    }

    if (plyr == ped && CTheScripts::pActiveScripts && !strcmp(CTheScripts::pActiveScripts->m_szName, "intro1")) {
        m_fDamageFactor = m_fDamageFactor * 0.77f;
    }
}

/*!
 *
 * @param ped
 * @param response
 * @return
 * @addr 0x4AD610
 */
bool CPedDamageResponseCalculator::ComputeWillForceDeath(CPed* ped, CPedDamageResponse& response) {
    if (ped->bNoCriticalHits) {
        return false;
    }

    const auto* const damagerPed = m_pDamager && m_pDamager->GetIsTypePed()
        ? m_pDamager->AsPed()
        : nullptr;

    switch (m_weaponType) {
    case WEAPON_KATANA: {
        if (!damagerPed || !damagerPed->IsPlayer()) {
            return false;
        }
        const auto* const fight = damagerPed->GetIntelligence()->GetTaskFighting();
        if (!fight || fight->m_nComboSet != MELEE_COMBO_SWORD) {
            return false;
        }
        if (fight->m_nCurrentMove == FIGHT_ATTACK_HIT_3 && CGeneral::GetRandomNumber() % 2) {
            return true;
        }
        return CGeneral::GetRandomNumber() % 8 == 0;
    }
    case WEAPON_PISTOL:
    case WEAPON_PISTOL_SILENCED:
    case WEAPON_DESERT_EAGLE:
    case WEAPON_SHOTGUN:
    case WEAPON_SAWNOFF_SHOTGUN:
    case WEAPON_SPAS12_SHOTGUN:
    case WEAPON_MICRO_UZI:
    case WEAPON_MP5:
    case WEAPON_AK47:
    case WEAPON_M4:
    case WEAPON_TEC9:
    case WEAPON_COUNTRYRIFLE:
    case WEAPON_SNIPERRIFLE:
    case WEAPON_MINIGUN:
    case WEAPON_UZI_DRIVEBY: {
        const auto canSurviveHeadshot = ped->IsPlayer() || ped->bNoCriticalHits
            || (!notsa::contains({ WEAPON_SNIPERRIFLE, WEAPON_COUNTRYRIFLE }, m_weaponType) && CGeneral::GetRandomNumber() % 8 != 0);

        if (ped->IsInVehicle() && !ped->IsPlayer() && !ped->bNoCriticalHits) {
            return m_bodyPart == PED_PIECE_HEAD;
        }
        if (!ped->IsPlayer() && damagerPed && damagerPed->IsPlayer()) {
            const auto* const playerData = damagerPed->GetPlayerData();
            if (playerData->m_nTargetBone == BONE_HEAD || playerData->m_bFreeAiming) {
                return m_bodyPart == PED_PIECE_HEAD;
            }
        }
        return !canSurviveHeadshot && m_bodyPart == PED_PIECE_HEAD;
    }
    case WEAPON_GRENADE:
    case WEAPON_RLAUNCHER:
    case WEAPON_RLAUNCHER_HS:
    case WEAPON_REMOTE_SATCHEL_CHARGE:
    case WEAPON_EXPLOSION:
        return CLocalisation::KillPeds() && !ped->IsPlayer() && !ped->bInVehicle && m_fDamageFactor + 1.0f > ped->m_fHealth;
    default:
        return false;
    }
}

/*!
 *
 * @param ped
 * @param response
 * @param bSpeak
 * @addr 0x4B3210
 */
void CPedDamageResponseCalculator::ComputeWillKillPed(CPed* ped, CPedDamageResponse& rsp, bool bSpeak) {
    if (ped->IsPlayer()) {
        if (CCheat::IsActive(CHEAT_INFINITE_HEALTH) && m_weaponType < WEAPON_LAST_WEAPON) {
            return;
        }
    }

    rsp.m_bForceDeath = ComputeWillForceDeath(ped, rsp);

    if (CCheat::IsActive(CHEAT_MEGA_PUNCH)) {
        if (notsa::contains({ WEAPON_UNARMED, WEAPON_BRASSKNUCKLE, WEAPON_PARACHUTE }, m_weaponType)) {
            m_fDamageFactor = ped->m_fHealth;
        }
    }

    if (!rsp.m_bForceDeath && m_weaponType == WEAPON_FALL && m_bSpeak) {
        const auto prevHP   = ped->m_fHealth;
        ped->m_fHealth      = std::max(prevHP - m_fDamageFactor, 5.0f);
        rsp.m_bHealthZero   = false;
        rsp.m_fDamageHealth = prevHP - ped->m_fHealth;
        if (bSpeak) {
            ped->Say(CTX_GLOBAL_PAIN_LOW, 0, 1.0, 0, 0, 0);
        }
        return;
    }

    if (rsp.m_bForceDeath || ped->m_fHealth - m_fDamageFactor < 1.0f) {
        rsp.m_fDamageHealth = ped->m_fHealth;
        rsp.m_bHealthZero   = true;
        ped->m_fHealth      = 0.0f;
        return;
    }

    rsp.m_bHealthZero   = false;
    rsp.m_fDamageHealth = m_fDamageFactor;
    ped->m_fHealth      = ped->m_fHealth - m_fDamageFactor;

    if (bSpeak) {
        ped->Say(
            m_weaponType == WEAPON_DROWNING
                ? CTX_GLOBAL_PAIN_CJ_DROWNING
                : m_fDamageFactor < 5.0f && ped->m_fHealth > 10.0f
                    ? CTX_GLOBAL_PAIN_LOW
                    : CTX_GLOBAL_PAIN_HIGH
        );
    }
}

/*!
 *
 * @param ped
 * @return
 * @addr 0x4B5C2A inlined
 */
bool CPedDamageResponseCalculator::IsBleedingWeapon(CPed* ped) const {
    if (ped->IsPlayer())
        return false;

    if (m_weaponType == WEAPON_KNIFE || m_weaponType == WEAPON_KATANA || m_weaponType == WEAPON_CHAINSAW)
        return true;

    return false;
}

/*!
 *
 * @param ped
 * @param response
 * @param bSpeak
 * @addr 0x4B5AC0
 */
void CPedDamageResponseCalculator::ComputeDamageResponse(CPed* ped, CPedDamageResponse& response, bool bSpeak) {
    if (response.m_bDamageCalculated) {
        return;
    }
    response                      = {};
    response.m_bDamageCalculated  = true;
    response.m_bCheckIfAffectsPed = true;

    AccountForPedDamageStats(ped, response);
    AccountForPedArmour(ped, response);
    ComputeWillKillPed(ped, response, bSpeak);

    if (m_pDamager && (m_pDamager == FindPlayerPed() || m_pDamager == FindPlayerVehicle())) {
        if (m_pDamager != ped && response.GetTotalDamage() > 3.0f) {
            FindPlayerInfo().m_nHavocCaused++;
        }
    }

    if (ped == FindPlayerPed()) {
        FindPlayerPed()->AnnoyPlayerPed(false);
    }

    if (ped->IsPlayer() && m_pDamager && m_pDamager->GetIsTypePed()) {
        CEventDamage event{ const_cast<CEntity*>(m_pDamager), 0, m_weaponType, m_bodyPart, 0, false, ped->bInVehicle };
        ped->AsPlayer()->MakeGroupRespondToPlayerTakingDamage(event);
    }

    if (response.GetTotalDamage() <= 0.0f) {
        return;
    }

    if (IsBleedingWeapon(ped)) {
        ped->field_72F = 200;
    }

    if (ped == FindPlayerPed()) {
        FindPlayerInfo().m_nLastTimeEnergyLost = CTimer::GetTimeInMS();
    }

    ped->m_nLastWeaponDamage = (char)m_weaponType;
    if (m_pDamager) {
        CEntity::SetEntityReference(ped->m_pLastEntityDamage, const_cast<CEntity*>(m_pDamager));
        ped->field_768 = CTimer::GetTimeInMS();

        // NOTE: Here the original calls `IsMember(ped)` on the player's group if the damager is the player, then ignores the result
    }
}

