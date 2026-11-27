#include "StdInc.h"
#include "Conversations.h"
#include "ConversationForPed.h"

// 0x43A7B0
void CConversations::Clear() {
    ZoneScoped;

    for (auto& conversation : m_Conversations) {
        conversation.Clear(true);
    }

    for (auto& node : m_Nodes) {
        node.Clear();
    }

    m_SettingUpConversation = false;
    m_AwkwardSayStatus      = eAwkwardSayStatus::INACTIVE;
}

// 0x43C590
void CConversations::Update() {
    ZoneScoped;

    const auto UpdateConversations = [] {
        for (auto& conversation : m_Conversations) {
            conversation.Update();
        }
    };

    switch (m_AwkwardSayStatus) {
    case eAwkwardSayStatus::LOADING: // 0x43C5D4
        if (AudioEngine.GetMissionAudioLoadingStatus(0) == 1) {
            AudioEngine.PlayLoadedMissionAudio(0);
            m_AwkwardSayStatus = eAwkwardSayStatus::PLAYING;
        }
        break;
    case eAwkwardSayStatus::PLAYING: // 0x43C59B
        if (AudioEngine.IsMissionAudioSampleFinished(0)) {
            m_AwkwardSayStatus = eAwkwardSayStatus::INACTIVE;
            UpdateConversations();
        }
        break;
    case eAwkwardSayStatus::INACTIVE: // 0x43C5BA
        UpdateConversations();
        break;
    }
}

// 0x43A840
void CConversations::StartSettingUpConversation(CPed* ped) {
    CEntity::SetEntityReference(m_SettingUpConversationPed, ped);
    m_SettingUpConversationNumNodes = 0;
    m_SettingUpConversation         = true;
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
    if (notsa::bugfixes::GenericOOB && m_SettingUpConversationNumNodes >= MAX_NUM_TEMP_CONVERSATION_NODES) {
        return; // NOTE/BUG: The original writes the 13th node over `m_Nodes[0]`
    }

    auto& node = m_TempNodes[m_SettingUpConversationNumNodes];
    strncpy(node.m_Name, questionKey, 6u);
    MakeUpperCase(node.m_Name);

    node.m_Speech  = questionWAV;
    node.m_SpeechY = answerYesWAV;
    node.m_SpeechN = answerNoWAV;

    if (answerYesKey) {
        strncpy(node.m_NameNodeYes, answerYesKey, 6u); // NOTE/BUG: Not upper-cased like the others (0x43A8DC), so a lower case yes link never matches
    } else {
        node.m_NameNodeYes[0] = '\0';
    }
    if (answerNoKey) {
        strncpy(node.m_NameNodeNo, answerNoKey, 6u);
        MakeUpperCase(node.m_NameNodeNo);
    } else {
        node.m_NameNodeNo[0] = '\0';
    }
    ++m_SettingUpConversationNumNodes;
}

// 0x43ADB0
void CConversations::DoneSettingUpConversation(bool suppressSubtitles) {
    const auto tempNodes = GetTempNodesBeingSetUp();

    // Resolve the yes/no links by name (the last node with a matching name wins)
    for (auto& tempNode : tempNodes) {
        tempNode.m_NodeNo  = -1;
        tempNode.m_NodeYes = -1;
        for (auto&& [i, other] : rngv::enumerate(tempNodes)) {
            if (!strcmp(tempNode.m_NameNodeYes, other.m_Name)) {
                tempNode.m_NodeYes = (int16)(i);
            }
            if (!strcmp(tempNode.m_NameNodeNo, other.m_Name)) {
                tempNode.m_NodeNo = (int16)(i);
            }
        }
    }

    auto* const conversation = FindFreeConversationSlot();
    if (notsa::bugfixes::GenericCrashing && !conversation) { // NOTE/BUG: The original writes the conversation through a null pointer
        m_SettingUpConversationNumNodes = 0;
        m_SettingUpConversation         = false;
        return;
    }

    for (auto& tempNode : tempNodes) {
        tempNode.m_FinalSlot = FindFreeNodeSlot();
    }

    const auto GetFinalSlot = [&](int16 tempNodeIdx) -> int16 {
        return tempNodeIdx < 0
            ? -1
            : (int16)(tempNodes[tempNodeIdx].m_FinalSlot);
    };
    for (const auto& tempNode : tempNodes) {
        auto& node = m_Nodes[tempNode.m_FinalSlot];
        strcpy(node.m_Name, tempNode.m_Name);
        node.m_NodeYes = GetFinalSlot(tempNode.m_NodeYes);
        node.m_NodeNo  = GetFinalSlot(tempNode.m_NodeNo);
        node.m_Speech  = tempNode.m_Speech;
        node.m_SpeechY = tempNode.m_SpeechY;
        node.m_SpeechN = tempNode.m_SpeechN;
    }

    // NOTE: Always the first temp node's slot, also when no node was set up (then it's the one of the previous conversation)
    conversation->m_FirstNode   = m_TempNodes[0].m_FinalSlot;
    conversation->m_CurrentNode = m_TempNodes[0].m_FinalSlot;
    CEntity::SetEntityReference(conversation->m_pPed, m_SettingUpConversationPed);
    conversation->m_LastChange                = CTimer::GetTimeInMS();
    conversation->m_LastTimeWeWereCloseEnough = 0;
    conversation->m_Enabled                   = true;
    conversation->m_SuppressSubtitles         = suppressSubtitles;
    conversation->m_Status                    = CConversationForPed::eStatus::INACTIVE;

    m_SettingUpConversationNumNodes           = 0;
    m_SettingUpConversation                   = false;
}

