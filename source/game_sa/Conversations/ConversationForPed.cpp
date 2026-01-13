#include "ConversationForPed.h"
#include "Conversations.h"

void CConversationForPed::InjectHooks() {
    RH_ScopedClass(CConversationForPed);
    RH_ScopedCategory("Conversations");

    RH_ScopedInstall(Clear, 0x43A770);
    RH_ScopedInstall(Update, 0x43C190, { .reversed = false });
    RH_ScopedInstall(IsPlayerInPositionForConversation, 0x43AC40);
}

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
    plugin::CallMethod<0x43C190, CConversationForPed*>(this);
}

// 0x43AC40
bool CConversationForPed::IsPlayerInPositionForConversation(bool randomConversation) {
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
    if (playerPed->GetMoveSpeed().Magnitude2D() > 0.01f) {
        return false;
    }

    // Check if player recently damaged the NPC
    if (m_pPed->m_pLastEntityDamage == playerPed && CTimer::GetTimeInMS() < m_pPed->m_LastDamagedTime + 6'000) {
        return false;
    }

    // For random conversations, check if player has group members
    if (randomConversation) {
        if (playerPed->GetGroup()->GetMembership().CountMembersExcludingLeader() >= 1) {
            return false;
        }
    }

    // Check if both player and NPC are ready for conversation
    if (!playerPed->PedIsReadyForConversation(randomConversation)) {
        return false;
    }

    return m_pPed->PedIsReadyForConversation(randomConversation);
}

// NOTSA
CConversationNode* CConversationForPed::GetCurrentNode() const {
    return m_CurrentNode != -1 ? &CConversations::m_Nodes[m_CurrentNode] : nullptr;
}

// NOTSA
CConversationNode* CConversationForPed::GetFirstNode() const {
    return m_FirstNode != -1 ? &CConversations::m_Nodes[m_FirstNode] : nullptr;
}
