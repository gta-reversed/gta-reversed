#include "StdInc.h"

#include "Input.h"
#include "ControllerConfigManager.h"
#include "Platform.h"
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
    ControlsManager.InitializeDefaultKeyboardAndMouseBindings();

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

    if (SetProperyAndSetFlag(DIJOFS_Z, PadConfigs[padNum].zAxisPresent) == PROP_READ_ONLY) {
        return S_FALSE;
    }

    if (SetProperyAndSetFlag(DIJOFS_RZ, PadConfigs[padNum].rzAxisPresent) == PROP_READ_ONLY) {
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
    auto& cfg = PadConfigs[padNum];
    cfg.vendorId = LOWORD(vidpid.dwData);
    cfg.productId = HIWORD(vidpid.dwData);
    cfg.present = true;
}

// 0x7469A0
void diMouseInit(bool exclusive) {
    WIN_FCHECK(PSGLOBAL(diInterface)->CreateDevice(GUID_SysMouse, &PSGLOBAL(diMouse), 0));
    WIN_FCHECK(PSGLOBAL(diMouse)->SetDataFormat(&c_dfDIMouse2));
    WIN_FCHECK(PSGLOBAL(diMouse)->SetCooperativeLevel(PSGLOBAL(window), DISCL_FOREGROUND | (exclusive ? DISCL_EXCLUSIVE : DISCL_NONEXCLUSIVE)));
    WIN_FCHECK(PSGLOBAL(diMouse)->Acquire());
}

// 0x7485C0
void diPadInit() {
    rng::fill(PadConfigs, CPadConfig{});

    // Initialize devices (+ Set PSGLOBAL(diDeviceX) vars)
    WIN_FCHECK(PSGLOBAL(diInterface)->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumDevicesCallback, NULL, DIEDFL_ALLDEVICES));
    
    // Pirulax: Original code queried the capabilities [for pad 0] too, but did nothing with it, so I'll skip that.

    const auto InitializePad = [](LPDIRECTINPUTDEVICE8 dev, DWORD padNum) {
        if (dev == NULL) {
            return;
        }
        WIN_FCHECK(diPadSetRanges(dev, padNum));
        diPadSetPIDVID(dev, padNum);
        PadConfigs[padNum].present  = true;
        ControlsManager.InitDefaultControlConfigJoyPad(36u);
    };
    InitializePad(PSGLOBAL(diDevice1), 0);
    InitializePad(PSGLOBAL(diDevice2), 1);
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
// NOTSA(Grinch_): Directly returning CMouseControllerState
CMouseControllerState GetMouseState() {
    DIMOUSESTATE2 mouseState;
    CMouseControllerState state;

    if (PSGLOBAL(diMouse)) {
        if (SUCCEEDED(PSGLOBAL(diMouse)->GetDeviceState(sizeof(DIMOUSESTATE2), &mouseState))) {
            state.m_AmountMoved = CVector2D(static_cast<float>(mouseState.lX), static_cast<float>(mouseState.lY));
            state.m_bWheelMovedUp = (mouseState.lZ > 0);
            state.m_bWheelMovedDown = (mouseState.lZ < 0);
            state.m_bLeftButton = mouseState.rgbButtons[0] & 0x80;
            state.m_bRightButton = mouseState.rgbButtons[1] & 0x80;
            state.m_bMiddleButton = mouseState.rgbButtons[2] & 0x80;
            state.m_bMsFirstXButton = mouseState.rgbButtons[3] & 0x80;
            state.m_bMsSecondXButton = mouseState.rgbButtons[4] & 0x80;
        }
    } else {
		diMouseInit(!FrontEndMenuManager.m_bMenuActive && IsVideoModeExclusive());
    }

	return state;
}

void InjectHooks() {
    RH_ScopedCategory("Win");
    RH_ScopedNamespaceName("Input");

    //RH_ScopedGlobalInstall(Initialise, 0x7487CF, { .reversed = false }); 
    RH_ScopedGlobalInstall(diMouseInit, 0x7469A0, {.locked = true}); // Can't be hooked because it fails with ACCESS DENIED and crashes
    //RH_ScopedGlobalInstall(InitialiseJoys, 0x7485C0, {.reversed = false});
    RH_ScopedGlobalInstall(EnumDevicesCallback, 0x747020);
    RH_ScopedGlobalInstall(diPadInit, 0x7485C0);
    RH_ScopedGlobalInstall(diPadSetRanges, 0x746D80);
}

bool IsKeyPressed(unsigned int keyCode) {
    return (GetKeyState(keyCode) & 0x8000) != 0;
}

bool IsKeyDown(unsigned int keyCode) {
    return GetAsyncKeyState(keyCode) >> 0x8;
}
} // namespace WinInput
