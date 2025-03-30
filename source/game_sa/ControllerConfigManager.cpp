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
    RH_ScopedInstall(SaveSettings, 0x52D200);
    RH_ScopedInstall(InitDefaultControlConfiguration, 0x530640);
    RH_ScopedInstall(InitDefaultControlConfigJoyPad, 0x530B00);
    RH_ScopedInstall(InitDefaultControlConfigMouse, 0x52F6F0);
    RH_ScopedInstall(InitialiseControllerActionNameArray, 0x52D260);
    RH_ScopedInstall(ReinitControls, 0x531F20);
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
    RH_ScopedInstall(AffectControllerStateOn_ButtonDown_Driving, 0x52F7B0);
    RH_ScopedInstall(AffectControllerStateOn_ButtonDown_FirstPersonOnly, 0x52F9E0);
    RH_ScopedInstall(GetControllerSettingTextMouse, 0x52F390);
    RH_ScopedInstall(GetControllerSettingTextJoystick, 0x52F450);
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
}

// 0x531EE0
CControllerConfigManager::CControllerConfigManager() {
    /* Member variable init done in header */

    MakeControllerActionsBlank();
    InitDefaultControlConfiguration();
    InitialiseControllerActionNameArray();
}

// 0x531730
void CControllerConfigManager::ClearPedMappings(eControllerAction action, CControllerKey::KeyCode button, eControllerType type) {
    if (GetIsKeyBlank(button, type)) {
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

    if (FrontEndMenuManager.m_ControlMethod) {
        CheckAndClear(CA_PED_CENTER_CAMERA_BEHIND_PLAYER, type, button);
    }
}


// 0x531670
void CControllerConfigManager::ClearCommonMappings(eControllerAction nop, CControllerKey::KeyCode button, eControllerType type) {
    if (GetIsKeyBlank(button, type)) {
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
bool CControllerConfigManager::SetControllerKeyAssociatedWithAction(eControllerAction action, CControllerKey::KeyCode button, eControllerType type) {
    ResetSettingOrder(action);
    auto existingMappings = 0u;
    for (auto i = 0u; i < CONTROLLER_NUM; ++i) {
        if (m_Actions[action].Keys[i].m_uiActionInitiator != rsNULL) {
            existingMappings++;
        }
    }
    m_Actions[action].Keys[type].m_uiActionInitiator  = button;
    m_Actions[action].Keys[type].m_uiSetOrder = (eContSetOrder)existingMappings++;
    return true;
}

// 0x5319D0
void CControllerConfigManager::ClearVehicleMappings(eControllerAction nop, CControllerKey::KeyCode button, eControllerType type) {
    if (GetIsKeyBlank(button, type)) {
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
void CControllerConfigManager::Clear1st3rdPersonMappings(eControllerAction action, CControllerKey::KeyCode button, eControllerType type) {
    if (GetIsKeyBlank(button, type)) {
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

    if (FrontEndMenuManager.m_ControlMethod) {
        CheckAndClear(CA_PED_1RST_PERSON_LOOK_LEFT, type, button);
        CheckAndClear(CA_PED_1RST_PERSON_LOOK_RIGHT, type, button);
        CheckAndClear(CA_PED_1RST_PERSON_LOOK_DOWN, type, button);
        CheckAndClear(CA_PED_1RST_PERSON_LOOK_UP, type, button);
    }
}

// 0x52F510
void CControllerConfigManager::StoreJoyButtonStates() {
    for (auto&& [idx, bs] : notsa::enumerate(m_ButtonStates)) {
        bs = (m_NewJoyState.rgbButtons[idx] & 0x80) != 0;
    }
}

// unused
// 0x531E90
const GxtChar* CControllerConfigManager::GetActionKeyName(eControllerAction action) {
    int32 v3 = 0;
    ResetSettingOrder(action);
    for (auto i = (int32)m_Actions[action].Keys[eControllerType::KEYBOARD].m_uiSetOrder; !i; i += 2) {
        if (++v3 >= 2) {
            return 0;
        }
    }
    return GetKeyNameForKeyboard(action, (eControllerType)v3);
}

// 0x531E20
const GxtChar* CControllerConfigManager::GetControllerSettingText(eControllerAction action, eContSetOrder priority) {
    for (auto type = 0u; type < CONTROLLER_NUM; ++type) {
        if (m_Actions[action].Keys[type].m_uiSetOrder == priority) {
            switch (type) {
            case eControllerType::KEYBOARD:
            case eControllerType::OPTIONAL_EXTRA_KEY: return GetKeyNameForKeyboard(action, (eControllerType)type);
            case eControllerType::MOUSE:              return GetControllerSettingTextMouse(action);
            case eControllerType::JOY_STICK:          return GetControllerSettingTextJoystick(action);
            default:                                  NOTSA_UNREACHABLE();
            }
        }
    }
    return 0;
}

// 0x531C20
void CControllerConfigManager::ClearSniperZoomMappings(eControllerAction nop, CControllerKey::KeyCode button, eControllerType type) {
    if (GetIsKeyBlank(button, type)) {
        return;
    }

    CheckAndClear(CA_PED_SNIPER_ZOOM_IN, type, button);
    CheckAndClear(CA_PED_SNIPER_ZOOM_OUT, type, button);
}

// 0x531BC0
void CControllerConfigManager::UnmapVehicleEnterExit(CControllerKey::KeyCode button, eControllerType type) {
    if (GetIsKeyBlank(button, type)) {
        return;
    }

    CheckAndClear(CA_VEHICLE_ENTER_EXIT, type, button);
}

// 0x52FD70
void CControllerConfigManager::ClearSettingsAssociatedWithAction(eControllerAction Action, eControllerType ArrayToEnter) {
    constexpr RsKeyCodes arrayToTest[4] = { rsNULL, rsNULL, (RsKeyCodes)(0), (RsKeyCodes)(0) };
    if (ArrayToEnter >= eControllerType::KEYBOARD && ArrayToEnter <= eControllerType::JOY_STICK) {
        m_Actions[Action].Keys[ArrayToEnter].m_uiActionInitiator  = arrayToTest[ArrayToEnter];
        m_Actions[Action].Keys[ArrayToEnter].m_uiSetOrder = eContSetOrder::NO_ORDER_SET;
    }
    ResetSettingOrder(Action);
}

// unused
// 0x52FD20
eControllerType CControllerConfigManager::AffectControllerStateOn_ButtonDown_VehicleAndThirdPersonOnly(CControllerKey::KeyCode button, eControllerType type, CControllerState* state) {
    CheckAndSetButton(CA_VEHICLE_ENTER_EXIT, type, button, state->ButtonTriangle);
    return type;
}

// 0x52FCA0
CControllerState* CControllerConfigManager::AffectControllerStateOn_ButtonDown_AllStates(CControllerKey::KeyCode button, eControllerType type, CControllerState* state) {
    CheckAndSetButton(CA_CAMERA_CHANGE_VIEW_ALL_SITUATIONS, type, button, state->Select);
    CheckAndSetPad(CA_CONVERSATION_NO, type, button, state->DPadLeft, state->DPadRight);
    CheckAndSetPad(CA_CONVERSATION_YES, type, button, state->DPadRight, state->DPadLeft);
    CheckAndSetButton(CA_NETWORK_TALK, type, button, state->m_bChatIndicated);
    return state;
}

// 0x52F580
int32 CControllerConfigManager::GetMouseButtonAssociatedWithAction(eControllerAction action) {
    return m_Actions[action].Keys[eControllerType::MOUSE].m_uiActionInitiator;
}

// 0x52FAB0
eControllerType CControllerConfigManager::AffectControllerStateOn_ButtonDown_FirstAndThirdPersonOnly(CControllerKey::KeyCode button, eControllerType type, CControllerState* state) {
    const auto CheckAndSetStick = [&](eControllerAction action, int16& stickValue, bool& stickFlag, int16 value) {
        if (m_Actions[action].Keys[type].m_uiActionInitiator == button) {
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

    if (FrontEndMenuManager.m_ControlMethod) {
        CheckAndSetStick(CA_PED_1RST_PERSON_LOOK_UP, state->RightStickY, m_bStickR_Up_Dwn_MovementBothDown[type], 128); 
        CheckAndSetStick(CA_PED_1RST_PERSON_LOOK_DOWN, state->RightStickY, m_bStickR_Up_Dwn_MovementBothDown[type], -128);
    }

    return type;
}

// 0x52FA20
int32 CControllerConfigManager::AffectControllerStateOn_ButtonDown_ThirdPersonOnly(CControllerKey::KeyCode button, eControllerType type, CControllerState* state) {
    CheckAndSetButton(CA_PED_LOOKBEHIND, type, button, state->ShockButtonR);
    CheckAndSetButton(CA_PED_JUMPING, type, button, state->ButtonSquare);
    CheckAndSetButton(CA_PED_ANSWER_PHONE, type, button, state->LeftShoulder1);
    CheckAndSetButton(CA_PED_CYCLE_WEAPON_LEFT, type, button, state->LeftShoulder2);
    CheckAndSetButton(CA_PED_CYCLE_WEAPON_RIGHT, type, button, state->RightShoulder2);
    CheckAndSetButton(CA_PED_SPRINT, type, button, state->ButtonCross);
    CheckAndSetButton(CA_PED_DUCK, type, button, state->ShockButtonL);

    if (FrontEndMenuManager.m_ControlMethod) {
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
    return m_Actions[action].Keys[type].m_uiActionInitiator;
}

// 0x52F9E0
int32 CControllerConfigManager::AffectControllerStateOn_ButtonDown_FirstPersonOnly(CControllerKey::KeyCode button, eControllerType type, CControllerState* state) {
    CheckAndSetButton(CA_PED_SNIPER_ZOOM_IN, type, button, state->ButtonSquare);
    CheckAndSetButton(CA_PED_SNIPER_ZOOM_OUT, type, button, state->ButtonCross);
    CheckAndSetButton(CA_PED_DUCK, type, button, state->ShockButtonL);
    return button;
}

// 0x52FD40
int32 CControllerConfigManager::HandleButtonRelease(CControllerKey::KeyCode button, eControllerType type, CControllerState* state) {
    const auto CheckAndReset = [&](eControllerAction action, int16& stateFlag) {
        if (m_Actions[action].Keys[type].m_uiActionInitiator == button) {
            stateFlag = 0;
        }
    };
    CheckAndReset(CA_NETWORK_TALK, state->m_bChatIndicated);
    CheckAndReset(CA_VEHICLE_MOUSE_LOOK, state->m_bVehicleMouseLook);
    return button;
}

// 0x52F7B0
eControllerType CControllerConfigManager::AffectControllerStateOn_ButtonDown_Driving(CControllerKey::KeyCode button, eControllerType type, CControllerState* state) {
    const auto CheckAndSetStick = [&](eControllerAction action, int16& stickValue, bool& stickFlag, int16 value) {
        if (m_Actions[action].Keys[type].m_uiActionInitiator == button) {
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

    // why double ? check: if you press both buttons you look behind
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
void CControllerConfigManager::ResetSettingOrder(eControllerAction action) {
    int32 result = 0;
    
    for (int32 priorityLevel = 1; priorityLevel < 5; priorityLevel++) {
        // Check if any key already has this priority level
        bool priorityExists = false;
        
        for (int32 controllerType = 0; controllerType < 4; controllerType++) {
            if (m_Actions[action].Keys[controllerType].m_uiSetOrder == priorityLevel) {
                priorityExists = true;
                break;
            }
        }

        if (!priorityExists) {
            // No key has this priority, find the one with lowest priority > priorityLevel
            bool found = false;
            
            for (int32 controllerType = 0; controllerType < 4; controllerType++) {
                const auto keyPriority = m_Actions[action].Keys[controllerType].m_uiSetOrder;
                
                // Only consider keys with priority > priorityLevel and not 0 (unset)
                if (keyPriority > priorityLevel && keyPriority != 0) {
                    // If no key found yet or this key has lower priority than current best
                    if (!found || keyPriority < m_Actions[action].Keys[result].m_uiSetOrder) {
                        result = controllerType;
                        found = true;
                    }
                }
            }
            
            // If we found a key with higher priority, adjust it to the current level
            if (found) {
                m_Actions[action].Keys[result].m_uiSetOrder = (eContSetOrder)priorityLevel;
            }
        }
    }
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
                UpdateJoy_ButtonDown(padBtn, eControllerType::JOY_STICK);
            } else {
                UpdateJoy_ButtonUp(padBtn, eControllerType::JOY_STICK);
            }
        }
    }
}

#ifdef INI_SETTINGS
// INI SETTINGS
// 0x530530
bool CControllerConfigManager::LoadSettings(FILESTREAM file) {
    if (!file) {
        return false;
    }
    
    // Read the entire file into memory
    char buffer[8192] = {0}; // Assuming the file won't be larger than this
    int fileSize = CFileMgr::Read(file, buffer, sizeof(buffer) - 1);
    if (fileSize <= 0) {
        return false;
    }
    buffer[fileSize] = '\0'; // Ensure null-termination
    
    MakeControllerActionsBlank(); // Clear existing bindings
    
    // Parse INI format
    char* line = strtok(buffer, "\r\n");
    eControllerType currentType = eControllerType::KEYBOARD;
    
    while (line) {
        // Skip empty lines and comments
        if (line[0] == '\0' || line[0] == ';' || line[0] == '#') {
            line = strtok(NULL, "\r\n");
            continue;
        }
        
        // Check for section header [SectionName]
        if (line[0] == '[') {
            char* end = strchr(line, ']');
            if (end) {
                *end = '\0';
                if (strcmp(line + 1, "Keyboard") == 0) {
                    currentType = eControllerType::KEYBOARD;
                } else if (strcmp(line + 1, "OptionalKey") == 0) {
                    currentType = eControllerType::OPTIONAL_EXTRA_KEY;
                } else if (strcmp(line + 1, "Mouse") == 0) {
                    currentType = eControllerType::MOUSE;
                } else if (strcmp(line + 1, "Joystick") == 0) {
                    currentType = eControllerType::JOY_STICK;
                }
            }
        } 
        // Parse key=value pairs
        else {
            char* separator = strchr(line, '=');
            if (separator) {
                *separator = '\0';
                char* actionName = line;
                char* value = separator + 1;
                
                // Find action by name or by ID
                eControllerAction actionId = CA_INVALID;
                
                // Try to parse as action ID first
                if (isdigit(actionName[0])) {
                    int id = atoi(actionName);
                    if (id >= 0 && id < NUM_OF_CONTROLLER_ACTIONS) {
                        actionId = (eControllerAction)id;
                    }
                } else {
                    // Try to find by name
                    for (int i = 0; i < NUM_OF_CONTROLLER_ACTIONS; i++) {
                        char textName[128] = {0}; // Initialize with zeros to ensure null-termination
                        GxtCharToUTF8(textName, m_ControllerActionName[i]);
                        if (strcmp(textName, actionName) == 0) {
                            actionId = (eControllerAction)i;
                            break;
                        }
                    }
                }
                
                if (actionId != CA_INVALID) {
                    // Parse the key value and order
                    char* orderStr = strchr(value, ',');
                    int keyValue = atoi(value);
                    int orderValue = 0;
                    
                    if (orderStr) {
                        *orderStr = '\0';
                        orderValue = atoi(orderStr + 1);
                    }
                    
                    // Set the binding
                    m_Actions[actionId].Keys[currentType].m_uiActionInitiator = (CControllerKey::KeyCode)keyValue;
                    m_Actions[actionId].Keys[currentType].m_uiSetOrder = (eContSetOrder)orderValue;
                }
            }
        }
        
        line = strtok(NULL, "\r\n");
    }
    
    return true;
}

// 0x52D200
int32 CControllerConfigManager::SaveSettings(FILESTREAM file) {
    if (!file) {
        return 0;
    }
    
    // Write header
    const char* header = "; GTA SA Controller Configuration\n; Generated by GTA:SA\n\n";
    CFileMgr::Write(file, header, (int32)strlen(header));
    
    // Write sections for each controller type
    const char* sections[] = {"Keyboard", "OptionalKey", "Mouse", "Joystick"};
    
    for (int i = 0; i < CONTROLLER_NUM; i++) {
        char sectionHeader[128];
        sprintf(sectionHeader, "[%s]\n", sections[i]);
        CFileMgr::Write(file, sectionHeader, (int32)strlen(sectionHeader));
        
        for (int j = 0; j < NUM_OF_CONTROLLER_ACTIONS; j++) {
            // Skip empty bindings
            if (GetIsKeyBlank(m_Actions[j].Keys[i].m_uiActionInitiator, (eControllerType)i)) {
                continue;
            }
            
            // Get action name
            char actionName[128];
            GxtCharToUTF8(actionName, m_ControllerActionName[j]);
            
            // Write key=value
            char line[256];
            sprintf(line, "%s=%d,%d\n", actionName, 
                   m_Actions[j].Keys[i].m_uiActionInitiator, 
                   m_Actions[j].Keys[i].m_uiSetOrder);
            CFileMgr::Write(file, line, (int32)strlen(line));
        }
        
        // Add a separator between sections
        CFileMgr::Write(file, "\n", 1);
    }
    
    return 1;
}
#else
// 0x530530
bool CControllerConfigManager::LoadSettings(FILE* file) {
    if (!file) {
        return 1;
    }

    // Check if file has valid header
    char header[32];
    CFileMgr::Read(file, header, 29);
    if (strncmp(header, "THIS FILE IS NOT VALID YET", 26) == 0) {
        // Invalid header
        return 1;
    }

    // Reset file position to start and read version
    CFileMgr::Seek(file, 0, 0);
    int32 version;
    CFileMgr::Read(file, &version, 4);
    
    if (version < 6) {
        // Version too old
        return 1;
    }

    // Verify file format by checking action IDs
    for (int32 controllerType = 0; controllerType < 4; controllerType++) {
        for (int32 actionId = 0; actionId < 59; actionId++) {
            int32 storedActionId;
            CFileMgr::Read(file, &storedActionId, 4);
            
            if (storedActionId != actionId) {
                // Invalid action ID in file
                return 0;
            }
            
            
            // Skip key data for now
            CFileMgr::Seek(file, 8, 1);
        }
    }

    // Go back to position after version info
    CFileMgr::Seek(file, 4, 0);
    
    // Clear existing settings
    MakeControllerActionsBlank();

    // Read key mappings for each controller type
    for (int32 controllerType = 0; controllerType < 4; controllerType++) {
        for (int32 actionId = 0; actionId < 59; actionId++) {
            // Skip action ID
            CFileMgr::Seek(file, 4, 1);
            
            // Read key and order for this action
            CFileMgr::Read(file, &m_Actions[actionId].Keys[controllerType], 8);
        }
    }

    return 1;
}

// 0x52D200
int32 CControllerConfigManager::SaveSettings(FILE* file) {
    if (!file) {
        return 0;
    }
    
    for (int32 controllerType = 0; controllerType < 4; controllerType++) {
        for (int32 actionId = 0; actionId < 59; actionId++) {
            // Write action ID
            CFileMgr::Write(file, &actionId, 4);
            
            // Write key mapping data
            CFileMgr::Write(file, &m_Actions[actionId].Keys[controllerType], 8);
        }
    }

    return 1;
}
#endif

// 0x530640
void CControllerConfigManager::InitDefaultControlConfiguration() {
    SetControllerKeyAssociatedWithAction(CA_GO_FORWARD, rsUP, eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(CA_GO_BACK, rsDOWN, eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(CA_GO_LEFT, rsLEFT, eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(CA_GO_RIGHT, rsRIGHT, eControllerType::KEYBOARD);

    if (FrontEndMenuManager.m_nTitleLanguage == 2) {
        SetControllerKeyAssociatedWithAction(CA_CONVERSATION_YES, (RsKeyCodes)'Z', eControllerType::KEYBOARD);
        SetControllerKeyAssociatedWithAction(CA_PED_SNIPER_ZOOM_IN, (RsKeyCodes)'Y', eControllerType::OPTIONAL_EXTRA_KEY);
    } else {
        SetControllerKeyAssociatedWithAction(CA_CONVERSATION_YES, (RsKeyCodes)'Y', eControllerType::KEYBOARD);
        SetControllerKeyAssociatedWithAction(CA_PED_SNIPER_ZOOM_IN, (RsKeyCodes)'X', eControllerType::OPTIONAL_EXTRA_KEY);
    }

    if (FrontEndMenuManager.m_nTitleLanguage == 1) {
        SetControllerKeyAssociatedWithAction(CA_GO_FORWARD, (RsKeyCodes)'Z', eControllerType::OPTIONAL_EXTRA_KEY);
        SetControllerKeyAssociatedWithAction(CA_GO_LEFT, (RsKeyCodes)'Q', eControllerType::OPTIONAL_EXTRA_KEY);
        SetControllerKeyAssociatedWithAction(CA_PED_CYCLE_WEAPON_LEFT, (RsKeyCodes)'A', eControllerType::KEYBOARD);
        SetControllerKeyAssociatedWithAction(CA_PED_SNIPER_ZOOM_OUT, (RsKeyCodes)'W', eControllerType::OPTIONAL_EXTRA_KEY);
    } else {
        SetControllerKeyAssociatedWithAction(CA_GO_FORWARD, (RsKeyCodes)'W', eControllerType::OPTIONAL_EXTRA_KEY);
        SetControllerKeyAssociatedWithAction(CA_GO_LEFT, (RsKeyCodes)'A', eControllerType::OPTIONAL_EXTRA_KEY);
        SetControllerKeyAssociatedWithAction(CA_PED_CYCLE_WEAPON_LEFT, (RsKeyCodes)'Q', eControllerType::KEYBOARD);
        SetControllerKeyAssociatedWithAction(CA_PED_SNIPER_ZOOM_OUT, (RsKeyCodes)'Z', eControllerType::OPTIONAL_EXTRA_KEY);
    }

    SetControllerKeyAssociatedWithAction(CA_GO_BACK, (RsKeyCodes)'S', eControllerType::OPTIONAL_EXTRA_KEY);
    SetControllerKeyAssociatedWithAction(CA_GO_RIGHT, (RsKeyCodes)'D', eControllerType::OPTIONAL_EXTRA_KEY);
    SetControllerKeyAssociatedWithAction(CA_PED_CYCLE_WEAPON_RIGHT, (RsKeyCodes)'E', eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(CA_PED_CYCLE_WEAPON_RIGHT, rsPADENTER, eControllerType::OPTIONAL_EXTRA_KEY);
    SetControllerKeyAssociatedWithAction(CA_PED_CYCLE_WEAPON_LEFT, rsPADDEL, eControllerType::OPTIONAL_EXTRA_KEY);
    SetControllerKeyAssociatedWithAction(CA_PED_FIRE_WEAPON, rsPADINS, eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(CA_PED_FIRE_WEAPON, rsLCTRL, eControllerType::OPTIONAL_EXTRA_KEY);
    SetControllerKeyAssociatedWithAction(CA_PED_SNIPER_ZOOM_IN, rsPGUP, eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(CA_PED_SNIPER_ZOOM_OUT, rsPGDN, eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(CA_PED_FIRE_WEAPON_ALT, (RsKeyCodes)'\\', eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(CA_GROUP_CONTROL_FWD, (RsKeyCodes)'G', eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(CA_GROUP_CONTROL_BWD, (RsKeyCodes)'H', eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(CA_CONVERSATION_NO, (RsKeyCodes)'N', eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(CA_VEHICLE_ENTER_EXIT, (RsKeyCodes)'F', eControllerType::OPTIONAL_EXTRA_KEY);
    SetControllerKeyAssociatedWithAction(CA_VEHICLE_ENTER_EXIT, rsENTER, eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(CA_CAMERA_CHANGE_VIEW_ALL_SITUATIONS, (RsKeyCodes)'V', eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(CA_CAMERA_CHANGE_VIEW_ALL_SITUATIONS, rsHOME, eControllerType::OPTIONAL_EXTRA_KEY);
    SetControllerKeyAssociatedWithAction(CA_PED_JUMPING, rsLSHIFT, eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(CA_PED_JUMPING, rsRCTRL, eControllerType::OPTIONAL_EXTRA_KEY);
    SetControllerKeyAssociatedWithAction(CA_PED_SPRINT, (RsKeyCodes)' ', eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(CA_PED_DUCK, (RsKeyCodes)'C', eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(CA_PED_ANSWER_PHONE, rsTAB, eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(CA_PED_WALK, rsLALT, eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(CA_PED_LOCK_TARGET, rsDEL, eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(CA_PED_LOCK_TARGET, rsCAPSLK, eControllerType::OPTIONAL_EXTRA_KEY);
    SetControllerKeyAssociatedWithAction(CA_PED_1RST_PERSON_LOOK_LEFT, rsPADLEFT, eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(CA_PED_1RST_PERSON_LOOK_RIGHT, rsPADRIGHT, eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(CA_PED_1RST_PERSON_LOOK_UP, rsPADDOWN, eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(CA_PED_1RST_PERSON_LOOK_DOWN, rsPADUP, eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(CA_PED_CENTER_CAMERA_BEHIND_PLAYER, (RsKeyCodes)'#', eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(CA_PED_LOOKBEHIND, rsPADEND, eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(CA_VEHICLE_STEER_UP, rsUP, eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(CA_VEHICLE_STEER_DOWN, rsDOWN, eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(CA_VEHICLE_RADIO_STATION_UP, rsINS, eControllerType::KEYBOARD);

    if (FrontEndMenuManager.m_nTitleLanguage == 1) {
        SetControllerKeyAssociatedWithAction(CA_VEHICLE_STEER_LEFT, (RsKeyCodes)'Q', eControllerType::KEYBOARD);
        SetControllerKeyAssociatedWithAction(CA_VEHICLE_LOOK_LEFT, (RsKeyCodes)'A', eControllerType::KEYBOARD);
        SetControllerKeyAssociatedWithAction(CA_VEHICLE_RADIO_STATION_UP, (RsKeyCodes)'`', eControllerType::OPTIONAL_EXTRA_KEY);
        SetControllerKeyAssociatedWithAction(CA_TOGGLE_SUBMISSIONS, (RsKeyCodes)233, eControllerType::KEYBOARD);
        SetControllerKeyAssociatedWithAction(CA_VEHICLE_ACCELERATE, (RsKeyCodes)'Z', eControllerType::KEYBOARD);
    } else {
        SetControllerKeyAssociatedWithAction(CA_VEHICLE_STEER_LEFT, (RsKeyCodes)'A', eControllerType::KEYBOARD);
        SetControllerKeyAssociatedWithAction(CA_VEHICLE_LOOK_LEFT, (RsKeyCodes)'Q', eControllerType::KEYBOARD);
        SetControllerKeyAssociatedWithAction(CA_VEHICLE_RADIO_STATION_UP, (RsKeyCodes)'4', eControllerType::OPTIONAL_EXTRA_KEY);
        SetControllerKeyAssociatedWithAction(CA_TOGGLE_SUBMISSIONS, (RsKeyCodes)'2', eControllerType::KEYBOARD);
        SetControllerKeyAssociatedWithAction(CA_VEHICLE_ACCELERATE, (RsKeyCodes)'W', eControllerType::KEYBOARD);
    }

    SetControllerKeyAssociatedWithAction(CA_TOGGLE_SUBMISSIONS, rsPLUS, eControllerType::OPTIONAL_EXTRA_KEY);
    SetControllerKeyAssociatedWithAction(CA_VEHICLE_FIRE_WEAPON, rsRCTRL, eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(CA_VEHICLE_FIRE_WEAPON, rsLALT, eControllerType::OPTIONAL_EXTRA_KEY);
    SetControllerKeyAssociatedWithAction(CA_VEHICLE_FIRE_WEAPON_ALT, rsLCTRL, eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(CA_VEHICLE_FIRE_WEAPON_ALT, rsPADINS, eControllerType::OPTIONAL_EXTRA_KEY);
    SetControllerKeyAssociatedWithAction(CA_VEHICLE_BRAKE, (RsKeyCodes)'S', eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(CA_VEHICLE_STEER_RIGHT, (RsKeyCodes)'D', eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(CA_VEHICLE_STEER_LEFT, rsLEFT, eControllerType::OPTIONAL_EXTRA_KEY);
    SetControllerKeyAssociatedWithAction(CA_VEHICLE_STEER_RIGHT, rsRIGHT, eControllerType::OPTIONAL_EXTRA_KEY);
    SetControllerKeyAssociatedWithAction(CA_VEHICLE_HORN, rsCAPSLK, eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(CA_VEHICLE_HORN, (RsKeyCodes)'H', eControllerType::OPTIONAL_EXTRA_KEY);
    SetControllerKeyAssociatedWithAction(CA_VEHICLE_HANDBRAKE, (RsKeyCodes)' ', eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(CA_VEHICLE_HANDBRAKE, rsRCTRL, eControllerType::OPTIONAL_EXTRA_KEY);
    SetControllerKeyAssociatedWithAction(CA_VEHICLE_LOOK_RIGHT, (RsKeyCodes)'E', eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(CA_VEHICLE_TURRET_LEFT, rsPADLEFT, eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(CA_VEHICLE_TURRET_RIGHT, rsPADRIGHT, eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(CA_VEHICLE_TURRET_UP, rsPADDOWN, eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(CA_VEHICLE_TURRET_DOWN, rsPADUP, eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(CA_VEHICLE_TURRET_UP, rsEND, eControllerType::OPTIONAL_EXTRA_KEY);
    SetControllerKeyAssociatedWithAction(CA_VEHICLE_TURRET_DOWN, rsDEL, eControllerType::OPTIONAL_EXTRA_KEY);
    SetControllerKeyAssociatedWithAction(CA_VEHICLE_RADIO_TRACK_SKIP, rsF5, eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(CA_VEHICLE_RADIO_STATION_DOWN, rsDEL, eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(CA_VEHICLE_RADIO_STATION_DOWN, (RsKeyCodes)'R', eControllerType::OPTIONAL_EXTRA_KEY);
    SetControllerKeyAssociatedWithAction(CA_PED_CYCLE_TARGET_LEFT, (RsKeyCodes)'[', eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(CA_PED_CYCLE_TARGET_RIGHT, (RsKeyCodes)']', eControllerType::KEYBOARD);
    SetControllerKeyAssociatedWithAction(CA_PED_CYCLE_TARGET_RIGHT, (RsKeyCodes)']', eControllerType::KEYBOARD);

    // NOTSA
    SetControllerKeyAssociatedWithAction(CA_PED_FIRE_WEAPON,            rsMOUSE_LEFT_BUTTON, eControllerType::MOUSE);
    SetControllerKeyAssociatedWithAction(CA_VEHICLE_FIRE_WEAPON,        rsMOUSE_LEFT_BUTTON, eControllerType::MOUSE);
    SetControllerKeyAssociatedWithAction(CA_PED_LOCK_TARGET,            rsMOUSE_RIGHT_BUTTON, eControllerType::MOUSE);
    SetControllerKeyAssociatedWithAction(CA_VEHICLE_MOUSE_LOOK,         rsMOUSE_RIGHT_BUTTON, eControllerType::MOUSE);
    SetControllerKeyAssociatedWithAction(CA_VEHICLE_LOOKBEHIND,         rsMOUSE_MIDDLE_BUTTON, eControllerType::MOUSE);
    SetControllerKeyAssociatedWithAction(CA_PED_LOOKBEHIND,             rsMOUSE_MIDDLE_BUTTON, eControllerType::MOUSE);
    SetControllerKeyAssociatedWithAction(CA_PED_CYCLE_WEAPON_LEFT,      rsMOUSE_WHEEL_UP_BUTTON, eControllerType::MOUSE);
    SetControllerKeyAssociatedWithAction(CA_PED_CYCLE_WEAPON_RIGHT,     rsMOUSE_WHEEL_DOWN_BUTTON, eControllerType::MOUSE);
    SetControllerKeyAssociatedWithAction(CA_VEHICLE_RADIO_STATION_UP,   rsMOUSE_WHEEL_UP_BUTTON, eControllerType::MOUSE);
    SetControllerKeyAssociatedWithAction(CA_VEHICLE_RADIO_STATION_DOWN, rsMOUSE_WHEEL_DOWN_BUTTON, eControllerType::MOUSE);
    SetControllerKeyAssociatedWithAction(CA_PED_SNIPER_ZOOM_IN,         rsMOUSE_WHEEL_UP_BUTTON, eControllerType::MOUSE);
    SetControllerKeyAssociatedWithAction(CA_PED_SNIPER_ZOOM_OUT,        rsMOUSE_WHEEL_DOWN_BUTTON, eControllerType::MOUSE);


    ClearSimButtonPressCheckers();
}

// 0x530B00
void CControllerConfigManager::InitDefaultControlConfigJoyPad(uint32 buttonCount) {
    m_WasJoyJustInitialised = true;

    buttonCount = std::min(buttonCount, 16u);

    std::array<std::pair<eControllerAction, RsKeyCodes>, 3> actions;

    auto set_actions = [&](std::initializer_list<std::pair<eControllerAction, RsKeyCodes>> init_list) {
        std::copy(init_list.begin(), init_list.end(), actions.begin());
        for (const auto& action : actions) {
            SetControllerKeyAssociatedWithAction(action.first, action.second, eControllerType::JOY_STICK);
        }
    };

    if (IsCheckSpecificGamepad()) {
        switch (buttonCount) {
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
            switch (buttonCount) {
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
void CControllerConfigManager::InitDefaultControlConfigMouse(const CMouseControllerState& state, bool controller) {
    if (state.m_bLeftButton) {
        m_MouseFoundInitSet = true;
        SetMouseButtonAssociatedWithAction(CA_PED_FIRE_WEAPON,            rsMOUSE_LEFT_BUTTON);
        SetMouseButtonAssociatedWithAction(CA_VEHICLE_FIRE_WEAPON,        rsMOUSE_LEFT_BUTTON);
    }
    if (state.m_bRightButton) {                                                      
        SetMouseButtonAssociatedWithAction(CA_PED_LOCK_TARGET,            rsMOUSE_RIGHT_BUTTON);
        SetMouseButtonAssociatedWithAction(CA_VEHICLE_MOUSE_LOOK,         rsMOUSE_RIGHT_BUTTON);
    }
    if (state.m_bMiddleButton) {                                                      
        SetMouseButtonAssociatedWithAction(CA_VEHICLE_LOOKBEHIND,         rsMOUSE_MIDDLE_BUTTON);
        SetMouseButtonAssociatedWithAction(CA_PED_LOOKBEHIND,             rsMOUSE_MIDDLE_BUTTON);
    }
    if (state.m_bWheelMovedUp || state.m_bWheelMovedDown) {
        SetMouseButtonAssociatedWithAction(CA_PED_CYCLE_WEAPON_LEFT,      rsMOUSE_WHEEL_UP_BUTTON);
        SetMouseButtonAssociatedWithAction(CA_PED_CYCLE_WEAPON_RIGHT,     rsMOUSE_WHEEL_DOWN_BUTTON);
        SetMouseButtonAssociatedWithAction(CA_VEHICLE_RADIO_STATION_UP,   rsMOUSE_WHEEL_UP_BUTTON);
        SetMouseButtonAssociatedWithAction(CA_VEHICLE_RADIO_STATION_DOWN, rsMOUSE_WHEEL_DOWN_BUTTON);
        SetMouseButtonAssociatedWithAction(CA_PED_SNIPER_ZOOM_IN,         rsMOUSE_WHEEL_UP_BUTTON);
        SetMouseButtonAssociatedWithAction(CA_PED_SNIPER_ZOOM_OUT,        rsMOUSE_WHEEL_DOWN_BUTTON);
    }
}

// 0x52D260
void CControllerConfigManager::InitialiseControllerActionNameArray() {
    const auto SetActionName = [&](eControllerAction action, const GxtChar* name) {
        GxtCharStrcpy(m_ControllerActionName[action], name);
    };
    SetActionName(CA_PED_LOOKBEHIND, "PED_LOOKBEHIND"_gxt);
    SetActionName(CA_PED_CYCLE_WEAPON_LEFT, "PED_CYCLE_WEAPON_LEFT"_gxt);
    SetActionName(CA_PED_CYCLE_WEAPON_RIGHT, "PED_CYCLE_WEAPON_RIGHT"_gxt);
    SetActionName(CA_PED_LOCK_TARGET, "PED_LOCK_TARGET"_gxt);
    SetActionName(CA_PED_JUMPING, "PED_JUMPING"_gxt);
    SetActionName(CA_PED_SPRINT, "PED_SPRINT"_gxt);
    SetActionName(CA_PED_WALK, "SNEAK_ABOUT"_gxt);
    SetActionName(CA_PED_CYCLE_TARGET_LEFT, "PED_CYCLE_TARGET_LEFT"_gxt);
    SetActionName(CA_PED_CYCLE_TARGET_RIGHT, "PED_CYCLE_TARGET_RIGHT"_gxt);
    SetActionName(CA_PED_CENTER_CAMERA_BEHIND_PLAYER, "PED_CENTER_CAMERA_BEHIND_PLAYER"_gxt);
    SetActionName(CA_VEHICLE_LOOKBEHIND, "VEHICLE_LOOKBEHIND"_gxt);
    SetActionName(CA_PED_DUCK, "PED_DUCK"_gxt);
    SetActionName(CA_PED_ANSWER_PHONE, "PED_ANSWER_PHONE"_gxt);
    SetActionName(CA_VEHICLE_STEER_LEFT, "VEHICLE_STEERLEFT"_gxt);
    SetActionName(CA_VEHICLE_STEER_RIGHT, "VEHICLE_STEERRIGHT"_gxt);
    SetActionName(CA_VEHICLE_STEER_UP, "VEHICLE_STEERUP"_gxt);
    SetActionName(CA_VEHICLE_STEER_DOWN, "VEHICLE_STEERDOWN"_gxt);
    SetActionName(CA_VEHICLE_LOOK_LEFT, "VEHICLE_LOOKLEFT"_gxt);
    SetActionName(CA_VEHICLE_LOOK_RIGHT, "VEHICLE_LOOKRIGHT"_gxt);
    SetActionName(CA_VEHICLE_MOUSE_LOOK, "VEHICLE_MOUSELOOK"_gxt);
    SetActionName(CA_VEHICLE_HORN, "VEHICLE_HORN"_gxt);
    SetActionName(CA_VEHICLE_HANDBRAKE, "VEHICLE_HANDBRAKE"_gxt);
    SetActionName(CA_VEHICLE_ACCELERATE, "VEHICLE_ACCELERATE"_gxt);
    SetActionName(CA_VEHICLE_BRAKE, "VEHICLE_BRAKE"_gxt);
    SetActionName(CA_VEHICLE_RADIO_STATION_UP, "VEHICLE_RADIO_STATION_UP"_gxt);
    SetActionName(CA_VEHICLE_RADIO_STATION_DOWN, "VEHICLE_RADIO_STATION_DOWN"_gxt);
    SetActionName(CA_TOGGLE_SUBMISSIONS, "TOGGLE_SUBMISSIONS"_gxt);
    SetActionName(CA_PED_SNIPER_ZOOM_IN, "PED_SNIPER_ZOOM_IN"_gxt);
    SetActionName(CA_PED_SNIPER_ZOOM_OUT, "PED_SNIPER_ZOOM_OUT"_gxt);
    SetActionName(CA_PED_1RST_PERSON_LOOK_LEFT, "PED_1RST_PERSON_LOOK_LEFT"_gxt);
    SetActionName(CA_PED_1RST_PERSON_LOOK_RIGHT, "PED_1RST_PERSON_LOOK_RIGHT"_gxt);
    SetActionName(CA_PED_1RST_PERSON_LOOK_UP, "PED_1RST_PERSON_LOOK_UP"_gxt);
    SetActionName(CA_PED_1RST_PERSON_LOOK_DOWN, "PED_1RST_PERSON_LOOK_DOWN"_gxt);
    SetActionName(CA_SHOW_MOUSE_POINTER_TOGGLE, "SHOW_MOUSE_POINTER_TOGGLE"_gxt);
    SetActionName(CA_CAMERA_CHANGE_VIEW_ALL_SITUATIONS, "CAMERA_CHANGE_VIEW_ALL_SITUATIONS"_gxt);
    SetActionName(CA_PED_FIRE_WEAPON, "PED_FIREWEAPON"_gxt);
    SetActionName(CA_PED_FIRE_WEAPON_ALT, "PED_FIREWEAPON_ALT"_gxt);
    SetActionName(CA_VEHICLE_FIRE_WEAPON, "VEHICLE_FIREWEAPON"_gxt);
    SetActionName(CA_VEHICLE_FIRE_WEAPON_ALT, "VEHICLE_FIREWEAPON_ALT"_gxt);
    SetActionName(CA_VEHICLE_ENTER_EXIT, "VEHICLE_ENTER_EXIT"_gxt);
    SetActionName(CA_CONVERSATION_NO, "CONVERSATION_NO"_gxt);
    SetActionName(CA_CONVERSATION_YES, "CONVERSATION_YES"_gxt);
    SetActionName(CA_GROUP_CONTROL_FWD, "GROUP_CONTROL_FWD"_gxt);
    SetActionName(CA_GROUP_CONTROL_BWD, "GROUP_CONTROL_BWD"_gxt);
    SetActionName(CA_GO_LEFT, "GO_LEFT"_gxt);
    SetActionName(CA_GO_RIGHT, "GO_RIGHT"_gxt);
    SetActionName(CA_GO_FORWARD, "GO_FORWARD"_gxt);
    SetActionName(CA_GO_BACK, "GO_BACK"_gxt);
    SetActionName(CA_VEHICLE_TURRET_LEFT, "VEHICLE_TURRETLEFT"_gxt);
    SetActionName(CA_VEHICLE_TURRET_RIGHT, "VEHICLE_TURRETRIGHT"_gxt);
    SetActionName(CA_VEHICLE_TURRET_UP, "VEHICLE_TURRETUP"_gxt);
    SetActionName(CA_VEHICLE_TURRET_DOWN, "VEHICLE_TURRETDOWN"_gxt);
    SetActionName(CA_NETWORK_TALK, "NETWORK_TALK"_gxt);
    SetActionName(CA_TOGGLE_DPAD, "TOGGLE_DPAD"_gxt);
    SetActionName(CA_SWITCH_DEBUG_CAM_ON, "SWITCH_DEBUG_CAM_ON"_gxt);
    SetActionName(CA_TAKE_SCREEN_SHOT, "TAKE_SCREEN_SHOT"_gxt);
}

// 0x531F20
#define LEGACY_WININPUT 1

bool CControllerConfigManager::ReinitControls() {
    CMouseControllerState MouseSetUp;
    ControlsManager.MakeControllerActionsBlank();
    ControlsManager.InitDefaultControlConfiguration();
#if LEGACY_WININPUT
    auto mouseState = WinInput::GetMouseState();
#endif
    ControlsManager.InitDefaultControlConfigMouse(mouseState, !FrontEndMenuManager.m_ControlMethod);
    // ControlsManager.InitDefaultControlConfigJoyPad(44u);
    return 0;
}

// 0x52F590
int8 CControllerConfigManager::SetMouseButtonAssociatedWithAction(eControllerAction actionID, CControllerKey::KeyCode button) {
    auto& action = m_Actions[actionID];
    const auto order = 1 + rng::count_if(action.Keys, [](auto&& key) { /* 1 + count of previously set keys for this control */
        return key.m_uiActionInitiator != rsNULL && key.m_uiActionInitiator != 0;
    });
    action.Keys[eControllerType::MOUSE].m_uiActionInitiator  = button;
    action.Keys[eControllerType::MOUSE].m_uiSetOrder = (eContSetOrder)order;
    return order;
}

// unused
// 0x52DA30
void CControllerConfigManager::StoreMouseButtonState(eMouseButtons button, bool state) {
    switch (button) {
    case MOUSE_BUTTON_LEFT:           CPad::PCTempMouseControllerState.m_bLeftButton = state; break;
    case MOUSE_BUTTON_MIDDLE:         CPad::PCTempMouseControllerState.m_bMiddleButton = state; break;
    case MOUSE_BUTTON_RIGHT:          CPad::PCTempMouseControllerState.m_bRightButton = state; break;
    case MOUSE_BUTTON_WHEEL_UP:       CPad::PCTempMouseControllerState.m_bWheelMovedUp = state; break;
    case MOUSE_BUTTON_WHEEL_DOWN:     CPad::PCTempMouseControllerState.m_bWheelMovedDown = state; break;
    case MOUSE_BUTTON_WHEEL_XBUTTON1: CPad::PCTempMouseControllerState.m_bMsFirstXButton = state; break;
    case MOUSE_BUTTON_WHEEL_XBUTTON2: CPad::PCTempMouseControllerState.m_bMsSecondXButton = state; break;
    }
}

// 0x52DAB0
void CControllerConfigManager::UpdateJoyInConfigMenus_ButtonDown(CControllerKey::KeyCode button, int32 padNumber) {
    CPad* pad = CPad::GetPad(padNumber);
    if (!pad || button == 0) {
        return;
    }
    switch (button) {
    case LEFTSTICKY:  pad->PCTempJoyState.ButtonTriangle = 255; break;
    case RIGHTSTICKX: pad->PCTempJoyState.ButtonCross = 255; break;
    case RIGHTSTICKY: pad->PCTempJoyState.ButtonSquare = 255; break;
    case LEFTSHOULDER1:
        if (!IsCheckSpecificGamepad()) {
            pad->PCTempJoyState.ButtonTriangle = 255;
        }
        break;
    case LEFTSHOULDER2:  pad->PCTempJoyState.LeftShoulder2 = 255; break;
    case RIGHTSHOULDER1: pad->PCTempJoyState.RightShoulder2 = 255; break;
    case RIGHTSHOULDER2: pad->PCTempJoyState.LeftShoulder1 = 255; break;
    case DPADUP:         pad->PCTempJoyState.RightShoulder1 = 255; break;
    case DPADDOWN:       pad->PCTempJoyState.Select = 255; break;
    case DPADLEFT:       pad->PCTempJoyState.ShockButtonL = 255; break;
    case DPADRIGHT:      pad->PCTempJoyState.ShockButtonR = 255; break;
    case START:          {
        if (padNumber == 1) {
            pad->PCTempJoyState.Start = 255;
        }
        break;
    }
    case SELECT:   pad->PCTempJoyState.DPadUp = 255; break;
    case SQUARE:   pad->PCTempJoyState.DPadRight = 255; break;
    case TRIANGLE: pad->PCTempJoyState.DPadDown = 255; break;
    case CROSS:    pad->PCTempJoyState.DPadLeft = 255; break;
    default:       break;
    }
}

// 0x530ED0
void CControllerConfigManager::UpdateJoy_ButtonDown(CControllerKey::KeyCode button, eControllerType type) {
    if (GetIsKeyBlank(button, type)) {
        return;
    }
    CPad* pad = CPad::GetPad(0);
    if (!pad) {
        return;
    }
    auto* const state = &GetControllerState(*pad, type);
    if (UseDrivingControls()) {
        AffectControllerStateOn_ButtonDown_Driving(button, type, state);
    } else {
        AffectControllerStateOn_ButtonDown_FirstAndThirdPersonOnly(button, type, state);
        if (UseFirstPersonControls()) {
            AffectControllerStateOn_ButtonDown_FirstPersonOnly(button, type, state);
        }
        AffectControllerStateOn_ButtonDown_AllStates(button, type, state);
    }
    AffectControllerStateOn_ButtonDown_VehicleAndThirdPersonOnly(button, type, state);
}

// unused
// 0x52DC10
void CControllerConfigManager::AffectControllerStateOn_ButtonDown_DebugStuff(int32, eControllerType) {
    // NOP
    return;
}

// 0x52DC20
void CControllerConfigManager::UpdateJoyInConfigMenus_ButtonUp(CControllerKey::KeyCode button, int32 padNumber) {
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
void CControllerConfigManager::UpdateJoy_ButtonUp(CControllerKey::KeyCode button, eControllerType type) {
    bool shouldProcessEvent = false;
    CControllerState p_PCTempKeyState;
    const auto pad = CPad::GetPad();
    if (pad) {
        switch (type) {
        case eControllerType::KEYBOARD:
        case eControllerType::OPTIONAL_EXTRA_KEY:
            p_PCTempKeyState = pad->PCTempKeyState;
            shouldProcessEvent = (button != rsNULL);
            break;
        case eControllerType::MOUSE:
            p_PCTempKeyState = pad->PCTempMouseState;
            shouldProcessEvent = (button != 0);
            break;
        case eControllerType::JOY_STICK:
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
bool CControllerConfigManager::GetIsKeyboardKeyDown(CControllerKey::KeyCode key) {
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
bool CControllerConfigManager::GetIsKeyboardKeyJustDown(CControllerKey::KeyCode key) {
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
bool CControllerConfigManager::GetIsMouseButtonDown(CControllerKey::KeyCode key) {
    return CheckMouseButtonState(key);
}
 
// 0x52F020
bool CControllerConfigManager::GetIsMouseButtonUp(CControllerKey::KeyCode key) {
    return !CheckMouseButtonState(key);
}

// 0x52F110
bool CControllerConfigManager::GetIsMouseButtonJustUp(CControllerKey::KeyCode key) {
    return CheckMouseButtonJustUpState(key);
}
    
// unused
// 0x52F2A0
bool CControllerConfigManager::GetIsKeyBlank(CControllerKey::KeyCode key, eControllerType type) {
    switch (type) {
    case eControllerType::KEYBOARD:
    case eControllerType::OPTIONAL_EXTRA_KEY:
        return key == rsNULL;
    case eControllerType::MOUSE:
    case eControllerType::JOY_STICK:
        return key == 0;
    default:
        NOTSA_UNREACHABLE();
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
    switch (m_Actions[action].Keys[eControllerType::MOUSE].m_uiActionInitiator) {
    case rsMOUSE_LEFT_BUTTON:       return TheText.Get("FEC_MSL");
    case rsMOUSE_MIDDLE_BUTTON:     return TheText.Get("FEC_MSM");
    case rsMOUSE_RIGHT_BUTTON:      return TheText.Get("FEC_MSR");
    case rsMOUSE_WHEEL_UP_BUTTON:   return TheText.Get("FEC_MWF");
    case rsMOUSE_WHEEL_DOWN_BUTTON: return TheText.Get("FEC_MWB");
    case rsMOUSE_X1_BUTTON:         return TheText.Get("FEC_MXO");
    case rsMOUSE_X2_BUTTON:         return TheText.Get("FEC_MXT");
    default:                        return 0;
    }
}

// 0x52F450
const GxtChar* CControllerConfigManager::GetControllerSettingTextJoystick(eControllerAction action) {
    static thread_local GxtChar s_buttonName[64]; // 0xB7147C

    const auto keyCode = (RsKeyCodes)m_Actions[action].Keys[eControllerType::JOY_STICK].m_uiActionInitiator;
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
void CControllerConfigManager::MakeControllerActionsBlank() {
    for (auto i = 0u; i < CONTROLLER_NUM; ++i) {
        for (auto j = 0u; j < NUM_OF_CONTROLLER_ACTIONS; ++j) {
            ClearSettingsAssociatedWithAction((eControllerAction)(j), (eControllerType)(i));
        }
    }
}

// 0x531140
void CControllerConfigManager::AffectPadFromKeyBoard() {
    constexpr eControllerType CONTROLLER_TYPES[] = {eControllerType::KEYBOARD, eControllerType::OPTIONAL_EXTRA_KEY};

    RsKeyCodes keyCode;

    // GTATranslateShiftKey(&keyCode); // No matter what you do, it won't work.
    
    bool inMenu = false;
    if (!CPad::padNumber) {
        inMenu = !FrontEndMenuManager.m_bMenuActive;
    }
    
    for (auto i = 0u; i < NUM_OF_CONTROLLER_ACTIONS; ++i) {
        // Handle key press logic
        for (const auto& type : CONTROLLER_TYPES) {
            const auto key = m_Actions[i].Keys[type].m_uiActionInitiator;
            
            if (GetIsKeyboardKeyDown(key) && inMenu && key != rsNULL) {
                bool useFirstPersonControls = false;
                CPad* pad = CPad::GetPad(0);
                
                bool useDrivingControls = CControllerConfigManager::UseDrivingControls();
                auto& cameraMode = TheCamera.m_aCams[TheCamera.m_nActiveCam].m_nMode;
                if (notsa::contains({MODE_1STPERSON, MODE_SNIPER, MODE_ROCKETLAUNCHER, 
                    MODE_ROCKETLAUNCHER_HS, MODE_M16_1STPERSON, MODE_CAMERA}, cameraMode)) {
                    useFirstPersonControls = true;
                }
                
                if (pad) {
                    CControllerState* state = &pad->PCTempKeyState;
                    
                    if (useDrivingControls) {
                        AffectControllerStateOn_ButtonDown_Driving(key, type, state);
                    } else {
                        AffectControllerStateOn_ButtonDown_FirstAndThirdPersonOnly(key, type, state);
                        
                        if (useFirstPersonControls) {
                            AffectControllerStateOn_ButtonDown_FirstPersonOnly(key, type, state);
                        } else {
                            AffectControllerStateOn_ButtonDown_ThirdPersonOnly(key, type, state);
                        }
                    }
                    
                    if (m_Actions[CA_VEHICLE_ENTER_EXIT].Keys[type].m_uiActionInitiator == key) {
                        state->ButtonTriangle = 255;
                    }
                    
                    AffectControllerStateOn_ButtonDown_AllStates(key, type, state);
                }
            }
        }
        
        // Handle key release logic
        CPad* pad = CPad::GetPad(0);
        if (!pad || FrontEndMenuManager.m_bMenuActive) {
            continue;
        }
        
        CControllerState* keyState = &pad->PCTempKeyState;
        
        for (const auto& type : CONTROLLER_TYPES) {
            const auto key = m_Actions[i].Keys[type].m_uiActionInitiator;
            if (!GetIsKeyboardKeyDown(key) && key != rsNULL) {
                if (m_Actions[CA_NETWORK_TALK].Keys[type].m_uiActionInitiator == key) {
                    keyState->m_bChatIndicated = 0;
                }
                if (m_Actions[CA_VEHICLE_MOUSE_LOOK].Keys[type].m_uiActionInitiator == key) {
                    keyState->m_bVehicleMouseLook = 0;
                }
            }
        }
    }
}

// 0x5314A0
void CControllerConfigManager::AffectPadFromMouse() {
    CPad* pad = CPad::GetPad();
    bool controlsEnabled = !CPad::padNumber && !FrontEndMenuManager.m_bMenuActive;
    CControllerState* state = &pad->PCTempMouseState;

    for (auto& action : m_Actions) {
        const auto button = action.Keys[eControllerType::MOUSE].m_uiActionInitiator;

        if (CControllerConfigManager::GetIsMouseButtonDown(button)) {
            if (controlsEnabled && button && pad) {
                if (UseDrivingControls()) {
                    AffectControllerStateOn_ButtonDown_Driving(button, eControllerType::MOUSE, state);
                } else {
                    AffectControllerStateOn_ButtonDown_FirstAndThirdPersonOnly(button, eControllerType::MOUSE, state);
                    if (UseFirstPersonControls()) {
                        AffectControllerStateOn_ButtonDown_FirstPersonOnly(button, eControllerType::MOUSE, state);
                    } else {
                        AffectControllerStateOn_ButtonDown_ThirdPersonOnly(button, eControllerType::MOUSE, state);
                    }
                    AffectControllerStateOn_ButtonDown_AllStates(button, eControllerType::MOUSE, state);

                    AffectControllerStateOn_ButtonDown_VehicleAndThirdPersonOnly(button, eControllerType::MOUSE, state);
                }
            }
        }

        if (GetIsMouseButtonUp(button) && button && pad) {
            if (!FrontEndMenuManager.m_bMenuActive) {
                if (m_Actions[CA_NETWORK_TALK].Keys[eControllerType::MOUSE].m_uiActionInitiator == button) {
                    state->m_bChatIndicated = 0;
                }
                if (m_Actions[CA_VEHICLE_MOUSE_LOOK].Keys[eControllerType::MOUSE].m_uiActionInitiator == button) {
                    state->m_bVehicleMouseLook = 0;
                }
            }
        }
    }
}

// 0x531C90
void CControllerConfigManager::DeleteMatchingActionInitiators(eControllerAction action, CControllerKey::KeyCode button, eControllerType type) {
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
    static thread_local GxtChar s_KeyName[50]; // 0xB714BC

    const auto key = m_Actions[action].Keys[type].m_uiActionInitiator;

    const auto KeyNameWithNumber = [&](const GxtChar* text, int32 key) {
        CMessages::InsertNumberInString(text, key, -1, -1, -1, -1, -1, s_KeyName);
        return s_KeyName;
    };

    if (key == rsNULL) {
        return 0;
    }

    if (key > 0xFF) { /* Non-ASCII keys */
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
        case rsF12:      return KeyNameWithNumber(TheText.Get("FEC_FNC"), key - rsF1 + 1);
        case rsINS:      return TheText.Get("FEC_IRT");
        case rsDEL:      return TheText.Get("FEC_DLL");
        case rsHOME:     return TheText.Get("FEC_HME");
        case rsEND:      return TheText.Get("FEC_END");
        case rsPGUP:     return TheText.Get("FEC_PGU");
        case rsPGDN:     return TheText.Get("FEC_PGD");
        case rsUP:       return TheText.Get("FEC_UPA");
        case rsDOWN:     return TheText.Get("FEC_DWA");
        case rsLEFT:     return TheText.Get("FEC_LFA");
        case rsRIGHT:    return TheText.Get("FEC_RFA");
        case rsDIVIDE:   return TheText.Get("FEC_FWS");
        case rsTIMES:    return TheText.Get("FECSTAR");
        case rsPLUS:     return TheText.Get("FEC_PLS");
        case rsMINUS:    return TheText.Get("FEC_MIN");
        case rsPADDEL:   return TheText.Get("FEC_DOT");
        case rsPADEND:   return KeyNameWithNumber(TheText.Get("FEC_NMN"), 1);
        case rsPADDOWN:  return KeyNameWithNumber(TheText.Get("FEC_NMN"), 2);
        case rsPADPGDN:  return KeyNameWithNumber(TheText.Get("FEC_NMN"), 3);
        case rsPADLEFT:  return KeyNameWithNumber(TheText.Get("FEC_NMN"), 4);
        case rsPAD5:     return KeyNameWithNumber(TheText.Get("FEC_NMN"), 5);
        case rsPADRIGHT: return KeyNameWithNumber(TheText.Get("FEC_NMN"), 6);
        case rsPADHOME:  return KeyNameWithNumber(TheText.Get("FEC_NMN"), 7);
        case rsPADUP:    return KeyNameWithNumber(TheText.Get("FEC_NMN"), 8);
        case rsPADPGUP:  return KeyNameWithNumber(TheText.Get("FEC_NMN"), 9);
        case rsPADINS:   return KeyNameWithNumber(TheText.Get("FEC_NMN"), 0);
        case rsNUMLOCK:  return TheText.Get("FEC_NLK");
        case rsPADENTER: return TheText.Get("FEC_ETR");
        case rsSCROLL:   return TheText.Get("FEC_SLK");
        case rsPAUSE:    return TheText.Get("FEC_PSB");
        case rsBACKSP:   return TheText.Get("FEC_BSP");
        case rsTAB:      return TheText.Get("FEC_TAB");
        case rsCAPSLK:   return TheText.Get("FEC_CLK");
        case rsENTER:    return TheText.Get("FEC_RTN");
        case rsLSHIFT:   return TheText.Get("FEC_LSF");
        case rsRSHIFT:   return TheText.Get("FEC_RSF");
        case rsSHIFT:    return TheText.Get("FEC_SFT");
        case rsLCTRL:    return TheText.Get("FEC_LCT");
        case rsRCTRL:    return TheText.Get("FEC_RCT");
        case rsLALT:     return TheText.Get("FEC_LAL");
        case rsRALT:     return TheText.Get("FEC_RAL");
        case rsLWIN:     return TheText.Get("FEC_LWD");
        case rsRWIN:     return TheText.Get("FEC_RWD");
        case rsAPPS:     return TheText.Get("FEC_WRC");
        default: return nullptr;
        }
    } else { /* ASCII keys */
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
            s_KeyName[0] = (GxtChar)(key ? key : '#');
            s_KeyName[1] = '\0';
            return s_KeyName;
        }
    }
    return nullptr;
}

// 0x52F560
const GxtChar* CControllerConfigManager::GetButtonComboText(eControllerAction event) {
    return event == CA_VEHICLE_LOOKBEHIND
        ? TheText.Get("FEC_LBC")
        : nullptr;
}

// 0x5303D0
const GxtChar* CControllerConfigManager::GetDefinedKeyByGxtName(eControllerAction action) {
    static thread_local GxtChar s_ButtonName[64]; // 0xB7147C
    if (FrontEndMenuManager.m_ControlMethod) {
        if (const auto keyCode = m_Actions[action].Keys[eControllerType::JOY_STICK].m_uiActionInitiator) {
            const auto joyButtonText = TheText.Get("FEC_JBO");
            CMessages::InsertNumberInString(joyButtonText, keyCode, -1, -1, -1, -1, -1, s_ButtonName);
            return s_ButtonName;
        }
    }

    if (const auto keyText = GetControllerSettingTextMouse(action)) {
        return keyText;
    }

    if (const auto keyText = GetKeyNameForKeyboard(action, eControllerType::KEYBOARD)) {
        return keyText;
    }

    if (const auto keyText = GetKeyNameForKeyboard(action, eControllerType::OPTIONAL_EXTRA_KEY)) {
        return keyText;
    }

    if (m_Actions[action].Keys[eControllerType::JOY_STICK].m_uiActionInitiator == (RsKeyCodes)(0)) {
        return GetControllerSettingTextMouse(action);
    }

    return 0;
}

// NOTSA
eControllerAction CControllerConfigManager::GetActionIDByName(std::string_view name) {
    for (auto&& [i, actionName] : notsa::enumerate(m_ControllerActionName)) {
        char actionNameUTF8[128] = {0}; // Initialize with zeros to ensure null termination
        GxtCharToUTF8(actionNameUTF8, actionName);
        if (std::string_view{ actionNameUTF8 } == name) {
            return (eControllerAction)(i);
        }
    }
    return CA_INVALID;
}

// NOTSA
bool CControllerConfigManager::CheckMouseButtonState(CControllerKey::KeyCode key) {
    const auto* pad = CPad::GetPad();
    switch (key) {
    case rsMOUSE_LEFT_BUTTON:       return pad->IsMouseLButton();
    case rsMOUSE_RIGHT_BUTTON:      return pad->IsMouseRButton();
    case rsMOUSE_MIDDLE_BUTTON:     return pad->IsMouseMButton();
    case rsMOUSE_WHEEL_UP_BUTTON:   return pad->IsMouseWheelUp();
    case rsMOUSE_WHEEL_DOWN_BUTTON: return pad->IsMouseWheelDown();
    case rsMOUSE_X1_BUTTON:         return pad->IsMouseBmx1();
    case rsMOUSE_X2_BUTTON:         return pad->IsMouseBmx2();
    case 0:                         return false; /* unset button */
    default:                        NOTSA_UNREACHABLE("Invalid key ({})", (int32)(key));
    }
}

// NOTSA
bool CControllerConfigManager::CheckMouseButtonJustUpState(CControllerKey::KeyCode key) {
    const auto* pad = CPad::GetPad();
    switch (key) {
    case rsMOUSE_LEFT_BUTTON:       return pad->IsMouseLButtonPressed();
    case rsMOUSE_RIGHT_BUTTON:      return pad->IsMouseRButtonPressed();
    case rsMOUSE_MIDDLE_BUTTON:     return pad->IsMouseMButtonPressed();
    case rsMOUSE_WHEEL_UP_BUTTON:   return pad->IsMouseWheelUpPressed();
    case rsMOUSE_WHEEL_DOWN_BUTTON: return pad->IsMouseWheelDownPressed();
    case rsMOUSE_X1_BUTTON:         return pad->IsMouseBmx1Pressed();
    case rsMOUSE_X2_BUTTON:         return pad->IsMouseBmx2Pressed();
    case 0:                         return false; /* unset button */
    default:                        NOTSA_UNREACHABLE("Invalid key ({})", (int32)(key));
    }
}

// NOTSA
bool CControllerConfigManager::IsCheckSpecificGamepad()
{
    // TODO: Reverse CPadConfig
    return (PadConfigs[0].vendorId == 0x3427 && PadConfigs[0].productId == 0x1190);
}

// NOTSA
void CControllerConfigManager::CheckAndClear(eControllerAction action, eControllerType type, CControllerKey::KeyCode button) {
    if (m_Actions[action].Keys[type].m_uiActionInitiator == button) {
        ClearSettingsAssociatedWithAction(action, type);
    }
};

// NOTSA
void CControllerConfigManager::CheckAndSetButton(eControllerAction action, eControllerType type, CControllerKey::KeyCode button, int16& stateButton) {
    if (m_Actions[action].Keys[type].m_uiActionInitiator == button) {
        stateButton = 255;
    }
}

// NOTSA
void CControllerConfigManager::CheckAndSetPad(eControllerAction action, eControllerType type, CControllerKey::KeyCode button, int16& dpad, int16& oppositeDpad) {
    if (m_Actions[action].Keys[type].m_uiActionInitiator == button) {
        if (dpad) {
            dpad         = 0;
            oppositeDpad = 0;
        } else {
            dpad = 255;
        }
    }
}

bool CControllerConfigManager::UseDrivingControls() {
    // FindPlayerPed() && FindPlayerVehicle() && FindPlayerPed()->GetPedState() == PEDSTATE_DRIVING && !pad->DisablePlayerControls
    if (const auto* const plyr = FindPlayerPed()) {
        if (FindPlayerVehicle()) {
            return plyr->GetPedState() == PEDSTATE_DRIVING && !CPad::GetPad(0)->DisablePlayerControls;
        }
    }
    return false;
}

bool CControllerConfigManager::UseFirstPersonControls() {
    return notsa::contains({ MODE_1STPERSON, MODE_SNIPER, MODE_ROCKETLAUNCHER, MODE_ROCKETLAUNCHER_HS, MODE_M16_1STPERSON, MODE_CAMERA }, TheCamera.m_aCams[TheCamera.m_nActiveCam].m_nMode);
}

CControllerState& CControllerConfigManager::GetControllerState(CPad& pad, eControllerType ctrl) {
    switch (ctrl) {
    case eControllerType::KEYBOARD:
    case eControllerType::OPTIONAL_EXTRA_KEY: return pad.PCTempKeyState;
    case eControllerType::MOUSE:     return pad.PCTempMouseState;
    case eControllerType::JOY_STICK:       return pad.PCTempJoyState;
    default:                   NOTSA_UNREACHABLE();
    }
}
