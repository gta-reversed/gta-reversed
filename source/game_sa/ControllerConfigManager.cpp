#include "StdInc.h"
#include "WinInput.h"

#ifndef NOTSA_USE_SDL3
#include "WndProc.h"
#endif

#include "ControllerConfigManager.h"

CControllerConfigManager& ControlsManager = *(CControllerConfigManager *) 0xB70198;
GxtChar (&NewStringWithNumber)[32] = *(GxtChar(*)[32])0xB7147C;

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
    RH_ScopedInstall(HandleButtonRelease, 0x52FD40);
    RH_ScopedInstall(ClearSettingsAssociatedWithAction, 0x52FD70);
    RH_ScopedInstall(GetKeyNameForKeyboard, 0x52FE10);
    RH_ScopedInstall(UnmapVehicleEnterExit, 0x531BC0);
    RH_ScopedInstall(ClearSniperZoomMappings, 0x531C20);
    RH_ScopedInstall(DeleteMatchingActionInitiators, 0x531C90);
    RH_ScopedInstall(GetControllerSettingText, 0x531E20);
    RH_ScopedInstall(GetActionKeyName, 0x531E90);
    RH_ScopedInstall(GetDefinedKeyByGxtName, 0x5303D0);
    RH_ScopedInstall(AffectPadFromMouse, 0x5314A0);
    RH_ScopedInstall(Clear1st3rdPersonMappings, 0x5318C0);
    RH_ScopedInstall(ClearVehicleMappings, 0x5319D0);
    RH_ScopedInstall(SetControllerKeyAssociatedWithAction, 0x530490);
    RH_ScopedInstall(MakeControllerActionsBlank, 0x530500);
    RH_ScopedInstall(AffectPadFromKeyBoard, 0x531140);
    RH_ScopedInstall(ClearCommonMappings, 0x531670);
    RH_ScopedInstall(ClearPedMappings, 0x531730);
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
void CControllerConfigManager::ClearPedMappings(eControllerAction action, KeyCode button, eControllerType type) {
    if (GetIsKeyBlank(button, type)) {
        return;
    }

    CheckAndClear(eControllerAction::CA_GO_LEFT, type, button);
    CheckAndClear(eControllerAction::CA_GO_RIGHT, type, button);
    CheckAndClear(eControllerAction::CA_GROUP_CONTROL_FWD, type, button);
    CheckAndClear(eControllerAction::CA_GROUP_CONTROL_BWD, type, button);
    CheckAndClear(eControllerAction::CA_CONVERSATION_NO, type, button);
    CheckAndClear(eControllerAction::CA_CONVERSATION_YES, type, button);
    CheckAndClear(eControllerAction::CA_PED_CYCLE_WEAPON_LEFT, type, button);
    CheckAndClear(eControllerAction::CA_PED_CYCLE_WEAPON_RIGHT, type, button);
    CheckAndClear(eControllerAction::CA_PED_JUMPING, type, button);
    CheckAndClear(eControllerAction::CA_PED_SPRINT, type, button);
    CheckAndClear(eControllerAction::CA_PED_LOOKBEHIND, type, button);
    CheckAndClear(eControllerAction::CA_PED_DUCK, type, button);

    if (action != eControllerAction::CA_PED_FIRE_WEAPON_ALT && FrontEndMenuManager.m_ControlMethod == eController::JOYPAD || !FrontEndMenuManager.m_ControlMethod) {
        CheckAndClear(eControllerAction::CA_PED_ANSWER_PHONE, type, button);
    }
    CheckAndClear(eControllerAction::CA_PED_WALK, type, button);
    if (FrontEndMenuManager.m_ControlMethod == eController::JOYPAD) {
        CheckAndClear(eControllerAction::CA_PED_CENTER_CAMERA_BEHIND_PLAYER, type, button);
    }
}


// 0x531670
void CControllerConfigManager::ClearCommonMappings(eControllerAction nop, KeyCode button, eControllerType type) {
    if (GetIsKeyBlank(button, type)) {
        return;
    }

    CheckAndClear(eControllerAction::CA_CAMERA_CHANGE_VIEW_ALL_SITUATIONS, type, button);
    CheckAndClear(eControllerAction::CA_NETWORK_TALK, type, button);
    CheckAndClear(eControllerAction::CA_SWITCH_DEBUG_CAM_ON, type, button);
    CheckAndClear(eControllerAction::CA_TOGGLE_DPAD, type, button);
    CheckAndClear(eControllerAction::CA_TAKE_SCREEN_SHOT, type, button);
    CheckAndClear(eControllerAction::CA_SHOW_MOUSE_POINTER_TOGGLE, type, button);
}

// 0x530490
void CControllerConfigManager::SetControllerKeyAssociatedWithAction(eControllerAction action, KeyCode button, eControllerType type) {
    ResetSettingOrder(action);
    const auto numOfSettings = GetNumOfSettingsForAction(action);
    m_Actions[action].Keys[type].m_uiActionInitiator = button;
    m_Actions[action].Keys[type].m_uiSetOrder = (eContSetOrder)(numOfSettings + 1);
}

// 0x5319D0
void CControllerConfigManager::ClearVehicleMappings(eControllerAction nop, KeyCode button, eControllerType type) {
    if (GetIsKeyBlank(button, type)) {
        return;
    }

    CheckAndClear(eControllerAction::CA_VEHICLE_FIRE_WEAPON_ALT, type, button);
    CheckAndClear(eControllerAction::CA_VEHICLE_FIRE_WEAPON, type, button);
    CheckAndClear(eControllerAction::CA_VEHICLE_STEER_LEFT, type, button);
    CheckAndClear(eControllerAction::CA_VEHICLE_STEER_RIGHT, type, button);
    CheckAndClear(eControllerAction::CA_VEHICLE_STEER_UP, type, button);
    CheckAndClear(eControllerAction::CA_VEHICLE_STEER_DOWN, type, button);
    CheckAndClear(eControllerAction::CA_VEHICLE_ACCELERATE, type, button);
    CheckAndClear(eControllerAction::CA_VEHICLE_BRAKE, type, button);
    CheckAndClear(eControllerAction::CA_VEHICLE_RADIO_STATION_UP, type, button);
    CheckAndClear(eControllerAction::CA_VEHICLE_RADIO_STATION_DOWN, type, button);
    CheckAndClear(eControllerAction::CA_VEHICLE_RADIO_TRACK_SKIP, type, button);
    CheckAndClear(eControllerAction::CA_VEHICLE_HORN, type, button);
    CheckAndClear(eControllerAction::CA_TOGGLE_SUBMISSIONS, type, button);
    CheckAndClear(eControllerAction::CA_VEHICLE_HANDBRAKE, type, button);
    CheckAndClear(eControllerAction::CA_VEHICLE_LOOKLEFT, type, button);
    CheckAndClear(eControllerAction::CA_VEHICLE_LOOKRIGHT, type, button);
    CheckAndClear(eControllerAction::CA_VEHICLE_LOOKBEHIND, type, button);
    CheckAndClear(eControllerAction::CA_VEHICLE_MOUSELOOK, type, button);
    CheckAndClear(eControllerAction::CA_VEHICLE_TURRETLEFT, type, button);
    CheckAndClear(eControllerAction::CA_VEHICLE_TURRETRIGHT, type, button);
    CheckAndClear(eControllerAction::CA_VEHICLE_TURRETUP, type, button);
    CheckAndClear(eControllerAction::CA_VEHICLE_TURRETDOWN, type, button);
}

// 0x5318C0
void CControllerConfigManager::Clear1st3rdPersonMappings(eControllerAction action, KeyCode button, eControllerType type) {
    if (GetIsKeyBlank(button, type)) {
        return;
    }

    if (action != eControllerAction::CA_PED_ANSWER_PHONE && FrontEndMenuManager.m_ControlMethod == eController::JOYPAD || !FrontEndMenuManager.m_ControlMethod) {
        CheckAndClear(eControllerAction::CA_PED_FIRE_WEAPON_ALT, type, button);
    }
    CheckAndClear(eControllerAction::CA_PED_FIRE_WEAPON, type, button);
    CheckAndClear(eControllerAction::CA_PED_LOCK_TARGET, type, button);
    CheckAndClear(eControllerAction::CA_GO_FORWARD, type, button);
    CheckAndClear(eControllerAction::CA_GO_BACK, type, button);
    if (FrontEndMenuManager.m_ControlMethod == eController::JOYPAD) {
        CheckAndClear(eControllerAction::CA_PED_1RST_PERSON_LOOK_LEFT, type, button);
        CheckAndClear(eControllerAction::CA_PED_1RST_PERSON_LOOK_RIGHT, type, button);
        CheckAndClear(eControllerAction::CA_PED_1RST_PERSON_LOOK_DOWN, type, button);
        CheckAndClear(eControllerAction::CA_PED_1RST_PERSON_LOOK_UP, type, button);
    }
}

// 0x52F510
void CControllerConfigManager::UpdateJoyButtonState(int32 padNo) {
    m_ButtonStates[0] = false;
    m_ButtonStates[4] = false;
    m_ButtonStates[8] = false;
    m_ButtonStates[12] = false;
    m_ButtonStates[16] = false;

    for (auto i = 0u; i < JOYBUTTON_COUNT; ++i) {
        m_ButtonStates[i] = (m_NewJoyState.rgbButtons[i] & 128) != 0;
    }
}

