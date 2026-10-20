#include "StdInc.h"

#include "ObjectScanner.h"

// 0x5FFF30
void CObjectScanner::ScanForObjectsInRange(CPed& ped) {
    if (ped.m_pVehicle && ped.bInVehicle)
        return;

    if (ped.IsAlive())
        CEntityScanner::ScanForEntitiesInRange(REPEATSECTOR_OBJECTS, ped);
}

CObject* CObjectScanner::GetClosestObjectInRange() const {
    return static_cast<CObject*>(m_pClosestEntityInRange);
}
