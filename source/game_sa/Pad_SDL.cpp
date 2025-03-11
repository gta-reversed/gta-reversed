#include <StdInc.h>

#ifdef NOTSA_USE_SDL3
#include <SDL3/SDL.h>

bool CPad::ProcessMouseEvent(const SDL_Event& e, CMouseControllerState& ms) {
    switch (e.type) {
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
    case SDL_EVENT_MOUSE_BUTTON_UP: {
        switch (e.button.button) {
        case SDL_BUTTON_LEFT:   ms.lmb = e.button.down;  break;
        case SDL_BUTTON_RIGHT:  ms.rmb = e.button.down;  break;
        case SDL_BUTTON_MIDDLE: ms.mmb = e.button.down;  break;
        case SDL_BUTTON_X1:     ms.bmx1 = e.button.down; break;
        case SDL_BUTTON_X2:     ms.bmx2 = e.button.down; break;
        }
        return true;
    }
    case SDL_EVENT_MOUSE_WHEEL: {
        ms.Z        += e.wheel.y;
        ms.wheelUp   = e.wheel.y > 0.f;
        ms.wheelDown = e.wheel.y < 0.f;
        return true;
    }
    case SDL_EVENT_MOUSE_MOTION: {
        ms.X = e.motion.xrel;
        ms.Y = e.motion.yrel;
        return true;
    }
    case SDL_EVENT_MOUSE_ADDED:
    case SDL_EVENT_MOUSE_REMOVED: {
        NOTSA_LOG_DEBUG(
            "SDL: Mouse ({}) {}",
            e.mdevice.which,
            e.mdevice.type == SDL_EVENT_MOUSE_ADDED ? "added" : "removed"
        );
        return true;
    }
    }
    return false;
}

bool CPad::ProcessKeyboardEvent(const SDL_Event& e, CKeyboardState& ks) {
    switch (e.type) {
    case SDL_EVENT_KEY_DOWN:
    case SDL_EVENT_KEY_UP: {
        if (e.key.repeat) {
            return true;
        }

        NOTSA_LOG_DEBUG("SDL: Key ({}): {}", (char)(e.key.key), e.key.down);

        const auto s = e.key.down ? 255 : 0;

        // Keypad doesn't have individual key values in SDL3
        switch (e.key.scancode) { 
        case SDL_SCANCODE_KP_0: ks.num0 = s; return true;
        case SDL_SCANCODE_KP_1: ks.num1 = s; return true;
        case SDL_SCANCODE_KP_2: ks.num2 = s; return true;
        case SDL_SCANCODE_KP_3: ks.num3 = s; return true;
        case SDL_SCANCODE_KP_4: ks.num4 = s; return true;
        case SDL_SCANCODE_KP_5: ks.num5 = s; return true;
        case SDL_SCANCODE_KP_6: ks.num6 = s; return true;
        case SDL_SCANCODE_KP_7: ks.num7 = s; return true;
        case SDL_SCANCODE_KP_8: ks.num8 = s; return true;
        case SDL_SCANCODE_KP_9: ks.num9 = s; return true;
        }

        switch (e.key.key) {
        case SDLK_F1:           ks.FKeys[0] = s;                break;                     
        case SDLK_F2:           ks.FKeys[1] = s;                break;                     
        case SDLK_F3:           ks.FKeys[2] = s;                break;                     
        case SDLK_F4:           ks.FKeys[3] = s;                break;                     
        case SDLK_F5:           ks.FKeys[4] = s;                break;                     
        case SDLK_F6:           ks.FKeys[5] = s;                break;                     
        case SDLK_F7:           ks.FKeys[6] = s;                break;                     
        case SDLK_F8:           ks.FKeys[7] = s;                break;                     
        case SDLK_F9:           ks.FKeys[8] = s;                break;                     
        case SDLK_F10:          ks.FKeys[9] = s;                break;                    
        case SDLK_F11:          ks.FKeys[10] = s;               break;                    
        case SDLK_F12:          ks.FKeys[11] = s;               break;
        case SDLK_ESCAPE:       ks.esc = s;                     break;
        case SDLK_INSERT:       ks.insert = s;                  break;
        case SDLK_DELETE:       ks.del = s;                     break;
        case SDLK_HOME:         ks.home = s;                    break;
        case SDLK_END:          ks.end = s;                     break;
        case SDLK_PAGEUP:       ks.pgup = s;                    break;
        case SDLK_PAGEDOWN:     ks.pgdn = s;                    break;
        case SDLK_UP:           ks.up = s;                      break; // Arrow up
        case SDLK_DOWN:         ks.down = s;                    break; // Arrow down
        case SDLK_LEFT:         ks.left = s;                    break; // Arrow left
        case SDLK_RIGHT:        ks.right = s;                   break; // Arrow right
        case SDLK_SCROLLLOCK:   ks.scroll = s;                  break;
        case SDLK_PAUSE:        ks.pause = s;                   break;
        case SDLK_NUMLOCKCLEAR: ks.numlock = s;                 break; // (?)
        case SDLK_SLASH:        ks.div = s;                     break;
        case SDLK_ASTERISK:     ks.mul = s;                     break;
        case SDLK_MINUS:        ks.sub = s;                     break;
        case SDLK_PLUS:         ks.add = s;                     break;
        case SDLK_RETURN:       ks.enter = s;                   break;
        case SDLK_PERIOD:       ks.decimal = s;                 break;
        case SDLK_TAB:          ks.tab = s;                     break;
        case SDLK_CAPSLOCK:     ks.capslock = s;                break;
        case SDLK_KP_ENTER:     ks.extenter = s;                break; // (?)
        case SDLK_LSHIFT:       ks.lshift = s;                  break;
        case SDLK_RSHIFT:       ks.rshift = s;                  break;
        case SDLK_LCTRL:        ks.lctrl = s;                   break;
        case SDLK_RCTRL:        ks.rctrl = s;                   break;
        case SDLK_LALT:         ks.lalt = s;                    break;
        case SDLK_RALT:         ks.ralt = s;                    break;
        case SDLK_LGUI:         ks.lwin = s;                    break;
        case SDLK_RGUI:         ks.rwin = s;                    break;
        case SDLK_APPLICATION:  ks.apps = s;                    break; // (?)
        default: { // Other keys
            const auto key = std::toupper(e.key.key);
            if (key <= 0xFF) {
                ks.standardKeys[key] = s;
            }
            return true;
        }
        }
        ks.shift = (ks.rshift || ks.lshift) ? 255 : 0;
        return true;
    }
    case SDL_EVENT_KEYBOARD_ADDED:
    case SDL_EVENT_KEYBOARD_REMOVED: {
        NOTSA_LOG_DEBUG(
            "SDL: Keyboard ({}) {}",
            e.kdevice.which,
            e.kdevice.type == SDL_EVENT_KEYBOARD_ADDED ? "added" : "removed"
        );
        return true;
    }
    }
    return false;
}

