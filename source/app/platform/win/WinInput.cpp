#include "StdInc.h"

#ifndef NOTSA_USE_SDL3

#include "WinInput.h"
#include "ControllerConfigManager.h"
#include "WinPlatform.h"
#include "VideoMode.h"
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

namespace WinInput {
// 0x746990
HRESULT CreateInput() {
    if (PSGLOBAL(diInterface)) {
        return S_OK; // Already created
    }

    if (SUCCEEDED(DirectInput8Create(GetModuleHandle(nullptr), DIRECTINPUT_VERSION, IID_IDirectInput8, (LPVOID*)&PSGLOBAL(diInterface), nullptr))) {
        return S_OK;
    }

    return S_FALSE; 
}

// 0x7487CF
bool Initialise() {
    ControlsManager.MakeControllerActionsBlank();
    ControlsManager.InitDefaultControlConfiguration();

    if (FAILED(CreateInput())) {
        return false;
    }

    diMouseInit(false);
    diPadInit();

    return true;
}

HRESULT ReleaseInput() {
    if (const auto ifc = PSGLOBAL(diInterface)) {
        return ifc->Release();
    }
    return S_OK; // Nothing to release
}

HRESULT Shutdown() {
    return ReleaseInput();
}

// 0x746D80
HRESULT diPadSetRanges(LPDIRECTINPUTDEVICE8 dev, DWORD padNum) {
    if (dev == NULL) {
        return S_OK; // Weird but okay
    }

    enum {
        NO_PROPERTY,    // Device doesn't have this property
        PROP_READ_ONLY, // Deivce does have this property, but it's read-only
        SUCCESS         // Device has property and we've set it successfully
    };

    const auto SetPropery = [&](DWORD prop) {
        // Set ranges
        DIDEVICEOBJECTINSTANCEA objinfo{
            sizeof(DIDEVICEOBJECTINSTANCEA)
        };
        DIPROPRANGE range{
            .diph = DIPROPHEADER{
                .dwSize       = sizeof(DIPROPRANGE),
                .dwHeaderSize = sizeof(DIPROPHEADER),
                .dwObj        = prop,
                .dwHow        = DIPH_BYOFFSET,
            },
            .lMin = -2000,
            .lMax = 2000,
        };
        if (FAILED(dev->GetObjectInfo(&objinfo, prop, DIPH_BYOFFSET))) {
            return NO_PROPERTY; 
        }
        if (FAILED(dev->SetProperty(DIPROP_RANGE, &range.diph))) {
            return PROP_READ_ONLY; 
        }
        return SUCCESS; 
    };

    if (SetPropery(DIJOFS_X) == PROP_READ_ONLY) {
        return S_FALSE;
    }

    if (SetPropery(DIJOFS_Y) == PROP_READ_ONLY) {
        return S_FALSE;
    }

    const auto SetProperyAndSetFlag = [&](DWORD prop, bool& flag) {
        const auto res = SetPropery(DIJOFS_Z);
        if (res != NO_PROPERTY) {
            flag = true;
        }
        return res;
    };

    if (SetProperyAndSetFlag(DIJOFS_Z, AllValidWinJoys.JoyStickNum[padNum].bZAxisPresent) == PROP_READ_ONLY) {
        return S_FALSE;
    }

    if (SetProperyAndSetFlag(DIJOFS_RZ, AllValidWinJoys.JoyStickNum[padNum].bZRotPresent) == PROP_READ_ONLY) {
        return S_FALSE;
    }

    return S_OK;
}

//! [NOTSA - From 0x7485C0] - Set device config product/vendor id
void diPadSetPIDVID(LPDIRECTINPUTDEVICE8 dev, DWORD padNum) {
    DIPROPDWORD vidpid{
        .diph = DIPROPHEADER{
            .dwSize       = sizeof(DIPROPDWORD),
            .dwHeaderSize = sizeof(DIPROPHEADER),
            .dwObj        = NULL,
            .dwHow        = DIPH_DEVICE,
        }
    };
    WIN_FCHECK(dev->GetProperty(DIPROP_VIDPID, &vidpid.diph));
    auto& cfg = AllValidWinJoys.JoyStickNum[padNum];
    cfg.wDeviceID = vidpid.dwData;
    cfg.wVendorID = LOWORD(vidpid.dwData);
    cfg.wProductID = HIWORD(vidpid.dwData);
    cfg.bJoyAttachedToPort = true;
}

// 0x7469A0
void diMouseInit(bool exclusive) {
    WIN_FCHECK(PSGLOBAL(diInterface)->CreateDevice(GUID_SysMouse, &PSGLOBAL(diMouse), 0));
    WIN_FCHECK(PSGLOBAL(diMouse)->SetDataFormat(&c_dfDIMouse2));
    WIN_FCHECK(PSGLOBAL(diMouse)->SetCooperativeLevel(PSGLOBAL(window), DISCL_FOREGROUND | (exclusive ? DISCL_EXCLUSIVE : DISCL_NONEXCLUSIVE)));
    WIN_FCHECK(PSGLOBAL(diMouse)->Acquire());
}

// 0x7485C0
static constexpr auto DEVICE_AXIS_MIN = -2000;
static constexpr auto DEVICE_AXIS_MAX = 2000;

// Context structure for device enumeration callback
struct EnumDevicesContext {
    int count = 0;
};

// Based on _InputEnumDevicesCallback, using context instead of static count
BOOL CALLBACK EnumDevicesCallback_Old( const DIDEVICEINSTANCE* pdidInstance, VOID* pContextData )
{
    HRESULT hr;
    EnumDevicesContext* ctx = static_cast<EnumDevicesContext*>(pContextData);

    LPDIRECTINPUTDEVICE8 *pJoystick = nullptr;

    if ( ctx->count == 0 )
        pJoystick = &PSGLOBAL(diDevice1);
    else if ( ctx->count == 1 )
        pJoystick = &PSGLOBAL(diDevice2);
    else {
        // Already have 2 devices, stop enumerating
        return DIENUM_STOP;
    }

    // Obtain an interface to the enumerated joystick.
    hr = PSGLOBAL(diInterface)->CreateDevice( pdidInstance->guidInstance, pJoystick, nullptr );

    // If it failed, then we can't use this joystick. (Maybe the user unplugged
    // it while we were in the middle of enumerating it.)
    if( FAILED(hr) )
        return DIENUM_CONTINUE;

    // Set the data format to use DirectInput's standard joystick control structure.
    hr = (*pJoystick)->SetDataFormat( &c_dfDIJoystick2 );
    if( FAILED(hr) )
    {
        (*pJoystick)->Release();
        *pJoystick = nullptr; // Ensure it's null if setup fails
        return DIENUM_CONTINUE;
    }

    // Set the cooperative level to share access with other applications.
    hr = (*pJoystick)->SetCooperativeLevel( PSGLOBAL(window), DISCL_NONEXCLUSIVE|DISCL_FOREGROUND );
    if( FAILED(hr) )
    {
        (*pJoystick)->Release();
        *pJoystick = nullptr; // Ensure it's null if setup fails
        return DIENUM_CONTINUE;
    }

    ctx->count++;

    // Stop enumeration if we have found two devices.
    if ( ctx->count >= 2 ) // todo: Use JOYPAD_COUNT or similar constant
        return DIENUM_STOP;

    return DIENUM_CONTINUE;
}


// Based on _InputAddJoyStick - Sets axis ranges and detects axis presence
void diPadAddJoyStick(LPDIRECTINPUTDEVICE8 lpDevice, INT padNum)
{
    if (!lpDevice) {
        return;
    }

    DIDEVICEOBJECTINSTANCE objInst;
    objInst.dwSize = sizeof( DIDEVICEOBJECTINSTANCE );

    DIPROPRANGE range;
    range.diph.dwSize = sizeof(DIPROPRANGE);
    range.diph.dwHeaderSize = sizeof(DIPROPHEADER);
    range.lMin = DEVICE_AXIS_MIN;
    range.lMax = DEVICE_AXIS_MAX;
    range.diph.dwHow = DIPH_BYOFFSET;

    // Set X range
    range.diph.dwObj = DIJOFS_X;
    if ( SUCCEEDED( lpDevice->GetObjectInfo( &objInst,	DIJOFS_X, DIPH_BYOFFSET ) ) ) {
        // Attempt to set range, ignore failure (might be read-only or unsupported)
        lpDevice->SetProperty( DIPROP_RANGE, &range.diph );
    }

    // Set Y range
    range.diph.dwObj = DIJOFS_Y;
    if ( SUCCEEDED( lpDevice->GetObjectInfo( &objInst,	DIJOFS_Y, DIPH_BYOFFSET ) ) ) {
        lpDevice->SetProperty( DIPROP_RANGE, &range.diph );
    }

    // Set Z range and check presence
    range.diph.dwObj = DIJOFS_Z;
    if ( SUCCEEDED( lpDevice->GetObjectInfo( &objInst,	DIJOFS_Z, DIPH_BYOFFSET ) ) ) {
        // Only mark as present if setting the range succeeds (implies axis is usable)
        if( SUCCEEDED( lpDevice->SetProperty( DIPROP_RANGE, &range.diph ) ) ) {
            AllValidWinJoys.JoyStickNum[padNum].bZAxisPresent = true;
        }
    }

    // Set RZ (Rotation Z) range and check presence
    range.diph.dwObj = DIJOFS_RZ;
    if ( SUCCEEDED( lpDevice->GetObjectInfo( &objInst,	DIJOFS_RZ, DIPH_BYOFFSET ) ) ) {
        if( SUCCEEDED( lpDevice->SetProperty( DIPROP_RANGE, &range.diph ) ) ) {
             AllValidWinJoys.JoyStickNum[padNum].bZRotPresent = true;
        }
    }
}

// Based on _InputAddJoys - Enumerates and sets up joysticks
HRESULT diPadAddJoys()
{
    HRESULT hr;
    EnumDevicesContext context; // Use context for enumeration count

    // Enumerate attached game controllers
    hr = PSGLOBAL(diInterface)->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumDevicesCallback_Old, &context, DIEDFL_ATTACHEDONLY );

