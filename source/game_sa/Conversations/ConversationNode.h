#pragma once

class CConversationNode {
public:
    char  m_Name[8];
    int16 m_NodeYes;
    int16 m_NodeNo;
    int32 m_Speech;  // eGlobalSpeechContext?
    int32 m_SpeechY; // eGlobalSpeechContext?
    int32 m_SpeechN; // eGlobalSpeechContext?

    void Clear();
    void ClearRecursively();

public: // NOTSA:
    static void InjectHooks();
};

VALIDATE_SIZE(CConversationNode, 0x18);
