#include "StdInc.h"
#include "WinInput.h"

#include "ControllerConfigManager.h"

#ifndef NOTSA_USE_SDL3
#include "WinPlatform.h"
#endif // !NOTSA_USE_SDL3

using enum eControllerAction;

CControllerConfigManager& ControlsManager = *(CControllerConfigManager*)0xB70198;

void CControllerConfigManager::InjectHooks() {
    RH_ScopedClass(CControllerConfigManager);
    RH_ScopedCategoryGlobal();
    RH_ScopedInstall(Constructor, 0x531EE0);
    RH_ScopedInstall(LoadSettings, 0x530530);
    RH_ScopedInstall(SaveSettings, 0x52D200);
    RH_ScopedInstall(InitDefaultControlConfiguration, 0x530640);
    RH_ScopedInstall(InitDefaultControlConfigJoyPad, 0x530B00);
    RH_ScopedInstall(InitDefaultControlConfigMouse, 0x52F6F0);
    RH_ScopedInstall(InitialiseControllerActionNameArray, 0x52D260);
    RH_ScopedInstall(ReinitControls, 0x531F20);
    RH_ScopedInstall(SetMouseButtonAssociatedWithAction, 0x52F590);
    RH_ScopedInstall(StoreMouseButtonState, 0x52DA30);
    RH_ScopedInstall(AffectControllerStateOn_ButtonUp, 0x531070);
    RH_ScopedInstall(AffectControllerStateOn_ButtonDown_DebugStuff, 0x52DC10);
    RH_ScopedInstall(AffectControllerStateOn_ButtonUp_DebugStuff, 0x52DD80);
    RH_ScopedInstall(ClearSimButtonPressCheckers, 0x52DD90);
    RH_ScopedInstall(GetIsKeyboardKeyDown, 0x52DDB0);
    RH_ScopedInstall(GetIsKeyboardKeyJustDown, 0x52E450);
    RH_ScopedInstall(GetIsMouseButtonDown, 0x52EF30);
    RH_ScopedInstall(GetIsMouseButtonUp, 0x52F020);
    RH_ScopedInstall(GetIsMouseButtonJustUp, 0x52F110);
    RH_ScopedInstall(GetIsKeyBlank, 0x52F2A0);
    RH_ScopedInstall(GetActionType, 0x52F2F0);
    RH_ScopedInstall(GetControllerKeyAssociatedWithAction, 0x52F4F0);
    RH_ScopedInstall(ResetSettingOrder, 0x52F5F0);
    RH_ScopedInstall(AffectControllerStateOn_ButtonDown_Driving, 0x52F7B0);
    RH_ScopedInstall(AffectControllerStateOn_ButtonDown_FirstPersonOnly, 0x52F9E0);
    RH_ScopedInstall(GetControllerSettingTextMouse, 0x52F390);
    RH_ScopedInstall(GetControllerSettingTextJoystick, 0x52F450);
    RH_ScopedInstall(GetNumOfSettingsForAction, 0x52F4A0);
    RH_ScopedInstall(GetIsActionAButtonCombo, 0x52F550);
    RH_ScopedInstall(GetButtonComboText, 0x52F560);
    RH_ScopedInstall(GetMouseButtonAssociatedWithAction, 0x52F580);
    RH_ScopedInstall(AffectControllerStateOn_ButtonDown_ThirdPersonOnly, 0x52FA20);
    RH_ScopedInstall(AffectControllerStateOn_ButtonDown_FirstAndThirdPersonOnly, 0x52FAB0);
    RH_ScopedInstall(AffectControllerStateOn_ButtonDown_AllStates, 0x52FCA0);
    RH_ScopedInstall(AffectControllerStateOn_ButtonDown_VehicleAndThirdPersonOnly, 0x52FD20);
    RH_ScopedInstall(AffectControllerStateOn_ButtonUp_All_Player_States, 0x52FD40);
    RH_ScopedInstall(ClearSettingsAssociatedWithAction, 0x52FD70);
    RH_ScopedInstall(GetControllerSettingTextKeyBoard, 0x52FE10);
    RH_ScopedInstall(DeleteMatchingVehicle_3rdPersonControls, 0x531BC0);
    RH_ScopedInstall(DeleteMatching1rstPersonControls, 0x531C20);
    RH_ScopedInstall(DeleteMatchingActionInitiators, 0x531C90);
    RH_ScopedInstall(GetControllerSettingTextWithOrderNumber, 0x531E20);
    RH_ScopedInstall(GetFirstKeyboardControllerText, 0x531E90);
    RH_ScopedInstall(GetGxtStringOfCommandKeys, 0x5303D0);
    RH_ScopedInstall(AffectPadFromMouse, 0x5314A0);
    RH_ScopedInstall(DeleteMatching1rst3rdPersonControls, 0x5318C0);
    RH_ScopedInstall(DeleteMatchingVehicleControls, 0x5319D0);
    RH_ScopedInstall(SetControllerKeyAssociatedWithAction, 0x530490);
    RH_ScopedInstall(MakeControllerActionsBlank, 0x530500);
    RH_ScopedInstall(AffectPadFromKeyBoard, 0x531140);
    RH_ScopedInstall(DeleteMatchingCommonControls, 0x531670);
    RH_ScopedInstall(DeleteMatching3rdPersonControls, 0x531730);
    RH_ScopedInstall(UpdateJoyInConfigMenus_ButtonDown, 0x52DAB0);
    RH_ScopedInstall(UpdateJoyInConfigMenus_ButtonUp, 0x52DC20);
    RH_ScopedInstall(GetJoyButtonJustUp, 0x52D1C0);
    RH_ScopedInstall(GetJoyButtonJustDown, 0x52D1E0);
    RH_ScopedInstall(UpdateJoy_ButtonDown, 0x530ED0);
    RH_ScopedInstall(UpdateJoyButtonState, 0x52F510);
}

// 0x531EE0
CControllerConfigManager::CControllerConfigManager() {
    /* Member variable init done in header */

    MakeControllerActionsBlank();
    InitDefaultControlConfiguration();
    InitialiseControllerActionNameArray();
}

// 0x531730
void CControllerConfigManager::DeleteMatching3rdPersonControls(eControllerAction action, KeyCode button, eControllerType type) {
    if (GetIsKeyBlank(button, type)) {
        return;
    }

    CheckAndClear(GO_LEFT, type, button);
    CheckAndClear(GO_RIGHT, type, button);
    CheckAndClear(GROUP_CONTROL_FWD, type, button);
    CheckAndClear(GROUP_CONTROL_BWD, type, button);
    CheckAndClear(CONVERSATION_NO, type, button);
    CheckAndClear(CONVERSATION_YES, type, button);
    CheckAndClear(PED_CYCLE_WEAPON_LEFT, type, button);
    CheckAndClear(PED_CYCLE_WEAPON_RIGHT, type, button);
    CheckAndClear(PED_JUMPING, type, button);
    CheckAndClear(PED_SPRINT, type, button);
    CheckAndClear(PED_LOOKBEHIND, type, button);
    CheckAndClear(PED_DUCK, type, button);

    if (action != PED_FIRE_WEAPON_ALT
        && FrontEndMenuManager.m_ControlMethod == eController::JOYPAD
        || FrontEndMenuManager.m_ControlMethod == eController::MOUSE_PLUS_KEYS)
    {
        CheckAndClear(PED_ANSWER_PHONE, type, button);
    }
    CheckAndClear(PED_WALK, type, button);
    if (FrontEndMenuManager.m_ControlMethod == eController::JOYPAD) {
        CheckAndClear(PED_CENTER_CAMERA_BEHIND_PLAYER, type, button);
    }
}

// 0x531670
void CControllerConfigManager::DeleteMatchingCommonControls(eControllerAction nop, KeyCode button, eControllerType type) {
    if (GetIsKeyBlank(button, type)) {
        return;
    }

    CheckAndClear(CAMERA_CHANGE_VIEW_ALL_SITUATIONS, type, button);
    CheckAndClear(NETWORK_TALK, type, button);
    CheckAndClear(SWITCH_DEBUG_CAM_ON, type, button);
    CheckAndClear(TOGGLE_DPAD, type, button);
    CheckAndClear(TAKE_SCREEN_SHOT, type, button);
    CheckAndClear(SHOW_MOUSE_POINTER_TOGGLE, type, button);
}

// 0x530490
void CControllerConfigManager::SetControllerKeyAssociatedWithAction(eControllerAction action, KeyCode button, eControllerType type) {
    ResetSettingOrder(action);
    const auto numOfSettings = GetNumOfSettingsForAction(action);
    m_Actions[+action].Keys[+type].m_uiActionInitiator = button;
    m_Actions[+action].Keys[+type].m_uiSetOrder = eContSetOrder(+numOfSettings + 1);
}

// 0x5319D0
void CControllerConfigManager::DeleteMatchingVehicleControls(eControllerAction nop, KeyCode button, eControllerType type) {
    if (GetIsKeyBlank(button, type)) {
        return;
    }

    CheckAndClear(VEHICLE_FIRE_WEAPON_ALT, type, button);
    CheckAndClear(VEHICLE_FIRE_WEAPON, type, button);
    CheckAndClear(VEHICLE_STEERLEFT, type, button);
    CheckAndClear(VEHICLE_STEERRIGHT, type, button);
    CheckAndClear(VEHICLE_STEERUP, type, button);
    CheckAndClear(VEHICLE_STEERDOWN, type, button);
    CheckAndClear(VEHICLE_ACCELERATE, type, button);
    CheckAndClear(VEHICLE_BRAKE, type, button);
    CheckAndClear(VEHICLE_RADIO_STATION_UP, type, button);
    CheckAndClear(VEHICLE_RADIO_STATION_DOWN, type, button);
    CheckAndClear(VEHICLE_RADIO_TRACK_SKIP, type, button);
    CheckAndClear(VEHICLE_HORN, type, button);
    CheckAndClear(TOGGLE_SUBMISSIONS, type, button);
    CheckAndClear(VEHICLE_HANDBRAKE, type, button);
    CheckAndClear(VEHICLE_LOOKLEFT, type, button);
    CheckAndClear(VEHICLE_LOOKRIGHT, type, button);
    CheckAndClear(VEHICLE_LOOKBEHIND, type, button);
    CheckAndClear(VEHICLE_MOUSELOOK, type, button);
    CheckAndClear(VEHICLE_TURRETLEFT, type, button);
    CheckAndClear(VEHICLE_TURRETRIGHT, type, button);
    CheckAndClear(VEHICLE_TURRETUP, type, button);
    CheckAndClear(VEHICLE_TURRETDOWN, type, button);
}

// 0x5318C0
void CControllerConfigManager::DeleteMatching1rst3rdPersonControls(eControllerAction action, KeyCode button, eControllerType type) {
    if (GetIsKeyBlank(button, type)) {
        return;
    }

    if (action != PED_ANSWER_PHONE && FrontEndMenuManager.m_ControlMethod == eController::JOYPAD || FrontEndMenuManager.m_ControlMethod == eController::MOUSE_PLUS_KEYS) {
        CheckAndClear(PED_FIRE_WEAPON_ALT, type, button);
    }
    CheckAndClear(PED_FIRE_WEAPON, type, button);
    CheckAndClear(PED_LOCK_TARGET, type, button);
    CheckAndClear(GO_FORWARD, type, button);
    CheckAndClear(GO_BACK, type, button);
    if (FrontEndMenuManager.m_ControlMethod == eController::JOYPAD) {
        CheckAndClear(PED_1RST_PERSON_LOOK_LEFT, type, button);
        CheckAndClear(PED_1RST_PERSON_LOOK_RIGHT, type, button);
        CheckAndClear(PED_1RST_PERSON_LOOK_DOWN, type, button);
        CheckAndClear(PED_1RST_PERSON_LOOK_UP, type, button);
    }
}

