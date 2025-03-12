#include "StdInc.h"

#include "ControllerConfigManager.h"

CControllerConfigManager& ControlsManager = *(CControllerConfigManager *) 0xB70198;

void CControllerConfigManager::InjectHooks() {
    RH_ScopedClass(CControllerConfigManager);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Constructor, 0x531EE0, { .reversed = false });
    RH_ScopedInstall(LoadSettings, 0x530530, { .reversed = false });
    RH_ScopedInstall(SaveSettings, 0x52D200, { .reversed = false });
    RH_ScopedInstall(InitDefaultControlConfiguration, 0x530640, { .reversed = false });
    RH_ScopedInstall(InitDefaultControlConfigMouse, 0x52F6F0);
    RH_ScopedInstall(InitialiseControllerActionNameArray, 0x52D260, { .reversed = false });
    RH_ScopedInstall(ReinitControls, 0x531F20, { .reversed = false });
    RH_ScopedInstall(SetMouseButtonAssociatedWithAction, 0x52F590);
    RH_ScopedInstall(StoreMouseButtonState, 0x52DA30, { .reversed = false });
    RH_ScopedInstall(UpdateJoyInConfigMenus_ButtonDown, 0x52DAB0, { .reversed = false });
    RH_ScopedInstall(UpdateJoy_ButtonDown, 0x530F42);
    RH_ScopedInstall(UpdateJoy_ButtonUp, 0x531070);
    RH_ScopedInstall(StoreJoyButtonStates, 0x52F510);
    RH_ScopedInstall(UpdateJoyInConfigMenus_ButtonUp, 0x52DC20, { .reversed = false });
    RH_ScopedInstall(AffectControllerStateOn_ButtonUp_DebugStuff, 0x52DD80, { .reversed = false });
    RH_ScopedInstall(ClearSimButtonPressCheckers, 0x52DD90, { .reversed = false });
    RH_ScopedInstall(GetJoyButtonJustUp, 0x52D1C0, { .reversed = false });
    RH_ScopedInstall(GetJoyButtonJustDown, 0x52D1E0, { .reversed = false });
    RH_ScopedInstall(GetIsKeyboardKeyDown, 0x52DDB0, { .reversed = false });
    RH_ScopedInstall(GetIsKeyboardKeyJustDown, 0x52E450, { .reversed = false });
    RH_ScopedInstall(GetIsMouseButtonDown, 0x52EF30);
    RH_ScopedInstall(GetIsMouseButtonUp, 0x52F020);
    RH_ScopedInstall(GetIsMouseButtonJustUp, 0x52F110);
    RH_ScopedInstall(GetIsKeyBlank, 0x52F2A0, { .reversed = false });
    RH_ScopedInstall(GetActionType, 0x52F2F0, { .reversed = false });
    RH_ScopedInstall(GetControllerSettingTextMouse, 0x52F390, { .reversed = false });
    RH_ScopedInstall(GetControllerSettingTextJoystick, 0x52F450, { .reversed = false });
    RH_ScopedInstall(ClearSettingsAssociatedWithAction, 0x52FD70, { .reversed = false });
    RH_ScopedInstall(MakeControllerActionsBlank, 0x530500, { .reversed = false });
    RH_ScopedInstall(AffectPadFromKeyBoard, 0x531140, { .reversed = false });
    RH_ScopedInstall(AffectPadFromMouse, 0x5314A0);
    RH_ScopedInstall(DeleteMatchingActionInitiators, 0x531C90, { .reversed = false });
    RH_ScopedInstall(SetControllerKeyAssociatedWithAction, 0x530490);
    RH_ScopedInstall(GetControllerKeyAssociatedWithAction, 0x52F4F0);
    RH_ScopedInstall(ResetSettingOrder, 0x52F5F0);
    RH_ScopedInstall(GetIsActionAButtonCombo, 0x52F550);

    RH_ScopedInstall(AffectControllerStateOn_ButtonDown, 0x530ED0, { .reversed = false });
    RH_ScopedInstall(AffectControllerStateOn_ButtonDown_Driving, 0x52F7B0, { .reversed = false });
    RH_ScopedInstall(AffectControllerStateOn_ButtonDown_VehicleAndThirdPersonOnly, 0x52FD20, { .reversed = false });
    RH_ScopedInstall(AffectControllerStateOn_ButtonDown_FirstAndThirdPersonOnly, 0x52FAB0, { .reversed = false });
    RH_ScopedInstall(AffectControllerStateOn_ButtonDown_FirstPersonOnly, 0x52F9E0, { .reversed = false });
    RH_ScopedInstall(AffectControllerStateOn_ButtonDown_ThirdPersonOnly, 0x52FA20, { .reversed = false });
    RH_ScopedInstall(AffectControllerStateOn_ButtonDown_AllStates, 0x52FCA0, { .reversed = false });
    RH_ScopedInstall(AffectControllerStateOn_ButtonDown_DebugStuff, 0x52DC10, { .reversed = false });
}

