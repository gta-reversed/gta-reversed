#include "StdInc.h"

#include "Darkel.h"
#include "Hud.h"

// Rampages are against to some specific 'model', using these below
// as enemies mean all peds/vehicles/bikes can be destroyed.
constexpr eModelID MODELTK_ANY_PED = eModelID(-1);
constexpr eModelID MODELTK_ANY_VEHICLE = eModelID(-2);
constexpr eModelID MODELTK_ANY_BIKE = eModelID(-3);

void CDarkel::InjectHooks() {
    RH_ScopedClass(CDarkel);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(FrenzyOnGoing, 0x43D1F0);
    RH_ScopedInstall(Init, 0x43CEB0);
    RH_ScopedInstall(DrawMessages, 0x43CEC0);
    RH_ScopedInstall(ReadStatus, 0x43D1E0);
    RH_ScopedInstall(RegisterKillNotByPlayer, 0x43D210);
    RH_ScopedInstall(ThisPedShouldBeKilledForFrenzy, 0x43D2F0);
    RH_ScopedInstall(ThisVehicleShouldBeKilledForFrenzy, 0x43D350);
    RH_ScopedInstall(StartFrenzy, 0x43D3B0);
    RH_ScopedInstall(ResetModelsKilledByPlayer, 0x43D6A0);
    RH_ScopedInstall(QueryModelsKilledByPlayer, 0x43D6C0);
    RH_ScopedInstall(FindTotalPedsKilledByPlayer, 0x43D6E0);
    RH_ScopedInstall(DealWithWeaponChangeAtEndOfFrenzy, 0x43D7A0);
    RH_ScopedInstall(CheckDamagedWeaponType, 0x43D9E0);
    RH_ScopedInstall(Update, 0x43DAC0);
    RH_ScopedInstall(ResetOnPlayerDeath, 0x43DC10);
    RH_ScopedInstall(FailKillFrenzy, 0x43DC60);
    RH_ScopedInstall(RegisterKillByPlayer, 0x43DCD0);
    RH_ScopedInstall(RegisterCarBlownUpByPlayer, 0x43DF20);

    // unused
    RH_ScopedInstall(CalcFade, 0x43D740);
}

// 0x43CEB0
void CDarkel::Init() {
    Status = DARKEL_STATUS_0;
}

// 0x43D1F0
bool CDarkel::FrenzyOnGoing() {
    return Status == DARKEL_STATUS_1 || Status == DARKEL_STATUS_4;
}

// 0x43CEC0
void CDarkel::DrawMessages() {
    if (CReplay::Mode == MODE_PLAYBACK)
        return;

    const auto elapsed = CTimer::GetTimeInMS() - CDarkel::TimeOfFrenzyStart;

    if (FrenzyOnGoing()) {
        if (pStartMessage) {
            if (bStandardSoundAndMessages) {
                if (elapsed >= 3000 && elapsed < 11'000) {
                    CMessages::AddBigMessage(pStartMessage, 3000, STYLE_MIDDLE);
                }
            } else if (elapsed < 8000) {
                CMessages::AddBigMessage(pStartMessage, 3000, STYLE_MIDDLE);
            }
        }

        CFont::SetProportional(true);
        CFont::SetBackground(false, false);
        CFont::SetScale(SCREEN_SCALE_X(0.5f), SCREEN_SCALE_Y(1.0f));
        CFont::SetOrientation(eFontAlignment::ALIGN_RIGHT);
        CFont::SetRightJustifyWrap(0.0f);
        CFont::SetFontStyle(FONT_MENU);
        CFont::SetWrapx(SCREEN_SCALE_X(640.0f));
        CFont::SetEdge(2);
        CFont::SetDropColor({0, 0, 0, 255});
        CFont::SetColor(HudColour.GetRGB(HUD_COLOUR_LIGHT_GRAY));

        const auto y1 = CHud::GetYPosBasedOnHealth(CWorld::PlayerInFocus, SCREEN_SCALE_Y(148.0f), 12);
        auto remainingTimerY = CHud::GetYPosBasedOnHealth(PED_TYPE_PLAYER2, y1, 12);

        const auto y2 = CHud::GetYPosBasedOnHealth(CWorld::PlayerInFocus, SCREEN_SCALE_Y(168.0f), 12);
        auto remainingKillsY = CHud::GetYPosBasedOnHealth(PED_TYPE_PLAYER2, y2, 12);
        if (FindPlayerPed(PED_TYPE_PLAYER2)) {
            remainingTimerY += SCREEN_SCALE_Y(72.0f);
            remainingKillsY += SCREEN_SCALE_Y(72.0f);
        }

        if (TimeLimit >= 0) {
            const auto remaining = TimeOfFrenzyStart + TimeLimit - CTimer::GetTimeInMS();
            sprintf_s(gString, "%d:%02d", remaining / 60'000, remaining % 60'000 / 1000);
            AsciiToGxtChar(gString, gGxtString);

            // blink in last 4 second
            if (remaining > 4000 || CTimer::GetFrameCounter() % 2) {
                CFont::PrintString(SCREEN_SCALE_FROM_RIGHT(32.0f), remainingTimerY, gGxtString);
            }
        }
        sprintf_s(gString, "%d", KillsNeeded < 0 ? 0 : KillsNeeded);
        AsciiToGxtChar(gString, gGxtString);
        CFont::PrintString(SCREEN_SCALE_FROM_RIGHT(32.0f), remainingKillsY, gGxtString);
    }

    if (Status == DARKEL_STATUS_2 && bStandardSoundAndMessages && elapsed < 5000) {
        CMessages::AddBigMessage(TheText.Get("KILLPA"), 3000, STYLE_MIDDLE);
    }
}