// unused
// 0x531E90
const GxtChar* CControllerConfigManager::GetActionKeyName(eControllerAction action) {
    ResetSettingOrder(action);
    for (const auto& type : CONTROLLER_TYPES_KEYBOARD) {
        if (m_Actions[action].Keys[type].m_uiSetOrder) {
            return GetKeyNameForKeyboard(action, type);
        }
    }
    return nullptr;
}

// 0x531E20
const GxtChar* CControllerConfigManager::GetControllerSettingText(eControllerAction action, eContSetOrder order) {
    for (const auto& type : CONTROLLER_TYPES_ALL) {
        if (m_Actions[action].Keys[type].m_uiSetOrder == order) {
            switch (type) {
            case eControllerType::KEYBOARD:
            case eControllerType::OPTIONAL_EXTRA_KEY: return GetKeyNameForKeyboard(action, (eControllerType)type);
            case eControllerType::MOUSE:              return GetControllerSettingTextMouse(action);
            case eControllerType::JOY_STICK:          return GetControllerSettingTextJoystick(action);
            default:                                  NOTSA_UNREACHABLE();
            }
        }
    }
    return nullptr;
}

// 0x531C20
void CControllerConfigManager::ClearSniperZoomMappings(eControllerAction nop, KeyCode button, eControllerType type) {
    if (GetIsKeyBlank(button, type)) {
        return;
    }

    CheckAndClear(eControllerAction::CA_PED_SNIPER_ZOOM_IN, type, button);
    CheckAndClear(eControllerAction::CA_PED_SNIPER_ZOOM_OUT, type, button);
}

// 0x531BC0
void CControllerConfigManager::UnmapVehicleEnterExit(KeyCode button, eControllerType type) {
    if (GetIsKeyBlank(button, type)) {
        return;
    }

    CheckAndClear(eControllerAction::CA_VEHICLE_ENTER_EXIT, type, button);
}

// 0x52FD70
void CControllerConfigManager::ClearSettingsAssociatedWithAction(eControllerAction action, eControllerType type) {
    switch (type) {
    case eControllerType::KEYBOARD:
    case eControllerType::OPTIONAL_EXTRA_KEY:
        m_Actions[action].Keys[type].m_uiActionInitiator = rsNULL;
        m_Actions[action].Keys[type].m_uiSetOrder        = NO_ORDER_SET;
        break;
    case eControllerType::MOUSE:
    case eControllerType::JOY_STICK:
        m_Actions[action].Keys[type].m_uiActionInitiator = (RsKeyCodes)0;
        m_Actions[action].Keys[type].m_uiSetOrder        = NO_ORDER_SET;
        break;
    default: NOTSA_UNREACHABLE();
    }
    ResetSettingOrder(action);
}

// unused
// 0x52FD20
void CControllerConfigManager::AffectControllerStateOn_ButtonDown_VehicleAndThirdPersonOnly(KeyCode button, eControllerType type, CControllerState* state) {
    CheckAndSetButton(eControllerAction::CA_VEHICLE_ENTER_EXIT, type, button, state->ButtonTriangle);
}

// 0x52FCA0
void CControllerConfigManager::AffectControllerStateOn_ButtonDown_AllStates(KeyCode button, eControllerType type, CControllerState* state) {
    CheckAndSetButton(eControllerAction::CA_CAMERA_CHANGE_VIEW_ALL_SITUATIONS, type, button, state->Select);
    CheckAndSetPad(eControllerAction::CA_CONVERSATION_NO, type, button, state->DPadLeft, state->DPadRight);
    CheckAndSetPad(eControllerAction::CA_CONVERSATION_YES, type, button, state->DPadRight, state->DPadLeft);
    CheckAndSetButton(eControllerAction::CA_NETWORK_TALK, type, button, state->m_bChatIndicated);
}

// 0x52F580
KeyCode CControllerConfigManager::GetMouseButtonAssociatedWithAction(eControllerAction action) {
    return m_Actions[action].Keys[eControllerType::MOUSE].m_uiActionInitiator;
}

// 0x52FAB0
void CControllerConfigManager::AffectControllerStateOn_ButtonDown_FirstAndThirdPersonOnly(KeyCode button, eControllerType type, CControllerState* state) {
    CheckAndSetButton(eControllerAction::CA_PED_FIRE_WEAPON, type, button, state->ButtonCircle);   
    CheckAndSetButton(eControllerAction::CA_PED_FIRE_WEAPON_ALT, type, button, state->LeftShoulder1);  
    CheckAndSetButton(eControllerAction::CA_PED_LOCK_TARGET, type, button, state->RightShoulder1);
    CheckAndSetStick(eControllerAction::CA_GO_FORWARD, type, button, state->LeftStickY, m_bStickL_Up_Dwn_MovementBothDown[type], -128);
    CheckAndSetStick(eControllerAction::CA_GO_BACK, type, button, state->LeftStickY, m_bStickL_Up_Dwn_MovementBothDown[type], 128);
    CheckAndSetStick(eControllerAction::CA_GO_LEFT, type, button, state->LeftStickX, m_bStickL_X_Rgh_Lft_MovementBothDown[type], -128);
    CheckAndSetStick(eControllerAction::CA_GO_RIGHT, type, button, state->LeftStickX, m_bStickL_X_Rgh_Lft_MovementBothDown[type], 128); 
    CheckAndSetButton(eControllerAction::CA_PED_WALK, type, button, state->m_bPedWalk);
    CheckAndSetPad(eControllerAction::CA_GROUP_CONTROL_FWD, type, button, state->DPadUp, state->DPadDown);
    CheckAndSetPad(eControllerAction::CA_GROUP_CONTROL_BWD, type, button, state->DPadDown, state->DPadUp);    
    CheckAndSetStick(eControllerAction::CA_PED_1RST_PERSON_LOOK_LEFT, type, button, state->RightStickX, m_bStickR_X_Rgh_Lft_MovementBothDown[type], -128);
    CheckAndSetStick(eControllerAction::CA_PED_1RST_PERSON_LOOK_RIGHT, type, button, state->RightStickX, m_bStickR_X_Rgh_Lft_MovementBothDown[type], 128);  
    if (FrontEndMenuManager.m_ControlMethod == eController::JOYPAD) {
        CheckAndSetStick(eControllerAction::CA_PED_1RST_PERSON_LOOK_UP, type, button, state->RightStickY, m_bStickR_Up_Dwn_MovementBothDown[type], 128);
        CheckAndSetStick(eControllerAction::CA_PED_1RST_PERSON_LOOK_DOWN, type, button, state->RightStickY, m_bStickR_Up_Dwn_MovementBothDown[type], -128);
    }
}

// 0x52FA20
void CControllerConfigManager::AffectControllerStateOn_ButtonDown_ThirdPersonOnly(KeyCode button, eControllerType type, CControllerState* state) {
    CheckAndSetButton(eControllerAction::CA_PED_LOOKBEHIND, type, button, state->ShockButtonR);
    CheckAndSetButton(eControllerAction::CA_PED_JUMPING, type, button, state->ButtonSquare);
    CheckAndSetButton(eControllerAction::CA_PED_ANSWER_PHONE, type, button, state->LeftShoulder1);
    CheckAndSetButton(eControllerAction::CA_PED_CYCLE_WEAPON_LEFT, type, button, state->LeftShoulder2);
    CheckAndSetButton(eControllerAction::CA_PED_CYCLE_WEAPON_RIGHT, type, button, state->RightShoulder2);
    CheckAndSetButton(eControllerAction::CA_PED_SPRINT, type, button, state->ButtonCross);
    CheckAndSetButton(eControllerAction::CA_PED_DUCK, type, button, state->ShockButtonL);

    if (FrontEndMenuManager.m_ControlMethod == eController::JOYPAD) {
        CheckAndSetButton(eControllerAction::CA_PED_CENTER_CAMERA_BEHIND_PLAYER, type, button, state->LeftShoulder1);
    }
}

// 0x52F550
bool CControllerConfigManager::GetIsActionAButtonCombo(eControllerAction Action)
{  
    /* NOT USED IN SA, but explains some things
    switch (Action) {
    case CA_VEHICLE_LOOKBEHIND:
    // case CA_PED_CYCLE_TARGET_LEFT:
    // case CA_PED_CYCLE_TARGET_RIGHT:
        return true;
    }
    */

    return false;
}

// 0x52F4F0
KeyCode CControllerConfigManager::GetControllerKeyAssociatedWithAction(eControllerAction action, eControllerType type) {
    return m_Actions[action].Keys[type].m_uiActionInitiator;
}

// 0x52F9E0
void CControllerConfigManager::AffectControllerStateOn_ButtonDown_FirstPersonOnly(KeyCode button, eControllerType type, CControllerState* state) {
    CheckAndSetButton(eControllerAction::CA_PED_SNIPER_ZOOM_IN, type, button, state->ButtonSquare);
    CheckAndSetButton(eControllerAction::CA_PED_SNIPER_ZOOM_OUT, type, button, state->ButtonCross);
    CheckAndSetButton(eControllerAction::CA_PED_DUCK, type, button, state->ShockButtonL);
}

// 0x52FD40
void CControllerConfigManager::HandleButtonRelease(KeyCode button, eControllerType type, CControllerState* state) {
    CheckAndReset(eControllerAction::CA_NETWORK_TALK, type, button, state->m_bChatIndicated);
    CheckAndReset(eControllerAction::CA_VEHICLE_MOUSELOOK, type, button, state->m_bVehicleMouseLook);
}