// 0x52F550
bool CControllerConfigManager::GetIsActionAButtonCombo(eControllerAction Action) {
    return 0;
}

// 0x531EE0
CControllerConfigManager::CControllerConfigManager() {
    plugin::CallMethod<0x531EE0, CControllerConfigManager*>(this);
}

CControllerConfigManager* CControllerConfigManager::Constructor() {
    this->CControllerConfigManager::CControllerConfigManager();
    return this;
}

// 0x52F510
void CControllerConfigManager::StoreJoyButtonStates() { // Name unknown (I made it up)
    for (auto&& [idx, bs] : notsa::enumerate(m_ButtonStates)) {
        bs = m_NewJoyState.rgbButtons[idx] >> 7;
    }
}

// NOTSA [Code combined from 0x7448B0 and 0x744930]
void CControllerConfigManager::HandleJoyButtonUpDown(int32 joyNo, bool isDown) {
    StoreJoyButtonStates();
    const auto forceConfigMenuMode = !isDown && notsa::contains({ MODE_FLYBY, MODE_FIXED }, TheCamera.GetActiveCamera().m_nMode); // Probably leftover debug stuff?
    for (auto i = isDown ? 1u : 2u; i < std::size(m_ButtonStates); i++) { // TODO: Why is this starting from 1/2?
        const auto padBtn = (eJOY_BUTTONS)((m_ButtonStates[i - 1] == isDown) ? i : 0); // This doesn't make sense
        if (forceConfigMenuMode || FrontEndMenuManager.m_bMenuActive || joyNo != 0) {
            if (isDown) {
                UpdateJoyInConfigMenus_ButtonDown(padBtn, joyNo);
            } else {
                UpdateJoyInConfigMenus_ButtonUp(padBtn, joyNo);
            }
        } else {
            if (isDown) {
                UpdateJoy_ButtonDown(padBtn, 3);
            } else {
                UpdateJoy_ButtonUp(padBtn, 3);
            }
        }
    }
}

// 0x530530
bool CControllerConfigManager::LoadSettings(FILESTREAM file) {
    return plugin::CallMethodAndReturn<bool, 0x530530, CControllerConfigManager*, FILESTREAM>(this, file);
}

// 0x52D200
void CControllerConfigManager::SaveSettings(FILESTREAM file) {
    plugin::CallMethod<0x52D200, CControllerConfigManager*, FILE*>(this, file);
}

// 0x530640
void CControllerConfigManager::InitDefaultControlConfiguration() {
    plugin::CallMethod<0x530640, CControllerConfigManager*>(this);
}

// 0x52F590
void CControllerConfigManager::SetMouseButtonAssociatedWithAction(const eControllerAction& action, eMouseButtons button) {
    int32 priorityCount = 0;
    for (int i = 0; i < 4; i++) {
        if (m_Actions[action].Keys[i].KeyCode != rsNULL) {
            priorityCount++;
        }
    }
    m_Actions[action].Keys[(int)eControllerType::MOUSE].KeyCode = (RsKeyCodes)button;
    m_Actions[action].Keys[(int)eControllerType::MOUSE].Priority = priorityCount + 1;
}

