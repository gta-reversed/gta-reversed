/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#include "StdInc.h"

#include <extensions/enumerate.hpp>

#include "MenuManager.h"
#include "MenuManager_Internal.h"
#include "MenuSystem.h"
#include "AEAudioUtility.h"
#include "Radar.h"
#include "Gamma.h"
#include "VideoMode.h"
#include "C_PcSave.h"
#include "platform.h"
#include "Hud.h"
#include "ControllerConfigManager.h"

CMenuManager& FrontEndMenuManager = *(CMenuManager*)0xBA6748;

CMenuManager& GetMenu() {
    return FrontEndMenuManager;
}

void CMenuManager::InjectHooks() {
    RH_ScopedClass(CMenuManager);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Initialise, 0x5744D0);
    RH_ScopedInstall(LoadAllTextures, 0x572EC0);
    RH_ScopedInstall(SwapTexturesRound, 0x5730A0);
    RH_ScopedInstall(UnloadTextures, 0x574630);
    RH_ScopedInstall(InitialiseChangedLanguageSettings, 0x573260);
    RH_ScopedInstall(HasLanguageChanged, 0x573CD0);
    RH_ScopedInstall(DoSettingsBeforeStartingAGame, 0x573330);
    RH_ScopedInstall(StretchX, 0x5733E0);
    RH_ScopedInstall(StretchY, 0x573410, { .reversed = false });
    RH_ScopedInstall(SwitchToNewScreen, 0x573680);
    RH_ScopedInstall(ScrollRadioStations, 0x573A00);
    RH_ScopedInstall(SetFrontEndRenderStates, 0x573A60);
    RH_ScopedInstall(SetDefaultPreferences, 0x573AE0);
    RH_ScopedInstall(GetNumberOfMenuOptions, 0x573E70);
    RH_ScopedInstall(JumpToGenericMessageScreen, 0x576AE0);

    RH_ScopedInstall(DrawFrontEnd, 0x57C290);
    RH_ScopedInstall(DrawBackground, 0x57B750);
    RH_ScopedInstall(DrawStandardMenus, 0x5794A0, { .reversed = false });
    RH_ScopedInstall(DrawWindow, 0x573EE0);
    RH_ScopedInstall(DrawWindowedText, 0x578F50);
    RH_ScopedInstall(DrawQuitGameScreen, 0x57D860);
    RH_ScopedInstall(DrawControllerScreenExtraText, 0x57D8D0, { .reversed = false });
    RH_ScopedInstall(DrawControllerBound, 0x57E6E0, { .reversed = false });
    RH_ScopedInstall(DrawControllerSetupScreen, 0x57F300, { .reversed = false });

    RH_ScopedInstall(CentreMousePointer, 0x57C520);
    RH_ScopedInstall(LoadSettings, 0x57C8F0);
    RH_ScopedInstall(SaveSettings, 0x57C660);
    RH_ScopedInstall(SaveStatsToFile, 0x57DDE0);
    RH_ScopedInstall(SaveLoadFileError_SetUpErrorScreen, 0x57C490);

    RH_ScopedInstall(CheckSliderMovement, 0x573440);
    RH_ScopedInstall(CheckFrontEndUpInput, 0x573840);
    RH_ScopedInstall(CheckFrontEndDownInput, 0x5738B0);
    RH_ScopedInstall(CheckFrontEndLeftInput, 0x573920);
    RH_ScopedInstall(CheckFrontEndRightInput, 0x573990);
    RH_ScopedInstall(CheckForMenuClosing, 0x576B70, { .locked = true });  // Must be hooked at all times otherwise imgui stops working! [The input at least does]
    RH_ScopedInstall(CheckHover, 0x57C4F0);
    RH_ScopedInstall(CheckMissionPackValidMenu, 0x57D720);
    RH_ScopedInstall(CheckCodesForControls, 0x57DB20, { .reversed = false });

    RH_ScopedInstall(DisplaySlider, 0x576860);
    RH_ScopedInstall(DisplayHelperText, 0x57E240);
    RH_ScopedInstall(SetHelperText, 0x57CD10);
    RH_ScopedInstall(ResetHelperText, 0x57CD30);
    RH_ScopedInstall(MessageScreen, 0x579330);
    RH_ScopedInstall(SmallMessageScreen, 0x574010);

    RH_ScopedInstall(PrintMap, 0x575130, { .reversed = false });
    RH_ScopedInstall(PrintStats, 0x574900, { .reversed = false });
    RH_ScopedInstall(PrintBriefs, 0x576320);
    RH_ScopedInstall(PrintRadioStationList, 0x5746F0);

    RH_ScopedInstall(UserInput, 0x57FD70, { .reversed = false });
    RH_ScopedInstall(AdditionalOptionInput, 0x5773D0, { .reversed = false });
    RH_ScopedInstall(CheckRedefineControlInput, 0x57E4D0);
    RH_ScopedInstall(RedefineScreenUserInput, 0x57EF50, { .reversed = false });

    RH_ScopedInstall(Process, 0x57B440);
    RH_ScopedInstall(ProcessStreaming, 0x573CF0);
    RH_ScopedInstall(ProcessFileActions, 0x578D60);
    RH_ScopedInstall(ProcessUserInput, 0x57B480, { .reversed = false });
    RH_ScopedInstall(ProcessMenuOptions, 0x576FE0);
    RH_ScopedInstall(ProcessPCMenuOptions, 0x57CD50);
    RH_ScopedInstall(ProcessMissionPackNewGame, 0x57D520);
}

// 0x574350
CMenuManager::CMenuManager() {
    plugin::CallMethod<0x574350>(this);
}

// 0x579440
CMenuManager::~CMenuManager() {
    UnloadTextures();
}

