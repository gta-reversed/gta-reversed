/*
* This file contains WndProc and related functions
*/

#include "StdInc.h"

#ifndef NOTSA_USE_SDL3
#include <libs/imgui/bindings/imgui_impl_win32.h>

#include "winincl.h"
#include <rwplcore.h>
#include <Dbt.h>
#include <dshow.h>

#include <platform/platform.h>
#include "PostEffects.h"
#include "AEAudioHardware.h"
#include "VideoMode.h"
#include "VideoPlayer.h"
#include "WinInput.h"
#include "WinPlatform.h"
#include "Gamma.h"

// Dear ImGui said I have to copy this here
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

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
            if (const auto dev = (IDirect3DDevice9*)RwD3D9GetCurrentD3DDevice()) {
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
        NOTSA_LOG_DEBUG("About to check CD drive");
        CTimer::SetCodePause(true);
        if (CCutsceneMgr::IsRunning()) {
            CCutsceneMgr::SkipCutscene();
        }
        while (!AEAudioHardware.CheckDVD()) {
            FrontEndMenuManager.NoDiskInDriveMessage();
            if (FrontEndMenuManager.m_bQuitGameNoDVD) {
                NOTSA_LOG_DEBUG("Exiting game as Audio CD was not inserted");
                break;
            }
        }
        NOTSA_LOG_DEBUG("GTA Audio DVD has been inserted");
        CTimer::SetCodePause(false);
        break;
    }
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void InjectHooksWndProcStuff() {
    RH_ScopedCategory("Win");
    RH_ScopedNamespaceName("Win");

    RH_ScopedGlobalInstall(__MainWndProc, 0x747EB0, {.locked = true}); // Locked because of ImGui
}
#endif