// 0x52F6F0
void CControllerConfigManager::InitDefaultControlConfigMouse(const CMouseControllerState& MouseSetUp, bool bMouseControls) {
    if (MouseSetUp.m_bLeftButton) {
        MouseFoundInitSet = true;
        SetMouseButtonAssociatedWithAction(CA_PED_FIREWEAPON, LEFT_MS_BUTTON);
        SetMouseButtonAssociatedWithAction(CA_VEHICLE_FIREWEAPON, LEFT_MS_BUTTON);
    }
    if (MouseSetUp.m_bRightButton) {
        SetMouseButtonAssociatedWithAction(CA_PED_LOCK_TARGET, RIGHT_MS_BUTTON);
        SetMouseButtonAssociatedWithAction(CA_VEHICLE_MOUSELOOK, RIGHT_MS_BUTTON);
    }
    if (MouseSetUp.m_bMiddleButton) {
        SetMouseButtonAssociatedWithAction(CA_VEHICLE_LOOKBEHIND, MIDDLE_MS_BUTTON);
        SetMouseButtonAssociatedWithAction(CA_PED_LOOKBEHIND, MIDDLE_MS_BUTTON);
    }
    if (MouseSetUp.m_bWheelMovedUp || MouseSetUp.m_bWheelMovedDown) {
        SetMouseButtonAssociatedWithAction(CA_PED_CYCLE_WEAPON_LEFT, WHEEL_MS_UP);
        SetMouseButtonAssociatedWithAction(CA_PED_CYCLE_WEAPON_RIGHT, WHEEL_MS_DOWN);
        SetMouseButtonAssociatedWithAction(CA_VEHICLE_RADIO_STATION_UP, WHEEL_MS_UP);
        SetMouseButtonAssociatedWithAction(CA_VEHICLE_RADIO_STATION_DOWN, WHEEL_MS_DOWN);
        SetMouseButtonAssociatedWithAction(CA_PED_SNIPER_ZOOM_IN, WHEEL_MS_UP);
        SetMouseButtonAssociatedWithAction(CA_PED_SNIPER_ZOOM_OUT, WHEEL_MS_DOWN);
    }
}

// 0x52D260
void CControllerConfigManager::InitialiseControllerActionNameArray() {
    plugin::CallMethod<0x52D260, CControllerConfigManager*>(this);
}

// 0x531F20
void CControllerConfigManager::ReinitControls() {
    plugin::CallMethod<0x531F20, CControllerConfigManager*>(this);
}

// 0x52DAB0
void CControllerConfigManager::UpdateJoyInConfigMenus_ButtonDown(eJOY_BUTTONS button, int32 padNumber) {
    plugin::CallMethod<0x52DAB0, CControllerConfigManager*, eJOY_BUTTONS, int32>(this, button, padNumber);
}

// 0x530F42
void CControllerConfigManager::UpdateJoy_ButtonDown(eJOY_BUTTONS button, int32 unk) {
    plugin::CallMethod<0x530F42>(this, button, unk);
}

// 0x52DC20
void CControllerConfigManager::UpdateJoyInConfigMenus_ButtonUp(eJOY_BUTTONS button, int32 padNumber) {
    plugin::CallMethod<0x52DC20, CControllerConfigManager*, eJOY_BUTTONS, int32>(this, button, padNumber);
}

// 0x531070
void CControllerConfigManager::UpdateJoy_ButtonUp(eJOY_BUTTONS button, int32 unk) {
    plugin::CallMethod<0x531070>(this, button, unk);
}

// unused
// 0x52DD80
void CControllerConfigManager::AffectControllerStateOn_ButtonUp_DebugStuff(int32, eControllerType) {
    // NOP
}

// 0x52DD90
void CControllerConfigManager::ClearSimButtonPressCheckers() {
    plugin::CallMethod<0x52DD90, CControllerConfigManager*>(this);
}

// unused
// 0x52D1C0
bool CControllerConfigManager::GetJoyButtonJustUp() {
    return plugin::CallMethodAndReturn<bool, 0x52D1C0, CControllerConfigManager*>(this);
}

// 0x52D1E0
bool CControllerConfigManager::GetJoyButtonJustDown() {
    return plugin::CallMethodAndReturn<bool, 0x52D1E0, CControllerConfigManager*>(this);
}

// 0x52DDB0
bool CControllerConfigManager::GetIsKeyboardKeyDown(RsKeyCodes key) {
    return plugin::CallMethodAndReturn<bool, 0x52DDB0, CControllerConfigManager*, RsKeyCodes>(this, key);
}

