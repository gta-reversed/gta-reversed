#include "StdInc.h"
#include "EventGlobalGroup.h"


void CEventGlobalGroup::InjectHooks() {
    RH_ScopedVirtualClass(CEventGlobalGroup, 0x85AAB4, 1);
    RH_ScopedCategory("Events");

    RH_ScopedInstall(GetSoundLevel, 0x4AB900);
    RH_ScopedInstall(AddEventsToPed, 0x4AB9C0);
    RH_ScopedInstall(AddEventsToGroup, 0x4AB8A0);
}

// 0x4AB900
float CEventGlobalGroup::GetSoundLevel(CEntity* entity, CVector& position) {
    float soundLevel = 0.0f;
    for (auto& event : GetEvents()) {
        if (event->GetSourceEntity() == entity || !entity) {
            float eventSoundLevel = event->GetSoundLevel(entity, position);
            if (eventSoundLevel > 0.0f) {
                soundLevel += CEvent::CalcSoundLevelIncrement(soundLevel, eventSoundLevel);
            }
        }
    }
    return soundLevel;
}

// 0x4AB9C0
void CEventGlobalGroup::AddEventsToPed(CPed* ped) {
    for (auto& event : GetEvents()) {
        CEvent* clonedEvent = event->Clone();
        ped->GetEventGroup().Add(clonedEvent, false);
        delete clonedEvent;
    }
}

// 0x4AB8A0
void CEventGlobalGroup::AddEventsToGroup(CPedGroup* pedGroup) {
    for (auto& event : GetEvents()) {
        CEvent* clonedEvent = event->Clone();
        pedGroup->GetIntelligence().AddEvent(clonedEvent);
        delete clonedEvent;
    }
}

