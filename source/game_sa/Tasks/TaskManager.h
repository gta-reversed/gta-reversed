/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Base.h"
#include <initializer_list>
#include "Task.h"
#include <algorithm>

enum ePrimaryTasks // array indices
{
    TASK_PRIMARY_INVALID = -1,

    TASK_PRIMARY_PHYSICAL_RESPONSE = 0,
    TASK_PRIMARY_EVENT_RESPONSE_TEMP,
    TASK_PRIMARY_EVENT_RESPONSE_NONTEMP,
    TASK_PRIMARY_PRIMARY,
    TASK_PRIMARY_DEFAULT,
    TASK_PRIMARY_MAX
};

enum eSecondaryTask : uint32 // array indexes
{
    TASK_SECONDARY_INVALID = (uint32)-1,

    TASK_SECONDARY_ATTACK = 0,              // want duck to be after attack
    TASK_SECONDARY_DUCK,                    // because attack controls ducking movement
    TASK_SECONDARY_SAY,
    TASK_SECONDARY_FACIAL_COMPLEX,
    TASK_SECONDARY_PARTIAL_ANIM,
    TASK_SECONDARY_IK,
    TASK_SECONDARY_MAX
};

class CTaskComplex;
class CTaskSimple;
class CPed;
class CTaskComplexFacial;

class CTaskManager {
public:

    std::array<CTask*, TASK_PRIMARY_MAX>   m_aPrimaryTasks{};
    std::array<CTask*, TASK_SECONDARY_MAX> m_aSecondaryTasks{};
    CPed*                                  m_pPed{};

public:
    static void InjectHooks();

    explicit CTaskManager(CPed* ped);
    ~CTaskManager();

    /*!
    * @0x681720
    * @brief Get the first present primary task (that is, the first non-null entry from `m_aPrimaryTasks`)
    */
    CTask* GetActiveTask();

    /*!
    * @notsa
    * @brief Get the highest priority task's index
    */
    size_t GetActiveTaskIndex() const;

    /*!
    * @addr 0x681740
    * @brief Find the first task with type `taskType`
    * Can be replced with `Find<T>(true)`
    */
    CTask* FindActiveTaskByType(eTaskType taskType);

    /*!
    * @addr 0x681810
    * @brief Similar to `FindActiveTaskByType` but only checks the given primary task's subtasks
    */
    CTask* FindTaskByType(ePrimaryTasks taskIndex, eTaskType taskType);

    /*!
    * @addr 0x681810
    * @brief Get the secondary task at `taskIndex`
    */
    CTask* GetTaskSecondary(eSecondaryTask taskIndex);

    /*!
     * @brief GetTaskSecondary(TASK_SECONDARY_FACIAL_COMPLEX)
     * @return The facial task (if any)
    */
    CTaskComplexFacial* GetTaskSecondaryFacial();

    /*
    * @addr unknown
    * @brief Check if any of the tasks in `m_aPrimaryTasks` is `task`
    */
    bool HasTaskPrimary(const CTask* task);

    /*
    * @addr 0x681820
    * @brief Check if any of the tasks in `m_aSecondaryTasks` is `task`
    */
    bool HasTaskSecondary(const CTask* task);

    /*!
    * @addr 0x6818A0
    * @brief Try aborting all tasks, if successful they're deleted
    */
    void FlushImmediately();

    /*!
    * @addr 0x681850
    * @brief Similar to `FlushImmediately` but doesn't try to abort, just deletes all tasks.
    */
    void Flush();

    /*!
    * @addr 0x681920
    * @brief Set the next sub task of a task in the task tree of `task`
    *
    * Traverses the task tree until up until it can create a new sub-task (Using `CreateNextSubTask`)
    * All old sub-tasks are deleted (freed) (By `SetSubTask`).
    * If no new sub-task can be created the last task (that is in one of the task slots) will not be freed.
    */
    void SetNextSubTask(CTaskComplex* task);

    void StopTimers(const CEvent* event);

    /*
    * @addr 0x6819D0
    * @brief Get simplest active primary task
    */
    CTask* GetSimplestActiveTask() { return GetLastTaskOf(GetActiveTask()); }

