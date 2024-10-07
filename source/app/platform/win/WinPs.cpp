#include "StdInc.h"

#include <platform/win/platform.h>
#include <platform/platform.h>
#include <ddraw.h>
#include <dsound.h>
#include "VideoMode.h"
#include "VideoModeSelectDialog.h"
#include "LoadingScreen.h"
#include "C_PcSave.h"

// NOTE: This macro doesn't do a whole lot. Leaving it here for completeness sake
#define USE_D3D9

static auto& PsGlobal = StaticRef<psGlobalType, 0xC8CF88>();

//! Disable "This function was depracated"
#pragma warning (disable : 28159 4996)

// 0x7455E0 - Get available videomem
HRESULT GetVideoMemInfo(LPDWORD total, LPDWORD available) {
	LPDIRECTDRAW7 dd;

    if (HRESULT hr = DirectDrawCreateEx(NULL, (LPVOID*)&dd, IID_IDirectDraw7, NULL); FAILED(hr)) {
        return hr;
    }

	DDSCAPS2 caps;
	ZeroMemory(&caps, sizeof(DDSCAPS2));
	caps.dwCaps = DDSCAPS_VIDEOMEMORY;
	HRESULT hr = dd->GetAvailableVidMem(&caps, total, available);
	dd->Release();

    return hr;
}

//! Check if D3D9 can be loaded (Originally this checked for versions 7 => 9, but GTA can only run with 9, so... :D
BOOL CheckDirectX() {
    const auto hD3D9DLL = LoadLibrary("D3D9.DLL");
    if (hD3D9DLL == NULL) {
        return FALSE;
    }
    FreeLibrary(hD3D9DLL);
    return TRUE;
}

//! 0x745840 - Check if DirectSound can be loaded
BOOL CheckDirectSound() {
    LPDIRECTSOUND ds;

    if (FAILED(DirectSoundCreate(NULL, &ds, NULL))) {
        return FALSE;
    }

    DSCAPS  caps{ sizeof(DSCAPS) };
    HRESULT hr = ds->GetCaps(&caps);

    ds->Release();

    return SUCCEEDED(hr);
}

// 0x7465B0
void InitialiseLanguage() {
//#pragma warning (disable : 4302) // "Type truncation from HKL to 

    // TODO: Use `GetLocaleInfoEx`
    const auto sysDefaultLCID = PRIMARYLANGID(GetSystemDefaultLCID());
	const auto usrDefaultLCID = PRIMARYLANGID(GetUserDefaultLCID());
    const auto kbLayoutLCID   = PRIMARYLANGID(LOWORD(GetKeyboardLayout(0)));

	FrontEndMenuManager.m_nTitleLanguage = sysDefaultLCID;

    FrontEndMenuManager.m_nTextLanguage = (int32)[&] {
	    switch (kbLayoutLCID) {
		case LANG_GERMAN:  return eLanguage::GERMAN;
		case LANG_SPANISH: return eLanguage::SPANISH;
		case LANG_FRENCH:  return eLanguage::FRENCH;
		case LANG_ITALIAN: return eLanguage::ITALIAN;
		default:           return eLanguage::AMERICAN;
	    }
    }();

    FrontEndMenuManager.m_nPrefsLanguage = [&] {
        switch (usrDefaultLCID) {
        case LANG_SPANISH: return eLanguage::SPANISH;
        default:           return eLanguage::AMERICAN;
        }
    }();

    // Reload text
	TheText.Unload(false);
	TheText.Load(false);
}

