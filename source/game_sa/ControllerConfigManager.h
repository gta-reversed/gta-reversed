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
    COMBOLOCK                         = -2,
    NUM_OF_NONE_CONTROLLER_ACTIONS    = -1,
    PED_FIRE_WEAPON                   = 0,
    PED_FIRE_WEAPON_ALT               = 1,
    PED_CYCLE_WEAPON_RIGHT            = 2,
    PED_CYCLE_WEAPON_LEFT             = 3,
    GO_FORWARD                        = 4,
    GO_BACK                           = 5,
    GO_LEFT                           = 6,
    GO_RIGHT                          = 7,
    PED_SNIPER_ZOOM_IN                = 8,
    PED_SNIPER_ZOOM_OUT               = 9,
    VEHICLE_ENTER_EXIT                = 10,
    CAMERA_CHANGE_VIEW_ALL_SITUATIONS = 11,
    PED_JUMPING                       = 12,
    PED_SPRINT                        = 13,
    PED_LOOKBEHIND                    = 14,
    PED_DUCK                          = 15,
    PED_ANSWER_PHONE                  = 16,
    PED_WALK                          = 17,
    VEHICLE_FIRE_WEAPON               = 18,
    VEHICLE_FIRE_WEAPON_ALT           = 19,
    VEHICLE_STEER_LEFT                = 20,
    VEHICLE_STEER_RIGHT               = 21,
    VEHICLE_STEER_UP                  = 22,
    VEHICLE_STEER_DOWN                = 23,
    VEHICLE_ACCELERATE                = 24,
    VEHICLE_BRAKE                     = 25,
    VEHICLE_RADIO_STATION_UP          = 26,
    VEHICLE_RADIO_STATION_DOWN        = 27,
    VEHICLE_RADIO_TRACK_SKIP          = 28,
    VEHICLE_HORN                      = 29,
    TOGGLE_SUBMISSIONS                = 30,
    VEHICLE_HANDBRAKE                 = 31,
    PED_1RST_PERSON_LOOK_LEFT         = 32,
    PED_1RST_PERSON_LOOK_RIGHT        = 33,
    VEHICLE_LOOKLEFT                  = 34,
    VEHICLE_LOOKRIGHT                 = 35,
    VEHICLE_LOOKBEHIND                = 36,
    VEHICLE_MOUSELOOK                 = 37,
    VEHICLE_TURRETLEFT                = 38,
    VEHICLE_TURRETRIGHT               = 39,
    VEHICLE_TURRETUP                  = 40,
    VEHICLE_TURRETDOWN                = 41,
    PED_CYCLE_TARGET_LEFT             = 42,
    PED_CYCLE_TARGET_RIGHT            = 43,
    PED_CENTER_CAMERA_BEHIND_PLAYER   = 44,
    PED_LOCK_TARGET                   = 45,
    NETWORK_TALK                      = 46,
    CONVERSATION_YES                  = 47,
    CONVERSATION_NO                   = 48,
    GROUP_CONTROL_FWD                 = 49,
    GROUP_CONTROL_BWD                 = 50,
    PED_1RST_PERSON_LOOK_UP           = 51,
    PED_1RST_PERSON_LOOK_DOWN         = 52,
    NUM_OF_1ST_PERSON_ACTIONS         = 53,
    TOGGLE_DPAD                       = 54,
    SWITCH_DEBUG_CAM_ON               = 55,
    TAKE_SCREEN_SHOT                  = 56,
    SHOW_MOUSE_POINTER_TOGGLE         = 57,
    SWITCH_CAM_DEBUG_MENU             = 58,

    // notsa
    NUM_OF_MAX_CONTROLLER_ACTIONS,
    NUM_OF_MIN_CONTROLLER_ACTIONS = 0,
};

enum eContSetOrder {
    NO_ORDER_SET = 0,
    FIRST        = 1,
    SECOND       = 2,
    THIRD        = 3,
    FOURTH       = 4,
};

// Enum to specify what type of mouse check to perform
enum eMouseCheckType {
    IS_DOWN, // Button is currently down
    IS_UP,   // Button is currently up
    JUST_UP  // Button was just released this frame
};

constexpr eContSetOrder CONTROLLER_ORDERS_VALID[] = {
    eContSetOrder::FIRST,
    eContSetOrder::SECOND,
    eContSetOrder::THIRD,
    eContSetOrder::FOURTH
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

    JOYBUTTON_COUNT
};

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
    bool              m_WasJoyJustInitialised{};
    DIJOYSTATE2       m_OldJoyState{};
    DIJOYSTATE2       m_NewJoyState{};
    ControlName       m_ControllerActionName[NUM_OF_MAX_CONTROLLER_ACTIONS]{};
    bool              m_ButtonStates[JOYBUTTON_COUNT]{}; // True if down, false if up or missing
    CControllerAction m_Actions[NUM_OF_MAX_CONTROLLER_ACTIONS]{};
    bool              m_bStickL_X_Rgh_Lft_MovementBothDown[eControllerType::CONTROLLER_NUM]{};
    bool              m_bStickL_Up_Dwn_MovementBothDown[eControllerType::CONTROLLER_NUM]{};
    bool              m_bStickR_X_Rgh_Lft_MovementBothDown[eControllerType::CONTROLLER_NUM]{};
    bool              m_bStickR_Up_Dwn_MovementBothDown[eControllerType::CONTROLLER_NUM]{};
    bool              m_MouseFoundInitSet{};
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
    bool UseDrivingControls();
    bool UseFirstPersonControls();
    bool IsKeyboardKeyDownInState(CKeyboardState& state, KeyCode key);
    CControllerState& GetControllerState(CPad& pad, eControllerType type);
    int16& GetControllerStateJoyStick(CPad& pad, KeyCode button);
    template<eMouseCheckType CheckType>
    bool CheckMouseButton(KeyCode key);

private:
    CControllerConfigManager* Constructor() {
        this->CControllerConfigManager::CControllerConfigManager();
        return this;
    }
};

VALIDATE_SIZE(CControllerConfigManager, 0x12E4);

extern CControllerConfigManager& ControlsManager;
