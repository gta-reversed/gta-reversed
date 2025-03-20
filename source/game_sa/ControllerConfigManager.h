/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "RenderWare.h"

#include "eInputs.h"

enum eActionType {
    ACTION_0,
    ACTION_1,
    ACTION_2,
    ACTION_3,
    ACTION_4,
    ACTION_5,
    ACTION_6,
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

    void SetMouseButtonAssociatedWithAction(const eControllerAction& action, eMouseButtons button);

    void StoreMouseButtonState(eMouseButtons button, bool state);
    void UpdateJoyInConfigMenus_ButtonDown(eJOY_BUTTONS button, int32 padNumber);
    void UpdateJoy_ButtonDown(eJOY_BUTTONS button, int32 unk);
    void UpdateJoyInConfigMenus_ButtonUp(eJOY_BUTTONS button, int32 padNumber);
    void UpdateJoy_ButtonUp(eJOY_BUTTONS button, int32 unk);
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
    void SetControllerKeyAssociatedWithAction(eControllerAction action, int32 key, eControllerType type);
    void ResetSettingOrder(eControllerAction event);
    RsKeyCodes GetControllerKeyAssociatedWithAction(eControllerAction event, eControllerType type);

    void GetDefinedKeyByGxtName(uint16 actionId, char* buf, uint16 bufsz);

    // Button down state handlers
    void AffectControllerStateOn_ButtonDown(int32 ButtonPress, eControllerType ControllerType);
    void AffectControllerStateOn_ButtonDown_Driving(int32 ButtonPress, eControllerType ControllerType, CControllerState* ControllerToUpdate);
    void AffectControllerStateOn_ButtonDown_VehicleAndThirdPersonOnly(int32 ButtonPress, eControllerType ControllerType, CControllerState* ControllerToUpdate);
    void AffectControllerStateOn_ButtonDown_FirstAndThirdPersonOnly(int32 ButtonPress, eControllerType ControllerType, CControllerState* ControllerToUpdate);
    void AffectControllerStateOn_ButtonDown_FirstPersonOnly(int32 ButtonPress, eControllerType ControllerType, CControllerState* ControllerToUpdate);
    void AffectControllerStateOn_ButtonDown_ThirdPersonOnly(int32 ButtonPress, eControllerType ControllerType, CControllerState* ControllerToUpdate);
    void AffectControllerStateOn_ButtonDown_AllStates(int32 ButtonPress, eControllerType ControllerType, CControllerState* ControllerToUpdate);
    void AffectControllerStateOn_ButtonDown_DebugStuff(int32 ButtonPress, eControllerType ControllerType);

    // NOTSA
    uint16 GetActionIDByName(std::string_view name);

    // not used
    bool GetIsActionAButtonCombo(eControllerAction Action);
};
// VALIDATE_SIZE(CControllerConfigManager, 0x12E4);

extern CControllerConfigManager &ControlsManager;
