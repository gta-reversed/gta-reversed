#include "StdInc.h"

#include "TaskComplexFollowPedFootsteps.h"
#include <reversiblebugfixes/Bugs.hpp>
#include "TaskSimpleStandStill.h"
#include "TaskSimpleHitHead.h"
#include "TaskSimpleGoToPoint.h"
#include "TaskComplexSeekEntityStandard.h"

constexpr auto   CLOSE_DIST              = 1.4f;  // 0x870C20
constexpr auto   MIN_FOOTSTEP_DIST       = 0.35f; // 0x870C24
constexpr auto   GO_TO_POINT_RADIUS      = 0.01f; // 0x870C28
constexpr auto   WALK_DIST               = 3.5f;  // 0x870C2C
constexpr auto   SEEK_DIST               = 8.f;   // 0x870C38
constexpr auto   STRAIGHT_LINE_DOT       = 0.95f;
constexpr uint32 FOOTSTEP_INTERVAL_MS    = 166;
constexpr uint32 LOS_CHECK_INTERVAL_MS   = 500;
constexpr int32  HIT_HEAD_STATIC_COUNTER = 30; // 0x86C938

void CTaskComplexFollowPedFootsteps::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexFollowPedFootsteps, 0x870cc0, 12);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedConstructorInstall(0x694E20, "1", {}, CPed*);
    RH_ScopedDestructorInstall(0x695DC0);

    RH_ScopedVMTInstall(Clone, 0x695890);
    RH_ScopedVMTInstall(GetTaskType, 0x694EC0);
    RH_ScopedVMTInstall(MakeAbortable, 0x694ED0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x694EE0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x695000);
    RH_ScopedVMTInstall(ControlSubTask, 0x695090);
    RH_ScopedVMTInstall(CreateSubTask, 0x695E40);
}

// 0x694E20
CTaskComplexFollowPedFootsteps::CTaskComplexFollowPedFootsteps(CPed* ped) :
    m_TargetPed{ ped },
    m_SubTaskCreateCheckTimer{ CTimer::GetTimeInMS() },
    m_PointRoute{ new CPointRoute() } {
}

// 0x695DC0
CTaskComplexFollowPedFootsteps::~CTaskComplexFollowPedFootsteps() {
    delete std::exchange(m_PointRoute, nullptr);
}

// 0x694EE0
CTask* CTaskComplexFollowPedFootsteps::CreateNextSubTask(CPed* ped) {
    if (!m_TargetPed) {
        return CreateSubTask(TASK_FINISHED, ped);
    }
    switch (const auto tt = m_pSubTask->GetTaskType()) {
    case TASK_NONE: // 0x694F05
        return CreateSubTask(TASK_FINISHED, ped);
    case TASK_SIMPLE_HIT_HEAD: // 0x694EFA
        return CreateSubTask(TASK_SIMPLE_STAND_STILL, ped);
    case TASK_SIMPLE_STAND_STILL: // 0x694F0A
    case TASK_SIMPLE_GO_TO_POINT: // 0x694F51
        return CreateSubTask(m_PointRoute && !m_PointRoute->IsEmpty() ? TASK_SIMPLE_GO_TO_POINT : TASK_SIMPLE_STAND_STILL, ped);
    case TASK_COMPLEX_SEEK_ENTITY: // 0x694F56
        // NOTE: The squared distance is compared with `CLOSE_DIST` (not squared), unlike in `ControlSubTask`.
        //       Not `CVector::DistSqr`: it is 2D on master (its `a - b` picks `CVector2D`'s `operator-`).
        return CreateSubTask((m_TargetPed->GetPosition() - ped->GetPosition()).SquaredMagnitude() > CLOSE_DIST ? TASK_COMPLEX_SEEK_ENTITY : TASK_SIMPLE_STAND_STILL, ped);
    default:
        NOTSA_UNREACHABLE_CASE(tt);
    }
}

