#include "StdInc.h"

#include "ControllerConfigManager.h"
#include "Input.h"
#include "WndProc.h"

CControllerConfigManager& ControlsManager = *(CControllerConfigManager*)0xB70198;
GxtChar (&NewStringWithNumber)[32] = *(GxtChar(*)[32])0xB7147C; // temp value

void CControllerConfigManager::InjectHooks() {
    RH_ScopedClass(CControllerConfigManager);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Constructor, 0x531EE0);
    RH_ScopedInstall(ReinitControls, 0x531F20, { .reversed = false });
    RH_ScopedInstall(ResetSettingOrder, 0x52F5F0, { .reversed = false });

    RH_ScopedInstall(SaveSettings, 0x52D200, { .reversed = false });
    RH_ScopedInstall(LoadSettings, 0x530530);

    RH_ScopedInstall(GetJoyButtonJustUp, 0x52D1C0);
    RH_ScopedInstall(GetJoyButtonJustDown, 0x52D1E0);
    RH_ScopedInstall(InitialiseControllerActionNameArray, 0x52D260);
    RH_ScopedInstall(StoreMouseButtonState, 0x52DA30);
    RH_ScopedInstall(UpdateJoyInConfigMenus_ButtonDown, 0x52DAB0);
    RH_ScopedInstall(AffectControllerStateOn_ButtonDown_DebugStuff, 0x52DC10);
    RH_ScopedInstall(UpdateJoyInConfigMenus_ButtonUp, 0x52DC20);
    RH_ScopedInstall(AffectControllerStateOn_ButtonUp_DebugStuff, 0x52DD80);
    RH_ScopedInstall(ClearSimButtonPressCheckers, 0x52DD90);
    RH_ScopedInstall(GetIsKeyboardKeyDown, 0x52DDB0);
    RH_ScopedInstall(GetIsKeyboardKeyJustDown, 0x52E450);
    RH_ScopedInstall(GetIsMouseButtonDown, 0x52EF30);
    RH_ScopedInstall(GetIsMouseButtonUp, 0x52F020);
    RH_ScopedInstall(GetIsMouseButtonJustUp, 0x52F110);
    RH_ScopedInstall(GetIsKeyBlank, 0x52F2A0);
    RH_ScopedInstall(GetActionType, 0x52F2F0);
    RH_ScopedInstall(GetControllerSettingTextMouse, 0x52F390);
    RH_ScopedInstall(GetControllerSettingTextJoystick, 0x52F450);
    RH_ScopedInstall(sub_52F4A0, 0x52F4A0);
    RH_ScopedInstall(GetControllerKeyAssociatedWithAction, 0x52F4F0);
    RH_ScopedInstall(StoreJoyButtonStates, 0x52F510);
    RH_ScopedInstall(GetIsActionAButtonCombo, 0x52F550);
    RH_ScopedInstall(GetButtonComboText, 0x52F560);
    RH_ScopedInstall(GetMouseButtonAssociatedWithAction, 0x52F580);
    RH_ScopedInstall(SetMouseButtonAssociatedWithAction, 0x52F590);
    RH_ScopedInstall(InitDefaultControlConfigMouse, 0x52F6F0);
    RH_ScopedInstall(HandleButtonPress, 0x52F7B0);
    RH_ScopedInstall(Handle1stPersonCameraActions, 0x52F9E0);
    RH_ScopedInstall(UpdatePadStateOnActions, 0x52FA20);
    RH_ScopedInstall(UpdateControllerStateOnAction, 0x52FAB0);
    RH_ScopedInstall(UpdatePadStateOnMenuActions, 0x52FCA0);
    RH_ScopedInstall(sub_52FD20, 0x52FD20);
    RH_ScopedInstall(AffectControllerStateOn_ButtonUp_All_Player_States, 0x52FD40);
    RH_ScopedInstall(ClearSettingsAssociatedWithAction, 0x52FD70);
    RH_ScopedInstall(GetControllerSettingTextKeyBoard, 0x52FE10);
    RH_ScopedInstall(GetDefinedKeyByGxtName, 0x5303D0);
    RH_ScopedInstall(SetControllerKeyAssociatedWithAction, 0x530490);
    RH_ScopedInstall(MakeControllerActionsBlank, 0x530500);
    RH_ScopedInstall(InitDefaultControlConfiguration, 0x530640);
    RH_ScopedInstall(InitDefaultControlConfigJoyPad, 0x530B00);
    RH_ScopedInstall(UpdateJoy_ButtonDown, 0x530ED0);
    RH_ScopedInstall(UpdateJoy_ButtonUp, 0x531070);
    RH_ScopedInstall(AffectPadFromKeyBoard, 0x531140);
    RH_ScopedInstall(AffectPadFromMouse, 0x5314A0);
    RH_ScopedInstall(DeleteMatchingCommonControls, 0x531670);
    RH_ScopedInstall(UnmapPedKey, 0x531730);
    RH_ScopedInstall(DeleteMatching1rst3rdPersonControls, 0x5318C0);
    RH_ScopedInstall(DeleteMatchingVehicleControls, 0x5319D0);
    RH_ScopedInstall(UnmapVehicleEnterExit, 0x531BC0);
    RH_ScopedInstall(DeleteMatching1rstPersonControls, 0x531C20);
    RH_ScopedInstall(DeleteMatchingActionInitiators, 0x531C90);
    RH_ScopedInstall(GetControllerSettingText, 0x531E20);
    RH_ScopedInstall(GetActionKeyName, 0x531E90);
}

// 0x531EE0
CControllerConfigManager::CControllerConfigManager() {
    // Member variable init done in header

    MakeControllerActionsBlank();
    InitDefaultControlConfiguration();
    InitialiseControllerActionNameArray();
}

// 0x531730
void CControllerConfigManager::UnmapPedKey(eControllerAction action, CControllerKey::KeyCode button, eControllerType type) {
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

    if (FrontEndMenuManager.m_bController) {
        CheckAndClear(CA_PED_CENTER_CAMERA_BEHIND_PLAYER, type, button);
    }
}