// 0x43D1E0
eDarkelStatus CDarkel::ReadStatus() {
    return Status;
}

// 0x43D210
void CDarkel::RegisterKillNotByPlayer(const CPed* killedPed) {
    if (auto player = FindPlayerPed(); killedPed != player->AsPed()) {
        if (player->GetPlayerGroup().m_groupMembership.IsMember(killedPed)) {
            CStats::DecrementStat(STAT_RESPECT, 2.0f);
            CStats::IncrementStat(STAT_RECRUITED_GANG_MEMBERS_KILLED);
            CStats::DisplayScriptStatUpdateMessage(STAT_UPDATE_DECREASE, STAT_GANG_STRENGTH, 1.0f);
        }
    }
    CStats::IncrementStat(STAT_PEOPLE_WASTED_BY_OTHERS);

    if (const auto friendly = killedPed->m_nPedType == PED_TYPE_GANG2; killedPed->IsGangster()) {
        CStats::IncrementStat(friendly ? STAT_FRIENDLY_GANG_MEMBERS_KILLED : STAT_ENEMY_GANG_MEMBERS_KILLED);
    }
}

// 0x43D2F0
bool CDarkel::ThisPedShouldBeKilledForFrenzy(const CPed& ped) {
    if (!FrenzyOnGoing())
        return false;

    if (ModelToKill[3] == MODELTK_ANY_PED || rng::find(ModelToKill, ped.m_nModelIndex) != ModelToKill.end()) {
        return !ped.IsPlayer();
    }

    return false;
}

// 0x43D350
bool CDarkel::ThisVehicleShouldBeKilledForFrenzy(const CVehicle& vehicle) {
    if (!FrenzyOnGoing())
        return false;

    if (ModelToKill[3] == MODELTK_ANY_VEHICLE || rng::find(ModelToKill, vehicle.m_nModelIndex) != ModelToKill.end())
        return true;

    return ModelToKill[3] == MODELTK_ANY_BIKE && vehicle.IsSubBike();
}

