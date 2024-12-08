/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#include "StdInc.h"

#include "Hud.h"
#include "Garages.h"
#include "IdleCam.h"
#include "MenuSystem.h"
#include "Radar.h"
#include "Vehicle.h"
#include "EntryExitManager.h"
#include "TaskSimpleUseGun.h"
#include "eHud.h"

void CHud::InjectHooks() {
    RH_ScopedClass(CHud);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Initialise, 0x5BA850);
    RH_ScopedInstall(ReInitialise, 0x588880);
    RH_ScopedInstall(Shutdown, 0x588850);
    RH_ScopedInstall(Draw, 0x58FAE0);
    RH_ScopedInstall(GetRidOfAllHudMessages, 0x588A50);
    RH_ScopedInstall(GetYPosBasedOnHealth, 0x588B60);
    RH_ScopedInstall(HelpMessageDisplayed, 0x588B50);
    RH_ScopedInstall(ResetWastedText, 0x589070);
    RH_ScopedInstall(SetMessage, 0x588F60);
    RH_ScopedInstall(SetBigMessage, 0x588FC0);
    RH_ScopedInstall(SetHelpMessage, 0x588BE0);
    RH_ScopedInstall(SetHelpMessageStatUpdate, 0x588D40);
    RH_ScopedInstall(SetHelpMessageWithNumber, 0x588E30);
    RH_ScopedInstall(SetVehicleName, 0x588F50);
    RH_ScopedInstall(SetZoneName, 0x588BB0);
    RH_ScopedInstall(DrawAfterFade, 0x58D490);
    RH_ScopedInstall(DrawAreaName, 0x58AA50);
    RH_ScopedInstall(DrawBustedWastedMessage, 0x58CA50);
    RH_ScopedInstall(DrawCrossHairs, 0x58E020, { .reversed = false }); // -
    RH_ScopedInstall(DrawFadeState, 0x58D580, { .reversed = false });  // untested
    RH_ScopedInstall(DrawHelpText, 0x58B6E0, { .reversed = false });
    RH_ScopedInstall(DrawMissionTimers, 0x58B180, { .reversed = false });
    RH_ScopedInstall(DrawMissionTitle, 0x58D240);
    RH_ScopedInstall(DrawOddJobMessage, 0x58CC80);
    RH_ScopedInstall(DrawRadar, 0x58A330);
    RH_ScopedInstall(DrawScriptText, 0x58C080);
    RH_ScopedInstall(DrawSubtitles, 0x58C250, { .reversed = false });
    RH_ScopedInstall(DrawSuccessFailedMessage, 0x58C6A0, { .reversed = false });
    RH_ScopedInstall(DrawVehicleName, 0x58AEA0);
    RH_ScopedInstall(DrawVitalStats, 0x589650, { .reversed = false });
    RH_ScopedInstall(DrawAmmo, 0x5893B0);
    RH_ScopedInstall(DrawPlayerInfo, 0x58EAF0, { .reversed = false });
    RH_ScopedInstall(DrawTripSkip, 0x58A160);
    RH_ScopedInstall(DrawWanted, 0x58D9A0, { .reversed = false });
    RH_ScopedInstall(DrawWeaponIcon, 0x58D7D0);
    RH_ScopedInstall(RenderArmorBar, 0x5890A0);
    RH_ScopedInstall(RenderBreathBar, 0x589190);
    RH_ScopedInstall(RenderHealthBar, 0x589270);
}

// 0x5BA850
void CHud::Initialise() {
    static constexpr SpriteFileName textures[]= { // 0x8D128C
        { "fist",           "fistm"           },
        { "siteM16",        "siteM16m"        },
        { "siterocket",     "siterocketm"     },
        { "radardisc",      "radardiscA"      },
        { "radarRingPlane", "radarRingPlaneA" },
        { "SkipIcon",       "SkipIconA"       },
    };

    auto txd = CTxdStore::AddTxdSlot("hud");
    CTxdStore::LoadTxd(txd, "MODELS\\HUD.TXD");
    CTxdStore::AddRef(txd);
    CTxdStore::PushCurrentTxd();
    CTxdStore::SetCurrentTxd(txd);

    for (auto i = 0u; i < std::size(Sprites); i++) {
        const auto& [texture, mask] = textures[i];
        Sprites[i].SetTexture(texture, mask);
    }
    CTxdStore::PopCurrentTxd();
    ReInitialise();
}

// 0x588880
void CHud::ReInitialise() {
    memset(m_pHelpMessageToPrint, 0, sizeof(m_pHelpMessageToPrint));
    memset(m_pLastHelpMessage,    0, sizeof(m_pLastHelpMessage));
    memset(m_pHelpMessage,        0, sizeof(m_pHelpMessage));
    memset(m_Message,             0, sizeof(m_Message));
    memset(m_BigMessage,          0, sizeof(m_BigMessage));
    memset(BigMessageX,           0, sizeof(BigMessageX));

    OddJob2On       = 0;
    OddJob2Timer    = 0;
    OddJob2XOffset  = 0.0f;
    OddJob2OffTimer = 0.0f;
    PagerXOffset    = 150.0f;

    std::ranges::fill(TimerCounterHideState, 0);
    std::ranges::fill(TimerCounterWasDisplayed, 0);
    TimerMainCounterWasDisplayed = false;
    TimerMainCounterHideState = 0;

    const CPlayerInfo& playerInfo   = FindPlayerInfo();
    m_LastTimeEnergyLost            = playerInfo.m_nLastTimeEnergyLost;
    m_LastDisplayScore              = playerInfo.m_nDisplayMoney;
    m_fHelpMessageStatUpdateValue   = 0.0f;
    m_Wants_To_Draw_Hud             = true;
    m_bDraw3dMarkers                = true;
    m_ZoneNameTimer                 = 0;
    m_pZoneName                     = nullptr;
    m_pLastZoneName                 = nullptr;
    m_ZoneState                     = NAME_DONT_SHOW;
    m_nHelpMessageTimer             = 0;
    m_nHelpMessageFadeTimer         = 0;
    m_nHelpMessageState             = 0;
    m_bHelpMessageQuick             = false;
    m_nHelpMessageStatId            = 0;
    m_nHelpMessageMaxStatValue      = 1000;
    m_bHelpMessagePermanent         = false;
    m_fHelpMessageTime              = 1.0f;
    m_fHelpMessageBoxWidth          = 200.0f;
    m_pVehicleName                  = nullptr;
    m_pLastVehicleName              = nullptr;
    m_pVehicleNameToPrint           = nullptr;
    m_VehicleNameTimer              = 0;
    m_VehicleFadeTimer              = 0;
    m_VehicleState                  = NAME_DONT_SHOW;
    bScriptDontDisplayRadar         = false;
    bScriptForceDisplayWithCounters = false;
    bScriptDontDisplayVehicleName   = false;
    bScriptDontDisplayAreaName      = false;
    m_ItemToFlash                   = ITEM_NONE;
    m_EnergyLostTimer               = 0;
    m_EnergyLostFadeTimer           = 0;
    m_EnergyLostState               = 5;
    m_DisplayScoreTimer             = 0;
    m_DisplayScoreFadeTimer         = 0;
    m_DisplayScoreState             = 5;
    m_LastWanted                    = 0;
    m_WantedTimer                   = 0;
    m_WantedFadeTimer               = 0;
    m_WantedState                   = 5;
    m_LastWeapon                    = 0;
    m_WeaponTimer                   = 0;
    m_WeaponFadeTimer               = 0;
    m_WeaponState                   = 5;
    bDrawClock                      = true;
    m_LastBreathTime                = 0;
}

// 0x588850
void CHud::Shutdown() {
    std::ranges::for_each(Sprites, [](auto& sprite) { sprite.Delete(); });
    CTxdStore::RemoveTxdSlot(CTxdStore::FindTxdSlot("hud"));
}

// 0x588B60
float CHud::GetYPosBasedOnHealth(uint8 playerId, float pos, int8 offset) {
    return (float)FindPlayerInfo(playerId).m_nMaxHealth < 101.0f
               ? pos - SCREEN_SCALE_Y((float)offset)
               : pos;
}

// 0x588B50
bool CHud::HelpMessageDisplayed() {
    return m_nHelpMessageState != 0;
}

// 0x588F60
void CHud::SetMessage(const GxtChar* message) {
    if (message) {
        strncpy_s((char*)m_Message, sizeof(m_Message), AsciiFromGxtChar(message), sizeof(m_Message));
    } else {
        m_Message[0] = '\0';
    }
}

// little bit different from OG
// 0x588FC0
void CHud::SetBigMessage(GxtChar* message, eMessageStyle style) {
    if (BigMessageX[style] != 0.0f) {
        return;
    }

    strncpy_s((char*)m_BigMessage[style], sizeof(m_BigMessage[style]), AsciiFromGxtChar(message), sizeof(m_BigMessage[style]));

    switch (style) {
    case STYLE_WHITE_MIDDLE_SMALLER: {
        if (strcmp(AsciiFromGxtChar(message), AsciiFromGxtChar(LastBigMessage[STYLE_WHITE_MIDDLE_SMALLER])) != 0) {
            OddJob2OffTimer = 0.0f;
            OddJob2On = 0;
        }
        strncpy_s((char*)LastBigMessage[style], sizeof(LastBigMessage[style]), AsciiFromGxtChar(message), sizeof(LastBigMessage[style]));
        break;
    }
    default: {
        message[0] = '\0';
    }
    }
}