    if( FAILED(hr) ) {
        spdlog::error("DirectInput EnumDevices failed with HRESULT: {:#x}", hr);
        return hr;
    }

    // If no devices were successfully initialized, return S_FALSE
    if ( PSGLOBAL(diDevice1) == nullptr && PSGLOBAL(diDevice2) == nullptr) {
        spdlog::info("No joysticks found or initialized.");
        return S_FALSE;
    }

    // Configure ranges and detect axes for the initialized devices
    diPadAddJoyStick(PSGLOBAL(diDevice1), 0);
    diPadAddJoyStick(PSGLOBAL(diDevice2), 1);

    return S_OK;
}


// Based on _InputInitialiseJoys - Main initialization function for pads
void diPadInit() {
    AllValidWinJoys = CJoySticks{}; // Clear existing joystick info

    // Enumerate, create, and perform basic setup for joystick devices
    if (FAILED(diPadAddJoys())) {
        // Error already logged in diPadAddJoys if EnumDevices failed.
        // If S_FALSE, it means no devices were found, which is not necessarily an error.
        spdlog::info("diPadAddJoys completed (possibly with no devices found).");
    }

    // Get VID/PID, capabilities, and finalize setup for successfully initialized devices
    const auto FinalizePadSetup = [](LPDIRECTINPUTDEVICE8 dev, DWORD padNum) {
        if (dev == nullptr) {
            return; // Skip if device wasn't initialized
        }

        DIDEVCAPS devCaps;
        devCaps.dwSize = sizeof(DIDEVCAPS);
        if (FAILED(dev->GetCapabilities(&devCaps))) {
            spdlog::warn("Failed to get capabilities for pad {}", padNum);
            // Continue without capabilities if needed, or handle error
        }

        DIPROPDWORD prop;
        prop.diph.dwSize = sizeof(DIPROPDWORD);
        prop.diph.dwHeaderSize = sizeof(DIPROPHEADER);
        prop.diph.dwObj = 0;
        prop.diph.dwHow = DIPH_DEVICE; // Use DIPH_DEVICE for VIDPID

        if (SUCCEEDED(dev->GetProperty(DIPROP_VIDPID, &prop.diph))) {
            auto& joyInfo = AllValidWinJoys.JoyStickNum[padNum];
            joyInfo.wVendorID = LOWORD(prop.dwData);
            joyInfo.wProductID = HIWORD(prop.dwData);
            joyInfo.wDeviceID = prop.dwData; // Store the full VIDPID
            spdlog::info("Pad {} VID: {:#06x}, PID: {:#06x}", padNum, joyInfo.wVendorID, joyInfo.wProductID);
        } else {
            spdlog::warn("Failed to get VID/PID for pad {}", padNum);
        }

        AllValidWinJoys.JoyStickNum[padNum].bJoyAttachedToPort = true; // Mark as initialized/attached

        // Initialize default controls only for the first pad, based on original logic
        if (padNum == 0) {
            ControlsManager.InitDefaultControlConfigJoyPad(devCaps.dwButtons);
            spdlog::info("Initialized default controls for pad 0 with {} buttons.", devCaps.dwButtons);
        }
    };

    FinalizePadSetup(PSGLOBAL(diDevice1), 0);
    FinalizePadSetup(PSGLOBAL(diDevice2), 1);
}