// 0x43D3B0
void CDarkel::StartFrenzy(eWeaponType weaponType, int32 timeLimit, uint16 killsNeeded, int32 modelToKill, const GxtChar* startMessage, int32 modelToKill2, int32 modelToKill3, int32 modelToKill4, bool standardSoundAndMessages, bool needHeadShot) {
    CGameLogic::ClearSkip(false);
    const eWeaponType weapon = (WeaponType != WEAPON_UZI_DRIVEBY) ? weaponType : WEAPON_MICRO_UZI;

    if (CGameLogic::IsCoopGameGoingOn()) {
        Status = DARKEL_STATUS_4;
        CGameLogic::DoWeaponStuffAtStartOf2PlayerGame(false);
    } else {
        Status = DARKEL_STATUS_1;
    }

    ModelToKill[3] = modelToKill;
    ModelToKill[2] = modelToKill2;
    ModelToKill[1] = modelToKill3;
    ModelToKill[0] = modelToKill4;

    KillsNeeded = killsNeeded;
    WeaponType = weaponType;
    pStartMessage = startMessage;
    if (bProperKillFrenzy = (startMessage == TheText.Get("PAGE_00"))) {
        pStartMessage = nullptr;
    }
    bStandardSoundAndMessages = standardSoundAndMessages;
    bHeadShotRequired = needHeadShot;
    TimeOfFrenzyStart = CTimer::GetTimeInMS();
    TimeLimit = timeLimit;
    PreviousTime = timeLimit / 1000;

    auto playerPed = FindPlayerPed();
    if (weapon < WEAPON_LAST_WEAPON) {
        InterruptedWeaponTypeSelected = playerPed->GetActiveWeapon().m_Type;
        playerPed->RemoveWeaponAnims(InterruptedWeaponTypeSelected, -1000.0f);

        const auto frenzyWeaponSlot = CWeaponInfo::GetWeaponInfo(weapon)->m_nSlot;
        InterruptedWeaponType = playerPed->GetWeaponInSlot(frenzyWeaponSlot).m_Type;
        AmmoInterruptedWeapon = playerPed->GetWeaponInSlot(frenzyWeaponSlot).m_TotalAmmo;

        if (InterruptedWeaponType != WEAPON_UNARMED) {
            const auto weaponInfo = CWeaponInfo::GetWeaponInfo(InterruptedWeaponType);
            CModelInfo::GetModelInfo(weaponInfo->m_nModelId1)->AddRef();
        }

        for (auto i = 0; i < 2; i++) {
            if (auto player = FindPlayerPed(i)) {
                player->GiveWeapon(weapon, 30'000, true);
                player->SetCurrentWeapon(weapon);
                player->MakeChangesForNewWeapon(weapon);
            }
        }

        if (FindPlayerVehicle()) {
            auto chosenWeapon = playerPed->m_pPlayerData->m_nChosenWeapon;
            playerPed->SetCurrentWeapon(chosenWeapon);

            if (auto& activeWeapon = playerPed->GetActiveWeapon(); activeWeapon.m_TotalAmmo >= activeWeapon.GetWeaponInfo().m_nAmmoClip) {
                activeWeapon.m_AmmoInClip = activeWeapon.GetWeaponInfo().m_nAmmoClip;
            } else {
                activeWeapon.m_AmmoInClip = activeWeapon.m_TotalAmmo;
            }

            playerPed->ClearWeaponTarget();
        }
    }
}

// 0x43D6A0
void CDarkel::ResetModelsKilledByPlayer(int32 playerId) {
    for (auto& model : RegisteredKills) {
        model[playerId] = 0;
    }
}

// 0x43D6C0
int16 CDarkel::QueryModelsKilledByPlayer(int32 modelId, int32 playerId) {
    return RegisteredKills[modelId][playerId];
}

// 0x43D6E0
int32 CDarkel::FindTotalPedsKilledByPlayer(int32 playerId) {
    int32 sum = 0;
    for (auto& model : RegisteredKills) {
        sum += model[playerId];
    }
    return sum;
}

// 0x43D7A0
void CDarkel::DealWithWeaponChangeAtEndOfFrenzy() {
    auto playerPed = FindPlayerPed();
    const eWeaponType weapon = (WeaponType != WEAPON_UZI_DRIVEBY) ? WeaponType : WEAPON_MICRO_UZI;
    if (WeaponType < WEAPON_LAST_WEAPON) {
        if (InterruptedWeaponType != WEAPON_UNARMED) {
            const auto modelId = CWeaponInfo::GetWeaponInfo(InterruptedWeaponType)->m_nModelId1;
            CModelInfo::GetModelInfo(modelId)->RemoveRef();
        }

        const auto weaponInfo = CWeaponInfo::GetWeaponInfo(weapon);
        playerPed->RemoveWeaponModel(playerPed->GetWeaponInSlot(weaponInfo->m_nSlot).GetWeaponInfo().m_nModelId1);

        playerPed->GetWeaponInSlot(weaponInfo->m_nSlot).m_Type = WEAPON_UNARMED;
        playerPed->GetWeaponInSlot(weaponInfo->m_nSlot).m_TotalAmmo = 0;
        playerPed->GetWeaponInSlot(weaponInfo->m_nSlot).m_AmmoInClip = 0;
        playerPed->GetWeaponInSlot(weaponInfo->m_nSlot).m_State = WEAPONSTATE_READY;
        playerPed->RemoveWeaponAnims(weapon, -1000.0f);

        if (auto mi = CModelInfo::GetModelInfo(weaponInfo->m_nModelId1); mi->m_nRefCount > 0) {
            mi->RemoveRef();
        }
        playerPed->GiveWeapon(InterruptedWeaponType, AmmoInterruptedWeapon, true);
    }

    if (FindPlayerVehicle()) {
        playerPed->RemoveWeaponModel(playerPed->GetActiveWeapon().GetWeaponInfo().m_nModelId1);

        auto& chosenWeaponSlot = playerPed->m_pPlayerData->m_nChosenWeapon;
        if (playerPed->GetWeaponInSlot(eWeaponSlot::SMG).m_Type != WEAPON_UNARMED) {
            chosenWeaponSlot = (uint8)eWeaponSlot::SMG;
        } else {
            chosenWeaponSlot = (uint8)eWeaponSlot::UNARMED;
        }

        playerPed->SetCurrentWeapon(chosenWeaponSlot);
        playerPed->MakeChangesForNewWeapon(chosenWeaponSlot);
    }
}

// 0x43D9E0
bool CDarkel::CheckDamagedWeaponType(eWeaponType damageWeaponId, eWeaponType expectedDamageWeaponId) {
    if (damageWeaponId == -1) {
        return false;
    }

    if (expectedDamageWeaponId == WEAPON_ANYMELEE) {
        switch (damageWeaponId) {
            case WEAPON_UNARMED:
            case WEAPON_BRASSKNUCKLE:
            case WEAPON_GOLFCLUB:
            case WEAPON_NIGHTSTICK:
            case WEAPON_KNIFE:
            case WEAPON_BASEBALLBAT:
            case WEAPON_SHOVEL:
            case WEAPON_POOL_CUE:
            case WEAPON_KATANA:
            case WEAPON_CHAINSAW:
            case WEAPON_DILDO1:
            case WEAPON_DILDO2:
            case WEAPON_VIBE1:
            case WEAPON_VIBE2:
            case WEAPON_FLOWERS:
            case WEAPON_CANE:
                return true;
            default:
                return false;
        }
    }
    if (expectedDamageWeaponId != WEAPON_ANYWEAPON) {
        return false;
    }

    switch (damageWeaponId) {
        case WEAPON_UNARMED:
        case WEAPON_BRASSKNUCKLE:
        case WEAPON_GOLFCLUB:
        case WEAPON_NIGHTSTICK:
        case WEAPON_KNIFE:
        case WEAPON_BASEBALLBAT:
        case WEAPON_SHOVEL:
        case WEAPON_POOL_CUE:
        case WEAPON_KATANA:
        case WEAPON_CHAINSAW:
        case WEAPON_DILDO1:
        case WEAPON_DILDO2:
        case WEAPON_VIBE1:
        case WEAPON_VIBE2:
        case WEAPON_FLOWERS:
        case WEAPON_CANE:
        case WEAPON_GRENADE:
        case WEAPON_TEARGAS:
        case WEAPON_MOLOTOV:
        case WEAPON_ROCKET:
        case WEAPON_ROCKET_HS:
        case WEAPON_FREEFALL_BOMB:
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
        case WEAPON_RLAUNCHER:
        case WEAPON_RLAUNCHER_HS:
        case WEAPON_FLAMETHROWER:
        case WEAPON_MINIGUN:
        case WEAPON_REMOTE_SATCHEL_CHARGE:
        case WEAPON_DETONATOR:
        case WEAPON_SPRAYCAN:
        case WEAPON_EXTINGUISHER:
        case WEAPON_CAMERA:
        case WEAPON_NIGHTVISION:
        case WEAPON_INFRARED:
        case WEAPON_EXPLOSION:
        case WEAPON_UZI_DRIVEBY:
            return true;
        default:
            return false;
    }
}

// 0x43DAC0
void CDarkel::Update() {
    ZoneScoped;

    if (!FrenzyOnGoing())
        return;

    const auto StartFrenzy = [&] {
        Status = DARKEL_STATUS_3;
        CPopulation::m_AllRandomPedsThisType = -1;
        TimeOfFrenzyStart = CTimer::GetTimeInMS();
        DealWithWeaponChangeAtEndOfFrenzy();
    };

    const int32 remaining = TimeOfFrenzyStart + TimeLimit - CTimer::GetTimeInMS();
    if (remaining <= 0 && TimeLimit >= 0) {
        if (Status == DARKEL_STATUS_4) {
            CGameLogic::SetMissionFailed();
        }
        StartFrenzy();
    } else if (Status != DARKEL_STATUS_4 || FindPlayerPed(PED_TYPE_PLAYER2)) {
        if (remaining / 1000 != PreviousTime) {
            if (PreviousTime < 12) {
                AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_TIMER_COUNT);
            }
            PreviousTime = remaining / 1000;
        }
    } else {
        CGameLogic::SetMissionFailed();
        StartFrenzy();
    }

    if (KillsNeeded <= 0) {
        if (Status == DARKEL_STATUS_4) {
            CGameLogic::GameState = GAMELOGIC_STATE_MISSION_PASSED;
            CGameLogic::TimeOfLastEvent = CTimer::GetTimeInMS();
        }

        Status = DARKEL_STATUS_2;
        CPopulation::m_AllRandomPedsThisType = -1;

        if (bProperKillFrenzy) {
            CStats::IncrementStat(STAT_RAMPAGES_PASSED);
        }
        TimeOfFrenzyStart = CTimer::GetTimeInMS();
        FindPlayerPed()->SetWantedLevel(0);

        DealWithWeaponChangeAtEndOfFrenzy();
    }
}

