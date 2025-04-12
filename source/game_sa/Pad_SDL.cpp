#include <StdInc.h>

#ifdef NOTSA_USE_SDL3
#include <SDL3/SDL.h>

bool CPad::ProcessMouseEvent(const SDL_Event& e, CMouseControllerState& ms) {
    switch (e.type) {
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
    case SDL_EVENT_MOUSE_BUTTON_UP: {
        switch (e.button.button) {
        case SDL_BUTTON_LEFT:   ms.isMouseLeftButtonPressed = e.button.down;  break;
        case SDL_BUTTON_RIGHT:  ms.isMouseRightButtonPressed = e.button.down;  break;
        case SDL_BUTTON_MIDDLE: ms.isMouseMiddleButtonPressed = e.button.down;  break;
        case SDL_BUTTON_X1:     ms.isMouseFirstXPressed = e.button.down; break;
        case SDL_BUTTON_X2:     ms.isMouseSecondXPressed = e.button.down; break;
        }
        return true;
    }
    case SDL_EVENT_MOUSE_WHEEL: {
        ms.isMouseWheelMovedUp   = e.wheel.y > 0.f;
        ms.isMouseWheelMovedDown = e.wheel.y < 0.f;
        return true;
    }
    case SDL_EVENT_MOUSE_MOTION: {
        ms.m_AmountMoved.x = e.motion.xrel;
        ms.m_AmountMoved.y = e.motion.yrel;
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

        //NOTSA_LOG_DEBUG("SDL: Key ({}): {}", (char)(e.key.key), e.key.down);

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

// New function to directly poll the joystick state
void CPad::UpdateJoystick(CControllerState& cs) {
#ifdef NOTSA_USE_SDL3
    // Get all available gamepads
    int sdl_gamepads_count = 0;
    SDL_JoystickID* sdl_gamepads = SDL_GetGamepads(&sdl_gamepads_count);
    
    if (sdl_gamepads_count > 0) {
        // Select the first gamepad (or a specific one by ID)
        SDL_Gamepad* gamepad = SDL_OpenGamepad(sdl_gamepads[0]);
        if (gamepad) {
            // Update analog axes
            const auto UpdateAxis = [&](int16& outA, int16& outB, SDL_GamepadAxis axis, bool isInverted, bool isSwapped) {
                float value = SDL_GetGamepadAxis(gamepad, axis) / 256.0f; // Convert to comparable range
                if (fabs(value) > 0.3f) { // Threshold to prevent drift
                    value = isInverted ? -value : value;
                    (isSwapped ? outA : outB) = static_cast<int16>(value);
                }
            };
            
            // Update analog axes
            UpdateAxis(cs.LeftStickY, cs.LeftStickX, SDL_GAMEPAD_AXIS_LEFTX, FrontEndMenuManager.m_bInvertPadX1, FrontEndMenuManager.m_bSwapPadAxis1);
            UpdateAxis(cs.LeftStickX, cs.LeftStickY, SDL_GAMEPAD_AXIS_LEFTY, FrontEndMenuManager.m_bInvertPadY1, FrontEndMenuManager.m_bSwapPadAxis2);
            UpdateAxis(cs.RightStickY, cs.RightStickX, SDL_GAMEPAD_AXIS_RIGHTX, FrontEndMenuManager.m_bInvertPadX2, FrontEndMenuManager.m_bSwapPadAxis1);
            UpdateAxis(cs.RightStickX, cs.RightStickY, SDL_GAMEPAD_AXIS_RIGHTY, FrontEndMenuManager.m_bInvertPadY2, FrontEndMenuManager.m_bSwapPadAxis2);
            
            // Update triggers
            cs.LeftShoulder2 = static_cast<uint8>(SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_LEFT_TRIGGER) > 1000 ? 255 : 0);
            cs.RightShoulder2 = static_cast<uint8>(SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_RIGHT_TRIGGER) > 1000 ? 255 : 0);
            
            // Update buttons
            cs.ButtonCross = SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_SOUTH) ? 255 : 0;
            cs.ButtonCircle = SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_EAST) ? 255 : 0;
            cs.ButtonSquare = SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_WEST) ? 255 : 0;
            cs.ButtonTriangle = SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_NORTH) ? 255 : 0;
            cs.Select = SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_BACK) ? 255 : 0;
            cs.Start = SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_START) ? 255 : 0;
            cs.DPadUp = SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_DPAD_UP) ? 255 : 0;
            cs.DPadDown = SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_DPAD_DOWN) ? 255 : 0;
            cs.DPadLeft = SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_DPAD_LEFT) ? 255 : 0;
            cs.DPadRight = SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_DPAD_RIGHT) ? 255 : 0;
            cs.LeftShoulder1 = SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_LEFT_SHOULDER) ? 255 : 0;
            cs.RightShoulder1 = SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER) ? 255 : 0;
            cs.ShockButtonL = SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_LEFT_STICK) ? 255 : 0;
            cs.ShockButtonR = SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_RIGHT_STICK) ? 255 : 0;
            
            // Close the gamepad
            SDL_CloseGamepad(gamepad);
        }
        SDL_free(sdl_gamepads);
    }
#endif
}

bool CPad::ProcessEvent(const SDL_Event& e, bool ignoreMouseEvents, bool ignoreKeyboardEvents) {
    // Mouse/keyboard events are meant to be handled by Pad 0
    if (!ignoreMouseEvents && GetPad(0)->ProcessMouseEvent(e, PCTempMouseControllerState)) {
        return true;
    }
    if (!ignoreKeyboardEvents && GetPad(0)->ProcessKeyboardEvent(e, TempKeyState)) {
        return true;
    }

    // Event not processed
    return false;
}
#endif
