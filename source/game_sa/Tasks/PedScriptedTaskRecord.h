#pragma once

#include "constants.h"

class CEventScriptCommand;
class CTask;
class CPed;

enum class eScriptedTaskStatus : int32 {
    NONE = -1,
    EVENT_ASSOCIATED = 0,
    TASK_ASSOCIATED,
    PRIMARY,
    FLUSHED,
    GROUP,
    BRAIN, // a.k.a attractor
    SECONDARY
};

class CPedScriptedTaskRecordData {
public:
    int32                m_opcode;
    CEventScriptCommand* m_event;
    CTask*               m_task;
    CPed*                m_ped;
    eScriptedTaskStatus  m_status;

public:
    static void InjectHooks();
    CPedScriptedTaskRecordData* Constructor();

    CPedScriptedTaskRecordData();
    ~CPedScriptedTaskRecordData() = default;

    void AssociateWithTask(CTask* task);
    void AssociateWithEvent(CEventScriptCommand* event);
    void Set(CPed* ped, int32 opcode, CEventScriptCommand* event);
    void Set(CPed* ped, int32 opcode, CTask* task);
    void SetAsGroupTask(CPed* ped, int32 opcode, CTask* task);
    void SetAsAttractorScriptTask(CPed* ped, int32 opcode, CTask* task);
    void Flush();
};
VALIDATE_SIZE(CPedScriptedTaskRecordData, 0x14);

class CPedScriptedTaskRecord {
public:
    static void InjectHooks();

    static inline auto (&ms_scriptedTasks)[TOTAL_SCRIPTED_TASKS] = StaticRef<CPedScriptedTaskRecordData[TOTAL_SCRIPTED_TASKS]>(0xC0B1E8);

    static CPedScriptedTaskRecordData* GetRecordAssociatedWithEvent(CEvent* event);
    static eScriptedTaskStatus GetStatus(CPed* ped);
    static eScriptedTaskStatus GetStatus(CPed* ped, int32 opcode);
    static int32 GetVacantSlot();
    static void Process();
};
