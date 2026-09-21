#include "StdInc.h"

#include "TaskComplexGoToPointAiming.h"
#include "TaskComplexGoToPointAndStandStill.h"
#include "TaskSimpleGunControl.h"
#include "TaskSimpleUseGun.h"

void CTaskComplexGoToPointAiming::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexGoToPointAiming, 0x86fe00, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x668790);
    RH_ScopedInstall(Destructor, 0x668870);
    RH_ScopedInstall(CreateSubTask, 0x6688D0);

    RH_ScopedVMTInstall(Clone, 0x66CD80);
    RH_ScopedVMTInstall(GetTaskType, 0x668860);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x66DD70);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x66DDB0);
    RH_ScopedVMTInstall(ControlSubTask, 0x6689E0);
}

// 0x668790
CTaskComplexGoToPointAiming::CTaskComplexGoToPointAiming(
    eMoveState     moveState,
    const CVector& movePos,
    CEntity*       aimAtEntity,
    CVector        aimPos,
    const float    targetRadius,
    const float    slowDownDist
) :
    m_aimPos{ aimPos },
    m_aimAtEntity{ aimAtEntity },
    m_moveState{ moveState },
    m_movePos{ movePos },
    m_moveTargetRadius{ targetRadius },
    m_slowDownDistance{ slowDownDist } {
    CEntity::SafeRegisterRef(m_aimAtEntity);
}

CTaskComplexGoToPointAiming::CTaskComplexGoToPointAiming(const CTaskComplexGoToPointAiming& o) :
    CTaskComplexGoToPointAiming{
        o.m_moveState,
        o.m_movePos,
        o.m_aimAtEntity,
        o.m_aimPos,
        o.m_moveTargetRadius,
        o.m_slowDownDistance
    } {
}

// 0x668870
CTaskComplexGoToPointAiming::~CTaskComplexGoToPointAiming() {
    CEntity::SafeCleanUpRef(m_aimAtEntity);
}

// 0x6688D0
CTask* CTaskComplexGoToPointAiming::CreateSubTask(eTaskType taskType) {
    switch (taskType) {
    case TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL: {
        return new CTaskComplexGoToPointAndStandStill{
            m_moveState,
            m_movePos,
            m_moveTargetRadius,
            m_slowDownDistance,
            false,
            false
        };
    }
    case TASK_SIMPLE_GUN_CTRL: {
        return new CTaskSimpleGunControl{
            m_aimAtEntity,
            m_aimPos,
            CVector{},
            GetTaskType() == TASK_COMPLEX_GO_TO_POINT_SHOOTING ? eGunCommand::FIREBURST : eGunCommand::NONE,
            1,
            600'000
        };
    }
    default:
        return nullptr;
    }
}

// 0x66DD70
CTask* CTaskComplexGoToPointAiming::CreateNextSubTask(CPed* ped) {
    switch (m_pSubTask->GetTaskType()) {
    case TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL:
    case TASK_SIMPLE_GUN_CTRL:
        return CreateSubTask(TASK_FINISHED);
    default:
        NOTSA_UNREACHABLE();
    }
}

// 0x66DDB0
CTask* CTaskComplexGoToPointAiming::CreateFirstSubTask(CPed* ped) {
    m_newTargetSet         = false;

    const auto* weaponInfo = CWeaponInfo::GetWeaponInfo(ped->GetActiveWeapon().m_Type, ped->GetWeaponSkill());

    if (weaponInfo->flags.bAimWithArm) {
        if (auto* secondaryTask = ped->GetTaskManager().GetTaskSecondary(TASK_SECONDARY_ATTACK)) {
            secondaryTask->MakeAbortable(ped, ABORT_PRIORITY_URGENT, nullptr);
        } else {
            const bool isShooting = GetTaskType() == TASK_COMPLEX_GO_TO_POINT_SHOOTING;
            ped->GetTaskManager().SetTaskSecondary(
                new CTaskSimpleUseGun{
                    m_aimAtEntity,
                    m_aimPos,
                    isShooting ? eGunCommand::FIREBURST : eGunCommand::AIM,
                    (uint16)(isShooting ? 3 : 1),
                    false },
                TASK_SECONDARY_ATTACK
            );
        }
    } else if (weaponInfo->flags.bCanAim
               && (weaponInfo->m_nWeaponFire == WEAPON_FIRE_INSTANT_HIT || weaponInfo->m_nWeaponFire == WEAPON_FIRE_AREA_EFFECT)) {
        return CreateSubTask(TASK_SIMPLE_GUN_CTRL);
    }

    return new CTaskComplexGoToPointAndStandStill{
        m_moveState,
        m_movePos,
        m_moveTargetRadius,
        m_slowDownDistance,
        false,
        false
    };
}

// 0x6689E0
CTask* CTaskComplexGoToPointAiming::ControlSubTask(CPed* ped) {
    const auto* weaponInfo = CWeaponInfo::GetWeaponInfo(ped->GetActiveWeapon().m_Type, ped->GetWeaponSkill());
    if (!m_newTargetSet) {
        switch (m_pSubTask->GetTaskType()) {
        case TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL: {
            static_cast<CTaskComplexGoToPointAndStandStill*>(m_pSubTask)->GoToPoint(m_movePos, 0.5f, 0.5f, false); // Binary passes [0x86FC84]=0.5, [0x86FC88]=0.5
            if (weaponInfo->flags.bAimWithArm) {
                if (auto* secondaryTask = ped->GetTaskManager().GetTaskSecondary(TASK_SECONDARY_ATTACK)) {
                    if (auto* useGun = ped->GetIntelligence()->GetTaskUseGun()) {
                        if (GetTaskType() == TASK_COMPLEX_GO_TO_POINT_SHOOTING && CGeneral::GetRandomNumber() % 64 == 0) {
                            useGun->ControlGun(ped, m_aimAtEntity, eGunCommand::FIRE);
                        } else {
                            useGun->ControlGun(ped, m_aimAtEntity, eGunCommand::AIM);
                        }
                    } else {
                        secondaryTask->MakeAbortable(ped, ABORT_PRIORITY_URGENT, nullptr);
                    }
                } else {
                    ped->GetTaskManager().SetTaskSecondary(
                        new CTaskSimpleUseGun{ m_aimAtEntity, m_aimPos, eGunCommand::AIM, 1, false },
                        TASK_SECONDARY_ATTACK
                    );
                }
            }
            break;
        }
        case TASK_SIMPLE_GUN_CTRL: {
            if (auto* useGun = ped->GetIntelligence()->GetTaskUseGun()) {
                useGun->ControlGunMove(ped->GetMoveSpeed());
            }
            break;
        }
        default:
            break;
        }
    } else {
        return CTaskComplex::ControlSubTask(ped);
    }
    return m_pSubTask;
}
