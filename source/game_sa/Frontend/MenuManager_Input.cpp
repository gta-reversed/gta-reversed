#include "StdInc.h"

#include "MenuManager.h"
#include "MenuManager_Internal.h"
#include "MenuSystem.h"
#include "app/app.h"
#include "VideoMode.h" // todo
#include "ControllerConfigManager.h"

#include "extensions/Configs/FastLoader.hpp"

/*!
 * @addr 0x57FD70
 */
void CMenuManager::UserInput() {
    { // NOTSA
    const auto pad = CPad::GetPad();
    if (pad->IsStandardKeyJustPressed('q') || pad->IsStandardKeyJustPressed('Q')) {
        CFont::PrintString(50, 250, "switched"_gxt);
        ReversibleHooks::SwitchHook("DisplaySlider");
    }
    }

    plugin::CallMethod<0x57FD70, CMenuManager*>(this);
}

/*!
 * @addr 0x57B480
 */
void CMenuManager::ProcessUserInput(bool downPressed, bool upPressed, bool acceptPressed, bool cancelPressed, int8 pressedLR) {
    plugin::CallMethod<0x57B480, CMenuManager*, bool, bool, bool, bool, int8>(this, downPressed, upPressed, acceptPressed, cancelPressed, pressedLR);
}

/*!
 * @addr 0x5773D0
 */
void CMenuManager::AdditionalOptionInput(bool* upPressed, bool* downPressed) {
    plugin::CallMethod<0x5773D0, CMenuManager*, bool*, bool*>(this, upPressed, downPressed);
}

/*!
 * @addr 0x57EF50
 */
void CMenuManager::RedefineScreenUserInput(bool* accept, bool* cancel) {
    plugin::CallMethod<0x57EF50, CMenuManager*, bool*, bool*>(this, accept, cancel);
}

/*!
 * @addr 0x57E4D0
 */
bool CMenuManager::CheckRedefineControlInput() {
    if (field_1B09) {
        if (m_bJustOpenedControlRedefWindow) {
            m_bJustOpenedControlRedefWindow = false;
        } else {
            GetCurrentKeyPressed(*m_pPressedKey);
            m_nPressedMouseButton = (RsKeyCodes)0;
            m_nJustDownJoyButton = 0;

            auto pad = CPad::GetPad();
            if (pad->IsMouseLButtonPressed()) {
                m_nPressedMouseButton = rsMOUSE_LEFT_BUTTON;
            } else if (pad->IsMouseRButtonPressed()) {
                m_nPressedMouseButton = rsMOUSE_RIGHT_BUTTON;
            } else if (pad->IsMouseMButtonPressed()) {
                m_nPressedMouseButton = rsMOUSE_MIDDLE_BUTTON;
            } else if (pad->IsMouseWheelUpPressed()) {
                m_nPressedMouseButton = rsMOUSE_WHEEL_UP_BUTTON;
            } else if (pad->IsMouseWheelDownPressed()) {
                m_nPressedMouseButton = rsMOUSE_WHEEL_DOWN_BUTTON;
            } else if (pad->IsMouseBmx1Pressed()) {
                m_nPressedMouseButton = rsMOUSE_X1_BUTTON;
            } else if (pad->IsMouseBmx2Pressed()) {
                m_nPressedMouseButton = rsMOUSE_X2_BUTTON;
            }
            m_nJustDownJoyButton = ControlsManager.GetJoyButtonJustDown();

            auto type = rsKEYBOARD;
            if (m_nPressedMouseButton && *m_pPressedKey == rsNULL) {
                type = rsPAD;
            }

            if (field_1B14) {
                if (field_1B0A) {
                    AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_SELECT);
                    field_1B14 = 0;
                    field_1B0B = 1;
                    field_1B0A = 0;
                } else {
                    if (*m_pPressedKey != rsNULL || m_nPressedMouseButton || m_nJustDownJoyButton) {
                        CheckCodesForControls(type);
                    }
                    field_1B15 = 1;
                }
            } else {
                m_pPressedKey = nullptr;
                field_1B09 = 0;
                field_38 = (RsKeyCodes)-1;
                m_bJustOpenedControlRedefWindow = false;
            }
        }
    }
    return field_EC || m_pPressedKey;
}