// 0x588BE0
void CHud::SetHelpMessage(const GxtChar* text, bool quickMessage, bool permanent, bool addToBrief) {
    if (m_BigMessage[STYLE_MIDDLE_SMALLER_HIGHER][0] || CGarages::MessageIDString[0] || CReplay::Mode == MODE_PLAYBACK || CCutsceneMgr::IsRunning()) {
        return;
    }

    std::ranges::fill(m_pHelpMessageToPrint, '\0');
    std::ranges::fill(m_pLastHelpMessage, '\0');
    std::ranges::fill(m_pHelpMessage, '\0');

    CMessages::StringCopy(m_pHelpMessage, text, sizeof(m_pHelpMessage));
    CMessages::InsertPlayerControlKeysInString(m_pHelpMessage);
    if (m_nHelpMessageState && CMessages::StringCompare(m_pHelpMessage, m_pHelpMessageToPrint, sizeof(m_pHelpMessage)))
        return;

    std::ranges::fill(m_pLastHelpMessage, '\0');
    if (!text) {
        m_pHelpMessage[0] = '\0';
        m_pHelpMessageToPrint[0] = '\0';
    }

    if (permanent) {
        m_nHelpMessageState = 1;
        CMessages::StringCopy(m_pHelpMessageToPrint, m_pHelpMessage, sizeof(m_pHelpMessage));
        CMessages::StringCopy(m_pLastHelpMessage, m_pHelpMessage, sizeof(m_pHelpMessage));
    } else {
        m_nHelpMessageState = 0;
    }

    if (addToBrief)
        CMessages::AddToPreviousBriefArray(text);

    m_bHelpMessagePermanent = permanent;
    m_bHelpMessageQuick = quickMessage;
    m_nHelpMessageStatId = 0;
    m_nHelpMessageMaxStatValue = 1000;
    m_fHelpMessageStatUpdateValue = 0.0f;
}

// 0x588D40
void CHud::SetHelpMessageStatUpdate(eStatUpdateState state, uint16 statId, float diff, float max) {
    if (m_BigMessage[STYLE_MIDDLE_SMALLER_HIGHER][0] || CGarages::MessageIDString[0] || CReplay::Mode == MODE_PLAYBACK || CCutsceneMgr::IsCutsceneProcessing()) {
        return;
    }

    std::ranges::fill(m_pHelpMessageToPrint, '\0');
    std::ranges::fill(m_pLastHelpMessage, '\0');
    std::ranges::fill(m_pHelpMessage, '\0');

    if (m_nHelpMessageState && CMessages::StringCompare(m_pHelpMessage, m_pHelpMessageToPrint, sizeof(m_pHelpMessage)))
        return;

    std::ranges::fill(m_pLastHelpMessage, '\0');
    m_nHelpMessageState = 0;
    m_bHelpMessageQuick = false;
    m_bHelpMessagePermanent = false;
    m_nHelpMessageStatId = statId;
    m_fHelpMessageStatUpdateValue = diff;
    m_nHelpMessageMaxStatValue = (uint32)max;
    sprintf_s(gString, state == STAT_UPDATE_INCREASE ? "+" : "-");
    AsciiToGxtChar(gString, m_pHelpMessage);
}

// 0x588E30
void CHud::SetHelpMessageWithNumber(const GxtChar* text, int32 number, bool quickMessage, bool permanent) {
    if (m_BigMessage[STYLE_MIDDLE_SMALLER_HIGHER][0] || CGarages::MessageIDString[0] || CReplay::Mode == MODE_PLAYBACK || CCutsceneMgr::IsCutsceneProcessing()) {
        return;
    }

    GxtChar str[400];
    CMessages::InsertNumberInString(text, number, -1, -1, -1, -1, -1, str);
    CMessages::GetStringLength(str);
    CMessages::StringCopy(m_pHelpMessage, str, sizeof(m_pHelpMessage));
    CMessages::InsertPlayerControlKeysInString(m_pHelpMessage);

    std::ranges::fill(m_pLastHelpMessage, '\0');
    if (permanent) {
        m_nHelpMessageState = 1;
        CMessages::StringCopy(m_pHelpMessageToPrint, m_pHelpMessage, sizeof(m_pHelpMessage));
        CMessages::StringCopy(m_pLastHelpMessage, m_pHelpMessage, sizeof(m_pHelpMessage));
    } else {
        m_nHelpMessageState = 0;
    }

    m_bHelpMessagePermanent = permanent;
    m_bHelpMessageQuick = quickMessage;
    m_nHelpMessageStatId = 0;
    m_nHelpMessageMaxStatValue = 1000;
    m_fHelpMessageStatUpdateValue = 0.0f;
}

// 0x588F50
void CHud::SetVehicleName(const GxtChar* name) {
    m_pVehicleName = name;
}

// 0x588BB0
void CHud::SetZoneName(const GxtChar* name, bool displayImmediately) {
    if (displayImmediately) {
        m_pZoneName = name;
        return;
    }
    if (CGame::currArea || m_ZoneState != NAME_DONT_SHOW) {
        return;
    }
    m_pZoneName = name;
}

// called each frame from Render2dStuff()
// 0x58FAE0
void CHud::Draw() {
    if (CReplay::Mode == MODE_PLAYBACK || CWeapon::ms_bTakePhoto || FrontEndMenuManager.m_bActivateMenuNextFrame || gbCineyCamProcessedOnFrame == CTimer::GetFrameCounter())
        return;

    RwRenderStateSet(rwRENDERSTATETEXTUREFILTER,        RWRSTATE(rwFILTERNEAREST));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE,    RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEFOGENABLE,            RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATESRCBLEND,             RWRSTATE(rwBLENDSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,            RWRSTATE(rwBLENDINVSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE,    RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATETEXTUREFILTER,        RWRSTATE(rwFILTERLINEAR));
    RwRenderStateSet(rwRENDERSTATETEXTUREADDRESS,       RWRSTATE(rwTEXTUREADDRESSCLAMP));
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER,        RWRSTATE(NULL));
    RwRenderStateSet(rwRENDERSTATESHADEMODE,            RWRSTATE(rwSHADEMODEFLAT));
    RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTION,    RWRSTATE(rwALPHATESTFUNCTIONGREATER));
    RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, RWRSTATE(NULL));

    if (!TheCamera.m_bWideScreenOn) {
        DrawCrossHairs();
        if (FrontEndMenuManager.m_bHudOn && CTheScripts::bDisplayHud) {
            DrawPlayerInfo();
            DrawWanted();
        }
        if (!bScriptDontDisplayVehicleName) {
            DrawVehicleName();
        }
        DrawMissionTimers();
    }

    if (!bScriptDontDisplayRadar && !TheCamera.m_bWideScreenOn) {
        CPed* player = FindPlayerPed();
        CPad* pad = CPad::GetPad();
        if (!pad->GetDisplayVitalStats(player) || FindPlayerVehicle()) {
            bDrawingVitalStats = false;
            DrawRadar();
        } else {
            bDrawingVitalStats = true;
            DrawVitalStats();
        }
        if (!CGameLogic::SkipCanBeActivated() || bDrawingVitalStats) {
            HelpTripSkipShown = false;
        } else {
            DrawTripSkip();
            if (!HelpTripSkipShown) {
                SetHelpMessage(TheText.Get("SKIP_1"), true, false, false);
                HelpTripSkipShown = true;
            }
        }
    }

    if (m_bDraw3dMarkers && !TheCamera.m_bWideScreenOn) {
        CRadar::Draw3dMarkers();
    }

    if (!CTimer::GetIsUserPaused()) {
        if (!m_BigMessage[STYLE_MIDDLE][0]) {
            if (CMenuSystem::GetNumMenusInUse()) {
                CMenuSystem::Process(CMenuSystem::MENU_UNDEFINED);
            }
            DrawScriptText(true);
        }
        if (CTheScripts::bDrawSubtitlesBeforeFade) {
            DrawSubtitles();
        }
        DrawHelpText();
        DrawOddJobMessage(true);
        DrawSuccessFailedMessage();
        DrawBustedWastedMessage();
    }
}

// 0x58D490
void CHud::DrawAfterFade() {
    ZoneScoped;

    RwRenderStateSet(rwRENDERSTATETEXTUREFILTER,     RWRSTATE(rwFILTERNEAREST));
    RwRenderStateSet(rwRENDERSTATETEXTUREADDRESS,    RWRSTATE(rwTEXTUREADDRESSCLAMP));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(FALSE));

    if (CTimer::GetIsUserPaused() || CReplay::Mode == MODE_PLAYBACK || CWeapon::ms_bTakePhoto)
        return;

    auto vehicle = FindPlayerVehicle();
    if (!vehicle || (!vehicle->IsSubPlane() && !vehicle->IsSubHeli())) {
        if (!CCutsceneMgr::ms_cutsceneProcessing) {
            if (!FrontEndMenuManager.m_bMenuActive && !TheCamera.m_bWideScreenOn && !bScriptDontDisplayAreaName) {
                DrawAreaName();
            }
        }
    }

    if (!m_BigMessage[STYLE_MIDDLE][0]) {
        DrawScriptText(false);
    }

    if (!CTheScripts::bDrawSubtitlesBeforeFade) {
        DrawSubtitles();
    }

    DrawMissionTitle();
    DrawOddJobMessage(false);
}

