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
    RH_ScopedInstall(FindFreeConversationSlot, 0x43A9B0);
    RH_ScopedInstall(GetConversationStatus, 0x43AA80);
    RH_ScopedInstall(FindConversationForPed, 0x43A9E0);
    RH_ScopedInstall(FindFreeNodeSlot, 0x43AA10);
}

// 0x43A7B0
void CConversations::Clear() {
    ZoneScoped;

    for (auto& conv : m_Conversations) {
        conv.Clear(true);
    }

    for (auto& node : m_Nodes) {
        node.Clear();
    }

    m_SettingUpConversation = false;
    m_AwkwardSayStatus = eAwkwardSayStatus::INACTIVE;
}

// 0x43C590
void CConversations::Update() {
    ZoneScoped;

    const auto updateConversations = [&]() {
        for (auto& conv : m_Conversations) {
            conv.Update();
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
    default:
        NOTSA_UNREACHABLE();
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
    auto& node = m_aTempNodes[m_SettingUpConversationNumNodes];
    strncpy(node.m_Name, questionKey, 6u);
    MakeUpperCase(node.m_Name);

    node.m_Speech  = questionWAV;
    node.m_SpeechY = answerYesWAV;
    node.m_SpeechN = answerNoWAV;

    const auto ProcessNode = [&](char* dest, const char* src) {
        if (src) {
            strncpy(dest, src, 6u);
            MakeUpperCase(dest);
        } else {
            dest[0] = '\0';
        }
    };

    ProcessNode(node.m_NameNodeYes, answerYesKey);
    ProcessNode(node.m_NameNodeNo, answerNoKey);

    ++m_SettingUpConversationNumNodes;
}

// 0x43A960
void CConversations::RemoveConversationForPed(CPed* ped) {
    if (const auto conv = FindConversationForPed(ped)) {
        assert(conv->GetFirstNode());
        conv->GetFirstNode()->ClearRecursively();
        conv->Clear(false);
    }
}

// 0x43A9B0
inline CConversationForPed* CConversations::FindFreeConversationSlot() {
    for (auto& conv : m_Conversations) {
        if (conv.m_pPed == nullptr) {
            return &conv;
        }
    }
    return nullptr;
}

// 0x43B0B0
bool CConversations::IsPlayerInPositionForConversation(CPed* ped, bool isRandomConversation) {
    if (const auto conversation = FindConversationForPed(ped)) {
        return conversation->IsPlayerInPositionForConversation(isRandomConversation);
    }

    // Originally in this case game would call a THISCALL function with nullptr `this`, so game would crash.
    NOTSA_UNREACHABLE("Couldn't find the specified ped in any conversation.");
}

// 0x43AAC0
bool CConversations::IsConversationGoingOn() {
    for (const auto& conv : m_Conversations) {
        if (conv.m_Status != CConversationForPed::eStatus::INACTIVE) {
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
    return !stricmp(pName, m_Nodes[conversation->m_CurrentNode].m_Name);
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

// 0x43AA80
inline CConversationForPed::eStatus CConversations::GetConversationStatus(CPed* ped) {
    // NOTE: Why max MAX_NUM_CONVERSATIONS - 1 ?
    for (auto i = 0; i < MAX_NUM_CONVERSATIONS - 1; ++i) {
        if (m_Conversations[i].m_pPed == ped) {
            return m_Conversations[i].m_Status;
        }
    }
    return m_Conversations[MAX_NUM_CONVERSATIONS - 1].m_Status;
}

// 0x43A840
void CConversations::StartSettingUpConversation(CPed* ped) {
    m_SettingUpConversationPed = ped;
    ped->RegisterReference(m_SettingUpConversationPed);
    m_SettingUpConversationNumNodes = 0;
    m_SettingUpConversation = true;
}

// 0x43ADB0
void CConversations::DoneSettingUpConversation(bool suppressSubtitles) {
    ZoneScoped;

    // We link temporary nodes by name (set Yes/No indexes)
    const auto numNodes = m_SettingUpConversationNumNodes;
    if (numNodes > 0) {
        for (auto i = 0; i < numNodes; ++i) {
            m_aTempNodes[i].m_NodeYes = -1;
            m_aTempNodes[i].m_NodeNo  = -1;

            for (auto j = 0; j < numNodes; ++j) {
                if (strcmp(m_aTempNodes[i].m_NameNodeYes, m_aTempNodes[j].m_Name) == 0) {
                    m_aTempNodes[i].m_NodeYes = static_cast<int16>(j);
                }
                if (strcmp(m_aTempNodes[i].m_NameNodeNo, m_aTempNodes[j].m_Name) == 0) {
                    m_aTempNodes[i].m_NodeNo = static_cast<int16>(j);
                }
            }
        }
    }

    auto* conversationSlot = FindFreeConversationSlot();
    if (!conversationSlot) {
        NOTSA_LOG_WARN("No free conversation slot available");

        // untested
        if (notsa::IsFixBugs()) {
            m_SettingUpConversation         = false;
            m_SettingUpConversationNumNodes = 0;
            return;
        }
    }

    // We allocate the final slots for the nodes and copy the data.
    if (numNodes > 0) {
        for (auto i = 0; i < numNodes; ++i) {
            m_aTempNodes[i].m_FinalSlot = FindFreeNodeSlot();
        }

        for (auto i = 0; i < numNodes; ++i) {
            const auto& tempNode  = m_aTempNodes[i];
            auto&       node      = m_Nodes[tempNode.m_FinalSlot];

            strcpy(node.m_Name, tempNode.m_Name);

            node.m_NodeYes = tempNode.m_NodeYes < 0 ? -1 : int16(m_aTempNodes[tempNode.m_NodeYes].m_FinalSlot);
            node.m_NodeNo  = tempNode.m_NodeNo < 0 ? -1 : int16(m_aTempNodes[tempNode.m_NodeNo].m_FinalSlot);

            node.m_Speech  = tempNode.m_Speech;
            node.m_SpeechY = tempNode.m_SpeechY;
            node.m_SpeechN = tempNode.m_SpeechN;
        }
    }

    // untested
    if (notsa::IsFixBugs()) {
        // if there are no nodes — leave uninitialized fields untouched
        conversationSlot->m_FirstNode   = numNodes > 0 ? m_aTempNodes[0].m_FinalSlot : -1;
        conversationSlot->m_CurrentNode = conversationSlot->m_FirstNode;
    } else {
        // always take [0], even if numNodes == 0
        conversationSlot->m_FirstNode   = m_aTempNodes[0].m_FinalSlot;
        conversationSlot->m_CurrentNode = m_aTempNodes[0].m_FinalSlot;
    }

    auto* ped = m_SettingUpConversationPed;
    conversationSlot->m_pPed = ped;
    ped->RegisterReference(reinterpret_cast<CEntity**>(&conversationSlot->m_pPed));

    conversationSlot->m_LastTimeWeWereCloseEnough = 0;
    conversationSlot->m_Status                    = CConversationForPed::eStatus::INACTIVE;
    conversationSlot->m_Enabled                   = true;
    conversationSlot->m_SuppressSubtitles         = suppressSubtitles;
    conversationSlot->m_LastChange                = CTimer::GetTimeInMS();

    m_SettingUpConversationNumNodes = 0;
    m_SettingUpConversation         = false;
}

// 0x43A9E0
inline CConversationForPed* CConversations::FindConversationForPed(CPed* ped) {
    for (auto& conv : m_Conversations) {
        if (conv.m_pPed == ped) {
            return &conv;
        }
    }
    return nullptr;
}

// 0x43AA10
inline int32 CConversations::FindFreeNodeSlot() {
    for (auto [i, node] : std::views::enumerate(m_Nodes)) {
        if (node.m_Name[0] == '\0') {
            node.m_Name[0] = 'X';
            node.m_Name[1] = '\0';
            return i;
        }
    }
    return 0;
}
