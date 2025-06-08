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
    if (m_bDontDrawFrontEnd) {
        return;
    }

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
    char buildInfo[128] = {0};
    strcpy_s(buildInfo, BUILD_NAME_FULL);

    char version[32];
    sprintf_s(version, " / RW %d.%d.%d.%d.%d",
        0xF & RwEngineGetVersion() >> 16,
        0xF & RwEngineGetVersion() >> 12,
        0xF & RwEngineGetVersion() >> 8,
        0xF & RwEngineGetVersion() >> 4,
        0xF & RwEngineGetVersion() >> 0
    );
    strcpy_s(buildInfo + strlen(buildInfo), 32u - strlen(buildInfo), version);

    CFont::SetProportional(true);
    CFont::SetScale(StretchX(0.25f), StretchY(0.5f));
    CFont::SetColor({ 255, 255, 255, 100 });
    CFont::SetOrientation(eFontAlignment::ALIGN_RIGHT);
    CFont::SetFontStyle(eFontStyle::FONT_SUBTITLES);
    CFont::PrintStringFromBottom(SCREEN_STRETCH_FROM_RIGHT(10.0f), SCREEN_STRETCH_FROM_BOTTOM(10.0f), GxtCharFromAscii(buildInfo));
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
    CRect screenRect(-5.0f, -5.0f, SCREEN_WIDTH + 5.0f, SCREEN_HEIGHT + 5.0f);
    CSprite2d::DrawRect(screenRect, MENU_BG);

    if (m_nBackgroundSprite) {
        m_aFrontEndSprites[m_nBackgroundSprite].Draw(backgroundRect, CRGBA(255, 255, 255, 255));
    }

    // 0x57BA02
    if (m_nCurrentScreen == SCREEN_MAP) {
        auto originalOrigin = m_vMapOrigin;
        auto originalZoom   = m_fMapZoom;

        PrintMap();

        m_fMapZoom   = originalZoom;
        m_vMapOrigin = originalOrigin;
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
        DrawStandardMenus(true);
    }

    auto pad = CPad::GetPad(m_nPlayerNumber);
    // 0x57BA6B
    if (m_nControllerError != eControllerError::NONE) {
        if (!field_1B4C_b1) {
            field_1B4C_b1 = true;
            errorStartTime = CTimer::GetTimeInMSPauseMode();
        }

        if (isErrorPendingReset) {
            errorStartTime = CTimer::GetTimeInMSPauseMode();
            isErrorPendingReset = false;
        }

        if (m_nControllerError == eControllerError::VEHICLE) {
            // Error! Changing controls on the 'In Vehicle' screen has caused one or more control actions to be unbound on the 'On Foot' screen. Please check all control actions are set
            // Press ESC to continue
            MessageScreen("FEC_ER3", false, false);
        } else if (m_nControllerError == eControllerError::FOOT) {
            // Error! Changing controls on the 'On Foot' screen has caused one or more control actions to be unbound on the 'In Vehicle' screen. Please check all control actions are set
            // Press ESC to continue
            MessageScreen("FEC_ER2", false, false);
        } else if (m_nControllerError == eControllerError::NOT_SETS) {
            // Error! One or more control actions are not bound to a key or button. Please check all control actions are set
            // Press ESC to continue
            MessageScreen("FEC_ERI", false, false);
        }

        CFont::RenderFontBuffer();
        auto elapsedTime = CTimer::GetTimeInMSPauseMode() - errorStartTime;
        if (elapsedTime > 7'000 || pad->IsEscJustPressed() && elapsedTime > 1'000) {
            m_nControllerError = eControllerError::NONE;
            isErrorPendingReset = true;
        }
    }

    // 0x57BC19
    if (m_bScanningUserTracks) {
        static bool updateScanningTime = StaticRef<bool>(0x8CDFFA); // true
        static int32 progressDirection = StaticRef<int32>(0x8CDFFC); // -1
        static int32 progressPosition = StaticRef<int32>(0x8CE000); // DEFAULT_SCREEN_WIDTH / 2

        if (!bScanningUserTracks) {
            bScanningUserTracks = true;
            m_nUserTrackScanningTimeMs = CTimer::GetTimeInMSPauseMode();
        }

        // SCANNING USER TRACKS - PLEASE WAIT
        //
        // Press ESC to cancel
        MessageScreen("FEA_SMP", false, false);

        const auto left = DEFAULT_SCREEN_WIDTH / 2 - 50;
        const auto bottom = DEFAULT_SCREEN_WIDTH / 2 - DEFAULT_SCREEN_HEIGHT / 6;
        const auto right = DEFAULT_SCREEN_WIDTH / 2 + 50;
        const auto top = DEFAULT_SCREEN_WIDTH / 2 - DEFAULT_SCREEN_HEIGHT / 6;

        // Calculate progress position
        if ((CTimer::m_FrameCounter & 4) != 0) {
            progressPosition -= progressDirection;
            if (progressPosition >= right) {
                progressDirection = 1;
            }
            if (progressPosition <= left) {
                progressDirection = -1;
            }
        }

        CSprite2d::DrawRect({
                StretchX(left),
                StretchY(bottom),
                StretchX(right + 5),
                StretchY(top + 5),
            }, MENU_PROGRESS_BG
        );

        CSprite2d::DrawRect({
                StretchX(float(progressPosition)),
                StretchY(bottom),
                StretchX(float(progressPosition + 5)),
                StretchY(top + 5),
            }, MENU_TEXT_LIGHT_GRAY
        );

        CFont::DrawFonts();
        ResetHelperText();

        if (updateScanningTime) {
            m_nUserTrackScanningTimeMs = CTimer::GetTimeInMSPauseMode();
            updateScanningTime = false;
        }

        if (AEUserRadioTrackManager.ScanUserTracks() || pad->IsEscJustPressed()) {
            if (CTimer::GetTimeInMSPauseMode() - m_nUserTrackScanningTimeMs > 3'000 || pad->IsEscJustPressed()) {
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
                    updateScanningTime = true;
                    SetHelperText(helperText);
                }
                DisplayHelperText(nullptr);
                return;
            }
        }
    } else if (m_DisplayTheMouse) { // 0x57BF62
        CFont::RenderFontBuffer();

        auto mouseX = float(m_nMousePosX);
        auto mouseY = float(m_nMousePosY);

        const auto DrawCursor = [=](auto spriteId) {
            CRect rect;

            rect.left   = mouseX + StretchX(6.0f);
            rect.bottom = mouseY + StretchY(3.0f);
            rect.right  = mouseX + SCREEN_STRETCH_X(24.0f);
            rect.top    = mouseY + SCREEN_SCALE_Y(21.0f);
            m_aFrontEndSprites[spriteId].Draw(rect, { 100, 100, 100, 50 }); // shadow

            rect.left   = mouseX;
            rect.bottom = mouseY;
            rect.right  = mouseX + SCREEN_STRETCH_X(18.0f);
            rect.top    = mouseY + SCREEN_SCALE_Y(18.0f);
            m_aFrontEndSprites[spriteId].Draw(rect, CRGBA(255, 255, 255, 255));
        };

        CRect mapRect(StretchX(60.0f), StretchY(60.0f), SCREEN_STRETCH_FROM_RIGHT(60.0f), SCREEN_STRETCH_FROM_BOTTOM(60.0f));

        if (m_nCurrentScreen == SCREEN_MAP && CPad::IsMouseLButton() && mapRect.IsPointInside({ mouseX, mouseY })) {
            DrawCursor(FRONTEND_SPRITE_CROSS_HAIR);
        } else {
            DrawCursor(FRONTEND_SPRITE_MOUSE);
        }
    }
}