// 0x58AA50
void CHud::DrawAreaName() {
    if (!m_pZoneName) {
        return;
    }

    if (m_pZoneName != m_pLastZoneName) {
        switch (m_ZoneState) {
        case NAME_DONT_SHOW:
            if (!CTheScripts::bPlayerIsOffTheMap && CTheScripts::bDisplayHud ||
                CEntryExitManager::ms_exitEnterState == EXIT_ENTER_STATE_1 ||
                CEntryExitManager::ms_exitEnterState == EXIT_ENTER_STATE_2
            ) {
                m_ZoneState = NAME_FADE_IN;
                m_ZoneNameTimer = 0;
                m_ZoneFadeTimer = 0;
                m_ZoneToPrint = m_pZoneName;
                if (m_VehicleState == NAME_SHOW || m_VehicleState == NAME_FADE_IN) {
                    m_VehicleState = NAME_FADE_OUT;
                }
            }
            break;
        case NAME_SHOW:
        case NAME_FADE_IN:
        case NAME_FADE_OUT:
            m_ZoneState = NAME_SWITCH;
            m_ZoneNameTimer = 0;
            break;
        case NAME_SWITCH:
            m_ZoneNameTimer = 0;
            break;
        default:
            break;
        }
        m_pLastZoneName = m_pZoneName;
    }

    if (!m_ZoneState)
        return;

    float alpha = 255.0f;
    switch (m_ZoneState) {
    case NAME_SHOW:
        m_ZoneFadeTimer = 1000;
        if (m_ZoneNameTimer > 3000) {
            m_ZoneState = NAME_FADE_OUT;
            m_ZoneFadeTimer = 1000;
        }
        break;

    case NAME_FADE_IN:
        if (!TheCamera.GetFading() && TheCamera.GetScreenFadeStatus() != NAME_FADE_IN) {
            m_ZoneFadeTimer += (int32)CTimer::GetTimeStepInMS();
        }

        if (m_ZoneFadeTimer > 1000) {
            m_ZoneFadeTimer = 1000;
            m_ZoneState = NAME_SHOW;
        }

        if (TheCamera.GetScreenFadeStatus() != NAME_FADE_IN) {
            alpha = (float)m_ZoneFadeTimer / 1000.0f * 255.0f;
            break;
        }
        m_ZoneState = NAME_FADE_OUT;
        m_ZoneFadeTimer = 1000;
        break;

    case NAME_FADE_OUT:
        if (!TheCamera.GetFading() && TheCamera.GetScreenFadeStatus() != NAME_FADE_IN) {
            m_ZoneFadeTimer -= (int32)CTimer::GetTimeStepInMS();
        }

        if (m_ZoneFadeTimer < 0) {
            m_ZoneFadeTimer = 0;
            m_ZoneState = NAME_DONT_SHOW;
        }

        if (TheCamera.GetScreenFadeStatus() != NAME_FADE_IN) {
            alpha = (float)m_ZoneFadeTimer / 1000.0f * 255.0f;
            break;
        }
        m_ZoneFadeTimer = 1000;
        break;

    case NAME_SWITCH:
        m_ZoneFadeTimer -= (int32)CTimer::GetTimeStepInMS();
        if (m_ZoneFadeTimer < 0) {
            m_ZoneFadeTimer = 0;
            m_ZoneState = NAME_FADE_IN;
            m_ZoneToPrint = m_pLastZoneName;
        }
        alpha = (float)m_ZoneFadeTimer / 1000.0f * 255.0f;
        break;

    default:
        break;
    }

    if (m_Message[0] || BigMessageX[STYLE_BOTTOM_RIGHT] != 0.0f || BigMessageX[STYLE_WHITE_MIDDLE] != 0.0f) {
        m_ZoneState = NAME_FADE_OUT;
        return;
    }

    m_ZoneNameTimer += (uint32)CTimer::GetTimeStepInMS();
    CFont::SetProportional(true);
    CFont::SetBackground(false, false);
    CFont::SetScaleForCurrentLanguage(SCREEN_STRETCH_X(1.2f), SCREEN_SCALE_Y(1.9f));
    CFont::SetEdge(2);
    CFont::SetOrientation(eFontAlignment::ALIGN_RIGHT);
    CFont::SetRightJustifyWrap(SCREEN_STRETCH_X(180.0f));
    CFont::SetDropColor({ 0, 0, 0, (uint8)alpha });
    CFont::SetFontStyle(FONT_GOTHIC);

    const CZoneInfo* info = CPopCycle::m_pCurrZoneInfo;
    const auto& color = info->ZoneColor; // Cppcheck: (warning) nullPointerRedundantCheck: Either the condition 'info' is redundant or there is possible null pointer dereference: info.
    if (CGangWars::bGangWarsActive && info && color.r && color.g && color.b) {
        CFont::SetColor({ color.r, color.g, color.b, (uint8)alpha});
    } else {
        CFont::SetColor(HudColour.GetRGBA(HUD_COLOUR_LIGHT_BLUE, (uint8)alpha));
    }

    CFont::PrintStringFromBottom(SCREEN_STRETCH_FROM_RIGHT(32.0f), SCREEN_SCALE_FROM_BOTTOM(104.0f) + SCREEN_SCALE_Y(76.0f), m_ZoneToPrint);
    CFont::SetSlant(0.0f);
}

// 0x58CA50
void CHud::DrawBustedWastedMessage() {
    auto& message      = m_BigMessage[STYLE_WHITE_MIDDLE];
    auto& messageX     = BigMessageX[STYLE_WHITE_MIDDLE];
    auto& messageAlpha = BigMessageAlpha[STYLE_WHITE_MIDDLE];

    if (!message[0]) {
        messageX = '\0';
        return;
    }

    if (messageX == 0.0f) {
        messageX = 1.0f;
        messageAlpha = 0.0f;

        if (m_VehicleState) {
            m_VehicleState = NAME_DONT_SHOW;
        }
        if (m_ZoneState) {
            m_ZoneState = NAME_DONT_SHOW;
        }
        return;
    }

    messageAlpha += CTimer::GetTimeStepInMS() * 0.4f;
    messageAlpha = std::min(messageAlpha, 255.0f);

    CFont::SetBackground(false, false);
    CFont::SetScale(SCREEN_STRETCH_X(2.1f), SCREEN_SCALE_Y(2.1f));
    CFont::SetProportional(true);
    CFont::SetJustify(false);
    CFont::SetOrientation(eFontAlignment::ALIGN_CENTER);
    CFont::SetFontStyle(FONT_GOTHIC);
    CFont::SetEdge(3);
    CFont::SetDropColor({ 0, 0, 0, (uint8)messageAlpha });
    CFont::SetColor(HudColour.GetRGBA(HUD_COLOUR_LIGHT_GRAY, (uint8)messageAlpha));
    CFont::PrintStringFromBottom(SCREEN_WIDTH / 2.0f, static_cast<float>(RsGlobal.maximumHeight / 2) - SCREEN_SCALE_Y(30.0f), message); // OG: posY static allocated var
}

// 0x589070
void CHud::ResetWastedText() {
    BigMessageX[STYLE_WHITE_MIDDLE] = 0.0f;
    m_BigMessage[STYLE_WHITE_MIDDLE][0] = '\0';

    BigMessageX[STYLE_MIDDLE] = 0.0f;
    m_BigMessage[STYLE_MIDDLE][0] = '\0';
}