// 0x5744D0
void CMenuManager::Initialise() {
    m_nPlayerNumber = 0;
    field_1B1C = 0;
    m_nCurrentScreenItem = 0;
    m_bSelectedSaveGame = 0;
    if (m_bDoVideoModeUpdate) {
        RwD3D9ChangeMultiSamplingLevels(m_nPrefsAntialiasing);
        SetVideoMode(m_nPrefsVideoMode);
        m_bDoVideoModeUpdate = false;
    }
    CentreMousePointer();
    m_bDrawMouse = false;

    m_nSelectedRow = 3;
    m_nSysMenu     = CMenuSystem::MENU_UNDEFINED;

    m_fMapZoom = FRONTEND_MAP_RANGE_MAX;

    if (m_vMapOrigin.x - FRONTEND_MAP_RANGE_MAX > 0.0f) {
        m_vMapOrigin.x = m_vMapOrigin.x - m_vMapOrigin.x - FRONTEND_MAP_RANGE_MAX;
    }

    if (m_vMapOrigin.x + FRONTEND_MAP_RANGE_MAX < DEFAULT_SCREEN_WIDTH) {
        m_vMapOrigin.x = DEFAULT_SCREEN_WIDTH - m_vMapOrigin.x + FRONTEND_MAP_RANGE_MAX + m_vMapOrigin.x;
    }

    if (m_vMapOrigin.y - FRONTEND_MAP_RANGE_MAX > 0.0f) {
        m_vMapOrigin.y = m_vMapOrigin.y - m_vMapOrigin.y - FRONTEND_MAP_RANGE_MAX;
    }

    if (m_vMapOrigin.y + FRONTEND_MAP_RANGE_MAX < DEFAULT_SCREEN_HEIGHT) {
        m_vMapOrigin.y = DEFAULT_SCREEN_HEIGHT - m_vMapOrigin.y + FRONTEND_MAP_RANGE_MAX + m_vMapOrigin.y;
    }

    CRadar::SetMapCentreToPlayerCoords();
    CPad::StopPadsShaking();
    if (!field_F4) {
        m_nCurrentScreen = SCREEN_INITIAL;
        m_bMapLoaded = true;
        field_1AF8 = 0;
        field_1AFC = 0;
        m_bDrawMouse = false;
    }
    m_nRadioStation = AudioEngine.GetCurrentRadioStationID();
    CFileMgr::SetDir("");
    s_PcSaveHelper.PopulateSlotInfo();
    CTimer::StartUserPause();
}

// 0x572EC0
void CMenuManager::LoadAllTextures() {
    if (m_bTexturesLoaded)
        return;

    CTimer::Stop();

    const auto LoadTexture = [=](unsigned memorySize, const char* slotName, const char* txdName, unsigned start, unsigned end) {
        CStreaming::MakeSpaceFor(memorySize);
        CStreaming::ImGonnaUseStreamingMemory();
        CGame::TidyUpMemory(false, true);
        CTxdStore::PushCurrentTxd();
        auto slot = CTxdStore::FindOrAddTxdSlot(slotName);
        CTxdStore::LoadTxd(slot, txdName);
        CTxdStore::AddRef(slot);
        CTxdStore::SetCurrentTxd(slot);

        for (auto i = start; i < end; i++) {
            const auto t = FrontEndFilenames[i];
            m_aFrontEndSprites[i].SetTexture(t.name, t.alpha);
            m_aFrontEndSprites[i].SetAddressing(rwTEXTUREADDRESSCLAMP);
        }

        CTxdStore::PopCurrentTxd();
        CStreaming::IHaveUsedStreamingMemory();
    };

    LoadTexture(0x21000, "frontend1",   "MODELS\\FRONTEN1.TXD",   FRONTEND1_START, FRONTEND2_START);
    LoadTexture(0x3000,  "frontend2",   "MODELS\\FRONTEN2.TXD",   FRONTEND2_START, FRONTEND3_START);
    LoadTexture(0x6800,  "frontend3",   "MODELS\\FRONTEN3.TXD",   FRONTEND3_START, FRONTEND4_START);       // NOTSA (for SwapTexturesRound)
    LoadTexture(0x4C000, "frontend_pc", "MODELS\\FRONTEN_pc.TXD", FRONTEND4_START, FRONTEND_SPRITE_COUNT); // OG: "MODELS/FRONTEN_pc.TXD"

    m_bTexturesLoaded = true;
    CTimer::Update();
}

// round always false;
// 0x5730A0
void CMenuManager::SwapTexturesRound(bool round) {
    if (m_bTexturesRound == round)
        return; // Swap doesn't needed

    const auto LoadTexture = [](const char* txdName, const char* slotName, unsigned size) {
        CStreaming::MakeSpaceFor(size);
        CStreaming::ImGonnaUseStreamingMemory();
        CTxdStore::PushCurrentTxd();
        auto slot = CTxdStore::FindOrAddTxdSlot(slotName);
        CTxdStore::LoadTxd(slot, txdName);
        CTxdStore::AddRef(slot);
        CTxdStore::SetCurrentTxd(slot);
    };

    m_bTexturesRound = round;

    if (round || !CTxdStore::FindTxdSlot("frontend3")) {
        auto slot = CTxdStore::FindTxdSlot("frontend2");
        std::ranges::for_each(m_apBackgroundTextures, [](auto& sprite) { sprite.Delete(); });

        CTxdStore::RemoveTxd(slot);
        LoadTexture("MODELS/FRONTEN3.TXD", "frontend3", 0x6800);

        for (unsigned i = FRONTEND3_START; i < FRONTEND4_START; i++) {
            m_aFrontEndSprites[i].SetTexture(FrontEndFilenames[i].name, FrontEndFilenames[i].alpha);
            m_aFrontEndSprites[i].SetAddressing(rwTEXTUREADDRESSCLAMP);
        }
    } else {
        auto slot = CTxdStore::FindTxdSlot("frontend3");
        std::ranges::for_each(m_apAdditionalBackgroundTextures, [](auto& sprite) { sprite.Delete(); });

        CTxdStore::RemoveTxd(slot);
        LoadTexture("MODELS/FRONTEN2.TXD", "frontend2", 0x4C000);

        for (unsigned i = FRONTEND2_START; i < FRONTEND3_START; i++) {
            m_aFrontEndSprites[i].SetTexture(FrontEndFilenames[i].name, FrontEndFilenames[i].alpha);
            m_aFrontEndSprites[i].SetAddressing(rwTEXTUREADDRESSCLAMP);
        }
    }
    CTxdStore::PopCurrentTxd();
    CStreaming::IHaveUsedStreamingMemory();
}

