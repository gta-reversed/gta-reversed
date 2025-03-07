#pragma once

class CKeyboardState {
public:
    int16 FKeys[12]{};         //!< F1 - F12 (0-11)
    int16 standardKeys[256]{}; //!< All other keys with keycode < 0xFF
    int16 esc{};               //!< Escape
    int16 insert{};            //!< Insert
    int16 del{};               //!< Delete
    int16 home{};              //!< Home
    int16 end{};               //!< End
    int16 pgup{};              //!< Page up
    int16 pgdn{};              //!< Page down
    int16 up{};                //!< Arrow up
    int16 down{};              //!< Arrow down
    int16 left{};              //!< Arrow left
    int16 right{};             //!< Arrow right
    int16 scroll{};            //!< Scroll lock (?)
    int16 pause{};             //!< Pause
    int16 numlock{};           //!< Numlock
    int16 div{};               //!< `/`
    int16 mul{};               //!< `*`
    int16 sub{};               //!< `-`
    int16 add{};               //!<  `+`
    int16 enter{};             //!< Enter (Regular)
    int16 decimal{};           //!<
    int16 num1{};              //!< Numpad 1
    int16 num2{};              //!< Numpad 2
    int16 num3{};              //!< Numpad 3
    int16 num4{};              //!< Numpad 4
    int16 num5{};              //!< Numpad 5
    int16 num6{};              //!< Numpad 6
    int16 num7{};              //!< Numpad 7
    int16 num8{};              //!< Numpad 8
    int16 num9{};              //!< Numpad 9
    int16 num0{};              //!< Numpad 0
    int16 back{};              //!< Return?
    int16 tab{};               //!< Tab
    int16 capslock{};          //!< Capslock
    int16 extenter{};          //!< Enter (Numpad?)
    int16 lshift{};            //!< Left shit
    int16 rshift{};            //!< Right shift
    int16 shift{};             //!< Shift keys combined (lshift | rshift)
    int16 lctrl{};             //!< Left ctrl
    int16 rctrl{};             //!< Right ctrl
    int16 lmenu{};             //!< ?
    int16 rmenu{};             //!< ?
    int16 lwin{};              //!< Left windows key
    int16 rwin{};              //!< Right windows key
    int16 apps{};              //!< Apps

public:
    static void InjectHooks();

    void Clear();
};

VALIDATE_SIZE(CKeyboardState, 0x270);