// 0x58E020
void CHud::DrawCrossHairs() {
    return plugin::Call<0x58E020>();

    plugin::Call<0x58E020>(); // for test purposes

    struct RestoreRenderState {
        ~RestoreRenderState() {
            RwRenderStateSet(rwRENDERSTATESRCBLEND,     RWRSTATE(rwBLENDSRCALPHA));
            RwRenderStateSet(rwRENDERSTATEDESTBLEND,    RWRSTATE(rwBLENDINVSRCALPHA));
            RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, RWRSTATE(TRUE));
        }
    };
    RestoreRenderState state;

    const CCam& currentCamera = CCamera::GetActiveCamera();
    const auto& camMode = currentCamera.m_nMode;

    bool bDrawCircleCrossHair = false;
    bool bDrawCustomCrossHair = false;
    bool bIgnoreCheckMeleeTypeWeapon = false;

    if (camMode != eCamMode::MODE_SNIPER) {
        if (camMode == eCamMode::MODE_1STPERSON) {
            CVehicle* vehicle = FindPlayerVehicle();
            if (vehicle && (vehicle->m_nModelIndex == eModelID::MODEL_HYDRA || vehicle->m_nModelIndex == eModelID::MODEL_HUNTER)) {
                bDrawCustomCrossHair = true;
            }
        } else if (
            camMode != eCamMode::MODE_ROCKETLAUNCHER && camMode != eCamMode::MODE_ROCKETLAUNCHER_HS &&
            camMode != eCamMode::MODE_M16_1STPERSON && camMode != eCamMode::MODE_HELICANNON_1STPERSON &&
            camMode != eCamMode::MODE_CAMERA
        ) {
            bIgnoreCheckMeleeTypeWeapon = true;
        }
    }

    auto* const player = FindPlayerPed();
    auto& activeWeapon = player->GetActiveWeapon(); // Cppcheck: (warning) nullPointerRedundantCheck: Either the condition 'player' is redundant or there is possible null pointer dereference: player.
    if (camMode != eCamMode::MODE_1STPERSON &&
        player &&
        !activeWeapon.IsTypeMelee() &&
        !bIgnoreCheckMeleeTypeWeapon
    ) {
        bDrawCustomCrossHair = true;
    }

    if (camMode == eCamMode::MODE_M16_1STPERSON_RUNABOUT || camMode == eCamMode::MODE_ROCKETLAUNCHER_RUNABOUT ||
        camMode == eCamMode::MODE_ROCKETLAUNCHER_RUNABOUT_HS || camMode == eCamMode::MODE_SNIPER_RUNABOUT
    ) {
        bDrawCircleCrossHair = true;
    }

    CTaskSimpleUseGun* localTakUseGun = player->GetIntelligence()->GetTaskUseGun();
    if (!player->m_pTargetedObject && !player->bIsRestoringLook && (!localTakUseGun || !localTakUseGun->m_SkipAim)) {
        if (camMode == MODE_AIMWEAPON || camMode == MODE_AIMWEAPON_FROMCAR || camMode == MODE_AIMWEAPON_ATTACHED) {
            if (player->m_nPedState != ePedState::PEDSTATE_ENTER_CAR && player->m_nPedState != ePedState::PEDSTATE_CARJACK) {
                if ((activeWeapon.m_Type >= eWeaponType::WEAPON_PISTOL &&
                     activeWeapon.m_Type <= eWeaponType::WEAPON_COUNTRYRIFLE
                    ) ||
                     activeWeapon.m_Type == eWeaponType::WEAPON_FLAMETHROWER || activeWeapon.m_Type == eWeaponType::WEAPON_MINIGUN
                ) {
                    bDrawCircleCrossHair = camMode == MODE_AIMWEAPON || TheCamera.m_bTransitionState;
                }
            }
        }
    }

    if (!bDrawCircleCrossHair && !bDrawCustomCrossHair && CTheScripts::bDrawCrossHair == eCrossHairType::NONE)
        return;

    CRect rect;
    const CRGBA black = CRGBA(255, 0, 0, 255); // TODO: RED FOR TES PURPOSES. OG : CRGBA(255, 255, 255, 255);
    if (bDrawCircleCrossHair) { // 0x58E1E1
        RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, RWRSTATE(rwFILTERLINEAR));
        RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,  RWRSTATE(FALSE));

        float hairMultXOnScreen = SCREEN_WIDTH * CCamera::m_f3rdPersonCHairMultX;
        float hairMultYOnScreen = SCREEN_HEIGHT * CCamera::m_f3rdPersonCHairMultY;
        float gunRadius = player->GetWeaponRadiusOnScreen();

        if (gunRadius == 0.2f) {
            rect.left   = hairMultXOnScreen - 1.0f;
            rect.bottom    = hairMultYOnScreen - 1.0f;
            rect.right  = hairMultXOnScreen + 1.0f;
            rect.top = hairMultYOnScreen + 1.0f;
            CSprite2d::DrawRect(rect, black);
        }

        rect.left   = hairMultXOnScreen - SCREEN_STRETCH_X(64.0f * gunRadius / 2.0f);
        rect.bottom    = hairMultYOnScreen - SCREEN_STRETCH_Y(64.0f * gunRadius / 2.0f);
        rect.right  = rect.left + SCREEN_STRETCH_X(64.0f * gunRadius / 2.0f);
        rect.top = rect.bottom  + SCREEN_STRETCH_Y(64.0f * gunRadius / 2.0f);
        Sprites[SPRITE_SITE_M16].Draw(rect, black); // left top

        rect.left   = hairMultXOnScreen + SCREEN_STRETCH_X(64.0f * gunRadius / 2.0f);
        Sprites[SPRITE_SITE_M16].Draw(rect, black); // right top

        rect.left   = hairMultXOnScreen - SCREEN_STRETCH_X(64.0f * gunRadius / 2.0f);
        rect.bottom   += SCREEN_STRETCH_Y(64.0f * gunRadius);
        Sprites[SPRITE_SITE_M16].Draw(rect, black); // left bottom

        rect.left   = hairMultXOnScreen + SCREEN_STRETCH_X(64.0f * gunRadius / 2.0f);
        Sprites[SPRITE_SITE_M16].Draw(rect, black); // right bottom
        return;
    }

    if (CTheScripts::bDrawCrossHair != eCrossHairType::FIXED_DRAW_1STPERSON_WEAPON) {
        if (camMode == MODE_M16_1STPERSON ||
            camMode == MODE_M16_1STPERSON_RUNABOUT ||
            camMode == MODE_1STPERSON_RUNABOUT ||
            camMode == MODE_HELICANNON_1STPERSON
        ) {
            RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, RWRSTATE(rwFILTERLINEAR));
            RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, RWRSTATE(FALSE));

            rect.left   = (SCREEN_WIDTH / 2.0f)   - SCREEN_STRETCH_X(64.0f / 2.0f); // top left
            rect.bottom    = (SCREEN_HEIGHT / 2.0f)  - SCREEN_STRETCH_Y(64.0f / 2.0f);
            rect.right  = ((SCREEN_WIDTH / 2.0f)  - SCREEN_STRETCH_X(64.0f / 2.0f)) + SCREEN_STRETCH_X(64.0f / 2.0f);
            rect.top = ((SCREEN_HEIGHT / 2.0f) - SCREEN_STRETCH_Y(64.0f / 2.0f)) + SCREEN_STRETCH_Y(64.0f / 2.0f);
            Sprites[SPRITE_SITE_M16].Draw(rect, black);

            rect.left   = (SCREEN_WIDTH / 2.0f)   + SCREEN_STRETCH_X(64.0f / 2.0f); // top right
            rect.bottom    = (SCREEN_HEIGHT / 2.0f)  - SCREEN_STRETCH_Y(64.0f / 2.0f);
            rect.right  = ((SCREEN_WIDTH / 2.0f)  - SCREEN_STRETCH_X(64.0f / 2.0f)) + SCREEN_STRETCH_X(64.0f / 2.0f);
            rect.top = ((SCREEN_HEIGHT / 2.0f) - SCREEN_STRETCH_Y(64.0f / 2.0f)) + SCREEN_STRETCH_Y(64.0f / 2.0f);
            Sprites[SPRITE_SITE_M16].Draw(rect, black);

            rect.left   = (SCREEN_WIDTH / 2.0f)   - SCREEN_STRETCH_X(64.0f / 2.0f); // bottom left
            rect.bottom    = SCREEN_STRETCH_Y(64.0f) + ((SCREEN_HEIGHT / 2.0f) - SCREEN_STRETCH_Y(64.0f / 2.0f));
            rect.right  = ((SCREEN_WIDTH / 2.0f)  - SCREEN_STRETCH_X(64.0f / 2.0f)) + SCREEN_STRETCH_X(64.0f / 2.0f);
            rect.top = ((SCREEN_HEIGHT / 2.0f) - SCREEN_STRETCH_Y(64.0f / 2.0f)) + SCREEN_STRETCH_Y(64.0f / 2.0f);
            Sprites[SPRITE_SITE_M16].Draw(rect, black);

            rect.left   = (SCREEN_WIDTH / 2.0f)   + SCREEN_STRETCH_X(64.0f / 2.0f); // bottom right
            rect.bottom    = SCREEN_STRETCH_Y(64.0f) + ((SCREEN_HEIGHT / 2.0f) - SCREEN_STRETCH_Y(64.0f / 2.0f));
            rect.right  = ((SCREEN_WIDTH / 2.0f)  - SCREEN_STRETCH_X(64.0f / 2.0f)) + SCREEN_STRETCH_X(64.0f / 2.0f);
            rect.top = ((SCREEN_HEIGHT / 2.0f) - SCREEN_STRETCH_Y(64.0f / 2.0f)) + SCREEN_STRETCH_Y(64.0f / 2.0f);
            Sprites[SPRITE_SITE_M16].Draw(rect, black);
            return;
        }
    }

    RwTexture* drawTexture = nullptr;
    float screenStretchCrossHairX = 0.0f;
    float screenStretchCrossHairY = 0.0f;
    float screenOffsetCenterX = 0.0f;
    float screenOffsetCenterY = 0.0f;

    if (activeWeapon.m_Type == eWeaponType::WEAPON_CAMERA || activeWeapon.m_Type == eWeaponType::WEAPON_SNIPERRIFLE ||
        CTheScripts::bDrawCrossHair == eCrossHairType::FIXED_DRAW_1STPERSON_WEAPON
    ) {
        if (activeWeapon.m_Type == eWeaponType::WEAPON_CAMERA || CTheScripts::bDrawCrossHair == eCrossHairType::FIXED_DRAW_1STPERSON_WEAPON) {
            screenStretchCrossHairX = SCREEN_STRETCH_X(256.0f);
            screenStretchCrossHairY = SCREEN_STRETCH_Y(192.0f);
        } else {
            screenStretchCrossHairX = SCREEN_STRETCH_X(210.0f);
            screenStretchCrossHairY = SCREEN_STRETCH_Y(210.0f);
        }

        screenOffsetCenterX = 0.0f;
        screenOffsetCenterY = 0.0f;

        CWeaponInfo& info = activeWeapon.GetWeaponInfo(eWeaponSkill::STD);
        if (info.m_nModelId1 <= 0) {
            return;
        }

        CBaseModelInfo* mi = CModelInfo::GetModelInfo(info.m_nModelId1);
        TxdDef* txd = CTxdStore::ms_pTxdPool->GetAt(mi->m_nTxdIndex);
        if (!txd->m_pRwDictionary) {
            return;
        }
        drawTexture = RwTexDictionaryFindHashNamedTexture(txd->m_pRwDictionary, CKeyGen::AppendStringToKey(mi->m_nKey, "CROSSHAIR"));
    } else {
        if (camMode != MODE_ROCKETLAUNCHER && camMode != MODE_1STPERSON && camMode != MODE_ROCKETLAUNCHER_RUNABOUT &&
            camMode != MODE_ROCKETLAUNCHER_HS && camMode != MODE_ROCKETLAUNCHER_RUNABOUT_HS
        ) {
            return;
        }
        drawTexture = Sprites[SPRITE_SITE_ROCKET].m_pTexture;
        screenStretchCrossHairX = SCREEN_STRETCH_X(24.0f);
        screenStretchCrossHairY = SCREEN_STRETCH_Y(24.0f);
        screenOffsetCenterX     = SCREEN_STRETCH_X(20.0f);
        screenOffsetCenterY     = SCREEN_STRETCH_Y(20.0f);
    }

    if (drawTexture) {
        RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, RWRSTATE(rwFILTERLINEAR));
        RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,  RWRSTATE(FALSE));

        RwRenderStateSet(rwRENDERSTATEZTESTENABLE,    RWRSTATE(FALSE));
        RwRenderStateSet(rwRENDERSTATETEXTUREADDRESS, RWRSTATE(RwTextureAddressMode::rwTEXTUREADDRESSCLAMP));
        RwRenderStateSet(rwRENDERSTATETEXTURERASTER,  RWRSTATE(drawTexture->raster));

        const auto RenderOneXLUSprite = [=](float x, float y, auto u, auto v) {
            CSprite::RenderOneXLUSprite(
                { x, y, 1.0f } ,
                { screenStretchCrossHairX / 2.0f, screenStretchCrossHairY / 2.0f },
                255, 255, 255, 255,
                0.01f,
                255,
                u, v
            );
        };

        RenderOneXLUSprite(
            (SCREEN_WIDTH  / 2.0f) - (screenStretchCrossHairX / 2.0f) - screenOffsetCenterX,
            (SCREEN_HEIGHT / 2.0f) - (screenStretchCrossHairY / 2.0f) - screenOffsetCenterY,
            0, 0
        );

        RenderOneXLUSprite(
            (SCREEN_WIDTH  / 2.0f) + (screenStretchCrossHairX / 2.0f) + screenOffsetCenterX,
            (SCREEN_HEIGHT / 2.0f) - (screenStretchCrossHairY / 2.0f) - screenOffsetCenterY,
            1, 0
        );

        RenderOneXLUSprite(
            (SCREEN_WIDTH  / 2.0f) - (screenStretchCrossHairX / 2.0f) - screenOffsetCenterX,
            (SCREEN_HEIGHT / 2.0f) + (screenStretchCrossHairY / 2.0f) + screenOffsetCenterY,
            0, 1
        );

        RenderOneXLUSprite(
            (SCREEN_WIDTH  / 2.0f) + (screenStretchCrossHairX / 2.0f) + screenOffsetCenterX,
            (SCREEN_HEIGHT / 2.0f) + (screenStretchCrossHairY / 2.0f) + screenOffsetCenterY,
            1, 1
        );
        return;
    }
}

