#pragma once

#include "DebugModule.h"

namespace notsa {
namespace debugmodules {
class ImGuiDebugModule final : public DebugModule {
public:
    void RenderWindow() override final;
    void RenderMenuEntry() override final;

    NOTSA_IMPLEMENT_DEBUG_MODULE_SERIALIZATION(ImGuiDebugModule, m_MetricsOpen, m_AboutOpen, m_DemoOpen);

private:
    bool m_MetricsOpen{};
    bool m_AboutOpen{};
    bool m_DemoOpen{};
    bool m_RequestedRestart{};
};
}; // namespace debugmodules
}; // namespace notsa
