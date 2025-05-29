#pragma once

class CConversationNode {
public:
    char  m_Name[8];
    int16 m_NodeYes;
    int16 m_NodeNo;
    int32 m_Speech;
    int32 m_SpeechY;
    int32 m_SpeechN;

    static void InjectHooks();
    void Clear();
    void ClearRecursively();
};

VALIDATE_SIZE(CConversationNode, 0x18);