// value: -1 or 1
// 0x573440
void CMenuManager::CheckSliderMovement(int8 value) {
    tMenuScreen* screen   = &aScreens[m_nCurrentScreen];
    tMenuScreenItem* item = &screen->m_aItems[m_nCurrentScreenItem];

    switch (item->m_nActionType) {
    case MENU_ACTION_BRIGHTNESS:
#ifdef FIX_BUGS
        m_PrefsBrightness += value * (384 / 16); // todo:
#else
        m_PrefsBrightness += value * 24.19f;
#endif
        m_PrefsBrightness = std::clamp(m_PrefsBrightness, 0, 384);

        SetBrightness((float)m_PrefsBrightness, false);
        break;
    case MENU_ACTION_RADIO_VOL: {
        m_nRadioVolume += int8(4) * value;
        m_nRadioVolume = std::clamp<int8>(m_nRadioVolume, 0, 64);
        AudioEngine.SetMusicMasterVolume(m_nRadioVolume);
        SaveSettings();
        break;
    }
    case MENU_ACTION_SFX_VOL: {
        m_nSfxVolume += int8(4) * value;
        m_nSfxVolume = std::clamp<int8>(m_nSfxVolume, 0, 64);
        AudioEngine.SetEffectsMasterVolume(m_nSfxVolume);
        SaveSettings();
        break;
    }
    case MENU_ACTION_DRAW_DIST: {
        constexpr auto step = 7.0f / 128.0f;
        float newDist = value <= 0 ? m_fDrawDistance - step : m_fDrawDistance + step;
        m_fDrawDistance = std::clamp(newDist, 0.925f, 1.8f);

        CRenderer::ms_lodDistScale = m_fDrawDistance;
        SaveSettings();
        break;
    }
    case MENU_ACTION_MOUSE_SENS: {
        // todo: [0.00813f; 0.0055f]; min value is changed by 0x84ED80.

        static float& minMouseAccel = *reinterpret_cast<float*>(0xBA672C);

        float val = (float)value / 3000.0f + CCamera::m_fMouseAccelHorzntl;
        CCamera::m_fMouseAccelHorzntl = std::clamp(val, minMouseAccel, 0.005f);

#ifdef FIX_BUGS
        CCamera::m_fMouseAccelVertical = CCamera::m_fMouseAccelHorzntl;
#endif
        SaveSettings();
        break;
    }
    default:
        return;
    }
}

// 0x573840
bool CMenuManager::CheckFrontEndUpInput() const {
    auto pad = CPad::GetPad(m_nPlayerNumber);
    return CPad::IsUpPressed() || CPad::GetAnaloguePadUp() || pad->IsDPadUpPressed();
}

// 0x5738B0
bool CMenuManager::CheckFrontEndDownInput() const {
    auto pad = CPad::GetPad(m_nPlayerNumber);
    return CPad::IsDownPressed() || CPad::GetAnaloguePadDown() || pad->IsDPadDownPressed();
}

// 0x573920
bool CMenuManager::CheckFrontEndLeftInput() const {
    auto pad = CPad::GetPad(m_nPlayerNumber);
    return CPad::IsLeftPressed() || CPad::GetAnaloguePadLeft() || pad->IsDPadLeftPressed();
}

// 0x573990
bool CMenuManager::CheckFrontEndRightInput() const {
    auto pad = CPad::GetPad(m_nPlayerNumber);
    return CPad::IsRightPressed() || CPad::GetAnaloguePadRight() || pad->IsDPadRightPressed();
}

