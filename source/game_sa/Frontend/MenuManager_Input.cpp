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
// Vanilla bug: Analogic directionals are not working in the menu for set setting ON/OFF.
void CMenuManager::UserInput() {
    static constexpr auto actionSelect       = { MENU_ACTION_BRIGHTNESS, MENU_ACTION_RADIO_VOL, MENU_ACTION_DRAW_DIST, MENU_ACTION_MOUSE_SENS };
    static constexpr auto mainMenu           = { SCREEN_MAIN_MENU, SCREEN_PAUSE_MENU, SCREEN_GAME_SAVE, SCREEN_GAME_WARNING_DONT_SAVE, SCREEN_SAVE_DONE_1, SCREEN_DELETE_FINISHED, SCREEN_EMPTY };
    static constexpr auto SLIDER_ACTIONS     = { MENU_ACTION_BRIGHTNESS, MENU_ACTION_RADIO_VOL, MENU_ACTION_SFX_VOL, MENU_ACTION_DRAW_DIST, MENU_ACTION_MOUSE_SENS };
    static constexpr auto specialScreens     = { SCREEN_AUDIO_SETTINGS, SCREEN_USER_TRACKS_OPTIONS, SCREEN_DISPLAY_SETTINGS, SCREEN_DISPLAY_ADVANCED, SCREEN_CONTROLLER_SETUP, SCREEN_MOUSE_SETTINGS };
    static constexpr auto specialMenuActions = { MENU_ACTION_BACK, MENU_ACTION_MENU, MENU_ACTION_CTRLS_JOYPAD, MENU_ACTION_CTRLS_FOOT, MENU_ACTION_CTRLS_CAR, MENU_ACTION_BRIGHTNESS, MENU_ACTION_RADIO_VOL, MENU_ACTION_SFX_VOL, MENU_ACTION_RADIO_STATION, MENU_ACTION_RESET_CFG, MENU_ACTION_DRAW_DIST, MENU_ACTION_MOUSE_SENS };
    static int8 oldOption                    = -99; // 0x8CE005

    // Early return conditions
    if (m_bScanningUserTracks || m_nControllerError != eControllerError::NONE) {
        return;
    }

    auto& curScreen = aScreens[m_nCurrentScreen].m_aItems;
    const auto pad = CPad::GetPad();

    // Initialize input flags
    bool bEnter = false;
    bool bExit = false;
    bool bUp = false;
    bool bDown = false;
    int8 sliderMove = 0;

    // Check for mouse movement to enable display
    m_DisplayTheMouse |= !m_DisplayTheMouse && (m_nOldMousePosX && m_nOldMousePosY) &&
                         (m_nOldMousePosX != m_nMousePosX || m_nOldMousePosY != m_nMousePosY);

    // Initialize old option for tracking selection changes
    oldOption = m_nCurrentScreenItem;

    // Check for mouse hover on menu options
    bool mouseOverAnyItem = false;
    for (auto rowToCheck = 0; rowToCheck < 12; ++rowToCheck) {
        if (curScreen[rowToCheck].m_nActionType >= eMenuAction::MENU_ACTION_BACK) {
            float itemPosY = curScreen[rowToCheck].m_Y;

            bool mouseOverItem = (StretchY(itemPosY) <= m_nMousePosY) &&
                                 (StretchY(itemPosY + 26) >= m_nMousePosY);

            if (mouseOverItem) {
                // 0x57FEEC - Mouse is over this item
                if ((field_1B74 & 1) == 0) {
                    field_1B74 |= 1u;
                    field_1B70 = m_nCurrentScreen;
                }

                m_CurrentMouseOption = rowToCheck;

                if (m_nOldMousePosX != m_nMousePosX || m_nOldMousePosY != m_nMousePosY) {
                    m_nCurrentScreenItem = rowToCheck;
                    m_DisplayTheMouse = true;
                }

                // 0x57FF72 - Update mouse bounds based on action type
                if (!notsa::contains(SLIDER_ACTIONS, curScreen[rowToCheck].m_nActionType)) {
                    m_MouseInBounds = eMouseInBounds::MENU_ITEM;
                }

                mouseOverAnyItem = true;
                break;
            }
        }
    }

    // 0x57FEBB - Reset selection only if mouse is not over any item
    if (m_DisplayTheMouse && !mouseOverAnyItem) {
        m_nCurrentScreenItem = oldOption;
        m_CurrentMouseOption = oldOption;
    }

    // 0x57FF83 - Process mouse and selection changes
    if (m_DisplayTheMouse && oldOption != m_nCurrentScreenItem) {
        if (curScreen[m_nCurrentScreenItem].m_nActionType == eMenuAction::MENU_ACTION_TEXT) {
            m_nCurrentScreenItem++;
            m_CurrentMouseOption++;
        }
        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_HIGHLIGHT);
    }

    // Update mouse position
    m_nOldMousePosY = m_nMousePosY;
    m_nOldMousePosX = m_nMousePosX;

    // Clamp mouse position within screen bounds
    m_nMousePosX = std::clamp(m_nMousePosWinX, 0, RsGlobal.maximumWidth);
    m_nMousePosY = std::clamp(m_nMousePosWinY, 0, RsGlobal.maximumHeight);

    // 0x580050 - Handle special case for controls screen
    if (m_nCurrentScreen == eMenuScreen::SCREEN_CONTROLS_DEFINITION) {
        if (m_nControllerError == eControllerError::NONE) {
            RedefineScreenUserInput(&bEnter, &bExit);
        }
    } else {
        // Process menu navigation for screens with multiple options
        if (GetNumberOfMenuOptions() > 1) {
            AdditionalOptionInput(&bUp, &bDown);
            if (CheckFrontEndDownInput()) {
                bDown = true;
                m_DisplayTheMouse = false;
            } else if (CheckFrontEndUpInput()) {
                bUp = true;
                m_DisplayTheMouse = false;
            }
        }

        // 0x5800C9 - Handle enter key or cross button
        if (m_nCurrentScreenItem != 0 || m_nCurrentScreen != eMenuScreen::SCREEN_PAUSE_MENU) {
            // Normal handling: key press
            if ((CPad::IsEnterJustPressed() && (m_nSysMenu & 128u) != 0) || pad->IsCrossPressed()) {
                m_DisplayTheMouse = false;
                bEnter = true;
            }
        } else {
            // Special handling for first item in pause menu: key release
            if ((CPad::f0x57C330() && (m_nSysMenu & 128u) != 0) || pad->f0x57C3A0()) {
                m_DisplayTheMouse = false;
                bEnter = true;
            }
        }

        // 0x5801A0 - Handle mouse click
        if (pad->f0x57C3C0() && m_MouseInBounds == eMouseInBounds::MENU_ITEM) {
            if (m_nCurrentScreen == eMenuScreen::SCREEN_MAP) {
                if (StretchY(388.0f) < m_nMousePosY) {
                    bEnter = true;
                }
            } else if (m_DisplayTheMouse) {
                bEnter = true;
            }
        }

        // 0x580206 - Handle slider movement
        if (pad->IsMouseLButton()) {
            switch (m_MouseInBounds) {
            case eMouseInBounds::SLIDER_RIGHT:
            case eMouseInBounds::DRAW_DIST_RIGHT:
            case eMouseInBounds::RADIO_VOL_RIGHT:
            case eMouseInBounds::SFX_VOL_RIGHT:
            case eMouseInBounds::MOUSE_SENS_RIGHT:
                CheckSliderMovement(1);
                break;
            case eMouseInBounds::SLIDER_LEFT:
            case eMouseInBounds::DRAW_DIST_LEFT:
            case eMouseInBounds::RADIO_VOL_LEFT:
            case eMouseInBounds::SFX_VOL_LEFT:
            case eMouseInBounds::MOUSE_SENS_LEFT:
                CheckSliderMovement(-1);
                break;
            }
        }

        // Lambda to check if joystick moved
        bool joystickMoved = [pad]() -> bool { // sub_4410E0
            return ((WORD*)pad)[11] && !((WORD*)pad)[35];
        }();

        // 0x58022B
        if (CPad::IsMouseLButtonPressed() || CPad::IsLeftPressed() || CPad::IsRightPressed() || pad->IsDPadLeftPressed() ||
            joystickMoved || CPad::GetAnaloguePadLeft() || CPad::GetAnaloguePadRight() ||
            CPad::IsMouseWheelUpPressed() || CPad::IsMouseWheelDownPressed()) {
            int actionType = curScreen[m_nCurrentScreenItem].m_nActionType;
            if (notsa::contains(actionSelect, actionType)) {
                AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_SELECT);
            } else if (actionType == eMenuAction::MENU_ACTION_SFX_VOL) {
                AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_NOISE_TEST);
            }
        }

        // 0x580350 - Only exit if not on main menus
        if ((pad->IsTrianglePressed() || CPad::IsEscJustPressed()) &&
            !notsa::contains(mainMenu, m_nCurrentScreen)) {
            m_DisplayTheMouse = false;
            bExit = true;
        }

        // Clear exit flag if any other action is taken
        if (bDown || bUp || bEnter) {
            bExit = false;
        }

        // Check for left/right slider movement
        const bool isLeftPressed = CPad::IsLeftDown() || pad->GetPedWalkLeftRight() < 0 || pad->f0x57C380();
        const bool isRightPressed = CPad::IsRightDown() || pad->GetPedWalkLeftRight() > 0 || pad->f0x57C390();
        if (isLeftPressed || isRightPressed) {
            auto& lastSliderMoveTime = isLeftPressed ? m_SlideLeftMoveTime : m_SlideRightMoveTime;
            if (CTimer::GetTimeInMSPauseMode() - lastSliderMoveTime > 200) {
                if (notsa::contains(SLIDER_ACTIONS, curScreen[m_nCurrentScreenItem].m_nActionType)
                    || curScreen[m_nCurrentScreenItem].m_nActionType == MENU_ACTION_STAT) {
                    sliderMove         = isLeftPressed ? -1 : 1;
                    lastSliderMoveTime = CTimer::GetTimeInMSPauseMode();
                }
            }
        }

        // 0x58052C - Handle mouse wheel movement
        if (m_nCurrentScreen != eMenuScreen::SCREEN_MAP) {
            if (CPad::IsMouseWheelUpPressed()) {
                sliderMove = 1;
            } else if (CPad::IsMouseWheelDownPressed()) {
                sliderMove = -1;
            }
        }

        if (CheckFrontEndRightInput()) {
            m_DisplayTheMouse = false;
            sliderMove = 1;
        } else if (CheckFrontEndLeftInput()) {
            m_DisplayTheMouse = false;
            sliderMove = -1;
        }

        // Play sound for specific screens and actions
        if (sliderMove != 0 && notsa::contains(specialScreens, m_nCurrentScreen) &&
            !notsa::contains(specialMenuActions, curScreen[m_nCurrentScreenItem].m_nActionType)) {
            AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_SELECT);
        }
    }

    // Process all user input
    ProcessUserInput(bDown, bUp, bEnter, bExit, sliderMove);
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
            AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_HIGHLIGHT);
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
            AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_HIGHLIGHT);
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
            m_MouseInBounds = eMouseInBounds::NONE;
        }

        ProcessMenuOptions(0, GoBackOneMenu, EnterMenuOption);

        if (!GoBackOneMenu) {
            eMenuEntryType menuType = aScreens[m_nCurrentScreen].m_aItems[m_nCurrentScreenItem].m_nType;

            // Audio feedback based on menu type and status
            if ((!m_isPreInitialised && !IsSaveSlot(menuType)) || (IsSaveSlot(menuType) && GetSavedGameState(m_nCurrentScreenItem - 1) == eSlotState::SLOT_FILLED)) {
                AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_SELECT);
            } else {
                AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_ERROR);
            }
        }
    }

    // 0x57B6D9 - Handle slider movement with wheel input
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

