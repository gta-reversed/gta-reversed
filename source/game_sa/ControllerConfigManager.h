/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "RenderWare.h"

#include "ePadButton.h"

enum eMouseButtons {
    MOUSE_BUTTON_NONE,
    MOUSE_BUTTON_LEFT,
    MOUSE_BUTTON_MIDDLE,
    MOUSE_BUTTON_RIGHT,
    MOUSE_BUTTON_WHEEL_UP,
    MOUSE_BUTTON_WHEEL_DOWN,
    MOUSE_BUTTON_WHEEL_XBUTTON1,
    MOUSE_BUTTON_WHEEL_XBUTTON2,
    MOUSE_BUTTON_NUM
};

enum eActionType {
    ACTION_FIRST_PERSON,
    ACTION_THIRD_PERSON,
    ACTION_IN_CAR,
    ACTION_IN_CAR_THIRD_PERSON,
    ACTION_COMMON_CONTROLS,
    ACTION_FIRST_THIRD_PERSON,
    ACTION_NOT_TYPE
};

enum eControllerAction {
    CA_UNLOAD = -2, // maybe?
    CA_INVALID = -1,
    CA_PED_FIRE_WEAPON,
    CA_PED_FIRE_WEAPON_ALT,
    CA_PED_CYCLE_WEAPON_RIGHT,
    CA_PED_CYCLE_WEAPON_LEFT,
    CA_GO_FORWARD,
    CA_GO_BACK,
    CA_GO_LEFT,
    CA_GO_RIGHT,
    CA_PED_SNIPER_ZOOM_IN,
    CA_PED_SNIPER_ZOOM_OUT,
    CA_VEHICLE_ENTER_EXIT,
    CA_CAMERA_CHANGE_VIEW_ALL_SITUATIONS,
    CA_PED_JUMPING,
    CA_PED_SPRINT,
    CA_PED_LOOKBEHIND,
    CA_PED_DUCK,
    CA_PED_ANSWER_PHONE,
    CA_PED_WALK,
    CA_VEHICLE_FIRE_WEAPON,
    CA_VEHICLE_FIRE_WEAPON_ALT,
    CA_VEHICLE_STEER_LEFT,
    CA_VEHICLE_STEER_RIGHT,
    CA_VEHICLE_STEER_UP,
    CA_VEHICLE_STEER_DOWN,
    CA_VEHICLE_ACCELERATE,
    CA_VEHICLE_BRAKE,
    CA_VEHICLE_RADIO_STATION_UP,
    CA_VEHICLE_RADIO_STATION_DOWN,
    CA_VEHICLE_RADIO_TRACK_SKIP,
    CA_VEHICLE_HORN,
    CA_TOGGLE_SUBMISSIONS,
    CA_VEHICLE_HANDBRAKE,
    CA_PED_1RST_PERSON_LOOK_LEFT,
    CA_PED_1RST_PERSON_LOOK_RIGHT,
    CA_VEHICLE_LOOK_LEFT,
    CA_VEHICLE_LOOK_RIGHT,
    CA_VEHICLE_LOOKBEHIND,
    CA_VEHICLE_MOUSE_LOOK,
    CA_VEHICLE_TURRET_LEFT,
    CA_VEHICLE_TURRET_RIGHT,
    CA_VEHICLE_TURRET_UP,
    CA_VEHICLE_TURRET_DOWN,
    CA_PED_CYCLE_TARGET_LEFT,
    CA_PED_CYCLE_TARGET_RIGHT,
    CA_PED_CENTER_CAMERA_BEHIND_PLAYER,
    CA_PED_LOCK_TARGET,
    CA_NETWORK_TALK,
    CA_CONVERSATION_YES,
    CA_CONVERSATION_NO,
    CA_GROUP_CONTROL_FWD,
    CA_GROUP_CONTROL_BWD,
    CA_PED_1RST_PERSON_LOOK_UP,
    CA_PED_1RST_PERSON_LOOK_DOWN,
    CA_NUM_OF_1ST_PERSON_ACTIONS,
    CA_TOGGLE_DPAD,
    CA_SWITCH_DEBUG_CAM_ON,
    CA_TAKE_SCREEN_SHOT,
    CA_SHOW_MOUSE_POINTER_TOGGLE,
    CA_SWITCH_CAM_DEBUG_MENU,
    NUM_OF_CONTROLLER_ACTIONS
};

// maybe, unitest
enum ePriority {
    PRIORITY_NONE = -1,
    PRIORITY_DISABLE,
    PRIORITY_HIGH,
    PRIORITY_MEDIUM,
    PRIORITY_LOW,
    PRIORITY_NUM
};

struct CControllerKey {
    uint32 KeyCode; // TODO: replace to ePadButton or RsKeyCodes?
    ePriority Priority;
};

struct CControllerAction {
    CControllerKey Keys[CONTROLLER_NUM];
};

struct CPadConfig {
    int32 field_0{};
    bool present{};         // Device exists
    bool zAxisPresent{};    // Has property DIJOFS_Z
    bool rzAxisPresent{};   // Has property DIJOFS_RZ
private:
    char __align{};
public:
    int32 vendorId{};
    int32 productId{};
};
VALIDATE_SIZE(CPadConfig, 0x10);
static inline auto& PadConfigs = StaticRef<std::array<CPadConfig, MAX_PADS>, 0xC92144>();

using ControlName = char[40];

class CControllerConfigManager {
public:
    bool              m_bJoyJustInitialised;

    DIJOYSTATE2       m_OldJoyState;
    DIJOYSTATE2       m_NewJoyState;

