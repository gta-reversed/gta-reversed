#include "StdInc.h"

#include "ControllerConfigManager.h"
#include "Input.h"

CControllerConfigManager& ControlsManager = *(CControllerConfigManager *) 0xB70198;
GxtChar (&NewStringWithNumber)[32] = *(GxtChar(*)[32])0xB7147C;

void CControllerConfigManager::InjectHooks() {
    RH_ScopedClass(CControllerConfigManager);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Constructor, 0x531EE0);
    RH_ScopedInstall(LoadSettings, 0x530530);
    RH_ScopedInstall(SaveSettings, 0x52D200, { .reversed = false });
    RH_ScopedInstall(InitializeDefaultKeyboardAndMouseBindings, 0x530640);
    RH_ScopedInstall(InitDefaultControlConfiguration, 0x530B00);
    RH_ScopedInstall(InitDefaultControlConfigMouse, 0x52F6F0);
    RH_ScopedInstall(InitialiseControllerActionNameArray, 0x52D260);
    RH_ScopedInstall(ReinitControls, 0x531F20, { .reversed = false });
    RH_ScopedInstall(SetMouseButtonAssociatedWithAction, 0x52F590);
    RH_ScopedInstall(StoreMouseButtonState, 0x52DA30);
    RH_ScopedInstall(UpdateJoyInConfigMenus_ButtonDown, 0x52DAB0);
    RH_ScopedInstall(UpdateJoy_ButtonDown, 0x530ED0);
    RH_ScopedInstall(UpdateJoy_ButtonUp, 0x531070);
    RH_ScopedInstall(StoreJoyButtonStates, 0x52F510);
    RH_ScopedInstall(AffectControllerStateOn_ButtonDown_DebugStuff, 0x52DC10);
    RH_ScopedInstall(UpdateJoyInConfigMenus_ButtonUp, 0x52DC20);
    RH_ScopedInstall(AffectControllerStateOn_ButtonUp_DebugStuff, 0x52DD80);
    RH_ScopedInstall(ClearSimButtonPressCheckers, 0x52DD90);
    RH_ScopedInstall(GetJoyButtonJustUp, 0x52D1C0);
    RH_ScopedInstall(GetJoyButtonJustDown, 0x52D1E0);
    RH_ScopedInstall(GetIsKeyboardKeyDown, 0x52DDB0);
    RH_ScopedInstall(GetIsKeyboardKeyJustDown, 0x52E450);
    RH_ScopedInstall(GetIsMouseButtonDown, 0x52EF30);
    RH_ScopedInstall(GetIsMouseButtonUp, 0x52F020);
    RH_ScopedInstall(GetIsMouseButtonJustUp, 0x52F110);
    RH_ScopedInstall(GetIsKeyBlank, 0x52F2A0);
    RH_ScopedInstall(GetActionType, 0x52F2F0);
    RH_ScopedInstall(GetControllerKeyAssociatedWithAction, 0x52F4F0);
    RH_ScopedInstall(ResetSettingOrder, 0x52F5F0);
    RH_ScopedInstall(HandleButtonPress, 0x52F7B0);
    RH_ScopedInstall(Handle1stPersonCameraActions, 0x52F9E0);
    RH_ScopedInstall(GetControllerSettingTextMouse, 0x52F390);
    RH_ScopedInstall(GetControllerSettingTextJoystick, 0x52F450);
    RH_ScopedInstall(GetIsActionAButtonCombo, 0x52F550);
    RH_ScopedInstall(GetButtonComboText, 0x52F560);
    RH_ScopedInstall(GetMouseButtonAssociatedWithAction, 0x52F580);
    RH_ScopedInstall(UpdatePadStateOnActions, 0x52FA20);
    RH_ScopedInstall(UpdateControllerStateOnAction, 0x52FAB0);
    RH_ScopedInstall(UpdatePadStateOnMenuActions, 0x52FCA0);
    RH_ScopedInstall(SetVehicleEnterExitButton, 0x52FD20);
    RH_ScopedInstall(HandleButtonRelease, 0x52FD40);
    RH_ScopedInstall(ClearSettingsAssociatedWithAction, 0x52FD70);
    RH_ScopedInstall(GetKeyNameForKeyboard, 0x52FE10);
    RH_ScopedInstall(UnmapVehicleEnterExit, 0x531BC0);
    RH_ScopedInstall(ClearSniperZoomMappings, 0x531C20);
    RH_ScopedInstall(DeleteMapping, 0x531C90);
    RH_ScopedInstall(GetControllerSettingText, 0x531E20);
    RH_ScopedInstall(GetActionKeyName, 0x531E90);
    RH_ScopedInstall(GetDefinedKeyByGxtName, 0x5303D0);
    RH_ScopedInstall(ProcessMouseInput, 0x5314A0);
    RH_ScopedInstall(Clear1st3rdPersonMappings, 0x5318C0);
    RH_ScopedInstall(ClearVehicleMappings, 0x5319D0);
    RH_ScopedInstall(SetKeyForAction, 0x530490);
    RH_ScopedInstall(MakeControllerActionsBlank, 0x530500);
    RH_ScopedInstall(ProcessKeyboardInput, 0x531140);
    RH_ScopedInstall(ClearCommonMappings, 0x531670);
    RH_ScopedInstall(ClearPedMappings, 0x531730);
}

// 0x531EE0
CControllerConfigManager::CControllerConfigManager() {
    for (auto i = 0u; i < NUM_OF_CONTROLLER_ACTIONS; ++i) {
        for (auto j = 0u; j < CONTROLLER_NUM; ++j) {
            m_Actions[i].Keys[j].KeyCode  = 0;
            m_Actions[i].Keys[j].Priority = PRIORITY_DISABLE;
        }
    }

    m_bJoyJustInitialised = false;
    MouseFoundInitSet     = false;

    MakeControllerActionsBlank();
    InitializeDefaultKeyboardAndMouseBindings();
    InitialiseControllerActionNameArray();
}

CControllerConfigManager* CControllerConfigManager::Constructor() {
    this->CControllerConfigManager::CControllerConfigManager();
    return this;
}

// 0x531730
void CControllerConfigManager::ClearPedMappings(eControllerAction action, int32 button, eControllerType type) {
    if (!GetIsKeyBlank(button, type)) {
        return;
    }

    CheckAndClear(CA_GO_LEFT, type, button);
    CheckAndClear(CA_GO_RIGHT, type, button);
    CheckAndClear(CA_GROUP_CONTROL_FWD, type, button);
    CheckAndClear(CA_GROUP_CONTROL_BWD, type, button);
    CheckAndClear(CA_CONVERSATION_NO, type, button);
    CheckAndClear(CA_CONVERSATION_YES, type, button);
    CheckAndClear(CA_PED_CYCLE_WEAPON_LEFT, type, button);
    CheckAndClear(CA_PED_CYCLE_WEAPON_RIGHT, type, button);
    CheckAndClear(CA_PED_JUMPING, type, button);
    CheckAndClear(CA_PED_SPRINT, type, button);
    CheckAndClear(CA_PED_LOOKBEHIND, type, button);
    CheckAndClear(CA_PED_DUCK, type, button);

    if (action != CA_PED_FIRE_WEAPON_ALT) {
        CheckAndClear(CA_PED_ANSWER_PHONE, type, button);
    }

    CheckAndClear(CA_PED_WALK, type, button);

    if (FrontEndMenuManager.m_bController) {
        CheckAndClear(CA_PED_CENTER_CAMERA_BEHIND_PLAYER, type, button);
    }
}


// 0x531670
void CControllerConfigManager::ClearCommonMappings(eControllerAction nop, int32 button, eControllerType type) {
    if (!GetIsKeyBlank(button, type)) {
        return;
    }

    CheckAndClear(CA_CAMERA_CHANGE_VIEW_ALL_SITUATIONS, type, button);
    CheckAndClear(CA_NETWORK_TALK, type, button);
    CheckAndClear(CA_SWITCH_DEBUG_CAM_ON, type, button);
    CheckAndClear(CA_TOGGLE_DPAD, type, button);
    CheckAndClear(CA_TAKE_SCREEN_SHOT, type, button);
    CheckAndClear(CA_SHOW_MOUSE_POINTER_TOGGLE, type, button);
}

// 0x530490
bool CControllerConfigManager::SetKeyForAction(eControllerAction action, int32 button, eControllerType type) {
    ResetSettingOrder(action);
    auto existingMappings = 0u;
    for (auto i = 0u; i < CONTROLLER_NUM; ++i) {
        if (m_Actions[action].Keys[i].KeyCode != rsNULL) {
            existingMappings++;
        }
    }
    m_Actions[action].Keys[type].KeyCode  = button;
    m_Actions[action].Keys[type].Priority = (ePriority)existingMappings++;
    return true;
}

// 0x5319D0
void CControllerConfigManager::ClearVehicleMappings(eControllerAction nop, int32 button, eControllerType type) {
    if (!GetIsKeyBlank(button, type)) {
        return;
    }

    CheckAndClear(CA_VEHICLE_FIRE_WEAPON_ALT, type, button);
    CheckAndClear(CA_VEHICLE_FIRE_WEAPON, type, button);
    CheckAndClear(CA_VEHICLE_STEER_LEFT, type, button);
    CheckAndClear(CA_VEHICLE_STEER_RIGHT, type, button);
    CheckAndClear(CA_VEHICLE_STEER_UP, type, button);
    CheckAndClear(CA_VEHICLE_STEER_DOWN, type, button);
    CheckAndClear(CA_VEHICLE_ACCELERATE, type, button);
    CheckAndClear(CA_VEHICLE_BRAKE, type, button);
    CheckAndClear(CA_VEHICLE_RADIO_STATION_UP, type, button);
    CheckAndClear(CA_VEHICLE_RADIO_STATION_DOWN, type, button);
    CheckAndClear(CA_VEHICLE_RADIO_TRACK_SKIP, type, button);
    CheckAndClear(CA_VEHICLE_HORN, type, button);
    CheckAndClear(CA_TOGGLE_SUBMISSIONS, type, button);
    CheckAndClear(CA_VEHICLE_HANDBRAKE, type, button);
    CheckAndClear(CA_VEHICLE_LOOK_LEFT, type, button);
    CheckAndClear(CA_VEHICLE_LOOK_RIGHT, type, button);
    CheckAndClear(CA_VEHICLE_LOOKBEHIND, type, button);
    CheckAndClear(CA_VEHICLE_MOUSE_LOOK, type, button);
    CheckAndClear(CA_VEHICLE_TURRET_LEFT, type, button);
    CheckAndClear(CA_VEHICLE_TURRET_RIGHT, type, button);
    CheckAndClear(CA_VEHICLE_TURRET_UP, type, button);
    CheckAndClear(CA_VEHICLE_TURRET_DOWN, type, button);
}

// 0x5318C0
void CControllerConfigManager::Clear1st3rdPersonMappings(eControllerAction action, int32 button, eControllerType type) {
    if (!GetIsKeyBlank(button, type)) {
        return;
    }

    if (action != CA_PED_ANSWER_PHONE)
    {
        CheckAndClear(CA_PED_FIRE_WEAPON_ALT, type, button);
    }

    CheckAndClear(CA_PED_FIRE_WEAPON, type, button);
    CheckAndClear(CA_PED_LOCK_TARGET, type, button);
    CheckAndClear(CA_GO_FORWARD, type, button);
    CheckAndClear(CA_GO_BACK, type, button);

    if (FrontEndMenuManager.m_bController) {
        CheckAndClear(CA_PED_1RST_PERSON_LOOK_LEFT, type, button);
        CheckAndClear(CA_PED_1RST_PERSON_LOOK_RIGHT, type, button);
        CheckAndClear(CA_PED_1RST_PERSON_LOOK_DOWN, type, button);
        CheckAndClear(CA_PED_1RST_PERSON_LOOK_UP, type, button);
    }
}

// 0x52F510
void CControllerConfigManager::StoreJoyButtonStates() {
    for (auto&& [idx, bs] : notsa::enumerate(m_ButtonStates)) {
        bs = m_NewJoyState.rgbButtons[idx] >> 7;
    }
}

