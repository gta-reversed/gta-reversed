#include "StdInc.h"
#include "TaskComplexUseScriptedBrain.h"
#include "ScriptedBrainTaskStore.h"
#include "TaskSimplePause.h"

void CTaskComplexUseScriptedBrain::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexUseScriptedBrain, 0x86e568, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x6358B0);
    RH_ScopedInstall(Destructor, 0x635900);

    RH_ScopedVMTInstall(Clone, 0x6371F0);
    RH_ScopedVMTInstall(GetTaskType, 0x6358F0);
    RH_ScopedVMTInstall(MakeAbortable, 0x635970);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x6359C0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x635A90);
    RH_ScopedVMTInstall(ControlSubTask, 0x635B60);
}

// 0x6358B0
CTaskComplexUseScriptedBrain::CTaskComplexUseScriptedBrain(char const* name) {
    strcpy_s(m_name, name);
}

// NOTSA
CTaskComplexUseScriptedBrain::CTaskComplexUseScriptedBrain(const CTaskComplexUseScriptedBrain& o) :
    CTaskComplexUseScriptedBrain{ m_name }
{
}

// 0x635900
CTaskComplexUseScriptedBrain::~CTaskComplexUseScriptedBrain() {
    ClearCurrentTask();
}

// 0x635970
bool CTaskComplexUseScriptedBrain::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    if (!event || event->GetEventType() != EVENT_SCRIPT_COMMAND) {
        return m_pSubTask->MakeAbortable(ped, priority, event);
    }

    if (auto* e = CEvent::DynCast<const CEventScriptCommand>(event); !e || e->m_task->GetTaskType() != TASK_COMPLEX_USE_EFFECT) {
        return m_pSubTask->MakeAbortable(ped, priority, event);
    }

    m_aborting = true;
    return false;
}

// 0x6359C0
CTask* CTaskComplexUseScriptedBrain::CreateNextSubTask(CPed* ped) {
    ClearCurrentTask();

    if (const auto task = CreateFirstSubTask(ped)) {
        return task;
    }

    return new CTaskSimplePause{ 2000 };
}

// 0x635A90
CTask* CTaskComplexUseScriptedBrain::CreateFirstSubTask(CPed* ped) {
    if (m_currTask = CScriptedBrainTaskStore::GetTask(ped)) {
        return m_currTask->Clone();
    }
    return nullptr;
}

// 0x635B60
CTask* CTaskComplexUseScriptedBrain::ControlSubTask(CPed* ped) {
    const auto brainsTask = CScriptedBrainTaskStore::GetTask(ped);

    // Task is same, or can't abort it..
    if (brainsTask == m_currTask || !m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_LEISURE, nullptr)) {
        return m_pSubTask;
    }

    // Return task cloned (if any)
    return brainsTask
        ? brainsTask->Clone()
        : nullptr;
}

void CTaskComplexUseScriptedBrain::ClearCurrentTask() {
    if (m_currTask) {
        CScriptedBrainTaskStore::Clear(m_currTask);
        m_currTask = nullptr;
    }
}
