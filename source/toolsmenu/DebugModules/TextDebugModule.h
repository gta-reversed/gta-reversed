#pragma once

#include "DebugModule.h"

class TextDebugModule final : public DebugModuleSingleWindow {
public:
    TextDebugModule();

    void RenderMainWindow() override;
    void RenderMenuEntry() override;

private:
    bool            m_FilterByKey{ false };
    ImGuiTextFilter m_Filter{};
    uint32          m_FilterKeyHash{};
};
