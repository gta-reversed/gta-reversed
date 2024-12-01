#include "StdInc.h"
#include "MBlur.h"
#include "EntryExitManager.h"
#include "Garages.h"
#include "PedClothesDesc.h"
#include "PostEffects.h"
#include <extensions/enumerate.hpp>

void CGameLogic::InjectHooks() {
    RH_ScopedClass(CGameLogic);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(CalcDistanceToForbiddenTrainCrossing, 0x4418E0);
    RH_ScopedInstall(ClearSkip, 0x441560);
    RH_ScopedInstall(DoWeaponStuffAtStartOf2PlayerGame, 0x4428B0);
    RH_ScopedInstall(FindCityClosestToPoint, 0x441B70);
    RH_ScopedInstall(ForceDeathRestart, 0x441240);
    RH_ScopedInstall(InitAtStartOfGame, 0x441210);
    RH_ScopedInstall(IsCoopGameGoingOn, 0x441390);
    RH_ScopedInstall(IsPlayerAllowedToGoInThisDirection, 0x441E10);
    RH_ScopedInstall(IsPlayerUse2PlayerControls, 0x442020);
    RH_ScopedInstall(IsPointWithinLineArea, 0x4416E0);
    RH_ScopedInstall(IsSkipWaitingForScriptToFadeIn, 0x4416C0);
    RH_ScopedInstall(LaRiotsActiveHere, 0x441C10);
    RH_ScopedInstall(Save, 0x5D33C0, {.reversed = true});
    RH_ScopedInstall(Load, 0x5D3440, {.reversed = true});
    RH_ScopedInstall(PassTime, 0x4414C0);
    RH_ScopedInstall(Remove2ndPlayerIfPresent, 0x4413C0);
    RH_ScopedInstall(ResetStuffUponResurrection, 0x442980);
    RH_ScopedInstall(StorePedsWeapons, 0x441D00);
    RH_ScopedInstall(RestorePedsWeapons, 0x441D30);
    RH_ScopedInstall(RestorePlayerStuffDuringResurrection, 0x442060);
    RH_ScopedInstall(SetPlayerWantedLevelForForbiddenTerritories, 0x441770);
    RH_ScopedInstall(SetUpSkip, 0x4423C0);
    RH_ScopedInstall(SkipCanBeActivated, 0x4415C0);
    RH_ScopedInstall(SortOutStreamingAndMemory, 0x441440);
    RH_ScopedInstall(StopPlayerMovingFromDirection, 0x441290);
    RH_ScopedInstall(Update, 0x442AD0);
    RH_ScopedInstall(UpdateSkip, 0x442480);
}

// 0x4418E0
float CGameLogic::CalcDistanceToForbiddenTrainCrossing(CVector vecPoint, CVector vecMoveSpeed, bool ignoreMoveSpeed, CVector& outDistance) {
    auto closest = 100'000.0f; // FLT_MAX

    const auto Calculate = [&closest, &vecPoint, &vecMoveSpeed, &outDistance, ignoreMoveSpeed](CVector2D posn) {
        const auto diff = posn - vecPoint;
        const auto dist = diff.Magnitude();

        if (((vecPoint * vecMoveSpeed).ComponentwiseSum() > 0.0f || ignoreMoveSpeed) && dist < closest) {
            outDistance = vecPoint;
            closest = dist;
        }
    };

    const auto cityUnlocked = CStats::GetStatValue(STAT_CITY_UNLOCKED);
    if (cityUnlocked == 0.0f) {
        Calculate(CVector2D{82.0f, -1021.0f});  // LS-SF; Flint Country
    } else if (cityUnlocked == 1.0f) {
        Calculate(CVector2D{-1568.0f, 537.0f}); // SF-LV; Downtown
    } else if (cityUnlocked < 1.0f) {
        Calculate(CVector2D{2270.0f,  277.0f}); // Not a train crossing; top left side of Palomino Creek, LS
    }

    return closest;
}

// 0x441560
void CGameLogic::ClearSkip(bool afterMission) {
    if (afterMission && SkipState == SKIP_AFTER_MISSION)
        return;

    if (SkipState == SKIP_IN_PROGRESS) {
        TheCamera.SetFadeColour(0, 0, 0);
        TheCamera.Fade(0.5f, eFadeFlag::FADE_OUT);
    }
    SkipState = SKIP_NONE;
    CPad::GetPad(PED_TYPE_PLAYER1)->bCamera = false;
}

// 0x4428B0
void CGameLogic::DoWeaponStuffAtStartOf2PlayerGame(bool shareWeapons) {
    auto player1 = FindPlayerPed(PED_TYPE_PLAYER1);
    auto player2 = FindPlayerPed(PED_TYPE_PLAYER2);
    RestorePedsWeapons(player1);

    if (shareWeapons) {
        for (auto& weapon : player1->m_aWeapons) {
            if (weapon.m_Type == WEAPON_UNARMED)
                continue;

            player2->GiveWeapon(weapon, true);
        }
        player1->PickWeaponAllowedFor2Player();
        player1->m_pPlayerData->m_nChosenWeapon = player1->m_pPlayerData->m_nChosenWeapon;
    }
}