// 0x5794A0
void CMenuManager::DrawStandardMenus(bool drawTitle) {
    constexpr uint16 DEFAULT_CONTENT_X = APP_MINIMAL_WIDTH / 2;
    constexpr uint16 DEFAULT_CONTENT_Y = APP_MINIMAL_HEIGHT / 4 + 10;

    const GxtChar* rightColumnText; // text for right column
    const GxtChar* displayText;     // text to display

    float itemYPosition = 0.0;
    bool shouldDrawStandardItems = true;
    CFont::SetBackground(false, false);
    CFont::SetProportional(true);
    CFont::SetOrientation(eFontAlignment::ALIGN_LEFT);
    CFont::SetWrapx(SCREEN_STRETCH_FROM_RIGHT(10.0f));
    CFont::SetRightJustifyWrap(SCREEN_SCALE_X(10.0f));
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

    // 0x5796B4
    if (aScreens[m_nCurrentScreen].m_aItems[0].m_nActionType == eMenuAction::MENU_ACTION_TEXT) {
        CFont::SetWrapx(SCREEN_STRETCH_FROM_RIGHT(40.0f));
        CFont::SetFontStyle(eFontStyle::FONT_SUBTITLES);
        CFont::SetScaleForCurrentLanguage(StretchX(0.5f), StretchY(1.2f));
        CFont::SetOrientation(eFontAlignment::ALIGN_LEFT);
        CFont::SetEdge(2);
        CFont::SetDropColor(MENU_BG);
        CFont::SetColor(MENU_TEXT_NORMAL);

        const auto GetText = [&](bool condition, const char* key = "") {
            return TheText.Get(!condition ? aScreens[m_nCurrentScreen].m_aItems[0].m_szName : key);
        };
        const GxtChar* textOne;

        // 0x57979A
        switch (m_nCurrentScreen) {
        case eMenuScreen::SCREEN_NEW_GAME_ASK:  textOne = GetText(m_bMainMenuSwitch, "FESZ_QQ"); break; // Are you sure you want to start a new game?
        case eMenuScreen::SCREEN_LOAD_GAME_ASK: textOne = GetText(m_bMainMenuSwitch, "FES_LCG"); break; // Are you sure you want to load this save game?
        case eMenuScreen::SCREEN_SAVE_WRITE_ASK: {
            switch (GetSavedGameState(m_SelectedSlot)) {
            case eSlotState::SLOT_FILLED:    textOne = TheText.Get("FESZ_QO"); break; // Are you sure you wish to overwrite this save file?
            case eSlotState::SLOT_CORRUPTED: textOne = TheText.Get("FESZ_QC"); break; // Proceed with overwriting this corrupted save game?
            default:                         textOne = GetText(false); break;
            }
            break;
        }
        case eMenuScreen::SCREEN_QUIT_GAME_ASK: textOne = GetText(m_bMainMenuSwitch, "FEQ_SRW"); break; // Are you sure you want to quit the game?
        default:                                textOne = GetText(false); break;
        }

        CFont::PrintString(StretchX(60.0f), StretchY(97.0f), textOne);
        CFont::SetWrapx(SCREEN_STRETCH_FROM_RIGHT(10.f));
        CFont::SetRightJustifyWrap(10.0f);
    }

    if (m_nCurrentScreen == eMenuScreen::SCREEN_CONTROLS_DEFINITION) {
        if (m_EditingControlOptions) {
            shouldDrawStandardItems = false;
        }
        DrawControllerScreenExtraText(-8);
    }

    const bool hasLabel = aScreens[m_nCurrentScreen].m_aItems[0].m_nActionType == eMenuAction::MENU_ACTION_TEXT;

    // 0x5798CE
    for (auto i = 0; i < std::size(aScreens[m_nCurrentScreen].m_aItems); i++) {
        auto itemType = aScreens[m_nCurrentScreen].m_aItems[i].m_nType;
        rightColumnText = 0;
        uint16 lineHeight = (itemType == eMenuEntryType::TI_MPACK) ? 20 : 30;
        CFont::SetFontStyle(eFontStyle::FONT_MENU);
        if (itemType < eMenuEntryType::TI_SLOT1 || itemType > eMenuEntryType::TI_SLOT8) {
            CFont::SetScale(StretchX(0.7f), StretchY(1.0f));
            CFont::SetEdge(2);
        } else {
            CFont::SetEdge(1);
            CFont::SetScale(StretchX(0.42f), StretchY(0.95f));
        }
        CFont::SetDropColor(HudColour.GetRGB(HUD_COLOUR_BLACK));
        if (i == m_nCurrentScreenItem && m_bMapLoaded) {
            CFont::SetColor(MENU_TEXT_SELECTED);
        } else {
            CFont::SetColor(MENU_TEXT_NORMAL);
        }

        switch (aScreens[m_nCurrentScreen].m_aItems[i].m_nAlign) {
        case eMenuAlign::MENU_ALIGN_LEFT:  CFont::SetOrientation(eFontAlignment::ALIGN_LEFT); break;
        case eMenuAlign::MENU_ALIGN_RIGHT: CFont::SetOrientation(eFontAlignment::ALIGN_RIGHT); break;
        default:                           CFont::SetOrientation(eFontAlignment::ALIGN_CENTER); break;
        }

        if (!aScreens[m_nCurrentScreen].m_aItems[i].m_X && !aScreens[m_nCurrentScreen].m_aItems[i].m_Y) {
            if (i == 0 || (i == 1 && hasLabel)) {
                aScreens[m_nCurrentScreen].m_aItems[i].m_X = DEFAULT_CONTENT_X;
                aScreens[m_nCurrentScreen].m_aItems[i].m_Y = DEFAULT_CONTENT_Y;
            } else {
                aScreens[m_nCurrentScreen].m_aItems[i].m_X = aScreens[m_nCurrentScreen].m_aItems[i - 1].m_X;
                aScreens[m_nCurrentScreen].m_aItems[i].m_Y = aScreens[m_nCurrentScreen].m_aItems[i - 1].m_Y + lineHeight;
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
                    displayText = nullptr;
                    aScreens[m_nCurrentScreen].m_aItems[i].m_Y = aScreens[m_nCurrentScreen].m_aItems[i - 1].m_Y;
                } else {
                    AsciiToGxtChar(MPacks.m_Name, (GxtChar*)gString);
                    displayText = (GxtChar*)gString;
                    aScreens[m_nCurrentScreen].m_aItems[i].m_nActionType = eMenuAction::MENU_ACTION_MPACK;
                }
            } else {
                NOTSA_UNREACHABLE("Index out of bounds for missionPacksArray");
            }
            break;
        }
        case eMenuEntryType::TI_MOUSEJOYPAD:
            displayText = TheText.Get(m_ControlMethod == eController::JOYPAD
                ? "FEJ_TIT" // Joypad Settings
                : "FEC_MOU" // Mouse Settings
            );
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
                    auto saveName = std::string(GxtCharToUTF8(GetNameOfSavedGame(i - 1)));
                    if (saveName.length() >= 32) {
                        saveName = std::format("{}...", saveName.substr(0, 32 - 4));
                    }

                    AsciiToGxtChar(saveName.c_str(), gGxtString);
                    displayText = gGxtString;
                    rightColumnText = GetSavedGameDateAndTime(i - 1);
                    break;
                }
                case eSlotState::SLOT_CORRUPTED: { // Corrupted save
                    displayText = TheText.Get("FESZ_CS");
                    if (!displayText) {
                        CFont::SetOrientation(eFontAlignment::ALIGN_CENTER);
                        aScreens[m_nCurrentScreen].m_aItems[i].m_X = DEFAULT_CONTENT_X;
                        displayText = TheText.Get(std::format("FEM_SL{}", i).c_str());
                        xOffset = StretchX(40.0f);
                    }
                    break;
                }
                default: { // Empty slot
                    AsciiToGxtChar(std::format("FEM_SL{}", i).c_str(), gGxtString);
                    CFont::SetOrientation(eFontAlignment::ALIGN_CENTER);
                    aScreens[m_nCurrentScreen].m_aItems[i].m_X = DEFAULT_CONTENT_X;
                    displayText = TheText.Get((const char*)gGxtString);
                    xOffset = StretchX(40.0f);
                    break;
                }
                }
                break;
            }
            displayText = TheText.Get(aScreens[m_nCurrentScreen].m_aItems[i].m_szName);
            break;
        }
        }

        const auto GetOnOffText = [](bool condition) {
            return TheText.Get(condition ? "FEM_ON" : "FEM_OFF"); // ON : OFF
        };

        // 0x579DA3
        switch (aScreens[m_nCurrentScreen].m_aItems[i].m_nActionType) {
        case eMenuAction::MENU_ACTION_RADIO_STATION:           rightColumnText = AudioEngine.GetRadioStationName(m_nRadioStation); break;
        case eMenuAction::MENU_ACTION_FRAME_LIMITER:           rightColumnText = GetOnOffText(m_bPrefsFrameLimiter); break;
        case eMenuAction::MENU_ACTION_SUBTITLES:               rightColumnText = GetOnOffText(m_bShowSubtitles); break;
        case eMenuAction::MENU_ACTION_WIDESCREEN:              rightColumnText = GetOnOffText(m_bWidescreenOn); break;
        case eMenuAction::MENU_ACTION_RADIO_EQ:                rightColumnText = GetOnOffText(m_bRadioEq); break;
        case eMenuAction::MENU_ACTION_RADIO_RETUNE:            rightColumnText = GetOnOffText(m_bRadioAutoSelect); break;
        case eMenuAction::MENU_ACTION_SHOW_LEGEND:             rightColumnText = GetOnOffText(m_bMapLegend); break;
        case eMenuAction::MENU_ACTION_HUD_MODE:                rightColumnText = GetOnOffText(m_bHudOn); break;
        case eMenuAction::MENU_ACTION_CONTROLS_JOY_INVERT_X:   rightColumnText = GetOnOffText(m_bInvertPadX1); break;
        case eMenuAction::MENU_ACTION_CONTROLS_JOY_INVERT_Y:   rightColumnText = GetOnOffText(m_bInvertPadY1); break;
        case eMenuAction::MENU_ACTION_CONTROLS_JOY_INVERT_X2:  rightColumnText = GetOnOffText(m_bInvertPadX2); break;
        case eMenuAction::MENU_ACTION_CONTROLS_JOY_INVERT_Y2:  rightColumnText = GetOnOffText(m_bInvertPadY2); break;
        case eMenuAction::MENU_ACTION_CONTROLS_JOY_SWAP_AXIS1: rightColumnText = GetOnOffText(m_bSwapPadAxis1); break;
        case eMenuAction::MENU_ACTION_CONTROLS_JOY_SWAP_AXIS2: rightColumnText = GetOnOffText(m_bSwapPadAxis2); break;
        case eMenuAction::MENU_ACTION_USER_TRACKS_AUTO_SCAN:   rightColumnText = GetOnOffText(m_bTracksAutoScan); break;
        case eMenuAction::MENU_ACTION_STORE_PHOTOS:            rightColumnText = GetOnOffText(m_bSavePhotos); break;
        case eMenuAction::MENU_ACTION_RADAR_MODE:
            switch (m_nRadarMode) {
            case eRadarMode::RADAR_MODE_MAPS_AND_BLIPS: rightColumnText = TheText.Get("FED_RDM"); break; // MAP & BLIPS
            case eRadarMode::RADAR_MODE_BLIPS_ONLY:     rightColumnText = TheText.Get("FED_RDB"); break; // BLIPS ONLY
            case eRadarMode::RADAR_MODE_OFF:            rightColumnText = TheText.Get("FEM_OFF"); break; // OFF
            default:                         NOTSA_UNREACHABLE();
            }
            break;
        case eMenuAction::MENU_ACTION_LANGUAGE:
            switch (m_nPrefsLanguage) {
            case eLanguage::AMERICAN: rightColumnText = TheText.Get("FEL_ENG"); break; // English
            case eLanguage::FRENCH:   rightColumnText = TheText.Get("FEL_FRE"); break; // French
            case eLanguage::GERMAN:   rightColumnText = TheText.Get("FEL_GER"); break; // German
            case eLanguage::ITALIAN:  rightColumnText = TheText.Get("FEL_ITA"); break; // Italian
            case eLanguage::SPANISH:  rightColumnText = TheText.Get("FEL_SPA"); break; // Spanish
            default:                  NOTSA_UNREACHABLE();
            }
            break;
        case eMenuAction::MENU_ACTION_FX_QUALITY:
            switch (g_fx.GetFxQuality()) {
            case FxQuality_e::FX_QUALITY_LOW:       rightColumnText = TheText.Get("FED_FXL"); break; // LOW
            case FxQuality_e::FX_QUALITY_MEDIUM:    rightColumnText = TheText.Get("FED_FXM"); break; // MEDIUM
            case FxQuality_e::FX_QUALITY_HIGH:      rightColumnText = TheText.Get("FED_FXH"); break; // HIGH
            case FxQuality_e::FX_QUALITY_VERY_HIGH: rightColumnText = TheText.Get("FED_FXV"); break; // VERY HIGH
            default:                                NOTSA_UNREACHABLE();
            }
            break;
        case eMenuAction::MENU_ACTION_MIP_MAPPING:
            rightColumnText = GetOnOffText(m_bPrefsMipMapping);
            if (!m_bMainMenuSwitch) {
                CFont::SetColor({ 14, 30, 47, 255 });
            }
            break;
        case eMenuAction::MENU_ACTION_ANTIALIASING: {
            if (m_nDisplayAntialiasing <= 1) {
                rightColumnText = TheText.Get("FEM_OFF");
            } else {
                GxtChar tmpBuffer[64];
                AsciiToGxtChar(std::format("{}", m_nDisplayAntialiasing - 1).c_str(), tmpBuffer);
                rightColumnText = tmpBuffer;
            }
            break;
        }
        case eMenuAction::MENU_ACTION_CONTROLS_MOUSE_INVERT_Y:
            // Standard for all new games
            rightColumnText = notsa::IsFixBugs() ? GetOnOffText(bInvertMouseY) : GetOnOffText(!bInvertMouseY);
            break;
        case eMenuAction::MENU_ACTION_RESOLUTION: {
            GxtChar tmpBuffer[1'024];
            auto Videomodes = GetVideoModeList();
            AsciiToGxtChar(std::format("{}", Videomodes[m_nDisplayVideoMode]).c_str(), tmpBuffer);
            rightColumnText = tmpBuffer;
            break;
        }
        case eMenuAction::MENU_ACTION_CONTROL_TYPE:
            rightColumnText = TheText.Get(m_ControlMethod == eController::JOYPAD
                ? "FET_CCN" // Joypad
                : "FET_SCN" // Mouse + Keys
            );
            break;
        case eMenuAction::MENU_ACTION_MOUSE_STEERING:
            rightColumnText = GetOnOffText(CVehicle::m_bEnableMouseSteering);
            if (m_ControlMethod == eController::JOYPAD) {
                CFont::SetColor({ 14, 30, 47, 255 });
            }
            break;
        case eMenuAction::MENU_ACTION_MOUSE_FLY:
            rightColumnText = GetOnOffText(CVehicle::m_bEnableMouseFlying);
            if (m_ControlMethod == eController::JOYPAD) {
                CFont::SetColor({ 14, 30, 47, 255 });
            }
            break;
        case eMenuAction::MENU_ACTION_USER_TRACKS_PLAY_MODE:
            if (m_nRadioMode < RADIO_MODE_COUNT) {
                rightColumnText = TheText.Get(std::format("FEA_PR{}", m_nRadioMode + 1).c_str());
            }
            break;
        default:
            break;
        }
        
        const auto scaledX = StretchX(aScreens[m_nCurrentScreen].m_aItems[i].m_X);
        const auto scaledY = StretchY(aScreens[m_nCurrentScreen].m_aItems[i].m_Y);

        if (displayText) {
            if ((isSlot && GetSavedGameState(itemType - 1) != eSlotState::SLOT_FILLED) || !isSlot) {
                CFont::PrintString(scaledX, scaledY, displayText);
            // v1.01 +
            } else if (isSlot && GetSavedGameState(itemType - 1) == eSlotState::SLOT_FILLED) {
                CFont::PrintString(StretchX(25.0f + aScreens[m_nCurrentScreen].m_aItems[i].m_X), scaledY, displayText);
                AsciiToGxtChar(std::format("{}:", i).c_str(), gGxtString);
                CFont::PrintString(scaledX, scaledY, gGxtString);
            }
        }

        // 0x57A204
        if (rightColumnText) {
            CFont::SetFontStyle(eFontStyle::FONT_MENU);
            CFont::SetEdge(1);
            CFont::SetOrientation(eFontAlignment::ALIGN_RIGHT);
            if (!isSlot) {
                CFont::SetScale(StretchX((m_nCurrentScreen == SCREEN_AUDIO_SETTINGS && i == 5) ? 0.56f : 0.7f), StretchY(1.0f));
            } else {
                CFont::SetScale(StretchX(0.35f), StretchY(0.95f));
            }

            CFont::PrintString(SCREEN_STRETCH_FROM_RIGHT(40.0f), scaledY, rightColumnText);
        }

        // Check if text was properly initialized and we're on the current selection
        if ((displayText && i == m_nCurrentScreenItem) && (m_nCurrentScreen != SCREEN_MAP && m_nCurrentScreen != SCREEN_BRIEF)) {
            // Calculate X position for highlighted item if not already done
            if (!xOffset) {
                const auto align = aScreens[m_nCurrentScreen].m_aItems[i].m_nAlign;
                xOffset = [&]() -> float {
                    switch (align) {
                    case eMenuAlign::MENU_ALIGN_LEFT:
                        return scaledX - StretchX(40.0f);
                    case eMenuAlign::MENU_ALIGN_RIGHT:
                        return StretchX(40.0f) + scaledX;
                    default:
                        return scaledX - StretchX(40.0f) - CFont::GetStringWidth(displayText, 1, 0) * 0.5f;
                    }
                }();
            }

            // 0x57A50C - Draw highlight rectangle for selected item if map is loaded
            if (m_bMapLoaded) {
                switch (m_nCurrentScreen) {
                case eMenuScreen::SCREEN_LOAD_FIRST_SAVE:
                case eMenuScreen::SCREEN_DELETE_FINISHED:
                case eMenuScreen::SCREEN_SAVE_DONE_1:
                    break;
                default:
                    m_aFrontEndSprites[0].Draw({
                            xOffset,
                            scaledY - StretchX(5.0f),
                            xOffset + StretchX(32.0f),
                            scaledY + StretchX(47.0f)
                        }, CRGBA(255, 255, 255, 255)
                    );
                    break;
                }
            }
        }

        // 0x57A5F1 - Check for video mode changes
        const auto currentItemName = aScreens[m_nCurrentScreen].m_aItems[m_nCurrentScreenItem].m_szName;
        const bool isDisplaySettings = m_nCurrentScreen == SCREEN_DISPLAY_SETTINGS || m_nCurrentScreen == SCREEN_DISPLAY_ADVANCED;

        if (!strcmp(currentItemName, "FED_RES")) { // RESOLUTION
            if (m_nDisplayVideoMode == m_nPrefsVideoMode && m_nHelperText == eHelperText::FET_APP) {
                ResetHelperText();
            } else if (m_nDisplayVideoMode != m_nPrefsVideoMode) {
                SetHelperText(eHelperText::FET_APP);
            }
        } else if (m_nDisplayVideoMode != m_nPrefsVideoMode && isDisplaySettings) {
            m_nDisplayVideoMode = m_nPrefsVideoMode;
            SetHelperText(eHelperText::FET_RSO);
        }

        if (!strcmp(currentItemName, "FED_AAS")) { // ANTI ALIASING
            if (m_nDisplayAntialiasing == m_nPrefsAntialiasing && m_nHelperText == eHelperText::FET_APP) {
                ResetHelperText();
            } else if (m_nDisplayAntialiasing != m_nPrefsAntialiasing) {
                SetHelperText(eHelperText::FET_APP);
            }
        } else if (m_nDisplayAntialiasing != m_nPrefsAntialiasing && isDisplaySettings) {
            m_nDisplayAntialiasing = m_nPrefsAntialiasing;
            SetHelperText(eHelperText::FET_RSO);
        }

        const auto processSlider = [&](float value, bool isRadioVolumeSlider, eMouseInBounds leftBound, eMouseInBounds rightBound) {
            const auto verticalOffset = isRadioVolumeSlider ? 30.f : 0.f;
            const auto sliderPos = DisplaySlider(StretchX(500.0f), StretchY(125.0f - verticalOffset), StretchY(4.0f), StretchY(20.0f), StretchX(100.0f), value, StretchX(3.0f));
            if (i == m_nCurrentScreenItem && shouldDrawStandardItems) {
                if (CheckHover(0, sliderPos - StretchX(3.0f), StretchY(125.0f - verticalOffset), StretchY(150.0f - verticalOffset))) {
                    m_MouseInBounds = leftBound;
                } else {
                    if (CheckHover(StretchX(3.0f) + sliderPos, StretchX(SCREEN_WIDTH), StretchY(125.0f - verticalOffset), StretchY(150.0f - verticalOffset))) {
                        m_MouseInBounds = rightBound;
                        if (StretchX(500.0f) <= m_nMousePosX) {
                            if (StretchY(125.0f - verticalOffset) <= m_nMousePosY && m_nMousePosY <= StretchY(150.0f - verticalOffset)) {
                                return;
                            }
                        }
                    }
                    m_MouseInBounds = eMouseInBounds::NONE;
                }
            }
        };

        // 0x57A7D4 - Handle sliders
        switch (aScreens[m_nCurrentScreen].m_aItems[i].m_nActionType) {
        case MENU_ACTION_BRIGHTNESS: processSlider(m_PrefsBrightness * 0.0026041667f, false, eMouseInBounds::SLIDER_LEFT, eMouseInBounds::SLIDER_RIGHT); break;
        case MENU_ACTION_RADIO_VOL:  processSlider(m_nRadioVolume / (64.f / 1.f), true, eMouseInBounds::RADIO_VOL_LEFT, eMouseInBounds::RADIO_VOL_RIGHT); break;
        case MENU_ACTION_SFX_VOL:    processSlider(m_nSfxVolume / (64.f / 1.f), false, eMouseInBounds::SFX_VOL_LEFT, eMouseInBounds::SFX_VOL_RIGHT); break;
        case MENU_ACTION_DRAW_DIST:  processSlider((m_fDrawDistance - 0.925f) * 1.1428572f, false, eMouseInBounds::DRAW_DIST_LEFT, eMouseInBounds::DRAW_DIST_RIGHT); break;
        case MENU_ACTION_MOUSE_SENS: processSlider(CCamera::m_fMouseAccelHorzntl / 0.005f, false, eMouseInBounds::MOUSE_SENS_LEFT, eMouseInBounds::MOUSE_SENS_RIGHT); break;
        default:                     break;
        }

        // 0x57B239
        if (displayText) {
            itemYPosition += 29 * CFont::GetNumberLines(60.0f, itemYPosition, displayText);
        }
        if (aScreens[m_nCurrentScreen].m_aItems[i].m_nActionType == MENU_ACTION_RADIO_STATION) {
            itemYPosition += 70;
        }
    }

    // 0x57B29F
    switch (m_nCurrentScreen) {
    case eMenuScreen::SCREEN_STATS:
    case eMenuScreen::SCREEN_AUDIO_SETTINGS:
    case eMenuScreen::SCREEN_DISPLAY_SETTINGS:
    case eMenuScreen::SCREEN_USER_TRACKS_OPTIONS:
    case eMenuScreen::SCREEN_DISPLAY_ADVANCED:
    case eMenuScreen::SCREEN_CONTROLLER_SETUP:
    case eMenuScreen::SCREEN_MOUSE_SETTINGS:
    case eMenuScreen::SCREEN_JOYPAD_SETTINGS:
        DisplayHelperText(nullptr);
        break;
    default:
        return;
    }
}

