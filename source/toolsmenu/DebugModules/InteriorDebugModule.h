#pragma once

#include "DebugModule.h"

class Interior_c;

namespace notsa {
namespace debugmodules {
class InteriorDebugModule final : public DebugModule {
public:
    void RenderWindow() override;
    void RenderMenuEntry() override;

private:
    void RenderSelectorTab();
    void RenderPropsTab();

private:
    bool m_IsOpen{}; // Main window's state
    bool m_ShowClosest{};
    bool m_UsePlayersInt{true};
    int  m_SelectedIntIdx{};
    Interior_c* m_Int{};
};
}; // namespace debugmodules
}; // namespace notsa