// 0x441B70
// 1 - Los Santos, 2 - San Fierro, 3 - Las Venturas
eLevelName CGameLogic::FindCityClosestToPoint(CVector2D point) {
    constexpr struct {
        eLevelName level;
        CVector2D coords;
    } CITIES[] = {
        {LEVEL_NAME_LOS_SANTOS,   { 1670.0f, -1137.0f}},
        {LEVEL_NAME_SAN_FIERRO,   {-1810.0f,   884.0f}},
        {LEVEL_NAME_LAS_VENTURAS, { 2161.0f,  2140.0f}},
    };

    std::pair<float, eLevelName> closest{FLT_MAX, NUM_LEVELS}; // NOTSA
    for (const auto& city : CITIES) {
        if (const auto d = DistanceBetweenPoints2D(city.coords, point); d < closest.first) {
            closest = {d, city.level};
        }
    }

    if (closest.second == NUM_LEVELS) {
        NOTSA_UNREACHABLE();
    }

    return closest.second;
}

// 0x441240
void CGameLogic::ForceDeathRestart() {
    CWorld::Players[CWorld::PlayerInFocus].m_nPlayerState = PLAYERSTATE_HAS_DIED;
    GameState = GAMELOGIC_STATE_WASTED;
    TimeOfLastEvent = CTimer::GetTimeInMS() - 3001;
    TheCamera.SetFadeColour(0, 0, 0);
    TheCamera.Fade(4.0f, eFadeFlag::FADE_IN);
}

// 0x441210
void CGameLogic::InitAtStartOfGame() {
    ZoneScoped;

    ActivePlayers            = true;
    SkipState                = SKIP_NONE;
    NumAfterDeathStartPoints = 0;
    GameState                = GAMELOGIC_STATE_PLAYING;
    TimeOfLastEvent          = 0;
    nPrintFocusHelpTimer     = 0;
    nPrintFocusHelpCounter   = 0;
    bScriptCoopGameGoingOn   = false;
}

// 2 players are playing
// 0x441390
bool CGameLogic::IsCoopGameGoingOn() {
    return FindPlayerPed(PED_TYPE_PLAYER1) && FindPlayerPed(PED_TYPE_PLAYER2);
}

// 0x441E10
bool CGameLogic::IsPlayerAllowedToGoInThisDirection(CPed* ped, CVector moveDirection, float distanceLimit) {
    const auto player1 = FindPlayerPed(PED_TYPE_PLAYER1);
    const auto player2 = FindPlayerPed(PED_TYPE_PLAYER2);

    if (!player1 || !player2 || !bLimitPlayerDistance)
        return true;

    if (IsAPlayerInFocusOn2PlayerGame()) {
        if (ped == GetFocusedPlayerPed()) {
            return true;
        }

        CVector headPos{0.0f, 0.0f, 0.2f};
        ped->GetTransformedBonePosition(headPos, BONE_HEAD, false);

        if (!ped->GetIsOnScreen()) {
            return false;
        }

        if (!CWorld::GetIsLineOfSightClear(*TheCamera.GetGameCamPosition(), headPos, true, false, false, false, false, true, false)) {
            return false;
        }
    }

    if (DistanceBetweenPoints2D(player1->GetPosition(), player2->GetPosition()) < MaxPlayerDistance + distanceLimit) {
        return true;
    }

    const auto dirDist = ((player1->GetPosition() - player2->GetPosition()) * moveDirection).ComponentwiseSum();
    if (ped == player1 && dirDist >= 0.0f || dirDist <= 0.0f) {
        return false;
    }
    
    return true;
}

// 0x442020
bool CGameLogic::IsPlayerUse2PlayerControls(CPed* ped) {
    if (!IsCoopGameGoingOn())
        return false;

    if (n2PlayerPedInFocus == eFocusedPlayer::PLAYER2)
        return true;

    return ped != GetFocusedPlayerPed();
}

// 0x4416E0
bool CGameLogic::IsPointWithinLineArea(const CVector* points, uint32 numPoints, float x, float y) {
    for (auto&& [i, point] : notsa::enumerate(std::span{points, numPoints})) {
        const auto nextPoint = (i != numPoints - 1) ? points[i + 1] : points[0];
        if (CCollision::Test2DLineAgainst2DLine(x, y, 1'000'000.0f, 0.0f, point.x, point.y, nextPoint.x - point.x, nextPoint.y - point.y))
            return true;
    }

    return false;
}

// 0x4416C0
bool CGameLogic::IsSkipWaitingForScriptToFadeIn() {
    if (CGameLogic::SkipState != SKIP_WAITING_SCRIPT) {
        return false;
    }
    CGameLogic::SkipState = SKIP_NONE;
    return true;
}

// 0x441C10
bool CGameLogic::LaRiotsActiveHere() {
    const auto coors = FindPlayerCoors();

    if (coors.z > 950.0f) {
        return false;
    }

    if (CCheat::IsActive(CHEAT_RIOT_MODE)) {
        return true;
    }

    if (!gbLARiots) {
        return false;
    }

    const CRect rects[]{
        {1620.0f, -2178.0f, 2824.0f, -1067.0f},
        {157.0f, -1950.0f, 1630.0f, -1192.0f},
    };
    return rng::any_of(rects, [&](auto rect) { return rect.IsPointInside(coors); });
}