// 0x573EE0
void CMenuManager::DrawWindow(const CRect& coords, const char* key, uint8 color, CRGBA backColor, bool /*unused*/, bool background) {
    if (background) {
        CSprite2d::DrawRect(coords, backColor);
    }

    if (key && *key) {
        CFont::SetWrapx(coords.right);
        CFont::SetColor(CRGBA(MENU_TEXT_LIGHT_GRAY - color, MENU_TEXT_LIGHT_GRAY.a));
        CFont::SetDropColor(MENU_BG);
        CFont::SetEdge(2);
        CFont::SetOrientation(eFontAlignment::ALIGN_LEFT);
        CFont::SetFontStyle(eFontStyle::FONT_GOTHIC);
        CFont::SetScale(SCREEN_SCALE_X(1.0f), SCREEN_SCALE_Y(1.4f));

        float x = coords.left + SCREEN_SCALE_X(10.0f);
        float y = std::min(coords.bottom, coords.top) - SCREEN_SCALE_Y(16.0f);
        CFont::PrintString(x, y, TheText.Get(key));
    }
}

// 0x578F50, untested
void CMenuManager::DrawWindowedText(float x, float y, float wrap, const char* title, const char* message, eFontAlignment alignment) {
    CFont::SetWrapx(x + wrap - StretchX(10.0f));
    CFont::SetRightJustifyWrap(StretchX(10.0f) + wrap);
    CFont::SetCentreSize(wrap - 2.0f * StretchX(10.0f));
    CFont::SetFontStyle(eFontStyle::FONT_SUBTITLES);
    CFont::SetOrientation(alignment);
    CFont::SetScale(StretchX(0.7f), StretchY(1.0f));

    CRect textRect;
    CFont::GetTextRect(&textRect, x, y, TheText.Get(message));
    textRect.left -= 4.0f;
    textRect.bottom += StretchY(22.0f);
    CSprite2d::DrawRect(textRect, MENU_BG);
    CFont::SetColor(MENU_TEXT_LIGHT_GRAY);
    CFont::SetDropColor(MENU_BG);
    CFont::SetEdge(2);
    CFont::SetOrientation(eFontAlignment::ALIGN_LEFT);
    CFont::SetFontStyle(eFontStyle::FONT_GOTHIC);
    CFont::SetScaleForCurrentLanguage(StretchX(1.1f), StretchY(1.4f));
    CFont::SetWrapx(textRect.right);

    if (title && *title) {
        CFont::PrintString(textRect.left + StretchX(20.0f), textRect.top - StretchY(16.0f), TheText.Get(title));
    }

    if (message && *message) {
        CFont::SetWrapx(x + wrap - StretchX(10.0f));
        CFont::SetRightJustifyWrap(StretchX(10.0f) + wrap);
        CFont::SetCentreSize(wrap - 2.0f * StretchX(10.0f));
        CFont::SetFontStyle(eFontStyle::FONT_SUBTITLES);
        CFont::SetOrientation(alignment);
        CFont::SetScale(StretchX(0.7f), StretchY(1.0f));

        CFont::SetDropShadowPosition(2);
        CFont::SetDropColor(MENU_BG);
        CFont::PrintString(x, y + StretchY(15.0f), TheText.Get(message));
    }
}