    char              m_arrControllerActionName[NUM_OF_CONTROLLER_ACTIONS][40]; // todo: 182 and GxtChar
    bool              m_ButtonStates[17];   // True if down, false if up or missing, enum ePadButton?
    CControllerAction m_Actions[NUM_OF_CONTROLLER_ACTIONS];

    bool m_bStickL_X_Rgh_Lft_MovementBothDown[CONTROLLER_NUM];
    bool m_bStickL_Up_Dwn_MovementBothDown[CONTROLLER_NUM];
    bool m_bStickR_X_Rgh_Lft_MovementBothDown[CONTROLLER_NUM];
    bool m_bStickR_Up_Dwn_MovementBothDown[CONTROLLER_NUM];

    bool MouseFoundInitSet;

public:
    static void InjectHooks();

    CControllerConfigManager();
    CControllerConfigManager* Constructor();

    void ClearPedMappings(eControllerAction action, int32 button, eControllerType controllerType);
    void ClearCommonMappings(eControllerAction nop, int32 button, eControllerType type);
    bool SetKeyForAction(eControllerAction action, int32 button, eControllerType type);
    void ClearVehicleMappings(eControllerAction nop, int32 button, eControllerType type);
    void Clear1st3rdPersonMappings(eControllerAction action, int32 button, eControllerType type);
    void StoreJoyButtonStates();
    const GxtChar* GetActionKeyName(eControllerAction action);
    const GxtChar* GetControllerSettingText(eControllerAction action, ePriority priority);
    void ClearSniperZoomMappings(eControllerAction nop, int32 button, eControllerType type);
    void UnmapVehicleEnterExit(int32 button, eControllerType type);
    eControllerType SetVehicleEnterExitButton(int32 button, eControllerType type, CControllerState* state);
    CControllerState* UpdatePadStateOnMenuActions(int32 button, eControllerType type, CControllerState* state);
    int32 GetMouseButtonAssociatedWithAction(eControllerAction action);
    eControllerType UpdateControllerStateOnAction(int32 button, eControllerType type, CControllerState* state);
    int32 UpdatePadStateOnActions(int32 button, eControllerType type, CControllerState* state);
    bool GetIsActionAButtonCombo(eControllerAction action);
    int32 GetControllerKeyAssociatedWithAction(eControllerAction action, eControllerType type);
    int32 Handle1stPersonCameraActions(int32 button, eControllerType type, CControllerState* state);
    int32 HandleButtonRelease(int32 button, eControllerType type, CControllerState* state);
    eControllerType HandleButtonPress(int32 button, eControllerType type, CControllerState* state);
    eControllerType ResetSettingOrder(eControllerAction action);
    void HandleJoyButtonUpDown(int32 joyNo, bool isDown);
    bool LoadSettings(FILESTREAM file);
    int32 SaveSettings(FILESTREAM file);
    void InitDefaultControlConfiguration(uint32 buttonCount);
    void InitializeDefaultKeyboardAndMouseBindings();
    bool InitDefaultControlConfigMouse(const CMouseControllerState& state, bool controller);
    void InitialiseControllerActionNameArray();
    bool ReinitControls();
    int8 SetMouseButtonAssociatedWithAction(eControllerAction action, int32 button);
    void StoreMouseButtonState(eMouseButtons button, bool state);
    void UpdateJoyInConfigMenus_ButtonDown(int32 button, int32 padNumber);
    void UpdateJoy_ButtonDown(int32 button, eControllerType type);
    void AffectControllerStateOn_ButtonDown_DebugStuff(int32, eControllerType);
    void UpdateJoyInConfigMenus_ButtonUp(int32 button, int32 padNumber);
    void UpdateJoy_ButtonUp(int32 button, eControllerType type);
    void AffectControllerStateOn_ButtonUp_DebugStuff(int32, eControllerType);
    void ClearSimButtonPressCheckers();
    int32 GetJoyButtonJustUp();
    int32 GetJoyButtonJustDown();
    bool GetIsKeyboardKeyDown(int32 key);
    bool GetIsKeyboardKeyJustDown(int32 key);
    bool GetIsMouseButtonDown(int32 key);
    bool GetIsMouseButtonUp(int32 key);
    bool GetIsMouseButtonJustUp(int32 key);
    bool GetIsKeyBlank(int32 key, eControllerType controller);
    eActionType GetActionType(eControllerAction action);
    eControllerType ClearSettingsAssociatedWithAction(eControllerAction action, eControllerType type);
    const GxtChar* GetControllerSettingTextMouse(eControllerAction action);
    const GxtChar* GetControllerSettingTextJoystick(eControllerAction action);
    eControllerType MakeControllerActionsBlank();
    void ProcessKeyboardInput();
    void ProcessMouseInput();
    void DeleteMapping(eControllerAction action, int32 button, eControllerType controllerType);
    const GxtChar* GetKeyNameForKeyboard(eControllerAction action, eControllerType type);
    const GxtChar* GetButtonComboText(eControllerAction event);

    const GxtChar* GetDefinedKeyByGxtName(eControllerAction action);

    // NOTSA
    eControllerAction GetActionIDByName(std::string_view name);
    bool CheckMouseButtonState(int32 key);
    bool CheckMouseButtonJustUpState(int32 key);
    bool IsCheckSpecificGamepad();
    void CheckAndClear(eControllerAction action, eControllerType type, int32 button);
    void CheckAndSetButton(eControllerAction action, eControllerType type, int32 button, int16& state);
    void CheckAndSetPad(eControllerAction action, eControllerType type, int32 button, int16& dpad, int16& oppositeDpad);
};
VALIDATE_SIZE(CControllerConfigManager, 0x12E4);

extern CControllerConfigManager &ControlsManager;