// 0x58D580
float CHud::DrawFadeState(DRAW_FADE_STATE fadingElement, int32 forceFadingIn) {
    return plugin::CallAndReturn<float, 0x58D580, DRAW_FADE_STATE, int32>(fadingElement, forceFadingIn);

    uint32 state, timer, fadeTimer;
    switch (fadingElement) {
    case WANTED_STATE:
        fadeTimer = m_WantedFadeTimer;
        state = m_WantedState;
        timer = m_WantedTimer;
        break;
    case ENERGY_LOST_STATE:
        fadeTimer = m_EnergyLostFadeTimer;
        state = m_EnergyLostState;
        timer = m_EnergyLostTimer;
        break;
    case DISPLAY_SCORE_STATE:
        fadeTimer = m_DisplayScoreFadeTimer;
        state = m_DisplayScoreState;
        timer = m_DisplayScoreTimer;
        break;
    case WEAPON_STATE:
        fadeTimer = m_WeaponFadeTimer;
        state = m_WeaponState;
        timer = m_WeaponTimer;
        break;
    default:
        state = fadingElement;
        timer = fadingElement;
        fadeTimer = fadingElement;
        break;
    }

    if (forceFadingIn) {
        switch (state) {
        case NAME_DONT_SHOW:
            fadeTimer = 0;
            break;
        case NAME_SWITCH:
        case NAME_FADE_OUT:
            timer = 5;
            state = NAME_FADE_IN;
            break;
        default:
            break;
        }
    }

    float alpha = 255.0f;
    if (state != NAME_DONT_SHOW) {
        switch (state) {
        case NAME_SWITCH:
            fadeTimer = 1000;
            if (timer > 10'000) {
                fadeTimer = 3000;
                state = NAME_FADE_OUT;
            }
            break;
        case NAME_FADE_IN:
            fadeTimer += (uint32)CTimer::GetTimeStepInMS();
            if (fadeTimer > 1000) {
                state = NAME_SWITCH;
                fadeTimer = 1000;
            }
            alpha = float(fadeTimer) / 1000.0f * 255.0f;
            break;
        case NAME_FADE_OUT:
            fadeTimer -= (uint32)CTimer::GetTimeStepInMS();
            if (fadeTimer < 0) {
                fadeTimer = 0;
                state = NAME_DONT_SHOW;
            }
            alpha = float(fadeTimer) / 1000.0f * 255.0f;
            break;
        default:
            break;
        }
        timer += (uint32)CTimer::GetTimeStepInMS();
    }

    switch (fadingElement) {
    case WANTED_STATE:
        m_WantedFadeTimer = fadeTimer;
        m_WantedState = state;
        m_WantedTimer = timer;
        break;
    case ENERGY_LOST_STATE:
        m_EnergyLostFadeTimer = fadeTimer;
        m_EnergyLostState = state;
        m_EnergyLostTimer = timer;
        break;
    case DISPLAY_SCORE_STATE:
        m_DisplayScoreFadeTimer = fadeTimer;
        m_DisplayScoreState = state;
        m_DisplayScoreTimer = timer;
        break;
    case WEAPON_STATE:
        m_WeaponFadeTimer = fadeTimer;
        m_WeaponState = state;
        m_WeaponTimer = timer;
        break;
    default:
        break;
    }

    return std::clamp(alpha, 0.0f, 255.0f);
}

// 0x58B6E0
void CHud::DrawHelpText() {
    plugin::Call<0x58B6E0>();
}

// 0x58B180
void CHud::DrawMissionTimers() {
    plugin::Call<0x58B180>();
}

// 0x58D240
void CHud::DrawMissionTitle() {
    auto& message      = m_BigMessage[STYLE_BOTTOM_RIGHT];
    auto& messageX     = BigMessageX[STYLE_BOTTOM_RIGHT];
    auto& messageAlpha = BigMessageAlpha[STYLE_BOTTOM_RIGHT];
    auto& messageInUse = BigMessageInUse[STYLE_BOTTOM_RIGHT];

    if (!message[0]) {
        messageX = 0.0f;
        return;
    }

    if (messageX == 0.0f) {
        messageInUse = -60.0f;
        messageX = 1.0f;
        m_ZoneState = NAME_DONT_SHOW;
        m_ZoneFadeTimer = 0;
        SetHelpMessage(nullptr, true, false, false);
        return;
    }

    CFont::SetBackground(false, false);
    CFont::SetProportional(true);
    CFont::SetRightJustifyWrap(0.0f);
    CFont::SetOrientation(eFontAlignment::ALIGN_RIGHT);
    CFont::SetFontStyle(FONT_PRICEDOWN);
    CFont::SetScale(SCREEN_STRETCH_X(1.0f), SCREEN_SCALE_Y(1.3f));

    if (messageInUse >= SCREEN_WIDTH - 20.0f) { // magic shit
        messageX += CTimer::GetTimeStep();
        if (messageX >= 120.0f) {
            messageX = 120.0f;
            messageAlpha -= CTimer::GetTimeStepInMS();
        }
        if (messageAlpha <= 0.0f) {
            messageAlpha = 0.0f;
            message[0] = '\0';
            messageX = 0.0f;
        }
    } else {
        messageAlpha = 255.0f;
        messageInUse += CTimer::GetTimeStepInMS() * 0.3f;
    }

    CFont::SetEdge(2);
    CFont::SetDropColor({ 0, 0, 0, uint8(messageAlpha) });
    CFont::SetColor(HudColour.GetRGBA(HUD_COLOUR_GOLD, (uint8)messageAlpha));
    CFont::PrintStringFromBottom(SCREEN_SCALE_FROM_RIGHT(20.0f), SCREEN_SCALE_FROM_BOTTOM(115.0f), message);
    CFont::SetEdge(0);
}

