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
#include "ControllerConfigManager.h"

// 0x57B440
void CMenuManager::Process() {
    ZoneScoped;

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
    switch (m_nCurrentScreen) {
    case SCREEN_LOAD_FIRST_SAVE:
        if (field_1B3C) {
            if (CGenericGameStorage::CheckSlotDataValid(m_SelectedSlot)) {
                if (!m_bMainMenuSwitch) {
                    DoSettingsBeforeStartingAGame();
                }

                m_bDontDrawFrontEnd = true;
                CGame::bMissionPackGame = false;
                m_bLoadingData = true;
            } else {
                // Load Game
                //
                // Load Failed! There was an error while loading the current game.
                // Please check your savegame directory and try again.
                JumpToGenericMessageScreen(SCREEN_GAME_SAVED, "FET_LG", "FES_LCE");
            }
            field_1B3C = false;
        } else {
            field_1B3C = true;
        }
        break;

    case SCREEN_DELETE_FINISHED:
        if (field_1B3D) {
            if (s_PcSaveHelper.DeleteSlot(m_SelectedSlot)) {
                s_PcSaveHelper.PopulateSlotInfo();
                SwitchToNewScreen(SCREEN_DELETE_SUCCESSFUL);
                m_nCurrentScreenItem = true;
            } else {
                // Delete Game
                //
                // Deleting Failed! There was an error while deleting the current game.
                // Please check your savegame directory and try again.
                JumpToGenericMessageScreen(SCREEN_GAME_SAVED, "FES_DEL", "FES_DEE");
            }
            field_1B3D = false;
        } else {
            field_1B3D = true;
        }
        break;

    case SCREEN_SAVE_DONE_1:
        if (field_1B3E) {
            if (CGame::bMissionPackGame) {
                CFileMgr::SetDirMyDocuments();
                sprintf_s(gString, "MPACK//MPACK%d//SCR.SCM", CGame::bMissionPackGame);
                auto file = CFileMgr::OpenFile(gString, "rb");
                CFileMgr::SetDir(""); // FIX_BUGS

                if (!file) {
                    // Save Game
                    //
                    // Save failed! The current Mission Pack is not available.
                    // Please recheck that the current Mission Pack is installed correctly.
                    return JumpToGenericMessageScreen(SCREEN_GAME_LOADED, "FET_SG", "FES_NIM");
                } else {
                    CFileMgr::CloseFile(file);
                }
            }

            if (s_PcSaveHelper.SaveSlot(m_SelectedSlot)) {
                // Save Game
                //
                // Save failed! There was an error while saving the current game.
                // Please check your savegame directory and try again.
                JumpToGenericMessageScreen(SCREEN_GAME_LOADED, "FET_SG", "FES_CMP");
            } else {
                // Save Game
                //
                // Save Successful. Select OK to continue.
                SwitchToNewScreen(SCREEN_SAVE_DONE_2);
            }
            s_PcSaveHelper.PopulateSlotInfo();

            field_1B3E = false;
        } else {
            field_1B3E = true;
        }
        break;

    default:
        return;
    }
}