// 0x52E450
bool CControllerConfigManager::GetIsKeyboardKeyJustDown(RsKeyCodes key) {
    return plugin::CallMethodAndReturn<bool, 0x52E450, CControllerConfigManager*, RsKeyCodes>(this, key);
}

// unused
// 0x52DA30
void CControllerConfigManager::StoreMouseButtonState(eMouseButtons button, bool state) {
    switch (button) {
    case LEFT_MS_BUTTON:
        CPad::PCTempMouseControllerState.m_bLeftButton = state;
        break;
    case MIDDLE_MS_BUTTON:
        CPad::PCTempMouseControllerState.m_bMiddleButton = state;
        break;
    case RIGHT_MS_BUTTON:
        CPad::PCTempMouseControllerState.m_bRightButton = state;
        break;
    case WHEEL_MS_UP:
        CPad::PCTempMouseControllerState.m_bWheelMovedUp = state;
        break;
    case WHEEL_MS_DOWN:
        CPad::PCTempMouseControllerState.m_bWheelMovedDown = state;
        break;
    case FIRST_MS_X_BUTTON:
        CPad::PCTempMouseControllerState.m_bMsFirstXButton = state;
        break;
    case SECOND_MS_X_BUTTON:
        CPad::PCTempMouseControllerState.m_bMsSecondXButton = state;
        break;
    default: // ex: case NO_BUTTON:
        break;
    }
}

// 0x52F020
bool CControllerConfigManager::GetIsMouseButtonUp(RsKeyCodes key) {
    switch (key) {
    case LEFT_MS_BUTTON:
        return !CPad::GetPad(0)->NewMouseControllerState.m_bLeftButton;
    case MIDDLE_MS_BUTTON:
        return !CPad::GetPad(0)->NewMouseControllerState.m_bMiddleButton;
    case RIGHT_MS_BUTTON:
        return !CPad::GetPad(0)->NewMouseControllerState.m_bRightButton;
    case WHEEL_MS_UP:
        return !CPad::GetPad(0)->NewMouseControllerState.m_bWheelMovedUp;
    case WHEEL_MS_DOWN:
        return !CPad::GetPad(0)->NewMouseControllerState.m_bWheelMovedDown;
    case FIRST_MS_X_BUTTON:
        return !CPad::GetPad(0)->NewMouseControllerState.m_bMsFirstXButton;
    case SECOND_MS_X_BUTTON:
        return !CPad::GetPad(0)->NewMouseControllerState.m_bMsSecondXButton;
    default:
        return false;
    }
}

// 0x52EF30
bool CControllerConfigManager::GetIsMouseButtonDown(RsKeyCodes key) {
    switch (key) {
    case LEFT_MS_BUTTON:
        return CPad::GetPad(0)->NewMouseControllerState.m_bLeftButton;
    case MIDDLE_MS_BUTTON:
        return CPad::GetPad(0)->NewMouseControllerState.m_bMiddleButton;
    case RIGHT_MS_BUTTON:
        return CPad::GetPad(0)->NewMouseControllerState.m_bRightButton;
    case WHEEL_MS_UP:
        return CPad::GetPad(0)->NewMouseControllerState.m_bWheelMovedUp;
    case WHEEL_MS_DOWN:
        return CPad::GetPad(0)->NewMouseControllerState.m_bWheelMovedDown;
    case FIRST_MS_X_BUTTON:
        return CPad::GetPad(0)->NewMouseControllerState.m_bMsFirstXButton;
    case SECOND_MS_X_BUTTON:
        return CPad::GetPad(0)->NewMouseControllerState.m_bMsSecondXButton;
    default:
        return false;
    }
}

