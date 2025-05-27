#include "StdInc.h"

#include "MenuManager.h"
#include "MenuManager_Internal.h"

#include "AudioEngine.h"
#include "AEUserRadioTrackManager.h"
#include "Pad.h"
#include "MenuSystem.h"
#include "PostEffects.h"
#include "app/platform/platform.h"
#include "ControllerConfigManager.h"
#include "VideoMode.h"

constexpr std::array<eControllerAction, 28> ControllerActionsAvailableOnFoot = {
    eControllerAction::CA_PED_FIRE_WEAPON,
    eControllerAction::CA_PED_CYCLE_WEAPON_RIGHT,
    eControllerAction::CA_PED_CYCLE_WEAPON_LEFT,
    eControllerAction::CA_PED_JUMPING,
    eControllerAction::CA_PED_SPRINT,
    eControllerAction::CA_CAMERA_CHANGE_VIEW_ALL_SITUATIONS,
    eControllerAction::CA_VEHICLE_ENTER_EXIT,
    eControllerAction::CA_GO_FORWARD,
    eControllerAction::CA_GO_BACK,
    eControllerAction::CA_GO_LEFT,
    eControllerAction::CA_GO_RIGHT,
    eControllerAction::CA_PED_LOOKBEHIND,
    eControllerAction::CA_PED_DUCK,
    eControllerAction::CA_PED_ANSWER_PHONE,
    eControllerAction::CA_VEHICLE_STEER_UP,
    eControllerAction::CA_VEHICLE_STEER_DOWN,
    eControllerAction::CA_VEHICLE_ACCELERATE,
    eControllerAction::CA_VEHICLE_RADIO_STATION_UP,
    eControllerAction::CA_VEHICLE_RADIO_STATION_DOWN,
    eControllerAction::CA_VEHICLE_RADIO_TRACK_SKIP,
    eControllerAction::CA_VEHICLE_HORN,
    eControllerAction::CA_VEHICLE_LOOKLEFT,
    eControllerAction::CA_VEHICLE_LOOKBEHIND,
    eControllerAction::CA_VEHICLE_MOUSELOOK,
    eControllerAction::CA_VEHICLE_TURRETLEFT,
    eControllerAction::CA_VEHICLE_TURRETRIGHT,
    eControllerAction::CA_PED_CYCLE_TARGET_LEFT,
    eControllerAction::CA_PED_FIRE_WEAPON_ALT
}; // 0x865598

constexpr std::array<eControllerAction, 25> ControllerActionsAvailableInCar = {
    eControllerAction::CA_PED_FIRE_WEAPON,
    eControllerAction::CA_PED_FIRE_WEAPON_ALT,
    eControllerAction::CA_GO_FORWARD,
    eControllerAction::CA_GO_BACK,
    eControllerAction::CA_GO_LEFT,
    eControllerAction::CA_GO_RIGHT,
    eControllerAction::CA_PED_SNIPER_ZOOM_IN,
    eControllerAction::CA_PED_SNIPER_ZOOM_OUT,
    eControllerAction::CA_PED_ANSWER_PHONE,
    eControllerAction::CA_VEHICLE_ENTER_EXIT,
    eControllerAction::CA_PED_WALK,
    eControllerAction::CA_VEHICLE_FIRE_WEAPON,
    eControllerAction::CA_VEHICLE_FIRE_WEAPON_ALT,
    eControllerAction::CA_VEHICLE_STEER_LEFT,
    eControllerAction::CA_VEHICLE_STEER_RIGHT,
    eControllerAction::CA_VEHICLE_STEER_UP,
    eControllerAction::CA_VEHICLE_BRAKE,
    eControllerAction::CA_VEHICLE_LOOKLEFT,
    eControllerAction::CA_VEHICLE_LOOKRIGHT,
    eControllerAction::CA_VEHICLE_LOOKBEHIND,
    eControllerAction::CA_VEHICLE_MOUSELOOK,
    eControllerAction::CA_TOGGLE_SUBMISSIONS,
    eControllerAction::CA_VEHICLE_HANDBRAKE,
    eControllerAction::CA_PED_1RST_PERSON_LOOK_LEFT,
    eControllerAction::CA_PED_1RST_PERSON_LOOK_RIGHT
}; // 0x865608

// 0x57C290
void CMenuManager::DrawFrontEnd() {
    if (m_bDontDrawFrontEnd)
        return;

    CFont::SetAlphaFade(255.0f);
    CSprite2d::InitPerFrame();
    CFont::InitPerFrame();
    DefinedState2d();
    SetFrontEndRenderStates();

    m_bRadioAvailable = !AudioEngine.IsRadioRetuneInProgress();

    if (m_nCurrentScreen == SCREEN_INITIAL) {
        m_nCurrentScreen = m_bMainMenuSwitch ? SCREEN_MAIN_MENU : SCREEN_PAUSE_MENU;
    }

    if (m_nCurrentScreenItem == 0 && aScreens[m_nCurrentScreen].m_aItems[0].m_nActionType == MENU_ACTION_TEXT) {
        m_nCurrentScreenItem = 1;
    }

    DrawBackground();
}

// NOTSA
void CMenuManager::DrawBuildInfo() {
    char buf[128] = {0};
    strcpy_s(buf, BUILD_NAME_FULL);
    char version[32];
    sprintf_s(
        version,
        " / RW %d.%d.%d.%d.%d",
        0xF & RwEngineGetVersion() >> 16,
        0xF & RwEngineGetVersion() >> 12,
        0xF & RwEngineGetVersion() >> 8,
        0xF & RwEngineGetVersion() >> 4,
        0xF & RwEngineGetVersion() >> 0
    );
    strcpy_s(buf + strlen(buf), 32u - strlen(buf), version);

    CFont::SetProportional(true);
    CFont::SetScale(StretchX(0.25f), StretchY(0.5f));
    CFont::SetColor({ 255, 255, 255, 100 });
    CFont::SetOrientation(eFontAlignment::ALIGN_RIGHT);
    CFont::SetFontStyle(eFontStyle::FONT_SUBTITLES);
    CFont::PrintStringFromBottom(SCREEN_STRETCH_FROM_RIGHT(10.0f), SCREEN_STRETCH_FROM_BOTTOM(10.0f), GxtCharFromAscii(buf));
}

