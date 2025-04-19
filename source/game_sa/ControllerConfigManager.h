/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "RenderWare.h"
#include "eControllerType.h"

enum eMouseButtons {
    MOUSE_BUTTON_NONE,
    MOUSE_BUTTON_LEFT,
    MOUSE_BUTTON_MIDDLE,
    MOUSE_BUTTON_RIGHT,
    MOUSE_BUTTON_WHEEL_UP,
    MOUSE_BUTTON_WHEEL_DOWN,
    MOUSE_BUTTON_WHEEL_XBUTTON1,
    MOUSE_BUTTON_WHEEL_XBUTTON2,
};

constexpr int MOUSE_BUTTONS = int(MOUSE_BUTTON_WHEEL_XBUTTON2 + 1);
constexpr int MOUSE_BUTTON_COUNT = int(MOUSE_BUTTONS + 1); // Please don't add more values to this enum, it is used in the array size of m_ButtonStates

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
    CA_COMBOLOCK                         = -2,
    CA_NONE    = -1,
    CA_PED_FIRE_WEAPON                   = 0,
    CA_PED_FIRE_WEAPON_ALT               = 1,
    CA_PED_CYCLE_WEAPON_RIGHT            = 2,
    CA_PED_CYCLE_WEAPON_LEFT             = 3,
    CA_GO_FORWARD                        = 4,
    CA_GO_BACK                           = 5,
    CA_GO_LEFT                           = 6,
    CA_GO_RIGHT                          = 7,
    CA_PED_SNIPER_ZOOM_IN                = 8,
    CA_PED_SNIPER_ZOOM_OUT               = 9,
    CA_VEHICLE_ENTER_EXIT                = 10,
    CA_CAMERA_CHANGE_VIEW_ALL_SITUATIONS = 11,
    CA_PED_JUMPING                       = 12,
    CA_PED_SPRINT                        = 13,
    CA_PED_LOOKBEHIND                    = 14,
    CA_PED_DUCK                          = 15,
    CA_PED_ANSWER_PHONE                  = 16,
    CA_PED_WALK                          = 17,
    CA_VEHICLE_FIRE_WEAPON               = 18,
    CA_VEHICLE_FIRE_WEAPON_ALT           = 19,
    CA_VEHICLE_STEER_LEFT                = 20,
    CA_VEHICLE_STEER_RIGHT               = 21,
    CA_VEHICLE_STEER_UP                  = 22,
    CA_VEHICLE_STEER_DOWN                = 23,
    CA_VEHICLE_ACCELERATE                = 24,
    CA_VEHICLE_BRAKE                     = 25,
    CA_VEHICLE_RADIO_STATION_UP          = 26,
    CA_VEHICLE_RADIO_STATION_DOWN        = 27,
    CA_VEHICLE_RADIO_TRACK_SKIP          = 28,
    CA_VEHICLE_HORN                      = 29,
    CA_TOGGLE_SUBMISSIONS                = 30,
    CA_VEHICLE_HANDBRAKE                 = 31,
    CA_PED_1RST_PERSON_LOOK_LEFT         = 32,
    CA_PED_1RST_PERSON_LOOK_RIGHT        = 33,
    CA_VEHICLE_LOOKLEFT                  = 34,
    CA_VEHICLE_LOOKRIGHT                 = 35,
    CA_VEHICLE_LOOKBEHIND                = 36,
    CA_VEHICLE_MOUSELOOK                 = 37,
    CA_VEHICLE_TURRETLEFT                = 38,
    CA_VEHICLE_TURRETRIGHT               = 39,
    CA_VEHICLE_TURRETUP                  = 40,
    CA_VEHICLE_TURRETDOWN                = 41,
    CA_PED_CYCLE_TARGET_LEFT             = 42,
    CA_PED_CYCLE_TARGET_RIGHT            = 43,
    CA_PED_CENTER_CAMERA_BEHIND_PLAYER   = 44,
    CA_PED_LOCK_TARGET                   = 45,
    CA_NETWORK_TALK                      = 46,
    CA_CONVERSATION_YES                  = 47,
    CA_CONVERSATION_NO                   = 48,
    CA_GROUP_CONTROL_FWD                 = 49,
    CA_GROUP_CONTROL_BWD                 = 50,
    CA_PED_1RST_PERSON_LOOK_UP           = 51,
    CA_PED_1RST_PERSON_LOOK_DOWN         = 52,
    CA_NUM_OF_1ST_PERSON_ACTIONS         = 53,
    CA_TOGGLE_DPAD                       = 54,
    CA_SWITCH_DEBUG_CAM_ON               = 55,
    CA_TAKE_SCREEN_SHOT                  = 56,
    CA_SHOW_MOUSE_POINTER_TOGGLE         = 57,
    CA_SWITCH_CAM_DEBUG_MENU             = 58,
};

// notsa
constexpr int NUM_OF_MIN_CONTROLLER_ACTIONS = 0;
constexpr int NUM_OF_MAX_CONTROLLER_ACTIONS = CA_SWITCH_CAM_DEBUG_MENU + 1;

