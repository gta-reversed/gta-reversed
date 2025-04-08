#pragma once

#include "winincl.h"
#include <dinput.h>
#include <windows.h>

namespace WinInput {

void diMouseInit(bool exclusive);
void diPadInit();

BOOL CALLBACK EnumDevicesCallback(LPCDIDEVICEINSTANCEA pInst, LPVOID);
CMouseControllerState GetMouseState();

void InjectHooks();
bool Initialise();

bool IsKeyDown(unsigned int keyCode);
bool IsKeyPressed(unsigned int keyCode);

HRESULT Shutdown();
static inline HWND g_windowHandle = nullptr;

}; // namespace WinInput