// 0x57EF50
void CMenuManager::RedefineScreenUserInput(bool* accept, bool* cancel) {
    if (m_EditingControlOptions) {
        return; // 0x57EF53 - Invert
    }

    const auto maxAction = m_nCurrentScreen == SCREEN_CONTROLS_DEFINITION ? GetMaxAction() : 0u;
    const auto pad = CPad::GetPad();

    // 0x57EF97 - Handle enter key/button press
    if (CPad::IsEnterJustPressed() && (m_nSysMenu & 0x80u) != 0 || pad->IsCrossPressed()) {
        m_DisplayTheMouse = false;
        *accept = true;
    }

    // 0x57EFF6 - Handle backspace key logic
    if (!CPad::IsBackspacePressed() || m_nCurrentScreen != SCREEN_CONTROLS_DEFINITION || m_JustExitedRedefine) {
        m_JustExitedRedefine = false;
    } else {
        m_MouseInBounds = eMouseInBounds::NONE;
        m_EditingControlOptions = true;
        m_bJustOpenedControlRedefWindow = true;
        m_DeleteAllBoundControls = true;
        m_pPressedKey = &m_KeyPressedCode;
    }

    // 0x57F058 - Reset key pressed state after a delay
    if (CTimer::GetTimeInMSPauseMode() - m_LastTransitionTime > 200) {
        rng::fill(m_KeyPressed, false);
        m_LastTransitionTime = CTimer::GetTimeInMSPauseMode();
    }

    // 0x57F086 - Handle up navigation (keyboard, pad, or mouse wheel up)
    if (CPad::IsUpDown() || CPad::GetAnaloguePadUp() || pad->IsDPadUpPressed() || CPad::IsMouseWheelUpPressed()) {
        m_DisplayTheMouse = CPad::IsMouseWheelUpPressed();
        // Only trigger once per key press, not while held
        if (!m_KeyPressed[2]) {
            m_KeyPressed[2] = true;
            m_LastTransitionTime = CTimer::GetTimeInMSPauseMode();
            m_ListSelection = m_ListSelection > 0 ? m_ListSelection - 1 : maxAction - 1;
        }
    } else {
        // Reset when key released
        m_KeyPressed[2] = false;
    }

    // 0x57F138 - Handle down navigation (keyboard, pad, or mouse wheel down)
    if (CPad::IsDownDown() || CPad::GetAnaloguePadDown() || pad->IsDPadDownPressed() || CPad::IsMouseWheelDownPressed()) {
        m_DisplayTheMouse = CPad::IsMouseWheelDownPressed();
        // Only trigger once per key press, not while held
        if (!m_KeyPressed[3]) {
            m_KeyPressed[3] = true;
            m_LastTransitionTime = CTimer::GetTimeInMSPauseMode();
            m_ListSelection = (m_ListSelection == maxAction - 1) ? 0 : m_ListSelection + 1;
        }
    } else {
        // Reset when key released
        m_KeyPressed[3] = false;
    }

    // 0x57F1F0 - Handle escape/triangle button for back
    if (CPad::IsEscJustPressed() || pad->IsTrianglePressed()) {
        m_DisplayTheMouse = false;
        *cancel = true;
    }

    // 0x57F235 - 0x57F299 - Handle mouse left button clicks
    if (CPad::IsMouseLButtonPressed()) {
        if (m_MouseInBounds == eMouseInBounds::BACK_BUTTON) {
            *cancel = true;
        } else if (m_MouseInBounds == eMouseInBounds::ENTER_MENU) {
            AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_SELECT);
            m_MouseInBounds = eMouseInBounds::SELECT;
        }
    }

    // 0x57F2A3 - Handle back logic for control redefinition
    if (*cancel) {
        if (m_bRadioAvailable) {
            m_RedefiningControls = m_RedefiningControls == eControlMode::FOOT
                ? eControlMode::VEHICLE
                : eControlMode::FOOT;
            DrawControllerBound(69, true);
            if (!m_bRadioAvailable) {
                m_nControllerError = m_RedefiningControls != eControlMode::FOOT
                    ? eControllerError::FOOT
                    : eControllerError::VEHICLE;
            }
        } else {
            m_nControllerError = eControllerError::NOT_SETS;
        }
    }
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

