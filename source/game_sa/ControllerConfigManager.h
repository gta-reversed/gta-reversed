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


struct CControllerKey {
    using KeyOrder = uint32;
    using KeyCode = uint32;

    KeyCode  Key{};   //!< `RsKeyCodes` (and ASCII chars) for keyboards (`rsNULL` -> unset)
                      //!< `ePadButton` for mouse/joystick/pad (`0` -> unset)
    KeyOrder Order{}; //!< Order in which this key/button was defined,
                      //!< - If set, [1, CONTROLLER_NUM],
                      //!< - `0` used if this key is not set,
                      //!< - -1 for disabling (?)
};

struct CControllerAction {
    CControllerKey Keys[CONTROLLER_NUM]{};
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
    bool                                                     m_WasJoyJustInitialised{};
    DIJOYSTATE2                                              m_OldJoyState{};
    DIJOYSTATE2                                              m_NewJoyState{};
    std::array<GxtChar[40], NUM_OF_CONTROLLER_ACTIONS>       m_ControllerActionName{};
    bool                                                     m_ButtonStates[17]{};     // True if down, false if up or missing, enum ePadButton?
    std::array<CControllerAction, NUM_OF_CONTROLLER_ACTIONS> m_Actions;
    bool                                                     m_bStickL_X_Rgh_Lft_MovementBothDown[CONTROLLER_NUM];
    bool                                                     m_bStickL_Up_Dwn_MovementBothDown[CONTROLLER_NUM];
    bool                                                     m_bStickR_X_Rgh_Lft_MovementBothDown[CONTROLLER_NUM];
    bool                                                     m_bStickR_Up_Dwn_MovementBothDown[CONTROLLER_NUM];
    bool                                                     m_MouseFoundInitSet;

public:
    static void InjectHooks();

    CControllerConfigManager();

