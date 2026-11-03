#include "StdInc.h"

#include "TaskGoToVehicleAndLean.h"
#include "TaskComplexGangLeader.h"
#include "TaskComplexGoToPointAndStandStill.h"
#include "TaskSimpleAchieveHeading.h"
#include "TaskSimpleStandStill.h"
#include "TaskLeanOnVehicle.h"

void CTaskGoToVehicleAndLean::InjectHooks() {
    RH_ScopedVirtualClass(CTaskGoToVehicleAndLean, 0x86FAC8, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x660E60);
    RH_ScopedInstall(Destructor, 0x660EE0);
    RH_ScopedInstall(DoTidyUp, 0x660F60);
    RH_ScopedInstall(CalcTargetPos, 0x664770);

    RH_ScopedVMTInstall(Clone, 0x6621B0);
    RH_ScopedVMTInstall(MakeAbortable, 0x664500);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x664590);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x664D40);
    RH_ScopedVMTInstall(ControlSubTask, 0x664E60);
}

// 0x660E60
CTaskGoToVehicleAndLean::CTaskGoToVehicleAndLean(CVehicle* vehicle, int32 leanAnimDurationInMs) :
    m_Vehicle{ vehicle },
    m_LeanAnimDurationInMs{ leanAnimDurationInMs } {
    CEntity::SafeRegisterRef(m_Vehicle);
}

// 0x660EE0
CTaskGoToVehicleAndLean::~CTaskGoToVehicleAndLean() {
    if (m_AnimsReferenced) {
        CAnimManager::RemoveAnimBlockRef(CAnimManager::GetAnimationBlockIndex("gangs"));
        m_AnimsReferenced = false;
    }
    CEntity::SafeCleanUpRef(m_Vehicle);
}

// 0x664500
bool CTaskGoToVehicleAndLean::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    if (priority == ABORT_PRIORITY_IMMEDIATE) {
        m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_IMMEDIATE, event);
    } else if (!m_pSubTask->MakeAbortable(ped, priority, event)) {
        return false;
    }
    DoTidyUp(ped);
    return true;
}

// 0x664590
CTask* CTaskGoToVehicleAndLean::CreateNextSubTask(CPed* ped) {
    if (!m_Vehicle) {
        DoTidyUp(ped);
        return nullptr;
    }

    switch (m_pSubTask->GetTaskType()) {
    case TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL: { // 0x6645EB - Got to the vehicle, now face away from it
        const auto& right = m_Vehicle->GetMatrix().GetRight();
        const auto  dir   = m_LeanOnRightSide ? right : -right;

        // BUG: The result of `LimitRadianAngle` is discarded, so the heading isn't limited
        const auto heading = CGeneral::GetRadianAngleBetweenPoints(dir.x, dir.y, 0.f, 0.f);
        return new CTaskSimpleAchieveHeading{ heading };
    }
    case TASK_SIMPLE_ACHIEVE_HEADING:
    case TASK_SIMPLE_STAND_STILL: {
        // 0x66469B - Lean on it if the anims are loaded, otherwise wait for them a little
        if (!m_AnimsReferenced) {
            return new CTaskSimpleStandStill{ 500, false, false, 8.f };
        }
        return new CTaskLeanOnVehicle{ m_Vehicle, m_LeanAnimDurationInMs, 0 };
    }
    default: // Finished leaning (or something went wrong)
        DoTidyUp(ped);
        return nullptr;
    }
}

// 0x664D40
CTask* CTaskGoToVehicleAndLean::CreateFirstSubTask(CPed* ped) {
    if (!m_Vehicle || m_Vehicle->vehicleFlags.bHasGangLeaningOn) { // Someone is already leaning on it
        DoTidyUp(ped);
        return nullptr;
    }
    m_Vehicle->vehicleFlags.bHasGangLeaningOn = true;

    m_TargetPos                               = CalcTargetPos(ped);
    if (CVector::DistSqr(m_TargetPos, ped->GetPosition()) < sq(1.f)) {
        return nullptr;
    }
    return new CTaskComplexGoToPointAndStandStill{ PEDMOVE_WALK, m_TargetPos, 0.05f, 2.f, false, true };
}

// 0x664E60
CTask* CTaskGoToVehicleAndLean::ControlSubTask(CPed* ped) {
    const auto AbortAndTidyUp = [&]() -> CTask* {
        DoTidyUp(ped);
        return nullptr;
    };

    if (!m_Vehicle) {
        return AbortAndTidyUp();
    }

    // 0x664E84 - Ped is getting bored of it
    if (ped->GetIntelligence()->m_AnotherStaticCounter > 30) {
        return AbortAndTidyUp();
    }

    // 0x664F07 - Stop if the vehicle is being driven [or is moving]...
    const auto IsVehicleMoving = [&] {
        return m_Vehicle->GetDriver() || m_Vehicle->GetSpeed(ped->GetPosition() - m_Vehicle->GetPosition()).Magnitude() > 0.01f;
    };
    if (IsVehicleMoving() && m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_URGENT, nullptr)) {
        return AbortAndTidyUp();
    }

    // 0x664F5C - ...or if the target position has moved
    if (CVector::DistSqr(CalcTargetPos(ped), m_TargetPos) > sq(0.1f) && m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_URGENT, nullptr)) {
        return AbortAndTidyUp();
    }

    CAnimManager::StreamAnimBlock("gangs", CTaskComplexGangLeader::ShouldLoadGangAnims(), m_AnimsReferenced);

    return m_pSubTask;
}

// 0x660F60
void CTaskGoToVehicleAndLean::DoTidyUp(CPed* ped) {
    if (m_Vehicle)
        m_Vehicle->vehicleFlags.bHasGangLeaningOn = false;

    if (m_pSubTask)
        m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_IMMEDIATE, nullptr);
}

// 0x664770
CVector CTaskGoToVehicleAndLean::CalcTargetPos(const CPed* ped) {
    m_LeanOnRightSide = m_Vehicle->GetRightVector().Dot(ped->GetPosition() - m_Vehicle->GetPosition()) > 0.f;

    const auto& bb      = m_Vehicle->GetModelInfo()->GetColModel()->GetBoundingBox();
    const auto  offsetX = m_LeanOnRightSide
        ? bb.m_vecMax.x + 0.5f
        : bb.m_vecMin.x - 0.5f;
    return m_Vehicle->GetMatrix().TransformPoint({ offsetX, 0.f, 0.f });
}
