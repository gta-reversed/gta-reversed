#include "StdInc.h"

#include "TaskGoToVehicleAndLean.h"
#include "TaskSimpleStandStill.h"
#include "TaskSimpleAchieveHeading.h"
#include "TaskLeanOnVehicle.h"
#include "TaskComplexGoToPointAndStandStill.h"
#include "TaskComplexGangLeader.h"

void CTaskGoToVehicleAndLean::InjectHooks() {
    RH_ScopedVirtualClass(CTaskGoToVehicleAndLean, 0x86FAC8, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x660E60);
    RH_ScopedInstall(Destructor, 0x660EE0);
    RH_ScopedInstall(Clone, 0x6621B0);
    RH_ScopedInstall(MakeAbortable, 0x664500);
    RH_ScopedInstall(CreateNextSubTask, 0x664590);
    RH_ScopedInstall(CreateFirstSubTask, 0x664D40);
    RH_ScopedInstall(ControlSubTask, 0x664E60);
    RH_ScopedInstall(CalcTargetPos, 0x664770);
    RH_ScopedInstall(DoTidyUp, 0x660F60);
}

// 0x660E60
CTaskGoToVehicleAndLean::CTaskGoToVehicleAndLean(CVehicle* vehicle, int32 leanAnimDurationInMs) : CTaskComplex() {
    m_Vehicle = vehicle;
    m_LeanAnimDurationInMs = leanAnimDurationInMs;
    m_LeanOnVehicle = false;
    CEntity::SafeRegisterRef(m_Vehicle);
}

// 0x660EE0
CTaskGoToVehicleAndLean::~CTaskGoToVehicleAndLean() {
    if (m_LeanOnVehicle) {
        CAnimManager::RemoveAnimBlockRef(CAnimManager::GetAnimationBlockIndex("gangs"));
        m_LeanOnVehicle = false;
    }
    CEntity::SafeCleanUpRef(m_Vehicle);
}

// 0x6621B0
CTask* CTaskGoToVehicleAndLean::Clone() const {
    return new CTaskGoToVehicleAndLean{ m_Vehicle, m_LeanAnimDurationInMs };
}

// 0x664500
bool CTaskGoToVehicleAndLean::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    if (priority != ABORT_PRIORITY_IMMEDIATE) {
        if (!m_pSubTask->MakeAbortable(ped, priority, event)) {
            return false;
        }
    } else {
        m_pSubTask->MakeAbortable(ped, priority, event);
    }
    if (m_Vehicle) {
        m_Vehicle->vehicleFlags.bHasGangLeaningOn = false;
    }
    if (m_pSubTask) {
        m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_IMMEDIATE, nullptr);
    }
    return true;
}

// 0x664590
CTask* CTaskGoToVehicleAndLean::CreateNextSubTask(CPed* ped) {
    if (!m_Vehicle) {
        if (m_pSubTask) {
            m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_IMMEDIATE, nullptr);
        }
        return nullptr;
    }
    switch (m_pSubTask->GetTaskType()) {
    case TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL: { // 0x66460B
        const auto right = m_Vehicle->GetRight();
        const auto fwdHeading = CGeneral::GetRadianAngleBetweenPoints(right.x, right.y, 0.0f, 0.0f);
        const auto targetHeading = CGeneral::LimitRadianAngle(!m_bCalcTargetPosSide ? -fwdHeading : fwdHeading);
        return new CTaskSimpleAchieveHeading{ targetHeading, 0.5f, 0.2f };
    }
    case TASK_SIMPLE_ACHIEVE_HEADING:
    case TASK_SIMPLE_STAND_STILL: { // 0x6646CB
        if (!m_LeanOnVehicle) {
            return new CTaskSimpleStandStill{ 500, false, false, 8.0f };
        }
        return new CTaskLeanOnVehicle{ m_Vehicle, m_LeanAnimDurationInMs, 0 };
    }
    default: // 0x664714
        DoTidyUp(ped);
        return nullptr;
    }
}

