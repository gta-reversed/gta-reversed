#pragma once

#include "DebugModule.h"

class TextDebugModule : public DebugModuleSingleWindow {
public:
    TextDebugModule();

    void RenderMainWindow() override final;
    void RenderMenuEntry() override final;

private:
    char   m_KeyFilter[64]{};
    uint32 m_KeyFilterHash{};
};
