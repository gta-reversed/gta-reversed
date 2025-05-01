#include "StdInc.h"
#include "EventSexyVehicle.h"


CEventSexyVehicle::CEventSexyVehicle(CVehicle* vehicle) : CEvent() {
    m_vehicle = vehicle;
    CEntity::SafeRegisterRef(m_vehicle);
}

CEventSexyVehicle::~CEventSexyVehicle() {
    CEntity::SafeCleanUpRef(m_vehicle);
}

bool CEventSexyVehicle::AffectsPed(CPed* ped) {
    return ped->IsAlive() && m_vehicle;
}

