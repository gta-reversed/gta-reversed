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

constexpr eControllerAction ControllerActionsAvailableOnFoot[] = { 
    eControllerAction::PED_FIRE_WEAPON,             // 0
    eControllerAction::PED_CYCLE_WEAPON_RIGHT,      // 2
    eControllerAction::PED_CYCLE_WEAPON_LEFT,       // 3
    eControllerAction::PED_JUMPING,                 // 12
    eControllerAction::PED_SPRINT,                  // 13
    eControllerAction::CAMERA_CHANGE_VIEW_ALL_SITUATIONS, // 11
    eControllerAction::VEHICLE_ENTER_EXIT,          // 10
    eControllerAction::GO_FORWARD,                  // 4
    eControllerAction::GO_BACK,                     // 5
    eControllerAction::GO_LEFT,                     // 6
    eControllerAction::GO_RIGHT,                    // 7
    eControllerAction::PED_LOOKBEHIND,              // 14
    eControllerAction::PED_DUCK,                    // 15
    eControllerAction::PED_ANSWER_PHONE,            // 16
    eControllerAction::VEHICLE_STEER_UP,            // 22
    eControllerAction::VEHICLE_STEER_DOWN,          // 23
    eControllerAction::VEHICLE_ACCELERATE,          // 24
    eControllerAction::VEHICLE_RADIO_STATION_UP,    // 26
    eControllerAction::VEHICLE_RADIO_STATION_DOWN,  // 27
    eControllerAction::VEHICLE_RADIO_TRACK_SKIP,    // 28
    eControllerAction::VEHICLE_HORN,                // 29
    eControllerAction::VEHICLE_LOOKLEFT,            // 34
    eControllerAction::VEHICLE_LOOKBEHIND,          // 36
    eControllerAction::VEHICLE_MOUSELOOK,           // 37
    eControllerAction::VEHICLE_TURRETLEFT,          // 38
    eControllerAction::VEHICLE_TURRETRIGHT,         // 39
    eControllerAction::PED_CYCLE_TARGET_LEFT,       // 42
    eControllerAction::PED_FIRE_WEAPON_ALT          // 1
}; // 0x865598

constexpr eControllerAction ControllerActionsAvailableInCar[] = {
    eControllerAction::PED_FIRE_WEAPON,             // 0
    eControllerAction::PED_FIRE_WEAPON_ALT,         // 1
    eControllerAction::GO_FORWARD,                  // 4
    eControllerAction::GO_BACK,                     // 5
    eControllerAction::GO_LEFT,                     // 6
    eControllerAction::GO_RIGHT,                    // 7
    eControllerAction::PED_SNIPER_ZOOM_IN,          // 8
    eControllerAction::PED_SNIPER_ZOOM_OUT,         // 9
    eControllerAction::PED_ANSWER_PHONE,            // 16
    eControllerAction::VEHICLE_ENTER_EXIT,          // 10
    eControllerAction::PED_WALK,                    // 17
    eControllerAction::VEHICLE_FIRE_WEAPON,         // 18
    eControllerAction::VEHICLE_FIRE_WEAPON_ALT,     // 19
    eControllerAction::VEHICLE_STEER_LEFT,          // 20
    eControllerAction::VEHICLE_STEER_RIGHT,         // 21
    eControllerAction::VEHICLE_STEER_UP,            // 22
    eControllerAction::VEHICLE_BRAKE,               // 25
    eControllerAction::VEHICLE_LOOKLEFT,            // 34
    eControllerAction::VEHICLE_LOOKRIGHT,           // 35
    eControllerAction::VEHICLE_LOOKBEHIND,          // 36
    eControllerAction::VEHICLE_MOUSELOOK,           // 37
    eControllerAction::TOGGLE_SUBMISSIONS,          // 30
    eControllerAction::VEHICLE_HANDBRAKE,           // 31
    eControllerAction::PED_1RST_PERSON_LOOK_LEFT,   // 32
    eControllerAction::PED_1RST_PERSON_LOOK_RIGHT   // 33
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
    CFont::PrintStringFromBottom(SCREEN_WIDTH - StretchX(10.0f), SCREEN_HEIGHT - StretchY(10.0f), GxtCharFromAscii(buf));
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
                SCREEN_WIDTH - StretchX(256.0f),
                0.0f,
                SCREEN_WIDTH,
                StretchY(256.0f),
            };
        case SCREEN_DELETE_FINISHED:
            return {
                SCREEN_WIDTH - StretchX(256.0f),
                0.0f,
                SCREEN_WIDTH,
                StretchY(256.0f),
            };
        case SCREEN_DELETE_SUCCESSFUL:
        case SCREEN_SAVE_WRITE_ASK:
            return {
                SCREEN_WIDTH - StretchX(256.0f),
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
                SCREEN_WIDTH - StretchX(300.0f),
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
    } else if (m_bDrawMouse) { // 0x57BF62
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

        CRect mapRect(StretchX(60.0f), StretchY(60.0f), SCREEN_WIDTH - StretchX(60.0f), SCREEN_HEIGHT - StretchY(60.0f));

        if (m_nCurrentScreen == SCREEN_MAP && CPad::NewMouseControllerState.m_bLeftButton && mapRect.IsPointInside(CVector2D(x, y))) {
            DrawCursor(FRONTEND_SPRITE_CROSS_HAIR);
        } else {
            DrawCursor(FRONTEND_SPRITE_MOUSE);
        }
    }
}

