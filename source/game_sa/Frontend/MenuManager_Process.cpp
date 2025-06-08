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
    // Exit early if menu is inactive or not on the map screen
    if (!m_bMenuActive || m_nCurrentScreen != SCREEN_MAP) {
        return;
    }

    // Exit if map isn't loaded or streaming is disabled
    if (!m_bMapLoaded || m_bStreamingDisabled) {
        return;
    }

    // Calculate tile sizes based on map zoom
    const float tileSizeX = (m_fMapZoom * 2.0f) / float(MAX_RADAR_WIDTH_TILES);
    const float tileSizeY = (m_fMapZoom * 2.0f) / float(MAX_RADAR_HEIGHT_TILES);

    // Calculate visible area boundaries
    const float mapLeft = m_vMapOrigin.x - m_fMapZoom;
    const float mapTop = m_vMapOrigin.y - m_fMapZoom;
    const float mapRight = mapLeft + m_fMapZoom * 2.0f;
    const float mapBottom = mapTop + m_fMapZoom * 2.0f;

    // Define screen boundaries with padding for visibility
    const float screenLeft = 60.0f - tileSizeX * 3.0f;
    const float screenRight = float(DEFAULT_SCREEN_WIDTH - 60.0f) + tileSizeX * 3.0f;
    const float screenTop = 60.0f - tileSizeY * 3.0f;
    const float screenBottom = float(DEFAULT_SCREEN_HEIGHT - 60.0f) + tileSizeY * 3.0f;

    // Iterate over map tiles to determine which sections to load or remove
    for (auto x = 0; x < MAX_RADAR_WIDTH_TILES; x++) {
        for (auto y = 0; y < MAX_RADAR_HEIGHT_TILES; y++) {
            // Calculate tile position in world coordinates
            float tileLeft = mapLeft + x * tileSizeX;
            float tileRight = tileLeft + tileSizeX;
            float tileTop = mapTop + y * tileSizeY;
            float tileBottom = tileTop + tileSizeY;

            // Check if tile is outside visible screen area
            if (tileRight <= screenLeft || tileLeft >= screenRight ||
                tileBottom <= screenTop || tileTop >= screenBottom) {
                CRadar::RemoveMapSection(x, y);
            } else {
                CRadar::RequestMapSection(x, y);
            }
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
        if (m_CurrentlyLoading) {
            if (CGenericGameStorage::CheckSlotDataValid(m_SelectedSlot)) {
                // Prepare for game start if not switching from main menu
                if (!m_bMainMenuSwitch) {
                    DoSettingsBeforeStartingAGame();
                }

                m_bDontDrawFrontEnd = true;
                CGame::bMissionPackGame = false;
                m_bLoadingData = true;
            } else {
                // Load Game
                //
                // Load Failed! There was an error while loading the current game. Please check your savegame directory and try again.
                JumpToGenericMessageScreen(SCREEN_GAME_SAVED, "FET_LG", "FES_LCE");
            }
            m_CurrentlyLoading = false;
        } else {
            m_CurrentlyLoading = true;
        }
        break;

    case SCREEN_DELETE_FINISHED:
        if (m_CurrentlyDeleting) {
            if (s_PcSaveHelper.DeleteSlot(m_SelectedSlot)) {
                // Update slot info and switch to success screen
                s_PcSaveHelper.PopulateSlotInfo();
                SwitchToNewScreen(SCREEN_DELETE_SUCCESSFUL);
                m_nCurrentScreenItem = true;
            } else {
                // Delete Game
                //
                // Deleting Failed! There was an error while deleting the current game. Please check your savegame directory and try again.
                JumpToGenericMessageScreen(SCREEN_GAME_SAVED, "FES_DEL", "FES_DEE");
            }
            m_CurrentlyDeleting = false;
        } else {
            m_CurrentlyDeleting = true;
        }
        break;

    case SCREEN_SAVE_DONE_1:
        if (m_CurrentlySaving) {
            if (CGame::bMissionPackGame) {
                // Check mission pack file availability
                CFileMgr::SetDirMyDocuments();
                sprintf_s(gString, "MPACK//MPACK%d//SCR.SCM", CGame::bMissionPackGame);
                auto file = CFileMgr::OpenFile(gString, "rb");
                CFileMgr::SetDir(""); // FIX_BUGS

                if (!file) {
                    // Save Game
                    //
                    // Save failed! The current Mission Pack is not available. Please recheck that the current Mission Pack is installed correctly.
                    return JumpToGenericMessageScreen(SCREEN_GAME_LOADED, "FET_SG", "FES_NIM");
                } else {
                    CFileMgr::CloseFile(file);
                }
            }

            if (s_PcSaveHelper.SaveSlot(m_SelectedSlot)) {
                // Save Game
                //
                // Save failed! There was an error while saving the current game. Please check your savegame directory and try again.
                JumpToGenericMessageScreen(SCREEN_GAME_LOADED, "FET_SG", "FES_CMP");
            } else {
                // Save Game
                //
                // Save Successful. Select OK to continue.
                SwitchToNewScreen(SCREEN_SAVE_DONE_2);
            }
            s_PcSaveHelper.PopulateSlotInfo();
            m_CurrentlySaving = false;
        } else {
            m_CurrentlySaving = true;
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
void CMenuManager::ProcessMenuOptions(int8 pressedLR, bool& cancelPressed, bool acceptPressed) {
    // Handle PC-specific options first
    if (ProcessPCMenuOptions(pressedLR, acceptPressed)) {
        return;
    }

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
    case MENU_ACTION_MPACK: {
        std::array<MPack, MPACK_COUNT> missionPacksArray = std::to_array(m_MissionPacks);
        if (m_nCurrentScreenItem - 2 < MPACK_COUNT && m_nCurrentScreenItem - 2 >= 0) {
            const auto& MPacks = missionPacksArray[m_nCurrentScreenItem - 2];
            m_nMissionPackGameId = MPacks.m_Id;
            NOTSA_LOG_DEBUG("Selected mission pack: {}", (int)MPacks.m_Id);
            SwitchToNewScreen(SCREEN_MISSION_PACK_LOADING_ASK);
        } else {
            NOTSA_UNREACHABLE("Index out of bounds for missionPacksArray");
        }
        return;
    }
    case MENU_ACTION_MPACKGAME:
        CGame::bMissionPackGame = m_nMissionPackGameId;
        DoSettingsBeforeStartingAGame();
        return;
    case MENU_ACTION_SAVE_SLOT: {
        if (item->m_nType >= eMenuEntryType::TI_SLOT1 && item->m_nType <= eMenuEntryType::TI_SLOT8) {
            auto slotState = CGenericGameStorage::ms_Slots[m_nCurrentScreenItem - 1];
            m_SelectedSlot = m_nCurrentScreenItem - 1;

            if (m_nCurrentScreen == SCREEN_DELETE_GAME && slotState != eSlotState::SLOT_FREE) {
                SwitchToNewScreen(SCREEN_DELETE_GAME_ASK);
            } else if (slotState == eSlotState::SLOT_FILLED) {
                SwitchToNewScreen(SCREEN_LOAD_GAME_ASK);
            }
        }
        return;
    }
    case MENU_ACTION_STANDARD_GAME:
        CGame::bMissionPackGame = 0;
        DoSettingsBeforeStartingAGame();
        m_bDontDrawFrontEnd = true;
        return;
    case MENU_ACTION_15:
        m_bDontDrawFrontEnd = true;
        return;
    case MENU_ACTION_SAVE_GAME: {
        if (item->m_nType >= eMenuEntryType::TI_SLOT1 && item->m_nType <= eMenuEntryType::TI_SLOT8) {
            m_SelectedSlot = m_nCurrentScreenItem - 1;
            SwitchToNewScreen(SCREEN_SAVE_WRITE_ASK);
        }
        return;
    }
    case MENU_ACTION_STAT: {
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
    }
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
        m_nRadarMode = (eRadarMode)((m_nRadarMode + pressedLR) % eRadarMode::RADAR_MODE_COUNT);
        SaveSettings();
        return;
    case MENU_ACTION_HUD_MODE:
        m_bHudOn = !m_bHudOn;
        SaveSettings();
        return;
    case MENU_ACTION_LANGUAGE: {
        // todo: MORE_LANGUAGES; does this ever execute?
        auto prevLanguage = m_nPrefsLanguage;
        if (pressedLR <= 0 && prevLanguage == eLanguage::AMERICAN) {
            m_nPrefsLanguage = eLanguage::SPANISH;
        } else if (prevLanguage == eLanguage::SPANISH) {
            m_nPrefsLanguage = eLanguage::AMERICAN;
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

// 0x57CD50
// @param pressedLR Arrow button pressed. <0 for left, >0 for right
// @param acceptPressed Is enter pressed. Used for AA mode and resolution
bool CMenuManager::ProcessPCMenuOptions(int8 pressedLR, bool acceptPressed) {
    tMenuScreen* screen   = &aScreens[m_nCurrentScreen];
    tMenuScreenItem* item = &screen->m_aItems[m_nCurrentScreenItem];

    switch (item->m_nActionType) {
    case MENU_ACTION_USER_TRACKS_SCAN:
        m_bScanningUserTracks = true;
        return true;
    case MENU_ACTION_CTRLS_JOYPAD:
        if (m_ControlMethod == eController::JOYPAD) {
            SwitchToNewScreen(SCREEN_JOYPAD_SETTINGS);
        } else if (m_ControlMethod == eController::MOUSE_PLUS_KEYS) {
            SwitchToNewScreen(SCREEN_MOUSE_SETTINGS);
        } else {
            NOTSA_UNREACHABLE();
        }
        return true;
    case MENU_ACTION_CTRLS_FOOT: // Redefine Controls -> Foot Controls
        m_RedefiningControls = eControlMode::FOOT;
        SwitchToNewScreen(SCREEN_CONTROLS_DEFINITION);
        m_ListSelection = 0;
        return true;
    case MENU_ACTION_CTRLS_CAR: // Redefine Controls -> Vehicle Controls
        m_RedefiningControls = eControlMode::VEHICLE;
        SwitchToNewScreen(SCREEN_CONTROLS_DEFINITION);
        m_ListSelection = 0;
        return true;
    case MENU_ACTION_FRAME_LIMITER:
        m_bPrefsFrameLimiter ^= true;
        SaveSettings();
        return true;
    case MENU_ACTION_RADAR_MODE:
        m_nRadarMode = eRadarMode(m_nRadarMode + (notsa::IsFixBugs() ? (pressedLR == 0 ? 1 : pressedLR) : 1));
        if (m_nRadarMode < eRadarMode::RADAR_MODE_MAPS_AND_BLIPS) {
            m_nRadarMode = eRadarMode(RADAR_MODE_COUNT - 1);
        } else if (m_nRadarMode >= eRadarMode::RADAR_MODE_COUNT) {
            m_nRadarMode = eRadarMode::RADAR_MODE_MAPS_AND_BLIPS;
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
        if (m_nPrefsLanguage != newLang) {
            m_nPrefsLanguage = newLang;
            m_bLanguageChanged = true;
            InitialiseChangedLanguageSettings(false);
            SaveSettings();
        }
        return true;
    }
    case MENU_ACTION_FX_QUALITY:
        g_fx.SetFxQuality(FxQuality_e(g_fx.m_FxQuality + (notsa::IsFixBugs() ? (pressedLR == 0 ? 1 : pressedLR) : 1)));
        if (g_fx.m_FxQuality < FX_QUALITY_LOW) {
            g_fx.SetFxQuality(FxQuality_e(FX_QUALITY_COUNT - 1));
        } else if (g_fx.m_FxQuality >= FX_QUALITY_COUNT) {
            g_fx.SetFxQuality(FxQuality_e(FX_QUALITY_LOW));
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
            if (m_nDisplayAntialiasing != m_nPrefsAntialiasing) {
                m_nPrefsAntialiasing = m_nDisplayAntialiasing;
                RwD3D9ChangeMultiSamplingLevels(m_nDisplayAntialiasing);
                SetVideoMode(m_nPrefsVideoMode);
                SaveSettings();
            }
            return true;
        }

        // scroll loop
        auto maxLevels = std::min(4u, RwD3D9EngineGetMaxMultiSamplingLevels());
        if (pressedLR > 0) {
            m_nDisplayAntialiasing = m_nDisplayAntialiasing == maxLevels ? 1 : m_nDisplayAntialiasing + 1;
        } else {
            m_nDisplayAntialiasing = m_nDisplayAntialiasing > 1 ? m_nDisplayAntialiasing - 1 : maxLevels;
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
        if (acceptPressed && m_nDisplayVideoMode != m_nPrefsVideoMode) {
            m_nPrefsVideoMode = m_nDisplayVideoMode;
            SetVideoMode(m_nDisplayVideoMode);
            CentreMousePointer();
            m_DisplayTheMouse = true;
            m_nCurrentScreenItem = 5;
            SaveSettings();
            CPostEffects::DoScreenModeDependentInitializations();
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
                while (!videoModes[m_nDisplayVideoMode]) {
                    ++m_nDisplayVideoMode;
                }
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
                while (!videoModes[m_nDisplayVideoMode]) {
                    --m_nDisplayVideoMode;
                }
            }
        }
        return true;
    }
    case MENU_ACTION_RESET_CFG: {
        const auto targetMenu = aScreens[m_nCurrentScreen].m_aItems[2].m_nTargetMenu;
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
        if (m_ControlMethod == eController::JOYPAD) {
            m_ControlMethod = eController::MOUSE_PLUS_KEYS;
            CCamera::m_bUseMouse3rdPerson = true;
        } else if (m_ControlMethod == eController::MOUSE_PLUS_KEYS) {
            m_ControlMethod = eController::JOYPAD;
            CCamera::m_bUseMouse3rdPerson = false;
        } else {
            NOTSA_UNREACHABLE();
        }
        SaveSettings();
        return true;
    case MENU_ACTION_MOUSE_STEERING:
        if (m_ControlMethod != eController::MOUSE_PLUS_KEYS) {
            return true;
        }
        CVehicle::m_bEnableMouseSteering ^= true;
        SaveSettings();
        return true;
    case MENU_ACTION_MOUSE_FLY:
        if (m_ControlMethod != eController::MOUSE_PLUS_KEYS) {
            return true;
        }
        CVehicle::m_bEnableMouseFlying ^= true;
        SaveSettings();
        return true;
    case MENU_ACTION_USER_TRACKS_PLAY_MODE:
        m_nRadioMode = eRadioMode((notsa::IsFixBugs() ? (pressedLR == 0 ? 1 : pressedLR) : pressedLR) + m_nRadioMode);
        if (m_nRadioMode < eRadioMode::RADIO_MODE_RADIO) {
            m_nRadioMode = eRadioMode(RADIO_MODE_COUNT - 1);
        } else if (m_nRadioMode >= eRadioMode::RADIO_MODE_COUNT) {
            m_nRadioMode = eRadioMode::RADIO_MODE_RADIO;
        }
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
    // Reset mission pack IDs
    for (auto& mpack : m_MissionPacks) {
        mpack.m_Id = 0u;
    }

    bool isAnyMissionPackAvailable = false;
    CFileMgr::SetDirMyDocuments();

    // Scan for available mission packs
    for (auto i = 0u; i < 25u; i++) {
        sprintf_s(gString, "MPACK//MPACK%d//MPACK.DAT", i);
        if (auto file = CFileMgr::OpenFile(gString, "rb")) {
            // MPACK.DAT file format:
            //
            // <ID>#<NAME OF THE MPACK>#
            // Ex.: '5#Design Your Own Mission#'

            isAnyMissionPackAvailable = true;

            // NOTSA
            VERIFY(fscanf_s(file, "%" PRIu8 "#%[^\n\r#]#", &m_MissionPacks[i].m_Id, SCANF_S_STR(m_MissionPacks[i].m_Name)) == 2);
            CFileMgr::CloseFile(file);
        }
    }

    if (isAnyMissionPackAvailable) {
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
