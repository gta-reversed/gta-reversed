#pragma once

#include "DebugModule.h"

class DarkelDebugModule final : public DebugModule {
public:
    void RenderWindow() override final;
    void RenderMenuEntry() override final;

    NOTSA_IMPLEMENT_DEBUG_MODULE_SERIALIZATION(DarkelDebugModule, m_WeaponType, m_TimeLimit, m_NumKillsNeeded, m_ModelsToKill, m_ModelsToKill, m_StandardSoundAndMsg, m_IsHeadshotRequired);

private:
    bool m_IsOpen{};

    int32 m_WeaponType{ 30 }; // eWeaponType
    int32 m_TimeLimit{ 120 };
    int32 m_NumKillsNeeded{ 10 };
    int32 m_ModelsToKill[4]{ -1, -1, -1, -1 };
    bool  m_StandardSoundAndMsg{ true };
    bool  m_IsHeadshotRequired{ false };
};
