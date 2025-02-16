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
    RH_ScopedInstall(AffectControllerStateOn_ButtonDown_DebugStuff, 0x52DC10, { .reversed = false });
    RH_ScopedInstall(UpdateJoyInConfigMenus_ButtonUp, 0x52DC20, { .reversed = false });
    RH_ScopedInstall(AffectControllerStateOn_ButtonUp_DebugStuff, 0x52DD80, { .reversed = false });
    RH_ScopedInstall(ClearSimButtonPressCheckers, 0x52DD90, { .reversed = false });
    RH_ScopedInstall(GetJoyButtonJustUp, 0x52D1C0, { .reversed = false });
    RH_ScopedInstall(GetJoyButtonJustDown, 0x52D1E0, { .reversed = false });
    RH_ScopedInstall(GetIsKeyboardKeyDown, 0x52DDB0, { .reversed = false });
    RH_ScopedInstall(GetIsKeyboardKeyJustDown, 0x52E450, { .reversed = false });
    RH_ScopedInstall(GetIsMouseButtonDown, 0x52EF30, { .reversed = false });
    RH_ScopedInstall(GetIsMouseButtonUp, 0x52F020, { .reversed = false });
    RH_ScopedInstall(GetIsMouseButtonJustUp, 0x52F110, { .reversed = false });
    RH_ScopedInstall(GetIsKeyBlank, 0x52F2A0, { .reversed = false });
    RH_ScopedInstall(GetActionType, 0x52F2F0, { .reversed = false });
    RH_ScopedInstall(GetControllerSettingTextMouse, 0x52F390, { .reversed = false });
    RH_ScopedInstall(GetControllerSettingTextJoystick, 0x52F450, { .reversed = false });
    RH_ScopedInstall(ClearSettingsAssociatedWithAction, 0x52FD70, { .reversed = false });
    RH_ScopedInstall(MakeControllerActionsBlank, 0x530500, { .reversed = false });
    RH_ScopedInstall(AffectPadFromKeyBoard, 0x531140, { .reversed = false });
    RH_ScopedInstall(AffectPadFromMouse, 0x5314A0, { .reversed = false });
    RH_ScopedInstall(DeleteMatchingActionInitiators, 0x531C90, { .reversed = false });
    RH_ScopedInstall(SetControllerKeyAssociatedWithAction, 0x530490);
    RH_ScopedInstall(GetControllerKeyAssociatedWithAction, 0x52F4F0);
    RH_ScopedInstall(ResetSettingOrder, 0x52F5F0);
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
        const auto padBtn = (ePadButton)((m_ButtonStates[i - 1] == isDown) ? i : 0); // This doesn't make sense
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

// 0x52F6F0
void CControllerConfigManager::InitDefaultControlConfigMouse(const CMouseControllerState& state, bool controller) {
    if (state.lmb) {
        MouseFoundInitSet = true;
        SetMouseButtonAssociatedWithAction(CA_PED_FIREWEAPON,             rsMOUSE_LEFT_BUTTON);
        SetMouseButtonAssociatedWithAction(CA_VEHICLE_FIREWEAPON,         rsMOUSE_LEFT_BUTTON);
    }

    if (state.rmb) {
        SetMouseButtonAssociatedWithAction(CA_PED_LOCK_TARGET,            rsMOUSE_RIGHT_BUTTON);
        SetMouseButtonAssociatedWithAction(CA_VEHICLE_MOUSELOOK,          rsMOUSE_RIGHT_BUTTON);
    }

    if (state.mmb) {
        SetMouseButtonAssociatedWithAction(CA_VEHICLE_LOOKBEHIND,         rsMOUSE_MIDDLE_BUTTON);
        SetMouseButtonAssociatedWithAction(CA_PED_LOOKBEHIND,             rsMOUSE_MIDDLE_BUTTON);
    }

    if (state.wheelUp || state.wheelDown) {
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
    plugin::CallMethod<0x52D260, CControllerConfigManager*>(this);
}

// 0x531F20
void CControllerConfigManager::ReinitControls() {
    plugin::CallMethod<0x531F20, CControllerConfigManager*>(this);
}

// 0x52F590
void CControllerConfigManager::SetMouseButtonAssociatedWithAction(eControllerAction action, RsKeyCodes button) {
    plugin::CallMethod<0x52F590, CControllerConfigManager*, eControllerAction, RsKeyCodes>(this, action, button);
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
        case MOUSE_BUTTON_NONE:
            break;
    }
}

// 0x52DAB0
void CControllerConfigManager::UpdateJoyInConfigMenus_ButtonDown(ePadButton button, int32 padNumber) {
    plugin::CallMethod<0x52DAB0, CControllerConfigManager*, ePadButton, int32>(this, button, padNumber);
}

// 0x530F42
void CControllerConfigManager::UpdateJoy_ButtonDown(ePadButton button, int32 unk) {
    plugin::CallMethod<0x530F42>(this, button, unk);
}

// unused
// 0x52DC10
void CControllerConfigManager::AffectControllerStateOn_ButtonDown_DebugStuff(int32, eControllerType) {
    // NOP
}

// 0x52DC20
void CControllerConfigManager::UpdateJoyInConfigMenus_ButtonUp(ePadButton button, int32 padNumber) {
    plugin::CallMethod<0x52DC20, CControllerConfigManager*, ePadButton, int32>(this, button, padNumber);
}

// 0x531070
void CControllerConfigManager::UpdateJoy_ButtonUp(ePadButton button, int32 unk) {
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

// 0x52EF30
bool CControllerConfigManager::GetIsMouseButtonDown(RsKeyCodes key) {
    return plugin::CallMethodAndReturn<bool, 0x52EF30, CControllerConfigManager*, RsKeyCodes>(this, key);
}
 
// 0x52F020
bool CControllerConfigManager::GetIsMouseButtonUp(RsKeyCodes key) {
    return plugin::CallMethodAndReturn<bool, 0x52F020, CControllerConfigManager*, RsKeyCodes>(this, key);
}

// 0x52F110
bool CControllerConfigManager::GetIsMouseButtonJustUp(RsKeyCodes key) {
    return plugin::CallMethodAndReturn<bool, 0x52F110, CControllerConfigManager*, RsKeyCodes>(this, key);
}

// unused
// 0x52F2A0
bool CControllerConfigManager::GetIsKeyBlank(int32 a1, eControllerType controller) {
    return plugin::CallMethodAndReturn<bool, 0x52F2A0, CControllerConfigManager*, int32, eControllerType>(this, a1, controller);
}

// 0x52F2F0
eActionType CControllerConfigManager::GetActionType(eControllerAction action) {
    switch (action) {
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
        case CA_SNEAK_ABOUT:
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
        case CA_VEHICLE_STEERLEFT:
        case CA_VEHICLE_STEERRIGHT:
        case CA_VEHICLE_STEERUP:
        case CA_VEHICLE_STEERDOWN:
        case CA_VEHICLE_ACCELERATE:
        case CA_VEHICLE_BRAKE:
        case CA_VEHICLE_RADIO_STATION_UP:
        case CA_VEHICLE_RADIO_STATION_DOWN:
        case CA_UNKNOWN_28:
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
        default:
            return ACTION_6;
    }
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

// 0x5314A0
void CControllerConfigManager::AffectPadFromMouse() {
    plugin::CallMethod<0x5314A0, CControllerConfigManager*>(this);
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
    auto& actionKey = m_Actions[action].Keys[type];
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
