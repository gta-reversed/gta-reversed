#include "StdInc.h"
#include "TaskComplexKillPedFromBoat.h"
#include "TaskComplexKillPedOnFoot.h"
#include "TaskSimpleStandStill.h"

// 0x6227C0
CTaskComplexKillPedFromBoat::CTaskComplexKillPedFromBoat(CPed* targetPed) :
    m_TargetPed{ targetPed } {
}

// NOTSA
CTaskComplexKillPedFromBoat::CTaskComplexKillPedFromBoat(const CTaskComplexKillPedFromBoat& o) :
    CTaskComplexKillPedFromBoat{ o.m_TargetPed } {
}

// 0x622890
CTask* CTaskComplexKillPedFromBoat::CreateNextSubTask(CPed* ped) {
    return new CTaskSimpleStandStill{ 0, true };
}

// 0x622900
CTask* CTaskComplexKillPedFromBoat::CreateFirstSubTask(CPed* ped) {
    ped->SetStayInSamePlace(true);
    return new CTaskSimpleStandStill{ 0, true };
}

// 0x622980
CTask* CTaskComplexKillPedFromBoat::ControlSubTask(CPed* ped) {
    const bool isCopChasingPlayer = ped->IsCop() && m_TargetPed && m_TargetPed->IsPlayer();
    switch (const auto tt = m_pSubTask->GetTaskType()) {
    case TASK_SIMPLE_STAND_STILL: { // 0x622AB8
        if (isCopChasingPlayer && FindPlayerWanted()->GetWantedLevel() != eWantedLevel::WANTED_CLEAN && m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_URGENT, nullptr)) {
            return new CTaskComplexKillPedOnFoot{ m_TargetPed };
        }
        return m_pSubTask;
    }
    case TASK_COMPLEX_KILL_PED_ON_FOOT: { // 0x6229BA
        if (isCopChasingPlayer) {
            if (const auto* const veh = m_TargetPed->GetVehicleIfInOne()) {
                if (veh->m_vehicleAudio.m_AuSettings.VehicleAudioType == AE_BOAT) { // 0x6229F5
                    AudioEngine.SayPedless(AE_SPEECH_PED, CTX_GLOBAL_POLICE_BOAT, ped, 0, 1.f, false, false, false);
                }
            } else if (m_TargetPed->physicalFlags.bSubmergedInWater) { // 0x622A13
                AudioEngine.SayPedless(AE_SPEECH_PED, CTX_GLOBAL_POLICE_OVERBOARD, ped, 0, 1.f, false, false, false);
            }
        }
        if (isCopChasingPlayer && FindPlayerWanted()->GetWantedLevel() == eWantedLevel::WANTED_CLEAN && m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_URGENT, nullptr)) { // 0x622A37
            return new CTaskSimpleStandStill{ 0, true };
        }
        return m_pSubTask;
    }
    default:
        NOTSA_UNREACHABLE_CASE(tt);
    }
}

void CTaskComplexKillPedFromBoat::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexKillPedFromBoat, 0x86da98, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedConstructorInstall(0x6227C0, "", {}, CPed*);
    RH_ScopedDestructorInstall(0x622830);

    RH_ScopedVMTInstall(Clone, 0x6238A0);
    RH_ScopedVMTInstall(GetTaskType, 0x622820);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x622890);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x622900);
    RH_ScopedVMTInstall(ControlSubTask, 0x622980);
}
