#include "StdInc.h"

#include "TaskSimpleDrownInCar.h"

void CTaskSimpleDrownInCar::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleDrownInCar, 0x86DE54, 9);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedVMTInstall(ProcessPed, 0x62FF70);
}

// 0x62FF70

// 0x0
bool CTaskSimpleDrownInCar::ProcessPed(CPed* ped) {
    ped->SetPedState(PEDSTATE_DIE);
    ped->bIsInTheAir = false;

    if (ped == FindPlayerPed()) {
        CStats::IncrementStat(STAT_TIMES_DROWNED, 1.0f);
        ped->m_pVehicle->m_nStatus = STATUS_FORCED_STOP;
    }

    CEventDeath event(true);
    ped->GetEventGroup().Add(&event, false);
    return true;
}