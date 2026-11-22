#include "StdInc.h"
#include "DebugModule.h"

#include <imgui.h>

using namespace ImGui;

void DebugModuleSingleWindow::RenderWindow() {
    if (m_wndIsOpen) {
        SetNextWindowSize(m_wndDefaultSize, ImGuiCond_FirstUseEver);
        if (Begin(m_wndName, &m_wndIsOpen)) {
            RenderMainWindow();
        }
        End();
    }
}
