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
    MOUSE_BUTTON_NONE = 0,
    MOUSE_BUTTON_LEFT,
    MOUSE_BUTTON_MIDDLE,
    MOUSE_BUTTON_RIGHT,
    MOUSE_BUTTON_WHEEL_UP,
    MOUSE_BUTTON_WHEEL_DOWN,
    MOUSE_BUTTON_WHEEL_XBUTTON1,
    MOUSE_BUTTON_WHEEL_XBUTTON2
};

enum eActionType {
    ACTION_0,
    ACTION_1,
    ACTION_2,
    ACTION_3,
    ACTION_4,
    ACTION_5,
    ACTION_6,
};

enum eControllerAction {
    CA_PED_FIREWEAPON,
    CA_PED_FIREWEAPON_ALT,
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
    CA_SNEAK_ABOUT,
    CA_VEHICLE_FIREWEAPON,
    CA_VEHICLE_FIREWEAPON_ALT,
    CA_VEHICLE_STEERLEFT,
    CA_VEHICLE_STEERRIGHT,
    CA_VEHICLE_STEERUP,
    CA_VEHICLE_STEERDOWN,
    CA_VEHICLE_ACCELERATE,
    CA_VEHICLE_BRAKE,
    CA_VEHICLE_RADIO_STATION_UP,
    CA_VEHICLE_RADIO_STATION_DOWN,
    CA_UNKNOWN_28,
    CA_VEHICLE_HORN = 29,
    CA_TOGGLE_SUBMISSIONS,
    CA_VEHICLE_HANDBRAKE,
    CA_PED_1RST_PERSON_LOOK_LEFT,
    CA_PED_1RST_PERSON_LOOK_RIGHT,
    CA_VEHICLE_LOOKLEFT,
    CA_VEHICLE_LOOKRIGHT,
    CA_VEHICLE_LOOKBEHIND,
    CA_VEHICLE_MOUSELOOK,
    CA_VEHICLE_TURRETLEFT,
    CA_VEHICLE_TURRETRIGHT,
    CA_VEHICLE_TURRETUP,
    CA_VEHICLE_TURRETDOWN,
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
    CA_UNKNOWN_53,
    CA_TOGGLE_DPAD = 54,
    CA_SWITCH_DEBUG_CAM_ON,
    CA_TAKE_SCREEN_SHOT,
    CA_SHOW_MOUSE_POINTER_TOGGLE
};

struct CControllerKey {
    uint32 KeyCode;
    uint32 Priority;
};

struct CControllerAction {
    CControllerKey Keys[4];
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
VALIDATE_SIZE(CPadConfig, 16);
static inline auto& PadConfigs = StaticRef<std::array<CPadConfig, 2>, 0xC92144>();

using ControlName = char[40];

class CControllerConfigManager {
public:
    bool              m_bJoyJustInitialised;

    DIJOYSTATE2       m_OldJoyState;
    DIJOYSTATE2       m_NewJoyState;

    char              m_arrControllerActionName[59][40]; // todo: 182
    bool              m_ButtonStates[17];   // True if down, false if up or missing
    CControllerAction m_Actions[59];

    bool m_bStickL_X_Rgh_Lft_MovementBothDown[4];
    bool m_bStickL_Up_Dwn_MovementBothDown[4];
    bool m_bStickR_X_Rgh_Lft_MovementBothDown[4];
    bool m_bStickR_Up_Dwn_MovementBothDown[4];

    bool MouseFoundInitSet;

public:
    static void InjectHooks();

    CControllerConfigManager();
    CControllerConfigManager* Constructor();


    bool LoadSettings(FILESTREAM file);
    void SaveSettings(FILESTREAM file);

    void InitDefaultControlConfiguration();
    void InitDefaultControlConfigMouse(const CMouseControllerState& state, bool controller);
    void InitialiseControllerActionNameArray();
    void ReinitControls();

    void SetMouseButtonAssociatedWithAction(eControllerAction action, RsKeyCodes button);

    void StoreMouseButtonState(eMouseButtons button, bool state);
    void UpdateJoyInConfigMenus_ButtonDown(ePadButton button, int32 padNumber);
    void UpdateJoy_ButtonDown(ePadButton button, int32 unk);
    void AffectControllerStateOn_ButtonDown_DebugStuff(int32, eControllerType);
    void UpdateJoyInConfigMenus_ButtonUp(ePadButton button, int32 padNumber);
    void UpdateJoy_ButtonUp(ePadButton button, int32 unk);
    void AffectControllerStateOn_ButtonUp_DebugStuff(int32, eControllerType);
    void ClearSimButtonPressCheckers();

    bool GetJoyButtonJustUp();
    bool GetJoyButtonJustDown();
    bool GetIsKeyboardKeyDown(RsKeyCodes key);
    bool GetIsKeyboardKeyJustDown(RsKeyCodes key);
    bool GetIsMouseButtonDown(RsKeyCodes key);
    bool GetIsMouseButtonUp(RsKeyCodes key);
    bool GetIsMouseButtonJustUp(RsKeyCodes key);
    bool GetIsKeyBlank(int32 a1, eControllerType controller);
    eActionType GetActionType(eControllerAction action);
    char* GetControllerSettingTextMouse(eControllerAction action);
    char* GetControllerSettingTextJoystick(eControllerAction action);
    void StoreJoyButtonStates();
    void HandleJoyButtonUpDown(int32 joyNo, bool isDown); // NOTSA

    void ClearSettingsAssociatedWithAction(eControllerAction action, eControllerType type);
    void MakeControllerActionsBlank();
    void AffectPadFromKeyBoard();
    void AffectPadFromMouse();
    void DeleteMatchingActionInitiators(eControllerAction Action, int32 KeyToBeChecked, eControllerType ControllerTypeToBeChecked);

    void GetDefinedKeyByGxtName(uint16 actionId, char* buf, uint16 bufsz);

    // NOTSA
    uint16 GetActionIDByName(std::string_view name);
};
VALIDATE_SIZE(CControllerConfigManager, 0x12E4);

extern CControllerConfigManager &ControlsManager;
