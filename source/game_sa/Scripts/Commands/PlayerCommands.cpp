#include <StdInc.h>

#include "Commands.hpp"
#include <CommandParser/Parser.hpp>

#include <TaskTypes/TaskSimplePlayerOnFoot.h>
#include <TaskTypes/TaskSimpleSwim.h>
#include "PedClothesDesc.h"
#include "MBlur.h"
#include "Ropes.h"

#include <RunningScript.h>

using namespace notsa::script;

/*!
* Various player commands
*/
namespace {

/// CREATE_PLAYER(0053)
int32 CreatePlayer(int32 playerId, CVector pos) {
    if (!CStreaming::IsModelLoaded(MODEL_PLAYER)) {
        CStreaming::RequestSpecialModel(0, "player", STREAMING_GAME_REQUIRED | STREAMING_KEEP_IN_MEMORY | STREAMING_PRIORITY_REQUEST);
        CStreaming::LoadAllRequestedModels(true);
    }

    // Create
    CPlayerPed::SetupPlayerPed(playerId);
    auto player = FindPlayerPed(playerId);
    player->SetCharCreatedBy(PED_MISSION);
    CPlayerPed::DeactivatePlayerPed(playerId);

    // Position in the world
    if (pos.z <= MAP_Z_LOW_LIMIT) {
        pos.z = CWorld::FindGroundZForCoord(pos.x, pos.y);
    }
    pos.z += player->GetDistanceFromCentreOfMassToBaseOfModel();
    player->SetPosn(pos);
    CTheScripts::ClearSpaceForMissionEntity(pos, player);
    CPlayerPed::ReactivatePlayerPed(playerId);

    // Set task
    player->GetTaskManager().SetTask(new CTaskSimplePlayerOnFoot(), TASK_PRIMARY_DEFAULT);

    return playerId;
}

/// GET_PLAYER_COORDINATES(0054) - Originally unsupported
CVector GetPlayerCoordinates(CPlayerInfo& pinfo) {
    return pinfo.GetPos();
}

/// IS_PLAYER_IN_AREA_2D(0056) - Originally unsupported
bool IsPlayerInArea2D(CRunningScript* S, CPlayerPed& player, CRect area, bool highlightArea) {
    if (highlightArea) {
        CTheScripts::HighlightImportantArea((uint32)S + (uint32)S->m_IP, area, MAP_Z_LOW_LIMIT);
    }

    if (CTheScripts::DbgFlag) {
        CTheScripts::DrawDebugSquare(area);
    }

    return player.bInVehicle
        ? player.m_pVehicle->IsWithinArea(area.left, area.top, area.right, area.bottom)
        : player.IsWithinArea(area.left, area.top, area.right, area.bottom);
}

/// IS_PLAYER_IN_AREA_3D(0057) - Originally unsupported
bool IsPlayerInArea3D(CRunningScript* S, CPlayerPed& player, CVector p1, CVector p2, bool highlightArea) {
    if (highlightArea) {
        CTheScripts::HighlightImportantArea((uint32)S + (uint32)S->m_IP, p1.x, p1.y, p2.x, p2.y, (p1.z + p2.z) / 2.0f);
    }

    if (CTheScripts::DbgFlag) {
        CTheScripts::DrawDebugCube(p1, p2);
    }

    return player.bInVehicle
        ? player.m_pVehicle->IsWithinArea(p1.x, p1.y, p1.z, p2.x, p2.y, p2.z)
        : player.IsWithinArea(p1.x, p1.y, p1.z, p2.x, p2.y, p2.z);
}

/// IS_PLAYER_PLAYING(0256)
auto IsPlayerPlaying(CPlayerInfo& player) -> notsa::script::CompareFlagUpdate {
    return { player.m_nPlayerState == PLAYERSTATE_PLAYING };
}

/// IS_PLAYER_CLIMBING(0A29)
bool IsPlayerClimbing(CPlayerPed& player) {
    return player.GetIntelligence()->GetTaskClimb();
}

/// SET_SWIM_SPEED(0A28)
void SetSwimSpeed(CPlayerPed& player, float speed) {
    if (auto swim = player.GetIntelligence()->GetTaskSwim()) {
        swim->m_fAnimSpeed = speed;
    }
}

/// SET_PLAYER_GROUP_TO_FOLLOW_ALWAYS(0A20)
void SetPlayerGroupToFollowAlways(CPlayerPed& player, bool enable) {
    player.ForceGroupToAlwaysFollow(enable);
}

/// SET_PLAYER_DUCK_BUTTON(082A)
void SetPlayerAbleToUseCrouch(uint32 playerIdx, bool enable) {
    CPad::GetPad(playerIdx)->bDisablePlayerDuck = !enable;
}

// IS_PLAYER_TOUCHING_OBJECT(0178) - Originally unsupported
bool IsPlayerTouchingObject(CPlayerPed& player, CObject& object) {
    return GetPedOrItsVehicle(player).GetHasCollidedWith(&object);
}

// IS_PLAYER_TOUCHING_OBJECT_ON_FOOT(023A) - Originally unsupported
bool IsPlayerTouchingObjectOnFoot(CPlayerPed& player, CObject& object) {
    return player.GetHasCollidedWith(&object);
}

/// ADD_SCORE(0109)
void AddScore(CPlayerInfo& player, int32 score) {
    player.m_nMoney += score;
}

/// IS_SCORE_GREATER(010A)
bool IsScoreGreater(CPlayerInfo& player, int32 score) {
    return player.m_nMoney > score;
}

/// STORE_SCORE(010B)
int32 StoreScore(CPlayerInfo& player) {
    return player.m_nMoney;
}

/// ALTER_WANTED_LEVEL(010D)
void AlterPlayerWantedLevel(CPlayerPed& player, int32 level) {
    player.SetWantedLevel(level);
}

/// ALTER_WANTED_LEVEL_NO_DROP(010E)
void AlterPlayerWantedLevelNoDrop(CPlayerPed& player, int32 level) {
    player.SetWantedLevelNoDrop(level);
}

/// IS_WANTED_LEVEL_GREATER(010F)
bool IsWantedLevelGreater(CPlayerPed& player, int32 level) {
    return (int32)player.GetWanted()->GetWantedLevel() > level;
}

/// CLEAR_WANTED_LEVEL(0110)
void ClearWantedLevel(CPlayerPed& player) {
    player.GetWanted()->SetWantedLevel(0);
}

/// SET_MAX_WANTED_LEVEL(01F0)
void SetMaxWantedLevel(int32 level) {
    CWanted::SetMaximumWantedLevel(level);
}

/// IS_PLAYER_DEAD(0117)
bool IsPlayerDead(CPlayerInfo& player) {
    return player.m_nPlayerState == PLAYERSTATE_HAS_DIED;
}

/// GET_PLAYER_CHAR(01F5)
int32 GetPlayerChar(CPlayerPed& player) {
    return GetPedPool()->GetRef(&player);
}

/// IS_PLAYER_PRESSING_HORN(0122)
bool IsPlayerPressingHorn(uint32 playerIdx) {
    return FindPlayerPed(playerIdx)->GetPedState() == PEDSTATE_DRIVING && CPad::GetPad(playerIdx)->GetHorn();
}

/// SET_PLAYER_CONTROL(01B4)
void SetPlayerControl(CPlayerInfo& player, bool state) {
    player.MakePlayerSafe(!state, 10.0);
}

/// SET_POLICE_IGNORE_PLAYER(01F7)
void SetPoliceIgnorePlayer(CPlayerPed& player, bool state) {
    player.GetWanted()->m_bPoliceBackOff = state;
    if (state) {
        CWorld::StopAllLawEnforcersInTheirTracks();
    }
}

/// SET_EVERYONE_IGNORE_PLAYER(03BF)
void SetEveryoneIgnorePlayer(CPlayerPed& player, bool state) {
    player.GetWanted()->m_bEverybodyBackOff = state;
    if (state) {
        CWorld::StopAllLawEnforcersInTheirTracks();
    }
}

/// APPLY_BRAKES_TO_PLAYERS_CAR(0221)
void ApplyBrakesToPlayersCar(uint32 playerIdx, bool state) {
    CPad::GetPad(playerIdx)->bApplyBrakes = state;
}

/// IS_PLAYER_IN_REMOTE_MODE(0241)
bool IsPlayerInRemoteMode(CPlayerInfo& player) {
    return player.IsPlayerInRemoteMode();
}

/// GET_NUM_OF_MODELS_KILLED_BY_PLAYER(0298)
int16 GetNumOfModelsKilledByPlayer(uint32 playerIdx, eModelID model) {
    return CDarkel::QueryModelsKilledByPlayer(model, playerIdx);
}

/// RESET_NUM_OF_MODELS_KILLED_BY_PLAYER(0297)
void ResetNumOfModelsKilledByPlayer(uint32 playerIdx) {
    CDarkel::ResetModelsKilledByPlayer(playerIdx);
}

/// SET_PLAYER_NEVER_GETS_TIRED(0330)
void SetPlayerNeverGetsTired(CPlayerInfo& player, bool state) {
    player.m_bDoesNotGetTired = state;
}

/// SET_PLAYER_FAST_RELOAD(0331)
void SetPlayerFastReload(CPlayerInfo& player, bool state) {
    player.m_bFastReload = state;
}

/// CAN_PLAYER_START_MISSION(03EE)
bool CanPlayerStartMission(CPlayerPed& player) {
    return player.CanPlayerStartMission();
}

/// MAKE_PLAYER_SAFE_FOR_CUTSCENE(03EF)
void MakePlayerSafeForCutscene(uint32 playerIdx) {
    CPad::GetPad(playerIdx)->bPlayerSafeForCutscene = true;
    FindPlayerInfo(playerIdx).MakePlayerSafe(true, 10000.0f);
    CCutsceneMgr::ms_cutsceneProcessing = true;
}

/// IS_PLAYER_TARGETTING_CHAR(0457)
bool IsPlayerTargettingChar(CPlayerPed& player, CPed* target) {
    CEntity* targetedObject = player.m_pTargetedObject;
    if (targetedObject && targetedObject->IsPed() && targetedObject == target) {
        return true;
    }

    if (CCamera::m_bUseMouse3rdPerson && player.GetPadFromPlayer()->GetTarget()) {
        return player.m_p3rdPersonMouseTarget && player.m_p3rdPersonMouseTarget == target;
    }

    return false;
}

/// IS_PLAYER_TARGETTING_OBJECT(0458)
bool IsPlayerTargettingObject(CPlayerPed& player, CObject* target) {
    CEntity* targetedObject = player.m_pTargetedObject;
    return targetedObject && targetedObject->IsObject() && targetedObject == target;
}

/// GIVE_REMOTE_CONTROLLED_MODEL_TO_PLAYER(046E)
void GiveRemoteControlledModelToPlayer(CPlayerPed& player, CVector posn, float angle, eModelID model) {
    if (posn.z <= MAP_Z_LOW_LIMIT) {
        posn.z = CWorld::FindGroundZForCoord(posn.x, posn.y);
    }
    CRemote::GivePlayerRemoteControlledCar(posn, DegreesToRadians(angle), model);
}

/// MAKE_PLAYER_FIRE_PROOF(055D)
void MakePlayerFireProof(CPlayerInfo& player, bool state) {
    player.m_bFireProof = state;
}

/// GET_PLAYER_MAX_ARMOUR(0945)
uint8 GetPlayerMaxArmour(CPlayerInfo& player) {
    return player.m_nMaxArmour;
}

/// SET_PLAYER_MOOD(04E3)
void SetPlayerMood(CPlayerPed& player, eCJMood mood, uint32 time) {
    CAEPedSpeechAudioEntity::SetCJMood(mood, time);
}

/// IS_PLAYER_WEARING(0500)
bool IsPlayerWearing(CPlayerPed& player, eClothesTexturePart bodyPart, const char* textureName) {
    return player.GetClothesDesc()->m_anTextureKeys[bodyPart] == CKeyGen::GetUppercaseKey(textureName);
}

/// SET_PLAYER_CAN_DO_DRIVE_BY(0501)
void SetPlayerCanDoDriveBy(CPlayerInfo& player, bool state) {
    player.m_bCanDoDriveBy = state;
}

/// SET_PLAYER_DRUNKENNESS(052C)
void SetPlayerDrunkenness(CPlayerInfo& player, uint8 intensity) {
    player.m_PlayerData.m_nDrunkenness     = intensity;
    player.m_PlayerData.m_nFadeDrunkenness = 0;
    if (!intensity) {
        CMBlur::ClearDrunkBlur();
    }
}

/// INCREASE_PLAYER_MAX_HEALTH(055E)
void IncreasePlayerMaxHealth(CPlayerInfo& player, uint8 value) {
    player.m_nMaxHealth += value;
    player.m_pPed->m_fHealth = player.m_nMaxHealth;
}

/// INCREASE_PLAYER_MAX_ARMOUR(055F)
void IncreasePlayerMaxArmour(CPlayerInfo& player, uint8 value) {
    player.m_nMaxArmour += value;
    player.m_pPed->m_fArmour = player.m_nMaxArmour;
}

/// ENSURE_PLAYER_HAS_DRIVE_BY_WEAPON(0563)
void EnsurePlayerHasDriveByWeapon(CPlayerPed& player, uint32 ammo) {
    if (!player.bInVehicle) {
        return;
    }
    const auto type = player.GetWeaponInSlot(eWeaponSlot::SMG).GetType();
    if (type != WEAPON_UNARMED) {
        if (player.GetWeaponInSlot(eWeaponSlot::SMG).GetTotalAmmo() < ammo) {
            player.SetAmmo(type, ammo);
        }
        return;
    }

    player.GiveWeapon(eWeaponType::WEAPON_MICRO_UZI, ammo, true);
    if (player.GetSavedWeapon() == eWeaponType::WEAPON_UNIDENTIFIED) {
        player.SetSavedWeapon(player.GetActiveWeapon().GetType());
    }
    player.SetCurrentWeapon(eWeaponType::WEAPON_MICRO_UZI);
}

/// IS_PLAYER_IN_INFO_ZONE(0583)
bool IsPlayerInInfoZone(CPlayerInfo& player, const char* zoneName) {
    return CTheZones::FindZone(player.GetPos(), zoneName, eZoneType::ZONE_TYPE_INFO);
}

/// IS_PLAYER_TARGETTING_ANYTHING(068C)
bool IsPlayerTargettingAnything(CPlayerPed& player) {
    return player.m_pTargetedObject;
}

/// DISABLE_PLAYER_SPRINT(06AF)
void DisablePlayerSprint(CPlayerInfo& player, bool state) {
    player.m_PlayerData.m_bPlayerSprintDisabled = state;
}

/// DELETE_PLAYER(06DF)
void DeletePlayer(uint32 playerIdx) {
    CPlayerPed::RemovePlayerPed(playerIdx);
}

/// SET_TWO_PLAYER_CAMERA_MODE(06E0)
void SetTwoPlayerCameraMode(int32 unused) {
    TheCamera.StartCooperativeCamMode();
}

/// LIMIT_TWO_PLAYER_DISTANCE(06F1)
void LimitTwoPlayerDistance(float limit) {
    CGameLogic::bLimitPlayerDistance = true;
    CGameLogic::MaxPlayerDistance    = limit;
}

/// RELEASE_TWO_PLAYER_DISTANCE(06F2)
void ReleaseTwoPlayerDistance() {
    CGameLogic::bLimitPlayerDistance = false;
}

/// SET_PLAYER_PLAYER_TARGETTING(06F3)
void SetPlayerPlayerTargetting(bool state) {
    CGameLogic::bPlayersCannotTargetEachOther = !state;
}

/// SET_PLAYERS_CAN_BE_IN_SEPARATE_CARS(06FA)
void SetPlayersCanBeInSeparateCars(bool state) {
    CGameLogic::bPlayersCanBeInSeparateCars = state;
}

/// BUILD_PLAYER_MODEL(070D)
void BuildPlayerModel(CPlayerPed* player) {
    CClothes::RebuildPlayer(player, false);
    CReplay::Init();
}

/// GIVE_PLAYER_CLOTHES(0784)
void GivePlayerClothes(CPlayerPed& player, notsa::script::Hash textureHash, notsa::script::Hash modelHash, eClothesTexturePart bodyPart) {
    player.GetClothesDesc()->SetTextureAndModel(textureHash, modelHash, bodyPart);
}

/// PLAYER_ENTERED_BUILDINGSITE_CRANE(079E)
void PlayerEnteredBuildingsiteCrane() {
    CRopes::PlayerControlsCrane    = eControlledCrane::WRECKING_BALL;
    CWaterLevel::m_bWaterFogScript = false;
}

/// PLAYER_ENTERED_DOCK_CRANE(079D)
void PlayerEnteredDockCrane() {
    CRopes::PlayerControlsCrane    = eControlledCrane::MAGNO_CRANE;
    CWaterLevel::m_bWaterFogScript = false;
}

/// PLAYER_ENTERED_LAS_VEGAS_CRANE(07FA)
void PlayerEnteredLasVegasCrane() {
    CRopes::PlayerControlsCrane    = eControlledCrane::LAS_VEGAS_CRANE;
    CWaterLevel::m_bWaterFogScript = false;
}

/// PLAYER_ENTERED_QUARRY_CRANE(07F9)
void PlayerEnteredQuarryCrane() {
    CRopes::PlayerControlsCrane = eControlledCrane::QUARRY_CRANE;
}

/// PLAYER_LEFT_CRANE(079F)
void PlayerLeftCrane() {
    CRopes::PlayerControlsCrane    = eControlledCrane::NONE;
    CWaterLevel::m_bWaterFogScript = true;
}

/// GET_PLAYER_GROUP(07AF)
int32 GetPlayerGroup(CPlayerInfo& player) {
    return CTheScripts::GetUniqueScriptThingIndex(player.m_PlayerData.m_nPlayerGroup, eScriptThingType::SCRIPT_THING_PED_GROUP);
}

/// SET_PLAYER_GROUP_RECRUITMENT(07B4)
void SetPlayerGroupRecruitment(CPlayerInfo& player, bool state) {
    player.m_PlayerData.m_bGroupStuffDisabled = !state;
}

/// SET_PLAYER_ENTER_CAR_BUTTON(07CC)
void SetPlayerEnterCarButton(uint32 playerIdx, bool state) {
    CPad::GetPad(playerIdx)->bDisablePlayerEnterCar = !state;
}

/// IS_PLAYER_PERFORMING_WHEELIE(07F1)
bool IsPlayerPerformingWheelie(CPlayerInfo& player) {
    return player.m_pPed->bInVehicle && player.m_pPed->m_pVehicle->GetVehicleAppearance() == eVehicleAppearance::VEHICLE_APPEARANCE_BIKE && player.m_nBikeRearWheelCounter;
}

/// IS_PLAYER_PERFORMING_STOPPIE(07F2)
bool IsPlayerPerformingStoppie(CPlayerInfo& player) {
    return player.m_pPed->bInVehicle && player.m_pPed->m_pVehicle->GetVehicleAppearance() == eVehicleAppearance::VEHICLE_APPEARANCE_BIKE && player.m_nBikeFrontWheelCounter;
}

/// SET_PLAYER_FIRE_BUTTON(0881)
void SetPlayerFireButton(uint32 playerIdx, bool state) {
    CPad::GetPad(playerIdx)->bDisablePlayerFireWeapon = !state;
}

/// SET_PLAYER_JUMP_BUTTON(0901)
void SetPlayerJumpButton(uint32 playerIdx, bool state) {
    CPad::GetPad(playerIdx)->bDisablePlayerJump = !state;
}

/// HAS_PLAYER_BOUGHT_ITEM(0942)
bool HasPlayerBoughtItem(uint32 itemId) {
    return CShopping::HasPlayerBought(itemId);
}

};

