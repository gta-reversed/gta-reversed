/*
* This file contains WndProc and related functions
*/

#include "StdInc.h"

#include "imgui_impl_win32.h"

#include <windows.h>
#include <rwplcore.h>
#include <Dbt.h>
#include <dshow.h>

#include <platform/platform.h>
#include "PostEffects.h"
#include "AEAudioHardware.h"
#include "VideoMode.h"
#include "VideoPlayer.h"
#include "Input.h"
#include "Gamma.h"

// Dear ImGui said I have to copy this here
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

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

/*!
* Process shift keys.
* Unless Win98, in which case `GTATranslateKey` should handle it.
* @addr 0x747CD0
*/
BOOL GTATranslateShiftKey(RsKeyCodes*) { // The in keycode is ignored, so we won't bother
    if (s_OSStatus.OSVer == WinVer::WIN_98) {
        return false; // Already handled by `GTATranslateKey`
    }

    constexpr struct { RsKeyCodes ck; INT vk; } Keys[]{
        {rsLSHIFT, VK_LSHIFT},
        {rsRSHIFT, VK_RSHIFT},
    };

    for (auto shouldBeDown : { false, true }) {
        for (auto [ck, vk] : Keys) {
            // GetKeyState reads from the message queue,
            // so we must call it like the og code
            const auto isDown = (HIWORD(GetKeyState(vk)) & 0x80) == 1; // Check is key pressed
            if (isDown == shouldBeDown) {
                RsEventHandler(
                    isDown ? rsKEYDOWN : rsKEYUP,
                    &ck
                );
            }
        }
    }

    return true;
}


