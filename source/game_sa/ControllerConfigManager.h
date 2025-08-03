/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "RenderWare.h"
#include "eControllerType.h"

const int SETTINGS_VERSION_NUM = 6;

enum class eActionType {
    FIRST_PERSON,
    THIRD_PERSON,
    IN_CAR,
    IN_CAR_THIRD_PERSON,
    COMMON_CONTROLS,
    FIRST_THIRD_PERSON,
    NONE
};

enum class eControllerAction {
    COMBOLOCK = -2,
    CA_NONE   = -1,

    PED_FIRE_WEAPON = 0,
    PED_FIRE_WEAPON_ALT,
    PED_CYCLE_WEAPON_RIGHT,
    PED_CYCLE_WEAPON_LEFT,
    GO_FORWARD,
    GO_BACK,
    GO_LEFT,
    GO_RIGHT,
    PED_SNIPER_ZOOM_IN,
    PED_SNIPER_ZOOM_OUT,
    VEHICLE_ENTER_EXIT,
    CAMERA_CHANGE_VIEW_ALL_SITUATIONS,
    PED_JUMPING,
    PED_SPRINT,
    PED_LOOKBEHIND,
    PED_DUCK,
    PED_ANSWER_PHONE,
    PED_WALK,
    VEHICLE_FIRE_WEAPON,
    VEHICLE_FIRE_WEAPON_ALT,
    VEHICLE_STEERLEFT,
    VEHICLE_STEERRIGHT,
    VEHICLE_STEERUP,
    VEHICLE_STEERDOWN,
    VEHICLE_ACCELERATE,
    VEHICLE_BRAKE,
    VEHICLE_RADIO_STATION_UP,
    VEHICLE_RADIO_STATION_DOWN,
    VEHICLE_RADIO_TRACK_SKIP,
    VEHICLE_HORN,
    TOGGLE_SUBMISSIONS,
    VEHICLE_HANDBRAKE,
    PED_1RST_PERSON_LOOK_LEFT,
    PED_1RST_PERSON_LOOK_RIGHT,
    VEHICLE_LOOKLEFT,
    VEHICLE_LOOKRIGHT,
    VEHICLE_LOOKBEHIND,
    VEHICLE_MOUSELOOK,
    VEHICLE_TURRETLEFT,
    VEHICLE_TURRETRIGHT,
    VEHICLE_TURRETUP,
    VEHICLE_TURRETDOWN,
    PED_CYCLE_TARGET_LEFT,
    PED_CYCLE_TARGET_RIGHT,
    PED_CENTER_CAMERA_BEHIND_PLAYER,
    PED_LOCK_TARGET,
    NETWORK_TALK,
    CONVERSATION_YES,
    CONVERSATION_NO,
    GROUP_CONTROL_FWD,
    GROUP_CONTROL_BWD,
    PED_1RST_PERSON_LOOK_UP,
    PED_1RST_PERSON_LOOK_DOWN,
    NUM_OF_1ST_PERSON_ACTIONS,
    TOGGLE_DPAD,
    SWITCH_DEBUG_CAM_ON,
    TAKE_SCREEN_SHOT,
    SHOW_MOUSE_POINTER_TOGGLE,
    SWITCH_CAM_DEBUG_MENU,

    // android widget...

    CONTROLLER_ACTION_COUNT
};

enum class eContSetOrder : uint32 {
    NONE = 0,
    FIRST,
    SECOND,
    THIRD,
    FOURTH,

    NUM_OR_ORDER_STATES
};

constexpr std::array<eContSetOrder, 4> CONTROLLER_ORDERS_SET = {
    eContSetOrder::FIRST,
    eContSetOrder::SECOND,
    eContSetOrder::THIRD,
    eContSetOrder::FOURTH
}; // NOTSA

// TODO: rename it based on CControllerState and GetControllerStateJoyStick
enum eJoyButtons : uint32 {
    NO_JOYBUTTONS = 0,
    JOYBUTTON_ONE,
    JOYBUTTON_TWO,
    JOYBUTTON_THREE,
    JOYBUTTON_FOUR,
    JOYBUTTON_FIVE,
    JOYBUTTON_SIX,
    JOYBUTTON_SEVEN,
    JOYBUTTON_EIGHT,
    JOYBUTTON_NINE,
    JOYBUTTON_TEN,
    JOYBUTTON_ELEVEN,
    JOYBUTTON_TWELVE,
    JOYBUTTON_THIRTEEN,
    JOYBUTTON_FOURTEEN,
    JOYBUTTON_FIFTHTEEN,
    JOYBUTTON_SIXTEEN,

