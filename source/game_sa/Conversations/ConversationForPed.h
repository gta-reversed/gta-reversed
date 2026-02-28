#pragma once

#include "ConversationNode.h"

class CPed;

class CConversationForPed {
public:
    enum class eStatus : uint32 {
        INACTIVE = 0,
        PLAYER_SPEAKING,
        PED_SPEAKING,
        WAITINGFORINPUT
    };

    int32   m_FirstNode;
    int32   m_CurrentNode;
    CPed*   m_pPed;
    uint32  m_LastChange;
    uint32  m_LastTimeWeWereCloseEnough;
    eStatus m_Status;
    bool    m_Enabled;
    bool    m_SuppressSubtitles;

    void Clear(bool dontClearNodes);
    void Update();
    bool IsPlayerInPositionForConversation(bool isRandomConversation);

private: // NOTSA:
    friend void InjectHooksMain();
    static void InjectHooks();

public:
    CConversationNode* GetCurrentNode() const;
    CConversationNode* GetFirstNode() const;
};

VALIDATE_SIZE(CConversationForPed, 0x1C);