// 0x747EB0
LRESULT CALLBACK __MainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    const auto imio = ImGui::GetCurrentContext() ? &ImGui::GetIO() : nullptr;

    if (imio) {
        if (imio->MouseDrawCursor) {
            imio->ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
        } else {
            imio->ConfigFlags |= ImGuiConfigFlags_NoMouse;
        }
    }

    if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam)) {
        return true;
    }

    switch (uMsg) {
    case WM_SETCURSOR: {
        ShowCursor(false);
        SetCursor(NULL);
        break;
    }
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN: 
    case WM_KEYUP:
    case WM_SYSKEYUP: { //< 0x74823B - wParam is a `VK_` (virtual key), lParam are the flags (See https://learn.microsoft.com/en-us/windows/win32/inputdev/wm-keyup)]
        if (RsKeyCodes ck; GTATranslateKey(&ck, lParam, wParam)) {
            RsKeyboardEventHandler(
                (uMsg == WM_KEYDOWN || uMsg == WM_SYSKEYDOWN)
                    ? rsKEYDOWN
                    : rsKEYUP,
                &ck
            );
        }
        if (wParam == VK_SHIFT) {
            RsKeyCodes ck;
            GTATranslateShiftKey(&ck); // Original code uses this variable for storage, so we can't pass in nullptr - TODO: Remove parameter
        }
        return 0;
    }
    case WM_MOUSEMOVE: { //< 0x748323
        FrontEndMenuManager.m_nMousePosWinX = GET_X_LPARAM(lParam);
        FrontEndMenuManager.m_nMousePosWinY = GET_Y_LPARAM(lParam);
        break;
    }
    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_MBUTTONDOWN: {
        SetCapture(hWnd);
        return 0;
    }
    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
    case WM_MBUTTONUP: {
        ReleaseCapture();
        return 0;
    }
    case WM_MOUSEWHEEL:
        return 0;
    case WM_SIZING: { // 0x74829E
        if (RwInitialized) {
            if (gGameState == GAME_STATE_IDLE) {
                RsEventHandler(rsIDLE, (void*)TRUE);
            }
        }

        const auto wndrect = reinterpret_cast<PRECT>(lParam);
        VERIFY(SetWindowPos(
            hWnd,
            HWND_TOP,
            0,                              0,
            wndrect->right - wndrect->left, wndrect->bottom - wndrect->top,
            SWP_NOSIZE | SWP_NOMOVE
        ));

        CPostEffects::SetupBackBufferVertex();

        return 0;
    }
    case WM_ACTIVATEAPP: { // 0x748087
        const auto wndBeingActivated = !!wParam;

        //> 0x748087 - Set gamma (If changed)
        if (gbGammaChanged) {
            if (const auto dev = RwD3D9GetCurrentD3DDevice()) {
                dev->SetGammaRamp(9, 0, wndBeingActivated ? &CGamma::ms_GammaTable : &CGamma::ms_SavedGamma);
            }
        }

        //
        switch (gGameState) {
        case GAME_STATE_PLAYING_LOGO:
        case GAME_STATE_PLAYING_INTRO: {
            const auto mc = VideoPlayer::GetMediaControl();

            OAFilterState state;
            VERIFY(SUCCEEDED(mc->GetState(10, &state)));
            if (wndBeingActivated) {
                VideoPlayer::UpdateWindow();
                if (state == State_Running) {
                    break;
                }
                VERIFY(SUCCEEDED(mc->Run()));
                VideoPlayer::WaitState(State_Running);
                SetFocus(PSGLOBAL(window));
            } else if (state == State_Running) { // Window is being deactivatd, pause media
                VERIFY(SUCCEEDED(mc->Pause()));
            }

            break;
        }
        case GAME_STATE_INITIAL: {
            if (!wndBeingActivated) { // 0x748171
                if (PSGLOBAL(fullScreen)) {
                    Windowed = true; // ???
                }
            }
            break;
        }
        }

        //> 0x748183 Clear pads
        for (auto& pad : CPad::Pads) {
            pad.Clear(false, true);
        }
        
        return 0;
    }
    case WM_DESTROY:
    case WM_CLOSE: { // 0x747EF3
        VERIFY(ClipCursor(nullptr));
        VERIFY(SUCCEEDED(WinInput::Shutdown()));
        PostQuitMessage(0);
        return 0;
    }
    case WM_SIZE: { // 0x747F04
        // Figure out new size
        const auto width = LOWORD(lParam), height = HIWORD(lParam);
        RwRect wndsz{
            .x = 0,     .y = 0,
            .w = width, .h = height, 
        };

        // Nothing to do?
        if (!RwInitialized || !wndsz.h || !wndsz.w) {
            return 0;
        }

        // Try resizing the camera
        RsEventHandler(rsCAMERASIZE, &wndsz);

        // Check if it has failed
        if (wndsz.w != width && wndsz.h != height) { // TODO/BUG: Shouldnt `&&` instead be `||`?
            VERIFY(ReleaseCapture());

            WINDOWPLACEMENT wndpl;
            VERIFY(GetWindowPlacement(PSGLOBAL(window), &wndpl));

            if (wndpl.showCmd == SW_SHOWMAXIMIZED) {
                SendMessage(PSGLOBAL(window), WM_WINDOWPOSCHANGED, 0, 0);
            }
        }

        return 0;
    }
    case WM_ACTIVATE: { // 0x747FA3
        const auto wndBeingActivated = !!wParam;

        if (wndBeingActivated) {
            CAudioEngine::ResumeAllSounds();
            ShowCursor(FALSE);
        } else {
            CAudioEngine::PauseAllSounds();
            VERIFY(SUCCEEDED(SetTimer(PSGLOBAL(window), 1, (UINT)(1000.f / RsGlobal.frameLimit), nullptr)));
        }

        ForegroundApp = wndBeingActivated;
        RsEventHandler(rsACTIVATE, (void*)wndBeingActivated);

        //> 0x74800C Clear pads
        for (auto& pad : CPad::Pads) {
            pad.Clear(false, true);
        }

#ifdef FIX_BUGS
        break;
#else
        return 0;
#endif
    }
    case WM_SETFOCUS: { // 0x748063
        ForegroundApp = true;
        if (!FrontEndMenuManager.m_bMainMenuSwitch && !FrontEndMenuManager.m_bMenuActive) { // OnSetFocus
            FrontEndMenuManager.m_bActivateMenuNextFrame = true;
        }
#ifdef FIX_BUGS
        break;
#else
        return 0;
#endif
    }
    case WM_KILLFOCUS: { // 0x748054
        ForegroundApp = false;
#ifdef FIX_BUGS
        break;
#else
        return 0;
#endif
    }
    case WM_GRAPHNOTIFY: { //< 0x74842A - Dispatched from VideoPlayer::Play
        switch (gGameState) {
        case GAME_STATE_PLAYING_INTRO:
        case GAME_STATE_PLAYING_LOGO: {
            VideoPlayer::OnGraphNotify();
            break;
        }
        }
#ifdef FIX_BUGS
        break;
#else
        return 0;
#endif
    }
    case WM_DEVICECHANGE: { //> 0x748282 - Handle AudioHardware DVD removal
        if (wParam != DBT_DEVICEREMOVECOMPLETE) {
            break;
        }
        const auto eparams = reinterpret_cast<PDEV_BROADCAST_HDR>(lParam);
        if (eparams->dbch_devicetype != DBT_DEVTYP_VOLUME) {
            break;
        }
        const auto idev = reinterpret_cast<PDEV_BROADCAST_VOLUME>(lParam);
        if ((idev->dbcv_flags & DBTF_MEDIA) == 0) { // Not a media drive?
            break;
        }
        if (!AEAudioHardware.m_bInitialised || !AEAudioHardware.IsStreamingFromDVD()) {
            break;
        }
        const auto dvletter = AEAudioHardware.GetDVDDriveLetter();
        if (dvletter < 'A' || (idev->dbcv_unitmask & (1 << dvletter)) == 0) {
            break;
        }
        DEV_LOG("About to check CD drive");
        CTimer::SetCodePause(true);
        if (CCutsceneMgr::IsRunning()) {
            CCutsceneMgr::SkipCutscene();
        }
        while (!AEAudioHardware.CheckDVD()) {
            FrontEndMenuManager.NoDiskInDriveMessage();
            if (FrontEndMenuManager.m_bQuitGameNoDVD) {
                DEV_LOG("Exiting game as Audio CD was not inserted");
                break;
            }
        }
        DEV_LOG("GTA Audio DVD has been inserted");
        CTimer::SetCodePause(false);
        break;
    }
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void InjectHooksWndProcStuff() {
    RH_ScopedCategory("Win");
    RH_ScopedNamespaceName("Win");

    RH_ScopedGlobalInstall(GTATranslateShiftKey, 0x747CD0);
    RH_ScopedGlobalInstall(GTATranslateKey, 0x747820);
    RH_ScopedGlobalInstall(__MainWndProc, 0x747EB0, {.locked = true}); // Locked because of ImGui
}
