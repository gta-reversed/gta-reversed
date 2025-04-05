#include "StdInc.h"

#include "PedGroupIntelligence.h"
#include "Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocator.h"
#include "Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocators.h"
#include <reversiblebugfixes/Bugs.hpp>

void CPedGroupIntelligence::InjectHooks() {
    RH_ScopedClass(CPedGroupIntelligence);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Constructor, 0x5F7250);

    RH_ScopedGlobalInstall(FlushTasks, 0x5F79C0);
    RH_ScopedOverloadedInstall(ReportFinishedTask, "Wrapper", 0x5F86F0, bool(CPedGroupIntelligence::*)(const CPed*, const CTask*));
    RH_ScopedOverloadedInstall(ReportFinishedTask, "Impl", 0x5F76C0, bool(CPedGroupIntelligence::*)(const CPed*, const CTask*, PedTaskPairs& taskPairs));
    RH_ScopedInstall(SetTask, 0x5F7540);
    RH_ScopedInstall(Flush, 0x5F7350);
    RH_ScopedOverloadedInstall(AddEvent, "", 0x5F7470, bool(CPedGroupIntelligence::*)(CEvent*));
    RH_ScopedInstall(SetDefaultTaskAllocatorType, 0x5FBB70);
    RH_ScopedInstall(SetDefaultTaskAllocator, 0x5FB280);
    RH_ScopedInstall(ComputeDefaultTasks, 0x5F88D0);
    RH_ScopedInstall(ProcessIgnorePlayerGroup, 0x5F87A0);
    RH_ScopedInstall(ReportAllBarScriptTasksFinished, 0x5F8780);
    RH_ScopedInstall(GetTaskDefault, 0x5F86C0);
    RH_ScopedInstall(GetTaskScriptCommand, 0x5F8690);
    RH_ScopedInstall(GetTaskSecondarySlot, 0x5F8650);
    RH_ScopedInstall(GetTaskSecondary, 0x5F8620);
    RH_ScopedInstall(GetTaskMain, 0x5F85A0);
    RH_ScopedInstall(SetScriptCommandTask, 0x5F8560);
    RH_ScopedInstall(IsCurrentEventValid, 0x5F77A0);
    RH_ScopedInstall(IsGroupResponding, 0x5F7760);
    //RH_ScopedInstall(SetEventResponseTask, 0x5F8510); // Register allocation is weird, better not to hook it at all
    RH_ScopedInstall(SetEventResponseTaskAllocator, 0x5F7440);
    RH_ScopedInstall(SetPrimaryTaskAllocator, 0x5F7410);
    RH_ScopedInstall(SetGroupDecisionMakerType, 0x5F7340);
    RH_ScopedInstall(ComputeEventResponseTasks, 0x5FC440);
    RH_ScopedInstall(Process, 0x5FC4A0);
    RH_ScopedInstall(ReportAllTasksFinished, 0x5F7730);
}

// 0x5F7250
CPedGroupIntelligence::CPedGroupIntelligence(CPedGroup* owner) : // TODO: Use `CPedGroup&`
    m_pPedGroup{owner},
    m_DefaultTaskAllocator{&CPedGroupDefaultTaskAllocators::Get(ePedGroupDefaultTaskAllocatorType::STAND_STILL)}
{
    /* Everything else is initialized in the header */
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

    if (m_HighestPriorityEvent != m_CurrentEvent) {
        delete std::exchange(m_HighestPriorityEvent, nullptr);
    }
    delete std::exchange(m_CurrentEvent, nullptr);

    delete std::exchange(m_PrimaryTaskAllocator, nullptr);
    delete std::exchange(m_EventResponseTaskAllocator, nullptr);

    m_DecisionMakerType = eDecisionMakerType::UNKNOWN;
    m_TaskSeqId         = TASK_INVALID;
}