// 0x576B70
void CMenuManager::CheckForMenuClosing() {
    const auto CanActivateMenu = [&]() -> bool {
        if (m_bDontDrawFrontEnd || m_bActivateMenuNextFrame) {
            return true;
        }

        if (m_bMenuActive) {
            switch (m_nCurrentScreen) {
            case SCREEN_PAUSE_MENU:
            case SCREEN_GAME_SAVE:
            case SCREEN_GAME_WARNING_DONT_SAVE:
                break;
            default:
                return false;
            }
        }

        if (!CPad::IsEscJustPressed()) {
            return false;
        }

        if (CReplay::Mode == MODE_PLAYBACK) {
            return false;
        }

        if (TheCamera.m_bWideScreenOn && !m_bMenuAccessWidescreen) {
            return false;
        }
        return true;
    };

    if (CanActivateMenu()) {
        if (!AudioEngine.IsRadioRetuneInProgress()) {
            switch (m_nCurrentScreen) {
            case SCREEN_SAVE_DONE_1:
            case SCREEN_DELETE_FINISHED:
                m_bDontDrawFrontEnd = false;
                return;
            default:
                break;
            }

            if ((!field_35 || !m_bActivateMenuNextFrame) && !m_bLoadingData) {
                AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_START);
                if (!g_FastLoaderConfig.ShouldLoadSaveGame()) { // If loading, skip menu audio
                    AudioEngine.Service();
                }
            }

            m_bMenuActive = !m_bMenuActive;

            if (m_bDontDrawFrontEnd) {
                m_bMenuActive = false;
            }

            if (m_bActivateMenuNextFrame) {
                m_bMenuActive = true;
            }

            if (m_bMenuActive) {
                if (!field_F4) {
                    // enter menu
                    DoRWStuffStartOfFrame(0, 0, 0, 0, 0, 0, 255);
                    DoRWStuffEndOfFrame();
                    DoRWStuffStartOfFrame(0, 0, 0, 0, 0, 0, 255);
                    DoRWStuffEndOfFrame();

                    auto pad = CPad::GetPad(m_nPlayerNumber);
                    field_1B34 = pad->DisablePlayerControls;
                    pad->Clear(false, true);
                    pad->ClearKeyBoardHistory();
                    pad->ClearMouseHistory();

                    if (IsVideoModeExclusive()) {
                        DIReleaseMouse();
                        InitialiseMouse(false);
                    }
                    Initialise();
                    LoadAllTextures();

                    SetBrightness((float)m_PrefsBrightness, true);
                }
            } else {
                AudioEngine.StopRadio(nullptr, false);
                AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_RADIO_RETUNE_STOP);

                if (m_nSysMenu >= 0u) {
                    CMenuSystem::SwitchOffMenu(0);
                    m_nSysMenu = 157;
                }

                auto pad = CPad::GetPad(m_nPlayerNumber);
                pad->Clear(false, true);
                pad->ClearKeyBoardHistory();
                pad->ClearMouseHistory();

                if (IsVideoModeExclusive()) {
                    DIReleaseMouse();
#ifdef FIX_BUGS // Causes the retarded fucktard code to not dispatch mouse input to WndProc => ImGUI mouse not working. Amazing piece of technology.
                    InitialiseMouse(false);
#else
                    InitialiseMouse(true);
#endif // !FIX_BUGS
                }

                m_fStatsScrollSpeed = 150.0f;
                SaveSettings();
                m_pPressedKey = nullptr;
                field_EC = 0;
                field_1AE8 = 0;
                m_bDontDrawFrontEnd = false;
                m_bActivateMenuNextFrame = false;
                field_1B09 = 0;
                m_bIsSaveDone = false;
                UnloadTextures();

                CTimer::EndUserPause();
                CTimer::Update();

                pad->JustOutOfFrontEnd = true;
                pad->LastTimeTouched = 0;
                CPad::GetPad(1)->LastTimeTouched = 0;

                SetBrightness((float)m_PrefsBrightness, true);

                if (field_F4) {
                    auto player = FindPlayerPed();

                    if (player->GetActiveWeapon().m_Type != WEAPON_CAMERA
                        || CTimer::GetTimeInMS() >= player->GetActiveWeapon().m_TimeForNextShotMs) {
                        TheCamera.SetFadeColour(0u, 0u, 0u);
                        TheCamera.Fade(0.0f, eFadeFlag::FADE_IN);
                        TheCamera.ProcessFade();
                        TheCamera.Fade(0.2f, eFadeFlag::FADE_OUT);
                    }
                }
                field_F4 = false;
                pad->DisablePlayerControls = field_1B34;
            }
        }
    }

    if (m_bIsSaveDone) {
        // enter menu 2
        DoRWStuffStartOfFrame(0, 0, 0, 0, 0, 0, 255);
        DoRWStuffEndOfFrame();
        DoRWStuffStartOfFrame(0, 0, 0, 0, 0, 0, 255);
        DoRWStuffEndOfFrame();

        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_START);

        if (!g_FastLoaderConfig.ShouldLoadSaveGame()) { // If loading, skip menu audio
            AudioEngine.Service();
        }

        auto pad = CPad::GetPad(m_nPlayerNumber);
        field_1B34 = pad->DisablePlayerControls;
        pad->DisablePlayerControls = true;
        m_bIsSaveDone = false;
        m_bMenuActive = true;
        field_F4 = true;

        if (IsVideoModeExclusive()) {
            DIReleaseMouse();
            InitialiseMouse(false);
        }

        Initialise();
        LoadAllTextures();

        m_nCurrentScreenItem = 0;

        m_nCurrentScreen = (!CCheat::m_bHasPlayerCheated) ? SCREEN_GAME_SAVE : SCREEN_GAME_WARNING_DONT_SAVE;
    }

    m_bActivateMenuNextFrame = false;
    m_bDontDrawFrontEnd = false;
}

