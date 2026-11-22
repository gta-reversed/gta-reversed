#include "StdInc.h"
#include "EventLeanOnVehicle.h"


void CEventLeanOnVehicle::InjectHooks() {
    RH_ScopedVirtualClass(CEventLeanOnVehicle, 0x86F878, 16);
    RH_ScopedCategory("Events");

    RH_ScopedInstall(Constructor, 0x65DAF0);
    RH_ScopedVMTInstall(IsValid, 0x4B16C0);
}

// 0x65DAF0
CEventLeanOnVehicle* CEventLeanOnVehicle::Constructor(CVehicle* vehicle, int32 leanAnimDurationInMs) { this->CEventLeanOnVehicle::CEventLeanOnVehicle(vehicle, leanAnimDurationInMs); return this; }

// 0x65DAF0
CEventLeanOnVehicle::CEventLeanOnVehicle(CVehicle* vehicle, int32 leanAnimDurationInMs) : CEvent() {
    m_vehicle = vehicle;
    m_leanAnimDurationInMs = leanAnimDurationInMs;
    CEntity::SafeRegisterRef(m_vehicle);
}

CEventLeanOnVehicle::~CEventLeanOnVehicle() {
    CEntity::SafeCleanUpRef(m_vehicle);
}

// 0x4B16C0
bool CEventLeanOnVehicle::IsValid(CPed* ped) {
    if (ped)
        return ped->IsAlive();

    if (IsValid(ped))
        return true;

    return false;
}