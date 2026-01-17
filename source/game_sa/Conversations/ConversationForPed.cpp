#include "ConversationForPed.h"
#include "Conversations.h"

void CConversationForPed::InjectHooks() {
    RH_ScopedClass(CConversationForPed);
    RH_ScopedCategory("Conversations");

    RH_ScopedInstall(Clear, 0x43A770);
    RH_ScopedInstall(Update, 0x43C190);
    RH_ScopedInstall(IsPlayerInPositionForConversation, 0x43AC40);
}

// inline
// 0x43A770
inline void CConversationForPed::Clear(bool dontClearNodes) {
    if (!dontClearNodes) {
        CConversations::m_Nodes[m_FirstNode].ClearRecursively();
    }
    m_FirstNode                 = -1;
    m_CurrentNode               = -1;
    m_pPed                      = nullptr;
    m_LastChange                = 0;
    m_LastTimeWeWereCloseEnough = 0;
}

// 0x43C190
void CConversationForPed::Update() {
    if (!m_pPed || !m_Enabled || !IsPlayerInPositionForConversation(false)) {
        m_Status = CConversationForPed::eStatus::INACTIVE;
        return;
    }

    const auto Speak = [](int32 speechId, CPed* ped) {
        if (speechId > 0) {
            ped->Say((eGlobalSpeechContext)speechId);
        }

        if (speechId < 0) {
            CConversations::AwkwardSay(-speechId, ped);
        }
    };

    const auto pad = CPad::GetPad();
    if (CTimer::GetTimeInMS() > m_LastTimeWeWereCloseEnough + 10'000 || m_Status == CConversationForPed::eStatus::PLAYER_SPEAKING && (CTimer::GetTimeInMS() > m_LastChange + 4'000 || pad->ConversationYesJustDown() || pad->ConversationNoJustDown())) {
        assert(GetCurrentNode());
        if (!m_SuppressSubtitles) {
            CMessages::ClearSmallMessagesOnly();
            CMessages::AddMessageJump(TheText.Get(GetCurrentNode()->m_Name), 4'000, 1, true);
        }
        Speak(GetCurrentNode()->m_Speech, m_pPed);

        m_Status     = CConversationForPed::eStatus::PED_SPEAKING;
        m_LastChange = CTimer::GetTimeInMS();
    } else if (CTimer::GetTimeInMS() > m_LastChange + 400) {
        switch (m_Status) {
        case CConversationForPed::eStatus::INACTIVE:
        case CConversationForPed::eStatus::PED_SPEAKING:
        case CConversationForPed::eStatus::WAITINGFORINPUT:         {
            assert(GetCurrentNode());
            if (pad->ConversationNoJustDown() && GetCurrentNode()->m_NodeNo >= 0) {
                if (!m_SuppressSubtitles) {
                    CMessages::ClearSmallMessagesOnly();
                    CMessages::AddMessageJump(TheText.Get(std::format("{}N", GetCurrentNode()->m_Name).c_str()), 4'000, 3, true);
                }
                Speak(GetCurrentNode()->m_SpeechN, FindPlayerPed());

                m_CurrentNode = GetCurrentNode()->m_NodeNo;
            } else if (pad->ConversationYesJustDown() && GetCurrentNode()->m_NodeYes >= 0) {
                if (!m_SuppressSubtitles) {
                    CMessages::ClearSmallMessagesOnly();
                    CMessages::AddMessageJump(TheText.Get(std::format("{}Y", GetCurrentNode()->m_Name).c_str()), 4'000, 3, true);
                }
                Speak(GetCurrentNode()->m_SpeechY, FindPlayerPed());

                m_CurrentNode = GetCurrentNode()->m_NodeYes;
            }

            assert(GetCurrentNode()); // Yes/No node
            if (!m_SuppressSubtitles) {
                CMessages::AddMessageQ(TheText.Get(GetCurrentNode()->m_Name), 4'000, 1, true);
            }

            m_Status     = CConversationForPed::eStatus::PLAYER_SPEAKING;
            m_LastChange = CTimer::GetTimeInMS();
            break;
        }
        }
    }
    m_LastTimeWeWereCloseEnough = CTimer::GetTimeInMS();
    if (m_Status == CConversationForPed::eStatus::PED_SPEAKING && CTimer::GetTimeInMS() > m_LastChange + 4'000) {
        m_Status = CConversationForPed::eStatus::WAITINGFORINPUT;
    }
}

// 0x43AC40
bool CConversationForPed::IsPlayerInPositionForConversation(bool isRandomConversation) {
    const auto playerPed = FindPlayerPed();

    // Check distance
    if ((FindPlayerCoors() - m_pPed->GetPosition()).Magnitude() > 4.0f) {
        return false;
    }

    // Check if player and NPC are facing each other (dot product of forward vectors)
    if (playerPed->GetForward().Dot(m_pPed->GetForward()) > 0.0f) {
        return false;
    }

    // Check player's move speed (2D only)
    if (playerPed->GetMoveSpeed().SquaredMagnitude2D() > sq(0.01f)) {
        return false;
    }

    // Check if ped was damaged by the player in the last 6 seconds
    if (m_pPed->m_pLastEntityDamage == playerPed && CTimer::GetTimeInMS() < m_pPed->m_LastDamagedTime + 6'000) {
        return false;
    }

    // For random conversations, check if player has group members
    if (isRandomConversation) {
        if (playerPed->GetGroup()->GetMembership().CountMembersExcludingLeader() >= 1) {
            return false;
        }
    }

    return playerPed->PedIsReadyForConversation(isRandomConversation) 
        && m_pPed->PedIsReadyForConversation(isRandomConversation);
}

// NOTSA
CConversationNode* CConversationForPed::GetCurrentNode() const {
    return m_CurrentNode != -1 ? &CConversations::m_Nodes[m_CurrentNode] : nullptr;
}

// NOTSA
CConversationNode* CConversationForPed::GetFirstNode() const {
    return m_FirstNode != -1 ? &CConversations::m_Nodes[m_FirstNode] : nullptr;
}
