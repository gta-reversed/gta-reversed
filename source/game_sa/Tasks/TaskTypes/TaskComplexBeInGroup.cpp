#include "StdInc.h"

#include "TaskComplexBeInGroup.h"

void CTaskComplexBeInGroup::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexBeInGroup, 0x86E22C, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x632E50);
    RH_ScopedInstall(Destructor, 0x632EA0);
    RH_ScopedInstall(MonitorMainGroupTask, 0x633010);
    RH_ScopedInstall(MonitorSecondaryGroupTask, 0x6330B0);
    RH_ScopedVMTInstall(Clone, 0x636BE0);
    RH_ScopedVMTInstall(GetTaskType, 0x632E90);
    RH_ScopedVMTInstall(MakeAbortable, 0x632EB0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x632F40);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x632FB0);
    RH_ScopedVMTInstall(ControlSubTask, 0x638AA0);
}

// 0x632E50
CTaskComplexBeInGroup::CTaskComplexBeInGroup(int32 groupId, bool isLeader) : CTaskComplex() {
    m_nGroupId           = groupId;
    m_Ped                = nullptr;
    m_MainTask           = nullptr;
    m_SecondaryTask      = nullptr;
    m_bIsLeader          = isLeader;
    m_nMainTaskId        = TASK_NONE;
    m_nSecondaryTaskSlot = -1;
}

// 0x633010
CTask* CTaskComplexBeInGroup::MonitorMainGroupTask(CPed* ped) {
    if (const auto groupMainTask = CPedGroups::GetGroup(m_nGroupId).GetIntelligence().GetTaskMain(ped)) {
        if (groupMainTask != m_MainTask || groupMainTask->GetTaskType() != m_nMainTaskId) {
            if (m_pSubTask->MakeAbortable(ped)) {
                m_MainTask = groupMainTask;
                m_nMainTaskId = groupMainTask->GetTaskType();
                return groupMainTask->Clone();
            }
        }
    } else if (m_pSubTask->MakeAbortable(ped)) {
        m_MainTask = nullptr;
        m_nMainTaskId = TASK_NONE;
        return nullptr;
    }
    return m_pSubTask;
}

// 0x6330B0
void CTaskComplexBeInGroup::MonitorSecondaryGroupTask(CPed* ped) {
    // Check if ped has finished the group task (if any)
    auto& groupIntel = CPedGroups::GetGroup(m_nGroupId).GetIntelligence();
    const auto grpSecTask = groupIntel.GetTaskSecondary(ped);
    const auto pedGrpSecTaskSlot = groupIntel.GetTaskSecondarySlot(ped);
#ifdef FIX_BUGS
    const auto pedGrpSecTask = pedGrpSecTaskSlot != -1 ? ped->GetTaskManager().GetTaskSecondary(pedGrpSecTaskSlot) : nullptr;
#else
    const auto pedGrpSecTask = ped->GetTaskManager().GetTaskSecondary(pedGrpSecTaskSlot);
#endif // FIX_BUGS
    if (m_SecondaryTask == grpSecTask) {
        if (m_SecondaryTask) { // Check if theres any task at all (Not both nullptr)
            // Check if ped has finished the task
            if (!pedGrpSecTask) {
                groupIntel.ReportFinishedTask(ped, m_SecondaryTask);
                m_SecondaryTask = nullptr;
                m_nSecondaryTaskSlot = -1;
            }
        }
    } else { // Group has a new task, apply it to the ped

        // Abort peds task in the previous stored slot
        if (const auto task = ped->GetTaskManager().GetTaskSecondary((eSecondaryTask)m_nSecondaryTaskSlot)) {
            task->MakeAbortable(ped, ABORT_PRIORITY_LEISURE, nullptr);
        }

        // Make sure ped has the task the new task (and abort current)
        if (!pedGrpSecTask || pedGrpSecTask->MakeAbortable(ped)) {
            m_SecondaryTask = grpSecTask;
            m_nSecondaryTaskSlot = pedGrpSecTaskSlot;
            if (grpSecTask) { 
                ped->GetTaskManager().SetTaskSecondary(grpSecTask->Clone(), pedGrpSecTaskSlot);
            }
        }
    }
}

// 0x632EB0
bool CTaskComplexBeInGroup::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event)
{ 
    if (priority == ABORT_PRIORITY_IMMEDIATE) {
        auto& groupIntel = GetGroup().GetIntelligence();
        for (auto task : { &m_MainTask, &m_SecondaryTask }) {
            if (*task && CTask::IsTaskPtr(*task)) {
                groupIntel.ReportFinishedTask(ped, *task);
                *task = nullptr;
            }
        }
    }

    return m_pSubTask->MakeAbortable(ped, priority, event);
}

// 0x632F40
CTask* CTaskComplexBeInGroup::CreateNextSubTask(CPed* ped) {
    auto& intel = GetGroup().GetIntelligence();
    intel.ReportFinishedTask(ped, m_pSubTask);      // Report this task as finished, and proceed
    if (const auto main = intel.GetTaskMain(ped)) { // Return group's main task (if any)
        m_MainTask = main;
        m_nMainTaskId = main->GetTaskType();
        return main->Clone();
    } else {
        m_MainTask = nullptr;
        m_nMainTaskId = TASK_NONE;
        return nullptr;
    }
}

// 0x632FB0
CTask* CTaskComplexBeInGroup::CreateFirstSubTask(CPed* ped) {
    m_Ped = ped;
    m_Ped->RegisterReference(reinterpret_cast<CEntity**>(&m_Ped));

    // Below code basically the same as `CreateNextSubTask`
    // TODO: Maybe move this into a separate function? (To reduce copy paste)
    auto& intel = GetGroup().GetIntelligence();
    if (const auto main = intel.GetTaskMain(ped)) { // Return group's main task (if any)
        m_MainTask = main;
        m_nMainTaskId = main->GetTaskType();
        return main->Clone();
    } else {
        m_MainTask = nullptr;
        m_nMainTaskId = TASK_NONE;
        return nullptr;
    }
}

// 0x638AA0
CTask* CTaskComplexBeInGroup::ControlSubTask(CPed* ped) {
    MonitorSecondaryGroupTask(ped);
    return MonitorMainGroupTask(ped);
}