// 0x52F110
bool CControllerConfigManager::GetIsMouseButtonJustUp(RsKeyCodes key) {
    switch (key) {
    case LEFT_MS_BUTTON:
        return !CPad::GetPad(0)->NewMouseControllerState.m_bLeftButton && CPad::GetPad(0)->OldMouseControllerState.m_bLeftButton;
    case MIDDLE_MS_BUTTON:
        return !CPad::GetPad(0)->NewMouseControllerState.m_bMiddleButton && CPad::GetPad(0)->OldMouseControllerState.m_bMiddleButton;
    case RIGHT_MS_BUTTON:
        return !CPad::GetPad(0)->NewMouseControllerState.m_bRightButton && CPad::GetPad(0)->OldMouseControllerState.m_bRightButton;
    case WHEEL_MS_UP:
        return !CPad::GetPad(0)->NewMouseControllerState.m_bWheelMovedUp && CPad::GetPad(0)->OldMouseControllerState.m_bWheelMovedUp;
    case WHEEL_MS_DOWN:
        return !CPad::GetPad(0)->NewMouseControllerState.m_bWheelMovedDown && CPad::GetPad(0)->OldMouseControllerState.m_bWheelMovedDown;
    case FIRST_MS_X_BUTTON:
        return !CPad::GetPad(0)->NewMouseControllerState.m_bMsFirstXButton && CPad::GetPad(0)->OldMouseControllerState.m_bMsFirstXButton;
    case SECOND_MS_X_BUTTON:
        return !CPad::GetPad(0)->NewMouseControllerState.m_bMsSecondXButton && CPad::GetPad(0)->OldMouseControllerState.m_bMsSecondXButton;
    default:
        return false;
    }
}

// unused
// 0x52F2A0
bool CControllerConfigManager::GetIsKeyBlank(int32 a1, eControllerType controller) {
    return plugin::CallMethodAndReturn<bool, 0x52F2A0, CControllerConfigManager*, int32, eControllerType>(this, a1, controller);
}

// 0x52F2F0
eActionType CControllerConfigManager::GetActionType(eControllerAction Action) {
    switch (Action) {
    case CA_PED_FIREWEAPON:
    case CA_PED_FIREWEAPON_ALT:
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
        return ACTION_5;
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
        return ACTION_1;
    case CA_VEHICLE_ENTER_EXIT:
        return ACTION_3;
    case CA_CAMERA_CHANGE_VIEW_ALL_SITUATIONS:
    case CA_NETWORK_TALK:
    case CA_TOGGLE_DPAD:
    case CA_SWITCH_DEBUG_CAM_ON:
    case CA_TAKE_SCREEN_SHOT:
    case CA_SHOW_MOUSE_POINTER_TOGGLE:
        return ACTION_4;
    case CA_VEHICLE_FIREWEAPON:
    case CA_VEHICLE_FIREWEAPON_ALT:
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
    case CA_VEHICLE_LOOKLEFT:
    case CA_VEHICLE_LOOKRIGHT:
    case CA_VEHICLE_LOOKBEHIND:
    case CA_VEHICLE_MOUSELOOK:
    case CA_VEHICLE_TURRETLEFT:
    case CA_VEHICLE_TURRETRIGHT:
    case CA_VEHICLE_TURRETUP:
    case CA_VEHICLE_TURRETDOWN:
        return ACTION_2;
    }
    return ACTION_6;
}

// 0x52F390
char* CControllerConfigManager::GetControllerSettingTextMouse(eControllerAction action) {
    return plugin::CallMethodAndReturn<char*, 0x52F390, CControllerConfigManager*, eControllerAction>(this, action);
}

// 0x52F450
char* CControllerConfigManager::GetControllerSettingTextJoystick(eControllerAction action) {
    return plugin::CallMethodAndReturn<char*, 0x52F450, CControllerConfigManager*, eControllerAction>(this, action);
}

// 0x52FD70
void CControllerConfigManager::ClearSettingsAssociatedWithAction(eControllerAction action, eControllerType type) {
    plugin::CallMethod<0x52FD70, CControllerConfigManager*, eControllerAction, eControllerType>(this, action, type);
}

// 0x530500
void CControllerConfigManager::MakeControllerActionsBlank() {
    plugin::CallMethod<0x530500, CControllerConfigManager*>(this);
}

// 0x531140
void CControllerConfigManager::AffectPadFromKeyBoard() {
    plugin::CallMethod<0x531140, CControllerConfigManager*>(this);
}

// 0x530ED0
void CControllerConfigManager::AffectControllerStateOn_ButtonDown(int32 ButtonPress, eControllerType ControllerType) {
    plugin::CallMethod<0x530ED0, CControllerConfigManager*, int32, eControllerType>(this, ButtonPress, ControllerType);
}

