#include "StdInc.h"
#include "TaskSimpleIKManager.h"

void CTaskSimpleIKManager::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleIKManager, 0x86E358, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x6337F0);
    RH_ScopedInstall(Destructor, 0x633830);

    RH_ScopedInstall(AddIKChainTask, 0x633940);
    RH_ScopedInstall(GetTaskAtSlot, 0x6339B0);
    RH_ScopedVMTInstall(Clone, 0x639350);
    RH_ScopedVMTInstall(GetTaskType, 0x633820);
    RH_ScopedVMTInstall(MakeAbortable, 0x6338A0);
    RH_ScopedVMTInstall(ProcessPed, 0x6338E0);
}

// For 0x639350
CTaskSimpleIKManager::CTaskSimpleIKManager(const CTaskSimpleIKManager& o) :
    CTaskSimpleIKManager{}
{
    for (auto&& [slot, t] : notsa::enumerate(o.m_IKChainTasks)) {
        m_IKChainTasks[slot] = CTask::Cast<CTaskSimpleIKChain>(t->Clone());
    }
}

// 0x633830
CTaskSimpleIKManager::~CTaskSimpleIKManager() {
    for (const auto task : m_IKChainTasks) {
        delete task;
    }
}

// 0x633940
void CTaskSimpleIKManager::AddIKChainTask(CTaskSimpleIKChain* task, eIKChainSlot slot) {
    if (slot == eIKChainSlot::UNKNOWN) { // Find free slot, and store `task` there.
        const auto it = rng::find(m_IKChainTasks, nullptr);
        if (it != m_IKChainTasks.end()) {
            *it = task;
        }
    } else { // Store `task` in the given `slot`
        assert(!m_IKChainTasks[(size_t)slot]); // Otherwise memory leak
        m_IKChainTasks[(size_t)slot] = task;
    }
}

// 0x6338A0
bool CTaskSimpleIKManager::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
    if (priority == eAbortPriority::ABORT_PRIORITY_IMMEDIATE) {
        for (auto& task : m_IKChainTasks) {
            delete std::exchange(task, nullptr);
        }
        return true;
    } else {
        m_IsAborting = true;
        return false;
    }
}

// 0x6338E0
bool CTaskSimpleIKManager::ProcessPed(CPed* ped) {
    if (m_IsAborting) {
        return true;
    }

    bool hasAllProcessedPed{true};
    for (auto& task : m_IKChainTasks) {
        if (task) {
            if (task->ProcessPed(ped)) {
                delete std::exchange(task, nullptr);
            } else {
                hasAllProcessedPed = false;
            }
        }
    }
    return hasAllProcessedPed;
}