// 0x52F510
void CControllerConfigManager::UpdateJoyButtonState(int32 padnumber) {
    for (auto i = 0; i < JOYBUTTON_COUNT; i++) {
        m_ButtonStates[i] = (m_NewJoyState.rgbButtons[i] & 0x80) ? true : false;
    }
}

// unused
// 0x531E90
const GxtChar* CControllerConfigManager::GetFirstKeyboardControllerText(eControllerAction action) {
    ResetSettingOrder(action);
    for (const auto& type : CONTROLLER_TYPES_KEYBOARD) {
        if (m_Actions[+action].Keys[+type].m_uiSetOrder != eContSetOrder::NONE) {
            return GetControllerSettingTextKeyBoard(action, type);
        }
    }
    return nullptr;
}

// 0x531E20
const GxtChar* CControllerConfigManager::GetControllerSettingTextWithOrderNumber(eControllerAction action, eContSetOrder order) {
    for (const auto& type : CONTROLLER_TYPES_ALL) {
        if (m_Actions[+action].Keys[+type].m_uiSetOrder == order) {
            switch (type) {
            case eControllerType::KEYBOARD:
            case eControllerType::OPTIONAL_EXTRA_KEY: return GetControllerSettingTextKeyBoard(action, type);
            case eControllerType::MOUSE:              return GetControllerSettingTextMouse(action);
            case eControllerType::JOY_STICK:          return GetControllerSettingTextJoystick(action);
            default:                                  NOTSA_UNREACHABLE();
            }
        }
    }
    return nullptr;
}

// 0x531C20
void CControllerConfigManager::DeleteMatching1rstPersonControls(eControllerAction nop, KeyCode button, eControllerType type) {
    if (GetIsKeyBlank(button, type)) {
        return;
    }

    CheckAndClear(PED_SNIPER_ZOOM_IN, type, button);
    CheckAndClear(PED_SNIPER_ZOOM_OUT, type, button);
}

// 0x531BC0
void CControllerConfigManager::DeleteMatchingVehicle_3rdPersonControls(KeyCode button, eControllerType type) {
    if (GetIsKeyBlank(button, type)) {
        return;
    }

    CheckAndClear(VEHICLE_ENTER_EXIT, type, button);
}

// 0x52FD70
void CControllerConfigManager::ClearSettingsAssociatedWithAction(eControllerAction action, eControllerType type) {
    switch (type) {
    case eControllerType::KEYBOARD:
    case eControllerType::OPTIONAL_EXTRA_KEY:
        m_Actions[+action].Keys[+type].m_uiActionInitiator = rsNULL;
        m_Actions[+action].Keys[+type].m_uiSetOrder        = eContSetOrder::NONE;
        break;
    case eControllerType::MOUSE:
    case eControllerType::JOY_STICK:
        m_Actions[+action].Keys[+type].m_uiActionInitiator = (RsKeyCodes)0;
        m_Actions[+action].Keys[+type].m_uiSetOrder        = eContSetOrder::NONE;
        break;
    default: NOTSA_UNREACHABLE();
    }
    ResetSettingOrder(action);
}

// unused
// 0x52FD20
void CControllerConfigManager::AffectControllerStateOn_ButtonDown_VehicleAndThirdPersonOnly(KeyCode button, eControllerType type, CControllerState& state) {
    CheckAndSetButton(VEHICLE_ENTER_EXIT, type, button, state.ButtonTriangle);
}

// 0x52FCA0
void CControllerConfigManager::AffectControllerStateOn_ButtonDown_AllStates(KeyCode button, eControllerType type, CControllerState& state) {
    CheckAndSetButton(CAMERA_CHANGE_VIEW_ALL_SITUATIONS, type, button, state.Select);
    CheckAndSetPad(CONVERSATION_NO, type, button, state.DPadRight, state.DPadLeft);
    CheckAndSetPad(CONVERSATION_YES, type, button, state.DPadLeft, state.DPadRight);
    CheckAndSetButton(NETWORK_TALK, type, button, state.m_bChatIndicated);
}

// 0x52F580
KeyCode CControllerConfigManager::GetMouseButtonAssociatedWithAction(eControllerAction action) {
    return m_Actions[+action].Keys[+eControllerType::MOUSE].m_uiActionInitiator;
}

// 0x52FAB0
void CControllerConfigManager::AffectControllerStateOn_ButtonDown_FirstAndThirdPersonOnly(KeyCode button, eControllerType type, CControllerState& state) {
    CheckAndSetButton(PED_FIRE_WEAPON, type, button, state.ButtonCircle);
    CheckAndSetButton(PED_FIRE_WEAPON_ALT, type, button, state.LeftShoulder1);
    CheckAndSetButton(PED_LOCK_TARGET, type, button, state.RightShoulder1);
    CheckAndSetStick(GO_FORWARD, type, button, state.LeftStickY, m_bStickL_Up_Dwn_MovementBothDown[+type], -128);
    CheckAndSetStick(GO_BACK, type, button, state.LeftStickY, m_bStickL_Up_Dwn_MovementBothDown[+type], 128);
    CheckAndSetStick(GO_LEFT, type, button, state.LeftStickX, m_bStickL_X_Rgh_Lft_MovementBothDown[+type], -128);
    CheckAndSetStick(GO_RIGHT, type, button, state.LeftStickX, m_bStickL_X_Rgh_Lft_MovementBothDown[+type], 128);
    CheckAndSetButton(PED_WALK, type, button, state.m_bPedWalk);
    CheckAndSetPad(GROUP_CONTROL_FWD, type, button, state.DPadDown, state.DPadUp);
    CheckAndSetPad(GROUP_CONTROL_BWD, type, button, state.DPadUp, state.DPadDown);
    CheckAndSetStick(PED_1RST_PERSON_LOOK_LEFT, type, button, state.RightStickX, m_bStickR_X_Rgh_Lft_MovementBothDown[+type], -128);
    CheckAndSetStick(PED_1RST_PERSON_LOOK_RIGHT, type, button, state.RightStickX, m_bStickR_X_Rgh_Lft_MovementBothDown[+type], 128);
    if (FrontEndMenuManager.m_ControlMethod == eController::JOYPAD) {
        CheckAndSetStick(PED_1RST_PERSON_LOOK_UP, type, button, state.RightStickY, m_bStickR_Up_Dwn_MovementBothDown[+type], 128);
        CheckAndSetStick(PED_1RST_PERSON_LOOK_DOWN, type, button, state.RightStickY, m_bStickR_Up_Dwn_MovementBothDown[+type], -128);
    }
}

// 0x52FA20
void CControllerConfigManager::AffectControllerStateOn_ButtonDown_ThirdPersonOnly(KeyCode button, eControllerType type, CControllerState& state) {
    CheckAndSetButton(PED_LOOKBEHIND, type, button, state.ShockButtonR);
    CheckAndSetButton(PED_JUMPING, type, button, state.ButtonSquare);
    CheckAndSetButton(PED_ANSWER_PHONE, type, button, state.LeftShoulder1);
    CheckAndSetButton(PED_CYCLE_WEAPON_LEFT, type, button, state.LeftShoulder2);
    CheckAndSetButton(PED_CYCLE_WEAPON_RIGHT, type, button, state.RightShoulder2);
    CheckAndSetButton(PED_SPRINT, type, button, state.ButtonCross);
    CheckAndSetButton(PED_DUCK, type, button, state.ShockButtonL);

    if (FrontEndMenuManager.m_ControlMethod == eController::JOYPAD) {
        CheckAndSetButton(PED_CENTER_CAMERA_BEHIND_PLAYER, type, button, state.LeftShoulder1);
    }
}

// 0x52F550
bool CControllerConfigManager::GetIsActionAButtonCombo(eControllerAction action) {
    /* NOT USED IN SA, but explains some things
    switch (action) {
    case VEHICLE_LOOK_BEHIND:
    // case PED_CYCLE_TARGET_LEFT:
    // case PED_CYCLE_TARGET_RIGHT:
        return true;
    }
    */
    return false;
}

// 0x52F4F0
KeyCode CControllerConfigManager::GetControllerKeyAssociatedWithAction(eControllerAction action, eControllerType type) {
    return m_Actions[+action].Keys[+type].m_uiActionInitiator;
}

// 0x52F9E0
void CControllerConfigManager::AffectControllerStateOn_ButtonDown_FirstPersonOnly(KeyCode button, eControllerType type, CControllerState& state) {
    CheckAndSetButton(PED_SNIPER_ZOOM_IN, type, button, state.ButtonSquare);
    CheckAndSetButton(PED_SNIPER_ZOOM_OUT, type, button, state.ButtonCross);
    CheckAndSetButton(PED_DUCK, type, button, state.ShockButtonL);
}

// 0x52FD40
void CControllerConfigManager::AffectControllerStateOn_ButtonUp_All_Player_States(KeyCode button, eControllerType type, CControllerState& state) {
    CheckAndReset(NETWORK_TALK, type, button, state.m_bChatIndicated);
    CheckAndReset(VEHICLE_MOUSELOOK, type, button, state.m_bVehicleMouseLook);
}

// 0x52F7B0
void CControllerConfigManager::AffectControllerStateOn_ButtonDown_Driving(KeyCode button, eControllerType type, CControllerState& state) {
    CheckAndSetButton(VEHICLE_FIRE_WEAPON, type, button, state.ButtonCircle);
    CheckAndSetButton(VEHICLE_FIRE_WEAPON_ALT, type, button, state.LeftShoulder1);

    // NOTE: original double, check if you press both buttons you look behind
    CheckAndSetButton(VEHICLE_LOOKBEHIND, type, button, state.LeftShoulder2);
    CheckAndSetButton(VEHICLE_LOOKBEHIND, type, button, state.RightShoulder2);

    CheckAndSetButton(VEHICLE_LOOKLEFT, type, button, state.LeftShoulder2);
    CheckAndSetButton(VEHICLE_LOOKRIGHT, type, button, state.RightShoulder2);
    CheckAndSetButton(VEHICLE_MOUSELOOK, type, button, state.m_bVehicleMouseLook);
    CheckAndSetButton(VEHICLE_HORN, type, button, state.ShockButtonL);
    CheckAndSetButton(VEHICLE_HANDBRAKE, type, button, state.RightShoulder1);
    CheckAndSetButton(VEHICLE_ACCELERATE, type, button, state.ButtonCross);
    CheckAndSetButton(VEHICLE_BRAKE, type, button, state.ButtonSquare);
    CheckAndSetStick(VEHICLE_STEERUP, type, button, state.LeftStickY, m_bStickL_Up_Dwn_MovementBothDown[+type], -128);
    CheckAndSetStick(VEHICLE_STEERDOWN, type, button, state.LeftStickY, m_bStickL_Up_Dwn_MovementBothDown[+type], 128);
    CheckAndSetStick(VEHICLE_STEERLEFT, type, button, state.LeftStickX, m_bStickL_X_Rgh_Lft_MovementBothDown[+type], -128);
    CheckAndSetStick(VEHICLE_STEERRIGHT, type, button, state.LeftStickX, m_bStickL_X_Rgh_Lft_MovementBothDown[+type], 128);
    CheckAndSetButton(VEHICLE_RADIO_STATION_UP, type, button, state.DPadUp);
    CheckAndSetButton(VEHICLE_RADIO_STATION_DOWN, type, button, state.DPadDown);
    CheckAndSetButton(VEHICLE_RADIO_TRACK_SKIP, type, button, state.m_bRadioTrackSkip);
    CheckAndSetButton(TOGGLE_SUBMISSIONS, type, button, state.ShockButtonR);
    CheckAndSetStick(VEHICLE_TURRETLEFT, type, button, state.RightStickX, m_bStickR_X_Rgh_Lft_MovementBothDown[+type], -128);
    CheckAndSetStick(VEHICLE_TURRETRIGHT, type, button, state.RightStickX, m_bStickR_X_Rgh_Lft_MovementBothDown[+type], 128);
    CheckAndSetStick(VEHICLE_TURRETUP, type, button, state.RightStickY, m_bStickR_Up_Dwn_MovementBothDown[+type], 128);
    CheckAndSetStick(VEHICLE_TURRETDOWN, type, button, state.RightStickY, m_bStickR_Up_Dwn_MovementBothDown[+type], -128);
}

