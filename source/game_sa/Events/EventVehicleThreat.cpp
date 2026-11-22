#include "StdInc.h"
#include "EventVehicleThreat.h"


CEventVehicleThreat::CEventVehicleThreat(CVehicle* vehicle) : CEventEditableResponse() {
    m_Vehicle = vehicle;
    CEntity::SafeRegisterRef(m_Vehicle);
}

CEventVehicleThreat::~CEventVehicleThreat() {
    CEntity::SafeCleanUpRef(m_Vehicle);
}

bool CEventVehicleThreat::AffectsPed(CPed* ped) {
    return ped->IsAlive();
}

