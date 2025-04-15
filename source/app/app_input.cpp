#include "StdInc.h"

#include "app_input.h"
#include "platform.h"
#include "ControllerConfigManager.h"

void AppInputInjectHooks() {
    RH_ScopedCategory("App");
    RH_ScopedNamespaceName("Input");

    RH_ScopedGlobalInstall(AttachInputDevices, 0x744A20);

    RH_ScopedGlobalInstall(HandleKeyDown, 0x743DF0);
    RH_ScopedGlobalInstall(HandleKeyUp, 0x7443C0);
    RH_ScopedGlobalInstall(KeyboardHandler, 0x744880);

    // Can't hook these, because argument is passed in eax (And I don't feel like dealing with that)
    //RH_ScopedGlobalInstall(HandlePadButtonDown, 0x7448B0);
    //RH_ScopedGlobalInstall(HandlePadButtonUp, 0x744930);

    RH_ScopedGlobalInstall(PadHandler, 0x7449F0);
}

/*
 *****************************************************************************
 */
// 0x743DF0
static RsEventStatus
HandleKeyDown(RsKeyStatus* keyStatus) {
    CPad* pad0 = CPad::GetPad(0);
    CPad* pad1 = CPad::GetPad(1);

    RwInt32 c = keyStatus->keyScanCode; // NOTSA: Use keyScanCode instead of keyCharCode

    if (c != rsNULL) {
        switch (c) {
        case rsESC: {
            CPad::TempKeyState.esc = 255;
            break;
        }

        case rsINS: {
            CPad::TempKeyState.insert = 255;
            break;
        }

        case rsDEL: {
            CPad::TempKeyState.del = 255;
            break;
        }

        case rsHOME: {
            CPad::TempKeyState.home = 255;
            break;
        }

        case rsEND: {
            CPad::TempKeyState.end = 255;
            break;
        }

        case rsPGUP: {
            CPad::TempKeyState.pgup = 255;
            break;
        }

        case rsPGDN: {
            CPad::TempKeyState.pgdn = 255;
            break;
        }

        case rsUP: {
            CPad::TempKeyState.up = 255;
            break;
        }

        case rsDOWN: {
            CPad::TempKeyState.down = 255;
            break;
        }

        case rsLEFT: {
            CPad::TempKeyState.left = 255;
            break;
        }

        case rsRIGHT: {
            CPad::TempKeyState.right = 255;
            break;
        }

        case rsNUMLOCK: {
            CPad::TempKeyState.numlock = 255;
            break;
        }

        case rsPADDEL: {
            CPad::TempKeyState.decimal = 255;
            break;
        }

        case rsPADEND: {
            CPad::TempKeyState.num1 = 255;
            break;
        }

        case rsPADDOWN: {
            CPad::TempKeyState.num2 = 255;
            break;
        }

        case rsPADPGDN: {
            CPad::TempKeyState.num3 = 255;
            break;
        }

        case rsPADLEFT: {
            CPad::TempKeyState.num4 = 255;
            break;
        }

        case rsPAD5: {
            CPad::TempKeyState.num5 = 255;
            break;
        }

        case rsPADRIGHT: {
            CPad::TempKeyState.num6 = 255;
            break;
        }

        case rsPADHOME: {
            CPad::TempKeyState.num7 = 255;
            break;
        }

        case rsPADUP: {
            CPad::TempKeyState.num8 = 255;
            break;
        }

        case rsPADPGUP: {
            CPad::TempKeyState.num9 = 255;
            break;
        }

        case rsPADINS: {
            CPad::TempKeyState.num0 = 255;
            break;
        }

        case rsDIVIDE: {
            CPad::TempKeyState.div = 255;
            break;
        }

        case rsTIMES: {
            CPad::TempKeyState.mul = 255;
            break;
        }

        case rsMINUS: {
            CPad::TempKeyState.sub = 255;
            break;
        }

        case rsPADENTER: {
            CPad::TempKeyState.enter = 255;
            break;
        }

        case rsPLUS: {
            CPad::TempKeyState.add = 255;
            break;
        }

        case rsENTER: {
            CPad::TempKeyState.extenter = 255;
            break;
        }

        case rsSCROLL: {
            CPad::TempKeyState.scroll = 255;
            break;
        }

        case rsPAUSE: {
            CPad::TempKeyState.pause = 255;
            break;
        }

        case rsBACKSP: {
            CPad::TempKeyState.back = 255;
            break;
        }

        case rsTAB: {
            CPad::TempKeyState.tab = 255;
            break;
        }

        case rsCAPSLK: {
            CPad::TempKeyState.capslock = 255;
            break;
        }

        case rsLSHIFT: {
            CPad::TempKeyState.lshift = 255;
            break;
        }

        case rsSHIFT: {
            CPad::TempKeyState.shift = 255;
            break;
        }

        case rsRSHIFT: {
            CPad::TempKeyState.rshift = 255;
            break;
        }

        case rsLCTRL: {
            CPad::TempKeyState.lctrl = 255;
            break;
        }

        case rsRCTRL: {
            CPad::TempKeyState.rctrl = 255;
            break;
        }

        case rsLALT: {
            CPad::TempKeyState.lalt = 255;
            break;
        }

        case rsRALT: {
            CPad::TempKeyState.ralt = 255;
            break;
        }

        case rsLWIN: {
            CPad::TempKeyState.lwin = 255;
            break;
        }

        case rsRWIN: {
            CPad::TempKeyState.rwin = 255;
            break;
        }

        case rsAPPS: {
            CPad::TempKeyState.apps = 255;
            break;
        }

        case rsF1:
        case rsF2:
        case rsF3:
        case rsF4:
        case rsF5:
        case rsF6:
        case rsF7:
        case rsF8:
        case rsF9:
        case rsF10:
        case rsF11:
        case rsF12: {
            CPad::TempKeyState.FKeys[c - rsF1] = 255;
            break;
        }

        default: {
            if (c < 255) {
                CPad::TempKeyState.standardKeys[c] = 255;
                //pad0->AddToPCCheatString(c);/////////////////////////////////////////////////////////////////////
            }
            break;
        }
        }

        if (CPad::padNumber != 0) // NOTSA: Use CPad::padNumber instead of CPad::m_bMapPadOneToPadTwo
        {
            auto& pctks = pad1->PCTempKeyState; // NOTSA: Use pctks for brevity
            if (c == 'D') {
                pctks.LeftStickX = 128;
            }
            if (c == 'A') {
                pctks.LeftStickX = -128;
            }
            if (c == 'W') {
                pctks.LeftStickY = 128;
            }
            if (c == 'S') {
                pctks.LeftStickY = -128;
            }
            if (c == 'J') {
                pctks.RightStickX = 128;
            }
            if (c == 'G') {
                pctks.RightStickX = -128;
            }
            if (c == 'Y') {
                pctks.RightStickY = 128;
            }
            if (c == 'H') {
                pctks.RightStickY = -128;
            }
            if (c == 'Z') {
                pctks.LeftShoulder1 = 255;
            }
            if (c == 'X') {
                pctks.LeftShoulder2 = 255;
            }
            if (c == 'C') {
                pctks.RightShoulder1 = 255;
            }
            if (c == 'V') {
                pctks.RightShoulder2 = 255;
            }
            if (c == 'O') {
                pctks.DPadUp = 255;
            }
            if (c == 'L') {
                pctks.DPadDown = 255;
            }
            if (c == 'K') {
                pctks.DPadLeft = 255;
            }
            if (c == ';') {
                pctks.DPadRight = 255;
            }
            if (c == 'B') {
                pctks.Start = 255;
            }
            if (c == 'N') {
                pctks.Select = 255;
            }
            if (c == 'M') {
                pctks.ButtonSquare = 255;
            }
            if (c == ',') {
                pctks.ButtonTriangle = 255;
            }
            if (c == '.') {
                pctks.ButtonCross = 255;
            }
            if (c == '/') {
                pctks.ButtonCircle = 255;
            }
            if (c == rsRSHIFT) {
                pctks.ShockButtonL = 255;
            }
            if (c == rsRCTRL) {
                pctks.ShockButtonR = 255;
            }
        }
    }

    return rsEVENTPROCESSED;
}