// 0x664D40
CTask* CTaskGoToVehicleAndLean::CreateFirstSubTask(CPed* ped) {
    if (!m_Vehicle) {
        if (m_pSubTask) {
            m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_IMMEDIATE, nullptr);
        }
        return nullptr;
    }
    if (m_Vehicle->vehicleFlags.bHasGangLeaningOn) {
        m_Vehicle->vehicleFlags.bHasGangLeaningOn = false;
        if (m_pSubTask) {
            m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_IMMEDIATE, nullptr);
        }
        return nullptr;
    }
    m_Vehicle->vehicleFlags.bHasGangLeaningOn = true;
    m_TargetPos = CalcTargetPos(ped);
    if ((m_TargetPos - ped->GetPosition()).SquaredMagnitude() >= 1.0f) {
        return new CTaskComplexGoToPointAndStandStill{ PEDMOVE_WALK, m_TargetPos, 0.05f, 2.0f, false, true };
    }
    if (m_pSubTask) {
        m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_IMMEDIATE, nullptr);
    }
    return nullptr;
}

// 0x664E60
CTask* CTaskGoToVehicleAndLean::ControlSubTask(CPed* ped) {
    if (!m_Vehicle) {
        if (m_pSubTask) {
            m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_IMMEDIATE, nullptr);
        }
        return nullptr;
    }
    if (ped->GetIntelligence()->m_nDecisionMakerType > 30) {
        m_Vehicle->vehicleFlags.bHasGangLeaningOn = false;
        if (m_pSubTask) {
            m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_IMMEDIATE, nullptr);
        }
        return nullptr;
    }
    const auto& vehPos = m_Vehicle->GetPosition();
    const auto& pedPos = ped->GetPosition();
    const auto relSpeed = m_Vehicle->GetSpeed(vehPos - pedPos);
    if (m_Vehicle->m_pDriver || relSpeed.SquaredMagnitude() > 0.01f) {
        if (!m_pSubTask->MakeAbortable(ped)) {
            return m_pSubTask;
        }
        DoTidyUp(ped);
        return nullptr;
    }
    if ((CalcTargetPos(ped) - m_TargetPos).SquaredMagnitude() > 0.04f * 0.04f) {
        if (!m_pSubTask->MakeAbortable(ped)) {
            return m_pSubTask;
        }
        DoTidyUp(ped);
        return nullptr;
    }
    if (!m_LeanOnVehicle) {
        if (CTaskComplexGangLeader::ShouldLoadGangAnims()) {
            const auto blk = CAnimManager::GetAnimationBlockIndex("gangs");
            if (!CAnimManager::GetAnimBlocks()[blk].IsLoaded) {
                CStreaming::RequestModel(IFPToModelId(blk), STREAMING_KEEP_IN_MEMORY);
                return m_pSubTask;
            }
            CAnimManager::AddAnimBlockRef(blk);
            m_LeanOnVehicle = true;
        }
        return m_pSubTask;
    }
    if (!CTaskComplexGangLeader::ShouldLoadGangAnims()) {
        CAnimManager::RemoveAnimBlockRef(CAnimManager::GetAnimationBlockIndex("gangs"));
        m_LeanOnVehicle = false;
    }
    return m_pSubTask;
}

// 0x664770
CVector CTaskGoToVehicleAndLean::CalcTargetPos(CPed* ped) {
    const auto* mi = CModelInfo::GetModelInfo(m_Vehicle->GetModelIndex());
    const auto bbox = mi->GetColModel()->GetBoundingBox();
    auto localPos = CVector{ 0.0f, 0.0f, 0.0f };
    localPos.x = m_bCalcTargetPosSide ? bbox.m_vecMax.x + 0.5f : bbox.m_vecMin.x - 0.5f;
    const auto& vehMatrix = m_Vehicle->GetMatrix();
    const auto& vehPos = m_Vehicle->GetPosition();
    const CVector vehFwd{ vehMatrix.GetForward().x, vehMatrix.GetForward().y, vehMatrix.GetForward().z };
    const auto& pedPos = ped->GetPosition();
    const CVector toPed{ pedPos.x - vehPos.x, pedPos.y - vehPos.y, pedPos.z - vehPos.z };
    if (vehFwd.x * toPed.x + vehFwd.y * toPed.y + vehFwd.z * toPed.z > 0.0f) {
        m_bCalcTargetPosSide = true;
        localPos.x = bbox.m_vecMax.x + 0.5f;
    } else {
        m_bCalcTargetPosSide = false;
        localPos.x = bbox.m_vecMin.x - 0.5f;
    }
    return vehMatrix.TransformPoint(localPos);
}

// 0x660F60
void CTaskGoToVehicleAndLean::DoTidyUp(CPed* ped) {
    if (m_Vehicle)
        m_Vehicle->vehicleFlags.bHasGangLeaningOn = false;

    if (m_pSubTask)
        m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_IMMEDIATE, nullptr);
}
