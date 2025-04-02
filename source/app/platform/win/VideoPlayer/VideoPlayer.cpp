#include "StdInc.h"

#include "VideoPlayer.h"

#include "dshow.h"
#pragma comment(lib, "strmiids.lib") // Class identifiers (CLSIDs) and interface identifiers (IIDs).

namespace VideoPlayer {

IGraphBuilder*& pvGraphBuilder = *(IGraphBuilder**)0xC920D4;
IMediaControl*& pvMediaControl = *(IMediaControl**)0xC920D8;
IMediaEventEx*& pvMediaEvent   = *(IMediaEventEx**)0xC920DC;
IVideoWindow*&  pvVideoWindow  = *(IVideoWindow**)0xC920E0;
IMediaSeeking*& pvMediaSeeking = *(IMediaSeeking**)0xC920E4;

void InjectHooks() {
    RH_ScopedNamespace(VideoPlayer);
    RH_ScopedCategory("App");

    RH_ScopedInstall(Shutdown, 0x7467B0);
    RH_ScopedInstall(WaitState, 0x745F90);
    RH_ScopedInstall(OnGraphNotify, 0x745FB0);
    RH_ScopedInstall(UpdateWindow, 0x7466D0);
    RH_ScopedInstall(Release, 0x746740);
    RH_ScopedInstall(Play, 0x747660);
}

// 0x746740
void Release() {
#define RELEASE(p) if (p) { (p)->Release(); }  (p) = nullptr;

    RELEASE(pvMediaEvent);
    RELEASE(pvMediaSeeking);
    RELEASE(pvMediaControl);
    RELEASE(pvVideoWindow);
    RELEASE(pvGraphBuilder);

#undef RELEASE
}

// 0x7467B0
void Shutdown() {
    if (pvMediaControl) {
        pvMediaControl->Stop();
    }
    Release();
}

// 0x745F90
void WaitState(int32 state) {
    OAFilterState curState;
    do {
        pvMediaControl->GetState(10, &curState);
    }
    while (state != curState);
}

// 0x745FB0
void OnGraphNotify() {
    LONG code = 0;
    LONG_PTR param1 = 0, param2 = 0;

    while (!FAILED(pvMediaEvent->GetEvent(&code, &param1, &param2, 0))) {
        pvMediaEvent->FreeEventParams(code, param1, param2);
        if (code != EC_COMPLETE) {
            continue;
        }
        // Possibly advance game state
        ChangeGameStateTo([] {
            switch (gGameState) {
            case GAME_STATE_PLAYING_LOGO:  return GAME_STATE_TITLE;
            case GAME_STATE_PLAYING_INTRO: return GAME_STATE_FRONTEND_LOADING;
            }
            return (eGameState)gGameState; // No change
        }());
        pvMediaEvent->SetNotifyWindow(NULL, 0, 0);
    }
}

// 0x7466D0
void UpdateWindow() {
    RECT rect;
    GetClientRect(PSGLOBAL(window), &rect);
    HRESULT hr;

    if (FAILED(hr = pvVideoWindow->SetWindowPosition(rect.left, rect.top, rect.right, rect.bottom))) {
        NOTSA_LOG_DEBUG("FAILED(hr=0x{:x}) in pvVideoWindow->SetWindowPosition() failed/n", hr);
        return;
    }

    if (FAILED(hr = pvVideoWindow->put_MessageDrain((OAHWND) PSGLOBAL(window)))) {
        NOTSA_LOG_DEBUG("FAILED(hr=0x{:x}) in (pvVideoWindow->put_MessageDrain((OAHWND) PSGLOBAL(window)))\n", hr);
        return;
    }

    SetFocus(PSGLOBAL(window));
}

// 0x747660
void Play(int32 nCmdShow, const char* path) {
    UpdateWindow(PSGLOBAL(window));

#ifdef FIX_BUGS
    const auto size = MultiByteToWideChar(0, 0, path, -1, nullptr, 0);
    WCHAR* fileName = new WCHAR[size];
    MultiByteToWideChar(0, 0, path, -1, fileName, size);
#else
    // Buffer overflow may happen in MultiByteToWideChar.
    WCHAR fileName[256] = { 0 };
    MultiByteToWideChar(0, 0, path, -1, fileName, sizeof(fileName) - 1);
#endif
    HRESULT hr;

    if (FAILED(hr = CoInitialize(nullptr))) {
        NOTSA_LOG_DEBUG("FAILED(hr=0x{:x}) in CoInitialize(NULL)\n", hr);
        return;
    }

    if (FAILED(hr = CoCreateInstance(CLSID_FilterGraph, nullptr, CLSCTX_INPROC, IID_IGraphBuilder, (void**)&pvGraphBuilder))) {
        NOTSA_LOG_DEBUG("FAILED(hr=0x{:x}) in CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC, IID_IGraphBuilder, (void **)&pvGraphBuilder)\n", hr);
        return;
    }

    if (FAILED(hr = pvGraphBuilder->RenderFile(&fileName[0], nullptr))) {
        NOTSA_LOG_DEBUG("FAILED(hr=0x{:x}) in pvGraphBuilder->RenderFile(&wFileName[0], NULL)\n", hr);
        return;
    }

    if (FAILED(hr = pvGraphBuilder->QueryInterface(IID_IMediaControl, (void**)&pvMediaControl))) {
        NOTSA_LOG_DEBUG("FAILED(hr=0x{:x}) in pvGraphBuilder->QueryInterface(IID_IMediaControl, (void **)&pvMediaControl)\n", hr);
        return;
    }

    if (FAILED(hr = pvGraphBuilder->QueryInterface(IID_IMediaEventEx, (void**)&pvMediaEvent))) {
        NOTSA_LOG_DEBUG("FAILED(hr=0x{:x}) in pvGraphBuilder->QueryInterface(IID_IMediaEventEx, (void **)&pvMediaEvent)\n", hr);
        return;
    }

    if (FAILED(hr = pvGraphBuilder->QueryInterface(IID_IMediaSeeking, (void**)&pvMediaSeeking))) {
        NOTSA_LOG_DEBUG("FAILED(hr=0x{:x}) in pvGraphBuilder->QueryInterface(IID_IMediaSeeking, (void **)&pMS)\n", hr);
        return;
    }

    if (FAILED(hr = pvGraphBuilder->QueryInterface(IID_IVideoWindow, (void**)&pvVideoWindow))) {
        NOTSA_LOG_DEBUG("FAILED(hr=0x{:x}) in pvGraphBuilder->QueryInterface(IID_IVideoWindow, (void **)&pvVideoWindow)\n", hr);
        return;
    }

    if (FAILED(hr = pvVideoWindow->put_Owner((OAHWND)PSGLOBAL(window)))) {
        NOTSA_LOG_DEBUG("FAILED(hr=0x{:x}) in pvVideoWindow->put_Owner((OAHWND) PSGLOBAL(window))\n", hr);
        return;
    }

    if (FAILED(hr = pvVideoWindow->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN))) {
        NOTSA_LOG_DEBUG("FAILED(hr=0x{:x}) in pvVideoWindow->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN)\n", hr);
        return;
    }

    if (FAILED(hr = pvMediaEvent->SetNotifyWindow((OAHWND)PSGLOBAL(window), WM_GRAPHNOTIFY, 0))) {
        NOTSA_LOG_DEBUG("FAILED(hr=0x{:x}) in pvMediaEvent->SetNotifyWindow((OAHWND)PSGLOBAL(window), WM_GRAPHNOTIFY, 0)\n", hr);
        return;
    }

    UpdateWindow();

    if (FAILED(hr = pvMediaControl->Run())) {
        NOTSA_LOG_DEBUG("FAILED(hr=0x{:x}) in pvMediaControl->Run()\n", hr);
        return;
    }

    SetFocus(PSGLOBAL(window));

#ifdef FIX_BUGS
    delete[] fileName;
#endif
};

auto GetMediaControl() -> IMediaControl* {
    return pvMediaControl;
}

} // namespace VideoPlayer