enum eContSetOrder {
    NO_ORDER_SET = 0,
    FIRST        = 1,
    SECOND       = 2,
    THIRD        = 3,
    FOURTH       = 4,
};

constexpr std::array<eContSetOrder, 5> CONTROLLER_ORDERS_SET = {
    eContSetOrder(NO_ORDER_SET + 1),
    eContSetOrder(FIRST + 1),
    eContSetOrder(SECOND + 1),
    eContSetOrder(THIRD + 1),
    eContSetOrder(FOURTH + 1)
}; // NOTSA

enum eJoyButtons : uint32 {
    NO_JOYBUTTONS       = 0,
    JOYBUTTON_ONE       = 1,
    JOYBUTTON_TWO       = 2,
    JOYBUTTON_THREE     = 3,
    JOYBUTTON_FOUR      = 4,
    JOYBUTTON_FIVE      = 5,
    JOYBUTTON_SIX       = 6,
    JOYBUTTON_SEVEN     = 7,
    JOYBUTTON_EIGHT     = 8,
    JOYBUTTON_NINE      = 9,
    JOYBUTTON_TEN       = 10,
    JOYBUTTON_ELEVEN    = 11,
    JOYBUTTON_TWELVE    = 12,
    JOYBUTTON_THIRTEEN  = 13,
    JOYBUTTON_FOURTEEN  = 14,
    JOYBUTTON_FIFTHTEEN = 15,
    JOYBUTTON_SIXTEEN   = 16,
};

constexpr int JOY_BUTTONS = int(JOYBUTTON_SIXTEEN);
constexpr int JOYBUTTON_COUNT = int(JOY_BUTTONS + 1 ); // Please don't add more values to this enum, it is used in the array size of m_ButtonStates

using KeyCode = uint32; // NOTSA: Originally that is RW type, but we use uint32 for consistency

struct CControllerKey {
    KeyCode       m_uiActionInitiator{};
    eContSetOrder m_uiSetOrder{};
};

VALIDATE_SIZE(CControllerKey, 0x8);

struct CControllerAction {
    CControllerKey Keys[eControllerType::CONTROLLER_NUM]{};
};

VALIDATE_SIZE(CControllerAction, 0x20);

struct JoyStruct {
    uint32 wDeviceID{};
    bool   bJoyAttachedToPort{};
    bool   bZAxisPresent{};
    bool   bZRotPresent{};
private:
    char __align{};
public:
    DWORD wVendorID{};
    DWORD wProductID{};
};

VALIDATE_SIZE(JoyStruct, 0x10);

struct CJoySticks {
    JoyStruct JoyStickNum[MAX_PADS]{};
};

VALIDATE_SIZE(CJoySticks, 0x20);
static inline auto& AllValidWinJoys = StaticRef<CJoySticks, 0xC92144>();

using ControlName = GxtChar[40];

class CControllerConfigManager {
public:
    bool              m_bJoyJustInitialised{};
    DIJOYSTATE2       m_OldJoyState{};
    DIJOYSTATE2       m_NewJoyState{};
    ControlName       m_ControllerActionName[NUM_OF_MAX_CONTROLLER_ACTIONS]{};
    bool              m_ButtonStates[JOYBUTTON_COUNT]{}; // True if down, false if up or missing
    CControllerAction m_Actions[NUM_OF_MAX_CONTROLLER_ACTIONS];
    bool              m_bStickL_X_Rgh_Lft_MovementBothDown[eControllerType::CONTROLLER_NUM];
    bool              m_bStickL_Up_Dwn_MovementBothDown[eControllerType::CONTROLLER_NUM];
    bool              m_bStickR_X_Rgh_Lft_MovementBothDown[eControllerType::CONTROLLER_NUM];
    bool              m_bStickR_Up_Dwn_MovementBothDown[eControllerType::CONTROLLER_NUM];
    bool              m_MouseFoundInitSet;
public:
    static void InjectHooks();

    CControllerConfigManager();

