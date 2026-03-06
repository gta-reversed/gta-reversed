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

    static inline auto& bScriptDontDisplayAreaName = StaticRef<bool>(0xBAA3F8);
    static inline auto& bScriptDontDisplayVehicleName = StaticRef<bool>(0xBAA3F9);
    static inline auto& bScriptForceDisplayWithCounters = StaticRef<bool>(0xBAA3FA);
    static inline auto& bScriptDontDisplayRadar = StaticRef<bool>(0xBAA3FB);

    static inline auto& bDrawClock = StaticRef<bool>(0xBAA400);

    static inline auto& m_pVehicleNameToPrint = StaticRef<const GxtChar*>(0xBAA444);
    static inline auto& m_VehicleState = StaticRef<eNameState>(0xBAA448);
    static inline auto& m_VehicleFadeTimer = StaticRef<int32>(0xBAA44C);
    static inline auto& m_VehicleNameTimer = StaticRef<int32>(0xBAA450);
    static inline auto& m_pLastVehicleName = StaticRef<const GxtChar*>(0xBAA454);
    static inline auto& m_pVehicleName = StaticRef<const GxtChar*>(0xBAA458);

    static inline auto& m_bDraw3dMarkers = StaticRef<bool>(0xBAA45C);
    static inline auto& m_Wants_To_Draw_Hud = StaticRef<bool>(0xBAA45D);

    static inline auto& m_fHelpMessageTime = StaticRef<float>(0xBAA460); // in seconds
    static inline auto& m_fHelpMessageBoxWidth = StaticRef<float>(0x8D0934); // default 200.0
    static inline auto& m_bHelpMessagePermanent = StaticRef<bool>(0xBAA464);
    static inline auto& m_fHelpMessageStatUpdateValue = StaticRef<float>(0xBAA468);
    static inline auto& m_nHelpMessageMaxStatValue = StaticRef<uint16>(0xBAA46C);
    static inline auto& m_nHelpMessageStatId = StaticRef<uint16>(0xBAA470);
    static inline auto& m_bHelpMessageQuick = StaticRef<bool>(0xBAA472);
    static inline auto& m_nHelpMessageState = StaticRef<int32>(0xBAA474);
    static inline auto& m_nHelpMessageFadeTimer = StaticRef<uint32>(0xBAA478);
    static inline auto& m_nHelpMessageTimer = StaticRef<uint32>(0xBAA47C);
    static inline auto (&m_pHelpMessageToPrint)[400] = StaticRef<GxtChar[400]>(0xBAA480);
    static inline auto (&m_pLastHelpMessage)[400] = StaticRef<GxtChar[400]>(0xBAA610);
    static inline auto (&m_pHelpMessage)[400] = StaticRef<GxtChar[400]>(0xBAA7A0);

    static inline auto& m_ZoneState = StaticRef<eNameState>(0xBAA930);
    static inline auto& m_ZoneFadeTimer = StaticRef<int32>(0xBAA934);
    static inline auto& m_ZoneNameTimer = StaticRef<uint32>(0xBAA938);
    static inline auto& m_ZoneToPrint = StaticRef<const GxtChar*>(0xBAB1D0);
    static inline auto& m_pLastZoneName = StaticRef<const GxtChar*>(0xBAB1D4);
    static inline auto& m_pZoneName = StaticRef<const GxtChar*>(0xBAB1D8);

    static inline auto& m_ItemToFlash = StaticRef<eHudItem>(0xBAB1DC);
    static inline auto& bDrawingVitalStats = StaticRef<bool>(0xBAB1DE);

    static inline auto& m_LastBreathTime = StaticRef<int32>(0xBAA3FC);

    static inline auto& m_WeaponState = StaticRef<uint32>(0xBAA404);
    static inline auto& m_WeaponFadeTimer = StaticRef<uint32>(0xBAA408);
    static inline auto& m_WeaponTimer = StaticRef<uint32>(0xBAA40C);
    static inline auto& m_LastWeapon = StaticRef<uint32>(0xBAA410);

    static inline auto& m_WantedState = StaticRef<uint32>(0xBAA414);
    static inline auto& m_WantedFadeTimer = StaticRef<uint32>(0xBAA418);
    static inline auto& m_WantedTimer = StaticRef<uint32>(0xBAA41C);
    static inline auto& m_LastWanted = StaticRef<uint32>(0xBAA420);

    static inline auto& m_DisplayScoreState = StaticRef<uint32>(0xBAA424);
    static inline auto& m_DisplayScoreFadeTimer = StaticRef<uint32>(0xBAA428);
    static inline auto& m_DisplayScoreTimer = StaticRef<uint32>(0xBAA42C);
    static inline auto& m_LastDisplayScore = StaticRef<uint32>(0xBAA430);

    static inline auto& m_EnergyLostState = StaticRef<uint32>(0xBAA434);
    static inline auto& m_EnergyLostFadeTimer = StaticRef<uint32>(0xBAA438);
    static inline auto& m_EnergyLostTimer = StaticRef<uint32>(0xBAA43C);
    static inline auto& m_LastTimeEnergyLost = StaticRef<uint32>(0xBAA440);

    static inline auto (&m_Message)[400] = StaticRef<GxtChar[400]>(0xBAB040);
    static inline auto (&m_BigMessage)[NUM_MESSAGE_STYLES][BIG_MESSAGE_SIZE] = StaticRef<GxtChar[NUM_MESSAGE_STYLES][BIG_MESSAGE_SIZE]>(0xBAACC0);
    static inline auto (&LastBigMessage)[NUM_MESSAGE_STYLES][BIG_MESSAGE_SIZE] = StaticRef<GxtChar[NUM_MESSAGE_STYLES][BIG_MESSAGE_SIZE]>(0xBAA940);
    static inline auto (&BigMessageAlpha)[NUM_MESSAGE_STYLES] = StaticRef<float[NUM_MESSAGE_STYLES]>(0xBAA3A4);
    static inline auto (&BigMessageInUse)[NUM_MESSAGE_STYLES] = StaticRef<float[NUM_MESSAGE_STYLES]>(0xBAA3C0);
    static inline auto (&BigMessageX)[NUM_MESSAGE_STYLES] = StaticRef<float[NUM_MESSAGE_STYLES]>(0xBAA3DC);

    static inline auto (&Sprites)[6] = StaticRef<CSprite2d[6]>(0xBAB1FC);

    static inline auto& TimerMainCounterHideState = StaticRef<int16>(0xBAA388);
    static inline auto& TimerMainCounterWasDisplayed = StaticRef<bool>(0xBAA38A);
    static inline auto (&TimerCounterHideState)[4] = StaticRef<int16[4]>(0xBAA38C);
    static inline auto (&TimerCounterWasDisplayed)[4] = StaticRef<int16[4]>(0xBAA394);

    static inline auto& OddJob2OffTimer = StaticRef<float>(0xBAA398);
    static inline auto& OddJob2XOffset = StaticRef<float>(0xBAA39C);
    static inline auto& OddJob2Timer = StaticRef<uint16>(0xBAA3A0);
    static inline auto& OddJob2On = StaticRef<uint16>(0xBAB1E0);

    static inline auto& PagerXOffset = StaticRef<float>(0x8D0938); // 150.0f
    static inline auto& HelpTripSkipShown = StaticRef<bool>(0xBAB229);

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
