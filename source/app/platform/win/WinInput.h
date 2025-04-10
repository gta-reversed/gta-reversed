#pragma once

#ifndef NOTSA_USE_SDL3
#include <dinput.h>
#include "winincl.h"

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

}; // namespace WinInput
#endif // NOTSA_USE_DINPUT

