/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/

#include "StdInc.h"

#include "TaskManager.h"

#include "TaskComplexFacial.h"

void CTaskManager::InjectHooks() {
    RH_ScopedClass(CTaskManager);
    RH_ScopedCategory("Tasks");

    RH_ScopedInstall(Constructor, 0x6816A0);
    RH_ScopedInstall(Destructor, 0x6816D0);
    RH_ScopedInstall(GetActiveTask, 0x681720);
    RH_ScopedInstall(FindActiveTaskByType, 0x681740);
    RH_ScopedInstall(FindTaskByType, 0x6817D0);
    RH_ScopedInstall(GetTaskSecondary, 0x681810);
    RH_ScopedInstall(HasTaskSecondary, 0x681820);
    RH_ScopedInstall(Flush, 0x681850);
    RH_ScopedInstall(FlushImmediately, 0x6818A0);
    RH_ScopedInstall(SetNextSubTask, 0x681920);
    //RH_ScopedOverloadedInstall(GetLastTaskOf, "of-task", 0x681970, CTask*(*)(CTask*));
    //RH_ScopedOverloadedInstall(GetLastTaskOf, "at-index", 0x681A00, CTask*(CTaskManager::*)(ePrimaryTasks));
    RH_ScopedInstall(StopTimers, 0x6819A0);
    RH_ScopedInstall(GetSimplestActiveTask, 0x6819D0);
    RH_ScopedInstall(AddSubTasks, 0x681A30);
    RH_ScopedInstall(ParentsControlChildren, 0x681A80);
    RH_ScopedInstall(SetTask, 0x681AF0);
    RH_ScopedInstall(SetTaskSecondary, 0x681B60);
    RH_ScopedInstall(ClearTaskEventResponse, 0x681BD0);
    RH_ScopedInstall(ManageTasks, 0x681C10);
}

static void DeleteTaskAndNull(CTask*& task) {
    delete task;
    task = nullptr;
}

/// Find the first task (or subtask) that is of the required type
inline CTask* FindFirstTaskOfType(CTask* task, eTaskType type) {
    for (; task; task = task->GetSubTask()) {
        if (task->GetTaskType() == type) {
            return task;
        }
    }
    return nullptr;
}

// 0x6816A0
CTaskManager::CTaskManager(CPed* ped) :
    m_pPed{ped}
{
}

// 0x6816D0
CTaskManager::~CTaskManager() {
    Flush();
}

// 0x681720
CTask* CTaskManager::GetActiveTask() {
    for (const auto task : m_aPrimaryTasks) {
        if (task) {
            return task;
        }
    }
    return nullptr;
}

size_t CTaskManager::GetActiveTaskIndex() const {
    for (auto&& [i, task] : notsa::enumerate(m_aPrimaryTasks)) {
        if (task) {
            return (size_t)i;
        }
    }
    return (size_t)-1;
}

// 0x681740
CTask* CTaskManager::FindActiveTaskByType(eTaskType taskType) {
    // First try current active task and it's sub-tasks
    if (const auto task = FindFirstTaskOfType(GetActiveTask(), taskType)) {
        return task;
    }

    // The same as above, but for all secondary tasks
    for (const auto sec : m_aSecondaryTasks) {
        if (const auto task = FindFirstTaskOfType(sec, taskType)) {
            return task;
        }
    }

    // Not found
    return nullptr;
}

// 0x6817D0
CTask* CTaskManager::FindTaskByType(ePrimaryTasks taskIndex, eTaskType taskId) {
    return FindFirstTaskOfType(GetTaskPrimary(taskIndex), taskId);
}

// 0x681810
CTask* CTaskManager::GetTaskSecondary(eSecondaryTask taskIndex) {
    return m_aSecondaryTasks[taskIndex];
}

CTaskComplexFacial* CTaskManager::GetTaskSecondaryFacial() {
    return CTask::Cast<CTaskComplexFacial>(GetTaskSecondary(TASK_SECONDARY_FACIAL_COMPLEX));
}

