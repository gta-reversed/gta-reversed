#include "StdInc.h"

#include "platform.h"
#include "VideoMode.h"
#include "app/app.h"

void RsInjectHooks() {
    RH_ScopedNamespaceName("Rs");
    RH_ScopedCategoryGlobal();

    RH_ScopedGlobalInstall(RsKeyFromScanCode, 0x6193F0);
    RH_ScopedGlobalInstall(RsTimer, 0x619410);
    RH_ScopedGlobalInstall(RsWindowSetText, 0x619420);
    RH_ScopedGlobalInstall(RsPathGetSeparator, 0x619430);
    RH_ScopedGlobalInstall(RsCameraBeginUpdate, 0x619450);
    RH_ScopedGlobalInstall(RsAlwaysOnTop, 0x619460);
    RH_ScopedGlobalInstall(RsRegisterImageLoader, 0x619470);
    RH_ScopedGlobalInstall(RsSetDebug, 0x619480);
    RH_ScopedGlobalInstall(RsMouseSetVisibility, 0x619490);
    RH_ScopedGlobalInstall(RsMouseSetPos, 0x6194A0);
    RH_ScopedGlobalInstall(RsSelectDevice, 0x6194B0);
    RH_ScopedGlobalInstall(RsInputDeviceAttach, 0x6194C0);
    RH_ScopedGlobalInstall(rsCommandLine, 0x619510);
    RH_ScopedGlobalInstall(rsPreInitCommandLine, 0x619530);
    RH_ScopedGlobalInstall(RsKeyboardEventHandler, 0x619560);
    RH_ScopedGlobalInstall(RsMouseEventHandler, 0x619580);
    RH_ScopedGlobalInstall(RsPadEventHandler, 0x6195A0);
    RH_ScopedGlobalInstall(RsPathnameDestroy, 0x6195C0);
    RH_ScopedGlobalInstall(RsPathnameCreate, 0x6195D0);
    RH_ScopedGlobalInstall(RsRwTerminate, 0x6195E0);
    RH_ScopedGlobalInstall(RsTerminate, 0x6195F0);
    RH_ScopedGlobalInstall(RsInitialize, 0x619600);
    RH_ScopedGlobalInstall(RsGrabScreen, 0x619AF0);
    RH_ScopedGlobalInstall(RsErrorMessage, 0x619B00);
    RH_ScopedGlobalInstall(RsWarningMessage, 0x619B30);
    RH_ScopedGlobalInstall(RsEventHandler, 0x619B60);
    RH_ScopedGlobalInstall(RsRwInitialize, 0x619C90);
}

static std::array<uint8, 256>& KeysShifted = *(std::array<uint8, 256>*)0x8D2D00;
static std::array<uint8, 256>& KeysNormal = *(std::array<uint8, 256>*)0x8D2C00;

// 0x6193F0
uint8 RsKeyFromScanCode(uint8 scan, int32 shiftKeyDown) {
    return (shiftKeyDown ? KeysShifted : KeysNormal)[scan];
}

// 0x619410
uint32 RsTimer() {
    return psTimer();
}

// 0x619420
void RsWindowSetText(const RwChar* str) {
    psWindowSetText(str);
}

// 0x619430
char RsPathGetSeparator() {
    return psPathGetSeparator();
}

// 0x619450
bool RsCameraBeginUpdate(RwCamera* camera) {
    return psCameraBeginUpdate(nullptr);
}

// 0x619440
RwCamera* RsCameraShowRaster(RwCamera* camera) {
    ZoneScoped;

    return psCameraShowRaster(camera);
}

// 0x619460
bool RsAlwaysOnTop(bool alwaysOnTop) {
    return psAlwaysOnTop(alwaysOnTop);
}

// 0x619470
bool RsRegisterImageLoader() {
    return true; // always true
}

// 0x619480
void RsSetDebug() {
    // return true;
}

