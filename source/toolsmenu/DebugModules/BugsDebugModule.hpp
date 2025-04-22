#pragma once

#include "DebugModule.h"

namespace notsa { 
namespace debugmodules {
class BugsDebugModule final : public DebugModule {
public:
    void RenderWindow() override;
    void RenderMenuEntry() override; 

    NOTSA_IMPLEMENT_DEBUG_MODULE_SERIALIZATION(BugsDebugModule, m_IsOpen);

private:
    bool m_IsOpen{};
    ImGuiTextFilter NameFilter;
};
}; // namespace debugmodules
}; // namespace notsa
