/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Stats.h"
#include "eHud.h"
#include <GxtChar.h>

class CSprite2d;
class CPed;
class CPlayerInfo;
class CPlayerPed;

class CHud {
public:
    static constexpr auto BIG_MESSAGE_SIZE = 128;

    static inline bool& bScriptDontDisplayAreaName = *(bool*)0xBAA3F8;
    static inline bool& bScriptDontDisplayVehicleName = *(bool*)0xBAA3F9;
    static inline bool& bScriptForceDisplayWithCounters = *(bool*)0xBAA3FA;
    static inline bool& bScriptDontDisplayRadar = *(bool*)0xBAA3FB;

    static inline bool& bDrawClock = *(bool*)0xBAA400;

    static inline const GxtChar*& m_pVehicleNameToPrint = *(const GxtChar**)0xBAA444;
    static inline eNameState& m_VehicleState = *(eNameState*)0xBAA448;
    static inline int32& m_VehicleFadeTimer = *(int32*)0xBAA44C;
    static inline int32& m_VehicleNameTimer = *(int32*)0xBAA450;
    static inline const GxtChar*& m_pLastVehicleName = *(const GxtChar**)0xBAA454;
    static inline const GxtChar*& m_pVehicleName = *(const GxtChar**)0xBAA458;

    static inline bool& m_bDraw3dMarkers = *(bool*)0xBAA45C;
    static inline bool& m_Wants_To_Draw_Hud = *(bool*)0xBAA45D;

    static inline float& m_fHelpMessageTime = *(float*)0xBAA460; // in seconds
    static inline float& m_fHelpMessageBoxWidth = *(float*)0x8D0934; // default 200.0
    static inline bool& m_bHelpMessagePermanent = *(bool*)0xBAA464;
    static inline float& m_fHelpMessageStatUpdateValue = *(float*)0xBAA468;
    static inline uint16& m_nHelpMessageMaxStatValue = *(uint16*)0xBAA46C;
    static inline uint16& m_nHelpMessageStatId = *(uint16*)0xBAA470;
    static inline bool& m_bHelpMessageQuick = *(bool*)0xBAA472;
    static inline int32& m_nHelpMessageState = *(int32*)0xBAA474;
    static inline uint32& m_nHelpMessageFadeTimer = *(uint32*)0xBAA478;
    static inline uint32& m_nHelpMessageTimer = *(uint32*)0xBAA47C;
    static inline GxtChar (&m_pHelpMessageToPrint)[400] = *(GxtChar(*)[400])0xBAA480;
    static inline GxtChar (&m_pLastHelpMessage)[400] = *(GxtChar(*)[400])0xBAA610;
    static inline GxtChar (&m_pHelpMessage)[400] = *(GxtChar(*)[400])0xBAA7A0;

    static inline eNameState& m_ZoneState = *(eNameState*)0xBAA930;
    static inline int32& m_ZoneFadeTimer = *(int32*)0xBAA934;
    static inline uint32& m_ZoneNameTimer = *(uint32*)0xBAA938;
    static inline const GxtChar*& m_ZoneToPrint = *(const GxtChar**)0xBAB1D0;
    static inline const GxtChar*& m_pLastZoneName = *(const GxtChar**)0xBAB1D4;
    static inline const GxtChar*& m_pZoneName = *(const GxtChar**)0xBAB1D8;

    static inline eHudItem& m_ItemToFlash = *(eHudItem*)0xBAB1DC;
    static inline bool& bDrawingVitalStats = *(bool*)0xBAB1DE;

    static inline int32& m_LastBreathTime = *(int32*)0xBAA3FC;

    static inline uint32& m_WeaponState = *(uint32*)0xBAA404;
    static inline uint32& m_WeaponFadeTimer = *(uint32*)0xBAA408;
    static inline uint32& m_WeaponTimer = *(uint32*)0xBAA40C;
    static inline uint32& m_LastWeapon = *(uint32*)0xBAA410;

    static inline uint32& m_WantedState = *(uint32*)0xBAA414;
    static inline uint32& m_WantedFadeTimer = *(uint32*)0xBAA418;
    static inline uint32& m_WantedTimer = *(uint32*)0xBAA41C;
    static inline uint32& m_LastWanted = *(uint32*)0xBAA420;

    static inline uint32& m_DisplayScoreState = *(uint32*)0xBAA424;
    static inline uint32& m_DisplayScoreFadeTimer = *(uint32*)0xBAA428;
    static inline uint32& m_DisplayScoreTimer = *(uint32*)0xBAA42C;
    static inline uint32& m_LastDisplayScore = *(uint32*)0xBAA430;

