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
            rect.right  = x + CMenuManager::StretchX(24.0f);
            rect.top = y + SCREEN_SCALE_Y(21.0f);
            m_aFrontEndSprites[spriteId].Draw(rect, { 100, 100, 100, 50 }); // shadow

            rect.left   = x;
            rect.bottom    = y;
            rect.right  = x + CMenuManager::StretchX(18.0f);
            rect.top = y + SCREEN_SCALE_Y(18.0f);
            m_aFrontEndSprites[spriteId].Draw(rect, { 255, 255, 255, 255 });
        };

        CRect mapRect(StretchX(60.0f), StretchY(60.0f), SCREEN_WIDTH - StretchX(60.0f), SCREEN_HEIGHT - StretchY(60.0f));

        if (m_nCurrentScreen == SCREEN_MAP && CPad::NewMouseControllerState.isMouseLeftButtonPressed && mapRect.IsPointInside(CVector2D(x, y))) {
            DrawCursor(FRONTEND_SPRITE_CROSS_HAIR);
        } else {
            DrawCursor(FRONTEND_SPRITE_MOUSE);
        }
    }
}

// 0x5794A0
enum eMenuPage : __int32
{
  MENUPAGE_STATUS = 0x0,
  MENUPAGE_LOADING_GAME = 0x1,
  MENUPAGE_RESUME = 0x2,
  MENUPAGE_SOUND_SETUP = 0x3,
  MENUPAGE_DISPLAY_SETUP = 0x4,
  MENUPAGE_MAP = 0x5,
  MENUPAGE_NEW_GAME_ASK = 0x6,
  MENUPAGE_SELECT_GAME = 0x7,
  MENUPAGE_MISSIONPACK_LOADING_ASK = 0x8,
  MENUPAGE_LOAD_GAME = 0x9,
  MENUPAGE_DELETE_GAME = 0xA,
  MENUPAGE_LOAD_SAVE_ASK = 0xB,
  MENUPAGE_DELETE_SAVE_ASK = 0xC,
  MENUPAGE_LOAD_FIRST_SAVE = 0xD,
  MENUPAGE_DELETE_FINISHED_1 = 0xE,
  MENUPAGE_DELETE_FINISHED_2 = 0xF,
  MENUPAGE_GAME_SAVE = 0x10,
  MENUPAGE_SAVE_WRITE_ASK = 0x11,
  MENUPAGE_SAVE_DONE_1 = 0x12,
  MENUPAGE_SAVE_DONE_2 = 0x13,
  MENUPAGE_GAME_SAVED = 0x14,
  MENUPAGE_GAME_LOADED = 0x15,
  MENUPAGE_GAME_WARNING_DONT_SAVE = 0x16,
  MENUPAGE_SHOW_DEFAULT_SETS = 0x17,
  MENUPAGE_AUDIO_SETS_DEFAULT = 0x18,
  MENUPAGE_CONTROLLER_SETS_DEFAULT = 0x19,
  MENUPAGE_USER_TRACKS = 0x1A,
  MENUPAGE_DISPLAY_ADVANCED = 0x1B,
  MENUPAGE_LANGUAGE = 0x1C,
  MENUPAGE_SAVE_GAME_DONE = 0x1D,
  MENUPAGE_SAVE_GAME_FAILED = 0x1E,
  MENUPAGE_SAVE_WRITE_FAILED = 0x1F,
  MENUPAGE_SAVE_FAILED_FILE_ERROR = 0x20,
  MENUPAGE_OPTIONS = 0x21,
  MENUPAGE_MAIN_MENU = 0x22,
  MENUPAGE_QUIT_GAME = 0x23,
  MENUPAGE_CONTROLLER_SETUP = 0x24,
  MENUPAGE_REDEFINE_CONTROLS = 0x25,
  MENUPAGE_CONTROLS_VEHICLE_ONFOOT = 0x26,
  MENUPAGE_MOUSE_SETUP = 0x27,
  MENUPAGE_JOYPAD_SETUP = 0x28,
  MENUPAGE_MAIN_MENU_CONTROLS = 0x29,
  MENUPAGE_QUIT_GAME_2 = 0x2A,
  MENUPAGE_EMPTY = 0x2B,
};

