#pragma once

#include "winincl.h"

#ifndef NOTSA_USE_SDL3
LRESULT CALLBACK __MainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void InjectHooksWndProcStuff();
#endif
