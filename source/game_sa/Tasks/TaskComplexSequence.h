#pragma once

#include "TaskComplex.h"

class CTask;
    
class CTaskComplexSequence : public CTaskComplex {
public:
    int32    m_nCurrentTaskIndex;      // Used in m_aTasks
    CTask*   m_aTasks[8];
    int32    m_bRepeatSequence;        // Sequence will loop if set to 1
    int32    m_nSequenceRepeatedCount; // m_nSequenceRepeatedCount simply tells us how many times the sequence has been repeated.
                                       // If m_bRepeatSequence is true, this can be greater than 1,
                                       // otherwise it's set to 1 when the sequence is done executing tasks.
    bool     m_bFlushTasks;
    uint32   m_nReferenceCount; // count of how many CTaskComplexUseSequence instances are using this sequence

public:
    CTaskComplexSequence();
    ~CTaskComplexSequence() override;

    CTask* Clone() override;
    eTaskType GetTaskType() override;
    bool MakeAbortable(class CPed* ped, eAbortPriority priority, const CEvent* event) override;
    CTask* CreateNextSubTask(CPed* ped) override;
    CTask* CreateFirstSubTask(CPed* ped) override;
    CTask* ControlSubTask(CPed* ped) override;

    void AddTask(CTask* task);
    CTask* CreateNextSubTask(CPed* ped, int32& taskIndex, int32& repeatCount);
    void Flush();
    bool Contains(eTaskType taskType);
    void f0x463610(bool flush);
    void f0x636BC0();

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CTaskComplexSequence* Constructor();

    CTask* Clone_Reversed();
    eTaskType GetId_Reversed() { return TASK_COMPLEX_SEQUENCE; }
    bool MakeAbortable_Reversed(class CPed* ped, eAbortPriority priority, const CEvent* event);
    CTask* CreateNextSubTask_Reversed(CPed* ped);
    CTask* CreateFirstSubTask_Reversed(CPed* ped);
    CTask* ControlSubTask_Reversed(CPed* ped);
};

VALIDATE_SIZE(CTaskComplexSequence, 0x40);
