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
    CPed*          Ped{};
    CTask*         Task{};
    eSecondaryTask Slot{ TASK_SECONDARY_INVALID };
    int32          MatchID{};
    bool           UsedTask{};

public:
    static void InjectHooks();

    void Flush();
};
VALIDATE_SIZE(CPedTaskPair, 0x14);
