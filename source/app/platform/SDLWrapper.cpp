#include "StdInc.h"

#include <game_sa/MouseControllerState.h>
#include <game_sa/KeyboardState.h>
#include <SDL3/SDL.h>
#include "SDLWrapper.hpp"
#include <bindings/imgui_impl_sdl3.h>

/* These are later given to `CPad` to update shit */
CMouseControllerState MouseState{};
CKeyboardState        KeyboardState{};

bool s_IsInitialized = false;

namespace {
bool ProcessMouseEvent(SDL_Event& e, CMouseControllerState& ms) {
    switch (e.type) {
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
    case SDL_EVENT_MOUSE_BUTTON_UP: {
        switch (e.button.button) {
        case SDL_BUTTON_LEFT:   ms.lmb = e.button.down;  break;
        case SDL_BUTTON_MIDDLE: ms.rmb = e.button.down;  break;
        case SDL_BUTTON_RIGHT:  ms.mmb = e.button.down;  break;
        case SDL_BUTTON_X1:     ms.bmx1 = e.button.down; break;
        case SDL_BUTTON_X2:     ms.bmx2 = e.button.down; break;
        }
        return true;
    }
    case SDL_EVENT_MOUSE_WHEEL: {
        ms.Z += e.wheel.y;
        return true;
    }
    case SDL_EVENT_MOUSE_MOTION: {
        ms.X = e.motion.xrel;
        ms.Y = e.motion.yrel;

        NOTSA_LOG_DEBUG("SDL_EVENT_MOUSE_MOTION: x: {:.2f}, y: {:.2f}", ms.X, ms.Y);
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
bool ProcessKeyboardEvent(SDL_Event& e, CKeyboardState& ks) {
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
        case SDLK_0:            ks.num0 = s;                    break;
        case SDLK_1:            ks.num1 = s;                    break;
        case SDLK_2:            ks.num2 = s;                    break;
        case SDLK_3:            ks.num3 = s;                    break;
        case SDLK_4:            ks.num4 = s;                    break;
        case SDLK_5:            ks.num5 = s;                    break;
        case SDLK_6:            ks.num6 = s;                    break;
        case SDLK_7:            ks.num7 = s;                    break;
        case SDLK_8:            ks.num8 = s;                    break;
        case SDLK_9:            ks.num9 = s;                    break;
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
        case SDLK_MENU:         ks.rmenu = ks.lmenu = s;        break;
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
bool ProcessGamepadEvent(SDL_Event & e, CControllerState& cs) {
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
bool ProcessJoyStickEvent(SDL_Event& e, CControllerState& cs) {
    return false;
}
};

namespace notsa {
namespace SDLWrapper {
bool Initialize() {
    return true;
}

void Terminate() {

}

void ProcessEvents() {
    // Sync Window state to SDL
#if 0
    {
        const auto sdlWnd = (SDL_Window*)(PSGLOBAL(sdlWindow));

        // Sync size
        const auto LastScreenWidth = StaticRef<int32>(0xC9BEE4);
        const auto LastScreenHeight = StaticRef<int32>(0xC9BEE8);
        SDL_SetWindowSize(sdlWnd, LastScreenWidth, LastScreenHeight);
        sdl_setsize
        // Sync pos
        tagRECT r{
            .left = 0,
            .top = 0,
            .right = LastScreenWidth,
            .bottom = LastScreenHeight
        };
        AdjustWindowRectEx(
            &r,
            GetWindowLongA(PSGLOBAL(window), -16),
            GetMenu(PSGLOBAL(window)) != 0,
            GetWindowLongA(PSGLOBAL(window), -20)
        );
        //SetWindowPos(PSGLOBAL(window), 0, r.left, r.bottom, r.right - r.left, r.bottom - r.top, 0x26u);
        //GetClientRect(PSGLOBAL(window), &r);
        //GetClientRect(PSGLOBAL(window), &r);
        //GetWindowRect(PSGLOBAL(window), &r);
        //AdjustWindowRect(&r, WS_TILED | WS_CAPTION, false);
        //SDL_SetWindowPosition(sdlWnd, r.left, r.top);

        SDL_SetWindowPosition((SDL_Window*)(PSGLOBAL(sdlWindow)), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    }
#endif

    // These values are deltas, thus they must be cleared each frame
    MouseState.X = 0.f;
    MouseState.Y = 0.f;
    MouseState.Z = 0.f;

    // Now process events
    const auto* const imCtx = ImGui::GetCurrentContext();
    const auto* const imIO  = imCtx ? &imCtx->IO : nullptr;
    for (SDL_Event e; SDL_PollEvent(&e);) {
        if (imIO) {
            ImGui_ImplSDL3_ProcessEvent(&e);
        }

        switch (e.type) {
        case SDL_EVENT_QUIT: {
            RsGlobal.quit = true;
            continue;
        }
        }

        if ((!imIO || !imIO->WantCaptureMouse) && ProcessMouseEvent(e, MouseState)) {
            continue;
        }
        if ((!imIO || !imIO->WantCaptureKeyboard) && ProcessKeyboardEvent(e, KeyboardState)) {
            continue;
        }

        NOTSA_LOG_DEBUG("SDL: Unprocessed event: {}", e.type);
    }
}

CMouseControllerState& GetMouseState() {
    return MouseState;
}

CKeyboardState& GetKeyboardState() {
    return KeyboardState;
}
}; // namespace SDLWrapper
}; // namespace notsa
