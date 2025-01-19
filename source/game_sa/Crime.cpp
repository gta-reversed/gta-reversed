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
    if (crimeType == CRIME_NONE
        || !pCommitedby
        || pCommitedby->GetPedType() <= PED_TYPE_PLAYER2) {
        return;
    }

    bool isPedCriminal = false;
    if (pVictim && pVictim->IsPed()) {
        isPedCriminal = CPedType::PoliceDontCareAboutCrimesAgainstPedType(pVictim->AsPed()->m_nPedType);
    }

    if (crimeType == CRIME_DAMAGED_PED
        && pVictim
        && pVictim->IsPed()
        && IsPedPointerValid(pVictim->AsPed())
        && !pCommitedby->AsPlayer()->GetWantedLevel()
        && pVictim->AsPed()->bBeingChasedByPolice) {
        ePedState m_nPedState = pVictim->AsPed()->m_nPedState;

        if (m_nPedState == PEDSTATE_DIE || m_nPedState == PEDSTATE_DEAD) {
            return;
        }

        // Good Citizen Bonus! +$50
        if (const auto text = TheText.Get("GOODBOY")) {
            CMessages::AddBigMessage(text, 5'000, eMessageStyle::STYLE_MIDDLE);
        }

        if (pCommitedby->GetPedType() == PED_TYPE_PLAYER1) {
            pCommitedby->AsPlayer()->GetPlayerInfoForThisPlayerPed()->m_nMoney += 50;
        }
        return;
    }

    const CPlayerPed* PlayerPed = pCommitedby ? pCommitedby->AsPlayer() : nullptr;
    if (!PlayerPed) {
        return;
    }

    const auto playerWanted = PlayerPed->m_pPlayerData->m_pWanted;
    CVector    suspectPos   = pCommitedby->GetPosition();
    if (pVictim && playerWanted->m_fMultiplier >= 0.0) {
        float SearchRadiusForCrime = CCrime::FindImmediateDetectionRange(crimeType);
        if (CWanted::WorkOutPolicePresence(suspectPos, SearchRadiusForCrime)
            || notsa::contains(
               {eCrimeType::CRIME_DAMAGE_CAR,
                eCrimeType::CRIME_DAMAGE_COP_CAR,
                eCrimeType::CRIME_SET_PED_ON_FIRE,
                eCrimeType::CRIME_SET_COP_PED_ON_FIRE},
                crimeType)
            && CLocalisation::GermanGame())
        {
            playerWanted->RegisterCrime_Immediately(crimeType, suspectPos, pVictim->AsPed(), isPedCriminal);
            playerWanted->SetWantedLevelNoDrop(WANTED_LEVEL_1); // We will never know if this is a bug or not.
        } else {
            playerWanted->RegisterCrime(crimeType, suspectPos, pVictim->AsPed(), isPedCriminal);
        }
    }

    switch (crimeType) {
    case CRIME_DAMAGED_COP:
        playerWanted->SetWantedLevelNoDrop(WANTED_LEVEL_1);
        return;
    case CRIME_DAMAGE_COP_CAR:
    case CRIME_STAB_COP:
        playerWanted->SetWantedLevelNoDrop(WANTED_LEVEL_2);
        return;
    }
}
