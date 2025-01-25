#include "StdInc.h"

#include "PedGroupIntelligence.h"
#include "Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocator.h"
#include "Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocators.h"

void CPedGroupIntelligence::InjectHooks() {
    RH_ScopedClass(CPedGroupIntelligence);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Constructor, 0x5F7250, { .reversed = false });

    RH_ScopedGlobalInstall(FlushTasks, 0x5F79C0);
    RH_ScopedOverloadedInstall(ReportFinishedTask, "Wrapper", 0x5F86F0, bool(CPedGroupIntelligence::*)(const CPed*, const CTask*));
    RH_ScopedOverloadedInstall(ReportFinishedTask, "Impl", 0x5F86F0, bool(CPedGroupIntelligence::*)(const CPed*, const CTask*, PedTaskPairs& taskPairs));

    RH_ScopedInstall(SetTask, 0x5F7540);
    RH_ScopedInstall(Flush, 0x5F7350);
    RH_ScopedOverloadedInstall(AddEvent, "", 0x5F7470, bool(CPedGroupIntelligence::*)(CEvent*));
    RH_ScopedInstall(SetDefaultTaskAllocatorType, 0x5FBB70);
    RH_ScopedInstall(SetDefaultTaskAllocator, 0x5FB280);
    RH_ScopedInstall(ComputeDefaultTasks, 0x5F88D0);
    RH_ScopedInstall(ProcessIgnorePlayerGroup, 0x5F87A0);
    RH_ScopedInstall(ReportAllBarScriptTasksFinished, 0x5F8780, { .reversed = false });
    RH_ScopedInstall(GetTaskDefault, 0x5F86C0, { .reversed = false });
    RH_ScopedInstall(GetTaskScriptCommand, 0x5F8690, { .reversed = false });
    RH_ScopedInstall(GetTaskSecondarySlot, 0x5F8650, { .reversed = false });
    RH_ScopedInstall(GetTaskSecondary, 0x5F8620, { .reversed = false });
    RH_ScopedInstall(GetTaskMain, 0x5F85A0);
    RH_ScopedInstall(SetScriptCommandTask, 0x5F8560, { .reversed = false });
    RH_ScopedInstall(IsCurrentEventValid, 0x5F77A0);
    RH_ScopedInstall(IsGroupResponding, 0x5F7760);
    //RH_ScopedInstall(SetEventResponseTask, 0x5F8510); // Register allocation is weird, better not to hook it at all
    RH_ScopedInstall(SetEventResponseTaskAllocator, 0x5F7440, { .reversed = false });
    RH_ScopedInstall(SetPrimaryTaskAllocator, 0x5F7410, { .reversed = false });
    RH_ScopedInstall(SetGroupDecisionMakerType, 0x5F7340, { .reversed = false });
    RH_ScopedInstall(ComputeEventResponseTasks, 0x5FC440, { .reversed = false });
    RH_ScopedInstall(Process, 0x5FC4A0, { .reversed = false });
    RH_ScopedInstall(ReportAllTasksFinished, 0x5F7730);

}

// 0x5F7250
CPedGroupIntelligence::CPedGroupIntelligence(CPedGroup* owner) : // TODO: Use `CPedGroup&`
    m_pPedGroup{owner}
{
    plugin::CallMethod<0x5F7250, CPedGroupIntelligence*>(this);
}

// Seemingly always inlined as `Flush()`
CPedGroupIntelligence::~CPedGroupIntelligence() {
    Flush();
}

// 0x5F7350
// NOTE(Pirulax): For some reason this is called `~CPedGroupIntelligence` in *there*, but it's actually `Flush`!
void CPedGroupIntelligence::Flush() {
    rng::for_each(m_PedTaskPairs, &CPedTaskPair::Flush);
    rng::for_each(m_SecondaryPedTaskPairs, &CPedTaskPair::Flush);
    rng::for_each(m_ScriptCommandPedTaskPairs, &CPedTaskPair::Flush);
    rng::for_each(m_DefaultPedTaskPairs, &CPedTaskPair::Flush);

    if (m_pEventGroupEvent != m_pOldEventGroupEvent) {
        delete std::exchange(m_pEventGroupEvent, nullptr);
    }
    delete std::exchange(m_pOldEventGroupEvent, nullptr);

    delete std::exchange(m_PrimaryTaskAllocator, nullptr);
    delete std::exchange(m_EventResponseTaskAllocator, nullptr);

    m_DecisionMakerType = -1;
    m_TaskSeqId         = TASK_INVALID;
}

