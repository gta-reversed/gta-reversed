#pragma once

#include "DebugModule.h"

class CPickupsDebugModule final : public DebugModule {
public:
    void RenderWindow() override final;
    void RenderMenuEntry() override final;

    NOTSA_IMPLEMENT_DEBUG_MODULE_SERIALIZATION(CPickupsDebugModule, m_IsOpen, m_FilterInactive, m_FilterInvisible, m_SelectedPickupIdx);

private:
    void DrawTable();

private:
    bool  m_IsOpen{};
    bool  m_FilterInactive{true};
    bool  m_FilterInvisible{};
    int32 m_SelectedPickupIdx{};
};
