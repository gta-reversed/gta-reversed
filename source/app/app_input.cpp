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

// 0x744A20
bool AttachInputDevices() {
    RsInputDeviceAttach(rsKEYBOARD, KeyboardHandler);
    RsInputDeviceAttach(rsPAD, PadHandler);
    return true;
}

// Combined code from `0x743DF0` and `0x7443C0`
RsEventStatus HandleKeyEvent(bool isDown, RsKeyStatus* ks) {
    // First handle keyboard keys
    const auto pKBKeyState = [ks]()  -> int16* {
        auto& tks = CPad::TempKeyState;
        switch (ks->keyScanCode) {
        case rsNULL:     return nullptr;
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
        case rsF12:      return &tks.FKeys[ks->keyScanCode - rsF1];
        case rsESC:      return &tks.esc;
        case rsINS:      return &tks.insert;
        case rsDEL:      return &tks.del;
        case rsHOME:     return &tks.home;
        case rsEND:      return &tks.end;
        case rsPGUP:     return &tks.pgup;
        case rsPGDN:     return &tks.pgdn;
        case rsUP:       return &tks.up;
        case rsDOWN:     return &tks.down;
        case rsLEFT:     return &tks.left;
        case rsRIGHT:    return &tks.right;
        case rsDIVIDE:   return &tks.div;
        case rsTIMES:    return &tks.mul;
        case rsPLUS:     return &tks.add;
        case rsMINUS:    return &tks.sub;
        case rsPADDEL:   return &tks.decimal;

        case rsPADEND:   return &tks.num1;
        case rsPADDOWN:  return &tks.num2;
        case rsPADPGDN:  return &tks.num3;
        case rsPADLEFT:  return &tks.num4;
        case rsPAD5:     return &tks.num5;
        case rsNUMLOCK:  return &tks.numlock;
        case rsPADRIGHT: return &tks.num6;
        case rsPADHOME:  return &tks.num7;
        case rsPADUP:    return &tks.num8;
        case rsPADPGUP:  return &tks.num9;
        case rsPADINS:   return &tks.num0;

        case rsPADENTER: return &tks.enter;
        case rsSCROLL:   return &tks.scroll;
        case rsPAUSE:    return &tks.pause;
        case rsBACKSP:   return &tks.back;
        case rsTAB:      return &tks.tab;
        case rsCAPSLK:   return &tks.capslock;
        case rsENTER:    return &tks.extenter;
        case rsLSHIFT:   return &tks.lshift;
        case rsRSHIFT:   return &tks.rshift;
        case rsSHIFT:    return &tks.shift;
        case rsLCTRL:    return &tks.lctrl;
        case rsRCTRL:    return &tks.rctrl;
        case rsLALT:     return &tks.lalt;
        case rsRALT:     return &tks.ralt;
        case rsLWIN:     return &tks.lwin;
        case rsRWIN:     return &tks.rwin;
        case rsAPPS:     return &tks.apps;
        default:         return (ks->keyScanCode < 255) ? &tks.standardKeys[ks->keyScanCode] : nullptr;
        }
    }();
    if (pKBKeyState) {
        *pKBKeyState = isDown ? 255 : 0;
    }

    // Then if a pad is plugged in, possibly handle that as well
    if (CPad::padNumber != 0) {
        const auto [pPadBtnState, downValue] = [&]() -> std::pair<int16*, int16> {
            auto& pctks = CPad::GetPad(1)->PCTempKeyState; // Why pad 1?
            switch (ks->keyScanCode) {
            case 'D':   return { &pctks.LeftStickX,      128 };
            case 'A':   return { &pctks.LeftStickX,     -128 };

            case 'W':   return { &pctks.LeftStickY,      128 };
            case 'S':   return { &pctks.LeftStickY,     -128 };

            case 'J':   return { &pctks.RightStickX,     128 };
            case 'G':   return { &pctks.RightStickX,    -128 };

            case 'Y':   return { &pctks.RightStickY,     128 };
            case 'H':   return { &pctks.RightStickY,    -128 };

            case 'Z':   return { &pctks.LeftShoulder1,   255 };
            case 'X':   return { &pctks.LeftShoulder2,   255 };

            case 'C':   return { &pctks.RightShoulder1,  255 };
            case 'V':   return { &pctks.RightShoulder2,  255 };

            case 'O':   return { &pctks.DPadUp,          255 };
            case 'L':   return { &pctks.DPadDown,        255 };
            case 'K':   return { &pctks.DPadLeft,        255 };
            case ';':   return { &pctks.DPadRight,       255 };

            case 'B':   return { &pctks.Start,           255 };
            case 'N':   return { &pctks.Select,          255 };

            case 'M':   return { &pctks.ButtonSquare,    255 };
            case ',':   return { &pctks.ButtonTriangle,  255 };
            case '.':   return { &pctks.ButtonCross,     255 };
            case '/':   return { &pctks.ButtonCircle,    255 };

            case rsRSHIFT: return { &pctks.ShockButtonL,    255 };
            case rsRCTRL:  return { &pctks.ShockButtonR,    255 };
            }
            return { NULL, NULL };
        }();
        if (pPadBtnState) {
            *pPadBtnState = isDown ? downValue : 0;
        }
    }

    return rsEVENTPROCESSED;
}

// 0x743DF0
RsEventStatus HandleKeyDown(RsKeyStatus* ks) {
    return HandleKeyEvent(true, ks);
}

// 0x7443C0
RsEventStatus HandleKeyUp(RsKeyStatus* ks) {
    return HandleKeyEvent(false, ks);
}

// 0x744880
RsEventStatus KeyboardHandler(RsEvent event, void* param) {
    switch (event) {
    case rsKEYDOWN:
        return HandleKeyDown((RsKeyStatus*)param);
    case rsKEYUP:
        return HandleKeyUp((RsKeyStatus*)param);
    default:
        return rsEVENTNOTPROCESSED;
    }
}

// 0x7448B0
RsEventStatus HandlePadButtonDown(RsKeyStatus* param) {
    ControlsManager.HandleJoyButtonUpDown(CPad::padNumber ? 1 : 0, true);
    return rsEVENTPROCESSED;
}

// 0x744930
RsEventStatus HandlePadButtonUp(RsKeyStatus* param) {
    ControlsManager.HandleJoyButtonUpDown(CPad::padNumber ? 1 : 0, false);
    return rsEVENTPROCESSED;
}

// 0x7449F0
RsEventStatus PadHandler(RsEvent event, void* param) {
    switch (event) {
    case rsPADBUTTONDOWN:
        return HandlePadButtonDown((RsKeyStatus*)param);
    case rsPADBUTTONUP:
        return HandlePadButtonUp((RsKeyStatus*)param);
    default:
        return rsEVENTNOTPROCESSED;
    }
}
