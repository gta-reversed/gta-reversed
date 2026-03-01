#include "StdInc.h"

#include "EventGotKnockedOverByCar.h"

// 0x4B1B60
CEventGotKnockedOverByCar::CEventGotKnockedOverByCar(CVehicle* vehicle) {
    m_vehicle = vehicle;
    CEntity::SafeRegisterRef(m_vehicle);
}

CEventGotKnockedOverByCar::~CEventGotKnockedOverByCar() {
    CEntity::SafeCleanUpRef(m_vehicle);
}

// 0x4B1C70
bool CEventGotKnockedOverByCar::AffectsPed(CPed* ped) {
    if (m_vehicle && !ped->IsPlayer())
        return ped->IsAlive();
    return false;
}

// 0x4B7960
CEventEditableResponse* CEventGotKnockedOverByCar::CloneEditable() const noexcept {
    return new CEventGotKnockedOverByCar(m_vehicle);
}
