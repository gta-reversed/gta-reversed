#include "StdInc.h"

#include "TaskComplexDestroyCarArmed.h"
#include "TaskSimplePause.h"
#include <TaskComplexGoToPointAndStandStill.h>

void CTaskComplexDestroyCarArmed::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexDestroyCarArmed, 0x86d9c4, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x621F50);
    RH_ScopedInstall(Destructor, 0x622010);

    RH_ScopedInstall(CalculateSearchPositionAndRanges, 0x628C80, { .State = HS::RedirectToGTA, .Locked = true });
    RH_ScopedInstall(CreateSubTask, 0x628DA0, { .State = HS::RedirectToGTA, .Locked = true });

    RH_ScopedVMTInstall(Clone, 0x623600);
    RH_ScopedVMTInstall(GetTaskType, 0x622000);
    RH_ScopedVMTInstall(MakeAbortable, 0x622070);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x62DF20);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x62E0A0);
    RH_ScopedVMTInstall(ControlSubTask, 0x628FA0);
}

// 0x621F50
CTaskComplexDestroyCarArmed::CTaskComplexDestroyCarArmed(CVehicle* vehicleToDestroy, uint32 a3, uint32 a4, uint32 a5) : 
    ctor58{ a3 },
    ctor5C{ a4 },
    ctor60{ a5 },
    m_VehToDestroy{vehicleToDestroy},
    m_bGotoPointAndStandStill{false}
{
    CEntity::SafeRegisterRef(m_VehToDestroy);
}

CTaskComplexDestroyCarArmed::CTaskComplexDestroyCarArmed(const CTaskComplexDestroyCarArmed& o) :
    CTaskComplexDestroyCarArmed{o.m_VehToDestroy, o.ctor58, o.ctor5C, o.ctor60}
{
}

// 0x622010
CTaskComplexDestroyCarArmed::~CTaskComplexDestroyCarArmed() {
    CEntity::SafeCleanUpRef(m_VehToDestroy);
}

// 0x628C80
void CTaskComplexDestroyCarArmed::CalculateSearchPositionAndRanges(CPed* ped) {
    m_PedPos = ped->GetPosition();
    m_VehiclePos = m_VehToDestroy->GetPosition();
    m_PedToVehDirUnnorm = m_VehiclePos - m_PedPos;
    m_PedVehDist = m_PedToVehDirUnnorm.Magnitude();

    const auto& weapon = ped->GetActiveWeapon();
    const auto& winfo  = weapon.GetWeaponInfo(ped);
    m_fWeaponRange = winfo.m_fWeaponRange / 2.f;
    m_fWeaponRangeClamped = std::max(0.8f, m_fWeaponRange * 0.9f);
    m_fWeaponRange *= 1.1f; // = winfo.m_fWeaponRange * 0.55f
}

// 0x628DA0
CTask* CTaskComplexDestroyCarArmed::CreateSubTask(eTaskType taskType, CPed* ped) {
    NOTSA_UNREACHABLE("Redirected code can't be reached");
    return nullptr;
    /* Redirected */

    /*
    * Missing stubs for the 3 of tasks below
    * Other than that, it should be correct
    switch (taskType) {
    case TASK_SIMPLE_GUN_CTRL: {
        const auto& winfo = ped->GetActiveWeapon().GetWeaponInfo(ped);
        if (winfo.flags.bThrow) {
            return new CTaskSimpleThrowControl{ m_VehToDestroy, false };
        }
        return new CTaskSimpleGunControl{ m_VehToDestroy, 0, 0, 3, 5, 5000 };
    }
    case TASK_COMPLEX_SEEK_ENTITY: {
        return new CTaskComplexSeekEntity<CEntitySeekPosCalculatorStandard>{
            m_VehToDestroy,
            50000,
            1000,
            1.0,
            2.0,
            2.0,
            1,
            1
        };
    }
    case TASK_SIMPLE_PAUSE: {
        return new CTaskSimplePause{ 100 };
    }
    case TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL: {
        return new CTaskComplexGoToPointAndStandStill{
            PEDMOVE_RUN,
            m_VehiclePos,
            0.5f,
            2.0f,
            false,
            false
        };
    }
    }
    */
}