// 0x52F7B0
void CControllerConfigManager::AffectControllerStateOn_ButtonDown_Driving(KeyCode button, eControllerType type, CControllerState* state) {
    CheckAndSetButton(eControllerAction::CA_VEHICLE_FIRE_WEAPON, type, button, state->ButtonCircle);
    CheckAndSetButton(eControllerAction::CA_VEHICLE_FIRE_WEAPON_ALT, type, button, state->LeftShoulder1);

    // NOTE: original double, check if you press both buttons you look behind
    CheckAndSetButton(eControllerAction::CA_VEHICLE_LOOKBEHIND, type, button, state->LeftShoulder2); 
    CheckAndSetButton(eControllerAction::CA_VEHICLE_LOOKBEHIND, type, button, state->RightShoulder2);

    CheckAndSetButton(eControllerAction::CA_VEHICLE_LOOKLEFT, type, button, state->LeftShoulder2);
    CheckAndSetButton(eControllerAction::CA_VEHICLE_LOOKRIGHT, type, button, state->RightShoulder2);
    CheckAndSetButton(eControllerAction::CA_VEHICLE_MOUSELOOK, type, button, state->m_bVehicleMouseLook);
    CheckAndSetButton(eControllerAction::CA_VEHICLE_HORN, type, button, state->ShockButtonL);
    CheckAndSetButton(eControllerAction::CA_VEHICLE_HANDBRAKE, type, button, state->RightShoulder1);
    CheckAndSetButton(eControllerAction::CA_VEHICLE_ACCELERATE, type, button, state->ButtonCross);
    CheckAndSetButton(eControllerAction::CA_VEHICLE_BRAKE, type, button, state->ButtonSquare);
    CheckAndSetStick(eControllerAction::CA_VEHICLE_STEER_UP, type, button, state->LeftStickY, m_bStickL_Up_Dwn_MovementBothDown[type], -128);
    CheckAndSetStick(eControllerAction::CA_VEHICLE_STEER_DOWN, type, button, state->LeftStickY, m_bStickL_Up_Dwn_MovementBothDown[type], 128);
    CheckAndSetStick(eControllerAction::CA_VEHICLE_STEER_LEFT, type, button, state->LeftStickX, m_bStickL_X_Rgh_Lft_MovementBothDown[type], -128);
    CheckAndSetStick(eControllerAction::CA_VEHICLE_STEER_RIGHT, type, button, state->LeftStickX, m_bStickL_X_Rgh_Lft_MovementBothDown[type], 128);
    CheckAndSetButton(eControllerAction::CA_VEHICLE_RADIO_STATION_UP, type, button, state->DPadUp);
    CheckAndSetButton(eControllerAction::CA_VEHICLE_RADIO_STATION_DOWN, type, button, state->DPadDown);
    CheckAndSetButton(eControllerAction::CA_VEHICLE_RADIO_TRACK_SKIP, type, button, state->m_bRadioTrackSkip);
    CheckAndSetButton(eControllerAction::CA_TOGGLE_SUBMISSIONS, type, button, state->ShockButtonR);
    CheckAndSetStick(eControllerAction::CA_VEHICLE_TURRETLEFT, type, button, state->RightStickX, m_bStickR_X_Rgh_Lft_MovementBothDown[type], -128);
    CheckAndSetStick(eControllerAction::CA_VEHICLE_TURRETRIGHT, type, button, state->RightStickX, m_bStickR_X_Rgh_Lft_MovementBothDown[type], 128);
    CheckAndSetStick(eControllerAction::CA_VEHICLE_TURRETUP, type, button, state->RightStickY, m_bStickR_Up_Dwn_MovementBothDown[type], 128);
    CheckAndSetStick(eControllerAction::CA_VEHICLE_TURRETDOWN, type, button, state->RightStickY, m_bStickR_Up_Dwn_MovementBothDown[type], -128);
}

// 0x52F5F0
void CControllerConfigManager::ResetSettingOrder(eControllerAction action) {
    eControllerType result = KEYBOARD;

    for (const auto& order : CONTROLLER_ORDERS_SET) {
        // Check if any key already has this priority level
        bool priorityExists = false;

        for (const auto& type : CONTROLLER_TYPES_ALL) {
            if (m_Actions[action].Keys[type].m_uiSetOrder == order) {
                priorityExists = true;
                break;
            }
        }

        if (!priorityExists) {
            // No key has this priority, find the one with lowest priority > priorityLevel
            bool found = false;

            for (const auto& type : CONTROLLER_TYPES_ALL) {
                const auto keyPriority = m_Actions[action].Keys[type].m_uiSetOrder;

                // Only consider keys with priority > priorityLevel and not 0 (unset)
                if (keyPriority > order && keyPriority != 0) {
                    // If no key found yet or this key has lower priority than current best
                    if (!found || keyPriority < m_Actions[action].Keys[result].m_uiSetOrder) {
                        result = type;
                        found = true;
                    }
                }
            }

            // If we found a key with higher priority, adjust it to the current level
            if (found) {
                m_Actions[action].Keys[result].m_uiSetOrder = (eContSetOrder)order;
            }
        }
    }
}

// NOTSA [Code combined from 0x7448B0 and 0x744930]
// void CControllerConfigManager::HandleJoyButtonUpDown(int32 joyNo, bool isDown) {
//     UpdateJoyButtonState(joyNo);
//     const auto forceConfigMenuMode = !isDown && notsa::contains({ MODE_FLYBY, MODE_FIXED }, TheCamera.GetActiveCamera().m_nMode); // Probably leftover debug stuff?
//     for (auto i = isDown ? 1u : 2u; i < std::size(m_ButtonStates); i++) { // TODO: Why is this starting from 1/2?
//         const auto padBtn = ((m_ButtonStates[i - 1] == isDown) ? i : 0); // This doesn't make sense
//         if (forceConfigMenuMode || FrontEndMenuManager.m_bMenuActive || joyNo != 0) {
//             if (isDown) {
//                 UpdateJoyInConfigMenus_ButtonDown(padBtn, joyNo);
//             } else {
//                 UpdateJoyInConfigMenus_ButtonUp(padBtn, joyNo);
//             }
//         } else {
//             if (isDown) {
//                 UpdateJoy_ButtonDown(padBtn, eControllerType::JOY_STICK);
//             } else {
//                 AffectControllerStateOn_ButtonUp(padBtn, eControllerType::JOY_STICK);
//             }
//         }
//     }
// }

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
        for (auto action = 0u; action < NUM_OF_MAX_CONTROLLER_ACTIONS; action++) {
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
        for (auto action = 0u; action < NUM_OF_MAX_CONTROLLER_ACTIONS; action++) {
            // Skip action ID
            CFileMgr::Seek(file, 4, 1);
            
            // Read key and order for this action
            CFileMgr::Read(file, &m_Actions[action].Keys[type], 8);
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
        for (int32 actionId = 0; actionId < NUM_OF_MAX_CONTROLLER_ACTIONS; actionId++) {
            // Write action ID
            CFileMgr::Write(file, &actionId, 4);
            
            // Write key mapping data
            CFileMgr::Write(file, &m_Actions[actionId].Keys[type], 8);
        }
    }

    return true;
}

