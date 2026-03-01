#pragma once

#include "DebugModule.h"

namespace notsa { 
namespace debugmodules {
class CheckpointsDebugModule final : public DebugModule {
public:
    void RenderWindow() final override;
    void RenderMenuEntry() final override; 

    NOTSA_IMPLEMENT_DEBUG_MODULE_SERIALIZATION(CheckpointsDebugModule, m_IsOpen, m_SelectedTypeIdx, m_CurrentID, m_Pos, m_UsePlayersPosition, m_UsePlayersDir, m_Size, m_Color, m_PulsePeriod, m_PulseFraction, m_RotateRate, m_PlaceOneRandom);

private:
    bool       m_IsOpen{};
    int        m_SelectedTypeIdx{};
    int        m_CurrentID{1};
    CVector    m_Pos{};
    bool       m_UsePlayersPosition{true};
    CVector    m_Dir{};
    bool       m_UsePlayersDir{true};
    float      m_Size{1.f};
    RwRGBAReal m_Color{ 1.f, 0.f, 0.f, 1.f };
    int        m_PulsePeriod{1'024};
    float      m_PulseFraction{0.25f};
    int        m_RotateRate{5};
    bool       m_PlaceOneRandom{};
};
}; // namespace debugmodules
}; // namespace notsa
