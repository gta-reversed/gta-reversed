#pragma once

#include "../DebugModule.h"
#include "toolsmenu/CDebugMenuToolInput.h"

class MissionDebugModule final : public DebugModule {
public:
    MissionDebugModule();

    void RenderWindow() override final;
    void RenderMenuEntry() override final;

    NOTSA_IMPLEMENT_DEBUG_MODULE_SERIALIZATION(MissionDebugModule, m_IsOpen, m_MissionIDToStart, m_SelectedMissionIdx, m_MissionToolInput);

private:
    bool StartMission(int32 missionId, bool bDoMissionCleanUp);

private:
    bool                m_IsOpen{};
    bool                m_ShouldStartMission{};
    int32               m_MissionIDToStart{};
    int32               m_SelectedMissionIdx{ -1 };
    CDebugMenuToolInput m_MissionToolInput{};
};