// NOTSA?
bool CTaskManager::HasTaskPrimary(const CTask* task) {
    for (auto& primaryTask : m_aPrimaryTasks) {
        if (primaryTask && primaryTask == task) {
            return true;
        }
    }
    return false;
}

// 0x681820
bool CTaskManager::HasTaskSecondary(const CTask* task) {
    for (auto& secondaryTask : m_aSecondaryTasks) {
        if (secondaryTask && secondaryTask == task) {
            return true;
        }
    }
    return false;
}

// 0x681850
void CTaskManager::Flush() {
    ApplyToRootTasks(DeleteTaskAndNull);
}

// 0x6818A0
void CTaskManager::FlushImmediately() {
    ApplyToRootTasks([this](CTask*& task) {
        if (task && task->MakeAbortable(m_pPed, ABORT_PRIORITY_IMMEDIATE, nullptr)) {
            delete std::exchange(task, nullptr);
        }
    });
}

// 0x681920
void CTaskManager::SetNextSubTask(CTaskComplex* ofTask) {
    if (!ofTask) {
        return;
    }

    assert(ofTask->IsComplex());

    for (auto task = ofTask; task; task = task->GetParent()) {
        if (const auto next = task->CreateNextSubTask(m_pPed)) {
            task->SetSubTask(next);
            AddSubTasks(next);
            return;
        }

        // If no subtask is created, the `task` is considered done.
        // So we go to it's parent and create it's next subtask.
        task->SetSubTask(nullptr); // This also deletes the previous sub-task
    }
}

// 0x681970
CTask* CTaskManager::GetLastTaskOf(CTask* task) {
    auto last{task};
    for (; task; task = task->GetSubTask())
        last = task;
    return last;
}

// 0x6819A0
void CTaskManager::StopTimers(const CEvent* event) {
    for (auto& primaryTask : m_aPrimaryTasks) {
        if (primaryTask) {
            primaryTask->StopTimer(event);
        }
    }
}

// 0x681A30
void CTaskManager::AddSubTasks(CTask* toTask) {
    if (!toTask) {
        return;
    }

    for (auto task = toTask; !task->IsSimple();) {
        const auto ctask = task->AsComplex();
        assert(GetTaskPool()->IsObjectValid(task)); // Sometimes tasks seem to get deleted which causes pure virtual calls (because of destructcors setting the VMT to their's)
        if (const auto sub = ctask->CreateFirstSubTask(m_pPed)) {
            ctask->SetSubTask(sub);
            task = sub; // Go on creating the subtask of the created task
        } else { // No sub task created, so we can stop here
            if (const auto parent = ctask->GetParent()) {
                SetNextSubTask(parent);
            }
            break;
        }
    }
}

// 0x681A80
void CTaskManager::ParentsControlChildren(CTask* parent) {
    for (auto task = parent; task && !task->IsSimple();) {
        const auto ctask      = task->AsComplex();
        const auto oldSubTask = ctask->GetSubTask();

        const auto newSubTask = ctask->ControlSubTask(m_pPed);

        if (newSubTask != oldSubTask) {
            oldSubTask->MakeAbortable(m_pPed);
            ctask->SetSubTask(newSubTask);
            AddSubTasks(newSubTask);
            return;
        }

        task = oldSubTask;
    }
}

void CTaskManager::AbortFirstPrimaryTaskIn(std::initializer_list<ePrimaryTasks> slots, CPed* ped, eAbortPriority priority, const CEvent* event) {
    for (const auto slot : slots) {
        if (const auto task = GetTaskPrimary(slot)) {
            task->MakeAbortable(ped, priority, event);
            break;
        }
    }
}

// 0x681BD0
void CTaskManager::ClearTaskEventResponse() {
    // Looking at the code, I think `nontemp` is only present if `temp` is, as it's not deleted otherwise.
    for (const auto idx : { TASK_PRIMARY_EVENT_RESPONSE_TEMP, TASK_PRIMARY_EVENT_RESPONSE_NONTEMP }) {
        if (auto& task = m_aPrimaryTasks[idx]) {
            DeleteTaskAndNull(task);
            break;
        }
    }
}

