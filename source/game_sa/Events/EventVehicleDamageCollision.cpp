#include "StdInc.h"

#include "EventVehicleDamageCollision.h"

void CEventVehicleDamageCollision::InjectHooks()
{
    RH_ScopedVirtualClass(CEventVehicleDamageCollision, 0x8710D0, 17);
    RH_ScopedCategory("Events");

    RH_ScopedInstall(Constructor, 0x6A0620);
    RH_ScopedVMTInstall(CloneEditable, 0x6A0670);
}

// 0x6A0620
CEventVehicleDamageCollision::CEventVehicleDamageCollision(CVehicle* vehicle, CEntity* attacker, eWeaponType weaponType) :
    CEventVehicleDamage(vehicle, attacker, weaponType)
{
    // empty
}

CEventVehicleDamageCollision* CEventVehicleDamageCollision::Constructor(CVehicle* vehicle, CEntity* attacker, eWeaponType weaponType)
{
    this->CEventVehicleDamageCollision::CEventVehicleDamageCollision(vehicle, attacker, weaponType);
    return this;
}

// 0x6A0670
CEventEditableResponse* CEventVehicleDamageCollision::CloneEditable()
{
    return new CEventVehicleDamageCollision(m_vehicle, m_attacker, m_weaponType);
}