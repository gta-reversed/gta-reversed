#include "StdInc.h"
#include "TaskComplexKillPedFromBoat.h"
#include "TaskSimpleStandStill.h"
#include "TaskComplexKillPedOnFoot.h"

void CTaskComplexKillPedFromBoat::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexKillPedFromBoat, 0x86da98, 11);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedInstall(Constructor, 0x6227C0);
    RH_ScopedInstall(Destructor, 0x622830);

    RH_ScopedVMTInstall(Clone, 0x6238A0);
    RH_ScopedVMTInstall(GetTaskType, 0x622820);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x622890);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x622900);
    RH_ScopedVMTInstall(ControlSubTask, 0x622980);
}

// 0x6227C0
CTaskComplexKillPedFromBoat::CTaskComplexKillPedFromBoat(CPed* ped) {
    m_Ped = ped;
    CEntity::SafeRegisterRef(m_Ped);
}

// 0x622830
CTaskComplexKillPedFromBoat::~CTaskComplexKillPedFromBoat() {
    CEntity::SafeCleanUpRef(m_Ped);
}

//! @notsa
static CTask* CreateStandStillTask() {
    return new CTaskSimpleStandStill{ 0, true, false, 8.f };
}

// 0x6238A0
CTask* CTaskComplexKillPedFromBoat::Clone() const {
    return new CTaskComplexKillPedFromBoat{ m_Ped };
}

// 0x622890
CTask* CTaskComplexKillPedFromBoat::CreateNextSubTask(CPed* ped) {
    return CreateStandStillTask();
}

// 0x622900
CTask* CTaskComplexKillPedFromBoat::CreateFirstSubTask(CPed* ped) {
    ped->SetStayInSamePlace(true);
    return CreateStandStillTask();
}

// 0x622980
CTask* CTaskComplexKillPedFromBoat::ControlSubTask(CPed* ped) {
    // Only cops care about the player in their boat
    const auto IsCopTargetingPlayer = [&] {
        return ped->m_nPedType == PED_TYPE_COP && m_Ped && m_Ped->IsPlayer();
    };
    const auto IsPlayerWanted = [] {
        return FindPlayerWanted()->GetWantedLevel() != eWantedLevel::WANTED_CLEAN;
    };
    const auto Say = [ped](eGlobalSpeechContext ctx) {
        AudioEngine.SayPedless(AE_SPEECH_PED, ctx, ped, 0, 1.f, false, false, false);
    };

    switch (m_pSubTask->GetTaskType()) {
    case TASK_SIMPLE_STAND_STILL: { // 0x622AB8 - Waiting in the boat, go after the player as soon as they become wanted
        if (!IsCopTargetingPlayer() || !IsPlayerWanted()) {
            break;
        }
        if (!m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_URGENT, nullptr)) {
            break;
        }
        return new CTaskComplexKillPedOnFoot{ m_Ped };
    }
    case TASK_COMPLEX_KILL_PED_ON_FOOT: { // 0x6229BA
        if (IsCopTargetingPlayer()) {
            if (m_Ped->IsInVehicle()) {
                if (m_Ped->m_pVehicle->m_vehicleAudio.m_AuSettings.VehicleAudioType == AE_BOAT) {
                    Say(CTX_GLOBAL_POLICE_BOAT);
                }
            } else if (m_Ped->physicalFlags.bSubmergedInWater) {
                Say(CTX_GLOBAL_POLICE_OVERBOARD);
            }
        }

        // 0x622A37 - Player is no longer wanted, go back to waiting
        if (!IsCopTargetingPlayer() || IsPlayerWanted()) {
            break;
        }
        if (!m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_URGENT, nullptr)) {
            break;
        }
        return CreateStandStillTask();
    }
    }
    return m_pSubTask;
}
