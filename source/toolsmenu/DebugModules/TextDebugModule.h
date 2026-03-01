#pragma once

#include "DebugModule.h"

class TextDebugModule : public DebugModuleSingleWindow {
public:
    TextDebugModule();

    void RenderMainWindow() override final;
    void RenderMenuEntry() override final;

private:
    ImGuiTextFilter m_Filter{};
    uint32          m_FilterTextHash{};
};
