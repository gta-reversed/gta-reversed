#include "StdInc.h"
#include "EventPotentialGetRunOver.h"


CEventPotentialGetRunOver::CEventPotentialGetRunOver(CVehicle* vehicle) {
    m_Vehicle = vehicle;
    CEntity::SafeRegisterRef(m_Vehicle);
}

CEventPotentialGetRunOver::~CEventPotentialGetRunOver() {
    CEntity::SafeCleanUpRef(m_Vehicle);
}

bool CEventPotentialGetRunOver::AffectsPed(CPed* ped) {
    return ped->IsAlive()
       && !ped->m_pAttachedTo
       && !ped->bInVehicle
       && m_Vehicle
       && !m_Vehicle->IsBoat();
}

