#pragma once

#include "winincl.h"

LRESULT CALLBACK __MainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void InjectHooksWndProcStuff();