// 0x576FE0
// @param pressedLR Arrow button pressed. <0 for left, >0 for right
// @param cancelPressed Returns true to go back.
// @param acceptPressed Is enter pressed. Used for AA mode and resolution
// @addr 0x57CD50
void CMenuManager::ProcessMenuOptions(int8 pressedLR, bool& cancelPressed, bool acceptPressed) {
    if (ProcessPCMenuOptions(pressedLR, acceptPressed))
        return;

    tMenuScreen* screen   = &aScreens[m_nCurrentScreen];
    tMenuScreenItem* item = &screen->m_aItems[m_nCurrentScreenItem];

    switch (item->m_nActionType) {
    case MENU_ACTION_BACK:
        cancelPressed = true;
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
        // -1 for 0-based index, additional -1 for skipping the standard game opt?
        m_nMissionPackGameId = m_MissionPacks[m_nCurrentScreenItem - 2].m_Id; // todo: maybe wrong
        SwitchToNewScreen(SCREEN_MISSION_PACK_LOADING_ASK);
        return;
    case MENU_ACTION_MPACKGAME:
        CGame::bMissionPackGame = m_nMissionPackGameId;
        DoSettingsBeforeStartingAGame();
        return;
    case MENU_ACTION_SAVE_SLOT:
        if (item->m_nType >= eMenuEntryType::TI_SLOT1 && item->m_nType <= eMenuEntryType::TI_SLOT8) {
            auto slot = CGenericGameStorage::ms_Slots[m_nCurrentScreenItem - 1];
            
            m_SelectedSlot = m_nCurrentScreenItem - 1;

            if (m_nCurrentScreen == SCREEN_DELETE_GAME && slot != eSlotState::SLOT_FREE) {
                SwitchToNewScreen(SCREEN_DELETE_GAME_ASK);
            } else if (slot == eSlotState::SLOT_FILLED) {
                SwitchToNewScreen(SCREEN_LOAD_GAME_ASK);
            }
        }
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
        if (item->m_nType >= eMenuEntryType::TI_SLOT1 && item->m_nType <= eMenuEntryType::TI_SLOT8) {
            auto slot = CGenericGameStorage::ms_Slots[m_nCurrentScreenItem - 1];
            m_SelectedSlot = m_nCurrentScreenItem - 1;

            SwitchToNewScreen(SCREEN_SAVE_WRITE_ASK);
        }
        return;
    case MENU_ACTION_STAT:
        // todo: refactor
        if (pressedLR != 1) {
            if (m_nStatsScrollDirection) {
                if (m_fStatsScrollSpeed != 0.0f) {
                    m_fStatsScrollSpeed = 0.0f;
                    m_nStatsScrollDirection = 0;
                }
            } else if (m_fStatsScrollSpeed != 0.0f) {
                if (m_fStatsScrollSpeed == 150.0f) {
                    m_fStatsScrollSpeed = 20.0f;
                }
                m_nStatsScrollDirection = 0;
            } else {
                m_fStatsScrollSpeed = 150.0f;
                m_nStatsScrollDirection = 0;
            }
        } else if (m_nStatsScrollDirection) {
            if (m_fStatsScrollSpeed == 0.0f) {
                m_fStatsScrollSpeed = 150.0f;
                m_nStatsScrollDirection = 1;
            } else {
                if (m_fStatsScrollSpeed == 150.0f) {
                    m_fStatsScrollSpeed = 20.0f;
                }
                m_nStatsScrollDirection = 1;
            }
        } else {
            if (m_fStatsScrollSpeed == 0.0f) {
                m_fStatsScrollSpeed = 150.0f;
                m_nStatsScrollDirection = 1;
            } else {
                m_fStatsScrollSpeed = 0.0f;
                m_nStatsScrollDirection = 1;
            }
        }
        break;
    case MENU_ACTION_INVERT_PAD:
        CPad::bInvertLook4Pad ^= true;
        return;
    case MENU_ACTION_23:
        m_DisplayControllerOnFoot ^= true;
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
        m_nRadarMode = (eRadarMode)((m_nRadarMode + pressedLR) % 3);
        SaveSettings();
        return;
    case MENU_ACTION_HUD_MODE:
        m_bHudOn = !m_bHudOn;
        SaveSettings();
        return;
    case MENU_ACTION_LANGUAGE: {
        // todo: MORE_LANGUAGES; does this ever execute?
        auto prevLanguage = m_nPrefsLanguage;
        if (pressedLR <= 0 && prevLanguage == eLanguage::ENGLISH) {
            m_nPrefsLanguage = eLanguage::SPANISH;
        } else if (prevLanguage == eLanguage::SPANISH) {
            m_nPrefsLanguage = eLanguage::ENGLISH;
        }

        m_nPreviousLanguage = (eLanguage)-99; // what the fuck
        m_bLanguageChanged = true;
        InitialiseChangedLanguageSettings(0);
        SaveSettings();
        return;
    }
    default:
        return;
    }
}

