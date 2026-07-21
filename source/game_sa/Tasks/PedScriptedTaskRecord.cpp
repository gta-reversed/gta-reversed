#include "StdInc.h"

void CPedScriptedTaskRecordData::InjectHooks()
{
    RH_ScopedClass(CPedScriptedTaskRecordData);
    RH_ScopedCategory("Tasks");

    RH_ScopedInstall(Constructor, 0x608330);
    RH_ScopedInstall(AssociateWithTask, 0x608520);
    RH_ScopedInstall(AssociateWithEvent, 0x608500);
    RH_ScopedOverloadedInstall(Set, "CEventScriptCommand", 0x608390, void(CPedScriptedTaskRecordData::*)(CPed*, int32, CEventScriptCommand*));
    RH_ScopedOverloadedInstall(Set, "CTask", 0x6083E0, void(CPedScriptedTaskRecordData::*)(CPed*, int32, CTask*));
    RH_ScopedInstall(SetAsGroupTask, 0x6084A0);
    RH_ScopedInstall(SetAsAttractorScriptTask, 0x608440);
    RH_ScopedInstall(Flush, 0x608350);
}

void CPedScriptedTaskRecord::InjectHooks()
{
    RH_ScopedClass(CPedScriptedTaskRecord);
    RH_ScopedCategory("Tasks");

    RH_ScopedInstall(GetRecordAssociatedWithEvent, 0x608580);
    RH_ScopedOverloadedInstall(GetStatus, "", 0x608750, eScriptedTaskStatus(*)(CPed*));
    RH_ScopedOverloadedInstall(GetStatus, "opcode", 0x608710, eScriptedTaskStatus(*)(CPed*, int32));
    RH_ScopedInstall(GetVacantSlot, 0x608540);
    RH_ScopedInstall(Process, 0x6085E0);
}

CPedScriptedTaskRecordData::CPedScriptedTaskRecordData()
{
    m_opcode = -1;
    m_event = nullptr;
    m_task = nullptr;
    m_ped = nullptr;
    m_status = eScriptedTaskStatus::FLUSHED;
}

// 0x608330
CPedScriptedTaskRecordData* CPedScriptedTaskRecordData::Constructor()
{
    this->CPedScriptedTaskRecordData::CPedScriptedTaskRecordData();
    return this;
}

// 0x608520
void CPedScriptedTaskRecordData::AssociateWithTask(CTask* task)
{
    m_event = nullptr;
    m_task = task;
    m_status = eScriptedTaskStatus::TASK_ASSOCIATED;
}

// 0x608500
void CPedScriptedTaskRecordData::AssociateWithEvent(CEventScriptCommand* event)
{
    m_event = event;
    m_status = eScriptedTaskStatus::EVENT_ASSOCIATED;
}

// 0x608390
void CPedScriptedTaskRecordData::Set(CPed* ped, int32 opcode, CEventScriptCommand* event)
{
    Flush();
    m_ped = ped;
    ped->RegisterReference(reinterpret_cast<CEntity**>(&m_ped));
    m_task = nullptr;
    m_status = eScriptedTaskStatus::EVENT_ASSOCIATED;
    m_opcode = opcode;
    m_event = event;
}

// 0x6083E0
void CPedScriptedTaskRecordData::Set(CPed* ped, int32 opcode, CTask* task)
{
    Flush();
    m_ped = ped;
    ped->RegisterReference(reinterpret_cast<CEntity**>(&m_ped));
    m_opcode = opcode;
    m_event = nullptr;
    m_task = task;
    m_status = eScriptedTaskStatus::TASK_ASSOCIATED;
}

// 0x6084A0
void CPedScriptedTaskRecordData::SetAsGroupTask(CPed* ped, int32 opcode, CTask* task)
{
    Flush();
    m_ped = ped;
    ped->RegisterReference(reinterpret_cast<CEntity**>(&m_ped));
    m_opcode = opcode;
    m_event = nullptr;
    m_task = task;
    m_status = eScriptedTaskStatus::GROUP;
}

// 0x608440
void CPedScriptedTaskRecordData::SetAsAttractorScriptTask(CPed* ped, int32 opcode, CTask* task)
{
    Flush();
    m_ped = ped;
    ped->RegisterReference(reinterpret_cast<CEntity**>(&m_ped));
    m_opcode = opcode;
    m_event = nullptr;
    m_task = task;
    m_status = eScriptedTaskStatus::BRAIN;
}

