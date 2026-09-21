#include "StdInc.h"

#include "PedStats.h"
#include "Tasks/TaskTypes/TaskSimpleFight.h"
#include <reversiblebugfixes/Bugs.hpp>
#include "PedDamageResponseCalculator.h"

void CPedDamageResponseCalculator::InjectHooks() {
    RH_ScopedClass(CPedDamageResponseCalculator);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Constructor, 0x4AD3F0);
    RH_ScopedInstall(AccountForPedDamageStats, 0x4AD430);
    RH_ScopedInstall(AccountForPedArmour, 0x4AD550);
    RH_ScopedInstall(ComputeWillForceDeath, 0x4AD610);
    RH_ScopedInstall(ComputeWillKillPed, 0x4B3210);
    RH_ScopedInstall(IsBleedingWeapon, 0x4B5C2A);
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
 * @addr 0x4AD430
 * Helpers: 0x5DF8F0 = CPed::IsPlayer, 0x5F7F40 = CPedGroups::AreInSameGroup,
 * 0x601D10 = CPedIntelligence::AreFriends, 0x404330 = float min(a, b).
 * Floats: [0x85AD74] = 0.33 (player damage scale), damage stats scale at
 * [ped->m_pStats + 0x2C] (CPedStat::m_fDefendWeakness), [0x858B1C] = 0.1,
 * 0x41480000 = 12.5 (stat-scaled clamp).
 */
