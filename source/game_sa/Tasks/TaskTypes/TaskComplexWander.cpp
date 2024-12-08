#include "StdInc.h"

#include "TaskComplexWander.h"

#include "TaskSimpleCarDriveTimed.h"
#include "TaskSimpleScratchHead.h"
#include "TaskSimpleStandStill.h"
#include "TaskSimpleGoToPoint.h"
#include "TaskSimpleRunAnim.h"

#include "TaskComplexObserveTrafficLightsAndAchieveHeading.h"
#include "TaskComplexCrossRoadLookAndAchieveHeading.h"
#include "TaskComplexWanderProstitute.h"
#include "TaskComplexWanderStandard.h"
#include "TaskComplexWanderCriminal.h"
#include "TaskComplexWanderMedic.h"
#include "TaskComplexWanderCop.h"
#include "TaskComplexLeaveCar.h"

void CTaskComplexWander::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexWander, 0x86FE84, 15);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x66F450);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x674140, { .reversed = false });
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x6740E0);
    RH_ScopedVMTInstall(ControlSubTask, 0x674C30);
    RH_ScopedVMTInstall(UpdateDir, 0x669DA0);
    RH_ScopedVMTInstall(UpdatePathNodes, 0x669ED0);
    RH_ScopedInstall(CreateSubTask, 0x671CB0);
    RH_ScopedInstall(ComputeTargetPos, 0x669F60);
    RH_ScopedInstall(ComputeTargetHeading, 0x66F530);
    RH_ScopedInstall(ValidNodes, 0x669F30);
    RH_ScopedInstall(ScanForBlockedNodes, 0x674560);
    RH_ScopedOverloadedInstall(ScanForBlockedNode, "", 0x671EF0, bool(CTaskComplexWander::*)(CPed*, const CNodeAddress&));
    RH_ScopedOverloadedInstall(ScanForBlockedNode, "1", 0x66F4C0, bool(CTaskComplexWander::*)(const CVector&, CEntity*));
    RH_ScopedInstall(GetWanderTaskByPedType, 0x673D00);
}
CTaskComplexWander* CTaskComplexWander::Constructor(eMoveState moveState, uint8 dir, bool bWanderSensibly, float fTargetRadius) { this->CTaskComplexWander::CTaskComplexWander(moveState, dir, bWanderSensibly, fTargetRadius); return this; }

// 0x66F450
CTaskComplexWander::CTaskComplexWander(eMoveState moveState, uint8 dir, bool bWanderSensibly, float fTargetRadius) : CTaskComplex() {
    m_nMoveState = moveState;
    m_nDir = dir;
    m_fTargetRadius = fTargetRadius;
    m_bWanderSensibly = bWanderSensibly;
    m_nLastUpdateDirFrameCount = 0;
}

// 0x674140


