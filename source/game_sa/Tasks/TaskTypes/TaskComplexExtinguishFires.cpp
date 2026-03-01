#include "StdInc.h"
#include "TaskComplexExtinguishFires.h"
#include "TaskSimpleStandStill.h"
#include "TaskComplexGoToPointAndStandStill.h"
#include "TaskSimpleGunControl.h"
#include "FireManager.h"

void CTaskComplexExtinguishFires::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexExtinguishFires, 0x870dd0, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x6975C0);
    RH_ScopedInstall(Destructor, 0x6975F0);

    RH_ScopedInstall(GetNearestFire, 0x697600);

    RH_ScopedVMTInstall(Clone, 0x6977B0);
    RH_ScopedVMTInstall(GetTaskType, 0x6975E0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x698110);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x698290);
    RH_ScopedVMTInstall(ControlSubTask, 0x698330);
}

CTaskComplexExtinguishFires::CTaskComplexExtinguishFires(const CTaskComplexExtinguishFires& o) :
    CTaskComplexExtinguishFires{}
{
}

// 0x697600
CFire* CTaskComplexExtinguishFires::GetNearestFire(CPed* ped) {
    const auto& pedPos = ped->GetPosition();
    if (const auto fire = gFireManager.FindNearestFire(pedPos)) {
        if ((fire->GetPosition() - pedPos).SquaredMagnitude() <= sq(20.f)) {
            return fire;
        }
    }
    return nullptr;
}

// 0x698110
CTask* CTaskComplexExtinguishFires::CreateNextSubTask(CPed* ped) {
    const auto nearestFireNow = GetNearestFire(ped);

    switch (m_pSubTask->GetTaskType()) {
    case TASK_SIMPLE_STAND_STILL:
    case TASK_SIMPLE_GUN_CTRL: {
        if (nearestFireNow == m_fireToExt) {
            nearestFireNow->Extinguish();
            m_fireToExt = nullptr;
        }
        return CreateFirstSubTask(ped);
    }
    case TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL: {
        if (nearestFireNow != m_fireToExt) {
            return CreateFirstSubTask(ped);
        }

        if (ped->GetActiveWeapon().m_Type == WEAPON_EXTINGUISHER) { // Inverted
            return new CTaskSimpleGunControl{ nullptr, nearestFireNow->GetPosition(), {}, eGunCommand::PISTOLWHIP, 1, -1 };
        } else {
            return new CTaskSimpleStandStill{ 2000 };
        }
    }
    default:
        NOTSA_UNREACHABLE();
    }
}

// 0x698290
CTask* CTaskComplexExtinguishFires::CreateFirstSubTask(CPed* ped) {
    if (m_fireToExt = GetNearestFire(ped)) {
        return new CTaskComplexGoToPointAndStandStill{ PEDMOVE_RUN, m_fireToExt->GetPosition(), 1.5f, 2.f, false, false };
    }
    return nullptr;
}

// 0x698330
CTask* CTaskComplexExtinguishFires::ControlSubTask(CPed* ped) {
    const auto nearestFireNow = GetNearestFire(ped);

    switch (m_pSubTask->GetTaskType()) {
    case TASK_SIMPLE_GUN_CTRL: {
        if (nearestFireNow != m_fireToExt) {
            if (m_pSubTask->MakeAbortable(ped)) {
                return CreateFirstSubTask(ped);
            }
        }
        break;
    }
    case TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL: {
        if (!nearestFireNow) {
            return nullptr;
        }
        if (m_fireToExt != nearestFireNow) {
            static_cast<CTaskComplexGoToPointAndStandStill*>(m_pSubTask)->GoToPoint(nearestFireNow->GetPosition());
        }
        break;
    }
    case TASK_SIMPLE_STAND_STILL:
        break;
    default:
        NOTSA_UNREACHABLE();
    }
    return m_pSubTask;
}
