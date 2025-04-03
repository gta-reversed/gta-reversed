#include "StdInc.h"
#include "TaskSimplePlayerOnFire.h"
#include "TaskComplexOnFire.h"

void CTaskSimplePlayerOnFire::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimplePlayerOnFire, 0x86e334, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x633560);
    RH_ScopedInstall(Destructor, 0x6335B0);

    RH_ScopedVMTInstall(Clone, 0x636E80);
    RH_ScopedVMTInstall(GetTaskType, 0x633590);
    RH_ScopedVMTInstall(MakeAbortable, 0x6335A0);
    RH_ScopedVMTInstall(ProcessPed, 0x6335C0);
}

// 0x6335C0
bool CTaskSimplePlayerOnFire::ProcessPed(CPed* ped) {
    m_timer.StartIfNotAlready(5000);

    if (!ped->m_pFire) {
        return true;
    }

    if (m_timer.IsOutOfTime() || ped->GetIntelligence()->GetTaskSwim()) {
        ped->m_pFire->Extinguish();
        return true;
    }

    if (FindPlayerPed(0) == ped && FindPlayerInfo().m_bFireProof) {
        return false;
    }

    // TODO: Add stubs
    CPedDamageResponse dmgResp;
    CTaskComplexOnFire::ComputeFireDamage(ped, dmgResp);
    if (!dmgResp.m_bHealthZero || m_bAddedDamageEvent) {
        if (CLocalisation::PedsOnFire()) { // Moved code a little to avoid dup
            ped->Say(CTX_GLOBAL_PAIN_ON_FIRE, 0, 0.1f); // Screaming
        }
    }

    if (!dmgResp.m_bHealthZero) {
        return false;
    }

    if (m_bAddedDamageEvent) {
        return true;
    }

    CEventDamage dmgEvent{ nullptr, CTimer::GetTimeInMS(), WEAPON_FLAMETHROWER, PED_PIECE_TORSO, 0u, false, !!ped->bInVehicle };
    ped->GetEventGroup().Add(&dmgEvent);

    return false;
}
