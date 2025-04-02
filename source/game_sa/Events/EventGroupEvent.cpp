#include "StdInc.h"

#include "EventGroupEvent.h"

// 0x4ADFD0
CEventGroupEvent::CEventGroupEvent(CPed* ped, CEvent* event) : CEvent() {
    m_ped = ped;
    m_event = event;
    CEntity::SafeRegisterRef(m_ped);
}

// 0x4AE070
CEventGroupEvent::~CEventGroupEvent() {
    CEntity::SafeCleanUpRef(m_ped);
    delete m_event;
}

// NOTSA, inlined
bool CEventGroupEvent::IsPriorityEvent() const {
    switch (m_event->GetEventType()) {
    case EVENT_LEADER_ENTERED_CAR_AS_DRIVER:
    case EVENT_LEADER_EXITED_CAR_AS_DRIVER:
    case EVENT_LEADER_QUIT_ENTERING_CAR_AS_DRIVER:
    case EVENT_PLAYER_COMMAND_TO_GROUP:
    case EVENT_PLAYER_COMMAND_TO_GROUP_GATHER:
    case EVENT_DRAGGED_OUT_CAR:
    case EVENT_LEADER_ENTRY_EXIT:
        return true;
    }
    return false;
}

// 0x4AE100
bool CEventGroupEvent::BaseEventTakesPriorityOverBaseEvent(const CEventGroupEvent& other) {
    return IsPriorityEvent()
        || !other.IsPriorityEvent() && GetEvent().TakesPriorityOver(other.GetEvent());
}
