#include "StdInc.h"

#include <SDL3/SDL.h>

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
    // NOTSA section kept as original
    {
        const auto pad = CPad::GetPad();
        if (pad->IsStandardKeyJustPressed('q') || pad->IsStandardKeyJustPressed('Q')) {
            CFont::PrintString(50, 250, "switched"_gxt);
            ReversibleHooks::SwitchHook("DisplaySlider");
        }
    }

    // Early return conditions
    if (m_bScanningUserTracks || m_nControllerError) {
        return;
    }

    // Initialize input flags
    bool bEnter = false;
    bool bExit = false;
    bool bUp = false;
    bool bDown = false;
    int8_t sliderMove = 0;

    // Check mouse selection
    if (!m_DisplayTheMouse) {
        if (m_nOldMousePosX && m_nOldMousePosY) {
            if (m_nOldMousePosX != m_nMousePosX || m_nOldMousePosY != m_nMousePosY) {
                m_DisplayTheMouse = true;
            }
        }
    }
    
    // Check for mouse hover on menu options
    static int32 oldOption = -99;
    oldOption = m_nCurrentScreenItem; // 0x8CE004 - static typo hasnt sense?
    
    for (int rowToCheck = 0; rowToCheck < 12; ++rowToCheck) {
        if (aScreens[m_nCurrentScreen].m_aItems[rowToCheck].m_nActionType < 2 || 
            aScreens[m_nCurrentScreen].m_aItems[rowToCheck].m_nActionType == eMenuAction::MENU_ACTION_TEXT)
            continue;

        float itemPosY = aScreens[m_nCurrentScreen].m_aItems[rowToCheck].m_Y;
        float itemBottomY = itemPosY + 26; // PLUS_LINE_HEIGHT_ON_SCREEN
        
        bool mouseOverItem = (CMenuManager::StretchY(itemPosY) <= m_nMousePosY) && (CMenuManager::StretchY(itemBottomY) >= m_nMousePosY);
                            
        if (mouseOverItem) {
            // Mouse is over this item
            if ((FrontEndMenuManager.field_1B74 & 1) == 0) {
                FrontEndMenuManager.field_1B74 |= 1u;
                FrontEndMenuManager.field_1B70 = m_nCurrentScreen;
            }
            
            oldOption = m_CurrentMouseOption;
            m_CurrentMouseOption = rowToCheck;

            if (m_nOldMousePosX != m_nMousePosX || m_nOldMousePosY != m_nMousePosY) {
                m_nCurrentScreenItem = rowToCheck;
                m_DisplayTheMouse = true;
            }
            
            // Update mouse bounds based on action type
            int actionType = aScreens[m_nCurrentScreen].m_aItems[rowToCheck].m_nActionType;
            if (actionType != 0x1B && actionType != 0x1C && actionType != 0x1D && 
                actionType != 0x3D && actionType != 0x3E) {
                m_MouseInBounds = 2;
            }

            break;
        }
        if (m_DisplayTheMouse) {
			m_nCurrentScreenItem = oldOption;
			m_CurrentMouseOption = oldOption;
		}
    }
    
    // Process mouse and selection changes
    if (m_DisplayTheMouse) {
        if (oldOption != m_nCurrentScreenItem) {
            if (aScreens[m_nCurrentScreen].m_aItems[m_nCurrentScreenItem].m_nActionType == 1) {
                m_nCurrentScreenItem++;
                m_CurrentMouseOption++;
            }
            AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_HIGHLIGHT, 0.0, 1.0);
        }
    }

    // Update mouse position
    m_nOldMousePosY = m_nMousePosY;
    m_nOldMousePosX = m_nMousePosX;
    m_nMousePosX = m_nMousePosWinX;
    m_nMousePosY = m_nMousePosWinY;
    
    // Clamp mouse position within screen bounds
    if (m_nMousePosWinX < 0) {
        m_nMousePosX = 0;
    }
    if (m_nMousePosX > RsGlobal.maximumWidth) {
        m_nMousePosX = RsGlobal.maximumWidth;
    }
    if (m_nMousePosWinY < 0) {
        m_nMousePosY = 0;
    }
    if (m_nMousePosY > RsGlobal.maximumHeight) {
        m_nMousePosY = RsGlobal.maximumHeight;
    }
    
    // Handle special case for controls screen
    if (m_nCurrentScreen == 0x26) {
        if (!m_nControllerError) {
            CMenuManager::RedefineScreenUserInput(&bEnter, &bExit);
        }
    } else {
        // Process menu navigation for screens with multiple options
        if ((unsigned __int8)CMenuManager::GetNumberOfMenuOptions() > 1) {
            CMenuManager::AdditionalOptionInput(&bUp, &bDown);
            
            if (CMenuManager::CheckFrontEndDownInput()) {
                bDown = true;
                m_DisplayTheMouse = false;
            } else if (CMenuManager::CheckFrontEndUpInput()) {
                bUp = true;
                m_DisplayTheMouse = false;
            }
        }
        
        // Handle enter/cross button input
        CPad* pad = CPad::GetPad(0);
        bool enterKeyPressed = false;
        
        if (m_nCurrentScreenItem != 0 || m_nCurrentScreen != 0x29) {
            // Normal enter key handling
            bool keyboardEnterPressed = (CPad::NewKeyState.enter && !CPad::OldKeyState.enter) || 
                                       (CPad::NewKeyState.extenter && !CPad::OldKeyState.extenter);
            bool systemMenuActive = (m_nSysMenu & 0x80u) != 0;
            bool crossButtonPressed = pad->NewState.ButtonCross && !pad->OldState.ButtonCross;
            
            if ((keyboardEnterPressed && systemMenuActive) || crossButtonPressed) {
                m_DisplayTheMouse = false;
                bEnter = true;
            }
        } else {
            // Special handling for first item in main menu controls
            bool keyboardEnterReleased = (!CPad::NewKeyState.enter && CPad::OldKeyState.enter) || 
                                         (!CPad::NewKeyState.extenter && CPad::OldKeyState.extenter);
            bool systemMenuActive = (m_nSysMenu & 0x80u) != 0;
            bool crossButtonReleased = !pad->NewState.ButtonCross && pad->OldState.ButtonCross;
            
            if ((keyboardEnterReleased && systemMenuActive) || crossButtonReleased) {
                m_DisplayTheMouse = false;
                bEnter = true;
            }
        }
        
        // Handle mouse click
        bool mouseLeftClicked = !CPad::NewMouseControllerState.isMouseLeftButtonPressed && 
                                CPad::OldMouseControllerState.isMouseLeftButtonPressed && 
                                m_MouseInBounds == 2;
        
        if (mouseLeftClicked) {
            if (m_nCurrentScreen == 5) {
                if (CMenuManager::StretchY(388.0) < m_nMousePosY) {
                    bEnter = true;
                }
            } else if (m_DisplayTheMouse) {
                bEnter = true;
            }
        }
        
        // Handle slider movement
        if (CPad::NewMouseControllerState.isMouseLeftButtonPressed) {
            switch (m_MouseInBounds) {
                case 6:
                case 8:
                case 0xA:
                case 0xC:
                case 0xE:
                    CMenuManager::CheckSliderMovement(1);
                    break;
                case 7:
                case 9:
                case 0xB:
                case 0xD:
                case 0xF:
                    CMenuManager::CheckSliderMovement(-1);
                    break;
                default:
                    break;
            }
        }
        
        // Handle left/right navigation
        bool shouldTriggerNavEvent = false;
        
        // Check various input methods for left/right navigation
        bool mouseLeftPressed = CPad::NewMouseControllerState.isMouseLeftButtonPressed && 
                               !CPad::OldMouseControllerState.isMouseLeftButtonPressed;
        bool keyLeftPressed = CPad::NewKeyState.left && !CPad::OldKeyState.left;
        bool keyRightPressed = CPad::NewKeyState.right && !CPad::OldKeyState.right;
        bool dpadLeftPressed = pad->NewState.DPadLeft && !pad->OldState.DPadLeft;
        bool analogLeftActive = CPad::GetAnaloguePadLeft();
        bool analogRightActive = CPad::GetAnaloguePadRight();
        bool wheelUpJustDown = CPad::IsMouseWheelUpPressed();
        bool wheelDownJustDown = CPad::IsMouseWheelDownPressed();

        // Lambda to check if joystick moved
        bool joystickMoved = [pad]() -> bool { // sub_4410E0
            return ((WORD*)pad)[11] && !((WORD*)pad)[35];
        }();

        // bool joystickMoved = sub_4410E0(pad);
        
        if (mouseLeftPressed || keyLeftPressed || keyRightPressed || dpadLeftPressed || 
            joystickMoved || analogLeftActive || analogRightActive || 
            wheelUpJustDown || wheelDownJustDown) {
            
            int actionType = aScreens[m_nCurrentScreen].m_aItems[m_nCurrentScreenItem].m_nActionType;
            
            switch (actionType) {
                case 0x1B:
                case 0x1C:
                case 0x3D:
                case 0x3E:
                    AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_SELECT, 0.0, 1.0);
                    break;
                case 0x1D:
                    AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_NOISE_TEST, 0.0, 1.0);
                    break;
                default:
                    break;
            }
        }
        
        // Handle escape/triangle button
        bool trianglePressed = pad->NewState.ButtonTriangle && !pad->OldState.ButtonTriangle;
        bool escPressed = CPad::NewKeyState.esc && !CPad::OldKeyState.esc;
        
        if (trianglePressed || escPressed) {
            // Only exit if not on main menus
            bool isMainMenu = (m_nCurrentScreen == 0x22 || 
                              m_nCurrentScreen == 0x29 || 
                              m_nCurrentScreen == 0x10 || 
                              m_nCurrentScreen == 0x16 || 
                              m_nCurrentScreen == 0x12 || 
                              m_nCurrentScreen == 0xE || 
                              m_nCurrentScreen == 0x2B);
            
            if (!isMainMenu) {
                m_DisplayTheMouse = false;
                bExit = true;
            }
        }
        
        // Clear exit flag if any other action is taken
        if (bDown || bUp || bEnter) {
            bExit = false;
        }
        
        // Check for left/right slider movement
        const bool isMouseWheelUsed[2] = {CPad::NewKeyState.left || pad->GetPedWalkLeftRight() < 0 || pad->NewState.DPadLeft,  CPad::NewKeyState.right || pad->GetPedWalkLeftRight() > 0 || pad->NewState.DPadRight};
        if (isMouseWheelUsed[0] || isMouseWheelUsed[1]) {
            auto& sliderMoveTime = isMouseWheelUsed[0] ? FrontEndMenuManager.m_nTimeSlideLeftMove : FrontEndMenuManager.m_nTimeSlideRightMove;
            
            // Process slide left move with time delay
            static constexpr auto sliders = {MENU_ACTION_STAT, MENU_ACTION_BRIGHTNESS, MENU_ACTION_RADIO_VOL, MENU_ACTION_SFX_VOL, MENU_ACTION_DRAW_DIST, MENU_ACTION_MOUSE_SENS};
            if (CTimer::m_snTimeInMillisecondsPauseMode - sliderMoveTime > 200 && notsa::contains(sliders, aScreens[m_nCurrentScreen].m_aItems[m_nCurrentScreenItem].m_nActionType)) {               
                sliderMove = isMouseWheelUsed[0] ? -1 : 1;
                sliderMoveTime = CTimer::m_snTimeInMillisecondsPauseMode;
            }
        }
        
        // Handle mouse wheel movement
        if (sliderMove == 0) {
            const bool wheelMovedUp = pad->IsMouseWheelUpPressed();
            if ( (wheelMovedUp || pad->IsMouseWheelDownPressed()) && m_nCurrentScreen != eMenuScreen::SCREEN_MAP) {
                sliderMove = wheelMovedUp ? 1 : -1;
            } else {
                // Check front end directional input
                if (CMenuManager::CheckFrontEndRightInput()) {
                    m_DisplayTheMouse = false;
                    sliderMove = 1;
                } else if (CMenuManager::CheckFrontEndLeftInput()) {
                    m_DisplayTheMouse = false;
                    sliderMove = -1;
                }
            }
        }

        static constexpr std::array<eMenuScreen, 6> specialScreens = {
            SCREEN_AUDIO_SETTINGS, SCREEN_USER_TRACKS_OPTIONS, SCREEN_DISPLAY_SETTINGS, SCREEN_DISPLAY_ADVANCED, SCREEN_CONTROLLER_SETUP, SCREEN_MOUSE_SETTINGS
        };

        static constexpr std::array<eMenuAction, 12> specialMenuActions = {
            MENU_ACTION_BACK, MENU_ACTION_MENU, MENU_ACTION_CTRLS_JOYPAD,
            MENU_ACTION_CTRLS_FOOT, MENU_ACTION_CTRLS_CAR, MENU_ACTION_BRIGHTNESS,
            MENU_ACTION_RADIO_VOL, MENU_ACTION_SFX_VOL, MENU_ACTION_RADIO_STATION,
            MENU_ACTION_RESET_CFG, MENU_ACTION_DRAW_DIST, MENU_ACTION_MOUSE_SENS
        };

        if (sliderMove != 0 && notsa::contains(specialScreens, m_nCurrentScreen) && !notsa::contains(specialMenuActions, aScreens[m_nCurrentScreen].m_aItems[m_nCurrentScreenItem].m_nActionType)) {
            AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_SELECT, 0.0, 1.0);
        }
    }
    
    // Process all user input
    CMenuManager::ProcessUserInput(bDown, bUp, bEnter, bExit, sliderMove);
}

