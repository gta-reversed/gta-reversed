/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "rw/skeleton.h"

#include "KeyboardState.h"
#include "MouseControllerState.h"
#include "ControllerState.h"

// Taken from GTA3 Script Compiler (miss2.exe)
enum ePadID {
    PAD1 = 0,
    PAD2 = 1,

    MAX_PADS
};

#define KEY_IS_PRESSED(btn)    (NewKeyState.btn && !OldKeyState.btn)

#define BUTTON_IS_PRESSED(btn) (NewState.btn && !OldState.btn)
#define BUTTON_IS_DOWN(btn)    (NewState.btn)
#define BUTTON_JUST_UP(btn)    (!NewState.btn && OldState.btn)

#define MOUSE_IS_PRESSED(btn)  (NewMouseControllerState.btn && !OldMouseControllerState.btn)
#define MOUSE_IS_DOWN(btn)     (NewMouseControllerState.btn)

class CPed;

class CPad {
public:
    CControllerState NewState;
    CControllerState OldState;
    int16            SteeringLeftRightBuffer[10];
    int32            DrunkDrivingBufferUsed;
    CControllerState PCTempKeyState;
    CControllerState PCTempJoyState;
    CControllerState PCTempMouseState;
    char             Phase;
    int16            Mode;
    int16            ShakeDur;

    union {
        struct {
            uint16 bCamera : 1;
            uint16 unk2 : 1;
            uint16 bPlayerAwaitsInGarage : 1;
            uint16 bPlayerOnInteriorTransition : 1;
            uint16 unk3 : 1; // 0x10 unused
            uint16 bPlayerSafe : 1;
            uint16 bPlayerTalksOnPhone : 1; // bPlayerSafeForPhoneCall?
            uint16 bPlayerSafeForCutscene : 1;

            uint16 bPlayerSkipsToDestination : 1; // bPlayerSafeForDestination?
        };
        uint16 DisablePlayerControls;
    };

    char     ShakeFreq;
    char     bHornHistory[5];
    char     iCurrHornHistory;
    char     JustOutOfFrontEnd;
    bool     bApplyBrakes;
    bool     bDisablePlayerEnterCar;
    bool     bDisablePlayerDuck;
    bool     bDisablePlayerFireWeapon;
    bool     bDisablePlayerFireWeaponWithL1;
    bool     bDisablePlayerCycleWeapon;
    bool     bDisablePlayerJump;
    bool     bDisablePlayerDisplayVitalStats;
    uint32   LastTimeTouched;
    int32    AverageWeapon;
    int32    AverageEntries;
    float    NoShakeBeforeThis;
    char     NoShakeFreq;
    char    _pad131[3];

public:
    static CMouseControllerState& PCTempMouseControllerState;
    static CMouseControllerState& NewMouseControllerState;
    static CMouseControllerState& OldMouseControllerState;

    static CKeyboardState& TempKeyState;
    static CKeyboardState& OldKeyState;
    static CKeyboardState& NewKeyState;

    static CPad (&Pads)[MAX_PADS];

    static bool& bInvertLook4Pad;
    static char& padNumber;

public:
    static void InjectHooks();

    CPad();
    ~CPad() = default; // 0x53ED60

    static void Initialise();
    static void ClearKeyBoardHistory();
    static void ClearMouseHistory();
    void Clear(bool enablePlayerControls, bool resetPhase);

    void Update(int32 pad);
    static void UpdatePads();
    void UpdateMouse();
    static void ProcessPad(ePadID padID);
    void ProcessPCSpecificStuff();
    CControllerState& ReconcileTwoControllersInput(CControllerState& out, const CControllerState& controllerA, const CControllerState& controllerB);

    void SetTouched();
    [[nodiscard]] uint32 GetTouchedTimeDelta() const;

    void SetDrunkInputDelay(int32 delay) { DrunkDrivingBufferUsed = delay; } // 0x53F910

    void StartShake(int16 time, uint8 frequency, uint32 arg2);
    void StartShake_Distance(int16 time, uint8 frequency, CVector pos);
    void StartShake_Train(const CVector2D& point);
    static void StopPadsShaking();
    void StopShaking(int16 pad);