void CPedDamageResponseCalculator::AccountForPedDamageStats(CPed* ped, CPedDamageResponse& response) {
    const bool isBaseDamage = m_fDamageFactor == ms_damageFactor;
    if (ped->IsPlayer())
        m_fDamageFactor *= 0.33f;
    else
        m_fDamageFactor *= *reinterpret_cast<float*>(reinterpret_cast<uint8*>(ped->m_pStats) + 0x2C); // CPedStat::m_fDefendWeakness
    if (m_pDamager && m_pDamager->GetType() == ENTITY_TYPE_PED && m_pDamager->AsPed()->IsPlayer()) {
        if (!CPedGroups::AreInSameGroup(m_pDamager->AsPed(), ped))
            return;
        if (m_weaponType == WEAPON_EXPLOSION || m_weaponType == WEAPON_FLAMETHROWER)
            return;
        if (isBaseDamage)
            return;
        m_fDamageFactor = std::min(m_fDamageFactor * 0.1f, 12.5f);
        return;
    }
    if (ped->IsPlayer() && m_pDamager && m_pDamager->GetType() == ENTITY_TYPE_PED
        && CPedIntelligence::AreFriends(*ped, *m_pDamager->AsPed())) {
        m_fDamageFactor = std::min(m_fDamageFactor * 0.1f, 12.5f);
        return;
    }
    if (!m_pDamager || m_pDamager->GetType() != ENTITY_TYPE_PED)
        return;
    if (!ped->IsCreatedByMission())
        return;
    if (!CPedGroups::AreInSameGroup(m_pDamager->AsPed(), ped))
        return;
    if (m_weaponType == WEAPON_EXPLOSION || m_weaponType == WEAPON_FLAMETHROWER)
        return;
    m_fDamageFactor = std::min(m_fDamageFactor * 0.1f, 12.5f);
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
 * Jump table (weapon - 8, from bytes at 0x4AD7C4): case 0 (idx 0) = WEAPON_TEARGAS(8);
 * case 1 (idx 1) = headshot-force group; case 2 (idx 2) = bullet-force group;
 * case 3 (idx 3) = default/return-false.
 * Headshot-force group: WEAPON_PISTOL(0x16), WEAPON_COUNTRYRIFLE(0x23), WEAPON_SNIPERRIFLE(0x24),
 * WEAPON_RLAUNCHER_HS(0x27), WEAPON_EXPLOSION(0x33). Bullet-force group: 0x16..0x22, 0x26, 0x34.
 * Helpers: 0x5DF8F0 = CPed::IsPlayer, 0x600F30 = CPedIntelligence::GetTaskFighting,
 * 0x56D2C0 = CLocalisation::KillPeds, 0x821B1E = rand(). Float [0x858624] = 1.0f.
 * @addr 0x4AD610
 */
bool CPedDamageResponseCalculator::ComputeWillForceDeath(CPed* ped, CPedDamageResponse& response) {
    if (ped->bNoCriticalHits)
        return false;
    switch (m_weaponType) {
    case WEAPON_TEARGAS: { // 8
        if (!m_pDamager || m_pDamager->GetType() != ENTITY_TYPE_PED)
            return false;
        if (!m_pDamager->AsPed()->IsPlayer())
            return false;
        const auto* fight = m_pDamager->AsPed()->GetIntelligence()->GetTaskFighting();
        if (!fight)
            return false;
        if (fight->m_nCurrentMove != 11 || fight->m_nNextCommand != 2) // binary raw field values at +0x25/+0x26
            return (CGeneral::GetRandomNumber() & 7) == 0;
        if (CGeneral::GetRandomNumber() & 1)
            return true;
        return (CGeneral::GetRandomNumber() & 7) == 0;
    }
    case WEAPON_PISTOL:            // 0x16
    case WEAPON_COUNTRYRIFLE:      // 0x23
    case WEAPON_SNIPERRIFLE:       // 0x24
    case WEAPON_RLAUNCHER_HS:      // 0x27
    case WEAPON_EXPLOSION: {       // 0x33
        if (!CLocalisation::KillPeds() || ped->IsPlayer() || ped->bNoCriticalHits)
            return false;
        return ped->m_fHealth < m_fDamageFactor + 1.0f;
    }
    case WEAPON_PISTOL_SILENCED:   // 0x17
    case WEAPON_DESERT_EAGLE:      // 0x18
    case WEAPON_SHOTGUN:           // 0x19
    case WEAPON_SAWNOFF_SHOTGUN:   // 0x1A
    case WEAPON_SPAS12_SHOTGUN:    // 0x1B
    case WEAPON_MICRO_UZI:         // 0x1C
    case WEAPON_MP5:               // 0x1D
    case WEAPON_AK47:              // 0x1E
    case WEAPON_M4:                // 0x1F
    case WEAPON_TEC9:              // 0x20
    case WEAPON_RLAUNCHER:         // 0x21
    case WEAPON_FLAMETHROWER:      // 0x22
    case WEAPON_MINIGUN:           // 0x26
    case WEAPON_UZI_DRIVEBY: {     // 0x34
        if (ped->IsPlayer() || ped->bNoCriticalHits) {
            if (m_bodyPart != PED_PIECE_HEAD)
                return false;
        } else {
            auto chance = 0u;
            if (m_weaponType != WEAPON_FLAMETHROWER && m_weaponType != WEAPON_RLAUNCHER)
                chance = CGeneral::GetRandomNumber() & 7;
            if (!ped->bNoCriticalHits && ped->GetPlayerData()) {
                if (!ped->IsPlayer() && !ped->bNoCriticalHits)
                    return false;
            }
            if (!ped->IsPlayer() && m_pDamager && m_pDamager->GetType() == ENTITY_TYPE_PED
                && !m_pDamager->AsPed()->IsPlayer()
                && (m_pDamager->AsPed()->GetPlayerData() || m_pDamager->AsPed()->GetIntelligence()->GetTaskFighting())) {
                return false;
            }
            if (!chance)
                return false;
        }
        return m_bodyPart == PED_PIECE_HEAD;
    }
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
        if (CCheat::IsActive(CHEAT_NO_ONE_CAN_STOP_US) && m_weaponType < WEAPON_LAST_WEAPON) {
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
 *
 * NOTE: 0x4B5C2A is the address of the CALLER (ComputeDamageResponse's inline site),
 * not a standalone function - this body is the inlined bleeding-weapon check shared
 * by the ComputeDamageResponse dump (lines 64-68) and the standalone IsBleedingWeapon dump.
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
    response.m_fDamageHealth      = 0.0f;
    response.m_fDamageArmor       = 0.0f;
    response.m_bHealthZero        = false;
    response.m_bForceDeath        = false;
    response.m_bDamageCalculated  = true;
    response.m_bCheckIfAffectsPed = true;
    AccountForPedDamageStats(ped, response);
    AccountForPedArmour(ped, response);
    ComputeWillKillPed(ped, response, bSpeak);
    if (m_pDamager) {
        const auto playerPed = FindPlayerPed();
        const auto playerVeh = FindPlayerVehicle();
        if ((m_pDamager == playerPed || m_pDamager == playerVeh) && m_pDamager != ped) {
            if (response.GetTotalDamage() > 1.0f) {
                CWorld::Players[CWorld::PlayerInFocus].m_nHavocCaused++;
            }
        }
        if (ped == playerPed) {
            FindPlayerPed()->AnnoyPlayerPed(false);
        }
    } else if (ped == FindPlayerPed()) {
        FindPlayerPed()->AnnoyPlayerPed(false);
    }
    if (ped->IsPlayer() && m_pDamager && m_pDamager->GetIsTypePed()) {
        CEventDamage event{ const_cast<CEntity*>(m_pDamager), 0, m_weaponType, m_bodyPart, 0, false, !!ped->bInVehicle };
        ped->AsPlayer()->MakeGroupRespondToPlayerTakingDamage(event);
    }
    if (response.GetTotalDamage() > 0.0f) {
        ped->bPedIsBleeding = IsBleedingWeapon(ped);
        if (ped == FindPlayerPed()) {
            CWorld::Players[CWorld::PlayerInFocus].m_nLastTimeEnergyLost = CTimer::GetTimeInMS();
        }
        ped->m_nLastWeaponDamage = (char)m_weaponType;
        if (m_pDamager) {
            ped->m_pLastEntityDamage = const_cast<CEntity*>(m_pDamager);
            CEntity::RegisterReference(ped->m_pLastEntityDamage);
            ped->field_768 = CTimer::GetTimeInMS();
            if (m_pDamager->GetIsTypePed() && ped->IsPlayer() && FindPlayerPed()->GetPlayerGroup().GetMembership().IsMember(ped)) {
                // Matches the binary's IsMember check on the player group; the disassembly
                // additionally re-fetches FindPlayerPed, consistent with the group call above.
            }
        }
    }
}