// 0x574630
void CMenuManager::UnloadTextures() {
    if (m_bTexturesLoaded) {
        SwapTexturesRound(false);

        const auto Unload = [](const char* slot, auto& sprites) {
            std::ranges::for_each(sprites, [](auto& sprite) { sprite.Delete(); });
            CTxdStore::RemoveTxd(CTxdStore::FindTxdSlot(slot));
        };

        Unload("frontend1",   m_apRadioSprites);
        Unload("frontend2",   m_apBackgroundTextures);
        Unload("frontend3",   m_apAdditionalBackgroundTextures); // NOTSA
        Unload("frontend_pc", m_apMouseTextures);

        m_bTexturesLoaded = false;
    }
}

// 0x573260
void CMenuManager::InitialiseChangedLanguageSettings(bool reinitControls) {
    if (!m_bLanguageChanged)
        return;

    m_bLanguageChanged = false;

    CTimer::Stop();

    char mission[8] = {0};
    TheText.GetNameOfLoadedMissionText(mission);
    TheText.Load(CGame::bMissionPackGame != 0);
    if (mission[0]) {
        TheText.LoadMissionText(mission);
    }

    CTimer::Update();

    auto lang88 = static_cast<eLanguage>(m_nLanguageF0x88);
    if (lang88 != eLanguage::FRENCH && lang88 != eLanguage::GERMAN) {
        switch (m_nPrefsLanguage) {
        case eLanguage::AMERICAN:
        case eLanguage::GERMAN:
        case eLanguage::ITALIAN:
        case eLanguage::SPANISH:
            CLocalisation::SetNormalGame();
            break;
        case eLanguage::FRENCH:
            CLocalisation::SetFrenchGame();
            break;
        default:
            break;
        }
    }

    if (lang88 == eLanguage::FRENCH) {
        CLocalisation::SetFrenchGame();
    }

    if (reinitControls) {
        ControlsManager.ReinitControls();
    }
}

// 0x573CD0
bool CMenuManager::HasLanguageChanged() {
    if (m_nPreviousLanguage == m_nPrefsLanguage)
        return false;

    m_nPreviousLanguage = m_nPrefsLanguage;
    return true;
}

// 0x573330
void CMenuManager::DoSettingsBeforeStartingAGame() {
    std::ranges::fill(CStats::FavoriteRadioStationList, 0);
    if (m_bMenuActive)
        AudioEngine.Reset();

    m_nRadioStation = CAEAudioUtility::GetRandomRadioStation();
    m_bDontDrawFrontEnd = true;
    m_bStartGameLoading = true;

    if (m_nTargetBlipIndex) {
        CRadar::ClearBlip(m_nTargetBlipIndex);
        m_nTargetBlipIndex = 0;
    }

    CStats::UpdateRespectStat(true);
    CCheat::m_bHasPlayerCheated = false;
    CHud::SetZoneName(nullptr, 1);
    TheCamera.SetFadeColour(0, 0, 0);
    TheCamera.Fade(0.0f, eFadeFlag::FADE_IN);
    TheCamera.ProcessFade();
    TheCamera.Fade(2.0f, eFadeFlag::FADE_OUT);
}

// 0x5733E0
float CMenuManager::StretchX(float x) {
    if (SCREEN_WIDTH == DEFAULT_SCREEN_WIDTH)
        return x;
    else
        return SCREEN_STRETCH_X(x);
}

// 0x573410
float CMenuManager::StretchY(float y) {
    if (SCREEN_HEIGHT == DEFAULT_SCREEN_HEIGHT)
        return y;
    else
        return SCREEN_STRETCH_Y(y);
}

// 0x573680
void CMenuManager::SwitchToNewScreen(eMenuScreen screen) {
    //return plugin::CallMethod<0x573680, CMenuManager*, eMenuScreen>(this, screen);

    // Works well, but needs more attention because of the trash gotos
    m_nPrevScreen = m_nCurrentScreen;
    m_nControllerError = 0;

    ResetHelperText();

    if (m_nCurrentScreen == SCREEN_DISPLAY_SETTINGS) {
        m_nDisplayAntialiasing = m_nPrefsAntialiasing;
        m_nDisplayVideoMode    = m_nPrefsVideoMode;
    }

    if (screen == SCREEN_GO_BACK) {
        if (m_bMainMenuSwitch) {
            switch (m_nCurrentScreen) {
            case SCREEN_START_GAME:
                m_nCurrentScreenItem = 0;
                break;
            case SCREEN_OPTIONS:
                m_nCurrentScreenItem = 1;
                break;
            case SCREEN_QUIT_GAME_ASK:
                m_nCurrentScreenItem = 2;
                break;
            default:
                m_nCurrentScreenItem = aScreens[m_nCurrentScreen].m_nStartEntry;
                break;
            }
        } else {
            m_nCurrentScreenItem = aScreens[m_nCurrentScreen].m_nStartEntry;
        }
        m_nCurrentScreen = aScreens[m_nCurrentScreen].m_nParentMenu;
    } else {
        if (screen) {
            m_nCurrentScreenItem = 0;
            m_nCurrentScreen     = screen;
        }
        else {
            m_nCurrentScreenItem = 0;
            m_nCurrentScreen     = aScreens[m_nCurrentScreen].m_aItems[m_nCurrentScreenItem].m_nTargetMenu;
        }
    }

    if (m_nCurrentScreen == SCREEN_GAME_SAVE) {
        m_nCurrentScreenItem = 9;
    }

    if (m_nPrevScreen == SCREEN_AUDIO_SETTINGS) {
        AudioEngine.StopRadio(nullptr, false);
        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_RADIO_RETUNE_STOP);
    }

    if (m_nCurrentScreen == SCREEN_AUDIO_SETTINGS) {
        AudioEngine.StartRadio(m_nRadioStation, 0);
    }

    if (m_bMainMenuSwitch) {
        switch (m_nCurrentScreen) {
        case SCREEN_NEW_GAME_ASK:
            CGame::bMissionPackGame = false;
            DoSettingsBeforeStartingAGame();
            m_bDontDrawFrontEnd = true;
            break;
        case SCREEN_MISSION_PACK_LOADING_ASK:
            CGame::bMissionPackGame = m_nMissionPackGameId;
            DoSettingsBeforeStartingAGame();
            m_bDontDrawFrontEnd = true;
            break;
        case SCREEN_LOAD_GAME_ASK:
            m_nCurrentScreen = SCREEN_LOAD_FIRST_SAVE;
            break;
        }
    }
}

