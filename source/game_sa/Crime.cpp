#include "StdInc.h"

#include "Crime.h"
#include "PedType.h"
#include "eWantedLevel.h"

void CCrime::InjectHooks() {
    RH_ScopedClass(CCrime);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(FindImmediateDetectionRange, 0x531FC0);
    RH_ScopedInstall(ReportCrime, 0x532010);
}

// 0x531FC0
float CCrime::FindImmediateDetectionRange(eCrimeType CrimeType) {
    switch (CrimeType) {
    case CRIME_DESTROY_HELI:
    case CRIME_DESTROY_PLANE:
    case CRIME_EXPLOSION:
        return 60.0f;
    case CRIME_DESTROY_VEHICLE:
        return 30.0f;
    default:
        return 14.0f;
    }
}

// 0x532010
void CCrime::ReportCrime(eCrimeType crimeType, CEntity* pVictim, CPed* pCommitedby) {
// 0x532010
void CCrime::ReportCrime(eCrimeType crimeType, CEntity* pVictim, CPed* pCommitedby) {
    if (crimeType == CRIME_NONE) { // Moved here from 0x5320FC
        return;
    }
    if (!pCommitedby || !pCommitedby->IsPlayer()) {
        return;
    }

    const bool isPedCriminal = pVictim && pVictim->IsPed() && CPedType::PoliceDontCareAboutCrimesAgainstPedType(pVictim->AsPed()->m_nPedType);
    if (   crimeType == CRIME_DAMAGED_PED
        && pVictim
        && pVictim->IsPed()
        && IsPedPointerValid(pVictim->AsPed())
        && !pCommitedby->AsPlayer()->GetWantedLevel()
        && pVictim->AsPed()->bBeingChasedByPolice
    ) {
        if (!pVictim->AsPed()->IsStateDying()) {
            if (const auto text = TheText.Get("GOODBOY")) { // Good Citizen Bonus! +$50
                CMessages::AddBigMessage(text, 5'000, eMessageStyle::STYLE_MIDDLE);
            }
            if (pCommitedby->m_nPedType == PED_TYPE_PLAYER1) {
                pCommitedby->AsPlayer()->GetPlayerInfoForThisPlayerPed()->m_nMoney += 50;
            }
        }
        return;
    }

    const auto plyrPed = pCommitedby ? pCommitedby->AsPlayer() : nullptr;
    if (!plyrPed) {
        return;
    }

    const auto plyrWanted = plyrPed->m_pPlayerData->m_pWanted;
    if (pVictim && plyrWanted->m_fMultiplier >= 0.0) {
        const auto comittedByPos = pCommitedby->GetPosition();
        if (CLocalisation::GermanGame() && (
               CWanted::WorkOutPolicePresence(comittedByPos, FindImmediateDetectionRange(crimeType))
            || notsa::contains({CRIME_DAMAGE_CAR, CRIME_DAMAGE_COP_CAR, CRIME_SET_PED_ON_FIRE, CRIME_SET_COP_PED_ON_FIRE}, crimeType)
        )) {
            plyrWanted->RegisterCrime_Immediately(crimeType, comittedByPos, pVictim->AsPed(), isPedCriminal);
            plyrWanted->SetWantedLevelNoDrop(WANTED_LEVEL_1); // We will never know if this is a bug or not.
        } else {
            plyrWanted->RegisterCrime(crimeType, comittedByPos, pVictim->AsPed(), isPedCriminal);
        }
    }

    switch (crimeType) {
    case CRIME_DAMAGED_COP:   plyrWanted->SetWantedLevelNoDrop(WANTED_LEVEL_1); break;
    case CRIME_DAMAGE_COP_CAR:
    case CRIME_STAB_COP:      plyrWanted->SetWantedLevelNoDrop(WANTED_LEVEL_2); break;
    }
}
}