// 0x52F5F0
void CControllerConfigManager::ResetSettingOrder(eControllerAction action) {
    eControllerType result = eControllerType::KEYBOARD;

    for (const auto& order : CONTROLLER_ORDERS_SET) {
        // Check if any key already has this priority level
        bool priorityExists = false;

        for (const auto& type : CONTROLLER_TYPES_ALL) {
            if (m_Actions[+action].Keys[+type].m_uiSetOrder == order) {
                priorityExists = true;
                break;
            }
        }

        if (!priorityExists) {
            // No key has this priority, find the one with lowest priority > priorityLevel
            bool found = false;

            for (const auto& type : CONTROLLER_TYPES_ALL) {
                const auto keyPriority = m_Actions[+action].Keys[+type].m_uiSetOrder;

                // Only consider keys with priority > priorityLevel and not 0 (unset)
                if (keyPriority > order && keyPriority != eContSetOrder::NONE) {
                    // If no key found yet or this key has lower priority than current best
                    if (!found || keyPriority < m_Actions[+action].Keys[+result].m_uiSetOrder) {
                        result = type;
                        found = true;
                    }
                }
            }

            // If we found a key with higher priority, adjust it to the current level
            if (found) {
                m_Actions[+action].Keys[+result].m_uiSetOrder = order;
            }
        }
    }
}

// 0x530530
bool CControllerConfigManager::LoadSettings(FILESTREAM file) {
    if (!file) {
        return true;
    }

    // Check if file has valid header
    char buffer[52] = {0};
    CFileMgr::Read(file, buffer, 29);
    if (!strncmp(buffer, "THIS FILE IS NOT VALID YET", 26)) {
        return true;
    }

    // Reset file position to start and read version
    CFileMgr::Seek(file, 0, 0);
    int32 version = 0;
    CFileMgr::Read(file, &version, 4);

    if (version < 6) {
        return true;
    }

    // Verify file format by checking action IDs
    auto actionId = 0u;
    for (const auto& type : CONTROLLER_TYPES_ALL) {
        for (auto action = 0; action < +CONTROLLER_ACTION_COUNT; action++) {
            CFileMgr::Read(file, &actionId, 4);
            if (actionId != action) {
                return false;
            }
            CFileMgr::Seek(file, 8, 1); // Skip key mapping data for validation pass
        }
    }

    // Go back to position after version info
    CFileMgr::Seek(file, 4, 0);

    // Clear existing settings
    MakeControllerActionsBlank();

    // Read key mappings for all controller types
    for (const auto& type : CONTROLLER_TYPES_ALL) {
        for (auto action = 0; action < +CONTROLLER_ACTION_COUNT; action++) {
            // Skip action ID
            CFileMgr::Seek(file, 4, 1);

            // Read key and order for this action
            CFileMgr::Read(file, &m_Actions[action].Keys[+type], 8);
        }
    }

    return true;
}

// 0x52D200
bool CControllerConfigManager::SaveSettings(FILESTREAM file) {
    if (!file) {
        return false;
    }

    for (auto type : CONTROLLER_TYPES_ALL) {
        for (auto action = 0; action < +CONTROLLER_ACTION_COUNT; action++) {
            // Write action ID
            CFileMgr::Write(file, &action, 4);

            // Write key mapping data
            CFileMgr::Write(file, &m_Actions[action].Keys[+type], 8);
        }
    }

    return true;
}

