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
static int32 oldOption = -99; // 0x8CE004

void CMenuManager::UserInput() {
    static constexpr auto actionSelect       = { MENU_ACTION_BRIGHTNESS, MENU_ACTION_RADIO_VOL, MENU_ACTION_DRAW_DIST, MENU_ACTION_MOUSE_SENS };
    static constexpr auto mainMenu           = { SCREEN_MAIN_MENU, SCREEN_PAUSE_MENU, SCREEN_GAME_SAVE, SCREEN_GAME_WARNING_DONT_SAVE, SCREEN_SAVE_DONE_1, SCREEN_DELETE_FINISHED, SCREEN_EMPTY };
    static constexpr auto slidersActions     = { MENU_ACTION_BRIGHTNESS, MENU_ACTION_RADIO_VOL, MENU_ACTION_SFX_VOL, MENU_ACTION_DRAW_DIST, MENU_ACTION_MOUSE_SENS };
    static constexpr auto specialScreens     = { SCREEN_AUDIO_SETTINGS, SCREEN_USER_TRACKS_OPTIONS, SCREEN_DISPLAY_SETTINGS, SCREEN_DISPLAY_ADVANCED, SCREEN_CONTROLLER_SETUP, SCREEN_MOUSE_SETTINGS };
    static constexpr auto specialMenuActions = { MENU_ACTION_BACK, MENU_ACTION_MENU, MENU_ACTION_CTRLS_JOYPAD, MENU_ACTION_CTRLS_FOOT, MENU_ACTION_CTRLS_CAR, MENU_ACTION_BRIGHTNESS, MENU_ACTION_RADIO_VOL, MENU_ACTION_SFX_VOL, MENU_ACTION_RADIO_STATION, MENU_ACTION_RESET_CFG, MENU_ACTION_DRAW_DIST, MENU_ACTION_MOUSE_SENS };

    auto &curScreen = aScreens[m_nCurrentScreen].m_aItems;

    // NOTSA section kept as original
    const auto pad = CPad::GetPad();

    {
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

    m_DisplayTheMouse |= !m_DisplayTheMouse && (m_nOldMousePosX && m_nOldMousePosY) &&
                         (m_nOldMousePosX != m_nMousePosX || m_nOldMousePosY != m_nMousePosY);

    // Check for mouse hover on menu options
    oldOption = m_nCurrentScreenItem;

    for (int rowToCheck = 0; rowToCheck < 12; ++rowToCheck) {
        if (curScreen[rowToCheck].m_nActionType >= 2 &&
            curScreen[rowToCheck].m_nActionType != eMenuAction::MENU_ACTION_TEXT) {
            float itemPosY = curScreen[rowToCheck].m_Y;
            float itemBottomY = itemPosY + 26; // PLUS_LINE_HEIGHT_ON_SCREEN

            bool mouseOverItem = (CMenuManager::StretchY(itemPosY) <= m_nMousePosY) &&
                                 (CMenuManager::StretchY(itemBottomY) >= m_nMousePosY);

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
                if ((notsa::contains(slidersActions, curScreen[rowToCheck].m_nActionType) == false)) {
                    m_MouseInBounds = 2;
                }

                break;
            }
            if (m_DisplayTheMouse) {
                m_nCurrentScreenItem = oldOption;
                m_CurrentMouseOption = oldOption;
            }
        }
    }

    // Process mouse and selection changes
    if (m_DisplayTheMouse && oldOption != m_nCurrentScreenItem) {
        if (curScreen[m_nCurrentScreenItem].m_nActionType == 1) {
            m_nCurrentScreenItem++;
            m_CurrentMouseOption++;
        }
        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_HIGHLIGHT, 0.0, 1.0);
    }

    // Update mouse position
    m_nOldMousePosY = m_nMousePosY;
    m_nOldMousePosX = m_nMousePosX;

    // Clamp mouse position within screen bounds
    m_nMousePosX = std::clamp(m_nMousePosWinX, 0, RsGlobal.maximumWidth);
    m_nMousePosY = std::clamp(m_nMousePosWinY, 0, RsGlobal.maximumHeight);

    // Handle special case for controls screen
    if (m_nCurrentScreen == eMenuScreen::SCREEN_CONTROLS_DEFINITION) {
        if (!m_nControllerError) {
            CMenuManager::RedefineScreenUserInput(&bEnter, &bExit);
        }
    } else {
        // Process menu navigation for screens with multiple options
        if (CMenuManager::GetNumberOfMenuOptions() > 1) {
            CMenuManager::AdditionalOptionInput(&bUp, &bDown);
            if (CMenuManager::CheckFrontEndDownInput()) {
                bDown = true;
                m_DisplayTheMouse = false;
            } else if (CMenuManager::CheckFrontEndUpInput()) {
                bUp = true;
                m_DisplayTheMouse = false;
            }
        }

        if (m_nCurrentScreenItem != eMenuScreen::SCREEN_STATS || m_nCurrentScreen != eMenuScreen::SCREEN_PAUSE_MENU) {
            // Normal enter key handling
            if ((pad->IsEnterJustPressed() && ((m_nSysMenu & 128u) != 0)) || pad->IsCrossPressed()) {
                m_DisplayTheMouse = false;
                bEnter = true;
            }
        } else {
            // Special handling for first item in main menu controls
            if ((pad->f0x57C330() && ((m_nSysMenu & 128u) != 0)) || pad->f0x57C3A0()) {
                m_DisplayTheMouse = false;
                bEnter = true;
            }
        }

        // Handle mouse click
        bEnter |= (pad->f0x57C3C0() && m_MouseInBounds == 2) &&
                  ((m_nCurrentScreen != eMenuScreen::SCREEN_MAP && CMenuManager::StretchY(388.0f) < m_nMousePosY) ||
                   m_DisplayTheMouse);

        // Handle slider movement
        if (pad->IsMouseLButton()) {
            if (notsa::contains({6, 8, 10, 12, 14}, m_MouseInBounds)) {
                CMenuManager::CheckSliderMovement(1);
            } else if (notsa::contains({7, 9, 11, 13, 15}, m_MouseInBounds)) {
                CMenuManager::CheckSliderMovement(-1);
            }
        }

        // Lambda to check if joystick moved
        bool joystickMoved = [pad]() -> bool { // sub_4410E0
            return ((WORD *)pad)[11] && !((WORD *)pad)[35];
        }();

        if (pad->IsMouseLButtonPressed() || pad->IsLeftPressed() || pad->IsRightPressed() || pad->IsDPadLeftPressed() ||
            joystickMoved || CPad::GetAnaloguePadLeft() || CPad::GetAnaloguePadRight() ||
            CPad::IsMouseWheelUpPressed() || CPad::IsMouseWheelDownPressed()) {
            int actionType = curScreen[m_nCurrentScreenItem].m_nActionType;
            if (notsa::contains(actionSelect, actionType)) {
                AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_SELECT, 0.0, 1.0);
            } else if (actionType == 0x1D) {
                AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_NOISE_TEST, 0.0, 1.0);
            }
        }

        // Only exit if not on main menus
        if ((pad->IsTrianglePressed() || pad->IsEscJustPressed() || pad->f0x57C360()) &&
            !notsa::contains(mainMenu, m_nCurrentScreen)) { // f0x57C360 notsa: back button
            m_DisplayTheMouse = false;
            bExit = true;
        }

        // Clear exit flag if any other action is taken
        if (bDown || bUp || bEnter) {
            bExit = false;
        }

        // Check for left/right slider movement
        const bool isRightPressed = pad->IsLeftDown() || pad->GetPedWalkLeftRight() < 0 || pad->f0x57C380();
        if (isRightPressed || pad->IsRightDown() || pad->GetPedWalkLeftRight() > 0 || pad->f0x57C390()) {
            auto &sliderMoveTime =
                isRightPressed ? FrontEndMenuManager.m_nTimeSlideLeftMove : FrontEndMenuManager.m_nTimeSlideRightMove;

            // Process slide left move with time delay
            if (CTimer::m_snTimeInMillisecondsPauseMode - sliderMoveTime > 200 &&
                (notsa::contains(slidersActions, curScreen[m_nCurrentScreenItem].m_nActionType) ||
                 curScreen[m_nCurrentScreenItem].m_nActionType == MENU_ACTION_STAT)) {
                sliderMove = isRightPressed ? -1 : 1;
                sliderMoveTime = CTimer::m_snTimeInMillisecondsPauseMode;
            }
        }

        // Handle mouse wheel movement
        if (sliderMove == 0) {
            const bool wheelMovedUp = pad->IsMouseWheelUpPressed();
            if ((wheelMovedUp || pad->IsMouseWheelDownPressed()) && m_nCurrentScreen != eMenuScreen::SCREEN_MAP) {
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

        if (sliderMove != 0 && notsa::contains(specialScreens, m_nCurrentScreen) &&
            !notsa::contains(specialMenuActions, curScreen[m_nCurrentScreenItem].m_nActionType)) {
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
        if (m_MenuIsAbleToQuit) {
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
void CMenuManager::AdditionalOptionInput(bool *pGoUpMenu, bool *pGoDownMenu) {
    // It hasn't been fully tested, it has a small bug when clicking that causes it to go back.
    plugin::CallMethod<0x5773D0, CMenuManager*, bool*, bool*>(this, pGoUpMenu, pGoDownMenu);
    /*
    // Define lambdas for the helper functions
    auto toggleBlipOption = [this](char menuResult) {
        switch (menuResult) {
            case 0: this->m_ShowLocationsBlips = !this->m_ShowLocationsBlips; break;
            case 1: this->m_ShowContactsBlips = !this->m_ShowContactsBlips; break;
            case 2: this->m_ShowMissionBlips = !this->m_ShowMissionBlips; break;
            case 3: this->m_ShowOtherBlips = !this->m_ShowOtherBlips; break;
            case 4: this->m_ShowGangAreaBlips = !this->m_ShowGangAreaBlips; break;
        }
    };

    auto femText = [](bool on) -> char* {
        return on ? (char*)"FEM_ON" : (char*)"FEM_OFF";
    };

    float v5;            // st7
    CPad *Pad;           // eax
    float v8;            // st7
    int32 blip;          // eax
    bool8 v10;           // bl
    float v12;           // st7
    float v13;           // st7
    float v14;           // st6
    float v15;           // st7
    float v16;           // st7
    float v17;           // st6
    float v18;           // st7
    float v19;           // st7
    float v20;           // st6
    float v21;           // st7
    float v22;           // st7
    float v23;           // st6
    float v24;           // st7
    bool8 v25;           // bl
    float v27;           // st7
    float v1;            // di
    unsigned __int8 v30; // bl
    float v32;           // st7
    bool8 v33;           // bl
    int32 v34;           // ecx
    float v35;           // st7
    float v37;           // st7
    float v38;           // st7
    float v39;           // st7
    float v40;           // st6
    float v41;           // st7
    // float v42 = 0;       // edx // UNREACHABLE
    float v43;           // st7
    float v44;           // st6
    float v45;           // st7
    float v46;           // st7
    float v47;           // st6
    float v48;           // st7
    // float v49 = 0;       // edx // UNREACHABLE
    float v50;           // st7
    float v51;           // st6
    float v52;           // st7
    float v53;           // st7
    float v54;           // st6
    float v55;           // st7
    uint32 v56;          // ecx
    float v57;           // st7
    float v58;           // st7
    float v59;           // st6
    float v60;           // st7
    float v61;           // st7
    float v62;           // st7
    float v63;           // st6
    float v64;           // st7
    float v65;           // st7
    float v66;           // st7
    float v67;           // st6
    float v68;           // st7
    float v69;           // st7
    float v70;           // st6
    uint8 NewMenu;       // al
    char v72;            // al
    char v73;            // al
    uint8 v75;           // al
    float v76;           // [esp+0h] [ebp-13Ch]
    int v77;             // [esp+4h] [ebp-138h]
    float v78;           // [esp+4h] [ebp-138h]
    float v79;           // [esp+8h] [ebp-134h]
    int v81;             // [esp+18h] [ebp-124h]
    float v82;           // [esp+2Ch] [ebp-110h]

    float v91;      // [esp+2Ch] [ebp-110h]
    float v92;      // [esp+2Ch] [ebp-110h]
    float v93;      // [esp+2Ch] [ebp-110h]
    float v94;      // [esp+2Ch] [ebp-110h]
    CVector2D v99;  // [esp+30h] [ebp-10Ch] BYREF
    char v100;      // [esp+3Bh] [ebp-101h]
    CVector2D v101; // [esp+3Ch] [ebp-100h] BYREF
    // char 50;      // [esp+47h] [ebp-F5h]
    float v103;     // [esp+48h] [ebp-F4h]
    float v104;     // [esp+4Ch] [ebp-F0h]
    float v105;     // [esp+50h] [ebp-ECh]
    float x;        // [esp+54h] [ebp-E8h]
    float y;        // [esp+58h] [ebp-E4h]
    int v108;       // [esp+5Ch] [ebp-E0h]
    float v114;     // [esp+94h] [ebp-A8h]
    float v115;     // [esp+98h] [ebp-A4h]
    float v116;     // [esp+9Ch] [ebp-A0h]
    float v117;     // [esp+A0h] [ebp-9Ch]
    CVector2D v118; // [esp+A4h] [ebp-98h] BYREF
    CVector2D v119; // [esp+ACh] [ebp-90h] BYREF
    CVector2D v120; // [esp+B4h] [ebp-88h] BYREF
    CVector2D v121; // [esp+BCh] [ebp-80h] BYREF
    CVector2D v122; // [esp+C4h] [ebp-78h] BYREF
    CVector2D v123; // [esp+CCh] [ebp-70h] BYREF
    CVector2D v124; // [esp+D4h] [ebp-68h] BYREF
    CVector2D v125; // [esp+DCh] [ebp-60h] BYREF
    CVector2D v126; // [esp+E4h] [ebp-58h] BYREF
    CVector2D v127; // [esp+ECh] [ebp-50h] BYREF
    CVector2D v128; // [esp+F4h] [ebp-48h] BYREF
    CVector2D v129; // [esp+FCh] [ebp-40h] BYREF
    CVector2D v130; // [esp+104h] [ebp-38h] BYREF
    CVector2D v131; // [esp+10Ch] [ebp-30h] BYREF
    CVector2D v132; // [esp+114h] [ebp-28h] BYREF
    CVector2D v133; // [esp+11Ch] [ebp-20h] BYREF
    CVector2D v134; // [esp+124h] [ebp-18h] BYREF
    CVector2D v135; // [esp+12Ch] [ebp-10h] BYREF
    CVector2D v136; // [esp+134h] [ebp-8h] BYREF

    if (this->m_nCurrentScreen) {
        if (this->m_nCurrentScreen == SCREEN_BRIEF) {
            if (CMenuManager::CheckFrontEndUpInput()) {
                m_nSelectedRow = this->m_nSelectedRow;
                if (m_nSelectedRow < 0x13u) {
                    if (CMessages::PreviousBriefs[m_nSelectedRow + 1].Text) {
                        this->m_nSelectedRow = m_nSelectedRow + 1;
                        *pGoUpMenu = 1;
                    }
                }
            }
            if (CMenuManager::CheckFrontEndDownInput()) {
                v75 = this->m_nSelectedRow;
                if (v75 > 3u) {
                    this->m_nSelectedRow = v75 - 1;
                    *pGoDownMenu = 1;
                }
            }
        } else if (this->m_nCurrentScreen == SCREEN_MAP && !this->m_bAllStreamingStuffLoaded) {
            FrontEndMenuManager.m_bDrawingMap = 1;
            m_nSysMenu = this->m_nSysMenu;
            v116 = this->m_vMapOrigin.y - this->m_fMapZoom;
            v104 = this->m_fMapZoom + this->m_vMapOrigin.y;
            v114 = this->m_vMapOrigin.x - this->m_fMapZoom;
            v105 = this->m_vMapOrigin.x + this->m_fMapZoom;
            v103 = 320.0f - this->m_vMapOrigin.x;
            v115 = 224.0f - this->m_vMapOrigin.y;
            v5 = 1.0f / this->m_fMapZoom;
            v117 = v103 * v5;
            v82 = v5 * v115;
            if (m_nSysMenu >= 0) {
            LABEL_171:
                CPad::GetPad(this->m_nPlayerNumber);
                if (CPad::NewKeyState.standardKeys[32]) {
                    CPad::GetPad(this->m_nPlayerNumber);
                    if (CPad::OldKeyState.standardKeys[32]) {
                        if (this->m_nSysMenu == 0x9D) {
                            AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_SELECT, 0.0f, 1.0f);
                            v79 = CMenuManager::StretchX(100.0f);
                            v78 = CMenuManager::StretchY(250.0f);
                            v76 = CMenuManager::StretchX(77.0f);
                            NewMenu = CMenuSystem::CreateNewMenu(CMenuSystem::eMenuType::MENU_TYPE_DEFAULT, "FEP_OPT", v76, v78, v79, 2, 1, 1, eFontAlignment::ALIGN_LEFT);
                            this->m_nSysMenu = NewMenu;
                            CMenuSystem::InsertMenu(NewMenu, 0, 0, (char *)"FED_BL1", (char *)"FED_BL2", (char *)"FED_BL3", (char *)"FED_BL4", (char *)"FED_BL5", 0, 0, 0, 0, 0, 0, 0);

                            CMenuSystem::InsertMenu(m_nSysMenu, 1, 0, femText(m_ShowLocationsBlips), femText(m_ShowContactsBlips), femText(m_ShowMissionBlips), femText(m_ShowOtherBlips), femText(m_ShowGangAreaBlips), nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);
                        }
                    }
                }
                if ((this->m_nSysMenu & 0x80u) == 0) {
                    v72 = CMenuSystem::CheckForAccept(this->m_nSysMenu);
                    if (v72 >= 0) {
                        ToggleBlipOption(this, v72);
                        CMenuSystem::InsertMenu(m_nSysMenu, 1, 0, femText(m_ShowLocationsBlips), femText(m_ShowContactsBlips), femText(m_ShowMissionBlips), femText(m_ShowOtherBlips), femText(m_ShowGangAreaBlips), nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);

                        if (CMenuSystem::CheckForSelected(this->m_nSysMenu) == 4) {
                            CMenuSystem::SetActiveMenuItem(this->m_nSysMenu, 0);
                        } else {
                            v73 = CMenuSystem::CheckForSelected(this->m_nSysMenu);
                            CMenuSystem::SetActiveMenuItem(this->m_nSysMenu, v73 + 1);
                        }
                    }
                    CPad::GetPad(this->m_nPlayerNumber);
                    if (!CPad::NewKeyState.standardKeys[32]) {
                        CPad::GetPad(this->m_nPlayerNumber);
                        if (!CPad::OldKeyState.standardKeys[32]) {
                            AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_BACK, 0.0f, 1.0f);
                            CMenuSystem::SwitchOffMenu(this->m_nSysMenu);
                            this->m_nSysMenu = -99;
                        }
                    }
                }
                CPad::GetPad(this->m_nPlayerNumber);
                if (CPad::NewKeyState.standardKeys[76] && (CPad::GetPad(this->m_nPlayerNumber), !CPad::OldKeyState.standardKeys[76]) ||
                    (CPad::GetPad(this->m_nPlayerNumber), CPad::NewKeyState.standardKeys[108]) && (CPad::GetPad(this->m_nPlayerNumber), !CPad::OldKeyState.standardKeys[108])) {
                    this->m_bMapLegend = !this->m_bMapLegend;
                }
                if (CTimer::m_snTimeInMillisecondsPauseMode - FrontEndMenuManager.field_1B38 > 0x14) {
                    FrontEndMenuManager.field_1B38 = CTimer::m_snTimeInMillisecondsPauseMode;
                }
                FrontEndMenuManager.m_bDrawingMap = 0;
                return;
            }
            Pad = CPad::GetPad(this->m_nPlayerNumber);
            if (Pad->NewState.ButtonCircle && !Pad->OldState.ButtonCircle ||
                (CPad::GetPad(this->m_nPlayerNumber), CPad::IsMouseRButtonPressed()) && (CPad::GetPad(this->m_nPlayerNumber), !CPad::NewMouseControllerState.isMouseLeftButtonPressed) ||
                (CPad::GetPad(this->m_nPlayerNumber), CPad::NewKeyState.standardKeys[84]) && (CPad::GetPad(this->m_nPlayerNumber), !CPad::OldKeyState.standardKeys[84]) ||
                (CPad::GetPad(this->m_nPlayerNumber), CPad::NewKeyState.standardKeys[116]) && (CPad::GetPad(this->m_nPlayerNumber), !CPad::OldKeyState.standardKeys[116])) {
                if (!CTheScripts::HideAllFrontEndMapBlips && !CTheScripts::bPlayerIsOffTheMap) {
                    if (this->m_nTargetBlipIndex) {
                        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_BACK, 0.0f, 1.0f);
                        CRadar::ClearBlip(this->m_nTargetBlipIndex);
                        this->m_nTargetBlipIndex = 0;
                    } else {
                        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_SELECT, 0.0f, 1.0f);
                        v8 = this->m_vMousePos.y;
                        x = this->m_vMousePos.x;
                        y = v8;
                        v108 = 0;
                        blip = CRadar::SetCoordBlip(BLIP_COORD, CVector(x, y, 0.0f), eBlipColour::BLIP_COLOUR_RED, eBlipDisplay::BLIP_DISPLAY_BLIPONLY);
                        this->m_nTargetBlipIndex = blip;
                        CRadar::SetBlipSprite(blip, eRadarSprite::RADAR_SPRITE_WAYPOINT);
                    }
                }
            }
            if (CPad::GetPad(this->m_nPlayerNumber)->NewState.LeftShoulder2 && !CPad::GetPad(this->m_nPlayerNumber)->NewState.RightShoulder2 ||
                (v10 = CPad::NewMouseControllerState.isMouseWheelMovedUp, CPad::GetPad(this->m_nPlayerNumber), v10) || (CPad::GetPad(this->m_nPlayerNumber), CPad::NewKeyState.pgup)) {
                if (CTimer::m_snTimeInMillisecondsPauseMode - FrontEndMenuManager.field_1B38 > 0x14) {
                    if (this->m_fMapZoom >= 1100.0f) {
                        this->m_fMapZoom = 1100.0f;
                    } else {
                        m_nPlayerNumber = this->m_nPlayerNumber;
                        this->m_fMapZoom = this->m_fMapZoom + 7.0f;
                        CPad::GetPad(m_nPlayerNumber);
                        if (CPad::NewMouseControllerState.isMouseWheelMovedUp) {
                            this->m_fMapZoom = this->m_fMapZoom + 21.0f;
                        }
                        this->m_vMapOrigin.x = this->m_vMapOrigin.x - (v117 * this->m_fMapZoom - v103);
                        this->m_vMapOrigin.y = this->m_vMapOrigin.y - (v82 * this->m_fMapZoom - v115);
                    }
                    v12 = this->m_vMousePos.y;
                    v121.x = this->m_vMousePos.x;
                    v121.y = v12;
                    CRadar::TransformRealWorldPointToRadarSpace(v99, v121);
                    CRadar::LimitRadarPoint(v99);
                    CRadar::TransformRadarPointToScreenSpace(v101, v99);
                    while (v101.x > 576.0f) {
                        v13 = this->m_vMousePos.x - 1.0f;
                        this->m_vMousePos.x = v13;
                        v14 = v13;
                        v15 = this->m_vMousePos.y;
                        v134.x = v14;
                        v134.y = v15;
                        CRadar::TransformRealWorldPointToRadarSpace(v99, v134);
                        CRadar::LimitRadarPoint(v99);
                        CRadar::TransformRadarPointToScreenSpace(v101, v99);
                    }
                    while (v101.x < 64.0f) {
                        v16 = this->m_vMousePos.x + 1.0f;
                        this->m_vMousePos.x = v16;
                        v17 = v16;
                        v18 = this->m_vMousePos.y;
                        v136.x = v17;
                        v136.y = v18;
                        CRadar::TransformRealWorldPointToRadarSpace(v99, v136);
                        CRadar::LimitRadarPoint(v99);
                        CRadar::TransformRadarPointToScreenSpace(v101, v99);
                    }
                    while (v101.y < 64.0f) {
                        v19 = this->m_vMousePos.y - 1.0f;
                        this->m_vMousePos.y = v19;
                        v20 = v19;
                        v21 = this->m_vMousePos.x;
                        y = v20;
                        v135.y = y;
                        v135.x = v21;
                        CRadar::TransformRealWorldPointToRadarSpace(v99, v135);
                        CRadar::LimitRadarPoint(v99);
                        CRadar::TransformRadarPointToScreenSpace(v101, v99);
                    }
                    while (v101.y > 384.0f) {
                        v22 = this->m_vMousePos.y + 1.0f;
                        this->m_vMousePos.y = v22;
                        v23 = v22;
                        v24 = this->m_vMousePos.x;
                        y = v23;
                        v118.y = y;
                        v118.x = v24;
                        CRadar::TransformRealWorldPointToRadarSpace(v99, v118);
                        CRadar::LimitRadarPoint(v99);
                        CRadar::TransformRadarPointToScreenSpace(v101, v99);
                    }
                }
            }
            if (CPad::GetPad(this->m_nPlayerNumber)->NewState.RightShoulder2 && !CPad::GetPad(this->m_nPlayerNumber)->NewState.LeftShoulder2 ||
                (v25 = CPad::NewMouseControllerState.isMouseWheelMovedDown, CPad::GetPad(this->m_nPlayerNumber), v25) || (CPad::GetPad(this->m_nPlayerNumber), CPad::NewKeyState.pgdn)) {
                if (CTimer::m_snTimeInMillisecondsPauseMode - FrontEndMenuManager.field_1B38 > 0x14) {
                    if (this->m_fMapZoom <= 300.0f) {
                        this->m_fMapZoom = 300.0f;
                    } else {
                        v81 = this->m_nPlayerNumber;
                        this->m_fMapZoom = this->m_fMapZoom - 7.0f;
                        // isMouseWheelMovedDown = CPad::NewMouseControllerState.isMouseWheelMovedDown;
                        CPad::GetPad(v81);
                        if (CPad::NewMouseControllerState.isMouseWheelMovedDown) {
                            this->m_fMapZoom = this->m_fMapZoom - 21.0f;
                        }
                        this->m_vMapOrigin.x = this->m_vMapOrigin.x - (v117 * this->m_fMapZoom - v103);
                        this->m_vMapOrigin.y = this->m_vMapOrigin.y - (v82 * this->m_fMapZoom - v115);
                    }
                }
            }
            v27 = this->m_vMousePos.y;
            v129.x = this->m_vMousePos.x;
            v129.y = v27;
            CRadar::TransformRealWorldPointToRadarSpace(v99, v129);
            CRadar::LimitRadarPoint(v99);
            CRadar::TransformRadarPointToScreenSpace(v101, v99);
            v1 = 0;
            v77 = this->m_nPlayerNumber;
            v103 = 0.0f;
            CPad::GetPad(v77);
            if (CPad::NewKeyState.up) {
                v103 = -128;
            }
            CPad::GetPad(this->m_nPlayerNumber);
            if (CPad::NewKeyState.down) {
                (v103) = 128;
            }
            CPad::GetPad(this->m_nPlayerNumber);
            if (CPad::NewKeyState.left) {
                v1 = -128;
            }
            CPad::GetPad(this->m_nPlayerNumber);
            if (CPad::NewKeyState.right) {
                v1 = 128;
            }
            if (CPad::GetPad(this->m_nPlayerNumber)->NewState.LeftStickX) {
                v1 = CPad::GetPad(this->m_nPlayerNumber)->NewState.LeftStickX;
            }
            if (CPad::GetPad(this->m_nPlayerNumber)->NewState.LeftStickY) {
                (v103) = CPad::GetPad(this->m_nPlayerNumber)->NewState.LeftStickY;
            }
            if (CPad::GetPad(this->m_nPlayerNumber)->NewState.LeftStickX) {
                v1 = CPad::GetPad(this->m_nPlayerNumber)->NewState.LeftStickX;
            }
            if (CPad::GetPad(this->m_nPlayerNumber)->NewState.DPadUp) {
                (v103) = -(CPad::GetPad(this->m_nPlayerNumber)->NewState.DPadUp * 0.60000002f);
            }
            if (CPad::GetPad(this->m_nPlayerNumber)->NewState.DPadDown) {
                (v103) = (CPad::GetPad(this->m_nPlayerNumber)->NewState.DPadDown * 0.60000002f);
            }
            if (CPad::GetPad(this->m_nPlayerNumber)->NewState.DPadLeft) {
                v1 = -(CPad::GetPad(this->m_nPlayerNumber)->NewState.DPadLeft * 0.60000002f);
            }
            if (CPad::GetPad(this->m_nPlayerNumber)->NewState.DPadRight) {
                v1 = (CPad::GetPad(this->m_nPlayerNumber)->NewState.DPadRight * 0.60000002f);
            }
            m_DisplayTheMouse = this->m_DisplayTheMouse;
            v30 = 2;
            v100 = 0;
            if (!m_DisplayTheMouse) {
                v100 = 1;
            LABEL_84:
                if (v1 > 0) {
                    if (v105 > 580.0f && v101.x >= 320.0f && v100) {
                        if (CTimer::m_snTimeInMillisecondsPauseMode - FrontEndMenuManager.field_1B38 > 0x14) {
                            this->m_vMapOrigin.x = this->m_vMapOrigin.x - v1 * 0.0078125f * 7.0f;
                        }
                        v38 = this->m_vMousePos.y;
                        v119.x = this->m_vMousePos.x;
                        v119.y = v38;
                        CRadar::TransformRealWorldPointToRadarSpace(v99, v119);
                        CRadar::LimitRadarPoint(v99);
                        CRadar::TransformRadarPointToScreenSpace(v101, v99);
                        while (v105 > 580.0f) {
                            if (v101.x >= 320.0f) {
                                break;
                            }
                            v39 = this->m_vMousePos.x + 1.0f;
                            this->m_vMousePos.x = v39;
                            v40 = v39;
                            v41 = this->m_vMousePos.y;
                            v120.x = v40;
                            v120.y = v41;
                            CRadar::TransformRealWorldPointToRadarSpace(v99, v120);
                            CRadar::LimitRadarPoint(v99);
                            CRadar::TransformRadarPointToScreenSpace(v101, v99);
                        }
                    } else {
                        v56 = CTimer::m_snTimeInMillisecondsPauseMode;
                        if (CTimer::m_snTimeInMillisecondsPauseMode - FrontEndMenuManager.field_1B38 <= 0x14 || v101.x >= 576.0f) {
                        LABEL_114:
                            if (v1 >= 0) {
                                goto LABEL_128;
                            }
                            if (v114 < 60.0f && v101.x <= 320.0f && v100) {
                                if (v56 - FrontEndMenuManager.field_1B38 > 0x14) {
                                    this->m_vMapOrigin.x = -v1 * 0.0078125f * 7.0f + this->m_vMapOrigin.x;
                                }
                                v57 = this->m_vMousePos.y;
                                v122.x = this->m_vMousePos.x;
                                v122.y = v57;
                                CRadar::TransformRealWorldPointToRadarSpace(v99, v122);
                                CRadar::LimitRadarPoint(v99);
                                CRadar::TransformRadarPointToScreenSpace(v101, v99);
                                while (v114 < 60.0f) {
                                    if (v101.x <= 320.0f) {
                                        break;
                                    }
                                    v58 = this->m_vMousePos.x - 1.0f;
                                    this->m_vMousePos.x = v58;
                                    v59 = v58;
                                    v60 = this->m_vMousePos.y;
                                    v124.x = v59;
                                    v124.y = v60;
                                    CRadar::TransformRealWorldPointToRadarSpace(v99, v124);
                                    CRadar::LimitRadarPoint(v99);
                                    CRadar::TransformRadarPointToScreenSpace(v101, v99);
                                }
                            } else {
                                if (v56 - FrontEndMenuManager.field_1B38 <= 0x14 || v101.x <= 64.0f) {
                                LABEL_128:
                                    if ((v103) > 0) {
                                        if (v104 > 388.0f && v101.y >= 224.0f && v100) {
                                            if (v56 - FrontEndMenuManager.field_1B38 > 0x14) {
                                                this->m_vMapOrigin.y = this->m_vMapOrigin.y - (v103) * 0.0078125f * 7.0f;
                                            }
                                            v61 = this->m_vMousePos.y;
                                            v126.x = this->m_vMousePos.x;
                                            v126.y = v61;
                                            CRadar::TransformRealWorldPointToRadarSpace(v99, v126);
                                            CRadar::LimitRadarPoint(v99);
                                            CRadar::TransformRadarPointToScreenSpace(v101, v99);
                                            while (v104 > 388.0f) {
                                                if (v101.y >= 224.0f) {
                                                    break;
                                                }
                                                v62 = this->m_vMousePos.y - 1.0f;
                                                this->m_vMousePos.y = v62;
                                                v63 = v62;
                                                v64 = this->m_vMousePos.x;
                                                y = v63;
                                                v128.y = y;
                                                v128.x = v64;
                                                CRadar::TransformRealWorldPointToRadarSpace(v99, v128);
                                                CRadar::LimitRadarPoint(v99);
                                                CRadar::TransformRadarPointToScreenSpace(v101, v99);
                                            }
                                        } else {
                                            if (v56 - FrontEndMenuManager.field_1B38 <= 0x14 || v101.y >= 384.0f) {
                                                goto LABEL_142;
                                            }
                                            this->m_vMousePos.y = this->m_vMousePos.y - (7 * v30) * ((v103) * 0.0078125f);
                                        }
                                        v56 = CTimer::m_snTimeInMillisecondsPauseMode;
                                    }
                                LABEL_142:
                                    if ((v103) < 0) {
                                        if (v116 < 60.0f && v101.y <= 224.0f && v100) {
                                            if (v56 - FrontEndMenuManager.field_1B38 > 0x14) {
                                                this->m_vMapOrigin.y = -(v103) * 0.0078125f * 7.0f + this->m_vMapOrigin.y;
                                            }
                                            v65 = this->m_vMousePos.y;
                                            v130.x = this->m_vMousePos.x;
                                            v130.y = v65;
                                            CRadar::TransformRealWorldPointToRadarSpace(v99, v130);
                                            CRadar::LimitRadarPoint(v99);
                                            CRadar::TransformRadarPointToScreenSpace(v101, v99);
                                            while (v116 < 60.0f) {
                                                if (v101.y <= 224.0f) {
                                                    break;
                                                }
                                                v66 = this->m_vMousePos.y + 1.0f;
                                                this->m_vMousePos.y = v66;
                                                v67 = v66;
                                                v68 = this->m_vMousePos.x;
                                                y = v67;
                                                v132.y = y;
                                                v132.x = v68;
                                                CRadar::TransformRealWorldPointToRadarSpace(v99, v132);
                                                CRadar::LimitRadarPoint(v99);
                                                CRadar::TransformRadarPointToScreenSpace(v101, v99);
                                            }
                                        } else if (v56 - FrontEndMenuManager.field_1B38 > 0x14 && v101.y > 64.0f) {
                                            this->m_vMousePos.y = -(v103) * 0.0078125f * (7 * v30) + this->m_vMousePos.y;
                                        }
                                    }
                                LABEL_155:
                                    v69 = this->m_vMapOrigin.y - this->m_fMapZoom;
                                    v104 = this->m_fMapZoom + this->m_vMapOrigin.y;
                                    v70 = this->m_vMapOrigin.x - this->m_fMapZoom;
                                    v105 = this->m_vMapOrigin.x + this->m_fMapZoom;
                                    if (v70 > 60.0f) {
                                        this->m_vMapOrigin.x = this->m_vMapOrigin.x - (v70 - 60.0f);
                                    }
                                    if (v105 < 580.0f) {
                                        this->m_vMapOrigin.x = 580.0f - v105 + this->m_vMapOrigin.x;
                                    }
                                    if (v69 > 60.0f) {
                                        this->m_vMapOrigin.y = this->m_vMapOrigin.y - (v69 - 60.0f);
                                    }
                                    if (v104 < 388.0f) {
                                        this->m_vMapOrigin.y = 388.0f - v104 + this->m_vMapOrigin.y;
                                    }
                                    if (this->m_vMousePos.x > 3000.0f) {
                                        this->m_vMousePos.x = 3000.0f;
                                    }
                                    if (this->m_vMousePos.x < -3000.0f) {
                                        this->m_vMousePos.x = -3000.0f;
                                    }
                                    if (this->m_vMousePos.y > 3000.0f) {
                                        this->m_vMousePos.y = 3000.0f;
                                    }
                                    if (this->m_vMousePos.y < -3000.0f) {
                                        this->m_vMousePos.y = -3000.0f;
                                    }
                                    goto LABEL_171;
                                }
                                this->m_vMousePos.x = (7 * v30) * (v1 * 0.0078125f) + this->m_vMousePos.x;
                            }
                            v56 = CTimer::m_snTimeInMillisecondsPauseMode;
                            goto LABEL_128;
                        }
                        this->m_vMousePos.x = this->m_vMousePos.x - -v1 * 0.0078125f * (7 * v30);
                    }
                }
                v56 = CTimer::m_snTimeInMillisecondsPauseMode;
                goto LABEL_114;
            }

            if (CMenuManager::StretchY(60.0f) >= m_nMousePosY) {
                v30 = 50;
                goto LABEL_84;
            }
            if (CMenuManager::StretchY(388.0f) <= m_nMousePosY) {
                v30 = 50;
                goto LABEL_84;
            }
            if (CMenuManager::StretchX(60.0f) >= m_nMousePosX || CMenuManager::StretchX(580.0f) <= m_nMousePosX) {
                v30 = 50;
                goto LABEL_84;
            }
            v32 = this->m_vMousePos.y;
            v123.x = this->m_vMousePos.x;
            v123.y = v32;
            CRadar::TransformRealWorldPointToRadarSpace(v99, v123);
            CRadar::LimitRadarPoint(v99);
            CRadar::TransformRadarPointToScreenSpace(v101, v99);
            v33 = CPad::NewMouseControllerState.isMouseLeftButtonPressed;
            CPad::GetPad(this->m_nPlayerNumber);
            if (!v33) {
                v91 = (float)this->m_nMousePosX;
                if (CMenuManager::StretchX(v101.x) > v91) {
                    do {
                        if (v101.x <= 64.0f) {
                            break;
                        }
                        v43 = this->m_vMousePos.x - 14.0f;
                        this->m_vMousePos.x = v43;
                        v44 = v43;
                        v45 = this->m_vMousePos.y;
                        v133.x = v44;
                        v133.y = v45;
                        CRadar::TransformRealWorldPointToRadarSpace(v99, v133);
                        CRadar::LimitRadarPoint(v99);
                        CRadar::TransformRadarPointToScreenSpace(v101, v99);
                        v92 = (float)this->m_nMousePosX;
                    } while (CMenuManager::StretchX(v101.x) > v92);
                }
                v93 = (float)this->m_nMousePosX;
                // if (CMenuManager::StretchX(v42) < v93) {
                if (CMenuManager::StretchX(v101.x) < v93) { // Probably v101.x
                    do {
                        if (v101.x >= 576.0f) {
                            break;
                        }
                        v46 = this->m_vMousePos.x - -14.0f;
                        this->m_vMousePos.x = v46;
                        v47 = v46;
                        v48 = this->m_vMousePos.y;
                        v125.x = v47;
                        v125.y = v48;
                        CRadar::TransformRealWorldPointToRadarSpace(v99, v125);
                        CRadar::LimitRadarPoint(v99);
                        CRadar::TransformRadarPointToScreenSpace(v101, v99);
                        v94 = (float)this->m_nMousePosX;
                    } while (CMenuManager::StretchX(v101.x) < v94);
                }
                if (CMenuManager::StretchY(v101.y) > this->m_nMousePosY) {
                    do {
                        if (v101.y <= 64.0f) {
                            break;
                        }
                        v50 = this->m_vMousePos.y + 14.0f;
                        this->m_vMousePos.y = v50;
                        v51 = v50;
                        v52 = this->m_vMousePos.x;
                        y = v51;
                        v131.y = y;
                        v131.x = v52;
                        CRadar::TransformRealWorldPointToRadarSpace(v99, v131);
                        CRadar::LimitRadarPoint(v99);
                        CRadar::TransformRadarPointToScreenSpace(v101, v99);
                    } while (CMenuManager::StretchY(v101.y) > this->m_nMousePosY);
                }
                // if (CMenuManager::StretchY(v49) < this->m_nMousePosY) {
                if (CMenuManager::StretchY(v101.y) < this->m_nMousePosY) {
                    do {
                        if (v101.y >= 384.0f) {
                            break;
                        }
                        v53 = this->m_vMousePos.y - 14.0f;
                        this->m_vMousePos.y = v53;
                        v54 = v53;
                        v55 = this->m_vMousePos.x;
                        y = v54;
                        v127.y = y;
                        v127.x = v55;
                        CRadar::TransformRealWorldPointToRadarSpace(v99, v127);
                        CRadar::LimitRadarPoint(v99);
                        CRadar::TransformRadarPointToScreenSpace(v101, v99);
                    } while (CMenuManager::StretchY(v101.y) < this->m_nMousePosY);
                }
                goto LABEL_155;
            }
            v34 = this->m_nMousePosX;
            v100 = 1;
            if (v34 >= RsGlobal.maximumWidth / 2) {
                if (v34 <= RsGlobal.maximumWidth / 2) {
                LABEL_80:
                    if (this->m_nMousePosY >= RsGlobal.maximumHeight / 2) {
                        if (this->m_nMousePosY <= RsGlobal.maximumHeight / 2) {
                            v30 = 50;
                            goto LABEL_84;
                        }
                        v37 = (this->m_nMousePosY - CMenuManager::StretchY(224.0f)) / CMenuManager::StretchY(224.0f) * 128.0f;
                    } else {
                        v37 = -((128.0f - this->m_nMousePosY) / CMenuManager::StretchY(224.0f) * 128.0f);
                    }
                    (v103) = v37;
                    v30 = 50;
                    goto LABEL_84;
                }
                v35 = ((float)v34 - CMenuManager::StretchX(320.0f)) / CMenuManager::StretchX(320.0f) * 128.0f;
            } else {
                v35 = -(128.0f - (float)v34 / CMenuManager::StretchX(320.0f) * 128.0f);
            }
            v1 = (short)v35;
            goto LABEL_80;
        }
    } else {
        CPad::GetPad(this->m_nPlayerNumber);
        if (CPad::NewKeyState.standardKeys[83] && (CPad::GetPad(this->m_nPlayerNumber), !CPad::OldKeyState.standardKeys[83]) ||
            (CPad::GetPad(this->m_nPlayerNumber), CPad::NewKeyState.standardKeys[115]) && (CPad::GetPad(this->m_nPlayerNumber), !CPad::OldKeyState.standardKeys[115])) {
            CMenuManager::SaveStatsToFile();
        }
    }*/
}

/*!
 * @addr 0x57EF50
 */
void CMenuManager::RedefineScreenUserInput(bool* enter, bool* exit) {
    if (m_EditingControlOptions) {
        return;
    }

    auto& pad = *CPad::GetPad(0);
    int   listSize;

    if (m_nCurrentScreen != 0x26) {
        listSize = *enter ? 1 : 0;
    } else {
        switch (m_RedefiningControls) {
        case 0:
            listSize = m_ControlMethod ? 28 : 22;
            break;
        case 1:
            listSize = 25;
            break;
        }
    }

    // Enter key handling
    if ((pad.NewKeyState.enter && !pad.OldKeyState.enter || pad.NewKeyState.extenter && !pad.OldKeyState.extenter) && (m_nSysMenu & 0x80u) != 0 || pad.NewState.ButtonCross && !pad.OldState.ButtonCross) {
        m_DisplayTheMouse = 0;
        *enter            = 1;
    }

    // Back key handling
    if (pad.NewKeyState.back && !pad.OldKeyState.back && m_nCurrentScreen == 0x26 && !m_JustExitedRedefine) {
        m_MouseInBounds                 = 16;
        m_EditingControlOptions         = 1;
        m_bJustOpenedControlRedefWindow = 1;
        m_DeleteAllBoundControls        = 1;
        m_pPressedKey                   = &m_KeyPressedCode;
    } else {
        m_JustExitedRedefine = 0;
    }

    // Timing check
    if (CTimer::m_snTimeInMillisecondsPauseMode - FrontEndMenuManager.m_LastPressed > 200) {
        field_1AF0                        = 0;
        field_1AF4                        = 0;
        FrontEndMenuManager.m_LastPressed = CTimer::m_snTimeInMillisecondsPauseMode;
    }

    // Up navigation
    if (pad.NewKeyState.up || pad.GetAnaloguePadUp() || (pad.NewMouseControllerState.isMouseWheelMovedUp && !pad.OldMouseControllerState.isMouseWheelMovedUp)) {
        m_DisplayTheMouse = 1;
        if (!field_1AF2) {
            field_1AF2                        = 1;
            FrontEndMenuManager.m_LastPressed = CTimer::m_snTimeInMillisecondsPauseMode;
            if (m_ListSelection > 0) {
                m_ListSelection--;
            } else {
                m_ListSelection = listSize - 1;
            }
        }
    } else {
        field_1AF2 = 0;
    }

    // Down navigation
    if (pad.NewKeyState.down || pad.GetAnaloguePadDown() || (pad.NewMouseControllerState.isMouseWheelMovedDown && !pad.OldMouseControllerState.isMouseWheelMovedDown)) {
        m_DisplayTheMouse = 1;
        if (!field_1AF3) {
            field_1AF3                        = 1;
            FrontEndMenuManager.m_LastPressed = CTimer::m_snTimeInMillisecondsPauseMode;
            if (m_ListSelection < listSize - 1) {
                m_ListSelection++;
            } else {
                m_ListSelection = 0;
            }
        }
    } else {
        field_1AF3 = 0;
    }

    // Exit handling
    if ((pad.NewKeyState.esc && !pad.OldKeyState.esc) || (pad.NewState.ButtonTriangle && !pad.OldState.ButtonTriangle)) {
        m_DisplayTheMouse = 0;
        *exit             = 1;
    }

    // Mouse click handling
    if (pad.NewMouseControllerState.isMouseLeftButtonPressed && !pad.OldMouseControllerState.isMouseLeftButtonPressed) {
        if (m_MouseInBounds == 3) {
            *exit = 1;
        } else if (m_MouseInBounds == 4) {
            AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_SELECT, 0.0f, 1.0f);
            m_MouseInBounds = 5;
        }
    }

    // Final state update
    if (*exit) {
        if (m_MenuIsAbleToQuit) {
            m_RedefiningControls = !m_RedefiningControls;
            DrawControllerBound(0x45, true);
            if (!m_MenuIsAbleToQuit && m_RedefiningControls) {
                m_nControllerError = 2;
            }
        } else {
            m_nControllerError = m_RedefiningControls;
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
            m_JustDownJoyButton = 0;

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

            m_JustDownJoyButton = ControlsManager.GetJoyButtonJustDown();

            // Android
            auto TypeOfControl = eControllerType::KEYBOARD;
            if (m_JustDownJoyButton != NO_JOYBUTTONS) {
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
                    if (*m_pPressedKey != rsNULL || m_nPressedMouseButton || m_JustDownJoyButton) {
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
                m_DisplayComboButtonErrMsg = 0;
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
    m_DisplayComboButtonErrMsg = false;
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
        m_DisplayComboButtonErrMsg = (ControlsManager.GetIsActionAButtonCombo(actionId)) ? 1 : 0;
        break;
    }

    // Handle escape key or invalid key press
    if (escapePressed || invalidKeyPressed || (m_DisplayComboButtonErrMsg && escapePressed)) {
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
            ControlsManager.DeleteMatchingActionInitiators(actionId, m_JustDownJoyButton, eControllerType::JOY_STICK);
            ControlsManager.SetControllerKeyAssociatedWithAction(actionId, m_JustDownJoyButton, controllerType);
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
