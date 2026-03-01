#include "StdInc.h"

#include "TaskComplexSeekEntityAiming.h"

#include "SeekEntity/TaskComplexSeekEntity.h"
#include "TaskSimpleUseGun.h"

void CTaskComplexSeekEntityAiming::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexSeekEntityAiming, 0x870c94, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x694B90);
    RH_ScopedInstall(Destructor, 0x694C10);

    RH_ScopedVMTInstall(Clone, 0x695820);
    RH_ScopedVMTInstall(GetTaskType, 0x694C00);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x695C20);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x695D30);
    RH_ScopedVMTInstall(ControlSubTask, 0x694C70);
}

// 0x694B90
CTaskComplexSeekEntityAiming::CTaskComplexSeekEntityAiming(CEntity* toSeek, float seekRadius, float aimRadius) :
    m_ToSeek{toSeek},
    m_SeekRadius{seekRadius},
    m_AimRadius{aimRadius}
{
    CEntity::SafeRegisterRef(m_ToSeek);
}

// 0x695820
CTaskComplexSeekEntityAiming::CTaskComplexSeekEntityAiming(const CTaskComplexSeekEntityAiming& o) :
    CTaskComplexSeekEntityAiming{o.m_ToSeek, o.m_AimRadius, o.m_AimRadius}
{
}

// 0x694C10
CTaskComplexSeekEntityAiming::~CTaskComplexSeekEntityAiming() {
    CEntity::SafeCleanUpRef(m_ToSeek);
}

// 0x695C20
CTask* CTaskComplexSeekEntityAiming::CreateNextSubTask(CPed* ped) {
    if (!m_ToSeek) {
        return nullptr;
    }
    switch (m_pSubTask->GetTaskType()) {
    case TASK_SIMPLE_STAND_STILL:
        return new CTaskComplexSeekEntity<>{
            m_ToSeek,
            50'000,
            1'000,
            m_SeekRadius,
            2.f,
            2.f,
            true,
            true
        };
    case TASK_COMPLEX_SEEK_ENTITY:
        return new CTaskSimpleStandStill{2'000};
    default:
        NOTSA_UNREACHABLE();
    }
}

// 0x695D30
CTask* CTaskComplexSeekEntityAiming::CreateFirstSubTask(CPed* ped) {
    if (!m_ToSeek) {
        return nullptr;
    }
    return new CTaskComplexSeekEntity<>{
        m_ToSeek,
        50'000,
        1'000,
        m_SeekRadius,
        2.f,
        2.f,
        true,
        true
    };
}

// 0x694C70
CTask* CTaskComplexSeekEntityAiming::ControlSubTask(CPed* ped) {
    const auto pedToSoughtDist2DSq = (ped->GetPosition() - m_ToSeek->GetPosition()).SquaredMagnitude2D();
    const auto pedUseGun           = ped->GetIntelligence()->GetTaskUseGun();
    switch (m_pSubTask->GetTaskType()) {
    case TASK_SIMPLE_STAND_STILL: {
        if (pedUseGun) {
            pedUseGun->ControlGun(ped, m_ToSeek, eGunCommand::AIM);
        }
        if (pedToSoughtDist2DSq >= sq(m_SeekRadius)) {
            m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_LEISURE);
        }
        break;
    }
    case TASK_COMPLEX_SEEK_ENTITY: {
        if (pedToSoughtDist2DSq >= sq(m_AimRadius)) {
            if (pedUseGun) {
                pedUseGun->ControlGun(ped, m_ToSeek, eGunCommand::END_LEISURE);
            }
        } else if (pedUseGun) {
            pedUseGun->ControlGun(ped, m_ToSeek, eGunCommand::AIM);
        } else {
            ped->GetTaskManager().SetTaskSecondary(
                new CTaskSimpleUseGun{
                    m_ToSeek,
                    {},
                    eGunCommand::AIM
                },
                TASK_SECONDARY_ATTACK
            );
        }
        break;
    }
    default:
        NOTSA_UNREACHABLE();
    }
    return m_pSubTask;
}
