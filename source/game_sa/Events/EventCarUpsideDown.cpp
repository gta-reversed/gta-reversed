#include "StdInc.h"
#include "EventCarUpsideDown.h"


void CEventCarUpsideDown::InjectHooks() {
    RH_ScopedVirtualClass(CEventCarUpsideDown, 0x85B660, 16);
    RH_ScopedCategory("Events");

    RH_ScopedInstall(Constructor, 0x4B1CC0);
    RH_ScopedVMTInstall(AffectsPed, 0x4B1DB0);
}

// 0x4B1CC0
CEventCarUpsideDown* CEventCarUpsideDown::Constructor(CVehicle* vehicle) { this->CEventCarUpsideDown::CEventCarUpsideDown(vehicle); return this; }

// 0x4B1CC0
CEventCarUpsideDown::CEventCarUpsideDown(CVehicle* vehicle) : CEvent() {
    m_vehicle = vehicle;
    CEntity::SafeRegisterRef(m_vehicle);
}

CEventCarUpsideDown::~CEventCarUpsideDown() {
#if FIX_BUGS
    CEntity::SafeCleanUpRef(m_vehicle);
#else
    CEntity::SafeRegisterRef(m_vehicle);
#endif
}

// 0x4B1DB0
bool CEventCarUpsideDown::AffectsPed(CPed* ped) {
    if (ped->IsPlayer()) return false;
    if (ped->bGetOutUpsideDownCar == 0u) return false;
    if (ped->bInVehicle == 0u) return false;
    if (ped->IsCreatedByMission() && (!ped->m_pVehicle || ped->m_pVehicle->IsBoat())) return false;
    if (ped->m_pVehicle->IsBike() || ped->m_pVehicle->IsQuad()) return false;
    return ped->IsAlive();
}