// 0x747020
BOOL CALLBACK EnumDevicesCallback(LPCDIDEVICEINSTANCEA inst, LPVOID) {
    static int32 snJoyCount = 0;

    LPDIRECTINPUTDEVICE8* joy = [] {
        switch (snJoyCount) {
        case 0:
            return &PSGLOBAL(diDevice1);
        case 1:
            return &PSGLOBAL(diDevice2);
        default:
            NOTSA_UNREACHABLE();
        }
    }();

    if (SUCCEEDED(PSGLOBAL(diInterface)->CreateDevice(inst->guidInstance, joy, nullptr))) {
        return TRUE;
    }

    if (SUCCEEDED((*joy)->SetDataFormat(&c_dfDIJoystick2))) {
        (*joy)->Release();
        return TRUE;
    }

    ++snJoyCount;

    if (FAILED((*joy)->SetCooperativeLevel(PSGLOBAL(window), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE))) {
        (*joy)->Release();
        return TRUE;
    }

    return snJoyCount != 2; // todo: CJoySticks, JOYPAD_COUNT
}

// 0x53F2D0
CMouseControllerState GetMouseSetUp() {
    CMouseControllerState state;

    if (!PSGLOBAL(diMouse)) {
        WinInput::diMouseInit(!FrontEndMenuManager.m_bMenuActive && IsVideoModeExclusive());
    }

    if (!PSGLOBAL(diMouse)) {
        return state; // No mouse device
    }
    
    DIDEVCAPS devCaps;
    devCaps.dwSize = sizeof(DIDEVCAPS);

    // Get device capabilities
    if (SUCCEEDED(PSGLOBAL(diMouse)->GetCapabilities(&devCaps))) {
        switch (devCaps.dwButtons) {
        case 8:
        case 7:
        case 6:
        case 5:
        case 4:
        case 3:
            state.isMouseMiddleButtonPressed = true; // Assumes button 3+ implies MMB exists
            [[fallthrough]];
        case 2:
            state.isMouseRightButtonPressed = true; // Assumes button 2+ implies RMB exists
            [[fallthrough]];
        case 1:
            state.isMouseLeftButtonPressed = true; // Assumes button 1+ implies LMB exists
            [[fallthrough]];
        default:
            break;
        }

        if (devCaps.dwAxes >= 3) {
            state.isMouseWheelMovedDown = true;
            state.isMouseWheelMovedUp = true;
        }

        if (devCaps.dwButtons >= 4) {
            state.isMouseFirstXPressed = true;
        }
        if (devCaps.dwButtons >= 5) {
            state.isMouseSecondXPressed = true;
        }
    }
    return state;
}

void InjectHooks() {
    RH_ScopedCategory("Win");
    RH_ScopedNamespaceName("Input");

    RH_ScopedGlobalInstall(Initialise, 0x7487CF); 
    RH_ScopedGlobalInstall(diMouseInit, 0x7469A0, {.locked = true}); // Can't be hooked because it fails with ACCESS DENIED and crashes
    RH_ScopedGlobalInstall(EnumDevicesCallback, 0x747020);
    RH_ScopedGlobalInstall(diPadInit, 0x7485C0);
    RH_ScopedGlobalInstall(diPadSetRanges, 0x746D80);
    RH_ScopedGlobalInstall(GetMouseSetUp, 0x53F2D0);
}

bool IsKeyPressed(unsigned int keyCode) {
    return (GetKeyState(keyCode) & 0x8000) != 0;
}

bool IsKeyDown(unsigned int keyCode) {
    return GetAsyncKeyState(keyCode) >> 0x8;
}
} // namespace WinInput

#endif // NOTSA_USE_DINPUT