    /*
    * @addr 0x681A00
    * @brief Last task in the task-chain
    */
    CTask* GetLastTaskOf(ePrimaryTasks taskIndex) const { return GetLastTaskOf(GetTaskPrimary(taskIndex)); }

    /*!
    * @addr 0x681970
    * @param The first task, `nullptr` is allowed, in which case it is returned.
    * @return Last task in the task-chain (Possible `task` itself) 
    *
    * This function had horrible naming, original name was `GetSimplestTask`
    */
    static CTask* GetLastTaskOf(CTask* task);

    /*!
    * @addr 0x681A30
    * @brief Create the subtasks recursively iff `task` is complex.
    */
    void AddSubTasks(CTask* task);

    /*!
    * @addr 0x681A80
    * @brief Control the given task and all it's subtasks, as in, call `ControlSubTask` on each one (iff it's complex)
    */
    void ParentsControlChildren(CTask* task);

    /*!
    * @addr 0x681AF0
    * @brief Set primary task
    * @param task The new task, might be null, in case the specified task is to be removed
    * @param taskIndex The index of the primary task to be changed
    */
    void SetTask(CTask* task, ePrimaryTasks taskIndex, bool unused = false) { ChangeTaskInSlot(m_aPrimaryTasks[taskIndex], task); }

    /*!
    * @addr 0x681B60
    * @brief Set the secondary task
    * @param task The new, dynamically allocated, task, might be null, in case the specified task will be removed.
    * @param taskIndex The index of the secondary task to be changed
    */
    void SetTaskSecondary(CTask* task, eSecondaryTask taskIndex) { ChangeTaskInSlot(m_aSecondaryTasks[taskIndex], task); }

    /*!
    * Abort the first task found out of the primary tasks given
    * @notsa
    */
    void AbortFirstPrimaryTaskIn(std::initializer_list<ePrimaryTasks> idxs, CPed* ped, eAbortPriority priority = ABORT_PRIORITY_LEISURE, const CEvent* event = nullptr);

    /*!
    * @brief Clear primary tasks `TASK_PRIMARY_EVENT_RESPONSE_TEMP` and `TASK_PRIMARY_EVENT_RESPONSE_NONTEMP`
    */
    void ClearTaskEventResponse();

    /*!
    * @addr 0x681C10
    */
    void ManageTasks();

    // Why they doesn't have version for *primary tasks*? :thinking:
    CTask* GetTaskPrimary(int32 taskIndex) const noexcept {
        return m_aPrimaryTasks[taskIndex];
    }

    //! @notsa
    CTask* GetPresistentEventResponseTask() const;

    //! @notsa
    CTask* GetTemporaryEventResponseTask() const;

    /*!
    * Get all the primary tasks [there might be null entries]
    */
    auto& GetPrimaryTasks() const { return m_aPrimaryTasks; }

    /*!
    * Get all the primary tasks [there might be null entries]
    */
    auto& GetSecondaryTasks() const { return m_aSecondaryTasks; }

    // NOTSA - Check if any of the given tasks is active
    bool IsAnyTaskActiveByType(std::initializer_list<eTaskType> types) {
        return rng::any_of(types, [this](auto type) {
            return FindActiveTaskByType(type) != nullptr;
        });
    }

    // TODO: Replace with `Find<>(true)`
    CTask* FindActiveTaskFromList(std::initializer_list<eTaskType> types) {
        for (auto type : types) {
            if (const auto task = FindActiveTaskByType(type)) {
                return task;
            }
        }
        return nullptr;
    }

    /*!
    * @notsa
    * @brief Find active task, check if its of type `T`, and return it, nullptr otherwise (if not found/not of the requested type
    */
    template<Task T>
    T* GetActiveTaskAs() {
        if (const auto task = GetActiveTask()) {
            if (task->GetTaskType() == T::Type) {
                return static_cast<T*>(task);
            }
        }
        return nullptr;
    }

    /*!
    * @notsa
    * @brief Find simplest active task, check if its of type `T`, and return it, nullptr otherwise (if not found/not of the requested type)
    */
    template<Task T>
    T* GetSimplestActiveTaskAs() {
        return CTask::DynCast<T>(GetSimplestActiveTask());
    }