// 0x573A00
void CMenuManager::ScrollRadioStations(int8 numStations) {
    if (!m_nRadioStation || CAudioEngine::IsCutsceneTrackActive()) {
        return;
    }

    m_nRadioStation = static_cast<eRadioID>(m_nRadioStation + numStations);
    if (m_nRadioStation <= RADIO_EMERGENCY_AA) {
        m_nRadioStation = RADIO_OFF;
    }
    if (m_nRadioStation >= RADIO_COUNT) {
        m_nRadioStation = RADIO_CLASSIC_HIP_HOP;
    }
    AudioEngine.RetuneRadio(m_nRadioStation);
    SaveSettings();
}

// 0x573A60
void CMenuManager::SetFrontEndRenderStates() {
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,       RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,      RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEFOGENABLE,         RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATESRCBLEND,          RWRSTATE(rwBLENDSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,         RWRSTATE(rwBLENDINVSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATETEXTUREFILTER,     RWRSTATE(rwFILTERLINEAR));
    RwRenderStateSet(rwRENDERSTATETEXTUREADDRESS,    RWRSTATE(rwTEXTUREADDRESSCLAMP));
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER,     RWRSTATE(NULL));
}

// 0x573AE0
void CMenuManager::SetDefaultPreferences(eMenuScreen screen) {
    switch (screen) {
    case SCREEN_AUDIO_SETTINGS:
        m_nSfxVolume       = 64;
        m_nRadioVolume     = 64;
        field_4E           = 1;
        m_bRadioEq         = true;
        m_bRadioAutoSelect = true;
        m_bTracksAutoScan  = false;
        m_nRadioMode       = 0;
        AudioEngine.SetMusicMasterVolume(m_nRadioVolume);
        AudioEngine.SetEffectsMasterVolume(m_nSfxVolume);
        AudioEngine.SetBassEnhanceOnOff(m_bRadioEq);
        AudioEngine.SetRadioAutoRetuneOnOff(m_bRadioAutoSelect);
        break;
    case SCREEN_DISPLAY_SETTINGS:
    case SCREEN_DISPLAY_ADVANCED:
        g_fx.SetFxQuality(FX_QUALITY_HIGH);
        SetBrightness(256.0f, true);
        m_PrefsBrightness                = 256;
        m_fDrawDistance                  = 1.2f;
        CRenderer::ms_lodDistScale       = 1.2f;
        m_bPrefsFrameLimiter             = true;
        m_bHudOn                         = true;
        m_bSavePhotos                    = true;
        m_bPrefsMipMapping               = true;
        m_nPrefsAntialiasing             = 1;
        m_nDisplayAntialiasing           = 1;
        m_bWidescreenOn                  = false;
        m_bMapLegend                     = false;
        m_nRadarMode                     = eRadarMode::MAPS_AND_BLIPS;
        m_nDisplayVideoMode              = m_nPrefsVideoMode;
        m_ShowLocationsBlips             = true;
        m_ShowContactsBlips              = true;
        m_ShowMissionBlips               = true;
        m_ShowOtherBlips                 = true;
        m_ShowGangAreaBlips              = true;
        m_bShowSubtitles                 = true;
        break;
    case SCREEN_CONTROLLER_SETUP:
        m_nController                    = 0;
        CCamera::m_fMouseAccelHorzntl    = 0.0025f;
        CCamera::m_bUseMouse3rdPerson    = true;
        CVehicle::m_bEnableMouseFlying   = true;
        CVehicle::m_bEnableMouseSteering = false;
        bInvertMouseY                    = false;
        m_bInvertPadX1                   = false;
        m_bInvertPadY1                   = false;
        m_bInvertPadX2                   = false;
        m_bInvertPadY2                   = false;
        m_bSwapPadAxis1                  = false;
        m_bSwapPadAxis2                  = false;
        break;
    default:
        return;
    }
}

// 0x573E70
uint32 CMenuManager::GetNumberOfMenuOptions() {
    // return plugin::CallMethodAndReturn<uint32, 0x573E70, CMenuManager*>(this);

    static int32& s_PrevScreen = *(int32*)0x8CDFF0;

    if (m_nCurrentScreen == SCREEN_MAP || m_nCurrentScreen == SCREEN_BRIEF) {
        return 2;
    }

    // return previously calculated value (caching)
    if (s_PrevScreen == m_nCurrentScreen) {
        return m_nNumberOfMenuOptions;
    }

    const auto Calc = [=]() -> uint8 {
        auto result = 0;
        for (auto& item : aScreens[m_nCurrentScreen].m_aItems) {
            const auto actionType = item.m_nActionType;
            if (!actionType) {
                break;
            }
            if (actionType != MENU_ACTION_TEXT && actionType != MENU_ACTION_SKIP) {
                result++;
            }
        }
        return result;
    };

    s_PrevScreen = m_nCurrentScreen;
    m_nNumberOfMenuOptions = 0;
    m_nNumberOfMenuOptions = Calc();

    return m_nNumberOfMenuOptions;
}

// 0x576AE0
void CMenuManager::JumpToGenericMessageScreen(eMenuScreen screen, const char* titleKey, const char* textKey) {
    // plugin::CallMethod<0x576AE0, CMenuManager*, eMenuPage, const char*, const char*>(this, screen, titleKey, textKey);

    auto& mscreen = aScreens[m_nCurrentScreen];

    SwitchToNewScreen(screen);
    if (screen == SCREEN_GAME_SAVED) {
        mscreen.m_aItems[1].m_nTargetMenu = SCREEN_START_GAME;
    } else if (screen == SCREEN_GAME_LOADED) {
        mscreen.m_aItems[1].m_nTargetMenu = SCREEN_GAME_SAVE;
    }
    strncpy_s(mscreen.m_szTitleName, titleKey, sizeof(mscreen.m_szTitleName));
    strncpy_s(mscreen.m_aItems[0].m_szName, textKey, sizeof(mscreen.m_aItems[0].m_szName));
}

