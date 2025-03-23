/*
* This file contains ps* function implementations for Windows
*/

#include "StdInc.h"

#include "WndProc.h"
#include "WinMain.h"
#include "Input.h"

#include "Gamma.h"

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

void WinPsInjectHooks();
void Win32InjectHooks() {
    RH_ScopedCategory("Win");
    RH_ScopedNamespaceName("Win");
    
    InjectHooksWndProcStuff();
    InjectWinMainStuff();
    WinPsInjectHooks();
    WinInput::InjectHooks();
}
