#pragma once

#include "TaskTimer.h"
#include "Enums/eEventType.h"

class CEvent;
class CTask;

class CEventHandlerHistory {
public:
    static void InjectHooks();

    CEventHandlerHistory() = default;
    ~CEventHandlerHistory();

    void ClearAllEvents();
    void ClearNonTempEvent();
    void ClearTempEvent();
    void ClearStoredActiveEvent();
    void Flush();
    CEvent* GetCurrentEvent() const { return m_CurrentTempEvent ? m_CurrentTempEvent : m_CurrentNonTempEvent; }
    int32 GetCurrentEventPriority() const;
    CEvent* GetStoredActiveEvent() const { return m_StoredEventActive; }
    bool IsRespondingToEvent(eEventType eventType) const;
    void RecordCurrentEvent(CPed* ped, CEvent& event);
    void StoreActiveEvent();
    bool TakesPriorityOverCurrentEvent(CEvent& event) const;
    void TickStoredEvent(CPed*);

    void RecordAbortedTask(CTask* task) { m_AbortedTask = task; }
    auto GetAbortedTask() const { return m_AbortedTask; }

private:
    CTask*     m_AbortedTask{}; //< Active task aborted (Using `MakeAbortable`) the lastk tick - It's read/set, but the value (other than null checks) isn't used anywhere
    CEvent*    m_CurrentNonTempEvent{};
    CEvent*    m_CurrentTempEvent{};
    CEvent*    m_StoredEventActive{};
    CTaskTimer m_StoreTimer{};
};
VALIDATE_SIZE(CEventHandlerHistory, 0x1C);