// 0x57B750
void CMenuManager::DrawBackground() {
    if (!m_bTexturesLoaded) {
        return;
    }

    const auto GetSpriteId = [=]() -> auto {
        switch (m_nCurrentScreen) {
        case SCREEN_STATS:
        case SCREEN_LANGUAGE:
        case SCREEN_QUIT_GAME_ASK:
            return FRONTEND_SPRITE_BACK4;
        case SCREEN_START_GAME:
        case SCREEN_USER_TRACKS_OPTIONS:
        case SCREEN_OPTIONS:
            return FRONTEND_SPRITE_BACK3;
        case SCREEN_BRIEF:
        case SCREEN_LOAD_GAME:
        case SCREEN_DELETE_GAME:
        case SCREEN_GAME_SAVE:
            return FRONTEND_SPRITE_BACK2;
        case SCREEN_AUDIO_SETTINGS:
            return FRONTEND_SPRITE_BACK5;
        case SCREEN_DISPLAY_SETTINGS:
        case SCREEN_DISPLAY_ADVANCED:
            return FRONTEND_SPRITE_BACK7;
        case SCREEN_MAP:
        case SCREEN_CONTROLS_DEFINITION:
            return FRONTEND_SPRITE_ARROW;
        case SCREEN_CONTROLLER_SETUP:
        case SCREEN_MOUSE_SETTINGS:
            return FRONTEND_SPRITE_BACK6;
        default:
            return FRONTEND_SPRITE_BACK8;
        }
    };

    const auto GetBackgroundRect = [=]() -> CRect {
        switch (m_nBackgroundSprite) {
        case SCREEN_LOAD_FIRST_SAVE:
        case SCREEN_SAVE_DONE_1:
            return {
                SCREEN_STRETCH_FROM_RIGHT(256.0f),
                0.0f,
                SCREEN_WIDTH,
                StretchY(256.0f),
            };
        case SCREEN_DELETE_FINISHED:
            return {
                SCREEN_STRETCH_FROM_RIGHT(256.0f),
                0.0f,
                SCREEN_WIDTH,
                StretchY(256.0f),
            };
        case SCREEN_DELETE_SUCCESSFUL:
        case SCREEN_SAVE_WRITE_ASK:
            return {
                SCREEN_STRETCH_FROM_RIGHT(256.0f),
                0.0f,
                SCREEN_WIDTH,
                StretchY(256.0f),
            };
        case SCREEN_GAME_SAVE:
            return {
                SCREEN_WIDTH / 2.0f - StretchX(128.0f),
                0.0f,
                StretchX(128.0f) + SCREEN_WIDTH / 2.0f,
                StretchY(256.0f),
            };
        case SCREEN_SAVE_DONE_2:
            return {
                SCREEN_STRETCH_FROM_RIGHT(300.0f),
                0.0f,
                SCREEN_WIDTH,
                StretchY(200.0f),
            };
        default:
            assert(true); // Bad R*
            return {};    // suppress warn
        }
    };

    m_nBackgroundSprite = GetSpriteId();
    auto backgroundRect = GetBackgroundRect();

    // 0x57B7E1
    CRect rect(-5.0f, -5.0f, SCREEN_WIDTH + 5.0f, SCREEN_HEIGHT + 5.0f);
    CSprite2d::DrawRect(rect, {0, 0, 0, 255});

    if (m_nBackgroundSprite) {
        m_aFrontEndSprites[m_nBackgroundSprite].Draw(backgroundRect, { 255, 255, 255, 255 });
    }

    // 0x57BA02
    if (m_nCurrentScreen == SCREEN_MAP) {
        auto origin = m_vMapOrigin;
        auto zoom   = m_fMapZoom;

        PrintMap();

        m_fMapZoom   = zoom;
        m_vMapOrigin = origin;
    }

#ifdef USE_BUILD_INFORMATION
    DrawBuildInfo();
#endif

    // 0x57BA42
    if (m_nCurrentScreen == SCREEN_CONTROLS_DEFINITION) {
        DrawControllerSetupScreen();
    } else if (m_nCurrentScreen == SCREEN_EMPTY) {
        DrawQuitGameScreen();
    } else {
        DrawStandardMenus(1);
    }

    auto pad = CPad::GetPad(m_nPlayerNumber);
    // 0x57BA6B
    if (m_nControllerError) {
        if (!field_1B4C_b1) {
            field_1B4C_b1 = true;
            field_1B48 = CTimer::GetTimeInMSPauseMode();
        }

        if (field_1B44) {
            field_1B48 = CTimer::GetTimeInMSPauseMode();
            field_1B44 = false;
        }

        switch (m_nControllerError) {
        case 1:
            // Error! Changing controls on the 'In Vehicle' screen has caused one or more control actions to be unbound on the 'On Foot' screen.
            // Please check all control actions are set~n~Press ESC to continue
            MessageScreen("FEC_ER3", false, false);
            break;
        case 2:
            // Error! Changing controls on the 'On Foot' screen has caused one or more control actions to be unbound on the 'In Vehicle' screen.
            // Please check all control actions are set~n~Press ESC to continue
            MessageScreen("FEC_ER2", false, false);
            break;
        default:
            // Error! One or more control actions are not bound to a key or button. Please check all control actions are set~n~Press ESC to continue
            MessageScreen("FEC_ERI", false, false);
            break;
        }
        CFont::RenderFontBuffer();
        auto time = CTimer::GetTimeInMSPauseMode() - field_1B48;
        if (time > 7000 || (pad->IsEscJustPressed() && time > 1000)) {
            m_nControllerError = 0;
            field_1B44 = true;
        }
    }

    // 0x57BC19
    if (m_bScanningUserTracks) {
        static uint32& s_ProgressPosition    = *(uint32*)0x8CE000;  // 140
        static uint32& s_ProgressDirection   = *(uint32*)0x8CDFFC; // -1
        static bool&   s_bUpdateScanningTime = *(bool*)0x8CDFFA;

        if (!bScanningUserTracks) {
            bScanningUserTracks = true;
            m_nUserTrackScanningTimeMs = CTimer::GetTimeInMSPauseMode();
        }

        // SCANNING USER TRACKS - PLEASE WAIT
        //        Press ESC to cancel
        MessageScreen("FEA_SMP", false, false);

        // calculate progress position
        if ((CTimer::m_FrameCounter & 4) != 0) {
            s_ProgressPosition -= s_ProgressDirection;
            if (s_ProgressPosition < DEFAULT_SCREEN_WIDTH / 2 + 50) {
                if (s_ProgressPosition <= DEFAULT_SCREEN_WIDTH / 2 - 50)
                    s_ProgressDirection = -1;
            } else {
                s_ProgressDirection = 1;
            }
        }

        CSprite2d::DrawRect(
            CRect(
                StretchX(float(DEFAULT_SCREEN_WIDTH / 2 - 50)),
                StretchY(245.0f),
                StretchX(float(DEFAULT_SCREEN_WIDTH / 2 + 50 + 5)),
                StretchY(250.0f)
            ),
            { 50, 50, 50, 255 }
        );

        CSprite2d::DrawRect(
            CRect(
                StretchX(float(s_ProgressPosition)),
                StretchY(245.0f),
                StretchX(float(s_ProgressPosition + 5)),
                StretchY(250.0f)
            ),
            { 225, 225, 225, 255 }
        );

        CFont::DrawFonts();
        ResetHelperText();

        if (s_bUpdateScanningTime) {
            m_nUserTrackScanningTimeMs = CTimer::GetTimeInMSPauseMode();
            s_bUpdateScanningTime = false;
        }

        if (AEUserRadioTrackManager.ScanUserTracks() || pad->IsEscJustPressed()) {
            if (CTimer::GetTimeInMSPauseMode() - m_nUserTrackScanningTimeMs > 3000 || pad->IsEscJustPressed()) {
                auto helperText = HELPER_NONE;
                switch (AEUserRadioTrackManager.m_nUserTracksScanState) {
                case USER_TRACK_SCAN_COMPLETE:
                    AEUserRadioTrackManager.m_nUserTracksScanState = USER_TRACK_SCAN_OFF;
                    if (!pad->IsEscJustPressed()) {
                        AEUserRadioTrackManager.Initialise();
                        helperText = FEA_SCS;
                    }
                    break;
                case USER_TRACK_SCAN_ERROR:
                    AEUserRadioTrackManager.m_nUserTracksScanState = USER_TRACK_SCAN_OFF;
                    if (!pad->IsEscJustPressed()) {
                        helperText = FEA_SCF;
                    }
                    break;
                }
                if (pad->IsEscJustPressed()) {
                    helperText = FEA_SCF;
                }

                if (helperText) {
                    m_bScanningUserTracks = false;
                    s_bUpdateScanningTime = true;
                    SetHelperText(helperText);
                }
                DisplayHelperText(nullptr);
                return;
            }
        }
    } else if (m_DisplayTheMouse) { // 0x57BF62
        CFont::RenderFontBuffer();

        auto x = float(m_nMousePosX);
        auto y = float(m_nMousePosY);

        const auto DrawCursor = [=](auto spriteId) {
            CRect rect;

            rect.left   = x + StretchX(6.0f);
            rect.bottom    = y + StretchY(3.0f);
            rect.right  = x + SCREEN_STRETCH_X(24.0f);
            rect.top = y + SCREEN_SCALE_Y(21.0f);
            m_aFrontEndSprites[spriteId].Draw(rect, { 100, 100, 100, 50 }); // shadow

            rect.left   = x;
            rect.bottom    = y;
            rect.right  = x + SCREEN_STRETCH_X(18.0f);
            rect.top = y + SCREEN_SCALE_Y(18.0f);
            m_aFrontEndSprites[spriteId].Draw(rect, { 255, 255, 255, 255 });
        };

        CRect mapRect(StretchX(60.0f), StretchY(60.0f), SCREEN_STRETCH_FROM_RIGHT(60.0f), SCREEN_STRETCH_FROM_BOTTOM(60.0f));

        if (m_nCurrentScreen == SCREEN_MAP && CPad::NewMouseControllerState .isMouseLeftButtonPressed && mapRect.IsPointInside(CVector2D(x, y))) {
            DrawCursor(FRONTEND_SPRITE_CROSS_HAIR);
        } else {
            DrawCursor(FRONTEND_SPRITE_MOUSE);
        }
    }
}

