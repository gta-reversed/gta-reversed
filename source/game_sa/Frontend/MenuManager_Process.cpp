#include "StdInc.h"

#include "MenuManager.h"
#include "MenuManager_Internal.h"
#include "AudioEngine.h"
#include "Pad.h"
#include "Cheat.h"
#include "GenericGameStorage.h"
#include "Camera.h"
#include "Replay.h"
#include "MenuSystem.h"
#include "PostEffects.h"
#include "VideoMode.h"
#include "C_PcSave.h"
#include "Radar.h"

// 0x57B440
void CMenuManager::Process() {
    if (m_bMenuActive) {
        ProcessStreaming(m_bAllStreamingStuffLoaded);
        UserInput();
        ProcessFileActions();
        D3DResourceSystem::TidyUpD3DIndexBuffers(1);
        D3DResourceSystem::TidyUpD3DTextures(1);
    }
    CheckForMenuClosing();
}

// 0x573CF0
void CMenuManager::ProcessStreaming(bool streamAll) {
    // return plugin::CallMethod<0x573CF0, CMenuManager*, bool>(this, streamAll);

    if (!m_bMenuActive || m_nCurrentScreen != SCREEN_MAP)
        return;

    if (!m_bMapLoaded)
        return;

    if (m_bStreamingDisabled)
        return;

    const float tileSizeX = (m_fMapZoom * 2.0f) / float(MAX_RADAR_WIDTH_TILES);
    const float tileSizeY = (m_fMapZoom * 2.0f) / float(MAX_RADAR_HEIGHT_TILES);
    const float left      = m_vMapOrigin.x - m_fMapZoom;
    const float top       = m_vMapOrigin.y - m_fMapZoom;

    for (auto x = 0; x < MAX_RADAR_WIDTH_TILES; x++) {
        for (auto y = 0; y < MAX_RADAR_HEIGHT_TILES; y++) {

            auto fx = float(x);
            auto fy = float(y);

            if (left + fx * tileSizeX <= 60.0f - tileSizeX * 3.0f || left + (fx + 1.0f) * tileSizeX >= float(DEFAULT_SCREEN_WIDTH - 60) + tileSizeX * 3.0f) {
                CRadar::RemoveMapSection(x, y);
                continue;
            }
            if (top + fy * tileSizeY <= 60.0f - tileSizeY * 3.0f || top + (fy + 1.0f) * tileSizeY >= float(DEFAULT_SCREEN_HEIGHT - 60) + tileSizeY * 3.0f) {
                CRadar::RemoveMapSection(x, y);
                continue;
            }

            CRadar::RequestMapSection(x, y);
        }
    }

    // todo: rename streamAll and m_bAllStreamingStuffLoaded
    if (streamAll) {
        CStreaming::LoadAllRequestedModels(false);
        m_bAllStreamingStuffLoaded = false;
    } else {
        CStreaming::LoadRequestedModels();
    }
}

// 0x578D60
void CMenuManager::ProcessFileActions() {
    plugin::CallMethod<0x578D60, CMenuManager*>(this);
}