// 0x57C520
void CMenuManager::CentreMousePointer() {
#ifdef FIX_BUGS
    // Not really a vanilla bug, because the vanilla game stops rendering when not in foreground
    if (!IsForegroundApp()) {
        return;
    }
#endif

    CVector2D pos{ SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f };
    if (pos.x != 0.0f && pos.y != 0.0f) {
        RsMouseSetPos(&pos);
    }
}

// 0x57C8F0
void CMenuManager::LoadSettings() {
    CFileMgr::SetDirMyDocuments();

    auto file = CFileMgr::OpenFile("gta_sa.set", "rb");
    if (!file) {
        CFileMgr::SetDir("");
        return;
    }

    const auto SetToDefaultSettings = [&]() {
        SetDefaultPreferences(SCREEN_AUDIO_SETTINGS);
        SetDefaultPreferences(SCREEN_DISPLAY_SETTINGS);
        SetDefaultPreferences(SCREEN_DISPLAY_ADVANCED);
        SetDefaultPreferences(SCREEN_CONTROLLER_SETUP);
        m_nPrefsVideoMode = 0;
        m_nPrefsLanguage = eLanguage::AMERICAN;
        m_nRadioStation = RADIO_CLASSIC_HIP_HOP;

        CFileMgr::CloseFile(file);
        CFileMgr::SetDir("");
    };

    const auto ReadFromFile = [&](auto& ref, size_t size = 0u) {
        CFileMgr::Read(file, &ref, size != 0 ? size : sizeof(ref));
    };

    {
        char buf[29]{0};
        ReadFromFile(buf, 29u);
        if (!strncmp(buf, TopLineEmptyFile, 26u)) {
            return SetToDefaultSettings();
        }

        CFileMgr::Seek(file, 0, 0);
    }

    uint32 version = 0u;
    struct { uint8 m_unk; uint8 m_separator; uint8 m_underscore; uint8 _align; } constants;
    FxQuality_e fxQuality = FX_QUALITY_HIGH;
    auto previousLang = m_nPrefsLanguage;

    ReadFromFile(version);
    if (version < SETTINGS_FILE_VERSION || !ControlsManager.LoadSettings(file)) {
        return SetToDefaultSettings();
    }

    ReadFromFile(CCamera::m_fMouseAccelHorzntl);
    ReadFromFile(bInvertMouseY);
    ReadFromFile(CVehicle::m_bEnableMouseSteering);
    ReadFromFile(CVehicle::m_bEnableMouseFlying);
    ReadFromFile(m_nSfxVolume);
    ReadFromFile(m_nRadioVolume);
    ReadFromFile(m_nRadioStation);
    ReadFromFile(m_bRadioAutoSelect);
    ReadFromFile(m_bRadioEq);
    ReadFromFile(m_PrefsBrightness);
    ReadFromFile(m_bPrefsMipMapping);
    ReadFromFile(m_bTracksAutoScan);
    ReadFromFile(m_nDisplayAntialiasing);
    ReadFromFile(fxQuality);
    ReadFromFile(constants.m_unk);
    ReadFromFile(m_fDrawDistance);
    ReadFromFile(m_bShowSubtitles);
    ReadFromFile(m_bWidescreenOn);
    ReadFromFile(m_bPrefsFrameLimiter);
    ReadFromFile(m_nDisplayVideoMode);
    ReadFromFile(m_nController);
    ReadFromFile(m_nPrefsLanguage);
    ReadFromFile(m_bHudOn);
    ReadFromFile(m_nRadarMode);
    ReadFromFile(m_nRadioMode);
    ReadFromFile(m_bSavePhotos);
    ReadFromFile(constants.m_separator);
    ReadFromFile(m_bInvertPadX1);
    ReadFromFile(m_bInvertPadY1);
    ReadFromFile(m_bInvertPadX2);
    ReadFromFile(m_bInvertPadY2);
    ReadFromFile(m_bSwapPadAxis1);
    ReadFromFile(m_bSwapPadAxis2);
    ReadFromFile(m_bMapLegend);
    ReadFromFile(m_nUserTrackIndex);
    ReadFromFile(m_nCurrentRwSubsystem);
    ReadFromFile(constants.m_underscore);

    if (constants.m_unk != 'T' ||
        constants.m_separator != 0x1D || // ASCII GS - Group Separator
        constants.m_underscore != '_'
    ) {
        return SetToDefaultSettings();
    }

    CCamera::m_bUseMouse3rdPerson = m_nController == 0;
    CRenderer::ms_lodDistScale = m_fDrawDistance;
    g_fx.SetFxQuality(fxQuality);
    SetBrightness(static_cast<float>(m_PrefsBrightness), true);
    m_nPrefsAntialiasing = m_nDisplayAntialiasing;
    m_bDoVideoModeUpdate = true;
    AudioEngine.SetMusicMasterVolume(m_nRadioVolume);
    AudioEngine.SetEffectsMasterVolume(m_nSfxVolume);
    AudioEngine.SetBassEnhanceOnOff(m_bRadioEq);
    AudioEngine.SetRadioAutoRetuneOnOff(m_bRadioAutoSelect);
    AudioEngine.RetuneRadio(m_nRadioStation);

    if (previousLang == m_nPrefsLanguage) {
        field_8C = false;
    } else {
        field_8C = true;
        TheText.Load(false);
        m_bLanguageChanged = true;
        InitialiseChangedLanguageSettings(false);
        OutputDebugStringA("The previously saved language is now in use"); // SA
    }

    CFileMgr::CloseFile(file);
    CFileMgr::SetDir("");
}

