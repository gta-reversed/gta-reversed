#include "StdInc.h"

#include "KeyboardState.h"

void CKeyboardState::InjectHooks() {
    RH_ScopedClass(CKeyboardState);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Clear, 0x53F090);
}

// 0x53F090
void CKeyboardState::Clear() {
    memset(this, 0, 24);
    memset(standardKeys, 0, sizeof(standardKeys));

    pgdn = 0;
    pgup = 0;
    end = 0;
    home = 0;
    del = 0;
    insert = 0;
    esc = 0;
    right = 0;
    left = 0;
    down = 0;
    up = 0;
    numlock = 0;
    add = 0;
    sub = 0;
    mul = 0;
    div = 0;
    num4 = 0;
    num3 = 0;
    num2 = 0;
    num1 = 0;
    decimal = 0;
    num8 = 0;
    num7 = 0;
    num6 = 0;
    num5 = 0;
    pause = 0;
    scroll = 0;
    num0 = 0;
    num9 = 0;
    extenter = 0;
    capslock = 0;
    tab = 0;
    back = 0;
    rmenu = 0;
    lmenu = 0;
    rctrl = 0;
    lctrl = 0;
    rshift = 0;
    shift = 0;
    lshift = 0;
    apps = 0;
    rwin = 0;
    lwin = 0;
}
