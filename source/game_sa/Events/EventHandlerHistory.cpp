#include "StdInc.h"

#include "EventHandlerHistory.h"

void CEventHandlerHistory::InjectHooks()
{
    RH_ScopedClass(CEventHandlerHistory);
    RH_ScopedCategory("Events");

    RH_ScopedInstall(ClearAllEvents, 0x4BC550);
    RH_ScopedInstall(ClearNonTempEvent, 0x4B8C60);
    RH_ScopedInstall(ClearTempEvent, 0x4B8C40);
    RH_ScopedInstall(GetCurrentEvent, 0x4B8CA0);
    RH_ScopedInstall(GetCurrentEventPriority, 0x4B8C80);
    RH_ScopedInstall(IsRespondingToEvent, 0x4B8B90);
    RH_ScopedInstall(RecordCurrentEvent, 0x4BC4B0);
    RH_ScopedInstall(StoreActiveEvent, 0x4B8BF0);
    RH_ScopedInstall(TakesPriorityOverCurrentEvent, 0x4BC580);
    RH_ScopedInstall(TickStoredEvent, 0x4B8C20);
}

CEventHandlerHistory::~CEventHandlerHistory()
{
    Flush();
}

// 0x4BC550
void CEventHandlerHistory::ClearAllEvents()
{
    ClearTempEvent();
    ClearNonTempEvent();
}

// 0x4B8C60
void CEventHandlerHistory::ClearNonTempEvent()
{
    delete m_CurrentNonTempEvent;
    m_CurrentNonTempEvent = nullptr;
}

// 0x4B8C40
void CEventHandlerHistory::ClearTempEvent()
{
    delete m_CurrentTempEvent;
    m_CurrentTempEvent = nullptr;
}

void CEventHandlerHistory::ClearStoredActiveEvent()
{
    delete m_StoredEventActive;
    m_StoredEventActive = nullptr;
}

void CEventHandlerHistory::Flush()
{
    ClearAllEvents();
    ClearStoredActiveEvent();
}

// 0x4B8C80
int32 CEventHandlerHistory::GetCurrentEventPriority() const
{
    if (m_CurrentTempEvent)
        return m_CurrentTempEvent->GetEventPriority();
    if (m_CurrentNonTempEvent)
        return m_CurrentNonTempEvent->GetEventPriority();
    return -1;
}

// 0x4B8B90
bool CEventHandlerHistory::IsRespondingToEvent(eEventType eventType) const
{
    if (eventType == EVENT_NONE)
        return m_CurrentNonTempEvent || m_StoredEventActive;

    bool isNonTempEvent = false;
    if (m_CurrentNonTempEvent && m_CurrentNonTempEvent->GetEventType() == eventType)
        isNonTempEvent = true;

    if (!m_StoredEventActive || m_StoredEventActive->GetEventType() != eventType)
        return isNonTempEvent;

    return true;
}

// 0x4BC4B0
void CEventHandlerHistory::RecordCurrentEvent(CPed* ped, CEvent& event)
{
    if (event.GetEventType() == EVENT_SCRIPT_COMMAND) {
        return;
    }
    if (CEventHandler::IsTemporaryEvent(event)) {
        if (m_CurrentNonTempEvent) {
            StoreActiveEvent();
            m_CurrentNonTempEvent = nullptr;
        }
        ClearTempEvent();
        m_CurrentTempEvent = event.Clone();
    } else {
        ClearNonTempEvent();
        m_CurrentNonTempEvent = event.Clone();
        ClearStoredActiveEvent();
        ClearTempEvent();
    }
}

// 0x4B8BF0
void CEventHandlerHistory::StoreActiveEvent()
{
    delete m_StoredEventActive;
    m_StoredEventActive = m_CurrentNonTempEvent;
    m_StoreTimer.Start(2000);
}

// 0x4BC580
bool CEventHandlerHistory::TakesPriorityOverCurrentEvent(CEvent& event) const
{
    if (m_CurrentNonTempEvent)
        return event.TakesPriorityOver(*m_CurrentNonTempEvent);
    if (!m_CurrentTempEvent)
        return true;
    if (CEventHandler::IsTemporaryEvent(event))
        return event.TakesPriorityOver(*m_CurrentTempEvent);
    if (!event.TakesPriorityOver(*m_CurrentTempEvent))
        return false;
    if (!m_StoredEventActive || event.TakesPriorityOver(*m_StoredEventActive))
        return true;
    return false;
}

// 0x4B8C20
void CEventHandlerHistory::TickStoredEvent(CPed*)
{
    if (m_StoredEventActive && !m_CurrentTempEvent) {
        m_CurrentNonTempEvent = m_StoredEventActive;
        m_StoredEventActive = nullptr;
    }
}

