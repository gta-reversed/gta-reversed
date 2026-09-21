#include "StdInc.h"

#include "TaskComplexKillPedFromBoat.h"
#include "TaskSimpleStandStill.h"
#include "TaskComplexKillPedOnFoot.h"
#include <Audio/Enums/PedSpeechContexts.h>

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

// 0x6238A0
CTask* CTaskComplexKillPedFromBoat::Clone() const {
    return new CTaskComplexKillPedFromBoat{ m_Ped };
}

// 0x622890
CTask* CTaskComplexKillPedFromBoat::CreateNextSubTask(CPed* ped) {
    return new CTaskSimpleStandStill{ 0, true, false, 8.0f };
}

// 0x622900
CTask* CTaskComplexKillPedFromBoat::CreateFirstSubTask(CPed* ped) {
    ped->bStayInSamePlace = true;
    return new CTaskSimpleStandStill{ 0, true, false, 8.0f };
}

// 0x622980
CTask* CTaskComplexKillPedFromBoat::ControlSubTask(CPed* ped) {
    switch (m_pSubTask->GetTaskType()) {
    case TASK_SIMPLE_STAND_STILL: { // 0xCB
        if (ped->m_nPedState != PEDSTATE_DRIVING || !m_Ped || !m_Ped->IsPlayer()) {
            return m_pSubTask;
        }
        if (FindPlayerWanted()->GetWantedLevel() == eWantedLevel::WANTED_CLEAN) {
            return m_pSubTask;
        }
        if (!m_pSubTask->MakeAbortable(ped)) {
            return m_pSubTask;
        }
        return new CTaskComplexKillPedOnFoot{ m_Ped, -1, 0, 0, 0, 1 };
    }
    case TASK_COMPLEX_KILL_PED_ON_FOOT: { // 0x3E8
        if (ped->m_nPedState == PEDSTATE_DRIVING && m_Ped && m_Ped->IsPlayer()) {
            const auto veh = m_Ped->m_pVehicle;
            if (veh && veh->m_vehicleAudio.m_AuSettings.VehicleAudioType == eAEVehicleSoundType::AE_BOAT) { // 0x1B8 == 3
                AudioEngine.SayPedless(AE_SPEECH_PED, CTX_GLOBAL_POLICE_BOAT, ped, 0, 1.0f, false, false, false);
            } else if (m_Ped->physicalFlags.bSubmergedInWater) {
                AudioEngine.SayPedless(AE_SPEECH_PED, CTX_GLOBAL_POLICE_OVERBOARD, ped, 0, 1.0f, false, false, false);
            }
        }
        if (ped->m_nPedState != PEDSTATE_DRIVING || !m_Ped || !m_Ped->IsPlayer()) {
            return m_pSubTask;
        }
        if (FindPlayerWanted()->GetWantedLevel() != eWantedLevel::WANTED_CLEAN) {
            return m_pSubTask;
        }
        if (!m_pSubTask->MakeAbortable(ped)) {
            return m_pSubTask;
        }
        return new CTaskSimpleStandStill{ 0, true, false, 8.0f };
    }
    default:
        return m_pSubTask;
    }
}
