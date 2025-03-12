#include "StdInc.h"

#include "Event.h"
#include "EventGroup.h"

void CEventGroup::InjectHooks() {
    RH_ScopedVirtualClass(CEventGroup, 0x85AAB0, 1);
    RH_ScopedCategory("Events");

    RH_ScopedInstall(Constructor, 0x4AB340);

    RH_ScopedOverloadedInstall(Add, "", 0x4AB420, CEvent*(CEventGroup::*)(CEvent*, bool));
    RH_ScopedInstall(HasScriptCommandOfTaskType, 0x4AB840);
    RH_ScopedInstall(HasEventOfType, 0x4AB5E0);
    RH_ScopedInstall(HasEvent, 0x4AB6A0);
    RH_ScopedInstall(GetHighestPriorityEvent, 0x4AB7C0);
    RH_ScopedInstall(TickEvents, 0x4AB6D0);
    RH_ScopedInstall(Remove, 0x4AB5A0);
    RH_ScopedInstall(RemoveInvalidEvents, 0x4AB760);
    RH_ScopedInstall(Reorganise, 0x4AB700);
    RH_ScopedInstall(Flush, 0x4AB370);
    RH_ScopedInstall(GetEventOfType, 0x4AB650);
}

// 0x4AB340
CEventGroup::CEventGroup(CPed* ped) :
    m_pPed{ped}
{
}

CEventGroup::~CEventGroup() {
    Flush();
}

// 0x4AB420
CEvent* CEventGroup::Add(CEvent* event, bool bValid) {
    if (m_pPed) {
        if (event->HasEditableResponse()) {
            const auto eventResp = static_cast<CEventEditableResponse*>(event);

            const auto bIsEventDamage          = event->GetEventType() == EVENT_DAMAGE;
            const auto bInformGroup            = eventResp->ComputeResponseTaskOfType(m_pPed, TASK_SIMPLE_INFORM_GROUP);
            const auto bInformRespectedFriends = eventResp->ComputeResponseTaskOfType(m_pPed, TASK_SIMPLE_INFORM_RESPECTED_FRIENDS);
            const auto bTriggerLookAt          = eventResp->ComputeResponseTaskOfType(m_pPed, TASK_SIMPLE_LOOK_AT_ENTITY_OR_COORD);

            eventResp->ComputeResponseTaskType(m_pPed, false);

            const auto bAddToEventGroup        = eventResp->WillRespond() || (bIsEventDamage && eventResp->m_bAddToEventGroup);

            if (!event->AffectsPed(m_pPed)) {
                return nullptr;
            }
            if (bInformGroup) {
                eventResp->InformGroup(m_pPed);
            }
            if (bInformRespectedFriends && (!bIsEventDamage || !static_cast<CEventDamage*>(event)->m_bStealthMode)) {
                eventResp->InformRespectedFriends(m_pPed);
            }
            if (bTriggerLookAt) {
                eventResp->TriggerLookAt(m_pPed);
            }
            eventResp->InformVehicleOccupants(m_pPed);
            if (!bAddToEventGroup) {
                return nullptr;
            }
        } else {
            if (!event->AffectsPed(m_pPed)) {
                return nullptr;
            }
        }
        m_pPed->GetIntelligence()->RecordEventForScript(event->GetEventType(), event->GetEventPriority());
    }

    if (m_count >= TOTAL_EVENTS_PER_EVENTGROUP) {
        return nullptr;
    }

    const auto clonedEvent = event->Clone();
    clonedEvent->m_bValid  = bValid;
    if (m_pPed) {
        clonedEvent->ReportCriminalEvent(m_pPed);
    }
    m_events[m_count++] = clonedEvent;
    
    return clonedEvent;
}

// 0x4AB840
bool CEventGroup::HasScriptCommandOfTaskType(eTaskType taskId) {
    for (auto& event : GetEvents()) {
        if (const auto eScriptCmd = CEvent::DynCast<CEventScriptCommand>(event)) {
            if (eScriptCmd->m_task && eScriptCmd->m_task->GetTaskType() == taskId) {
                return true;
            }
        }
    }
    return false;
}

// 0x4AB5E0
bool CEventGroup::HasEventOfType(CEvent* eventOfType) const {
    return GetEventOfType(eventOfType->GetEventType()) != nullptr;
}

// 0x4AB7C0
CEvent* CEventGroup::GetHighestPriorityEvent() {
    CEvent* theEvent = nullptr;
    int32 highestPriority = -1;
    for (auto& event : GetEvents()) {
        int32 eventPriority = event->GetEventPriority();
        bool bIsPriorityGreater = false;
        if (event->GetEventType() == EVENT_SCRIPT_COMMAND) {
            if (eventPriority > highestPriority)
                bIsPriorityGreater = true;
        } else if (eventPriority >= highestPriority) {
            bIsPriorityGreater = true;
        }
        if (bIsPriorityGreater && event->AffectsPed(m_pPed)) {
            highestPriority = eventPriority;
            theEvent = event;
        }
    }
    return theEvent;
}

// 0x4AB6D0
void CEventGroup::TickEvents() {
    rng::for_each(GetEvents(), &CEvent::Tick);
}

// 0x4AB6A0
bool CEventGroup::HasEvent(CEvent* needle) {
    return notsa::contains(GetEvents(), needle);
}

// 0x4AB5A0
void CEventGroup::Remove(CEvent* event) {
    const auto it = rng::find(GetEvents(), event);
    if (it != GetEvents().end()) {
        delete std::exchange(*it, nullptr);
    }
}

// 0x4AB760
void CEventGroup::RemoveInvalidEvents(bool bEverythingButScriptEvents) {
    for (auto& event : GetEvents()) {
        if (!event) {
            continue;
        }
        if (event->IsValid(m_pPed)) {
            if (!bEverythingButScriptEvents || event->GetEventType() == EVENT_SCRIPT_COMMAND) {
                continue;
            }
        }
        delete std::exchange(event, nullptr);
    }
}

// 0x4AB700
void CEventGroup::Reorganise() {
    const auto   old{m_events}; // Copy old event array
    const size_t cnt{m_count};  // And count
    m_count = 0;
    for (size_t i = 0; i < cnt; i++) { // Make array contiguous and re-count number of events
        if (const auto e = old[i]) {
            m_events[m_count++] = e;
        }
    }
}

// 0x4AB370
void CEventGroup::Flush(bool bKeepJoinGroupScriptCommands) {
    const auto eScriptCmdBeInGroup = [=, this]() -> CEvent* {
        if (bKeepJoinGroupScriptCommands) {
            for (auto& event : GetEvents()) {
                if (const auto eScriptCmd = CEvent::DynCast<CEventScriptCommand>(event)) {
                    if (eScriptCmd->m_task && eScriptCmd->m_task->GetTaskType() == TASK_COMPLEX_BE_IN_GROUP) {
                        return std::exchange(event, nullptr); // null the slot out so it doesn't get deleted
                    }
                }
            }
        }
        return nullptr;
    }();

    for (auto& event : GetEvents()) {
        delete std::exchange(event, nullptr);
    }

    m_count = 0;
    if (eScriptCmdBeInGroup) {
        m_events[m_count++] = eScriptCmdBeInGroup;
    }
}

// 0x4AB650
CEvent* CEventGroup::GetEventOfType(eEventType type) const noexcept {
    for (auto& event : GetEvents()) {
        if (event->GetEventType() == type) {
            return event;
        }
    }
    return nullptr;
}