void CMenuManager::DrawStandardMenus(bool drawTitle) {

    constexpr float MENU_DEFAULT_CONTENT_X = 320.0f;
    constexpr float MENU_DEFAULT_CONTENT_Y = 130.0f;

    eSlotState v10;                      // ecx
    int i;                      // edi
    GxtChar* v19;                                       // edx
    int v21;                      // eax
    const GxtChar* v24;                    // eax
    __int32 v29;                  // eax
    const GxtChar *pTextToShow_RightColumn;                    // eax
    const GxtChar **VideoModeList;         // eax
    int v37;                      // eax
    char m_nType;                 // al
    double v48;                   // st7
    double v49;                   // st7
    int v51;                      // eax
    double v52;                   // st7
    double v53;                   // st7
    int v54 = 0; //// ?????                      // eax
    CRGBA *v56;                   // eax
    eMenuScreen v58;              // dl
    eMenuScreen v60;              // dl
    double v61;                   // st7
    double v62;                   // st7
    double maximumHeight;         // st6
    int v64;                      // eax
    double v65;                   // st7
    double v66;                   // st7
    double v68;                   // st7
    bool v69;                     // c0
    bool v70;                     // c3
    double v71;                   // st7
    double v72;                   // st7
    double v73;                   // st6
    int v74;                      // eax
    double v75;                   // st7
    double v76;                   // st7
    double v77;                   // st7
    double v78;                   // st7
    double v79;                   // st7
    double v80;                   // st7
    double v81;                   // st6
    int v82;                      // eax
    double v83;                   // st7
    double v84;                   // st7
    double v85;                   // st7
    double v86;                   // st7
    double v87;                   // st7
    double v88;                   // st7
    double v89;                   // st6
    //int v155;                      // eax
    double v91;                   // st7
    double v92;                   // st7
    double v93;                   // st7
    double v94;                   // st7
    double v95;                   // st7
    double v96;                   // st7
    double v97;                   // st6
    //int v155;                      // eax
    double v99;                   // st7
    double v100;                  // st7
    double v101;                  // st7
    float v102;                   // [esp+0h] [ebp-13Ch]
    float v103;                   // [esp+0h] [ebp-13Ch]
    int32 v104;                   // [esp+0h] [ebp-13Ch]
    float v105;                   // [esp+0h] [ebp-13Ch]
    int32 v106;                   // [esp+0h] [ebp-13Ch]
    float v107;                   // [esp+0h] [ebp-13Ch]
    int32 v108;                   // [esp+0h] [ebp-13Ch]
    float v109;                   // [esp+0h] [ebp-13Ch]
    int32 v110;                   // [esp+0h] [ebp-13Ch]
    float v111;                   // [esp+0h] [ebp-13Ch]
    int32 v112;                   // [esp+0h] [ebp-13Ch]
    float v113;                   // [esp+4h] [ebp-138h]
    int32 v114;                   // [esp+4h] [ebp-138h]
    int32 v115;                   // [esp+4h] [ebp-138h]
    float v116;                   // [esp+4h] [ebp-138h]
    int32 v117;                   // [esp+4h] [ebp-138h]
    int32 v118;                   // [esp+4h] [ebp-138h]
    float v119;                   // [esp+4h] [ebp-138h]
    int32 v120;                   // [esp+4h] [ebp-138h]
    int32 v121;                   // [esp+4h] [ebp-138h]
    float v122;                   // [esp+4h] [ebp-138h]
    int32 v123;                   // [esp+4h] [ebp-138h]
    int32 v124;                   // [esp+4h] [ebp-138h]
    float v125;                   // [esp+4h] [ebp-138h]
    int32 v126;                   // [esp+4h] [ebp-138h]
    int32 v127;                   // [esp+4h] [ebp-138h]
    // float maximumWidth;           // [esp+8h] [ebp-134h]
    int32 v134;                   // [esp+8h] [ebp-134h]
    int32 v135;                   // [esp+8h] [ebp-134h]
    int32 v136;                   // [esp+8h] [ebp-134h]
    int32 v137;                   // [esp+8h] [ebp-134h]
    int32 v138;                   // [esp+8h] [ebp-134h]
    int32 v139;                   // [esp+8h] [ebp-134h]
    int32 v140;                   // [esp+8h] [ebp-134h]
    int32 v141;                   // [esp+8h] [ebp-134h]
    int32 v142;                   // [esp+8h] [ebp-134h]
    int32 v143;                   // [esp+8h] [ebp-134h]

    float v145;                   // [esp+1Ch] [ebp-120h]
    float v146;                   // [esp+1Ch] [ebp-120h]
    float v147;                   // [esp+1Ch] [ebp-120h]
    float v148;                   // [esp+1Ch] [ebp-120h]
    float v150;                   // [esp+1Ch] [ebp-120h]
    float v151;                   // [esp+1Ch] [ebp-120h]
    float v152;                   // [esp+1Ch] [ebp-120h]
    float v153;                   // [esp+1Ch] [ebp-120h]
    float v154;                   // [esp+1Ch] [ebp-120h]
    float v155;                   // [esp+1Ch] [ebp-120h]
    __int16 x;                    // [esp+20h] [ebp-11Ch]
    float xa;                     // [esp+20h] [ebp-11Ch]
    float xb;                     // [esp+20h] [ebp-11Ch]
    float xc;                     // [esp+20h] [ebp-11Ch]
    float xd;                     // [esp+20h] [ebp-11Ch]
    float xe;                     // [esp+20h] [ebp-11Ch]
    int xf;                       // [esp+20h] [ebp-11Ch]
    float xg;                     // [esp+20h] [ebp-11Ch]
    float xh;                     // [esp+20h] [ebp-11Ch]
    float xi;                     // [esp+20h] [ebp-11Ch]
    float xj;                     // [esp+20h] [ebp-11Ch]
    float xk;                     // [esp+20h] [ebp-11Ch]
    float xl;                     // [esp+20h] [ebp-11Ch]
    float textb;                  // [esp+28h] [ebp-114h]
    float text;                   // [esp+28h] [ebp-114h]
    float textc;                  // [esp+28h] [ebp-114h]
    float textd;                  // [esp+28h] [ebp-114h]
    GxtChar* pTextToShow = nullptr;              // [esp+28h] [ebp-114h]
    float h;                      // [esp+2Ch] [ebp-110h]
    float ya;                     // [esp+2Ch] [ebp-110h]
    float yb;                     // [esp+2Ch] [ebp-110h]
    float yc = 0; ///???????                     // [esp+2Ch] [ebp-110h]
    float yd = 0; ///???????                       // [esp+2Ch] [ebp-110h]
    float v181;                   // [esp+34h] [ebp-108h]
    float buttonTextPosY;         // [esp+38h] [ebp-104h]
    RwRGBA color;                 // [esp+3Ch] [ebp-100h] BYREF
    float y;                      // [esp+40h] [ebp-FCh]
    float v185;                   // [esp+44h] [ebp-F8h]
    float v186;                   // [esp+48h] [ebp-F4h]
    float scaleX;                 // [esp+4Ch] [ebp-F0h]
    float v188;                   // [esp+50h] [ebp-ECh]
    float v189;                   // [esp+54h] [ebp-E8h]
    float v190;                   // [esp+58h] [ebp-E4h]
    float v191;                   // [esp+5Ch] [ebp-E0h]
    float v192;                   // [esp+60h] [ebp-DCh]
    float v193;                   // [esp+64h] [ebp-D8h]
    float v194;                   // [esp+68h] [ebp-D4h]
    float v195;                   // [esp+6Ch] [ebp-D0h]
    float v196;                   // [esp+70h] [ebp-CCh]
    float v197;                   // [esp+74h] [ebp-C8h]
    float v198;                   // [esp+78h] [ebp-C4h]
    float v199;                   // [esp+7Ch] [ebp-C0h]
    float v200;                   // [esp+80h] [ebp-BCh]
    float v201;                   // [esp+84h] [ebp-B8h]
    float v202;                   // [esp+88h] [ebp-B4h]
    float v203;                   // [esp+8Ch] [ebp-B0h]
    float v204;                   // [esp+90h] [ebp-ACh]
    float v205;                   // [esp+94h] [ebp-A8h]
    float v207;                   // [esp+9Ch] [ebp-A0h]
    float scaleY;                 // [esp+A0h] [ebp-9Ch]
    float v209;                   // [esp+A4h] [ebp-98h]
    float v211;                   // [esp+ACh] [ebp-90h]
    float v212;                   // [esp+B0h] [ebp-8Ch]
    float v213;                   // [esp+B4h] [ebp-88h]
    float v214;                   // [esp+B8h] [ebp-84h]
    float v215;                   // [esp+BCh] [ebp-80h]
    float v216;                   // [esp+C0h] [ebp-7Ch]
    float v217;                   // [esp+C4h] [ebp-78h]

    buttonTextPosY = 0.0;
    bool shouldDrawStandardItems = true;
    CFont::SetBackground(0, 0);
    CFont::SetProportional(1);
    CFont::SetOrientation(eFontAlignment::ALIGN_LEFT);
    CFont::SetWrapx(RsGlobal.maximumWidth - 10);
    CFont::SetRightJustifyWrap(10.0);
    CFont::SetCentreSize(RsGlobal.maximumWidth);

    if (m_nCurrentScreen == SCREEN_STATS) {
        CMenuManager::PrintStats();
    }    
    if (m_nCurrentScreen == MENUPAGE_RESUME) {
        CMenuManager::PrintBriefs();
    }

    if (m_nCurrentScreen == MENUPAGE_SOUND_SETUP && drawTitle) {
        CMenuManager::PrintRadioStationList();
    }

        
    if (drawTitle) {
        if (aScreens[m_nCurrentScreen].m_szTitleName[0]) {
            if (m_nCurrentScreen != MENUPAGE_MAP || !m_bMapLoaded) {
                CFont::SetOrientation(eFontAlignment::ALIGN_LEFT);
                CFont::SetFontStyle(FONT_GOTHIC);
                if (RsGlobal.maximumHeight == 448) {
                    h = 2.0999999;
                } else {
                    h = RsGlobal.maximumHeight * 0.0046874997;
                }    
                if (RsGlobal.maximumWidth == 640) {
                    CFont::SetScale(1.3, h);
                } else {
                    textb = RsGlobal.maximumWidth * 0.0020312499;
                    CFont::SetScale(textb, h);
                }    
                CFont::SetEdge(1);
                CFont::SetColor(HudColour.GetRGB(HUD_COLOUR_LIGHT_BLUE));
                CFont::SetDropColor(HudColour.GetRGB(HUD_COLOUR_BLACK));
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
                CFont::PrintString(text, ya, TheText.Get(aScreens[m_nCurrentScreen].m_szTitleName));
            }    
        }    
    }    

    if (aScreens[m_nCurrentScreen].m_aItems[0].m_nActionType == 1) {
        CFont::SetWrapx(RsGlobal.maximumWidth - 40);
        CFont::SetFontStyle(FONT_SUBTITLES);
        if (RsGlobal.maximumHeight == 448) {
            yb = 1.2;
        } else {
            yb = RsGlobal.maximumHeight * 0.0026785715;
        }
        if (RsGlobal.maximumWidth == 640) {
            CFont::SetScaleForCurrentLanguage(0.48999998, yb);
        } else {
            textc = RsGlobal.maximumWidth * 0.00076562498;
            CFont::SetScaleForCurrentLanguage(textc, yb);
        }
        CFont::SetOrientation(eFontAlignment::ALIGN_LEFT);
        CFont::SetEdge(2);
        CFont::SetDropColor(CRGBA(0, 0, 0, 0xFFu));
        CFont::SetColor(CRGBA(0x4Au, 0x5Au, 0x6Bu, 0xFFu));

        const GxtChar* textOne;

        switch (m_nCurrentScreen) {
        case MENUPAGE_NEW_GAME_ASK:
            if (!m_bMainMenuSwitch) {
                textOne = TheText.Get(aScreens[m_nCurrentScreen].m_aItems[0].m_szName);
                break;
            }
            textOne = TheText.Get("FESZ_QQ");
            break;
        case MENUPAGE_LOAD_SAVE_ASK:
            if (!m_bMainMenuSwitch) {
                textOne = TheText.Get(aScreens[m_nCurrentScreen].m_aItems[0].m_szName);
                break;
            }
            textOne = TheText.Get("FES_LCG");
            break;
        case MENUPAGE_SAVE_WRITE_ASK:
            v10 = CGenericGameStorage::ms_Slots[m_bSelectedSaveGame];
            if ((int)v10) {
                if (v10 != (eSlotState)2) {
                    textOne = TheText.Get(aScreens[m_nCurrentScreen].m_aItems[0].m_szName);
                    break;
                }
                textOne = TheText.Get("FESZ_QC");
            } else {
                textOne = TheText.Get("FESZ_QO");
            }
            break;
        case MENUPAGE_QUIT_GAME:
            if (!m_bMainMenuSwitch) {
                textOne = TheText.Get(aScreens[m_nCurrentScreen].m_aItems[0].m_szName);
                break;
            }
            textOne = TheText.Get("FEQ_SRW");
            break;
        default:
            textOne = TheText.Get(aScreens[m_nCurrentScreen].m_aItems[0].m_szName);
            break;
        }
        if (RsGlobal.maximumHeight == 448) {
            yc = 97.0;
        } else {
            yc = RsGlobal.maximumHeight * 0.21651785;
        }
        if (RsGlobal.maximumWidth == 640) {
            CFont::PrintString(60.0, yc, textOne);
        } else {
            textd = RsGlobal.maximumWidth * 0.09375;
            CFont::PrintString(textd, yc, textOne);
        }
        CFont::SetWrapx(RsGlobal.maximumWidth - 10);
        CFont::SetRightJustifyWrap(10.0);
    }
    if (m_nCurrentScreen == MENUPAGE_CONTROLS_VEHICLE_ONFOOT) {
        if (m_EditingControlOptions) {
            shouldDrawStandardItems = false;
        }
        CMenuManager::DrawControllerScreenExtraText(-8);
    }

    i = 0;
    bool weHaveLabel = aScreens[m_nCurrentScreen].m_aItems[0].m_nActionType == 1;

    for (i = 0; i < 12; i++) {
        auto itemType = aScreens[m_nCurrentScreen].m_aItems[i].m_nType;
        pTextToShow_RightColumn = 0;
        int MENU_DEFAULT_LINE_HEIGHT = itemType == 9 ? 20 : 30;
        if (itemType < 1 || itemType > 8) {
            CFont::SetFontStyle(FONT_MENU);
            scaleY = CMenuManager::StretchY(1.0);
            scaleX = CMenuManager::StretchX(0.7);
            CFont::SetScale(scaleX, scaleY);
            CFont::SetEdge(2);
            CFont::SetDropColor(CRGBA(0, 0, 0, 255));
        } else {
            CFont::SetFontStyle(FONT_MENU);
            CFont::SetEdge(1);
            scaleY = CMenuManager::StretchY(0.95);
            scaleX = CMenuManager::StretchX(0.42);
            CFont::SetScale(scaleX, scaleY);
            CFont::SetDropColor(CRGBA(0, 0, 0, 255));
        }
        if (i == m_nCurrentScreenItem && m_bMapLoaded) {
            CFont::SetColor(CRGBA(172, 203, 241, 255));
        } else {
            CFont::SetColor(CRGBA(74, 90, 107, 255));
        }
        if (aScreens[m_nCurrentScreen].m_aItems[i].m_nAlign == 1) {
            CFont::SetOrientation(eFontAlignment::ALIGN_LEFT);
        } else if (aScreens[m_nCurrentScreen].m_aItems[i].m_nAlign == 2) {
            CFont::SetOrientation(eFontAlignment::ALIGN_RIGHT);
        } else {
            CFont::SetOrientation(eFontAlignment::ALIGN_CENTER);
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

        if (!(aScreens[m_nCurrentScreen].m_aItems[i].m_nActionType != 1 && aScreens[m_nCurrentScreen].m_aItems[i].m_szName[0] != '\0')) {
            continue;
        }
        
        itemType = aScreens[m_nCurrentScreen].m_aItems[i].m_nType;
        yd = 0;

        if (itemType < 1 || itemType > 8) {
            if (itemType == 9) {
                // if (.../) {
                //     AsciiToGxtChar(.../, (GxtChar*)gString);
                //     pTextToShow                                    = (GxtChar*)gString;
                //     aScreens[m_nCurrentScreen].m_aItems[i].m_nActionType = MENU_ACTION_MPACK;
                // } else {
                    aScreens[m_nCurrentScreen].m_aItems[i].m_nActionType = MENU_ACTION_SKIP;
                    pTextToShow = nullptr;
                // }
            } else if (itemType == 10) {
                if (m_ControlMethod) {
                    pTextToShow = (GxtChar*)TheText.Get("FEJ_TIT");
                } else {
                    pTextToShow = (GxtChar*)TheText.Get("FEC_MOU");
                }
            } else {
                pTextToShow = (GxtChar*)TheText.Get(aScreens[m_nCurrentScreen].m_aItems[i].m_szName);
            }
        } else {
            aScreens[m_nCurrentScreen].m_aItems[i].m_X = 80;
            CFont::SetOrientation(eFontAlignment::ALIGN_LEFT);
            int saveSlotState = (int)CGenericGameStorage::ms_Slots[i - 1];
            
            if (saveSlotState == 0) {
                auto saveName = CGenericGameStorage::ms_SlotFileName[i - 1];
                sprintf(gString, "%s", saveName);
                AsciiToGxtChar(gString, gGxtString);
                pTextToShow = gGxtString;
                
                if (GxtCharStrlen(gGxtString) >= 254) {
                    AsciiToGxtChar("...", gGxtString2);
                    GxtCharStrcat(gGxtString, gGxtString2);
                }
                
                pTextToShow_RightColumn = GetSavedGameDateAndTime(i - 1);
            } else if (saveSlotState == 2) {
                pTextToShow = (GxtChar*)TheText.Get("FESZ_CS");
                if (!pTextToShow) {
                    sprintf(gString, "FEM_SL%d", i);
                    CFont::SetOrientation(eFontAlignment::ALIGN_CENTER);
                    aScreens[m_nCurrentScreen].m_aItems[i].m_X = MENU_DEFAULT_CONTENT_X;
                    pTextToShow                                = (GxtChar*)TheText.Get((const char*)gGxtString);
                    yd = CMenuManager::StretchX(40.0);
                }
            } else {
                sprintf((char*)gGxtString, "FEM_SL%d", i);
                CFont::SetOrientation(eFontAlignment::ALIGN_CENTER);
                aScreens[m_nCurrentScreen].m_aItems[i].m_X = MENU_DEFAULT_CONTENT_X;
                pTextToShow                                = (GxtChar*)TheText.Get((const char*)gGxtString);
                yd = CMenuManager::StretchX(40.0);
            }
        }

        switch (aScreens[m_nCurrentScreen].m_aItems[i].m_nActionType) {
        case 32:
            pTextToShow_RightColumn = AudioEngine.GetRadioStationName(m_nRadioStation);
            break;
        case 24:
            pTextToShow_RightColumn = (m_bPrefsFrameLimiter) ? TheText.Get("FEM_ON") : TheText.Get("FEM_OFF");
            break;
        case 25:
            pTextToShow_RightColumn = (m_bShowSubtitles) ? TheText.Get("FEM_ON") : TheText.Get("FEM_OFF");
            break;
        case 26:
            pTextToShow_RightColumn = (m_bWidescreenOn) ? TheText.Get("FEM_ON") : TheText.Get("FEM_OFF");
            break;
        case 30:
            pTextToShow_RightColumn = (m_bRadioEq) ? TheText.Get("FEM_ON") : TheText.Get("FEM_OFF");
            break;
        case 31:
            pTextToShow_RightColumn = (m_bRadioAutoSelect) ? TheText.Get("FEM_ON") : TheText.Get("FEM_OFF");
            break;
        case 33:
            pTextToShow_RightColumn = (m_bMapLegend) ? TheText.Get("FEM_ON") : TheText.Get("FEM_OFF");
            break;
        case 35:
            pTextToShow_RightColumn = (m_bHudOn) ? TheText.Get("FEM_ON") : TheText.Get("FEM_OFF");
            break;
        case 47:
            pTextToShow_RightColumn = (m_bInvertPadX1) ? TheText.Get("FEM_ON") : TheText.Get("FEM_OFF");
            break;
        case 48:
            pTextToShow_RightColumn = (m_bInvertPadY1) ? TheText.Get("FEM_ON") : TheText.Get("FEM_OFF");
            break;
        case 49:
            pTextToShow_RightColumn = (m_bInvertPadX2) ? TheText.Get("FEM_ON") : TheText.Get("FEM_OFF");
            break;
        case 50:
            pTextToShow_RightColumn = (m_bInvertPadY2) ? TheText.Get("FEM_ON") : TheText.Get("FEM_OFF");
            break;
        case 51:
            pTextToShow_RightColumn = (m_bSwapPadAxis1) ? TheText.Get("FEM_ON") : TheText.Get("FEM_OFF");
            break;
        case 52:
            pTextToShow_RightColumn = (m_bSwapPadAxis2) ? TheText.Get("FEM_ON") : TheText.Get("FEM_OFF");
            break;
        case 64:
            pTextToShow_RightColumn = (m_bTracksAutoScan) ? TheText.Get("FEM_ON") : TheText.Get("FEM_OFF");
            break;
        case 65:
            pTextToShow_RightColumn = (m_bSavePhotos) ? TheText.Get("FEM_ON") : TheText.Get("FEM_OFF");
            break;
        case 34:
            if (m_nRadarMode) {
                v29 = m_nRadarMode - 1;
                if (v29) {
                    if (v29 != 1) {
                        break;
                    }
                    pTextToShow_RightColumn = TheText.Get("FEM_OFF");
                } else {
                    pTextToShow_RightColumn = TheText.Get("FED_RDB");
                }
            } else {
                pTextToShow_RightColumn = TheText.Get("FED_RDM");
            }
            break;

        case 36:
            switch ((int)m_nPrefsLanguage) {
            case 0u:
                pTextToShow_RightColumn = TheText.Get("FEL_ENG");
                break;

            case 1u:
                pTextToShow_RightColumn = TheText.Get("FEL_FRE");
                break;

            case 2u:
                pTextToShow_RightColumn = TheText.Get("FEL_GER");
                break;

            case 3u:
                pTextToShow_RightColumn = TheText.Get("FEL_ITA");
                break;

            case 4u:
                pTextToShow_RightColumn = TheText.Get("FEL_SPA");
                break;

            default:
                break;
            }
            break;
        case 42:
            switch (g_fx.GetFxQuality()) {
            case 0u:
                pTextToShow_RightColumn = TheText.Get("FED_FXL");
                break;

            case 1u:
                pTextToShow_RightColumn = TheText.Get("FED_FXM");
                break;

            case 2u:
                pTextToShow_RightColumn = TheText.Get("FED_FXH");
                break;

            case 3u:
                pTextToShow_RightColumn = TheText.Get("FED_FXV");
                break;

            default:
                break;
            }
            break;
        case 43:
            pTextToShow_RightColumn = m_bPrefsMipMapping ? TheText.Get("FEM_ON") : TheText.Get("FEM_OFF");
            if (!m_bMainMenuSwitch) {
                CFont::SetColor(CRGBA(0xEu, 0x1Eu, 0x2Fu, 0xFFu));
            }
            break;
        case 44: {
            if (m_nDisplayAntialiasing <= 1) {
                pTextToShow_RightColumn = TheText.Get("FEM_OFF");
            } else {
                GxtChar tmpBuffer[64];
                AsciiToGxtChar(std::format("{}", m_nDisplayAntialiasing - 1).c_str(), tmpBuffer);
                pTextToShow_RightColumn = tmpBuffer;
            }
            break;
        }
        case 46:
            pTextToShow_RightColumn = (bInvertMouseY) ? TheText.Get("FEM_ON") : TheText.Get("FEM_OFF"); // NOSTA FIX
            break;
        case 56: {
                GxtChar tmpBuffer[1024];
                auto Videomodes = GetVideoModeList();
                AsciiToGxtChar(std::format("{}", Videomodes[m_nDisplayVideoMode]).c_str(), tmpBuffer);                
                pTextToShow_RightColumn = tmpBuffer;
                break;
            }
        case 58:
            pTextToShow_RightColumn = (m_ControlMethod) ? ((m_ControlMethod != 1) ? nullptr : TheText.Get("FET_CCN")) : TheText.Get("FET_SCN");
            break;
        case 59:
            pTextToShow_RightColumn = (CVehicle::m_bEnableMouseSteering) ? TheText.Get("FEM_ON") : TheText.Get("FEM_OFF");
            if (m_ControlMethod == 1) {
                CFont::SetColor(CRGBA(0xEu, 0x1Eu, 0x2Fu, 0xFFu));
            }
            break;
        case 60:
            pTextToShow_RightColumn = (CVehicle::m_bEnableMouseFlying) ? TheText.Get("FEM_ON") : TheText.Get("FEM_OFF");
            if (m_ControlMethod == 1) {
                CFont::SetColor(CRGBA(0xEu, 0x1Eu, 0x2Fu, 0xFFu));
            }
            break;
        case 63:
            if (m_nRadioMode) {
                if (m_nRadioMode == 1) {
                    pTextToShow_RightColumn = TheText.Get("FEA_PR2");
                } else {
                    if (m_nRadioMode != 2) {
                        break;
                    }
                    pTextToShow_RightColumn = TheText.Get("FEA_PR3");
                }
            } else {
                pTextToShow_RightColumn = TheText.Get("FEA_PR1");
            }
            break;

        default:
            break;
        }

        if (pTextToShow) {
            if ((int)CGenericGameStorage::ms_Slots[i - 1] > 0 || itemType < 1 || itemType > 8) {
                CFont::PrintString(CMenuManager::StretchX(aScreens[m_nCurrentScreen].m_aItems[i].m_X), CMenuManager::StretchY(aScreens[m_nCurrentScreen].m_aItems[i].m_Y), pTextToShow);
            // v1.01 + 
            } else {
                CFont::PrintString(CMenuManager::StretchX(25.0 + aScreens[m_nCurrentScreen].m_aItems[i].m_X), CMenuManager::StretchY(aScreens[m_nCurrentScreen].m_aItems[i].m_Y), pTextToShow);
                sprintf(gString, "%d:", i);
                AsciiToGxtChar(gString, gGxtString);
                CFont::PrintString(CMenuManager::StretchX(aScreens[m_nCurrentScreen].m_aItems[i].m_X), CMenuManager::StretchY(aScreens[m_nCurrentScreen].m_aItems[i].m_Y), gGxtString);
            }
        }

        if (pTextToShow_RightColumn) {
            CFont::SetFontStyle(FONT_MENU);
            CFont::SetEdge(1);
            CFont::SetOrientation(eFontAlignment::ALIGN_RIGHT);
            m_nType = aScreens[m_nCurrentScreen].m_aItems[i].m_nType;
            if (m_nType < 1 || m_nType > 8) {
                CFont::SetScale(CMenuManager::StretchX((m_nCurrentScreen == SCREEN_AUDIO_SETTINGS && i == 5) ? 0.56f : 0.7f), CMenuManager::StretchY(1.0f));
            } else {
                CFont::SetScale(CMenuManager::StretchX(0.35f), CMenuManager::StretchY(0.95f));
            }
            
            // Calculate position using screen macros
            float yPos = CMenuManager::StretchY(aScreens[m_nCurrentScreen].m_aItems[i].m_Y);
            float xPos = SCREEN_STRETCH_FROM_RIGHT(40.0f);
            CFont::PrintString(xPos, yPos, pTextToShow_RightColumn);
        }

        // Check if text was properly initialized and we're on the current selection
        if ((pTextToShow && i == m_nCurrentScreenItem) && (m_nCurrentScreen != SCREEN_MAP && m_nCurrentScreen != SCREEN_BRIEF)) {
            // Calculate X position for highlighted item if not already done
            if (!yd) {
                int align = aScreens[m_nCurrentScreen].m_aItems[i].m_nAlign;

                const float scaledPosX = CMenuManager::StretchX(aScreens[m_nCurrentScreen].m_aItems[i].m_X);
                yd = [&]() -> float {
                    switch (aScreens[m_nCurrentScreen].m_aItems[i].m_nAlign) {
                        case eMenuAlign::MENU_ALIGN_LEFT:
                            return scaledPosX - CMenuManager::StretchX(40.0f);
                        case eMenuAlign::MENU_ALIGN_RIGHT:
                            return CMenuManager::StretchX(40.0f) + scaledPosX;
                        default:
                            return scaledPosX - CMenuManager::StretchX(40.0f) - CFont::GetStringWidth(pTextToShow, 1, 0) * 0.5f;
                    }
                }();
            }

            // Draw highlight rectangle for selected item if map is loaded
            if (m_bMapLoaded) {
                if (m_nCurrentScreen != SCREEN_LOAD_FIRST_SAVE && 
                    m_nCurrentScreen != SCREEN_DELETE_FINISHED && 
                    m_nCurrentScreen != SCREEN_SAVE_DONE_1) {
                    float buttonPosY = CMenuManager::StretchY(aScreens[m_nCurrentScreen].m_aItems[i].m_Y);
                    CRect rect(
                        yd,                                        // left
                        buttonPosY - CMenuManager::StretchX(5.0f),  // top
                        yd + CMenuManager::StretchX(32.0f),        // right
                        buttonPosY + CMenuManager::StretchX(47.0f)  // bottom
                    );
                    m_aFrontEndSprites[0].Draw(rect, CRGBA(255, 255, 255, 255));



                }
            }
        }

        // Check for video mode changes
        if (m_nDisplayVideoMode == m_nPrefsVideoMode && 
            !strcmp(aScreens[m_nCurrentScreen].m_aItems[m_nCurrentScreenItem].m_szName, "FED_RES") && m_nHelperText == FET_APP) {
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
        if (m_nDisplayAntialiasing != m_nPrefsAntialiasing) {
            if (strcmp(aScreens[m_nCurrentScreen].m_aItems[m_nCurrentScreenItem].m_szName, "FED_AAS")) {
                if (m_nCurrentScreen == SCREEN_DISPLAY_SETTINGS || m_nCurrentScreen == SCREEN_DISPLAY_ADVANCED) {
                    m_nDisplayAntialiasing = m_nPrefsAntialiasing;
                    CMenuManager::SetHelperText(eHelperText::FET_RSO);
                }
            }
        }
        if (m_nDisplayVideoMode != m_nPrefsVideoMode) {
            if (strcmp(aScreens[m_nCurrentScreen].m_aItems[m_nCurrentScreenItem].m_szName, "FED_RES")) {
                if (m_nCurrentScreen == SCREEN_DISPLAY_SETTINGS || m_nCurrentScreen == SCREEN_DISPLAY_ADVANCED) {
                    m_nDisplayVideoMode = m_nPrefsVideoMode;
                    CMenuManager::SetHelperText(eHelperText::FET_RSO);
                }
            }
        }
        
        // Handle sliders
        switch (aScreens[m_nCurrentScreen].m_aItems[i].m_nActionType) {
        case 27: { // Brightness slider
            const float sliderFieldPos = DisplaySlider(StretchX(500.0f), StretchY(125.0f), StretchY(4.0f), StretchY(20.0f), StretchX(100.0f), m_PrefsBrightness * 0.0026041667f, StretchX(3.0f));
            if (i == m_nCurrentScreenItem && shouldDrawStandardItems) {
                    if (CheckHover(0, sliderFieldPos - StretchX(3.0f), StretchY(125.0f), StretchY(150.0f))) {
                        m_MouseInBounds = 7;
                    } else if (CheckHover(StretchX(3.0f) + sliderFieldPos, StretchX(RsGlobal.maximumWidth), StretchY(125.0f), StretchY(150.0f))) {
                        m_MouseInBounds = m_nMousePosX < StretchX(500.0f) ? 16 : (m_nMousePosY < StretchY(125.0f) || m_nMousePosY > StretchY(150.0f)) ? 16 : 6;
                    } else {
                        m_MouseInBounds = 16;
                    }
                }
                break;
            }
            case 28: { // Radio volume slider
                const float sliderFieldPos = DisplaySlider(StretchX(500.0f), StretchY(95.0f), StretchY(4.0f), StretchY(20.0f), StretchX(100.0f), m_nRadioVolume * 0.015625f, StretchX(3.0f));
                if (i == m_nCurrentScreenItem && shouldDrawStandardItems) {
                    if (CheckHover(0, sliderFieldPos - StretchX(3.0f), StretchY(95.0f), StretchY(120.0f))) {
                        m_MouseInBounds = 11;
                    } else if (CheckHover(StretchX(3.0f) + sliderFieldPos, StretchX(RsGlobal.maximumWidth), StretchY(95.0f), StretchY(120.0f))) {
                        m_MouseInBounds = StretchX(500.0f) <= m_nMousePosX && StretchY(95.0f) <= m_nMousePosY && StretchY(120.0f) >= m_nMousePosY ? 10 : 16;
                    } else {
                        m_MouseInBounds = 16;
                    }
                }
                break;
            }
            case 29: { // SFX volume slider
                const float sliderFieldPos = DisplaySlider(StretchX(500.0f), StretchY(125.0f), StretchY(4.0f), StretchY(20.0f), StretchX(100.0f), m_nSfxVolume * 0.015625f, StretchX(3.0f));
                if (i == m_nCurrentScreenItem && shouldDrawStandardItems) {
                    if (CheckHover(0, sliderFieldPos - StretchX(3.0f), StretchY(125.0f), StretchY(150.0f))) {
                        m_MouseInBounds = 13;
                    } else if (CheckHover(StretchX(3.0f) + sliderFieldPos, StretchX(RsGlobal.maximumWidth), StretchY(125.0f), StretchY(150.0f))) {
                        m_MouseInBounds = m_nMousePosX < StretchX(500.0f) ? 16 : (m_nMousePosY < StretchY(125.0f) || m_nMousePosY > StretchY(150.0f)) ? 16 : 12;
                    } else {
                        m_MouseInBounds = 16;
                    }
                }
                break;
            }
            case 61: { // Draw distance slider
                const float sliderFieldPos = DisplaySlider(StretchX(500.0f), StretchY(125.0f), StretchY(4.0f), StretchY(20.0f), StretchX(100.0f), (m_fDrawDistance - 0.92500001f) * 1.1428572f, StretchX(3.0f));
                if (i == m_nCurrentScreenItem && shouldDrawStandardItems) {
                    if (CheckHover(0, sliderFieldPos - StretchX(3.0f), StretchY(125.0f), StretchY(150.0f))) {
                        m_MouseInBounds = 9;
                    } else if (CheckHover(StretchX(3.0f) + sliderFieldPos, StretchX(RsGlobal.maximumWidth), StretchY(125.0f), StretchY(150.0f))) {
                        m_MouseInBounds = m_nMousePosX < StretchX(500.0f) ? 16 : (m_nMousePosY < StretchY(125.0f) || m_nMousePosY > StretchY(150.0f)) ? 16 : 8;
                    } else {
                        m_MouseInBounds = 16;
                    }
                }
                break;
            }
            case 62: { // Mouse sensitivity slider
                const float sliderFieldPos = DisplaySlider(StretchX(500.0f), StretchY(125.0f), StretchY(4.0f), StretchY(20.0f), StretchX(100.0f), CCamera::m_fMouseAccelHorzntl / 0.0049999999, StretchX(3.0f));
                if (i == m_nCurrentScreenItem && shouldDrawStandardItems) {
                    if (CheckHover(0, sliderFieldPos - StretchX(3.0f), StretchY(125.0f), StretchY(150.0f))) {
                        m_MouseInBounds = 15;
                    } else if (CheckHover(StretchX(3.0f) + sliderFieldPos, StretchX(RsGlobal.maximumWidth), StretchY(125.0f), StretchY(150.0f))) {
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
            buttonTextPosY = (29 * CFont::GetNumberLinesNoPrint(60.0, buttonTextPosY, (const GxtChar*)pTextToShow)) + buttonTextPosY;
        }
        if (aScreens[m_nCurrentScreen].m_aItems[i].m_nActionType == 32) {
            buttonTextPosY = buttonTextPosY + 70.0;
        }
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
void CMenuManager::DrawControllerScreenExtraText(int32 startingYPos) {
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
            float posY = StretchY(float(startingYPos));
            
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
            
            startingYPos += verticalSpacing;
        }
        
        /*/ Handle combo text display - Dummy function deprecated
        if (DEPRECATEDCOMBO) {
            auto comboText = CControllerConfigManager::GetButtonComboText(m_ListSelection);
            if (comboText) {
                CFont::SetColor({200, 50, 50, 255});
                CFont::PrintString(posX, StretchY(float(drawTitle + 10)), comboText);
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
