#include "StdInc.h"

#include "EventVehicleDied.h"

void CEventVehicleDied::InjectHooks()
{
    RH_ScopedVirtualClass(CEventVehicleDied, 0x85B400, 16);
    RH_ScopedCategory("Events");

    RH_ScopedInstall(Constructor, 0x4B0D10);
    RH_ScopedVMTInstall(Clone, 0x4B76D0);
    RH_ScopedVMTInstall(AffectsPed, 0x4B0E00);
}

CEventVehicleDied::CEventVehicleDied (CVehicle* vehicle)
{
    m_vehicle = vehicle;
    m_IsOccupantLeavingCar = false;
    m_vehicle->RegisterReference(reinterpret_cast<CEntity**>(&m_vehicle)); // todo: possible nullptr
}

CEventVehicleDied::~CEventVehicleDied ()
{
    CEntity::SafeCleanUpRef(m_vehicle);
}

// 0x4B0D10
CEventVehicleDied* CEventVehicleDied::Constructor(CVehicle* vehicle)
{
    this->CEventVehicleDied::CEventVehicleDied(vehicle);
    return this;
}

// 0x4B76D0
CEvent* CEventVehicleDied::Clone()
{
    return new CEventVehicleDied(m_vehicle);
}

// 0x4B0E00
bool CEventVehicleDied::AffectsPed(CPed* ped)
{
    if (ped->IsAlive() && !m_IsOccupantLeavingCar) {
        m_IsOccupantLeavingCar = ped->GetTaskManager().Has<TASK_COMPLEX_LEAVE_CAR>();
        if ((m_IsOccupantLeavingCar || ped->bInVehicle) && m_vehicle)
            return ped->m_pVehicle == m_vehicle;
    }
    return false;
}