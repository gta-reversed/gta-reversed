#pragma once

#include "DebugModule.h"

class CullZonesDebugModule final : public DebugModule {
public:
    void RenderWindow() override final;
    void Render3D() override final;
    void RenderMenuEntry() override final;

    NOTSA_IMPLEMENT_DEBUG_MODULE_SERIALIZATION(CullZonesDebugModule, m_IsOpen, m_ZOffset, m_DrawMirrors, m_DrawTunnels, m_DrawCulls);
    
private:
    bool m_IsOpen{};
    float m_ZOffset{};

    bool                 m_DrawMirrors{};
    CCullZoneReflection* m_MirrorCurrent{};
    uint32               m_MirrorCurrentIdx{};

    bool       m_DrawTunnels{};
    CCullZone* m_TunnelCurrent{};
    uint32     m_TunnelCurrentIdx{};

    bool       m_DrawCulls{};
    CCullZone* m_CullCurrent{};
    uint32     m_CullCurrentIdx{};
};