// 0x608350
void CPedScriptedTaskRecordData::Flush()
{
    CEntity::SafeCleanUpRef(m_ped);
    m_ped = nullptr;
    m_event = nullptr;
    m_task = nullptr;
    m_status = eScriptedTaskStatus::FLUSHED;
    m_opcode = -1;
}

// 0x608580
CPedScriptedTaskRecordData* CPedScriptedTaskRecord::GetRecordAssociatedWithEvent(CEvent* event)
{
    for (auto& taskRecordData : ms_scriptedTasks) {
        if (taskRecordData.m_event == event)
            return &taskRecordData;
    }
    return nullptr;
}

// 0x608750
eScriptedTaskStatus CPedScriptedTaskRecord::GetStatus(CPed* ped)
{
    for (auto & taskRecordData : ms_scriptedTasks) {
        if (taskRecordData.m_ped == ped)
            return eScriptedTaskStatus::TASK_ASSOCIATED;
    }
    return eScriptedTaskStatus::EVENT_ASSOCIATED;
}

// 0x608710
eScriptedTaskStatus CPedScriptedTaskRecord::GetStatus(CPed* ped, int32 opcode)
{
    for (auto & taskRecordData : ms_scriptedTasks) {
        if ((opcode == -1 || taskRecordData.m_opcode == opcode) && taskRecordData.m_ped == ped)
            return taskRecordData.m_status;
    }
    return eScriptedTaskStatus::NONE;
}

// 0x608540
int32 CPedScriptedTaskRecord::GetVacantSlot()
{
    for (int32 i = 0; i < TOTAL_SCRIPTED_TASKS; i++) {
        CPedScriptedTaskRecordData& taskRecordData = ms_scriptedTasks[i];
        if (!taskRecordData.m_event && !taskRecordData.m_task && !taskRecordData.m_ped)
            return i;
    }
    return -1;
}

// 0x6085E0
void CPedScriptedTaskRecord::Process()
{
    for (auto& taskRecordData : ms_scriptedTasks) {
        if (taskRecordData.m_event || taskRecordData.m_task || taskRecordData.m_ped) {
            if (!taskRecordData.m_ped) {
                taskRecordData.Flush();
            }
            else if (taskRecordData.m_event) {
                if (taskRecordData.m_ped->GetEventGroup().HasEvent(taskRecordData.m_event))
                    taskRecordData.m_status = eScriptedTaskStatus::EVENT_ASSOCIATED;
                else
                    taskRecordData.Flush();
            }
            else if (taskRecordData.m_task) {
                eScriptedTaskStatus status = taskRecordData.m_status;
                if (status == eScriptedTaskStatus::GROUP || status == eScriptedTaskStatus::BRAIN) {
                    CTask* scriptedTask = nullptr;
                    if (status == eScriptedTaskStatus::GROUP) {
                        CPedGroup* pedGroup = CPedGroups::GetPedsGroup(taskRecordData.m_ped);
                        if (pedGroup)
                            scriptedTask = pedGroup->GetIntelligence().GetTaskScriptCommand(taskRecordData.m_ped);
                    }
                    else if (status == eScriptedTaskStatus::BRAIN) {
                        scriptedTask = CScriptedBrainTaskStore::GetTask(taskRecordData.m_ped);
                    }
                    if (scriptedTask != taskRecordData.m_task)
                        taskRecordData.Flush();
                }
                else {
                    CTaskManager& taskManager = taskRecordData.m_ped->GetTaskManager();
                    if (taskManager.GetActiveTask() == taskRecordData.m_task)
                        taskRecordData.m_status = eScriptedTaskStatus::TASK_ASSOCIATED;
                    else if (taskManager.HasTaskPrimary(taskRecordData.m_task))
                        taskRecordData.m_status = eScriptedTaskStatus::PRIMARY;
                    else if (taskManager.HasTaskSecondary(taskRecordData.m_task))
                        taskRecordData.m_status = eScriptedTaskStatus::SECONDARY;
                    else
                        taskRecordData.Flush();
                }
            }
        }
    }
}