// 0x530640
void CControllerConfigManager::InitDefaultControlConfiguration() {
    SetControllerKeyAssociatedWithAction(GO_FORWARD, rsUP, eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(GO_BACK, rsDOWN, eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(GO_LEFT, rsLEFT, eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(GO_RIGHT, rsRIGHT, eControllerType::KEYBOARD);

    if (FrontEndMenuManager.m_nTextLanguage == 2) {
        SetControllerKeyAssociatedWithAction(CONVERSATION_YES, (RsKeyCodes)'Z', eControllerType::KEYBOARD);
        SetControllerKeyAssociatedWithAction(PED_SNIPER_ZOOM_IN, (RsKeyCodes)'Y', eControllerType::OPTIONAL_EXTRA_KEY);
    } else {
        SetControllerKeyAssociatedWithAction(CONVERSATION_YES, (RsKeyCodes)'Y', eControllerType::KEYBOARD);
        SetControllerKeyAssociatedWithAction(PED_SNIPER_ZOOM_IN, (RsKeyCodes)'X', eControllerType::OPTIONAL_EXTRA_KEY);
    }

    if (FrontEndMenuManager.m_nTextLanguage == 1) {
        SetControllerKeyAssociatedWithAction(GO_FORWARD, (RsKeyCodes)'Z', eControllerType::OPTIONAL_EXTRA_KEY);
        SetControllerKeyAssociatedWithAction(GO_LEFT, (RsKeyCodes)'Q', eControllerType::OPTIONAL_EXTRA_KEY);
        SetControllerKeyAssociatedWithAction(PED_CYCLE_WEAPON_LEFT, (RsKeyCodes)'A', eControllerType::KEYBOARD);
        SetControllerKeyAssociatedWithAction(PED_SNIPER_ZOOM_OUT, (RsKeyCodes)'W', eControllerType::OPTIONAL_EXTRA_KEY);
    } else {
        SetControllerKeyAssociatedWithAction(GO_FORWARD, (RsKeyCodes)'W', eControllerType::OPTIONAL_EXTRA_KEY);
        SetControllerKeyAssociatedWithAction(GO_LEFT, (RsKeyCodes)'A', eControllerType::OPTIONAL_EXTRA_KEY);
        SetControllerKeyAssociatedWithAction(PED_CYCLE_WEAPON_LEFT, (RsKeyCodes)'Q', eControllerType::KEYBOARD);
        SetControllerKeyAssociatedWithAction(PED_SNIPER_ZOOM_OUT, (RsKeyCodes)'Z', eControllerType::OPTIONAL_EXTRA_KEY);
    }

    SetControllerKeyAssociatedWithAction(GO_BACK, (RsKeyCodes)'S', eControllerType::OPTIONAL_EXTRA_KEY);
    SetControllerKeyAssociatedWithAction(GO_RIGHT, (RsKeyCodes)'D', eControllerType::OPTIONAL_EXTRA_KEY);
    SetControllerKeyAssociatedWithAction(PED_CYCLE_WEAPON_RIGHT, (RsKeyCodes)'E', eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(PED_CYCLE_WEAPON_RIGHT, rsPADENTER, eControllerType::OPTIONAL_EXTRA_KEY);
    SetControllerKeyAssociatedWithAction(PED_CYCLE_WEAPON_LEFT, rsPADDEL, eControllerType::OPTIONAL_EXTRA_KEY);
    SetControllerKeyAssociatedWithAction(PED_FIRE_WEAPON, rsPADINS, eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(PED_FIRE_WEAPON, rsLCTRL, eControllerType::OPTIONAL_EXTRA_KEY);
    SetControllerKeyAssociatedWithAction(PED_SNIPER_ZOOM_IN, rsPGUP, eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(PED_SNIPER_ZOOM_OUT, rsPGDN, eControllerType::KEYBOARD);
    // Fixes key mapping issue between old Windows 98 and modern systems
    SetControllerKeyAssociatedWithAction(PED_FIRE_WEAPON_ALT, RsKeyCodes(notsa::IsFixBugs() ? '0' : '\\'), eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(GROUP_CONTROL_FWD, (RsKeyCodes)'G', eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(GROUP_CONTROL_BWD, (RsKeyCodes)'H', eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(CONVERSATION_NO, (RsKeyCodes)'N', eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(VEHICLE_ENTER_EXIT, (RsKeyCodes)'F', eControllerType::OPTIONAL_EXTRA_KEY);
    SetControllerKeyAssociatedWithAction(VEHICLE_ENTER_EXIT, rsENTER, eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(CAMERA_CHANGE_VIEW_ALL_SITUATIONS, (RsKeyCodes)'V', eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(CAMERA_CHANGE_VIEW_ALL_SITUATIONS, rsHOME, eControllerType::OPTIONAL_EXTRA_KEY);
    SetControllerKeyAssociatedWithAction(PED_JUMPING, rsLSHIFT, eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(PED_JUMPING, rsRCTRL, eControllerType::OPTIONAL_EXTRA_KEY);
    SetControllerKeyAssociatedWithAction(PED_SPRINT, (RsKeyCodes)' ', eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(PED_DUCK, (RsKeyCodes)'C', eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(PED_ANSWER_PHONE, rsTAB, eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(PED_WALK, rsLALT, eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(PED_LOCK_TARGET, rsDEL, eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(PED_LOCK_TARGET, rsCAPSLK, eControllerType::OPTIONAL_EXTRA_KEY);
    SetControllerKeyAssociatedWithAction(PED_1RST_PERSON_LOOK_LEFT, rsPADLEFT, eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(PED_1RST_PERSON_LOOK_RIGHT, rsPADRIGHT, eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(PED_1RST_PERSON_LOOK_UP, rsPADDOWN, eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(PED_1RST_PERSON_LOOK_DOWN, rsPADUP, eControllerType::KEYBOARD);
    // Fixes key mapping issue between old Windows 98 and modern systems
    SetControllerKeyAssociatedWithAction(PED_CENTER_CAMERA_BEHIND_PLAYER, RsKeyCodes(notsa::IsFixBugs() ? '3' : '#'), eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(PED_LOOKBEHIND, rsPADEND, eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(VEHICLE_STEERUP, rsUP, eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(VEHICLE_STEERDOWN, rsDOWN, eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(VEHICLE_RADIO_STATION_UP, rsINS, eControllerType::KEYBOARD);

    if (FrontEndMenuManager.m_nTextLanguage == 1) {
        SetControllerKeyAssociatedWithAction(VEHICLE_STEERLEFT, (RsKeyCodes)'Q', eControllerType::KEYBOARD);
        SetControllerKeyAssociatedWithAction(VEHICLE_LOOKLEFT, (RsKeyCodes)'A', eControllerType::KEYBOARD);
        SetControllerKeyAssociatedWithAction(VEHICLE_RADIO_STATION_UP, (RsKeyCodes)'`', eControllerType::OPTIONAL_EXTRA_KEY);
        SetControllerKeyAssociatedWithAction(TOGGLE_SUBMISSIONS, (RsKeyCodes)233, eControllerType::KEYBOARD);
        SetControllerKeyAssociatedWithAction(VEHICLE_ACCELERATE, (RsKeyCodes)'Z', eControllerType::KEYBOARD);
    } else {
        SetControllerKeyAssociatedWithAction(VEHICLE_STEERLEFT, (RsKeyCodes)'A', eControllerType::KEYBOARD);
        SetControllerKeyAssociatedWithAction(VEHICLE_LOOKLEFT, (RsKeyCodes)'Q', eControllerType::KEYBOARD);
        SetControllerKeyAssociatedWithAction(VEHICLE_RADIO_STATION_UP, (RsKeyCodes)'4', eControllerType::OPTIONAL_EXTRA_KEY);
        SetControllerKeyAssociatedWithAction(TOGGLE_SUBMISSIONS, (RsKeyCodes)'2', eControllerType::KEYBOARD);
        SetControllerKeyAssociatedWithAction(VEHICLE_ACCELERATE, (RsKeyCodes)'W', eControllerType::KEYBOARD);
    }

    SetControllerKeyAssociatedWithAction(TOGGLE_SUBMISSIONS, rsPLUS, eControllerType::OPTIONAL_EXTRA_KEY);
    SetControllerKeyAssociatedWithAction(VEHICLE_FIRE_WEAPON, rsRCTRL, eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(VEHICLE_FIRE_WEAPON, rsLALT, eControllerType::OPTIONAL_EXTRA_KEY);
    SetControllerKeyAssociatedWithAction(VEHICLE_FIRE_WEAPON_ALT, rsLCTRL, eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(VEHICLE_FIRE_WEAPON_ALT, rsPADINS, eControllerType::OPTIONAL_EXTRA_KEY);
    SetControllerKeyAssociatedWithAction(VEHICLE_BRAKE, (RsKeyCodes)'S', eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(VEHICLE_STEERRIGHT, (RsKeyCodes)'D', eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(VEHICLE_STEERLEFT, rsLEFT, eControllerType::OPTIONAL_EXTRA_KEY);
    SetControllerKeyAssociatedWithAction(VEHICLE_STEERRIGHT, rsRIGHT, eControllerType::OPTIONAL_EXTRA_KEY);
    SetControllerKeyAssociatedWithAction(VEHICLE_HORN, rsCAPSLK, eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(VEHICLE_HORN, (RsKeyCodes)'H', eControllerType::OPTIONAL_EXTRA_KEY);
    SetControllerKeyAssociatedWithAction(VEHICLE_HANDBRAKE, (RsKeyCodes)' ', eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(VEHICLE_HANDBRAKE, rsRCTRL, eControllerType::OPTIONAL_EXTRA_KEY);
    SetControllerKeyAssociatedWithAction(VEHICLE_LOOKRIGHT, (RsKeyCodes)'E', eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(VEHICLE_TURRETLEFT, rsPADLEFT, eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(VEHICLE_TURRETRIGHT, rsPADRIGHT, eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(VEHICLE_TURRETUP, rsPADDOWN, eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(VEHICLE_TURRETDOWN, rsPADUP, eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(VEHICLE_TURRETUP, rsEND, eControllerType::OPTIONAL_EXTRA_KEY);
    SetControllerKeyAssociatedWithAction(VEHICLE_TURRETDOWN, rsDEL, eControllerType::OPTIONAL_EXTRA_KEY);
    SetControllerKeyAssociatedWithAction(VEHICLE_RADIO_TRACK_SKIP, rsF5, eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(VEHICLE_RADIO_STATION_DOWN, rsDEL, eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(VEHICLE_RADIO_STATION_DOWN, (RsKeyCodes)'R', eControllerType::OPTIONAL_EXTRA_KEY);
    SetControllerKeyAssociatedWithAction(PED_CYCLE_TARGET_LEFT, (RsKeyCodes)'[', eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(PED_CYCLE_TARGET_RIGHT, (RsKeyCodes)']', eControllerType::KEYBOARD);

    // NOTE: If by chance there is no mouse the key assignment will fail, keep this.
    if (notsa::IsFixBugs()) {
        SetControllerKeyAssociatedWithAction(VEHICLE_LOOKBEHIND, rsPAD5, eControllerType::KEYBOARD);
        SetControllerKeyAssociatedWithAction(VEHICLE_MOUSELOOK, '*', eControllerType::KEYBOARD);
    }

    ClearSimButtonPressCheckers();
}

// 0x530B00
void CControllerConfigManager::InitDefaultControlConfigJoyPad(uint32 buttonCount) {
    m_bJoyJustInitialised = true;

    buttonCount = std::min<uint32>(buttonCount, JOYBUTTON_COUNT - 1);

    // Define all possible button mappings in order from highest to lowest button number
    using ButtonMapping = std::pair<eJoyButtons, eControllerAction>;

    // Arrays for specific and standard controller configurations
    constexpr ButtonMapping specificMappings[29] = {
        { JOYBUTTON_SIXTEEN,   CONVERSATION_NO                   },
        { JOYBUTTON_FIFTHTEEN, GROUP_CONTROL_BWD                 },
        { JOYBUTTON_FIFTHTEEN, VEHICLE_RADIO_STATION_DOWN        },
        { JOYBUTTON_FOURTEEN,  CONVERSATION_YES                  },
        { JOYBUTTON_THIRTEEN,  GROUP_CONTROL_FWD                 },
        { JOYBUTTON_THIRTEEN,  VEHICLE_RADIO_STATION_UP          },
        { JOYBUTTON_ELEVEN,    PED_LOOKBEHIND                    },
        { JOYBUTTON_ELEVEN,    TOGGLE_SUBMISSIONS                },
        { JOYBUTTON_TEN,       VEHICLE_HORN                      },
        { JOYBUTTON_TEN,       PED_DUCK                          },
        { JOYBUTTON_NINE,      CAMERA_CHANGE_VIEW_ALL_SITUATIONS },
        { JOYBUTTON_EIGHT,     VEHICLE_HANDBRAKE                 },
        { JOYBUTTON_EIGHT,     PED_LOCK_TARGET                   },
        { JOYBUTTON_SEVEN,     PED_ANSWER_PHONE                  },
        { JOYBUTTON_SEVEN,     PED_FIRE_WEAPON_ALT               },
        { JOYBUTTON_SEVEN,     VEHICLE_FIRE_WEAPON_ALT           },
        { JOYBUTTON_SIX,       PED_CYCLE_WEAPON_RIGHT            },
        { JOYBUTTON_SIX,       VEHICLE_LOOKRIGHT                 },
        { JOYBUTTON_FIVE,      PED_CYCLE_WEAPON_LEFT             },
        { JOYBUTTON_FIVE,      VEHICLE_LOOKLEFT                  },
        { JOYBUTTON_FOUR,      VEHICLE_BRAKE                     },
        { JOYBUTTON_FOUR,      PED_JUMPING                       },
        { JOYBUTTON_FOUR,      PED_SNIPER_ZOOM_IN                },
        { JOYBUTTON_THREE,     VEHICLE_ACCELERATE                },
        { JOYBUTTON_THREE,     PED_SPRINT                        },
        { JOYBUTTON_THREE,     PED_SNIPER_ZOOM_OUT               },
        { JOYBUTTON_TWO,       PED_FIRE_WEAPON                   },
        { JOYBUTTON_TWO,       VEHICLE_FIRE_WEAPON               },
        { JOYBUTTON_ONE,       VEHICLE_ENTER_EXIT                }
    };

    constexpr ButtonMapping standardMappings[29] = {
        { JOYBUTTON_SIXTEEN,   CONVERSATION_NO                   },
        { JOYBUTTON_FIFTHTEEN, GROUP_CONTROL_BWD                 },
        { JOYBUTTON_FIFTHTEEN, VEHICLE_RADIO_STATION_DOWN        },
        { JOYBUTTON_FOURTEEN,  CONVERSATION_YES                  },
        { JOYBUTTON_THIRTEEN,  GROUP_CONTROL_FWD                 },
        { JOYBUTTON_THIRTEEN,  VEHICLE_RADIO_STATION_UP          },
        { JOYBUTTON_ELEVEN,    PED_LOOKBEHIND                    },
        { JOYBUTTON_ELEVEN,    TOGGLE_SUBMISSIONS                },
        { JOYBUTTON_TEN,       VEHICLE_HORN                      },
        { JOYBUTTON_TEN,       PED_DUCK                          },
        { JOYBUTTON_NINE,      CAMERA_CHANGE_VIEW_ALL_SITUATIONS },
        { JOYBUTTON_EIGHT,     VEHICLE_HANDBRAKE                 },
        { JOYBUTTON_EIGHT,     PED_LOCK_TARGET                   },
        { JOYBUTTON_SEVEN,     PED_ANSWER_PHONE                  },
        { JOYBUTTON_SEVEN,     PED_FIRE_WEAPON_ALT               },
        { JOYBUTTON_SEVEN,     VEHICLE_FIRE_WEAPON_ALT           },
        { JOYBUTTON_SIX,       PED_CYCLE_WEAPON_RIGHT            },
        { JOYBUTTON_SIX,       VEHICLE_LOOKRIGHT                 },
        { JOYBUTTON_FIVE,      PED_CYCLE_WEAPON_LEFT             },
        { JOYBUTTON_FIVE,      VEHICLE_LOOKLEFT                  },
        { JOYBUTTON_FOUR,      VEHICLE_ENTER_EXIT                },
        { JOYBUTTON_THREE,     VEHICLE_BRAKE                     },
        { JOYBUTTON_THREE,     PED_JUMPING                       },
        { JOYBUTTON_THREE,     PED_SNIPER_ZOOM_IN                },
        { JOYBUTTON_TWO,       VEHICLE_ACCELERATE                },
        { JOYBUTTON_TWO,       PED_SPRINT                        },
        { JOYBUTTON_TWO,       PED_SNIPER_ZOOM_OUT               },
        { JOYBUTTON_ONE,       PED_FIRE_WEAPON                   },
        { JOYBUTTON_ONE,       VEHICLE_FIRE_WEAPON               }
    };

    // Choose which mapping array to use
    const auto& mappings = AllValidWinJoys.JoyStickNum[PAD1].wVendorID == 0x3427 && AllValidWinJoys.JoyStickNum[PAD1].wProductID == 0x1190
        ? specificMappings
        : standardMappings;

    for (size_t i = 0; i < std::size(mappings); ++i) {
        if (mappings[i].first <= (eJoyButtons)buttonCount) {
            SetControllerKeyAssociatedWithAction(mappings[i].second, (RsKeyCodes)mappings[i].first, eControllerType::JOY_STICK);
        }
    }
}

// 0x52F6F0
void CControllerConfigManager::InitDefaultControlConfigMouse(const CMouseControllerState& MouseSetUp, bool bMouseControls) {
#ifdef NOTSA_USE_SDL3
    constexpr bool isForcedMouseBlinding = true;
#else
    constexpr bool isForcedMouseBlinding = false;
#endif

    m_MouseFoundInitSet = false;
    if (MouseSetUp.isMouseLeftButtonPressed || isForcedMouseBlinding) {
        m_MouseFoundInitSet = true;
        SetMouseButtonAssociatedWithAction(PED_FIRE_WEAPON,            rsMOUSE_LEFT_BUTTON);
        SetMouseButtonAssociatedWithAction(VEHICLE_FIRE_WEAPON,        rsMOUSE_LEFT_BUTTON);
    }
    if (MouseSetUp.isMouseRightButtonPressed || isForcedMouseBlinding) {
        SetMouseButtonAssociatedWithAction(PED_LOCK_TARGET,            rsMOUSE_RIGHT_BUTTON);
        SetMouseButtonAssociatedWithAction(VEHICLE_MOUSELOOK,          rsMOUSE_RIGHT_BUTTON);
    }
    if (MouseSetUp.isMouseMiddleButtonPressed || isForcedMouseBlinding) {
        SetMouseButtonAssociatedWithAction(VEHICLE_LOOKBEHIND,         rsMOUSE_MIDDLE_BUTTON);
        SetMouseButtonAssociatedWithAction(PED_LOOKBEHIND,             rsMOUSE_MIDDLE_BUTTON);
    }
    if (MouseSetUp.isMouseWheelMovedUp || MouseSetUp.isMouseWheelMovedDown || isForcedMouseBlinding) {
        SetMouseButtonAssociatedWithAction(PED_CYCLE_WEAPON_LEFT,      rsMOUSE_WHEEL_UP_BUTTON);
        SetMouseButtonAssociatedWithAction(PED_CYCLE_WEAPON_RIGHT,     rsMOUSE_WHEEL_DOWN_BUTTON);
        SetMouseButtonAssociatedWithAction(VEHICLE_RADIO_STATION_UP,   rsMOUSE_WHEEL_UP_BUTTON);
        SetMouseButtonAssociatedWithAction(VEHICLE_RADIO_STATION_DOWN, rsMOUSE_WHEEL_DOWN_BUTTON);
        SetMouseButtonAssociatedWithAction(PED_SNIPER_ZOOM_IN,         rsMOUSE_WHEEL_UP_BUTTON);
        SetMouseButtonAssociatedWithAction(PED_SNIPER_ZOOM_OUT,        rsMOUSE_WHEEL_DOWN_BUTTON);
    }

    // This assert maybe is in the original game, but probably is missing by release build.
    // Prevents 'wrong' vehicle keys init. In cases where the mouse starts incorrectly.
    assert(m_MouseFoundInitSet == bMouseControls || !bMouseControls);
}

// 0x52D260
void CControllerConfigManager::InitialiseControllerActionNameArray() {
    GxtCharStrcpy(m_ControllerActionName[+PED_LOOKBEHIND], "PED_LOOKBEHIND"_gxt);
    GxtCharStrcpy(m_ControllerActionName[+PED_CYCLE_WEAPON_LEFT], "PED_CYCLE_WEAPON_LEFT"_gxt);
    GxtCharStrcpy(m_ControllerActionName[+PED_CYCLE_WEAPON_RIGHT], "PED_CYCLE_WEAPON_RIGHT"_gxt);
    GxtCharStrcpy(m_ControllerActionName[+PED_LOCK_TARGET], "PED_LOCK_TARGET"_gxt);
    GxtCharStrcpy(m_ControllerActionName[+PED_JUMPING], "PED_JUMPING"_gxt);
    GxtCharStrcpy(m_ControllerActionName[+PED_SPRINT], "PED_SPRINT"_gxt);
    GxtCharStrcpy(m_ControllerActionName[+PED_WALK], "SNEAK_ABOUT"_gxt);
    GxtCharStrcpy(m_ControllerActionName[+PED_CYCLE_TARGET_LEFT], "PED_CYCLE_TARGET_LEFT"_gxt);
    GxtCharStrcpy(m_ControllerActionName[+PED_CYCLE_TARGET_RIGHT], "PED_CYCLE_TARGET_RIGHT"_gxt);
    GxtCharStrcpy(m_ControllerActionName[+PED_CENTER_CAMERA_BEHIND_PLAYER], "PED_CENTER_CAMERA_BEHIND_PLAYER"_gxt);
    GxtCharStrcpy(m_ControllerActionName[+VEHICLE_LOOKBEHIND], "VEHICLE_LOOKBEHIND"_gxt);
    GxtCharStrcpy(m_ControllerActionName[+PED_DUCK], "PED_DUCK"_gxt);
    GxtCharStrcpy(m_ControllerActionName[+PED_ANSWER_PHONE], "PED_ANSWER_PHONE"_gxt);
    GxtCharStrcpy(m_ControllerActionName[+VEHICLE_STEERLEFT], "VEHICLE_STEERLEFT"_gxt);
    GxtCharStrcpy(m_ControllerActionName[+VEHICLE_STEERRIGHT], "VEHICLE_STEERRIGHT"_gxt);
    GxtCharStrcpy(m_ControllerActionName[+VEHICLE_STEERUP], "VEHICLE_STEERUP"_gxt);
    GxtCharStrcpy(m_ControllerActionName[+VEHICLE_STEERDOWN], "VEHICLE_STEERDOWN"_gxt);
    GxtCharStrcpy(m_ControllerActionName[+VEHICLE_LOOKLEFT], "VEHICLE_LOOKLEFT"_gxt);
    GxtCharStrcpy(m_ControllerActionName[+VEHICLE_LOOKRIGHT], "VEHICLE_LOOKRIGHT"_gxt);
    GxtCharStrcpy(m_ControllerActionName[+VEHICLE_MOUSELOOK], "VEHICLE_MOUSELOOK"_gxt);
    GxtCharStrcpy(m_ControllerActionName[+VEHICLE_HORN], "VEHICLE_HORN"_gxt);
    GxtCharStrcpy(m_ControllerActionName[+VEHICLE_HANDBRAKE], "VEHICLE_HANDBRAKE"_gxt);
    GxtCharStrcpy(m_ControllerActionName[+VEHICLE_ACCELERATE], "VEHICLE_ACCELERATE"_gxt);
    GxtCharStrcpy(m_ControllerActionName[+VEHICLE_BRAKE], "VEHICLE_BRAKE"_gxt);
    GxtCharStrcpy(m_ControllerActionName[+VEHICLE_RADIO_STATION_UP], "VEHICLE_RADIO_STATION_UP"_gxt);
    GxtCharStrcpy(m_ControllerActionName[+VEHICLE_RADIO_STATION_DOWN], "VEHICLE_RADIO_STATION_DOWN"_gxt);
    if (notsa::IsFixBugs()) { // Fix: Missing on vanilla game
        GxtCharStrcpy(m_ControllerActionName[+VEHICLE_RADIO_TRACK_SKIP], "VEHICLE_RADIO_TRACK_SKIP"_gxt);
    }
    GxtCharStrcpy(m_ControllerActionName[+TOGGLE_SUBMISSIONS], "TOGGLE_SUBMISSIONS"_gxt);
    GxtCharStrcpy(m_ControllerActionName[+PED_SNIPER_ZOOM_IN], "PED_SNIPER_ZOOM_IN"_gxt);
    GxtCharStrcpy(m_ControllerActionName[+PED_SNIPER_ZOOM_OUT], "PED_SNIPER_ZOOM_OUT"_gxt);
    GxtCharStrcpy(m_ControllerActionName[+PED_1RST_PERSON_LOOK_LEFT], "PED_1RST_PERSON_LOOK_LEFT"_gxt);
    GxtCharStrcpy(m_ControllerActionName[+PED_1RST_PERSON_LOOK_RIGHT], "PED_1RST_PERSON_LOOK_RIGHT"_gxt);
    GxtCharStrcpy(m_ControllerActionName[+PED_1RST_PERSON_LOOK_UP], "PED_1RST_PERSON_LOOK_UP"_gxt);
    GxtCharStrcpy(m_ControllerActionName[+PED_1RST_PERSON_LOOK_DOWN], "PED_1RST_PERSON_LOOK_DOWN"_gxt);
    GxtCharStrcpy(m_ControllerActionName[+SHOW_MOUSE_POINTER_TOGGLE], "SHOW_MOUSE_POINTER_TOGGLE"_gxt);
    GxtCharStrcpy(m_ControllerActionName[+CAMERA_CHANGE_VIEW_ALL_SITUATIONS], "CAMERA_CHANGE_VIEW_ALL_SITUATIONS"_gxt);
    GxtCharStrcpy(m_ControllerActionName[+PED_FIRE_WEAPON], "PED_FIREWEAPON"_gxt);
    GxtCharStrcpy(m_ControllerActionName[+PED_FIRE_WEAPON_ALT], "PED_FIREWEAPON_ALT"_gxt);
    GxtCharStrcpy(m_ControllerActionName[+VEHICLE_FIRE_WEAPON], "VEHICLE_FIREWEAPON"_gxt);
    GxtCharStrcpy(m_ControllerActionName[+VEHICLE_FIRE_WEAPON_ALT], "VEHICLE_FIREWEAPON_ALT"_gxt);
    GxtCharStrcpy(m_ControllerActionName[+VEHICLE_ENTER_EXIT], "VEHICLE_ENTER_EXIT"_gxt);
    GxtCharStrcpy(m_ControllerActionName[+CONVERSATION_NO], "CONVERSATION_NO"_gxt);
    GxtCharStrcpy(m_ControllerActionName[+CONVERSATION_YES], "CONVERSATION_YES"_gxt);
    GxtCharStrcpy(m_ControllerActionName[+GROUP_CONTROL_FWD], "GROUP_CONTROL_FWD"_gxt);
    GxtCharStrcpy(m_ControllerActionName[+GROUP_CONTROL_BWD], "GROUP_CONTROL_BWD"_gxt);
    GxtCharStrcpy(m_ControllerActionName[+GO_LEFT], "GO_LEFT"_gxt);
    GxtCharStrcpy(m_ControllerActionName[+GO_RIGHT], "GO_RIGHT"_gxt);
    GxtCharStrcpy(m_ControllerActionName[+GO_FORWARD], "GO_FORWARD"_gxt);
    GxtCharStrcpy(m_ControllerActionName[+GO_BACK], "GO_BACK"_gxt);
    GxtCharStrcpy(m_ControllerActionName[+VEHICLE_TURRETLEFT], "VEHICLE_TURRETLEFT"_gxt);
    GxtCharStrcpy(m_ControllerActionName[+VEHICLE_TURRETRIGHT], "VEHICLE_TURRETRIGHT"_gxt);
    GxtCharStrcpy(m_ControllerActionName[+VEHICLE_TURRETUP], "VEHICLE_TURRETUP"_gxt);
    GxtCharStrcpy(m_ControllerActionName[+VEHICLE_TURRETDOWN], "VEHICLE_TURRETDOWN"_gxt);
    GxtCharStrcpy(m_ControllerActionName[+NETWORK_TALK], "NETWORK_TALK"_gxt);
    GxtCharStrcpy(m_ControllerActionName[+TOGGLE_DPAD], "TOGGLE_DPAD"_gxt);
    GxtCharStrcpy(m_ControllerActionName[+SWITCH_DEBUG_CAM_ON], "SWITCH_DEBUG_CAM_ON"_gxt);
    GxtCharStrcpy(m_ControllerActionName[+TAKE_SCREEN_SHOT], "TAKE_SCREEN_SHOT"_gxt);
}

// 0x531F20
void CControllerConfigManager::ReinitControls() {
    ControlsManager.MakeControllerActionsBlank();
    ControlsManager.InitDefaultControlConfiguration();
#ifdef NOTSA_USE_SDL3
    const auto MouseSetUp = CMouseControllerState{};
#else
    const auto MouseSetUp = WinInput::GetMouseSetUp();
#endif
    ControlsManager.InitDefaultControlConfigMouse(MouseSetUp, FrontEndMenuManager.m_ControlMethod == eController::MOUSE_PLUS_KEYS);

    if (AllValidWinJoys.JoyStickNum[PAD1].bJoyAttachedToPort) {
#ifdef NOTSA_USE_SDL3
        ControlsManager.InitDefaultControlConfigJoyPad(JOYBUTTON_COUNT - 1);
#else
        DIDEVCAPS devCaps;
        devCaps.dwSize = sizeof(DIDEVCAPS);
        if (!FAILED(PSGLOBAL(diDevice1)->GetCapabilities(&devCaps))) {
            ControlsManager.InitDefaultControlConfigJoyPad(devCaps.dwButtons);
        }
#endif
    }
}

// 0x52F590
void CControllerConfigManager::SetMouseButtonAssociatedWithAction(eControllerAction action, KeyCode button) {
    ResetSettingOrder(action);
    const auto numOfSettings = GetNumOfSettingsForAction(action);
    m_Actions[+action].Keys[+eControllerType::MOUSE].m_uiActionInitiator = button;
    m_Actions[+action].Keys[+eControllerType::MOUSE].m_uiSetOrder = eContSetOrder(+numOfSettings + 1);
}

// unused
// 0x52DA30
void CControllerConfigManager::StoreMouseButtonState(RsKeyCodes button, bool state) {
    switch (button) {
    case rsMOUSE_LEFT_BUTTON:       CPad::TempMouseControllerState.isMouseLeftButtonPressed = state; break;
    case rsMOUSE_MIDDLE_BUTTON:     CPad::TempMouseControllerState.isMouseMiddleButtonPressed = state; break;
    case rsMOUSE_RIGHT_BUTTON:      CPad::TempMouseControllerState.isMouseRightButtonPressed = state; break;
    case rsMOUSE_WHEEL_UP_BUTTON:   CPad::TempMouseControllerState.isMouseWheelMovedUp = state; break;
    case rsMOUSE_WHEEL_DOWN_BUTTON: CPad::TempMouseControllerState.isMouseWheelMovedDown = state; break;
    case rsMOUSE_X1_BUTTON:         CPad::TempMouseControllerState.isMouseFirstXPressed = state; break;
    case rsMOUSE_X2_BUTTON:         CPad::TempMouseControllerState.isMouseSecondXPressed = state; break;
    default:                        NOTSA_UNREACHABLE();
    }
}

// 0x530ED0
void CControllerConfigManager::UpdateJoy_ButtonDown(KeyCode button, eControllerType type) {
    if (GetIsKeyBlank(button, type)) {
        return;
    }

    AffectControllerStateOn_ButtonDown(button, type);
}

// unused
// 0x52DC10
void CControllerConfigManager::AffectControllerStateOn_ButtonDown_DebugStuff(KeyCode button, eControllerType type) {
    // NOP
    return;
}

// 0x52DAB0
void CControllerConfigManager::UpdateJoyInConfigMenus_ButtonDown(KeyCode button, int32 padNumber) {
    CPad* pad = CPad::GetPad(padNumber);
    if (!pad || !button) {
        return;
    }

    if ((button != eJoyButtons::JOYBUTTON_TWELVE && padNumber != 1) || notsa::IsFixBugs()) {
        GetControllerStateJoyStick(*pad, button) = 255;
    }
}

// 0x52DC20
void CControllerConfigManager::UpdateJoyInConfigMenus_ButtonUp(KeyCode button, int32 padNumber) {
    CPad* pad = CPad::GetPad(padNumber);
    if (!pad || !button) {
        return;
    }

    if ((button != eJoyButtons::JOYBUTTON_TWELVE && padNumber != 1) || notsa::IsFixBugs()) {
        GetControllerStateJoyStick(*pad, button) = 0;
    }
}

// 0x531070
void CControllerConfigManager::AffectControllerStateOn_ButtonUp(KeyCode button, eControllerType type) {
    const auto pad = CPad::GetPad();
    if (!GetIsKeyBlank(button, type) && pad && !FrontEndMenuManager.m_bMenuActive) {
        AffectControllerStateOn_ButtonUp_All_Player_States(button, type, GetControllerState(*pad, type));
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
    for (const auto& type : CONTROLLER_TYPES_ALL) {
        m_bStickL_Up_Dwn_MovementBothDown[+type]    = false;
        m_bStickL_X_Rgh_Lft_MovementBothDown[+type] = false;
        m_bStickR_Up_Dwn_MovementBothDown[+type]    = false;
        m_bStickR_X_Rgh_Lft_MovementBothDown[+type] = false;
    }
}

// unused
// 0x52D1C0
eJoyButtons CControllerConfigManager::GetJoyButtonJustUp() {
    // Check each button from 0 to JOYBUTTON_SIXTEEN-1
    for (int32 buttonIndex = eJoyButtons::NO_JOYBUTTONS; buttonIndex < eJoyButtons::JOYBUTTON_SIXTEEN; buttonIndex++) {
        // Check if button is released in current state but was pressed in previous state
        const bool isCurrentlyPressed = (m_NewJoyState.rgbButtons[buttonIndex] & 0x80) != 0;
        const bool wasPreviouslyPressed = (m_OldJoyState.rgbButtons[buttonIndex] & 0x80) != 0;

        if (!isCurrentlyPressed && wasPreviouslyPressed) {
            // Return the button ID (1-based index)
            return (eJoyButtons)(buttonIndex + 1);
        }
    }
    // No button was found that was just released
    return eJoyButtons::NO_JOYBUTTONS;
}

// 0x52D1E0
eJoyButtons CControllerConfigManager::GetJoyButtonJustDown() {
    // Check each button from 0 to JOYBUTTON_SIXTEEN-1
    for (int32 buttonIndex = eJoyButtons::NO_JOYBUTTONS; buttonIndex < eJoyButtons::JOYBUTTON_SIXTEEN; buttonIndex++) {
        // Check if button is pressed in current state but wasn't pressed in previous state
        const bool isCurrentlyPressed = (m_NewJoyState.rgbButtons[buttonIndex] & 0x80) != 0;
        const bool wasPreviouslyPressed = (m_OldJoyState.rgbButtons[buttonIndex] & 0x80) != 0;

        if (isCurrentlyPressed && !wasPreviouslyPressed) {
            // Return the button ID (1-based index)
            return (eJoyButtons)(buttonIndex + 1);
        }
    }
    // No button was found that was just pressed
    return eJoyButtons::NO_JOYBUTTONS;
}

// 0x52DDB0
bool CControllerConfigManager::GetIsKeyboardKeyDown(KeyCode key) {
    return IsKeyboardKeyDownInState(CPad::NewKeyState, key);
}

// 0x52E450
bool CControllerConfigManager::GetIsKeyboardKeyJustDown(KeyCode key) {
    return IsKeyboardKeyDownInState(CPad::NewKeyState, key) && !IsKeyboardKeyDownInState(CPad::OldKeyState, key);
}

// Enum to specify what type of mouse check to perform
enum class eMouseCheckType {
    IS_DOWN, // Button is currently down
    IS_UP,   // Button is currently up
    JUST_UP  // Button was just released this frame
};

// NOTSA: But at the moment of compile is 1:1.
template<eMouseCheckType CheckType>
constexpr inline bool CheckMouseButton(KeyCode& key) {
    auto* pad = CPad::GetPad();
    if (!pad || !key) {
        return false;
    }

    bool result = false;
    if constexpr (CheckType == eMouseCheckType::JUST_UP) {
        switch (key) {
        case rsMOUSE_LEFT_BUTTON:       result = pad->IsMouseLButtonPressed(); break;
        case rsMOUSE_MIDDLE_BUTTON:     result = pad->IsMouseMButtonPressed(); break;
        case rsMOUSE_RIGHT_BUTTON:      result = pad->IsMouseRButtonPressed(); break;
        case rsMOUSE_WHEEL_UP_BUTTON:   result = pad->IsMouseWheelUpPressed(); break;
        case rsMOUSE_WHEEL_DOWN_BUTTON: result = pad->IsMouseWheelDownPressed(); break;
        case rsMOUSE_X1_BUTTON:         result = pad->IsMouseBmx1Pressed(); break;
        case rsMOUSE_X2_BUTTON:         result = pad->IsMouseBmx2Pressed(); break;
        default:                        NOTSA_UNREACHABLE("Invalid Key: {}", (uint32)key); break;
        }
    } else if constexpr (CheckType == eMouseCheckType::IS_DOWN || CheckType == eMouseCheckType::IS_UP) {
        switch (key) {
        case rsMOUSE_LEFT_BUTTON:       result = pad->IsMouseLButton(); break;
        case rsMOUSE_MIDDLE_BUTTON:     result = pad->IsMouseMButton(); break;
        case rsMOUSE_RIGHT_BUTTON:      result = pad->IsMouseRButton(); break;
        case rsMOUSE_WHEEL_UP_BUTTON:   result = pad->IsMouseWheelUp(); break;
        case rsMOUSE_WHEEL_DOWN_BUTTON: result = pad->IsMouseWheelDown(); break;
        case rsMOUSE_X1_BUTTON:         result = pad->IsMouseBmx1(); break;
        case rsMOUSE_X2_BUTTON:         result = pad->IsMouseBmx2(); break;
        default:                        NOTSA_UNREACHABLE("Invalid Key: {}", (uint32)key); break;
        }
    }

    if constexpr (CheckType == eMouseCheckType::IS_UP) {
        return !result;
    } else {
        return result;
    }
    NOTSA_UNREACHABLE("Invalid CheckType: {}", (uint32)CheckType); // Unreachable
}

// 0x52EF30
bool CControllerConfigManager::GetIsMouseButtonDown(KeyCode Key) {
    return CheckMouseButton<eMouseCheckType::IS_DOWN>(Key);
}

// 0x52F020
bool CControllerConfigManager::GetIsMouseButtonUp(KeyCode Key) {
    return CheckMouseButton<eMouseCheckType::IS_UP>(Key);
}

// 0x52F110
bool CControllerConfigManager::GetIsMouseButtonJustUp(KeyCode Key) {
    return CheckMouseButton<eMouseCheckType::JUST_UP>(Key);
}

// unused
// 0x52F2A0
bool CControllerConfigManager::GetIsKeyBlank(KeyCode key, eControllerType type) {
    switch (type) {
    case eControllerType::KEYBOARD:
    case eControllerType::OPTIONAL_EXTRA_KEY: return key == rsNULL;
    case eControllerType::MOUSE:
    case eControllerType::JOY_STICK:          return key == 0;
    default:                                  NOTSA_UNREACHABLE();
    }
}

// 0x52F2F0
eActionType CControllerConfigManager::GetActionType(eControllerAction action) {
    switch (action) {
    case PED_FIRE_WEAPON:
    case PED_FIRE_WEAPON_ALT:
    case GO_FORWARD:
    case GO_BACK:
    case GO_LEFT:
    case GO_RIGHT:
    case PED_SNIPER_ZOOM_IN:
    case PED_SNIPER_ZOOM_OUT:
    case PED_1RST_PERSON_LOOK_LEFT:
    case PED_1RST_PERSON_LOOK_RIGHT:
    case PED_LOCK_TARGET:
    case PED_1RST_PERSON_LOOK_UP:
    case PED_1RST_PERSON_LOOK_DOWN:
        return eActionType::FIRST_THIRD_PERSON;
    case PED_CYCLE_WEAPON_RIGHT:
    case PED_CYCLE_WEAPON_LEFT:
    case PED_JUMPING:
    case PED_SPRINT:
    case PED_LOOKBEHIND:
    case PED_DUCK:
    case PED_ANSWER_PHONE:
    case PED_WALK:
    case PED_CYCLE_TARGET_LEFT:
    case PED_CYCLE_TARGET_RIGHT:
    case PED_CENTER_CAMERA_BEHIND_PLAYER:
    case CONVERSATION_YES:
    case CONVERSATION_NO:
    case GROUP_CONTROL_FWD:
    case GROUP_CONTROL_BWD:
        return eActionType::THIRD_PERSON;
    case VEHICLE_ENTER_EXIT:
        return eActionType::IN_CAR_THIRD_PERSON;
    case CAMERA_CHANGE_VIEW_ALL_SITUATIONS:
    case NETWORK_TALK:
    case TOGGLE_DPAD:
    case SWITCH_DEBUG_CAM_ON:
    case TAKE_SCREEN_SHOT:
    case SHOW_MOUSE_POINTER_TOGGLE:
        return eActionType::COMMON_CONTROLS;
    case VEHICLE_FIRE_WEAPON:
    case VEHICLE_FIRE_WEAPON_ALT:
    case VEHICLE_STEERLEFT:
    case VEHICLE_STEERRIGHT:
    case VEHICLE_STEERUP:
    case VEHICLE_STEERDOWN:
    case VEHICLE_ACCELERATE:
    case VEHICLE_BRAKE:
    case VEHICLE_RADIO_STATION_UP:
    case VEHICLE_RADIO_STATION_DOWN:
    case VEHICLE_RADIO_TRACK_SKIP:
    case VEHICLE_HORN:
    case TOGGLE_SUBMISSIONS:
    case VEHICLE_HANDBRAKE:
    case VEHICLE_LOOKLEFT:
    case VEHICLE_LOOKRIGHT:
    case VEHICLE_LOOKBEHIND:
    case VEHICLE_MOUSELOOK:
    case VEHICLE_TURRETLEFT:
    case VEHICLE_TURRETRIGHT:
    case VEHICLE_TURRETUP:
    case VEHICLE_TURRETDOWN:
        return eActionType::IN_CAR;
    default:
        return eActionType::NONE;
    }
}

// 0x52F390
const GxtChar* CControllerConfigManager::GetControllerSettingTextMouse(eControllerAction action) {
    switch (GetMouseButtonAssociatedWithAction(action)) {
    case rsMOUSE_LEFT_BUTTON:       return TheText.Get("FEC_MSL"); // LMB
    case rsMOUSE_MIDDLE_BUTTON:     return TheText.Get("FEC_MSM"); // MMB
    case rsMOUSE_RIGHT_BUTTON:      return TheText.Get("FEC_MSR"); // RMB
    case rsMOUSE_WHEEL_UP_BUTTON:   return TheText.Get("FEC_MWF"); // MS WHEEL UP
    case rsMOUSE_WHEEL_DOWN_BUTTON: return TheText.Get("FEC_MWB"); // MS WHEEL DN
    case rsMOUSE_X1_BUTTON:         return TheText.Get("FEC_MXO"); // MXB1
    case rsMOUSE_X2_BUTTON:         return TheText.Get("FEC_MXT"); // MXB2
    default:                        return nullptr; // Please not add 'NOTSA_UNREACHABLE' !!!
    }
}

// 0x52F450
const GxtChar* CControllerConfigManager::GetControllerSettingTextJoystick(eControllerAction action) {
    GxtChar(&NewStringWithNumber)[32] = *(GxtChar(*)[32])0xB7147C;

    const auto key = GetControllerKeyAssociatedWithAction(action, eControllerType::JOY_STICK);
    if (!GetIsKeyBlank(key, eControllerType::JOY_STICK)) {
        CMessages::InsertNumberInString(TheText.Get("FEC_JBO"), key, -1, -1, -1, -1, -1, NewStringWithNumber); // JOY~1~
        return NewStringWithNumber;
    }
    return nullptr; // Please not add 'NOTSA_UNREACHABLE' !!!
}

// unused
// 0x52F4A0
eContSetOrder CControllerConfigManager::GetNumOfSettingsForAction(eControllerAction action) {
    uint32 count = 0;
    for (const auto& type : CONTROLLER_TYPES_ALL) {
        if (!GetIsKeyBlank(GetControllerKeyAssociatedWithAction(action, type), type)) {
            count++;
        }
    }
    return (eContSetOrder)count;
}

// 0x530500
void CControllerConfigManager::MakeControllerActionsBlank() {
    for (const auto& type : CONTROLLER_TYPES_ALL) {
        for (auto action = 0; action < +CONTROLLER_ACTION_COUNT; ++action) {
            ClearSettingsAssociatedWithAction((eControllerAction)action, type);
        }
    }
}

// 0x531140
void CControllerConfigManager::AffectPadFromKeyBoard() {
#ifndef NOTSA_USE_SDL3
    RsKeyCodes keyCode;
    GTATranslateShiftKey(&keyCode);
#endif

    const auto inMenu = !CPad::padNumber && !FrontEndMenuManager.m_bMenuActive;
    for (auto& action : m_Actions) {
        // Handle key press logic
        for (const auto& type : CONTROLLER_TYPES_KEYBOARD) {
            const auto key = action.Keys[+type].m_uiActionInitiator;

            if (GetIsKeyboardKeyDown(key) && inMenu && !GetIsKeyBlank(key, type)) {
                if (inMenu) {
                    AffectControllerStateOn_ButtonDown(key, type);
                }
                auto* pad = CPad::GetPad();
                if (!pad || FrontEndMenuManager.m_bMenuActive) {
                    continue;
                }
                AffectControllerStateOn_ButtonUp_All_Player_States(key, type, GetControllerState(*pad, type));
            }
        }
    }
}

// 0x5314A0
void CControllerConfigManager::AffectPadFromMouse() {
    bool inMenu = !CPad::padNumber && !FrontEndMenuManager.m_bMenuActive;
    for (auto& action : m_Actions) {
        const auto button = action.Keys[+eControllerType::MOUSE].m_uiActionInitiator;

        if (GetIsMouseButtonDown(button)) {
            if (inMenu && !GetIsKeyBlank(button, eControllerType::MOUSE)) {
                AffectControllerStateOn_ButtonDown(button, eControllerType::MOUSE);
            }
        }

        if (GetIsMouseButtonUp(button)) {
            if (auto* pad = CPad::GetPad()) {
                AffectControllerStateOn_ButtonUp_All_Player_States(button, eControllerType::MOUSE, GetControllerState(*pad, eControllerType::MOUSE));
            }
        }
    }
}

// 0x531C90
void CControllerConfigManager::DeleteMatchingActionInitiators(eControllerAction action, KeyCode button, eControllerType type) {
    if (GetIsKeyBlank(button, type)) {
        return;
    }
    switch (GetActionType(action)) {
    case eActionType::FIRST_PERSON:
        DeleteMatchingCommonControls(action, button, type);
        DeleteMatching1rstPersonControls(action, button, type);
        DeleteMatching1rst3rdPersonControls(action, button, type);
        break;
    case eActionType::THIRD_PERSON:
        DeleteMatchingCommonControls(action, button, type);
        DeleteMatching1rst3rdPersonControls(action, button, type);
        DeleteMatching3rdPersonControls(action, button, type);
        DeleteMatchingVehicle_3rdPersonControls(button, type);
        break;
    case eActionType::IN_CAR:
        DeleteMatchingCommonControls(action, button, type);
        DeleteMatchingVehicleControls(action, button, type);
        DeleteMatchingVehicle_3rdPersonControls(button, type);
        break;
    case eActionType::IN_CAR_THIRD_PERSON:
        DeleteMatchingCommonControls(action, button, type);
        DeleteMatching1rst3rdPersonControls(action, button, type);
        DeleteMatching3rdPersonControls(action, button, type);
        DeleteMatchingVehicleControls(action, button, type);
        break;
    case eActionType::COMMON_CONTROLS:
        DeleteMatchingCommonControls(action, button, type);
        DeleteMatching1rstPersonControls(action, button, type);
        DeleteMatching1rst3rdPersonControls(action, button, type);
        DeleteMatching3rdPersonControls(action, button, type);
        DeleteMatchingVehicleControls(action, button, type);
        DeleteMatchingVehicle_3rdPersonControls(button, type);
        break;
    case eActionType::FIRST_THIRD_PERSON:
        DeleteMatchingCommonControls(action, button, type);
        DeleteMatching1rstPersonControls(action, button, type);
        DeleteMatching1rst3rdPersonControls(action, button, type);
        DeleteMatching3rdPersonControls(action, button, type);
        DeleteMatchingVehicle_3rdPersonControls(button, type);
        break;
    default:
        return;
    }
}

// 0x52FE10
const GxtChar* CControllerConfigManager::GetControllerSettingTextKeyBoard(eControllerAction action, eControllerType type) {
    GxtChar(&s_KeyName)[50] = *(GxtChar(*)[50])0xB714BC; // temp value
    rng::fill(s_KeyName, 0);

    const auto key = GetControllerKeyAssociatedWithAction(action, type);
    if (GetIsKeyBlank(key, type)) {
        return nullptr;
    }

    const auto KeyNameWithNumber = [&](const GxtChar* text, int32 key) {
        CMessages::InsertNumberInString(text, key, -1, -1, -1, -1, -1, s_KeyName);
        return s_KeyName;
    };

    if (key >= rsF1 && key <= rsF12) {
        return KeyNameWithNumber(TheText.Get("FEC_FNC"), key - rsF1 + 1); // F~1~
    }

    if (key > 255) { /* Non-ASCII keys */
        switch (key) {
        case rsINS:      return TheText.Get("FEC_IRT");                       // INS
        case rsDEL:      return TheText.Get("FEC_DLL");                       // DEL
        case rsHOME:     return TheText.Get("FEC_HME");                       // HOME
        case rsEND:      return TheText.Get("FEC_END");                       // END
        case rsPGUP:     return TheText.Get("FEC_PGU");                       // PGUP
        case rsPGDN:     return TheText.Get("FEC_PGD");                       // PGDN
        case rsUP:       return TheText.Get("FEC_UPA");                       // UP
        case rsDOWN:     return TheText.Get("FEC_DWA");                       // DOWN
        case rsLEFT:     return TheText.Get("FEC_LFA");                       // LEFT
        case rsRIGHT:    return TheText.Get("FEC_RFA");                       // RIGHT
        case rsDIVIDE:   return TheText.Get("FEC_FWS");                       // NUM /
        case rsTIMES:    return TheText.Get("FECSTAR");                       // NUM STAR
        case rsPLUS:     return TheText.Get("FEC_PLS");                       // NUM +
        case rsMINUS:    return TheText.Get("FEC_MIN");                       // NUM -
        case rsPADDEL:   return TheText.Get("FEC_DOT");                       // NUM .
        case rsPADEND:   return KeyNameWithNumber(TheText.Get("FEC_NMN"), 1); // NUM~1~
        case rsPADDOWN:  return KeyNameWithNumber(TheText.Get("FEC_NMN"), 2); // NUM~1~
        case rsPADPGDN:  return KeyNameWithNumber(TheText.Get("FEC_NMN"), 3); // NUM~1~
        case rsPADLEFT:  return KeyNameWithNumber(TheText.Get("FEC_NMN"), 4); // NUM~1~
        case rsPAD5:     return KeyNameWithNumber(TheText.Get("FEC_NMN"), 5); // NUM~1~
        case rsNUMLOCK:  return TheText.Get("FEC_NLK");                       // NUMLOCK
        case rsPADRIGHT: return KeyNameWithNumber(TheText.Get("FEC_NMN"), 6); // NUM~1~
        case rsPADHOME:  return KeyNameWithNumber(TheText.Get("FEC_NMN"), 7); // NUM~1~
        case rsPADUP:    return KeyNameWithNumber(TheText.Get("FEC_NMN"), 8); // NUM~1~
        case rsPADPGUP:  return KeyNameWithNumber(TheText.Get("FEC_NMN"), 9); // NUM~1~
        case rsPADINS:   return KeyNameWithNumber(TheText.Get("FEC_NMN"), 0); // NUM~1~
        case rsPADENTER: return TheText.Get("FEC_ETR");                       // ENT
        case rsSCROLL:   return TheText.Get("FEC_SLK");                       // SCROLL LOCK
        case rsPAUSE:    return TheText.Get("FEC_PSB");                       // BREAK
        case rsBACKSP:   return TheText.Get("FEC_BSP");                       // BSPACE
        case rsTAB:      return TheText.Get("FEC_TAB");                       // TAB
        case rsCAPSLK:   return TheText.Get("FEC_CLK");                       // CAPSLOCK
        case rsENTER:    return TheText.Get("FEC_RTN");                       // RETURN
        case rsLSHIFT:   return TheText.Get("FEC_LSF");                       // LSHIFT
        case rsRSHIFT:   return TheText.Get("FEC_RSF");                       // RSHIFT
        case rsSHIFT:    return TheText.Get("FEC_SFT");                       // SHIFT
        case rsLCTRL:    return TheText.Get("FEC_LCT");                       // LCTRL
        case rsRCTRL:    return TheText.Get("FEC_RCT");                       // RCTRL
        case rsLALT:     return TheText.Get("FEC_LAL");                       // LALT
        case rsRALT:     return TheText.Get("FEC_RAL");                       // RALT
        case rsLWIN:     return TheText.Get("FEC_LWD");                       // LWIN
        case rsRWIN:     return TheText.Get("FEC_RWD");                       // RWIN
        case rsAPPS:     return TheText.Get("FEC_WRC");                       // WINCLICK
        default:         return nullptr; // Please not add 'NOTSA_UNREACHABLE' !!!
        }
    } else { /* ASCII keys */
        switch (key) {
        case '*': return TheText.Get("FEC_AST"); // Asterisk
        case ' ': return TheText.Get("FEC_SPC"); // SPACE
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
            s_KeyName[0] = (GxtChar)key ? key : '#';
            s_KeyName[1] = '\0';
            return s_KeyName;
        }
    }
    return nullptr; // Please not add 'NOTSA_UNREACHABLE' !!!
}

// 0x52F560
const GxtChar* CControllerConfigManager::GetButtonComboText(eControllerAction action) {
    return action == VEHICLE_LOOKBEHIND ? TheText.Get("FEC_LBC") : nullptr; // Use Look Left With Look Right.
}

// 0x5303D0
void CControllerConfigManager::GetGxtStringOfCommandKeys(eControllerAction action, GxtChar* pStringToFill, uint16 maximumLength) {
    const auto setString = [pStringToFill, maximumLength](const GxtChar* var) {
        const auto len = maximumLength - CMessages::GetStringLength(var);
        CMessages::StringCopy(pStringToFill, var, len);
    };

    if (FrontEndMenuManager.m_ControlMethod == eController::JOYPAD) {
        if (auto keyText = GetControllerSettingTextJoystick(action)) {
            return setString(keyText);
        }
    }

    if (auto keyText = GetControllerSettingTextMouse(action)) {
        return setString(keyText);
    }

    if (auto keyText = GetControllerSettingTextKeyBoard(action, eControllerType::KEYBOARD)) {
        return setString(keyText);
    }

    if (auto keyText = GetControllerSettingTextKeyBoard(action, eControllerType::OPTIONAL_EXTRA_KEY)) {
        return setString(keyText);
    }

    if (auto keyText = GetControllerSettingTextJoystick(action)) {
        return setString(keyText);
    }

    // Two call, ok
    setString(GetControllerSettingTextMouse(action));
}

// unknown address
CJoySticks::CJoySticks() {
    ClearJoyInfo(0);
    ClearJoyInfo(1);
}

// unused
// 0x744DD0 ?
void CJoySticks::ClearJoyInfo(int32 JoyNumber) {
    JoyStickNum[JoyNumber].wDeviceID = 0;
    JoyStickNum[JoyNumber].bJoyAttachedToPort = false;
    JoyStickNum[JoyNumber].bZAxisPresent = false;
    JoyStickNum[JoyNumber].bZRotPresent = false;
}

// NOTSA
eControllerAction CControllerConfigManager::GetActionIDByName(std::string_view name) {
    for (auto&& [i, actionName] : rngv::enumerate(m_ControllerActionName)) {
        if (std::string_view(reinterpret_cast<const char*>(actionName)) == name) {
            return (eControllerAction)i;
        }
    }
    return CA_NONE;
}

// inline
void CControllerConfigManager::CheckAndClear(eControllerAction action, eControllerType type, KeyCode button) {
    if (GetControllerKeyAssociatedWithAction(action, type) == button) {
        ClearSettingsAssociatedWithAction(action, type);
    }
}

// inline
void CControllerConfigManager::CheckAndReset(eControllerAction action, eControllerType type, KeyCode button, int16& state) {
    if (GetControllerKeyAssociatedWithAction(action, type) == button) {
        state = 0;
    }
}

// inline
void CControllerConfigManager::CheckAndSetButton(eControllerAction action, eControllerType type, KeyCode button, int16& state) {
    if (GetControllerKeyAssociatedWithAction(action, type) == button) {
        state = 255;
    }
}

// inline
void CControllerConfigManager::CheckAndSetPad(eControllerAction action, eControllerType type, KeyCode button, int16& primaryState, int16& secondaryState) {
    if (GetControllerKeyAssociatedWithAction(action, type) == button) {
        if (primaryState) {
            primaryState   = 0;
            secondaryState = 0;
        } else {
            secondaryState = 255;
        }
    }
}

// inline
void CControllerConfigManager::CheckAndSetStick(eControllerAction action, eControllerType type, KeyCode button, int16& state, bool& movementBothDown, int16 value) {
    if (GetControllerKeyAssociatedWithAction(action, type) == button) {
        if (state == -value || movementBothDown) {
            state = 0;
            movementBothDown = true;
        } else {
            state = value;
        }
    }
}

// inlined
int16& CControllerConfigManager::GetControllerStateJoyStick(CPad& pad, KeyCode button) {
    const auto specGamepad = AllValidWinJoys.JoyStickNum[PAD1].wVendorID == 0x3427 && AllValidWinJoys.JoyStickNum[PAD1].wProductID == 0x1190;
    switch (button) {
    case eJoyButtons::JOYBUTTON_ONE:       return specGamepad ? pad.PCTempJoyState.ButtonTriangle : pad.PCTempJoyState.ButtonCircle;
    case eJoyButtons::JOYBUTTON_TWO:       return specGamepad ? pad.PCTempJoyState.ButtonCircle : pad.PCTempJoyState.ButtonCross;
    case eJoyButtons::JOYBUTTON_THREE:     return specGamepad ? pad.PCTempJoyState.ButtonCross : pad.PCTempJoyState.ButtonSquare;
    case eJoyButtons::JOYBUTTON_FOUR:      return specGamepad ? pad.PCTempJoyState.ButtonSquare : pad.PCTempJoyState.ButtonTriangle;
    case eJoyButtons::JOYBUTTON_FIVE:      return pad.PCTempJoyState.LeftShoulder2;
    case eJoyButtons::JOYBUTTON_SIX:       return pad.PCTempJoyState.RightShoulder2;
    case eJoyButtons::JOYBUTTON_SEVEN:     return pad.PCTempJoyState.LeftShoulder1;
    case eJoyButtons::JOYBUTTON_EIGHT:     return pad.PCTempJoyState.RightShoulder1;
    case eJoyButtons::JOYBUTTON_NINE:      return pad.PCTempJoyState.Select;
    case eJoyButtons::JOYBUTTON_TEN:       return pad.PCTempJoyState.ShockButtonL;
    case eJoyButtons::JOYBUTTON_ELEVEN:    return pad.PCTempJoyState.ShockButtonR;
    case eJoyButtons::JOYBUTTON_TWELVE:    return pad.PCTempJoyState.Start;
    case eJoyButtons::JOYBUTTON_THIRTEEN:  return pad.PCTempJoyState.DPadUp;
    case eJoyButtons::JOYBUTTON_FOURTEEN:  return pad.PCTempJoyState.DPadRight;
    case eJoyButtons::JOYBUTTON_FIFTHTEEN: return pad.PCTempJoyState.DPadDown;
    case eJoyButtons::JOYBUTTON_SIXTEEN:   return pad.PCTempJoyState.DPadLeft;
    default:                               NOTSA_UNREACHABLE("Invalid button ({})", (uint32)button);
    }
}

// inline
bool CControllerConfigManager::UseDrivingControls() {
    // FindPlayerPed() && FindPlayerVehicle() && FindPlayerPed()->GetPedState() == PEDSTATE_DRIVING && !pad->DisablePlayerControls
    if (FindPlayerVehicle()) {
        if (auto ped = FindPlayerPed()) {
            return ped->IsStateDriving() && !CPad::GetPad()->DisablePlayerControls;
        }
    }
    return false;
}

// inline
bool CControllerConfigManager::UseFirstPersonControls() {
    return notsa::contains({ MODE_1STPERSON, MODE_SNIPER, MODE_ROCKETLAUNCHER, MODE_ROCKETLAUNCHER_HS, MODE_M16_1STPERSON, MODE_CAMERA }, TheCamera.m_aCams[TheCamera.m_nActiveCam].m_nMode);
}

// inline
void CControllerConfigManager::AffectControllerStateOn_ButtonDown(KeyCode button, eControllerType type) {
    const auto ped = CPad::GetPad();
    if (!ped) {
        return;
    }

    auto& state = GetControllerState(*ped, type);
    if (UseDrivingControls()) {
        AffectControllerStateOn_ButtonDown_Driving(button, type, state);
        AffectControllerStateOn_ButtonDown_VehicleAndThirdPersonOnly(button, type, state);
        AffectControllerStateOn_ButtonDown_AllStates(button, type, state);
    } else {
        AffectControllerStateOn_ButtonDown_FirstAndThirdPersonOnly(button, type, state);
        if (UseFirstPersonControls()) {
            AffectControllerStateOn_ButtonDown_FirstPersonOnly(button, type, state);
            AffectControllerStateOn_ButtonDown_AllStates(button, type, state);
        } else {
            AffectControllerStateOn_ButtonDown_ThirdPersonOnly(button, type, state);
            AffectControllerStateOn_ButtonDown_VehicleAndThirdPersonOnly(button, type, state);
            AffectControllerStateOn_ButtonDown_AllStates(button, type, state);
        }
    }
}

// inline
bool CControllerConfigManager::IsKeyboardKeyDownInState(CKeyboardState& state, KeyCode key) {
    if (key < 0xFF) {
        return state.standardKeys[key];
    }

    if (key >= rsF1 && key <= rsF12) {
        return state.FKeys[key - rsF1];
    }

    switch (key) {
    case rsESC:      return state.esc;
    case rsINS:      return state.insert;
    case rsDEL:      return state.del;
    case rsHOME:     return state.home;
    case rsEND:      return state.end;
    case rsPGUP:     return state.pgup;
    case rsPGDN:     return state.pgdn;
    case rsUP:       return state.up;
    case rsDOWN:     return state.down;
    case rsLEFT:     return state.left;
    case rsRIGHT:    return state.right;
    case rsDIVIDE:   return state.div;
    case rsTIMES:    return state.mul;
    case rsPLUS:     return state.add;
    case rsMINUS:    return state.sub;
    case rsPADDEL:   return state.decimal;
    case rsPADEND:   return state.num1;
    case rsPADDOWN:  return state.num2;
    case rsPADPGDN:  return state.num3;
    case rsPADLEFT:  return state.num4;
    case rsPAD5:     return state.num5;
    case rsNUMLOCK:  return state.numlock;
    case rsPADRIGHT: return state.num6;
    case rsPADHOME:  return state.num7;
    case rsPADUP:    return state.num8;
    case rsPADPGUP:  return state.num9;
    case rsPADINS:   return state.num0;
    case rsPADENTER: return state.enter;
    case rsSCROLL:   return state.scroll;
    case rsPAUSE:    return state.pause;
    case rsBACKSP:   return state.back;
    case rsTAB:      return state.tab;
    case rsCAPSLK:   return state.capslock;
    case rsENTER:    return state.extenter;
    case rsLSHIFT:   return state.lshift;
    case rsRSHIFT:   return state.rshift;
    case rsSHIFT:    return state.shift;
    case rsLCTRL:    return state.lctrl;
    case rsRCTRL:    return state.rctrl;
    case rsLALT:     return state.lalt;
    case rsRALT:     return state.ralt;
    case rsLWIN:     return state.lwin;
    case rsRWIN:     return state.rwin;
    case rsAPPS:     return state.apps;
    }

    return false;
}

// inline
CControllerState& CControllerConfigManager::GetControllerState(CPad& pad, eControllerType type) {
    switch (type) {
    case eControllerType::KEYBOARD:
    case eControllerType::OPTIONAL_EXTRA_KEY: return pad.PCTempKeyState;
    case eControllerType::MOUSE:              return pad.PCTempMouseState;
    case eControllerType::JOY_STICK:          return pad.PCTempJoyState;
    default:                                  NOTSA_UNREACHABLE();
    }
}