// 0x57C660
void CMenuManager::SaveSettings() {
    CFileMgr::SetDirMyDocuments();

    auto file = CFileMgr::OpenFile("gta_sa.set", "w+b");
    if (!file) {
        CFileMgr::SetDir("");
        return;
    }

    const auto WriteToFile = [&](auto&& v, size_t size = 0u) {
        CFileMgr::Write(file, &v, size != 0 ? size : sizeof(v));
    };

    WriteToFile(SETTINGS_FILE_VERSION);
    ControlsManager.SaveSettings(file);
    WriteToFile(CCamera::m_fMouseAccelHorzntl);
    WriteToFile(bInvertMouseY);
    WriteToFile(CVehicle::m_bEnableMouseSteering);
    WriteToFile(CVehicle::m_bEnableMouseFlying);
    WriteToFile(m_nSfxVolume);
    WriteToFile(m_nRadioVolume);
    WriteToFile(m_nRadioStation);
    WriteToFile(m_bRadioAutoSelect);
    WriteToFile(m_bRadioEq);
    WriteToFile(m_PrefsBrightness);
    WriteToFile(m_bPrefsMipMapping);
    WriteToFile(m_bTracksAutoScan);
    WriteToFile(m_nPrefsAntialiasing);
    WriteToFile(g_fx.GetFxQuality());
    WriteToFile(int8(84)); // T
    WriteToFile(m_fDrawDistance);
    WriteToFile(m_bShowSubtitles);
    WriteToFile(m_bWidescreenOn);
    WriteToFile(m_bPrefsFrameLimiter);
    WriteToFile(m_nPrefsVideoMode);
    WriteToFile(m_nController);
    WriteToFile(m_nPrefsLanguage);
    WriteToFile(m_bHudOn);
    WriteToFile(m_nRadarMode);
    WriteToFile(m_nRadioMode);
    WriteToFile(m_bSavePhotos);
    WriteToFile(int8(29)); // GS - Group Separator
    WriteToFile(m_bInvertPadX1);
    WriteToFile(m_bInvertPadY1);
    WriteToFile(m_bInvertPadX2);
    WriteToFile(m_bInvertPadY2);
    WriteToFile(m_bSwapPadAxis1);
    WriteToFile(m_bSwapPadAxis2);
    WriteToFile(m_bMapLegend);
    WriteToFile(m_nUserTrackIndex);
    WriteToFile(RwEngineGetCurrentSubSystem());
    WriteToFile(int8(95)); // _ underscore

    CFileMgr::CloseFile(file);
    CFileMgr::SetDir("");
}

