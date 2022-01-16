#pragma once

#include "TaskComplex.h"

class CTask;
    
class CTaskComplexSequence : public CTaskComplex {
public:
    int32    m_nCurrentTaskIndex;      // Used in m_aTasks
    CTask*   m_aTasks[8];
    int32    m_bRepeatSequence;        // it's either 1 or 0. Sequence will loop if set to 1
    int32    m_nSequenceRepeatedCount; // m_nSequenceRepeatedCount simply tells us how many times the sequence has been repeated.
                                       // If m_bRepeatSequence is true, this can be greater than 1,
                                       // otherwise it's set to 1 when the sequence is done executing tasks.
    bool     m_bFlushTasks;
    char     field_39;
    char     field_3A;
    char     field_3B;
    uint8    m_nReferenceCount; // count of how many CTaskComplexUseSequence instances are using this sequence

public:
    CTaskComplexSequence();
    ~CTaskComplexSequence();

    // original virtual functions
    CTask* Clone() override;
    eTaskType GetTaskType() override;
    bool MakeAbortable(class CPed* ped, eAbortPriority priority, const CEvent* event) override;
    CTask* CreateNextSubTask(CPed* ped) override;
    CTask* CreateFirstSubTask(CPed* ped) override;
    CTask* ControlSubTask(CPed* ped) override;

    void AddTask(CTask* pTask);
    CTask* CreateNextSubTask(CPed* ped, int32& taskIndex, int32& repeatCount);
    void Flush();

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