// 0x7443C0
static RsEventStatus
HandleKeyUp(RsKeyStatus* keyStatus) {
    CPad* pad0 = CPad::GetPad(0);
    CPad* pad1 = CPad::GetPad(1);

    RwInt32 c = keyStatus->keyScanCode; // NOTSA: Use keyScanCode instead of keyCharCode

    if (c != rsNULL) {
        switch (c) {
        case rsESC: {
            CPad::TempKeyState.esc = 0;
            break;
        }

        case rsINS: {
            CPad::TempKeyState.insert = 0;
            break;
        }

        case rsDEL: {
            CPad::TempKeyState.del = 0;
            break;
        }

        case rsHOME: {
            CPad::TempKeyState.home = 0;
            break;
        }

        case rsEND: {
            CPad::TempKeyState.end = 0;
            break;
        }

        case rsPGUP: {
            CPad::TempKeyState.pgup = 0;
            break;
        }

        case rsPGDN: {
            CPad::TempKeyState.pgdn = 0;
            break;
        }

        case rsUP: {
            CPad::TempKeyState.up = 0;
            break;
        }

        case rsDOWN: {
            CPad::TempKeyState.down = 0;
            break;
        }

        case rsLEFT: {
            CPad::TempKeyState.left = 0;
            break;
        }

        case rsRIGHT: {
            CPad::TempKeyState.right = 0;
            break;
        }

        case rsNUMLOCK: {
            CPad::TempKeyState.numlock = 0;
            break;
        }

        case rsPADDEL: {
            CPad::TempKeyState.decimal = 0;
            break;
        }

        case rsPADEND: {
            CPad::TempKeyState.num1 = 0;
            break;
        }

        case rsPADDOWN: {
            CPad::TempKeyState.num2 = 0;
            break;
        }

        case rsPADPGDN: {
            CPad::TempKeyState.num3 = 0;
            break;
        }

        case rsPADLEFT: {
            CPad::TempKeyState.num4 = 0;
            break;
        }

        case rsPAD5: {
            CPad::TempKeyState.num5 = 0;
            break;
        }

        case rsPADRIGHT: {
            CPad::TempKeyState.num6 = 0;
            break;
        }

        case rsPADHOME: {
            CPad::TempKeyState.num7 = 0;
            break;
        }

        case rsPADUP: {
            CPad::TempKeyState.num8 = 0;
            break;
        }

        case rsPADPGUP: {
            CPad::TempKeyState.num9 = 0;
            break;
        }

        case rsPADINS: {
            CPad::TempKeyState.num0 = 0;
            break;
        }

        case rsDIVIDE: {
            CPad::TempKeyState.div = 0;
            break;
        }

        case rsTIMES: {
            CPad::TempKeyState.mul = 0;
            break;
        }

        case rsMINUS: {
            CPad::TempKeyState.sub = 0;
            break;
        }

        case rsPADENTER: {
            CPad::TempKeyState.enter = 0;
            break;
        }

        case rsPLUS: {
            CPad::TempKeyState.add = 0;
            break;
        }

        case rsENTER: {
            CPad::TempKeyState.extenter = 0;
            break;
        }

        case rsSCROLL: {
            CPad::TempKeyState.scroll = 0;
            break;
        }

        case rsPAUSE: {
            CPad::TempKeyState.pause = 0;
            break;
        }

        case rsBACKSP: {
            CPad::TempKeyState.back = 0;
            break;
        }

        case rsTAB: {
            CPad::TempKeyState.tab = 0;
            break;
        }

        case rsCAPSLK: {
            CPad::TempKeyState.capslock = 0;
            break;
        }

        case rsLSHIFT: {
            CPad::TempKeyState.lshift = 0;
            break;
        }

        case rsSHIFT: {
            CPad::TempKeyState.shift = 0;
            break;
        }

        case rsRSHIFT: {
            CPad::TempKeyState.rshift = 0;
            break;
        }

        case rsLCTRL: {
            CPad::TempKeyState.lctrl = 0;
            break;
        }

        case rsRCTRL: {
            CPad::TempKeyState.rctrl = 0;
            break;
        }

        case rsLALT: {
            CPad::TempKeyState.lalt = 0;
            break;
        }

        case rsRALT: {
            CPad::TempKeyState.ralt = 0;
            break;
        }

        case rsLWIN: {
            CPad::TempKeyState.lwin = 0;
            break;
        }

        case rsRWIN: {
            CPad::TempKeyState.rwin = 0;
            break;
        }

        case rsAPPS: {
            CPad::TempKeyState.apps = 0;
            break;
        }

        case rsF1:
        case rsF2:
        case rsF3:
        case rsF4:
        case rsF5:
        case rsF6:
        case rsF7:
        case rsF8:
        case rsF9:
        case rsF10:
        case rsF11:
        case rsF12: {
            CPad::TempKeyState.FKeys[c - rsF1] = 0;
            break;
        }

        default: {
            if (c < 255) {
                CPad::TempKeyState.standardKeys[c] = 0;
            }
            break;
        }
        }

        if (CPad::padNumber != 0) // NOTSA: Use CPad::padNumber instead of CPad::m_bMapPadOneToPadTwo
        {
            auto& pctks = pad1->PCTempKeyState; // NOTSA: Use pctks for brevity
            if (c == 'D') {
                pctks.LeftStickX = 0;
            }
            if (c == 'A') {
                pctks.LeftStickX = 0;
            }
            if (c == 'W') {
                pctks.LeftStickY = 0;
            }
            if (c == 'S') {
                pctks.LeftStickY = 0;
            }
            if (c == 'J') {
                pctks.RightStickX = 0;
            }
            if (c == 'G') {
                pctks.RightStickX = 0;
            }
            if (c == 'Y') {
                pctks.RightStickY = 0;
            }
            if (c == 'H') {
                pctks.RightStickY = 0;
            }
            if (c == 'Z') {
                pctks.LeftShoulder1 = 0;
            }
            if (c == 'X') {
                pctks.LeftShoulder2 = 0;
            }
            if (c == 'C') {
                pctks.RightShoulder1 = 0;
            }
            if (c == 'V') {
                pctks.RightShoulder2 = 0;
            }
            if (c == 'O') {
                pctks.DPadUp = 0;
            }
            if (c == 'L') {
                pctks.DPadDown = 0;
            }
            if (c == 'K') {
                pctks.DPadLeft = 0;
            }
            if (c == ';') {
                pctks.DPadRight = 0;
            }
            if (c == 'B') {
                pctks.Start = 0;
            }
            if (c == 'N') {
                pctks.Select = 0;
            }
            if (c == 'M') {
                pctks.ButtonSquare = 0;
            }
            if (c == ',') {
                pctks.ButtonTriangle = 0;
            }
            if (c == '.') {
                pctks.ButtonCross = 0;
            }
            if (c == '/') {
                pctks.ButtonCircle = 0;
            }
            if (c == rsRSHIFT) {
                pctks.ShockButtonL = 0;
            }
            if (c == rsRCTRL) {
                pctks.ShockButtonR = 0;
            }
        }
    }

    return rsEVENTPROCESSED;
}