// 0x695000
CTask* CTaskComplexFollowPedFootsteps::CreateFirstSubTask(CPed* ped) {
    if (!m_TargetPed) {
        return CreateSubTask(TASK_FINISHED, ped);
    }
    // NOTE: The squared distance is compared with `CLOSE_DIST` (not squared), unlike in `ControlSubTask`.
    //       Not `CVector::DistSqr`: it is 2D on master (its `a - b` picks `CVector2D`'s `operator-`).
    return CreateSubTask((m_TargetPed->GetPosition() - ped->GetPosition()).SquaredMagnitude() > CLOSE_DIST ? TASK_COMPLEX_SEEK_ENTITY : TASK_SIMPLE_STAND_STILL, ped);
}

// 0x695090
CTask* CTaskComplexFollowPedFootsteps::ControlSubTask(CPed* ped) {
    const auto TryChangeSubTaskTo = [&](eTaskType tt) {
        return m_pSubTask->MakeAbortable(ped) ? CreateSubTask(tt, ped) : m_pSubTask;
    };

    if (!m_TargetPed || !m_PointRoute || !m_TargetPed->IsAlive()) {
        return TryChangeSubTaskTo(TASK_FINISHED);
    }
    if (notsa::isa<CTaskComplexSeekEntityStandard>(m_pSubTask)) {
        return m_pSubTask;
    }
    if (m_TargetPed->bInVehicle) { // NOTE: Not `IsInVehicle()`, the original only checks the flag
        return TryChangeSubTaskTo(TASK_COMPLEX_SEEK_ENTITY);
    }
    if (ped->GetIntelligence()->m_AnotherStaticCounter > HIT_HEAD_STATIC_COUNTER) { // 0x69510C
        return TryChangeSubTaskTo(TASK_SIMPLE_HIT_HEAD);
    }

    if (CTimer::GetTimeInMS() - m_LineOfSightCheckTimer > LOS_CHECK_INTERVAL_MS) { // 0x69512A
        m_LineOfSightCheckTimer = CTimer::GetTimeInMS();
        m_HasLineOfSight        = CWorld::GetIsLineOfSightClear(ped->GetPosition(), m_TargetPed->GetPosition(), true, false, false, true);
    }

    const CVector targetPos   = m_TargetPed->GetPosition();
    const CVector pedToTarget = targetPos - ped->GetPosition();
    const auto    distSq      = pedToTarget.SquaredMagnitude();
    if (distSq < sq(CLOSE_DIST) && !notsa::isa<CTaskSimpleStandStill>(m_pSubTask)) { // 0x695202
        if (!m_HasLineOfSight) {
            return m_pSubTask;
        }
        m_PointRoute->Clear();
        return TryChangeSubTaskTo(TASK_SIMPLE_STAND_STILL);
    }
    if (distSq > sq(SEEK_DIST)) { // 0x695244
        return TryChangeSubTaskTo(TASK_COMPLEX_SEEK_ENTITY);
    }
    if (pedToTarget.SquaredMagnitude2D() < sq(1.f) && std::abs(pedToTarget.z) > 2.f) { // 0x69525F
        return TryChangeSubTaskTo(TASK_COMPLEX_SEEK_ENTITY);
    }

    if (m_HasLineOfSight) { // 0x695287
        m_PointRoute->Clear();
        m_PointRoute->AddUnlessFull(m_TargetPed->GetPosition());
        m_SubTaskCreateCheckTimer = CTimer::GetTimeInMS() - FOOTSTEP_INTERVAL_MS * 2;
        if (auto* const tSimplest = ped->GetTaskManager().GetSimplestActiveTask(); CTask::IsGoToTask(tSimplest)) {
            // NOTE: Not `GetSimplestActiveTaskAs<CTaskSimpleGoToPoint>()`: `IsGoToTask` also accepts `CTaskSimpleGoToPointFine`.
            //       `UpdatePoint` only writes `CTaskSimpleGoTo` members, which both have.
            static_cast<CTaskSimpleGoToPoint*>(tSimplest)->UpdatePoint(m_TargetPed->GetPosition());
        }
    }

    if (CTimer::GetTimeInMS() - m_SubTaskCreateCheckTimer < FOOTSTEP_INTERVAL_MS || m_PointRoute->IsFull()) { // 0x695310
        if (notsa::isa<CTaskSimpleStandStill>(m_pSubTask) && m_PointRoute->GetSize() > 1) {                   // 0x695584
            return TryChangeSubTaskTo(TASK_SIMPLE_GO_TO_POINT);
        }
        return m_pSubTask;
    }

    // Record the target's position as a new footstep
    m_SubTaskCreateCheckTimer = CTimer::GetTimeInMS();
    if (m_PointRoute->IsEmpty()) { // 0x695334
        m_PointRoute->AddUnlessFull(targetPos);
    } else {
        const auto    numPoints    = m_PointRoute->GetSize();
        const CVector lastPoint    = (*m_PointRoute)[numPoints - 1];
        const CVector lastToTarget = targetPos - lastPoint;
        if (lastToTarget.SquaredMagnitude() > sq(MIN_FOOTSTEP_DIST)) { // 0x6953A0
            if (numPoints >= 2) {
                // Still going the same way - replace the last footstep instead of adding one
                const CVector prevToLast = lastPoint - (*m_PointRoute)[numPoints - 2];
                if (prevToLast.Normalized().Dot(lastToTarget.Normalized()) >= STRAIGHT_LINE_DOT) { // 0x69542B
                    m_PointRoute->PopBack();
                }
            }
            m_PointRoute->AddUnlessFull(targetPos);
        }
    }

    const auto IsCloseEnoughToWalk = [&] {
        if (notsa::bugfixes::CTaskComplexFollowPedFootsteps_ControlSubTask_TrailLength) {
            float   trailLength{};
            CVector prevPoint = ped->GetPosition();
            for (const auto& point : m_PointRoute->GetAll()) {
                trailLength += (point - prevPoint).Magnitude();
                prevPoint = point;
            }
            return trailLength + (m_TargetPed->GetPosition() - prevPoint).Magnitude() <= WALK_DIST;
        }
        const CVector firstPoint = (*m_PointRoute)[0];
        float         trailDistSq{};
        for (const auto& point : m_PointRoute->GetAll() | rngv::drop(1)) {
            trailDistSq += (point - firstPoint).SquaredMagnitude();
        }
        return (ped->GetPosition() - m_TargetPed->GetPosition()).SquaredMagnitude() + trailDistSq <= sq(WALK_DIST);
    };
    m_MoveState = IsCloseEnoughToWalk() // 0x69553E
        ? PEDMOVE_WALK
        : PEDMOVE_RUN;
    if (auto* const tGoToPoint = notsa::dyn_cast<CTaskSimpleGoToPoint>(m_pSubTask)) { // 0x69555D
        tGoToPoint->m_moveState = m_MoveState;
    }
    return m_pSubTask;
}

// 0x695E40
CTask* CTaskComplexFollowPedFootsteps::CreateSubTask(eTaskType taskType, CPed* ped) {
    if (!m_TargetPed || !m_PointRoute) {
        return nullptr;
    }
    switch (taskType) {
    case TASK_SIMPLE_STAND_STILL: // 0x695ED6
        return new CTaskSimpleStandStill{ 10'000 };
    case TASK_SIMPLE_HIT_HEAD: // 0x695E9E
        return new CTaskSimpleHitHead{};
    case TASK_SIMPLE_GO_TO_POINT: // 0x695F1C
        return new CTaskSimpleGoToPoint{ m_MoveState, m_PointRoute->PopFront(), GO_TO_POINT_RADIUS };
    case TASK_COMPLEX_SEEK_ENTITY: // 0x695FA8
        return new CTaskComplexSeekEntityStandard{
            m_TargetPed,
            50'000,
            1'000,
            m_TargetPed->bInVehicle ? 4.f : 1.f,
            2.f,
            2.f,
            true,
            true
        };
    case TASK_FINISHED: // 0x695F8B
        return nullptr;
    default:
        NOTSA_UNREACHABLE_CASE(taskType);
    }
}