// 0x747420
RwBool psInitialize() {
    auto ps = &PsGlobal;

    RsGlobal.ps = ps;

    ps->lastMousePos.y = 0.f;
    ps->lastMousePos.x = 0.f;
    ps->fullScreen    = FALSE;
    ps->diInterface   = NULL;
    ps->diMouse       = NULL;
    ps->diDevice1     = NULL;
    ps->diDevice2     = NULL;
    
    CFileMgr::Initialise();
    const auto usrdir = InitUserDirectories();
    s_PcSaveHelper.SetSaveDirectory(usrdir);

    gGameState = GAME_STATE_INITIAL;

    // TODO: Load vendor from CPUID

    // Figure out Windows version (TODO: Use `IsWindowsVersion*` from VersionHelpers.h instead)
    s_OSStatus.OSVer = [&] {
        OSVERSIONINFO verInfo{ sizeof(OSVERSIONINFO) };	
	    GetVersionEx(&verInfo);

        if (verInfo.dwPlatformId == VER_PLATFORM_WIN32_NT) {
            switch (const auto mv = verInfo.dwMajorVersion) {
            case 4:  return WinVer::WIN_NT;
            case 5:  return WinVer::WIN_2000_XP_2003;
            case 6:  return WinVer::WIN_VISTA_OR_LATER;
            default: return mv < 4 ? WinVer::WIN_NT : WinVer::WIN_VISTA_OR_LATER;
            }
        } else if (verInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) {
            return verInfo.dwMajorVersion > 4 || verInfo.dwMajorVersion == 4 && verInfo.dwMinorVersion != 0
                ? WinVer::WIN_98
                : WinVer::WIN_95;
        }
        NOTSA_UNREACHABLE(); // If this is reached the game wouldn't run anyways, so might as well just crash it.
    }();

    if (s_OSStatus.OSVer == WinVer::WIN_95) {
        MessageBoxW(
            NULL,
            (LPCWSTR)TheText.Get("WIN_95"),	 // Grand Theft Auto San Andreas cannot run on Windows 95
            (LPCWSTR)TheText.Get("WIN_TTL"), // Grand Theft Auto San Andreas
            MB_OK
        );

        return FALSE;
    }

    // Originally figured out available dx version and only allowed dx9, but we've simplified it.
    if (!CheckDirectX()) {
        MessageBoxW(
            NULL,
            (LPCWSTR)TheText.Get("WIN_DX"),	 // Grand Theft Auto San Andreas requires at least DirectX version 8.1
            (LPCWSTR)TheText.Get("WIN_TTL"), // Grand Theft Auto San Andreas
            MB_OK
        );
        return FALSE;
    }

    // CheckDirectX() Checks for Dx9 only, so use that
    s_OSStatus.DxVer = 0x900;

    if (!CheckDirectSound()) {
        MessageBoxW(
            NULL,
            (LPCWSTR)TheText.Get("WIN_NSC"), // Grand Theft Auto San Andreas requires a sound card (I guess?)
            (LPCWSTR)TheText.Get("WIN_TTL"), // Grand Theft Auto San Andreas
            MB_ICONEXCLAMATION
        );
        return FALSE;
    }

    MEMORYSTATUS memstat{ sizeof(MEMORYSTATUS) };
    GlobalMemoryStatus(&memstat); // TODO: `GlobalMemoryStatusEx`
    s_OSStatus.RAM.TotalPhys    = memstat.dwTotalPhys;
    s_OSStatus.RAM.AvailPhys    = memstat.dwAvailPhys;
    s_OSStatus.RAM.TotalVirtual = memstat.dwTotalVirtual;
    s_OSStatus.RAM.AvailVirtual = memstat.dwAvailVirtual;

    VERIFY(SUCCEEDED(GetVideoMemInfo(&s_OSStatus.VRAM.Total, &s_OSStatus.VRAM.Avail)));
    VERIFY(SUCCEEDED(CoInitialize(NULL)));

    // Load setting only after everything was checked - TODO: Move this out from here, it's not platform specific
    FrontEndMenuManager.LoadSettings();

    return TRUE;
}

// 0x7458A0
void psTerminate() {
    // NOP
}

// 0x7451B0
void psWindowSetText(const char* str) {
    SetWindowTextA(PSGLOBAL(window), str);
}

// 0x7451D0
void psErrorMessage(const char* str) {
    MessageBoxA(nullptr, str, RsGlobal.appName, MB_ICONERROR | MB_TASKMODAL | MB_TOPMOST);
}

// 0x7451F0
void psWarningMessage(const char* str) {
    MessageBoxA(nullptr, str, RsGlobal.appName, MB_ICONWARNING | MB_TASKMODAL | MB_TOPMOST);
}

// 0x745210
bool psCameraBeginUpdate(RwCamera* camera) {
    if (RwCameraBeginUpdate(Scene.m_pRwCamera)) {
        return true;
    }
    RsEventHandler(rsACTIVATE, nullptr);
    return false;
}

// 0x745240
RwCamera* psCameraShowRaster(RwCamera* camera) {
    auto flags = FrontEndMenuManager.m_bPrefsFrameLimiter || CLoadingScreen::m_bActive ? rwRASTERFLIPWAITVSYNC : rwRASTERFLIPDONTWAIT;
    return RwCameraShowRaster(camera, PSGLOBAL(window), flags);
}

// 0x745270
uint32 psTimer() {
    return OS_TimeMS();
}