// 0x622070
bool CTaskComplexDestroyCarArmed::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
    switch (priority) {
    case ABORT_PRIORITY_URGENT:
    case ABORT_PRIORITY_IMMEDIATE:
        return m_pSubTask->MakeAbortable(ped, priority, event);
    default:
        return false;
    }
}

// 0x62DF20
CTask* CTaskComplexDestroyCarArmed::CreateNextSubTask(CPed* ped) {
    if (!m_VehToDestroy) {
        return nullptr;
    }

    switch (m_pSubTask->GetTaskType()) {
    case TASK_SIMPLE_GUN_CTRL:
    case TASK_SIMPLE_THROW_CTRL: {
        if (!ped->bStayInSamePlace) {
            CalculateSearchPositionAndRanges(ped);
            return CreateSubTask(TASK_COMPLEX_SEEK_ENTITY, ped);
        }
        return CreateSubTask(TASK_SIMPLE_PAUSE, ped);
    }
    case TASK_COMPLEX_SEEK_ENTITY:
    case TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL: {
        return CreateSubTask(TASK_SIMPLE_GUN_CTRL, ped);
    }
    case TASK_SIMPLE_PAUSE: {
        if (!ped->bStayInSamePlace) {
            CalculateSearchPositionAndRanges(ped);
            return CreateSubTask(TASK_COMPLEX_SEEK_ENTITY, ped);
        }

        const auto& winfo = ped->GetActiveWeapon().GetWeaponInfo(ped);
        if (DistanceBetweenPointsSquared(m_VehToDestroy->GetPosition(), ped->GetPosition()) >= sq(winfo.m_fTargetRange)) {
            return CreateSubTask(TASK_SIMPLE_PAUSE, ped);
        }
        return CreateSubTask(TASK_SIMPLE_GUN_CTRL, ped);
    }
    default:
        return nullptr;
    }
}

// 0x62E0A0
CTask* CTaskComplexDestroyCarArmed::CreateFirstSubTask(CPed* ped) {
    if (!m_VehToDestroy) {
        return nullptr;
    }

    m_bNeedsToCreatefirstSubTask = false;

    // Dead code:
    // const auto& winfo = ped->GetActiveWeapon().GetWeaponInfo(ped);
    CalculateSearchPositionAndRanges(ped);

    if (m_PedVehDist <= m_fWeaponRangeClamped) {
        ped->m_fAimingRotation = CGeneral::GetRadianAngleBetweenPoints(m_VehiclePos, m_PedPos);
        return CreateSubTask(TASK_SIMPLE_GUN_CTRL, ped);
    } else if (ped->bStayInSamePlace) {
        return CreateSubTask(TASK_SIMPLE_PAUSE, ped);
    } else if (m_bGotoPointAndStandStill) {
        return CreateSubTask(TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL, ped);
    } else {
        return CreateSubTask(TASK_COMPLEX_SEEK_ENTITY, ped);
    }
}

// 0x628FA0
CTask* CTaskComplexDestroyCarArmed::ControlSubTask(CPed* ped) {
    if (!m_VehToDestroy) {
        return nullptr;
    }

    if (m_bNeedsToCreatefirstSubTask) {
        return CreateFirstSubTask(ped);
    }

    if (m_VehToDestroy->m_fHealth <= 0.f) {
        return nullptr;
    }

    // TODO: Refactor (Most of the code is common, just find a way to get rid of the jump)
    switch (m_pSubTask->GetTaskType()) {
    case TASK_COMPLEX_SEEK_ENTITY: {
        CalculateSearchPositionAndRanges(ped);
        if (m_PedVehDist >= m_fWeaponRange && !ped->bKindaStayInSamePlace) {
            return m_pSubTask;
        }
        ped->SetMoveState(PEDMOVE_STILL);
        return CreateSubTask(TASK_SIMPLE_GUN_CTRL, ped);
    }
    case TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL: {
        CalculateSearchPositionAndRanges(ped);
        if (m_PedVehDist < dword4C) {
            ped->SetMoveState(PEDMOVE_STILL);
            return CreateSubTask(TASK_SIMPLE_GUN_CTRL, ped);
        }
        if (m_PedVehDist >= m_fWeaponRange && !ped->bKindaStayInSamePlace) {
            return m_pSubTask;
        }
        ped->SetMoveState(PEDMOVE_STILL);
        return CreateSubTask(TASK_SIMPLE_GUN_CTRL, ped);
    }
    default: {
        return m_pSubTask;
    }
    }
}
