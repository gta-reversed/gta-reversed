/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

class CDecisionSimple {
public:
    eTaskType  m_anTasks[6];
    float      m_afChances[6];
    uint32     m_nCount; // tasks count (max 6)

public:
    void Set(int32* taskTypes, uint8* chances, int32 count);
    void MakeDecision(int32 taskType, int16& outTaskType, int32& outDecisionIndex);
    void SetDefault();
};

VALIDATE_SIZE(CDecisionSimple, 0x34);