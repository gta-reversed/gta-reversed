#include "StdInc.h"
#include "EventCopCarBeingStolen.h"


void CEventCopCarBeingStolen::InjectHooks() {
    RH_ScopedVirtualClass(CEventCopCarBeingStolen, 0x85B548, 16);
    RH_ScopedCategory("Events");

    RH_ScopedInstall(Constructor, 0x4B1740);
    RH_ScopedVMTInstall(AffectsPed, 0x4B1860);
}

// 0x4B1740
CEventCopCarBeingStolen* CEventCopCarBeingStolen::Constructor(CPed* hijacker, CVehicle* vehicle) { this->CEventCopCarBeingStolen::CEventCopCarBeingStolen(hijacker, vehicle); return this; }

// 0x4B1740
CEventCopCarBeingStolen::CEventCopCarBeingStolen(CPed* hijacker, CVehicle* vehicle) : CEvent() {
    m_hijacker = hijacker;
    m_vehicle = vehicle;
    CEntity::SafeRegisterRef(m_hijacker);
    CEntity::SafeRegisterRef(m_vehicle);
}

CEventCopCarBeingStolen::~CEventCopCarBeingStolen() {
    CEntity::SafeCleanUpRef(m_hijacker);
    CEntity::SafeCleanUpRef(m_vehicle);
}

// 0x4B1860
bool CEventCopCarBeingStolen::AffectsPed(CPed* ped) {
    if (m_hijacker && m_vehicle && m_hijacker->IsAlive() && ped->IsAlive() && ped->m_nPedType == PED_TYPE_COP) {
        if (m_vehicle == ped->m_pVehicle && ped->bInVehicle)
            return true;
    }
    return false;
}