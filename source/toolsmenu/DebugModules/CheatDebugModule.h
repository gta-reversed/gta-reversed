#pragma once

#include "DebugModule.h"

class CheatDebugModule final : public DebugModule {
public:
    void RenderWindow() override;
    void RenderMenuEntry() override;
    void Update() override;

    NOTSA_IMPLEMENT_DEBUG_MODULE_SERIALIZATION(CheatDebugModule, m_IsOpen);

private:
    bool m_IsOpen{ false };
    bool m_GodMode{};
};
