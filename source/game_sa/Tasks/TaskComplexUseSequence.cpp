#include "StdInc.h"

#include "TaskComplexUseSequence.h"

void CTaskComplexUseSequence::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexUseSequence, 0x86e518, 11);
    RH_ScopedCategory("Tasks");

    RH_ScopedInstall(Constructor, 0x635450);
    RH_ScopedVMTInstall(Clone, 0x637100);
    RH_ScopedVMTInstall(MakeAbortable, 0x639730);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x6354A0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x6354D0);
    RH_ScopedVMTInstall(ControlSubTask, 0x635530);
}

// 0x635450
CTaskComplexUseSequence::CTaskComplexUseSequence(int32 sequenceIndex) :
    m_nSequenceIndex{sequenceIndex}
{
    CTaskSequences::ms_taskSequence[m_nSequenceIndex].m_RefCnt++;
}

CTaskComplexUseSequence::CTaskComplexUseSequence(const CTaskComplexUseSequence& o) :
    CTaskComplexUseSequence{o.m_nCurrentTaskIndex}
{
    m_nCurrentTaskIndex = o.m_nCurrentTaskIndex;
    m_nEndTaskIndex     = o.m_nEndTaskIndex;
}

CTaskComplexUseSequence::~CTaskComplexUseSequence() {
    if (m_nSequenceIndex != -1) {
        auto complexSequence = &CTaskSequences::ms_taskSequence[m_nSequenceIndex];
        bool bFinalReference = complexSequence->m_RefCnt == 1;
        complexSequence->m_RefCnt--;
        if (bFinalReference && complexSequence->m_bFlushTasks) {
            complexSequence->m_bFlushTasks = false;
            complexSequence->Flush();
        }
    }
}

CTaskComplexUseSequence* CTaskComplexUseSequence::Constructor(int32 sequenceIndex) {
    this->CTaskComplexUseSequence::CTaskComplexUseSequence(sequenceIndex);
    return this;
}

bool CTaskComplexUseSequence::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    bool bMakeAbortable = m_pSubTask->MakeAbortable(ped, priority, event);
    if (bMakeAbortable && event && event->GetEventType() == EVENT_DAMAGE) {
        auto* eventDamage = (CEventDamage*)event;
        if (eventDamage->m_damageResponse.m_bHealthZero && eventDamage->m_bAddToEventGroup) {
            CTaskComplexSequence* sequence = &CTaskSequences::ms_taskSequence[m_nSequenceIndex];
            bool bFinalReference = sequence->m_RefCnt == 1;
            sequence->m_RefCnt--;
            if (bFinalReference && sequence->m_bFlushTasks) {
                sequence->m_bFlushTasks = false;
                sequence->Flush();
            }
            m_nSequenceIndex = -1;
        }
    }
    return bMakeAbortable;
}

CTask* CTaskComplexUseSequence::CreateNextSubTask(CPed* ped) {
    if (m_nSequenceIndex == -1) {
        return nullptr;
    } else {
        auto taskComplexSequence = &CTaskSequences::ms_taskSequence[m_nSequenceIndex];
        return taskComplexSequence->CreateNextSubTask(ped, m_nCurrentTaskIndex, m_nSequenceRepeatedCount);
    }
}


CTask* CTaskComplexUseSequence::CreateFirstSubTask(CPed* ped) {
    if (m_nSequenceIndex == -1) {
        return nullptr;
    }

    CTask* firstSubTask = nullptr;
    CTask* currentSequenceTask = CTaskSequences::ms_taskSequence[m_nSequenceIndex].m_Tasks[m_nCurrentTaskIndex];
    if (currentSequenceTask) {
        firstSubTask = currentSequenceTask->Clone();
    }

    if (m_nEndTaskIndex != -1) {
        if (firstSubTask && firstSubTask->GetTaskType() == TASK_COMPLEX_USE_SEQUENCE) {
            auto sequence = reinterpret_cast<CTaskComplexUseSequence*>(firstSubTask);
            sequence->m_nCurrentTaskIndex = m_nEndTaskIndex;
            sequence->m_nEndTaskIndex = -1;
        }
    }
    return firstSubTask;
}

// 0x0
CTask* CTaskComplexUseSequence::ControlSubTask(CPed* ped) {
    return m_pSubTask;
}
