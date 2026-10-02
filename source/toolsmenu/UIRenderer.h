#pragma once

#include "DebugModules/DebugModules.h"
#include <extensions/Singleton.hpp>

namespace notsa {
namespace ui {
class UIRenderer : public notsa::Singleton<UIRenderer> {
public:
    UIRenderer();
    ~UIRenderer();


    //! Request restart of render (done on before frame)
    void RequestReInit() { m_ReInitRequested = true; }

    //! Same as ImGui::GetIO(), but won't crash the code if called before ctx is created
    auto GetImIO() const { return m_ImIO; }

    //! Get debug modules manager
    auto& GetDebugModules() { return m_DebugModules; }

    //! Set active
    void SetIsActive(bool active);

    //! Is active
    bool IsActive() { return m_InputActive; }

private:
    //! Render 3D stuff in the world (If rendered elsewhere it won't be visible)
    void Render3D();

    //! Post render updates (Before DirectX scene (`RwCameraBeginUpdate` & `RwCameraEndUpdate`))
    void PreRender();

    //! Render UI to ImGui frame (Will be rendered later in `Render2D`)
    void RenderImGui();

    //! Render 2D stuff (Also used to render the UI itself)
    void Render2D();

    //! Post render updates (After DirectX `Present` (`RwCameraShowRaster`))
    void PostRender();

    //! Random code you want to run (Called from `PreRender`)
    void DebugCode();

private:
    friend void ::RenderEffects();  // For `Render3D()`
    friend void ::FrontendIdle();   // For `DrawLoop()` VVV
    friend void ::Idle(void*);      // For `DrawLoop()` Yes, called in 2 places, but they are never called in the same frame (As `Idle` is called when not in the menu only)
    friend void ::CPad::UpdatePads();
    friend void ::Idle(void*);

private:
    bool          m_Initialised{};
    bool          m_InputActive{};
    bool          m_ReInitRequested{};
    ImGuiContext* m_ImCtx{};
    ImGuiIO*      m_ImIO{};
    DebugModules  m_DebugModules{ m_ImCtx };
    ImVec2        m_LastMousePos{};
};
}; // namespace ui
}; // namespace notsa
