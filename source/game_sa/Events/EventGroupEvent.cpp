#include "StdInc.h"

#include "EventGroupEvent.h"

// 0x4ADFD0
CEventGroupEvent::CEventGroupEvent(CPed* ped, CEvent* event) :
    m_Ped{ ped },
    m_GroupEvent{ event } {
}

// 0x4AE070
CEventGroupEvent::~CEventGroupEvent() {
    delete m_GroupEvent;
}

// NOTSA, inlined
bool CEventGroupEvent::IsPriorityEvent() const {
    switch (m_GroupEvent->GetEventType()) {
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
        || !other.IsPriorityEvent() && GetGroupEvent().TakesPriorityOver(other.GetGroupEvent());
}

void CEventGroupEvent::InjectHooks() {
    RH_ScopedVirtualClass(CEventGroupEvent, 0x85ae48, 16);
    RH_ScopedCategory("Events");

    RH_ScopedConstructorInstall(0x4ADFD0, "1", {}, CPed*, CEvent*);
    RH_ScopedDestructorInstall(0x4AE070);

    RH_ScopedInstall(BaseEventTakesPriorityOverBaseEvent, 0x4AE100);
    RH_ScopedVMTInstall(GetEventType, 0x4AE030);
    RH_ScopedVMTInstall(GetEventPriority, 0x4AE060);
    RH_ScopedVMTInstall(GetLifeTime, 0x4AE040);
    RH_ScopedVMTInstall(Clone, 0x4B6EE0);
    RH_ScopedVMTInstall(AffectsPed, 0x4AE0D0);
    RH_ScopedVMTInstall(AffectsPedGroup, 0x4AE0E0);
    RH_ScopedVMTInstall(GetLocalSoundLevel, 0x4AE050);
}
