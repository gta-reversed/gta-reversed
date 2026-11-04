#include "StdInc.h"
#include "PedScanner.h"

// 0x5FFEF0
void CPedScanner::ScanForPedsInRange(CPed& ped) {
    if (ped.IsAlive())
        CEntityScanner::ScanForEntitiesInRange(REPEATSECTOR_PEDS, ped);
}

// 0x5FFF20
CPed* CPedScanner::GetClosestPedInRange() const {
    return static_cast<CPed*>(m_pClosestEntityInRange);
}
