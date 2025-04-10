#pragma once

class CKeyboardState {
public:
    int16 FKeys[12];
    int16 standardKeys[256];
    int16 esc;
    int16 insert;
    int16 del;
    int16 home;
    int16 end;
    int16 pgup;
    int16 pgdn;
    int16 up;
    int16 down;
    int16 left;
    int16 right;
    int16 scroll;
    int16 pause;
    int16 numlock;
    int16 div;
    int16 mul;
    int16 sub;
    int16 add;
    int16 enter;
    int16 decimal;
    int16 num1;
    int16 num2;
    int16 num3;
    int16 num4;
    int16 num5;
    int16 num6;
    int16 num7;
    int16 num8;
    int16 num9;
    int16 num0;
    int16 back;
    int16 tab;
    int16 capslock;
    int16 extenter;
    int16 lshift;
    int16 rshift;
    int16 shift;
    int16 lctrl;
    int16 rctrl;
    int16 lmenu;
    int16 rmenu;
    int16 lwin;
    int16 rwin;
    int16 apps;

public:
    static void InjectHooks();

    void Clear();
};

VALIDATE_SIZE(CKeyboardState, 0x270);