/*
 *****************************************************************************
 */
// 0x744880
static RsEventStatus
KeyboardHandler(RsEvent event, void* param) {
    /*
     * ...then the application events, if necessary...
     */
    switch (event) {
    case rsKEYDOWN: {
        return HandleKeyDown((RsKeyStatus*)param);
    }

    case rsKEYUP: {
        return HandleKeyUp((RsKeyStatus*)param);
    }

    default: {
        return rsEVENTNOTPROCESSED;
    }
    }
}

/*
 *****************************************************************************
 */
// 0x7448B0
static RsEventStatus
HandlePadButtonDown(RsPadButtonStatus* padButtonStatus) {
    bool  bPadTwo   = false;
    int32 padNumber = padButtonStatus->padID;

    CPad* pad = CPad::GetPad(padNumber);

    if (CPad::padNumber != 0) { // NOTSA: Use CPad::padNumber instead of CPad::m_bMapPadOneToPadTwo
        padNumber = 1;
    }

    if (padNumber == 1) {
        bPadTwo = true;
    }

    ControlsManager.UpdateJoyButtonState(padNumber);

    for (int32 i = 0; i < 16; i++) // NOTSA: Use 16 instead of _TODOCONST(16)
    {
        RsPadButtons btn = (RsPadButtons)0;
        if (ControlsManager.m_ButtonStates[i] == true) {
            btn = (RsPadButtons)(i + 1);

            if (FrontEndMenuManager.m_bMenuActive || bPadTwo) {
                ControlsManager.UpdateJoyInConfigMenus_ButtonDown(btn, padNumber);
            } else {
                ControlsManager.AffectControllerStateOn_ButtonDown((KeyCode)btn, eControllerType::JOY_STICK); // NOTSA: Use enum
            }
        }

    }
    return rsEVENTPROCESSED;
}

    /*
 *****************************************************************************
 */
