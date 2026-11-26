#include "StdInc.h"
#include "EventSoundLoud.h"

CEventSoundLoud::CEventSoundLoud(CEntity* entity) :
    m_Entity{ entity } {
}

bool CEventSoundLoud::AffectsPed(CPed* ped) {
    if (ped->IsPlayer() || !ped->IsAlive() || !TakesPriorityOver(*this)) {
        return false;
    }
    return CEvent::GetSoundLevel(nullptr, ped->GetPosition()) >= 70.0f;
}