// 0x5F7470
bool CPedGroupIntelligence::AddEvent(CEvent* event) {
    if (event->GetEventType() != EVENT_GROUP_EVENT) {
        return false;
    }
    const auto eGrpEvent = static_cast<CEventGroupEvent*>(event);
    if (!eGrpEvent->AffectsPedGroup(m_pPedGroup) || !eGrpEvent->GetEvent().AffectsPedGroup(m_pPedGroup)) {
        return false;
    }
    if (const auto src = eGrpEvent->GetEvent().GetSourceEntity()) {
        if (src->IsPed() && m_pPedGroup->GetMembership().IsMember(src->AsPed())) {
            return false;
        }
    }
    if (eGrpEvent->GetEvent().HasEditableResponse()) {
        const auto eEditableResponse = static_cast<CEventEditableResponse*>(&eGrpEvent->GetEvent());
        eEditableResponse->ComputeResponseTaskType(m_pPedGroup);
        if (!eEditableResponse->WillRespond()) {
            return false;
        }
    }
    if (!m_pEventGroupEvent || m_pEventGroupEvent->BaseEventTakesPriorityOverBaseEvent(*eGrpEvent)) {
        delete std::exchange(m_pEventGroupEvent, static_cast<CEventGroupEvent*>(eGrpEvent->Clone()));
    }
    return true;
}

// 0x5F8560
void CPedGroupIntelligence::SetScriptCommandTask(CPed* ped, const CTask& task) {
    SetTask(ped, task, m_ScriptCommandPedTaskPairs);
}

// notsa
CPedTaskPair* CPedGroupIntelligence::GetPedsTaskPair(CPed* ped, PedTaskPairs& taskPairs) const {
    for (auto& tp : taskPairs) {
        if (tp.m_Ped == ped) {
            return &tp;
        }
    }
    return nullptr;   
}

// notsa (not sure)
CTask* CPedGroupIntelligence::GetTask(CPed* ped, PedTaskPairs& taskPairs) const {
    if (const auto tp = GetPedsTaskPair(ped, taskPairs)) {
        return tp->m_Task;
    }
    return nullptr;
}

// 0x5F85A0
CTask* CPedGroupIntelligence::GetTaskMain(CPed* ped) {
    CTask* t{};
    (t = GetTaskScriptCommand(ped)) || (t = GetTask(ped, m_PedTaskPairs)) || (t = GetTaskDefault(ped));
    return t;
}

CTask* CPedGroupIntelligence::GetTaskDefault(CPed* ped) {
    return GetTask(ped, m_DefaultPedTaskPairs);
}

// 0x5F8690
CTask* CPedGroupIntelligence::GetTaskScriptCommand(CPed* ped) {
    return GetTask(ped, m_ScriptCommandPedTaskPairs);
}

// 0x5F8620
CTask* CPedGroupIntelligence::GetTaskSecondary(CPed* ped) {
    return GetTask(ped, m_SecondaryPedTaskPairs);
}

// 0x5F8650
eSecondaryTask CPedGroupIntelligence::GetTaskSecondarySlot(CPed* ped) {
    return plugin::CallMethodAndReturn<eSecondaryTask, 0x5F8650>(this, ped);
}

void CPedGroupIntelligence::Process() {
    plugin::CallMethod<0x5FC4A0>(this);
}

// 0x5F7410
void CPedGroupIntelligence::SetPrimaryTaskAllocator(CTaskAllocator* taskAllocator) {
    plugin::CallMethod<0x5F7410, CPedGroupIntelligence*, CTaskAllocator*>(this, taskAllocator);
}

// 0x5F8510
void CPedGroupIntelligence::SetEventResponseTask(CPed* ped, bool hasMainTask, const CTask& mainTask, bool hasSecondaryTask, const CTask& secondaryTask, int32 slot) {
    if (hasMainTask) {
        SetTask(ped, mainTask, m_PedTaskPairs);
    }
    if (hasSecondaryTask) {
        SetTask(ped, secondaryTask, m_SecondaryPedTaskPairs, slot, false);
    }
}

// 0x5F76C0
bool CPedGroupIntelligence::ReportFinishedTask(const CPed* ped, const CTask* task, PedTaskPairs& taskPairs) {
    const auto tt = task->GetTaskType();
    for (auto& tp : taskPairs) {
        if (tp.m_Ped != ped) {
            continue;
        }
        if (!tp.m_Task || tp.m_Task->GetTaskType() != tt) {
            continue;
        }
        delete std::exchange(tp.m_Task, nullptr);
        return true;
    }
    return false;
}

// 0x5F86F0
bool CPedGroupIntelligence::ReportFinishedTask(const CPed* ped, const CTask* task) {
    if (ReportFinishedTask(ped, task, m_ScriptCommandPedTaskPairs)) {
        return true;
    }
    if (ReportFinishedTask(ped, task, m_PedTaskPairs)) {
        return true;
    }
    ReportFinishedTask(ped, task, m_SecondaryPedTaskPairs);
    ReportFinishedTask(ped, task, m_DefaultPedTaskPairs);
    return false; // Yes, it's `false` - Intended or not, no clue, but the return value of this function isn't used in any of the xrefs.
}

// 0x5F7540
void CPedGroupIntelligence::SetTask(CPed* ped, const CTask& task, PedTaskPairs& taskPairs, int32 slot, bool force) const {
    // Commented out as script tasks are `new`'d, and deleted after this finishes.
    //assert(!GetTaskPool()->IsObjectValid(&task)); // Shouldn't be `new`'d [Keep in mind that there might be false positives]

    const auto tp = GetPedsTaskPair(ped, taskPairs);
    if (!tp) {
        return;
    }
    if (tp->m_Task && (force || tp->m_Task->GetTaskType() != task.GetTaskType())) {
        delete std::exchange(tp->m_Task, task.Clone());
        tp->m_Slot = slot;
    } else if (!tp->m_Task) {
        tp->m_Task = task.Clone();
        tp->m_Slot = slot;
    }
}

