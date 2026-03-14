#pragma once

#include <Weapon.h>
#include <Vector.h>
#include <Enums/eLevelName.h>

class CEntity;
class CPed;
class CVehicle;

enum eSkipState {
    SKIP_NONE,
    SKIP_AVAILABLE,
    SKIP_IN_PROGRESS,
    SKIP_FADING_OUT,
    SKIP_AFTER_MISSION,
    SKIP_WAITING_SCRIPT
};

enum eGameLogicState {
    GAMELOGIC_STATE_PLAYING,
    GAMELOGIC_STATE_WASTED,
    GAMELOGIC_STATE_BUSTED,
    GAMELOGIC_STATE_MISSION_FAILED,
    GAMELOGIC_STATE_MISSION_PASSED
};

// Used in CGameLogic::n2PlayerPedInFocus
enum class eFocusedPlayer : int32 {
    PLAYER1,
    PLAYER2,
    NONE
};

class CPed;

class CGameLogic {
public:
    static inline auto& SavedWeapons = StaticRef<std::array<CWeapon, NUM_WEAPON_SLOTS>>(0x96A9B8);

    static inline auto& AfterDeathStartPointOrientations = StaticRef<std::array<float, 16>>(0x96A850);
    static inline auto& AfterDeathStartPoints = StaticRef<std::array<CVector, 16>>(0x96A8E0);

    static inline auto& SkipPosition = StaticRef<CVector>(0x96A8D4);
    static inline auto& SkipHeading = StaticRef<float>(0x96A8A4);

    static inline auto& nPrintFocusHelpCounter = StaticRef<int32>(0x96A8B8);
    static inline auto& nPrintFocusHelpTimer = StaticRef<int32>(0x96A8B4);
    static inline auto& f2PlayerStartHeading = StaticRef<float>(0x96A840);
    static inline auto& vec2PlayerStartLocation = StaticRef<CVector>(0x96A9AC);
    static inline auto& bPlayersCanBeInSeparateCars = StaticRef<bool>(0x96A8B3);
    static inline auto& bPlayersCannotTargetEachOther = StaticRef<bool>(0x96A8B2);

    static inline auto& NumAfterDeathStartPoints = StaticRef<int32>(0x96A890);

    static inline auto& SkipToBeFinishedByScript = StaticRef<bool>(0x96A894);
    static inline auto& SkipVehicle = StaticRef<CVehicle*>(0x96A898);
    static inline auto& SkipTimer = StaticRef<uint32>(0x96A89C);
    static inline auto& SkipState = StaticRef<eSkipState>(0x96A8A0);

    static inline auto& bScriptCoopGameGoingOn = StaticRef<bool>(0x96A8A8);
    static inline auto& TimeOfLastEvent = StaticRef<int32>(0x96A8AC);
    static inline auto& GameState = StaticRef<eGameLogicState>(0x96A8B0);
    static inline auto& ActivePlayers = StaticRef<int32>(0x96A8B1);

    static inline auto& bPenaltyForDeathApplies = StaticRef<bool>(0x8A5E48);
    static inline auto& bPenaltyForArrestApplies = StaticRef<bool>(0x8A5E49);
    static inline auto& bLimitPlayerDistance = StaticRef<bool>(0x8A5E4A);
    static inline auto& MaxPlayerDistance = StaticRef<float>(0x8A5E4C);  // default 20.0
    static inline auto& n2PlayerPedInFocus = StaticRef<eFocusedPlayer>(0x8A5E50); // default eFocusedPlayer::NONE

public:
    static void InjectHooks();

    static float CalcDistanceToForbiddenTrainCrossing(CVector vecPoint, CVector vecMoveSpeed, bool ignoreMoveSpeed, CVector& outDistance);
    static void ClearSkip(bool afterMission);
    static void DoWeaponStuffAtStartOf2PlayerGame(bool shareWeapons);
    static void StorePedsWeapons(CPed* ped);
    static eLevelName FindCityClosestToPoint(CVector2D point);
    static void ForceDeathRestart();
    static void InitAtStartOfGame();
    static bool IsCoopGameGoingOn();
    static bool IsPlayerAllowedToGoInThisDirection(CPed* ped, CVector moveDirection, float distanceLimit);
    static bool IsPlayerUse2PlayerControls(CPed* ped);
    static bool IsPointWithinLineArea(const CVector* points, uint32 numPoints, float x, float y);
    static bool IsSkipWaitingForScriptToFadeIn();
    static bool LaRiotsActiveHere();
    static void Save();
    static void Load();
    static void PassTime(uint32 minutes);
    static void Remove2ndPlayerIfPresent();
    static void ResetStuffUponResurrection();
    static void RestorePedsWeapons(CPed* ped);
    static void RestorePlayerStuffDuringResurrection(CPlayerPed* player, CVector posn, float playerStartHeading);
    static void SetPlayerWantedLevelForForbiddenTerritories(bool immediately);
    static void SetUpSkip(CVector coors, float angle, bool afterMission, CEntity* vehicle, bool finishedByScript);
    static bool SkipCanBeActivated();
    static void SortOutStreamingAndMemory(const CVector& translation, float angle);
    static void StopPlayerMovingFromDirection(int32 playerId, CVector direction);
    static void Update();
    static void UpdateSkip();

    // @notsa
    static bool IsAPlayerInFocusOn2PlayerGame();

    // @notsa
    static CPlayerPed* GetFocusedPlayerPed();

    // @notsa
    static bool CanPlayerTripSkip();

    // @notsa
    static void SetMissionFailed();
};