CTask* CTaskComplexWander::CreateNextSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x674140, CTaskComplexWander*, CPed*>(this, ped); // untested

    switch (m_pSubTask->GetTaskType()) {
    case TASK_SIMPLE_SCRATCH_HEAD: {
        m_nDir++;
        UpdatePathNodes(ped, m_nDir, m_LastNode, m_NextNode, m_nDir);

        if (ValidNodes()) {
            return CreateSubTask(ped, TASK_SIMPLE_GO_TO_POINT);
        }

        return new CTaskSimpleScratchHead();
    }
    case TASK_COMPLEX_CROSS_ROAD_LOOK_AND_ACHIEVE_HEADING:
    case TASK_COMPLEX_OBSERVE_TRAFFIC_LIGHTS_AND_ACHIEVE_HEADING:
    case TASK_COMPLEX_SIT_DOWN_THEN_IDLE_THEN_STAND_UP:
        return CreateSubTask(ped, TASK_SIMPLE_GO_TO_POINT);

    case TASK_SIMPLE_CAR_DRIVE_TIMED:
        return CreateFirstSubTask(ped);

    case TASK_COMPLEX_LEAVE_CAR:
        if (ped->bInVehicle) {
            return CreateSubTask(ped, TASK_SIMPLE_CAR_DRIVE_TIMED);
        }
        return CreateFirstSubTask(ped);

    case TASK_COMPLEX_SEQUENCE:
    case TASK_SIMPLE_GO_TO_POINT:
        if (m_bAllNodesBlocked) {
            m_bAllNodesBlocked = false;
            return new CTaskSimpleScratchHead();
        }

        UpdateDir(ped);
        UpdatePathNodes(ped, m_nDir, m_LastNode, m_NextNode, m_nDir);

        if (m_NextNode.m_wAreaId == m_LastNode.m_wAreaId || m_NextNode.m_wNodeId == m_LastNode.m_wNodeId) {
            CVector outTargetPos;
            ComputeTargetPos(ped, outTargetPos, m_NextNode);

            auto* sequence = new CTaskComplexSequence();
            sequence->AddTask(new CTaskSimpleStandStill(500, false, false, 8.0f));
            sequence->AddTask(new CTaskSimpleRunAnim(ped->m_nAnimGroup, ANIM_ID_ROADCROSS, 4.0F, false));
            sequence->AddTask(new CTaskSimpleScratchHead());
            sequence->AddTask(new CTaskSimpleGoToPoint(m_nMoveState, outTargetPos, m_fTargetRadius, false, false));
            return sequence;
        }

        if (!ValidNodes())
            return new CTaskSimpleScratchHead();

        if (m_bWanderSensibly) {
            if (ThePaths.TestForPedTrafficLight(m_LastNode, m_NextNode)) {
                return CreateSubTask(ped, TASK_COMPLEX_OBSERVE_TRAFFIC_LIGHTS_AND_ACHIEVE_HEADING);
            }

            if (ThePaths.TestCrossesRoad(m_LastNode, m_NextNode)) {
                return new CTaskComplexCrossRoadLookAndAchieveHeading(2000, ComputeTargetHeading(ped));
            }
        }
        return CreateSubTask(ped, TASK_SIMPLE_GO_TO_POINT);

    case TASK_COMPLEX_IN_AIR_AND_LAND:
    default:
        return nullptr;
    }
}

// 0x6740E0


CTask* CTaskComplexWander::CreateFirstSubTask(CPed* ped) {
    if (ped->bInVehicle) {
        return CreateSubTask(ped, TASK_COMPLEX_LEAVE_CAR);
    }

    UpdateDir(ped);
    UpdatePathNodes(ped, m_nDir, m_LastNode, m_NextNode, m_nDir);
    return CreateSubTask(ped, TASK_SIMPLE_GO_TO_POINT);
}

// 0x674C30


CTask* CTaskComplexWander::ControlSubTask(CPed* ped) {
    auto subTaskId = m_pSubTask->GetTaskType();
    if (subTaskId == TASK_COMPLEX_LEAVE_CAR || subTaskId == TASK_SIMPLE_CAR_DRIVE_TIMED) {
        return m_pSubTask;
    }

    if (m_bNewDir && subTaskId == TASK_SIMPLE_GO_TO_POINT) {
        m_bNewDir = false;
        m_NextNode.ResetAreaId();
        m_LastNode.ResetAreaId();
        return CreateFirstSubTask(ped);
    } else {
        if (m_bNewNodes && subTaskId == TASK_SIMPLE_GO_TO_POINT) {
            m_bNewNodes = false;
            CVector outTargetPos;
            ComputeTargetPos(ped, outTargetPos, m_NextNode);
            static_cast<CTaskSimpleGoToPoint*>(m_pSubTask)->UpdatePoint(outTargetPos, 0.5f, false);
        } else {
            ScanForBlockedNodes(ped);
            ScanForStuff(ped);
        }

        if (subTaskId == TASK_SIMPLE_GO_TO_POINT) {
            static_cast<CTaskSimpleGoToPoint*>(m_pSubTask)->m_moveState = m_nMoveState;
            if (m_nMoveState <= PEDMOVE_WALK) {
                ped->Say(CTX_GLOBAL_CHAT);
            }
        }

        if (ped->m_pIntelligence->m_AnotherStaticCounter > 30) {
            if (m_pSubTask->MakeAbortable(ped)) {
                return CreateSubTask(ped, TASK_SIMPLE_SCRATCH_HEAD);
            }
        }
    }
    return m_pSubTask;
}

