#include "StdInc.h"
#include "EventPedEnteredMyVehicle.h"


CEventPedEnteredMyVehicle::CEventPedEnteredMyVehicle(CPed* pedThatEntered, CVehicle* vehicle, eTargetDoor targetDoor) :
    m_Vehicle{vehicle},
    m_PedThatEntered{pedThatEntered},
    m_TargetDoor{targetDoor}
{
    CEntity::SafeRegisterRef(m_PedThatEntered);
    CEntity::SafeRegisterRef(m_Vehicle);
}

CEventPedEnteredMyVehicle::~CEventPedEnteredMyVehicle() {
    CEntity::SafeCleanUpRef(m_PedThatEntered);
    CEntity::SafeCleanUpRef(m_Vehicle);
}

bool CEventPedEnteredMyVehicle::AffectsPed(CPed* ped) {
    return ped->IsAlive()
        && ped->m_pVehicle
        && ped->bInVehicle
        && ped->m_pVehicle == m_Vehicle
        && m_PedThatEntered;
}

