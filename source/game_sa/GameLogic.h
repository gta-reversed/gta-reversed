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
    inline static std::array<CWeapon, NUM_WEAPON_SLOTS>& SavedWeapons = *reinterpret_cast<std::array<CWeapon, NUM_WEAPON_SLOTS>*>(0x96A9B8);

    inline static std::array<float, 16>& AfterDeathStartPointOrientations = *reinterpret_cast<std::array<float, 16>*>(0x96A850);
    inline static std::array<CVector, 16>& AfterDeathStartPoints = *reinterpret_cast<std::array<CVector, 16>*>(0x96A8E0);

    inline static CVector& SkipPosition = *reinterpret_cast<CVector*>(0x96A8D4);
    inline static float& SkipHeading = *reinterpret_cast<float*>(0x96A8A4);

    static inline int32& nPrintFocusHelpCounter = *reinterpret_cast<int32*>(0x96A8B8);
    static inline int32& nPrintFocusHelpTimer = *reinterpret_cast<int32*>(0x96A8B4);
    static inline float& f2PlayerStartHeading = *reinterpret_cast<float*>(0x96A840);
    static inline CVector& vec2PlayerStartLocation = *reinterpret_cast<CVector*>(0x96A9AC);
    static inline bool& bPlayersCanBeInSeparateCars = *reinterpret_cast<bool*>(0x96A8B3);
    static inline bool& bPlayersCannotTargetEachOther = *reinterpret_cast<bool*>(0x96A8B2);

    static inline int32& NumAfterDeathStartPoints = *reinterpret_cast<int32*>(0x96A890);

    static inline bool& SkipToBeFinishedByScript = *reinterpret_cast<bool*>(0x96A894);
    static inline CVehicle*& SkipVehicle = *reinterpret_cast<CVehicle**>(0x96A898);
    static inline uint32& SkipTimer = *reinterpret_cast<uint32*>(0x96A89C);
    static inline eSkipState& SkipState = *reinterpret_cast<eSkipState*>(0x96A8A0);

    static inline bool& bScriptCoopGameGoingOn = *reinterpret_cast<bool*>(0x96A8A8);
    static inline int32& TimeOfLastEvent = *reinterpret_cast<int32*>(0x96A8AC);
    static inline eGameLogicState& GameState = *reinterpret_cast<eGameLogicState*>(0x96A8B0);
    static inline int32& ActivePlayers = *reinterpret_cast<int32*>(0x96A8B1);

    static inline bool& bPenaltyForDeathApplies = *reinterpret_cast<bool*>(0x8A5E48);
    static inline bool& bPenaltyForArrestApplies = *reinterpret_cast<bool*>(0x8A5E49);
    static inline bool& bLimitPlayerDistance = *reinterpret_cast<bool*>(0x8A5E4A);
    static inline float& MaxPlayerDistance = *reinterpret_cast<float*>(0x8A5E4C);  // default 20.0
    static inline eFocusedPlayer& n2PlayerPedInFocus = *reinterpret_cast<eFocusedPlayer*>(0x8A5E50); // default eFocusedPlayer::NONE

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