    [[nodiscard]] int16 GetCarGunLeftRight() const;
    [[nodiscard]] int16 GetCarGunUpDown() const;
    [[nodiscard]] int16 GetCarGunFired() const;
    [[nodiscard]] int16 CarGunJustDown() const;

    int16 GetSteeringLeftRight();
    [[nodiscard]] int16 GetSteeringUpDown() const;
    int32 GetSteeringMode(); // Android

    int16 GetPedWalkLeftRight(CPed* ped) const;
    [[nodiscard]] int16 GetPedWalkLeftRight() const;
    int16 GetPedWalkUpDown(CPed* ped) const;
    [[nodiscard]] int16 GetPedWalkUpDown() const;

    [[nodiscard]] bool GetLookLeft() const;
    [[nodiscard]] bool GetLookRight() const;

    [[nodiscard]] bool GetHorn() const;
    [[nodiscard]] bool HornJustDown() const;

    [[nodiscard]] int16 GetBrake() const;
    [[nodiscard]] int16 GetHandBrake() const;

    [[nodiscard]] bool GetExitVehicle() const;
    [[nodiscard]] bool ExitVehicleJustDown() const;

    [[nodiscard]] uint8 GetMeleeAttack(bool bCheckButtonCircleStateOnly) const;
    [[nodiscard]] uint8 MeleeAttackJustDown(bool bCheckButtonCircleStateOnly) const;
    [[nodiscard]] int16 GetAccelerate() const;
    [[nodiscard]] bool GetAccelerateJustDown() const;
    [[nodiscard]] bool CycleWeaponLeftJustDown() const;
    [[nodiscard]] bool CycleWeaponRightJustDown() const;
    [[nodiscard]] bool GetTarget() const;
    [[nodiscard]] bool GetSprint() const;
    [[nodiscard]] bool SprintJustDown() const;
    int16 GetDisplayVitalStats(CPed* ped) const;
    [[nodiscard]] bool CollectPickupJustDown() const;
    [[nodiscard]] bool GetForceCameraBehindPlayer() const;
    [[nodiscard]] bool SniperZoomIn() const;
    [[nodiscard]] bool SniperZoomOut() const;

    bool WeaponJustDown(CPed* ped) const;
    [[nodiscard]] bool GetEnterTargeting() const;
    int32 GetWeapon(CPed* ped) const;
    int16 AimWeaponLeftRight(CPed* ped) const;
    int16 AimWeaponUpDown(CPed* ped) const;

    static CPad* GetPad(int32 nPadNumber = 0)                               { return &Pads[nPadNumber]; }                                                                    // 0x53FB70

