#include "StdInc.h"
#include "TaskComplexFleeShooting.h"
#include "TaskSimpleUseGun.h"
#include "TaskSimpleGunControl.h"

void CTaskComplexFleeShooting::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexFleeShooting, 0x86f7c4, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x65C900);
    RH_ScopedInstall(Destructor, 0x65C9A0);

    RH_ScopedVMTInstall(Clone, 0x65CFB0);
    RH_ScopedVMTInstall(GetTaskType, 0x65C990);
    RH_ScopedVMTInstall(MakeAbortable, 0x65CA00);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x65CA60);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x65CA70);
    RH_ScopedVMTInstall(ControlSubTask, 0x65CA90);
}

// 0x65C900
CTaskComplexFleeShooting::CTaskComplexFleeShooting(
    CEntity* entity,
    bool pedScream,
    float safeDist,
    int32 fleeTime,
    int32 shootTime,
    int32 shootRecoverTime,
    int32 posCheckPeriod,
    float posChangeTolerance
) :
    CTaskComplexSmartFleeEntity{ entity, pedScream, safeDist, fleeTime, posCheckPeriod, posChangeTolerance},
    m_shootRecoverTime{shootRecoverTime},
    m_shootTime{shootTime}
{
    /* Entity ref already registered in SmartFlee */
}

CTaskComplexFleeShooting::CTaskComplexFleeShooting(const CTaskComplexFleeShooting& o) :
    CTaskComplexFleeShooting{ o.fleeFrom, o.m_pedScream, o.m_safeDistance, o.m_time, o.m_shootTime, o.m_shootRecoverTime, o.m_posCheckPeriod, o.m_posChangeTolerance }
{
}

// 0x65C9A0
CTaskComplexFleeShooting::~CTaskComplexFleeShooting() {
    /* Entity ref already cleaned up in SmartFlee */
}

// 0x65CA00
bool CTaskComplexFleeShooting::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
    if (const auto useGun = CTask::DynCast<CTaskSimpleUseGun>(ped->GetTaskManager().GetTaskSecondary(TASK_SECONDARY_ATTACK))) {
        useGun->MakeAbortable(ped, priority, event);
    }
    return m_pSubTask->MakeAbortable(ped, priority, event);
}

// 0x65CA70
CTask* CTaskComplexFleeShooting::CreateFirstSubTask(CPed* ped) {
    m_gunTimer.SetAsOutOfTime();
    return CTaskComplexSmartFleeEntity::CreateFirstSubTask(ped);
}

// 0x65CA90
CTask* CTaskComplexFleeShooting::ControlSubTask(CPed* ped) {
    if (const auto attack = ped->GetTaskManager().GetTaskSecondary(TASK_SECONDARY_ATTACK)) {
        if (const auto useGun = CTask::DynCast<CTaskSimpleUseGun>(attack)) {
            if (m_gunTimer.IsOutOfTime()) {
                if (useGun->GetLastGunCommand() == eGunCommand::FIREBURST) {
                    m_gunTimer.Start(m_shootRecoverTime);

                    useGun->ControlGun(ped, fleeFrom, eGunCommand::END_LEISURE);
                }
            } else {
                useGun->ControlGun(ped, fleeFrom, eGunCommand::FIREBURST);
            }
        }
    } else {
        m_moveState = PEDMOVE_SPRINT;
        if (m_gunTimer.IsOutOfTime()) {
            if (ped->GetActiveWeapon().GetWeaponInfo(ped).flags.bAimWithArm) {
                m_gunTimer.Start(m_shootTime);

                ped->GetTaskManager().SetTaskSecondary(
                    new CTaskSimpleUseGun{ fleeFrom, {}, eGunCommand::FIREBURST, 3 },
                    TASK_SECONDARY_ATTACK
                );
            }
        }
    }

    return CTaskComplexSmartFleeEntity::ControlSubTask(ped);
}
