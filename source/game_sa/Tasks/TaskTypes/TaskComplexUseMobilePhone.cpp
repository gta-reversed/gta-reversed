#include "StdInc.h"
#include "TaskComplexUseMobilePhone.h"
#include "TaskSimpleRunAnim.h"
#include "TaskSimplePause.h"
#include "TaskSimpleStandStill.h"
#include "TaskSimplePlayerOnFoot.h"
#include "EventOnFire.h"

// 0x6348A0
CTaskComplexUseMobilePhone::CTaskComplexUseMobilePhone(int32 duration) :
    m_Duration{ duration } {
}

// 0x636FD0
CTaskComplexUseMobilePhone::CTaskComplexUseMobilePhone(const CTaskComplexUseMobilePhone& o) :
    CTaskComplexUseMobilePhone{ o.m_Duration } {
}

// 0x6348F0
void CTaskComplexUseMobilePhone::RemovePhoneModel(CPed* ped) {
    ped->RemoveWeaponModel(MODEL_CELLPHONE);
    ped->SetCurrentWeapon(ped->GetSavedWeapon());
    ped->SetSavedWeapon(WEAPON_UNIDENTIFIED);
    if (auto* const playerData = ped->GetPlayerData()) {
        playerData->m_bDontAllowWeaponChange = false;
    }
}

// 0x634A40
int8 CTaskComplexUseMobilePhone::Stop(CPed* ped) {
    if (m_IsQuitting) {
        return true;
    }
    m_IsQuitting = true;
    return MakeAbortable(ped, ABORT_PRIORITY_LEISURE);
}

// 0x634930
bool CTaskComplexUseMobilePhone::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    const auto StopUsingPhone = [ped] {
        ped->bUsingMobilePhone    = false;
        ped->bDontAcceptIKLookAts = false;
    };

    if (priority == ABORT_PRIORITY_IMMEDIATE) { // 0x634943
        const bool aborted = m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_IMMEDIATE, event);
        if (aborted) {
            StopUsingPhone();
            if (ped->GetSavedWeapon() != WEAPON_UNIDENTIFIED) {
                RemovePhoneModel(ped);
            }
        }
        return aborted;
    }

    if (notsa::isa<CTaskSimplePause>(m_pSubTask)) { // 0x63499E
        StopUsingPhone();
        return true;
    }

    if (event) { // 0x6349DA
        if (notsa::isa<CEventOnFire>(event) && ped->IsPlayer()) {
            return true;
        }
        m_IsAborting = true;
    }

    m_Timer.Start(m_Duration);

    if (m_pSubTask->GetTaskType() == TASK_SIMPLE_PHONE_OUT) { // 0x634A1E - NOTE: A `CTaskSimpleRunAnim` with a custom type, no `isa` possible
        m_pSubTask->MakeAbortable(ped, priority, event);
    }
    return false;
}