// 0x530640
void CControllerConfigManager::InitDefaultControlConfiguration() {
    SetControllerKeyAssociatedWithAction(eControllerAction::CA_GO_FORWARD, rsUP, eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(eControllerAction::CA_GO_BACK, rsDOWN, eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(eControllerAction::CA_GO_LEFT, rsLEFT, eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(eControllerAction::CA_GO_RIGHT, rsRIGHT, eControllerType::KEYBOARD);

    if (FrontEndMenuManager.m_nTextLanguage == 2) {
        SetControllerKeyAssociatedWithAction(eControllerAction::CA_CONVERSATION_YES, (RsKeyCodes)'Z', eControllerType::KEYBOARD);
        SetControllerKeyAssociatedWithAction(eControllerAction::CA_PED_SNIPER_ZOOM_IN, (RsKeyCodes)'Y', eControllerType::OPTIONAL_EXTRA_KEY);
    } else {
        SetControllerKeyAssociatedWithAction(eControllerAction::CA_CONVERSATION_YES, (RsKeyCodes)'Y', eControllerType::KEYBOARD);
        SetControllerKeyAssociatedWithAction(eControllerAction::CA_PED_SNIPER_ZOOM_IN, (RsKeyCodes)'X', eControllerType::OPTIONAL_EXTRA_KEY);
    }

    if (FrontEndMenuManager.m_nTextLanguage == 1) {
        SetControllerKeyAssociatedWithAction(eControllerAction::CA_GO_FORWARD, (RsKeyCodes)'Z', eControllerType::OPTIONAL_EXTRA_KEY);
        SetControllerKeyAssociatedWithAction(eControllerAction::CA_GO_LEFT, (RsKeyCodes)'Q', eControllerType::OPTIONAL_EXTRA_KEY);
        SetControllerKeyAssociatedWithAction(eControllerAction::CA_PED_CYCLE_WEAPON_LEFT, (RsKeyCodes)'A', eControllerType::KEYBOARD);
        SetControllerKeyAssociatedWithAction(eControllerAction::CA_PED_SNIPER_ZOOM_OUT, (RsKeyCodes)'W', eControllerType::OPTIONAL_EXTRA_KEY);
    } else {
        SetControllerKeyAssociatedWithAction(eControllerAction::CA_GO_FORWARD, (RsKeyCodes)'W', eControllerType::OPTIONAL_EXTRA_KEY);
        SetControllerKeyAssociatedWithAction(eControllerAction::CA_GO_LEFT, (RsKeyCodes)'A', eControllerType::OPTIONAL_EXTRA_KEY);
        SetControllerKeyAssociatedWithAction(eControllerAction::CA_PED_CYCLE_WEAPON_LEFT, (RsKeyCodes)'Q', eControllerType::KEYBOARD);
        SetControllerKeyAssociatedWithAction(eControllerAction::CA_PED_SNIPER_ZOOM_OUT, (RsKeyCodes)'Z', eControllerType::OPTIONAL_EXTRA_KEY);
    }

    SetControllerKeyAssociatedWithAction(eControllerAction::CA_GO_BACK, (RsKeyCodes)'S', eControllerType::OPTIONAL_EXTRA_KEY);
    SetControllerKeyAssociatedWithAction(eControllerAction::CA_GO_RIGHT, (RsKeyCodes)'D', eControllerType::OPTIONAL_EXTRA_KEY);
    SetControllerKeyAssociatedWithAction(eControllerAction::CA_PED_CYCLE_WEAPON_RIGHT, (RsKeyCodes)'E', eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(eControllerAction::CA_PED_CYCLE_WEAPON_RIGHT, rsPADENTER, eControllerType::OPTIONAL_EXTRA_KEY);
    SetControllerKeyAssociatedWithAction(eControllerAction::CA_PED_CYCLE_WEAPON_LEFT, rsPADDEL, eControllerType::OPTIONAL_EXTRA_KEY);
    SetControllerKeyAssociatedWithAction(eControllerAction::CA_PED_FIRE_WEAPON, rsPADINS, eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(eControllerAction::CA_PED_FIRE_WEAPON, rsLCTRL, eControllerType::OPTIONAL_EXTRA_KEY);
    SetControllerKeyAssociatedWithAction(eControllerAction::CA_PED_SNIPER_ZOOM_IN, rsPGUP, eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(eControllerAction::CA_PED_SNIPER_ZOOM_OUT, rsPGDN, eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(eControllerAction::CA_PED_FIRE_WEAPON_ALT, notsa::IsFixBugs() ? (RsKeyCodes)'0' : (RsKeyCodes)'\\', eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(eControllerAction::CA_GROUP_CONTROL_FWD, (RsKeyCodes)'G', eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(eControllerAction::CA_GROUP_CONTROL_BWD, (RsKeyCodes)'H', eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(eControllerAction::CA_CONVERSATION_NO, (RsKeyCodes)'N', eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(eControllerAction::CA_VEHICLE_ENTER_EXIT, (RsKeyCodes)'F', eControllerType::OPTIONAL_EXTRA_KEY);
    SetControllerKeyAssociatedWithAction(eControllerAction::CA_VEHICLE_ENTER_EXIT, rsENTER, eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(eControllerAction::CA_CAMERA_CHANGE_VIEW_ALL_SITUATIONS, (RsKeyCodes)'V', eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(eControllerAction::CA_CAMERA_CHANGE_VIEW_ALL_SITUATIONS, rsHOME, eControllerType::OPTIONAL_EXTRA_KEY);
    SetControllerKeyAssociatedWithAction(eControllerAction::CA_PED_JUMPING, rsLSHIFT, eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(eControllerAction::CA_PED_JUMPING, rsRCTRL, eControllerType::OPTIONAL_EXTRA_KEY);
    SetControllerKeyAssociatedWithAction(eControllerAction::CA_PED_SPRINT, (RsKeyCodes)' ', eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(eControllerAction::CA_PED_DUCK, (RsKeyCodes)'C', eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(eControllerAction::CA_PED_ANSWER_PHONE, rsTAB, eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(eControllerAction::CA_PED_WALK, rsLALT, eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(eControllerAction::CA_PED_LOCK_TARGET, rsDEL, eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(eControllerAction::CA_PED_LOCK_TARGET, rsCAPSLK, eControllerType::OPTIONAL_EXTRA_KEY);
    SetControllerKeyAssociatedWithAction(eControllerAction::CA_PED_1RST_PERSON_LOOK_LEFT, rsPADLEFT, eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(eControllerAction::CA_PED_1RST_PERSON_LOOK_RIGHT, rsPADRIGHT, eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(eControllerAction::CA_PED_1RST_PERSON_LOOK_UP, rsPADDOWN, eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(eControllerAction::CA_PED_1RST_PERSON_LOOK_DOWN, rsPADUP, eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(eControllerAction::CA_PED_CENTER_CAMERA_BEHIND_PLAYER, !notsa::IsFixBugs() ? (RsKeyCodes)'3' : (RsKeyCodes)'#', eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(eControllerAction::CA_PED_LOOKBEHIND, rsPADEND, eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(eControllerAction::CA_VEHICLE_STEER_UP, rsUP, eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(eControllerAction::CA_VEHICLE_STEER_DOWN, rsDOWN, eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(eControllerAction::CA_VEHICLE_RADIO_STATION_UP, rsINS, eControllerType::KEYBOARD);

    if (FrontEndMenuManager.m_nTextLanguage == 1) {
        SetControllerKeyAssociatedWithAction(eControllerAction::CA_VEHICLE_STEER_LEFT, (RsKeyCodes)'Q', eControllerType::KEYBOARD);
        SetControllerKeyAssociatedWithAction(eControllerAction::CA_VEHICLE_LOOKLEFT, (RsKeyCodes)'A', eControllerType::KEYBOARD);
        SetControllerKeyAssociatedWithAction(eControllerAction::CA_VEHICLE_RADIO_STATION_UP, (RsKeyCodes)'`', eControllerType::OPTIONAL_EXTRA_KEY);
        SetControllerKeyAssociatedWithAction(eControllerAction::CA_TOGGLE_SUBMISSIONS, (RsKeyCodes)233, eControllerType::KEYBOARD);
        SetControllerKeyAssociatedWithAction(eControllerAction::CA_VEHICLE_ACCELERATE, (RsKeyCodes)'Z', eControllerType::KEYBOARD);
    } else {
        SetControllerKeyAssociatedWithAction(eControllerAction::CA_VEHICLE_STEER_LEFT, (RsKeyCodes)'A', eControllerType::KEYBOARD);
        SetControllerKeyAssociatedWithAction(eControllerAction::CA_VEHICLE_LOOKLEFT, (RsKeyCodes)'Q', eControllerType::KEYBOARD);
        SetControllerKeyAssociatedWithAction(eControllerAction::CA_VEHICLE_RADIO_STATION_UP, (RsKeyCodes)'4', eControllerType::OPTIONAL_EXTRA_KEY);
        SetControllerKeyAssociatedWithAction(eControllerAction::CA_TOGGLE_SUBMISSIONS, (RsKeyCodes)'2', eControllerType::KEYBOARD);
        SetControllerKeyAssociatedWithAction(eControllerAction::CA_VEHICLE_ACCELERATE, (RsKeyCodes)'W', eControllerType::KEYBOARD);
    }

    SetControllerKeyAssociatedWithAction(eControllerAction::CA_TOGGLE_SUBMISSIONS, rsPLUS, eControllerType::OPTIONAL_EXTRA_KEY);
    SetControllerKeyAssociatedWithAction(eControllerAction::CA_VEHICLE_FIRE_WEAPON, rsRCTRL, eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(eControllerAction::CA_VEHICLE_FIRE_WEAPON, rsLALT, eControllerType::OPTIONAL_EXTRA_KEY);
    SetControllerKeyAssociatedWithAction(eControllerAction::CA_VEHICLE_FIRE_WEAPON_ALT, rsLCTRL, eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(eControllerAction::CA_VEHICLE_FIRE_WEAPON_ALT, rsPADINS, eControllerType::OPTIONAL_EXTRA_KEY);
    SetControllerKeyAssociatedWithAction(eControllerAction::CA_VEHICLE_BRAKE, (RsKeyCodes)'S', eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(eControllerAction::CA_VEHICLE_STEER_RIGHT, (RsKeyCodes)'D', eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(eControllerAction::CA_VEHICLE_STEER_LEFT, rsLEFT, eControllerType::OPTIONAL_EXTRA_KEY);
    SetControllerKeyAssociatedWithAction(eControllerAction::CA_VEHICLE_STEER_RIGHT, rsRIGHT, eControllerType::OPTIONAL_EXTRA_KEY);
    SetControllerKeyAssociatedWithAction(eControllerAction::CA_VEHICLE_HORN, rsCAPSLK, eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(eControllerAction::CA_VEHICLE_HORN, (RsKeyCodes)'H', eControllerType::OPTIONAL_EXTRA_KEY);
    SetControllerKeyAssociatedWithAction(eControllerAction::CA_VEHICLE_HANDBRAKE, (RsKeyCodes)' ', eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(eControllerAction::CA_VEHICLE_HANDBRAKE, rsRCTRL, eControllerType::OPTIONAL_EXTRA_KEY);
    SetControllerKeyAssociatedWithAction(eControllerAction::CA_VEHICLE_LOOKRIGHT, (RsKeyCodes)'E', eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(eControllerAction::CA_VEHICLE_TURRETLEFT, rsPADLEFT, eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(eControllerAction::CA_VEHICLE_TURRETRIGHT, rsPADRIGHT, eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(eControllerAction::CA_VEHICLE_TURRETUP, rsPADDOWN, eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(eControllerAction::CA_VEHICLE_TURRETDOWN, rsPADUP, eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(eControllerAction::CA_VEHICLE_TURRETUP, rsEND, eControllerType::OPTIONAL_EXTRA_KEY);
    SetControllerKeyAssociatedWithAction(eControllerAction::CA_VEHICLE_TURRETDOWN, rsDEL, eControllerType::OPTIONAL_EXTRA_KEY);
    SetControllerKeyAssociatedWithAction(eControllerAction::CA_VEHICLE_RADIO_TRACK_SKIP, rsF5, eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(eControllerAction::CA_VEHICLE_RADIO_STATION_DOWN, rsDEL, eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(eControllerAction::CA_VEHICLE_RADIO_STATION_DOWN, (RsKeyCodes)'R', eControllerType::OPTIONAL_EXTRA_KEY);
    SetControllerKeyAssociatedWithAction(eControllerAction::CA_PED_CYCLE_TARGET_LEFT, (RsKeyCodes)'[', eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(eControllerAction::CA_PED_CYCLE_TARGET_RIGHT, (RsKeyCodes)']', eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(eControllerAction::CA_PED_CYCLE_TARGET_RIGHT, (RsKeyCodes)']', eControllerType::KEYBOARD);

    ClearSimButtonPressCheckers();
}

// 0x530B00
void CControllerConfigManager::InitDefaultControlConfigJoyPad(uint32 buttonCount) {
    m_bJoyJustInitialised = true;

    buttonCount = std::min<uint32>(buttonCount, JOYBUTTON_MAX);

    // Define all possible button mappings in order from highest to lowest button number
    using ButtonMapping = std::pair<eJoyButtons, eControllerAction>;

    constexpr int mappingCount = 29;

    // Arrays for specific and standard controller configurations
    // constexpr ButtonMapping modernMapping[mappingCount] = {
    //     { JOYBUTTON_SIXTEEN,   eControllerAction::CA_CONVERSATION_NO                   },
    //     { JOYBUTTON_FIFTHTEEN, eControllerAction::CA_GROUP_CONTROL_BWD                 },
    //     { JOYBUTTON_FIFTHTEEN, eControllerAction::CA_VEHICLE_RADIO_STATION_DOWN        },
    //     { JOYBUTTON_FOURTEEN,  eControllerAction::CA_CONVERSATION_YES                  },
    //     { JOYBUTTON_THIRTEEN,  eControllerAction::CA_GROUP_CONTROL_FWD                 },
    //     { JOYBUTTON_THIRTEEN,  eControllerAction::CA_VEHICLE_RADIO_STATION_UP          },
    //     { JOYBUTTON_ELEVEN,    eControllerAction::CA_PED_LOOKBEHIND                    },
    //     { JOYBUTTON_ELEVEN,    eControllerAction::CA_TOGGLE_SUBMISSIONS                },
    //     { JOYBUTTON_TEN,       eControllerAction::CA_VEHICLE_HORN                      },
    //     { JOYBUTTON_TEN,       eControllerAction::CA_PED_DUCK                          },
    //     { JOYBUTTON_NINE,      eControllerAction::CA_CAMERA_CHANGE_VIEW_ALL_SITUATIONS },
    //     { JOYBUTTON_EIGHT,     eControllerAction::CA_VEHICLE_HANDBRAKE                 },
    //     { JOYBUTTON_EIGHT,     eControllerAction::CA_PED_LOCK_TARGET                   },
    //     { JOYBUTTON_SEVEN,     eControllerAction::CA_PED_ANSWER_PHONE                  },
    //     { JOYBUTTON_SEVEN,     eControllerAction::CA_PED_FIRE_WEAPON_ALT               },
    //     { JOYBUTTON_SEVEN,     eControllerAction::CA_VEHICLE_FIRE_WEAPON_ALT           },
    //     { JOYBUTTON_SIX,       eControllerAction::CA_PED_CYCLE_WEAPON_RIGHT            },
    //     { JOYBUTTON_SIX,       eControllerAction::CA_VEHICLE_LOOKRIGHT                 },
    //     { JOYBUTTON_FIVE,      eControllerAction::CA_PED_CYCLE_WEAPON_LEFT             },
    //     { JOYBUTTON_FIVE,      eControllerAction::CA_VEHICLE_LOOKLEFT                  },
    //     { JOYBUTTON_FOUR,      eControllerAction::CA_VEHICLE_BRAKE                     },
    //     { JOYBUTTON_FOUR,      eControllerAction::CA_PED_JUMPING                       },
    //     { JOYBUTTON_FOUR,      eControllerAction::CA_PED_SNIPER_ZOOM_IN                },
    //     { JOYBUTTON_THREE,     eControllerAction::CA_VEHICLE_ACCELERATE                },
    //     { JOYBUTTON_THREE,     eControllerAction::CA_PED_SPRINT                        },
    //     { JOYBUTTON_THREE,     eControllerAction::CA_PED_SNIPER_ZOOM_OUT               },
    //     { JOYBUTTON_TWO,       eControllerAction::CA_PED_FIRE_WEAPON                   },
    //     { JOYBUTTON_TWO,       eControllerAction::CA_VEHICLE_FIRE_WEAPON               },
    //     { JOYBUTTON_ONE,       eControllerAction::CA_VEHICLE_ENTER_EXIT                }
    // };

    // Arrays for specific and standard controller configurations
    constexpr ButtonMapping specificMappings[mappingCount] = {
        { JOYBUTTON_SIXTEEN,   eControllerAction::CA_CONVERSATION_NO                   },
        { JOYBUTTON_FIFTHTEEN, eControllerAction::CA_GROUP_CONTROL_BWD                 },
        { JOYBUTTON_FIFTHTEEN, eControllerAction::CA_VEHICLE_RADIO_STATION_DOWN        },
        { JOYBUTTON_FOURTEEN,  eControllerAction::CA_CONVERSATION_YES                  },
        { JOYBUTTON_THIRTEEN,  eControllerAction::CA_GROUP_CONTROL_FWD                 },
        { JOYBUTTON_THIRTEEN,  eControllerAction::CA_VEHICLE_RADIO_STATION_UP          },
        { JOYBUTTON_ELEVEN,    eControllerAction::CA_PED_LOOKBEHIND                    },
        { JOYBUTTON_ELEVEN,    eControllerAction::CA_TOGGLE_SUBMISSIONS                },
        { JOYBUTTON_TEN,       eControllerAction::CA_VEHICLE_HORN                      },
        { JOYBUTTON_TEN,       eControllerAction::CA_PED_DUCK                          },
        { JOYBUTTON_NINE,      eControllerAction::CA_CAMERA_CHANGE_VIEW_ALL_SITUATIONS },
        { JOYBUTTON_EIGHT,     eControllerAction::CA_VEHICLE_HANDBRAKE                 },
        { JOYBUTTON_EIGHT,     eControllerAction::CA_PED_LOCK_TARGET                   },
        { JOYBUTTON_SEVEN,     eControllerAction::CA_PED_ANSWER_PHONE                  },
        { JOYBUTTON_SEVEN,     eControllerAction::CA_PED_FIRE_WEAPON_ALT               },
        { JOYBUTTON_SEVEN,     eControllerAction::CA_VEHICLE_FIRE_WEAPON_ALT           },
        { JOYBUTTON_SIX,       eControllerAction::CA_PED_CYCLE_WEAPON_RIGHT            },
        { JOYBUTTON_SIX,       eControllerAction::CA_VEHICLE_LOOKRIGHT                 },
        { JOYBUTTON_FIVE,      eControllerAction::CA_PED_CYCLE_WEAPON_LEFT             },
        { JOYBUTTON_FIVE,      eControllerAction::CA_VEHICLE_LOOKLEFT                  },
        { JOYBUTTON_FOUR,      eControllerAction::CA_VEHICLE_BRAKE                     },
        { JOYBUTTON_FOUR,      eControllerAction::CA_PED_JUMPING                       },
        { JOYBUTTON_FOUR,      eControllerAction::CA_PED_SNIPER_ZOOM_IN                },
        { JOYBUTTON_THREE,     eControllerAction::CA_VEHICLE_ACCELERATE                },
        { JOYBUTTON_THREE,     eControllerAction::CA_PED_SPRINT                        },
        { JOYBUTTON_THREE,     eControllerAction::CA_PED_SNIPER_ZOOM_OUT               },
        { JOYBUTTON_TWO,       eControllerAction::CA_PED_FIRE_WEAPON                   },
        { JOYBUTTON_TWO,       eControllerAction::CA_VEHICLE_FIRE_WEAPON               },
        { JOYBUTTON_ONE,       eControllerAction::CA_VEHICLE_ENTER_EXIT                }
    };

    constexpr ButtonMapping standardMappings[mappingCount] = {
        { JOYBUTTON_SIXTEEN,   eControllerAction::CA_CONVERSATION_NO                   },
        { JOYBUTTON_FIFTHTEEN, eControllerAction::CA_GROUP_CONTROL_BWD                 },
        { JOYBUTTON_FIFTHTEEN, eControllerAction::CA_VEHICLE_RADIO_STATION_DOWN        },
        { JOYBUTTON_FOURTEEN,  eControllerAction::CA_CONVERSATION_YES                  },
        { JOYBUTTON_THIRTEEN,  eControllerAction::CA_GROUP_CONTROL_FWD                 },
        { JOYBUTTON_THIRTEEN,  eControllerAction::CA_VEHICLE_RADIO_STATION_UP          },
        { JOYBUTTON_ELEVEN,    eControllerAction::CA_PED_LOOKBEHIND                    },
        { JOYBUTTON_ELEVEN,    eControllerAction::CA_TOGGLE_SUBMISSIONS                },
        { JOYBUTTON_TEN,       eControllerAction::CA_VEHICLE_HORN                      },
        { JOYBUTTON_TEN,       eControllerAction::CA_PED_DUCK                          },
        { JOYBUTTON_NINE,      eControllerAction::CA_CAMERA_CHANGE_VIEW_ALL_SITUATIONS },
        { JOYBUTTON_EIGHT,     eControllerAction::CA_VEHICLE_HANDBRAKE                 },
        { JOYBUTTON_EIGHT,     eControllerAction::CA_PED_LOCK_TARGET                   },
        { JOYBUTTON_SEVEN,     eControllerAction::CA_PED_ANSWER_PHONE                  },
        { JOYBUTTON_SEVEN,     eControllerAction::CA_PED_FIRE_WEAPON_ALT               },
        { JOYBUTTON_SEVEN,     eControllerAction::CA_VEHICLE_FIRE_WEAPON_ALT           },
        { JOYBUTTON_SIX,       eControllerAction::CA_PED_CYCLE_WEAPON_RIGHT            },
        { JOYBUTTON_SIX,       eControllerAction::CA_VEHICLE_LOOKRIGHT                 },
        { JOYBUTTON_FIVE,      eControllerAction::CA_PED_CYCLE_WEAPON_LEFT             },
        { JOYBUTTON_FIVE,      eControllerAction::CA_VEHICLE_LOOKLEFT                  },
        { JOYBUTTON_FOUR,      eControllerAction::CA_VEHICLE_ENTER_EXIT                },
        { JOYBUTTON_THREE,     eControllerAction::CA_VEHICLE_BRAKE                     },
        { JOYBUTTON_THREE,     eControllerAction::CA_PED_JUMPING                       },
        { JOYBUTTON_THREE,     eControllerAction::CA_PED_SNIPER_ZOOM_IN                },
        { JOYBUTTON_TWO,       eControllerAction::CA_VEHICLE_ACCELERATE                },
        { JOYBUTTON_TWO,       eControllerAction::CA_PED_SPRINT                        },
        { JOYBUTTON_TWO,       eControllerAction::CA_PED_SNIPER_ZOOM_OUT               },
        { JOYBUTTON_ONE,       eControllerAction::CA_PED_FIRE_WEAPON                   },
        { JOYBUTTON_ONE,       eControllerAction::CA_VEHICLE_FIRE_WEAPON               }
    };

    // constexpr modernMappingOn = true; // This is a placeholder, replace with actual condition to check if modern mapping is needed

    // if (!modernMappingOn) {
        // Choose which mapping array to use
        const auto& mappings = (AllValidWinJoys.JoyStickNum[PAD1].wVendorID == 0x3427 && AllValidWinJoys.JoyStickNum[PAD1].wProductID == 0x1190) ? specificMappings : standardMappings;

    // } else {
    //     // Use modern mapping array
    //     const auto& mappings = modernMapping;
    // }

    for (size_t i = 0; i < std::size(mappings); ++i) {
        if (mappings[i].first <= buttonCount) {
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
        SetMouseButtonAssociatedWithAction(eControllerAction::CA_PED_FIRE_WEAPON,            rsMOUSE_LEFT_BUTTON);
        SetMouseButtonAssociatedWithAction(eControllerAction::CA_VEHICLE_FIRE_WEAPON,        rsMOUSE_LEFT_BUTTON);
    }
    if (MouseSetUp.isMouseRightButtonPressed || isForcedMouseBlinding) {                                                      
        SetMouseButtonAssociatedWithAction(eControllerAction::CA_PED_LOCK_TARGET,            rsMOUSE_RIGHT_BUTTON);
        SetMouseButtonAssociatedWithAction(eControllerAction::CA_VEHICLE_MOUSELOOK,          rsMOUSE_RIGHT_BUTTON);
    }
    if (MouseSetUp.isMouseMiddleButtonPressed || isForcedMouseBlinding) {                                                      
        SetMouseButtonAssociatedWithAction(eControllerAction::CA_VEHICLE_LOOKBEHIND,         rsMOUSE_MIDDLE_BUTTON);
        SetMouseButtonAssociatedWithAction(eControllerAction::CA_PED_LOOKBEHIND,             rsMOUSE_MIDDLE_BUTTON);
    }
    if (MouseSetUp.isMouseWheelMovedUp || MouseSetUp.isMouseWheelMovedDown || isForcedMouseBlinding) {
        SetMouseButtonAssociatedWithAction(eControllerAction::CA_PED_CYCLE_WEAPON_LEFT,      rsMOUSE_WHEEL_UP_BUTTON);
        SetMouseButtonAssociatedWithAction(eControllerAction::CA_PED_CYCLE_WEAPON_RIGHT,     rsMOUSE_WHEEL_DOWN_BUTTON);
        SetMouseButtonAssociatedWithAction(eControllerAction::CA_VEHICLE_RADIO_STATION_UP,   rsMOUSE_WHEEL_UP_BUTTON);
        SetMouseButtonAssociatedWithAction(eControllerAction::CA_VEHICLE_RADIO_STATION_DOWN, rsMOUSE_WHEEL_DOWN_BUTTON);
        SetMouseButtonAssociatedWithAction(eControllerAction::CA_PED_SNIPER_ZOOM_IN,         rsMOUSE_WHEEL_UP_BUTTON);
        SetMouseButtonAssociatedWithAction(eControllerAction::CA_PED_SNIPER_ZOOM_OUT,        rsMOUSE_WHEEL_DOWN_BUTTON);
    }

    // This assert maybe is in the original game, but probably is missing by release build.
    // Prevents 'wrong' vehicle keys init. In cases where the mouse starts incorrectly.
    assert(m_MouseFoundInitSet == bMouseControls || !bMouseControls );
}

// 0x52D260
void CControllerConfigManager::InitialiseControllerActionNameArray() {
    GxtCharStrcpy(m_ControllerActionName[eControllerAction::CA_PED_LOOKBEHIND], "PED_LOOKBEHIND"_gxt);
    GxtCharStrcpy(m_ControllerActionName[eControllerAction::CA_PED_CYCLE_WEAPON_LEFT], "PED_CYCLE_WEAPON_LEFT"_gxt);
    GxtCharStrcpy(m_ControllerActionName[eControllerAction::CA_PED_CYCLE_WEAPON_RIGHT], "PED_CYCLE_WEAPON_RIGHT"_gxt);
    GxtCharStrcpy(m_ControllerActionName[eControllerAction::CA_PED_LOCK_TARGET], "PED_LOCK_TARGET"_gxt);
    GxtCharStrcpy(m_ControllerActionName[eControllerAction::CA_PED_JUMPING], "PED_JUMPING"_gxt);
    GxtCharStrcpy(m_ControllerActionName[eControllerAction::CA_PED_SPRINT], "PED_SPRINT"_gxt);
    GxtCharStrcpy(m_ControllerActionName[eControllerAction::CA_PED_WALK], "SNEAK_ABOUT"_gxt);
    GxtCharStrcpy(m_ControllerActionName[eControllerAction::CA_PED_CYCLE_TARGET_LEFT], "PED_CYCLE_TARGET_LEFT"_gxt);
    GxtCharStrcpy(m_ControllerActionName[eControllerAction::CA_PED_CYCLE_TARGET_RIGHT], "PED_CYCLE_TARGET_RIGHT"_gxt);
    GxtCharStrcpy(m_ControllerActionName[eControllerAction::CA_PED_CENTER_CAMERA_BEHIND_PLAYER], "PED_CENTER_CAMERA_BEHIND_PLAYER"_gxt);
    GxtCharStrcpy(m_ControllerActionName[eControllerAction::CA_VEHICLE_LOOKBEHIND], "VEHICLE_LOOKBEHIND"_gxt);
    GxtCharStrcpy(m_ControllerActionName[eControllerAction::CA_PED_DUCK], "PED_DUCK"_gxt);
    GxtCharStrcpy(m_ControllerActionName[eControllerAction::CA_PED_ANSWER_PHONE], "PED_ANSWER_PHONE"_gxt);
    GxtCharStrcpy(m_ControllerActionName[eControllerAction::CA_VEHICLE_STEER_LEFT], "VEHICLE_STEERLEFT"_gxt);
    GxtCharStrcpy(m_ControllerActionName[eControllerAction::CA_VEHICLE_STEER_RIGHT], "VEHICLE_STEERRIGHT"_gxt);
    GxtCharStrcpy(m_ControllerActionName[eControllerAction::CA_VEHICLE_STEER_UP], "VEHICLE_STEERUP"_gxt);
    GxtCharStrcpy(m_ControllerActionName[eControllerAction::CA_VEHICLE_STEER_DOWN], "VEHICLE_STEERDOWN"_gxt);
    GxtCharStrcpy(m_ControllerActionName[eControllerAction::CA_VEHICLE_LOOKLEFT], "VEHICLE_LOOKLEFT"_gxt);
    GxtCharStrcpy(m_ControllerActionName[eControllerAction::CA_VEHICLE_LOOKRIGHT], "VEHICLE_LOOKRIGHT"_gxt);
    GxtCharStrcpy(m_ControllerActionName[eControllerAction::CA_VEHICLE_MOUSELOOK], "VEHICLE_MOUSELOOK"_gxt);
    GxtCharStrcpy(m_ControllerActionName[eControllerAction::CA_VEHICLE_HORN], "VEHICLE_HORN"_gxt);
    GxtCharStrcpy(m_ControllerActionName[eControllerAction::CA_VEHICLE_HANDBRAKE], "VEHICLE_HANDBRAKE"_gxt);
    GxtCharStrcpy(m_ControllerActionName[eControllerAction::CA_VEHICLE_ACCELERATE], "VEHICLE_ACCELERATE"_gxt);
    GxtCharStrcpy(m_ControllerActionName[eControllerAction::CA_VEHICLE_BRAKE], "VEHICLE_BRAKE"_gxt);
    GxtCharStrcpy(m_ControllerActionName[eControllerAction::CA_VEHICLE_RADIO_STATION_UP], "VEHICLE_RADIO_STATION_UP"_gxt);
    GxtCharStrcpy(m_ControllerActionName[eControllerAction::CA_VEHICLE_RADIO_STATION_DOWN], "VEHICLE_RADIO_STATION_DOWN"_gxt);
    if (notsa::IsFixBugs()) { // Fix: Missing on vanilla game
        GxtCharStrcpy(m_ControllerActionName[eControllerAction::CA_VEHICLE_RADIO_TRACK_SKIP], "VEHICLE_RADIO_TRACK_SKIP"_gxt);
    }
    GxtCharStrcpy(m_ControllerActionName[eControllerAction::CA_TOGGLE_SUBMISSIONS], "TOGGLE_SUBMISSIONS"_gxt);
    GxtCharStrcpy(m_ControllerActionName[eControllerAction::CA_PED_SNIPER_ZOOM_IN], "PED_SNIPER_ZOOM_IN"_gxt);
    GxtCharStrcpy(m_ControllerActionName[eControllerAction::CA_PED_SNIPER_ZOOM_OUT], "PED_SNIPER_ZOOM_OUT"_gxt);
    GxtCharStrcpy(m_ControllerActionName[eControllerAction::CA_PED_1RST_PERSON_LOOK_LEFT], "PED_1RST_PERSON_LOOK_LEFT"_gxt);
    GxtCharStrcpy(m_ControllerActionName[eControllerAction::CA_PED_1RST_PERSON_LOOK_RIGHT], "PED_1RST_PERSON_LOOK_RIGHT"_gxt);
    GxtCharStrcpy(m_ControllerActionName[eControllerAction::CA_PED_1RST_PERSON_LOOK_UP], "PED_1RST_PERSON_LOOK_UP"_gxt);
    GxtCharStrcpy(m_ControllerActionName[eControllerAction::CA_PED_1RST_PERSON_LOOK_DOWN], "PED_1RST_PERSON_LOOK_DOWN"_gxt);
    GxtCharStrcpy(m_ControllerActionName[eControllerAction::CA_SHOW_MOUSE_POINTER_TOGGLE], "SHOW_MOUSE_POINTER_TOGGLE"_gxt);
    GxtCharStrcpy(m_ControllerActionName[eControllerAction::CA_CAMERA_CHANGE_VIEW_ALL_SITUATIONS], "CAMERA_CHANGE_VIEW_ALL_SITUATIONS"_gxt);
    GxtCharStrcpy(m_ControllerActionName[eControllerAction::CA_PED_FIRE_WEAPON], "PED_FIREWEAPON"_gxt);
    GxtCharStrcpy(m_ControllerActionName[eControllerAction::CA_PED_FIRE_WEAPON_ALT], "PED_FIREWEAPON_ALT"_gxt);
    GxtCharStrcpy(m_ControllerActionName[eControllerAction::CA_VEHICLE_FIRE_WEAPON], "VEHICLE_FIREWEAPON"_gxt);
    GxtCharStrcpy(m_ControllerActionName[eControllerAction::CA_VEHICLE_FIRE_WEAPON_ALT], "VEHICLE_FIREWEAPON_ALT"_gxt);
    GxtCharStrcpy(m_ControllerActionName[eControllerAction::CA_VEHICLE_ENTER_EXIT], "VEHICLE_ENTER_EXIT"_gxt);
    GxtCharStrcpy(m_ControllerActionName[eControllerAction::CA_CONVERSATION_NO], "CONVERSATION_NO"_gxt);
    GxtCharStrcpy(m_ControllerActionName[eControllerAction::CA_CONVERSATION_YES], "CONVERSATION_YES"_gxt);
    GxtCharStrcpy(m_ControllerActionName[eControllerAction::CA_GROUP_CONTROL_FWD], "GROUP_CONTROL_FWD"_gxt);
    GxtCharStrcpy(m_ControllerActionName[eControllerAction::CA_GROUP_CONTROL_BWD], "GROUP_CONTROL_BWD"_gxt);
    GxtCharStrcpy(m_ControllerActionName[eControllerAction::CA_GO_LEFT], "GO_LEFT"_gxt);
    GxtCharStrcpy(m_ControllerActionName[eControllerAction::CA_GO_RIGHT], "GO_RIGHT"_gxt);
    GxtCharStrcpy(m_ControllerActionName[eControllerAction::CA_GO_FORWARD], "GO_FORWARD"_gxt);
    GxtCharStrcpy(m_ControllerActionName[eControllerAction::CA_GO_BACK], "GO_BACK"_gxt);
    GxtCharStrcpy(m_ControllerActionName[eControllerAction::CA_VEHICLE_TURRETLEFT], "VEHICLE_TURRETLEFT"_gxt);
    GxtCharStrcpy(m_ControllerActionName[eControllerAction::CA_VEHICLE_TURRETRIGHT], "VEHICLE_TURRETRIGHT"_gxt);
    GxtCharStrcpy(m_ControllerActionName[eControllerAction::CA_VEHICLE_TURRETUP], "VEHICLE_TURRETUP"_gxt);
    GxtCharStrcpy(m_ControllerActionName[eControllerAction::CA_VEHICLE_TURRETDOWN], "VEHICLE_TURRETDOWN"_gxt);
    GxtCharStrcpy(m_ControllerActionName[eControllerAction::CA_NETWORK_TALK], "NETWORK_TALK"_gxt);
    GxtCharStrcpy(m_ControllerActionName[eControllerAction::CA_TOGGLE_DPAD], "TOGGLE_DPAD"_gxt);
    GxtCharStrcpy(m_ControllerActionName[eControllerAction::CA_SWITCH_DEBUG_CAM_ON], "SWITCH_DEBUG_CAM_ON"_gxt);
    GxtCharStrcpy(m_ControllerActionName[eControllerAction::CA_TAKE_SCREEN_SHOT], "TAKE_SCREEN_SHOT"_gxt);
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
        ControlsManager.InitDefaultControlConfigJoyPad(JOYBUTTON_MAX);
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
    m_Actions[action].Keys[eControllerType::MOUSE].m_uiActionInitiator = button;
    m_Actions[action].Keys[eControllerType::MOUSE].m_uiSetOrder = (eContSetOrder)(numOfSettings + 1);
}

// unused
// 0x52DA30
void CControllerConfigManager::StoreMouseButtonState(eMouseButtons button, bool state) {
    switch (button) {
    case MOUSE_BUTTON_LEFT:           CPad::TempMouseControllerState.isMouseLeftButtonPressed = state; break;
    case MOUSE_BUTTON_MIDDLE:         CPad::TempMouseControllerState.isMouseMiddleButtonPressed = state; break;
    case MOUSE_BUTTON_RIGHT:          CPad::TempMouseControllerState.isMouseRightButtonPressed = state; break;
    case MOUSE_BUTTON_WHEEL_UP:       CPad::TempMouseControllerState.isMouseWheelMovedUp = state; break;
    case MOUSE_BUTTON_WHEEL_DOWN:     CPad::TempMouseControllerState.isMouseWheelMovedDown = state; break;
    case MOUSE_BUTTON_WHEEL_XBUTTON1: CPad::TempMouseControllerState.isMouseFirstXPressed = state; break;
    case MOUSE_BUTTON_WHEEL_XBUTTON2: CPad::TempMouseControllerState.isMouseSecondXPressed = state; break;
    default:                          NOTSA_UNREACHABLE();
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
void CControllerConfigManager::AffectControllerStateOn_ButtonDown_DebugStuff(int32, eControllerType) {
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
        HandleButtonRelease(button, type, &GetControllerState(*pad, type));
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
        m_bStickL_Up_Dwn_MovementBothDown[type]    = false;
        m_bStickL_X_Rgh_Lft_MovementBothDown[type] = false;
        m_bStickR_Up_Dwn_MovementBothDown[type]    = false;
        m_bStickR_X_Rgh_Lft_MovementBothDown[type] = false;
    }
}

// unused
// 0x52D1C0
eJoyButtons CControllerConfigManager::GetJoyButtonJustUp() {
    if (m_bJoyJustInitialised) {
        return eJoyButtons::NO_JOYBUTTONS;
    }
    
    // Check each button from 0 to JOYBUTTON_SIXTEEN
    for (auto buttonIndex = 0; buttonIndex < int32(notsa::IsFixBugs() ? JOYBUTTON_COUNT : JOYBUTTON_MAX); buttonIndex++) {
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
    if (m_bJoyJustInitialised) {
        return eJoyButtons::NO_JOYBUTTONS;
    }

    // Check each button from 0 to JOYBUTTON_SIXTEEN-1
    for (auto buttonIndex = 0; buttonIndex < int32(notsa::IsFixBugs() ? JOYBUTTON_COUNT : JOYBUTTON_MAX); buttonIndex++) {
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
constexpr inline bool CheckMouseButton(KeyCode &key) {
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
eActionType CControllerConfigManager::GetActionType(eControllerAction Action) {
    static constexpr auto firstThirdPersonActions = {CA_PED_FIRE_WEAPON, CA_PED_FIRE_WEAPON_ALT, CA_GO_FORWARD, CA_GO_BACK, CA_GO_LEFT, CA_GO_RIGHT, CA_PED_SNIPER_ZOOM_IN, CA_PED_SNIPER_ZOOM_OUT, CA_PED_1RST_PERSON_LOOK_LEFT, CA_PED_1RST_PERSON_LOOK_RIGHT, CA_PED_LOCK_TARGET, CA_PED_1RST_PERSON_LOOK_UP, CA_PED_1RST_PERSON_LOOK_DOWN};
    static constexpr auto thirdPersonActions = {CA_PED_CYCLE_WEAPON_RIGHT, CA_PED_CYCLE_WEAPON_LEFT, CA_PED_JUMPING, CA_PED_SPRINT, CA_PED_LOOKBEHIND, CA_PED_DUCK, CA_PED_ANSWER_PHONE, CA_PED_WALK, CA_PED_CYCLE_TARGET_LEFT, CA_PED_CYCLE_TARGET_RIGHT, CA_PED_CENTER_CAMERA_BEHIND_PLAYER, CA_CONVERSATION_YES, CA_CONVERSATION_NO, CA_GROUP_CONTROL_FWD, CA_GROUP_CONTROL_BWD};
    static constexpr auto commonControlActions = {CA_CAMERA_CHANGE_VIEW_ALL_SITUATIONS, CA_NETWORK_TALK, CA_TOGGLE_DPAD, CA_SWITCH_DEBUG_CAM_ON, CA_TAKE_SCREEN_SHOT, CA_SHOW_MOUSE_POINTER_TOGGLE};
    static constexpr auto inCarActions = {CA_VEHICLE_FIRE_WEAPON, CA_VEHICLE_FIRE_WEAPON_ALT, CA_VEHICLE_STEER_LEFT, CA_VEHICLE_STEER_RIGHT, CA_VEHICLE_STEER_UP, CA_VEHICLE_STEER_DOWN, CA_VEHICLE_ACCELERATE, CA_VEHICLE_BRAKE, CA_VEHICLE_RADIO_STATION_UP, CA_VEHICLE_RADIO_STATION_DOWN, CA_VEHICLE_RADIO_TRACK_SKIP, CA_VEHICLE_HORN, CA_TOGGLE_SUBMISSIONS, CA_VEHICLE_HANDBRAKE, CA_VEHICLE_LOOKLEFT, CA_VEHICLE_LOOKRIGHT, CA_VEHICLE_LOOKBEHIND, CA_VEHICLE_MOUSELOOK, CA_VEHICLE_TURRETLEFT, CA_VEHICLE_TURRETRIGHT, CA_VEHICLE_TURRETUP, CA_VEHICLE_TURRETDOWN};

    // Check if action is in any of the arrays
    if (notsa::contains(firstThirdPersonActions, Action)) {
        return eActionType::ACTION_FIRST_THIRD_PERSON;
    } else if (notsa::contains(thirdPersonActions, Action)) {
        return eActionType::ACTION_THIRD_PERSON;
    } else if (Action == eControllerAction::CA_VEHICLE_ENTER_EXIT) {
        return eActionType::ACTION_IN_CAR_THIRD_PERSON;
    } else if (notsa::contains(commonControlActions, Action)) {
        return eActionType::ACTION_COMMON_CONTROLS;
    } else if (notsa::contains(inCarActions, Action)) {
        return eActionType::ACTION_IN_CAR;
    }

    return eActionType::ACTION_NOT_TYPE;
}

// 0x52F390
const GxtChar* CControllerConfigManager::GetControllerSettingTextMouse(eControllerAction action) {
    const auto button = GetMouseButtonAssociatedWithAction(action);
    switch (button) {
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
    if (const auto keyCode = GetControllerKeyAssociatedWithAction(action, eControllerType::JOY_STICK); !GetIsKeyBlank(keyCode, eControllerType::JOY_STICK)) {
        CMessages::InsertNumberInString(TheText.Get("FEC_JBO"), keyCode, -1, -1, -1, -1, -1, NewStringWithNumber); // JOY~1~
        return NewStringWithNumber;
    }
    return nullptr; // Please not add 'NOTSA_UNREACHABLE' !!!
}

// unused
// 0x52F4A0
eContSetOrder CControllerConfigManager::GetNumOfSettingsForAction(eControllerAction action) {
    const auto& actionLocal = m_Actions[action];
    auto count = 0u;
    for (const auto& type : CONTROLLER_TYPES_ALL) {
        if (!GetIsKeyBlank(actionLocal.Keys[type].m_uiActionInitiator, type)) {
            count++;
        }
    }
    return (eContSetOrder)count;
}

// 0x530500
void CControllerConfigManager::MakeControllerActionsBlank() {
    for (const auto& type : CONTROLLER_TYPES_ALL) {
        for (auto action = 0u; action < NUM_OF_MAX_CONTROLLER_ACTIONS; ++action) {
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
            const auto key = action.Keys[type].m_uiActionInitiator;

            if (GetIsKeyboardKeyDown(key) && inMenu && !GetIsKeyBlank(key, type)) {
                if (inMenu) {
                    AffectControllerStateOn_ButtonDown(key, type);
                }
                auto* pad = CPad::GetPad();
                if (!pad || FrontEndMenuManager.m_bMenuActive) {
                    continue;
                }
                HandleButtonRelease(key, type, &GetControllerState(*pad, type));
            }
        }
    }
}

// 0x5314A0
void CControllerConfigManager::AffectPadFromMouse() {
    bool inMenu = !CPad::padNumber && !FrontEndMenuManager.m_bMenuActive;
    for (auto& action : m_Actions) {
        const auto button = action.Keys[eControllerType::MOUSE].m_uiActionInitiator;

        if (GetIsMouseButtonDown(button)) {
            if (inMenu && !GetIsKeyBlank(button, eControllerType::MOUSE)) {
                AffectControllerStateOn_ButtonDown(button, eControllerType::MOUSE);
            }
        }

        if (GetIsMouseButtonUp(button)) {
            if (auto* pad = CPad::GetPad()) {
                HandleButtonRelease(button, eControllerType::MOUSE, &GetControllerState(*pad, eControllerType::MOUSE));
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
    return action == eControllerAction::CA_VEHICLE_LOOKBEHIND ? TheText.Get("FEC_LBC") : nullptr; // Use Look Left With Look Right.
}

// 0x5303D0
const GxtChar* CControllerConfigManager::GetDefinedKeyByGxtName(eControllerAction action) {
    if (FrontEndMenuManager.m_ControlMethod) {
        if (const auto keyText = GetControllerSettingTextJoystick(action)) {
            return keyText;
        }
    }

    if (const auto keyText = GetControllerSettingTextMouse(action)) {
        return keyText;
    }

    if (const auto keyText = GetKeyNameForKeyboard(action, KEYBOARD)) {
        return keyText;
    }

    if (const auto keyText = GetKeyNameForKeyboard(action, OPTIONAL_EXTRA_KEY)) {
        return keyText;
    }

    if (GetIsKeyBlank(GetControllerKeyAssociatedWithAction(action, eControllerType::JOY_STICK), eControllerType::JOY_STICK)) {
        return GetControllerSettingTextMouse(action);
    }

    return nullptr;
}

// NOTSA
eControllerAction CControllerConfigManager::GetActionIDByName(std::string_view name) {
    for (auto&& [i, actionName] : rngv::enumerate(m_ControllerActionName)) {
        if (std::string_view(reinterpret_cast<const char*>(actionName)) == name) {
            return (eControllerAction)i;
        }
    }
    return eControllerAction::CA_NONE;
}

// inline
void CControllerConfigManager::CheckAndClear(eControllerAction action, eControllerType type, KeyCode button) {
    if (GetControllerKeyAssociatedWithAction(action, type) == button) {
        ClearSettingsAssociatedWithAction(action, type);
    }
};

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
void CControllerConfigManager::CheckAndSetPad(eControllerAction action, eControllerType type, KeyCode button, int16& dpad, int16& oppositeDpad) {
    if (GetControllerKeyAssociatedWithAction(action, type) == button) {
        if (dpad) {
            dpad         = 0;
            oppositeDpad = 0;
        } else {
            dpad = 255;
        }
    }
}

// inline
void CControllerConfigManager::CheckAndSetStick(eControllerAction action, eControllerType type, KeyCode button, int16& state, bool& movementBothDown, int16 value) {
    if (GetControllerKeyAssociatedWithAction(action, type) == button) {
        if (movementBothDown) {
            state = 0;
            movementBothDown = true; // Maybe nop
        } else {
            state = value;
        }
    }
};

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
    if (auto ped = FindPlayerPed()) {
        if (FindPlayerVehicle()) {
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

    auto* state = &GetControllerState(*ped, type);
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
    if (key >= 0 && key < 0xFF) {
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