/*!
 * @addr 0x57B480
 */
void CMenuManager::ProcessUserInput(bool GoDownMenu, bool GoUpMenu, bool EnterMenuOption, bool GoBackOneMenu, int8 LeftRight) {
    if (m_nCurrentScreen == eMenuScreen::SCREEN_EMPTY || CheckRedefineControlInput()) {
        return;
    }

    // Handle down navigation
    if (GetNumberOfMenuOptions() > 1 && GoDownMenu) {
        if (m_nCurrentScreen != eMenuScreen::SCREEN_MAP) {
            AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_HIGHLIGHT, 0.0, 1.0);
        }

        m_nCurrentScreenItem++;
        
        for (; (aScreens[m_nCurrentScreen].m_aItems[m_nCurrentScreenItem].m_nActionType == eMenuAction::MENU_ACTION_SKIP); m_nCurrentScreenItem++);

        // Wrap around if reached end or empty item
        if (m_nCurrentScreenItem >= eMenuEntryType::TI_OPTION || !aScreens[m_nCurrentScreen].m_aItems[m_nCurrentScreenItem].m_nActionType) {
            m_nCurrentScreenItem = (aScreens[m_nCurrentScreen].m_aItems[0].m_nActionType == eMenuAction::MENU_ACTION_TEXT) ? 1 : 0;
        }
    }

    // Handle up navigation
    if (GetNumberOfMenuOptions() > 1 && GoUpMenu) {
        if (m_nCurrentScreen != eMenuScreen::SCREEN_MAP) {
            AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_HIGHLIGHT, 0.0, 1.0);
        }

        auto firstItemSpecial = (aScreens[m_nCurrentScreen].m_aItems[0].m_nActionType == 1);

        if (m_nCurrentScreenItem <= (firstItemSpecial ? 1 : 0)) {
            // Wrap to end
            for (; m_nCurrentScreenItem < eMenuEntryType::TI_ENTER && aScreens[m_nCurrentScreen].m_aItems[m_nCurrentScreenItem + 1].m_nActionType; m_nCurrentScreenItem++);

            // Skip entries marked as MENU_ACTION_SKIP (backwards)
            for (; (aScreens[m_nCurrentScreen].m_aItems[m_nCurrentScreenItem].m_nActionType == eMenuAction::MENU_ACTION_SKIP); m_nCurrentScreenItem--);

        } else {
            // Move to previous item
            m_nCurrentScreenItem--;

            // Skip entries marked as MENU_ACTION_SKIP (backwards)
            for (; (aScreens[m_nCurrentScreen].m_aItems[m_nCurrentScreenItem].m_nActionType == eMenuAction::MENU_ACTION_SKIP); m_nCurrentScreenItem--);
        }
    }

    // Handle accept action
    if (EnterMenuOption) {
        if (m_nCurrentScreen == eMenuScreen::SCREEN_CONTROLS_DEFINITION) {
            m_EditingControlOptions = true;
            m_bJustOpenedControlRedefWindow = true;
            m_pPressedKey = &m_KeyPressedCode;
            m_MouseInBounds = 16;
        }

        ProcessMenuOptions(0, GoBackOneMenu, EnterMenuOption);

        if (!GoBackOneMenu) {
            eMenuEntryType menuType = aScreens[m_nCurrentScreen].m_aItems[m_nCurrentScreenItem].m_nType;
            
            // Audio feedback based on menu type and status
            if ((!m_isPreInitialised && !IsSaveSlot(menuType)) || (IsSaveSlot(menuType) && GetSavedGameState(m_nCurrentScreenItem - 1) == eSlotState::SLOT_FILLED)) {
                AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_SELECT, 0.0, 1.0);
            } else {
                AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_ERROR, 0.0, 1.0);
            }
        }
    }

    // Handle slider movement with wheel input
    if (LeftRight && aScreens[m_nCurrentScreen].m_aItems[m_nCurrentScreenItem].m_nType == eMenuEntryType::TI_OPTION) {
        ProcessMenuOptions(LeftRight, GoBackOneMenu, 0);
        CheckSliderMovement(LeftRight);
    }

    // Handle cancel/back action
    if (GoBackOneMenu) {
        if (m_bRadioAvailable) {
            AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_BACK);
            SwitchToNewScreen(eMenuScreen::SCREEN_GO_BACK); // Go back one screen
        } else {
            AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_ERROR);
        }
    }
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
// NOTE: Mix Android version. The PC version uses the RW classes.
bool CMenuManager::CheckRedefineControlInput() {
    if (m_EditingControlOptions) {
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

            // Android
            auto TypeOfControl = eControllerType::KEYBOARD;
            if (m_nJustDownJoyButton != NO_JOYBUTTONS) {
                TypeOfControl = eControllerType::JOY_STICK;
            } else if (m_nPressedMouseButton) {
                TypeOfControl = eControllerType::MOUSE;
            } if (*m_pPressedKey != rsNULL) {
                TypeOfControl = eControllerType::KEYBOARD;
            }

            if (m_CanBeDefined) {
                if (m_DeleteAllBoundControls) {
                    AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_SELECT);
                    m_CanBeDefined = false;
                    m_DeleteAllNextDefine = true;
                    m_DeleteAllBoundControls = false;
                } else {
                    if (*m_pPressedKey != rsNULL || m_nPressedMouseButton || m_nJustDownJoyButton) {
                        CheckCodesForControls(TypeOfControl);
                    }
                    m_JustExitedRedefine = true;
                }
            } else {
                m_pPressedKey = nullptr;
                m_EditingControlOptions = false;
                m_KeyPressedCode = (RsKeyCodes)-1;
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
                if (!m_isPreInitialised) {
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

#ifndef NOTSA_USE_SDL3
                    if (IsVideoModeExclusive()) {
                        DIReleaseMouse();
                        InitialiseMouse(false);
                    }
#endif // NOTSA_USE_SDL3

                    Initialise();
                    LoadAllTextures();

                    SetBrightness((float)m_PrefsBrightness, true);
                }
            } else {
                AudioEngine.StopRadio(nullptr, false);
                AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_RADIO_RETUNE_STOP);

                if (m_nSysMenu != CMenuSystem::MENU_UNDEFINED) {
                    CMenuSystem::SwitchOffMenu(0);
                    m_nSysMenu = CMenuSystem::MENU_UNDEFINED;
                }

                auto pad = CPad::GetPad(m_nPlayerNumber);
                pad->Clear(false, true);
                pad->ClearKeyBoardHistory();
                pad->ClearMouseHistory();

#ifndef NOTSA_USE_SDL3
                if (IsVideoModeExclusive()) {
                    DIReleaseMouse();
#ifdef FIX_BUGS // Causes the retarded fucktard code to not dispatch mouse input to WndProc => ImGUI mouse not working. Amazing piece of technology.
                    InitialiseMouse(false);
#else
                    InitialiseMouse(true);
#endif // !FIX_BUGS
                }
#endif // NOTSA_USE_SDL3

                m_fStatsScrollSpeed = 150.0f;
                SaveSettings();
                m_pPressedKey = nullptr;
                field_EC = 0;
                m_MenuIsAbleToQuit = 0;
                m_bDontDrawFrontEnd = false;
                m_bActivateMenuNextFrame = false;
                m_EditingControlOptions = false;
                m_bIsSaveDone = false;
                UnloadTextures();

                CTimer::EndUserPause();
                CTimer::Update();

                pad->JustOutOfFrontEnd = true;
                pad->LastTimeTouched = 0;
                CPad::GetPad(1)->LastTimeTouched = 0;

                SetBrightness((float)m_PrefsBrightness, true);

                if (m_isPreInitialised) {
                    auto player = FindPlayerPed();

                    if (player->GetActiveWeapon().m_Type != WEAPON_CAMERA
                        || CTimer::GetTimeInMS() >= player->GetActiveWeapon().m_TimeForNextShotMs) {
                        TheCamera.SetFadeColour(0u, 0u, 0u);
                        TheCamera.Fade(0.0f, eFadeFlag::FADE_IN);
                        TheCamera.ProcessFade();
                        TheCamera.Fade(0.2f, eFadeFlag::FADE_OUT);
                    }
                }
                m_isPreInitialised = false;
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
        m_isPreInitialised = true;

#ifndef NOTSA_USE_SDL3
        if (IsVideoModeExclusive()) {
            DIReleaseMouse();
            InitialiseMouse(false);
        }
#endif // NOTSA_USE_SDL3

        Initialise();
        LoadAllTextures();

        m_nCurrentScreenItem = 0;

        m_nCurrentScreen = (!CCheat::m_bHasPlayerCheated) ? SCREEN_GAME_SAVE : SCREEN_GAME_WARNING_DONT_SAVE;
    }

    m_bActivateMenuNextFrame = false;
    m_bDontDrawFrontEnd = false;
}