bool CPad::ProcessGamepadEvent(const SDL_Event & e, CControllerState& cs) {
    //switch (e.type) {
    //case SDL_EVENT_GAMEPAD_AXIS_MOTION: {
    //    const auto UpdateAxis = [&](int16& outA, int16& outB, bool isInverted, bool isSwapped) {
    //        int16 pos = e.gaxis.value;
    //        if (fabs(pos) > 0.3f) {
    //            pos = isInverted ? -pos : pos;
    //            pos /= 128; // SDL values are [-32768 to 32767], need to map to [-128, 127] (Actually should be [-128, 128] (?))
    //            (isSwapped ? outA : outB) = pos;
    //        }
    //    };
    //    switch (e.gaxis.axis) {
    //    case SDL_GAMEPAD_AXIS_LEFTX:
    //    case SDL_GAMEPAD_AXIS_LEFTY:
    //    case SDL_GAMEPAD_AXIS_RIGHTX:
    //    case SDL_GAMEPAD_AXIS_RIGHTY:
    //    }
    //        
    //    UpdateAxis(cs.LeftStickY, cs.LeftStickX, FrontEndMenuManager.m_bInvertPadX1, FrontEndMenuManager.m_bSwapPadAxis1);
    //    UpdateAxis(cs.LeftStickX, cs.LeftStickY, FrontEndMenuManager.m_bInvertPadY1, FrontEndMenuManager.m_bSwapPadAxis2);
    //
    //    UpdateAxis(cs.LeftStickY, cs.LeftStickX, FrontEndMenuManager.m_bInvertPadX2, FrontEndMenuManager.m_bSwapPadAxis1);
    //    UpdateAxis(cs.LeftStickX, cs.LeftStickY, FrontEndMenuManager.m_bInvertPadY2, FrontEndMenuManager.m_bSwapPadAxis2);
    //    return true;
    //}
    //}
    return false;
}

bool CPad::ProcessJoyStickEvent(const SDL_Event& e, CControllerState& cs) {
    return false;
}

bool CPad::ProcessEvent(const SDL_Event& e, bool ignoreMouseEvents, bool ignoreKeyboardEvents) {
    NOTSA_LOG_DEBUG("ignoreKeyboardEvents: {}", ignoreKeyboardEvents);
    NOTSA_LOG_DEBUG("ignoreMouseEvents: {}", ignoreMouseEvents);

    // Mouse/keyboard events are meant to be handled by Pad 0
    if (!ignoreMouseEvents && GetPad(0)->ProcessMouseEvent(e, TempMouseControllerState)) {
        return true;
    }
    if (!ignoreKeyboardEvents && GetPad(0)->ProcessKeyboardEvent(e, TempKeyState)) {
        return true;
    }

    // Other events (I guess) can be handled by any pad
    for (auto& pad : Pads) {
        if (pad.ProcessJoyStickEvent(e, pad.PCTempJoyState)) {
            return true;
        }
    }

    // Event not processed
    return false;
}
#endif