// unused
// 0x531E90
const GxtChar* CControllerConfigManager::GetActionKeyName(eControllerAction action) {
    int32 v3 = 0;
    ResetSettingOrder(action);
    for (auto i = (int32)m_Actions[action].Keys[CONTROLLER_KEYBOARD1].Priority; !i; i += 2) {
        if (++v3 >= 2) {
            return 0;
        }
    }
    return GetKeyNameForKeyboard(action, (eControllerType)v3);
}

// 0x531E20
const GxtChar* CControllerConfigManager::GetControllerSettingText(eControllerAction action, ePriority priority) {
    for (auto type = 0u; type < CONTROLLER_NUM; ++type) {
        if (m_Actions[action].Keys[type].Priority == priority) {
            switch (type) {
            case CONTROLLER_KEYBOARD1:
            case CONTROLLER_KEYBOARD2:
                return GetKeyNameForKeyboard(action, (eControllerType)type);
            case CONTROLLER_MOUSE:
                return GetControllerSettingTextMouse(action);
            case CONTROLLER_PAD:
                return GetControllerSettingTextJoystick(action);
            default:
                NOTSA_UNREACHABLE();
            }
        }
    }
    return 0;
}

// 0x531C20
void CControllerConfigManager::ClearSniperZoomMappings(eControllerAction nop, int32 button, eControllerType type) {
    if (!GetIsKeyBlank(button, type)) {
        return;
    }

    CheckAndClear(CA_PED_SNIPER_ZOOM_IN, type, button);
    CheckAndClear(CA_PED_SNIPER_ZOOM_OUT, type, button);
}

// 0x531BC0
void CControllerConfigManager::UnmapVehicleEnterExit(int32 button, eControllerType type) {
    if (!GetIsKeyBlank(button, type)) {
        return;
    }

    CheckAndClear(CA_VEHICLE_ENTER_EXIT, type, button);
}

// 0x52FD70
eControllerType CControllerConfigManager::ClearSettingsAssociatedWithAction(eControllerAction action, eControllerType type) {
    switch (type) {
    case CONTROLLER_KEYBOARD1:
        m_Actions[action].Keys[CONTROLLER_KEYBOARD1].KeyCode  = rsNULL;
        m_Actions[action].Keys[CONTROLLER_KEYBOARD1].Priority = PRIORITY_DISABLE;
        break;
    case CONTROLLER_KEYBOARD2:
        m_Actions[action].Keys[CONTROLLER_KEYBOARD2].KeyCode  = rsNULL;
        m_Actions[action].Keys[CONTROLLER_KEYBOARD2].Priority = PRIORITY_DISABLE;
        break;
    case CONTROLLER_MOUSE:
        m_Actions[action].Keys[CONTROLLER_MOUSE].KeyCode  = 0;
        m_Actions[action].Keys[CONTROLLER_MOUSE].Priority = PRIORITY_DISABLE;
        break;
    case CONTROLLER_PAD:
        m_Actions[action].Keys[CONTROLLER_PAD].KeyCode  = 0;
        m_Actions[action].Keys[CONTROLLER_PAD].Priority = PRIORITY_DISABLE;
        break;
    default:
        NOTSA_UNREACHABLE();
    }
    return ResetSettingOrder(action);
}

// unused
// 0x52FD20
eControllerType CControllerConfigManager::SetVehicleEnterExitButton(int32 button, eControllerType type, CControllerState* state) {
    CheckAndSetButton(CA_VEHICLE_ENTER_EXIT, type, button, state->ButtonTriangle);
    return type;
}

// 0x52FCA0
CControllerState* CControllerConfigManager::UpdatePadStateOnMenuActions(int32 button, eControllerType type, CControllerState* state) {
    CheckAndSetButton(CA_CAMERA_CHANGE_VIEW_ALL_SITUATIONS, type, button, state->Select);
    CheckAndSetPad(CA_CONVERSATION_NO, type, button, state->DPadLeft, state->DPadRight);
    CheckAndSetPad(CA_CONVERSATION_YES, type, button, state->DPadRight, state->DPadLeft);
    CheckAndSetButton(CA_NETWORK_TALK, type, button, state->m_bChatIndicated);
    return state;
}

// 0x52F580
int32 CControllerConfigManager::GetMouseButtonAssociatedWithAction(eControllerAction action) {
    return m_Actions[action].Keys[CONTROLLER_MOUSE].KeyCode;
}

// 0x52FAB0
eControllerType CControllerConfigManager::UpdateControllerStateOnAction(int32 button, eControllerType type, CControllerState* state) {
    const auto CheckAndSetStick = [&](eControllerAction action, int16& stickValue, bool& stickFlag, int16 value) {
        if (m_Actions[action].Keys[type].KeyCode == button) {
            if (stickFlag || (stickValue != 0 && stickValue != value)) {
                stickValue = 0;
                stickFlag  = true;
            } else {
                stickValue = value;
            }
        }
    };

    CheckAndSetButton(CA_PED_FIRE_WEAPON, type, button, state->ButtonCircle);   
    CheckAndSetButton(CA_PED_FIRE_WEAPON_ALT, type, button, state->LeftShoulder1);  
    CheckAndSetButton(CA_PED_LOCK_TARGET, type, button, state->RightShoulder1);

    CheckAndSetStick(CA_GO_FORWARD, state->LeftStickY, m_bStickL_Up_Dwn_MovementBothDown[type], -128);   
    CheckAndSetStick(CA_GO_BACK, state->LeftStickY, m_bStickL_Up_Dwn_MovementBothDown[type], 128);    
    CheckAndSetStick(CA_GO_LEFT, state->LeftStickX, m_bStickL_X_Rgh_Lft_MovementBothDown[type], -128);
    CheckAndSetStick(CA_GO_RIGHT, state->LeftStickX, m_bStickL_X_Rgh_Lft_MovementBothDown[type], 128); 

    CheckAndSetButton(CA_PED_WALK, type, button, state->m_bPedWalk);

    CheckAndSetPad(CA_GROUP_CONTROL_FWD, type, button, state->DPadUp, state->DPadDown);
    CheckAndSetPad(CA_GROUP_CONTROL_BWD, type, button, state->DPadDown, state->DPadUp);    

    CheckAndSetStick(CA_PED_1RST_PERSON_LOOK_LEFT, state->RightStickX, m_bStickR_X_Rgh_Lft_MovementBothDown[type], -128); 
    CheckAndSetStick(CA_PED_1RST_PERSON_LOOK_RIGHT, state->RightStickX, m_bStickR_X_Rgh_Lft_MovementBothDown[type], 128);  

    if (FrontEndMenuManager.m_bController) {
        CheckAndSetStick(CA_PED_1RST_PERSON_LOOK_UP, state->RightStickY, m_bStickR_Up_Dwn_MovementBothDown[type], 128); 
        CheckAndSetStick(CA_PED_1RST_PERSON_LOOK_DOWN, state->RightStickY, m_bStickR_Up_Dwn_MovementBothDown[type], -128);
    }

    return type;
}

// 0x52FA20
int32 CControllerConfigManager::UpdatePadStateOnActions(int32 button, eControllerType type, CControllerState* state) {
    CheckAndSetButton(CA_PED_LOOKBEHIND, type, button, state->ShockButtonR);
    CheckAndSetButton(CA_PED_JUMPING, type, button, state->ButtonSquare);
    CheckAndSetButton(CA_PED_ANSWER_PHONE, type, button, state->LeftShoulder1);
    CheckAndSetButton(CA_PED_CYCLE_WEAPON_LEFT, type, button, state->LeftShoulder2);
    CheckAndSetButton(CA_PED_CYCLE_WEAPON_RIGHT, type, button, state->RightShoulder2);
    CheckAndSetButton(CA_PED_SPRINT, type, button, state->ButtonCross);
    CheckAndSetButton(CA_PED_DUCK, type, button, state->ShockButtonL);

    if (FrontEndMenuManager.m_bController) {
        CheckAndSetButton(CA_PED_CENTER_CAMERA_BEHIND_PLAYER, type, button, state->LeftShoulder1);
    }
    return button;
}

// 0x52F550
bool CControllerConfigManager::GetIsActionAButtonCombo(eControllerAction action) {
    // NOP
    return false;
}

// 0x52F4F0
int32 CControllerConfigManager::GetControllerKeyAssociatedWithAction(eControllerAction action, eControllerType type) {
    return m_Actions[action].Keys[type].KeyCode;
}

// 0x52F9E0
int32 CControllerConfigManager::Handle1stPersonCameraActions(int32 button, eControllerType type, CControllerState* state) {
    CheckAndSetButton(CA_PED_SNIPER_ZOOM_IN, type, button, state->ButtonSquare);
    CheckAndSetButton(CA_PED_SNIPER_ZOOM_OUT, type, button, state->ButtonCross);
    CheckAndSetButton(CA_PED_DUCK, type, button, state->ShockButtonL);
    return button;
}

// 0x52FD40
int32 CControllerConfigManager::HandleButtonRelease(int32 button, eControllerType type, CControllerState* state) {
    const auto CheckAndReset = [&](eControllerAction action, int16& stateFlag) {
        if (m_Actions[action].Keys[type].KeyCode == button) {
            stateFlag = 0;
        }
    };
    CheckAndReset(CA_NETWORK_TALK, state->m_bChatIndicated);
    CheckAndReset(CA_VEHICLE_MOUSE_LOOK, state->m_bVehicleMouseLook);
    return button;
}

// 0x52F7B0
eControllerType CControllerConfigManager::HandleButtonPress(int32 button, eControllerType type, CControllerState* state) {
    const auto CheckAndSetStick = [&](eControllerAction action, int16& stickValue, bool& stickFlag, int16 value) {
        if (m_Actions[action].Keys[type].KeyCode == button) {
            if (stickFlag) {
                stickValue = 0;
                stickFlag  = true;
            } else {
                stickValue = value;
            }
        }
    };

    CheckAndSetButton(CA_VEHICLE_FIRE_WEAPON, type, button, state->ButtonCircle);
    CheckAndSetButton(CA_VEHICLE_FIRE_WEAPON_ALT, type, button, state->LeftShoulder1);
    // why double ? check
    CheckAndSetButton(CA_VEHICLE_LOOKBEHIND, type, button, state->LeftShoulder2); 
    CheckAndSetButton(CA_VEHICLE_LOOKBEHIND, type, button, state->RightShoulder2);

    CheckAndSetButton(CA_VEHICLE_LOOK_LEFT, type, button, state->LeftShoulder2);
    CheckAndSetButton(CA_VEHICLE_LOOK_RIGHT, type, button, state->RightShoulder2);
    CheckAndSetButton(CA_VEHICLE_MOUSE_LOOK, type, button, state->m_bVehicleMouseLook);
    CheckAndSetButton(CA_VEHICLE_HORN, type, button, state->ShockButtonL);
    CheckAndSetButton(CA_VEHICLE_HANDBRAKE, type, button, state->RightShoulder1);
    CheckAndSetButton(CA_VEHICLE_ACCELERATE, type, button, state->ButtonCross);
    CheckAndSetButton(CA_VEHICLE_BRAKE, type, button, state->ButtonSquare);
    CheckAndSetStick(CA_VEHICLE_STEER_UP, state->LeftStickY, m_bStickL_Up_Dwn_MovementBothDown[type], -128);
    CheckAndSetStick(CA_VEHICLE_STEER_DOWN, state->LeftStickY, m_bStickL_Up_Dwn_MovementBothDown[type], 128);
    CheckAndSetStick(CA_VEHICLE_STEER_LEFT, state->LeftStickX, m_bStickL_X_Rgh_Lft_MovementBothDown[type], -128);
    CheckAndSetStick(CA_VEHICLE_STEER_RIGHT, state->LeftStickX, m_bStickL_X_Rgh_Lft_MovementBothDown[type], 128);
    CheckAndSetButton(CA_VEHICLE_RADIO_STATION_UP, type, button, state->DPadUp);
    CheckAndSetButton(CA_VEHICLE_RADIO_STATION_DOWN, type, button, state->DPadDown);
    CheckAndSetButton(CA_VEHICLE_RADIO_TRACK_SKIP, type, button, state->m_bRadioTrackSkip);
    CheckAndSetButton(CA_TOGGLE_SUBMISSIONS, type, button, state->ShockButtonR);
    CheckAndSetStick(CA_VEHICLE_TURRET_LEFT, state->RightStickX, m_bStickR_X_Rgh_Lft_MovementBothDown[type], -128);
    CheckAndSetStick(CA_VEHICLE_TURRET_RIGHT, state->RightStickX, m_bStickR_X_Rgh_Lft_MovementBothDown[type], 128);
    CheckAndSetStick(CA_VEHICLE_TURRET_UP, state->RightStickY, m_bStickR_Up_Dwn_MovementBothDown[type], 128);
    CheckAndSetStick(CA_VEHICLE_TURRET_DOWN, state->RightStickY, m_bStickR_Up_Dwn_MovementBothDown[type], -128);
    return type;
}