// 0x57C4F0
bool CMenuManager::CheckHover(int32 left, int32 right, int32 top, int32 bottom) const {
    // debug: CSprite2d::DrawRect(CRect(left, top, right, bottom), CRGBA(255, 0, 0, 255));
    return (
        m_nMousePosX > left  &&
        m_nMousePosX < right &&
        m_nMousePosY > top   &&
        m_nMousePosY < bottom
    );
}

// 0x57D720
bool CMenuManager::CheckMissionPackValidMenu() {
    CFileMgr::SetDirMyDocuments();

    sprintf_s(gString, "MPACK//MPACK%d//SCR.SCM", CGame::bMissionPackGame);
    auto scr = CFileMgr::OpenFile(gString, "rb");
    sprintf_s(gString, "MPACK//MPACK%d//TEXT.GXT", CGame::bMissionPackGame);
    auto gxt = CFileMgr::OpenFile(gString, "rb");

    CFileMgr::SetDir("");

    if (!scr) {
        if (gxt) {
            CFileMgr::CloseFile(gxt);
        }
        CTimer::StartUserPause();

        while (true) {
            MessageLoop();
            CPad::UpdatePads();

            //                 Load failed!
            //  The current Mission Pack is not available.
            // Please recheck that the current Mission Pack
            //          is installed correctly!
            //
            //   Press RETURN to start a new standard game.
            CMenuManager::MessageScreen("NO_PAK", true, false);

            DoRWStuffStartOfFrame(0, 0, 0, 0, 0, 0, 0);
            DoRWStuffEndOfFrame();
            auto pad = CPad::GetPad();

            if (CPad::IsEnterJustPressed() || CPad::IsReturnJustPressed() || pad->IsCrossPressed())
                break;
        }
        CTimer::EndUserPause();
        CGame::bMissionPackGame = false;
        DoSettingsBeforeStartingAGame();
        m_bActivateMenuNextFrame = false;

        return false;
    }

    CFileMgr::CloseFile(scr);
    return true;
}

// 0x57DB20
bool CMenuManager::CheckCodesForControls(RsInputDeviceType type) {
    return plugin::CallMethodAndReturn<bool, 0x57DB20, CMenuManager*, RsInputDeviceType>(this, type);
}
