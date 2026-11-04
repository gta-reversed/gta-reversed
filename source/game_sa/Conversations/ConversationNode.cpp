#include "ConversationNode.h"
#include "Conversations.h"

void CConversationNode::InjectHooks() {
    RH_ScopedClass(CConversationNode);
    RH_ScopedCategory("Conversations");
    RH_ScopedInstall(ClearRecursively, 0x43A7A0);
}

void CConversationNode::Clear() {
    m_Name[0] = '\0';
    m_NodeYes = -1;
    m_NodeNo  = -1;
    m_Speech  = 0;
    m_SpeechY = 0;
    m_SpeechN = 0;
}

// 0x43A710
void CConversationNode::ClearRecursively() {
    if (m_NodeYes >= 0) {
        CConversations::m_Nodes[m_NodeYes].ClearRecursively();
    }
    if (m_NodeNo >= 0) {
        CConversations::m_Nodes[m_NodeNo].ClearRecursively();
    }
    Clear();
}
