#include "StdInc.h"

#include "PedGroupIntelligence.h"

void CPedGroupIntelligence::InjectHooks() {
    RH_ScopedClass(CPedGroupIntelligence);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Constructor, 0x5F7250, { .reversed = false });

    RH_ScopedGlobalInstall(FlushTasks, 0x5F79C0, { .reversed = false });
    //RH_ScopedGlobalInstall(ReportFinishedTask_, 0x5F76C0, { .reversed = false });

    RH_ScopedInstall(SetTask, 0x5F7540);
    RH_ScopedInstall(Flush, 0x5F7350);
    RH_ScopedInstall(SetDefaultTaskAllocatorType, 0x5FBB70, { .reversed = false });
    RH_ScopedInstall(SetDefaultTaskAllocator, 0x5FB280, { .reversed = false });
    RH_ScopedInstall(ComputeDefaultTasks, 0x5F88D0, { .reversed = false });
    RH_ScopedInstall(ProcessIgnorePlayerGroup, 0x5F87A0, { .reversed = false });
    RH_ScopedInstall(ReportAllBarScriptTasksFinished, 0x5F8780, { .reversed = false });
    //RH_ScopedInstall(ReportFinishedTask, 0x5F86F0, { .reversed = false });
    RH_ScopedInstall(GetTaskDefault, 0x5F86C0, { .reversed = false });
    RH_ScopedInstall(GetTaskScriptCommand, 0x5F8690, { .reversed = false });
    RH_ScopedInstall(GetTaskSecondarySlot, 0x5F8650, { .reversed = false });
    RH_ScopedInstall(GetTaskSecondary, 0x5F8620, { .reversed = false });
    RH_ScopedInstall(GetTaskMain, 0x5F85A0);
    RH_ScopedInstall(SetScriptCommandTask, 0x5F8560, { .reversed = false });
    RH_ScopedInstall(IsCurrentEventValid, 0x5F77A0, { .reversed = false });
    RH_ScopedInstall(IsGroupResponding, 0x5F7760, { .reversed = false });
    //RH_ScopedInstall(AddEvent, 0x5F7470, { .reversed = false });
    //RH_ScopedInstall(SetEventResponseTask, 0x5F8510); // Register allocation is weird, better not to hook it at all
    RH_ScopedInstall(SetEventResponseTaskAllocator, 0x5F7440, { .reversed = false });
    RH_ScopedInstall(SetPrimaryTaskAllocator, 0x5F7410, { .reversed = false });
    RH_ScopedInstall(SetGroupDecisionMakerType, 0x5F7340, { .reversed = false });
    RH_ScopedInstall(ComputeEventResponseTasks, 0x5FC440, { .reversed = false });
    RH_ScopedInstall(Process, 0x5FC4A0, { .reversed = false });
    RH_ScopedInstall(ReportAllTasksFinished, 0x5F7730);

    RH_ScopedOverloadedInstall(ReportFinishedTask, "", 0x5F86F0, bool(CPedGroupIntelligence::*)(const CPed*, const CTask*), { .reversed = false });
}

// 0x5F7250
CPedGroupIntelligence::CPedGroupIntelligence(CPedGroup* owner) : // TODO: Use `CPedGroup&`
    m_pPedGroup{owner}
{
    plugin::CallMethod<0x5F7250, CPedGroupIntelligence*>(this);
}

// Unknown address (If any)
CPedGroupIntelligence::~CPedGroupIntelligence() {
    Flush(); // Not sure if it does this at all, but it worked so far, so let's leave it like this for now
}

// 0x5F7350
void CPedGroupIntelligence::Flush() { // Pirulax: For some reason this is called `~CPedGroupIntelligence` in *there*...
    plugin::CallMethod<0x5F7350, CPedGroupIntelligence*>(this);
}

// 0x5F7470
bool CPedGroupIntelligence::AddEvent(CEvent* event) {
    return plugin::CallMethodAndReturn<bool, 0x5F7470, CPedGroupIntelligence*, CEvent*>(this, event);
}