// 0x5D33C0
void CGameLogic::Save() {
    CGenericGameStorage::SaveDataToWorkBuffer(&CGameLogic::NumAfterDeathStartPoints,                sizeof(int32));
    CGenericGameStorage::SaveDataToWorkBuffer(&CGameLogic::bPenaltyForDeathApplies,                 sizeof(bool));
    CGenericGameStorage::SaveDataToWorkBuffer(&CGameLogic::bPenaltyForArrestApplies,                sizeof(bool));
    CGenericGameStorage::SaveDataToWorkBuffer(&CGameLogic::GameState,                               sizeof(eGameState));
    CGenericGameStorage::SaveDataToWorkBuffer(&CGameLogic::TimeOfLastEvent,                         sizeof(uint32));

    for (int32 i = 0; i < NumAfterDeathStartPoints; i++) {
        CGenericGameStorage::SaveDataToWorkBuffer(&CGameLogic::AfterDeathStartPoints[i],            sizeof(CVector));
        CGenericGameStorage::SaveDataToWorkBuffer(&CGameLogic::AfterDeathStartPointOrientations[i], sizeof(float));
    }
}

// 0x5D3440
void CGameLogic::Load() {
    CGenericGameStorage::LoadDataFromWorkBuffer(&CGameLogic::NumAfterDeathStartPoints,                sizeof(int32));
    CGenericGameStorage::LoadDataFromWorkBuffer(&CGameLogic::bPenaltyForDeathApplies,                 sizeof(bool));
    CGenericGameStorage::LoadDataFromWorkBuffer(&CGameLogic::bPenaltyForArrestApplies,                sizeof(bool));
    CGenericGameStorage::LoadDataFromWorkBuffer(&CGameLogic::GameState,                               sizeof(eGameState));
    CGenericGameStorage::LoadDataFromWorkBuffer(&CGameLogic::TimeOfLastEvent,                         sizeof(uint32));
    for (int32 i = 0; i < NumAfterDeathStartPoints; ++i) {
        CGenericGameStorage::LoadDataFromWorkBuffer(&CGameLogic::AfterDeathStartPoints[i],            sizeof(CVector));
        CGenericGameStorage::LoadDataFromWorkBuffer(&CGameLogic::AfterDeathStartPointOrientations[i], sizeof(float));
    }
}

// 0x4414C0
void CGameLogic::PassTime(uint32 minutes) {
    auto weekDay = CClock::GetGameWeekDay();
    auto hours = CClock::GetGameClockHours();
    auto mins = minutes + CClock::GetGameClockMinutes();

    if (mins >= 60) {
        hours += mins / 60;
        mins %= 60;
    }

    if (hours >= 24) {
        auto days = hours / 24;
        hours %= 24;

        CStats::IncrementStat(STAT_DAYS_PASSED_IN_GAME, (float)days);
        weekDay += days;
        if (weekDay > 7) {
            weekDay = 1;
        }
    }

    CClock::SetGameClock(hours, mins, weekDay);
    CPickups::PassTime(minutes * 1000);
}

// 0x4413C0
void CGameLogic::Remove2ndPlayerIfPresent() {
    if (auto ped = FindPlayerPed(PED_TYPE_PLAYER2)) {
        CWorld::Remove(ped);
        delete ped;
        CWorld::Players[PED_TYPE_PLAYER2].m_pPed = nullptr;

        auto player1 = FindPlayerPed(PED_TYPE_PLAYER1);
        CClothes::RebuildPlayer(player1, false);
        player1->CantBeKnockedOffBike = false;
    }
    n2PlayerPedInFocus = eFocusedPlayer::NONE;
}

// 0x442980
void CGameLogic::ResetStuffUponResurrection() {
    auto& player = CWorld::Players[CWorld::PlayerInFocus];
    auto playerPed = player.m_pPed;

    CMessages::ClearMessages(false);
    CCarCtrl::ClearInterestingVehicleList();
    CWorld::ClearExcitingStuffFromArea(player.GetPos(), 4000.0f, true);
    PassTime(12 * 60);
    RestorePlayerStuffDuringResurrection(playerPed, playerPed->GetPosition(), playerPed->m_fCurrentRotation * RadiansToDegrees(1.0f));
    SortOutStreamingAndMemory(playerPed->GetPosition(), playerPed->GetHeading());
    TheCamera.m_fCamShakeForce = 0.0f;
    TheCamera.SetMotionBlur(0, 0, 0, 0, 0);
    CPad::GetPad(PED_TYPE_PLAYER1)->StopShaking(0);
    CReferences::RemoveReferencesToPlayer();
    CCarCtrl::CountDownToCarsAtStart = 10;
    CPopulation::m_CountDownToPedsAtStart = 10;
    CPad::GetPad(CWorld::PlayerInFocus)->DisablePlayerControls = 0;

    playerPed->ClearWeapons();
    GameState = GAMELOGIC_STATE_PLAYING;
    TimeOfLastEvent = 0;
}

// used in CGameLogic::DoWeaponStuffAtStartOf2PlayerGame
// 0x441D00
void CGameLogic::StorePedsWeapons(CPed* ped) {
    rng::copy(ped->m_aWeapons, SavedWeapons.begin());
}

