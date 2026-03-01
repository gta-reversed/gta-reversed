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
    int16 m_NodeYes;
    int16 m_NodeNo;
    int32 m_Speech;
    int32 m_SpeechY;
    int32 m_SpeechN;
    //void  Clear();
};

VALIDATE_SIZE(CTempConversationNode, 0x2C);

class CConversations {
public:
    enum class eAwkwardSayStatus : int32 {
        INACTIVE = 0,
        LOADING  = 1,
        PLAYING  = 2,
    };

    static inline eAwkwardSayStatus& m_AwkwardSayStatus              = StaticRef<eAwkwardSayStatus, 0x9691C4>();
    static inline bool&              m_SettingUpConversation         = StaticRef<bool, 0x9691D0>();
    static inline auto&              m_Conversations                 = StaticRef<std::array<CConversationForPed, MAX_NUM_CONVERSATIONS>, 0x9691D8>();
    static inline auto&              m_Nodes                         = StaticRef<std::array<CConversationNode, MAX_NUM_CONVERSATION_NODES>, 0x969570>();
    static inline auto&              m_aTempNodes                    = StaticRef<std::array<CTempConversationNode, MAX_NUM_TEMP_CONVERSATION_NODES>, 0x969360>();
    static inline auto&              m_SettingUpConversationNumNodes = StaticRef<int32, 0x9691C8>();
    static inline auto&              m_SettingUpConversationPed      = StaticRef<CPed*, 0x9691CC>();

    static void InjectHooks();
    static void Clear();
    static void RemoveConversationForPed(CPed* ped);

    static void                 Update();
    static void                 SetUpConversationNode(const char* questionKey, const char* answerYesKey, const char* answerNoKey, int32 questionWAV, int32 answerYesWAV, int32 answerNoWAV);
    static bool                 IsPlayerInPositionForConversation(CPed* ped, bool randomConversation);
    static bool                 IsConversationGoingOn();
    static CConversationForPed* FindConversationForPed(CPed* ped);
    static bool                 IsConversationAtNode(const char* pName, CPed* pPed);
    static void                 AwkwardSay(int32 whatToSay, CPed* speaker);
    static void                 EnableConversation(CPed* ped, bool enabled);
    static void                 StartSettingUpConversation(CPed* ped);
    static void                 DoneSettingUpConversation(bool bSuppressSubtitles);

    /*
    static void FindFreeNodeSlot();
    static void FindFreeConversationSlot();
    
     */
};
