#include "StdInc.h"

#include "TaskSimpleDrown.h"
#include "TaskSimpleDie.h"

void CTaskSimpleDrown::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleDrown, 0x86DE30, 9);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedVMTInstall(ProcessPed, 0x639900);
}

// 0x62FEC0
CTaskSimpleDrown::CTaskSimpleDrown() : CTaskSimpleDie(ANIM_GROUP_DEFAULT, ANIM_ID_DROWN, 4.0f, 0.0f) {
    m_bShouldExecute = true;
}

// 0x639900

// 0x0
bool CTaskSimpleDrown::ProcessPed(CPed* ped) {
    if (m_bShouldExecute) {
        ped->SetPedState(PEDSTATE_DIE);
        ped->bIsInTheAir = false; 
        if (ped == FindPlayerPed()) {
            CStats::IncrementStat(STAT_TIMES_DROWNED, 1.0f);
        }
        m_bShouldExecute = false;
    }
    return CTaskSimpleDie::ProcessPed(ped);
}