    /*!
    * @notsa
    * @brief Check if the simplest active task is any of the given types
    */
    bool IsSimplestActiveTaskOfType(std::initializer_list<eTaskType> types) {
        if (const auto task = GetSimplestActiveTask()) {
            return notsa::contains(types, task->GetTaskType());
        }
        return false;
    }

    /*!
    * @notsa
    * @brief Find task from the give types and return the first one.
    */
    template<eTaskType... Ts>
    CTask* Find(bool activeOnly = true) { // TODO: For now just return `CTask*`, but would be nice to return the first common base class somehow
        // This won't work if the task has no `Type` member
        // If it has a `GetTaskType` function feel free to add it,
        // otherwise don't.
        CTask* ret{};
        if (activeOnly) {
            (... || (ret = FindActiveTaskByType(Ts)));
        } else {
            const auto FindPrimaryTaskByType = [this](eTaskType type) -> CTask* { // Based on `CPedIntelligence::FindTaskByType`
                for (const auto idx : { TASK_PRIMARY_DEFAULT, TASK_PRIMARY_PRIMARY, TASK_PRIMARY_EVENT_RESPONSE_TEMP, TASK_PRIMARY_EVENT_RESPONSE_NONTEMP }) {
                    if (const auto task = FindTaskByType(idx, type)) {
                        return task;
                    }
                }
                return nullptr;
            };
            (... || (ret = FindPrimaryTaskByType(Ts)));
        }
        return ret;
    }

    /*!
    * @notsa
    * @brief Find a task from the given types and return the first one.
    */
    template<Task... Ts>
        requires(sizeof...(Ts) > 1)
    CTask* Find(bool activeOnly = true) {
        return Find<Ts::Type...>(activeOnly);
    }

    /*!
    * @notsa
    * @brief Find task of the given type `T`
    */
    template<Task T>
    T* Find(bool activeOnly = true) {
        return static_cast<T*>(Find<T::Type>(activeOnly));
    }

    /*!
    * @notsa
    * @brief Check if any of the tasks is of the given types
    */
    template<Task... Ts>
    bool HasAnyOf(bool activeOnly = true) {
        return (... || Find<Ts::Type>(activeOnly));
    }

    template<eTaskType... Ts>
    bool HasAnyOf(bool activeOnly = true) {
        return (... || Find<Ts>(activeOnly));
    }

    /*!
    * @notsa
    * @brief Check if any active task is of type `T`
    */
    template<eTaskType T>
    bool Has(bool activeOnly = true) {
        return Find<T>(activeOnly);
    }

    /*!
    * @notsa
    * @brief Check if any active task is of type `T`
    */
    template<Task T>
    bool Has(bool activeOnly = true) {
        return Find<T::Type>(activeOnly);
    }

    /*!
    * @notsa
    * @brief Check if the first found task is the same in both managers
    */
    template<eTaskType... Ts>
    bool IsFirstFoundTaskMatching(CTaskManager& mgr) {
        if (const auto our = Find<Ts...>()) {
            if (const auto other = mgr.Find<Ts...>()) {
                if (our->GetTaskType() == other->GetTaskType()) {
                    return true;
                }
            }
        }
        return false;
    }

    /*!
    * @notsa
    * @brief Apply function to all tasks in `m_aPrimaryTasks` and `m_aSecondaryTasks`
    */
    void ApplyToRootTasks(const auto& fn) {
        rng::for_each(m_aPrimaryTasks, fn);
        rng::for_each(m_aSecondaryTasks, fn);
    }
protected:
    /*
    * @notsa
    * @brief Helper of `SetTask`, `SetTaskSecondary`
    *
    * @param taskInSlot Reference to either `m_aPrimaryTasks` and `m_aSecondaryTasks`
    * @param changeTo The new slot to be set, allowed to be `nullptr`, in which case `taskInSlot` is deleted and nulled out
    */
    void ChangeTaskInSlot(CTask*& taskInSlot, CTask* changeTo);

private:
    CTaskManager* Constructor(CPed* ped) {
        this->CTaskManager::CTaskManager(ped);
        return this;
    }

    CTaskManager* Destructor() {
        this->CTaskManager::~CTaskManager();
        return this;
    }
};

VALIDATE_SIZE(CTaskManager, 0x30);