void notsa::script::commands::player::RegisterHandlers() {
    REGISTER_COMMAND_HANDLER_BEGIN("Player");

    REGISTER_COMMAND_HANDLER(COMMAND_CREATE_PLAYER, CreatePlayer);
    REGISTER_COMMAND_HANDLER(COMMAND_ADD_SCORE, AddScore);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_SCORE_GREATER, IsScoreGreater);
    REGISTER_COMMAND_HANDLER(COMMAND_STORE_SCORE, StoreScore);
    REGISTER_COMMAND_HANDLER(COMMAND_ALTER_WANTED_LEVEL, AlterPlayerWantedLevel);
    REGISTER_COMMAND_HANDLER(COMMAND_ALTER_WANTED_LEVEL_NO_DROP, AlterPlayerWantedLevelNoDrop);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_WANTED_LEVEL_GREATER, IsWantedLevelGreater);
    REGISTER_COMMAND_HANDLER(COMMAND_CLEAR_WANTED_LEVEL, ClearWantedLevel);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_PLAYER_DEAD, IsPlayerDead);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_PLAYER_PRESSING_HORN, IsPlayerPressingHorn);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_PLAYER_CONTROL, SetPlayerControl);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_MAX_WANTED_LEVEL, SetMaxWantedLevel);
    REGISTER_COMMAND_HANDLER(COMMAND_GET_PLAYER_CHAR, GetPlayerChar);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_POLICE_IGNORE_PLAYER, SetPoliceIgnorePlayer);
    REGISTER_COMMAND_HANDLER(COMMAND_APPLY_BRAKES_TO_PLAYERS_CAR, ApplyBrakesToPlayersCar);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_PLAYER_IN_REMOTE_MODE, IsPlayerInRemoteMode);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_PLAYER_PLAYING, IsPlayerPlaying);
    REGISTER_COMMAND_HANDLER(COMMAND_RESET_NUM_OF_MODELS_KILLED_BY_PLAYER, ResetNumOfModelsKilledByPlayer);
    REGISTER_COMMAND_HANDLER(COMMAND_GET_NUM_OF_MODELS_KILLED_BY_PLAYER, GetNumOfModelsKilledByPlayer);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_PLAYER_NEVER_GETS_TIRED, SetPlayerNeverGetsTired);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_PLAYER_FAST_RELOAD, SetPlayerFastReload);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_EVERYONE_IGNORE_PLAYER, SetEveryoneIgnorePlayer);
    REGISTER_COMMAND_HANDLER(COMMAND_CAN_PLAYER_START_MISSION, CanPlayerStartMission);
    REGISTER_COMMAND_HANDLER(COMMAND_MAKE_PLAYER_SAFE_FOR_CUTSCENE, MakePlayerSafeForCutscene);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_PLAYER_TARGETTING_CHAR, IsPlayerTargettingChar);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_PLAYER_TARGETTING_OBJECT, IsPlayerTargettingObject);
    REGISTER_COMMAND_HANDLER(COMMAND_GIVE_REMOTE_CONTROLLED_MODEL_TO_PLAYER, GiveRemoteControlledModelToPlayer);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_PLAYER_MOOD, SetPlayerMood);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_PLAYER_WEARING, IsPlayerWearing);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_PLAYER_CAN_DO_DRIVE_BY, SetPlayerCanDoDriveBy);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_PLAYER_DRUNKENNESS, SetPlayerDrunkenness);
    REGISTER_COMMAND_HANDLER(COMMAND_MAKE_PLAYER_FIRE_PROOF, MakePlayerFireProof);
    REGISTER_COMMAND_HANDLER(COMMAND_INCREASE_PLAYER_MAX_HEALTH, IncreasePlayerMaxHealth);
    REGISTER_COMMAND_HANDLER(COMMAND_INCREASE_PLAYER_MAX_ARMOUR, IncreasePlayerMaxArmour);
    REGISTER_COMMAND_HANDLER(COMMAND_ENSURE_PLAYER_HAS_DRIVE_BY_WEAPON, EnsurePlayerHasDriveByWeapon);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_PLAYER_IN_INFO_ZONE, IsPlayerInInfoZone);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_PLAYER_TARGETTING_ANYTHING, IsPlayerTargettingAnything);
    REGISTER_COMMAND_HANDLER(COMMAND_DISABLE_PLAYER_SPRINT, DisablePlayerSprint);
    REGISTER_COMMAND_HANDLER(COMMAND_DELETE_PLAYER, DeletePlayer);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_TWO_PLAYER_CAMERA_MODE, SetTwoPlayerCameraMode);
    REGISTER_COMMAND_HANDLER(COMMAND_LIMIT_TWO_PLAYER_DISTANCE, LimitTwoPlayerDistance);
    REGISTER_COMMAND_HANDLER(COMMAND_RELEASE_TWO_PLAYER_DISTANCE, ReleaseTwoPlayerDistance);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_PLAYER_PLAYER_TARGETTING, SetPlayerPlayerTargetting);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_PLAYERS_CAN_BE_IN_SEPARATE_CARS, SetPlayersCanBeInSeparateCars);
    REGISTER_COMMAND_HANDLER(COMMAND_BUILD_PLAYER_MODEL, BuildPlayerModel);
    REGISTER_COMMAND_HANDLER(COMMAND_GIVE_PLAYER_CLOTHES, GivePlayerClothes);
    REGISTER_COMMAND_HANDLER(COMMAND_PLAYER_ENTERED_DOCK_CRANE, PlayerEnteredDockCrane);
    REGISTER_COMMAND_HANDLER(COMMAND_PLAYER_ENTERED_BUILDINGSITE_CRANE, PlayerEnteredBuildingsiteCrane);
    REGISTER_COMMAND_HANDLER(COMMAND_PLAYER_LEFT_CRANE, PlayerLeftCrane);
    REGISTER_COMMAND_HANDLER(COMMAND_GET_PLAYER_GROUP, GetPlayerGroup);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_PLAYER_GROUP_RECRUITMENT, SetPlayerGroupRecruitment);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_PLAYER_ENTER_CAR_BUTTON, SetPlayerEnterCarButton);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_PLAYER_PERFORMING_WHEELIE, IsPlayerPerformingWheelie);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_PLAYER_PERFORMING_STOPPIE, IsPlayerPerformingStoppie);
    REGISTER_COMMAND_HANDLER(COMMAND_PLAYER_ENTERED_QUARRY_CRANE, PlayerEnteredQuarryCrane);
    REGISTER_COMMAND_HANDLER(COMMAND_PLAYER_ENTERED_LAS_VEGAS_CRANE, PlayerEnteredLasVegasCrane);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_PLAYER_DUCK_BUTTON, SetPlayerAbleToUseCrouch);
    REGISTER_COMMAND_HANDLER(COMMAND_GET_PLAYER_MAX_ARMOUR, GetPlayerMaxArmour);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_PLAYER_GROUP_TO_FOLLOW_ALWAYS, SetPlayerGroupToFollowAlways);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_SWIM_SPEED, SetSwimSpeed);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_PLAYER_CLIMBING, IsPlayerClimbing);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_PLAYER_FIRE_BUTTON, SetPlayerFireButton);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_PLAYER_JUMP_BUTTON, SetPlayerJumpButton);
    REGISTER_COMMAND_HANDLER(COMMAND_HAS_PLAYER_BOUGHT_ITEM, HasPlayerBoughtItem);

    // -----------------------------[ NOP ]-----------------------------
    REGISTER_COMMAND_NOP(COMMAND_GIVE_REMOTE_CONTROLLED_CAR_TO_PLAYER);
    REGISTER_COMMAND_NOP(COMMAND_SHUT_PLAYER_UP, uint32, uint32);
    REGISTER_COMMAND_NOP(COMMAND_IS_PLAYER_IN_SHORTCUT_TAXI, uint32);
    REGISTER_COMMAND_NOP(COMMAND_SET_PLAYER_HOOKER);
    REGISTER_COMMAND_NOP(COMMAND_SET_JAMES_CAR_ON_PATH_TO_PLAYER);
    REGISTER_COMMAND_NOP(COMMAND_RESET_HAVOC_CAUSED_BY_PLAYER);
    REGISTER_COMMAND_NOP(COMMAND_GET_HAVOC_CAUSED_BY_PLAYER);
    REGISTER_COMMAND_NOP(COMMAND_SET_PLAYER_AUTO_AIM);
    REGISTER_COMMAND_NOP(COMMAND_CHECK_FOR_PED_MODEL_AROUND_PLAYER);
    REGISTER_COMMAND_NOP(COMMAND_SET_PLAYER_HAS_MET_DEBBIE_HARRY);
    REGISTER_COMMAND_NOP(COMMAND_GET_BUS_FARES_COLLECTED_BY_PLAYER);
    REGISTER_COMMAND_NOP(COMMAND_SET_GANG_ATTACK_PLAYER_WITH_COPS);
    REGISTER_COMMAND_NOP(COMMAND_TASK_PLAYER_ON_FOOT);
    REGISTER_COMMAND_NOP(COMMAND_TASK_PLAYER_IN_CAR);
    REGISTER_COMMAND_NOP(COMMAND_GET_CLOSEST_BUYABLE_OBJECT_TO_PLAYER);
    REGISTER_COMMAND_NOP(COMMAND_CLEAR_TWO_PLAYER_CAMERA_MODE);
    REGISTER_COMMAND_NOP(COMMAND_SET_PLAYER_PASSENGER_CAN_SHOOT);
    REGISTER_COMMAND_NOP(COMMAND_SWITCH_PLAYER_CROSSHAIR);
    REGISTER_COMMAND_NOP(COMMAND_GIVE_PLAYER_TATTOO);
    REGISTER_COMMAND_NOP(COMMAND_SET_TWO_PLAYER_CAM_MODE_SEPARATE_CARS);
    REGISTER_COMMAND_NOP(COMMAND_SET_TWO_PLAYER_CAM_MODE_SAME_CAR_SHOOTING);
    REGISTER_COMMAND_NOP(COMMAND_SET_TWO_PLAYER_CAM_MODE_SAME_CAR_NO_SHOOTING);
    REGISTER_COMMAND_NOP(COMMAND_SET_TWO_PLAYER_CAM_MODE_NOT_BOTH_IN_CAR);
    REGISTER_COMMAND_NOP(COMMAND_SET_PLAYER_CAN_BE_DAMAGED);
    REGISTER_COMMAND_NOP(COMMAND_GET_PLAYERS_GANG_IN_CAR_ACTIVE);
    REGISTER_COMMAND_NOP(COMMAND_SET_PLAYERS_GANG_IN_CAR_ACTIVE);
    REGISTER_COMMAND_NOP(COMMAND_SET_PLAYER_FIRE_WITH_SHOULDER_BUTTON);
    REGISTER_COMMAND_NOP(COMMAND_PLAYER_PUT_ON_GOGGLES);
    REGISTER_COMMAND_NOP(COMMAND_SET_RENDER_PLAYER_WEAPON);

    // -------------------------[ UNSUPPORTED ]-------------------------
    REGISTER_UNSUPPORTED_COMMAND_HANDLER(COMMAND_GET_PLAYER_COORDINATES, GetPlayerCoordinates);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_PLAYER_COORDINATES);
    REGISTER_UNSUPPORTED_COMMAND_HANDLER(COMMAND_IS_PLAYER_IN_AREA_2D, IsPlayerInArea2D);
    REGISTER_UNSUPPORTED_COMMAND_HANDLER(COMMAND_IS_PLAYER_IN_AREA_3D, IsPlayerInArea3D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_STORE_CAR_PLAYER_IS_IN);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_PLAYER_IN_CAR);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_PLAYER_IN_MODEL);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_PLAYER_IN_ANY_CAR);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_LOCATE_PLAYER_ANY_MEANS_2D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_LOCATE_PLAYER_ON_FOOT_2D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_LOCATE_PLAYER_IN_CAR_2D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_LOCATE_STOPPED_PLAYER_ANY_MEANS_2D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_LOCATE_STOPPED_PLAYER_ON_FOOT_2D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_LOCATE_STOPPED_PLAYER_IN_CAR_2D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_LOCATE_PLAYER_ANY_MEANS_3D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_LOCATE_PLAYER_ON_FOOT_3D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_LOCATE_PLAYER_IN_CAR_3D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_LOCATE_STOPPED_PLAYER_ANY_MEANS_3D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_LOCATE_STOPPED_PLAYER_ON_FOOT_3D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_LOCATE_STOPPED_PLAYER_IN_CAR_3D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_WARP_PLAYER_FROM_CAR_TO_COORD);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_GET_PLAYER_HEADING);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_PLAYER_HEADING);
    REGISTER_UNSUPPORTED_COMMAND_HANDLER(COMMAND_IS_PLAYER_TOUCHING_OBJECT, IsPlayerTouchingObject);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_PLAYER_HEALTH_GREATER);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_PLAYER_IN_AREA_ON_FOOT_2D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_PLAYER_IN_AREA_IN_CAR_2D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_PLAYER_STOPPED_IN_AREA_2D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_PLAYER_STOPPED_IN_AREA_ON_FOOT_2D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_PLAYER_STOPPED_IN_AREA_IN_CAR_2D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_PLAYER_IN_AREA_ON_FOOT_3D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_PLAYER_IN_AREA_IN_CAR_3D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_PLAYER_STOPPED_IN_AREA_3D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_PLAYER_STOPPED_IN_AREA_ON_FOOT_3D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_PLAYER_STOPPED_IN_AREA_IN_CAR_3D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_GIVE_WEAPON_TO_PLAYER);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_CURRENT_PLAYER_WEAPON);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_TURN_PLAYER_TO_FACE_COORD);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_PLAYER_AS_LEADER);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_LOCATE_PLAYER_ANY_MEANS_CAR_2D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_LOCATE_PLAYER_ON_FOOT_CAR_2D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_LOCATE_PLAYER_IN_CAR_CAR_2D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_LOCATE_PLAYER_ANY_MEANS_CAR_3D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_LOCATE_PLAYER_ON_FOOT_CAR_3D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_LOCATE_PLAYER_IN_CAR_CAR_3D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_TURN_PLAYER_TO_FACE_CHAR);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_PLAYER_HEALTH);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_GET_PLAYER_HEALTH);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_STOP_PLAYER_LOOKING);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_GANG_PLAYER_ATTITUDE);
    REGISTER_UNSUPPORTED_COMMAND_HANDLER(COMMAND_IS_PLAYER_TOUCHING_OBJECT_ON_FOOT, IsPlayerTouchingObjectOnFoot);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_ANIM_GROUP_FOR_PLAYER);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_PLAYER_HEED_THREATS);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_PLAYER_STOPPED);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_PLAYER_IN_ANGLED_AREA_2D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_PLAYER_IN_ANGLED_AREA_ON_FOOT_2D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_PLAYER_IN_ANGLED_AREA_IN_CAR_2D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_PLAYER_STOPPED_IN_ANGLED_AREA_2D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_PLAYER_STOPPED_IN_ANGLED_AREA_ON_FOOT_2D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_PLAYER_STOPPED_IN_ANGLED_AREA_IN_CAR_2D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_PLAYER_IN_ANGLED_AREA_3D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_PLAYER_IN_ANGLED_AREA_ON_FOOT_3D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_PLAYER_IN_ANGLED_AREA_IN_CAR_3D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_PLAYER_STOPPED_IN_ANGLED_AREA_3D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_PLAYER_STOPPED_IN_ANGLED_AREA_ON_FOOT_3D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_PLAYER_STOPPED_IN_ANGLED_AREA_IN_CAR_3D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_PLAYER_SHOOTING_IN_AREA);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_CURRENT_PLAYER_WEAPON);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_PLAYER_IN_TAXI);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_PLAYER_SHOOTING);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_EXPLODE_PLAYER_HEAD);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_PLAYER_VISIBLE);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_ADD_ARMOUR_TO_PLAYER);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_START_KILL_FRENZY_HEADSHOT);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_ACTIVATE_MILITARY_CRANE);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_WARP_PLAYER_INTO_CAR);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_GIVE_PLAYER_DETONATOR);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_STORE_CAR_PLAYER_IS_IN_NO_SAVE);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_MAKE_PLAYER_UNSAFE);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_MAKE_PLAYER_SAFE);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_GET_AMMO_IN_PLAYER_WEAPON);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_PLAYER_SITTING_IN_CAR);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_PLAYER_SITTING_IN_ANY_CAR);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_PLAYER_LIFTING_A_PHONE);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_PLAYER_ON_FOOT);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_ENABLE_PLAYER_CONTROL_CAMERA);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_PLAYER_TARGETTING_ANY_CHAR);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_GET_CURRENT_PLAYER_WEAPON);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_PLAYER_ON_ANY_BIKE);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_HAS_PLAYER_GOT_WEAPON);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_PLAYER_FACING_CHAR);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_PLAYER_STANDING_ON_A_VEHICLE);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_PLAYER_FOOT_DOWN);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_PLAYER_IN_ANY_BOAT);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_PLAYER_IN_ANY_HELI);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_PLAYER_IN_ANY_PLANE);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_PLAYER_IN_FLYING_VEHICLE);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_GET_PLAYER_DRUNKENNESS);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_PLAYER_DRUG_LEVEL);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_GET_PLAYER_DRUG_LEVEL);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_PLAYER_TOUCHING_VEHICLE);
}
