#include "StdInc.h"

#include "EventVehicleOnFire.h"

void CEventVehicleOnFire::InjectHooks()
{
    RH_ScopedVirtualClass(CEventVehicleOnFire, 0x85B440, 17);
    RH_ScopedCategory("Events");

    RH_ScopedInstall(Constructor, 0x4B10C0);
    RH_ScopedVMTInstall(AffectsPed, 0x4B4FD0);
    RH_ScopedVMTInstall(CloneEditable, 0x4B7740);
}

// 0x4B10C0
CEventVehicleOnFire::CEventVehicleOnFire(CVehicle* vehicle)
{
    m_vehicle = vehicle;
    CEntity::SafeRegisterRef(m_vehicle);
}

CEventVehicleOnFire::~CEventVehicleOnFire()
{
    CEntity::SafeCleanUpRef(m_vehicle);
}

CEventVehicleOnFire* CEventVehicleOnFire::Constructor(CVehicle* vehicle)
{
    this->CEventVehicleOnFire::CEventVehicleOnFire(vehicle);
    return this;
}

// 0x4B4FD0
bool CEventVehicleOnFire::AffectsPed(CPed* ped)
{
    if (m_vehicle) {
        if (!ped->m_pVehicle || !ped->bInVehicle || ped->m_pVehicle == m_vehicle) {
            if (!ped->IsPlayer() && ped->IsAlive()) {
                CPedGroup* pedGroup = CPedGroups::GetPedsGroup(ped);
                if (pedGroup && !pedGroup->GetMembership().IsLeader(ped)) {
                    if (ped->bInVehicle)
                        return false;
                }
                if (ped->bInVehicle)
                    return true;
                CVector distance = ped->GetPosition() - m_vehicle->GetPosition();
                return distance.SquaredMagnitude() <= 100.0f;
            }
        }
    }
    return false;
}

// 0x4B7740
CEventEditableResponse* CEventVehicleOnFire::CloneEditable()
{
    return new CEventVehicleOnFire(m_vehicle);
}