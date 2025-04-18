/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

class CPed;
class CTask;

class CPedTaskPair {
public:
    CPed*  m_Ped{};
    CTask* m_Task{};
    int32  m_Slot{-1};
    int32  m_MatchID{};
    int8   m_bUsedTask{};

public:
    static void InjectHooks();

    void Flush();
};
VALIDATE_SIZE(CPedTaskPair, 0x14);