// 0x57D860
void CMenuManager::DrawQuitGameScreen() {
    m_DisplayTheMouse = false;
    CSprite2d::DrawRect({ 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT }, MENU_BG);
    SaveSettings();
    RsEventHandler(rsQUITAPP, nullptr);
}

// 0x57D8D0
void CMenuManager::DrawControllerScreenExtraText(int32 startingYPos) {
    const auto maxActions      = GetMaxAction();
    const auto verticalSpacing = GetVerticalSpacing();
    if (maxActions > 0) {
        for (auto action = 0u; action < maxActions; action++) {
            float textX = StretchX(240.0f);
            float textY = StretchY(float(startingYPos));
            
            for (const auto& order : CONTROLLER_ORDERS_SET) {
                const auto buttonText = ControlsManager.GetControllerSettingText((eControllerAction)action, order);
                if (buttonText) {
                    CFont::PrintString(textX, textY, buttonText);
                    textX += StretchX(75.0f);
                }
            }
            
            if (action == m_ListSelection && m_EditingControlOptions) {
                if (CTimer::GetTimeInMSPauseMode() - LastFlash > 150) {
                    ColourSwitch ^= true;
                    LastFlash = CTimer::GetTimeInMSPauseMode();
                }
                    
                if (ColourSwitch) {
                    CFont::SetColor(MENU_BG);
                    CFont::PrintString(textX, textY, TheText.Get("FEC_QUE")); // ???
                    CFont::SetColor(MENU_TEXT_NORMAL);
                }
            }

            // 0x57DA95
            if (m_MenuIsAbleToQuit) {
                const auto comboText = ControlsManager.GetButtonComboText((eControllerAction)m_ListSelection);
                if (comboText) {
                    CFont::SetColor(MENU_ERROR);
                    CFont::PrintString(textX, StretchY(textY + 10.f), comboText);
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

    // Main loop - process each action
    for (auto i = 0u; i < maxActions; i++) {
        const auto currentY = StretchY(float(verticalOffset + i * verticalSpacing));
        auto currentX = StretchX(270.0f);
        eControllerAction action = eControllerAction::CA_NONE;

        // Set default text color
        CFont::SetColor(MENU_TEXT_WHITE);

        // Map action index to controller action
        if (m_RedefiningControls == eControlMode::VEHICLE) {
            for (const auto& mapping : CarActionMappings) {
                if (mapping.first == ControllerActionsAvailableInCar[i]) {
                    action = (eControllerAction)mapping.second;
                    break;
                }
            }
        } else if (m_RedefiningControls == eControlMode::FOOT) {
            for (const auto& mapping : PedActionMappings) {
                if (mapping.first == (eControllerAction)i) { // Cast action to eControllerAction for comparison
                    if (m_ControlMethod == eController::MOUSE_PLUS_KEYS && notsa::contains({ eControllerAction::CA_VEHICLE_STEER_UP, eControllerAction::CA_CONVERSATION_YES, eControllerAction::CA_VEHICLE_STEER_DOWN, eControllerAction::CA_CONVERSATION_NO }, mapping.first)) {
                        action = eControllerAction::CA_NONE;
                    } else {
                        action = (eControllerAction)mapping.second;
                    }
                    break;
                }
            }
        }

        // 0x57EA1E - Highlight selected action
        if (m_ListSelection == i && !isOppositeScreen) {
            CSprite2d::DrawRect({
                    StretchX(260.0f),
                    StretchY(float(i * verticalSpacing + verticalOffset + 1)),
                    SCREEN_STRETCH_FROM_RIGHT(20.0f),
                    StretchY(float(i * verticalSpacing + verticalOffset + 1 + 10))
                }, MENU_TEXT_SELECTED
            );
            CFont::SetColor(MENU_TEXT_WHITE);
        }

        // Set text properties
        CFont::SetOrientation(eFontAlignment::ALIGN_LEFT);
        CFont::SetScale(StretchX(0.3f), StretchY(0.6f));
        CFont::SetFontStyle(eFontStyle::FONT_SUBTITLES);
        CFont::SetWrapx(StretchX(100.0f) + SCREEN_WIDTH);

        // Draw control bindings
        bool hasControl = false;
        if (action != eControllerAction::CA_NONE) {
            for (const auto& order : CONTROLLER_ORDERS_SET) {
                if (m_DeleteAllNextDefine && m_ListSelection == i) {
                    break;
                }
                if (auto buttonText = ControlsManager.GetControllerSettingText(action, order)) {
                    hasControl = true;
                    if (!isOppositeScreen) {
                        CFont::PrintString(currentX, currentY, buttonText);
                    }
                    currentX += StretchX(75.0f);
                }
            }
        }

        // 0x57EBD9 + 0x57EBEA - Handle special cases and selection
        if (action == eControllerAction::CA_COMBOLOCK) {
            // 0x57EBEA
            CFont::SetColor(MENU_BG);
            if (!isOppositeScreen) {
                CFont::PrintString(currentX, currentY, TheText.Get("FEC_CMP")); // COMBO: Uses LOOK LEFT + LOOK RIGHT together
            }
        }

        // 0x57EB63 + 0x57EBD9 + 0x57EC13 - Handle unbound case
        if (action != eControllerAction::CA_NONE && action != eControllerAction::CA_COMBOLOCK && !hasControl) {
            // 0x57EC1F
            m_bRadioAvailable = false; // wtf
            if (m_ListSelection != i || !m_EditingControlOptions) {
                CFont::SetColor(MENU_ERROR);
                if (!isOppositeScreen) {
                    CFont::PrintString(currentX, currentY, TheText.Get("FEC_UNB")); // UNBOUND
                }
            }
        }

        // 0x57ECA2 - Handle selection and state
        if (i == m_ListSelection) {
            if (action == eControllerAction::CA_NONE || action == eControllerAction::CA_COMBOLOCK) {
                // CANNOT SET A CONTROL FOR THIS ACTION
                DisplayHelperText("FET_EIG");
            } else {
                m_OptionToChange = action;
                if (m_EditingControlOptions) {
                    if (CTimer::GetTimeInMSPauseMode() - m_lastBlinkTime > 150) {
                        m_isTextBlinking ^= true;
                        m_lastBlinkTime = CTimer::GetTimeInMSPauseMode();
                    }
                    if (m_isTextBlinking) {
                        CFont::SetColor(MENU_BG);
                        if (!isOppositeScreen) {
                            CFont::PrintString(currentX, currentY, TheText.Get("FEC_QUE")); // ???
                        }
                    }
                    if (m_DeleteAllBoundControls) {
                        // BACKSPACE - CLEAR
                        // CLICK LMB / RETURN - CHANGE
                        DisplayHelperText("FET_CIG"); 
                    } else {
                        // SELECT A NEW CONTROL FOR THIS ACTION
                        // ESC - CANCEL
                        DisplayHelperText("FET_RIG");
                    }
                    m_CanBeDefined = true;
                } else {
                    // BACKSPACE - CLEAR
                    // CLICK LMB / RETURN - CHANGE
                    DisplayHelperText("FET_CIG"); 
                    m_CanBeDefined = false;
                    m_DeleteAllBoundControls = false;
                }
            }
        }
    }
}

// 0x57F300
void CMenuManager::DrawControllerSetupScreen() {
    const auto verticalSpacing = GetVerticalSpacing();
    const auto maxActions      = GetMaxAction();
    std::array<const GxtChar*, 44> keys = {
        TheText.Get("FEC_FIR"),                                                             // Fire
        TheText.Get("FEC_FIA"),                                                             // Secondary Fire
        TheText.Get("FEC_NWE"),                                                             // Next weapon/target
        TheText.Get("FEC_PWE"),                                                             // Previous weapon/target
        TheText.Get(m_RedefiningControls == eControlMode::VEHICLE ? "FEC_ACC" : "FEC_FOR"), // Accelerate : Forward
        TheText.Get(m_RedefiningControls == eControlMode::VEHICLE ? "FEC_BRA" : "FEC_BAC"), // Brake/Reverse : Backwards
        TheText.Get("FEC_LEF"),                                                             // Left
        TheText.Get("FEC_RIG"),                                                             // Right
        TheText.Get("FEC_PLU"),                                                             // Steer Forward/Down
        TheText.Get("FEC_PLD"),                                                             // Steer Back/Up
        TheText.Get(m_RedefiningControls == eControlMode::VEHICLE ? "FEC_TSK" : "FEC_COY"), // Trip Skip : Conversation - Yes
        TheText.Get("FEC_CON"),                                                             // Conversation - No
        TheText.Get("FEC_GPF"),                                                             // Group Ctrl Forward
        TheText.Get("FEC_GPB"),                                                             // Group Ctrl Back
        TheText.Get("FEC_ZIN"),                                                             // Zoom in
        TheText.Get("FEC_ZOT"),                                                             // Zoom out
        TheText.Get("FEC_EEX"),                                                             // Enter+exit
        TheText.Get("FEC_RSC"),                                                             // Next radio station
        TheText.Get("FEC_RSP"),                                                             // Previous radio station
        TheText.Get("FEC_RTS"),                                                             // User track skip
        TheText.Get("FEC_HRN"),                                                             // Horn
        TheText.Get("FEC_SUB"),                                                             // Sub-mission
        TheText.Get("FEC_CMR"),                                                             // Change camera
        TheText.Get("FEC_JMP"),                                                             // Jump
        TheText.Get("FEC_SPN"),                                                             // Sprint
        TheText.Get("FEC_HND"),                                                             // Handbrake
        TheText.Get("FEC_TAR"),                                                             // Aim Weapon
        TheText.Get("FEC_CRO"),                                                             // Crouch
        TheText.Get("FEC_ANS"),                                                             // Action
        TheText.Get("FEC_PDW"),                                                             // Walk
        TheText.Get("FEC_TFL"),                                                             // Special Ctrl Left
        TheText.Get("FEC_TFR"),                                                             // Special Ctrl Right
        TheText.Get("FEC_TFU"),                                                             // Special Ctrl Up
        TheText.Get("FEC_TFD"),                                                             // Special Ctrl Down
        TheText.Get("FEC_LBA"),                                                             // Look behind
        TheText.Get("FEC_VML"),                                                             // Mouse Look
        TheText.Get("FEC_LOL"),                                                             // Look left
        TheText.Get("FEC_LOR"),                                                             // Look right
        TheText.Get("FEC_LDU"),                                                             // Look Down
        TheText.Get("FEC_LUD"),                                                             // Look Up
        nullptr,                                                                            // index 40
        nullptr,                                                                            // index 41
        TheText.Get("FEC_CEN"),                                                             // Center camera
        nullptr                                                                             // index 43
    };

    // 0x57F68E
    CFont::SetFontStyle(eFontStyle::FONT_GOTHIC);
    CFont::SetScale(StretchX(0.9f), StretchY(1.7f));
    CFont::SetEdge(0);
    CFont::SetColor(HudColour.GetRGB(HUD_COLOUR_LIGHT_BLUE));
    CFont::SetOrientation(eFontAlignment::ALIGN_RIGHT);
    CFont::PrintString(SCREEN_STRETCH_FROM_RIGHT(48.0f), StretchY(11.0f),
        TheText.Get(m_ControlMethod == eController::JOYPAD
            ? "FET_CCN" // Joypad
            : "FET_SCN" // Mouse + Keys
        )
    );
    CFont::SetOrientation(eFontAlignment::ALIGN_LEFT);
    CFont::PrintString(StretchX(48.0f), StretchY(11.0f),
        TheText.Get(m_RedefiningControls == eControlMode::VEHICLE
            ? "FET_CCR" // Vehicle Controls
            : "FET_CFT" // Foot Controls 
        )
    ); 
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
            if (StretchX(20.0f) < m_nMousePosX
                && StretchX(600.0f) > m_nMousePosX
                && StretchY(float(i * verticalSpacing + 69)) < m_nMousePosY
                && StretchY(float(verticalSpacing * (i + 1) + 69)) > m_nMousePosY)
            {
                m_CurrentMouseOption = i;
                if (m_nOldMousePosX != m_nMousePosX || m_nOldMousePosY != m_nMousePosY) {
                    m_ListSelection = i;
                }
                if (m_MouseInBounds == eMouseInBounds::SELECT && i == m_ListSelection) {
                    m_EditingControlOptions = true;
                    m_bJustOpenedControlRedefWindow = true;
                    m_pPressedKey = &m_KeyPressedCode;
                }
                m_MouseInBounds = eMouseInBounds::NONE;
            }
        }
        // 0x57F9E1
        CFont::SetColor(MENU_TEXT_NORMAL);
        CFont::SetScale(StretchX(0.4f), StretchY(0.6f));
        CFont::SetFontStyle(eFontStyle::FONT_MENU);
        CFont::SetWrapx(StretchX(100.0f) + SCREEN_WIDTH);
        auto actionText = m_RedefiningControls == eControlMode::VEHICLE
            ? keys[+ControllerActionsAvailableInCar[i]]
            : keys[+ControllerActionsAvailableOnFoot[i]];

        if (actionText) {
            CFont::PrintString(StretchX(40.0f), StretchY(float(i * verticalSpacing + 69)), actionText);
        }
    }

    // 0x57FAF9
    DrawControllerBound(69, false);
    auto backText = TheText.Get("FEDS_TB"); // Back
    if (!m_EditingControlOptions) {
        CFont::SetScale(StretchX(0.7f), StretchY(1.0f));
        const auto textWidth = StretchX(CFont::GetStringWidth(backText, true, false));
        if (StretchX(35.0f) + textWidth > m_nMousePosX
            && StretchX(15.0f) < m_nMousePosX
            && SCREEN_STRETCH_FROM_BOTTOM(33.0f) < m_nMousePosY
            && SCREEN_STRETCH_FROM_BOTTOM(10.0f) > m_nMousePosY) {
            m_MouseInBounds = eMouseInBounds::BACK_BUTTON;
        } else if (StretchX(20.0f) < m_nMousePosX
            && StretchX(600.0f) > m_nMousePosX
            && StretchY(48.0f) < m_nMousePosY
            && SCREEN_STRETCH_FROM_BOTTOM(33.0f) > m_nMousePosY) {
            m_MouseInBounds = eMouseInBounds::ENTER_MENU;
        } else {
            m_MouseInBounds = eMouseInBounds::NONE;
        }
    }

    // 0x57FCC4
    CFont::SetFontStyle(eFontStyle::FONT_MENU);
    CFont::SetScale(StretchX(0.7f), StretchY(1.0f));
    CFont::SetOrientation(eFontAlignment::ALIGN_LEFT);
    CFont::SetEdge(0);
    CFont::SetColor(MENU_TEXT_NORMAL);
    CFont::PrintString(StretchX(33.0f), SCREEN_STRETCH_FROM_BOTTOM(38.0f), backText);
}

/**
 * Draws slider
 * @param x       widget pos x
 * @param y       widget pos y
 * @param barHeight1      height of start strip
 * @param barHeight2      height of end strip
 * @param length  widget length
 * @param value   current value in range [ 0.0f, 1.0f ] (progress)
 * @param barSpacing size of strip
 * @return
 * @see Audio Setup -> Radio or SFX volume
 * @addr 0x576860
 */
int32 CMenuManager::DisplaySlider(float x, float y, float barHeight1, float barHeight2, float length, float value, float barSpacing) {
    constexpr auto NUM_BARS               = 16;
    constexpr auto BAR_WIDTH_FACTOR       = 1.0f / NUM_BARS;         // 0.0625f
    constexpr auto VALUE_THRESHOLD_OFFSET = BAR_WIDTH_FACTOR / 2.0f; // 0.03125f
    
    float maxBarHeight = std::max(barHeight1, barHeight2);
    auto lastActiveBarX = 0;

    for (auto i = 0; i < NUM_BARS; i++) {
        float barIndex     = float(i);
        float currentBarX  = x + barIndex * length * BAR_WIDTH_FACTOR;
        float barFreeSpace = (barHeight1 * (NUM_BARS - barIndex) + barHeight2 * barIndex) * BAR_WIDTH_FACTOR;

        // Set bar color based on value threshold
        CRGBA barColor = barIndex * BAR_WIDTH_FACTOR + VALUE_THRESHOLD_OFFSET < value
            ? MENU_TEXT_SELECTED
            : MENU_TEXT_NORMAL;
        if (barColor == MENU_TEXT_SELECTED) {
            lastActiveBarX = (int32)currentBarX;
        }

        CRect barRect = {
            currentBarX,
            y + maxBarHeight - barFreeSpace,
            currentBarX + barSpacing,
            y + maxBarHeight
        };

        // Draw shadow rectangle with resolution scaling
        CSprite2d::DrawRect({
                barRect.left + StretchX(2.0f),
                barRect.bottom + StretchY(2.0f),
                barRect.right + StretchX(2.0f),
                barRect.top + StretchY(2.0f)
            },MENU_SHADOW
        );

        // Draw foreground rectangle
        CSprite2d::DrawRect(barRect, barColor);
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