// 0x634A60
CTask* CTaskComplexUseMobilePhone::CreateNextSubTask(CPed* ped) {
    const auto StopUsingPhone = [ped] {
        ped->bUsingMobilePhone    = false;
        ped->bDontAcceptIKLookAts = false;
    };

    switch (const auto tt = m_pSubTask->GetTaskType()) {
    case TASK_SIMPLE_PHONE_TALK: // 0x634AA2
        return new CTaskSimpleRunAnim{ ANIM_GROUP_DEFAULT, ANIM_ID_PHONE_OUT, 4.f, TASK_SIMPLE_PHONE_OUT, "PhoneOut", false };
    case TASK_SIMPLE_PHONE_IN: // 0x634B0F
        m_Timer.Start(m_Duration);
        return new CTaskSimpleRunAnim{ ANIM_GROUP_DEFAULT, ANIM_ID_PHONE_TALK, 4.f, TASK_SIMPLE_PHONE_TALK, "PhoneChat", false };
    case TASK_SIMPLE_PAUSE: // 0x634AE9
        m_IsAborting = false;
        m_Timer.Stop();
        if (!m_IsQuitting) {
            return CreateFirstSubTask(ped);
        }
        StopUsingPhone();
        return nullptr;
    case TASK_SIMPLE_PHONE_OUT: // 0x634B71
        if (!m_IsQuitting && m_IsAborting) {
            return new CTaskSimplePause{ 1'000 };
        }
        StopUsingPhone();
        return nullptr;
    default:
        NOTSA_UNREACHABLE_CASE(tt);
    }
}

// 0x634C10
CTask* CTaskComplexUseMobilePhone::CreateFirstSubTask(CPed* ped) {
    ped->bUsingMobilePhone    = true;
    ped->bDontAcceptIKLookAts = true;
    if (ped->IsPlayer()) {
        ped->AsPlayer()->ClearWeaponTarget();
    }
    if (ped->GetSavedWeapon() == WEAPON_UNIDENTIFIED) { // 0x634C6E
        ped->SetSavedWeapon(ped->GetActiveWeapon().m_Type);
        ped->SetCurrentWeapon(WEAPON_UNARMED);
    }
    if (auto* const playerData = ped->GetPlayerData()) {
        playerData->m_bDontAllowWeaponChange = true;
    }
    if (ped->IsPlayer()) {
        TheCamera.ClearPlayerWeaponMode();
    }
    CTaskSimpleStandStill{}.ProcessPed(ped);
    return new CTaskSimpleRunAnim{ ANIM_GROUP_DEFAULT, ANIM_ID_PHONE_IN, 4.f, TASK_SIMPLE_PHONE_IN, "PhoneIn", false };
}

// 0x634D60
CTask* CTaskComplexUseMobilePhone::ControlSubTask(CPed* ped) {
    if (auto* const playerData = ped->GetPlayerData()) {
        playerData->m_bDontAllowWeaponChange = true;
    }

    CTask* subTask = m_pSubTask;
    if (!notsa::isa<CTaskSimplePause>(m_pSubTask)) { // 0x634DA6
        if (const auto* const phoneIn = RpAnimBlendClumpGetAssociation(ped->GetRpClump(), ANIM_ID_PHONE_IN)) {
            if (phoneIn->GetCurrentTime() >= 0.85f) { // 0x634DE1
                ped->AddWeaponModel(MODEL_CELLPHONE);
            }
        } else if (const auto* const phoneOut = RpAnimBlendClumpGetAssociation(ped->GetRpClump(), ANIM_ID_PHONE_OUT)) {
            if (phoneOut->GetCurrentTime() >= 1.566f && phoneOut->GetCurrentTime() - phoneOut->m_TimeStep < 1.566f) { // 0x634E19
                RemovePhoneModel(ped);
            }
        } else if (m_Timer.IsOutOfTime()) { // 0x634E2F
            subTask = new CTaskSimpleRunAnim{ ANIM_GROUP_DEFAULT, ANIM_ID_PHONE_OUT, 4.f, TASK_SIMPLE_PHONE_OUT, "PhoneOut", false };
        }
    }

    if (auto* const playerData = ped->GetPlayerData()) { // 0x634E80
        playerData->m_bPlayerSprintDisabled = true;
        if (playerData->m_fTimeCanRun < 0.0f) {
            playerData->m_fTimeCanRun = 0.0f;
        }
        if (auto* const tOnFoot = ped->GetTaskManager().Find<CTaskSimplePlayerOnFoot>(false)) {
            tOnFoot->PlayerControlZelda(ped->AsPlayer(), true);
        }
        playerData->m_bPlayerSprintDisabled = false;
    }

    return subTask;
}

void CTaskComplexUseMobilePhone::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexUseMobilePhone, 0x86e454, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedConstructorInstall(0x6348A0, "1", {}, int32);
    RH_ScopedDestructorInstall(0x6348E0);

    RH_ScopedInstall(RemovePhoneModel, 0x6348F0);
    RH_ScopedInstall(Stop, 0x634A40);
    RH_ScopedVMTInstall(Clone, 0x636FD0);
    RH_ScopedVMTInstall(GetTaskType, 0x6348D0);
    RH_ScopedVMTInstall(MakeAbortable, 0x634930);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x634A60);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x634C10);
    RH_ScopedVMTInstall(ControlSubTask, 0x634D60);
}