// 0x573440
void CMenuManager::CheckSliderMovement(int32 LeftRight) {
    auto& screen = aScreens[m_nCurrentScreen];
    auto& item   = screen.m_aItems[m_nCurrentScreenItem];

    if (LeftRight != 1 && LeftRight != -1) {
        assert(false && "value invalid in CheckSliderMovement()");
    }

    switch (item.m_nActionType) {
    case MENU_ACTION_BRIGHTNESS:
        m_PrefsBrightness = std::clamp<int32>(LeftRight * int32(363.0f / 15.0f) + m_PrefsBrightness, 0, 384);
        SetBrightness((float)m_PrefsBrightness, false);
        break;
    case MENU_ACTION_RADIO_VOL: {
        m_nRadioVolume = std::clamp<int8>(4 * LeftRight + m_nRadioVolume, 0, 64);
        AudioEngine.SetMusicMasterVolume(m_nRadioVolume);
        break;
    }
    case MENU_ACTION_SFX_VOL: {
        m_nSfxVolume = std::clamp<int8>(4 * LeftRight + m_nSfxVolume, 0, 64);
        AudioEngine.SetEffectsMasterVolume(m_nSfxVolume);
        break;
    }
    case MENU_ACTION_DRAW_DIST: {
        constexpr auto step        = 7.0f / 128.0f;
        float          newDist     = LeftRight <= 0 ? m_fDrawDistance - step : m_fDrawDistance + step;
        m_fDrawDistance            = std::clamp<float>(newDist, 0.925f, 1.8f);
        CRenderer::ms_lodDistScale = m_fDrawDistance;
        break;
    }
    case MENU_ACTION_MOUSE_SENS: {
        // 0xBA672C = minimum mouse sensitivity = 1.0f/3200.0f
        // BUG: when selecting on the 8 or 9 element, it randomly switches to the 9 or 8 element
        TheCamera.m_fMouseAccelHorzntl = std::fminf(std::fmaxf(((LeftRight * (1.0f / 200.0f)) / 15.0f) + TheCamera.m_fMouseAccelHorzntl, 1.0f/3200.0f), (1.0f / 200.0f));
#ifdef FIX_BUGS
        TheCamera.m_fMouseAccelVertical = TheCamera.m_fMouseAccelHorzntl * 0.6f;
#endif
        break;
    }
    default:
        return;
    }

    SaveSettings();
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
                    m_StatusDisablePlayerControls = pad->DisablePlayerControls;
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
                pad->DisablePlayerControls = m_StatusDisablePlayerControls;
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
        m_StatusDisablePlayerControls = pad->DisablePlayerControls;
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
            MessageScreen("NO_PAK", true, false);

            DoRWStuffStartOfFrame(0, 0, 0, 0, 0, 0, 0);
            DoRWStuffEndOfFrame();
            auto pad = CPad::GetPad();

            if (CPad::IsEnterJustPressed() || CPad::IsReturnJustPressed() || pad->IsCrossPressed()) {
                break;
            }
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
        m_KeyPressedCode = (RsKeyCodes)-1;
        m_bJustOpenedControlRedefWindow = false;
        SaveSettings();
    }
}