// 0x57DDE0
void CMenuManager::SaveStatsToFile() {
    CFileMgr::SetDirMyDocuments();
    char date[12]{};
    _strdate_s(date, 12u);

    const GxtChar* lastMissionPassedKey = TheText.Get("ITBEG"); // In the beginning
    if (CStats::LastMissionPassedName[0]) {
        lastMissionPassedKey = TheText.Get(CStats::LastMissionPassedName);
    }

    auto file = CFileMgr::OpenFile("stats.html", "w");
    const auto End = [&]() {
        CFileMgr::SetDir("");
        CFileMgr::CloseFile(file); // FIX_BUGS
        m_nHelperText = FEA_STS; // STATS SAVED TO 'STATS.HTML'
        m_nHelperTextFadingAlpha = 300;
    };
    if (!file) {
        End();
        return;
    }

    const auto ToUpperCase = [](const char* s) {
        // HACK: This seems to be the cleanest way...
        auto wide = UTF8ToUnicode(s);
        rng::transform(wide, wide.begin(), towupper);
        return UnicodeToUTF8(wide);
    };

    // FIX_BUGS: Use UTF-8 instead of ANSI.
    fprintf_s(file, "<meta charset=\"UTF-8\"/>\n");

    fprintf_s(file, "<title>Grand Theft Auto San Andreas Stats</title>\n");
    fprintf_s(file, "<body bgcolor=\"#000000\" leftmargin=\"10\" topmargin=\"10\" marginwidth=\"10\" marginheight=\"10\">\n");
    fprintf_s(file, "<table width=\"560\" align=\"center\" border=\"0\" cellpadding=\"5\" cellspacing=\"0\">\n"
                  "<tr align=\"center\" valign=\"top\"> \n"
                  "<td height=\"59\" colspan=\"2\" bgcolor=\"#000000\"><div align=\"center\"><font color=\"#FFFFFF\" size=\"5\" face=\"Arial, \n");
    fprintf_s(file, "Helvetica, sans-serif\">-------------------------------------------------------------------</font><font \nsize=\"5\" face=\"Arial, Helvetica, sans-serif\"><br>\n");
    fprintf_s(file, "<strong><font color=\"#FFFFFF\">GRAND THEFT AUTO SAN ANDREAS ");
    fprintf_s(file, "%s</font></strong><br><font\n", ToUpperCase(GxtCharToUTF8(TheText.Get("FEH_STA"))).c_str()); // Stats
    fprintf_s(file, "color=\"#FFFFFF\">-------------------------------------------------------------------</font></font></div></td> </tr>\n");
    fprintf_s(file, "<tr align=\"center\" valign=\"top\" bgcolor=\"#000000\">     <td height=\"22\" colspan=\"2\">&nbsp;</td>  </tr>\n"
                    "<tr align=\"center\" valign=\"top\" bgcolor=\"#000000\"> \n");
    fprintf_s(file, R"(<td height="40" colspan="2"> <p><font color="#F0000C" size="2" face="Arial, Helvetica, sans-serif"><stro)");
    fprintf_s(file, "ng><font color=\"#F0000C\" size=\"1\">%s: \n", GxtCharToUTF8(TheText.Get("FES_DAT"), 0u)); // DATE
    fprintf_s(file, "%s</font><br>        %s: </strong>", date, GxtCharToUTF8(TheText.Get("FES_CMI"), 0u));     // LAST MISSION PASSED
    fprintf_s(file, "%s<strong><br></strong> </font></p></td></tr>\n", ToUpperCase(GxtCharToUTF8(lastMissionPassedKey)).c_str());
    fprintf_s(file, "<tr align=\"center\" valign=\"top\" bgcolor=\"#000000\"> <td height=\"5\" colspan=\"2\"></td> </tr> <tr align=\"center\" valign=\"top\" bgcolor=\"#000000\"> \n"
                  "<td height=\"10\" colspan=\"2\"></td> </tr> <tr align=\"center\" valign=\"top\" bgcolor=\"#000000\"> \n");
    fprintf_s(file, R"(<td height="20" colspan="2"><font color="#F0000C" size="2" face="Arial, Helvetica, sans-serif">)");
    fprintf_s(file, "<strong> %s</strong>\n ", GxtCharToUTF8(TheText.Get("CRIMRA"), 0u)); // Criminal rating:
    TextCopy(gGxtString, CStats::FindCriminalRatingString());
    fprintf_s(file, "%s (%d)</font></td>  </tr>", GxtCharToUTF8(gGxtString, 0u), CStats::FindCriminalRatingNumber());
    fprintf_s(file, "<tr align=\"left\" valign=\"top\" bgcolor=\"#000000\"><td height=\"10\" colspan=\"2\"></td>  </tr>\n");

    static constexpr const char* strToPrint[] = {
        "FES_PLA", "FES_MON", "FES_WEA", "FES_GAN",
        "FES_CRI", "FES_ACH", "FES_MIS", "FES_MSC"
    };
    for (auto menuItem = 0u; menuItem < 8u; menuItem++) {
        auto numStatLines = CStats::ConstructStatLine(99'999, menuItem);

        fprintf_s(file, "</font></strong></div></td> </tr> <tr align=\"left\" valign=\"top\" bgcolor=\"#000000\">  <td height=\"25\" colspan=\"2\"></td> </tr>\n"
                      "<tr align=\"left\" valign=\"top\"><td height=\"30\" bgcolor=\"#000000\"><font color=\"#009900\" size=\"4\" face=\"Arial, Helvetica, sans-serif\"><strong>\n");

        fprintf_s(file, "%s", GxtCharToUTF8(TheText.Get(strToPrint[menuItem]), 0u));
        fprintf_s(file, "</strong></font></td> <td width=\"500\" align=\"right\" valign=\"middle\" bgcolor=\"#000000\"> <div align=\"right\"><strong><font color=\"#FF0CCC\">\n");
        if (numStatLines <= 0)
            continue;

        for (auto stat = 0; stat < numStatLines; stat++) {
            CStats::ConstructStatLine(stat, menuItem);

            auto str = GxtCharToUTF8(gGxtString, 0u);
            if (*str) {
                fprintf_s(file, "</font></strong></div></td> </tr> <tr align=\"left\" valign=\"top\" bgcolor=\"#000000\">  <td height=\"10\" colspan=\"2\"></td> </tr>\n");
            }

            fprintf_s(file, "<tr align=\"left\" valign=\"top\"><td width=\"500\" height=\"22\" bgcolor=\"#555555\"><font color=\"#FFFFFF\" size=\"2\" face=\"Arial, Helvetica, sans-serif\"><strong>\n");
            fprintf_s(file, "%s", (*str) ? str : " ");
            fprintf_s(file, "</strong></font></td> <td width=\"500\" align=\"right\" valign=\"middle\" bgcolor=\"#555555\"> <div align=\"right\"><strong><font color=\"#FFFFFF\">\n");
            auto val = GxtCharToUTF8(gGxtString2, 0u);
            auto valFormatted = (char*)val;

            // todo. xref: CStats::ConstructStatLine, PrintStats
            static uint16& unk = *reinterpret_cast<uint16*>(0xB794CC);
            if (unk) { // stat line formatted in percents?
                sprintf_s(valFormatted, 5u, "%0.0f%%", std::min(atoi(val) / 10.0f, 100.0f)); // max length: "100%\0"
            }

            for (auto v = valFormatted; *v; v++) {
                if (*v == '|') {
                    *v = -70; // double vertical bar
                }
            }
            fprintf_s(file, "%s", valFormatted);
        }
    }

    End();
}

// 0x57C490
// PS2 leftover?
void CMenuManager::SaveLoadFileError_SetUpErrorScreen() {
    switch (s_PcSaveHelper.error) {
    case C_PcSave::eErrorCode::E1:
    case C_PcSave::eErrorCode::E2:
    case C_PcSave::eErrorCode::E3:
        SwitchToNewScreen(SCREEN_SAVE_GAME_FAILED);
        break;
    case C_PcSave::eErrorCode::FAILED_TO_OPEN:
    case C_PcSave::eErrorCode::FAILED_TO_READ:
    case C_PcSave::eErrorCode::FAILED_TO_CLOSE:
        SwitchToNewScreen(SCREEN_SAVE_WRITE_FAILED);
        break;
    case C_PcSave::eErrorCode::SLOT_INVALID:
        SwitchToNewScreen(SCREEN_SAVE_FAILED_FILE_ERROR);
        break;
    case C_PcSave::eErrorCode::E8:
    case C_PcSave::eErrorCode::E9:
    case C_PcSave::eErrorCode::E10:
        SwitchToNewScreen(SCREEN_SAVE_GAME_DONE);
        break;
    default:
        return;
    }
}

// 0x57E240
void CMenuManager::DisplayHelperText(const char* key) {
    switch (m_nCurrentScreen) {
    case SCREEN_MAP:
        CFont::SetScale(StretchY(0.4f), StretchY(0.5f));
        break;
    default:
        CFont::SetScale(StretchY(0.4f), StretchY(0.6f));
        break;
    }

    CFont::SetFontStyle(eFontStyle::FONT_MENU);
    CFont::SetOrientation(eFontAlignment::ALIGN_RIGHT);
    CFont::SetEdge(0);

    float x = 610.f;
    float y = 10.f;

    if (key) {
        CFont::SetColor({ 255, 255, 255, 255 });
        CFont::PrintStringFromBottom(StretchX(x), SCREEN_HEIGHT - StretchY(y), TheText.Get(key));
        return;
    }

    uint8 alpha = 255;
    if (m_nHelperText && m_nHelperText != 1) {
        if (CTimer::GetTimeInMSPauseMode() - m_nTimeHelperTextUpdated > 10) {
            m_nTimeHelperTextUpdated = CTimer::GetTimeInMSPauseMode();
            m_nHelperTextFadingAlpha -= 2;
        }
        if (m_nHelperTextFadingAlpha < 1) {
            ResetHelperText();
        }
        alpha = std::min(m_nHelperTextFadingAlpha, 255);
    }

    CFont::SetColor(CRGBA(255, 255, 255, alpha));

    const GxtChar* text{};
    switch (m_nHelperText) {
    case FET_APP:
        text = TheText.Get("FET_APP"); // CLICK LMB / RETURN - APPLY NEW SETTING
        break;
    case FET_HRD:
        text = TheText.Get("FET_HRD"); // DEFAULT SETTINGS RESTORED
        break;
    case FET_RSO:
        text = TheText.Get("FET_RSO"); // ORIGINAL SETTING RESTORED
        break;
    case FEA_SCF:
        text = TheText.Get("FEA_SCF"); // FAILED TO SCAN USER TRACKS
        break;
    case FEA_SCS:
        text = TheText.Get("FEA_SCS"); // USER TRACKS SCANNED SUCCESSFULLY
        break;
    case FEA_STS:
        text = TheText.Get("FET_STS"); // STATS SAVED TO 'STATS.HTML'
        break;
    default:
        text = nullptr;
        break;
    }

    if (text) {
        CFont::PrintStringFromBottom(StretchX(x), SCREEN_HEIGHT - StretchY(y), text);
        return;
    }

    switch (aScreens[m_nCurrentScreen].m_aItems[m_nCurrentScreenItem].m_nActionType) {
    case MENU_ACTION_BACK:
        text = TheText.Get("FEH_BPO"); // CLICK LMB / RETURN - BACK
        break;
    case MENU_ACTION_MENU:
    case MENU_ACTION_CTRLS_JOYPAD:
    case MENU_ACTION_CTRLS_FOOT:
    case MENU_ACTION_CTRLS_CAR:
        text = TheText.Get("FEH_JMP"); // CLICK LMB / RETURN - ENTER MENU
        break;
    case MENU_ACTION_USER_TRACKS_SCAN:
        text = TheText.Get("FEH_SNC"); // CLICK LMB / RETURN - SCAN USER TRACKS
        break;
    case MENU_ACTION_RESOLUTION:
        text = TheText.Get("FET_MIG"); // LEFT / RIGHT / MOUSEWHEEL - ADJUST
        break;
    default:
        break;
    }

    if (text) {
        CFont::PrintStringFromBottom(StretchX(x), SCREEN_HEIGHT - StretchY(y), text);
        return;
    }

    switch (m_nCurrentScreen) {
    case SCREEN_STATS:
        text = TheText.Get("FEH_SSA"); // CURSORS - MOVE~n~S - SAVE TO FILE
        break;
    case SCREEN_MAP:
        text = TheText.Get("FET_MIG"); // LEFT / RIGHT / MOUSEWHEEL - ADJUST
        y = 2.0f;
        break;
    default:
        text = TheText.Get("FET_MIG"); // LEFT / RIGHT / MOUSEWHEEL - ADJUST
        break;
    }

    CFont::PrintStringFromBottom(StretchX(x), SCREEN_HEIGHT - StretchY(y), text);
}

// 0x57CD10
void CMenuManager::SetHelperText(eHelperText messageId) {
    m_nHelperText            = messageId;
    m_nHelperTextFadingAlpha = 300;
}

// 0x57CD30
void CMenuManager::ResetHelperText() {
    m_nHelperText            = HELPER_NONE;
    m_nHelperTextFadingAlpha = 300;
}

// 0x57C5E0
void CMenuManager::NoDiskInDriveMessage() {
    DoRWStuffStartOfFrame(0, 0, 0, 0, 0, 0, 0);
    if (RsGlobal.quit) {
        return;
    }
    MessageLoop();
    CPad::UpdatePads();
    MessageScreen("NO_PCCD", true, false);
    CFont::DrawFonts();
    DoRWStuffEndOfFrame();
    if (CPad::IsEscJustPressed()) {
        m_bQuitGameNoDVD = true;
        RsEventHandler(rsQUITAPP, NULL);
    }
}

// 0x579330
void CMenuManager::MessageScreen(const char* key, bool blackBackground, bool cameraUpdateStarted) {
    const CRect fullscreen = CRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    if (!cameraUpdateStarted) {
        if (!RsCameraBeginUpdate(Scene.m_pRwCamera))
            return;

        if (blackBackground) {
            CSprite2d::DrawRect(fullscreen, { 0, 0, 0, 255 });
        }
    }

    CSprite2d::SetRecipNearClip();
    CSprite2d::InitPerFrame();
    CFont::InitPerFrame();
    DefinedState2d();

    if (blackBackground) {
        CSprite2d::DrawRect(fullscreen, { 0, 0, 0, 255 });
    }

    SmallMessageScreen(key);
    CFont::RenderFontBuffer();
}

// 0x574010
void CMenuManager::SmallMessageScreen(const char* key) {
    CFont::SetBackground(false, false);
    CFont::SetProportional(true);
    CFont::SetFontStyle(eFontStyle::FONT_SUBTITLES);
    CFont::SetCentreSize(SCREEN_SCALE_X(430.0f));
    CFont::SetOrientation(eFontAlignment::ALIGN_CENTER);
    CFont::SetColor(HudColour.GetRGB(HUD_COLOUR_LIGHT_GRAY));
    CFont::SetDropShadowPosition(0);
    CFont::SetScale(StretchX(0.56f), StretchY(1.0f));

    DrawWindow(
        CRect(
            StretchX(95.0f),
            StretchY(125.0f),
            SCREEN_WIDTH - StretchX(95.0f),
            SCREEN_HEIGHT - StretchY(165.0f)
        ),
        nullptr,
        0,
        { 0, 0, 0, 255 },
        false,
        true
    );

    float x = StretchX(320.0f);
    float y = StretchY(190.0f);

    const auto lines = CFont::GetNumberLines(StretchX(320.0f), StretchX(135.0f), TheText.Get(key));
    if (lines > 1) {
        y = StretchY(float(200 - lines * 9));
    }

    auto text = TheText.Get(key);
    if (!TheText.m_bCdErrorLoaded) {
        for (auto& k : { "NOCD", "OPENCD", "WRONGCD", "CDERROR" }) {
            if (!_stricmp(k, key)) {
                // FIX_BUGS: AsciiToGxtChar <-> GxtCharToAscii (then UTF-8)
                GxtCharToUTF8(TheText.m_szCdErrorText, text);
            }
        }
    }
    CFont::PrintString(x, y, text);
}

//! NOTSA
void CMenuManager::SimulateGameLoad(bool newGame, uint32 slot) {
    m_bDontDrawFrontEnd     = newGame;
    m_bSelectedSaveGame     = slot;
    CGame::bMissionPackGame = false;
    if (newGame) {
        DoSettingsBeforeStartingAGame();
    } else {
        m_nCurrentScreen = SCREEN_LOAD_FIRST_SAVE;
        field_1B3C = true;
    } 
}

// NOTSA
void CMenuManager::SetBrightness(float brightness, bool arg2) {
    gamma.SetGamma(brightness / 512.0f, arg2);
}