// 0x5794A0
void CMenuManager::DrawStandardMenus(bool drawTitle) {
    CRGBA color;
    float buttonTextPosY   = 0.0f;

    // Font alignment setup
    CFont::SetBackground(0, 0);
    CFont::SetProportional(1);
    CFont::SetOrientation(eFontAlignment::ALIGN_LEFT);
    CFont::SetWrapx(RsGlobal.maximumWidth - 10);
    CFont::SetRightJustifyWrap(10.0);
    CFont::SetCentreSize(RsGlobal.maximumWidth);

    // Special case screens
    if (this->m_nCurrentScreen == SCREEN_STATS) {
        CMenuManager::PrintStats();
DRAW_TITLE:
        if (drawTitle) {
            eMenuScreen currentScreen = this->m_nCurrentScreen;
            if (aScreens[currentScreen].m_szTitleName[0]) {
                if (currentScreen != SCREEN_MAP || !this->m_bMapLoaded) {
                    CFont::SetOrientation(eFontAlignment::ALIGN_LEFT);
                    CFont::SetFontStyle(FONT_GOTHIC);
                    float h;
                    if (RsGlobal.maximumHeight == 448) {
                        h = 2.1;
                    } else {
                        h = RsGlobal.maximumHeight * 0.0046874997;
                    }
                    if (RsGlobal.maximumWidth == 640) {
                        CFont::SetScale(1.3, h);
                    } else {
                        CFont::SetScale(RsGlobal.maximumWidth * 0.0020312499, h);
                    }
                    CFont::SetEdge(1);
                    CRGBA color = HudColour.GetRGB(HUD_COLOUR_LIGHT_BLUE);
                    CFont::SetColor(color);
                    color = HudColour.GetRGB(HUD_COLOUR_BLACK);
                    CFont::SetDropColor(color);
                    float ya, text;
                    if (RsGlobal.maximumHeight == 448) {
                        ya = 28.0;
                    } else {
                        ya = RsGlobal.maximumHeight * 0.0625;
                    }
                    if (RsGlobal.maximumWidth == 640) {
                        text = 40.0;
                    } else {
                        text = RsGlobal.maximumWidth * 0.0625;
                    }
                    const char* titleText = TheText.Get(aScreens[this->m_nCurrentScreen].m_szTitleName);
                    CFont::PrintString(text, ya, titleText);
                }
            }
        }
        goto LABEL_25;
    }
    if (this->m_nCurrentScreen == SCREEN_BRIEF) {
      CMenuManager::PrintBriefs();
      goto DRAW_TITLE;
    }
    if (this->m_nCurrentScreen != SCREEN_AUDIO_SETTINGS) {
        goto DRAW_TITLE;
    }
    if (drawTitle) {
        CMenuManager::PrintRadioStationList();
        goto DRAW_TITLE;
    }
LABEL_25:

    // Draw screen description text
    if (aScreens[m_nCurrentScreen].m_aItems[0].m_nActionType == MENU_ACTION_TEXT) {
        CFont::SetWrapx(RsGlobal.maximumWidth - 40.0f);
        CFont::SetFontStyle(FONT_SUBTITLES);

        float descFontHeight = (RsGlobal.maximumHeight == 448) ? 1.2f : RsGlobal.maximumHeight * 0.0026786f;
        float descFontWidth  = (RsGlobal.maximumWidth == 640) ? 0.49f : RsGlobal.maximumWidth * 0.0007656f;

        CFont::SetScaleForCurrentLanguage(descFontWidth, descFontHeight);
        CFont::SetOrientation(eFontAlignment::ALIGN_LEFT);
        CFont::SetEdge(2);
        CFont::SetDropColor(CRGBA(0, 0, 0, 255));
        CFont::SetColor(CRGBA(74, 90, 107, 255));

        float descPosY = (RsGlobal.maximumHeight == 448) ? 97.0f : RsGlobal.maximumHeight * 0.2165f;
        float descPosX = (RsGlobal.maximumWidth == 640) ? 60.0f : RsGlobal.maximumWidth * 0.09375f;

        const auto textKey = aScreens[m_nCurrentScreen].m_aItems[0].m_szName;
        const char* descText = nullptr;

        // Handle special screen text cases
        switch (m_nCurrentScreen) {
        case SCREEN_NEW_GAME_ASK:
            if (m_bMainMenuSwitch) {
                descText = TheText.Get("FESZ_QQ");
            } else {
                descText = TheText.Get(textKey);
            }
            break;
        case SCREEN_LOAD_GAME:
            if (m_bMainMenuSwitch) {
                descText = TheText.Get("FES_LCG");
            } else {
                descText = TheText.Get(textKey);
            }
            break;
        case SCREEN_GAME_SAVE: {
            int saveGameStatus = (int)CGenericGameStorage::ms_Slots[m_bSelectedSaveGame];
            if (saveGameStatus == 0) {
                descText = TheText.Get("FESZ_QO");
            } else if (saveGameStatus == 2) {
                descText = TheText.Get("FESZ_QC");
            } else {
                descText = TheText.Get(textKey);
            }
        } break;
        case SCREEN_QUIT_GAME_ASK:
            if (m_bMainMenuSwitch) {
                descText = TheText.Get("FEQ_SRW");
            } else {
                descText = TheText.Get(textKey);
            }
            break;
        default:
            descText = TheText.Get(textKey);
            break;
        }

        if (descText) {
            CFont::PrintString(descPosX, descPosY, descText);
        }

        CFont::SetWrapx(RsGlobal.maximumWidth - 10.0f);
        CFont::SetRightJustifyWrap(10.0f);
    }

    // Special case for controller setup screen
    if (m_nCurrentScreen == SCREEN_CONTROLS_DEFINITION) {
        if (!m_EditingControlOptions) {
            DrawControllerScreenExtraText(-8);
        }
    }

    // Process all menu items
    bool hasTextActionType = (aScreens[m_nCurrentScreen].m_aItems[0].m_nActionType == MENU_ACTION_TEXT);

    for (int buttonIndex = 0; buttonIndex < 12; buttonIndex++) {
        const char* buttonText = nullptr;
        const char* prefSwitch = nullptr;
        float buttonPosX = 0.0f;
        float buttonPosY = 0.0f;

        int currentButtonIndex = buttonIndex;
        int buttonType = aScreens[m_nCurrentScreen].m_aItems[buttonIndex].m_szName[8];
        int xOffset = (buttonType == 9) ? 20 : 30;

        // Set font style based on button type
        if (buttonType < 1 || buttonType > 8) {
            CFont::SetFontStyle(FONT_MENU);
            float fontHeight = (RsGlobal.maximumHeight == 448) ? 1.0f : RsGlobal.maximumHeight * 0.0022321f;
            float fontWidth  = (RsGlobal.maximumWidth == 640) ? 0.7f : RsGlobal.maximumWidth * 0.0010938f;
            CFont::SetScale(fontWidth, fontHeight);
            CFont::SetEdge(2);
        } else {
            CFont::SetFontStyle(FONT_MENU);
            CFont::SetEdge(1);
            float scaleY = (RsGlobal.maximumHeight == 448) ? 0.95f : RsGlobal.maximumHeight * 0.0021205f;
            float scaleX = (RsGlobal.maximumWidth == 640) ? 0.42f : RsGlobal.maximumWidth * 0.0006563f;
            CFont::SetScale(scaleX, scaleY);
        }

        // Set text color
        CFont::SetDropColor(CRGBA(0, 0, 0, 255));
        if (buttonIndex == m_nCurrentScreenItem && m_bMapLoaded) {
            CFont::SetColor(CRGBA(172, 203, 241, 255)); // Highlighted
        } else {
            CFont::SetColor(CRGBA(74, 90, 107, 255)); // Normal
        }

        // Set text alignment
        int alignType = aScreens[m_nCurrentScreen].m_aItems[currentButtonIndex].m_nAlign;
        if (alignType == 1) {
            CFont::SetOrientation(eFontAlignment::ALIGN_LEFT);
        } else if (alignType == 2) {
            CFont::SetOrientation(eFontAlignment::ALIGN_RIGHT);
        } else {
            CFont::SetOrientation(eFontAlignment::ALIGN_CENTER);
        }

        // Initialize position if not set
        if (aScreens[m_nCurrentScreen].m_aItems[currentButtonIndex].m_X == 0 && aScreens[m_nCurrentScreen].m_aItems[currentButtonIndex].m_Y == 0 || buttonType == 9) {
            if (buttonIndex == 0 || (buttonIndex == 1 && hasTextActionType)) {
                aScreens[m_nCurrentScreen].m_aItems[currentButtonIndex].m_X = 320;
                aScreens[m_nCurrentScreen].m_aItems[currentButtonIndex].m_Y = 130;
            } else {
                // Default positions based on button index
                aScreens[m_nCurrentScreen].m_aItems[currentButtonIndex].m_X = 320;
                aScreens[m_nCurrentScreen].m_aItems[currentButtonIndex].m_Y = xOffset + 130 + (buttonIndex * 25);
            }
        }

        // Skip empty or text-only items
        int actionType = aScreens[m_nCurrentScreen].m_aItems[currentButtonIndex].m_nActionType;
        if (actionType == MENU_ACTION_TEXT || !aScreens[m_nCurrentScreen].m_aItems[buttonIndex].m_szName[0]) {
            continue;
        }

        // Get button text based on type
        if (buttonType < 1 || buttonType > 8) {
            if (buttonType == 9) {
                // Handle stats scroll
                int8* statsScrollDirectionPtr = &m_nStatsScrollDirection + 261 * buttonIndex;
                if (*(statsScrollDirectionPtr - 169)) {
                    char tempStr[256];
                    strcpy(tempStr, (char*)(statsScrollDirectionPtr - 168));
                    GxtChar gxtStr[256];
                    AsciiToGxtChar(tempStr, gxtStr);
                    buttonText = (const char*)gxtStr;
                    aScreens[m_nCurrentScreen].m_aItems[buttonIndex].m_nActionType = MENU_ACTION_MPACK;
                } else {
                    aScreens[m_nCurrentScreen].m_aItems[buttonIndex].m_nActionType = (eMenuAction)20;
                    buttonText = nullptr;
                    aScreens[m_nCurrentScreen].m_aItems[buttonIndex].m_Y = aScreens[m_nCurrentScreen].m_szTitleName[18 * buttonIndex + 6];
                }
            } else if (buttonType == 10) {
                buttonText = m_ControlMethod ? TheText.Get("FEJ_TIT") : TheText.Get("FEC_MOU");
            } else {
                buttonText = TheText.Get(aScreens[m_nCurrentScreen].m_aItems[buttonIndex].m_szName);
            }
        } else {
            // Save game slots handling
            aScreens[m_nCurrentScreen].m_aItems[buttonIndex].m_X = 80;
            CFont::SetOrientation(eFontAlignment::ALIGN_LEFT);

            int saveGameStatus = (int)CGenericGameStorage::ms_Slots[buttonIndex];
            if (saveGameStatus == 0 || saveGameStatus == 2) {
                // Empty slot or corrupted
                char slotStr[32];
                sprintf(slotStr, "FEM_SL%d", buttonIndex);
                CFont::SetOrientation(eFontAlignment::ALIGN_CENTER);
                aScreens[m_nCurrentScreen].m_aItems[buttonIndex].m_X = 320;
                buttonText                                           = TheText.Get(slotStr);
                buttonPosX                                           = (RsGlobal.maximumWidth == 640) ? 40 : (int)(RsGlobal.maximumWidth * 0.0625f);
            } else {
                // Valid saved game
                const GxtChar* saveName = CGenericGameStorage::ms_SlotFileName[buttonIndex - 1];
                if (saveName != nullptr && GxtCharStrlen(saveName) < 254) {
                    buttonText = (const GxtChar*)saveName;
                } else {
                    // Truncate or handle invalid name
                    GxtChar tempName[256];
                    strcpy((char*)tempName, (const char*)saveName);
                    if (GxtCharStrlen(saveName) >= 254) {
                        GxtChar ellipsis[4];
                        AsciiToGxtChar("...", ellipsis);
                        GxtCharStrcat(tempName, ellipsis);
                    }
                    buttonText = (const GxtChar*)tempName;
                }
            }
        }

        // Handle special action types
        switch (actionType) {
        case 24: // Frame limiter
            prefSwitch = m_bPrefsFrameLimiter ? TheText.Get("FEM_ON") : TheText.Get("FEM_OFF");
            break;
        case 25: // Subtitles
            prefSwitch = m_bShowSubtitles ? TheText.Get("FEM_ON") : TheText.Get("FEM_OFF");
            break;
        case 26: // Widescreen
            prefSwitch = m_bWidescreenOn ? TheText.Get("FEM_ON") : TheText.Get("FEM_OFF");
            break;
        case 30: // Radio EQ
            prefSwitch = m_bRadioEq ? TheText.Get("FEM_ON") : TheText.Get("FEM_OFF");
            break;
        case 31: // Radio auto select
            prefSwitch = m_bRadioAutoSelect ? TheText.Get("FEM_ON") : TheText.Get("FEM_OFF");
            break;
        case 32: // Radio station
            prefSwitch = AudioEngine.GetRadioStationName((eRadioID)m_nRadioStation);
            break;
        case 33: // Map legend
            prefSwitch = m_bMapLegend ? TheText.Get("FEM_ON") : TheText.Get("FEM_OFF");
            break;
        case 34: // Radar mode
            if (m_nRadarMode == 0) {
                prefSwitch = TheText.Get("FED_RDM");
            } else if (m_nRadarMode == 1) {
                prefSwitch = TheText.Get("FED_RDB");
            } else if (m_nRadarMode == 2) {
                prefSwitch = TheText.Get("FEM_OFF");
            }
            break;
        case 35: // HUD on/off
            prefSwitch = m_bHudOn ? TheText.Get("FEM_ON") : TheText.Get("FEM_OFF");
            break;
        case 36: // Language
            switch (m_nPrefsLanguage) {
            case eLanguage::AMERICAN:
                prefSwitch = TheText.Get("FEL_ENG");
                break;
            case eLanguage::FRENCH:
                prefSwitch = TheText.Get("FEL_FRE");
                break;
            case eLanguage::GERMAN:
                prefSwitch = TheText.Get("FEL_GER");
                break;
            case eLanguage::ITALIAN:
                prefSwitch = TheText.Get("FEL_ITA");
                break;
            case eLanguage::SPANISH:
                prefSwitch = TheText.Get("FEL_SPA");
                break;
            }
            break;
        case 42: // FX Quality
            switch (g_fx.GetFxQuality()) {
            case 0:
                prefSwitch = TheText.Get("FED_FXL");
                break;
            case 1:
                prefSwitch = TheText.Get("FED_FXM");
                break;
            case 2:
                prefSwitch = TheText.Get("FED_FXH");
                break;
            case 3:
                prefSwitch = TheText.Get("FED_FXV");
                break;
            }
            break;
        case 43: // MipMapping
            prefSwitch = m_bPrefsMipMapping ? TheText.Get("FEM_ON") : TheText.Get("FEM_OFF");
            if (!m_bMainMenuSwitch) {
                CFont::SetColor(CRGBA(0xEu, 0x1Eu, 0x2Fu, 0xFFu));
            }
            break;
        case 44: // Antialiasing
        {
            auto* antialiasingBuffer = CMemoryMgr::Calloc(0x64u, 1u);
            if (m_nDisplayAntialiasing <= 1) {
                prefSwitch = TheText.Get("FEM_OFF");
            } else {
                char antialiasingBuffer[64];
                snprintf(antialiasingBuffer, sizeof(antialiasingBuffer), "%d", m_nDisplayAntialiasing - 1);
                GxtChar v227[64]; // Assuming this is the GxtChar buffer equivalent
                AsciiToGxtChar(antialiasingBuffer, v227);
                prefSwitch = v227;
            }
            CMemoryMgr::Free(antialiasingBuffer);
        } break;
        case 46: // Invert Mouse Y
            prefSwitch = bInvertMouseY ? TheText.Get("FEM_OFF") : TheText.Get("FEM_ON");
            break;
        case 47: // Invert Pad X1
            prefSwitch = m_bInvertPadX1 ? TheText.Get("FEM_ON") : TheText.Get("FEM_OFF");
            break;
        case 48: // Invert Pad Y1
            prefSwitch = m_bInvertPadY1 ? TheText.Get("FEM_ON") : TheText.Get("FEM_OFF");
            break;
        case 49: // Invert Pad X2
            prefSwitch = m_bInvertPadX2 ? TheText.Get("FEM_ON") : TheText.Get("FEM_OFF");
            break;
        case 50: // Invert Pad Y2
            prefSwitch = m_bInvertPadY2 ? TheText.Get("FEM_ON") : TheText.Get("FEM_OFF");
            break;
        case 51: // Swap Pad Axis 1
            prefSwitch = m_bSwapPadAxis1 ? TheText.Get("FEM_ON") : TheText.Get("FEM_OFF");
            break;
        case 52: // Swap Pad Axis 2
            prefSwitch = m_bSwapPadAxis2 ? TheText.Get("FEM_ON") : TheText.Get("FEM_OFF");
            break;
        case 56: // Video Mode
        {
            const auto* videoModeText = GetVideoModeList();
            GxtChar     v227[64]; // Assuming this is the GxtChar buffer equivalent
            AsciiToGxtChar(videoModeText[m_nDisplayVideoMode], v227);
            int videoModeIndex = 0;
            if (!v227[0]) {
                prefSwitch = v227;
                break;
            }
            while (v227[videoModeIndex] != 40) {
                if (!v227[++videoModeIndex]) {
                    prefSwitch = v227;
                    break;
                }
            }
            for (; videoModeIndex > 0; --videoModeIndex) {
                if (v227[videoModeIndex] != 32) {
                    break;
                }
            }
            v227[videoModeIndex] = 0;
            prefSwitch           = v227;
        } break;
        case 58: // Control Method
            prefSwitch = m_ControlMethod == 1 ? TheText.Get("FET_CCN") : TheText.Get("FET_SCN");
            break;
        case 59: // Mouse Steering
            prefSwitch = CVehicle::m_bEnableMouseSteering ? TheText.Get("FEM_ON") : TheText.Get("FEM_OFF");
            if (m_ControlMethod == 1) {
                CFont::SetColor(CRGBA(0xE, 0x1E, 0x2F, 0xFF));
            }
            break;
        case 60: // Mouse Flying
            prefSwitch = CVehicle::m_bEnableMouseFlying ? TheText.Get("FEM_ON") : TheText.Get("FEM_OFF");
            if (m_ControlMethod == 1) {
                CFont::SetColor(CRGBA(0xEu, 0x1Eu, 0x2Fu, 0xFFu));
            }
            break;
        case 63: // Radio Mode
            if (m_nRadioMode == 0) {
                prefSwitch = TheText.Get("FEA_PR1");
            } else if (m_nRadioMode == 1) {
                prefSwitch = TheText.Get("FEA_PR2");
            } else if (m_nRadioMode == 2) {
                prefSwitch = TheText.Get("FEA_PR3");
            }
            break;
        case 64: // Tracks Auto Scan
            prefSwitch = m_bTracksAutoScan ? TheText.Get("FEM_ON") : TheText.Get("FEM_OFF");
            break;
        case 65: // Save Photos
            prefSwitch = m_bSavePhotos ? TheText.Get("FEM_ON") : TheText.Get("FEM_OFF");
            break;
        default:
            break;
        }

        float buttonTextPosY = 0.0f;

        float buttonPosXScaled = 0.0f;
        float buttonPosYScaled = 0.0f;
        float buttonPosYFloat  = 0.0f;
        float buttonPosXFloat  = 0.0f;
        float drawDistanceSliderHeight;
        float drawDistanceSliderPosY;
        float drawDistanceSliderEdge;
        float drawDistanceSliderWidth;
        float drawDistanceSliderPosX;

        if (buttonText) {
            int currentScreen = m_nCurrentScreen;
            int buttonPosY    = aScreens[currentScreen].m_aItems[currentButtonIndex].m_Y;
            int buttonOffsetY = currentButtonIndex * 18 + currentScreen * 226;
            buttonPosYFloat   = buttonPosY;
            if (RsGlobal.maximumHeight == 448) {
                buttonPosYScaled = buttonPosYFloat;
            } else {
                buttonPosYScaled = RsGlobal.maximumHeight * buttonPosYFloat * 0.002232143;
            }
            float buttonPosXFloat = *(&aScreens[0].m_aItems[0].m_X + buttonOffsetY);
            if (RsGlobal.maximumWidth == 640) {
                buttonPosXScaled = buttonPosXFloat;
            } else {
                buttonPosXScaled = RsGlobal.maximumWidth * buttonPosXFloat * 0.0015625;
            }
            CFont::PrintString(buttonPosXScaled, buttonPosYScaled, buttonText);
        }
        if (prefSwitch) {
            CFont::SetFontStyle(FONT_MENU);
            CFont::SetEdge(1);
            CFont::SetOrientation(eFontAlignment::ALIGN_RIGHT);
            int  currentScreen = m_nCurrentScreen;
            char buttonType    = aScreens[currentScreen].m_aItems[currentButtonIndex].m_nType;
            if (buttonType < 1 || buttonType > 8) {
                if (currentScreen == SCREEN_AUDIO_SETTINGS && buttonIndex == 5) {
                    float audioSettingsFontHeight;
                    if (RsGlobal.maximumHeight == 448) {
                        audioSettingsFontHeight = 1.0;
                    } else {
                        audioSettingsFontHeight = RsGlobal.maximumHeight * 0.002232143;
                    }
                    float audioSettingsFontWidth;
                    if (RsGlobal.maximumWidth == 640) {
                        audioSettingsFontWidth = 0.56;
                        CFont::SetScale(0.56, audioSettingsFontHeight);
                    } else {
                        audioSettingsFontWidth = RsGlobal.maximumWidth * 0.00087500003;
                        CFont::SetScale(audioSettingsFontWidth, audioSettingsFontHeight);
                    }
                } else {
                    float defaultFontHeight;
                    if (RsGlobal.maximumHeight == 448) {
                        defaultFontHeight = 1.0;
                    } else {
                        defaultFontHeight = RsGlobal.maximumHeight * 0.002232143;
                    }
                    float defaultFontWidth;
                    if (RsGlobal.maximumWidth == 640) {
                        defaultFontWidth = 0.69999999;
                    } else {
                        defaultFontWidth = RsGlobal.maximumWidth * 0.00109375;
                    }
                    CFont::SetScale(defaultFontWidth, defaultFontHeight);
                }
            } else {
                float buttonFontHeight;
                if (RsGlobal.maximumHeight == 448) {
                    buttonFontHeight = 0.94999999;
                } else {
                    buttonFontHeight = RsGlobal.maximumHeight * 0.0021205356;
                }
                float buttonFontWidth;
                if (RsGlobal.maximumWidth == 640) {
                    buttonFontWidth = 0.34999999;
                    CFont::SetScale(0.34999999, buttonFontHeight);
                } else {
                    buttonFontWidth = RsGlobal.maximumWidth * 0.000546875;
                    CFont::SetScale(buttonFontWidth, buttonFontHeight);
                }
            }
            buttonPosYFloat = aScreens[m_nCurrentScreen].m_aItems[currentButtonIndex].m_Y;
            if (RsGlobal.maximumHeight == 448) {
                buttonPosYScaled = buttonPosYFloat;
            } else {
                buttonPosYScaled = RsGlobal.maximumHeight * buttonPosYFloat * 0.002232143;
            }
            float buttonPosXFloat;
            if (RsGlobal.maximumWidth == 640) {
                buttonPosXFloat = 40.0;
            } else {
                buttonPosXFloat = RsGlobal.maximumWidth * 0.0625;
            }
            buttonPosXScaled = RsGlobal.maximumWidth - buttonPosXFloat;
            CFont::PrintString(buttonPosXScaled, buttonPosYScaled, prefSwitch);
        }
        if (buttonText) {
            if (buttonIndex == m_nCurrentScreenItem) {
                int currentScreen = m_nCurrentScreen;
                if (currentScreen != SCREEN_MAP && currentScreen != SCREEN_BRIEF) {
                    if (!buttonPosXFloat) {
                        int buttonOffset = 226 * currentScreen + currentButtonIndex * 18;
                        if (*(&aScreens[0].m_aItems[0].m_nAlign + buttonOffset) == 1) {
                            float buttonPosXFloat  = *(&aScreens[0].m_aItems[0].m_X + buttonOffset) * (RsGlobal.maximumWidth * 0.0015625);
                            float buttonPosXScaled = buttonPosXFloat - CMenuManager::StretchX(40.0);
                        } else if (*(&aScreens[0].m_aItems[0].m_nAlign + buttonOffset) == 2) {
                            float buttonPosXScaled = CMenuManager::StretchX(40.0) + *(&aScreens[0].m_aItems[0].m_X + buttonOffset) * (RsGlobal.maximumWidth * 0.0015625);
                        } else {
                            float buttonPosXFloat  = *(&aScreens[0].m_aItems[0].m_X + buttonOffset) * (RsGlobal.maximumWidth * 0.0015625);
                            float buttonPosXScaled = buttonPosXFloat - CMenuManager::StretchX(40.0) - CFont::GetStringWidth(buttonText, 1, 0) * 0.5;
                        }
                    }
                    if (m_bMapLoaded) {
                        int currentScreen = m_nCurrentScreen;
                        if (currentScreen != SCREEN_LOAD_FIRST_SAVE && currentScreen != SCREEN_DELETE_FINISHED && currentScreen != SCREEN_SAVE_DONE_1) {
                            int   buttonPosY       = aScreens[currentScreen].m_aItems[currentButtonIndex].m_Y;
                            float buttonPosXScaled = buttonPosY;
                            CRect rect;
                            rect.left              = buttonPosXScaled;
                            float buttonPosYScaled = buttonPosY * (RsGlobal.maximumHeight * 0.002232143);
                            rect.top               = buttonPosYScaled - CMenuManager::StretchX(5.0);
                            rect.right             = CMenuManager::StretchX(32.0) + buttonPosXScaled;
                            rect.bottom            = CMenuManager::StretchX(47.0) + buttonPosYScaled;
                            m_aFrontEndSprites[m_nCurrentScreen].Draw(rect, CRGBA(0xFFu, 0xFFu, 0xFFu, 0xFFu));
                        }
                    }
                }
            }
        }
        if (m_nDisplayVideoMode == m_nPrefsVideoMode && !strcmp(aScreens[m_nCurrentScreen].m_aItems[m_nCurrentScreenItem].m_szName, "FED_RES") && m_nHelperText == FET_APP) {
            CMenuManager::ResetHelperText();
        }
        if (m_nDisplayAntialiasing == m_nPrefsAntialiasing && !strcmp(aScreens[m_nCurrentScreen].m_aItems[m_nCurrentScreenItem].m_szName, "FED_AAS") && m_nHelperText == FET_APP) {
            CMenuManager::ResetHelperText();
        }
        if (m_nDisplayVideoMode != m_nPrefsVideoMode && !strcmp(aScreens[m_nCurrentScreen].m_aItems[m_nCurrentScreenItem].m_szName, "FED_RES")) {
            CMenuManager::SetHelperText(eHelperText::FET_APP);
        }
        if (m_nDisplayAntialiasing != m_nPrefsAntialiasing && !strcmp(aScreens[m_nCurrentScreen].m_aItems[m_nCurrentScreenItem].m_szName, "FED_AAS")) {
            CMenuManager::SetHelperText(eHelperText::FET_APP);
        }
        m_nPrefsAntialiasing = m_nPrefsAntialiasing;
        if (m_nDisplayAntialiasing != m_nPrefsAntialiasing) {
            int currentScreen = m_nCurrentScreen;
            if (strcmp(aScreens[currentScreen].m_aItems[m_nCurrentScreenItem].m_szName, "FED_AAS")) {
                if (currentScreen == SCREEN_DISPLAY_SETTINGS || currentScreen == SCREEN_DISPLAY_ADVANCED) {
                    m_nDisplayAntialiasing = m_nPrefsAntialiasing;
                    CMenuManager::SetHelperText(eHelperText::FET_RSO);
                }
            }
        }
        m_nPrefsVideoMode = m_nPrefsVideoMode;
        if (m_nDisplayVideoMode != m_nPrefsVideoMode) {
            int currentScreen = m_nCurrentScreen;
            if (strcmp(aScreens[currentScreen].m_aItems[m_nCurrentScreenItem].m_szName, "FED_RES")) {
                if (currentScreen == SCREEN_DISPLAY_SETTINGS || currentScreen == SCREEN_DISPLAY_ADVANCED) {
                    m_nDisplayVideoMode = m_nPrefsVideoMode;
                    CMenuManager::SetHelperText(eHelperText::FET_RSO);
                }
            }
        }
        switch (aScreens[m_nCurrentScreen].m_aItems[currentButtonIndex].m_nActionType) {
        case 27:
            float sliderWidth;
            float sliderPosX;
            if (RsGlobal.maximumWidth == 640) {
                sliderWidth = 3.0;
                sliderPosX  = 100.0;
            } else {
                float sliderWidthScaled = RsGlobal.maximumWidth;
                sliderWidth             = sliderWidthScaled * 0.0046875002;
                sliderPosX              = sliderWidthScaled * 0.15625;
            }
            float sliderHeight;
            float sliderPosY;
            float sliderEdge;
            if (RsGlobal.maximumHeight == 448) {
                sliderHeight = 20.0;
                sliderEdge   = 4.0;
                sliderPosY   = 125.0;
            } else {
                float sliderHeightScaled = RsGlobal.maximumHeight;
                sliderHeight             = 0.044642858 * sliderHeightScaled;
                sliderEdge               = 0.0089285718 * sliderHeightScaled;
                sliderPosY               = sliderHeightScaled * 0.27901787;
            }
            float sliderMaxWidth;
            if (RsGlobal.maximumWidth == 640) {
                sliderMaxWidth = 500.0;
            } else {
                sliderMaxWidth = RsGlobal.maximumWidth * 0.78125;
            }
            CMenuManager::DisplaySlider(sliderMaxWidth, sliderPosY, sliderEdge, sliderHeight, sliderPosX, m_PrefsBrightness * 0.0026041667, sliderWidth);
            if (buttonIndex == m_nCurrentScreenItem && drawTitle) {
                float sliderHoverPosX  = sliderPosX;
                int   sliderHoverPosY  = CMenuManager::StretchY(150.0);
                int   sliderHoverEdge  = CMenuManager::StretchY(125.0);
                float sliderHoverWidth = sliderHoverPosX - CMenuManager::StretchX(3.0);
                if (!CMenuManager::CheckHover(0, sliderHoverWidth, sliderHoverEdge, sliderHoverPosY)) {
                    int   sliderHoverPosYScaled  = CMenuManager::StretchY(150.0);
                    int   sliderHoverEdgeScaled  = CMenuManager::StretchY(125.0);
                    float sliderHoverWidthScaled = RsGlobal.maximumWidth;
                    int   sliderHoverWidthMax    = CMenuManager::StretchX(sliderHoverWidthScaled);
                    float sliderHoverWidthMin    = CMenuManager::StretchX(3.0) + sliderHoverPosX;
                    if (!CMenuManager::CheckHover(sliderHoverWidthMin, sliderHoverWidthMax, sliderHoverEdgeScaled, sliderHoverPosYScaled)) {
                        m_MouseInBounds = 16;
                        break;
                    }
                    m_MouseInBounds            = 6;
                    float sliderMaxWidthScaled = CMenuManager::StretchX(500.0);
                    if (!(sliderMaxWidthScaled < m_nMousePosX) && !(sliderMaxWidthScaled == m_nMousePosX)) {
                        m_MouseInBounds = 16;
                        break;
                    }
                    m_nMousePosY = m_nMousePosY;
                    if (CMenuManager::StretchY(125.0) > m_nMousePosY) {
                        m_MouseInBounds = 16;
                        break;
                    }
                    float drawDistanceSliderHoverPosYMax = CMenuManager::StretchY(150.0);
                    if (drawDistanceSliderHoverPosYMax < m_nMousePosY) {
                        m_MouseInBounds = 16;
                        break;
                    }
                    break;
                }
                m_MouseInBounds = 7;
            }
            break;
        case 28: {
            float radioSliderWidth;
            float radioSliderPosX;
            if (RsGlobal.maximumWidth == 640) {
                radioSliderWidth = 3.0;
                radioSliderPosX  = 100.0;
            } else {
                float radioSliderWidthScaled = RsGlobal.maximumWidth;
                radioSliderWidth             = radioSliderWidthScaled * 0.0046875002;
                radioSliderPosX              = radioSliderWidthScaled * 0.15625;
            }
            float radioSliderHeight;
            float radioSliderPosY;
            float radioSliderEdge;
            if (RsGlobal.maximumHeight == 448) {
                radioSliderHeight = 20.0;
                radioSliderEdge   = 4.0;
                radioSliderPosY   = 95.0;
            } else {
                float radioSliderHeightScaled = RsGlobal.maximumHeight;
                radioSliderHeight             = 0.044642858 * radioSliderHeightScaled;
                radioSliderEdge               = 0.0089285718 * radioSliderHeightScaled;
                radioSliderPosY               = radioSliderHeightScaled * 0.21205357;
            }
            float radioSliderMaxWidth;
            if (RsGlobal.maximumWidth == 640) {
                radioSliderMaxWidth = 500.0;
            } else {
                radioSliderMaxWidth = RsGlobal.maximumWidth * 0.78125;
            }
            float radioSliderValue = m_nRadioVolume * 0.015625;
            CMenuManager::DisplaySlider(radioSliderMaxWidth, radioSliderPosY, radioSliderEdge, radioSliderHeight, radioSliderPosX, radioSliderValue, radioSliderWidth);
            if (buttonIndex == m_nCurrentScreenItem && drawTitle) {
                float radioSliderHoverPosX  = radioSliderPosX;
                int   radioSliderHoverPosY  = CMenuManager::StretchY(120.0);
                int   radioSliderHoverEdge  = CMenuManager::StretchY(95.0);
                float radioSliderHoverWidth = radioSliderHoverPosX - CMenuManager::StretchX(3.0);
                if (!CMenuManager::CheckHover(0, radioSliderHoverWidth, radioSliderHoverEdge, radioSliderHoverPosY)) {
                    int   radioSliderHoverPosYScaled  = CMenuManager::StretchY(120.0);
                    int   radioSliderHoverEdgeScaled  = CMenuManager::StretchY(95.0);
                    float radioSliderHoverWidthScaled = RsGlobal.maximumWidth;
                    int   radioSliderHoverWidthMax    = CMenuManager::StretchX(radioSliderHoverWidthScaled);
                    float radioSliderHoverWidthMin    = CMenuManager::StretchX(3.0) + radioSliderHoverPosX;
                    if (CMenuManager::CheckHover(radioSliderHoverWidthMin, radioSliderHoverWidthMax, radioSliderHoverEdgeScaled, radioSliderHoverPosYScaled)) {
                        m_MouseInBounds = 10;
                        if (CMenuManager::StretchX(500.0) <= m_nMousePosX) {
                            m_nMousePosY = m_nMousePosY;
                            if (CMenuManager::StretchY(95.0) <= m_nMousePosY) {
                                if (this->StretchY(120.0) < m_nMousePosY) {
                                    m_MouseInBounds = 16;
                                    break;
                                }
                            }
                        }
                    }
                    m_MouseInBounds = 16;
                    break;
                }
                m_MouseInBounds = 11;
            }
            break;
        }
        case 29:
            float sfxSliderWidth;
            float sfxSliderPosX;
            if (RsGlobal.maximumWidth == 640) {
                sfxSliderWidth = 3.0;
                sfxSliderPosX  = 100.0;
            } else {
                float sfxSliderWidthScaled = RsGlobal.maximumWidth;
                sfxSliderWidth             = sfxSliderWidthScaled * 0.0046875002;
                sfxSliderPosX              = sfxSliderWidthScaled * 0.15625;
            }
            float sfxSliderHeight;
            float sfxSliderPosY;
            float sfxSliderEdge;
            if (RsGlobal.maximumHeight == 448) {
                sfxSliderHeight = 20.0;
                sfxSliderEdge   = 4.0;
                sfxSliderPosY   = 125.0;
            } else {
                float sfxSliderHeightScaled = RsGlobal.maximumHeight;
                sfxSliderHeight             = 0.044642858 * sfxSliderHeightScaled;
                sfxSliderEdge               = 0.0089285718 * sfxSliderHeightScaled;
                sfxSliderPosY               = sfxSliderHeightScaled * 0.27901787;
            }
            float sfxSliderMaxWidth;
            if (RsGlobal.maximumWidth == 640) {
                sfxSliderMaxWidth = 500.0;
            } else {
                sfxSliderMaxWidth = RsGlobal.maximumWidth * 0.78125;
            }
            CMenuManager::DisplaySlider(sfxSliderMaxWidth, sfxSliderPosY, sfxSliderEdge, sfxSliderHeight, sfxSliderPosX, m_nSfxVolume * 0.015625, sfxSliderWidth);
            if (buttonIndex == m_nCurrentScreenItem && drawTitle) {
                float sfxSliderHoverPosX  = sfxSliderPosX;
                int   sfxSliderHoverPosY  = CMenuManager::StretchY(150.0);
                int   sfxSliderHoverEdge  = CMenuManager::StretchY(125.0);
                float sfxSliderHoverWidth = sfxSliderHoverPosX - CMenuManager::StretchX(3.0);
                if (!CMenuManager::CheckHover(0, sfxSliderHoverWidth, sfxSliderHoverEdge, sfxSliderHoverPosY)) {
                    int   sfxSliderHoverPosYScaled  = CMenuManager::StretchY(150.0);
                    int   sfxSliderHoverEdgeScaled  = CMenuManager::StretchY(125.0);
                    float sfxSliderHoverWidthScaled = RsGlobal.maximumWidth;
                    int   sfxSliderHoverWidthMax    = CMenuManager::StretchX(sfxSliderHoverWidthScaled);
                    float sfxSliderHoverWidthMin    = CMenuManager::StretchX(3.0) + sfxSliderHoverPosX;
                    if (!CMenuManager::CheckHover(sfxSliderHoverWidthMin, sfxSliderHoverWidthMax, sfxSliderHoverEdgeScaled, sfxSliderHoverPosYScaled)) {
                        m_MouseInBounds = 16;
                        break;
                    }
                    m_MouseInBounds                            = 12;
                    float sfxSliderMaxWidthScaled              = CMenuManager::StretchX(500.0);
                    bool  isMousePosXLessThanSfxSliderMaxWidth = sfxSliderMaxWidthScaled < m_nMousePosX;
                    bool  isMousePosXEqualToSfxSliderMaxWidth  = sfxSliderMaxWidthScaled == m_nMousePosX;
                    if (!isMousePosXEqualToSfxSliderMaxWidth && !isMousePosXLessThanSfxSliderMaxWidth) {
                        m_MouseInBounds = 16;
                        break;
                    }
                    m_nMousePosY = m_nMousePosY;
                    if (this->StretchY(125.0f) > m_nMousePosY) {
                        m_MouseInBounds = 16;
                        break;
                    }
                    float drawDistanceSliderHoverPosYMax = CMenuManager::StretchY(150.0);
                    if (drawDistanceSliderHoverPosYMax < m_nMousePosY) {
                        m_MouseInBounds = 16;
                        break;
                    }
                    break;
                }
                m_MouseInBounds = 13;
            }
            break;
        case 61: {
            if (RsGlobal.maximumWidth == 640) {
                drawDistanceSliderWidth = 3.0;
                drawDistanceSliderPosX  = 100.0;
            } else {
                float drawDistanceSliderWidthScaled = RsGlobal.maximumWidth;
                drawDistanceSliderWidth             = drawDistanceSliderWidthScaled * 0.0046875002;
                drawDistanceSliderPosX              = drawDistanceSliderWidthScaled * 0.15625;
            }
            if (RsGlobal.maximumHeight == 448) {
                drawDistanceSliderHeight = 20.0;
                drawDistanceSliderEdge   = 4.0;
                drawDistanceSliderPosY   = 125.0;
            } else {
                float drawDistanceSliderHeightScaled = RsGlobal.maximumHeight;
                drawDistanceSliderHeight             = 0.044642858 * drawDistanceSliderHeightScaled;
                drawDistanceSliderEdge               = 0.0089285718 * drawDistanceSliderHeightScaled;
                drawDistanceSliderPosY               = drawDistanceSliderHeightScaled * 0.27901787;
            }
            float drawDistanceSliderMaxWidth;
            if (RsGlobal.maximumWidth == 640) {
                drawDistanceSliderMaxWidth = 500.0;
            } else {
                drawDistanceSliderMaxWidth = RsGlobal.maximumWidth * 0.78125;
            }
            float drawDistanceSliderValue = (m_fDrawDistance - 0.92500001) * 1.1428572;
            CMenuManager::DisplaySlider(drawDistanceSliderMaxWidth, drawDistanceSliderPosY, drawDistanceSliderEdge, drawDistanceSliderHeight, drawDistanceSliderPosX, drawDistanceSliderValue, drawDistanceSliderWidth);
            if (buttonIndex == m_nCurrentScreenItem && drawTitle) {
                float drawDistanceSliderHoverPosX  = drawDistanceSliderPosX;
                float drawDistanceSliderHoverWidth = drawDistanceSliderHoverPosX - CMenuManager::StretchX(3.0);
                if (CMenuManager::CheckHover(0, drawDistanceSliderHoverWidth, CMenuManager::StretchY(125.0), CMenuManager::StretchY(150.0))) {
                    m_MouseInBounds = 9;
                } else {
                    int   drawDistanceSliderHoverPosYScaled  = CMenuManager::StretchY(150.0);
                    int   drawDistanceSliderHoverEdgeScaled  = CMenuManager::StretchY(125.0);
                    float drawDistanceSliderHoverWidthScaled = RsGlobal.maximumWidth;
                    int   drawDistanceSliderHoverWidthMax    = CMenuManager::StretchX(drawDistanceSliderHoverWidthScaled);
                    float drawDistanceSliderHoverWidthMin    = CMenuManager::StretchX(3.0) + drawDistanceSliderHoverPosX;
                    if (!CMenuManager::CheckHover(drawDistanceSliderHoverWidthMin, drawDistanceSliderHoverWidthMax, drawDistanceSliderHoverEdgeScaled, drawDistanceSliderHoverPosYScaled)) {
                        m_MouseInBounds = 16;
                        break;
                    }
                    m_MouseInBounds                        = 8;
                    float drawDistanceSliderMaxWidthScaled = CMenuManager::StretchX(500.0);
                    if (!(drawDistanceSliderMaxWidthScaled < m_nMousePosX) && !(drawDistanceSliderMaxWidthScaled == m_nMousePosX)) {
                        m_MouseInBounds = 16;
                        break;
                    }
                    m_nMousePosY = m_nMousePosY;
                    if (this->StretchY(125.0) > m_nMousePosY) {
                        m_MouseInBounds = 16;
                        break;
                    }
                    float drawDistanceSliderHoverPosYMax = CMenuManager::StretchY(150.0);
                    if (drawDistanceSliderHoverPosYMax < m_nMousePosY) {
                        m_MouseInBounds = 16;
                        break;
                    }
                }
            }
            break;
        }
        case 62: {
            float mouseSensitivitySliderWidth;
            float mouseSensitivitySliderPosX;
            if (RsGlobal.maximumWidth == 640) {
                mouseSensitivitySliderWidth = 3.0;
                mouseSensitivitySliderPosX  = 100.0;
            } else {
                float mouseSensitivitySliderWidthScaled = RsGlobal.maximumWidth;
                mouseSensitivitySliderWidth             = mouseSensitivitySliderWidthScaled * 0.0046875002;
                mouseSensitivitySliderPosX              = mouseSensitivitySliderWidthScaled * 0.15625;
            }
            float mouseSensitivitySliderHeight;
            float mouseSensitivitySliderPosY;
            float mouseSensitivitySliderEdge;
            if (RsGlobal.maximumHeight == 448) {
                mouseSensitivitySliderHeight = 20.0;
                mouseSensitivitySliderEdge   = 4.0;
                mouseSensitivitySliderPosY   = 125.0;
            } else {
                mouseSensitivitySliderHeight             = 0.044642858 * RsGlobal.maximumHeight;
                mouseSensitivitySliderEdge               = 0.0089285718 * RsGlobal.maximumHeight;
                mouseSensitivitySliderPosY               = RsGlobal.maximumHeight * 0.27901787;
            }
            float mouseSensitivitySliderMaxWidth;
            if (RsGlobal.maximumWidth == 640) {
                mouseSensitivitySliderMaxWidth = 500.0;
            } else {
                mouseSensitivitySliderMaxWidth = RsGlobal.maximumWidth * 0.78125;
            }
            float mouseSensitivitySliderValue = CCamera::m_fMouseAccelHorzntl / 0.0049999999;
            CMenuManager::DisplaySlider(mouseSensitivitySliderMaxWidth, mouseSensitivitySliderPosY, mouseSensitivitySliderEdge, mouseSensitivitySliderHeight, mouseSensitivitySliderPosX, mouseSensitivitySliderValue, mouseSensitivitySliderWidth);
            if (buttonIndex == m_nCurrentScreenItem && drawTitle) {
                if (CMenuManager::CheckHover(0, mouseSensitivitySliderPosX - CMenuManager::StretchX(3.0), CMenuManager::StretchY(125.0), CMenuManager::StretchY(150.0))) {
                    m_MouseInBounds = 15;
                } else {
                    m_MouseInBounds = 14;
                    if (!CMenuManager::CheckHover(CMenuManager::StretchX(3.0) + mouseSensitivitySliderPosX, CMenuManager::StretchX(RsGlobal.maximumWidth), CMenuManager::StretchY(125.0), CMenuManager::StretchY(150.0)) || CMenuManager::StretchX(500.0) > m_nMousePosX) {
                        m_MouseInBounds = 16;
                        break;
                    }
                }
            }
            break;
        }
        default:
            break;
        }
        switch (m_nCurrentScreen) {
        case 0:
        case 3:
        case 4:
        case 26:
        case 27:
        case 36:
        case 39:
        case 40:
            CMenuManager::DisplayHelperText(0);
            break;
        default:
            return;
        }
        if (buttonText) {
            buttonTextPosY = (29 * CFont::ProcessCurrentString(0, 60.0, buttonTextPosY, buttonText)) + buttonTextPosY;
        }
        screenOffset = 226 * m_nCurrentScreen;
        if (*(&aScreens[0].m_aItems[buttonIndex].m_nActionType + screenOffset) == 32) {
            buttonTextPosY = buttonTextPosY + 70.0;
        }
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
    m_bDrawMouse = false;
    CRect coords(0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT);
    CSprite2d::DrawRect(coords, { 0, 0, 0, 255 });
    SaveSettings();
    RsEventHandler(rsQUITAPP, nullptr);
}

// 0x57D8D0
void CMenuManager::DrawControllerScreenExtraText(int32 StartingYPos) {
    eControllerAction maxActions = eControllerAction::NUM_OF_MIN_CONTROLLER_ACTIONS;
    uint8 verticalSpacing;

    if (m_RedefiningControls == 1) {
        maxActions = eControllerAction::VEHICLE_BRAKE;
        verticalSpacing = 13;
    } else {
        if (m_ControlMethod) {
            verticalSpacing = 11;
            maxActions = eControllerAction::VEHICLE_RADIO_TRACK_SKIP;
        } else {
            verticalSpacing = 15;
            maxActions = eControllerAction::VEHICLE_STEER_UP;
        }
    }

    if (maxActions > 0) {
        float posX = 0.0f;
        
        for (auto actionIndex : std::views::iota(static_cast<int>(eControllerAction::NUM_OF_MIN_CONTROLLER_ACTIONS), static_cast<int>(maxActions))) {
            posX = StretchX(240.0f);
            float posY = StretchY(float(StartingYPos));
            
            for (const auto order : {eContSetOrder::FIRST, eContSetOrder::SECOND, eContSetOrder::THIRD, eContSetOrder::FOURTH}) {
                const auto buttonText = ControlsManager.GetControllerSettingText(static_cast<eControllerAction>(actionIndex), order);
                if (buttonText) {
                    CFont::PrintString(posX, posY, buttonText);
                    posX += StretchX(75.0f);
                }
            }
            
            if (static_cast<eControllerAction>(actionIndex) == m_ListSelection) {
                if (m_EditingControlOptions) {
                    if (CTimer::m_snTimeInMillisecondsPauseMode - FrontEndMenuManager.LastFlash > 150) {
                        FrontEndMenuManager.ColourSwitch = (FrontEndMenuManager.ColourSwitch) ? false : true;
                        FrontEndMenuManager.LastFlash  = CTimer::m_snTimeInMillisecondsPauseMode;
                    }
                    
                    if (FrontEndMenuManager.ColourSwitch) {
                        CFont::SetColor({0, 0, 0, 255});
                        CFont::PrintString(posX, posY, TheText.Get("FEC_QUE"));
                        CFont::SetColor({74, 90, 107, 255});
                    }
                }
            }
            
            StartingYPos += verticalSpacing;
        }
        
        /*/ Handle combo text display - Dummy function deprecated
        if (DEPRECATEDCOMBO) {
            auto comboText = CControllerConfigManager::GetButtonComboText(m_ListSelection);
            if (comboText) {
                CFont::SetColor({200, 50, 50, 255});
                CFont::PrintString(posX, StretchY(float(a2 + 10)), comboText);
                }
                }*/
            }
        }
        
// 0x57E6E0
void CMenuManager::DrawControllerBound(uint16 verticalOffset, bool isOppositeScreen) {
    int   controllerAction;
    int   actionIndex = 0;
    float currentY;
    int   currentX;
    bool  hasControl;

    const uint8 verticalSpacing = m_RedefiningControls ? 13 : (4 * ~m_ControlMethod + 11);
    const uint8 maxActions      = m_RedefiningControls ? 25 : (m_ControlMethod ? 28 : 22);

    struct ControlActionMapping {
        eControllerAction actionToTest;
        int controllerAction;
    };

    static constexpr ControlActionMapping CarActionMappings[] = {
        { eControllerAction::PED_CYCLE_WEAPON_RIGHT,            -1 },
        { eControllerAction::PED_CYCLE_WEAPON_LEFT,             -1 },
        { eControllerAction::CAMERA_CHANGE_VIEW_ALL_SITUATIONS, -1 },
        { eControllerAction::PED_JUMPING,                       -1 },
        { eControllerAction::PED_SPRINT,                        -1 },
        { eControllerAction::PED_LOOKBEHIND,                    -1 },
        { eControllerAction::PED_DUCK,                          -1 },
        { eControllerAction::VEHICLE_STEER_DOWN,                -1 },
        { eControllerAction::VEHICLE_ACCELERATE,                -1 },
        { eControllerAction::VEHICLE_RADIO_STATION_UP,          -1 },
        { eControllerAction::VEHICLE_RADIO_STATION_DOWN,        -1 },
        { eControllerAction::VEHICLE_RADIO_TRACK_SKIP,          -1 },
        { eControllerAction::VEHICLE_HORN,                      -1 },
        { eControllerAction::VEHICLE_TURRETLEFT,                -1 },
        { eControllerAction::VEHICLE_TURRETRIGHT,               -1 },
        { eControllerAction::PED_CYCLE_TARGET_LEFT,             -1 },
        { eControllerAction::PED_FIRE_WEAPON,                   18 },
        { eControllerAction::PED_FIRE_WEAPON_ALT,               19 },
        { eControllerAction::GO_FORWARD,                        24 },
        { eControllerAction::GO_BACK,                           25 },
        { eControllerAction::GO_LEFT,                           20 },
        { eControllerAction::GO_RIGHT,                          21 },
        { eControllerAction::PED_SNIPER_ZOOM_IN,                22 },
        { eControllerAction::PED_SNIPER_ZOOM_OUT,               23 },
        { eControllerAction::VEHICLE_ENTER_EXIT,                47 },
        { eControllerAction::PED_ANSWER_PHONE,                  10 },
        { eControllerAction::PED_WALK,                          26 },
        { eControllerAction::VEHICLE_FIRE_WEAPON,               27 },
        { eControllerAction::VEHICLE_FIRE_WEAPON_ALT,           28 },
        { eControllerAction::VEHICLE_STEER_LEFT,                29 },
        { eControllerAction::VEHICLE_STEER_RIGHT,               30 },
        { eControllerAction::VEHICLE_STEER_UP,                  11 },
        { eControllerAction::VEHICLE_BRAKE,                     31 },
        { eControllerAction::TOGGLE_SUBMISSIONS,                38 },
        { eControllerAction::VEHICLE_HANDBRAKE,                 39 },
        { eControllerAction::PED_1RST_PERSON_LOOK_LEFT,         41 },
        { eControllerAction::PED_1RST_PERSON_LOOK_RIGHT,        40 },
        { eControllerAction::VEHICLE_LOOKLEFT,                  36 },
        { eControllerAction::VEHICLE_LOOKRIGHT,                 37 },
        { eControllerAction::VEHICLE_LOOKBEHIND,                34 },
        { eControllerAction::VEHICLE_MOUSELOOK,                 35 },
    };

    static constexpr ControlActionMapping PedActionMappings[] = {
        { eControllerAction::PED_FIRE_WEAPON,                   0  },
        { eControllerAction::VEHICLE_RADIO_TRACK_SKIP,          0  },
        { eControllerAction::PED_FIRE_WEAPON_ALT,               2  },
        { eControllerAction::PED_CYCLE_WEAPON_RIGHT,            3  },
        { eControllerAction::PED_CYCLE_WEAPON_LEFT,             49 },
        { eControllerAction::GO_FORWARD,                        50 },
        { eControllerAction::GO_BACK,                           48 },
        { eControllerAction::GO_LEFT,                           47 },
        { eControllerAction::VEHICLE_MOUSELOOK,                 47 },
        { eControllerAction::GO_RIGHT,                          4  },
        { eControllerAction::TOGGLE_SUBMISSIONS,                4  },
        { eControllerAction::PED_SNIPER_ZOOM_IN,                5  },
        { eControllerAction::VEHICLE_HANDBRAKE,                 5  },
        { eControllerAction::PED_SNIPER_ZOOM_OUT,               6  },
        { eControllerAction::PED_1RST_PERSON_LOOK_LEFT,         6  },
        { eControllerAction::VEHICLE_ENTER_EXIT,                7  },
        { eControllerAction::PED_1RST_PERSON_LOOK_RIGHT,        7  },
        { eControllerAction::CAMERA_CHANGE_VIEW_ALL_SITUATIONS, 8  },
        { eControllerAction::PED_JUMPING,                       9  },
        { eControllerAction::PED_SPRINT,                        10 },
        { eControllerAction::VEHICLE_LOOKBEHIND,                10 },
        { eControllerAction::PED_LOOKBEHIND,                    11 },
        { eControllerAction::PED_CYCLE_TARGET_RIGHT,            11 },
        { eControllerAction::PED_DUCK,                          12 },
        { eControllerAction::PED_ANSWER_PHONE,                  13 },
        { eControllerAction::PED_WALK,                          45 },
        { eControllerAction::VEHICLE_FIRE_WEAPON,               15 },
        { eControllerAction::VEHICLE_FIRE_WEAPON_ALT,           16 },
        { eControllerAction::VEHICLE_STEER_UP,                  32 },
        { eControllerAction::CONVERSATION_YES,                  32 },
        { eControllerAction::VEHICLE_STEER_DOWN,                3  },
        { eControllerAction::CONVERSATION_NO,                   33 },
        { eControllerAction::VEHICLE_TURRETLEFT,                -1 },
        { eControllerAction::VEHICLE_TURRETRIGHT,               -1 },
        { eControllerAction::VEHICLE_TURRETUP,                  -1 },
        { eControllerAction::VEHICLE_TURRETDOWN,                -1 },
        { eControllerAction::PED_CYCLE_TARGET_LEFT,             -1 },
        { eControllerAction::PED_CENTER_CAMERA_BEHIND_PLAYER,   -1 },
        { eControllerAction::NETWORK_TALK,                      -1 },
        { eControllerAction::GROUP_CONTROL_FWD,                 -1 },
        { eControllerAction::GROUP_CONTROL_BWD,                 -1 },
        { eControllerAction::PED_1RST_PERSON_LOOK_UP,           -1 },
        { eControllerAction::PED_1RST_PERSON_LOOK_DOWN,         -1 },
        { eControllerAction::VEHICLE_RADIO_STATION_DOWN,        1  },
        { eControllerAction::VEHICLE_HORN,                      1  },
        { eControllerAction::VEHICLE_RADIO_STATION_UP,          44 },
        { eControllerAction::VEHICLE_BRAKE,                     52 },
        { eControllerAction::VEHICLE_ACCELERATE,                51 },
        { eControllerAction::VEHICLE_STEER_LEFT,                17 },
        { eControllerAction::VEHICLE_STEER_RIGHT,               14 },
        { eControllerAction::PED_LOCK_TARGET,                   14 },
    };

    currentY = StretchY(float(verticalOffset));

    // Main loop - process each action
    while (actionIndex < maxActions) {
        currentX         = StretchX(270.0f);
        hasControl       = false;
        controllerAction = 0;

        // Set default text color
        CFont::SetColor({ 255, 255, 255, 255 });

        // Map action index to controller action
        if (m_RedefiningControls == 1) {
            for (auto& mapping : CarActionMappings) {
                if (mapping.actionToTest == ControllerActionsAvailableInCar[actionIndex]) {
                    controllerAction = mapping.controllerAction;
                    break;
                }
            }
        } else {
            for (auto& mapping : PedActionMappings) {
                if (mapping.actionToTest == actionIndex) {
                    controllerAction = !(m_ControlMethod == 0 && notsa::contains({ eControllerAction::VEHICLE_STEER_UP, eControllerAction::CONVERSATION_YES, eControllerAction::VEHICLE_STEER_DOWN, eControllerAction::CONVERSATION_NO }, mapping.actionToTest)) ? mapping.controllerAction : -1;
                    break;
                }
            }
        }

        // Highlight selected action
        if (m_ListSelection == actionIndex && !isOppositeScreen) {
            CSprite2d::DrawRect(
                CRect(
                    StretchX(260.0f),
                    StretchY(actionIndex * verticalSpacing + verticalOffset + 1.0f),
                    SCREEN_WIDTH - StretchX(20.0f),
                    StretchY(actionIndex * verticalSpacing + verticalOffset + 1.0f + 10.0f)
                ),
                { 172, 203, 241, 255 }
            );
            CFont::SetColor({ 255, 255, 255, 255 });
        }

        // Set text properties
        CFont::SetOrientation(eFontAlignment::ALIGN_LEFT);
        CFont::SetScale(StretchX(0.3f), StretchY(0.6f));
        CFont::SetFontStyle(FONT_SUBTITLES);
        CFont::SetWrapx(StretchX(100.0f) + SCREEN_WIDTH);

        // Draw control bindings
        int controlOrderIndex = 1;
        hasControl = false;
        if (controllerAction != -1) {
            while (controlOrderIndex <= 4) {
                if (m_DeleteAllNextDefine && m_ListSelection == actionIndex) {
                    break;
                }
                const auto buttonText = ControlsManager.GetControllerSettingText((eControllerAction)controllerAction, (eContSetOrder)controlOrderIndex);
                if (buttonText) {
                    if (!isOppositeScreen) {
                        CFont::PrintString(currentX, currentY, buttonText);
                    }
                    currentX += StretchX(75.0f);
                    hasControl = true;
                }
                controlOrderIndex++;
            }
        }

        // Handle unbound controls or special cases
        if (!hasControl) {
            if (controllerAction != -2) {
                m_bRadioAvailable = 0;
                CFont::SetColor({ 200, 50, 50, 255 });
                if (!isOppositeScreen) {
                    CFont::PrintString(currentX, currentY, TheText.Get("FEC_UNB"));
                }
            } else {
                CFont::SetColor({ 0, 0, 0, 255 });
                if (!isOppositeScreen) {
                    CFont::PrintString(currentX, currentY, TheText.Get("FEC_CMP"));
                }
            }
        }

        // Handle selected action special behaviors
        if (actionIndex == m_ListSelection) {
            if (controllerAction == -1 || controllerAction == -2) {
                if (actionIndex == m_ListSelection) {
                    DisplayHelperText("FET_EIG");
                }
            } else {
                m_OptionToChange = (eControllerAction)controllerAction;
                if (m_EditingControlOptions) {
                    if (CTimer::m_snTimeInMillisecondsPauseMode - m_lastBlinkTime > 150) {
                        m_isTextBlinking = !m_isTextBlinking;
                        m_lastBlinkTime  = CTimer::m_snTimeInMillisecondsPauseMode;
                    }

                    if (m_isTextBlinking) {
                        CFont::SetColor({ 0, 0, 0, 255 });
                        if (!isOppositeScreen) {
                            CFont::PrintString(currentX, currentY, TheText.Get("FEC_QUE"));
                        }
                    }

                    if (m_DeleteAllBoundControls) {
                        DisplayHelperText("FET_CIG");
                    } else {
                        DisplayHelperText("FET_RIG");
                    }
                    m_CanBeDefined = true;
                } else {
                    DisplayHelperText("FET_CIG");
                    m_CanBeDefined = false;
                    m_DeleteAllBoundControls = false;
                }
            }
        }

        // Move to next line and action
        currentY = StretchY(float(verticalOffset + (actionIndex + 1) * verticalSpacing));
        actionIndex++;
    }
}

// 0x57F300
void CMenuManager::DrawControllerSetupScreen() {
    // Calculate spacing and max items based on control scheme
    const auto verticalSpacing = m_RedefiningControls ? 13u : (4u * (!m_ControlMethod) + 11u);
    const auto maxControlActions = m_RedefiningControls ? 25u : (m_ControlMethod ? 28u : 22u);

    const char* keys[]{
        "FEC_FIR", "FEC_FIA", "FEC_NWE", "FEC_PWE", m_RedefiningControls ? "FEC_ACC" : "FEC_FOR", m_RedefiningControls ? "FEC_BRA" : "FEC_BAC", "FEC_LEF", "FEC_RIG", "FEC_PLU", "FEC_PLD", m_RedefiningControls ? "FEC_TSK" : "FEC_COY", "FEC_CON", "FEC_GPF", "FEC_GPB", "FEC_ZIN", "FEC_ZOT", "FEC_EEX", "FEC_RSC", "FEC_RSP", "FEC_RTS", "FEC_HRN", "FEC_SUB", "FEC_CMR", "FEC_JMP", "FEC_SPN", "FEC_HND", "FEC_TAR", "FEC_CRO", "FEC_ANS", "FEC_PDW", "FEC_TFL", "FEC_TFR", "FEC_TFU", "FEC_TFD", "FEC_LBA", "FEC_VML", "FEC_LOL", "FEC_LOR", "FEC_LDU", "FEC_LUD", "", "", "FEC_CEN", nullptr
    };

    // 0x57F68E
    CFont::SetFontStyle(FONT_GOTHIC);
    CFont::SetScale(StretchX(0.9f), StretchY(1.7f));
    CFont::SetEdge(0);
    CFont::SetColor(HudColour.GetRGB(HUD_COLOUR_LIGHT_BLUE));
    CFont::SetOrientation(eFontAlignment::ALIGN_RIGHT);
    CFont::PrintString(
        SCREEN_WIDTH - StretchX(48.0f), StretchY(11.0f), TheText.Get(m_ControlMethod ? "FET_CCN" : "FET_SCN")
    );
    CFont::SetOrientation(eFontAlignment::ALIGN_LEFT);
    CFont::PrintString(
        StretchX(48.0f), StretchY(11.0f), TheText.Get(m_RedefiningControls ? "FET_CCR" : "FET_CFT")
    );
    CSprite2d::DrawRect({ StretchX(20.0f), StretchY(50.0f), SCREEN_WIDTH - StretchX(20.0f), SCREEN_HEIGHT - StretchY(50.0f) }, { 49, 101, 148, 100 });

    // 0x57F8C0
    for (auto i = 0u; i < maxControlActions; i++) {
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
        CFont::PrintString(StretchX(40.0f), StretchY(i * verticalSpacing + 69.0f), TheText.Get(keys[m_RedefiningControls ? ControllerActionsAvailableInCar[i] : ControllerActionsAvailableOnFoot[i]]));
    }

    // 0x57FAF9
    DrawControllerBound(0x45u, false);
    if (!m_EditingControlOptions) {
        CFont::SetScale(StretchX(0.7f), StretchY(1.0f));
        const auto color = StretchX(
            CFont::GetStringWidth(TheText.Get("FEDS_TB"), true, false)
        );
        if (StretchX(35.0f) + color <= m_nMousePosX
            || StretchX(15.0f) >= m_nMousePosX
            || SCREEN_HEIGHT - StretchY(33.0f) >= m_nMousePosY
            || SCREEN_HEIGHT - StretchY(10.0f) <= m_nMousePosY) {
            if (StretchX(20.0f) >= m_nMousePosX
                || StretchX(600.0f) <= m_nMousePosX
                || StretchY(48.0f) >= m_nMousePosY
                || SCREEN_HEIGHT - StretchY(33.0f) <= m_nMousePosY) {
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
    CFont::PrintString(
        StretchX(33.0f),
        SCREEN_HEIGHT - StretchY(38.0f),
        TheText.Get("FEDS_TB")
    );
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

    const auto BARS = 16;
    const auto COLOR_ON  = CRGBA(172, 203, 241, 255); // Fresh Air
    const auto COLOR_OFF = CRGBA(74, 90, 107, 255);   // Deep Space Sparkle
    const auto COLOR_SHADOW = CRGBA(0, 0, 0, 200);

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
        CSprite2d::DrawRect(CRect(left + StretchX(2.0f), top + StretchY(2.0f), right + StretchX(2.0f), bottom + StretchY(2.0f)), COLOR_SHADOW);
        CSprite2d::DrawRect(CRect(left, top, right, bottom), color);
    }
    return lastActiveBarX;
}