// 0x7452B0
RwImage* psGrabScreen(RwCamera* camera) {
    auto* device = static_cast<IDirect3DDevice9*>(RwD3D9GetCurrentD3DDevice());
    assert(device);

    D3DDISPLAYMODE displayMode{};
    VERIFY(SUCCEEDED(device->GetDisplayMode(0, &displayMode)));

    IDirect3DSurface9* surface = nullptr;
    VERIFY(SUCCEEDED(device->CreateOffscreenPlainSurface(displayMode.Width, displayMode.Height, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &surface, nullptr)));
    VERIFY(SUCCEEDED(device->GetFrontBufferData(0, surface)));

    D3DLOCKED_RECT lockedRect{};
#ifndef FIX_BUGS
    // It's not needed as ClientToScreen func works with fullscreen mode.
    if (PSGLOBAL(fullScreen)) { // todo: Doesn't work properly with III.VC.SA.WindowedMode.asi
        VERIFY(SUCCEEDED(surface->LockRect(&lockedRect, nullptr, D3DLOCK_READONLY)));
    } else {
#endif
        RECT rect;
#ifdef FIX_BUGS
        // SA code gets the whole window of the game, which includes the titlebar etc.
        //
        // BUG: There should be bugs for older versions of Windows IIRC.
        // One example would be Vista version of the func doesn't count Aero effects of windows.
        //
        // TODO: Test with dual monitors etc.

        GetClientRect(PSGLOBAL(window), &rect);

        // GetClientRect returns relative positions unlike GetWindowRect.
        // i.e. will return { 0, 0, width, height }.
        ClientToScreen(PSGLOBAL(window), (POINT*)(&rect)); // kinda hacky but should work.
        rect.right += rect.left;
        rect.bottom += rect.top;
#else
        GetWindowRect(PSGLOBAL(window), &rect);
#endif
        displayMode.Height = rect.bottom - rect.top;
        displayMode.Width = rect.right - rect.left;
        VERIFY(SUCCEEDED(surface->LockRect(&lockedRect, &rect, D3DLOCK_READONLY)));
#ifndef FIX_BUGS
    }
#endif

    RwImage* image = RwImageCreate(int32(displayMode.Width), int32(displayMode.Height), 32);
    if (image) {
        RwImageAllocatePixels(image);

        auto* pixels = (RwRGBA*)RwImageGetPixels(image);
        auto* imagePixels = (uint8*)lockedRect.pBits;
        assert(pixels && imagePixels);

        for (auto h = 0u; h < displayMode.Height; h++) {
            for (auto w = 0u; w < displayMode.Width; w++) {
                pixels->red   = imagePixels[sizeof(RwRGBA) * w + 2];
                pixels->green = imagePixels[sizeof(RwRGBA) * w + 1];
                pixels->blue  = imagePixels[sizeof(RwRGBA) * w + 0];
                pixels->alpha = 255;
                pixels++;
            }
            imagePixels += lockedRect.Pitch;
        }
    }

    { // FIX_BUGS
        surface->UnlockRect();
        surface->Release();
        delete surface;
    }

    return image;
}

// 0x7453E0
void psMouseSetVisibility(bool visible) {
    ::ShowCursor(visible);
}

// 0x7453F0
void psMouseSetPos(RwV2d* pos) {
    POINT point = { .x = LONG(pos->x), .y = LONG(pos->y) };
    ::ClientToScreen(PSGLOBAL(window), &point);
    ::SetCursorPos(point.x, point.y);
    PSGLOBAL(lastMousePos) = *pos;
}

// 0x745470
char* psPathnameCreate(const char* buffer) {
    const auto pathSize = std::strlen(buffer) + 1u;
    auto path = (char*)CMemoryMgr::Malloc(pathSize);
    if (path) {
        strcpy_s(path, pathSize, buffer);

        while (auto ch = std::strchr(path, '/')) {
            *ch = psPathGetSeparator();
        }
    }

    return path;
}

// 0x7454E0
void psPathnameDestroy(char* buffer) {
    if (buffer) {
        RwFree(buffer);
    }
}

// 0x745500
char psPathGetSeparator() {
    return '\\';
}

// 0x745520
bool psInstallFileSystem() {
    return true;
}

// 0x745530
bool psNativeTextureSupport() {
    return RwD3D9DeviceSupportsDXTTexture();
}

// 0x745540
RsEventStatus psDebugMessageHandler(RsEvent event, void* param) {
    if (rsINITDEBUG == event) {
        RwDebugSetHandler(psDebugMessageHandler);
#if defined(DEBUG) && defined(RWTRACE)
        RwDebugSetTraceState(true);
#else
        RwDebugSetTraceState(false);
#endif
    }

    return rsEVENTNOTPROCESSED;
}

