#include "StdInc.h"

#ifdef NOTSA_USE_SDL3
#include <SDL3/SDL.h>
#include "SDLWrapper.hpp"
#include <bindings/imgui_impl_sdl3.h>
#include <WindowedMode.hpp>
#include "PostEffects.h"
#include "UIRenderer.h"

namespace notsa {
namespace SDLWrapper {
bool Initialize() {
    return true;
}

void Terminate() {

}

void ProcessEvents() {
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
        case SDL_EVENT_WINDOW_RESIZED: {
            const auto w = e.window.data1,
                       h = e.window.data2;

            NOTSA_LOG_DEBUG(
                "SDL: Window resized: {} x {}",
                w, h
            );

            continue;
        }
        case SDL_EVENT_MOUSE_MOTION: {
            if (notsa::ui::UIRenderer::GetSingleton().IsActive()) {
                break;
            }
            static CVector2D s_MousePos{};
            if (FrontEndMenuManager.m_bMenuActive) {
                s_MousePos.x += e.motion.xrel * CCamera::m_fMouseAccelHorzntl * 100.f;
                s_MousePos.y += e.motion.yrel * CCamera::m_fMouseAccelVertical * 100.f;
            } else {
                s_MousePos.x = e.motion.x;
                s_MousePos.y = e.motion.y;
            }
            FrontEndMenuManager.m_nMousePosWinX = (int32)(s_MousePos.x);
            FrontEndMenuManager.m_nMousePosWinY = (int32)(s_MousePos.y);
            break;
        }
        }

        if (CPad::ProcessEvent(e, imIO && imIO->WantCaptureMouse, imIO && imIO->WantCaptureKeyboard)) {
            continue;
        }

        //NOTSA_LOG_DEBUG("SDL: Unprocessed event: {}", e.type);
    }
}
}; // namespace SDLWrapper
}; // namespace notsa
#endif
