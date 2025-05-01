#pragma once

#include "DebugModule.h"

class COcclusionDebugModule final : public DebugModule {
public:
    void RenderWindow() override final;
    void RenderMenuEntry() override final;
    void Render3D() override final;

    NOTSA_IMPLEMENT_DEBUG_MODULE_SERIALIZATION(COcclusionDebugModule, m_IsOpen, m_DrawActiveOcclusions);

private:
    bool m_IsOpen{};
    bool m_DrawActiveOcclusions;
};
