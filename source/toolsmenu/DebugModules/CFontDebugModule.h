#pragma once

#include "DebugModule.h"

class FontDebugModule final : public DebugModule {
public:
    void RenderWindow() override final;
    void RenderMenuEntry() override final;

//    NOTSA_IMPLEMENT_DEBUG_MODULE_SERIALIZATION(DarkelDebugModule, m_WeaponType, m_TimeLimit, m_NumKillsNeeded, m_ModelsToKill, m_ModelsToKill, m_StandardSoundAndMsg, m_IsHeadshotRequired);

private:
    bool m_IsOpen{};
};
