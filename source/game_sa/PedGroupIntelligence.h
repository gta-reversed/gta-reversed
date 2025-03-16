/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include <concepts>

#include "PedTaskPair.h"

#include "Tasks/TaskTypes/TaskSimpleNone.h"

class CPed;
class CTask;
class CGroupEventHandler;
class CPedGroupDefaultTaskAllocator;
class CTaskAllocator;
class CEvent;
class CPedGroup;
class CEventGroupEvent;

enum class ePedGroupDefaultTaskAllocatorType : uint32 {
    FOLLOW_ANY_MEANS,
    FOLLOW_LIMITED,
    STAND_STILL,
    CHAT,
    SIT_IN_LEADER_CAR,
    RANDOM,
};

class CPedGroupIntelligence {
    using PedTaskPairs = std::array<CPedTaskPair, TOTAL_PED_GROUP_MEMBERS>;
public:
    static void InjectHooks();

    CPedGroupIntelligence(CPedGroup* owner);
    ~CPedGroupIntelligence();

    void Flush();

    bool            AddEvent(CEvent* event);
    void            ComputeDefaultTasks(CPed* ped);
    CTaskAllocator* ComputeEventResponseTasks();
    void            ComputeScriptCommandTasks();
    static void     FlushTasks(PedTaskPairs& taskPairs, CPed* ped);

    //! @notsa
    CPedTaskPair*  GetPedsTaskPair(CPed* ped, PedTaskPairs& taskPairs) const;

    CTask*         GetTask(CPed* ped, PedTaskPairs& taskPairs);
    CTask*         GetTaskMain(CPed* ped);
    CTask*         GetTaskDefault(CPed* ped);
    CTask*         GetTaskScriptCommand(CPed* ped);
    CTask*         GetTaskSecondary(CPed* ped);
    eSecondaryTask GetTaskSecondarySlot(CPed* ped);

    bool IsCurrentEventValid();
    bool IsGroupResponding();
    void Process();
    void ProcessIgnorePlayerGroup();
    void ReportAllBarScriptTasksFinished();
    void ReportAllTasksFinished(PedTaskPairs& taskPairs);
    //void ReportAllTasksFinished();
    bool ReportFinishedTask(const CPed* ped, const CTask* task, CPedTaskPair* taskpair);
    bool ReportFinishedTask(const CPed* ped, const CTask* task);
    void SetDefaultTask(CPed* ped, const CTask* task);
    void SetDefaultTaskAllocator(CPedGroupDefaultTaskAllocator const* PedGroupDefaultTaskAllocator);
    void SetDefaultTaskAllocatorType(ePedGroupDefaultTaskAllocatorType nPedGroupTaskAllocator);

    /*!
    * @addr 0x5F8510
    * @note The tasks passed to this function **shouldn't** be `new`-d, but rather stack allocated!
    * @note Also, it's often inlined into an unused `CTaskSimpleNone` and `SetTask` call.
    */
    void SetEventResponseTask(
        CPed*        ped,
        bool         hasMainTask,

        const CTask& mainTask,
        bool         hasSecondaryTask = false,

        const CTask& secondaryTask = CTaskSimpleNone{},
        int32        slot = -1
    );
    //! @notsa
    void SetEventResponseTask(CPed* ped, const CTask& task) { SetEventResponseTask(ped, true, task); }
    void SetEventResponseTaskAllocator(CTaskAllocator* a);
    void SetGroupDecisionMakerType(int32 t);
    void SetPrimaryTaskAllocator(CTaskAllocator* taskAllocator);
    void SetScriptCommandTask(CPed* ped, const CTask* task);

    auto GetOldEvent()     { return m_pOldEventGroupEvent; }
    auto GetCurrentEvent() { return m_pEventGroupEvent; }

    template<std::derived_from<CEvent> T>
    auto AddEvent(T event) { // TODO: Remove in final
        return AddEvent(&event);
    }

    //! `task` shouldn't be `new`-d, but rather stack allocated!
    void SetTask(CPed* ped, const CTask& task, PedTaskPairs& taskPairs, int32 slot = -1, bool force = false);

private: // Wrappers for hooks
    // 0x5F7250
    CPedGroupIntelligence* Constructor() {
        this->CPedGroupIntelligence::CPedGroupIntelligence(nullptr);
        return this;
    }

private:
    CPedGroup*                     m_pPedGroup{};
    CEventGroupEvent*              m_pOldEventGroupEvent{};
    CEventGroupEvent*              m_pEventGroupEvent{};
    PedTaskPairs                   m_PedTaskPairs{};
    PedTaskPairs                   m_SecondaryPedTaskPairs{};
    PedTaskPairs                   m_ScriptCommandPedTaskPairs{};
    PedTaskPairs                   m_DefaultPedTaskPairs{};
    CPedGroupDefaultTaskAllocator* m_DefaultTaskAllocator{};
    CTaskAllocator*                m_PrimaryTaskAllocator{};
    CTaskAllocator*                m_EventResponseTaskAllocator{};
    int32                          m_DecisionMakerType{-1};
    int32                          m_TaskSeqId{-1}; // Used in CTaskSequences::ms_taskSequence
};
VALIDATE_SIZE(CPedGroupIntelligence, 0x2A0);
