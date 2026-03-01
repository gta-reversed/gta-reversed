#pragma once

#include "DebugModule.h"

class AudioZonesDebugModule final : public DebugModule {
public:
    void Render3D() override final;
    void RenderMenuEntry() override final;

    NOTSA_IMPLEMENT_DEBUG_MODULE_SERIALIZATION(AudioZonesDebugModule, m_IsOpen, m_ShowAudioZones);

private:
    bool m_IsOpen{};
    bool m_ShowAudioZones{};

    CMatrix m_Transform = CMatrix::GetIdentity();
};
