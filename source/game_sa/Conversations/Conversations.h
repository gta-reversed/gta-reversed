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
    char  m_Name[8];
    char  m_NameNodeYes[8];
    char  m_NameNodeNo[8];
    int32 m_FinalSlot;
    int16 m_NodeYes, m_NodeNo;
    int32 m_Speech, m_SpeechY, m_SpeechN;
    //void  Clear(); // unknown
};

VALIDATE_SIZE(CTempConversationNode, 0x2C);

class CConversations {
public:
    enum class eAwkwardSayStatus : int32 {
        INACTIVE = 0,
        LOADING  = 1,
        PLAYING  = 2,
    };

    static inline auto& m_Nodes = StaticRef<std::array<CConversationNode, MAX_NUM_CONVERSATION_NODES>, 0x969570>();
    static inline CTempConversationNode m_aTempNodes[MAX_NUM_TEMP_CONVERSATION_NODES]; // 0x969360
    static inline CConversationForPed m_Conversations[MAX_NUM_CONVERSATIONS]; // 0x9691D8
    static inline bool m_SettingUpConversation; // 0x9691D0
    static inline CPed* m_SettingUpConversationPed; // 0x9691CC
    static inline int32 m_SettingUpConversationNumNodes; // 0x9691C8
    static inline eAwkwardSayStatus m_AwkwardSayStatus; // 0x9691C4

public:
    static void Clear();

    static void Update();

    static void AwkwardSay(int32 whatToSay, CPed* speakingPed);

    static void StartSettingUpConversation(CPed* ped);

    static void DoneSettingUpConversation(bool suppressSubtitles);

    static void SetUpConversationNode(const char* name, const char* linkYes, const char* linkNo, int32 speech, int32 speechY, int32 speechN);

    static bool IsConversationAtNode(const char* name, CPed* ped);

    static void RemoveConversationForPed(CPed* ped);

    static CConversationForPed* FindFreeConversationSlot();

    static CConversationForPed* FindConversationForPed(CPed* ped);

    static int32 FindFreeNodeSlot();

    static bool IsPlayerInPositionForConversation(CPed* ped, bool randomConversation);

    static void EnableConversation(CPed* ped, bool enable);

    static CConversationForPed::eStatus GetConversationStatus(CPed* ped);

    static bool IsConversationGoingOn();

public: // NOTSA:
    static void InjectHooks();
};
