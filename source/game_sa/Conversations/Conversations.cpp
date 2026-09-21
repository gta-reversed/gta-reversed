#include "StdInc.h"
#include "Conversations.h"
#include "ConversationForPed.h"

void CConversations::InjectHooks() {
    RH_ScopedClass(CConversations);
    RH_ScopedCategory("Conversations");

    RH_ScopedInstall(Clear, 0x43A7B0);
    RH_ScopedInstall(Update, 0x43C590);
    RH_ScopedInstall(SetUpConversationNode, 0x43A870);
    RH_ScopedInstall(RemoveConversationForPed, 0x43A960);
    RH_ScopedInstall(IsPlayerInPositionForConversation, 0x43B0B0);
    RH_ScopedInstall(IsConversationGoingOn, 0x43AAC0);
    RH_ScopedInstall(IsConversationAtNode, 0x43B000);
    RH_ScopedInstall(AwkwardSay, 0x43A810);
    RH_ScopedInstall(EnableConversation, 0x43A7F0);
    RH_ScopedInstall(StartSettingUpConversation, 0x43A840);
    RH_ScopedInstall(DoneSettingUpConversation, 0x43ADB0);
}

// 0x43A7B0
void CConversations::Clear() {
    ZoneScoped;

    for (auto& conversation : m_Conversations) {
        conversation.Clear(true);
    }

    for (auto& node : m_Nodes) {
        node.Clear();
    }

    m_SettingUpConversation = 0;
    m_AwkwardSayStatus      = eAwkwardSayStatus::INACTIVE;
}

// 0x43C590
void CConversations::Update() {
    ZoneScoped;

    const auto updateConversations = [&]() {
        for (auto& conversation : m_Conversations) {
            conversation.Update();
        }
    };

    switch (m_AwkwardSayStatus) {
    case eAwkwardSayStatus::LOADING:
        if (AudioEngine.GetMissionAudioLoadingStatus(0) == 1) {
            AudioEngine.PlayLoadedMissionAudio(0);
            m_AwkwardSayStatus = eAwkwardSayStatus::PLAYING;
        }
        break;
    case eAwkwardSayStatus::PLAYING:
        if (AudioEngine.IsMissionAudioSampleFinished(0)) {
            m_AwkwardSayStatus = eAwkwardSayStatus::INACTIVE;
            updateConversations();
        }
        break;
    case eAwkwardSayStatus::INACTIVE:
        updateConversations();
        break;
    }
}

// 0x43A870
void CConversations::SetUpConversationNode(
    const char* questionKey,
    const char* answerYesKey,
    const char* answerNoKey,
    int32       questionWAV,
    int32       answerYesWAV,
    int32       answerNoWAV
) {
    auto& node = CConversations::m_aTempNodes[CConversations::m_SettingUpConversationNumNodes];
    strncpy(node.m_Name, questionKey, 6u);
    MakeUpperCase(node.m_Name);

    node.m_Speech  = questionWAV;
    node.m_SpeechY = answerYesWAV;
    node.m_SpeechN = answerNoWAV;

    if (answerYesKey) {
        strncpy(node.m_NameNodeYes, answerYesKey, 6u);
        MakeUpperCase(node.m_NameNodeYes);
    } else {
        node.m_NameNodeYes[0] = '\0';
    }
    if (answerNoKey) {
        strncpy(node.m_NameNodeNo, answerNoKey, 6u);
        MakeUpperCase(node.m_NameNodeNo);
    } else {
        node.m_NameNodeNo[0] = '\0';
    }
    ++CConversations::m_SettingUpConversationNumNodes;
}

// 0x43A960
void CConversations::RemoveConversationForPed(CPed* ped) {
    for (auto& conversation : m_Conversations) {
        if (conversation.m_pPed == ped) {
            conversation.Clear(false);
        }
    }
}

// 0x43B0B0
bool CConversations::IsPlayerInPositionForConversation(CPed* ped, bool randomConversation) {
    return FindConversationForPed(ped)->IsPlayerInPositionForConversation(randomConversation);
}

// 0x43AAC0
bool CConversations::IsConversationGoingOn() {
    for (const auto& conversation : m_Conversations) {
        if (conversation.m_Status != CConversationForPed::eStatus::INACTIVE) {
            return true;
        }
    }
    return false;
}

// 0x43B000
bool CConversations::IsConversationAtNode(const char* pName, CPed* pPed) {
    auto conversation = FindConversationForPed(pPed);
    assert(conversation);

    if (conversation->m_CurrentNode < 0 || conversation->m_Status == CConversationForPed::eStatus::PLAYER_SPEAKING) {
        return false;
    }
    // NOTSA - using stricmp instead of MakeUpperCase + strcmp
    return !stricmp(pName, CConversations::m_Nodes[conversation->m_CurrentNode].m_Name);
}

