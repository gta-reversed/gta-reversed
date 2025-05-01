#include "StdInc.h"

#include "TaskComplexUseGoggles.h"

#include "TaskSimpleRunAnim.h"
#include "TaskSimplePlayerOnFoot.h"

// 0x634F40
CTask* CTaskComplexUseGoggles::CreateNextSubTask(CPed* ped) {
    switch (m_pSubTask->GetTaskType()) {
    case TASK_SIMPLE_GOGGLES_OFF:
        ped->TakeOffGoggles();
        return nullptr;
    case TASK_SIMPLE_GOGGLES_ON:
        ped->PutOnGoggles();
        ped->GetWeaponAE().AddAudioEvent(AE_WEAPON_FIRE);
        return nullptr;
    default:
        return nullptr;
    }
}

// 0x634F90
CTask* CTaskComplexUseGoggles::CreateFirstSubTask(CPed* ped) {
    ped->m_pPlayerData->m_bDontAllowWeaponChange = true;

    const char* taskName = ped->m_pGogglesObject ? "GogglesOff" : "GogglesOn";
    const eTaskType taskType = ped->m_pGogglesObject ? TASK_SIMPLE_GOGGLES_OFF : TASK_SIMPLE_GOGGLES_ON;

    return new CTaskSimpleRunAnim(ANIM_GROUP_GOGGLES, ANIM_ID_FIRE, 4.0f, taskType, taskName, false);
}

// 0x635050
CTask* CTaskComplexUseGoggles::ControlSubTask(CPed* ped) {
    if (ped->m_pPlayerData) {
        ped->m_pPlayerData->m_bDontAllowWeaponChange = true; // FIX_BUGS: V595 The 'ped->m_pPlayerData' pointer was utilized before it was verified against nullptr.
        ped->m_pPlayerData->m_bPlayerSprintDisabled = true;
        ped->m_pPlayerData->m_fTimeCanRun = std::max(ped->m_pPlayerData->m_fTimeCanRun, 0.0f);

        auto* task = ped->GetIntelligence()->FindTaskByType(TASK_SIMPLE_PLAYER_ON_FOOT);
        if (task)
            static_cast<CTaskSimplePlayerOnFoot*>(task)->PlayerControlZelda(ped->AsPlayer(), true);

        ped->m_pPlayerData->m_bPlayerSprintDisabled = false;
    }

    return m_pSubTask;
}

void CTaskComplexUseGoggles::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexUseGoggles, 0x86E4A8, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedVMTInstall(Clone, 0x637060);
    RH_ScopedVMTInstall(GetTaskType, 0x634F10);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x634F40);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x634F90);
    RH_ScopedVMTInstall(ControlSubTask, 0x635050);
};

void TaskComplexUseGogglesTestCode() {
    CStreaming::RequestModel(MODEL_NVGOGGLES, STREAMING_GAME_REQUIRED);
    CPlayerPed* player = FindPlayerPed();
    player->GiveWeapon(WEAPON_NIGHTVISION, 1, true);
}
