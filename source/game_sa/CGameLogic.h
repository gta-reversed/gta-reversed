#pragma once

#include "CVector.h"

enum eGameState {
    GAME_STATE_INITIAL = 0,
    GAME_STATE_LOGO = 1,
    GAME_STATE_PLAYING_LOGO = 2,
    GAME_STATE_TITLE = 3,
    GAME_STATE_PLAYING_INTRO = 4,
    GAME_STATE_FRONTEND_LOADING = 5,
    GAME_STATE_FRONTEND_LOADED = 6,
    GAME_STATE_FRONTEND_IDLE = 7,
    GAME_STATE_LOADING_STARTED = 8,
    GAME_STATE_LOADING_FINISHED = 9,
};

enum eSkipState {
    SKIP_STATE_0,
    SKIP_STATE_1,
    SKIP_STATE_2,
    SKIP_STATE_3,
    SKIP_STATE_4,
    SKIP_STATE_5,
};

class CPed;

class CGameLogic {
public:
    static float& f2PlayerStartHeading;
    static CVector& vec2PlayerStartLocation;
    static bool& bPlayersCanBeInSeparateCars;

    static int32& TimeOfLastEvent;
    static eGameState& GameState;
    static bool& bPlayersCannotTargetEachOther;
    static int32& ActivePlayers; // unused
    static float& m_96A890;
    static int32& nPrintFocusHelpTimer;
    static int32& nPrintFocusHelpCounter;
    static bool& bScriptCoopGameGoingOn;

    static bool& SkipToBeFinishedByScript;
    static CVehicle*& SkipVehicle;
    static uint32& SkipTimer;
    static eSkipState& SkipState;
    static int32& SkipDestinationOrientation;
    static int32& SkipDestination;

    static int32& NumAfterDeathStartPoints;

    static bool& bPenaltyForDeathApplies;
    static bool& bPenaltyForArrestApplies;
    static bool& bLimitPlayerDistance;
    static float& MaxPlayerDistance;
    static int32& n2PlayerPedInFocus;

public:
    static void InjectHooks();

    static float CalcDistanceToForbiddenTrainCrossing(CVector vecPoint, CVector vecMoveSpeed, bool someBool, CVector* pOutDistance);
    static void ClearSkip(bool a1);
    static void DoWeaponStuffAtStartOf2PlayerGame(bool a1);
    static void StorePedsWeapons(CPed* ped);
    static void FindCityClosestToPoint(float x, float y);
    static void ForceDeathRestart();
    static void InitAtStartOfGame();
    static bool IsCoopGameGoingOn();
    static bool IsPlayerAllowedToGoInThisDirection(CPed* pPed, float moveDirectionX, float moveDirectionY, float moveDirectionZ, float distanceLimit);
    static bool IsPlayerUse2PlayerControls(CPed* ped);
    static bool IsPointWithinLineArea(signed int a1, float x, float y);
    static bool IsSkipWaitingForScriptToFadeIn();
    static bool LaRiotsActiveHere();
    static void Save();
    static void Load();
    static void PassTime(uint32 time);
    static void Remove2ndPlayerIfPresent();
    static void ResetStuffUponResurrection();
    static void RestorePedsWeapons(CPed* ped);
    static void RestorePlayerStuffDuringResurrection(CPlayerPed* player, float x, float y, float z, float playerStartHeading);
    static void SetPlayerWantedLevelForForbiddenTerritories(uint16 townNumber);
    static void SetUpSkip(int32 fX, int32 fY, int32 fZ, float fAngle, bool bAfterMission, CEntity* vehicle, bool bFinishedByScript);
    static void SkipCanBeActivated();
    static void SortOutStreamingAndMemory(CVector const& translation, float angle);
    static void StopPlayerMovingFromDirection(int32 playerId, CVector direction);
    static void Update();
    static void UpdateSkip();
};
