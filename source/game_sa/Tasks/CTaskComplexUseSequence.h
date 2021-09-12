#pragma once

#include "CTaskComplex.h"

class CPed;

class CTaskComplexUseSequence : public CTaskComplex {
public:
    int32 m_nSequenceIndex;         // Used in CTaskSequences::ms_taskSequence global array
    int32 m_nCurrentTaskIndex;      // Used in CTaskComplexSequence::m_aTasks array
    int32 m_nEndTaskIndex;          // Sequence will stop performing tasks when current index is equal to endTaskIndex
    int32 m_nSequenceRepeatedCount; // m_nSequenceRepeatedCount simply tells us how many times the sequence has been repeated.
                                  // If CTaskComplexSequence::m_bRepeatSequence is true, this can be greater than 1, 
                                  // otherwise it's set to 1 when the sequence is done executing tasks.

public:
    CTaskComplexUseSequence() {} // blame R* for this
    CTaskComplexUseSequence(int32 sequenceIndex);
    ~CTaskComplexUseSequence();

    CTask* Clone() override;
    eTaskType GetTaskType() override;
    bool MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) override;
    CTask* CreateNextSubTask(CPed* ped) override;
    CTask* CreateFirstSubTask(CPed* ped) override;
    CTask* ControlSubTask(CPed* ped) override;

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CTaskComplexUseSequence* Constructor(int32 sequenceIndex);

    CTask* Clone_Reversed();
    eTaskType GetId_Reversed() { return TASK_COMPLEX_USE_SEQUENCE; };
    bool MakeAbortable_Reversed(CPed* ped, eAbortPriority priority, const CEvent* event);
    CTask* CreateNextSubTask_Reversed(CPed* ped);
    CTask* CreateFirstSubTask_Reversed(CPed* ped);
    CTask* ControlSubTask_Reversed(CPed* ped);
};

VALIDATE_SIZE(CTaskComplexUseSequence, 0x1C);