// 0x57C4F0
[[nodiscard]] bool CMenuManager::CheckHover(float left, float right, float top, float bottom) const {
    // debug: CSprite2d::DrawRect(CRect(left, top, right, bottom), CRGBA(255, 0, 0, 255));
    return (
        (float)m_nMousePosX > left  &&
        (float)m_nMousePosX < right &&
        (float)m_nMousePosY > top   &&
        (float)m_nMousePosY < bottom
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
// NOTE: Mix of Android version. The PC version uses the RW classes.
void CMenuManager::CheckCodesForControls(eControllerType type) {
    auto actionId          = (eControllerAction)m_OptionToChange;
    bool escapePressed     = false;
    bool invalidKeyPressed = false;
    m_MenuIsAbleToQuit = false;
    eControllerType controllerType = eControllerType::KEYBOARD;

    // Handle different input types
    switch (type) {
    case eControllerType::KEYBOARD: {
        // Handle keyboard input
        RsKeyCodes keyCode = *m_pPressedKey;

        if (keyCode == rsESC) {
            AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_ERROR);
            escapePressed = true;
        } else if (!keyCode || notsa::contains({ rsF1, rsF2, rsF3, rsLWIN, rsRWIN }, keyCode)) { // Fixed from v1.01
            AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_ERROR);
            invalidKeyPressed = true;
        } else {
            AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_SELECT);
            if ((ControlsManager.GetControllerKeyAssociatedWithAction(actionId, eControllerType::KEYBOARD) != rsNULL) && (ControlsManager.GetControllerKeyAssociatedWithAction(actionId, eControllerType::KEYBOARD) != *m_pPressedKey)) {
                controllerType = eControllerType::OPTIONAL_EXTRA_KEY;
            }
        }
        break;
    }
    case eControllerType::MOUSE: {
        // Mouse input
        controllerType = eControllerType::MOUSE;
        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_SELECT);
        break;
    }
    case eControllerType::JOY_STICK:
        // Joystick/controller input
        controllerType = eControllerType::JOY_STICK;
        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_SELECT);
        m_MenuIsAbleToQuit = (ControlsManager.GetIsActionAButtonCombo(actionId)) ? 1 : 0;
        break;
    }

    // Handle escape key or invalid key press
    if (escapePressed || invalidKeyPressed || (m_MenuIsAbleToQuit && escapePressed)) {
        m_DeleteAllNextDefine = 0;
        m_pPressedKey = nullptr;
        m_EditingControlOptions = false;
        m_KeyPressedCode = (RsKeyCodes)-1;
        m_bJustOpenedControlRedefWindow = false;
        return;
    }

    if (!invalidKeyPressed) {
        // Process delete all bound controls
        if (m_DeleteAllNextDefine) {
            for (const auto controlType : CONTROLLER_TYPES_ALL) {
                ControlsManager.ClearSettingsAssociatedWithAction(actionId, controlType);
            }
            m_DeleteAllNextDefine = 0;
        }

        // Clear settings for the current controller type
        ControlsManager.ClearSettingsAssociatedWithAction(actionId, controllerType);

        // Set the new control based on input type
        switch (type) {
        case eControllerType::MOUSE: {
            ControlsManager.DeleteMatchingActionInitiators(actionId, m_nPressedMouseButton, eControllerType::MOUSE);
            ControlsManager.SetControllerKeyAssociatedWithAction(actionId, m_nPressedMouseButton, controllerType);
            break;
        }
        case eControllerType::JOY_STICK: {
            ControlsManager.DeleteMatchingActionInitiators(actionId, m_nJustDownJoyButton, eControllerType::JOY_STICK);
            ControlsManager.SetControllerKeyAssociatedWithAction(actionId, m_nJustDownJoyButton, controllerType);
            break;
        }
        // Keyboard + Optional Extra Key
        case eControllerType::KEYBOARD:
        case eControllerType::OPTIONAL_EXTRA_KEY: {
            ControlsManager.DeleteMatchingActionInitiators(actionId, *m_pPressedKey, eControllerType::KEYBOARD);
            ControlsManager.DeleteMatchingActionInitiators(actionId, *m_pPressedKey, eControllerType::OPTIONAL_EXTRA_KEY);
            ControlsManager.SetControllerKeyAssociatedWithAction(actionId, *m_pPressedKey, controllerType);
            break;
        }
        default:
            NOTSA_UNREACHABLE();
            break;
        }

        // Reset state
        m_pPressedKey = nullptr;
        m_EditingControlOptions = false;
        m_KeyPressedCode = (RsKeyCodes) - 1;
        m_bJustOpenedControlRedefWindow = false;
        SaveSettings();
    }
}