// 0x43DC10
void CDarkel::ResetOnPlayerDeath() {
    CHud::SetHelpMessage(nullptr, true, false, false);
    if (FrenzyOnGoing()) {
        Status = DARKEL_STATUS_3;
        CPopulation::m_AllRandomPedsThisType = -1;
        TimeOfFrenzyStart = CTimer::GetTimeInMS();
        DealWithWeaponChangeAtEndOfFrenzy();
    }
}

// 0x43DC60
void CDarkel::FailKillFrenzy() {
    if (Status == DARKEL_STATUS_4) {
        CGameLogic::SetMissionFailed();
    }
    ResetOnPlayerDeath();
}

// 0x43DCD0
void CDarkel::RegisterKillByPlayer(const CPed& killedPed, eWeaponType damageWeaponId, bool headShotted, int32 playerId) {
    switch (killedPed.m_nPedType) {
    case PED_TYPE_COP:
    case PED_TYPE_DEALER:
        CStats::IncrementStat(STAT_RESPECT, 1.5f);
        break;
    default:
        break;
    }

    if (const auto friendly = killedPed.m_nPedType == PED_TYPE_GANG2; killedPed.IsGangster()) {
        if (friendly) {
            CStats::IncrementStat(STAT_FRIENDLY_GANG_MEMBERS_KILLED);
            CStats::DecrementStat(STAT_RESPECT, 5.0f);

            if (auto player = FindPlayerPed(); &killedPed != player->AsPed()) {
                if (player->GetPlayerGroup().m_groupMembership.IsMember(&killedPed)) {
                    CStats::IncrementStat(STAT_RECRUITED_GANG_MEMBERS_KILLED);
                    CStats::DisplayScriptStatUpdateMessage(STAT_UPDATE_DECREASE, STAT_GANG_STRENGTH, 1.0f);
                }
            }
        } else {
            CStats::IncrementStat(STAT_ENEMY_GANG_MEMBERS_KILLED);
            CStats::IncrementStat(STAT_RESPECT, 0.75f);
        }
    }

    if (FrenzyOnGoing()) {
        const auto CountFrenzyKill = [&] {
            if (ThisPedShouldBeKilledForFrenzy(killedPed) && (!bHeadShotRequired || headShotted)) {
                KillsNeeded--;
                AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_PART_MISSION_COMPLETE);
            }

            CStats::IncrementStat(killedPed.IsCop() ? STAT_HIGHEST_POLICE_PEDS_KILLED_ON_RAMPAGE : STAT_HIGHEST_CIVILIAN_PEDS_KILLED_ON_RAMPAGE);
        };

        if (damageWeaponId == WeaponType || (WeaponType == WEAPON_ANYMELEE || WeaponType == WEAPON_ANYWEAPON) && CheckDamagedWeaponType(damageWeaponId, WeaponType)) {
            CountFrenzyKill();
        }

        switch (damageWeaponId) {
        case WEAPON_EXPLOSION:
            CountFrenzyKill();
            break;
        case WEAPON_UZI_DRIVEBY:
            if (WeaponType == WEAPON_MICRO_UZI) {
                CountFrenzyKill();
            }
            break;
        case WEAPON_RAMMEDBYCAR:
            if (WeaponType == WEAPON_RUNOVERBYCAR) { // wrong?
                CountFrenzyKill();
            }
            break;
        case WEAPON_RUNOVERBYCAR:
            if (WeaponType == WEAPON_RAMMEDBYCAR) { // wrong?
                CountFrenzyKill();
            }
            break;
        case WEAPON_FLAMETHROWER:
            if (WeaponType == WEAPON_MOLOTOV) {
                CountFrenzyKill();
            }
            break;
        default:
            break;
        }

        RegisteredKills[killedPed.m_nModelIndex][playerId]++;
        CStats::IncrementStat(STAT_PEOPLE_YOUVE_WASTED);
        CStats::PedsKilledOfThisType[killedPed.bChrisCriminal ? PED_TYPE_CRIMINAL : killedPed.m_nPedType]++;

        if (headShotted) {
            CStats::IncrementStat(STAT_NUMBER_OF_HEADSHOTS);
        }
        CStats::IncrementStat(STAT_KILLS_SINCE_LAST_CHECKPOINT);

        if (playerId == PED_TYPE_PLAYER1 && !FindPlayerPed(PED_TYPE_PLAYER1)->bInVehicle) {
            CGangWars::AddKillToProvocation(killedPed.m_nPedType);
        }

        if (killedPed.m_nPedType >= PED_TYPE_DEALER) { // BUG?
            CPopCycle::PlayerKilledADealer();
        }
    }
}

