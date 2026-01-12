#pragma once

class CConversationNode {
public:
    char  m_Name[8];
    int16 m_NodeYes, m_NodeNo;
    int32 m_Speech, m_SpeechY, m_SpeechN;

    void Clear();
    void ClearRecursively();

public: // NOTSA:
    static void InjectHooks();
};

VALIDATE_SIZE(CConversationNode, 0x18);
