#include "StdInc.h"

#include "TaskComplexSequence.h"

void CTaskComplexSequence::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexSequence, 0x86e200, 11);
    RH_ScopedCategory("Tasks");

    RH_ScopedInstall(Constructor, 0x632BD0);

    RH_ScopedVMTInstall(Clone, 0x5F6710);
    RH_ScopedVMTInstall(MakeAbortable, 0x632C00);
    RH_ScopedVMTOverloadedInstall(CreateNextSubTask, "V", 0x638A40, CTask*(CTaskComplexSequence::*)(CPed*));
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x638A60);
    RH_ScopedVMTInstall(ControlSubTask, 0x632D00);

    RH_ScopedOverloadedInstall(CreateNextSubTask, "Internal", 0x632C70, CTask*(CTaskComplexSequence::*)(CPed*, int32&, int32&) const);
    RH_ScopedOverloadedInstall(AddTask, "0", 0x632D10, void(CTaskComplexSequence::*)(CTask*));
    RH_ScopedOverloadedInstall(AddTask, "1", 0x632D50, void(CTaskComplexSequence::*)(int32, CTask*));

    RH_ScopedInstall(Flush, 0x632C10);
    RH_ScopedInstall(Contains, 0x41BF10);
    RH_ScopedInstall(SetCanBeEmptied, 0x463610);
    RH_ScopedInstall(DeRegister, 0x636BC0);
}

// For 0x5F6710
CTaskComplexSequence::CTaskComplexSequence(const CTaskComplexSequence& o) :
    m_RepeatMode{o.m_RepeatMode},
    m_CurrTaskIdx{o.m_CurrTaskIdx}
{
    rng::transform(o.m_Tasks, m_Tasks, [](CTask* t){
        return t ? t->Clone() : nullptr;
    });
}

// 0x6389F0
CTaskComplexSequence::~CTaskComplexSequence() {
    Flush();
}

// 0x632C10
void CTaskComplexSequence::Flush() {
    for (auto& task : m_Tasks) {
        delete std::exchange(task, nullptr);
    }
    m_CurrTaskIdx = 0;
    m_RepeatMode  = eRepeatMode::DONT_REPEAT;
    m_RepeatedCnt = 0;
}

CTaskComplexSequence* CTaskComplexSequence::Constructor() {
    this->CTaskComplexSequence::CTaskComplexSequence();
    return this;
}

// 0x638A40
CTask* CTaskComplexSequence::CreateNextSubTask(CPed* ped) {
    return CreateNextSubTask(ped, m_CurrTaskIdx, m_RepeatedCnt);
}

// 0x638A60
CTask* CTaskComplexSequence::CreateFirstSubTask(CPed* ped) {
    const auto curr = m_Tasks[m_CurrTaskIdx];;
    return curr
        ? curr->Clone()
        : nullptr;
}

// 0x632D00
CTask* CTaskComplexSequence::ControlSubTask(CPed* ped) {
    return m_pSubTask;
}

// 0x632D10
void CTaskComplexSequence::AddTask(CTask* task) {
    assert(task);

    for (auto& t : m_Tasks) {
        if (!t) {
            t = task;
            return;
        }
    }

    delete task;
    DEV_LOG("Failed to add task to sequence");
}

// 0x632D50
void CTaskComplexSequence::AddTask(int32 sequenceIdx, CTask* task) {
    assert(task);
    if (sequenceIdx >= (int32)std::size(m_Tasks)) {
        delete task;
        DEV_LOG("Failed to add task to sequence");
    } else {
        delete m_Tasks[sequenceIdx];
        m_Tasks[sequenceIdx] = task;
    }
}

// 0x632C70
CTask* CTaskComplexSequence::CreateNextSubTask(CPed* ped, int32& taskIndex, int32& repeatCount) const {
    UNUSED(ped);

    taskIndex++;
    const auto bSeqEnd = taskIndex == std::size(m_Tasks) || !m_Tasks[taskIndex];

    switch (m_RepeatMode) {
    case eRepeatMode::FOREVER: {
        if (bSeqEnd) {
            taskIndex = 0;
            repeatCount++;
        }
        // NOTE: The `if` statement here (in the original code) is useless: `m_RepeatMode` will always be `FOREVER` (1) here
        //       Though, the original code was reachable if `m_RepeatMode != 0`, not only if `m_RepeatMode == 1` (like in our case)
        //       For more info see the `default` case.
        return m_Tasks[taskIndex]->Clone();
    }
    case eRepeatMode::DONT_REPEAT: {
        return bSeqEnd
            ? nullptr
            : m_Tasks[taskIndex]->Clone();
    }
    default: {
        // The code is really weird, I can't decide if used `m_RepeatMode` to also
        // store a variable amount of repeats or not.
        // If this is ever reached, that means that `m_RepeatMode` is not **just** a simple enum
        // but rather the number of times to repeat the sequence [for values > 1]
        NOTSA_UNREACHABLE();
    }
    }
}

// 0x41BF10
bool CTaskComplexSequence::Contains(eTaskType taskType) const {
    for (const auto& task : m_Tasks) {
        if (task->GetTaskType() == taskType) {
            return true;
        }
    }
    return false;
}

// 0x463610
void CTaskComplexSequence::SetCanBeEmptied(bool flush) {
    if (m_RefCnt || !flush) {
        m_bFlushTasks = flush;
    } else {
        m_bFlushTasks = false;
        Flush();
    }
}

// 0x636BC0
void CTaskComplexSequence::DeRegister() {
    if (!--m_RefCnt && m_bFlushTasks) {
        m_bFlushTasks = false;
        Flush();
    }
}

void CTaskComplexSequence::SetRepeatMode(bool repeat) {
    m_RepeatMode = repeat
        ? eRepeatMode::FOREVER
        : eRepeatMode::DONT_REPEAT;
}