    [[nodiscard]] bool NextStationJustUp() const noexcept                   { return !DisablePlayerControls && IsDPadUpJustUp(); }                                           // 0x5405B0
    [[nodiscard]] bool LastStationJustUp() const noexcept                   { return !DisablePlayerControls && IsDPadDownJustUp(); }                                         // 0x5405E0
    [[nodiscard]] bool GetLookBehindForCar() const noexcept;                                                                                                                 // 0x53FE70
    [[nodiscard]] bool GetLookBehindForPed() const noexcept                 { return !DisablePlayerControls && BUTTON_IS_DOWN(ShockButtonR); }                               // 0x53FEC0
    [[nodiscard]] bool GetHydraulicJump() const noexcept                    { return !DisablePlayerControls && BUTTON_IS_DOWN(ShockButtonR); }                               // 0x53FF70
    [[nodiscard]] bool GetDuck() const noexcept                             { return !DisablePlayerControls && !bDisablePlayerDuck && BUTTON_IS_DOWN(ShockButtonL); }        // 0x540700
    [[nodiscard]] bool DuckJustDown() const noexcept                        { return !DisablePlayerControls && !bDisablePlayerDuck && IsLeftShockPressed(); }                // 0x540720
    [[nodiscard]] bool GetJump() const noexcept                             { return !DisablePlayerControls && !bDisablePlayerDuck && BUTTON_IS_DOWN(ButtonSquare); }        // 0x540750
    [[nodiscard]] bool JumpJustDown() const noexcept                        { return !DisablePlayerControls && !bDisablePlayerDuck && IsSquarePressed(); }                   // 0x540770
    [[nodiscard]] bool ShiftTargetLeftJustDown() const noexcept             { return !DisablePlayerControls && IsLeftShoulder2Pressed(); }                                   // 0x540850
    [[nodiscard]] bool ShiftTargetRightJustDown() const noexcept            { return !DisablePlayerControls && IsRightShoulder2Pressed(); }                                  // 0x540880
    [[nodiscard]] bool GetGroupControlForward() const noexcept              { return !DisablePlayerControls && BUTTON_IS_DOWN(DPadUp); }                                     // 0x541190
    [[nodiscard]] bool GetGroupControlBack() const noexcept                 { return !DisablePlayerControls && BUTTON_IS_DOWN(DPadDown); }                                   // 0x5411B0
    [[nodiscard]] bool ConversationYesJustDown() const noexcept             { return !DisablePlayerControls && IsDPadRightPressed(); }                                       // 0x5411D0
    [[nodiscard]] bool ConversationNoJustDown() const noexcept              { return !DisablePlayerControls && IsDPadLeftPressed(); }                                        // 0x541200
    [[nodiscard]] bool GroupControlForwardJustDown() const noexcept         { return !DisablePlayerControls && IsDPadUpPressed(); }                                          // 0x541230
    [[nodiscard]] bool GroupControlBackJustDown() const noexcept            { return !DisablePlayerControls && IsDPadDownPressed(); }                                        // 0x541260

    // KEYBOARD
    [[nodiscard]] bool IsFKeyJustDown(uint8 key) const noexcept             { return NewKeyState.FKeys[key] && OldKeyState.FKeys[key]; }
    [[nodiscard]] bool IsFKeyJustPressed(uint8 key) const noexcept          { return NewKeyState.FKeys[key] && !OldKeyState.FKeys[key]; }
    [[nodiscard]] bool IsF1JustPressed() const noexcept                     { return IsFKeyJustPressed(0); }
    [[nodiscard]] bool IsF2JustPressed() const noexcept                     { return IsFKeyJustPressed(1); }
    [[nodiscard]] bool IsF3JustPressed() const noexcept                     { return IsFKeyJustPressed(2); }
    [[nodiscard]] bool IsF4JustPressed() const noexcept                     { return IsFKeyJustPressed(3); }
    [[nodiscard]] bool IsF5JustPressed() const noexcept                     { return IsFKeyJustPressed(4); }
    [[nodiscard]] bool IsF6JustPressed() const noexcept                     { return IsFKeyJustPressed(5); }
    [[nodiscard]] bool IsF7JustPressed() const noexcept                     { return IsFKeyJustPressed(6); }
    [[nodiscard]] bool IsF8JustPressed() const noexcept                     { return IsFKeyJustPressed(7); }
    [[nodiscard]] bool IsF9JustPressed() const noexcept                     { return IsFKeyJustPressed(8); }
    [[nodiscard]] bool IsF10JustPressed() const noexcept                    { return IsFKeyJustPressed(9); }
    [[nodiscard]] bool IsF11JustPressed() const noexcept                    { return IsFKeyJustPressed(10); }
    [[nodiscard]] bool IsF12JustPressed() const noexcept                    { return IsFKeyJustPressed(11); }