// 0x5F79C0
void CPedGroupIntelligence::FlushTasks(PedTaskPairs& taskPairs, CPed* ped) {
    for (auto& tp : taskPairs) {
        if (ped && tp.m_Ped != ped) {
            continue;
        }
        delete std::exchange(tp.m_Task, nullptr);
    }
}

// 0x5FB280
void CPedGroupIntelligence::SetDefaultTaskAllocator(const CPedGroupDefaultTaskAllocator& ta) {
    m_DefaultTaskAllocator = &ta;
    for (auto& tp : m_DefaultPedTaskPairs) {
        delete std::exchange(tp.m_Ped, nullptr);
    }
    m_DefaultTaskAllocator->AllocateDefaultTasks(m_pPedGroup, nullptr);
}

// 0x5FBB70
void CPedGroupIntelligence::SetDefaultTaskAllocatorType(ePedGroupDefaultTaskAllocatorType taType) {
    SetDefaultTaskAllocator(CPedGroupDefaultTaskAllocators::Get(taType));
}

// 0x5F88D0
void CPedGroupIntelligence::ComputeDefaultTasks(CPed* ped) {
    FlushTasks(m_DefaultPedTaskPairs, ped);
    m_DefaultTaskAllocator->AllocateDefaultTasks(m_pPedGroup, ped);
}

// 0x5F87A0
void CPedGroupIntelligence::ProcessIgnorePlayerGroup() {
    if (!FindPlayerWanted()->m_bEverybodyBackOff) {
        return;
    }
    if (m_pOldEventGroupEvent) {
        switch (m_pOldEventGroupEvent->GetEvent().GetEventType()) {
        case EVENT_LEADER_ENTRY_EXIT:
        case EVENT_LEADER_ENTERED_CAR_AS_DRIVER:
            return;
        }
    }
    if (FindPlayerPed() != m_pPedGroup->GetMembership().GetLeader()) {
        if (!m_pOldEventGroupEvent) {
            return;
        }
        const auto src = m_pOldEventGroupEvent->GetEvent().GetSourceEntity();
        if (!src || !src->IsPed()) {
            return;
        }
        const auto grp = src->AsPed()->GetGroup();
        if (!grp || grp->GetMembership().GetLeader() != FindPlayerPed()) {
            return;
        }
    }
    delete std::exchange(m_pOldEventGroupEvent, nullptr);
    delete std::exchange(m_EventResponseTaskAllocator, nullptr);
    FlushTasks(m_PedTaskPairs, nullptr);
    FlushTasks(m_SecondaryPedTaskPairs, nullptr);
}

// 0x5F8780
void CPedGroupIntelligence::ReportAllBarScriptTasksFinished() {
    ReportAllTasksFinished(m_PedTaskPairs);
    ReportAllTasksFinished(m_SecondaryPedTaskPairs);
}

// 0x5F7730
void CPedGroupIntelligence::ReportAllTasksFinished(PedTaskPairs& taskPairs) {
    for (auto& tp : taskPairs) {
        delete tp.m_Task;
        tp.m_Task = nullptr;
    }
}

// 0x5F77A0
bool CPedGroupIntelligence::IsCurrentEventValid() {
    if (m_pOldEventGroupEvent) {
        const auto event = &m_pOldEventGroupEvent->GetEvent();
        if (event->GetEventType() == EVENT_PLAYER_COMMAND_TO_GROUP) {
            if (const auto src = event->GetSourceEntity()) {
                if (src->IsPed()) {
                    if (m_pPedGroup->GetMembership().IsMember(src->AsPed())) {
                        return false;
                    }
                }
            }
        }
    }
    return true;
}

// 0x5F7760
bool CPedGroupIntelligence::IsGroupResponding() {
    const auto CheckTaskPairs = [](PedTaskPairs& tps) {
        for (auto& tp : tps) {
            if (tp.m_Ped && tp.m_Task) {
                return true;
            }
        }
        return false;
    };
    return CheckTaskPairs(m_PedTaskPairs) || CheckTaskPairs(m_SecondaryPedTaskPairs);
}

// 0x5F7440
void CPedGroupIntelligence::SetEventResponseTaskAllocator(CTaskAllocator* ta) {
    delete std::exchange(m_EventResponseTaskAllocator, ta);
}

// 0x5F7340
void CPedGroupIntelligence::SetGroupDecisionMakerType(int32 t) {
    return plugin::CallMethod<0x5F7340, CPedGroupIntelligence*, int32>(this, t);
}

// 0x5FC440
CTaskAllocator* CPedGroupIntelligence::ComputeEventResponseTasks() {
    return plugin::CallMethodAndReturn<CTaskAllocator*, 0x5FC440, CPedGroupIntelligence*>(this);
}
