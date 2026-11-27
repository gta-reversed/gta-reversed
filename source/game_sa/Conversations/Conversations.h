#pragma once

#include "ConversationForPed.h"
#include "PedToPlayerConversations.h"
#include "ConversationNode.h"

class CPed;

enum {
    MAX_NUM_CONVERSATIONS = 14,
    MAX_NUM_CONVERSATION_NODES = 50,
    MAX_NUM_TEMP_CONVERSATION_NODES = 12,
};

class CTempConversationNode {
public:
    char  m_Name[8]{};
    char  m_NameNodeYes[8]{};
    char  m_NameNodeNo[8]{};
    int32 m_FinalSlot{};
    int16 m_NodeYes{};
    int16 m_NodeNo{};
    int32 m_Speech{};
    int32 m_SpeechY{};
    int32 m_SpeechN{};
};

VALIDATE_SIZE(CTempConversationNode, 0x2C);

class CConversations {
public:
    enum class eAwkwardSayStatus : int32 {
        INACTIVE = 0,
        LOADING  = 1,
        PLAYING  = 2,
    };

public:
    static void Clear();
    static void Update();

    static void StartSettingUpConversation(CPed* ped);
    static void SetUpConversationNode(const char* questionKey, const char* answerYesKey, const char* answerNoKey, int32 questionWAV, int32 answerYesWAV, int32 answerNoWAV);
    static void DoneSettingUpConversation(bool suppressSubtitles);
    static void RemoveConversationForPed(CPed* ped);

    static CConversationForPed*         FindConversationForPed(CPed* ped);
    static CConversationForPed*         FindFreeConversationSlot();
    static int32                        FindFreeNodeSlot();
    static CConversationForPed::eStatus GetConversationStatus(CPed* ped);

    static bool IsPlayerInPositionForConversation(CPed* ped, bool randomConversation);
    static bool IsConversationGoingOn();
    static bool IsConversationAtNode(const char* name, CPed* ped);
    static void AwkwardSay(eAudioEvents whatToSay, CPed* speaker);
    static void EnableConversation(CPed* ped, bool enabled);

    //! Temp nodes added since `StartSettingUpConversation` (NOTSA)
    static auto GetTempNodesBeingSetUp() { return std::span{ m_TempNodes.data(), (size_t)(m_SettingUpConversationNumNodes) }; }

    static void InjectHooks();

public:
    static inline auto& m_AwkwardSayStatus              = StaticRef<eAwkwardSayStatus>(0x9691C4);
    static inline auto& m_SettingUpConversation         = StaticRef<bool>(0x9691D0);
    static inline auto& m_Conversations                 = StaticRef<std::array<CConversationForPed, MAX_NUM_CONVERSATIONS>>(0x9691D8);
    static inline auto& m_Nodes                         = StaticRef<std::array<CConversationNode, MAX_NUM_CONVERSATION_NODES>>(0x969570);
    static inline auto& m_TempNodes                     = StaticRef<std::array<CTempConversationNode, MAX_NUM_TEMP_CONVERSATION_NODES>>(0x969360);
    static inline auto& m_SettingUpConversationNumNodes = StaticRef<int32>(0x9691C8);
    static inline auto& m_SettingUpConversationPed      = StaticRef<CPed*>(0x9691CC);
};