// 0x7458B0
bool psAlwaysOnTop(bool alwaysOnTop) {
    const auto hwnd = PSGLOBAL(window);

    RECT winRect;
    VERIFY(GetWindowRect(hwnd, &winRect));

    return SetWindowPos(
        hwnd,
        alwaysOnTop ? HWND_TOPMOST : HWND_NOTOPMOST,
        winRect.left,                 winRect.top,
        winRect.right - winRect.left, winRect.bottom - winRect.top,
        NULL
    );
}

// NOTSA
auto GetNativeResolutionOfCurrentSubsystem() {
#ifdef USE_D3D9
    const auto d3d = Direct3DCreate9(D3D_SDK_VERSION);
#else
    const auto d3d = Direct3DCreate8(D3D_SDK_VERSION);
#endif
    const notsa::ScopeGuard released3d{[d3d] { d3d->Release(); }};
    assert(d3d != NULL);

    D3DDISPLAYMODE nativeRes;
    d3d->GetAdapterDisplayMode(RwEngineGetCurrentSubSystem(), &nativeRes);

    DEV_LOG("Got native resolution from RW subsystem ({}): {} x {}", RwEngineGetCurrentSubSystem(), nativeRes.Width, nativeRes.Height);

    return std::make_pair(nativeRes.Width, nativeRes.Height);
}

BOOL CheckDefaultVideoModeSupported() {
    // IMPROVEMENT/FIX_BUGS: The game will now default to native adapter
    // resolution instead of 800x600.

    const auto SearchVideoMode = [](int32 width, int32 height) {
        for (auto i = 0; i < RwEngineGetNumVideoModes(); i++) {
            const auto vm = RwEngineGetVideoModeInfo(i);
            if (vm.width == width && vm.height == height && vm.depth == 32 && (vm.flags & rwVIDEOMODEEXCLUSIVE)) {
                return i;
            }
        }

        return -1;
    };

    if (notsa::IsFixBugs()) {
        const auto&& [w, h] = GetNativeResolutionOfCurrentSubsystem();
        if (const auto vm = SearchVideoMode(w, h); vm != -1) {
            GcurSelVM = vm;
            return TRUE;
        }
        /* fallthrough if native res is not supported! */
    }

    if (const auto vm = SearchVideoMode(800, 600); vm != -1) {
        GcurSelVM = vm;
        return TRUE;
    }

    MessageBox(NULL, "Cannot find 800x600x32 video mode", "GTA: San Andreas", IDOK);
    return FALSE;
}