    static inline uint32& m_EnergyLostState = *(uint32*)0xBAA434;
    static inline uint32& m_EnergyLostFadeTimer = *(uint32*)0xBAA438;
    static inline uint32& m_EnergyLostTimer = *(uint32*)0xBAA43C;
    static inline uint32& m_LastTimeEnergyLost = *(uint32*)0xBAA440;

    static inline GxtChar  (&m_Message)[400] = *(GxtChar (*)[400])0xBAB040;
    static inline GxtChar  (&m_BigMessage)[NUM_MESSAGE_STYLES][BIG_MESSAGE_SIZE] = *(GxtChar (*)[NUM_MESSAGE_STYLES][128])0xBAACC0;
    static inline GxtChar  (&LastBigMessage)[NUM_MESSAGE_STYLES][BIG_MESSAGE_SIZE] = *(GxtChar(*)[NUM_MESSAGE_STYLES][128])0xBAA940;
    static inline float (&BigMessageAlpha)[NUM_MESSAGE_STYLES] = *(float(*)[NUM_MESSAGE_STYLES])0xBAA3A4;
    static inline float (&BigMessageInUse)[NUM_MESSAGE_STYLES] = *(float(*)[NUM_MESSAGE_STYLES])0xBAA3C0;
    static inline float (&BigMessageX)[NUM_MESSAGE_STYLES] = *(float(*)[NUM_MESSAGE_STYLES])0xBAA3DC;

    static inline CSprite2d (&Sprites)[6] = *(CSprite2d(*)[6])0xBAB1FC;

    static inline int16& TimerMainCounterHideState = *(int16*)0xBAA388;
    static inline bool& TimerMainCounterWasDisplayed = *(bool*)0xBAA38A;
    static inline int16 (&TimerCounterHideState)[4] = *(int16(*)[4])0xBAA38C;
    static inline int16 (&TimerCounterWasDisplayed)[4] = *(int16(*)[4])0xBAA394;

    static inline float& OddJob2OffTimer = *(float*)0xBAA398;
    static inline float& OddJob2XOffset = *(float*)0xBAA39C;
    static inline uint16& OddJob2Timer = *(uint16*)0xBAA3A0;
    static inline uint16& OddJob2On = *(uint16*)0xBAB1E0;

    static inline float& PagerXOffset = *(float*)0x8D0938; // 150.0f
    static inline bool& HelpTripSkipShown = *(bool*)0xBAB229;

public:
    static void InjectHooks();

    static void Initialise();
    static void ReInitialise();
    static void Shutdown();

    static void GetRidOfAllHudMessages(bool arg0);
    static float GetYPosBasedOnHealth(uint8 playerId, float pos, int8 offset);
    static bool HelpMessageDisplayed();

    static void SetMessage(const GxtChar* message);
    static void SetBigMessage(GxtChar* message, eMessageStyle style);
    static void SetHelpMessage(const GxtChar* text, bool quickMessage = false, bool permanent = false, bool addToBrief = false);
    static void SetHelpMessageStatUpdate(eStatUpdateState state, uint16 statId, float diff, float max);
    static void SetHelpMessageWithNumber(const GxtChar* text, int32 number, bool quickMessage, bool permanent);
    static void SetVehicleName(const GxtChar* name);
    static void SetZoneName(const GxtChar* name, bool displayImmediately);

    static void Draw();
    static void DrawAfterFade();
    static void DrawAreaName();
    static void DrawBustedWastedMessage();
    static void ResetWastedText();
    static void DrawCrossHairs();
    static float DrawFadeState(DRAW_FADE_STATE fadeState, int32 arg1);
    static void DrawHelpText();
    static void DrawMissionTimers();
    static void DrawMissionTitle();
    static void DrawOddJobMessage(bool displayImmediately);
    static void DrawRadar();
    static void DrawScriptText(bool displayImmediately);
    static void DrawSubtitles();
    static void DrawSuccessFailedMessage();
    static void DrawVehicleName();
    static void DrawVitalStats();
    static void DrawAmmo(CPed* ped, int32 x, int32 y, float alpha);
    static void DrawPlayerInfo();
    static inline void DrawClock();
    static inline void DrawMoney(const CPlayerInfo& playerInfo, uint8 alpha);
    static inline void DrawWeapon(CPlayerPed* ped0, CPlayerPed* ped1);

    static void DrawTripSkip();
    static void DrawWanted();
    static void DrawWeaponIcon(CPed* ped, int32 x, int32 y, float alpha);
    static void RenderArmorBar(int32 playerId, int32 x, int32 y);
    static void RenderBreathBar(int32 playerId, int32 x, int32 y);
    static void RenderHealthBar(int32 playerId, int32 x, int32 y);
};