    void ClearPedMappings(eControllerAction action, CControllerKey::KeyCode button, eControllerType controllerType);
    void ClearCommonMappings(eControllerAction nop, CControllerKey::KeyCode button, eControllerType type);
    bool SetControllerKeyAssociatedWithAction(eControllerAction action, CControllerKey::KeyCode button, eControllerType type);
    void ClearVehicleMappings(eControllerAction nop, CControllerKey::KeyCode button, eControllerType type);
    void Clear1st3rdPersonMappings(eControllerAction action, CControllerKey::KeyCode button, eControllerType type);
    void StoreJoyButtonStates();
    const GxtChar* GetActionKeyName(eControllerAction action);
    const GxtChar* GetControllerSettingText(eControllerAction action, CControllerKey::KeyOrder priority);
    void ClearSniperZoomMappings(eControllerAction nop, CControllerKey::KeyCode button, eControllerType type);
    void UnmapVehicleEnterExit(CControllerKey::KeyCode button, eControllerType type);
    eControllerType AffectControllerStateOn_ButtonDown_VehicleAndThirdPersonOnly(CControllerKey::KeyCode button, eControllerType type, CControllerState* state);
    CControllerState* AffectControllerStateOn_ButtonDown_AllStates(CControllerKey::KeyCode button, eControllerType type, CControllerState* state);
    int32 GetMouseButtonAssociatedWithAction(eControllerAction action);
    eControllerType AffectControllerStateOn_ButtonDown_FirstAndThirdPersonOnly(CControllerKey::KeyCode button, eControllerType type, CControllerState* state);
    int32 AffectControllerStateOn_ButtonDown_ThirdPersonOnly(CControllerKey::KeyCode button, eControllerType type, CControllerState* state);
    bool GetIsActionAButtonCombo(eControllerAction action);
    int32 GetControllerKeyAssociatedWithAction(eControllerAction action, eControllerType type);
    int32 AffectControllerStateOn_ButtonDown_FirstPersonOnly(CControllerKey::KeyCode button, eControllerType type, CControllerState* state);
    int32 HandleButtonRelease(CControllerKey::KeyCode button, eControllerType type, CControllerState* state);
    eControllerType AffectControllerStateOn_ButtonDown_Driving(CControllerKey::KeyCode button, eControllerType type, CControllerState* state);
    void ResetSettingOrder(eControllerAction action);
    void HandleJoyButtonUpDown(int32 joyNo, bool isDown);
    bool LoadSettings(FILESTREAM file);
    int32 SaveSettings(FILESTREAM file);
    void InitDefaultControlConfigJoyPad(uint32 buttonCount);
    void InitDefaultControlConfiguration();
    void InitDefaultControlConfigMouse(const CMouseControllerState& state, bool controller);
    void InitialiseControllerActionNameArray();
    bool ReinitControls();
    int8 SetMouseButtonAssociatedWithAction(eControllerAction action, CControllerKey::KeyCode button);
    void StoreMouseButtonState(eMouseButtons button, bool state);
    void UpdateJoyInConfigMenus_ButtonDown(CControllerKey::KeyCode button, int32 padNumber);
    void UpdateJoy_ButtonDown(CControllerKey::KeyCode button, eControllerType type);
    void AffectControllerStateOn_ButtonDown_DebugStuff(int32, eControllerType);
    void UpdateJoyInConfigMenus_ButtonUp(CControllerKey::KeyCode button, int32 padNumber);
    void UpdateJoy_ButtonUp(CControllerKey::KeyCode button, eControllerType type);
    void AffectControllerStateOn_ButtonUp_DebugStuff(int32, eControllerType);
    void ClearSimButtonPressCheckers();
    int32 GetJoyButtonJustUp();
    int32 GetJoyButtonJustDown();
    bool GetIsKeyboardKeyDown(CControllerKey::KeyCode button);
    bool GetIsKeyboardKeyJustDown(CControllerKey::KeyCode button);
    bool GetIsMouseButtonDown(CControllerKey::KeyCode button);
    bool GetIsMouseButtonUp(CControllerKey::KeyCode button);
    bool GetIsMouseButtonJustUp(CControllerKey::KeyCode button);
    bool GetIsKeyBlank(CControllerKey::KeyCode button, eControllerType controller);
    eActionType GetActionType(eControllerAction action);
    void ClearSettingsAssociatedWithAction(eControllerAction action, eControllerType type);
    const GxtChar* GetControllerSettingTextMouse(eControllerAction action);
    const GxtChar* GetControllerSettingTextJoystick(eControllerAction action);
    void MakeControllerActionsBlank();
    void AffectPadFromKeyBoard();
    void AffectPadFromMouse();
    void DeleteMatchingActionInitiators(eControllerAction action, CControllerKey::KeyCode button, eControllerType controllerType);
    const GxtChar* GetKeyNameForKeyboard(eControllerAction action, eControllerType type);
    const GxtChar* GetButtonComboText(eControllerAction event);

    const GxtChar* GetDefinedKeyByGxtName(eControllerAction action);

    // NOTSA
    eControllerAction GetActionIDByName(std::string_view name);

private:
    bool CheckMouseButtonState(CControllerKey::KeyCode button);
    bool CheckMouseButtonJustUpState(CControllerKey::KeyCode button);
    bool IsCheckSpecificGamepad();
    void CheckAndClear(eControllerAction action, eControllerType type, CControllerKey::KeyCode button);
    void CheckAndSetButton(eControllerAction action, eControllerType type, CControllerKey::KeyCode button, int16& state);
    void CheckAndSetPad(eControllerAction action, eControllerType type, CControllerKey::KeyCode button, int16& dpad, int16& oppositeDpad);
    static bool UseDrivingControls();
    static bool UseFirstPersonControls();
    static CControllerState& GetControllerState(CPad& pad, eControllerType ctrl);

private:
    CControllerConfigManager* Constructor() {
        this->CControllerConfigManager::CControllerConfigManager();
        return this;
    }
};
VALIDATE_SIZE(CControllerConfigManager, 0x12E4);

extern CControllerConfigManager &ControlsManager;