// 0x744930
static RsEventStatus
HandlePadButtonUp(RsPadButtonStatus * padButtonStatus) {
    bool  bPadTwo   = false;
    int32 padNumber = padButtonStatus->padID;

    CPad* pad = CPad::GetPad(padNumber);

    if (CPad::padNumber != 0) { // NOTSA: Use CPad::padNumber instead of CPad::m_bMapPadOneToPadTwo
        padNumber = 1;
    }

    if (padNumber == 1) {
        bPadTwo = true;
    }

    bool  bCam = false;
    int16 mode = TheCamera.m_aCams[TheCamera.m_nActiveCam].m_nMode;
    if (mode == MODE_FLYBY || mode == MODE_FIXED) {
        bCam = true;
    }

    ControlsManager.UpdateJoyButtonState(padNumber);

    for (int32 i = 1; i < 16; i++) // NOTSA: Use 16 instead of _TODOCONST(16)
    {
        RsPadButtons btn = (RsPadButtons)0;
        if (ControlsManager.m_ButtonStates[i] == false) {
            btn = (RsPadButtons)(i + 1); // bug ?, cycle begins from 1(not zero), 1+1==2==rsPADBUTTON2, so we skip rsPADBUTTON1, right ?

            if (FrontEndMenuManager.m_bMenuActive || bPadTwo || bCam) {
                ControlsManager.UpdateJoyInConfigMenus_ButtonUp(btn, padNumber);
            } else {
                ControlsManager.AffectControllerStateOn_ButtonUp((KeyCode)btn, eControllerType::JOY_STICK); // NOTSA: Use enum
            }
        }

    }
    return rsEVENTPROCESSED;
}
    /*
*****************************************************************************
*/
// 0x7449F0
static RsEventStatus
PadHandler(RsEvent event, void* param) {
    switch (event) {
    case rsPADBUTTONDOWN: {
        return HandlePadButtonDown((RsPadButtonStatus*)param);
    }

    case rsPADBUTTONUP: {
        return HandlePadButtonUp((RsPadButtonStatus*)param);
    }

    default: {
        return rsEVENTNOTPROCESSED;
    }
    }
}

/*
*****************************************************************************
*/
// 0x744A20
bool AttachInputDevices() {
    RsInputDeviceAttach(rsKEYBOARD, KeyboardHandler);
    RsInputDeviceAttach(rsPAD, PadHandler);
    return true;
}
