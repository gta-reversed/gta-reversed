#pragma once

#include <windows.h>
#include <skeleton.h>

LRESULT CALLBACK __MainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL GTATranslateShiftKey(RsKeyCodes*);
void InjectHooksWndProcStuff();