    JOYBUTTON_COUNT
};

using KeyCode = uint32; // NOTSA: Originally that is RW type, but we use uint32 for consistency

struct CControllerConfig {
    KeyCode       m_uiActionInitiator;
    eContSetOrder m_uiSetOrder;
};

VALIDATE_SIZE(CControllerConfig, 0x8);

struct CControllerAction {
    CControllerConfig Keys[+eControllerType::CONTROLLER_TYPES_COUNT]{};
};

VALIDATE_SIZE(CControllerAction, 0x20);

const auto NUM_OF_GAME_JOYS = 2;

struct JoyStruct {
    uint32 wDeviceID;
    bool   bJoyAttachedToPort;
    bool   bZAxisPresent;
    bool   bZRotPresent;
    DWORD  wVendorID;
    DWORD  wProductID;
};

VALIDATE_SIZE(JoyStruct, 0x10);

struct CJoySticks {
    CJoySticks();

    JoyStruct JoyStickNum[NUM_OF_GAME_JOYS]{};
    void      ClearJoyInfo(int32 JoyNumber);
};

VALIDATE_SIZE(CJoySticks, 0x20);

static inline auto& AllValidWinJoys = StaticRef<CJoySticks, 0xC92144>();

using ControlName = GxtChar[40];

class CControllerConfigManager {
public:
    bool m_bJoyJustInitialised{};

    DIJOYSTATE2 m_OldJoyState;
    DIJOYSTATE2 m_NewJoyState;

    ControlName m_ControllerActionName[+eControllerAction::CONTROLLER_ACTION_COUNT];

    bool m_ButtonStates[JOYBUTTON_COUNT]; // True if down, false if up or missing
    CControllerAction m_Actions[+eControllerAction::CONTROLLER_ACTION_COUNT]{};

    bool m_bStickL_X_Rgh_Lft_MovementBothDown[+eControllerType::CONTROLLER_TYPES_COUNT];
    bool m_bStickL_Up_Dwn_MovementBothDown[+eControllerType::CONTROLLER_TYPES_COUNT];

    bool m_bStickR_X_Rgh_Lft_MovementBothDown[+eControllerType::CONTROLLER_TYPES_COUNT];
    bool m_bStickR_Up_Dwn_MovementBothDown[+eControllerType::CONTROLLER_TYPES_COUNT];
    bool m_MouseFoundInitSet{};

public:
    static void InjectHooks();

    CControllerConfigManager();


    eJoyButtons GetJoyButtonJustUp();
    eJoyButtons GetJoyButtonJustDown();

    void InitDefaultControlConfiguration();
    void InitDefaultControlConfigMouse(const CMouseControllerState& state, bool mouseControls);
    void InitDefaultControlConfigJoyPad(uint32 buttonCount);
    void InitialiseControllerActionNameArray();
    void StoreMouseButtonState(RsKeyCodes button, bool state);
    void UpdateJoyInConfigMenus_ButtonDown(KeyCode button, int32 padNumber);
    void AffectControllerStateOn_ButtonDown(KeyCode button, eControllerType type);
    void AffectControllerStateOn_ButtonDown_Driving(KeyCode button, eControllerType type, CControllerState& state);
    void AffectControllerStateOn_ButtonDown_FirstAndThirdPersonOnly(KeyCode button, eControllerType type, CControllerState& state);
    void AffectControllerStateOn_ButtonDown_ThirdPersonOnly(KeyCode button, eControllerType type, CControllerState& state);
    void AffectControllerStateOn_ButtonDown_FirstPersonOnly(KeyCode button, eControllerType type, CControllerState& state);
    void AffectControllerStateOn_ButtonDown_VehicleAndThirdPersonOnly(KeyCode button, eControllerType type, CControllerState& state);
    void AffectControllerStateOn_ButtonDown_DebugStuff(KeyCode button, eControllerType type);

    void AffectControllerStateOn_ButtonDown_AllStates(KeyCode button, eControllerType type, CControllerState& state);
    void ClearSimButtonPressCheckers();
    const GxtChar* GetFirstKeyboardControllerText(eControllerAction action);
    eActionType GetActionType(eControllerAction action);
    void UpdateJoyInConfigMenus_ButtonUp(KeyCode button, int32 padNumber);
    void AffectControllerStateOn_ButtonUp(KeyCode button, eControllerType type);
    void AffectControllerStateOn_ButtonUp_All_Player_States(KeyCode button, eControllerType type, CControllerState& state);
    void AffectControllerStateOn_ButtonUp_DebugStuff(int32, eControllerType);

