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

// TODO: Rename
static uint32 dword_865598[] = {0,2,3,12,13,11,10,4,5,6,7,14,15,16,22,23,24,26,27,28,29,34,36,37,38}; // 0x865598
static uint32 dword_865608[] = {0,1,4,5,6,7,8,9,16,10,17,18,19,20,21,22,25,34,35,36,37,30,31,32,33}; // 0x865608
static GxtChar dword_86539C[] = "FEM_ON"; // 0x86539C

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

        if (m_nCurrentScreen == SCREEN_MAP && CPad::NewMouseControllerState.lmb && mapRect.IsPointInside(CVector2D(x, y))) {
            DrawCursor(FRONTEND_SPRITE_CROSS_HAIR);
        } else {
            DrawCursor(FRONTEND_SPRITE_MOUSE);
        }
    }
}

// 0x5794A0
void CMenuManager::DrawStandardMenus(uint8 a1) {
    plugin::CallMethod<0x5794A0, CMenuManager*, uint8>(this, a1);
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
void CMenuManager::DrawControllerScreenExtraText(int32 a1) {
    plugin::CallMethod<0x57D8D0, CMenuManager*, int32>(this, a1);
}

// 0x57E6E0
void CMenuManager::DrawControllerBound(uint16 verticalOffset, bool isRedefining) {
    // TODO: Magic number
    const auto verticalSpacing = m_RedefiningControls ? 13u : (4u * !m_bController + 11u);
    const auto maxActions      = m_RedefiningControls ? 25u : (m_bController ? 28u : 22u);

    const auto getControllerAction = [&](int32 index) -> eControllerAction {
        if (m_RedefiningControls) {
            switch (dword_865608[index]) {
            case 0: return CA_VEHICLE_FIRE_WEAPON;
            case 1: return CA_VEHICLE_FIRE_WEAPON_ALT;
            case 4: return CA_VEHICLE_ACCELERATE;
            case 5: return CA_VEHICLE_BRAKE;
            case 6: return CA_VEHICLE_STEER_LEFT;
            case 7: return CA_VEHICLE_STEER_RIGHT;
            case 8: return CA_VEHICLE_STEER_UP;
            case 9: return CA_VEHICLE_STEER_DOWN;
            case 10: return CA_CONVERSATION_YES;
            case 16: return CA_VEHICLE_ENTER_EXIT;
            case 17: return CA_VEHICLE_RADIO_STATION_UP;
            case 18: return CA_VEHICLE_RADIO_STATION_DOWN;
            case 19: return CA_VEHICLE_RADIO_TRACK_SKIP;
            case 20: return CA_VEHICLE_HORN;
            case 21: return CA_TOGGLE_SUBMISSIONS;
            case 22: return CA_CAMERA_CHANGE_VIEW_ALL_SITUATIONS;
            case 25: return CA_VEHICLE_HANDBRAKE;
            case 30: return CA_VEHICLE_TURRET_LEFT;
            case 31: return CA_VEHICLE_TURRET_RIGHT;
            case 32: return CA_VEHICLE_TURRET_DOWN;
            case 33: return CA_VEHICLE_TURRET_UP;
            case 34: return CA_VEHICLE_LOOKBEHIND;
            case 35: return CA_VEHICLE_MOUSE_LOOK;
            case 36: return CA_VEHICLE_LOOK_LEFT;
            case 37: return CA_VEHICLE_LOOK_RIGHT;
            default: return CA_INVALID;
            }
        } else {
            switch (index) {
            case 0:
            case 28: return CA_PED_FIRE_WEAPON;
            case 1: return CA_PED_CYCLE_WEAPON_RIGHT;
            case 2: return CA_PED_CYCLE_WEAPON_LEFT;
            case 3: return CA_GROUP_CONTROL_FWD;
            case 4: return CA_GROUP_CONTROL_BWD;
            case 5: return CA_CONVERSATION_NO;
            case 6:
            case 37: return CA_CONVERSATION_YES;
            case 7:
            case 30: return CA_GO_FORWARD;
            case 8:
            case 31: return CA_GO_BACK;
            case 9:
            case 32: return CA_GO_LEFT;
            case 10:
            case 33: return CA_GO_RIGHT;
            case 11: return CA_PED_SNIPER_ZOOM_IN;
            case 12: return CA_PED_SNIPER_ZOOM_OUT;
            case 13:
            case 36: return CA_VEHICLE_ENTER_EXIT;
            case 14:
            case 43: return CA_CAMERA_CHANGE_VIEW_ALL_SITUATIONS;
            case 15: return CA_PED_JUMPING;
            case 16: return CA_PED_SPRINT;
            case 17: return CA_PED_LOCK_TARGET;
            case 18: return CA_PED_DUCK;
            case 19: return CA_PED_ANSWER_PHONE;
            case 20: return CA_PED_WALK;
            case 21:
            case 45: return CA_PED_LOOKBEHIND;
            case 22:
            case 47: return m_bController ? CA_PED_1RST_PERSON_LOOK_LEFT : CA_INVALID;
            case 23:
            case 48: return m_bController ? CA_PED_1RST_PERSON_LOOK_RIGHT : CA_INVALID;
            case 24: return CA_PED_1RST_PERSON_LOOK_UP;
            case 25: return CA_PED_1RST_PERSON_LOOK_DOWN;
            case 26: return CA_PED_CENTER_CAMERA_BEHIND_PLAYER;
            case 27:
            case 29: return CA_PED_FIRE_WEAPON_ALT;
            default: return CA_INVALID;
            }
        }
    };

    auto currentY = StretchY(verticalOffset);
    for (auto actionIndex = 0u; actionIndex < maxActions; ++actionIndex) {
        auto currentX = StretchX(270.0f);
        CFont::SetColor({ 255, 255, 255, 255 });
        const auto controllerAction = getControllerAction(actionIndex);

        // 0x57EA1E
        if (m_ListSelection == actionIndex && !isRedefining) {
            CSprite2d::DrawRect({ StretchX(260.0f), StretchY(actionIndex * verticalSpacing + verticalOffset + 1.0f), SCREEN_WIDTH - StretchX(20.0f), StretchY((actionIndex * verticalSpacing + verticalOffset + 1.0f) + 10.0f) }, { 172, 203, 241, 255 });
            CFont::SetColor({ 255, 255, 255, 255 });
        }

        // 0x57EB09
        CFont::SetOrientation(eFontAlignment::ALIGN_LEFT);
        CFont::SetScale(StretchX(0.3f), StretchY(0.6f));
        CFont::SetFontStyle(FONT_SUBTITLES);
        CFont::SetWrapx(StretchX(100.0f) + SCREEN_WIDTH);

        bool isControlPrinted = false;
        if (controllerAction != CA_INVALID) {
            for (auto priority = 1u; priority < PRIORITY_NUM; ++priority) {
                if (m_DeleteAllNextDefine && m_ListSelection == actionIndex) {
                    break;
                }

                const auto buttonText = ControlsManager.GetControllerSettingText(controllerAction, (ePriority)priority);
                if (buttonText) {
                    if (!isRedefining) {
                        CFont::PrintString(currentX, currentY, buttonText);
                    }
                    currentX += StretchX(75.0f);
                    isControlPrinted = true;
                }
            }
        }

        if (!isControlPrinted) {
            if (controllerAction != static_cast<eControllerAction>(-2)) {
                m_bRadioAvailable = 0;
                CFont::SetColor({ 200, 50, 50, 255 });

                if (!isRedefining) {
                    CFont::PrintString(currentX, currentY, TheText.Get("FEC_UNB"));
                }
            } else {
                CFont::SetColor({ 0, 0, 0, 255 });
                if (!isRedefining) {
                    CFont::PrintString(currentX, currentY, TheText.Get("FEC_CMP"));
                }
            }
        }

        if (actionIndex == m_ListSelection) {
            if (controllerAction == CA_INVALID || controllerAction == static_cast<eControllerAction>(-2)) {
                if (actionIndex == m_ListSelection) {
                    CMenuManager::DisplayHelperText("FET_EIG");
                }
            } else {
                m_OptionToChange = controllerAction;
                if (m_EditingControlOptions) {
                    if (CTimer::m_snTimeInMillisecondsPauseMode - m_lastBlinkTime > 150) {
                        m_isTextBlinking = !m_isTextBlinking;
                        m_lastBlinkTime  = CTimer::m_snTimeInMillisecondsPauseMode;
                    }
                    if (m_isTextBlinking) {
                        CFont::SetColor({ 0, 0, 0, 255 });
                        if (!isRedefining) {
                            CFont::PrintString(currentX, currentY, TheText.Get("FEC_QUE"));
                        }
                    }
                    if (m_DeleteAllBoundControls) {
                        CMenuManager::DisplayHelperText("FET_CIG");
                    } else {
                        CMenuManager::DisplayHelperText("FET_RIG");
                    }
                    m_CanBeDefined = true;
                } else {
                    CMenuManager::DisplayHelperText("FET_CIG");
                    m_CanBeDefined           = false;
                    m_DeleteAllBoundControls = false;
                }
            }
        }
        currentY = StretchY(float(verticalOffset + (actionIndex + 1) * verticalSpacing));
    }
}

// 0x57F300
void CMenuManager::DrawControllerSetupScreen() {
    // TODO: Magic number
    const auto v51 = m_RedefiningControls ? 13u : (4u * (!m_bController) + 11u);
    const auto max = m_RedefiningControls ? 25u : (m_bController ? 28u : 22u);

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
        SCREEN_WIDTH - StretchX(48.0f), StretchY(11.0f), TheText.Get(m_bController ? "FET_CCN" : "FET_SCN")
    );
    CFont::SetOrientation(eFontAlignment::ALIGN_LEFT);
    CFont::PrintString(
        StretchX(48.0f), StretchY(11.0f), TheText.Get(m_RedefiningControls ? "FET_CCR" : "FET_CFT")
    );
    CSprite2d::DrawRect({ StretchX(20.0f), StretchY(50.0f), SCREEN_WIDTH - StretchX(20.0f), SCREEN_HEIGHT - StretchY(50.0f) }, { 49, 101, 148, 100 });

    // 0x57F8C0
    for (auto i = 0u; i < max; i++) {
        if (!m_EditingControlOptions) {
            if (StretchX(20.0f) < m_nMousePosX && StretchX(600.0f) > m_nMousePosX) {
                if (StretchY(i * v51 + 69.0f) < m_nMousePosY && StretchY(v51 * (i + 1) + 69.0f) > m_nMousePosY) {
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
        CFont::PrintString(
            StretchX(40.0f), StretchY(i * v51 + 69.0f), TheText.Get(keys[m_RedefiningControls ? dword_865608[i] : dword_865598[i]])
        );
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