// 0x52F5F0
eControllerType CControllerConfigManager::ResetSettingOrder(eControllerAction action) {
    eControllerType type = CONTROLLER_KEYBOARD1;
    for (auto i = 1u; i < PRIORITY_NUM + 1; ++i) {
        bool priorityFound = false;
        for (int j = 0u; j < CONTROLLER_NUM; ++j) {
            if (m_Actions[action].Keys[j].Priority == i) {
                priorityFound = true;
                break;
            }
        }

        if (!priorityFound) {
            signed int minPriority = PRIORITY_NONE;
            for (auto j = 0u; j < CONTROLLER_NUM; ++j) {
                ePriority currentPriority = m_Actions[action].Keys[j].Priority;
                if (currentPriority > (ePriority)i && currentPriority != PRIORITY_DISABLE) {
                    if (minPriority == PRIORITY_NONE || currentPriority < m_Actions[action].Keys[type].Priority) {
                        minPriority = currentPriority;
                        type = (eControllerType)j;
                    }
                }
            }
            if (minPriority != PRIORITY_NONE) {
                m_Actions[action].Keys[type].Priority = (ePriority)i;
            }
        }
    }
    return type;
}

// NOTSA [Code combined from 0x7448B0 and 0x744930]
void CControllerConfigManager::HandleJoyButtonUpDown(int32 joyNo, bool isDown) {
    StoreJoyButtonStates();
    const auto forceConfigMenuMode = !isDown && notsa::contains({ MODE_FLYBY, MODE_FIXED }, TheCamera.GetActiveCamera().m_nMode); // Probably leftover debug stuff?
    for (auto i = isDown ? 1u : 2u; i < std::size(m_ButtonStates); i++) { // TODO: Why is this starting from 1/2?
        const auto padBtn = (ePadButton)((m_ButtonStates[i - 1] == isDown) ? i : 0); // This doesn't make sense
        if (forceConfigMenuMode || FrontEndMenuManager.m_bMenuActive || joyNo != 0) {
            if (isDown) {
                UpdateJoyInConfigMenus_ButtonDown(padBtn, joyNo);
            } else {
                UpdateJoyInConfigMenus_ButtonUp(padBtn, joyNo);
            }
        } else {
            if (isDown) {
                UpdateJoy_ButtonDown(padBtn, CONTROLLER_PAD);
            } else {
                UpdateJoy_ButtonUp(padBtn, CONTROLLER_PAD);
            }
        }
    }
}

// 0x530530
bool CControllerConfigManager::LoadSettings(FILESTREAM file) {
    if (!file) {
        return notsa::IsFixBugs() ? false : true;
    }

    char header[29];
    CFileMgr::Read(file, header, sizeof(header) - 1);
    header[sizeof(header) - 1] = '\0';

    if (strncmp(header, "THIS FILE IS NOT VALID YET", 26) == 0) {
        return notsa::IsFixBugs() ? false : true;
    }

    CFileMgr::Seek(file, 0, SEEK_SET);
    eActionType action;
    CFileMgr::Read(file, &action, sizeof(action));

    if (action <= ACTION_FIRST_THIRD_PERSON) { // Maybe eActionType, maybe not
        return notsa::IsFixBugs() ? false : true;
    }

    for ([[maybe_unused]] auto i : std::views::iota(0u, 4u * std::size(m_Actions))) {
        int32 temp;
        CFileMgr::Read(file, &temp, sizeof(temp));
        if (notsa::IsFixBugs() && temp != i % std::size(m_Actions)) {
            return false;
        }
        CFileMgr::Seek(file, CONTROLLER_NUM * 2, SEEK_CUR);
    }

    CFileMgr::Seek(file, sizeof(int32), SEEK_SET);
    MakeControllerActionsBlank();

    for (auto& action : m_Actions) {
        CFileMgr::Seek(file, sizeof(action.Keys[0].KeyCode), SEEK_CUR);
        CFileMgr::Read(file, &action.Keys, CONTROLLER_NUM);
    }
    return true;
}

// 0x52D200
int32 CControllerConfigManager::SaveSettings(FILESTREAM file) {
    return plugin::CallMethodAndReturn<int32, 0x52D200, CControllerConfigManager*, FILE*>(this, file);
}

// 0x530640
void CControllerConfigManager::InitializeDefaultKeyboardAndMouseBindings() {
    SetKeyForAction(CA_GO_FORWARD, rsUP, CONTROLLER_KEYBOARD1);
    SetKeyForAction(CA_GO_BACK, rsDOWN, CONTROLLER_KEYBOARD1);
    SetKeyForAction(CA_GO_LEFT, rsLEFT, CONTROLLER_KEYBOARD1);
    SetKeyForAction(CA_GO_RIGHT, rsRIGHT, CONTROLLER_KEYBOARD1);

    if (FrontEndMenuManager.m_nTitleLanguage == 2) {
        SetKeyForAction(CA_CONVERSATION_YES, (RsKeyCodes)'Z', CONTROLLER_KEYBOARD1);
        SetKeyForAction(CA_PED_SNIPER_ZOOM_IN, (RsKeyCodes)'Y', CONTROLLER_KEYBOARD2);
    } else {
        SetKeyForAction(CA_CONVERSATION_YES, (RsKeyCodes)'Y', CONTROLLER_KEYBOARD1);
        SetKeyForAction(CA_PED_SNIPER_ZOOM_IN, (RsKeyCodes)'X', CONTROLLER_KEYBOARD2);
    }

    if (FrontEndMenuManager.m_nTitleLanguage == 1) {
        SetKeyForAction(CA_GO_FORWARD, (RsKeyCodes)'Z', CONTROLLER_KEYBOARD2);
        SetKeyForAction(CA_GO_LEFT, (RsKeyCodes)'Q', CONTROLLER_KEYBOARD2);
        SetKeyForAction(CA_PED_CYCLE_WEAPON_LEFT, (RsKeyCodes)'A', CONTROLLER_KEYBOARD1);
        SetKeyForAction(CA_PED_SNIPER_ZOOM_OUT, (RsKeyCodes)'W', CONTROLLER_KEYBOARD2);
    } else {
        SetKeyForAction(CA_GO_FORWARD, (RsKeyCodes)'W', CONTROLLER_KEYBOARD2);
        SetKeyForAction(CA_GO_LEFT, (RsKeyCodes)'A', CONTROLLER_KEYBOARD2);
        SetKeyForAction(CA_PED_CYCLE_WEAPON_LEFT, (RsKeyCodes)'Q', CONTROLLER_KEYBOARD1);
        SetKeyForAction(CA_PED_SNIPER_ZOOM_OUT, (RsKeyCodes)'Z', CONTROLLER_KEYBOARD2);
    }

    SetKeyForAction(CA_GO_BACK, (RsKeyCodes)'S', CONTROLLER_KEYBOARD2);
    SetKeyForAction(CA_GO_RIGHT, (RsKeyCodes)'D', CONTROLLER_KEYBOARD2);
    SetKeyForAction(CA_PED_CYCLE_WEAPON_RIGHT, (RsKeyCodes)'E', CONTROLLER_KEYBOARD1);
    SetKeyForAction(CA_PED_CYCLE_WEAPON_RIGHT, rsPADENTER, CONTROLLER_KEYBOARD2);
    SetKeyForAction(CA_PED_CYCLE_WEAPON_LEFT, rsPADDEL, CONTROLLER_KEYBOARD2);
    SetKeyForAction(CA_PED_FIRE_WEAPON, rsPADINS, CONTROLLER_KEYBOARD1);
    SetKeyForAction(CA_PED_FIRE_WEAPON, rsLCTRL, CONTROLLER_KEYBOARD2);
    SetKeyForAction(CA_PED_SNIPER_ZOOM_IN, rsPGUP, CONTROLLER_KEYBOARD1);
    SetKeyForAction(CA_PED_SNIPER_ZOOM_OUT, rsPGDN, CONTROLLER_KEYBOARD1);
    SetKeyForAction(CA_PED_FIRE_WEAPON_ALT, (RsKeyCodes)'\\', CONTROLLER_KEYBOARD1);
    SetKeyForAction(CA_GROUP_CONTROL_FWD, (RsKeyCodes)'G', CONTROLLER_KEYBOARD1);
    SetKeyForAction(CA_GROUP_CONTROL_BWD, (RsKeyCodes)'H', CONTROLLER_KEYBOARD1);
    SetKeyForAction(CA_CONVERSATION_NO, (RsKeyCodes)'N', CONTROLLER_KEYBOARD1);
    SetKeyForAction(CA_VEHICLE_ENTER_EXIT, (RsKeyCodes)'F', CONTROLLER_KEYBOARD2);
    SetKeyForAction(CA_VEHICLE_ENTER_EXIT, rsENTER, CONTROLLER_KEYBOARD1);
    SetKeyForAction(CA_CAMERA_CHANGE_VIEW_ALL_SITUATIONS, (RsKeyCodes)'V', CONTROLLER_KEYBOARD1);
    SetKeyForAction(CA_CAMERA_CHANGE_VIEW_ALL_SITUATIONS, rsHOME, CONTROLLER_KEYBOARD2);
    SetKeyForAction(CA_PED_JUMPING, rsLSHIFT, CONTROLLER_KEYBOARD1);
    SetKeyForAction(CA_PED_JUMPING, rsRCTRL, CONTROLLER_KEYBOARD2);
    SetKeyForAction(CA_PED_SPRINT, (RsKeyCodes)32, CONTROLLER_KEYBOARD1);
    SetKeyForAction(CA_PED_DUCK, (RsKeyCodes)'C', CONTROLLER_KEYBOARD1);
    SetKeyForAction(CA_PED_ANSWER_PHONE, rsTAB, CONTROLLER_KEYBOARD1);
    SetKeyForAction(CA_PED_WALK, rsLALT, CONTROLLER_KEYBOARD1);
    SetKeyForAction(CA_PED_LOCK_TARGET, rsDEL, CONTROLLER_KEYBOARD1);
    SetKeyForAction(CA_PED_LOCK_TARGET, rsCAPSLK, CONTROLLER_KEYBOARD2);
    SetKeyForAction(CA_PED_1RST_PERSON_LOOK_LEFT, rsPADLEFT, CONTROLLER_KEYBOARD1);
    SetKeyForAction(CA_PED_1RST_PERSON_LOOK_RIGHT, rsPADRIGHT, CONTROLLER_KEYBOARD1);
    SetKeyForAction(CA_PED_1RST_PERSON_LOOK_UP, rsPADDOWN, CONTROLLER_KEYBOARD1);
    SetKeyForAction(CA_PED_1RST_PERSON_LOOK_DOWN, rsPADUP, CONTROLLER_KEYBOARD1);
    SetKeyForAction(CA_PED_CENTER_CAMERA_BEHIND_PLAYER, (RsKeyCodes)'#', CONTROLLER_KEYBOARD1);
    SetKeyForAction(CA_PED_LOOKBEHIND, rsPADEND, CONTROLLER_KEYBOARD1);
    SetKeyForAction(CA_VEHICLE_STEER_UP, rsUP, CONTROLLER_KEYBOARD1);
    SetKeyForAction(CA_VEHICLE_STEER_DOWN, rsDOWN, CONTROLLER_KEYBOARD1);
    SetKeyForAction(CA_VEHICLE_RADIO_STATION_UP, rsINS, CONTROLLER_KEYBOARD1);

    if (FrontEndMenuManager.m_nTitleLanguage == 1) {
        SetKeyForAction(CA_VEHICLE_STEER_LEFT, (RsKeyCodes)'Q', CONTROLLER_KEYBOARD1);
        SetKeyForAction(CA_VEHICLE_LOOK_LEFT, (RsKeyCodes)'A', CONTROLLER_KEYBOARD1);
        SetKeyForAction(CA_VEHICLE_RADIO_STATION_UP, (RsKeyCodes)'`', CONTROLLER_KEYBOARD2);
        SetKeyForAction(CA_TOGGLE_SUBMISSIONS, (RsKeyCodes)233, CONTROLLER_KEYBOARD1);
        SetKeyForAction(CA_VEHICLE_ACCELERATE, (RsKeyCodes)'Z', CONTROLLER_KEYBOARD1);
    } else {
        SetKeyForAction(CA_VEHICLE_STEER_LEFT, (RsKeyCodes)'A', CONTROLLER_KEYBOARD1);
        SetKeyForAction(CA_VEHICLE_LOOK_LEFT, (RsKeyCodes)'Q', CONTROLLER_KEYBOARD1);
        SetKeyForAction(CA_VEHICLE_RADIO_STATION_UP, (RsKeyCodes)'4', CONTROLLER_KEYBOARD2);
        SetKeyForAction(CA_TOGGLE_SUBMISSIONS, (RsKeyCodes)'2', CONTROLLER_KEYBOARD1);
        SetKeyForAction(CA_VEHICLE_ACCELERATE, (RsKeyCodes)'W', CONTROLLER_KEYBOARD1);
    }

    SetKeyForAction(CA_TOGGLE_SUBMISSIONS, rsPLUS, CONTROLLER_KEYBOARD2);
    SetKeyForAction(CA_VEHICLE_FIRE_WEAPON, rsRCTRL, CONTROLLER_KEYBOARD1);
    SetKeyForAction(CA_VEHICLE_FIRE_WEAPON, rsLALT, CONTROLLER_KEYBOARD2);
    SetKeyForAction(CA_VEHICLE_FIRE_WEAPON_ALT, rsLCTRL, CONTROLLER_KEYBOARD1);
    SetKeyForAction(CA_VEHICLE_FIRE_WEAPON_ALT, rsPADINS, CONTROLLER_KEYBOARD2);
    SetKeyForAction(CA_VEHICLE_BRAKE, (RsKeyCodes)'S', CONTROLLER_KEYBOARD1);
    SetKeyForAction(CA_VEHICLE_STEER_RIGHT, (RsKeyCodes)'D', CONTROLLER_KEYBOARD1);
    SetKeyForAction(CA_VEHICLE_STEER_LEFT, rsLEFT, CONTROLLER_KEYBOARD2);
    SetKeyForAction(CA_VEHICLE_STEER_RIGHT, rsRIGHT, CONTROLLER_KEYBOARD2);
    SetKeyForAction(CA_VEHICLE_HORN, rsCAPSLK, CONTROLLER_KEYBOARD1);
    SetKeyForAction(CA_VEHICLE_HORN, (RsKeyCodes)'H', CONTROLLER_KEYBOARD2);
    SetKeyForAction(CA_VEHICLE_HANDBRAKE, (RsKeyCodes)' ', CONTROLLER_KEYBOARD1);
    SetKeyForAction(CA_VEHICLE_HANDBRAKE, rsRCTRL, CONTROLLER_KEYBOARD2);
    SetKeyForAction(CA_VEHICLE_LOOK_RIGHT, (RsKeyCodes)'E', CONTROLLER_KEYBOARD1);
    SetKeyForAction(CA_VEHICLE_TURRET_LEFT, rsPADLEFT, CONTROLLER_KEYBOARD1);
    SetKeyForAction(CA_VEHICLE_TURRET_RIGHT, rsPADRIGHT, CONTROLLER_KEYBOARD1);
    SetKeyForAction(CA_VEHICLE_TURRET_UP, rsPADDOWN, CONTROLLER_KEYBOARD1);
    SetKeyForAction(CA_VEHICLE_TURRET_DOWN, rsPADUP, CONTROLLER_KEYBOARD1);
    SetKeyForAction(CA_VEHICLE_TURRET_UP, rsEND, CONTROLLER_KEYBOARD2);
    SetKeyForAction(CA_VEHICLE_TURRET_DOWN, rsDEL, CONTROLLER_KEYBOARD2);
    SetKeyForAction(CA_VEHICLE_RADIO_TRACK_SKIP, rsF5, CONTROLLER_KEYBOARD1);
    SetKeyForAction(CA_VEHICLE_RADIO_STATION_DOWN, rsDEL, CONTROLLER_KEYBOARD1);
    SetKeyForAction(CA_VEHICLE_RADIO_STATION_DOWN, (RsKeyCodes)'R', CONTROLLER_KEYBOARD2);
    SetKeyForAction(CA_PED_CYCLE_TARGET_LEFT, (RsKeyCodes)'[', CONTROLLER_KEYBOARD1);
    SetKeyForAction(CA_PED_CYCLE_TARGET_RIGHT, (RsKeyCodes)']', CONTROLLER_KEYBOARD1);
    ClearSimButtonPressCheckers();
}