// 0x5794A0
void CMenuManager::DrawStandardMenus(bool drawTitle) {
    constexpr uint16 MENU_DEFAULT_CONTENT_X = 320;
    constexpr uint16 MENU_DEFAULT_CONTENT_Y = 130;

    const GxtChar *pTextToShow_RightColumn; // text for right column
    GxtChar *pTextToShow = nullptr;         // text to display

    float buttonTextPosY = 0.0;
    bool shouldDrawStandardItems = true;
    CFont::SetBackground(0, 0);
    CFont::SetProportional(1);
    CFont::SetOrientation(eFontAlignment::ALIGN_LEFT);
    CFont::SetWrapx(SCREEN_STRETCH_FROM_RIGHT(10.0f));
    CFont::SetRightJustifyWrap(SCREEN_SCALE_X(10.0));
    CFont::SetCentreSize(SCREEN_WIDTH);

    if (m_nCurrentScreen == eMenuScreen::SCREEN_STATS) {
        PrintStats();
    }
    if (m_nCurrentScreen == eMenuScreen::SCREEN_BRIEF) {
        PrintBriefs();
    }

    if (m_nCurrentScreen == eMenuScreen::SCREEN_AUDIO_SETTINGS && drawTitle) {
        PrintRadioStationList();
    }

    if (drawTitle && aScreens[m_nCurrentScreen].m_szTitleName[0]) {
        if (m_nCurrentScreen != eMenuScreen::SCREEN_MAP || !m_bMapLoaded) {
            CFont::SetOrientation(eFontAlignment::ALIGN_LEFT);
            CFont::SetFontStyle(eFontStyle::FONT_GOTHIC);
            CFont::SetScale(StretchX(1.3f), StretchY(2.1f));
            CFont::SetEdge(1);
            CFont::SetColor(HudColour.GetRGB(HUD_COLOUR_LIGHT_BLUE));
            CFont::SetDropColor(HudColour.GetRGB(HUD_COLOUR_BLACK));
            CFont::PrintString(StretchX(40.0f), StretchY(28.0f), TheText.Get(aScreens[m_nCurrentScreen].m_szTitleName));
        }
    }

    if (aScreens[m_nCurrentScreen].m_aItems[0].m_nActionType == eMenuAction::MENU_ACTION_TEXT) {
        CFont::SetWrapx(SCREEN_STRETCH_FROM_RIGHT(40.0f));
        CFont::SetFontStyle(FONT_SUBTITLES);
        CFont::SetScale(StretchX(0.5f), StretchY(1.2f));
        CFont::SetOrientation(eFontAlignment::ALIGN_LEFT);
        CFont::SetEdge(2);
        CFont::SetDropColor(CRGBA(0, 0, 0, 0xFFu));
        CFont::SetColor(CRGBA(0x4Au, 0x5Au, 0x6Bu, 0xFFu));

        const GxtChar *textOne;

        switch (m_nCurrentScreen) {
        case eMenuScreen::SCREEN_NEW_GAME_ASK:
            textOne = TheText.Get((!m_bMainMenuSwitch) ? aScreens[m_nCurrentScreen].m_aItems[0].m_szName : "FESZ_QQ");
            break;
        case eMenuScreen::SCREEN_LOAD_GAME_ASK:
            textOne = TheText.Get((!m_bMainMenuSwitch) ? aScreens[m_nCurrentScreen].m_aItems[0].m_szName : "FES_LCG");
            break;
        case eMenuScreen::SCREEN_SAVE_WRITE_ASK: {
            eSlotState slotToCheck = GetSavedGameState(m_SelectedSlot);
            textOne = TheText.Get((eSlotState::SLOT_FILLED != slotToCheck) ? (slotToCheck != eSlotState::SLOT_CORRUPTED ? aScreens[m_nCurrentScreen].m_aItems[0].m_szName : "FESZ_QC") : "FESZ_QO");
            break;
        }
        case eMenuScreen::SCREEN_QUIT_GAME_ASK:
            textOne = TheText.Get((!m_bMainMenuSwitch) ? aScreens[m_nCurrentScreen].m_aItems[0].m_szName : "FEQ_SRW");
            break;
        default:
            textOne = TheText.Get(aScreens[m_nCurrentScreen].m_aItems[0].m_szName);
            break;
        }

        CFont::PrintString(StretchX(60.0f), StretchY(97.0f), textOne);
        CFont::SetWrapx(SCREEN_STRETCH_FROM_RIGHT(10.0f));
        CFont::SetRightJustifyWrap(SCREEN_SCALE_X(10.0f));
    }

    if (m_nCurrentScreen == eMenuScreen::SCREEN_CONTROLS_DEFINITION) {
        if (m_EditingControlOptions) {
            shouldDrawStandardItems = false;
        }
        DrawControllerScreenExtraText(-8);
    }

    const bool weHaveLabel = aScreens[m_nCurrentScreen].m_aItems[0].m_nActionType == eMenuAction::MENU_ACTION_TEXT;

    for (int i = 0; i < std::size(aScreens[m_nCurrentScreen].m_aItems); i++) {
        auto itemType = aScreens[m_nCurrentScreen].m_aItems[i].m_nType;
        pTextToShow_RightColumn = 0;
        int MENU_DEFAULT_LINE_HEIGHT = (itemType == eMenuEntryType::TI_MPACK) ? 20 : 30;
        CFont::SetFontStyle(FONT_MENU);
        if (itemType < eMenuEntryType::TI_SLOT1 || itemType > eMenuEntryType::TI_SLOT8) {
            CFont::SetScale(StretchX(0.7f), StretchY(1.0f));
            CFont::SetEdge(2);
        } else {
            CFont::SetEdge(1);
            CFont::SetScale(StretchX(0.42f), StretchY(0.95f));
        }
        CFont::SetDropColor(HudColour.GetRGB(HUD_COLOUR_BLACK));
        CFont::SetColor((i == m_nCurrentScreenItem && m_bMapLoaded) ? CRGBA(172, 203, 241, 255) : CRGBA(74, 90, 107, 255));

        switch (aScreens[m_nCurrentScreen].m_aItems[i].m_nAlign) {
        case eMenuAlign::MENU_ALIGN_LEFT:
            CFont::SetOrientation(eFontAlignment::ALIGN_LEFT);
            break;
        case eMenuAlign::MENU_ALIGN_RIGHT:
            CFont::SetOrientation(eFontAlignment::ALIGN_RIGHT);
            break;
        default:
            CFont::SetOrientation(eFontAlignment::ALIGN_CENTER);
            break;
        }

        if (!aScreens[m_nCurrentScreen].m_aItems[i].m_X && !aScreens[m_nCurrentScreen].m_aItems[i].m_Y) {
            if (i == 0 || (i == 1 && weHaveLabel)) {
                aScreens[m_nCurrentScreen].m_aItems[i].m_X = MENU_DEFAULT_CONTENT_X;
                aScreens[m_nCurrentScreen].m_aItems[i].m_Y = MENU_DEFAULT_CONTENT_Y;
            } else {
                aScreens[m_nCurrentScreen].m_aItems[i].m_X = aScreens[m_nCurrentScreen].m_aItems[i - 1].m_X;
                aScreens[m_nCurrentScreen].m_aItems[i].m_Y = aScreens[m_nCurrentScreen].m_aItems[i - 1].m_Y + MENU_DEFAULT_LINE_HEIGHT;
            }
        }

        if (aScreens[m_nCurrentScreen].m_aItems[i].m_nActionType == eMenuAction::MENU_ACTION_TEXT || std::string(aScreens[m_nCurrentScreen].m_aItems[i].m_szName).empty()) {
            continue;
        }

        itemType = aScreens[m_nCurrentScreen].m_aItems[i].m_nType;
        const bool isSlot = IsSaveSlot(itemType);

        float xOffset = 0;

        switch (itemType) {
        case eMenuEntryType::TI_MPACK: {
            std::array<MPack, MPACK_COUNT> missionPacksArray = std::to_array(m_MissionPacks);
            // Check if the index is within bounds
            if (i - 2 < MPACK_COUNT && i - 2 >= 0) {
                const auto& MPacks = missionPacksArray[i - 2];
                if (std::string(MPacks.m_Name).empty()) {
                    aScreens[m_nCurrentScreen].m_aItems[i].m_nActionType = eMenuAction::MENU_ACTION_SKIP;
                    pTextToShow = nullptr;
                    aScreens[m_nCurrentScreen].m_aItems[i].m_Y = aScreens[m_nCurrentScreen].m_aItems[i - 1].m_Y;
                } else {
                    AsciiToGxtChar(MPacks.m_Name, (GxtChar*)gString);
                    pTextToShow = (GxtChar*)gString;
                    aScreens[m_nCurrentScreen].m_aItems[i].m_nActionType = eMenuAction::MENU_ACTION_MPACK;
                }
            } else {
                NOTSA_UNREACHABLE("Index out of bounds for missionPacksArray");
            }
            break;
        }
        case eMenuEntryType::TI_MOUSEJOYPAD:
            switch (m_ControlMethod) {
            case eController::JOYPAD:          pTextToShow = (GxtChar*)TheText.Get("FEJ_TIT"); break; // Joypad Settings
            case eController::MOUSE_PLUS_KEYS: pTextToShow = (GxtChar*)TheText.Get("FEC_MOU"); break; // Mouse Settings
            default:                           NOTSA_UNREACHABLE();
            }
            break;
        default: {
            if (isSlot) {
                aScreens[m_nCurrentScreen].m_aItems[i].m_X = 80;
                CFont::SetOrientation(eFontAlignment::ALIGN_LEFT);

                switch (GetSavedGameState(i - 1)) {
                case eSlotState::SLOT_FILLED: { // Valid save
                    /*
                    GxtCharStrcpy(gGxtString, GetNameOfSavedGame(i - 1));
                    if (GxtCharStrlen(gGxtString) >= 254) {
                        gGxtString[254] = '\0';
                        AsciiToGxtChar("...", gGxtString2);
                        GxtCharStrcat(gGxtString, gGxtString2);
                    }
                    */

                    // NOTSA: Right way for modern GxtChar
                    auto stringSavegame = std::string(GxtCharToUTF8(GetNameOfSavedGame(i - 1)));
                    if (stringSavegame.length() >= 32) {
                        stringSavegame = std::format("{}...", stringSavegame.substr(0, 32 - 4));
                    }

                    AsciiToGxtChar(stringSavegame.c_str(), gGxtString);
                    pTextToShow = gGxtString;
                    pTextToShow_RightColumn = GetSavedGameDateAndTime(i - 1);
                    break;
                }
                case eSlotState::SLOT_CORRUPTED: { // Corrupted save
                    pTextToShow = (GxtChar*)TheText.Get("FESZ_CS");
                    if (!pTextToShow) {
                        CFont::SetOrientation(eFontAlignment::ALIGN_CENTER);
                        aScreens[m_nCurrentScreen].m_aItems[i].m_X = MENU_DEFAULT_CONTENT_X;
                        pTextToShow                                = (GxtChar*)TheText.Get(std::format("FEM_SL{}", i).c_str());
                        xOffset                                    = StretchX(40.0);
                    }
                    break;
                }
                default: { // Empty slot
                    AsciiToGxtChar(std::format("FEM_SL{}", i).c_str(), gGxtString);
                    CFont::SetOrientation(eFontAlignment::ALIGN_CENTER);
                    aScreens[m_nCurrentScreen].m_aItems[i].m_X = MENU_DEFAULT_CONTENT_X;
                    pTextToShow                                = (GxtChar*)TheText.Get((const char*)gGxtString);
                    xOffset                                    = StretchX(40.0);
                    break;
                }
                }
                break;
            }
            pTextToShow = (GxtChar*)TheText.Get(aScreens[m_nCurrentScreen].m_aItems[i].m_szName);
            break;
        }
        }

        switch (aScreens[m_nCurrentScreen].m_aItems[i].m_nActionType) {
        case eMenuAction::MENU_ACTION_RADIO_STATION:
            pTextToShow_RightColumn = AudioEngine.GetRadioStationName(m_nRadioStation);
            break;
        case eMenuAction::MENU_ACTION_FRAME_LIMITER:
            pTextToShow_RightColumn = TheText.Get(m_bPrefsFrameLimiter ? "FEM_ON" : "FEM_OFF");
            break;
        case eMenuAction::MENU_ACTION_SUBTITLES:
            pTextToShow_RightColumn = TheText.Get(m_bShowSubtitles ? "FEM_ON" : "FEM_OFF");
            break;
        case eMenuAction::MENU_ACTION_WIDESCREEN:
            pTextToShow_RightColumn = TheText.Get(m_bWidescreenOn ? "FEM_ON" : "FEM_OFF");
            break;
        case eMenuAction::MENU_ACTION_RADIO_EQ:
            pTextToShow_RightColumn = TheText.Get(m_bRadioEq ? "FEM_ON" : "FEM_OFF");
            break;
        case eMenuAction::MENU_ACTION_RADIO_RETUNE:
            pTextToShow_RightColumn = TheText.Get(m_bRadioAutoSelect ? "FEM_ON" : "FEM_OFF");
            break;
        case eMenuAction::MENU_ACTION_SHOW_LEGEND:
            pTextToShow_RightColumn = TheText.Get(m_bMapLegend ? "FEM_ON" : "FEM_OFF");
            break;
        case eMenuAction::MENU_ACTION_HUD_MODE:
            pTextToShow_RightColumn = TheText.Get(m_bHudOn ? "FEM_ON" : "FEM_OFF");
            break;
        case eMenuAction::MENU_ACTION_CONTROLS_JOY_INVERT_X:
            pTextToShow_RightColumn = TheText.Get(m_bInvertPadX1 ? "FEM_ON" : "FEM_OFF");
            break;
        case eMenuAction::MENU_ACTION_CONTROLS_JOY_INVERT_Y:
            pTextToShow_RightColumn = TheText.Get(m_bInvertPadY1 ? "FEM_ON" : "FEM_OFF");
            break;
        case eMenuAction::MENU_ACTION_CONTROLS_JOY_INVERT_X2:
            pTextToShow_RightColumn = TheText.Get(m_bInvertPadX2 ? "FEM_ON" : "FEM_OFF");
            break;
        case eMenuAction::MENU_ACTION_CONTROLS_JOY_INVERT_Y2:
            pTextToShow_RightColumn = TheText.Get(m_bInvertPadY2 ? "FEM_ON" : "FEM_OFF");
            break;
        case eMenuAction::MENU_ACTION_CONTROLS_JOY_SWAP_AXIS1:
            pTextToShow_RightColumn = TheText.Get(m_bSwapPadAxis1 ? "FEM_ON" : "FEM_OFF");
            break;
        case eMenuAction::MENU_ACTION_CONTROLS_JOY_SWAP_AXIS2:
            pTextToShow_RightColumn = TheText.Get(m_bSwapPadAxis2 ? "FEM_ON" : "FEM_OFF");
            break;
        case eMenuAction::MENU_ACTION_USER_TRACKS_AUTO_SCAN:
            pTextToShow_RightColumn = TheText.Get(m_bTracksAutoScan ? "FEM_ON" : "FEM_OFF");
            break;
        case eMenuAction::MENU_ACTION_STORE_PHOTOS:
            pTextToShow_RightColumn = TheText.Get(m_bSavePhotos ? "FEM_ON" : "FEM_OFF");
            break;
        case eMenuAction::MENU_ACTION_RADAR_MODE:
            pTextToShow_RightColumn = TheText.Get(m_nRadarMode == 0 ? "FED_RDM" : (m_nRadarMode == 1 ? "FED_RDB" : "FEM_OFF"));
            break;
        case eMenuAction::MENU_ACTION_LANGUAGE:
            switch (m_nPrefsLanguage) {
            case eLanguage::AMERICAN:
                pTextToShow_RightColumn = TheText.Get("FEL_ENG");
                break;
            case eLanguage::FRENCH:
                pTextToShow_RightColumn = TheText.Get("FEL_FRE");
                break;
            case eLanguage::GERMAN:
                pTextToShow_RightColumn = TheText.Get("FEL_GER");
                break;
            case eLanguage::ITALIAN:
                pTextToShow_RightColumn = TheText.Get("FEL_ITA");
                break;
            case eLanguage::SPANISH:
                pTextToShow_RightColumn = TheText.Get("FEL_SPA");
                break;
            default:
                break;
            }
            break;
        case eMenuAction::MENU_ACTION_FX_QUALITY:
            switch (g_fx.GetFxQuality()) {
            case FxQuality_e::FX_QUALITY_LOW:
                pTextToShow_RightColumn = TheText.Get("FED_FXL");
                break;

            case FxQuality_e::FX_QUALITY_MEDIUM:
                pTextToShow_RightColumn = TheText.Get("FED_FXM");
                break;

            case FxQuality_e::FX_QUALITY_HIGH:
                pTextToShow_RightColumn = TheText.Get("FED_FXH");
                break;

            case FxQuality_e::FX_QUALITY_VERY_HIGH:
                pTextToShow_RightColumn = TheText.Get("FED_FXV");
                break;

            default:
                break;
            }
            break;
        case eMenuAction::MENU_ACTION_MIP_MAPPING:
            pTextToShow_RightColumn = TheText.Get(m_bPrefsMipMapping ? "FEM_ON" : "FEM_OFF");
            if (!m_bMainMenuSwitch) { CFont::SetColor(CRGBA(14, 30, 47, 255)); }
            break;
        case eMenuAction::MENU_ACTION_ANTIALIASING: {
            if (m_nDisplayAntialiasing <= 1) {
                pTextToShow_RightColumn = TheText.Get("FEM_OFF");
            } else {
                GxtChar tmpBuffer[64];
                AsciiToGxtChar(std::format("{}", m_nDisplayAntialiasing - 1).c_str(), tmpBuffer);
                pTextToShow_RightColumn = tmpBuffer;
            }
            break;
        }
        case eMenuAction::MENU_ACTION_CONTROLS_MOUSE_INVERT_Y:
            if (notsa::IsFixBugs()) {
                pTextToShow_RightColumn = TheText.Get(bInvertMouseY ? "FEM_ON" : "FEM_OFF");
            } else {
                pTextToShow_RightColumn = TheText.Get(bInvertMouseY ? "FEM_OFF" : "FEM_ON");
            }
            break;
        case eMenuAction::MENU_ACTION_RESOLUTION: {
            GxtChar tmpBuffer[1'024];
            auto Videomodes = GetVideoModeList();
            AsciiToGxtChar(std::format("{}", Videomodes[m_nDisplayVideoMode]).c_str(), tmpBuffer);
            pTextToShow_RightColumn = tmpBuffer;
            break;
        }
        case eMenuAction::MENU_ACTION_CONTROL_TYPE:
            switch (m_ControlMethod) {
            case eController::JOYPAD:          pTextToShow_RightColumn = TheText.Get("FET_CCN"); break; // Joypad
            case eController::MOUSE_PLUS_KEYS: pTextToShow_RightColumn = TheText.Get("FET_SCN"); break; // Mouse + Keys
            default:                           NOTSA_UNREACHABLE();
            }
            break;
        case eMenuAction::MENU_ACTION_MOUSE_STEERING:
            pTextToShow_RightColumn = TheText.Get((CVehicle::m_bEnableMouseSteering) ? "FEM_ON" : "FEM_OFF");
            if (m_ControlMethod == eController::JOYPAD) {
                CFont::SetColor(CRGBA(14, 30, 47, 255));
            }
            break;
        case eMenuAction::MENU_ACTION_MOUSE_FLY:
            pTextToShow_RightColumn = TheText.Get((CVehicle::m_bEnableMouseFlying) ? "FEM_ON" : "FEM_OFF");
            if (m_ControlMethod == eController::JOYPAD) {
                CFont::SetColor(CRGBA(14, 30, 47, 255));
            }
            break;
        case eMenuAction::MENU_ACTION_USER_TRACKS_PLAY_MODE:
            pTextToShow_RightColumn = TheText.Get(m_nRadioMode == 0 ? "FEA_PR1" : (m_nRadioMode == 1 ? "FEA_PR2" : "FEA_PR3"));
            break;
        default:
            break;
        }
        
        const auto scaledPosX = StretchX(aScreens[m_nCurrentScreen].m_aItems[i].m_X);
        const auto scaledPosY = StretchY(aScreens[m_nCurrentScreen].m_aItems[i].m_Y);

        if (pTextToShow) {
            if ((isSlot && GetSavedGameState(itemType - 1) != eSlotState::SLOT_FILLED) || !isSlot) {
                CFont::PrintString(scaledPosX, scaledPosY, pTextToShow);
            // v1.01 +
            } else if (isSlot && GetSavedGameState(itemType - 1) == eSlotState::SLOT_FILLED) {
                CFont::PrintString(StretchX(25.0f + aScreens[m_nCurrentScreen].m_aItems[i].m_X), scaledPosY, pTextToShow);
                AsciiToGxtChar(std::format("{}:", i).c_str(), gGxtString);
                CFont::PrintString(scaledPosX, scaledPosY, gGxtString);
            }
        }

        if (pTextToShow_RightColumn) {
            CFont::SetFontStyle(FONT_MENU);
            CFont::SetEdge(1);
            CFont::SetOrientation(eFontAlignment::ALIGN_RIGHT);
            if (!isSlot) {
                CFont::SetScale(StretchX((m_nCurrentScreen == SCREEN_AUDIO_SETTINGS && i == 5) ? 0.56f : 0.7f), StretchY(1.0f));
            } else {
                CFont::SetScale(StretchX(0.35f), StretchY(0.95f));
            }

            CFont::PrintString(SCREEN_STRETCH_FROM_RIGHT(40.0f), scaledPosY, pTextToShow_RightColumn);
        }

        // Check if text was properly initialized and we're on the current selection
        if ((pTextToShow && i == m_nCurrentScreenItem) && (m_nCurrentScreen != SCREEN_MAP && m_nCurrentScreen != SCREEN_BRIEF)) {
            // Calculate X position for highlighted item if not already done
            if (!xOffset) {
                const auto align = aScreens[m_nCurrentScreen].m_aItems[i].m_nAlign;
                xOffset = [&]() -> float {
                    switch (align) {
                    case eMenuAlign::MENU_ALIGN_LEFT:
                        return scaledPosX - StretchX(40.0f);
                    case eMenuAlign::MENU_ALIGN_RIGHT:
                        return StretchX(40.0f) + scaledPosX;
                    default:
                        return scaledPosX - StretchX(40.0f) - CFont::GetStringWidth(pTextToShow, 1, 0) * 0.5f;
                    }
                }();
            }

            // Draw highlight rectangle for selected item if map is loaded
            if (m_bMapLoaded) {
                if (m_nCurrentScreen != eMenuScreen::SCREEN_LOAD_FIRST_SAVE && m_nCurrentScreen != eMenuScreen::SCREEN_DELETE_FINISHED && m_nCurrentScreen != eMenuScreen::SCREEN_SAVE_DONE_1) {
                    CRect rect(xOffset, // left
                               scaledPosY - StretchX(5.0f), // top
                               xOffset + StretchX(32.0f),   // right
                               scaledPosY + StretchX(47.0f) // bottom
                    );
                    m_aFrontEndSprites[0].Draw(rect, CRGBA(255, 255, 255, 255));
                }
            }
        }

        // Check for video mode changes
        if (m_nDisplayVideoMode == m_nPrefsVideoMode && !strcmp(aScreens[m_nCurrentScreen].m_aItems[m_nCurrentScreenItem].m_szName, "FED_RES") && m_nHelperText == FET_APP) {
            ResetHelperText();
        }
        if (m_nDisplayAntialiasing == m_nPrefsAntialiasing && !strcmp(aScreens[m_nCurrentScreen].m_aItems[m_nCurrentScreenItem].m_szName, "FED_AAS") && m_nHelperText == FET_APP) {
            ResetHelperText();
        }
        if (m_nDisplayVideoMode != m_nPrefsVideoMode && !strcmp(aScreens[m_nCurrentScreen].m_aItems[m_nCurrentScreenItem].m_szName, "FED_RES")) {
            SetHelperText(eHelperText::FET_APP);
        }
        if (m_nDisplayAntialiasing != m_nPrefsAntialiasing && !strcmp(aScreens[m_nCurrentScreen].m_aItems[m_nCurrentScreenItem].m_szName, "FED_AAS")) {
            SetHelperText(eHelperText::FET_APP);
        }
        if (m_nDisplayAntialiasing != m_nPrefsAntialiasing) {
            if (strcmp(aScreens[m_nCurrentScreen].m_aItems[m_nCurrentScreenItem].m_szName, "FED_AAS")) {
                if (m_nCurrentScreen == eMenuScreen::SCREEN_DISPLAY_SETTINGS || m_nCurrentScreen == eMenuScreen::SCREEN_DISPLAY_ADVANCED) {
                    m_nDisplayAntialiasing = m_nPrefsAntialiasing;
                    SetHelperText(eHelperText::FET_RSO);
                }
            }
        }
        if (m_nDisplayVideoMode != m_nPrefsVideoMode) {
            if (strcmp(aScreens[m_nCurrentScreen].m_aItems[m_nCurrentScreenItem].m_szName, "FED_RES")) {
                if (m_nCurrentScreen == eMenuScreen::SCREEN_DISPLAY_SETTINGS || m_nCurrentScreen == eMenuScreen::SCREEN_DISPLAY_ADVANCED) {
                    m_nDisplayVideoMode = m_nPrefsVideoMode;
                    SetHelperText(eHelperText::FET_RSO);
                }
            }
        }

        // Handle sliders
        switch (aScreens[m_nCurrentScreen].m_aItems[i].m_nActionType) {
        case eMenuAction::MENU_ACTION_BRIGHTNESS: { // Brightness slider
            const auto sliderFieldPos = DisplaySlider(StretchX(500.0f), StretchY(125.0f), StretchY(4.0f), StretchY(20.0f), StretchX(100.0f), m_PrefsBrightness * 0.0026041667f, int32(StretchX(3.0f)));
            if (i == m_nCurrentScreenItem && shouldDrawStandardItems) {
                if (CheckHover(0, sliderFieldPos - StretchX(3.0f), StretchY(125.0f), StretchY(150.0f))) {
                    m_MouseInBounds = 7;
                } else if (CheckHover(StretchX(3.0f) + sliderFieldPos, StretchX(SCREEN_WIDTH), StretchY(125.0f), StretchY(150.0f))) {
                    m_MouseInBounds = m_nMousePosX < StretchX(500.0f) ? 16 : (m_nMousePosY < StretchY(125.0f) || m_nMousePosY > StretchY(150.0f)) ? 16 : 6;
                } else {
                    m_MouseInBounds = 16;
                }
            }
            break;
        }
        case eMenuAction::MENU_ACTION_RADIO_VOL: { // Radio volume slider
            const auto sliderFieldPos = DisplaySlider(StretchX(500.0f), StretchY(95.0f), StretchY(4.0f), StretchY(20.0f), StretchX(100.0f), m_nRadioVolume * 0.015625f, int32(StretchX(3.0f)));
            if (i == m_nCurrentScreenItem && shouldDrawStandardItems) {
                if (CheckHover(0, sliderFieldPos - StretchX(3.0f), StretchY(95.0f), StretchY(120.0f))) {
                    m_MouseInBounds = 11;
                } else if (CheckHover(StretchX(3.0f) + sliderFieldPos, StretchX(SCREEN_WIDTH), StretchY(95.0f), StretchY(120.0f))) {
                    m_MouseInBounds = StretchX(500.0f) <= m_nMousePosX && StretchY(95.0f) <= m_nMousePosY && StretchY(120.0f) >= m_nMousePosY ? 10 : 16;
                } else {
                    m_MouseInBounds = 16;
                }
            }
            break;
        }
        case eMenuAction::MENU_ACTION_SFX_VOL: { // SFX volume slider
            const auto sliderFieldPos = DisplaySlider(StretchX(500.0f), StretchY(125.0f), StretchY(4.0f), StretchY(20.0f), StretchX(100.0f), m_nSfxVolume * 0.015625f, int32(StretchX(3.0f)));
            if (i == m_nCurrentScreenItem && shouldDrawStandardItems) {
                if (CheckHover(0, sliderFieldPos - StretchX(3.0f), StretchY(125.0f), StretchY(150.0f))) {
                    m_MouseInBounds = 13;
                } else if (CheckHover(StretchX(3.0f) + sliderFieldPos, StretchX(SCREEN_WIDTH), StretchY(125.0f), StretchY(150.0f))) {
                    m_MouseInBounds = m_nMousePosX < StretchX(500.0f) ? 16 : (m_nMousePosY < StretchY(125.0f) || m_nMousePosY > StretchY(150.0f)) ? 16 : 12;
                } else {
                    m_MouseInBounds = 16;
                }
            }
            break;
        }
        case eMenuAction::MENU_ACTION_DRAW_DIST: { // Draw distance slider
            const auto sliderFieldPos = DisplaySlider(StretchX(500.0f), StretchY(125.0f), StretchY(4.0f), StretchY(20.0f), StretchX(100.0f), (m_fDrawDistance - 0.92500001f) * 1.1428572f, int32(StretchX(3.0f)));
            if (i == m_nCurrentScreenItem && shouldDrawStandardItems) {
                if (CheckHover(0, sliderFieldPos - StretchX(3.0f), StretchY(125.0f), StretchY(150.0f))) {
                    m_MouseInBounds = 9;
                } else if (CheckHover(StretchX(3.0f) + sliderFieldPos, StretchX(SCREEN_WIDTH), StretchY(125.0f), StretchY(150.0f))) {
                    m_MouseInBounds = m_nMousePosX < StretchX(500.0f) ? 16 : (m_nMousePosY < StretchY(125.0f) || m_nMousePosY > StretchY(150.0f)) ? 16 : 8;
                } else {
                    m_MouseInBounds = 16;
                }
            }
            break;
        }
        case eMenuAction::MENU_ACTION_MOUSE_SENS: { // Mouse sensitivity slider
            const auto sliderFieldPos = DisplaySlider(StretchX(500.0f), StretchY(125.0f), StretchY(4.0f), StretchY(20.0f), StretchX(100.0f), CCamera::m_fMouseAccelHorzntl / 0.005f, int32(StretchX(3.0f)));
            if (i == m_nCurrentScreenItem && shouldDrawStandardItems) {
                if (CheckHover(0, sliderFieldPos - StretchX(3.0f), StretchY(125.0f), StretchY(150.0f))) {
                    m_MouseInBounds = 15;
                } else if (CheckHover(StretchX(3.0f) + sliderFieldPos, StretchX(SCREEN_WIDTH), StretchY(125.0f), StretchY(150.0f))) {
                    m_MouseInBounds = m_nMousePosX < StretchX(500.0f) ? 16 : 14;
                } else {
                    m_MouseInBounds = 16;
                }
            }
            break;
        }
        default:
            break;
        }

        if (pTextToShow && pTextToShow != nullptr) {
            buttonTextPosY = (29 * CFont::GetNumberLines(60.0, buttonTextPosY, (const GxtChar *)pTextToShow)) + buttonTextPosY;
        }
        if (aScreens[m_nCurrentScreen].m_aItems[i].m_nActionType == 32) {
            buttonTextPosY = buttonTextPosY + 70.0f;
        }
    }
    switch (m_nCurrentScreen) {
    case eMenuScreen::SCREEN_STATS:
    case eMenuScreen::SCREEN_AUDIO_SETTINGS:
    case eMenuScreen::SCREEN_DISPLAY_SETTINGS:
    case eMenuScreen::SCREEN_USER_TRACKS_OPTIONS:
    case eMenuScreen::SCREEN_DISPLAY_ADVANCED:
    case eMenuScreen::SCREEN_CONTROLLER_SETUP:
    case eMenuScreen::SCREEN_MOUSE_SETTINGS:
    case eMenuScreen::SCREEN_JOYPAD_SETTINGS:
        DisplayHelperText(0);
        break;
    default:
        return;
    }
}

// 0x573EE0
void CMenuManager::DrawWindow(const CRect& coords, const char* key, uint8 color, CRGBA backColor, bool unused, bool background) {
    if (background) {
        CSprite2d::DrawRect(coords, backColor);
    }

    if (key && *key) {
        CFont::SetWrapx(coords.right);
        CFont::SetColor(CRGBA(225 - color, 225 - color, 225 - color, 255));
        CFont::SetDropColor(CRGBA(0, 0, 0, 255));
        CFont::SetEdge(2);
        CFont::SetOrientation(eFontAlignment::ALIGN_LEFT);
        CFont::SetFontStyle(FONT_GOTHIC);
        CFont::SetScale(SCREEN_SCALE_X(1.0f), SCREEN_SCALE_Y(1.4f));

        float x = coords.left + SCREEN_SCALE_X(10.0f);
        float y = std::min(coords.bottom, coords.top) - SCREEN_SCALE_Y(16);
        CFont::PrintString(x, y, TheText.Get(const_cast<char*>(key)));
    }
}

// 0x578F50, untested
void CMenuManager::DrawWindowedText(float x, float y, float wrap, const char* title, const char* message, eFontAlignment alignment) {
    // return plugin::CallMethod<0x578F50, CMenuManager*, float, float, float, Const char*, Const char*, eFontAlignment>(this, x, y, a4, str, message, alignment);

    CFont::SetWrapx(x + wrap - StretchX(10.0f));
    CFont::SetRightJustifyWrap(StretchX(10.0f) + wrap);
    CFont::SetCentreSize(wrap - 2.0f * StretchX(10.0f));
    CFont::SetFontStyle(FONT_SUBTITLES);
    CFont::SetOrientation(alignment);
    CFont::SetScale(StretchX(0.7f), StretchY(1.0f));

    CRect rt;
    CFont::GetTextRect(&rt, x, y, TheText.Get(message));
    rt.left -= 4.0f;
    rt.bottom  += StretchY(22.0f);
    CSprite2d::DrawRect(rt, {0, 0, 0, 255});
    CFont::SetColor({225, 225, 225, 255});
    CFont::SetDropColor({0, 0, 0, 255});
    CFont::SetEdge(2);
    CFont::SetOrientation(eFontAlignment::ALIGN_LEFT);
    CFont::SetFontStyle(FONT_GOTHIC);
    CFont::SetScaleForCurrentLanguage(StretchX(1.1f), StretchY(1.4f));
    CFont::SetWrapx(rt.right);

    if (title && *title) {
        CFont::PrintString(rt.left + StretchX(20.0f), rt.top - StretchY(16.0f), TheText.Get(title));
    }

    if (message && *message) {
        CFont::SetWrapx(x + wrap - StretchX(10.0f));
        CFont::SetRightJustifyWrap(StretchX(10.0f) + wrap);
        CFont::SetCentreSize(wrap - 2.0f * StretchX(10.0f));
        CFont::SetFontStyle(FONT_SUBTITLES);
        CFont::SetOrientation(alignment);
        CFont::SetScale(StretchX(0.7f), StretchY(1.0f));

        CFont::SetDropShadowPosition(2);
        CFont::SetDropColor({ 0, 0, 0, 255 });
        CFont::PrintString(x, y + StretchY(15.0f), TheText.Get(message));
    }
}

// 0x57D860
void CMenuManager::DrawQuitGameScreen() {
    m_DisplayTheMouse = false;
    CRect coords(0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT);
    CSprite2d::DrawRect(coords, { 0, 0, 0, 255 });
    SaveSettings();
    RsEventHandler(rsQUITAPP, nullptr);
}

// 0x57D8D0
void CMenuManager::DrawControllerScreenExtraText(int32 startingYPos) {
    const auto maxActions      = GetMaxAction();
    const auto verticalSpacing = GetVerticalSpacing();
    if (maxActions > 0) {
        for (auto action = 0u; action < maxActions; action++) {
            float posX = StretchX(240.0f);
            float posY = StretchY(float(startingYPos));
            
            for (const auto& order : CONTROLLER_ORDERS_SET) {
                const auto buttonText = ControlsManager.GetControllerSettingText((eControllerAction)action, order);
                if (buttonText) {
                    CFont::PrintString(posX, posY, buttonText);
                    posX += StretchX(75.0f);
                }
            }
            
            if (action == m_ListSelection) {
                if (m_EditingControlOptions) {
                    if (CTimer::m_snTimeInMillisecondsPauseMode - FrontEndMenuManager.LastFlash > 150) {
                        FrontEndMenuManager.ColourSwitch ^= true;
                        FrontEndMenuManager.LastFlash = CTimer::m_snTimeInMillisecondsPauseMode;
                    }
                    
                    if (FrontEndMenuManager.ColourSwitch) {
                        CFont::SetColor({0, 0, 0, 255});
                        CFont::PrintString(posX, posY, TheText.Get("FEC_QUE")); // ???
                        CFont::SetColor({74, 90, 107, 255});
                    }
                }
            }

            // 0x57DA95
            if (m_MenuIsAbleToQuit) {
                const auto comboText = ControlsManager.GetButtonComboText((eControllerAction)m_ListSelection);
                if (comboText) {
                    CFont::SetColor({200, 50, 50, 255});
                    CFont::PrintString(posX, StretchY(posY + 10.f), comboText);
                }
            }

            startingYPos += verticalSpacing;
        }
    }
}

// 0x57E6E0
void CMenuManager::DrawControllerBound(uint16 verticalOffset, bool isOppositeScreen) {
    const auto verticalSpacing = GetVerticalSpacing();
    const auto maxActions      = GetMaxAction();

    using ControlActionMapping = std::pair<eControllerAction, int32>;

    static constexpr std::array<ControlActionMapping, 41> CarActionMappings = {{
        { eControllerAction::CA_PED_CYCLE_WEAPON_RIGHT,            -1 },
        { eControllerAction::CA_PED_CYCLE_WEAPON_LEFT,             -1 },
        { eControllerAction::CA_CAMERA_CHANGE_VIEW_ALL_SITUATIONS, -1 },
        { eControllerAction::CA_PED_JUMPING,                       -1 },
        { eControllerAction::CA_PED_SPRINT,                        -1 },
        { eControllerAction::CA_PED_LOOKBEHIND,                    -1 },
        { eControllerAction::CA_PED_DUCK,                          -1 },
        { eControllerAction::CA_VEHICLE_STEER_DOWN,                -1 },
        { eControllerAction::CA_VEHICLE_ACCELERATE,                -1 },
        { eControllerAction::CA_VEHICLE_RADIO_STATION_UP,          -1 },
        { eControllerAction::CA_VEHICLE_RADIO_STATION_DOWN,        -1 },
        { eControllerAction::CA_VEHICLE_RADIO_TRACK_SKIP,          -1 },
        { eControllerAction::CA_VEHICLE_HORN,                      -1 },
        { eControllerAction::CA_VEHICLE_TURRETLEFT,                -1 },
        { eControllerAction::CA_VEHICLE_TURRETRIGHT,               -1 },
        { eControllerAction::CA_PED_CYCLE_TARGET_LEFT,             -1 },
        { eControllerAction::CA_PED_FIRE_WEAPON,                   18 },
        { eControllerAction::CA_PED_FIRE_WEAPON_ALT,               19 },
        { eControllerAction::CA_GO_FORWARD,                        24 },
        { eControllerAction::CA_GO_BACK,                           25 },
        { eControllerAction::CA_GO_LEFT,                           20 },
        { eControllerAction::CA_GO_RIGHT,                          21 },
        { eControllerAction::CA_PED_SNIPER_ZOOM_IN,                22 },
        { eControllerAction::CA_PED_SNIPER_ZOOM_OUT,               23 },
        { eControllerAction::CA_VEHICLE_ENTER_EXIT,                47 },
        { eControllerAction::CA_PED_ANSWER_PHONE,                  10 },
        { eControllerAction::CA_PED_WALK,                          26 },
        { eControllerAction::CA_VEHICLE_FIRE_WEAPON,               27 },
        { eControllerAction::CA_VEHICLE_FIRE_WEAPON_ALT,           28 },
        { eControllerAction::CA_VEHICLE_STEER_LEFT,                29 },
        { eControllerAction::CA_VEHICLE_STEER_RIGHT,               30 },
        { eControllerAction::CA_VEHICLE_STEER_UP,                  11 },
        { eControllerAction::CA_VEHICLE_BRAKE,                     31 },
        { eControllerAction::CA_TOGGLE_SUBMISSIONS,                38 },
        { eControllerAction::CA_VEHICLE_HANDBRAKE,                 39 },
        { eControllerAction::CA_PED_1RST_PERSON_LOOK_LEFT,         41 },
        { eControllerAction::CA_PED_1RST_PERSON_LOOK_RIGHT,        40 },
        { eControllerAction::CA_VEHICLE_LOOKLEFT,                  36 },
        { eControllerAction::CA_VEHICLE_LOOKRIGHT,                 37 },
        { eControllerAction::CA_VEHICLE_LOOKBEHIND,                34 },
        { eControllerAction::CA_VEHICLE_MOUSELOOK,                 35 },
    }};

    static constexpr std::array<ControlActionMapping, 51> PedActionMappings = {{
        { eControllerAction::CA_PED_FIRE_WEAPON,                   0  },
        { eControllerAction::CA_VEHICLE_RADIO_TRACK_SKIP,          0  },
        { eControllerAction::CA_PED_FIRE_WEAPON_ALT,               2  },
        { eControllerAction::CA_PED_CYCLE_WEAPON_RIGHT,            3  },
        { eControllerAction::CA_PED_CYCLE_WEAPON_LEFT,             49 },
        { eControllerAction::CA_GO_FORWARD,                        50 },
        { eControllerAction::CA_GO_BACK,                           48 },
        { eControllerAction::CA_GO_LEFT,                           47 },
        { eControllerAction::CA_VEHICLE_MOUSELOOK,                 47 },
        { eControllerAction::CA_GO_RIGHT,                          4  },
        { eControllerAction::CA_TOGGLE_SUBMISSIONS,                4  },
        { eControllerAction::CA_PED_SNIPER_ZOOM_IN,                5  },
        { eControllerAction::CA_VEHICLE_HANDBRAKE,                 5  },
        { eControllerAction::CA_PED_SNIPER_ZOOM_OUT,               6  },
        { eControllerAction::CA_PED_1RST_PERSON_LOOK_LEFT,         6  },
        { eControllerAction::CA_VEHICLE_ENTER_EXIT,                7  },
        { eControllerAction::CA_PED_1RST_PERSON_LOOK_RIGHT,        7  },
        { eControllerAction::CA_CAMERA_CHANGE_VIEW_ALL_SITUATIONS, 8  },
        { eControllerAction::CA_PED_JUMPING,                       9  },
        { eControllerAction::CA_PED_SPRINT,                        10 },
        { eControllerAction::CA_VEHICLE_LOOKBEHIND,                10 },
        { eControllerAction::CA_PED_LOOKBEHIND,                    11 },
        { eControllerAction::CA_PED_CYCLE_TARGET_RIGHT,            11 },
        { eControllerAction::CA_PED_DUCK,                          12 },
        { eControllerAction::CA_PED_ANSWER_PHONE,                  13 },
        { eControllerAction::CA_PED_WALK,                          45 },
        { eControllerAction::CA_VEHICLE_FIRE_WEAPON,               15 },
        { eControllerAction::CA_VEHICLE_FIRE_WEAPON_ALT,           16 },
        { eControllerAction::CA_VEHICLE_STEER_UP,                  32 },
        { eControllerAction::CA_CONVERSATION_YES,                  32 },
        { eControllerAction::CA_VEHICLE_STEER_DOWN,                33 },
        { eControllerAction::CA_CONVERSATION_NO,                   33 },
        { eControllerAction::CA_VEHICLE_TURRETLEFT,                -1 },
        { eControllerAction::CA_VEHICLE_TURRETRIGHT,               -1 },
        { eControllerAction::CA_VEHICLE_TURRETUP,                  -1 },
        { eControllerAction::CA_VEHICLE_TURRETDOWN,                -1 },
        { eControllerAction::CA_PED_CYCLE_TARGET_LEFT,             -1 },
        { eControllerAction::CA_PED_CENTER_CAMERA_BEHIND_PLAYER,   -1 },
        { eControllerAction::CA_NETWORK_TALK,                      -1 },
        { eControllerAction::CA_GROUP_CONTROL_FWD,                 -1 },
        { eControllerAction::CA_GROUP_CONTROL_BWD,                 -1 },
        { eControllerAction::CA_PED_1RST_PERSON_LOOK_UP,           -1 },
        { eControllerAction::CA_PED_1RST_PERSON_LOOK_DOWN,         -1 },
        { eControllerAction::CA_VEHICLE_RADIO_STATION_DOWN,        1  },
        { eControllerAction::CA_VEHICLE_HORN,                      1  },
        { eControllerAction::CA_VEHICLE_RADIO_STATION_UP,          44 },
        { eControllerAction::CA_VEHICLE_BRAKE,                     52 },
        { eControllerAction::CA_VEHICLE_ACCELERATE,                51 },
        { eControllerAction::CA_VEHICLE_STEER_LEFT,                17 },
        { eControllerAction::CA_VEHICLE_STEER_RIGHT,               14 },
        { eControllerAction::CA_PED_LOCK_TARGET,                   14 },
    }};

    auto currentY = StretchY(float(verticalOffset));

    auto actionIndex = 0u;
    // Main loop - process each action
    while (actionIndex < maxActions) {
        auto currentX = StretchX(270.0f);
        eControllerAction controllerAction = eControllerAction::CA_NONE;

        // Set default text color
        CFont::SetColor({ 255, 255, 255, 255 });

        // Map action index to controller action
        switch (m_RedefiningControls) {
        case eControlMode::VEHICLE:
            for (const auto& mapping : CarActionMappings) {
                if (mapping.first == ControllerActionsAvailableInCar[actionIndex]) {
                    controllerAction = (eControllerAction)mapping.second;
                    break;
                }
            }
            break;
        case eControlMode::FOOT:
            for (const auto& mapping : PedActionMappings) {
                if (mapping.first == (eControllerAction)actionIndex) { // Cast actionIndex to eControllerAction for comparison
                    if (m_ControlMethod == eController::MOUSE_PLUS_KEYS && notsa::contains({ eControllerAction::CA_VEHICLE_STEER_UP, eControllerAction::CA_CONVERSATION_YES, eControllerAction::CA_VEHICLE_STEER_DOWN, eControllerAction::CA_CONVERSATION_NO }, mapping.first)) {
                        controllerAction = eControllerAction::CA_NONE;
                    } else {
                        controllerAction = (eControllerAction)mapping.second;
                    }
                    break;
                }
            }
            break;
        default:
            NOTSA_UNREACHABLE();
        }

        const auto isSelected = (m_ListSelection == actionIndex && !isOppositeScreen);

        // Highlight selected action
        if (isSelected) {
            CSprite2d::DrawRect({
                    StretchX(260.0f),
                    StretchY(actionIndex * verticalSpacing + verticalOffset + 1.f),
                    SCREEN_STRETCH_FROM_RIGHT(20.0f),
                    StretchY(actionIndex * verticalSpacing + verticalOffset + 1.f + 10.f)
                }, { 172, 203, 241, 255 }
            );
            CFont::SetColor({ 255, 255, 255, 255 });
        }

        // Set text properties
        CFont::SetOrientation(eFontAlignment::ALIGN_LEFT);
        CFont::SetScale(StretchX(0.3f), StretchY(0.6f));
        CFont::SetFontStyle(FONT_SUBTITLES);
        CFont::SetWrapx(StretchX(100.0f) + SCREEN_WIDTH);

        // Draw control bindings
        auto hasControl = false;
        if (controllerAction != eControllerAction::CA_NONE && controllerAction != eControllerAction::CA_COMBOLOCK) {

            for (const auto& order : CONTROLLER_ORDERS_SET) {
                if (m_DeleteAllNextDefine && m_ListSelection == actionIndex) {
                    break;
                }
                if (const auto buttonText = ControlsManager.GetControllerSettingText((eControllerAction)controllerAction, (eContSetOrder)order)) {
                    hasControl = true;
                    if (!isOppositeScreen) {
                        CFont::PrintString(currentX, currentY, buttonText);
                    }
                    currentX += StretchX(75.0f);
                }
            }
        }

        // NOTE: Deal with the logic further, because beautifully and fixing the display of “UNBOUND” and “???” at once is not possible

        // 0x57EBD9 + 0x57EBEA
        if (controllerAction == eControllerAction::CA_COMBOLOCK) {
            CFont::SetColor({ 0, 0, 0, 255 });
            if (!isOppositeScreen) {
                CFont::PrintString(currentX, currentY, TheText.Get("FEC_CMP")); // COMBO: Uses LOOK LEFT + LOOK RIGHT together
            }
        } else {
            const auto isEditable = controllerAction > eControllerAction::CA_NONE;
            const auto shouldUpdateBlink = isSelected && isEditable && m_EditingControlOptions;
            if (shouldUpdateBlink) {
                // 0x57ECEB
                if (CTimer::m_snTimeInMillisecondsPauseMode - m_lastBlinkTime > 150) {
                    m_isTextBlinking ^= true;
                    m_lastBlinkTime = CTimer::m_snTimeInMillisecondsPauseMode;
                }
            }

            const auto isQuestionMarkVisible = shouldUpdateBlink && !hasControl && m_isTextBlinking;
            if (!hasControl && isEditable) {
                if (isQuestionMarkVisible) {
                    // 0x57ED19
                    CFont::SetColor({ 0, 0, 0, 255 });
                    if (!isOppositeScreen) {
                        CFont::PrintString(currentX, currentY, TheText.Get("FEC_QUE")); // ???
                    }
                } else if (!isSelected || !m_EditingControlOptions) {
                    // 0x57EC1F - 0x57EC9A
                    m_bRadioAvailable = 0;
                    CFont::SetColor({ 200, 50, 50, 255 });
                    if (!isOppositeScreen) {
                        CFont::PrintString(currentX, currentY, TheText.Get("FEC_UNB")); // UNBOUND
                    }
                }
            }

            // 0x57ECAA
            if (isSelected) {
                if (!isEditable) {
                    DisplayHelperText("FET_EIG"); // CANNOT SET A CONTROL FOR THIS ACTION
                } else {
                    m_OptionToChange = (int32)controllerAction;
                    if (m_EditingControlOptions) {
                        if (m_DeleteAllBoundControls) {
                            DisplayHelperText("FET_CIG"); // BACKSPACE - CLEAR~n~CLICK LMB / RETURN - CHANGE
                        } else {
                            DisplayHelperText("FET_RIG"); // SELECT A NEW CONTROL FOR THIS ACTION~n~ESC - CANCEL
                        }
                        m_CanBeDefined = true;
                    } else {
                        DisplayHelperText("FET_CIG"); // BACKSPACE - CLEAR~n~CLICK LMB / RETURN - CHANGE
                        m_CanBeDefined = false;
                        m_DeleteAllBoundControls = false;
                    }
                }
            }
        }

        // Move to next line and action
        actionIndex++;
        currentY = StretchY(float(verticalOffset + actionIndex * verticalSpacing));
    }
}

// 0x57F300
void CMenuManager::DrawControllerSetupScreen() {
    // Calculate spacing and max items based on control scheme
    const auto verticalSpacing = GetVerticalSpacing();
    const auto maxActions      = GetMaxAction();
    // Create a std::array of GxtChar pointers with all entries
    std::array<const GxtChar*, 44> keys = {
        TheText.Get("FEC_FIR"),                                                      // Fire
        TheText.Get("FEC_FIA"),                                                      // Secondary Fire
        TheText.Get("FEC_NWE"),                                                      // Next weapon/target
        TheText.Get("FEC_PWE"),                                                      // Previous weapon/target
        TheText.Get(m_ControlMethod == eController::JOYPAD ? "FEC_ACC" : "FEC_FOR"), // Accelerate : Forward
        TheText.Get(m_ControlMethod == eController::JOYPAD ? "FEC_BRA" : "FEC_BAC"), // Brake/Reverse : Backwards
        TheText.Get("FEC_LEF"),                                                      // Left
        TheText.Get("FEC_RIG"),                                                      // Right
        TheText.Get("FEC_PLU"),                                                      // Steer Forward/Down
        TheText.Get("FEC_PLD"),                                                      // Steer Back/Up
        TheText.Get(m_ControlMethod == eController::JOYPAD ? "FEC_TSK" : "FEC_COY"), // Trip Skip : Conversation - Yes
        TheText.Get("FEC_CON"),                                                      // Conversation - No
        TheText.Get("FEC_GPF"),                                                      // Group Ctrl Forward
        TheText.Get("FEC_GPB"),                                                      // Group Ctrl Back
        TheText.Get("FEC_ZIN"),                                                      // Zoom in
        TheText.Get("FEC_ZOT"),                                                      // Zoom out
        TheText.Get("FEC_EEX"),                                                      // Enter+exit
        TheText.Get("FEC_RSC"),                                                      // Next radio station
        TheText.Get("FEC_RSP"),                                                      // Previous radio station
        TheText.Get("FEC_RTS"),                                                      // User track skip
        TheText.Get("FEC_HRN"),                                                      // Horn
        TheText.Get("FEC_SUB"),                                                      // Sub-mission
        TheText.Get("FEC_CMR"),                                                      // Change camera
        TheText.Get("FEC_JMP"),                                                      // Jump
        TheText.Get("FEC_SPN"),                                                      // Sprint
        TheText.Get("FEC_HND"),                                                      // Handbrake
        TheText.Get("FEC_TAR"),                                                      // Aim Weapon
        TheText.Get("FEC_CRO"),                                                      // Crouch
        TheText.Get("FEC_ANS"),                                                      // Action
        TheText.Get("FEC_PDW"),                                                      // Walk
        TheText.Get("FEC_TFL"),                                                      // Special Ctrl Left
        TheText.Get("FEC_TFR"),                                                      // Special Ctrl Right
        TheText.Get("FEC_TFU"),                                                      // Special Ctrl Up
        TheText.Get("FEC_TFD"),                                                      // Special Ctrl Down
        TheText.Get("FEC_LBA"),                                                      // Look behind
        TheText.Get("FEC_VML"),                                                      // Mouse Look
        TheText.Get("FEC_LOL"),                                                      // Look left
        TheText.Get("FEC_LOR"),                                                      // Look right
        TheText.Get("FEC_LDU"),                                                      // Look Down
        TheText.Get("FEC_LUD"),                                                      // Look Up
        nullptr,                                                                     // index 40
        nullptr,                                                                     // index 41
        TheText.Get("FEC_CEN"),                                                      // Center camera
        nullptr                                                                      // index 43
    };

    // 0x57F68E
    CFont::SetFontStyle(FONT_GOTHIC);
    CFont::SetScale(StretchX(0.9f), StretchY(1.7f));
    CFont::SetEdge(0);
    CFont::SetColor(HudColour.GetRGB(HUD_COLOUR_LIGHT_BLUE));
    CFont::SetOrientation(eFontAlignment::ALIGN_RIGHT);
    const GxtChar* text;
    switch (m_ControlMethod) {
    case eController::JOYPAD:          text = TheText.Get("FET_CCN"); break; // Joypad
    case eController::MOUSE_PLUS_KEYS: text = TheText.Get("FET_SCN"); break; // Mouse + Keys
    default:                           NOTSA_UNREACHABLE();
    }
    CFont::PrintString(SCREEN_STRETCH_FROM_RIGHT(48.0f), StretchY(11.0f), text);
    CFont::SetOrientation(eFontAlignment::ALIGN_LEFT);
    switch (m_RedefiningControls) {
    case eControlMode::VEHICLE: text = TheText.Get("FET_CCR"); break; // Vehicle Controls
    case eControlMode::FOOT:    text = TheText.Get("FET_CFT"); break; // Foot Controls
    default:                    NOTSA_UNREACHABLE();
    }
    CFont::PrintString(StretchX(48.0f), StretchY(11.0f), text);
    CSprite2d::DrawRect({
        StretchX(20.0f),
        StretchY(50.0f),
        SCREEN_STRETCH_FROM_RIGHT(20.0f),
        SCREEN_STRETCH_FROM_BOTTOM(50.0f)
        }, { 49, 101, 148, 100 }
    );

    // 0x57F8C0
    for (auto i = 0u; i < maxActions; i++) {
        if (!m_EditingControlOptions) {
            if (StretchX(20.0f) < m_nMousePosX && StretchX(600.0f) > m_nMousePosX) {
                if (StretchY(i * verticalSpacing + 69.0f) < m_nMousePosY && StretchY(verticalSpacing * (i + 1) + 69.0f) > m_nMousePosY) {
                    m_CurrentMouseOption = i;
                    if (m_nOldMousePosX != m_nMousePosX || m_nOldMousePosY != m_nMousePosY) {
                        m_ListSelection = i;
                    }
                    if (m_MouseInBounds == 5 && i == m_ListSelection) {
                        m_EditingControlOptions         = true;
                        m_bJustOpenedControlRedefWindow = true;
                        m_pPressedKey                   = &m_KeyPressedCode;
                    }
                    m_MouseInBounds = 16;
                }
            }
        }
        // 0x57F9E1
        CFont::SetColor({ 74, 90, 107, 255 });
        CFont::SetScale(StretchX(0.4f), StretchY(0.6f));
        CFont::SetFontStyle(FONT_MENU);
        CFont::SetWrapx(StretchX(100.0f) + SCREEN_WIDTH);
        const GxtChar* actionText = nullptr;
        switch (m_RedefiningControls) {
        case eControlMode::VEHICLE: actionText = keys[+ControllerActionsAvailableInCar[i]]; break;
        case eControlMode::FOOT:    actionText = keys[+ControllerActionsAvailableOnFoot[i]]; break;
        default:                    NOTSA_UNREACHABLE();
        };

        if (actionText) {
            CFont::PrintString(StretchX(40.0f), StretchY(i * verticalSpacing + 69.0f), actionText);
        }
    }

    // 0x57FAF9
    DrawControllerBound(0x45u, false);
    const auto textBack = TheText.Get("FEDS_TB"); // Back
    if (!m_EditingControlOptions) {
        CFont::SetScale(StretchX(0.7f), StretchY(1.0f));
        const auto color = StretchX(CFont::GetStringWidth(textBack, true, false));
        if (StretchX(35.0f) + color <= m_nMousePosX
            || StretchX(15.0f) >= m_nMousePosX
            || SCREEN_STRETCH_FROM_BOTTOM(33.0f) >= m_nMousePosY
            || SCREEN_STRETCH_FROM_BOTTOM(10.0f) <= m_nMousePosY) {
            if (StretchX(20.0f) >= m_nMousePosX
                || StretchX(600.0f) <= m_nMousePosX
                || StretchY(48.0f) >= m_nMousePosY
                || SCREEN_STRETCH_FROM_BOTTOM(33.0f) <= m_nMousePosY) {
                m_MouseInBounds = 16;
            } else {
                m_MouseInBounds = 4;
            }
        } else {
            m_MouseInBounds = 3;
        }
    }

    // 0x57FCC4
    CFont::SetFontStyle(FONT_MENU);
    CFont::SetScale(StretchX(0.7f), StretchY(1.0f));
    CFont::SetOrientation(eFontAlignment::ALIGN_LEFT);
    CFont::SetEdge(0);
    CFont::SetColor({ 74, 90, 107, 255 });
    CFont::PrintString(StretchX(33.0f), SCREEN_STRETCH_FROM_BOTTOM(38.0f), textBack);
}

/**
 * Draws slider
 * @param x       widget pos x
 * @param y       widget pos y
 * @param h1      height of start strip
 * @param h2      height of end strip
 * @param length  widget length
 * @param value   current value in range [ 0.0f, 1.0f ] (progress)
 * @param spacing size of strip
 * @return
 * @see Audio Setup -> Radio or SFX volume
 * @addr 0x576860
 */
int32 CMenuManager::DisplaySlider(float x, float y, float h1, float h2, float length, float value, int32 spacing) {
    // return plugin::CallMethodAndReturn<int32, 0x576860, CMenuManager*, float, float, float, float, float, float, int32>(this, x, y, h1, h2, length, value, size);

    constexpr auto BARS = 16;
    constexpr auto COLOR_ON  = CRGBA(172, 203, 241, 255); // Fresh Air
    constexpr auto COLOR_OFF = CRGBA(74, 90, 107, 255);   // Deep Space Sparkle
    constexpr auto COLOR_SHADOW = CRGBA(0, 0, 0, 200);

    CRGBA color;

    auto lastActiveBarX = 0;
    for (auto i = 0; i < BARS; i++) {
        const auto fi = float(i);

        float curBarX = fi * length / BARS + x;

        if (fi / (float)BARS + 1 / (BARS * 2.f) < value) {
            color = COLOR_ON;
            lastActiveBarX = (int32)curBarX;
        } else {
            color = COLOR_OFF;
        }

        float maxBarHeight = std::max(h1, h2);

        float curBarFreeSpace = ((BARS - fi) * h1 + fi * h2) / (float)BARS;
        float left   = curBarX;
        float top    = y + maxBarHeight - curBarFreeSpace;
        float right  = float(spacing) + curBarX;
        float bottom = y + maxBarHeight;

        // Useless shadow for stripe. Drawing black on black = nothing. Change color to CRGBA(40, 40, 40, 200) to test
        CSprite2d::DrawRect({
                left + StretchX(2.0f),
                top + StretchY(2.0f),
                right + StretchX(2.0f),
                bottom + StretchY(2.0f)
            }, COLOR_SHADOW
        );
        CSprite2d::DrawRect({ left, top, right, bottom }, color);
    }
    return lastActiveBarX;
}

// NOTSA
// Based on 0x57D914 - 0x57D933
uint32 CMenuManager::GetMaxAction() {
    // TODO: Magis values
    switch (m_RedefiningControls) {
    case eControlMode::FOOT:
        switch (m_ControlMethod) {
        case eController::MOUSE_PLUS_KEYS:
            return 22;
        case eController::JOYPAD:
            return 28;
        default:
            NOTSA_UNREACHABLE();
        }
    case eControlMode::VEHICLE:
        return 25;
    default:
        NOTSA_UNREACHABLE();
    }
}

// NOTSA
// Based on 0x57D8F1 - 0x57D905
uint32 CMenuManager::GetVerticalSpacing() {
    // TODO: Magis values
    switch (m_RedefiningControls) {
    case eControlMode::FOOT:
        switch (m_ControlMethod) {
        case eController::MOUSE_PLUS_KEYS:
            return 15; // 4 * 1 + 11
        case eController::JOYPAD:
            return 11; // 4 * 0 + 11
        default:
            NOTSA_UNREACHABLE();
        }
    case eControlMode::VEHICLE:
        return 13;
    default:
        NOTSA_UNREACHABLE();
    }
}
