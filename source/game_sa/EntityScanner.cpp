#include "StdInc.h"

#include "EntityScanner.h"

void CEntityScanner::InjectHooks() {
    RH_ScopedClass(CEntityScanner);
    RH_ScopedCategoryGlobal();

    // RH_ScopedInstall(Clear, 0x5FF9D0);
    // RH_ScopedInstall(ScanForEntitiesInRange, 0x5FFA20);
    RH_ScopedInstall(GetClosestPedInRange, 0x5FFF20);
}

// 0x5FF9D0
void CEntityScanner::Clear() {
    plugin::CallMethod<0x5FF9D0, CEntityScanner*>(this);
}

// 0x5FFA20
void CEntityScanner::ScanForEntitiesInRange(int32 arg2, CPed* ped) {
    plugin::CallMethod<0x5FFA20, CEntityScanner*, int32, CPed*>(this, arg2, ped);
}

// 0x5FFF20
CEntity* CEntityScanner::GetClosestPedInRange() {
    return m_pClosestEntityInRange;
}