// 0x576FE0
void CMenuManager::ProcessMenuOptions(int8 pressedLR, bool* cancelPressed, bool acceptPressed) {
    return plugin::CallMethod<0x576FE0, CMenuManager*, int8, bool*, bool>(this, pressedLR, cancelPressed, acceptPressed);

    if (ProcessPCMenuOptions(pressedLR, acceptPressed))
        return;

    tMenuScreen* screen   = &aScreens[m_nCurrentScreen];
    tMenuScreenItem* item = &screen->m_aItems[m_nCurrentScreenItem];

    switch (item->m_nActionType) {
    case MENU_ACTION_BACK:
        *cancelPressed = true;
        return;
    case MENU_ACTION_YES:
    case MENU_ACTION_NO:
    case MENU_ACTION_MENU:
        SwitchToNewScreen(item->m_nTargetMenu);
        return;
    case MENU_ACTION_NEW_GAME:
        ProcessMissionPackNewGame();
        return;
    case MENU_ACTION_MPACK:
        m_nMissionPackGameId = m_MissionPacks[m_nCurrentScreenItem - 2].m_Id; // todo: maybe wrong
        SwitchToNewScreen(SCREEN_MISSION_PACK_LOADING_ASK);
        return;
    case MENU_ACTION_MPACKGAME:
        CGame::bMissionPackGame = m_nMissionPackGameId;
        DoSettingsBeforeStartingAGame();
        return;
    case MENU_ACTION_SAVE_SLOT:
        // todo:
        return;
    case MENU_ACTION_STANDARD_GAME:
        CGame::bMissionPackGame = 0;
        DoSettingsBeforeStartingAGame();
        m_bDontDrawFrontEnd = true;
        return;
    case MENU_ACTION_15:
        m_bDontDrawFrontEnd = true;
        return;
    case MENU_ACTION_SAVE_GAME:
        // todo:
        return;
    case MENU_ACTION_STAT:
        // todo:
        break;
    case MENU_ACTION_INVERT_PAD:
        CPad::bInvertLook4Pad ^= true;
        return;
    case MENU_ACTION_23:
        field_31 ^= true;
        return;
    case MENU_ACTION_SUBTITLES:
        m_bShowSubtitles ^= true;
        SaveSettings();
        return;
    case MENU_ACTION_WIDESCREEN:
        m_bWidescreenOn ^= true;
        SaveSettings();
        return;
    case MENU_ACTION_RADIO_EQ:
        m_bRadioEq ^= true;
        AudioEngine.SetBassEnhanceOnOff(m_bRadioEq);
        SaveSettings();
        return;
    case MENU_ACTION_RADIO_RETUNE:
        m_bRadioAutoSelect ^= true;
        AudioEngine.SetRadioAutoRetuneOnOff(m_bRadioAutoSelect);
        SaveSettings();
        return;
    case MENU_ACTION_RADIO_STATION:
        ScrollRadioStations(pressedLR);
        return;
    case MENU_ACTION_SHOW_LEGEND:
        m_bMapLegend ^= true;
        return;
    case MENU_ACTION_RADAR_MODE:
        m_nRadarMode += pressedLR; // todo: refactor

        if (m_nRadarMode < 0) {
            m_nRadarMode = 2;
        }

        if (m_nRadarMode > 2) {
            m_nRadarMode = 0;
        }

        SaveSettings();
        return;
    case MENU_ACTION_HUD_MODE:
        m_bHudOn ^= true;
        SaveSettings();
        return;
    case MENU_ACTION_LANGUAGE:
        // todo:
        return;
    default:
        return;
    }
}