// 0x43A810
void CConversations::AwkwardSay(int32 whatToSay, CPed* speaker) {
    AudioEngine.PreloadMissionAudio(0, whatToSay);
    AudioEngine.AttachMissionAudioToPed(0, speaker);
    m_AwkwardSayStatus = eAwkwardSayStatus::LOADING;
}

// 0x43AA40
void CConversations::EnableConversation(CPed* ped, bool enabled) {
    FindConversationForPed(ped)->m_Enabled = enabled;
}

// 0x43A840
void CConversations::StartSettingUpConversation(CPed* ped) {
    m_SettingUpConversationPed = ped;
    ped->RegisterReference(m_SettingUpConversationPed);
    m_SettingUpConversationNumNodes = 0;
    m_SettingUpConversation         = true;
}

// 0x43ADB0
void CConversations::DoneSettingUpConversation(bool bSuppressSubtitles) {
    // Resolve the `Yes/No` node names of the temp nodes into indices (Of temp nodes)
    if (m_SettingUpConversationNumNodes > 0) {
        for (auto i = 0; i < m_SettingUpConversationNumNodes; i++) {
            auto& tempNode = m_aTempNodes[i];

            tempNode.m_NodeYes = -1;
            tempNode.m_NodeNo  = -1;

            for (auto j = 0; j < m_SettingUpConversationNumNodes; j++) {
                if (strcmp(m_aTempNodes[j].m_Name, tempNode.m_NameNodeYes) == 0) {
                    tempNode.m_NodeYes = static_cast<int16>(j);
                }
                if (strcmp(m_aTempNodes[j].m_Name, tempNode.m_NameNodeNo) == 0) {
                    tempNode.m_NodeNo = static_cast<int16>(j);
                }
            }
        }
    }

    // Find a free conversation
    CConversationForPed* conversation = nullptr;
    for (auto& c : m_Conversations) {
        if (!c.m_pPed) {
            conversation = &c;
            break;
        }
    }
    assert(conversation);

    if (m_SettingUpConversationNumNodes > 0) {
        // Assign a free node to each temp node
        for (auto i = 0; i < m_SettingUpConversationNumNodes; i++) {
            auto& tempNode = m_aTempNodes[i];

            auto nodeIndex = 0;
            for (; nodeIndex < MAX_NUM_CONVERSATION_NODES; nodeIndex++) {
                if (!m_Nodes[nodeIndex].m_Name[0]) {
                    m_Nodes[nodeIndex].m_Name[0] = 'X'; // Mark as used (The name is overwritten below)
                    m_Nodes[nodeIndex].m_Name[1] = '\0';
                    break;
                }
            }
            if (nodeIndex >= MAX_NUM_CONVERSATION_NODES) { // No free node was found
                nodeIndex = 0;
            }

            tempNode.m_FinalSlot = nodeIndex;
        }

        // Copy the temp nodes over to the nodes they were assigned
        if (m_SettingUpConversationNumNodes > 0) {
            for (auto i = 0; i < m_SettingUpConversationNumNodes; i++) {
                const auto& tempNode = m_aTempNodes[i];
                auto&       node     = m_Nodes[tempNode.m_FinalSlot];

                strcpy(node.m_Name, tempNode.m_Name);

                node.m_NodeYes = tempNode.m_NodeYes < 0
                    ? -1
                    : m_aTempNodes[tempNode.m_NodeYes].m_FinalSlot;
                node.m_NodeNo = tempNode.m_NodeNo < 0
                    ? -1
                    : m_aTempNodes[tempNode.m_NodeNo].m_FinalSlot;

                node.m_Speech  = tempNode.m_Speech;
                node.m_SpeechY = tempNode.m_SpeechY;
                node.m_SpeechN = tempNode.m_SpeechN;
            }
        }
    }

    conversation->m_FirstNode   = m_aTempNodes[0].m_FinalSlot;
    conversation->m_CurrentNode = m_aTempNodes[0].m_FinalSlot;
    conversation->m_pPed        = m_SettingUpConversationPed;
    CEntity::RegisterReference(conversation->m_pPed);
    conversation->m_LastChange                = CTimer::m_snTimeInMilliseconds;
    conversation->m_LastTimeWeWereCloseEnough = 0;
    conversation->m_Enabled                   = true;
    conversation->m_SuppressSubtitles         = bSuppressSubtitles;
    conversation->m_Status                    = CConversationForPed::eStatus::INACTIVE;

    m_SettingUpConversationNumNodes = 0;
    m_SettingUpConversation         = false;
}

CConversationForPed* CConversations::FindConversationForPed(CPed* ped) {
    for (auto& conversation : m_Conversations) {
        if (conversation.m_pPed == ped) {
            return &conversation;
        }
    }
    return nullptr;
}