// 0x530B00
void CControllerConfigManager::InitDefaultControlConfiguration(uint32 buttonCount) {
    m_bJoyJustInitialised = true;

    const int buttonCountLimited = std::min(buttonCount, 16u);

    std::array<std::pair<eControllerAction, RsKeyCodes>, 3> actions;

    auto set_actions = [&](std::initializer_list<std::pair<eControllerAction, RsKeyCodes>> init_list) {
        std::copy(init_list.begin(), init_list.end(), actions.begin());
        for (const auto& action : actions) {
            SetKeyForAction(action.first, action.second, CONTROLLER_PAD);
        }
    };

    if (IsCheckSpecificGamepad()) {
        switch (buttonCountLimited) {
            case 16: set_actions({{CA_CONVERSATION_NO,(RsKeyCodes)16}}); [[fallthrough]];
            case 15: set_actions({{CA_GROUP_CONTROL_BWD,(RsKeyCodes)15},{CA_VEHICLE_RADIO_STATION_DOWN,(RsKeyCodes)15}}); [[fallthrough]];
            case 14: set_actions({{CA_CONVERSATION_YES,(RsKeyCodes)14}}); [[fallthrough]];
            case 13: set_actions({{CA_GROUP_CONTROL_FWD,(RsKeyCodes)13},{CA_VEHICLE_RADIO_STATION_UP,(RsKeyCodes)13}}); [[fallthrough]];
            case 12: 
            case 11: set_actions({{CA_PED_LOOKBEHIND,(RsKeyCodes)11},{CA_TOGGLE_SUBMISSIONS,(RsKeyCodes)11}}); [[fallthrough]];
            case 10: set_actions({{CA_VEHICLE_HORN,(RsKeyCodes)10},{CA_PED_DUCK,(RsKeyCodes)10}}); [[fallthrough]];
            case 9:  set_actions({{CA_CAMERA_CHANGE_VIEW_ALL_SITUATIONS,(RsKeyCodes)9}}); [[fallthrough]];
            case 8:  set_actions({{CA_VEHICLE_HANDBRAKE,(RsKeyCodes)8},{CA_PED_LOCK_TARGET,(RsKeyCodes)8}}); [[fallthrough]];
            case 7:  set_actions({{CA_PED_ANSWER_PHONE,(RsKeyCodes)7},{CA_PED_FIRE_WEAPON_ALT,(RsKeyCodes)7},{CA_VEHICLE_FIRE_WEAPON_ALT,(RsKeyCodes)7}}); [[fallthrough]];
            case 6:  set_actions({{CA_PED_CYCLE_WEAPON_RIGHT,(RsKeyCodes)6},{CA_VEHICLE_LOOK_RIGHT,(RsKeyCodes)6}}); [[fallthrough]];
            case 5:  set_actions({{CA_PED_CYCLE_WEAPON_LEFT,(RsKeyCodes)5},{CA_VEHICLE_LOOK_LEFT,(RsKeyCodes)5}}); [[fallthrough]];
            case 4:  set_actions({{CA_VEHICLE_BRAKE,(RsKeyCodes)4},{CA_PED_JUMPING,(RsKeyCodes)4},{CA_PED_SNIPER_ZOOM_IN,(RsKeyCodes)4}}); [[fallthrough]];
            case 3:  set_actions({{CA_VEHICLE_ACCELERATE,(RsKeyCodes)3},{CA_PED_SPRINT,(RsKeyCodes)3},{CA_PED_SNIPER_ZOOM_OUT,(RsKeyCodes)3}}); [[fallthrough]];
            case 2:  set_actions({{CA_PED_FIRE_WEAPON,(RsKeyCodes)2},{CA_VEHICLE_FIRE_WEAPON,(RsKeyCodes)2}}); [[fallthrough]];
            case 1:  set_actions({{CA_VEHICLE_ENTER_EXIT,(RsKeyCodes)1}}); break;
            default: return;
        }
        } else {
            switch (buttonCountLimited) {
            case 16: set_actions({{CA_CONVERSATION_NO,(RsKeyCodes)16}}); [[fallthrough]];
            case 15: set_actions({{CA_GROUP_CONTROL_BWD,(RsKeyCodes)15},{CA_VEHICLE_RADIO_STATION_DOWN,(RsKeyCodes)15}}); [[fallthrough]];
            case 14: set_actions({{CA_CONVERSATION_YES,(RsKeyCodes)14}}); [[fallthrough]];
            case 13: set_actions({{CA_GROUP_CONTROL_FWD,(RsKeyCodes)13},{CA_VEHICLE_RADIO_STATION_UP,(RsKeyCodes)13}}); [[fallthrough]];
            case 12: 
            case 11: set_actions({{CA_PED_LOOKBEHIND,(RsKeyCodes)11},{CA_TOGGLE_SUBMISSIONS,(RsKeyCodes)11}}); [[fallthrough]];
            case 10: set_actions({{CA_VEHICLE_HORN,(RsKeyCodes)10},{CA_PED_DUCK,(RsKeyCodes)10}}); [[fallthrough]];
            case 9:  set_actions({{CA_CAMERA_CHANGE_VIEW_ALL_SITUATIONS,(RsKeyCodes)9}}); [[fallthrough]];
            case 8:  set_actions({{CA_VEHICLE_HANDBRAKE,(RsKeyCodes)8},{CA_PED_LOCK_TARGET,(RsKeyCodes)8}}); [[fallthrough]];
            case 7:  set_actions({{CA_PED_ANSWER_PHONE,(RsKeyCodes)7},{CA_PED_FIRE_WEAPON_ALT,(RsKeyCodes)7},{CA_VEHICLE_FIRE_WEAPON_ALT,(RsKeyCodes)7}}); [[fallthrough]];
            case 6:  set_actions({{CA_PED_CYCLE_WEAPON_RIGHT,(RsKeyCodes)6},{CA_VEHICLE_LOOK_RIGHT,(RsKeyCodes)6}}); [[fallthrough]];
            case 5:  set_actions({{CA_PED_CYCLE_WEAPON_LEFT,(RsKeyCodes)5},{CA_VEHICLE_LOOK_LEFT,(RsKeyCodes)5}}); [[fallthrough]];
            case 4:  set_actions({{CA_VEHICLE_ENTER_EXIT,(RsKeyCodes)4}}); [[fallthrough]];
            case 3:  set_actions({{CA_VEHICLE_BRAKE,(RsKeyCodes)3},{CA_PED_JUMPING,(RsKeyCodes)3},{CA_PED_SNIPER_ZOOM_IN,(RsKeyCodes)3}}); [[fallthrough]];
            case 2:  set_actions({{CA_VEHICLE_ACCELERATE,(RsKeyCodes)2},{CA_PED_SPRINT,(RsKeyCodes)2},{CA_PED_SNIPER_ZOOM_OUT,(RsKeyCodes)2}}); [[fallthrough]];
            case 1:  set_actions({{CA_PED_FIRE_WEAPON,(RsKeyCodes)1},{CA_VEHICLE_FIRE_WEAPON,(RsKeyCodes)1}}); break;
            default: return;
        }
    }
}