// 0x57CD50
bool CMenuManager::ProcessPCMenuOptions(int8 pressedLR, bool acceptPressed) {
    return plugin::CallMethodAndReturn<bool, 0x57CD50, CMenuManager*, int8, uint8>(this, pressedLR, acceptPressed);

    tMenuScreen* screen   = &aScreens[m_nCurrentScreen];
    tMenuScreenItem* item = &screen->m_aItems[m_nCurrentScreenItem];

    switch (item->m_nActionType) {
    case MENU_ACTION_USER_TRACKS_SCAN:
        m_bScanningUserTracks = true;
        return true;
    case MENU_ACTION_CTRLS_JOYPAD:
        SwitchToNewScreen(m_nController == 1 ? SCREEN_JOYPAD_SETTINGS : SCREEN_MOUSE_SETTINGS);
        return true;
    case MENU_ACTION_CTRLS_FOOT: // Redefine Controls -> Foot Controls
        field_B7 = 0;
        SwitchToNewScreen(SCREEN_CONTROLS_DEFINITION);
        field_90 = 0;
        return true;
    case MENU_ACTION_CTRLS_CAR: // Redefine Controls -> Vehicle Controls
        field_B7 = 1;
        SwitchToNewScreen(SCREEN_CONTROLS_DEFINITION);
        field_90 = 0;
        return true;
    case MENU_ACTION_FRAME_LIMITER:
        m_bPrefsFrameLimiter ^= true;
        SaveSettings();
        return true;
    case MENU_ACTION_RADAR_MODE:
        // todo: ?
        if (++m_nRadarMode > 2) {
            m_nRadarMode = 0;
        }
        SaveSettings();
        return true;
    case MENU_ACTION_LANG_ENGLISH:
    case MENU_ACTION_LANG_FRENCH:
    case MENU_ACTION_LANG_GERMAN:
    case MENU_ACTION_LANG_ITALIAN:
    case MENU_ACTION_LANG_SPANISH:
#ifdef USE_LANG_RUSSIAN
    case MENU_ACTION_LANG_RUSSIAN:
#endif
#ifdef USE_LANG_JAPANESE
    case MENU_ACTION_LANG_JAPANESE:
#endif
    {
        auto newLang = static_cast<eLanguage>(item->m_nActionType - MENU_ACTION_LANG_ENGLISH);
        if (m_nPrefsLanguage == newLang) {
            return true;
        }
        m_nPrefsLanguage = newLang;
        m_bLanguageChanged = true;
        InitialiseChangedLanguageSettings(false);
        SaveSettings();
        return true;
    }
    case MENU_ACTION_FX_QUALITY:
        switch (g_fx.GetFxQuality()) {
        case FXQUALITY_LOW:
            g_fx.SetFxQuality(FXQUALITY_MEDIUM);
            break;
        case FXQUALITY_MEDIUM:
            g_fx.SetFxQuality(FXQUALITY_HIGH);
            break;
        case FXQUALITY_HIGH:
            g_fx.SetFxQuality(FXQUALITY_VERY_HIGH);
            break;
        case FXQUALITY_VERY_HIGH:
            g_fx.SetFxQuality(FXQUALITY_LOW);
            break;
        }
        SaveSettings();
        return true;
    case MENU_ACTION_MIP_MAPPING:
        if (!m_bMainMenuSwitch) {
            return true;
        }
        m_bPrefsMipMapping ^= true;
        RwTextureSetMipmapping(m_bPrefsMipMapping);
        SaveSettings();
        return true;
    case MENU_ACTION_ANTIALIASING: {
        if (acceptPressed) {
            if (m_nDisplayAntialiasing == m_nPrefsAntialiasing) {
                return true;
            }
            m_nPrefsAntialiasing = m_nDisplayAntialiasing;
            RwD3D9ChangeMultiSamplingLevels(m_nDisplayAntialiasing);
            SetVideoMode(m_nPrefsVideoMode);
            SaveSettings();
            return true;
        }

        // scroll loop
        auto levels = std::min(4u, RwD3D9EngineGetMaxMultiSamplingLevels());
        if (pressedLR > 0) {
            m_nDisplayAntialiasing = m_nDisplayAntialiasing == levels ? 1 : m_nDisplayAntialiasing + 1;
        } else {
            m_nDisplayAntialiasing = m_nDisplayAntialiasing > 1 ? (m_nDisplayAntialiasing - 1) : levels;
        }

        return true;
    }
    case MENU_ACTION_45:
        field_1B14 = 1;
        field_1B09 = 1;
        field_1B08 = 1;
        field_1B0C = m_nCurrentScreenItem;
        field_F0 = (int32)&field_38;
        return true;
    case MENU_ACTION_CONTROLS_MOUSE_INVERT_Y:
        bInvertMouseY = bInvertMouseY == 0;
        SaveSettings();
        return true;
    case MENU_ACTION_CONTROLS_JOY_INVERT_X:
        m_bInvertPadX1 ^= true;
        SaveSettings();
        return true;
    case MENU_ACTION_CONTROLS_JOY_INVERT_Y:
        m_bInvertPadY1 ^= true;
        SaveSettings();
        return true;
    case MENU_ACTION_CONTROLS_JOY_INVERT_X2:
        m_bInvertPadX2 ^= true;
        SaveSettings();
        return true;
    case MENU_ACTION_CONTROLS_JOY_INVERT_Y2:
        m_bInvertPadY2 ^= true;
        SaveSettings();
        return true;
    case MENU_ACTION_CONTROLS_JOY_SWAP_AXIS1:
        m_bSwapPadAxis1 ^= true;
        SaveSettings();
        return true;
    case MENU_ACTION_CONTROLS_JOY_SWAP_AXIS2:
        m_bSwapPadAxis2 ^= true;
        SaveSettings();
        return true;
    case MENU_ACTION_QUIT:
        SwitchToNewScreen(SCREEN_EMPTY);
        return true;
    case MENU_ACTION_MENU_CLOSE:
        m_bDontDrawFrontEnd = 1;
        return true;
    case MENU_ACTION_PAUSE:
        SwitchToNewScreen(SCREEN_GO_BACK);
        return true;
    case MENU_ACTION_RESOLUTION: {
        if (acceptPressed) {
            if (m_nDisplayVideoMode != m_nPrefsVideoMode) {
                m_nPrefsVideoMode = m_nDisplayVideoMode;
                SetVideoMode(m_nDisplayVideoMode);
                CentreMousePointer();
                m_bDrawMouse = true;
                m_nCurrentScreenItem = 5;
                SaveSettings();
                CPostEffects::DoScreenModeDependentInitializations();
            }
        }

        auto numVideoModes = RwEngineGetNumVideoModes();
        auto videoModes = GetVideoModeList();

        if (pressedLR > 0) {
            do {
                m_nDisplayVideoMode = m_nPrefsVideoMode ? (m_nPrefsVideoMode - 1) : (numVideoModes - 1);
            } while (!videoModes[m_nPrefsVideoMode]);
        } else {
            do {
                m_nDisplayVideoMode = (m_nPrefsVideoMode + 1) % numVideoModes;
            } while (!videoModes[m_nPrefsVideoMode]);
        }

        return true;
    }
    case MENU_ACTION_RESET_CFG: {
        auto targetMenu = aScreens[m_nCurrentScreen].m_aItems[2].m_nTargetMenu; // ?
        SetDefaultPreferences(targetMenu);
        if (targetMenu == SCREEN_DISPLAY_ADVANCED) {
            RwD3D9ChangeMultiSamplingLevels(m_nPrefsAntialiasing);
            SetVideoMode(m_nPrefsVideoMode);
        } else if (targetMenu == SCREEN_CONTROLLER_SETUP) {
            ControlsManager.ReinitControls();
        }
        SaveSettings();
        SwitchToNewScreen(targetMenu);
        SetHelperText(FET_HRD);
        return true;
    }
    case MENU_ACTION_CONTROL_TYPE:
        if (m_nController == 1) {
            m_nController = 0;
            CCamera::m_bUseMouse3rdPerson = true;
        } else {
            m_nController = 1;
            CCamera::m_bUseMouse3rdPerson = false;
        }
        SaveSettings();
        return true;
    case MENU_ACTION_MOUSE_STEERING:
        if (m_nController) {
            return true;
        }
        CVehicle::m_bEnableMouseSteering ^= true;
        SaveSettings();
        return true;
    case MENU_ACTION_MOUSE_FLY:
        if (m_nController) {
            return true;
        }
        CVehicle::m_bEnableMouseFlying ^= true;
        SaveSettings();
        return true;
    case MENU_ACTION_USER_TRACKS_PLAY_MODE:
        m_nRadioMode += pressedLR;

        if (pressedLR + m_nRadioMode < 0) {
            m_nRadioMode = 2;
        }

        if (m_nRadioMode <= 2) {
            SaveSettings();
            return true;
        }

        m_nRadioMode = 0;
        SaveSettings();
        return true;
    case MENU_ACTION_USER_TRACKS_AUTO_SCAN:
        m_bTracksAutoScan ^= true;
        SaveSettings();
        return true;
    case MENU_ACTION_STORE_PHOTOS:
        m_bSavePhotos ^= true;
        SaveSettings();
        return true;
    default:
        return false;
    }
}

// 0x57D520
void CMenuManager::ProcessMissionPackNewGame() {
    plugin::CallMethod<0x57D520, CMenuManager*>(this);
}