// 0x669DA0


void CTaskComplexWander::UpdateDir(CPed* ped) {
    uint8 newDir = m_nDir;
    if (m_NextNode.IsAreaValid()) {
        const CPathNode* pathNodes = ThePaths.m_pPathNodes[m_NextNode.m_wAreaId];
        if (pathNodes) {
            const CPathNode* pathNode = &pathNodes[m_NextNode.m_wNodeId];
            if (pathNode) {
                if (pathNode->m_nNumLinks >= 3u && CTimer::GetFrameCounter() != m_nLastUpdateDirFrameCount && m_bWanderSensibly) {
                    m_nLastUpdateDirFrameCount = CTimer::GetFrameCounter();
                    uint8 remainder = (3 * CTimer::GetFrameCounter() + ped->m_nRandomSeed) % 100;
                    if (remainder <= 90u) {
                        if (remainder > 80u) {
                            newDir = (newDir + 10) % 8;
                        }
                    } else {
                        newDir = (m_nDir + 6) % 8;
                    }
                }
            }
        }
    }

    if (newDir != m_nDir) {
        uint8 outDir;
        UpdatePathNodes(ped, newDir, m_LastNode, m_NextNode, outDir);

        uint8 currentDir = m_nDir;
        uint8 theDir = outDir;
        if (outDir <= currentDir) {
            theDir = m_nDir;
        }

        if (theDir == currentDir) {
            currentDir = outDir;
        }

        if (((theDir - currentDir + 8) % 8) <= 3u) {
            m_nDir = newDir;
        }
    }
}

// 0x669ED0

// 0x0
void CTaskComplexWander::UpdatePathNodes(const CPed* ped, uint8 dir, CNodeAddress& originNode, CNodeAddress& targetNode, uint8& outDir) {
    originNode = targetNode;
    targetNode.m_wAreaId = (uint16)-1;
    const CVector& pos = ped->GetPosition();
    ThePaths.FindNextNodeWandering(PATH_TYPE_PED, pos, &originNode, &targetNode, dir, &outDir);
}

// 0x671CB0
CTask* CTaskComplexWander::CreateSubTask(CPed* ped, eTaskType taskType) {
    switch (taskType) {
    case TASK_SIMPLE_CAR_DRIVE_TIMED: {
        return new CTaskSimpleCarDriveTimed(ped->m_pVehicle, 2000);
    }
    case TASK_SIMPLE_GO_TO_POINT: {
        CVector outTargetPos;
        ComputeTargetPos(ped, outTargetPos, m_NextNode);
        return new CTaskSimpleGoToPoint(m_nMoveState, outTargetPos, m_fTargetRadius, false, false);
    }
    case TASK_FINISHED: {
        return nullptr;
    }
    case TASK_COMPLEX_LEAVE_CAR: {
        return new CTaskComplexLeaveCar(ped->m_pVehicle, 0, 0, true, false);
    }
    case TASK_COMPLEX_OBSERVE_TRAFFIC_LIGHTS_AND_ACHIEVE_HEADING: {
        return new CTaskComplexObserveTrafficLightsAndAchieveHeading(2000, ComputeTargetHeading(ped));
    }
    case TASK_COMPLEX_CROSS_ROAD_LOOK_AND_ACHIEVE_HEADING: {
        return new CTaskComplexCrossRoadLookAndAchieveHeading(2000, ComputeTargetHeading(ped));
    }
    case TASK_SIMPLE_SCRATCH_HEAD: {
        return new CTaskSimpleScratchHead();
    }
    default: {
        return nullptr;
    }
    }
}

// 0x66F530
float CTaskComplexWander::ComputeTargetHeading(CPed* ped) {
    auto position = ThePaths.TakeWidthIntoAccountForWandering(m_NextNode, ped->m_nRandomSeed);
    position -= ped->GetPosition();
    float radianAngle = CGeneral::GetRadianAngleBetweenPoints(position.x, position.y, 0.0f, 0.0f);
    return CGeneral::LimitRadianAngle(radianAngle);
}