// 0x52F6F0
bool CControllerConfigManager::InitDefaultControlConfigMouse(const CMouseControllerState& state, bool controller) {
    bool result = false;
    if (state.lmb) {
        MouseFoundInitSet = true;
        SetMouseButtonAssociatedWithAction(CA_PED_FIRE_WEAPON,            rsMOUSE_LEFT_BUTTON);
        SetMouseButtonAssociatedWithAction(CA_VEHICLE_FIRE_WEAPON,        rsMOUSE_LEFT_BUTTON);
    }

    if (state.rmb) {                                                      
        SetMouseButtonAssociatedWithAction(CA_PED_LOCK_TARGET,            rsMOUSE_RIGHT_BUTTON);
        SetMouseButtonAssociatedWithAction(CA_VEHICLE_MOUSE_LOOK,         rsMOUSE_RIGHT_BUTTON);
    }

    if (state.mmb) {                                                      
        SetMouseButtonAssociatedWithAction(CA_VEHICLE_LOOKBEHIND,         rsMOUSE_MIDDLE_BUTTON);
        SetMouseButtonAssociatedWithAction(CA_PED_LOOKBEHIND,             rsMOUSE_MIDDLE_BUTTON);
    }

    if (state.wheelUp || (result = state.wheelDown) != 0) {
        SetMouseButtonAssociatedWithAction(CA_PED_CYCLE_WEAPON_LEFT,      rsMOUSE_WHEEL_UP_BUTTON);
        SetMouseButtonAssociatedWithAction(CA_PED_CYCLE_WEAPON_RIGHT,     rsMOUSE_WHEEL_DOWN_BUTTON);
        SetMouseButtonAssociatedWithAction(CA_VEHICLE_RADIO_STATION_UP,   rsMOUSE_WHEEL_UP_BUTTON);
        SetMouseButtonAssociatedWithAction(CA_VEHICLE_RADIO_STATION_DOWN, rsMOUSE_WHEEL_DOWN_BUTTON);
        SetMouseButtonAssociatedWithAction(CA_PED_SNIPER_ZOOM_IN,         rsMOUSE_WHEEL_UP_BUTTON);
        SetMouseButtonAssociatedWithAction(CA_PED_SNIPER_ZOOM_OUT,        rsMOUSE_WHEEL_DOWN_BUTTON);
    }
    return result;
}


// 0x52D260
void CControllerConfigManager::InitialiseControllerActionNameArray() {
    // Use a std::array to store the action names and their corresponding enum values.
    constexpr std::array<std::pair<const char*, eControllerAction>, 56> actionData = {
        { { "PED_LOOKBEHIND", CA_PED_LOOKBEHIND },
         { "PED_CYCLE_WEAPON_LEFT", CA_PED_CYCLE_WEAPON_LEFT },
         { "PED_CYCLE_WEAPON_RIGHT", CA_PED_CYCLE_WEAPON_RIGHT },
         { "PED_LOCK_TARGET", CA_PED_LOCK_TARGET },
         { "PED_JUMPING", CA_PED_JUMPING },
         { "PED_SPRINT", CA_PED_SPRINT },
         { "SNEAK_ABOUT", CA_PED_WALK },
         { "PED_CYCLE_TARGET_LEFT", CA_PED_CYCLE_TARGET_LEFT },
         { "PED_CYCLE_TARGET_RIGHT", CA_PED_CYCLE_TARGET_RIGHT },
         { "PED_CENTER_CAMERA_BEHIND_PLAYER", CA_PED_CENTER_CAMERA_BEHIND_PLAYER },
         { "VEHICLE_LOOKBEHIND", CA_VEHICLE_LOOKBEHIND },
         { "PED_DUCK", CA_PED_DUCK },
         { "PED_ANSWER_PHONE", CA_PED_ANSWER_PHONE },
         { "VEHICLE_STEERLEFT", CA_VEHICLE_STEER_LEFT },
         { "VEHICLE_STEERRIGHT", CA_VEHICLE_STEER_RIGHT },
         { "VEHICLE_STEERUP", CA_VEHICLE_STEER_UP },
         { "VEHICLE_STEERDOWN", CA_VEHICLE_STEER_DOWN },
         { "VEHICLE_LOOKLEFT", CA_VEHICLE_LOOK_LEFT },
         { "VEHICLE_LOOKRIGHT", CA_VEHICLE_LOOK_RIGHT },
         { "VEHICLE_MOUSELOOK", CA_VEHICLE_MOUSE_LOOK },
         { "VEHICLE_HORN", CA_VEHICLE_HORN },
         { "VEHICLE_HANDBRAKE", CA_VEHICLE_HANDBRAKE },
         { "VEHICLE_ACCELERATE", CA_VEHICLE_ACCELERATE },
         { "VEHICLE_BRAKE", CA_VEHICLE_BRAKE },
         { "VEHICLE_RADIO_STATION_UP", CA_VEHICLE_RADIO_STATION_UP },
         { "VEHICLE_RADIO_STATION_DOWN", CA_VEHICLE_RADIO_STATION_DOWN },
         { "TOGGLE_SUBMISSIONS", CA_TOGGLE_SUBMISSIONS },
         { "PED_SNIPER_ZOOM_IN", CA_PED_SNIPER_ZOOM_IN },
         { "PED_SNIPER_ZOOM_OUT", CA_PED_SNIPER_ZOOM_OUT },
         { "PED_1RST_PERSON_LOOK_LEFT", CA_PED_1RST_PERSON_LOOK_LEFT },
         { "PED_1RST_PERSON_LOOK_RIGHT", CA_PED_1RST_PERSON_LOOK_RIGHT },
         { "PED_1RST_PERSON_LOOK_UP", CA_PED_1RST_PERSON_LOOK_UP },
         { "PED_1RST_PERSON_LOOK_DOWN", CA_PED_1RST_PERSON_LOOK_DOWN },
         { "SHOW_MOUSE_POINTER_TOGGLE", CA_SHOW_MOUSE_POINTER_TOGGLE },
         { "CAMERA_CHANGE_VIEW_ALL_SITUATIONS", CA_CAMERA_CHANGE_VIEW_ALL_SITUATIONS },
         { "PED_FIREWEAPON", CA_PED_FIRE_WEAPON },
         { "PED_FIREWEAPON_ALT", CA_PED_FIRE_WEAPON_ALT },
         { "VEHICLE_FIREWEAPON", CA_VEHICLE_FIRE_WEAPON },
         { "VEHICLE_FIREWEAPON_ALT", CA_VEHICLE_FIRE_WEAPON_ALT },
         { "VEHICLE_ENTER_EXIT", CA_VEHICLE_ENTER_EXIT },
         { "CONVERSATION_NO", CA_CONVERSATION_NO },
         { "CONVERSATION_YES", CA_CONVERSATION_YES },
         { "GROUP_CONTROL_FWD", CA_GROUP_CONTROL_FWD },
         { "GROUP_CONTROL_BWD", CA_GROUP_CONTROL_BWD },
         { "GO_LEFT", CA_GO_LEFT },
         { "GO_RIGHT", CA_GO_RIGHT },
         { "GO_FORWARD", CA_GO_FORWARD },
         { "GO_BACK", CA_GO_BACK },
         { "VEHICLE_TURRETLEFT", CA_VEHICLE_TURRET_LEFT },
         { "VEHICLE_TURRETRIGHT", CA_VEHICLE_TURRET_RIGHT },
         { "VEHICLE_TURRETUP", CA_VEHICLE_TURRET_UP },
         { "VEHICLE_TURRETDOWN", CA_VEHICLE_TURRET_DOWN },
         { "NETWORK_TALK", CA_NETWORK_TALK },
         { "TOGGLE_DPAD", CA_TOGGLE_DPAD },
         { "SWITCH_DEBUG_CAM_ON", CA_SWITCH_DEBUG_CAM_ON },
         { "TAKE_SCREEN_SHOT", CA_TAKE_SCREEN_SHOT } }
    };
    // wat
    GxtChar src[40];

    for (const auto& action : actionData) {
        AsciiToGxtChar(action.first, src);
        CMessages::StringCopy(src, GxtCharFromAscii(m_arrControllerActionName[action.second]), sizeof(src));
    }
}

// 0x531F20
bool CControllerConfigManager::ReinitControls() {
    return 0;
}

// 0x52F590
int8 CControllerConfigManager::SetMouseButtonAssociatedWithAction(eControllerAction action, int32 button) {
    int8 existingMappings = 0;
    for (auto i = 0u; i < CONTROLLER_NUM; ++i) {
        if (m_Actions[action].Keys[i].KeyCode != rsNULL && m_Actions[action].Keys[i].KeyCode != 0) {
            existingMappings++;
        }
    }
    m_Actions[action].Keys[CONTROLLER_MOUSE].KeyCode  = button;
    m_Actions[action].Keys[CONTROLLER_MOUSE].Priority = (ePriority)existingMappings++;
    return existingMappings;
}

// unused
// 0x52DA30
void CControllerConfigManager::StoreMouseButtonState(eMouseButtons button, bool state) {
    switch (button) {
        case MOUSE_BUTTON_LEFT:
            CPad::PCTempMouseControllerState.lmb = state;
            break;
        case MOUSE_BUTTON_MIDDLE:
            CPad::PCTempMouseControllerState.mmb = state;
            break;
        case MOUSE_BUTTON_RIGHT:
            CPad::PCTempMouseControllerState.rmb = state;
            break;
        case MOUSE_BUTTON_WHEEL_UP:
            CPad::PCTempMouseControllerState.wheelUp = state;
            break;
        case MOUSE_BUTTON_WHEEL_DOWN:
            CPad::PCTempMouseControllerState.wheelDown = state;
            break;
        case MOUSE_BUTTON_WHEEL_XBUTTON1:
            CPad::PCTempMouseControllerState.bmx1 = state;
            break;
        case MOUSE_BUTTON_WHEEL_XBUTTON2:
            CPad::PCTempMouseControllerState.bmx2 = state;
            break;
    }
}

// 0x52DAB0
void CControllerConfigManager::UpdateJoyInConfigMenus_ButtonDown(int32 button, int32 padNumber) {
    CPad* pad = CPad::GetPad(padNumber);
    if (!pad || button == 0) {
        return;
    }

    switch (button) { // TODO: enum ePadButton ?
    case 5: pad->PCTempJoyState.LeftShoulder2 = 255; break;
    case 6: pad->PCTempJoyState.RightShoulder2 = 255; break;
    case 7: pad->PCTempJoyState.LeftShoulder1 = 255; break;
    case 8: pad->PCTempJoyState.RightShoulder1 = 255; break;
    case 9: pad->PCTempJoyState.Select = 255; break;
    case 10: pad->PCTempJoyState.ShockButtonL = 255; break;
    case 11: pad->PCTempJoyState.ShockButtonR = 255; break;
    case 12:
        if (padNumber == 1) {
            pad->PCTempJoyState.Start = 255;
        }
        break;
    case 13: pad->PCTempJoyState.DPadUp = 255; break;
    case 14: pad->PCTempJoyState.DPadRight = 255; break;
    case 15: pad->PCTempJoyState.DPadDown = 255; break;
    case 16: pad->PCTempJoyState.DPadLeft = 255; break;
    default: break;
    }

    switch (button) {
    case 1: pad->PCTempJoyState.ButtonTriangle = 255; break;
    case 2: pad->PCTempJoyState.ButtonCross = 255; break;   
    case 3: pad->PCTempJoyState.ButtonSquare = 255; break;  
    case 4: if (!IsCheckSpecificGamepad()) pad->PCTempJoyState.ButtonTriangle = 255; break;
    default: break;
    }
}

// 0x530ED0
void CControllerConfigManager::UpdateJoy_ButtonDown(int32 button, eControllerType type) {
    if (!GetIsKeyBlank(button, type)) {
        return;
    }

    CPad* pad = CPad::GetPad(0);
    if (!pad) {
        return;
    }

    bool inVehicleAndDriving = false;
    if (const auto playerPed = FindPlayerPed()) {
        if (const auto playerVehicle = FindPlayerVehicle()) {
            inVehicleAndDriving = playerPed->m_nPedState == PEDSTATE_DRIVING && !pad->DisablePlayerControls;
        }
    }

    eCamMode camMode = TheCamera.m_aCams[TheCamera.m_nActiveCam].m_nMode;
    const auto isFirstPersonMode = camMode == MODE_1STPERSON || camMode == MODE_SNIPER || camMode == MODE_ROCKETLAUNCHER || camMode == MODE_ROCKETLAUNCHER_HS || camMode == MODE_M16_1STPERSON || camMode == MODE_CAMERA;

    CControllerState* state;
    switch (type) {
    case CONTROLLER_KEYBOARD1:
    case CONTROLLER_KEYBOARD2:
        state = &pad->PCTempKeyState;
        break;
    case CONTROLLER_MOUSE:
        state = &pad->PCTempMouseState;
        break;
    case CONTROLLER_PAD:
        state = &pad->PCTempJoyState;
        break;
    default:
        NOTSA_UNREACHABLE();
    }

    if (inVehicleAndDriving) {
        HandleButtonPress(button, type, state);
    } else {
        UpdateControllerStateOnAction(button, type, state);
        if (isFirstPersonMode) {
            Handle1stPersonCameraActions(button, type, state);
        }
        UpdatePadStateOnMenuActions(button, type, state);
    }
    SetVehicleEnterExitButton(button, type, state);
}