// It looks like the original, but needs to be recheck
// 0x58CC80
void CHud::DrawOddJobMessage(bool displayImmediately) {
    const auto& m1 = m_BigMessage[STYLE_BOTTOM_RIGHT];
    const auto& m4 = m_BigMessage[STYLE_MIDDLE_SMALLER_HIGHER];
    if (displayImmediately == CTheScripts::bDrawOddJobTitleBeforeFade && !m1[0] && m4[0]) {
        CFont::SetBackground(false, false);
        CFont::SetJustify(false);
        CFont::SetScaleForCurrentLanguage(SCREEN_STRETCH_X(0.6f), SCREEN_SCALE_Y(1.35f));
        CFont::SetOrientation(eFontAlignment::ALIGN_CENTER);
        CFont::SetProportional(true);
        CFont::SetCentreSize(SCREEN_STRETCH_X(350.0f));
        CFont::SetFontStyle(FONT_MENU);
        CFont::SetEdge(2);
        CFont::SetDropColor({ 0, 0, 0, 255 });
        CFont::SetColor(HudColour.GetRGB(HUD_COLOUR_GOLD));
        CFont::PrintStringFromBottom(static_cast<float>(RsGlobal.maximumWidth / 2), SCREEN_STRETCH_Y(140.0f), m4);
    }

    if (!displayImmediately)
        return;

    const auto& m6 = m_BigMessage[STYLE_LIGHT_BLUE_TOP];
    if (m6[0]) {
        CFont::SetBackground(false, false);
        CFont::SetJustify(false);
        CFont::SetScaleForCurrentLanguage(SCREEN_STRETCH_X(1.0f), SCREEN_SCALE_Y(1.8f));
        CFont::SetOrientation(eFontAlignment::ALIGN_CENTER);
        CFont::SetProportional(true);
        CFont::SetCentreSize(SCREEN_STRETCH_X(500.0f));
        CFont::SetFontStyle(FONT_PRICEDOWN);
        CFont::SetEdge(2);
        CFont::SetDropColor({ 0, 0, 0, 255 });
        CFont::SetColor(HudColour.GetRGB(HUD_COLOUR_LIGHT_BLUE));
        CFont::PrintString(static_cast<float>(RsGlobal.maximumWidth / 2), SCREEN_STRETCH_Y(60.0f), m6);
    }

    const auto& m3 = m_BigMessage[STYLE_MIDDLE_SMALLER];
    if (m3[0]) {
        CFont::SetBackground(false, false);
        CFont::SetJustify(false);
        CFont::SetScaleForCurrentLanguage(SCREEN_STRETCH_X(0.6f), SCREEN_SCALE_Y(1.35f));
        CFont::SetOrientation(eFontAlignment::ALIGN_CENTER);
        CFont::SetProportional(true);
        CFont::SetCentreSize(SCREEN_STRETCH_X(500.0f));
        CFont::SetFontStyle(FONT_MENU);
        CFont::SetEdge(2);
        CFont::SetDropColor({ 0, 0, 0, 255 });
        CFont::SetColor(HudColour.GetRGB(HUD_COLOUR_GOLD));
        CFont::PrintString(static_cast<float>(RsGlobal.maximumWidth / 2), SCREEN_STRETCH_Y(155.0f), m3);
    }

    if (OddJob2OffTimer > 0.0f) {
        OddJob2OffTimer -= CTimer::GetTimeStepInMS();
    }

    const auto& m5 = m_BigMessage[STYLE_WHITE_MIDDLE_SMALLER];
    if (!m5[0])
        return;

    if (OddJob2OffTimer > 0.0f)
        return;

    switch (OddJob2On) {
    case 0:
        OddJob2XOffset = 380.0f;
        OddJob2On = 1;
        break;
    case 1:
        if (OddJob2XOffset <= 2.0f) {
            OddJob2On = 2;
            OddJob2Timer = 0;
        } else {
            OddJob2XOffset -= std::min(OddJob2XOffset / 6.0f, 40.0f);
        }
        break;
    case 2:
        OddJob2Timer += (uint16)CTimer::GetTimeStepInMS();
        if (OddJob2Timer > 1500) {
            OddJob2On = 3;
        }
        break;
    case 3:
        OddJob2XOffset -= std::max(OddJob2XOffset / 5.0f, 30.0f);
        if (OddJob2XOffset < -380.0f) {
            OddJob2On = 0;
            OddJob2OffTimer = 5000.0f;
        }
        break;
    default:
        break;
    }

    if (!m1[0]) {
        CFont::SetBackground(false, false);
        CFont::SetScaleForCurrentLanguage(SCREEN_STRETCH_X(0.6f), SCREEN_SCALE_Y(1.35f));
        CFont::SetOrientation(eFontAlignment::ALIGN_CENTER);
        CFont::SetProportional(true);
        CFont::SetCentreSize(SCREEN_STRETCH_X(500.0f));
        CFont::SetFontStyle(FONT_MENU);
        CFont::SetEdge(2);
        CFont::SetDropColor({ 0, 0, 0, 255 });
        CFont::SetColor(HudColour.GetRGB(HUD_COLOUR_LIGHT_GRAY));
        CFont::PrintString(static_cast<float>(RsGlobal.maximumWidth / 2), SCREEN_STRETCH_Y(217.0f), m5);
    }
}

// 0x58A330
void CHud::DrawRadar() {
    if (CEntryExitManager::ms_exitEnterState == EXIT_ENTER_STATE_1 ||
        CEntryExitManager::ms_exitEnterState == EXIT_ENTER_STATE_2 ||
        FrontEndMenuManager.m_nRadarMode == eRadarMode::OFF ||
        (m_ItemToFlash == ITEM_RADAR && EachFrames(8))
    ) {
        return;
    }

    RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, RWRSTATE(rwFILTERLINEAR));
    RwRenderStateSet(rwRENDERSTATESHADEMODE,     RWRSTATE(rwFILTERNEAREST));

    CRadar::DrawMap();

    if (FrontEndMenuManager.m_nRadarMode == eRadarMode::BLIPS_ONLY) {
        CRadar::DrawBlips();
        return;
    }

    CVehicle* vehicle = FindPlayerVehicle();
    CRect rect;
    if (vehicle && vehicle->IsSubPlane() && vehicle->m_nModelIndex != MODEL_VORTEX) {
        float angle = PI - std::atan2(-vehicle->m_matrix->GetRight().z, vehicle->m_matrix->GetUp().z);
        CRadar::DrawRotatingRadarSprite(
            Sprites[SPRITE_RADAR_RING_PLANE],
            SCREEN_STRETCH_X(87.0f),
            SCREEN_STRETCH_FROM_BOTTOM(66.0f),
            angle,
            (uint32)SCREEN_STRETCH_X(78.0f),
            (uint32)SCREEN_STRETCH_Y(59.0f),
            CRGBA(255, 255, 255, 255)
        );
    }

    CPlayerPed* player = FindPlayerPed();
    // Draws Altimeter on Planes And Helis or when parachuting down
    if (vehicle && (vehicle->IsSubPlane() || vehicle->IsSubHeli() && vehicle->m_nModelIndex != MODEL_VORTEX)
        || player->GetActiveWeapon().m_Type == WEAPON_PARACHUTE
    ) {
        rect.left   = SCREEN_STRETCH_X(40.0f) - SCREEN_STRETCH_X(20.0f);
        rect.bottom    = SCREEN_STRETCH_FROM_BOTTOM(104.0f);
        rect.right  = SCREEN_STRETCH_X(40.0f) - SCREEN_STRETCH_X(10.0f);
        rect.top = SCREEN_STRETCH_Y(76.0f) + SCREEN_STRETCH_FROM_BOTTOM(104.0f);
        CSprite2d::DrawRect(rect, { 10, 10, 10, 100 }); // rectangle

        const CVector& pos = vehicle ? vehicle->GetPosition() : player->GetPosition();
        auto lineY = 950.0f;
        if (pos.z <= 200.0f) {
            lineY = 200.0f;
        };
        RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RWRSTATE(NULL));

        rect.left   = SCREEN_STRETCH_X(40.0f) - SCREEN_STRETCH_X(25.0f);
        rect.bottom    = SCREEN_STRETCH_FROM_BOTTOM(104.0f) + SCREEN_STRETCH_Y(76.0f) - std::min(SCREEN_STRETCH_Y(76.0f), SCREEN_STRETCH_Y(76.0f) * pos.z / lineY);
        rect.right  = SCREEN_STRETCH_X(40.0f) - 5.0f;
        rect.top = rect.bottom + 2.0f;
        CSprite2d::DrawRect(rect, { 200, 200, 200, 200 }); // horizontal line (current height)
    }

    // NOTSA: rects are optimized
    const auto black = CRGBA(0, 0, 0, 255);

    rect.left   = SCREEN_STRETCH_X(36.0f);
    rect.bottom    = SCREEN_STRETCH_FROM_BOTTOM(108.0f);
    rect.right  = SCREEN_STRETCH_X(87.0f);
    rect.top = SCREEN_STRETCH_FROM_BOTTOM(66.0f);
    Sprites[SPRITE_RADAR_DISC].Draw(rect, black); // top left

    rect.bottom = SCREEN_STRETCH_FROM_BOTTOM(24.0f);
    Sprites[SPRITE_RADAR_DISC].Draw(rect, black); // bottom left

    rect.left = SCREEN_STRETCH_X(138.0f);
    rect.bottom  = SCREEN_STRETCH_FROM_BOTTOM(108.0f);
    Sprites[SPRITE_RADAR_DISC].Draw(rect, black); // top right

    rect.bottom = SCREEN_STRETCH_FROM_BOTTOM(24.0f);
    Sprites[SPRITE_RADAR_DISC].Draw(rect, black); // bottom right

    CRadar::DrawBlips();
}

// 0x58C080
void CHud::DrawScriptText(bool displayImmediately) {
    CTheScripts::DrawScriptSpritesAndRectangles(displayImmediately);

    GxtChar textFormatted[400];
    for (auto& scriptText : CTheScripts::IntroTextLines) { // todo: NOTSA optimization std::span{ CTheScripts::IntroTextLines, CTheScripts::NumberOfIntroTextLinesThisFrame }
        if (!scriptText.m_szGxtEntry[0])
            continue;
        if (scriptText.m_bDrawBeforeFade != displayImmediately)
            continue;

        CFont::SetScale(SCREEN_SCALE_X(scriptText.m_fLetterWidth), SCREEN_SCALE_Y(scriptText.m_fLetterHeight / 2.0f));
        CFont::SetColor(scriptText.m_Color);
        CFont::SetJustify(scriptText.m_bJustify);

        if (scriptText.m_bRightJustify)
            CFont::SetOrientation(eFontAlignment::ALIGN_RIGHT);
        else
            CFont::SetOrientation((eFontAlignment)!scriptText.m_bCentered);

        CFont::SetWrapx(SCREEN_SCALE_X(scriptText.m_fLineHeight)); // todo: SCREEN_SCALE_X used while height passed - it's ok?
        CFont::SetCentreSize(SCREEN_SCALE_X(scriptText.m_fLineWidth));
        CFont::SetBackground(scriptText.m_bWithBackground, false);
        CFont::SetBackgroundColor(scriptText.m_BackgroundBoxColor);
        CFont::SetProportional(scriptText.m_bProportional);
        CFont::SetDropColor(scriptText.m_BackgroundColor);

        if (scriptText.m_nOutlineType)
            CFont::SetEdge(scriptText.m_nOutlineType);
        else
            CFont::SetDropShadowPosition(scriptText.m_nShadowType);

        CFont::SetFontStyle((eFontStyle)scriptText.m_nFont);
        CMessages::InsertNumberInString(TheText.Get(scriptText.m_szGxtEntry), scriptText.param1, scriptText.param2, -1, -1, -1, -1, textFormatted);
        CMessages::InsertPlayerControlKeysInString(textFormatted);

        // todo: Replace DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT
        // The first letter doesn't look good in window mode, but it looks fine in full-screen mode
        CFont::PrintString(SCREEN_SCALE_FROM_RIGHT(DEFAULT_SCREEN_WIDTH - scriptText.m_Pos.x), SCREEN_SCALE_FROM_BOTTOM(DEFAULT_SCREEN_HEIGHT - scriptText.m_Pos.y), textFormatted);
        CFont::SetEdge(0);
    }
}

