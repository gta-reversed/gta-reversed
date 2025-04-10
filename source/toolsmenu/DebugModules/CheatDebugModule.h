#pragma once

#include "DebugModule.h"

class CheatDebugModule final : public DebugModule {
public:
    void RenderWindow() override final;
    void RenderMenuEntry() override final;

    NOTSA_IMPLEMENT_DEBUG_MODULE_SERIALIZATION(CheatDebugModule, m_IsOpen);

private:
    bool m_IsOpen{ false };
};
