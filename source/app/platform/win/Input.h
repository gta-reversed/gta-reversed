#pragma once

#include <Windows.h>
#include <dinput.h>

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
