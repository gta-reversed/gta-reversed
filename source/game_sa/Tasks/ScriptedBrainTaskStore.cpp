#include "StdInc.h"

void CScriptedBrainTaskEntry::InjectHooks()
{
    RH_ScopedClass(CScriptedBrainTaskEntry);
    RH_ScopedCategory("Tasks");

    RH_ScopedInstall(Constructor, 0x62EC40);
}

void CScriptedBrainTaskStore::InjectHooks()
{
    RH_ScopedClass(CScriptedBrainTaskStore);
    RH_ScopedCategory("Tasks");

    CScriptedBrainTaskEntry::InjectHooks();
    RH_ScopedInstall(SetTask, 0x635720);
    RH_ScopedInstall(GetTask, 0x6357C0);
    RH_ScopedOverloadedInstall(Clear, "ped", 0x635850, void(*)(CPed*));
    RH_ScopedOverloadedInstall(Clear, "task", 0x6357F0, void(*)(CTask*));
}

// 0x62EC40
CScriptedBrainTaskEntry::CScriptedBrainTaskEntry()
{
    m_ped  = nullptr;
    m_task = nullptr;
}

CScriptedBrainTaskEntry* CScriptedBrainTaskEntry::Constructor()
{
    this->CScriptedBrainTaskEntry::CScriptedBrainTaskEntry();
    return this;
}

// 0x635720
CTask* CScriptedBrainTaskStore::SetTask(CPed* ped, CTask* task)
{
    int32 freeEntryIndex = -1;
    for (int32 i = 0; i < TOTAL_SCRIPTED_BRAIN_TASK_ENTRIES; i++) {
        CScriptedBrainTaskEntry& entry = ms_entries[i];

        if (!entry.m_ped && freeEntryIndex == -1)
            freeEntryIndex = i;

        if (entry.m_ped == ped) {
            delete entry.m_task;
            entry.m_task = task;
            return task;
        }
    }

    if (freeEntryIndex == -1) {
        delete task;
        return nullptr;
    }

    CScriptedBrainTaskEntry& entry = ms_entries[freeEntryIndex];

    entry.m_ped = ped;
    ped->RegisterReference(reinterpret_cast<CEntity**>(&entry.m_ped));

    delete entry.m_task;
    entry.m_task = task;

    return task;
}

// 0x6357C0
CTask* CScriptedBrainTaskStore::GetTask(CPed* ped) {
    if (const auto brain = GetOf(ped)) {
        return brain->m_task;
    }
    return nullptr;
}

// 0x635850
void CScriptedBrainTaskStore::Clear(CPed* ped)
{
    for (auto& entry : ms_entries) {
        if (entry.m_ped != ped)
            continue;

        delete entry.m_task;
        entry.m_task = nullptr;
        CEntity::ClearReference(entry.m_ped);
        return;
    }
}

// 0x6357F0
void CScriptedBrainTaskStore::Clear(CTask* task)
{
    for (auto& entry : ms_entries) {
        if (entry.m_task != task)
            continue;

        delete entry.m_task;
        entry.m_task = nullptr;
        CEntity::ClearReference(entry.m_ped);
        return;
    }
}

auto CScriptedBrainTaskStore::GetOf(CPed* ped) -> CScriptedBrainTaskEntry* {
    const auto it = rng::find_if(ms_entries, [ped](CScriptedBrainTaskEntry& entry) { return entry.m_ped == ped; });
    return it != rng::end(ms_entries) ? &*it : nullptr;
}