// 0x7460A0
RwUInt32 GetBestRefreshRate(RwUInt32 width, RwUInt32 height, RwUInt32 depth) {
#ifdef USE_D3D9
    const auto d3d = Direct3DCreate9(D3D_SDK_VERSION);
#else
    const auto d3d = Direct3DCreate8(D3D_SDK_VERSION);
#endif
    const notsa::ScopeGuard released3d{ [d3d] { d3d->Release(); } };

    assert(d3d != NULL);

    RwUInt32 refreshRate = INT_MAX;

    const auto format = depth == 32
        ? D3DFMT_X8R8G8B8
        : depth == 24
            ? D3DFMT_R8G8B8
            : D3DFMT_R5G6B5;
    
#ifdef USE_D3D9
    for (auto i = d3d->GetAdapterModeCount(GcurSelSS, format); i-- > 0;) {
#else
    for (auto i = d3d->GetAdapterModeCount(GcurSel); i-- > 0;) {
#endif
        D3DDISPLAYMODE mode;

#ifdef USE_D3D9
        d3d->EnumAdapterModes(GcurSelSS, format, i, &mode);
#else
        d3d->EnumAdapterModes(GcurSel, i, &mode);
#endif

        if (mode.Width != width || mode.Height != height || mode.Format != format) {
            continue;
        }

        if (mode.RefreshRate == 0) {
            return 0;
        }

        if (mode.RefreshRate < refreshRate && mode.RefreshRate >= 60) {
            refreshRate = mode.RefreshRate;
        }
    }

    return refreshRate;
}

// 0x746190
bool psSelectDevice() {
    const auto wnd  = PSGLOBAL(window);
    const auto inst = PSGLOBAL(instance);

    if (!UseDefaultVM) {
        GnumSubSystems = RwEngineGetNumSubSystems();
        if (!GnumSubSystems) {
            DEV_LOG("No SubSystems to select from!");
            return FALSE;
        }

        /* Just to be sure ... */
        GnumSubSystems = std::min(MAX_SUBSYSTEMS, GnumSubSystems);

        /* Get the names of all the sub systems */
        for (auto i = 0; i < GnumSubSystems; i++) {
            RwEngineGetSubSystemInfo(&GsubSysInfo[i], i);
        }

        /* Get the default selection */
        GcurSelSS = RwEngineGetCurrentSubSystem();
    }

    MultipleSubSystems = GnumSubSystems > 1;

    // Select video mode to use
    if (MultipleSubSystems && !UseDefaultVM) {
        if (!CreateVidModeSelectDialog(inst, wnd)) {
            return FALSE; // User failed to select video mode
        }
    }

    // Set selected subsystem
    if (!RwEngineSetSubSystem(GcurSelSS)) {
        DEV_LOG("Failed: RwEngineSetSubSystem({})", GcurSelSS);
        return FALSE;
    }

    DEV_LOG("GcurSelSS={}", GcurSelSS);

    if (!UseDefaultVM && !MultipleSubSystems) {
        const auto vmDisplay = FrontEndMenuManager.m_nDisplayVideoMode;
        if (!vmDisplay || !GetVideoModeList()[vmDisplay]) {
            if (IsVMNotSelected && !CheckDefaultVideoModeSupported()) {
                return FALSE;
            }
        } else {
            GcurSelVM = FrontEndMenuManager.m_nPrefsVideoMode = vmDisplay;
        }
    }

    FrontEndMenuManager.m_nCurrentScreenItem = 0;

    // Set selected videomode
    if (!RwEngineSetVideoMode(GcurSelVM)) {
        DEV_LOG("Failed: RwEngineSetVideoMode({})", GcurSelVM);
        return FALSE;
    }

    DEV_LOG("GcurSelVM={}", GcurSelVM);

    if (const auto vmi = RwEngineGetVideoModeInfo(GcurSelVM); vmi.flags & rwVIDEOMODEEXCLUSIVE) {
        if (const auto rr = GetBestRefreshRate(vmi.width, vmi.height, vmi.depth); rr != -1) {
            DEV_LOG("Refresh Rate: {} Hz", rr);
            RwD3D9EngineSetRefreshRate(rr);
        }

        RsGlobal.maximumHeight = vmi.height;
        RsGlobal.maximumWidth  = vmi.width;
        PSGLOBAL(fullScreen)   = true;
    }

    RwD3D9EngineSetMultiSamplingLevels(FrontEndMenuManager.m_nPrefsAntialiasing);

    return TRUE;
}

void WinPsInjectHooks() {
    RH_ScopedCategory("Win");
    RH_ScopedNamespaceName("Ps");

    RH_ScopedGlobalInstall(psInitialize, 0x747420);
    RH_ScopedGlobalInstall(psTerminate, 0x7458A0);
    RH_ScopedGlobalInstall(psWindowSetText, 0x7451B0);
    RH_ScopedGlobalInstall(psErrorMessage, 0x7451D0);
    RH_ScopedGlobalInstall(psWarningMessage, 0x7451F0);
    RH_ScopedGlobalInstall(psCameraBeginUpdate, 0x745210);
    RH_ScopedGlobalInstall(psCameraShowRaster, 0x745240);
    RH_ScopedGlobalInstall(psTimer, 0x745270);
    RH_ScopedGlobalInstall(psGrabScreen, 0x7452B0);
    RH_ScopedGlobalInstall(psMouseSetVisibility, 0x7453E0);
    RH_ScopedGlobalInstall(psMouseSetPos, 0x7453F0);
    RH_ScopedGlobalInstall(psPathnameCreate, 0x745470);
    RH_ScopedGlobalInstall(psPathnameDestroy, 0x7454E0);
    RH_ScopedGlobalInstall(psPathGetSeparator, 0x745500);
    RH_ScopedGlobalInstall(psInstallFileSystem, 0x745520);
    RH_ScopedGlobalInstall(psNativeTextureSupport, 0x745530);
    RH_ScopedGlobalInstall(psDebugMessageHandler, 0x745540);
    RH_ScopedGlobalInstall(psAlwaysOnTop, 0x7458B0);
    RH_ScopedGlobalInstall(psSelectDevice, 0x746190);

    RH_ScopedGlobalInstall(InitialiseLanguage, 0x7465B0);
    RH_ScopedGlobalInstall(CheckDirectSound, 0x745840);
    RH_ScopedGlobalInstall(GetVideoMemInfo, 0x7455E0);
}