// 0x5F8560
void CPedGroupIntelligence::SetScriptCommandTask(CPed* ped, const CTask* task) {
    plugin::CallMethod<0x5F8560, CPedGroupIntelligence*, CPed*, const CTask*>(this, ped, task);
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
CTask* CPedGroupIntelligence::GetTask(CPed* ped, PedTaskPairs& taskPairs) {
    if (const auto tp = GetPedsTaskPair(ped, taskPairs)) {
        return tp->m_Task;
    }
    return nullptr;
}

// 0x5F85A0
CTask* CPedGroupIntelligence::GetTaskMain(CPed* ped) {
    if (const auto t = GetTaskScriptCommand(ped)) {
        return t;
    }
    if (const auto t = GetTask(ped, m_PedTaskPairs)) {
        return t;
    }
    if (const auto t = GetTaskDefault(ped)) {
        return t;
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
    return plugin::CallMethodAndReturn<eSecondaryTask, 0x5F8650>(this, ped);
}

void CPedGroupIntelligence::Process() {
    plugin::CallMethod<0x5FC4A0>(this);
}

// 0x5F7410
void CPedGroupIntelligence::SetPrimaryTaskAllocator(CTaskAllocator* taskAllocator) {
    plugin::CallMethod<0x5F7410, CPedGroupIntelligence*, CTaskAllocator*>(this, taskAllocator);
}

// 0x5FBB70
void CPedGroupIntelligence::SetDefaultTaskAllocatorType(ePedGroupDefaultTaskAllocatorType nPedGroupTaskAllocator) {
    plugin::CallMethod<0x5FBB70, CPedGroupIntelligence*, ePedGroupDefaultTaskAllocatorType>(this, nPedGroupTaskAllocator);
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

// 0x5F86F0
bool CPedGroupIntelligence::ReportFinishedTask(const CPed* ped, const CTask* task) {
    return plugin::CallMethodAndReturn<bool, 0x5F86F0>(this, ped, task);
}

// 0x5F7540
void CPedGroupIntelligence::SetTask(CPed* ped, const CTask& task, PedTaskPairs& taskPairs, int32 slot, bool force) {
    assert(!GetTaskPool()->IsObjectValid(&task)); // Shouldn't be `new`'d [Keep in mind that there might be false positives]

    const auto tp = GetPedsTaskPair(ped, taskPairs);
    if (!tp) {
        return;
    }
    if (tp->m_Task && (force || tp->m_Task->GetTaskType() != const_cast<CTask&>(task).GetTaskType())) {
        delete std::exchange(tp->m_Task, const_cast<CTask&>(task).Clone());
        tp->m_Slot = slot;
    } else if (!tp->m_Task) {
        tp->m_Task = const_cast<CTask&>(task).Clone();
        tp->m_Slot = slot;
    }
}

// 0x5F79C0
void CPedGroupIntelligence::FlushTasks(PedTaskPairs& taskPairs, CPed* ped) {
    return plugin::Call<0x5F79C0, PedTaskPairs&, CPed*>(taskPairs, ped);
}

// 0x5FB280
void CPedGroupIntelligence::SetDefaultTaskAllocator(CPedGroupDefaultTaskAllocator const* a2) {
    return plugin::CallMethod<0x5FB280, CPedGroupIntelligence*, CPedGroupDefaultTaskAllocator const*>(this, a2);
}

// 0x5F88D0
void CPedGroupIntelligence::ComputeDefaultTasks(CPed* ped) {
    return plugin::CallMethod<0x5F88D0, CPedGroupIntelligence*, CPed *>(this, ped);
}

// 0x5F87A0
void CPedGroupIntelligence::ProcessIgnorePlayerGroup() {
    return plugin::CallMethod<0x5F87A0, CPedGroupIntelligence*>(this);
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
    return plugin::CallMethodAndReturn<bool, 0x5F77A0, CPedGroupIntelligence*>(this);
}

// 0x5F7760
bool CPedGroupIntelligence::IsGroupResponding() {
    return plugin::CallMethodAndReturn<bool, 0x5F7760, CPedGroupIntelligence*>(this);
}

// 0x5F7440
void CPedGroupIntelligence::SetEventResponseTaskAllocator(CTaskAllocator* a) {
    return plugin::CallMethod<0x5F7440, CPedGroupIntelligence*, CTaskAllocator*>(this, a);
}

// 0x5F7340
void CPedGroupIntelligence::SetGroupDecisionMakerType(int32 t) {
    return plugin::CallMethod<0x5F7340, CPedGroupIntelligence*, int32>(this, t);
}

// 0x5FC440
CTaskAllocator* CPedGroupIntelligence::ComputeEventResponseTasks() {
    return plugin::CallMethodAndReturn<CTaskAllocator*, 0x5FC440, CPedGroupIntelligence*>(this);
}