// 0x619490
void RsMouseSetVisibility(bool visibility) {
    psMouseSetVisibility(visibility);
}

// 0x6194A0
void RsMouseSetPos(RwV2d* pos) {
    psMouseSetPos(pos);
}

// 0x6194B0
bool RsSelectDevice() {
    return psSelectDevice();
}

// 0x6194C0
int32 RsInputDeviceAttach(RsInputDeviceType type, RsInputEventHandler eventHandler) {
    switch (type) {
    case rsKEYBOARD:
        RsGlobal.keyboard.inputEventHandler = eventHandler;
        RsGlobal.keyboard.used = true;
        break;
    case rsMOUSE:
        RsGlobal.mouse.inputEventHandler = eventHandler;
        RsGlobal.mouse.used = true;
        break;
    case rsPAD:
        RsGlobal.pad.inputEventHandler = eventHandler;
        RsGlobal.pad.used = true;
        break;
    default:
        return false;
    }

    return true;
}

// 0x619510
bool rsCommandLine(void* param) {
    RsEventHandler(rsFILELOAD, param);
    return true;
}

// 0x619530
bool rsPreInitCommandLine(RwChar* arg) {
    if (strcmp(arg, RWSTRING("-vms")) == 0) {
        DefaultVM = FALSE;
        return true;
    }
    return false;
}

// 0x619560
RsEventStatus RsKeyboardEventHandler(RsEvent event, void* param) { // Param should be `RsKeyCodes*` (so pass in a ptr to a `RsKeyCodes`)
    if (RsGlobal.keyboard.used) {
        return RsGlobal.keyboard.inputEventHandler(event, param);
    }
    return rsEVENTNOTPROCESSED;
}

// 0x619580
RsEventStatus RsMouseEventHandler(RsEvent event, void* param) {
    if (RsGlobal.mouse.used) {
        return RsGlobal.mouse.inputEventHandler(event, param);
    }
    return rsEVENTNOTPROCESSED;
}

// 0x6195A0
RsEventStatus RsPadEventHandler(RsEvent event, void* param) {
    if (RsGlobal.pad.used) {
        return RsGlobal.pad.inputEventHandler(event, param);
    }
    return rsEVENTNOTPROCESSED;
}

// 0x6195D0
RwChar* RsPathnameCreate(const RwChar* buffer) {
    return psPathnameCreate(buffer);
}

// 0x6195C0
void RsPathnameDestroy(RwChar* buffer) {
    psPathnameDestroy(buffer);
}

// 0x6195E0
void RsRwTerminate() {
    RwEngineStop();
    RwEngineClose();
    RwEngineTerm();
}

// 0x6195F0
void RsTerminate() {
    psTerminate();
}

// 0x619600
bool RsInitialize() {
    RsGlobal.appName        = "GTA: San Andreas";
    RsGlobal.maximumWidth   = DEFAULT_SCREEN_WIDTH;
    RsGlobal.maximumHeight  = DEFAULT_SCREEN_HEIGHT;
    RsGlobal.frameLimit     = APP_MAX_FPS;
    RsGlobal.quit           = false;

    RsGlobal.keyboard.inputDeviceType   = rsKEYBOARD;
    RsGlobal.keyboard.inputEventHandler = nullptr;
    RsGlobal.keyboard.used              = false;

    RsGlobal.mouse.inputDeviceType   = rsMOUSE;
    RsGlobal.mouse.inputEventHandler = nullptr;
    RsGlobal.mouse.used              = false;

    RsGlobal.pad.inputDeviceType   = rsPAD;
    RsGlobal.pad.inputEventHandler = nullptr;
    RsGlobal.pad.used              = false;

    return psInitialize();
}

// 0x619AF0
RwImage* RsGrabScreen(RwCamera* camera) {
    return psGrabScreen(camera);
}