// @param pressedLR Arrow button pressed. <0 for left, >0 for right
// @param acceptPressed Is enter pressed. Used for AA mode and resolution
// @addr 0x57CD50
bool CMenuManager::ProcessPCMenuOptions(int8 pressedLR, bool acceptPressed) {
    tMenuScreen* screen   = &aScreens[m_nCurrentScreen];
    tMenuScreenItem* item = &screen->m_aItems[m_nCurrentScreenItem];

    switch (item->m_nActionType) {
    case MENU_ACTION_USER_TRACKS_SCAN:
        m_bScanningUserTracks = true;
        return true;
    case MENU_ACTION_CTRLS_JOYPAD:
        SwitchToNewScreen(m_ControlMethod ? SCREEN_JOYPAD_SETTINGS : SCREEN_MOUSE_SETTINGS);
        return true;
    case MENU_ACTION_CTRLS_FOOT: // Redefine Controls -> Foot Controls
        m_RedefiningControls = false;
        SwitchToNewScreen(SCREEN_CONTROLS_DEFINITION);
        m_ListSelection = 0;
        return true;
    case MENU_ACTION_CTRLS_CAR: // Redefine Controls -> Vehicle Controls
        m_RedefiningControls = true;
        SwitchToNewScreen(SCREEN_CONTROLS_DEFINITION);
        m_ListSelection = 0;
        return true;
    case MENU_ACTION_FRAME_LIMITER:
        m_bPrefsFrameLimiter ^= true;
        SaveSettings();
        return true;
    case MENU_ACTION_RADAR_MODE:
        switch (m_nRadarMode) {
        case eRadarMode::MAPS_AND_BLIPS:
            m_nRadarMode = eRadarMode::BLIPS_ONLY;
            break;
        case eRadarMode::BLIPS_ONLY:
            m_nRadarMode = eRadarMode::OFF;
            break;
        case eRadarMode::OFF:
            m_nRadarMode = eRadarMode::MAPS_AND_BLIPS;
            break;
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
        case FX_QUALITY_LOW:
            g_fx.SetFxQuality(FX_QUALITY_MEDIUM);
            break;
        case FX_QUALITY_MEDIUM:
            g_fx.SetFxQuality(FX_QUALITY_HIGH);
            break;
        case FX_QUALITY_HIGH:
            g_fx.SetFxQuality(FX_QUALITY_VERY_HIGH);
            break;
        case FX_QUALITY_VERY_HIGH:
            g_fx.SetFxQuality(FX_QUALITY_LOW);
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
            // ((void(*)(int))0x745C70)(m_nPrefsVideoMode);
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
        m_CanBeDefined = true;
        m_EditingControlOptions = true;
        m_bJustOpenedControlRedefWindow = true;
        m_OptionToChange = m_nCurrentScreenItem;
        m_pPressedKey = &m_KeyPressedCode;
        return true;
    case MENU_ACTION_CONTROLS_MOUSE_INVERT_Y:
        bInvertMouseY ^= true;
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
                ++m_nDisplayVideoMode;
            }
#ifdef FIX_BUGS // Out of array bounds access
            while (m_nDisplayVideoMode < numVideoModes && !videoModes[m_nDisplayVideoMode]);
#else
            while (!videoModes[m_nDisplayVideoMode]);
#endif

            if (m_nDisplayVideoMode >= numVideoModes) {
                m_nDisplayVideoMode = 0;
                while (!videoModes[m_nDisplayVideoMode])
                    ++m_nDisplayVideoMode;
            }
        } else {
            do {
                --m_nDisplayVideoMode;
            }
#ifdef FIX_BUGS // Out of array bounds access
            while (m_nDisplayVideoMode >= 0 && !videoModes[m_nDisplayVideoMode]);
#else
            while (!videoModes[m_nDisplayVideoMode]);
#endif

            if (m_nDisplayVideoMode < 0) {
                m_nDisplayVideoMode = numVideoModes - 1;
                while (!videoModes[m_nDisplayVideoMode])
                    --m_nDisplayVideoMode;
            }
        }

        return true;
    }
    case MENU_ACTION_RESET_CFG: {
        const auto& targetMenu = aScreens[m_nCurrentScreen].m_aItems[2].m_nTargetMenu; // ?
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
        if (m_ControlMethod) {
            m_ControlMethod = false;
            CCamera::m_bUseMouse3rdPerson = true;
        } else {
            m_ControlMethod = true;
            CCamera::m_bUseMouse3rdPerson = false;
        }
        SaveSettings();
        return true;
    case MENU_ACTION_MOUSE_STEERING:
        if (m_ControlMethod) {
            return true;
        }
        CVehicle::m_bEnableMouseSteering ^= true;
        SaveSettings();
        return true;
    case MENU_ACTION_MOUSE_FLY:
        if (m_ControlMethod) {
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
    for (auto mpack : m_MissionPacks) {
        mpack.m_Id = 0u;
    }

    auto isAnyAvailable = false;
    CFileMgr::SetDirMyDocuments();
    for (auto i = 0u; i < 25u; i++) {
        sprintf_s(gString, "MPACK//MPACK%d//MPACK.DAT", i);
        if (auto file = CFileMgr::OpenFile(gString, "rb")) {
            // MPACK.DAT file format:
            //
            // <ID>#<NAME OF THE MPACK>#
            // Ex.: '5#Design Your Own Mission#'

            if (!isAnyAvailable) {
                isAnyAvailable = true;
            }

            // NOTSA
            VERIFY(fscanf_s(file, "%" PRIu8 "#%[^\n\r#]#", &m_MissionPacks[i].m_Id, SCANF_S_STR(m_MissionPacks[i].m_Name)) == 2);
            CFileMgr::CloseFile(file);
        }
    }

    if (isAnyAvailable) {
        SwitchToNewScreen(SCREEN_SELECT_GAME);
    } else {
        SwitchToNewScreen(SCREEN_NEW_GAME_ASK);

        auto screen = &aScreens[m_nCurrentScreen];
        if (CGame::bMissionPackGame) {
            // Are you sure you want to start a new standard game?
            // All current game progress in this Mission Pack will be lost. Proceed?
            strncpy_s(screen->m_aItems[0].m_szName, "FESZ_MR", 8u);
        } else {
            // Are you sure you want to start a new game?
            // All current game progress will be lost. Proceed?
            strncpy_s(screen->m_aItems[0].m_szName, "FESZ_QR", 8u);
        }
    }
}
