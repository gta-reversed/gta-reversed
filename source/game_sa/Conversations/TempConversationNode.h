#pragma once

class CTempConversationNode {
public:
    char  m_Name[8];
    char  m_NameNodeYes[8];
    char  m_NameNodeNo[8];
    int32 m_FinalSlot;
    int16 m_NodeYes;
    int16 m_NodeNo;
    int32 m_Speech; // eGlobalSpeechContext?
    int32 m_SpeechY; // eGlobalSpeechContext?
    int32 m_SpeechN; // eGlobalSpeechContext?
    //void  Clear(); // unknown
};

VALIDATE_SIZE(CTempConversationNode, 0x2C);
