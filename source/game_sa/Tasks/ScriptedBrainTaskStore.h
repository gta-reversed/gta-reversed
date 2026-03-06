#pragma once

#include "constants.h"

class CTask;
class CPed;

class CScriptedBrainTaskEntry {
public:
    CPed*  m_ped;
    int32  unused1;
    int32  unused2;
    CTask* m_task;

public:
    static void InjectHooks();

    CScriptedBrainTaskEntry();
    ~CScriptedBrainTaskEntry() = default;

private:
    CScriptedBrainTaskEntry* Constructor();
};

VALIDATE_SIZE(CScriptedBrainTaskEntry, 0x10);

class CScriptedBrainTaskStore {
public:
    static inline auto (&ms_entries)[TOTAL_SCRIPTED_BRAIN_TASK_ENTRIES] = StaticRef<CScriptedBrainTaskEntry[TOTAL_SCRIPTED_BRAIN_TASK_ENTRIES]>(0xC188F0);

    static void InjectHooks();

    static CTask* SetTask(CPed* ped, CTask* task);

    /// Get brain entry task of ped (possibly null)
    static CTask* GetTask(CPed* ped);

    static void Clear(CPed* ped);
    static void Clear(CTask* task);

    /* NOTSA */

    /// Get brain of `ped`, nullptr if not found.
    static auto GetOf(CPed* ped) -> CScriptedBrainTaskEntry*;
};
