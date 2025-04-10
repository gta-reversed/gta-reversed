#pragma once

#include <windows.h>

LRESULT CALLBACK __MainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void InjectHooksWndProcStuff();