    [[nodiscard]] bool IsStandardKeyJustDown(uint8 key) const noexcept      { return NewKeyState.standardKeys[key] && OldKeyState.standardKeys[key]; }                       //
    [[nodiscard]] bool IsStandardKeyJustPressed(uint8 key) const noexcept   { return NewKeyState.standardKeys[key] && !OldKeyState.standardKeys[key]; }                      // 0x4D59B0
    [[nodiscard]] bool IsLeftCtrlJustPressed() const noexcept               { return KEY_IS_PRESSED(lctrl); }                                                                //
    [[nodiscard]] bool IsRightCtrlJustPressed() const noexcept              { return KEY_IS_PRESSED(rctrl); }                                                                //
    [[nodiscard]] bool IsCtrlJustDown() const noexcept                      { return IsLeftCtrlJustPressed() || IsRightCtrlJustPressed(); }                                  //
    [[nodiscard]] bool IsLeftCtrlJustDown() const noexcept                  { return NewKeyState.lctrl && OldKeyState.lctrl; }                                               //
    [[nodiscard]] bool IsRightCtrlJustDown() const noexcept                 { return NewKeyState.rctrl && OldKeyState.rctrl; }                                               //
    [[nodiscard]] bool IsCtrlPressed() const noexcept                       { return IsLeftCtrlJustDown() || IsRightCtrlJustDown(); }                                        //
    [[nodiscard]] static bool IsUpPressed() noexcept                        { return KEY_IS_PRESSED(up); }                                                                   //
    [[nodiscard]] static bool IsDownPressed() noexcept                      { return KEY_IS_PRESSED(down); }                                                                 //
    [[nodiscard]] static bool IsLeftPressed() noexcept                      { return KEY_IS_PRESSED(left); }                                                                 //
    [[nodiscard]] static bool IsRightPressed() noexcept                     { return KEY_IS_PRESSED(right); }                                                                //
    static bool IsPadEnterJustPressed() noexcept                            { return KEY_IS_PRESSED(enter); }                                                                //
    static bool IsReturnJustPressed() noexcept                              { return KEY_IS_PRESSED(extenter); }                                                             //
    static bool IsEnterJustPressed() noexcept                               { return IsPadEnterJustPressed() || IsReturnJustPressed(); }                                     // 0x4D5980
    static bool f0x57C330() { return !NewKeyState.enter && OldKeyState.enter || !NewKeyState.extenter && OldKeyState.extenter; }                                             // 0x57C330

    static bool IsMenuKeyJustPressed() noexcept                             { return KEY_IS_PRESSED(lmenu); }                                                                // 0x744D50
    static bool IsTabJustPressed() noexcept                                 { return KEY_IS_PRESSED(tab); }                                                                  // 0x744D90
    static bool IsEscJustPressed() noexcept                                 { return KEY_IS_PRESSED(esc); }                                                                  // 0x572DB0

    bool IsRadioTrackSkipPressed() { return BUTTON_IS_PRESSED(m_bRadioTrackSkip); } // 0x4E7F20
    static bool f0x57C360() { return NewKeyState.back && !OldKeyState.back; }       // 0x57C360

    // KEYBOARD END

    // PAD
    [[nodiscard]] bool IsCrossPressed() const noexcept                      { return BUTTON_IS_PRESSED(ButtonCross); }                                                       // 0x4D59E0
    [[nodiscard]] bool f0x57C3A0() const noexcept                           { return !NewState.ButtonCross && OldState.ButtonCross; }                                        // 0x57C3A0
    [[nodiscard]] bool IsCirclePressed() const noexcept                     { return BUTTON_IS_PRESSED(ButtonCircle); }                                                      // 0x53EF60
    [[nodiscard]] bool IsTrianglePressed() const noexcept                   { return BUTTON_IS_PRESSED(ButtonTriangle); }                                                    // 0x53EF40
    [[nodiscard]] bool IsSquarePressed() const noexcept                     { return BUTTON_IS_PRESSED(ButtonSquare); }                                                      // 0x53EF20

    [[nodiscard]] bool IsLeftShockPressed() const noexcept                  { return BUTTON_IS_PRESSED(ShockButtonL); }                                                      // 0x509840
    [[nodiscard]] bool IsRightShockPressed() const noexcept                 { return BUTTON_IS_PRESSED(ShockButtonR); }                                                      //

    [[nodiscard]] bool IsRightStickYPressed() const noexcept                { return BUTTON_IS_PRESSED(RightStickY); }                                                       // 0x53ED90

    [[nodiscard]] bool IsStartPressed() const noexcept                      { return BUTTON_IS_PRESSED(Start); }                                                             //
    [[nodiscard]] bool IsSelectPressed() const noexcept                     { return BUTTON_IS_PRESSED(Select); }                                                            // 0x53EF00