// 0x52F7B0
void CControllerConfigManager::AffectControllerStateOn_ButtonDown_Driving(int32 ButtonPress, eControllerType ControllerType, CControllerState* ControllerToUpdate) {
    plugin::CallMethod<0x52F7B0, CControllerConfigManager*, int32, eControllerType, CControllerState*>(this, ButtonPress, ControllerType, ControllerToUpdate);
}

// 0x52FD20
void CControllerConfigManager::AffectControllerStateOn_ButtonDown_VehicleAndThirdPersonOnly(int32 ButtonPress, eControllerType ControllerType, CControllerState* ControllerToUpdate) {
    plugin::CallMethod<0x52FD20, CControllerConfigManager*, int32, eControllerType, CControllerState*>(this, ButtonPress, ControllerType, ControllerToUpdate);
}

// 0x52FAB0
void CControllerConfigManager::AffectControllerStateOn_ButtonDown_FirstAndThirdPersonOnly(int32 ButtonPress, eControllerType ControllerType, CControllerState* ControllerToUpdate) {
    plugin::CallMethod<0x52FAB0, CControllerConfigManager*, int32, eControllerType, CControllerState*>(this, ButtonPress, ControllerType, ControllerToUpdate);
}

// 0x52F9E0
void CControllerConfigManager::AffectControllerStateOn_ButtonDown_FirstPersonOnly(int32 ButtonPress, eControllerType ControllerType, CControllerState* ControllerToUpdate) {
    plugin::CallMethod<0x52F9E0, CControllerConfigManager*, int32, eControllerType, CControllerState*>(this, ButtonPress, ControllerType, ControllerToUpdate);
}

// 0x52FA20
void CControllerConfigManager::AffectControllerStateOn_ButtonDown_ThirdPersonOnly(int32 ButtonPress, eControllerType ControllerType, CControllerState* ControllerToUpdate) {
    plugin::CallMethod<0x52FA20, CControllerConfigManager*, int32, eControllerType, CControllerState*>(this, ButtonPress, ControllerType, ControllerToUpdate);
}

// 0x52FCA0
void CControllerConfigManager::AffectControllerStateOn_ButtonDown_AllStates(int32 ButtonPress, eControllerType ControllerType, CControllerState* ControllerToUpdate) {
    plugin::CallMethod<0x52FCA0, CControllerConfigManager*, int32, eControllerType, CControllerState*>(this, ButtonPress, ControllerType, ControllerToUpdate);
}

// 0x52DC10
// NOP
void CControllerConfigManager::AffectControllerStateOn_ButtonDown_DebugStuff(int32 ButtonPress, eControllerType ControllerType) {
    plugin::CallMethod<0x52DC10, CControllerConfigManager*, int32, eControllerType>(this, ButtonPress, ControllerType);
}

// 0x5314A0
void CControllerConfigManager::AffectPadFromMouse() {
    bool isNonMenuPad = false;
    if (CPad::padNumber == 0) {
        isNonMenuPad = !FrontEndMenuManager.m_bMenuActive;
    }

    // Iterate through all actions using a range-based for loop with index
    for (auto& action : m_Actions) {
        auto& keyCode = action.Keys[MOUSE].KeyCode;
        
        if (GetIsMouseButtonDown((RsKeyCodes)keyCode)) {
            if (isNonMenuPad && keyCode) {
                CPad* pad = CPad::GetPad(0);
                bool isVehicleControl = false;
                bool isFirstPersonCamera = false;

                // Check if player is in vehicle 
                if (CEntity* vehicle = FindPlayerVehicle(-1, 0)) {
                    CPlayerPed* playerPed = FindPlayerPed(-1);
                    if (playerPed && playerPed->m_nPedState == PEDSTATE_DRIVING) {
                        isVehicleControl = (pad->DisablePlayerControls == 0);
                    }
                }

                // Check camera mode
                isFirstPersonCamera |= notsa::contains({ MODE_1STPERSON, MODE_SNIPER, MODE_ROCKETLAUNCHER, MODE_ROCKETLAUNCHER_HS, MODE_M16_1STPERSON, MODE_CAMERA }, TheCamera.m_aCams[TheCamera.m_nActiveCam].m_nMode);

                auto* mousePad = &CPad::GetPad(0)->PCTempMouseState;
                
                if (pad) {
                    if (isVehicleControl) {
                        AffectControllerStateOn_ButtonDown_Driving(keyCode, eControllerType::MOUSE, mousePad);
                    } else {
                        AffectControllerStateOn_ButtonDown_FirstAndThirdPersonOnly(keyCode, eControllerType::MOUSE, mousePad);

                        if (isFirstPersonCamera) {
                            AffectControllerStateOn_ButtonDown_FirstPersonOnly(keyCode, eControllerType::MOUSE, mousePad);
                        } else {
                            AffectControllerStateOn_ButtonDown_ThirdPersonOnly(keyCode, eControllerType::MOUSE, mousePad);
                        }
                    }

                    // Check for triangle button action
                    if (keyCode == eControllerAction::CA_VEHICLE_ENTER_EXIT) {
                        mousePad->ButtonTriangle = 255;
                    }

                    AffectControllerStateOn_ButtonDown_AllStates(keyCode, eControllerType::MOUSE, mousePad);
                }
            }
        }

        if (GetIsMouseButtonUp((RsKeyCodes)keyCode)) {
            CControllerState* mouseState = &CPad::GetPad(0)->PCTempMouseState;
            
            if (keyCode && CPad::GetPad(0) && !FrontEndMenuManager.m_bMenuActive) {
                if (keyCode == eControllerAction::CA_NETWORK_TALK) {
                    mouseState->m_bChatIndicated = false;
                }
                if (keyCode == eControllerAction::CA_VEHICLE_MOUSELOOK) {
                    mouseState->m_bVehicleMouseLook = false;
                }
            }
        }
    }
}

