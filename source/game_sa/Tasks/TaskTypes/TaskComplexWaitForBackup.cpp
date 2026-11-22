#include "StdInc.h"
#include "TaskComplexWaitForBackup.h"
#include "TaskSimpleGunControl.h"
#include "TaskSimpleAchieveHeading.h"
#include "TaskSimplePause.h"
#include "TaskComplexLeaveCar.h"

void CTaskComplexWaitForBackup::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexWaitForBackup, 0x870da4, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x697290);
    RH_ScopedInstall(Destructor, 0x697330);

    RH_ScopedInstall(CreateSubTask, 0x6973B0);

    RH_ScopedVMTInstall(Clone, 0x697740);
    RH_ScopedVMTInstall(GetTaskType, 0x697320);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x697F10);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x6980C0);
    RH_ScopedVMTInstall(ControlSubTask, 0x6973A0);
}

// 0x697290
CTaskComplexWaitForBackup::CTaskComplexWaitForBackup(CPed* aggressor, CPed* backup, float waitDist) :
    m_backup{backup},
    m_aggressor{aggressor},
    m_waitDistance{waitDist}
{
    CEntity::SafeRegisterRef(m_backup);
    CEntity::SafeRegisterRef(m_aggressor);
}

CTaskComplexWaitForBackup::CTaskComplexWaitForBackup(const CTaskComplexWaitForBackup& o) :
    CTaskComplexWaitForBackup{o.m_aggressor, o.m_backup, o.m_waitDistance}
{
}

// 0x697330
CTaskComplexWaitForBackup::~CTaskComplexWaitForBackup() {
    CEntity::SafeCleanUpRef(m_backup);
    CEntity::SafeCleanUpRef(m_aggressor);
}

// 0x6973B0
CTask* CTaskComplexWaitForBackup::CreateSubTask(eTaskType taskType, CPed* ped) {
    switch (taskType) {
    case TASK_SIMPLE_GUN_CTRL:
        return new CTaskSimpleGunControl{ m_aggressor, {}, {}, eGunCommand::AIM, 100, 2000 };
    case TASK_SIMPLE_ACHIEVE_HEADING: {
        ped->m_fAimingRotation = (m_aggressor->GetPosition() - ped->GetPosition()).Heading();
        return new CTaskSimpleAchieveHeading{ ped->m_fAimingRotation, 0.5f, 0.2f };
    }
    case TASK_SIMPLE_PAUSE:
        return new CTaskSimplePause{ ped->IsInVehicle() ? CGeneral::GetRandomNumberInRange(1000, 2000) : 0 };
    case TASK_COMPLEX_LEAVE_CAR:
        return new CTaskComplexLeaveCar{ ped->m_pVehicle, TARGET_DOOR_FRONT_LEFT, 0, true, false };
    case TASK_FINISHED:
        return nullptr;
    default:
        NOTSA_UNREACHABLE();
    }
}

// 0x697F10
CTask* CTaskComplexWaitForBackup::CreateNextSubTask(CPed* ped) {
    switch (m_pSubTask->GetTaskType()) {
    case TASK_SIMPLE_GUN_CTRL: {
        if (m_backup && m_backup->IsAlive() && !m_timer.IsOutOfTime()) {
            if (CVector2D{ ped->GetPosition() - m_backup->GetPosition() }.SquaredMagnitude() <= sq(m_waitDistance)) {
                return CreateSubTask(TASK_SIMPLE_ACHIEVE_HEADING, ped);
            }
        }
        return CreateSubTask(TASK_FINISHED, ped);
    }
    case TASK_SIMPLE_ACHIEVE_HEADING: {
        const auto& winfo = ped->GetActiveWeapon().GetWeaponInfo();
        return CreateSubTask(
            winfo.m_nWeaponFire == eWeaponFire::WEAPON_FIRE_INSTANT_HIT
                ? TASK_SIMPLE_GUN_CTRL
                : TASK_FINISHED,
            ped
        );
    }
    case TASK_COMPLEX_LEAVE_CAR:
        return CreateSubTask(TASK_SIMPLE_ACHIEVE_HEADING, ped);
    case TASK_SIMPLE_PAUSE:
        return CreateSubTask(
            ped->IsInVehicle()
                ? TASK_COMPLEX_LEAVE_CAR
                : TASK_SIMPLE_ACHIEVE_HEADING,
            ped
        );
    default:
        NOTSA_UNREACHABLE();
    }
}

// 0x6980C0
CTask* CTaskComplexWaitForBackup::CreateFirstSubTask(CPed* ped) {
    m_timer.Start(20'000);
    return CreateSubTask(TASK_SIMPLE_PAUSE, ped);
}