    void ClearSettingsAssociatedWithAction(eControllerAction action, eControllerType type);
    void MakeControllerActionsBlank();
    void ReinitControls();

    bool GetIsActionAButtonCombo(eControllerAction action);
    const GxtChar* GetButtonComboText(eControllerAction action);
    const GxtChar* GetControllerSettingTextKeyBoard(eControllerAction action, eControllerType type);
    const GxtChar* GetControllerSettingTextMouse(eControllerAction action);
    const GxtChar* GetControllerSettingTextJoystick(eControllerAction action);
    void UpdateJoyButtonState(int32 PadID);
    eContSetOrder GetNumOfSettingsForAction(eControllerAction action);

    // Retrieves GXT string representation of command keys for display
    void GetGxtStringOfCommandKeys(eControllerAction action, GxtChar* pStringToFill, uint16 maximumLength);
    const GxtChar* GetControllerSettingTextWithOrderNumber(eControllerAction action, eContSetOrder order);
    void SetControllerKeyAssociatedWithAction(eControllerAction action, KeyCode button, eControllerType type);
    KeyCode GetControllerKeyAssociatedWithAction(eControllerAction action, eControllerType type);
    void SetMouseButtonAssociatedWithAction(eControllerAction action, KeyCode button);
    KeyCode GetMouseButtonAssociatedWithAction(eControllerAction action);

    // Removes all action initiators matching the specified button and controller type
    void DeleteMatchingActionInitiators(eControllerAction action, KeyCode button, eControllerType type);
    void AffectPadFromKeyBoard();
    void AffectPadFromMouse();
    bool GetIsKeyboardKeyDown(KeyCode button);

    bool GetIsMouseButtonJustUp(KeyCode button);
    bool GetIsMouseButtonDown(KeyCode button);
    bool GetIsMouseButtonUp(KeyCode button);
    void ResetSettingOrder(eControllerAction action);
    bool SaveSettings(FILESTREAM file);
    bool LoadSettings(FILESTREAM file);

    void UpdateJoy_ButtonDown(KeyCode button, eControllerType type);

    // NOTSA
    eControllerAction GetActionIDByName(std::string_view name);

protected:
    void DeleteMatchingCommonControls(eControllerAction nop, KeyCode button, eControllerType type);
    void DeleteMatching3rdPersonControls(eControllerAction action, KeyCode button, eControllerType type);
    void DeleteMatchingVehicleControls(eControllerAction nop, KeyCode button, eControllerType type);
    void DeleteMatchingVehicle_3rdPersonControls(KeyCode button, eControllerType type);
    void DeleteMatching1rstPersonControls(eControllerAction nop, KeyCode button, eControllerType type);
    void DeleteMatching1rst3rdPersonControls(eControllerAction action, KeyCode button, eControllerType type);
    bool GetIsKeyBlank(KeyCode button, eControllerType type);

    // Checks if the specified keyboard key was just pressed this frame
    bool GetIsKeyboardKeyJustDown(KeyCode button);

private:
    // inline region, original this use define's?
    void CheckAndClear(eControllerAction action, eControllerType type, KeyCode button);
    void CheckAndSetButton(eControllerAction action, eControllerType type, KeyCode button, int16& state);
    void CheckAndReset(eControllerAction action, eControllerType type, KeyCode button, int16& state);
    void CheckAndSetPad(eControllerAction action, eControllerType type, KeyCode button, int16& primaryState, int16& secondaryState);
    void CheckAndSetStick(eControllerAction action, eControllerType type, KeyCode button, int16& state, bool& movementBothDown, int16 value);

    bool UseDrivingControls();
    bool UseFirstPersonControls();
    bool IsKeyboardKeyDownInState(CKeyboardState& state, KeyCode key);
    CControllerState& GetControllerState(CPad& pad, eControllerType type);
    int16& GetControllerStateJoyStick(CPad& pad, KeyCode button);

private:
    CControllerConfigManager* Constructor() {
        this->CControllerConfigManager::CControllerConfigManager();
        return this;
    }
};

VALIDATE_SIZE(CControllerConfigManager, 0x12E4);

extern CControllerConfigManager& ControlsManager;