// 0x619B00
void RsErrorMessage(const RwChar* msg) {
    RsKeyStatus ks;
    if (RsGlobal.keyboard.used) {
        RsKeyboardEventHandler(rsKEYUP, &ks);
    }
    psErrorMessage(msg);
}

// 0x619B30
void RsWarningMessage(const RwChar* msg) {
    RsKeyStatus ks;
    if (RsGlobal.keyboard.used) {
        RsKeyboardEventHandler(rsKEYUP, &ks);
    }
    psWarningMessage(msg);
}

/*
static RwMemoryFunctions g_objMemFunctions = { // 0x8D6228
    CMemoryMgr::Malloc,
    CMemoryMgr::Free,
    CMemoryMgr::Realloc,
    CMemoryMgr::Calloc
};
*/

// 0x745510
RwMemoryFunctions* psGetMemoryFunctions() {
    return plugin::CallAndReturn<RwMemoryFunctions*, 0x745510>();
    // return &g_objMemFunctions;
}

// 0x619C90
bool RsRwInitialize(void* param) { // Win32: Param is HWND
    if (!RwEngineInit(psGetMemoryFunctions(), 0, rsRESOURCESDEFAULTARENASIZE))
        return false;

    AppEventHandler(rsINITDEBUG, nullptr);
    psInstallFileSystem();
    if (!AppEventHandler(rsPLUGINATTACH, nullptr))
        return false;

    if (!AppEventHandler(rsINPUTDEVICEATTACH, nullptr))
        return false;

    RwEngineOpenParams openParams = {.displayID = param};
    if (!RwEngineOpen(&openParams)) {
        RwEngineTerm();
        return false;
    }

    auto res = [&param] {
        if (auto r = AppEventHandler(rsSELECTDEVICE, param); r != rsEVENTNOTPROCESSED) {
            return r;
        } else {
            return (RsEventStatus)psSelectDevice();
        }
    }();

    if (res == rsEVENTERROR || !RwEngineStart()) {
        RwEngineClose();
        RwEngineTerm();
        return false;
    }

    AppEventHandler(rsREGISTERIMAGELOADER, nullptr);
    psNativeTextureSupport();
    RwTextureSetMipmapping(true);
    RwTextureSetAutoMipmapping(false);

    return true;
}

// 0x619B60
RsEventStatus RsEventHandler(RsEvent event, void* param) {
    RsEventStatus result = AppEventHandler(event, param);

    if (event == rsQUITAPP)
        RsGlobal.quit = true;

    if (result != rsEVENTNOTPROCESSED)
        return result;

    switch (event) {
    case rsCOMMANDLINE:
        RsEventHandler(rsFILELOAD, param);
        return rsEVENTPROCESSED;

    case rsINITDEBUG:
    case rsREGISTERIMAGELOADER:
        return rsEVENTPROCESSED;

    case rsRWINITIALIZE: // Win32: Param is HWND 
        return RSEVENT_SUCCEED(RsRwInitialize(param));

    case rsRWTERMINATE:
        RsRwTerminate();
        return rsEVENTPROCESSED;

    case rsSELECTDEVICE:
        return RSEVENT_SUCCEED(RsSelectDevice());

    case rsINITIALIZE:
        return RSEVENT_SUCCEED(RsInitialize());

    case rsTERMINATE:
        RsTerminate();
        return rsEVENTPROCESSED;

    case rsPREINITCOMMANDLINE:
        return RSEVENT_SUCCEED(rsPreInitCommandLine((RwChar*)param));

    default:
        break;
    }

    return rsEVENTNOTPROCESSED;
}

// Returns true if ratio is 5:3, 16:9 or 16:10.
bool IsWideScreenRatio(float ratio) {
    return ratio == 0.6f || ratio == 10.0f / 16.0f || ratio == 9.0f / 16.0f;
}

// Returns true if ratio is 4:3 or 5:4.
bool IsFullScreenRatio(float ratio) {
    return ratio == 3.0f / 4.0f || ratio == 4.0f / 5.0f;
}