    void ClearPedMappings(eControllerAction action, KeyCode button, eControllerType type);
    void ClearCommonMappings(eControllerAction nop, KeyCode button, eControllerType type);
    void SetControllerKeyAssociatedWithAction(eControllerAction action, KeyCode button, eControllerType type);
    void ClearVehicleMappings(eControllerAction nop, KeyCode button, eControllerType type);
    void Clear1st3rdPersonMappings(eControllerAction action, KeyCode button, eControllerType type);
    void UpdateJoyButtonState(int32 PadID);
    const GxtChar* GetActionKeyName(eControllerAction action);
    const GxtChar* GetControllerSettingText(eControllerAction action, eContSetOrder order);
    void ClearSniperZoomMappings(eControllerAction nop, KeyCode button, eControllerType type);
    void UnmapVehicleEnterExit(KeyCode button, eControllerType type);
    void AffectControllerStateOn_ButtonDown_VehicleAndThirdPersonOnly(KeyCode button, eControllerType type, CControllerState* state);
    void AffectControllerStateOn_ButtonDown_AllStates(KeyCode button, eControllerType type, CControllerState* state);
    KeyCode GetMouseButtonAssociatedWithAction(eControllerAction action);
    void AffectControllerStateOn_ButtonDown_FirstAndThirdPersonOnly(KeyCode button, eControllerType type, CControllerState* state);
    void AffectControllerStateOn_ButtonDown_ThirdPersonOnly(KeyCode button, eControllerType type, CControllerState* state);
    bool GetIsActionAButtonCombo(eControllerAction action);
    KeyCode GetControllerKeyAssociatedWithAction(eControllerAction action, eControllerType type);
    void AffectControllerStateOn_ButtonDown_FirstPersonOnly(KeyCode button, eControllerType type, CControllerState* state);
    void HandleButtonRelease(KeyCode button, eControllerType type, CControllerState* state);
    void AffectControllerStateOn_ButtonDown_Driving(KeyCode button, eControllerType type, CControllerState* state);
    void ResetSettingOrder(eControllerAction action);
    // void HandleJoyButtonUpDown(int32 joyNo, bool isDown);
    bool LoadSettings(FILESTREAM file);
    bool SaveSettings(FILESTREAM file);
    void InitDefaultControlConfigJoyPad(uint32 buttonCount);
    void InitDefaultControlConfiguration();
    void InitDefaultControlConfigMouse(const CMouseControllerState& state, bool controller);
    void InitialiseControllerActionNameArray();
    void ReinitControls();
    void SetMouseButtonAssociatedWithAction(eControllerAction action, KeyCode button);
    void StoreMouseButtonState(eMouseButtons button, bool state);
    void UpdateJoyInConfigMenus_ButtonDown(KeyCode button, int32 padNumber);
    void UpdateJoy_ButtonDown(KeyCode button, eControllerType type);
    void AffectControllerStateOn_ButtonDown_DebugStuff(int32, eControllerType);
    void UpdateJoyInConfigMenus_ButtonUp(KeyCode button, int32 padNumber);
    void AffectControllerStateOn_ButtonUp(KeyCode button, eControllerType type);
    void AffectControllerStateOn_ButtonUp_DebugStuff(int32, eControllerType);
    void ClearSimButtonPressCheckers();
    eJoyButtons GetJoyButtonJustUp();
    eJoyButtons GetJoyButtonJustDown();
    bool GetIsKeyboardKeyDown(KeyCode button);
    bool GetIsKeyboardKeyJustDown(KeyCode button);
    bool GetIsMouseButtonDown(KeyCode button);
    bool GetIsMouseButtonUp(KeyCode button);
    bool GetIsMouseButtonJustUp(KeyCode button);
    bool GetIsKeyBlank(KeyCode button, eControllerType type);
    eActionType GetActionType(eControllerAction action);
    void ClearSettingsAssociatedWithAction(eControllerAction action, eControllerType type);
    const GxtChar* GetControllerSettingTextMouse(eControllerAction action);
    const GxtChar* GetControllerSettingTextJoystick(eControllerAction action);
    eContSetOrder GetNumOfSettingsForAction(eControllerAction action);
    void MakeControllerActionsBlank();
    void AffectPadFromKeyBoard();
    void AffectPadFromMouse();
    void DeleteMatchingActionInitiators(eControllerAction action, KeyCode button, eControllerType type);
    const GxtChar* GetKeyNameForKeyboard(eControllerAction action, eControllerType type);
    const GxtChar* GetButtonComboText(eControllerAction event);
    const GxtChar* GetDefinedKeyByGxtName(eControllerAction action);
    void AffectControllerStateOn_ButtonDown(KeyCode button, eControllerType type);

    // NOTSA
    eControllerAction GetActionIDByName(std::string_view name);
private:
    // inline region
    void CheckAndClear(eControllerAction action, eControllerType type, KeyCode button);
    void CheckAndSetButton(eControllerAction action, eControllerType type, KeyCode button, int16& state);
    void CheckAndReset(eControllerAction action, eControllerType type, KeyCode button, int16& state);
    void CheckAndSetPad(eControllerAction action, eControllerType type, KeyCode button, int16& dpad, int16& oppositeDpad);
    void CheckAndSetStick(eControllerAction action, eControllerType type, KeyCode button, int16& state, bool& movementBothDown, int16 value);
    int16& GetControllerStateJoyStick(CPad& pad, KeyCode button);
    bool UseDrivingControls();
    bool UseFirstPersonControls();
    bool IsKeyboardKeyDownInState(CKeyboardState& state, KeyCode key);
    CControllerState& GetControllerState(CPad& pad, eControllerType type);
private:
    CControllerConfigManager* Constructor() {
        this->CControllerConfigManager::CControllerConfigManager();
        return this;
    }
};

VALIDATE_SIZE(CControllerConfigManager, 0x12E4);

extern CControllerConfigManager& ControlsManager;