    [[nodiscard]] bool IsDPadLeftPressed() const noexcept                   { return BUTTON_IS_PRESSED(DPadLeft); }                                                          // 0x53EEE0
    [[nodiscard]] bool IsDPadRightPressed() const noexcept                  { return BUTTON_IS_PRESSED(DPadRight); }                                                         //
    [[nodiscard]] bool IsDPadUpPressed() const noexcept                     { return BUTTON_IS_PRESSED(DPadUp); }                                                            // 0x53EE60
    [[nodiscard]] bool IsDPadDownPressed() const noexcept                   { return BUTTON_IS_PRESSED(DPadDown); }                                                          // 0x53EEA0

    [[nodiscard]] bool IsDPadLeftJustUp() const noexcept                    { return BUTTON_JUST_UP(DPadLeft); }                                                             //
    [[nodiscard]] bool f0x541170() const noexcept                           { return !DisablePlayerControls && NewState.DPadLeft != 0; }                                     // 0x541170
    [[nodiscard]] bool f0x57C380() const noexcept                           { return NewState.DPadLeft != 0; }                                                               // 0x57C380

    [[nodiscard]] bool IsDPadRightJustUp() const noexcept                   { return BUTTON_JUST_UP(DPadRight); }                                                            //
    [[nodiscard]] bool f0x541150() const noexcept                           { return !DisablePlayerControls && NewState.DPadRight != 0; }                                    // 0x541150
    [[nodiscard]] bool f0x57C390() const noexcept                           { return NewState.DPadRight != 0; }                                                              // 0x57C390

    [[nodiscard]] bool IsDPadUpJustUp() const noexcept                      { return BUTTON_JUST_UP(DPadUp); }                                                               // 0x53EE80
    [[nodiscard]] bool IsDPadDownJustUp() const noexcept                    { return BUTTON_JUST_UP(DPadDown); }                                                             // 0x53EEC0

    [[nodiscard]] bool IsLeftShoulder1Pressed() const noexcept              { return BUTTON_IS_PRESSED(LeftShoulder1); }                                                     // 0x53EDC0
    [[nodiscard]] bool IsLeftShoulder1() const noexcept                     { return BUTTON_IS_DOWN(LeftShoulder1); }                                                        // 0x53EDB0
    bool f0x4D5970()                                                        { return NewState.LeftShoulder1 == 0; }                                                          // 0x4D5970

    [[nodiscard]] bool IsLeftShoulder2JustUp() const noexcept               { return BUTTON_JUST_UP(LeftShoulder2); }                                                        // 0x53EE00
    [[nodiscard]] bool IsLeftShoulder2Pressed() const noexcept              { return BUTTON_IS_PRESSED(LeftShoulder2); }                                                     // 0x53EDE0

    [[nodiscard]] bool IsRightShoulder1Pressed() const noexcept             { return BUTTON_IS_PRESSED(RightShoulder1); }                                                    // 0x53EE20
    [[nodiscard]] bool IsRightShoulder1Up() const noexcept                  { return BUTTON_JUST_UP(RightShoulder1); }                                                       //

    [[nodiscard]] bool IsRightShoulder2Pressed() const noexcept             { return BUTTON_IS_PRESSED(RightShoulder2); }                                                    //
    [[nodiscard]] bool IsRightShoulder2JustUp() const noexcept              { return BUTTON_JUST_UP(RightShoulder2); }                                                       // 0x53EE40

    [[nodiscard]] int16 GetLeftStickX() const noexcept                      { return BUTTON_IS_DOWN(LeftStickX); }
    [[nodiscard]] int16 GetLeftStickY() const noexcept                      { return BUTTON_IS_DOWN(LeftStickY); }
    [[nodiscard]] int16 GetRightStickX() const noexcept                     { return BUTTON_IS_DOWN(RightStickX); }
    [[nodiscard]] int16 GetRightStickY() const noexcept                     { return BUTTON_IS_DOWN(RightStickY); }

    bool IsSteeringInAnyDirection() { return GetSteeringLeftRight() || GetSteeringUpDown(); }

    // PAD END