// unused
// 0x52DC10
void CControllerConfigManager::AffectControllerStateOn_ButtonDown_DebugStuff(int32, eControllerType) {
    // NOP
    return;
}

// 0x52DC20
void CControllerConfigManager::UpdateJoyInConfigMenus_ButtonUp(int32 button, int32 padNumber) {
    CPad* pad = CPad::GetPad(padNumber);
    if (!pad || button == 0) {
        return;
    }

    switch (button) {
        case 5:  pad->PCTempJoyState.LeftShoulder2 = 0; break;
        case 6:  pad->PCTempJoyState.RightShoulder2 = 0; break;
        case 7:  pad->PCTempJoyState.LeftShoulder1 = 0; break;
        case 8:  pad->PCTempJoyState.RightShoulder1 = 0; break;
        case 9:  pad->PCTempJoyState.Select = 0; break;
        case 10: pad->PCTempJoyState.ShockButtonL = 0; break;
        case 11: pad->PCTempJoyState.ShockButtonR = 0; break;
        case 12: if (padNumber == 1) pad->PCTempJoyState.Start = 0; break;
        case 13: pad->PCTempJoyState.DPadUp = 0; break;
        case 14: pad->PCTempJoyState.DPadRight = 0; break;
        case 15: pad->PCTempJoyState.DPadDown = 0; break;
        case 16: pad->PCTempJoyState.DPadLeft = 0; break;
        default: break;
    }
    if (IsCheckSpecificGamepad()) {
        switch (button) {
        case 1: pad->PCTempJoyState.ButtonTriangle = 0; break;
        case 2: pad->PCTempJoyState.ButtonCircle = 0; break;
        case 3: pad->PCTempJoyState.ButtonCross = 0; break;
        case 4: pad->PCTempJoyState.ButtonSquare = 0; break;
        }
    }
}

// 0x531070
void CControllerConfigManager::UpdateJoy_ButtonUp(int32 button, eControllerType type) {
    bool shouldProcessEvent = false;
    CControllerState p_PCTempKeyState;
    const auto pad = CPad::GetPad();
    if (pad) {
        switch (type) {
        case CONTROLLER_KEYBOARD1:
        case CONTROLLER_KEYBOARD2:
            p_PCTempKeyState = pad->PCTempKeyState;
            shouldProcessEvent = (button != rsNULL);
            break;
        case CONTROLLER_MOUSE:
            p_PCTempKeyState = pad->PCTempMouseState;
            shouldProcessEvent = (button != 0);
            break;
        case CONTROLLER_PAD:
            p_PCTempKeyState = pad->PCTempJoyState;
            shouldProcessEvent = (button != 0);
            break;
        default:
            p_PCTempKeyState = (CControllerState)type;
            break;
        }
    }

    if (shouldProcessEvent && pad && !FrontEndMenuManager.m_bMenuActive) {
        HandleButtonRelease(button, type, &p_PCTempKeyState);
    }
}

// unused
// 0x52DD80
void CControllerConfigManager::AffectControllerStateOn_ButtonUp_DebugStuff(int32, eControllerType) {
    // NOP
    return;
}

// 0x52DD90
void CControllerConfigManager::ClearSimButtonPressCheckers() {
    for (auto i = 0u; i < CONTROLLER_NUM; ++i) {
        m_bStickL_Up_Dwn_MovementBothDown[i]    = false;
        m_bStickL_X_Rgh_Lft_MovementBothDown[i] = false;
        m_bStickR_Up_Dwn_MovementBothDown[i]    = false;
        m_bStickR_X_Rgh_Lft_MovementBothDown[i] = false;
    }
}

// unused
// 0x52D1C0
int32 CControllerConfigManager::GetJoyButtonJustUp() {
    int32 v1 = 0;
    // NOTE: I don't get it.
    while ((m_NewJoyState.rgbButtons[v1] & 0x80) != 0 || (m_OldJoyState.rgbButtons[v1] & 0x80) == 0) {
        if (++v1 >= 16) {
            return 0;
        }
    }
    return v1 + 1;
}

// 0x52D1E0
int32 CControllerConfigManager::GetJoyButtonJustDown() {
    int32 v1 = 0;
    // NOTE: I don't get it.
    while ((m_NewJoyState.rgbButtons[v1] & 0x80) == 0 || (m_OldJoyState.rgbButtons[v1] & 0x80) != 0) {
        if (++v1 >= 16) {
            return 0;
        }
    }
    return v1 + 1;
}

// 0x52DDB0
bool CControllerConfigManager::GetIsKeyboardKeyDown(int32 key) {
    CPad* pad = CPad::GetPad();
    if (key < 255 && pad->NewKeyState.standardKeys[key]) {
        return true;
    }
    // TODO: normal magis value
    for (auto i = 0u; i <= rsF12 - rsF1; ++i) {
        if (i + rsF1 == key && pad->NewKeyState.FKeys[i]) {
            return true;
        }
    }

    switch (key) {
    // NOTE: If anyone wants to translate all the code into `Pad.h`, please do:
    case rsESC: return pad->NewKeyState.esc;
    case rsINS: return pad->NewKeyState.insert;
    case rsDEL: return pad->NewKeyState.del;
    case rsHOME: return pad->NewKeyState.home;
    case rsEND: return pad->NewKeyState.end;
    case rsPGUP: return pad->NewKeyState.pgup;
    case rsPGDN: return pad->NewKeyState.pgdn;
    case rsUP: return pad->NewKeyState.up;
    case rsDOWN: return pad->NewKeyState.down;
    case rsLEFT: return pad->NewKeyState.left;
    case rsRIGHT: return pad->NewKeyState.right;
    case rsDIVIDE: return pad->NewKeyState.div;
    case rsTIMES: return pad->NewKeyState.mul;
    case rsPLUS: return pad->NewKeyState.add;
    case rsMINUS: return pad->NewKeyState.sub;
    case rsPADDEL: return pad->NewKeyState.decimal;
    case rsPADEND: return pad->NewKeyState.num1;
    case rsPADDOWN: return pad->NewKeyState.num2;
    case rsPADPGDN: return pad->NewKeyState.num3;
    case rsPADLEFT: return pad->NewKeyState.num4;
    case rsPAD5: return pad->NewKeyState.num5;
    case rsNUMLOCK: return pad->NewKeyState.numlock;
    case rsPADRIGHT: return pad->NewKeyState.num6;
    case rsPADHOME: return pad->NewKeyState.num7;
    case rsPADUP: return pad->NewKeyState.num8;
    case rsPADPGUP: return pad->NewKeyState.num9;
    case rsPADINS: return pad->NewKeyState.num0;
    case rsPADENTER: return pad->NewKeyState.enter;
    case rsSCROLL: return pad->NewKeyState.scroll;
    case rsPAUSE: return pad->NewKeyState.pause;
    case rsBACKSP: return pad->NewKeyState.back;
    case rsTAB: return pad->NewKeyState.tab;
    case rsCAPSLK: return pad->NewKeyState.capslock;
    case rsENTER: return pad->NewKeyState.extenter;
    case rsLSHIFT: return pad->NewKeyState.lshift;
    case rsRSHIFT: return pad->NewKeyState.rshift;
    case rsSHIFT: return pad->NewKeyState.shift;
    case rsLCTRL: return pad->NewKeyState.lctrl;
    case rsRCTRL: return pad->NewKeyState.rctrl;
    case rsLALT: return pad->NewKeyState.lmenu;
    case rsRALT: return pad->NewKeyState.rmenu;
    case rsLWIN: return pad->NewKeyState.lwin;
    case rsRWIN: return pad->NewKeyState.rwin;
    case rsAPPS: return pad->NewKeyState.apps;
    default: return false;
    }
}

// 0x52E450
bool CControllerConfigManager::GetIsKeyboardKeyJustDown(int32 key) {
    CPad* pad = CPad::GetPad();
    if (key < 255 && pad->NewKeyState.standardKeys[key] && !pad->OldKeyState.standardKeys[key]) {
        return true;
    }
    // TODO: nornal magis value
    for (auto i = 0u; i <= rsF12 - rsF1; ++i) {
        if (i + rsF1 == key && pad->NewKeyState.FKeys[i] && !pad->OldKeyState.FKeys[i]) {
            return true;
        }
    }

    switch (key) {
    // NOTE: If anyone wants to translate all the code into `Pad.h`, please do:
    case rsESC: return pad->NewKeyState.esc && !pad->OldKeyState.esc;
    case rsINS: return pad->NewKeyState.insert && !pad->OldKeyState.insert;
    case rsDEL: return pad->NewKeyState.del && !pad->OldKeyState.del;
    case rsHOME: return pad->NewKeyState.home && !pad->OldKeyState.home;
    case rsEND: return pad->NewKeyState.end && !pad->OldKeyState.end;
    case rsPGUP: return pad->NewKeyState.pgup && !pad->OldKeyState.pgup;
    case rsPGDN: return pad->NewKeyState.pgdn && !pad->OldKeyState.pgdn;
    case rsUP: return pad->NewKeyState.up && !pad->OldKeyState.up;
    case rsDOWN: return pad->NewKeyState.down && !pad->OldKeyState.down;
    case rsLEFT: return pad->NewKeyState.left && !pad->OldKeyState.left;
    case rsRIGHT: return pad->NewKeyState.right && !pad->OldKeyState.right;
    case rsDIVIDE: return pad->NewKeyState.div && !pad->OldKeyState.div;
    case rsTIMES: return pad->NewKeyState.mul && !pad->OldKeyState.mul;
    case rsPLUS: return pad->NewKeyState.add && !pad->OldKeyState.add;
    case rsMINUS: return pad->NewKeyState.sub && !pad->OldKeyState.sub;
    case rsPADDEL: return pad->NewKeyState.decimal && !pad->OldKeyState.decimal;
    case rsPADEND: return pad->NewKeyState.num1 && !pad->OldKeyState.num1;
    case rsPADDOWN: return pad->NewKeyState.num2 && !pad->OldKeyState.num2;
    case rsPADPGDN: return pad->NewKeyState.num3 && !pad->OldKeyState.num3;
    case rsPADLEFT: return pad->NewKeyState.num4 && !pad->OldKeyState.num4;
    case rsPAD5: return pad->NewKeyState.num5 && !pad->OldKeyState.num5;
    case rsNUMLOCK: return pad->NewKeyState.numlock && !pad->OldKeyState.numlock;
    case rsPADRIGHT: return pad->NewKeyState.num6 && !pad->OldKeyState.num6;
    case rsPADHOME: return pad->NewKeyState.num7 && !pad->OldKeyState.num7;
    case rsPADUP: return pad->NewKeyState.num8 && !pad->OldKeyState.num8;
    case rsPADPGUP: return pad->NewKeyState.num9 && !pad->OldKeyState.num9;
    case rsPADINS: return pad->NewKeyState.num0 && !pad->OldKeyState.num0;
    case rsPADENTER: return pad->NewKeyState.enter && !pad->OldKeyState.enter;
    case rsSCROLL: return pad->NewKeyState.scroll && !pad->OldKeyState.scroll;
    case rsPAUSE: return pad->NewKeyState.pause && !pad->OldKeyState.pause;
    case rsBACKSP: return pad->NewKeyState.back && !pad->OldKeyState.back;
    case rsTAB: return pad->NewKeyState.tab && !pad->OldKeyState.tab;
    case rsCAPSLK: return pad->NewKeyState.capslock && !pad->OldKeyState.capslock;
    case rsENTER: return pad->NewKeyState.extenter && !pad->OldKeyState.extenter;
    case rsLSHIFT: return pad->NewKeyState.lshift && !pad->OldKeyState.lshift;
    case rsRSHIFT: return pad->NewKeyState.rshift && !pad->OldKeyState.rshift;
    case rsSHIFT: return pad->NewKeyState.shift && !pad->OldKeyState.shift;
    case rsLCTRL: return pad->NewKeyState.lctrl && !pad->OldKeyState.lctrl;
    case rsRCTRL: return pad->NewKeyState.rctrl && !pad->OldKeyState.rctrl;
    case rsLALT: return pad->NewKeyState.lmenu && !pad->OldKeyState.lmenu;
    case rsRALT: return pad->NewKeyState.rmenu && !pad->OldKeyState.rmenu;
    case rsLWIN: return pad->NewKeyState.lwin && !pad->OldKeyState.lwin;
    case rsRWIN: return pad->NewKeyState.rwin && !pad->OldKeyState.rwin;
    case rsAPPS: return pad->NewKeyState.apps && !pad->OldKeyState.apps;
    default: return false;
    }
}