// 0x43A960
void CConversations::RemoveConversationForPed(CPed* ped) {
    // NOTE: Clears every conversation of the ped, not only the first one
    for (auto& conversation : m_Conversations) {
        if (conversation.m_pPed == ped) {
            conversation.Clear(false);
        }
    }
}

// 0x43A9E0
CConversationForPed* CConversations::FindConversationForPed(CPed* ped) {
    for (auto& conversation : m_Conversations) {
        if (conversation.m_pPed == ped) {
            return &conversation;
        }
    }
    return nullptr;
}

// 0x43A9B0
CConversationForPed* CConversations::FindFreeConversationSlot() {
    return FindConversationForPed(nullptr);
}

// 0x43AA10
int32 CConversations::FindFreeNodeSlot() {
    for (auto&& [i, node] : rngv::enumerate(m_Nodes)) {
        if (!node.m_Name[0]) {
            node.m_Name[0] = 'X'; // Mark it as used
            node.m_Name[1] = '\0';
            return (int32)(i);
        }
    }
    return 0; // NOTE: The original returns the (used) first slot when all of them are taken
}

// 0x43AA80
CConversationForPed::eStatus CConversations::GetConversationStatus(CPed* ped) {
    const auto* const conversation = FindConversationForPed(ped);
    if (notsa::bugfixes::GenericCrashing && !conversation) {
        return CConversationForPed::eStatus::INACTIVE;
    }
    return conversation->m_Status;
}

// 0x43B0B0
bool CConversations::IsPlayerInPositionForConversation(CPed* ped, bool randomConversation) {
    auto* const conversation = FindConversationForPed(ped);
    if (notsa::bugfixes::GenericCrashing && !conversation) {
        return false;
    }
    return conversation->IsPlayerInPositionForConversation(randomConversation);
}

// 0x43AAC0
bool CConversations::IsConversationGoingOn() {
    return rng::any_of(m_Conversations, [](const CConversationForPed& conversation) {
        return conversation.m_Status != CConversationForPed::eStatus::INACTIVE;
    });
}

// 0x43B000
bool CConversations::IsConversationAtNode(const char* name, CPed* ped) {
    const auto* const conversation = FindConversationForPed(ped);
    if (notsa::bugfixes::GenericCrashing && !conversation) {
        return false;
    }

    if (conversation->m_CurrentNode < 0 || conversation->m_Status == CConversationForPed::eStatus::PLAYER_SPEAKING) {
        return false;
    }
    // NOTE: The original upper-cases `name` in place and uses `strcmp` (node names are always upper case)
    return !stricmp(name, m_Nodes[conversation->m_CurrentNode].m_Name);
}

// 0x43A810
void CConversations::AwkwardSay(eAudioEvents whatToSay, CPed* speaker) {
    AudioEngine.PreloadMissionAudio(0, whatToSay);
    AudioEngine.AttachMissionAudioToPed(0, speaker);
    m_AwkwardSayStatus = eAwkwardSayStatus::LOADING;
}

// 0x43AA40
void CConversations::EnableConversation(CPed* ped, bool enabled) {
    auto* const conversation = FindConversationForPed(ped);
    if (notsa::bugfixes::GenericCrashing && !conversation) {
        return;
    }
    conversation->m_Enabled = enabled;
}

void CConversations::InjectHooks() {
    RH_ScopedClass(CConversations);
    RH_ScopedCategory("Conversations");

    RH_ScopedInstall(Clear, 0x43A7B0);
    RH_ScopedInstall(Update, 0x43C590);
    RH_ScopedInstall(StartSettingUpConversation, 0x43A840);
    RH_ScopedInstall(SetUpConversationNode, 0x43A870);
    RH_ScopedInstall(DoneSettingUpConversation, 0x43ADB0);
    RH_ScopedInstall(RemoveConversationForPed, 0x43A960);
    RH_ScopedInstall(FindConversationForPed, 0x43A9E0);
    RH_ScopedInstall(FindFreeConversationSlot, 0x43A9B0);
    RH_ScopedInstall(FindFreeNodeSlot, 0x43AA10);
    RH_ScopedInstall(GetConversationStatus, 0x43AA80);
    RH_ScopedInstall(IsPlayerInPositionForConversation, 0x43B0B0);
    RH_ScopedInstall(IsConversationGoingOn, 0x43AAC0);
    RH_ScopedInstall(IsConversationAtNode, 0x43B000);
    RH_ScopedInstall(AwkwardSay, 0x43A810);
    RH_ScopedInstall(EnableConversation, 0x43AA40);
}