// 0x669F60
void CTaskComplexWander::ComputeTargetPos(const CPed* ped, CVector& outTargetPos, const CNodeAddress& targetNodeAddress) {
    outTargetPos = ThePaths.TakeWidthIntoAccountForWandering(targetNodeAddress, ped->m_nRandomSeed);
    outTargetPos.z += 1.0f;
}

// 0x669F30
bool CTaskComplexWander::ValidNodes() const {
    if (m_NextNode.IsValid() && m_LastNode.IsValid()) {
        if (m_NextNode.m_wAreaId != m_LastNode.m_wAreaId || m_NextNode.m_wNodeId != m_LastNode.m_wNodeId) {
            return true;
        }
    }
    return false;
}

// 0x674560
void CTaskComplexWander::ScanForBlockedNodes(CPed* ped) {
    if (m_pSubTask->GetTaskType() == TASK_SIMPLE_GO_TO_POINT && m_NextNode.IsValid()) {
        if (ScanForBlockedNode(ped, m_NextNode)) {
            m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_LEISURE, nullptr);

            uint8 outDir = 0;
            UpdatePathNodes(ped, m_nDir, m_LastNode, m_NextNode, outDir);
            if (ScanForBlockedNode(ped, m_NextNode) || &m_NextNode == &m_LastNode) {
                m_bAllNodesBlocked = true;
            }
        }
    }
}

// 0x671EF0
bool CTaskComplexWander::ScanForBlockedNode(CPed* ped, const CNodeAddress& targetNodeAddress) {
    auto outVec = ThePaths.TakeWidthIntoAccountForWandering(targetNodeAddress, ped->m_nRandomSeed);
    if (DistanceBetweenPointsSquared2D(outVec, ped->GetPosition()) <= sq(3.0f)) {
        CPed* closestPed = ped->GetIntelligence()->GetPedScanner().GetClosestPedInRange();
        if (ScanForBlockedNode(outVec, closestPed)) {
            return true;
        }

        CVehicle* closestVehicle = ped->GetIntelligence()->GetVehicleScanner().GetClosestVehicleInRange();
        if (ScanForBlockedNode(outVec, closestVehicle)) {
            return true;
        }
    }
    return false;
}

// 0x66F4C0
bool CTaskComplexWander::ScanForBlockedNode(const CVector& position, CEntity* entity) {
    if (!entity)
        return false;

    float fRadius = entity->GetModelInfo()->GetColModel()->GetBoundRadius() + 1.0f;
    if (DistanceBetweenPointsSquared2D(entity->GetPosition(), position) < sq(fRadius))
        return true;

    return false;
}

// 0x673D00
CTaskComplexWander* CTaskComplexWander::GetWanderTaskByPedType(CPed* ped) {
    uint8 randomDir = CGeneral::GetRandomNumberInRange(0, 8);
    switch (ped->m_nPedType) {
    case PED_TYPE_COP:        return new CTaskComplexWanderCop(PEDMOVE_WALK, randomDir);
    case PED_TYPE_MEDIC:      return new CTaskComplexWanderMedic(PEDMOVE_WALK, randomDir);
    case PED_TYPE_CRIMINAL:   return new CTaskComplexWanderCriminal(PEDMOVE_WALK, randomDir);
    case PED_TYPE_PROSTITUTE: return new CTaskComplexWanderProstitute(PEDMOVE_WALK, randomDir);
    default:                  return new CTaskComplexWanderStandard(PEDMOVE_WALK, randomDir);
    }
}

// 0x669FF0 - OG name unknown
float CTaskComplexWander::GetDistSqOfClosestPathNodeToPed(CPed* ped) {
    return rng::min(
        std::array{ m_NextNode, m_LastNode }
     | rng::views::transform([ped](CNodeAddress node) {
            if (node.IsValid() && ThePaths.IsAreaNodesAvailable(node)) {
                return (ped->GetPosition() - ThePaths.GetPathNode(node)->GetPosition()).SquaredMagnitude();
            }
            return 999'999.f;
        })
    );
}