// 0x441D30
void CGameLogic::RestorePedsWeapons(CPed* ped) {
    ped->ClearWeapons();
    for (auto& weapon : SavedWeapons) {
        const auto IsModelLoaded = [](int id) { return id == MODEL_INVALID || CStreaming::GetInfo(id).IsLoaded(); };

        if (rng::all_of(weapon.GetWeaponInfo().GetModels(), IsModelLoaded)) { // FIX_BUGS: They checked modelId1 twice
            ped->GiveWeapon(weapon.m_Type, weapon.m_TotalAmmo, true);
        }
    }
}

// 0x442060
void CGameLogic::RestorePlayerStuffDuringResurrection(CPlayerPed* player, CVector posn, float playerStartHeading) {
    ClearSkip(false);

    if (player->m_fHealth <= 0.0f) {
        CStats::UpdateStatsOnRespawn();
    }
    auto playerData = player->m_pPlayerData;
    auto playerInfo = player->GetPlayerInfoForThisPlayerPed();

    player->physicalFlags.bDestroyed = false;
    player->m_fArmour = 0.0f;
    player->m_fHealth = static_cast<float>(playerInfo->m_nMaxHealth);
    player->m_bIsVisible = true;
    player->m_nDeathTimeMS = 0;
    player->bDoBloodyFootprints = false;
    playerData->m_nDrunkenness = 0;
    playerData->m_nFadeDrunkenness = 0;
    CMBlur::ClearDrunkBlur();
    playerData->m_nDrugLevel = 0;
    player->ClearAdrenaline();
    player->ResetSprintEnergy();
    if (auto& fire = player->m_pFire) {
        fire->createdByScript = false;
        fire->Extinguish();
        fire = nullptr;
    }
    player->GetAE().TurnOffJetPack();
    player->bInVehicle = false;
    if (auto vehicle = player->m_pVehicle) {
        CEntity::CleanUpOldReference(vehicle);
    }
    player->m_pVehicle = nullptr;
    player->GetWanted()->Reset();
    player->RestartNonPartialAnims();
    playerInfo->MakePlayerSafe(false, 10'000.0f);
    player->m_bRemoveFromWorld = false;
    player->ClearWeaponTarget();
    player->SetInitialState(false);
    player->GetAcquaintance() = CPedType::GetPedTypeAcquaintances(player->m_nPedType);
    CCarCtrl::ClearInterestingVehicleList();

    player->Teleport(posn + CVector{0.0f, 0.0f, 1.0f}, false);
    player->m_fCurrentRotation = player->m_fAimingRotation = DegreesToRadians(playerStartHeading);
    player->SetHeading(player->m_fCurrentRotation);

    CTheScripts::ClearSpaceForMissionEntity(posn, player);
    CWorld::ClearExcitingStuffFromArea(posn, 4000.0, 1);
    player->RestoreHeadingRate();
    player->m_nAreaCode = AREA_CODE_NORMAL_WORLD;
    player->m_pEnex = 0;
    CEntryExitManager::ms_entryExitStackPosn = 0;
    CGame::currArea = AREA_CODE_NORMAL_WORLD;
    CPopulation::bInPoliceStation = 0;
    CStreaming::RemoveBuildingsNotInArea(AREA_CODE_NORMAL_WORLD);
    TheCamera.SetCameraDirectlyBehindForFollowPed_CamOnAString();
    TheCamera.RestoreWithJumpCut();
    CReferences::RemoveReferencesToPlayer();
    CGarages::PlayerArrestedOrDied();
    CStats::SetStatValue(STAT_KILLS_SINCE_LAST_CHECKPOINT, 0.0);
    CShopping::RemoveLoadedShop();
    CWorld::Remove(player);
    CWorld::Add(player);
    CHud::ResetWastedText();
    CStreaming::StreamZoneModels(posn);
    CPostEffects::m_smokeyEnable = false;
    CTimeCycle::StopExtraColour(0);
    CPostEffects::ScriptResetForEffects();

    if (auto clothesDesc = playerData->m_pPedClothesDesc; clothesDesc->m_anModelKeys[9]) {
        clothesDesc->SetTextureAndModel(nullptr, nullptr, CLOTHES_TEXTURE_SPECIAL);
        CClothes::RebuildPlayer(player, false);
    } else {
        CClothes::RebuildPlayerIfNeeded(player);
    }
    CAudioZones::Update(true, posn);
    AudioEngine.StopPoliceScanner(true);
    CWaterLevel::FindNearestWaterAndItsFlow();
    TheCamera.m_fDistanceToWater = 100'000.0f;
}

// @brief Set wanted level to 4 if player is in a forbidden territory.
// @param immediately   Do position check without waiting for the frame counter.
// @addr 0x441770
void CGameLogic::SetPlayerWantedLevelForForbiddenTerritories(bool immediately) {
    const auto  ped = FindPlayerPed();
    const auto& coords = ped->GetPosition();

    if ((!immediately && (CTimer::GetFrameCounter() % 32) != 18) || coords.z > 950.0f)
        return;

    if (ped->m_pIntelligence->GetTaskSwim() || ped->GetWantedLevel() >= 4)
        return;

    const auto SetWantedIfInArea = [&](auto* vertices, size_t size) {
        if (IsPointWithinLineArea(vertices, size, coords.x, coords.y)) {
            ped->SetWantedLevel(4);
            if (immediately) {
                ped->GetWanted()->m_nLastTimeWantedLevelChanged = 0;
            }
        }
    };

    // 0x8A5EC8; Las Venturas
    constexpr CVector town1ShapeVertices[] = {
        {3000.0f, 535.0f, 0.0f},
        {1759.0f, 576.0f, 0.0f},
        {989.0f, 693.0f, 0.0f},
        {-128.0f, 490.0f, 0.0f},
        {-845.0f, 707.0f, 0.0f},
        {-1477.0f, 1677.0f, 0.0f},
        {-2154.0f, 2497.0f, 0.0f},
        {-2971.0f, 2180.0f, 0.0f},
        {-3000.0f, 3000.0f, 0.0f},
        {3000.0f, 3000.0f, 0.0f}
    };

    // 0x8A5E58; San Fierro
    constexpr CVector town2ShapeVertices[] = {
        {28.0f, -3000.0f, 0.0f},
        {-30.0f, -1280.0f, 0.0f},
        {-148.0f, -911.0f, 0.0f},
        {-487.0f, -372.0f, 0.0f},
        {-1028.0f, -424.0f, 0.0f},
        {-1145.0f, 479.0f, 0.0f},
        {-1461.0f, 1488.0f, 0.0f},
        {-3000.0f, 1668.0f, 0.0f},
        {-3000.0f, -3000.0f, 0.0f}
    };

    // LV
    if (CStats::GetStatValue(STAT_CITY_UNLOCKED) <= 1.0f) {
        SetWantedIfInArea(town1ShapeVertices, std::size(town1ShapeVertices));
    }

    // SF
    if (CStats::GetStatValue(STAT_CITY_UNLOCKED) <= 0.0f) {
        SetWantedIfInArea(town2ShapeVertices, std::size(town2ShapeVertices));
    }
}

// 0x4423C0
void CGameLogic::SetUpSkip(CVector coors, float angle, bool afterMission, CEntity* vehicle, bool finishedByScript) {
    if (SkipState == SKIP_IN_PROGRESS) {
        TheCamera.SetFadeColour(0, 0, 0);
        TheCamera.Fade(0.5f, eFadeFlag::FADE_OUT);
    }
    SkipState = SKIP_NONE;
    CPad::GetPad(PED_TYPE_PLAYER1)->bCamera = false;
    SkipPosition = coors;
    SkipState = afterMission ? SKIP_AFTER_MISSION : SKIP_AVAILABLE;
    SkipTimer = CTimer::GetTimeInMS();
    if (vehicle) {
        CEntity::RegisterReference(SkipVehicle);
    }
    SkipToBeFinishedByScript = finishedByScript;
}

// 0x4415C0
bool CGameLogic::SkipCanBeActivated() {
    if (!CGame::CanSeeOutSideFromCurrArea() || TheCamera.m_bFading || (SkipState != SKIP_AVAILABLE && SkipState != SKIP_AFTER_MISSION))
        return false;

    if (auto vehicle = FindPlayerVehicle()) {
        switch (vehicle->m_nVehicleSubType) {
        case VEHICLE_TYPE_BIKE:
        case VEHICLE_TYPE_AUTOMOBILE:
        case VEHICLE_TYPE_MTRUCK:
        case VEHICLE_TYPE_QUAD:
        case VEHICLE_TYPE_BMX:
            if (!SkipVehicle || SkipVehicle == vehicle) {
                return true;
            }
            break;
        }
    }

    return SkipState == SKIP_AFTER_MISSION && FindPlayerPed()->GetIntelligence()->GetTaskSwim();
}

// 0x441440
void CGameLogic::SortOutStreamingAndMemory(const CVector& translation, float angle) {
    CTimer::Stop();
    RwCameraSetFarClipPlane(TheCamera.m_pRwCamera, CTimeCycle::FindFarClipForCoors(translation));
    CStreaming::FlushRequestList();
    CStreaming::DeleteRwObjectsAfterDeath(translation);
    CStreaming::RemoveUnusedModelsInLoadedList();
    CGame::DrasticTidyUpMemory(true);
    CStreaming::LoadSceneCollision(translation);
    CRenderer::RequestObjectsInDirection(translation, angle, STREAMING_LOADING_SCENE);
    CStreaming::LoadScene(translation);
    CTimer::Update();
}

// 0x441290
void CGameLogic::StopPlayerMovingFromDirection(int32 playerId, CVector direction) {
    if (auto obj = [ped = FindPlayerPed(playerId)]() -> CPhysical* {
        if (ped->IsInVehicle()) {
            return ped->GetVehicleIfInOne();
        } else if (ped->bIsStanding || ped->m_pIntelligence->GetTaskJetPack()) {
            return ped;
        }

        return nullptr;
    }()) {
        direction.z = std::max(0.0f, direction.z);

        if (const auto compSum = (direction * obj->m_vecMoveSpeed).ComponentwiseSum(); compSum < 0.0f) {
            obj->m_vecMoveSpeed -= direction * compSum;
        }
    }
}

// 0x442AD0
void CGameLogic::Update() {
    ZoneScoped;

    CStats::UpdateRespectStat(0);
    CStats::UpdateSexAppealStat();
    SetPlayerWantedLevelForForbiddenTerritories(false);

    if (CCutsceneMgr::ms_cutsceneProcessing)
        return;

    auto& player1 = FindPlayerInfo(PED_TYPE_PLAYER1);
    auto player1Ped = player1.m_pPed;

    const auto Process = [&] {
        const auto PunishPlayer = [&player1, player1Ped](int32 fee) {
            player1.m_nMoney = std::max(player1.m_nMoney - fee, 0);
            player1Ped->ClearWeapons();
        };

        const auto ResetForResurrectionAndFadeOut = [&player1, &player1Ped](CVector& restartPoint, float restartAngle, bool fadeOutNow) {
            player1.m_nPlayerState = PLAYERSTATE_PLAYING;
            GameState = GAMELOGIC_STATE_PLAYING;

            if (auto vehicle = player1Ped->GetVehicleIfInOne()) {
                if (auto& driver = vehicle->m_pDriver; player1Ped == driver) {
                    CEntity::CleanUpOldReference(driver);
                    driver = nullptr;

                    if (vehicle->m_nStatus != STATUS_WRECKED) {
                        vehicle->m_nStatus = STATUS_ABANDONED;
                    }
                } else {
                    vehicle->RemovePassenger(player1Ped);
                }
            }

            CMessages::ClearMessages(false);
            CHud::SetHelpMessage(nullptr, true, false, false);
            CCarCtrl::ClearInterestingVehicleList();
            CGarages::CloseHideOutGaragesBeforeSave();
            CWorld::ClearExcitingStuffFromArea(player1.GetPos(), 4000.0f, true);

            if (IsCoopGameGoingOn()) {
                restartPoint = vec2PlayerStartLocation;
                restartAngle = DegreesToRadians(f2PlayerStartHeading);
                RestorePedsWeapons(player1Ped);
            }
            Remove2ndPlayerIfPresent();
            PassTime(12 * 60);
            FindPlayerInfo().m_nNumHoursDidntEat = 0;
            RestorePlayerStuffDuringResurrection(player1Ped, restartPoint, restartAngle);
            SortOutStreamingAndMemory(player1Ped->GetPosition(), player1Ped->GetHeading());

            TheCamera.m_fCamShakeForce = 0.0f;
            TheCamera.SetMotionBlur(0, 0, 0, 0, 0); // todo: eBlurType enum
            CPad::GetPad(PED_TYPE_PLAYER1)->StopShaking(0);
            CReferences::RemoveReferencesToPlayer();
            CCarCtrl::CountDownToCarsAtStart = 10;
            CPopulation::m_CountDownToPedsAtStart = 10;
            CPad::GetPad(CWorld::PlayerInFocus)->DisablePlayerControls = false;

            if (fadeOutNow) {
                TheCamera.SetFadeColour(0, 0, 0);
                TheCamera.Fade(3.0f, eFadeFlag::FADE_OUT);
            } else {
                fadeOutNow = true;
            }
        };

        const auto FadeInIfNewEvent = [&] {
            if (CTimer::GetPreviousTimeInMS() - TimeOfLastEvent <= 3000) {
                TheCamera.SetFadeColour(0, 0, 0);
                TheCamera.Fade(2.0f, eFadeFlag::FADE_IN);
            }
        };

        for (auto& player : CWorld::Players) {
            auto ped = player.m_pPed;
            if (!ped || player.m_nPlayerState != PLAYERSTATE_PLAYING)
                continue;

            ped->GetTaskManager().GetSimplestActiveTask();
            if (ped->m_nPedState == PEDSTATE_DEAD || ped->m_nPedState == PEDSTATE_DIE && ped->bIsDyingStuck) {
                ped->ClearAdrenaline();
                player.KillPlayer();
                GameState = GAMELOGIC_STATE_WASTED;
                TimeOfLastEvent = CTimer::GetTimeInMS();
            } else if (ped->m_nPedState == PEDSTATE_ARRESTED) {
                ped->ClearAdrenaline();
                player.ArrestPlayer();
                ped->Say(CTX_GLOBAL_ARRESTED, 2300, 1.0f, 1u, 1u);
                GameState = GAMELOGIC_STATE_BUSTED;
                TimeOfLastEvent = CTimer::GetTimeInMS();

                if (ped == FindPlayerPed(PED_TYPE_PLAYER2) && CHud::m_BigMessage[2][0] == '\0') {
                    CMessages::AddBigMessage(TheText.Get("BUSTED"), 5000, STYLE_WHITE_MIDDLE);
                }
            }
        }

        switch (GameState) {
        case GAMELOGIC_STATE_WASTED: {
            CWeaponEffects::ClearCrossHairs();
            if (CTimer::GetTimeInMS() - TimeOfLastEvent <= 3000)
                break;

            FadeInIfNewEvent();

            if (TheCamera.m_bFading)
                break;

            if (!IsCoopGameGoingOn() && bPenaltyForDeathApplies) {
                if (!player1.m_bFreeHealthCare) {
                    PunishPlayer(100);
                } else {
                    player1.m_bFreeHealthCare = false;
                }
            }

            auto posn = player1.GetPos();
            if (auto enEx = player1Ped->m_pEnex) {
                enEx->GetPositionRelativeToOutsideWorld(posn);
            }

            CVector restartPoint;
            float restartAngle;
            CRestart::FindClosestHospitalRestartPoint(posn, restartPoint, restartAngle);

            ResetForResurrectionAndFadeOut(restartPoint, restartAngle, CRestart::bFadeInAfterNextDeath);
            break;
        }
        case GAMELOGIC_STATE_BUSTED: {
            CWeaponEffects::ClearCrossHairs();
            auto delta = CTimer::GetTimeInMS() - TimeOfLastEvent;
            if (delta > 3000) {
                FadeInIfNewEvent();
            }

            if (!CTheScripts::IsPlayerOnAMission() && !CWorld::Players[0].m_nBustedAudioStatus) {
                if (CGeneral::GetRandomNumberInRange(0, 4)) {
                    player1.m_nBustedAudioStatus = 2;
                } else {
                    player1.m_nBustedAudioStatus = 1;
                    player1.m_nLastBustMessageNumber = player1.m_nLastBustMessageNumber % 28 + 1;
                }
            }

            if (delta < 4000 || TheCamera.m_bFading)
                break;

            if (!IsCoopGameGoingOn() && bPenaltyForArrestApplies) {
                if (!player1.m_bGetOutOfJailFree) {
                    const auto fee = [&] {
                        switch (player1Ped->GetWantedLevel()) {
                        case 1:
                            return 100;
                        case 2:
                            return 200;
                        case 3:
                            return 400;
                        case 4:
                            return 600;
                        case 5:
                            return 900;
                        case 6:
                            return 1500;
                        default:
                            NOTSA_UNREACHABLE(); // NOTSA, SA returns 100.
                        }
                    }();
                    PunishPlayer(fee);
                } else {
                    player1.m_bGetOutOfJailFree = false;
                }
            }

            auto posn = player1.GetPos();
            if (auto enEx = player1Ped->m_pEnex) {
                enEx->GetPositionRelativeToOutsideWorld(posn);
            }

            CVector restartPoint;
            float restartAngle;
            CRestart::FindClosestPoliceRestartPoint(posn, restartPoint, restartAngle);
            ResetForResurrectionAndFadeOut(restartPoint, restartAngle, CRestart::bFadeInAfterNextArrest);
            break;
        }
        case GAMELOGIC_STATE_MISSION_FAILED: // really have no idea
        case GAMELOGIC_STATE_MISSION_PASSED: {
            if (CTimer::GetTimeInMS() - TimeOfLastEvent <= 3000)
                break;

            FadeInIfNewEvent();

            if (TheCamera.m_bFading)
                break;

            auto posn = player1.GetPos();
            if (auto enEx = player1Ped->m_pEnex) {
                enEx->GetPositionRelativeToOutsideWorld(posn);
            }

            CVector restartPoint;
            float restartAngle;
            CRestart::FindClosestPoliceRestartPoint(posn, restartPoint, restartAngle);
            ResetForResurrectionAndFadeOut(restartPoint, restartAngle, true);
            break;
        }
        }

        if (bLimitPlayerDistance && IsCoopGameGoingOn()) {
            if (auto diff = FindPlayerCoors(PED_TYPE_PLAYER2) - FindPlayerCoors(PED_TYPE_PLAYER1); diff.Magnitude2D() > MaxPlayerDistance) {
                StopPlayerMovingFromDirection(PED_TYPE_PLAYER1,  diff.Normalized());
                StopPlayerMovingFromDirection(PED_TYPE_PLAYER2, -diff.Normalized());
            }
        }
    };

    if (!IsCoopGameGoingOn()) {
        UpdateSkip();
        Process();
    } else {
        static float& lastPlayerDistance = *(float*)(0x96AB24);
        const auto dist = DistanceBetweenPoints2D(FindPlayerCoors(PED_TYPE_PLAYER1), FindPlayerCoors(PED_TYPE_PLAYER2));

        const auto UpdateTimerAndLastDistance = [&dist] {
            if (IsAPlayerInFocusOn2PlayerGame() && nPrintFocusHelpTimer != 0) {
                nPrintFocusHelpTimer = CTimer::GetTimeInMS() + 30'000;
            }

            lastPlayerDistance = dist;
        };

        if (dist > 100.0f) {
            if (GameState != GAMELOGIC_STATE_MISSION_FAILED) {
                SetMissionFailed();
            }
        } else if (dist > 85.0f && lastPlayerDistance <= 85.0f) {
            CHud::SetHelpMessage(TheText.Get("WRN_2P"), false, false, false); // any further and 2P game will end.
        } else if (dist > MaxPlayerDistance && nPrintFocusHelpCounter < 5) {
            if (lastPlayerDistance > MaxPlayerDistance && (uint32)nPrintFocusHelpTimer < CTimer::GetTimeInMS()) {
                CHud::SetHelpMessage(TheText.Get("WRN2_2P"), false, false, false); // press SELECT to toggle focus to individual player
            }
            else if ((uint32)nPrintFocusHelpTimer < CTimer::GetTimeInMS()) {
                CHud::SetHelpMessage(TheText.Get("WRN1_2P"), false, false, false); // distance limit reached
            }

            nPrintFocusHelpTimer = CTimer::GetTimeInMS() + 60'000;
            nPrintFocusHelpCounter++;
        }

        UpdateTimerAndLastDistance();
        Process();
    }
}

// 0x442480
void CGameLogic::UpdateSkip() {
    if (SkipState == SKIP_AVAILABLE && DistanceBetweenPoints2D(FindPlayerCoors(), SkipPosition) < 25.0f ||
        CanPlayerTripSkip() && CTimer::GetTimeInMS() > SkipTimer + 35'000) {
        SkipState = SKIP_NONE;
        return;
    }

    auto player = FindPlayerPed();
    switch (SkipState) {
    case SKIP_AVAILABLE:
    case SKIP_AFTER_MISSION:
        if (!SkipCanBeActivated() || player->GetTaskManager().FindActiveTaskByType(TASK_COMPLEX_LEAVE_CAR)) {
            return;
        }

        // TRIP SKIP
        if (auto pad = CPad::GetPad(PED_TYPE_PLAYER1); pad->IsDPadRightPressed()) {
            SkipState = SKIP_IN_PROGRESS;
            pad->bCamera = true;

            TheCamera.SetFadeColour(0, 0, 0);
            TheCamera.Fade(2.5f, eFadeFlag::FADE_IN);
            SkipTimer = CTimer::m_snTimeInMilliseconds + 3000;
            CMessages::AddBigMessage(TheText.Get("SKIP"), 4500, STYLE_BOTTOM_RIGHT);
        }
        break;

    case SKIP_IN_PROGRESS: {
        if (CTimer::GetTimeInMS() < SkipTimer)
            return;

        CTimer::Suspend();
        CStreaming::LoadSceneCollision(SkipPosition);
        CStreaming::LoadScene(SkipPosition);
        auto playerDistToSkipPos = DistanceBetweenPoints2D(FindPlayerCoors(), SkipPosition);

        if (auto vehicle = FindPlayerVehicle()) {
            CTheScripts::ClearSpaceForMissionEntity(SkipPosition, vehicle);

            vehicle->Teleport(SkipPosition, false);
            vehicle->SetHeading(DegreesToRadians(SkipHeading));

            if (vehicle->IsBike()) {
                vehicle->AsBike()->PlaceOnRoadProperly();
            } else if (vehicle->IsAutomobile()) {
                vehicle->AsAutomobile()->PlaceOnRoadProperly();
            }

            vehicle->m_vecMoveSpeed = vehicle->GetMatrix().GetForward() / 2.5f; // Make the vehicle appear as it's driving to the point.
            CCamera::GetActiveCamera().m_fHorizontalAngle = vehicle->GetHeading() - FRAC_PI_2;
        } else {
            CTheScripts::ClearSpaceForMissionEntity(SkipPosition, player);
            player->Teleport(SkipPosition, false);
            player->GetPosition().z = CWorld::FindGroundZForCoord(SkipPosition.x, SkipPosition.y) + 1.0f;
        }
        SkipTimer = CTimer::GetTimeInMS() + 1500;
        CHud::SetMessage(nullptr);
        CTimer::Resume();
        PassTime((uint32)(playerDistToSkipPos / 20.0f + 23.0f));

        if (SkipToBeFinishedByScript) {
            SkipState = SKIP_WAITING_SCRIPT;
            CPad::GetPad(PED_TYPE_PLAYER1)->bCamera = false;
        } else {
            TheCamera.SetFadeColour(0, 0, 0);
            TheCamera.Fade(1.5f, eFadeFlag::FADE_OUT);
            SkipState = SKIP_FADING_OUT;
        }
        break;
    }
    case SKIP_FADING_OUT:
        if (CTimer::GetTimeInMS() > SkipTimer) {
            SkipState = SKIP_NONE;
            CPad::GetPad(PED_TYPE_PLAYER1)->bCamera = false;
        }
        break;

    case SKIP_WAITING_SCRIPT:
    default:
        return;
    }
}

// notsa
bool CGameLogic::IsAPlayerInFocusOn2PlayerGame() {
    return n2PlayerPedInFocus == eFocusedPlayer::PLAYER1 || n2PlayerPedInFocus == eFocusedPlayer::PLAYER2;
}

// notsa
CPlayerPed* CGameLogic::GetFocusedPlayerPed() {
    if (!IsAPlayerInFocusOn2PlayerGame()) {
        return nullptr;
    } else {
        return FindPlayerPed((int32)n2PlayerPedInFocus);
    }
}

// notsa
bool CGameLogic::CanPlayerTripSkip() {
    return SkipState == SKIP_AVAILABLE || SkipState == SKIP_AFTER_MISSION;
}

// notsa
void CGameLogic::SetMissionFailed(){
    GameState = GAMELOGIC_STATE_MISSION_FAILED;
    TimeOfLastEvent = CTimer::GetTimeInMS();
}
