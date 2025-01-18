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

// Android
static const float WANTED_LEVEL_RANGES[12] = {60.0f, 14.0f, 14.0f, 14.0f, 60.0f, 60.0f, 14.0f, 14.0f, 30.0f, 0.0f, 0.0f, 0.0f};

// 0x531FC0
float CCrime::FindImmediateDetectionRange(eCrimeType CrimeType)
{
  if ((CrimeType - CRIME_DESTROY_HELI) > CRIME_VEHICLE_DAMAGE)
    return 14.0;
  else
    return WANTED_LEVEL_RANGES[CrimeType - CRIME_DESTROY_HELI];
}

// 0x532010
void CCrime::ReportCrime(eCrimeType CrimeType, CEntity* pVictim, CPed* pCommitedby)
{
    if (pCommitedby == nullptr || pCommitedby->m_nPedType == PED_TYPE_PLAYER2)
    {
        return;
    }

    bool isVictimPolice = false;
    if (pVictim && (pVictim->IsPed())) {
        isVictimPolice = CPedType::PoliceDontCareAboutCrimesAgainstPedType(pVictim->AsPed()->m_nPedType);
    }

    if (CrimeType == CRIME_NONE)
        return;

    auto pPlayerWanted = FindPlayerWanted();
    const auto& wantedLevel = pPlayerWanted->m_nWantedLevel;

    CPed* pPedVictim = nullptr;
    if (pVictim && pVictim->IsPed()) {
        pPedVictim = pVictim->AsPed();
    }

    int newWantedLevel = WANTED_CLEAN;
    if (CrimeType != CRIME_DAMAGED_PED || (!pVictim->IsPed()) || !IsPedPointerValid(pPedVictim) || wantedLevel || !pPedVictim->bBeingChasedByPolice) {
        if (pPlayerWanted->m_fMultiplier < 0.0) {
            if (CrimeType == CRIME_STAB_COP) {
                newWantedLevel = WANTED_LEVEL_2;
            } else {
                if (CrimeType != CRIME_DAMAGE_COP_CAR) {
                    if (CrimeType != CRIME_DAMAGED_COP)
                        return;
                    newWantedLevel = WANTED_LEVEL_1;
                } else {
                    newWantedLevel = WANTED_LEVEL_2;
                }
            }
        } else {
            bool isntCrimeImmediate = (CrimeType - CRIME_DESTROY_HELI) > CRIME_VEHICLE_DAMAGE;
            CVector suspectPos = pCommitedby->GetPosition();
            if (!CWanted::WorkOutPolicePresence(suspectPos, CCrime::FindImmediateDetectionRange(CrimeType))) {
                if (CrimeType > CRIME_SET_COP_PED_ON_FIRE || 
                    (CrimeType != CRIME_DAMAGE_CAR && 
                        CrimeType != CRIME_DAMAGE_COP_CAR && 
                        CrimeType != CRIME_SET_PED_ON_FIRE && 
                        CrimeType != CRIME_SET_COP_PED_ON_FIRE) || 
                    !CLocalisation::GermanGame()) 
                {
                    if (isntCrimeImmediate) {
                        pPlayerWanted->RegisterCrime(CrimeType, suspectPos, pPedVictim, isVictimPolice);
                    } else {
                        pPlayerWanted->RegisterCrime_Immediately(CrimeType, suspectPos, pPedVictim, isVictimPolice);
                        CPlayerPed* PlayerPed = FindPlayerPed(-1);
                        PlayerPed->SetWantedLevelNoDrop(WANTED_LEVEL_1);
                    }

                    if (CrimeType == CRIME_STAB_COP) {
                        newWantedLevel = WANTED_LEVEL_2;
                    } else if (CrimeType != CRIME_DAMAGE_COP_CAR) {
                        if (CrimeType != CRIME_DAMAGED_COP)
                            return;
                        newWantedLevel = WANTED_LEVEL_1;
                    }
                }
            }
        }
    }

    if (newWantedLevel != CRIME_NONE) {
        CPlayerPed * PlayerPed = FindPlayerPed(-1);
        PlayerPed->SetWantedLevelNoDrop(newWantedLevel);
        return;
    }

    // Avoid crashes
    if (!pPedVictim) {
        return;
    }

    ePedState state = pPedVictim->m_nPedState;
    if (state != PEDSTATE_DIE && state != PEDSTATE_DEAD) {
        // Good Citizen Bonus! +$50
        CMessages::AddBigMessage(TheText.Get("GOODBOY"), 5'000, STYLE_MIDDLE);
        CWorld::Players[CWorld::PlayerInFocus].m_nMoney += 50;
    }
}
