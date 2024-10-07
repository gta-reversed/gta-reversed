/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Task.h"

class CPed;

class NOTSA_EXPORT_VTABLE CTaskComplex : public CTask {
public:
    CTaskComplex() = default;
    ~CTaskComplex() override {
        delete m_pSubTask;
        m_pSubTask = nullptr;
    }

    CTask* GetSubTask() const final override { return m_pSubTask; }
    bool IsSimple() const final override { return false; }

    bool MakeAbortable(class CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override {
        return m_pSubTask->MakeAbortable(ped, priority, event);
    }

    virtual void SetSubTask(CTask* subTask) final { // TODO: Make this non-virtual
        if (m_pSubTask == subTask) { // Same task set already, do nothing
            return;
        }
        delete std::exchange(m_pSubTask, subTask); // Delete old, set new
        if (subTask) {
            subTask->m_Parent = this;
        }
    }

    /*
    * @brief Create the next sub-task after the previous has finished.
    *
    * @param ped The ped this task belongs to
    *
    * @return The new sub-task
    */
    virtual CTask* CreateNextSubTask(CPed* ped) = 0;

    /*!
     * @brief Creates the first sub-task. Called once after task creation
     *
     * @param ped The ped this task belongs to
     * 
     * @return The sub-task, or null (to indicate that the task has finished)
    */
    virtual CTask* CreateFirstSubTask(CPed* ped) = 0;

    /*!
    * @brief Control this task's sub-task
    *
    * This function is called each update tick.
    * 
    * @param ped The ped this task belongs to
    * 
    * @return A new subtask   - To be set as the new sub-task (Aborting (Using `MakeAbortable`) the previous one)
    * @return The old subtask - If sub-task task is still active
    * @return Null            - Indicating that this task has finished (This causes the parent (if any) to create a new sub-task (using `CreateNextSubTask`))
    */
    virtual CTask* ControlSubTask(CPed* ped) = 0;

protected:
    CTask* m_pSubTask{};
};
VALIDATE_SIZE(CTaskComplex, 0xC);
