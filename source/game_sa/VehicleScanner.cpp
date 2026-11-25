#include "StdInc.h"

#include "VehicleScanner.h"

// 0x5FFE60
void CVehicleScanner::ScanForVehiclesInRange(CPed& ped) {
    if (!ped.IsAlive())
        return;

    if (ped.m_pVehicle && ped.bInVehicle) {
        if (ped.IsCreatedByMission())
            CEntityScanner::ScanForEntitiesInRange(REPEATSECTOR_VEHICLES, ped);

        CEntity::SafeCleanUpRef(m_pClosestEntityInRange);
        m_pClosestEntityInRange = ped.m_pVehicle;
        CEntity::SafeRegisterRef(m_pClosestEntityInRange);
    } else {
        CEntityScanner::ScanForEntitiesInRange(REPEATSECTOR_VEHICLES, ped);
    }
}

CVehicle* CVehicleScanner::GetClosestVehicleInRange() const {
    return static_cast<CVehicle*>(m_pClosestEntityInRange);
}