// 0x52EF30
bool CControllerConfigManager::GetIsMouseButtonDown(int32 key) {
    return CheckMouseButtonState(key);
}
 
// 0x52F020
bool CControllerConfigManager::GetIsMouseButtonUp(int32 key) {
    return !CheckMouseButtonState(key);
}

// 0x52F110
bool CControllerConfigManager::GetIsMouseButtonJustUp(int32 key) {
    return CheckMouseButtonJustUpState(key);
}

// unused
// 0x52F2A0
bool CControllerConfigManager::GetIsKeyBlank(int32 key, eControllerType type) {
    switch (type) {
    case CONTROLLER_KEYBOARD1:
    case CONTROLLER_KEYBOARD2:
        return (key == rsNULL);
    case CONTROLLER_MOUSE:
    case CONTROLLER_PAD:
        return (key == 0);
    default:
        return true;
    }
}

// 0x52F2F0
eActionType CControllerConfigManager::GetActionType(eControllerAction action) {
    switch (action) {
    case CA_PED_FIRE_WEAPON:
    case CA_PED_FIRE_WEAPON_ALT:
    case CA_GO_FORWARD:
    case CA_GO_BACK:
    case CA_GO_LEFT:
    case CA_GO_RIGHT:
    case CA_PED_SNIPER_ZOOM_IN:
    case CA_PED_SNIPER_ZOOM_OUT:
    case CA_PED_1RST_PERSON_LOOK_LEFT:
    case CA_PED_1RST_PERSON_LOOK_RIGHT:
    case CA_PED_LOCK_TARGET:
    case CA_PED_1RST_PERSON_LOOK_UP:
    case CA_PED_1RST_PERSON_LOOK_DOWN:
        return ACTION_FIRST_THIRD_PERSON;
    case CA_PED_CYCLE_WEAPON_RIGHT:
    case CA_PED_CYCLE_WEAPON_LEFT:
    case CA_PED_JUMPING:
    case CA_PED_SPRINT:
    case CA_PED_LOOKBEHIND:
    case CA_PED_DUCK:
    case CA_PED_ANSWER_PHONE:
    case CA_PED_WALK:
    case CA_PED_CYCLE_TARGET_LEFT:
    case CA_PED_CYCLE_TARGET_RIGHT:
    case CA_PED_CENTER_CAMERA_BEHIND_PLAYER:
    case CA_CONVERSATION_YES:
    case CA_CONVERSATION_NO:
    case CA_GROUP_CONTROL_FWD:
    case CA_GROUP_CONTROL_BWD:
        return ACTION_THIRD_PERSON;
    case CA_VEHICLE_ENTER_EXIT:
        return ACTION_IN_CAR_THIRD_PERSON;
    case CA_CAMERA_CHANGE_VIEW_ALL_SITUATIONS:
    case CA_NETWORK_TALK:
    case CA_TOGGLE_DPAD:
    case CA_SWITCH_DEBUG_CAM_ON:
    case CA_TAKE_SCREEN_SHOT:
    case CA_SHOW_MOUSE_POINTER_TOGGLE:
        return ACTION_COMMON_CONTROLS;
    case CA_VEHICLE_FIRE_WEAPON:
    case CA_VEHICLE_FIRE_WEAPON_ALT:
    case CA_VEHICLE_STEER_LEFT:
    case CA_VEHICLE_STEER_RIGHT:
    case CA_VEHICLE_STEER_UP:
    case CA_VEHICLE_STEER_DOWN:
    case CA_VEHICLE_ACCELERATE:
    case CA_VEHICLE_BRAKE:
    case CA_VEHICLE_RADIO_STATION_UP:
    case CA_VEHICLE_RADIO_STATION_DOWN:
    case CA_VEHICLE_RADIO_TRACK_SKIP:
    case CA_VEHICLE_HORN:
    case CA_TOGGLE_SUBMISSIONS:
    case CA_VEHICLE_HANDBRAKE:
    case CA_VEHICLE_LOOK_LEFT:
    case CA_VEHICLE_LOOK_RIGHT:
    case CA_VEHICLE_LOOKBEHIND:
    case CA_VEHICLE_MOUSE_LOOK:
    case CA_VEHICLE_TURRET_LEFT:
    case CA_VEHICLE_TURRET_RIGHT:
    case CA_VEHICLE_TURRET_UP:
    case CA_VEHICLE_TURRET_DOWN:
        return ACTION_IN_CAR;
    default:
        return ACTION_NOT_TYPE;
    }
}

// 0x52F390
const GxtChar* CControllerConfigManager::GetControllerSettingTextMouse(eControllerAction action) {
    switch (m_Actions[action].Keys[CONTROLLER_MOUSE].KeyCode) {
    case rsMOUSE_LEFT_BUTTON:
        return TheText.Get("FEC_MSL");
    case rsMOUSE_MIDDLE_BUTTON:
        return TheText.Get("FEC_MSM");
    case rsMOUSE_RIGHT_BUTTON:
        return TheText.Get("FEC_MSR");
    case rsMOUSE_WHEEL_UP_BUTTON:
        return TheText.Get("FEC_MWF");
    case rsMOUSE_WHEEL_DOWN_BUTTON:
        return TheText.Get("FEC_MWB");
    case rsMOUSE_X1_BUTTON:
        return TheText.Get("FEC_MXO");
    case rsMOUSE_X2_BUTTON:
        return TheText.Get("FEC_MXT");
    default:
        return 0;
    }
}

// 0x52F450
const GxtChar* CControllerConfigManager::GetControllerSettingTextJoystick(eControllerAction action) {
    static thread_local GxtChar s_buttonName[64]; // 0xB7147C

    const auto keyCode = (RsKeyCodes)m_Actions[action].Keys[CONTROLLER_PAD].KeyCode;
    if (!keyCode) {
        return 0;
    }

    const GxtChar* joystickButtonText = TheText.Get("FEC_JBO");

    GxtChar number[8];
    AsciiToGxtChar(std::to_string(keyCode).c_str(), number);

    TextCopy(s_buttonName, joystickButtonText);
    GxtCharStrcat(s_buttonName, " "_gxt);
    GxtCharStrcat(s_buttonName, number);
    return s_buttonName;
}

// 0x530500
eControllerType CControllerConfigManager::MakeControllerActionsBlank() {
    eControllerType result;
    for (auto i = 0u; i < CONTROLLER_NUM; ++i) {
        for (auto j = 0u; j < NUM_OF_CONTROLLER_ACTIONS; ++j) {
            result = ClearSettingsAssociatedWithAction((eControllerAction)j, (eControllerType)i);
        }
    }
    return result;
}

// 0x531140
void CControllerConfigManager::ProcessKeyboardInput() {
    bool inMenu = !CPad::padNumber && !FrontEndMenuManager.m_bMenuActive;

    for (auto i = 0u; i < NUM_OF_CONTROLLER_ACTIONS; ++i) {
        const auto key1 = m_Actions[i].Keys[CONTROLLER_KEYBOARD1].KeyCode;
        const auto key2 = m_Actions[i].Keys[CONTROLLER_KEYBOARD2].KeyCode;
        CPad* pad = CPad::GetPad();

        auto ProcessKeyEvent = [&](int32 button, eControllerType type, CControllerState* state) {
            if (!pad) {
                return;
            }

            bool driving = false;
            if (const auto playerPed = FindPlayerPed()) {
                if (const auto playerVehicle = FindPlayerVehicle()) {
                    driving = playerPed->m_nPedState == PEDSTATE_DRIVING && !pad->DisablePlayerControls;
                }
            }

            eCamMode camMode = TheCamera.m_aCams[TheCamera.m_nActiveCam].m_nMode;
            const auto firstPerson = camMode == MODE_1STPERSON || camMode == MODE_SNIPER || camMode == MODE_ROCKETLAUNCHER || camMode == MODE_ROCKETLAUNCHER_HS || camMode == MODE_M16_1STPERSON || camMode == MODE_CAMERA;

            if (driving) {
                HandleButtonPress(button, type, state);
            } else {
                UpdateControllerStateOnAction(button, type, state);
                if (firstPerson) {
                    Handle1stPersonCameraActions(button, type, state);
                } else {
                    UpdatePadStateOnActions(button, type, state);
                }
                UpdatePadStateOnMenuActions(button, type, state);

                SetVehicleEnterExitButton(button, type, state);
            }
        };

        if (GetIsKeyboardKeyDown(key1)) {
            if (inMenu && key1 != rsNULL && pad) {
                ProcessKeyEvent(key1, CONTROLLER_KEYBOARD1, &pad->PCTempKeyState);
            }
        }

        if (GetIsKeyboardKeyDown(key2)) {
            if (inMenu && key2 != rsNULL && pad) {
                ProcessKeyEvent(key2, CONTROLLER_KEYBOARD2, &pad->PCTempKeyState);
            }
        }

        if (pad) {
            CControllerState* tempState = &pad->PCTempKeyState;
            if (!FrontEndMenuManager.m_bMenuActive) {
                if (!GetIsKeyboardKeyDown(key2) && key1 != rsNULL && GetIsKeyboardKeyDown(key1)) {
                    if (m_Actions[CA_NETWORK_TALK].Keys[CONTROLLER_KEYBOARD1].KeyCode == key1) {
                        tempState->m_bChatIndicated = 0;
                    }
                    if (m_Actions[CA_VEHICLE_MOUSE_LOOK].Keys[CONTROLLER_KEYBOARD1].KeyCode == key1) {
                        tempState->m_bVehicleMouseLook = 0;
                    }
                }
                if (!GetIsKeyboardKeyDown(key1) && key2 != rsNULL && GetIsKeyboardKeyDown(key2)) {
                    if (m_Actions[CA_NETWORK_TALK].Keys[CONTROLLER_KEYBOARD2].KeyCode == key2) {
                        tempState->m_bChatIndicated = 0;
                    }
                    if (m_Actions[CA_VEHICLE_MOUSE_LOOK].Keys[CONTROLLER_KEYBOARD2].KeyCode == key2) {
                        tempState->m_bVehicleMouseLook = 0;
                    }
                }
            }
        }
    }
}