// 0x5F7470
bool CPedGroupIntelligence::AddEvent(CEvent* event) {
    const auto eGrpEvent = notsa::dyn_cast<CEventGroupEvent>(event);
    if (!eGrpEvent) {
        return false;
    }
    if (!eGrpEvent->AffectsPedGroup(m_pPedGroup) || !eGrpEvent->GetEvent().AffectsPedGroup(m_pPedGroup)) {
        return false;
    }
    if (const auto src = eGrpEvent->GetEvent().GetSourceEntity()) {
        if (src->IsPed() && m_pPedGroup->GetMembership().IsMember(src->AsPed())) {
            return false;
        }
    }
    if (eGrpEvent->GetEvent().HasEditableResponse()) {
        const auto eEditableResponse = notsa::cast<CEventEditableResponse>(&eGrpEvent->GetEvent());
        eEditableResponse->ComputeResponseTaskType(m_pPedGroup);
        if (!eEditableResponse->WillRespond()) {
            return false;
        }
    }
    if (!m_HighestPriorityEvent || eGrpEvent->BaseEventTakesPriorityOverBaseEvent(*m_HighestPriorityEvent)) {
        delete std::exchange(m_HighestPriorityEvent, notsa::cast<CEventGroupEvent>(eGrpEvent->Clone()));
    }
    return true;
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

// 0x5F85A0
CTask* CPedGroupIntelligence::GetTaskMain(CPed* ped) {
    CTask* t{};
    (t = GetTaskScriptCommand(ped)) || (t = GetTask(ped, m_PedTaskPairs)) || (t = GetTaskDefault(ped));
    return t;
}

// 0x5F7660
CTask* CPedGroupIntelligence::GetTask(CPed* ped, PedTaskPairs& taskPairs) const {
    if (const auto tp = GetPedsTaskPair(ped, taskPairs)) {
        return tp->m_Task;
    }
    return nullptr;
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
    if (const auto tp = GetPedsTaskPair(ped, m_SecondaryPedTaskPairs)) {
        return tp->m_Slot;
    }
    NOTSA_UNREACHABLE(); // Otherwise returned `0`, which is a valid slot...
}

// 0x5FC4A0
void CPedGroupIntelligence::Process() {
    ProcessIgnorePlayerGroup();

    if (m_CurrentEvent && !m_PrimaryTaskAllocator) {
        if (!IsGroupResponding() || !IsCurrentEventValid()) {
            delete std::exchange(m_CurrentEvent, nullptr);
            delete std::exchange(m_EventResponseTaskAllocator, nullptr);
        }
    }

    bool hasMemberListChanged{};
    for (auto&& [i, mem] : rngv::enumerate(m_pPedGroup->GetMembership().GetMembers())) {
        if (m_PedTaskPairs[i].m_Ped == mem) {
            continue;
        }
        m_PedTaskPairs[i].m_Ped          = mem;
        m_SecondaryPedTaskPairs[i].m_Ped = mem;
        m_DefaultPedTaskPairs[i].m_Ped   = mem;
        hasMemberListChanged             = true;
    }
    if (hasMemberListChanged) {
        if (m_PrimaryTaskAllocator) { // 0x5FC616
            m_PrimaryTaskAllocator->AllocateTasks(this);
        } else { // 0x5FC620
            ComputeDefaultTasks(nullptr);
            ComputeScriptCommandTasks();
            if (m_CurrentEvent) { // 0x5FC659
                if (m_EventResponseTaskAllocator) { // 0x5FC65E
                    FlushTasks(m_PedTaskPairs, nullptr);
                    m_EventResponseTaskAllocator->AllocateTasks(this);
                } else {
                    m_EventResponseTaskAllocator = ComputeEventResponseTasks();
                }
            }
        }
    }

    CEvent* eventToAdd{};
    if (m_HighestPriorityEvent) { // 0x5FC6A2
        if (ShouldSetHighestPriorityEventAsCurrent()) {
            delete std::exchange(m_CurrentEvent, std::exchange(m_HighestPriorityEvent, nullptr));
            if (notsa::bugfixes::GenericUB) {
                if (m_PrimaryTaskAllocator == m_EventResponseTaskAllocator) {
                    m_PrimaryTaskAllocator = nullptr;
                }
            }
            delete std::exchange(m_EventResponseTaskAllocator, ComputeEventResponseTasks());
        } else if ( // 0x5FC6F6
               m_HighestPriorityEvent->GetEvent().GetEventType() == m_CurrentEvent->GetEvent().GetEventType()
            && m_HighestPriorityEvent->GetEvent().CanBeInterruptedBySameEvent()
        ) {
            eventToAdd = m_HighestPriorityEvent->Clone();
        }
    }

    if (m_PrimaryTaskAllocator && !m_PrimaryTaskAllocator->ProcessGroup(this)) { // 0x5FC770
        delete std::exchange(m_PrimaryTaskAllocator, nullptr);
    }

    if (m_HighestPriorityEvent && m_CurrentEvent != m_HighestPriorityEvent) { // 0x5FC792
        delete std::exchange(m_HighestPriorityEvent, nullptr);
    }

    if (!m_PrimaryTaskAllocator && m_CurrentEvent) { // 0x5FC7A7
        if (m_EventResponseTaskAllocator) {
            m_PrimaryTaskAllocator = m_EventResponseTaskAllocator->ProcessGroup(this);
        }
    }

    if (eventToAdd) { // 0x5FC7C6
        AddEvent(eventToAdd);
    }
    delete eventToAdd;
}

// 0x5F7410
void CPedGroupIntelligence::SetPrimaryTaskAllocator(CTaskAllocator* ta) {
    if (notsa::bugfixes::GenericUB) {
        if (m_EventResponseTaskAllocator == m_PrimaryTaskAllocator) {
            m_EventResponseTaskAllocator = nullptr;
        }
    }
    delete std::exchange(m_PrimaryTaskAllocator, ta);
}

// 0x5F8510
void CPedGroupIntelligence::SetEventResponseTask(CPed* ped, bool hasMainTask, const CTask& mainTask, bool hasSecondaryTask, const CTask& secondaryTask, eSecondaryTask slot) {
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
void CPedGroupIntelligence::SetTask(CPed* ped, const CTask& task, PedTaskPairs& taskPairs, eSecondaryTask slot, bool force) const {
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

// 0x5F8560
void CPedGroupIntelligence::SetScriptCommandTask(CPed* ped, const CTask& task) {
    SetTask(ped, task, m_ScriptCommandPedTaskPairs);
}

// @unk
void CPedGroupIntelligence::SetDefaultTask(CPed* ped, const CTask& task) {
    SetTask(ped, task, m_DefaultPedTaskPairs);
}

// separated this out for readability @ 0x5FC6A2
bool CPedGroupIntelligence::ShouldSetHighestPriorityEventAsCurrent() {
    assert(m_HighestPriorityEvent);

    // No current event?
    if (!m_CurrentEvent) {
        return true;
    }

    // Same type, but current can't be interrupted by it?
    if (m_HighestPriorityEvent->GetEvent().GetEventType() == m_CurrentEvent->GetEvent().GetEventType()) {
        if (!m_CurrentEvent->GetEvent().CanBeInterruptedBySameEvent()) {
            return false;
        }
    }

    // Event with higher priority takes precedence
    return m_HighestPriorityEvent->BaseEventTakesPriorityOverBaseEvent(*m_CurrentEvent);
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
        delete std::exchange(tp.m_Task, nullptr);
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
    if (m_CurrentEvent) {
        switch (m_CurrentEvent->GetEvent().GetEventType()) {
        case EVENT_LEADER_ENTRY_EXIT:
        case EVENT_LEADER_ENTERED_CAR_AS_DRIVER:
            return;
        }
    }
    if (FindPlayerPed() != m_pPedGroup->GetMembership().GetLeader()) {
        if (!m_CurrentEvent) {
            return;
        }
        const auto src = m_CurrentEvent->GetEvent().GetSourceEntity();
        if (!src || !src->IsPed()) {
            return;
        }
        const auto grp = src->AsPed()->GetGroup();
        if (!grp || grp->GetMembership().GetLeader() != FindPlayerPed()) {
            return;
        }
    }
    delete std::exchange(m_CurrentEvent, nullptr);
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
    if (m_CurrentEvent) {
        const auto event = &m_CurrentEvent->GetEvent();
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
    return CheckTaskPairs(m_PedTaskPairs)
        || CheckTaskPairs(m_SecondaryPedTaskPairs);
}

// 0x5F7440
void CPedGroupIntelligence::SetEventResponseTaskAllocator(CTaskAllocator* ta) {
    delete std::exchange(m_EventResponseTaskAllocator, ta);
}

// 0x5FC440
CTaskAllocator* CPedGroupIntelligence::ComputeEventResponseTasks() {
    FlushTasks(m_PedTaskPairs, nullptr);
    FlushTasks(m_SecondaryPedTaskPairs, nullptr);
    return CGroupEventHandler::ComputeEventResponseTasks(*m_CurrentEvent, m_pPedGroup);
}

void CPedGroupIntelligence::ComputeScriptCommandTasks() {
    plugin::CallMethod<0x5F7800>(this);
}
