#pragma once

class CPed;
class CConversationNode;

class CConversationForPed {
public:
    enum eStatus : uint32 {
        INACTIVE = 0,
        PLAYER_SPEAKING,
        PED_SPEAKING,
        WAITINGFORINPUT
    };

public:
    void Clear(bool dontClearNodes);
    void Update();
    bool IsPlayerInPositionForConversation(bool isRandomConversation);

    //! The node the conversation is at (NOTSA)
    CConversationNode& GetCurrentNode() const;

    static void InjectHooks();

public: // `CConversations` reads and writes these
    int32   m_FirstNode{};
    int32   m_CurrentNode{};
    CPed*   m_pPed{};
    uint32  m_LastChange{};
    uint32  m_LastTimeWeWereCloseEnough{};
    eStatus m_Status{};
    bool    m_Enabled{};
    bool    m_SuppressSubtitles{};
};

VALIDATE_SIZE(CConversationForPed, 0x1C);