    // MOUSE
    static bool f0x57C3C0() noexcept               { return !NewMouseControllerState.lmb && OldMouseControllerState.lmb; }          // 0x57C3C0
    static bool IsMouseLButtonPressed() noexcept   { return MOUSE_IS_PRESSED(lmb); }                                                // 0x4D5A00
    static bool IsMouseRButtonPressed() noexcept   { return MOUSE_IS_PRESSED(rmb); }                                                // 0x572E70
    static bool IsMouseMButtonPressed() noexcept   { return MOUSE_IS_PRESSED(mmb); }                                                // 0x57C3E0
    static bool IsMouseWheelUpPressed() noexcept   { return MOUSE_IS_PRESSED(wheelUp); }                                            // 0x57C400
    static bool IsMouseWheelDownPressed() noexcept { return MOUSE_IS_PRESSED(wheelDown); }                                          // 0x57C420
    static bool IsMouseBmx1Pressed() noexcept      { return MOUSE_IS_PRESSED(bmx1); }                                               // 0x57C440
    static bool IsMouseBmx2Pressed() noexcept      { return MOUSE_IS_PRESSED(bmx2); }                                               // 0x57C460
    static bool IsMouseLButton() noexcept          { return MOUSE_IS_DOWN(lmb); }                                                   // 0x45AF70
    static bool IsMouseRButton() noexcept          { return MOUSE_IS_DOWN(rmb); }                                                   // 0x45AF80
    // MOUSE END

    int32 sub_541320() { return AverageWeapon / AverageEntries; } // 0x541320
    int32 sub_541290();
    bool sub_541170() const noexcept;
    bool sub_541150() const noexcept;
    static bool sub_540A40();
    static bool sub_540A10();
    static bool GetAnaloguePadLeft();
    static bool GetAnaloguePadUp();
    static bool GetAnaloguePadRight();
    static bool GetAnaloguePadDown();
    bool sub_540530() const noexcept;
    bool sub_5404F0() const noexcept { return Mode != 1 ? 0 : IsDPadDownPressed(); } // 0x5404F0
    bool IsPhaseEqual11() const noexcept { return Phase == 11; } // 0x53FB60

    static void SetCurrentPad(int8 pad) { padNumber = pad; } // 0x53ED70

    // 0x541A60
    static bool UpdatePadsTillStable() { return true; }
    bool ArePlayerControlsDisabled() { return DisablePlayerControls != 0; }
    bool DebugMenuJustPressed();
};

VALIDATE_SIZE(CPad, 0x134);

// return pressed key, in order of CKeyboardState
int GetCurrentKeyPressed(RsKeyCodes& keys);

// todo: move these fucks out
IDirectInputDevice8* DIReleaseMouse();
void InitialiseMouse(bool exclusive);

/*
Android has 99 funcs

These are missing:
CheckPointSave(void)
CinemaCamera(void)
CraneReleaseJustDown(void)
CycleCameraModeDownJustDown(bool)
DiveJustDown(void)
DropItemJustDown(void)
EditCodesForControls(int *,int)
EditString(char *,int)
FixPadsAfterSave(void)
GetAbortClimb(void)
GetAutoClimb(void)
GetExitTargeting(void)
GetLeftAnalogue(CVector2D *)
GetNitroFired(void)
GetRightAnalogue(CVector2D *)
GetTurretLeft(void)
GetTurretRight(void)
IsFlyingRCVehicle(void)
LookAroundLeftRight(void)
LookAroundUpDown(void)
ProcessStoppie(void)
ProcessWheelie(float)
SwimJumpJustDown(void)
UpdatePadsTillStable(void)
UseBomb(void)
UseRocket(void)
WeaponUsesAttackButton(CPed *)
WeaponUsesTargetingButton(void)
WillBulletHitSomething(CPed *)

- bigger than one line, move to .cpp
bool 0x5404A0() { }
bool 0x53FE50() { }
bool 0x53FD70() { }
bool 0x541340() { }
bool 0x540F80() { }
bool 0x540E80() { }
bool 0x540CC0() { }
bool 0x540BD0() { }
bool 0x53FCD0() { }
 - end
*/