// 0x531670
void CControllerConfigManager::DeleteMatchingCommonControls(eControllerAction nop, CControllerKey::KeyCode button, eControllerType type) {
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
void CControllerConfigManager::SetControllerKeyAssociatedWithAction(eControllerAction action, CControllerKey::KeyCode button, eControllerType type) {
    ResetSettingOrder(action);
    auto& actionLocal = m_Actions[action];
    auto existingMappings = 0u;
    for (const auto& typeTemp : CONTROLLER_TYPES_ALL) {
        if (!GetIsKeyBlank(button, typeTemp)) {
            existingMappings++;
        }
    }
    actionLocal.Keys[type].Key = button;
    actionLocal.Keys[type].Order = (eContSetOrder)existingMappings++;
}

// 0x5319D0
void CControllerConfigManager::DeleteMatchingVehicleControls(eControllerAction nop, CControllerKey::KeyCode button, eControllerType type) {
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
void CControllerConfigManager::DeleteMatching1rst3rdPersonControls(eControllerAction action, CControllerKey::KeyCode button, eControllerType type) {
    if (GetIsKeyBlank(button, type)) {
        return;
    }
    if (action != CA_PED_ANSWER_PHONE) {
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
        bs = (m_NewJoyState.rgbButtons[idx] & 0x80) != 0;
    }
}

// unused
// 0x531E90
const GxtChar* CControllerConfigManager::GetActionKeyName(eControllerAction action) {
    ResetSettingOrder(action);
    for (const auto& type : CONTROLLER_TYPES_KEYBOARD) {
        if (m_Actions[action].Keys[type].Order) {
            return GetControllerSettingTextKeyBoard(action, type);
        }
    }
    return nullptr;
}

// 0x531E20
const GxtChar* CControllerConfigManager::GetControllerSettingText(eControllerAction action, eContSetOrder order) {
    for (const auto& type : CONTROLLER_TYPES_ALL ) {
        if (m_Actions[action].Keys[type].Order == order) {
            switch (type) {
            case CONTROLLER_KEYBOARD1:
            case CONTROLLER_KEYBOARD2: return GetControllerSettingTextKeyBoard(action, type);
            case CONTROLLER_MOUSE:     return GetControllerSettingTextMouse(action);
            case CONTROLLER_PAD:       return GetControllerSettingTextJoystick(action);
            default:                   NOTSA_UNREACHABLE();
            }
        }
    }
    return nullptr;
}

// 0x531C20
void CControllerConfigManager::DeleteMatching1rstPersonControls(eControllerAction nop, CControllerKey::KeyCode button, eControllerType type) {
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
void CControllerConfigManager::ClearSettingsAssociatedWithAction(eControllerAction action, eControllerType type) {
    switch (type) {
    case CONTROLLER_KEYBOARD1:
    case CONTROLLER_KEYBOARD2:
        m_Actions[action].Keys[type].Key   = rsNULL;
        m_Actions[action].Keys[type].Order = NO_ORDER_SET;
        break;
    case CONTROLLER_MOUSE:
    case CONTROLLER_PAD:
        m_Actions[action].Keys[type].Key   = (RsKeyCodes)0;
        m_Actions[action].Keys[type].Order = NO_ORDER_SET;
        break;
    default: NOTSA_UNREACHABLE();
    }
    ResetSettingOrder(action);
}

// unused
// 0x52FD20
eControllerType CControllerConfigManager::sub_52FD20(CControllerKey::KeyCode button, eControllerType type, CControllerState* state) {
    CheckAndSetButton(CA_VEHICLE_ENTER_EXIT, type, button, state->ButtonTriangle);
    return type;
}

// 0x52FCA0
CControllerState* CControllerConfigManager::UpdatePadStateOnMenuActions(CControllerKey::KeyCode button, eControllerType type, CControllerState* state) {
    CheckAndSetButton(CA_CAMERA_CHANGE_VIEW_ALL_SITUATIONS, type, button, state->Select);
    CheckAndSetPad(CA_CONVERSATION_NO, type, button, state->DPadLeft, state->DPadRight);
    CheckAndSetPad(CA_CONVERSATION_YES, type, button, state->DPadRight, state->DPadLeft);
    CheckAndSetButton(CA_NETWORK_TALK, type, button, state->m_bChatIndicated);
    return state;
}

// 0x52F580
int32 CControllerConfigManager::GetMouseButtonAssociatedWithAction(eControllerAction action) {
    return m_Actions[action].Keys[CONTROLLER_MOUSE].Key;
}

// 0x52FAB0
eControllerType CControllerConfigManager::UpdateControllerStateOnAction(CControllerKey::KeyCode button, eControllerType type, CControllerState* state) {
    CheckAndSetButton(CA_PED_FIRE_WEAPON, type, button, state->ButtonCircle);
    CheckAndSetButton(CA_PED_FIRE_WEAPON_ALT, type, button, state->LeftShoulder1);
    CheckAndSetButton(CA_PED_LOCK_TARGET, type, button, state->RightShoulder1);

    CheckAndSetStick_FirstThirdPerson(CA_GO_FORWARD, type, button, state->LeftStickY, m_bStickL_Up_Dwn_MovementBothDown[type], -128);
    CheckAndSetStick_FirstThirdPerson(CA_GO_BACK, type, button, state->LeftStickY, m_bStickL_Up_Dwn_MovementBothDown[type], 128);
    CheckAndSetStick_FirstThirdPerson(CA_GO_LEFT, type, button, state->LeftStickX, m_bStickL_X_Rgh_Lft_MovementBothDown[type], -128);
    CheckAndSetStick_FirstThirdPerson(CA_GO_RIGHT, type, button, state->LeftStickX, m_bStickL_X_Rgh_Lft_MovementBothDown[type], 128);

    CheckAndSetButton(CA_PED_WALK, type, button, state->m_bPedWalk);

    CheckAndSetPad(CA_GROUP_CONTROL_FWD, type, button, state->DPadUp, state->DPadDown);
    CheckAndSetPad(CA_GROUP_CONTROL_BWD, type, button, state->DPadDown, state->DPadUp);

    CheckAndSetStick_FirstThirdPerson(CA_PED_1RST_PERSON_LOOK_LEFT, type, button, state->RightStickX, m_bStickR_X_Rgh_Lft_MovementBothDown[type], -128);
    CheckAndSetStick_FirstThirdPerson(CA_PED_1RST_PERSON_LOOK_RIGHT, type, button, state->RightStickX, m_bStickR_X_Rgh_Lft_MovementBothDown[type], 128);  

    if (FrontEndMenuManager.m_bController) {
        CheckAndSetStick_FirstThirdPerson(CA_PED_1RST_PERSON_LOOK_UP, type, button, state->RightStickY, m_bStickR_Up_Dwn_MovementBothDown[type], 128);
        CheckAndSetStick_FirstThirdPerson(CA_PED_1RST_PERSON_LOOK_DOWN, type, button, state->RightStickY, m_bStickR_Up_Dwn_MovementBothDown[type], -128);
    }

    return type;
}

// 0x52FA20
int32 CControllerConfigManager::UpdatePadStateOnActions(CControllerKey::KeyCode button, eControllerType type, CControllerState* state) {
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
    return m_Actions[action].Keys[type].Key;
}

// 0x52F9E0
void CControllerConfigManager::Handle1stPersonCameraActions(CControllerKey::KeyCode button, eControllerType type, CControllerState* state) {
    CheckAndSetButton(CA_PED_SNIPER_ZOOM_IN, type, button, state->ButtonSquare);
    CheckAndSetButton(CA_PED_SNIPER_ZOOM_OUT, type, button, state->ButtonCross);
    CheckAndSetButton(CA_PED_DUCK, type, button, state->ShockButtonL);
}

// 0x52FD40
void CControllerConfigManager::AffectControllerStateOn_ButtonUp_All_Player_States(CControllerKey::KeyCode button, eControllerType type, CControllerState* state) {
    CheckAndResetButtonState(CA_NETWORK_TALK, type, button, state->m_bChatIndicated);
    CheckAndResetButtonState(CA_VEHICLE_MOUSE_LOOK, type, button, state->m_bVehicleMouseLook);
}

// 0x52F7B0
void CControllerConfigManager::HandleButtonPress(CControllerKey::KeyCode button, eControllerType type, CControllerState* state) {
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
    CheckAndSetStick_Driving(CA_VEHICLE_STEER_UP, type, button, state->LeftStickY, m_bStickL_Up_Dwn_MovementBothDown[type], -128);
    CheckAndSetStick_Driving(CA_VEHICLE_STEER_DOWN, type, button, state->LeftStickY, m_bStickL_Up_Dwn_MovementBothDown[type], 128);
    CheckAndSetStick_Driving(CA_VEHICLE_STEER_LEFT, type, button, state->LeftStickX, m_bStickL_X_Rgh_Lft_MovementBothDown[type], -128);
    CheckAndSetStick_Driving(CA_VEHICLE_STEER_RIGHT, type, button, state->LeftStickX, m_bStickL_X_Rgh_Lft_MovementBothDown[type], 128);
    CheckAndSetButton(CA_VEHICLE_RADIO_STATION_UP, type, button, state->DPadUp);
    CheckAndSetButton(CA_VEHICLE_RADIO_STATION_DOWN, type, button, state->DPadDown);
    CheckAndSetButton(CA_VEHICLE_RADIO_TRACK_SKIP, type, button, state->m_bRadioTrackSkip);
    CheckAndSetButton(CA_TOGGLE_SUBMISSIONS, type, button, state->ShockButtonR);
    CheckAndSetStick_Driving(CA_VEHICLE_TURRET_LEFT, type, button, state->RightStickX, m_bStickR_X_Rgh_Lft_MovementBothDown[type], -128);
    CheckAndSetStick_Driving(CA_VEHICLE_TURRET_RIGHT, type, button, state->RightStickX, m_bStickR_X_Rgh_Lft_MovementBothDown[type], 128);
    CheckAndSetStick_Driving(CA_VEHICLE_TURRET_UP, type, button, state->RightStickY, m_bStickR_Up_Dwn_MovementBothDown[type], 128);
    CheckAndSetStick_Driving(CA_VEHICLE_TURRET_DOWN, type, button, state->RightStickY, m_bStickR_Up_Dwn_MovementBothDown[type], -128);
}

// 0x52F5F0
void CControllerConfigManager::ResetSettingOrder(eControllerAction ctrlAction) {
    //auto& action = m_Actions[ctrlAction];
    rng::sort(m_Actions[ctrlAction].Keys, [](auto&& a, auto&& b) {
        return b.Order != -1 && a.Order < b.Order;
    });


    //for (auto order = 1; order <= CONTROLLER_NUM; ++order) {
    //    if (rng::find(m_Actions[ctrlAction].Keys, order, &CControllerKey::Order)) {
    //        continue;
    //    }
    //    auto it = rng::max_element(
    //        action.Keys,
    //        [](uint32 a, uint32 b) {
    //            return b != -1 && a < b;
    //        },
    //        &CControllerKey::Order
    //    );
    //    if (it == rng::end(action.Keys) || it->Order == -1) {
    //        continue;
    //    }
    //    it->Order = order;
    //    return (eControllerType)(it - std::begin(action.Keys));
    //
    //    //ePriority minPriority = PRIORITY_NONE;
    //    //for (int32 controller = CONTROLLER_FIRST; controller <= CONTROLLER_LAST; ++controller) {
    //    //    ePriority currentPriority = m_Actions[ctrlAction].Keys[controller].Priority;
    //    //    if (currentPriority > (ePriority)priority && currentPriority != PRIORITY_DISABLE) {
    //    //        if (minPriority == PRIORITY_NONE || currentPriority < m_Actions[ctrlAction].Keys[type].Priority) {
    //    //            minPriority = currentPriority;
    //    //            type = (eControllerType)(controller);
    //    //        }
    //    //    }
    //    //}
    //    //if (minPriority != PRIORITY_NONE) {
    //    //    m_Actions[ctrlAction].Keys[type].Priority = (ePriority)(priority);
    //    //}
    //}
    //return CONTROLLER_KEYBOARD1;
}

// NOTSA [Code combined from 0x7448B0 and 0x744930]
void CControllerConfigManager::HandleJoyButtonUpDown(int32 joyNo, bool isDown) {
    StoreJoyButtonStates();
    const auto forceConfigMenuMode = !isDown && notsa::contains({ MODE_FLYBY, MODE_FIXED }, TheCamera.GetActiveCamera().m_nMode); // Probably leftover debug stuff?
    for (auto i = isDown ? 1u : 2u; i < std::size(m_ButtonStates); i++) { // TODO: Why is this starting from 1/2?
        const auto padBtn = (m_ButtonStates[i - 1] == isDown) ? i : 0; // This doesn't make sense
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
        return true; // 0x530543 - Invert
    }

    char buffer[29] = {0};
    CFileMgr::Read(file, buffer, 29);
    if (!strncmp(buffer, "THIS FILE IS NOT VALID YET", 26)) {
        return true; // 0x530562 - Invert
    }

    CFileMgr::Seek(file, 0, SEEK_SET);
    int32 version;
    CFileMgr::Read(file, &version, sizeof(version));

    if (version < 6) {
        return true; // 0x530591 - Invert
    }

    int32 actionId = 0;
    // Verify file format by checking action IDs
    for (auto& action : m_Actions) {
        CFileMgr::Read(file, &actionId, CONTROLLER_NUM);
        if (actionId != action.Keys->Order) {
            return false;
        }
        CFileMgr::Seek(file, 8, SEEK_CUR); // Skip key mapping data for validation pass
    }

    CFileMgr::Seek(file, 4, SEEK_SET);
    MakeControllerActionsBlank();

    for (auto& action : m_Actions) {
        CFileMgr::Seek(file, 4, SEEK_CUR);
        CFileMgr::Read(file, &action.Keys, CONTROLLER_NUM);
    }
    return true;
}

// 0x52D200
int32 CControllerConfigManager::SaveSettings(FILESTREAM file) {
    return plugin::CallMethodAndReturn<int32, 0x52D200, CControllerConfigManager*, FILE*>(this, file);
}

// 0x530640
void CControllerConfigManager::InitDefaultControlConfiguration() {
    SetControllerKeyAssociatedWithAction(CA_GO_FORWARD, rsUP, CONTROLLER_KEYBOARD1);
    SetControllerKeyAssociatedWithAction(CA_GO_BACK, rsDOWN, CONTROLLER_KEYBOARD1);
    SetControllerKeyAssociatedWithAction(CA_GO_LEFT, rsLEFT, CONTROLLER_KEYBOARD1);
    SetControllerKeyAssociatedWithAction(CA_GO_RIGHT, rsRIGHT, CONTROLLER_KEYBOARD1);
    if (FrontEndMenuManager.m_nTextLanguage == 2) {
        SetControllerKeyAssociatedWithAction(CA_CONVERSATION_YES, (RsKeyCodes)'Z', CONTROLLER_KEYBOARD1);
        SetControllerKeyAssociatedWithAction(CA_PED_SNIPER_ZOOM_IN, (RsKeyCodes)'Y', CONTROLLER_KEYBOARD2);
    } else {
        SetControllerKeyAssociatedWithAction(CA_CONVERSATION_YES, (RsKeyCodes)'Y', CONTROLLER_KEYBOARD1);
        SetControllerKeyAssociatedWithAction(CA_PED_SNIPER_ZOOM_IN, (RsKeyCodes)'X', CONTROLLER_KEYBOARD2);
    }
    if (FrontEndMenuManager.m_nTextLanguage == 1) {
        SetControllerKeyAssociatedWithAction(CA_GO_FORWARD, (RsKeyCodes)'Z', CONTROLLER_KEYBOARD2);
        SetControllerKeyAssociatedWithAction(CA_GO_LEFT, (RsKeyCodes)'Q', CONTROLLER_KEYBOARD2);
        SetControllerKeyAssociatedWithAction(CA_PED_CYCLE_WEAPON_LEFT, (RsKeyCodes)'A', CONTROLLER_KEYBOARD1);
        SetControllerKeyAssociatedWithAction(CA_PED_SNIPER_ZOOM_OUT, (RsKeyCodes)'W', CONTROLLER_KEYBOARD2);
    } else {
        SetControllerKeyAssociatedWithAction(CA_GO_FORWARD, (RsKeyCodes)'W', CONTROLLER_KEYBOARD2);
        SetControllerKeyAssociatedWithAction(CA_GO_LEFT, (RsKeyCodes)'A', CONTROLLER_KEYBOARD2);
        SetControllerKeyAssociatedWithAction(CA_PED_CYCLE_WEAPON_LEFT, (RsKeyCodes)'Q', CONTROLLER_KEYBOARD1);
        SetControllerKeyAssociatedWithAction(CA_PED_SNIPER_ZOOM_OUT, (RsKeyCodes)'Z', CONTROLLER_KEYBOARD2);
    }
    SetControllerKeyAssociatedWithAction(CA_GO_BACK, (RsKeyCodes)'S', CONTROLLER_KEYBOARD2);
    SetControllerKeyAssociatedWithAction(CA_GO_RIGHT, (RsKeyCodes)'D', CONTROLLER_KEYBOARD2);
    SetControllerKeyAssociatedWithAction(CA_PED_CYCLE_WEAPON_RIGHT, (RsKeyCodes)'E', CONTROLLER_KEYBOARD1);
    SetControllerKeyAssociatedWithAction(CA_PED_CYCLE_WEAPON_RIGHT, rsPADENTER, CONTROLLER_KEYBOARD2);
    SetControllerKeyAssociatedWithAction(CA_PED_CYCLE_WEAPON_LEFT, rsPADDEL, CONTROLLER_KEYBOARD2);
    SetControllerKeyAssociatedWithAction(CA_PED_FIRE_WEAPON, rsPADINS, CONTROLLER_KEYBOARD1);
    SetControllerKeyAssociatedWithAction(CA_PED_FIRE_WEAPON, rsLCTRL, CONTROLLER_KEYBOARD2);
    SetControllerKeyAssociatedWithAction(CA_PED_SNIPER_ZOOM_IN, rsPGUP, CONTROLLER_KEYBOARD1);
    SetControllerKeyAssociatedWithAction(CA_PED_SNIPER_ZOOM_OUT, rsPGDN, CONTROLLER_KEYBOARD1);
    SetControllerKeyAssociatedWithAction(CA_PED_FIRE_WEAPON_ALT, (RsKeyCodes)'\\', CONTROLLER_KEYBOARD1);
    SetControllerKeyAssociatedWithAction(CA_GROUP_CONTROL_FWD, (RsKeyCodes)'G', CONTROLLER_KEYBOARD1);
    SetControllerKeyAssociatedWithAction(CA_GROUP_CONTROL_BWD, (RsKeyCodes)'H', CONTROLLER_KEYBOARD1);
    SetControllerKeyAssociatedWithAction(CA_CONVERSATION_NO, (RsKeyCodes)'N', CONTROLLER_KEYBOARD1);
    SetControllerKeyAssociatedWithAction(CA_VEHICLE_ENTER_EXIT, (RsKeyCodes)'F', CONTROLLER_KEYBOARD2);
    SetControllerKeyAssociatedWithAction(CA_VEHICLE_ENTER_EXIT, rsENTER, CONTROLLER_KEYBOARD1);
    SetControllerKeyAssociatedWithAction(CA_CAMERA_CHANGE_VIEW_ALL_SITUATIONS, (RsKeyCodes)'V', CONTROLLER_KEYBOARD1);
    SetControllerKeyAssociatedWithAction(CA_CAMERA_CHANGE_VIEW_ALL_SITUATIONS, rsHOME, CONTROLLER_KEYBOARD2);
    SetControllerKeyAssociatedWithAction(CA_PED_JUMPING, rsLSHIFT, CONTROLLER_KEYBOARD1);
    SetControllerKeyAssociatedWithAction(CA_PED_JUMPING, rsRCTRL, CONTROLLER_KEYBOARD2);
    SetControllerKeyAssociatedWithAction(CA_PED_SPRINT, (RsKeyCodes)' ', CONTROLLER_KEYBOARD1);
    SetControllerKeyAssociatedWithAction(CA_PED_DUCK, (RsKeyCodes)'C', CONTROLLER_KEYBOARD1);
    SetControllerKeyAssociatedWithAction(CA_PED_ANSWER_PHONE, rsTAB, CONTROLLER_KEYBOARD1);
    SetControllerKeyAssociatedWithAction(CA_PED_WALK, rsLALT, CONTROLLER_KEYBOARD1);
    SetControllerKeyAssociatedWithAction(CA_PED_LOCK_TARGET, rsDEL, CONTROLLER_KEYBOARD1);
    SetControllerKeyAssociatedWithAction(CA_PED_LOCK_TARGET, rsCAPSLK, CONTROLLER_KEYBOARD2);
    SetControllerKeyAssociatedWithAction(CA_PED_1RST_PERSON_LOOK_LEFT, rsPADLEFT, CONTROLLER_KEYBOARD1);
    SetControllerKeyAssociatedWithAction(CA_PED_1RST_PERSON_LOOK_RIGHT, rsPADRIGHT, CONTROLLER_KEYBOARD1);
    SetControllerKeyAssociatedWithAction(CA_PED_1RST_PERSON_LOOK_UP, rsPADDOWN, CONTROLLER_KEYBOARD1);
    SetControllerKeyAssociatedWithAction(CA_PED_1RST_PERSON_LOOK_DOWN, rsPADUP, CONTROLLER_KEYBOARD1);
    SetControllerKeyAssociatedWithAction(CA_PED_CENTER_CAMERA_BEHIND_PLAYER, (RsKeyCodes)'#', CONTROLLER_KEYBOARD1);
    SetControllerKeyAssociatedWithAction(CA_PED_LOOKBEHIND, rsPADEND, CONTROLLER_KEYBOARD1);
    SetControllerKeyAssociatedWithAction(CA_VEHICLE_STEER_UP, rsUP, CONTROLLER_KEYBOARD1);
    SetControllerKeyAssociatedWithAction(CA_VEHICLE_STEER_DOWN, rsDOWN, CONTROLLER_KEYBOARD1);
    SetControllerKeyAssociatedWithAction(CA_VEHICLE_RADIO_STATION_UP, rsINS, CONTROLLER_KEYBOARD1);
    if (FrontEndMenuManager.m_nTextLanguage == 1) {
        SetControllerKeyAssociatedWithAction(CA_VEHICLE_STEER_LEFT, (RsKeyCodes)'Q', CONTROLLER_KEYBOARD1);
        SetControllerKeyAssociatedWithAction(CA_VEHICLE_LOOK_LEFT, (RsKeyCodes)'A', CONTROLLER_KEYBOARD1);
        SetControllerKeyAssociatedWithAction(CA_VEHICLE_RADIO_STATION_UP, (RsKeyCodes)'`', CONTROLLER_KEYBOARD2);
        SetControllerKeyAssociatedWithAction(CA_TOGGLE_SUBMISSIONS, (RsKeyCodes)233, CONTROLLER_KEYBOARD1);
        SetControllerKeyAssociatedWithAction(CA_VEHICLE_ACCELERATE, (RsKeyCodes)'Z', CONTROLLER_KEYBOARD1);
    } else {
        SetControllerKeyAssociatedWithAction(CA_VEHICLE_STEER_LEFT, (RsKeyCodes)'A', CONTROLLER_KEYBOARD1);
        SetControllerKeyAssociatedWithAction(CA_VEHICLE_LOOK_LEFT, (RsKeyCodes)'Q', CONTROLLER_KEYBOARD1);
        SetControllerKeyAssociatedWithAction(CA_VEHICLE_RADIO_STATION_UP, (RsKeyCodes)'4', CONTROLLER_KEYBOARD2);
        SetControllerKeyAssociatedWithAction(CA_TOGGLE_SUBMISSIONS, (RsKeyCodes)'2', CONTROLLER_KEYBOARD1);
        SetControllerKeyAssociatedWithAction(CA_VEHICLE_ACCELERATE, (RsKeyCodes)'W', CONTROLLER_KEYBOARD1);
    }
    SetControllerKeyAssociatedWithAction(CA_TOGGLE_SUBMISSIONS, rsPLUS, CONTROLLER_KEYBOARD2);
    SetControllerKeyAssociatedWithAction(CA_VEHICLE_FIRE_WEAPON, rsRCTRL, CONTROLLER_KEYBOARD1);
    SetControllerKeyAssociatedWithAction(CA_VEHICLE_FIRE_WEAPON, rsLALT, CONTROLLER_KEYBOARD2);
    SetControllerKeyAssociatedWithAction(CA_VEHICLE_FIRE_WEAPON_ALT, rsLCTRL, CONTROLLER_KEYBOARD1);
    SetControllerKeyAssociatedWithAction(CA_VEHICLE_FIRE_WEAPON_ALT, rsPADINS, CONTROLLER_KEYBOARD2);
    SetControllerKeyAssociatedWithAction(CA_VEHICLE_BRAKE, (RsKeyCodes)'S', CONTROLLER_KEYBOARD1);
    SetControllerKeyAssociatedWithAction(CA_VEHICLE_STEER_RIGHT, (RsKeyCodes)'D', CONTROLLER_KEYBOARD1);
    SetControllerKeyAssociatedWithAction(CA_VEHICLE_STEER_LEFT, rsLEFT, CONTROLLER_KEYBOARD2);
    SetControllerKeyAssociatedWithAction(CA_VEHICLE_STEER_RIGHT, rsRIGHT, CONTROLLER_KEYBOARD2);
    SetControllerKeyAssociatedWithAction(CA_VEHICLE_HORN, rsCAPSLK, CONTROLLER_KEYBOARD1);
    SetControllerKeyAssociatedWithAction(CA_VEHICLE_HORN, (RsKeyCodes)'H', CONTROLLER_KEYBOARD2);
    SetControllerKeyAssociatedWithAction(CA_VEHICLE_HANDBRAKE, (RsKeyCodes)' ', CONTROLLER_KEYBOARD1);
    SetControllerKeyAssociatedWithAction(CA_VEHICLE_HANDBRAKE, rsRCTRL, CONTROLLER_KEYBOARD2);
    SetControllerKeyAssociatedWithAction(CA_VEHICLE_LOOK_RIGHT, (RsKeyCodes)'E', CONTROLLER_KEYBOARD1);
    SetControllerKeyAssociatedWithAction(CA_VEHICLE_TURRET_LEFT, rsPADLEFT, CONTROLLER_KEYBOARD1);
    SetControllerKeyAssociatedWithAction(CA_VEHICLE_TURRET_RIGHT, rsPADRIGHT, CONTROLLER_KEYBOARD1);
    SetControllerKeyAssociatedWithAction(CA_VEHICLE_TURRET_UP, rsPADDOWN, CONTROLLER_KEYBOARD1);
    SetControllerKeyAssociatedWithAction(CA_VEHICLE_TURRET_DOWN, rsPADUP, CONTROLLER_KEYBOARD1);
    SetControllerKeyAssociatedWithAction(CA_VEHICLE_TURRET_UP, rsEND, CONTROLLER_KEYBOARD2);
    SetControllerKeyAssociatedWithAction(CA_VEHICLE_TURRET_DOWN, rsDEL, CONTROLLER_KEYBOARD2);
    SetControllerKeyAssociatedWithAction(CA_VEHICLE_RADIO_TRACK_SKIP, rsF5, CONTROLLER_KEYBOARD1);
    SetControllerKeyAssociatedWithAction(CA_VEHICLE_RADIO_STATION_DOWN, rsDEL, CONTROLLER_KEYBOARD1);
    SetControllerKeyAssociatedWithAction(CA_VEHICLE_RADIO_STATION_DOWN, (RsKeyCodes)'R', CONTROLLER_KEYBOARD2);
    SetControllerKeyAssociatedWithAction(CA_PED_CYCLE_TARGET_LEFT, (RsKeyCodes)'[', CONTROLLER_KEYBOARD1);
    SetControllerKeyAssociatedWithAction(CA_PED_CYCLE_TARGET_RIGHT, (RsKeyCodes)']', CONTROLLER_KEYBOARD1);
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
            SetControllerKeyAssociatedWithAction(action.first, action.second, CONTROLLER_PAD);
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
    if (state.lmb) {
        m_MouseFoundInitSet = true;
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
    GxtCharStrcpy(m_ControllerActionName[CA_PED_LOOKBEHIND], "PED_LOOKBEHIND"_gxt);
    GxtCharStrcpy(m_ControllerActionName[CA_PED_CYCLE_WEAPON_LEFT], "PED_CYCLE_WEAPON_LEFT"_gxt);
    GxtCharStrcpy(m_ControllerActionName[CA_PED_CYCLE_WEAPON_RIGHT], "PED_CYCLE_WEAPON_RIGHT"_gxt);
    GxtCharStrcpy(m_ControllerActionName[CA_PED_LOCK_TARGET], "PED_LOCK_TARGET"_gxt);
    GxtCharStrcpy(m_ControllerActionName[CA_PED_JUMPING], "PED_JUMPING"_gxt);
    GxtCharStrcpy(m_ControllerActionName[CA_PED_SPRINT], "PED_SPRINT"_gxt);
    GxtCharStrcpy(m_ControllerActionName[CA_PED_WALK], "SNEAK_ABOUT"_gxt);
    GxtCharStrcpy(m_ControllerActionName[CA_PED_CYCLE_TARGET_LEFT], "PED_CYCLE_TARGET_LEFT"_gxt);
    GxtCharStrcpy(m_ControllerActionName[CA_PED_CYCLE_TARGET_RIGHT], "PED_CYCLE_TARGET_RIGHT"_gxt);
    GxtCharStrcpy(m_ControllerActionName[CA_PED_CENTER_CAMERA_BEHIND_PLAYER], "PED_CENTER_CAMERA_BEHIND_PLAYER"_gxt);
    GxtCharStrcpy(m_ControllerActionName[CA_VEHICLE_LOOKBEHIND], "VEHICLE_LOOKBEHIND"_gxt);
    GxtCharStrcpy(m_ControllerActionName[CA_PED_DUCK], "PED_DUCK"_gxt);
    GxtCharStrcpy(m_ControllerActionName[CA_PED_ANSWER_PHONE], "PED_ANSWER_PHONE"_gxt);
    GxtCharStrcpy(m_ControllerActionName[CA_VEHICLE_STEER_LEFT], "VEHICLE_STEERLEFT"_gxt);
    GxtCharStrcpy(m_ControllerActionName[CA_VEHICLE_STEER_RIGHT], "VEHICLE_STEERRIGHT"_gxt);
    GxtCharStrcpy(m_ControllerActionName[CA_VEHICLE_STEER_UP], "VEHICLE_STEERUP"_gxt);
    GxtCharStrcpy(m_ControllerActionName[CA_VEHICLE_STEER_DOWN], "VEHICLE_STEERDOWN"_gxt);
    GxtCharStrcpy(m_ControllerActionName[CA_VEHICLE_LOOK_LEFT], "VEHICLE_LOOKLEFT"_gxt);
    GxtCharStrcpy(m_ControllerActionName[CA_VEHICLE_LOOK_RIGHT], "VEHICLE_LOOKRIGHT"_gxt);
    GxtCharStrcpy(m_ControllerActionName[CA_VEHICLE_MOUSE_LOOK], "VEHICLE_MOUSELOOK"_gxt);
    GxtCharStrcpy(m_ControllerActionName[CA_VEHICLE_HORN], "VEHICLE_HORN"_gxt);
    GxtCharStrcpy(m_ControllerActionName[CA_VEHICLE_HANDBRAKE], "VEHICLE_HANDBRAKE"_gxt);
    GxtCharStrcpy(m_ControllerActionName[CA_VEHICLE_ACCELERATE], "VEHICLE_ACCELERATE"_gxt);
    GxtCharStrcpy(m_ControllerActionName[CA_VEHICLE_BRAKE], "VEHICLE_BRAKE"_gxt);
    GxtCharStrcpy(m_ControllerActionName[CA_VEHICLE_RADIO_STATION_UP], "VEHICLE_RADIO_STATION_UP"_gxt);
    GxtCharStrcpy(m_ControllerActionName[CA_VEHICLE_RADIO_STATION_DOWN], "VEHICLE_RADIO_STATION_DOWN"_gxt);
    GxtCharStrcpy(m_ControllerActionName[CA_TOGGLE_SUBMISSIONS], "TOGGLE_SUBMISSIONS"_gxt);
    GxtCharStrcpy(m_ControllerActionName[CA_PED_SNIPER_ZOOM_IN], "PED_SNIPER_ZOOM_IN"_gxt);
    GxtCharStrcpy(m_ControllerActionName[CA_PED_SNIPER_ZOOM_OUT], "PED_SNIPER_ZOOM_OUT"_gxt);
    GxtCharStrcpy(m_ControllerActionName[CA_PED_1RST_PERSON_LOOK_LEFT], "PED_1RST_PERSON_LOOK_LEFT"_gxt);
    GxtCharStrcpy(m_ControllerActionName[CA_PED_1RST_PERSON_LOOK_RIGHT], "PED_1RST_PERSON_LOOK_RIGHT"_gxt);
    GxtCharStrcpy(m_ControllerActionName[CA_PED_1RST_PERSON_LOOK_UP], "PED_1RST_PERSON_LOOK_UP"_gxt);
    GxtCharStrcpy(m_ControllerActionName[CA_PED_1RST_PERSON_LOOK_DOWN], "PED_1RST_PERSON_LOOK_DOWN"_gxt);
    GxtCharStrcpy(m_ControllerActionName[CA_SHOW_MOUSE_POINTER_TOGGLE], "SHOW_MOUSE_POINTER_TOGGLE"_gxt);
    GxtCharStrcpy(m_ControllerActionName[CA_CAMERA_CHANGE_VIEW_ALL_SITUATIONS], "CAMERA_CHANGE_VIEW_ALL_SITUATIONS"_gxt);
    GxtCharStrcpy(m_ControllerActionName[CA_PED_FIRE_WEAPON], "PED_FIREWEAPON"_gxt);
    GxtCharStrcpy(m_ControllerActionName[CA_PED_FIRE_WEAPON_ALT], "PED_FIREWEAPON_ALT"_gxt);
    GxtCharStrcpy(m_ControllerActionName[CA_VEHICLE_FIRE_WEAPON], "VEHICLE_FIREWEAPON"_gxt);
    GxtCharStrcpy(m_ControllerActionName[CA_VEHICLE_FIRE_WEAPON_ALT], "VEHICLE_FIREWEAPON_ALT"_gxt);
    GxtCharStrcpy(m_ControllerActionName[CA_VEHICLE_ENTER_EXIT], "VEHICLE_ENTER_EXIT"_gxt);
    GxtCharStrcpy(m_ControllerActionName[CA_CONVERSATION_NO], "CONVERSATION_NO"_gxt);
    GxtCharStrcpy(m_ControllerActionName[CA_CONVERSATION_YES], "CONVERSATION_YES"_gxt);
    GxtCharStrcpy(m_ControllerActionName[CA_GROUP_CONTROL_FWD], "GROUP_CONTROL_FWD"_gxt);
    GxtCharStrcpy(m_ControllerActionName[CA_GROUP_CONTROL_BWD], "GROUP_CONTROL_BWD"_gxt);
    GxtCharStrcpy(m_ControllerActionName[CA_GO_LEFT], "GO_LEFT"_gxt);
    GxtCharStrcpy(m_ControllerActionName[CA_GO_RIGHT], "GO_RIGHT"_gxt);
    GxtCharStrcpy(m_ControllerActionName[CA_GO_FORWARD], "GO_FORWARD"_gxt);
    GxtCharStrcpy(m_ControllerActionName[CA_GO_BACK], "GO_BACK"_gxt);
    GxtCharStrcpy(m_ControllerActionName[CA_VEHICLE_TURRET_LEFT], "VEHICLE_TURRETLEFT"_gxt);
    GxtCharStrcpy(m_ControllerActionName[CA_VEHICLE_TURRET_RIGHT], "VEHICLE_TURRETRIGHT"_gxt);
    GxtCharStrcpy(m_ControllerActionName[CA_VEHICLE_TURRET_UP], "VEHICLE_TURRETUP"_gxt);
    GxtCharStrcpy(m_ControllerActionName[CA_VEHICLE_TURRET_DOWN], "VEHICLE_TURRETDOWN"_gxt);
    GxtCharStrcpy(m_ControllerActionName[CA_NETWORK_TALK], "NETWORK_TALK"_gxt);
    GxtCharStrcpy(m_ControllerActionName[CA_TOGGLE_DPAD], "TOGGLE_DPAD"_gxt);
    GxtCharStrcpy(m_ControllerActionName[CA_SWITCH_DEBUG_CAM_ON], "SWITCH_DEBUG_CAM_ON"_gxt);
    GxtCharStrcpy(m_ControllerActionName[CA_TAKE_SCREEN_SHOT], "TAKE_SCREEN_SHOT"_gxt);
}

// 0x531F20
bool CControllerConfigManager::ReinitControls() {
    return 0;
}

// 0x52F590
void CControllerConfigManager::SetMouseButtonAssociatedWithAction(eControllerAction action, CControllerKey::KeyCode button) {
    auto& actionLocal = m_Actions[action];
    auto assignedSlotsCount = 0u;
    for (const auto& type : CONTROLLER_TYPES_ALL) {
        if (!GetIsKeyBlank(button, type)) {
            assignedSlotsCount++;
        }
    }
    actionLocal.Keys[CONTROLLER_MOUSE].Key = button;
    actionLocal.Keys[CONTROLLER_MOUSE].Order = (eContSetOrder)assignedSlotsCount++;
}

// unused
// 0x52DA30
void CControllerConfigManager::StoreMouseButtonState(eMouseButtons button, bool state) {
    switch (button) {
    case MOUSE_BUTTON_LEFT:           CPad::PCTempMouseControllerState.lmb = state; break;
    case MOUSE_BUTTON_MIDDLE:         CPad::PCTempMouseControllerState.mmb = state; break;
    case MOUSE_BUTTON_RIGHT:          CPad::PCTempMouseControllerState.rmb = state; break;
    case MOUSE_BUTTON_WHEEL_UP:       CPad::PCTempMouseControllerState.wheelUp = state; break;
    case MOUSE_BUTTON_WHEEL_DOWN:     CPad::PCTempMouseControllerState.wheelDown = state; break;
    case MOUSE_BUTTON_WHEEL_XBUTTON1: CPad::PCTempMouseControllerState.bmx1 = state; break;
    case MOUSE_BUTTON_WHEEL_XBUTTON2: CPad::PCTempMouseControllerState.bmx2 = state; break;
    default:                          NOTSA_UNREACHABLE();
    }
}

// 0x52DAB0
void CControllerConfigManager::UpdateJoyInConfigMenus_ButtonDown(CControllerKey::KeyCode button, int32 padNumber) {
    CPad* pad = CPad::GetPad(padNumber);
    if (!pad || button == 0) {
        return;
    }

    switch (button) {
    case 5:  pad->PCTempJoyState.LeftShoulder2 = 255; break;
    case 6:  pad->PCTempJoyState.RightShoulder2 = 255; break;
    case 7:  pad->PCTempJoyState.LeftShoulder1 = 255; break;
    case 8:  pad->PCTempJoyState.RightShoulder1 = 255; break;
    case 9:  pad->PCTempJoyState.Select = 255; break;
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

    if (IsCheckSpecificGamepad()) {
        switch (button) {
        case 1:  pad->PCTempJoyState.ButtonTriangle = 255; break;
        case 2:  pad->PCTempJoyState.ButtonCircle = 255; break;
        case 3:  pad->PCTempJoyState.ButtonCross = 255; break;
        case 4:  pad->PCTempJoyState.ButtonSquare = 255; break;
        }
    } else {
        switch (button) {
        case 1:  pad->PCTempJoyState.ButtonCircle = 255; break;
        case 2:  pad->PCTempJoyState.ButtonCross = 255; break;
        case 3:  pad->PCTempJoyState.ButtonSquare = 255; break;
        case 4:  pad->PCTempJoyState.ButtonTriangle = 255; break;
        }
    }
}

// 0x530ED0
void CControllerConfigManager::UpdateJoy_ButtonDown(CControllerKey::KeyCode button, eControllerType type) {
    if (GetIsKeyBlank(button, type)) {
        return;
    }

    CPad* pad = CPad::GetPad();
    if (!pad) {
        return; // 0x530FF7 - Invert
    }

    auto* state = &GetControllerState(*pad, type);
    if (UseDrivingControls()) {
        HandleButtonPress(button, type, state);
        sub_52FD20(button, type, state);
    } else {
        UpdateControllerStateOnAction(button, type, state);
        if (UseFirstPersonControls()) {
            Handle1stPersonCameraActions(button, type, state);
        } else {
            UpdatePadStateOnActions(button, type, state);
            sub_52FD20(button, type, state);
        }
    }
    UpdatePadStateOnMenuActions(button, type, state);
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
    case 12:
        if (padNumber == 1) {
            pad->PCTempJoyState.Start = 0;
        }
        break;
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
    } else {
        switch (button) {
        case 1: pad->PCTempJoyState.ButtonCircle = 0; break;
        case 2: pad->PCTempJoyState.ButtonCross = 0; break;
        case 3: pad->PCTempJoyState.ButtonSquare = 0; break;
        case 4: pad->PCTempJoyState.ButtonTriangle = 0; break;
        }
    }
}

// 0x531070
void CControllerConfigManager::UpdateJoy_ButtonUp(CControllerKey::KeyCode button, eControllerType type) {
    const auto pad = CPad::GetPad();
    if (!GetIsKeyBlank(button, type) && pad && !FrontEndMenuManager.m_bMenuActive) {
        AffectControllerStateOn_ButtonUp_All_Player_States(button, type, &GetControllerState(*pad, type));
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

    if (key >= rsF1 && key <= rsF12) {
        return pad->NewKeyState.FKeys[key - rsF1];
    }

    switch (key) {
    // NOTE: If anyone wants to translate all the code into `Pad.h`, please do:
    case rsESC:      return pad->NewKeyState.esc;
    case rsINS:      return pad->NewKeyState.insert;
    case rsDEL:      return pad->NewKeyState.del;
    case rsHOME:     return pad->NewKeyState.home;
    case rsEND:      return pad->NewKeyState.end;
    case rsPGUP:     return pad->NewKeyState.pgup;
    case rsPGDN:     return pad->NewKeyState.pgdn;
    case rsUP:       return pad->NewKeyState.up;
    case rsDOWN:     return pad->NewKeyState.down;
    case rsLEFT:     return pad->NewKeyState.left;
    case rsRIGHT:    return pad->NewKeyState.right;
    case rsDIVIDE:   return pad->NewKeyState.div;
    case rsTIMES:    return pad->NewKeyState.mul;
    case rsPLUS:     return pad->NewKeyState.add;
    case rsMINUS:    return pad->NewKeyState.sub;
    case rsPADDEL:   return pad->NewKeyState.decimal;
    case rsPADEND:   return pad->NewKeyState.num1;
    case rsPADDOWN:  return pad->NewKeyState.num2;
    case rsPADPGDN:  return pad->NewKeyState.num3;
    case rsPADLEFT:  return pad->NewKeyState.num4;
    case rsPAD5:     return pad->NewKeyState.num5;
    case rsNUMLOCK:  return pad->NewKeyState.numlock;
    case rsPADRIGHT: return pad->NewKeyState.num6;
    case rsPADHOME:  return pad->NewKeyState.num7;
    case rsPADUP:    return pad->NewKeyState.num8;
    case rsPADPGUP:  return pad->NewKeyState.num9;
    case rsPADINS:   return pad->NewKeyState.num0;
    case rsPADENTER: return pad->NewKeyState.enter;
    case rsSCROLL:   return pad->NewKeyState.scroll;
    case rsPAUSE:    return pad->NewKeyState.pause;
    case rsBACKSP:   return pad->NewKeyState.back;
    case rsTAB:      return pad->NewKeyState.tab;
    case rsCAPSLK:   return pad->NewKeyState.capslock;
    case rsENTER:    return pad->NewKeyState.extenter;
    case rsLSHIFT:   return pad->NewKeyState.lshift;
    case rsRSHIFT:   return pad->NewKeyState.rshift;
    case rsSHIFT:    return pad->NewKeyState.shift;
    case rsLCTRL:    return pad->NewKeyState.lctrl;
    case rsRCTRL:    return pad->NewKeyState.rctrl;
    case rsLALT:     return pad->NewKeyState.lmenu;
    case rsRALT:     return pad->NewKeyState.rmenu;
    case rsLWIN:     return pad->NewKeyState.lwin;
    case rsRWIN:     return pad->NewKeyState.rwin;
    case rsAPPS:     return pad->NewKeyState.apps;
    default:         return false;
    }
}

// 0x52E450
bool CControllerConfigManager::GetIsKeyboardKeyJustDown(CControllerKey::KeyCode key) {
    CPad* pad = CPad::GetPad();
    if (key < 255 && pad->NewKeyState.standardKeys[key] && !pad->OldKeyState.standardKeys[key]) {
        return true;
    }

    if (key >= rsF1 && key <= rsF12) {
        const auto index = key - rsF1;
        if (pad->NewKeyState.FKeys[index] && !pad->OldKeyState.FKeys[index]) {
            return true;
        }
    }

    switch (key) {
    // NOTE: If anyone wants to translate all the code into `Pad.h`, please do:
    case rsESC:      return pad->NewKeyState.esc && !pad->OldKeyState.esc;
    case rsINS:      return pad->NewKeyState.insert && !pad->OldKeyState.insert;
    case rsDEL:      return pad->NewKeyState.del && !pad->OldKeyState.del;
    case rsHOME:     return pad->NewKeyState.home && !pad->OldKeyState.home;
    case rsEND:      return pad->NewKeyState.end && !pad->OldKeyState.end;
    case rsPGUP:     return pad->NewKeyState.pgup && !pad->OldKeyState.pgup;
    case rsPGDN:     return pad->NewKeyState.pgdn && !pad->OldKeyState.pgdn;
    case rsUP:       return pad->NewKeyState.up && !pad->OldKeyState.up;
    case rsDOWN:     return pad->NewKeyState.down && !pad->OldKeyState.down;
    case rsLEFT:     return pad->NewKeyState.left && !pad->OldKeyState.left;
    case rsRIGHT:    return pad->NewKeyState.right && !pad->OldKeyState.right;
    case rsDIVIDE:   return pad->NewKeyState.div && !pad->OldKeyState.div;
    case rsTIMES:    return pad->NewKeyState.mul && !pad->OldKeyState.mul;
    case rsPLUS:     return pad->NewKeyState.add && !pad->OldKeyState.add;
    case rsMINUS:    return pad->NewKeyState.sub && !pad->OldKeyState.sub;
    case rsPADDEL:   return pad->NewKeyState.decimal && !pad->OldKeyState.decimal;
    case rsPADEND:   return pad->NewKeyState.num1 && !pad->OldKeyState.num1;
    case rsPADDOWN:  return pad->NewKeyState.num2 && !pad->OldKeyState.num2;
    case rsPADPGDN:  return pad->NewKeyState.num3 && !pad->OldKeyState.num3;
    case rsPADLEFT:  return pad->NewKeyState.num4 && !pad->OldKeyState.num4;
    case rsPAD5:     return pad->NewKeyState.num5 && !pad->OldKeyState.num5;
    case rsNUMLOCK:  return pad->NewKeyState.numlock && !pad->OldKeyState.numlock;
    case rsPADRIGHT: return pad->NewKeyState.num6 && !pad->OldKeyState.num6;
    case rsPADHOME:  return pad->NewKeyState.num7 && !pad->OldKeyState.num7;
    case rsPADUP:    return pad->NewKeyState.num8 && !pad->OldKeyState.num8;
    case rsPADPGUP:  return pad->NewKeyState.num9 && !pad->OldKeyState.num9;
    case rsPADINS:   return pad->NewKeyState.num0 && !pad->OldKeyState.num0;
    case rsPADENTER: return pad->NewKeyState.enter && !pad->OldKeyState.enter;
    case rsSCROLL:   return pad->NewKeyState.scroll && !pad->OldKeyState.scroll;
    case rsPAUSE:    return pad->NewKeyState.pause && !pad->OldKeyState.pause;
    case rsBACKSP:   return pad->NewKeyState.back && !pad->OldKeyState.back;
    case rsTAB:      return pad->NewKeyState.tab && !pad->OldKeyState.tab;
    case rsCAPSLK:   return pad->NewKeyState.capslock && !pad->OldKeyState.capslock;
    case rsENTER:    return pad->NewKeyState.extenter && !pad->OldKeyState.extenter;
    case rsLSHIFT:   return pad->NewKeyState.lshift && !pad->OldKeyState.lshift;
    case rsRSHIFT:   return pad->NewKeyState.rshift && !pad->OldKeyState.rshift;
    case rsSHIFT:    return pad->NewKeyState.shift && !pad->OldKeyState.shift;
    case rsLCTRL:    return pad->NewKeyState.lctrl && !pad->OldKeyState.lctrl;
    case rsRCTRL:    return pad->NewKeyState.rctrl && !pad->OldKeyState.rctrl;
    case rsLALT:     return pad->NewKeyState.lmenu && !pad->OldKeyState.lmenu;
    case rsRALT:     return pad->NewKeyState.rmenu && !pad->OldKeyState.rmenu;
    case rsLWIN:     return pad->NewKeyState.lwin && !pad->OldKeyState.lwin;
    case rsRWIN:     return pad->NewKeyState.rwin && !pad->OldKeyState.rwin;
    case rsAPPS:     return pad->NewKeyState.apps && !pad->OldKeyState.apps;
    default:         return false;
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
    case CONTROLLER_KEYBOARD1:
    case CONTROLLER_KEYBOARD2:
        return key == rsNULL;
    case CONTROLLER_MOUSE:
    case CONTROLLER_PAD:
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
    switch (m_Actions[action].Keys[CONTROLLER_MOUSE].Key) {
    case rsMOUSE_LEFT_BUTTON:       return TheText.Get("FEC_MSL"); // LMB
    case rsMOUSE_MIDDLE_BUTTON:     return TheText.Get("FEC_MSM"); // MMB
    case rsMOUSE_RIGHT_BUTTON:      return TheText.Get("FEC_MSR"); // RMB
    case rsMOUSE_WHEEL_UP_BUTTON:   return TheText.Get("FEC_MWF"); // MS WHEEL UP
    case rsMOUSE_WHEEL_DOWN_BUTTON: return TheText.Get("FEC_MWB"); // MS WHEEL DN
    case rsMOUSE_X1_BUTTON:         return TheText.Get("FEC_MXO"); // MXB1
    case rsMOUSE_X2_BUTTON:         return TheText.Get("FEC_MXT"); // MXB2
    default:                        NOTSA_UNREACHABLE();
    }
}

// 0x52F450
const GxtChar* CControllerConfigManager::GetControllerSettingTextJoystick(eControllerAction action) {
    const auto keyCode = m_Actions[action].Keys[CONTROLLER_PAD].Key;
    if (!keyCode) {
        return nullptr; // 0x52F460 - Invert
    }

    CMessages::InsertNumberInString(TheText.Get("FEC_JBO"), keyCode, -1, -1, -1, -1, -1, NewStringWithNumber); // JOY~1~
    return NewStringWithNumber;
}

// unused
// 0x52F4A0
int32 CControllerConfigManager::sub_52F4A0(eControllerAction action) {
    const auto& actionLocal = m_Actions[action];
    auto assignedSlotsCount = 0u;
    for (const auto& type : CONTROLLER_TYPES_ALL) {
        if (!GetIsKeyBlank(actionLocal.Keys[type].Key, type)) {
            assignedSlotsCount++;
        }
    }
    return assignedSlotsCount;
}

// 0x530500
void CControllerConfigManager::MakeControllerActionsBlank() {
    for (const auto& type : CONTROLLER_TYPES_ALL) {
        for (auto action = 0u; action < NUM_OF_MAX_CONTROLLER_ACTIONS; action++) {
            ClearSettingsAssociatedWithAction((eControllerAction)action, type);
        }
    }
}

// 0x531140
void CControllerConfigManager::AffectPadFromKeyBoard() {
    RsKeyCodes keyCode;
    GTATranslateShiftKey(&keyCode); // No matter what you do, it won't work.
    const auto inMenu = !CPad::padNumber && !FrontEndMenuManager.m_bMenuActive;

    for (const auto& action : m_Actions ) {
        // Handle key press logic
        for (const auto& type : CONTROLLER_TYPES_KEYBOARD) {
            const auto key = action.Keys[type].Key;

            if (GetIsKeyboardKeyDown(key) && inMenu && key != rsNULL) {
                if (CPad* pad = CPad::GetPad()) {
                    auto* state = &pad->PCTempKeyState;
                    if (UseDrivingControls()) {
                        HandleButtonPress(key, type, state);
                        sub_52FD20(key, type, state);
                    } else {
                        UpdateControllerStateOnAction(key, type, state);
                        if (UseFirstPersonControls()) {
                            Handle1stPersonCameraActions(key, type, state);
                        } else {
                            UpdatePadStateOnActions(key, type, state);
                            sub_52FD20(key, type, state);
                        }
                    }
                    UpdatePadStateOnMenuActions(key, type, state);
                }
            }
        }

        // Handle key release logic
        CPad* pad = CPad::GetPad();
        if (!pad || FrontEndMenuManager.m_bMenuActive) {
            continue;
        }

        auto* state = &pad->PCTempKeyState;

        for (const auto& type : CONTROLLER_TYPES_KEYBOARD) {
            const auto key = action.Keys[type].Key;
            if (!GetIsKeyboardKeyDown(key) && key != rsNULL) {
                AffectControllerStateOn_ButtonUp_All_Player_States(key, type, state);
            }
        }
    }
}

// 0x5314A0
void CControllerConfigManager::AffectPadFromMouse() {
    const auto inMenu = !CPad::padNumber && !FrontEndMenuManager.m_bMenuActive;

    for (auto& action : m_Actions) {
        const auto button = action.Keys[CONTROLLER_MOUSE].Key;

        if (GetIsMouseButtonDown(button)) {
            if (CPad* pad = CPad::GetPad()) {
                auto* state = &pad->PCTempMouseState;
                if (inMenu && button) {
                    if (UseDrivingControls()) {
                        HandleButtonPress(button, CONTROLLER_MOUSE, state);
                        sub_52FD20(button, CONTROLLER_MOUSE, state);
                    } else {
                        UpdateControllerStateOnAction(button, CONTROLLER_MOUSE, state);
                        if (UseFirstPersonControls()) {
                            Handle1stPersonCameraActions(button, CONTROLLER_MOUSE, state);
                        } else {
                            UpdatePadStateOnActions(button, CONTROLLER_MOUSE, state);
                            sub_52FD20(button, CONTROLLER_MOUSE, state);
                        }
                    }
                    UpdatePadStateOnMenuActions(button, CONTROLLER_MOUSE, state);
                }
            }
        }

        CPad* pad = CPad::GetPad();
        if (!pad || FrontEndMenuManager.m_bMenuActive) {
            continue;
        }

        auto* state = &pad->PCTempMouseState;

        if (GetIsMouseButtonUp(button) && button) {
            AffectControllerStateOn_ButtonUp_All_Player_States(button, CONTROLLER_MOUSE, state);
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
        DeleteMatchingCommonControls(action, button, type);
        DeleteMatching1rstPersonControls(action, button, type);
        DeleteMatching1rst3rdPersonControls(action, button, type);
        break;
    case ACTION_THIRD_PERSON:
        DeleteMatchingCommonControls(action, button, type);
        DeleteMatching1rst3rdPersonControls(action, button, type);
        UnmapPedKey(action, button, type);
        UnmapVehicleEnterExit(button, type);
        break;
    case ACTION_IN_CAR:
        DeleteMatchingCommonControls(action, button, type);
        DeleteMatchingVehicleControls(action, button, type);
        UnmapVehicleEnterExit(button, type);
        break;
    case ACTION_IN_CAR_THIRD_PERSON:
        DeleteMatchingCommonControls(action, button, type);
        DeleteMatching1rst3rdPersonControls(action, button, type);
        UnmapPedKey(action, button, type);
        DeleteMatchingVehicleControls(action, button, type);
        break;
    case ACTION_COMMON_CONTROLS:
        DeleteMatchingCommonControls(action, button, type);
        DeleteMatching1rstPersonControls(action, button, type);
        DeleteMatching1rst3rdPersonControls(action, button, type);
        UnmapPedKey(action, button, type);
        DeleteMatchingVehicleControls(action, button, type);
        UnmapVehicleEnterExit(button, type);
        break;
    case ACTION_FIRST_THIRD_PERSON:
        DeleteMatchingCommonControls(action, button, type);
        DeleteMatching1rstPersonControls(action, button, type);
        DeleteMatching1rst3rdPersonControls(action, button, type);
        UnmapPedKey(action, button, type);
        UnmapVehicleEnterExit(button, type);
        break;
    default:
        NOTSA_UNREACHABLE();
    }
}

// 0x52FE10
const GxtChar* CControllerConfigManager::GetControllerSettingTextKeyBoard(eControllerAction action, eControllerType type) {
    GxtChar(&s_KeyName)[50] = *(GxtChar(*)[50])0xB714BC; // temp value
    rng::fill(s_KeyName, 0);

    const auto key = m_Actions[action].Keys[type].Key;

    const auto KeyNameWithNumber = [&](const GxtChar* text, int32 key) {
        CMessages::InsertNumberInString(text, key, -1, -1, -1, -1, -1, s_KeyName);
        return s_KeyName;
    };

    if (key == rsNULL) {
        return 0;
    }

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
        default:         NOTSA_UNREACHABLE();
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
    NOTSA_UNREACHABLE();
}

// 0x52F560
const GxtChar* CControllerConfigManager::GetButtonComboText(eControllerAction action) {
    return action == CA_VEHICLE_LOOKBEHIND ? TheText.Get("FEC_LBC") : nullptr; // Use Look Left With Look Right.
}

// 0x5303D0
const GxtChar* CControllerConfigManager::GetDefinedKeyByGxtName(eControllerAction action) {
    // TODO: replace to swith good idea?
    if (FrontEndMenuManager.m_bController) {
        if (const auto keyCode = m_Actions[action].Keys[CONTROLLER_PAD].Key) {
            CMessages::InsertNumberInString(TheText.Get("FEC_JBO"), keyCode, -1, -1, -1, -1, -1, NewStringWithNumber); // JOY~1~
            return NewStringWithNumber;
        }
    }

    if (const auto keyText = GetControllerSettingTextMouse(action)) {
        return keyText;
    }

    if (const auto keyText = GetControllerSettingTextKeyBoard(action, CONTROLLER_KEYBOARD1)) {
        return keyText;
    }

    if (const auto keyText = GetControllerSettingTextKeyBoard(action, CONTROLLER_KEYBOARD2)) {
        return keyText;
    }

    if (m_Actions[action].Keys[CONTROLLER_PAD].Key == (RsKeyCodes)0) {
        return GetControllerSettingTextMouse(action);
    }

    NOTSA_UNREACHABLE();
}

// NOTSA
eControllerAction CControllerConfigManager::GetActionIDByName(std::string_view name) {
    for (auto&& [i, actionName] : notsa::enumerate(m_ControllerActionName)) {
        char actionNameUTF8[128] = {0};
        GxtCharToUTF8(actionNameUTF8, actionName);
        if (std::string_view{ actionNameUTF8 } == name) {
            return (eControllerAction)i;
        }
    }
    return (eControllerAction)-1;
}

// iniline
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
    default:                        NOTSA_UNREACHABLE("Invalid key ({})", (int32)key);
    }
}

// iniline
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
    default:                        NOTSA_UNREACHABLE("Invalid key ({})", (int32)key);
    }
}

// TODO: Reverse CPadConfig
bool CControllerConfigManager::IsCheckSpecificGamepad() {
    return (PadConfigs[0].vendorId == 0x3427 && PadConfigs[0].productId == 0x1190);
}

// iniline
void CControllerConfigManager::CheckAndClear(eControllerAction action, eControllerType type, CControllerKey::KeyCode button) {
    if (m_Actions[action].Keys[type].Key == button) {
        ClearSettingsAssociatedWithAction(action, type);
    }
}

// iniline
void CControllerConfigManager::CheckAndSetButton(eControllerAction action, eControllerType type, CControllerKey::KeyCode button, int16& stateButton) {
    if (m_Actions[action].Keys[type].Key == button) {
        stateButton = 255;
    }
}

// iniline
void CControllerConfigManager::CheckAndSetPad(eControllerAction action, eControllerType type, CControllerKey::KeyCode button, int16& dpad, int16& oppositeDpad) {
    if (m_Actions[action].Keys[type].Key == button) {
        if (dpad) {
            dpad         = 0;
            oppositeDpad = 0;
        } else {
            dpad = 255;
        }
    }
}

// iniline
void CControllerConfigManager::CheckAndSetStick_FirstThirdPerson(eControllerAction action, eControllerType type, CControllerKey::KeyCode button, int16& stickValue, bool& stickFlag, int16 value) {
    if (m_Actions[action].Keys[type].Key == button) {
        if (stickFlag || (stickValue != 0 && stickValue != value)) {
            stickValue = 0;
            stickFlag  = true;
        } else {
            stickValue = value;
        }
    }
}

// iniline
void CControllerConfigManager::CheckAndResetButtonState(eControllerAction action, eControllerType type, CControllerKey::KeyCode button, int16& stateFlag) {
    if (m_Actions[action].Keys[type].Key == button) {
        stateFlag = 0;
    }
}

// iniline
void CControllerConfigManager::CheckAndSetStick_Driving(eControllerAction action, eControllerType type, CControllerKey::KeyCode button, int16& stickValue, bool& stickFlag, int16 value) {
    if (m_Actions[action].Keys[type].Key == button) {
        if (stickFlag) {
            stickValue = 0;
            stickFlag  = true;
        } else {
            stickValue = value;
        }
    }
}

// iniline
bool CControllerConfigManager::UseDrivingControls() {
    // FindPlayerPed() && FindPlayerVehicle() && FindPlayerPed()->GetPedState() == PEDSTATE_DRIVING && !pad->DisablePlayerControls
    if (const auto* const ped = FindPlayerPed()) {
        if (FindPlayerVehicle()) {
            return ped->IsStateDriving() && !CPad::GetPad()->DisablePlayerControls;
        }
    }
    return false;
}

// iniline
bool CControllerConfigManager::UseFirstPersonControls() {
    return notsa::contains({ MODE_1STPERSON, MODE_SNIPER, MODE_ROCKETLAUNCHER, MODE_ROCKETLAUNCHER_HS, MODE_M16_1STPERSON, MODE_CAMERA }, TheCamera.m_aCams[TheCamera.m_nActiveCam].m_nMode);
}

// iniline
CControllerState& CControllerConfigManager::GetControllerState(CPad& pad, eControllerType type) {
    switch (type) {
    case CONTROLLER_KEYBOARD1:
    case CONTROLLER_KEYBOARD2: return pad.PCTempKeyState;
    case CONTROLLER_MOUSE:     return pad.PCTempMouseState;
    case CONTROLLER_PAD:       return pad.PCTempJoyState;
    default:                   NOTSA_UNREACHABLE();
    }
}
