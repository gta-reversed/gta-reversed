/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "eTaskType.h"

enum eAbortPriority : int32 {
    ABORT_PRIORITY_LEISURE = 0,
    ABORT_PRIORITY_URGENT,
    ABORT_PRIORITY_IMMEDIATE
};

class CEvent;
class CPed;
class CTaskSimple;
class CTaskComplex;
class CTask;

template<typename T>
concept Task = std::is_base_of_v<CTask, T>;

class NOTSA_EXPORT_VTABLE CTask {
    friend class CTaskComplex;

public:
    static bool IsGoToTask(CTask* task);
    static bool IsTaskPtr(CTask* task);

    void* operator new(size_t size);
    void operator delete(void* object);

    CTask() { m_Parent = nullptr; } // 0x61A340
    virtual ~CTask() = default; // 0x61A660

    //! @return A clone of this task
    virtual CTask* Clone() const = 0;

    //! @return The subtask
    virtual CTask* GetSubTask() const = 0;

    //! @brief If this task is of base type `CTaskSimple`
    virtual bool IsSimple() const = 0;

    //! @brief If this task is of base type `CTaskComplex`
    bool IsComplex() const { return !IsSimple(); }

    //! @return This task's type
    virtual eTaskType GetTaskType() const = 0;

    //! @brief Stop internal timer due to some event
    virtual void StopTimer(const CEvent* event);

    /*!
    * @brief Potentially causes the task to finish earlier than it would otherwise
    *
    * @param ped The ped this task belongs to
    * @param priority The priority (LEISURE, URGENT, IMMIDIATE)
    * @param event The event that for which the task should be aborted for
    *
    * @return If the task was made abortable
    */
    virtual bool MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) = 0;

    //! @return This task as a `CTaskSimple`
    auto AsSimple()  { assert(IsSimple()); return reinterpret_cast<CTaskSimple*>(this); }

    //! @return This task as a `CTaskComplex`
    auto AsComplex() { assert(IsComplex()); return reinterpret_cast<CTaskComplex*>(this); }

    //! @return The parent of this task. The parent is always of base type `CTaskComplex` (Because only complex tasks can have sub-tasks)
    auto GetParent() const { return m_Parent; }

    //! @breif Clone a task and check if it's of the specified type
    template<Task T>
    static T* CloneIfIs(CTask* t) {
        return t && notsa::isa<T>(t)
            ? notsa::cast_if_present<T>(t->Clone())
            : nullptr;
    }

public: // Casting.hpp support //
    template<typename From, typename Self>
    static constexpr bool classof(const From* f) { return f->GetTaskType() == Self::Type; }

protected:
    CTaskComplex* m_Parent{};
};
VALIDATE_SIZE(CTask, 0x8);