// 0x58C250
void CHud::DrawSubtitles() {
    plugin::Call<0x58C250>();
}

// 0x58C6A0
void CHud::DrawSuccessFailedMessage() {
    plugin::Call<0x58C6A0>();
}

// 0x58AEA0
void CHud::DrawVehicleName() {
    if (!m_pVehicleName) {
        m_VehicleState = NAME_DONT_SHOW;
        m_VehicleNameTimer = 0;
        m_VehicleFadeTimer = 0;
        m_pLastVehicleName = nullptr;
        return;
    }

    if (m_pVehicleName != m_pLastVehicleName) {
        switch (m_VehicleState) {
        case NAME_DONT_SHOW:
            m_VehicleState = NAME_FADE_IN;
            m_VehicleNameTimer = 0;
            m_VehicleFadeTimer = 0;
            m_pVehicleNameToPrint = m_pVehicleName;
            if (m_ZoneState == NAME_SHOW || m_ZoneState == NAME_FADE_IN) {
                m_ZoneState = NAME_FADE_OUT;
            }
            break;
        case NAME_SHOW:
        case NAME_FADE_IN:
        case NAME_FADE_OUT:
        case NAME_SWITCH:
            m_VehicleState = NAME_SWITCH;
            m_VehicleNameTimer = 0;
            break;
        default:
            break;
        }
        m_pLastVehicleName = m_pVehicleName;
    }

    if (!m_VehicleState)
        return;

    float alpha = 0.0f;
    switch (m_VehicleState) {
    case NAME_SHOW:
        if (m_VehicleNameTimer > 3000) {
            m_VehicleState = NAME_FADE_OUT;
            m_VehicleFadeTimer = 1000;
        }
        alpha = 255.0f;
        break;
    case NAME_FADE_IN:
        m_VehicleFadeTimer += (int32)CTimer::GetTimeStepInMS();
        if (m_VehicleFadeTimer > 1000) {
            m_VehicleFadeTimer = 1000;
            m_VehicleState = NAME_SHOW;
        }
        alpha = float(m_VehicleFadeTimer) / 1000.0f * 255.0f;
        break;
    case NAME_FADE_OUT:
        m_VehicleFadeTimer -= (int32)CTimer::GetTimeStepInMS();
        if (m_VehicleFadeTimer < 0) {
            m_VehicleState = NAME_DONT_SHOW;
            m_VehicleFadeTimer = 0;
        }
        alpha = float(m_VehicleFadeTimer) / 1000.0f * 255.0f;
        break;
    case NAME_SWITCH:
        m_VehicleFadeTimer -= (int32)CTimer::GetTimeStepInMS();
        if (m_VehicleFadeTimer < 0) {
            m_VehicleNameTimer = 0;
            m_VehicleState = NAME_FADE_IN;
            m_VehicleFadeTimer = 0;
            m_pVehicleNameToPrint = m_pLastVehicleName;
        }
        alpha = float(m_VehicleFadeTimer) / 1000.0f * 255.0f;
        break;
    default:
        break;
    }

    if (!m_Message[0]) {
        m_VehicleNameTimer += (int32)CTimer::GetTimeStepInMS();
        CFont::SetProportional(true);
        CFont::SetBackground(false, false);
        CFont::SetScaleForCurrentLanguage(SCREEN_STRETCH_X(1.0f), SCREEN_SCALE_Y(1.5f));
        CFont::SetOrientation(eFontAlignment::ALIGN_RIGHT);
        CFont::SetRightJustifyWrap(0.0f);
        CFont::SetFontStyle(eFontStyle::FONT_MENU);
        CFont::SetEdge(2);
        CFont::SetColor(HudColour.GetRGBA(HUD_COLOUR_GREEN, (uint8)alpha));
        CFont::SetDropColor({ 0, 0, 0, (uint8)alpha });
        if (CTheScripts::bDisplayHud) {
            CFont::PrintString(
                SCREEN_STRETCH_FROM_RIGHT(32.0f),
                SCREEN_STRETCH_FROM_BOTTOM(104.0f),
                m_pVehicleNameToPrint
            );
        }
        CFont::SetSlant(0.0f);
    }
}

// 0x589650
void CHud::DrawVitalStats() {
    plugin::Call<0x589650>();
}

// 0x588A50
void CHud::GetRidOfAllHudMessages(bool arg0) {
    std::ranges::fill(m_pHelpMessageToPrint, '\0');
    std::ranges::fill(m_pLastHelpMessage, '\0');
    std::ranges::fill(m_pHelpMessage, '\0');
    std::ranges::fill(m_Message, '\0');

    m_ZoneNameTimer               = 0;
    m_pZoneName                   = nullptr;
    m_ZoneState                   = NAME_DONT_SHOW;
    m_nHelpMessageTimer           = 0;
    m_nHelpMessageFadeTimer       = 0;
    m_nHelpMessageState           = 0;
    m_bHelpMessageQuick           = false;
    m_nHelpMessageMaxStatValue    = 1000;
    m_nHelpMessageStatId          = 0;
    m_fHelpMessageStatUpdateValue = 0.0f;
    m_bHelpMessagePermanent       = false;
    m_fHelpMessageTime            = 1.0f;
    m_pVehicleName                = nullptr;
    m_pVehicleNameToPrint         = nullptr;
    m_VehicleNameTimer            = 0;
    m_VehicleFadeTimer            = 0;
    m_VehicleState                = NAME_DONT_SHOW;

    for (auto i = 0; i < NUM_MESSAGE_STYLES; ++i) {
        if (BigMessageX[i] != 0.0f)
            continue;

        if (arg0) {
            if (BigMessageX[i] == BigMessageX[STYLE_BOTTOM_RIGHT] ||
                BigMessageX[i] == BigMessageX[STYLE_MIDDLE_SMALLER_HIGHER]
            ) {
                continue;
            }
        }
        std::ranges::fill(m_BigMessage[i], '\0');
    }
}

// 0x5893B0
void CHud::DrawAmmo(CPed* ped, int32 x, int32 y, float alpha) {
    const auto MAX_CLIP = 9999;

    const auto& weapon = ped->GetActiveWeapon();
    const auto& totalAmmo = weapon.m_TotalAmmo;
    const auto& ammoInClip = weapon.m_AmmoInClip;
    const auto& ammoClip = CWeaponInfo::GetWeaponInfo(weapon.m_Type, ped->GetWeaponSkill())->m_nAmmoClip;

    if (ammoClip <= 1 || ammoClip >= 1000) {
        sprintf_s(gString, "%d", totalAmmo);
    } else {
        uint32 total, current;

        if (weapon.m_Type == WEAPON_FLAMETHROWER ) {
            uint32 out = MAX_CLIP;
            if ((totalAmmo - ammoInClip) / 10 <= MAX_CLIP) {
                out = (totalAmmo - ammoInClip) / 10u;
            }
            total = out;

            current = ammoInClip / 10;
        } else {
            auto out = totalAmmo - ammoInClip;
            if (totalAmmo - ammoInClip > MAX_CLIP) {
                out = MAX_CLIP;
            }
            total = out;

            current = ammoInClip;
        }
        sprintf_s(gString, "%d-%d", total, current);
    }
    AsciiToGxtChar(gString, gGxtString);

    CFont::SetBackground(false, false);
    CFont::SetScale(SCREEN_STRETCH_X(0.3f), SCREEN_STRETCH_Y(0.7f));
    CFont::SetOrientation(eFontAlignment::ALIGN_CENTER);
    CFont::SetCentreSize(SCREEN_STRETCH_Y(640.0f));
    CFont::SetProportional(true);
    CFont::SetEdge(1);
    CFont::SetDropColor({ 0, 0, 0, 255 });
    CFont::SetFontStyle(eFontStyle::FONT_SUBTITLES);

    if (   totalAmmo - weapon.m_AmmoInClip >= MAX_CLIP
        || CDarkel::FrenzyOnGoing()
        || weapon.m_Type == WEAPON_UNARMED
        || weapon.m_Type == WEAPON_DETONATOR
        || weapon.m_Type == WEAPON_DILDO1
        || weapon.m_Type == WEAPON_DILDO2
        || weapon.m_Type == WEAPON_VIBE1
        || weapon.m_Type == WEAPON_VIBE2
        || weapon.m_Type == WEAPON_FLOWERS
        || weapon.m_Type == WEAPON_CANE
        || weapon.m_Type == WEAPON_PARACHUTE
        || CWeaponInfo::GetWeaponInfo(weapon.m_Type)->m_nWeaponFire == WEAPON_FIRE_USE
        || CWeaponInfo::GetWeaponInfo(weapon.m_Type)->m_nSlot <= 1
    ) {
        CFont::SetEdge(0);
        return;
    }

    CFont::SetColor(HudColour.GetRGBA(HUD_COLOUR_LIGHT_BLUE, (uint8)alpha));
    CFont::PrintString((float)x, (float)y, gGxtString);
    CFont::SetEdge(0);
}

// 0x58EAF0
void CHud::DrawPlayerInfo() {
    return plugin::Call<0x58EAF0>();
}

