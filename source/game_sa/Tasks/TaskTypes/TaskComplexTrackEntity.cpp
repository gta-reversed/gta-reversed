#include "StdInc.h"
#include "TaskComplexTrackEntity.h"
#include "TaskSimpleGoToPointFine.h"
#include "TaskSimpleStandStill.h"
#include "TaskComplexFollowNodeRoute.h"

void CTaskComplexTrackEntity::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexTrackEntity, 0x86F998, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedConstructorInstall(0x65F3B0, "1", {}, CEntity*, CVector, bool, int32, float, float, bool);
    RH_ScopedDestructorInstall(0x65F460);

    RH_ScopedInstall(SetOffsetPos, 0x65F760);
    RH_ScopedInstall(CalcTargetPos, 0x65F780);
    RH_ScopedInstall(CalcMoveRatio, 0x65F930);

    RH_ScopedVMTInstall(Clone, 0x65F4E0);
    RH_ScopedVMTInstall(GetTaskType, 0x65F450);
    RH_ScopedVMTInstall(MakeAbortable, 0x65F4C0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x65F590);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x65F700);
    RH_ScopedVMTInstall(ControlSubTask, 0x663640, { .Reversed = true, .State = HS::RedirectToGTA, .Locked = true }); // Locked because it fucks up the stack and crashes
}

// 0x65F3B0
CTaskComplexTrackEntity::CTaskComplexTrackEntity(
    CEntity* entityToTrack,
    CVector  offsetPos,
    bool     useSprint,
    int32    giveUpTime,
    float    nodeRouteDist,
    float    giveUpDist,
    bool     isLocalOffset
) :
    m_EntityToTrack{ entityToTrack },
    m_OffsetPos{ offsetPos },
    m_UseSprint{ useSprint },
    m_GiveUpTime{ giveUpTime },
    m_NodeRouteDist{ nodeRouteDist },
    m_GiveUpDist{ giveUpDist },
    m_IsLocalOffset{ isLocalOffset } {
}

// For 0x65F4E0
CTaskComplexTrackEntity::CTaskComplexTrackEntity(const CTaskComplexTrackEntity& o) :
    CTaskComplexTrackEntity{
        o.m_EntityToTrack,
        o.m_OffsetPos,
        o.m_UseSprint,
        o.m_GiveUpTime,
        o.m_NodeRouteDist,
        o.m_GiveUpDist,
        o.m_IsLocalOffset,
    } {
    m_MoveBlendRatio = o.m_MoveBlendRatio;
}

// 0x65F780
void CTaskComplexTrackEntity::CalcTargetPos(CPed* ped) {
    m_TargetPos = m_EntityToTrack->GetPosition();
    m_TargetPos += m_IsLocalOffset
        ? CVector{ m_EntityToTrack->GetMatrix().TransformVector2D(m_OffsetPos) }
        : m_OffsetPos;
}

// 0x65F930
void CTaskComplexTrackEntity::CalcMoveRatio(CPed* ped) {
    // Calculate the distance to the target (We're using `GetPiecewiseLinear` for convinience, it was an if-else chain originally)
    m_TargetBlendRatio = std::sqrt(CGeneral::GetPiecewiseLinear({
        { 0.2f, 0.0f }, // 0x86F9D0
        { 1.0f, 0.5f }, // 0x86F9CC
        { 5.0f, 1.0f }, // 0x86F9C8
    }, std::sqrt(m_DistToTargetSq))) * 3.f;

    // Clamp ratio if no sprinting
    if (!m_UseSprint && m_TargetBlendRatio > 2.0f) {
        m_TargetBlendRatio = 2.0f;
    }

    // Accelerate gradually, decelerate instantly (BUG/TODO: Framerate dependent)
    m_MoveBlendRatio = m_TargetBlendRatio - m_MoveBlendRatio > 0.2f
        ? m_MoveBlendRatio + 0.2f
        : m_TargetBlendRatio;
}

// 0x65F4C0
bool CTaskComplexTrackEntity::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    return m_pSubTask->MakeAbortable(ped, priority, event);
}

// 0x65F590
CTask* CTaskComplexTrackEntity::CreateNextSubTask(CPed* ped) {
    if (!m_EntityToTrack) {
        return nullptr;
    }

    if (!notsa::isa_and_nonnull<CTaskSimpleStandStill>(m_pSubTask)) {
        return new CTaskSimpleStandStill{};
    }

    if (m_DistToTargetSq < sq(m_NodeRouteDist)) {
        return new CTaskSimpleGoToPointFine{ m_MoveBlendRatio, m_TargetPos, 0.25f };
    }

    return new CTaskComplexFollowNodeRoute{
        PEDMOVE_RUN,
        m_EntityToTrack->GetPosition(),
        0.5f,
        0.2f,
        2.0f
    };
}

// 0x65F700
CTask* CTaskComplexTrackEntity::CreateFirstSubTask(CPed* ped) {
    if (m_MoveBlendRatio < 0.0) {
        m_MoveBlendRatio = [ped] {
            switch (ped->m_nMoveState) {
            case PEDMOVE_STILL: return 0.0f;
            case PEDMOVE_WALK:  return 1.0f;
            case PEDMOVE_RUN:   return 2.0f;
            default:            return 3.0f;
            }
        }();
    }
    return CreateNextSubTask(ped);
}

// 0x663640
CTask* CTaskComplexTrackEntity::ControlSubTask(CPed* ped) {
    if (m_EntityToTrack && !m_GiveUpTimer.IsOutOfTime()) {
        CalcTargetPos(ped);
        switch (m_pSubTask->GetTaskType()) {
        case TASK_COMPLEX_FOLLOW_POINT_ROUTE: {
            // If we're totally out of range...
            if (m_DistToTargetSq > sq(m_GiveUpDist)) {
                return nullptr;
            }

            // If we're now in range be a little more precise and create `TASK_SIMPLE_GO_TO_POINT_FINE`
            if (m_DistToTargetSq < sq(m_NodeRouteDist) && MakeAbortable(ped)) {
                return CreateNextSubTask(ped);
            }

            break;
        }
        case TASK_SIMPLE_GO_TO_POINT_FINE: {
            // Check if we're still in range, if not, abort and go back to `TASK_COMPLEX_FOLLOW_POINT_ROUTE`
            if (m_DistToTargetSq >= sq(m_NodeRouteDist) && MakeAbortable(ped)) {
                return CreateNextSubTask(ped);
            }

            CalcMoveRatio(ped);

            auto* const tGoToPointFine = notsa::cast<CTaskSimpleGoToPointFine>(m_pSubTask);
            tGoToPointFine->SetTargetPos(m_TargetPos);
            tGoToPointFine->SetMoveRatio(m_MoveBlendRatio);

            break;
        }
        }
    } else if (MakeAbortable(ped)) {
        return nullptr;
    }
    return m_pSubTask;
}
