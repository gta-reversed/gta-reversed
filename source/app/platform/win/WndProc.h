#pragma once

#include <windows.h>
#include <skeleton.h>
#include "winincl.h"

#ifndef NOTSA_USE_SDL3
BOOL GTATranslateShiftKey(RsKeyCodes*);
void InjectHooksWndProcStuff();
#endif