inline void CHud::DrawClock() {
    char ascii[16];
    GxtChar gxtText[16];
    CFont::SetBackground(false, false);
    CFont::SetScale(SCREEN_STRETCH_X(0.55f), SCREEN_STRETCH_Y(1.1f));
    CFont::SetProportional(false);
    CFont::SetFontStyle(FONT_PRICEDOWN);
    CFont::SetOrientation(eFontAlignment::ALIGN_RIGHT);
    CFont::SetRightJustifyWrap(0.0f);
    CFont::SetEdge(2);
    CFont::SetDropColor({0, 0, 0, 255});
    sprintf_s(ascii, "%02d:%02d", CClock::ms_nGameClockHours, CClock::ms_nGameClockMinutes);
    AsciiToGxtChar(ascii, gxtText);
    CFont::SetColor(HudColour.GetRGB(HUD_COLOUR_LIGHT_GRAY));
    CFont::PrintString(SCREEN_STRETCH_FROM_RIGHT(32.0f), SCREEN_STRETCH_Y(22.0f), gxtText);
    CFont::SetEdge(0);
}

inline void CHud::DrawMoney(const CPlayerInfo& playerInfo, uint8 alpha) {
    char ascii[16];
    GxtChar gxtText[16];

    if (playerInfo.m_nDisplayMoney < 0) {
        CFont::SetColor(HudColour.GetRGBA(HUD_COLOUR_RED, alpha));
        auto m_nDisplayMoney = playerInfo.m_nDisplayMoney;
        if (m_nDisplayMoney < 0) {
            m_nDisplayMoney = -m_nDisplayMoney;
        }
        sprintf_s(ascii, "-$%07d", m_nDisplayMoney);
    } else {
        CFont::SetColor(HudColour.GetRGBA(HUD_COLOUR_GREEN, alpha));
        sprintf_s(ascii, "$%08d", std::abs(playerInfo.m_nDisplayMoney));
    }
    AsciiToGxtChar(ascii, gxtText);
    CFont::SetProportional(false);
    CFont::SetBackground(false, false);
    CFont::SetScale(SCREEN_STRETCH_X(0.55f), SCREEN_STRETCH_Y(1.1f));
    CFont::SetOrientation(eFontAlignment::ALIGN_RIGHT);
    CFont::SetRightJustifyWrap(0.0f);
    CFont::SetFontStyle(FONT_PRICEDOWN);
    CFont::SetDropShadowPosition(0);
    CFont::SetEdge(2);
    CFont::SetDropColor({ 0, 0, 0, uint8(alpha) });
    CFont::PrintString(SCREEN_STRETCH_FROM_RIGHT(32.0f), GetYPosBasedOnHealth(CWorld::PlayerInFocus, SCREEN_STRETCH_Y(89.0f), 12), gxtText);
    CFont::SetEdge(0);
}

inline void CHud::DrawWeapon(CPlayerPed* ped0, CPlayerPed* ped1) {
    const auto magic = SCREEN_WIDTH * 0.17343046f; // todo: magic
    if (m_WeaponState) {
        DrawWeaponIcon(ped0, (int32)(SCREEN_WIDTH - (SCREEN_STRETCH_X(32.0f) + magic)), (int32)SCREEN_STRETCH_Y(20.0f), (float)m_WeaponFadeTimer);
        if (ped1) {
            const auto posX = (int32)(SCREEN_WIDTH - (SCREEN_STRETCH_X(32.0f) + 111.0f));
            const auto posY = (int32)GetYPosBasedOnHealth(CWorld::PlayerInFocus, SCREEN_STRETCH_Y(138.0f), 12);
            DrawWeaponIcon(ped1, posX, posY, (float)m_WeaponFadeTimer);
        }

        const auto ammoPosX = (int32)(SCREEN_WIDTH - (magic + SCREEN_STRETCH_X(32.0f)) + SCREEN_STRETCH_X(47.0f / 2.0f));
        const auto ammoPosY = SCREEN_STRETCH_Y(43.0f);
        DrawAmmo(ped0, ammoPosX, (int32)ammoPosY + (int32)SCREEN_STRETCH_Y(20.0f), (float)m_WeaponFadeTimer);
        if (ped1) {
            const auto posY = (int32)GetYPosBasedOnHealth(CWorld::PlayerInFocus, ammoPosY + SCREEN_STRETCH_Y(138.0f), 12);
            DrawAmmo(ped1, ammoPosX, posY, (float)m_WeaponFadeTimer);
        }
    }
}

// 0x58A160
void CHud::DrawTripSkip() {
    CRect rect{
        SCREEN_STRETCH_X(54.0f),
        SCREEN_STRETCH_FROM_BOTTOM(189.0f),
        SCREEN_STRETCH_X(118.0f),
        SCREEN_STRETCH_FROM_BOTTOM(125.0f)
    };
    Sprites[SPRITE_SKIP_ICON].Draw(rect, CRGBA(255, 255, 255, 255));

    CFont::SetBackground(false, false);
    CFont::SetScale(SCREEN_STRETCH_X(0.3f), SCREEN_SCALE_Y(0.7f));
    CFont::SetOrientation(eFontAlignment::ALIGN_CENTER);
    CFont::SetCentreSize(SCREEN_WIDTH);
    CFont::SetProportional(true);
    CFont::SetEdge(1);
    CFont::SetDropColor({ 0, 0, 0, 255 });
    CFont::SetFontStyle(eFontStyle::FONT_MENU);
    CFont::SetColor(HudColour.GetRGB(HUD_COLOUR_LIGHT_GRAY));
    CFont::PrintString(
        SCREEN_STRETCH_X(64.0f) / 2.0f + SCREEN_STRETCH_X(54.0f),
        SCREEN_STRETCH_FROM_BOTTOM(127.0f),
        TheText.Get("FEC_TSK") // TRIP SKIP
    );
}

// 0x58D9A0
void CHud::DrawWanted() {
    return plugin::Call<0x58D9A0>();
}

// 0x58D7D0
void CHud::DrawWeaponIcon(CPed* ped, int32 x, int32 y, float alpha) {
    const auto x0 = (float)x;
    const auto y0 = (float)y;
    const float width  = SCREEN_STRETCH_X(47.0f);
    const float height = SCREEN_STRETCH_Y(58.0f);
    const float halfWidth  = width / 2.0f;
    const float halfHeight = height / 2.0f;

    RwRenderStateSet(rwRENDERSTATETEXTUREFILTER,     RWRSTATE(rwFILTERLINEAR));

    auto modelId = ped->GetActiveWeapon().GetWeaponInfo().m_nModelId1;
    if (modelId <= 0) {
        Sprites[SPRITE_FIST].Draw({ x0, y0, width + x0, height + y0 }, CRGBA(255, 255, 255, (uint8)alpha));
        return;
    }

    auto mi = CModelInfo::GetModelInfo(modelId);
    auto txd = CTxdStore::ms_pTxdPool->GetAt(mi->m_nTxdIndex);
    if (!txd)
        return;

    auto texture = RwTexDictionaryFindHashNamedTexture(txd->m_pRwDictionary, CKeyGen::AppendStringToKey(mi->m_nKey, "ICON"));
    if (!texture)
        return;

    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,   RWRSTATE(NULL));
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RWRSTATE(RwTextureGetRaster(texture)));
    CSprite::RenderOneXLUSprite(
        { x0 + halfWidth, y0 + halfHeight, 1.0f },
        { halfWidth, halfHeight },
        255u, 255u, 255u, 255,
        1.0f,
        255,
        0, 0
    );
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,  RWRSTATE(FALSE));
}

// 0x5890A0
void CHud::RenderArmorBar(int32 playerId, int32 x, int32 y) {
    auto* player = FindPlayerPed(playerId);
    if ((m_ItemToFlash == ITEM_ARMOUR && EachFrames(8)) || player->m_fArmour <= 1.0f)
        return;

    const auto info = player->GetPlayerInfoForThisPlayerPed();
    CSprite2d::DrawBarChart(
        (float)x,
        (float)y,
        (uint16)SCREEN_STRETCH_X(62.0f),
        (uint8)SCREEN_STRETCH_Y(9.0f),
        player->m_fArmour / (float)info->m_nMaxArmour * 100.0f,
        false,
        false,
        true,
        HudColour.GetRGB(HUD_COLOUR_LIGHT_GRAY),
        CRGBA(0, 0, 0, 0)
    );
}

// 0x589190
void CHud::RenderBreathBar(int32 playerId, int32 x, int32 y) {
    if (m_ItemToFlash == ITEM_BREATH && EachFrames(8))
        return;

    auto* player = FindPlayerPed(playerId);
    CSprite2d::DrawBarChart(
        (float)x,
        (float)y,
        (uint16)SCREEN_STRETCH_X(62.0f),
        (uint8)SCREEN_STRETCH_Y(9.0f),
        player->m_pPlayerData->m_fBreath / CStats::GetFatAndMuscleModifier(STAT_MOD_AIR_IN_LUNG) * 100.0f,
        false,
        false,
        true,
        HudColour.GetRGB(HUD_COLOUR_LIGHT_BLUE),
        CRGBA(0, 0, 0, 0)
    );
}

// 0x589270
void CHud::RenderHealthBar(int32 playerId, int32 x, int32 y) {
    if (m_ItemToFlash == ITEM_HEALTH && EachFrames(8))
        return;

    auto* player = FindPlayerPed(playerId);
    if ((int)player->m_fHealth < 10 && EachFrames(8))
        return;

    const float x109 = SCREEN_STRETCH_X(109.0f);
    const auto info = player->GetPlayerInfoForThisPlayerPed();
    const auto totalWidth = uint16(x109 * (float)info->m_nMaxHealth / CStats::GetFatAndMuscleModifier(STAT_MOD_10));

    CSprite2d::DrawBarChart(
        x109 - (float)totalWidth + (float)x,
        (float)y,
        totalWidth,
        (uint8)SCREEN_STRETCH_Y(9.0f),
        player->m_fHealth * 100.0f / (float)info->m_nMaxHealth,
        false,
        false,
        true,
        HudColour.GetRGB(HUD_COLOUR_RED),
        CRGBA(0, 0, 0, 0)
    );
}
