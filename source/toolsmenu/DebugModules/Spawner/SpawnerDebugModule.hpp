#pragma once

#include "../DebugModule.h"

class SpawnerDebugModule final : public DebugModule {
public:
    SpawnerDebugModule();

    void RenderWindow() override final;
    void RenderMenuEntry() override final;

    NOTSA_IMPLEMENT_DEBUG_MODULE_SERIALIZATION(SpawnerDebugModule, m_IsOpen); // Good enough for now (Would need a refactor for other stuff....)

private:
    bool m_IsOpen{};
};