// 0x43DF20
void CDarkel::RegisterCarBlownUpByPlayer(CVehicle& vehicle, int32 playerId) {
    if (ThisVehicleShouldBeKilledForFrenzy(vehicle)) {
        KillsNeeded--;
        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_PART_MISSION_COMPLETE);
    }
    RegisteredKills[vehicle.m_nModelIndex][playerId]++;

    switch (vehicle.GetVehicleAppearance()) {
    case VEHICLE_APPEARANCE_AUTOMOBILE:
    case VEHICLE_APPEARANCE_BIKE:
        CStats::IncrementStat(STAT_ROAD_VEHICLES_DESTROYED);
        break;
    case VEHICLE_APPEARANCE_HELI:
    case VEHICLE_APPEARANCE_PLANE:
        CStats::IncrementStat(STAT_PLANES_HELICOPTERS_DESTROYED);
        break;
    case VEHICLE_APPEARANCE_BOAT:
        CStats::IncrementStat(STAT_BOATS_DESTROYED);
        break;
    default:
        break;
    }

    if (FrenzyOnGoing()) {
        if (vehicle.m_nModelIndex == MODEL_RHINO) {
            CStats::IncrementStat(STAT_HIGHEST_NUMBER_OF_TANKS_DESTROYED_ON_RAMPAGE);
        } else if (vehicle.IsLawEnforcementVehicle()) {
            CStats::IncrementStat(STAT_HIGHEST_POLICE_VEHICLES_DESTROYED_ON_RAMPAGE);
        } else {
            CStats::IncrementStat(STAT_HIGHEST_CIVILIAN_VEHICLES_DESTROYED_ON_RAMPAGE);
        }
    }
}

// unused
// 0x43D740
uint8 CDarkel::CalcFade(uint32 t, uint32 begin, uint32 end) {
    if (t < begin || t > end) {
        return 0;
    }

    if (t >= begin + 500) {
        if (t <= end - 500) {
            return 255;
        }

        return 255 * (end - t) / 500;
    }

    return 255 * (t - begin) / 500;
}
