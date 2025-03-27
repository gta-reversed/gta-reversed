#pragma once

#include "TaskComplex.h"

class CPed;

class NOTSA_EXPORT_VTABLE CTaskComplexUseSequence : public CTaskComplex {
public:
    static constexpr auto Type = TASK_COMPLEX_USE_SEQUENCE;

    CTaskComplexUseSequence() = default; // blame R* for this
    explicit CTaskComplexUseSequence(int32 sequenceIndex);
    CTaskComplexUseSequence(const CTaskComplexUseSequence&);
    ~CTaskComplexUseSequence() override;

    eTaskType GetTaskType() const override { return Type; } // 0x635490
    CTask* Clone() const override { return new CTaskComplexUseSequence{*this}; } // 0x637100
    bool MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    CTask* CreateNextSubTask(CPed* ped) override;
    CTask* CreateFirstSubTask(CPed* ped) override;
    CTask* ControlSubTask(CPed* ped) override;

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CTaskComplexUseSequence* Constructor(int32 sequenceIndex);


public:
    int32 m_nSequenceIndex{};         /**< Used in CTaskSequences::ms_taskSequence global array */
    int32 m_nCurrentTaskIndex{};      /**< Used in CTaskComplexSequence::m_aTasks array */
    int32 m_nEndTaskIndex{-1};        /**< Sequence will stop performing tasks when current index is equal to endTaskIndex */
    int32 m_nSequenceRepeatedCount{}; /**< This simply tells us how many times the sequence has been repeated.
                                           If `CTaskComplexSequence::m_bRepeatSequence` is true, this can be greater than 1,
                                           otherwise it's set to 1 when the sequence is done executing the sequence tasks. */
};
VALIDATE_SIZE(CTaskComplexUseSequence, 0x1C);
