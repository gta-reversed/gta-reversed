#include "StdInc.h"
#include "ImGuiDebugModule.hpp"
#include "UIRenderer.h"

using namespace ImGui;

namespace notsa {
namespace debugmodules {
void ImGuiDebugModule::RenderWindow() {
    if (m_MetricsOpen) {
        ShowMetricsWindow(&m_MetricsOpen);
    }
    if (m_AboutOpen) {
        ShowAboutWindow(&m_AboutOpen);
    }
    if (m_DemoOpen) {
        ShowDemoWindow(&m_DemoOpen);
    }
    if (m_RequestedRestart) {
        const notsa::ui::ScopedWindow wnd{ "Confirm UI Renderer Restart", {200.f, 150.f}, m_RequestedRestart, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize };
        if (m_RequestedRestart) {
            if (Button("Confirm")) {
                ::notsa::ui::UIRenderer::GetSingleton().RequestReInit();
            }
            SameLine();
            if (Button("Decline")) {
                m_RequestedRestart = false;
            }
        }
    }
}

void ImGuiDebugModule::RenderMenuEntry() {
    notsa::ui::DoNestedMenuIL({ "Stats" }, [&] {
        MenuItem("ImGui", nullptr, &m_MetricsOpen);
    });

    notsa::ui::DoNestedMenuIL({ "Extra", "ImGui"}, [&] {
        MenuItem("About", nullptr, &m_AboutOpen);
        MenuItem("Demo", nullptr, &m_DemoOpen);
        MenuItem("Restart UI Renderer", nullptr, &m_RequestedRestart);
    });
}
}; // namespace debugmodules
}; // namespace notsa