// 0x681C10
void CTaskManager::ManageTasks() {
    // Obviously this isn't how they did it, but, I wanted it to be readable...
    enum ProcessResult {
        TT_FINISHED, //< The task tree has finished.
        TT_SUCCESS,  //< There's currently an active task.
        TT_ADVANCED  //< New sub-tasks were created, and there's currently an active task.
    };
    const auto ProcessTaskTree = [this](CTask* root, bool bIsPrimary) {
        ParentsControlChildren(root);

        auto lastTask = GetLastTaskOf(root);

        // The task tree can only be "terminated" with a `Simple` task. (Because a complex task must always have a sub-task)
        // If the last task is complex that means that creating all sub-tasks (See `AddSubTasks`) failed.
        // (Meaning that the task tree has finished and the root can be deleted)
        if (!lastTask->IsSimple()) {
            if (!bIsPrimary) {
                return TT_FINISHED;
            }

            // Try setting next sub-task
            SetNextSubTask(lastTask->GetParent());

            // If setting the next sub-task above was successful then the last task has changed
            lastTask = GetLastTaskOf(root);

            // A complex task in this case means that the primary task has finished
            if (lastTask->IsComplex()) {
                return TT_FINISHED;
            }
        }

        // Check if the simple task has finished
        if (!lastTask->AsSimple()->ProcessPed(m_pPed)) {
            return TT_SUCCESS;
        }

        // It has, try creating the next sub-task
        SetNextSubTask(lastTask->GetParent());

        // Check if the task tree has finished (See `SetNextSubTask` for more info)
        if (!root->GetSubTask()) {
            return TT_FINISHED;
        }

        // New sub-tasks were successfully created
        return TT_ADVANCED;
    };

    // Process active primary task's tree
    if (const auto activeTaskIdx = GetActiveTaskIndex(); activeTaskIdx != (size_t)-1) {
        auto*& activePrimaryTask = m_aPrimaryTasks[activeTaskIdx];

        if (!GetLastTaskOf(activePrimaryTask)->IsSimple()) {
            delete std::exchange(activePrimaryTask, nullptr);
            return;
        }

        // The `10` here has no particular meaning, it's just an arbitrary iteration limit
        for (size_t i = 0; i < 10; i++) {
            const auto res = ProcessTaskTree(activePrimaryTask, true);

            if (res == TT_FINISHED) {
                delete std::exchange(activePrimaryTask, nullptr);
            }

            if (res != TT_ADVANCED) { // TT_FINISHED, TT_SUCCESS
                break;
            }
        }
    }
    
    // Process secondary task-tree's
    for (auto& secondaryTask : m_aSecondaryTasks) {
        if (!secondaryTask) {
            continue;
        }
        ProcessResult res;
        while ((res = ProcessTaskTree(secondaryTask, false)) == TT_ADVANCED);
        if (res == TT_FINISHED) {
            delete std::exchange(secondaryTask, nullptr);
        }
    }
}

// @notsa
CTask* CTaskManager::GetPresistentEventResponseTask() const {
    for (const auto i : { TASK_PRIMARY_PHYSICAL_RESPONSE, TASK_PRIMARY_EVENT_RESPONSE_NONTEMP }) {
        if (const auto t = GetTaskPrimary(i)) {
            return t; 
        }
    }
    return nullptr;
}

CTask* CTaskManager::GetTemporaryEventResponseTask() const {
    return  GetTaskPrimary(TASK_PRIMARY_EVENT_RESPONSE_TEMP);
}

void CTaskManager::ChangeTaskInSlot(CTask*& taskInSlot, CTask* changeTo) {
    // Nothing to do in case they're the same
    if (taskInSlot == changeTo) {
        return;
    }

    // Delete current
    delete taskInSlot;

    // Set the task in the slot, and add it's subtasks
    taskInSlot = changeTo;
    AddSubTasks(taskInSlot);

    // Task trees must always end with a simple task, so if that's not the case delete it immidiately.
    if (taskInSlot && !GetLastTaskOf(taskInSlot)->IsSimple()) {
        delete std::exchange(taskInSlot, nullptr);
    }
}
