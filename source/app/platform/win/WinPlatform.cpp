/*
* This file contains ps* function implementations for Windows
*/

#include "StdInc.h"

#include "WinPlatform.h"
#include "WndProc.h"
#include "WinMain.h"
#include "WinInput.h"
#include "Gamma.h"

#define SHIFTED 0x8000
#define KEY_DOWN(keystate) ((keystate & SHIFTED) != 0)
#define KEY_UP(keystate) ((keystate & SHIFTED) == 0)

// @notsa
// @brief Resets the screen gamma if ever changed.
void ResetGammaWhenExiting() {
    if (gbGammaChanged) {
        if (auto d3dDevice = (IDirect3DDevice9*)RwD3D9GetCurrentD3DDevice()) {
            d3dDevice->SetGammaRamp(0u, D3DSGR_CALIBRATE, &CGamma::ms_SavedGamma);
        }
        gbGammaChanged = false;
    }
}

/*!
* Process shift keys.
* Unless Win98, in which case `GTATranslateKey` should handle it.
* @addr 0x747CD0
*/
BOOL GTATranslateShiftKey(RsKeyCodes*) { // The in keycode is ignored, so we won't bother
    if (s_OSStatus.OSVer == WinVer::WIN_98) {
        return false; // Already handled by `GTATranslateKey`
    }

    RsKeyCodes keyCode = rsNULL;
    if (KEY_DOWN(GetKeyState(VK_LSHIFT))) {
        keyCode = rsLSHIFT;
        RsKeyboardEventHandler(rsKEYDOWN, &keyCode);
    }
    if (KEY_DOWN(GetKeyState(VK_RSHIFT))) {
        keyCode = rsRSHIFT;
        RsKeyboardEventHandler(rsKEYDOWN, &keyCode);
    }
    if (KEY_UP(GetKeyState(VK_LSHIFT))) {
        keyCode = rsLSHIFT;
        RsKeyboardEventHandler(rsKEYUP, &keyCode);
    }
    if (KEY_UP(GetKeyState(VK_RSHIFT))) {
        keyCode = rsRSHIFT;
        RsKeyboardEventHandler(rsKEYUP, &keyCode);
    }

    return true;
}

// 0x747820
BOOL GTATranslateKey(RsKeyCodes* ck, LPARAM lParam, UINT vk) {
    *ck = [&] {
        // Handle extended keys
        const auto Ext = [kf = HIWORD(lParam)](RsKeyCodes extended, RsKeyCodes unextended) {
            return (kf & KF_EXTENDED) ? extended : unextended;
        };

        switch (vk) {
        case VK_RETURN:     return Ext(rsPADENTER, rsENTER);
        case VK_CONTROL:    return Ext(rsRCTRL, rsLCTRL);
        case VK_MENU:       return Ext(rsRALT, rsLALT);
        case VK_PRIOR:      return Ext(rsPGUP, rsPADPGUP);
        case VK_NEXT:       return Ext(rsPGDN, rsPADPGDN);
        case VK_END:        return Ext(rsEND, rsPADEND);
        case VK_HOME:       return Ext(rsHOME, rsPADHOME);
        case VK_LEFT:       return Ext(rsLEFT, rsPADLEFT);
        case VK_UP:         return Ext(rsUP, rsPADUP);
        case VK_RIGHT:      return Ext(rsRIGHT, rsPADRIGHT);
        case VK_DOWN:       return Ext(rsDOWN, rsPADDOWN);
        case VK_INSERT:     return Ext(rsINS, rsPADINS);
        case VK_DELETE:     return Ext(rsDEL, rsPADDEL);
        case VK_BACK:       return rsBACKSP;
        case VK_TAB:        return rsTAB;
        case VK_PAUSE:      return rsPAUSE;
        case VK_CAPITAL:    return rsCAPSLK;
        case VK_ESCAPE:     return rsESC;
        case VK_LWIN:       return rsLWIN;
        case VK_RWIN:       return rsRWIN;
        case VK_APPS:       return rsAPPS;
        case VK_NUMPAD0:    return rsPADINS;
        case VK_NUMPAD1:    return rsPADEND;
        case VK_NUMPAD2:    return rsPADDOWN;
        case VK_NUMPAD3:    return rsPADPGDN;
        case VK_NUMPAD4:    return rsPADLEFT;
        case VK_NUMPAD5:    return rsPAD5;
        case VK_NUMPAD6:    return rsPADRIGHT;
        case VK_NUMPAD7:    return rsPADHOME;
        case VK_NUMPAD8:    return rsPADUP;
        case VK_NUMPAD9:    return rsPADPGUP;
        case VK_MULTIPLY:   return rsTIMES;
        case VK_ADD:        return rsPLUS;
        case VK_SUBTRACT:   return rsMINUS;
        case VK_DECIMAL:    return rsPADDEL;
        case VK_DIVIDE:     return rsDIVIDE;
        case VK_F1:         return rsF1;
        case VK_F2:         return rsF2;
        case VK_F3:         return rsF3;
        case VK_F4:         return rsF4;
        case VK_F5:         return rsF5;
        case VK_F6:         return rsF6;
        case VK_F7:         return rsF7;
        case VK_F8:         return rsF8;
        case VK_F9:         return rsF9;
        case VK_F10:        return rsF10;
        case VK_F11:        return rsF11;
        case VK_F12:        return rsF12;
        case VK_NUMLOCK:    return rsNUMLOCK;
        case VK_SCROLL:     return rsSCROLL;
        case VK_SHIFT: {
            return s_OSStatus.OSVer == WinVer::WIN_98 // Will be handled later
                ? rsSHIFT
                : rsNULL;
        }
        default: { // Try mapping to regular ASCII char
            const auto chr = MapVirtualKey(vk, MAPVK_VK_TO_CHAR);
            if (chr <= 0xFF) {
                return (RsKeyCodes)(chr);
            }
            break;
        }
        }
        return rsNULL;
        }();
    return *ck != rsNULL;
}

void WinPsInjectHooks();
void Win32InjectHooks() {
    RH_ScopedCategory("Win");
    RH_ScopedNamespaceName("Win");

    RH_ScopedGlobalInstall(GTATranslateShiftKey, 0x747CD0);
    RH_ScopedGlobalInstall(GTATranslateKey, 0x747820);

#ifndef NOTSA_USE_SDL3
    InjectHooksWndProcStuff();
    WinInput::InjectHooks();
#endif
    InjectWinMainStuff();
    WinPsInjectHooks();
}