// 0x5314A0
void CControllerConfigManager::ProcessMouseInput() {
    CPad* pad = CPad::GetPad();
    bool controlsEnabled = !CPad::padNumber && !FrontEndMenuManager.m_bMenuActive;
    CControllerState* state = &pad->PCTempMouseState;

    for (auto i = 0u; i < NUM_OF_CONTROLLER_ACTIONS; ++i) {
        const auto button = m_Actions[i].Keys[CONTROLLER_MOUSE].KeyCode;

        if (CControllerConfigManager::GetIsMouseButtonDown(button)) {
            if (controlsEnabled && button && pad) {
                bool driving = false;
                if (const auto playerPed = FindPlayerPed()) {
                    if (const auto playerVehicle = FindPlayerVehicle()) {
                        driving = playerPed->m_nPedState == PEDSTATE_DRIVING && !pad->DisablePlayerControls;
                    }
                }

                eCamMode camMode = TheCamera.m_aCams[TheCamera.m_nActiveCam].m_nMode;
                bool firstPerson = camMode == MODE_1STPERSON || camMode == MODE_SNIPER || camMode == MODE_ROCKETLAUNCHER || camMode == MODE_ROCKETLAUNCHER_HS || camMode == MODE_M16_1STPERSON || camMode == MODE_CAMERA;

                if (driving) {
                    HandleButtonPress(button, CONTROLLER_MOUSE, state);
                } else {
                    UpdateControllerStateOnAction(button, CONTROLLER_MOUSE, state);
                    if (firstPerson) {
                        Handle1stPersonCameraActions(button, CONTROLLER_MOUSE, state);
                    } else {
                        UpdatePadStateOnActions(button, CONTROLLER_MOUSE, state);
                    }
                    UpdatePadStateOnMenuActions(button, CONTROLLER_MOUSE, state);

                    SetVehicleEnterExitButton(button, CONTROLLER_MOUSE, state);
                }
            }
        }

        if (GetIsMouseButtonUp(button) && button && pad) {
            if (!FrontEndMenuManager.m_bMenuActive) {
                if (m_Actions[CA_NETWORK_TALK].Keys[CONTROLLER_MOUSE].KeyCode == button) {
                    state->m_bChatIndicated = 0;
                }
                if (m_Actions[CA_VEHICLE_MOUSE_LOOK].Keys[CONTROLLER_MOUSE].KeyCode == button) {
                    state->m_bVehicleMouseLook = 0;
                }
            }
        }
    }
}
    // 0x531C90
void CControllerConfigManager::DeleteMapping(eControllerAction action, int32 button, eControllerType type) {
    if (!GetIsKeyBlank(button, type)) {
        return;
    }

    switch (GetActionType(action)) {
    case ACTION_FIRST_PERSON:
        ClearCommonMappings(action, button, type);
        ClearSniperZoomMappings(action, button, type);
        Clear1st3rdPersonMappings(action, button, type);
        break;
    case ACTION_THIRD_PERSON:
        ClearCommonMappings(action, button, type);
        Clear1st3rdPersonMappings(action, button, type);
        ClearPedMappings(action, button, type);
        UnmapVehicleEnterExit(button, type);
        break;
    case ACTION_IN_CAR:
        ClearCommonMappings(action, button, type);
        ClearVehicleMappings(action, button, type);
        UnmapVehicleEnterExit(button, type);
        break;
    case ACTION_IN_CAR_THIRD_PERSON:
        ClearCommonMappings(action, button, type);
        Clear1st3rdPersonMappings(action, button, type);
        ClearPedMappings(action, button, type);
        ClearVehicleMappings(action, button, type);
        break;
    case ACTION_COMMON_CONTROLS:
        ClearCommonMappings(action, button, type);
        ClearSniperZoomMappings(action, button, type);
        Clear1st3rdPersonMappings(action, button, type);
        ClearPedMappings(action, button, type);
        ClearVehicleMappings(action, button, type);
        UnmapVehicleEnterExit(button, type);
        break;
    case ACTION_FIRST_THIRD_PERSON:
        ClearCommonMappings(action, button, type);
        ClearSniperZoomMappings(action, button, type);
        Clear1st3rdPersonMappings(action, button, type);
        ClearPedMappings(action, button, type);
        UnmapVehicleEnterExit(button, type);
        break;
    default:
        return;
    }
}

// 0x52FE10
const GxtChar* CControllerConfigManager::GetKeyNameForKeyboard(eControllerAction action, eControllerType type) {
    static thread_local GxtChar s_keyName[50]; // 0xB714BC

    const auto key = m_Actions[action].Keys[type].KeyCode;
    // TODO: refactor code
    const auto printKey = [&](const GxtChar* text, int32 key) {
        CMessages::InsertNumberInString(text, key, -1, -1, -1, -1, -1, s_keyName);
        return s_keyName;
    };

    if (key == rsNULL) {
        return 0;
    }
    if (key >= 256) {
        switch (key) {
        case rsF1:
        case rsF2:
        case rsF3:
        case rsF4:
        case rsF5:
        case rsF6:
        case rsF7:
        case rsF8:
        case rsF9:
        case rsF10:
        case rsF11:
        case rsF12:
            // TODO: refactor code
            return printKey(TheText.Get("FEC_FNC"), key - rsESC);
        case rsINS: return TheText.Get("FEC_IRT");
        case rsDEL: return TheText.Get("FEC_DLL");
        case rsHOME: return TheText.Get("FEC_HME");
        case rsEND: return TheText.Get("FEC_END");
        case rsPGUP: return TheText.Get("FEC_PGU");
        case rsPGDN: return TheText.Get("FEC_PGD");
        case rsUP: return TheText.Get("FEC_UPA");
        case rsDOWN: return TheText.Get("FEC_DWA");
        case rsLEFT: return TheText.Get("FEC_LFA");
        case rsRIGHT: return TheText.Get("FEC_RFA");
        case rsDIVIDE: return TheText.Get("FEC_FWS");
        case rsTIMES: return TheText.Get("FECSTAR");
        case rsPLUS: return TheText.Get("FEC_PLS");
        case rsMINUS: return TheText.Get("FEC_MIN");
        case rsPADDEL: return TheText.Get("FEC_DOT");
            // TODO: refactor code
        case rsPADEND: return printKey(TheText.Get("FEC_NMN"), 1);
        case rsPADDOWN: return printKey(TheText.Get("FEC_NMN"), 2);
        case rsPADPGDN: return printKey(TheText.Get("FEC_NMN"), 3);
        case rsPADLEFT: return printKey(TheText.Get("FEC_NMN"), 4);
        case rsPAD5: return printKey(TheText.Get("FEC_NMN"), 5);
        case rsPADRIGHT: return printKey(TheText.Get("FEC_NMN"), 6);
        case rsPADHOME: return printKey(TheText.Get("FEC_NMN"), 7);
        case rsPADUP: return printKey(TheText.Get("FEC_NMN"), 8);
        case rsPADPGUP: return printKey(TheText.Get("FEC_NMN"), 9);
        case rsPADINS: return printKey(TheText.Get("FEC_NMN"), 0);

        case rsNUMLOCK: return TheText.Get("FEC_NLK");
        case rsPADENTER: return TheText.Get("FEC_ETR");
        case rsSCROLL: return TheText.Get("FEC_SLK");
        case rsPAUSE: return TheText.Get("FEC_PSB");
        case rsBACKSP: return TheText.Get("FEC_BSP");
        case rsTAB: return TheText.Get("FEC_TAB");
        case rsCAPSLK: return TheText.Get("FEC_CLK");
        case rsENTER: return TheText.Get("FEC_RTN");
        case rsLSHIFT: return TheText.Get("FEC_LSF");
        case rsRSHIFT: return TheText.Get("FEC_RSF");
        case rsSHIFT: return TheText.Get("FEC_SFT");
        case rsLCTRL: return TheText.Get("FEC_LCT");
        case rsRCTRL: return TheText.Get("FEC_RCT");
        case rsLALT: return TheText.Get("FEC_LAL");
        case rsRALT: return TheText.Get("FEC_RAL");
        case rsLWIN: return TheText.Get("FEC_LWD");
        case rsRWIN: return TheText.Get("FEC_RWD");
        case rsAPPS: return TheText.Get("FEC_WRC");
        default: return 0;
        }
    } else {
        switch (static_cast<char>(key)) {
        case '*': return TheText.Get("FEC_AST");
        case ' ': return TheText.Get("FEC_SPC");
        case '^':
            if (FrontEndMenuManager.m_nTextLanguage == GERMAN) {
                return "|"_gxt;
            }
            break;
        case 178:
            if (FrontEndMenuManager.m_nTextLanguage == FRENCH) {
                return "2"_gxt;
            }
            break;
        default:
            s_keyName[0] = key;
            if (!s_keyName[0]) {
                s_keyName[0] = '#';
            }
            s_keyName[1] = '\0';
            return s_keyName;
        }
    }
    return 0;
}


// 0x52F560
const GxtChar* CControllerConfigManager::GetButtonComboText(eControllerAction event) {
    return event == CA_VEHICLE_LOOKBEHIND ? TheText.Get("FEC_LBC") : 0;
}

// 0x5303D0
const GxtChar* CControllerConfigManager::GetDefinedKeyByGxtName(eControllerAction action) {
    static thread_local GxtChar s_buttonName[64]; // 0xB7147C
    if (FrontEndMenuManager.m_bController) {
        if (const auto keyCode = m_Actions[action].Keys[CONTROLLER_PAD].KeyCode) {
            const auto joyButtonText = TheText.Get("FEC_JBO");
            CMessages::InsertNumberInString(joyButtonText, keyCode, -1, -1, -1, -1, -1, s_buttonName);
            return s_buttonName;
        }
    }

    if (const auto keyText = GetControllerSettingTextMouse(action)) {
        return keyText;
    }

    if (const auto keyText = GetKeyNameForKeyboard(action, CONTROLLER_KEYBOARD1)) {
        return keyText;
    }

    if (const auto keyText = GetKeyNameForKeyboard(action, CONTROLLER_KEYBOARD2)) {
        return keyText;
    }

    if (const auto keyCode = !m_Actions[action].Keys[CONTROLLER_PAD].KeyCode) {
        return GetControllerSettingTextMouse(action);
    }

    return 0;
}

// NOTSA
eControllerAction CControllerConfigManager::GetActionIDByName(std::string_view name) {
    for (auto&& [i, actionName] : notsa::enumerate(m_arrControllerActionName)) {
        if (std::string_view{ actionName } == name) {
            return (eControllerAction)i;
        }
    }
    return CA_INVALID;
}

// NOTSA
bool CControllerConfigManager::CheckMouseButtonState(int32 key) {
    const auto* pad = CPad::GetPad();
    switch (key) {
    case rsMOUSE_LEFT_BUTTON:
        return pad->IsMouseLButton();
    case rsMOUSE_RIGHT_BUTTON:
        return pad->IsMouseRButton();
    case rsMOUSE_MIDDLE_BUTTON:
        return pad->IsMouseMButton();
    case rsMOUSE_WHEEL_UP_BUTTON:
        return pad->IsMouseWheelUp();
    case rsMOUSE_WHEEL_DOWN_BUTTON:
        return pad->IsMouseWheelDown();
    case rsMOUSE_X1_BUTTON:
        return pad->IsMouseBmx1();
    case rsMOUSE_X2_BUTTON:
        return pad->IsMouseBmx2();
    default:
        return false;
    }
}

// NOTSA
bool CControllerConfigManager::CheckMouseButtonJustUpState(int32 key) {
    const auto* pad = CPad::GetPad();
    switch (key) {
    case rsMOUSE_LEFT_BUTTON:
        return pad->IsMouseLButtonPressed();
    case rsMOUSE_RIGHT_BUTTON:
        return pad->IsMouseRButtonPressed();
    case rsMOUSE_MIDDLE_BUTTON:
        return pad->IsMouseMButtonPressed();
    case rsMOUSE_WHEEL_UP_BUTTON:
        return pad->IsMouseWheelUpPressed();
    case rsMOUSE_WHEEL_DOWN_BUTTON:
        return pad->IsMouseWheelDownPressed();
    case rsMOUSE_X1_BUTTON:
        return pad->IsMouseBmx1Pressed();
    case rsMOUSE_X2_BUTTON:
        return pad->IsMouseBmx2Pressed();
    default:
        return false;
    }
}

// NOTSA
bool CControllerConfigManager::IsCheckSpecificGamepad()
{
    // TODO: Reverse CPadConfig
    return (PadConfigs[0].vendorId == 0x3427 && PadConfigs[0].productId == 0x1190);
}

// NOTSA
void CControllerConfigManager::CheckAndClear(eControllerAction action, eControllerType type, int32 button) {
    if (m_Actions[action].Keys[type].KeyCode == button) {
        ClearSettingsAssociatedWithAction(action, type);
    }
};

// NOTSA
void CControllerConfigManager::CheckAndSetButton(eControllerAction action, eControllerType type, int32 button, int16& stateButton) {
    if (m_Actions[action].Keys[type].KeyCode == button) {
        stateButton = 255;
    }
}

// NOTSA
void CControllerConfigManager::CheckAndSetPad(eControllerAction action, eControllerType type, int32 button, int16& dpad, int16& oppositeDpad) {
    if (m_Actions[action].Keys[type].KeyCode == button) {
        if (dpad) {
            dpad         = 0;
            oppositeDpad = 0;
        } else {
            dpad = 255;
        }
    }
}