// 0x531C90
void CControllerConfigManager::DeleteMatchingActionInitiators(eControllerAction Action, int32 KeyToBeChecked, eControllerType ControllerTypeToBeChecked) {
    plugin::CallMethod<0x531C90, CControllerConfigManager*, eControllerAction, int32, eControllerType>(this, Action, KeyToBeChecked, ControllerTypeToBeChecked);
}

// 0x52F5F0
void CControllerConfigManager::ResetSettingOrder(eControllerAction event) {
    for (int32 priority = 1; priority < 5; ++priority) {
        bool foundPriority = false;
        int32 result = -1;
        for (int32 keyIndex = 0; keyIndex < 4; ++keyIndex) {
            if (m_Actions[event].Keys[keyIndex].Priority == static_cast<int32>(priority)) {
                foundPriority = true;
                break;
            }
            if (static_cast<int32>(m_Actions[event].Keys[keyIndex].Priority) > priority && m_Actions[event].Keys[keyIndex].Priority != 0) {
                if (result == -1 || m_Actions[event].Keys[keyIndex].Priority < m_Actions[event].Keys[result].Priority) {
                    result = keyIndex;
                }
            }
        }
        if (!foundPriority && result != -1) {
            m_Actions[event].Keys[result].Priority = static_cast<int32>(priority);
        }
    }
}

// 0x530490
void CControllerConfigManager::SetControllerKeyAssociatedWithAction(eControllerAction action, int32 key, eControllerType type) {
    ResetSettingOrder(action);

    // Count how many keys are already assigned (not equal to 1056)
    int priorityCount = 0;
    for (int i = 0; i < 4; i++) {
        if (m_Actions[action].Keys[i].KeyCode != 1056) {
            priorityCount++;
        }
    }

    // Set the new key and its priority
    auto& actionKey = m_Actions[action].Keys[(int)type];
    actionKey.KeyCode = key;
    actionKey.Priority = priorityCount + 1;
}

// 0x52F4F0
RsKeyCodes CControllerConfigManager::GetControllerKeyAssociatedWithAction(eControllerAction event, eControllerType type) {
    return (RsKeyCodes)m_Actions[event].Keys[type].KeyCode;
}

// 0x5303D0
void CControllerConfigManager::GetDefinedKeyByGxtName(uint16 actionId, char* buf, uint16 bufsz) {
    plugin::CallMethod<0x5303D0>(this, actionId, buf, bufsz);
}

// NOTSA
uint16 CControllerConfigManager::GetActionIDByName(std::string_view name) {
    for (auto&& [i, actionName] : notsa::enumerate(m_arrControllerActionName)) {
        if (std::string_view{ actionName } == name) {
            return i;
        }
    }
    return (uint16)-1;
}
