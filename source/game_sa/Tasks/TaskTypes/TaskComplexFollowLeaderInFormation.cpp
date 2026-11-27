#include "StdInc.h"

#include "TaskComplexFollowLeaderInFormation.h"
#include "TaskSimpleAchieveHeading.h"
#include "TaskComplexEnterCarAsPassenger.h"
#include "TaskComplexLeaveCar.h"
#include "TaskSimpleCarDrive.h"
#include "TaskSimplePause.h"
#include "TaskSimpleStandStill.h"
#include "SeekEntity/TaskComplexSeekEntity.h"

void CTaskComplexFollowLeaderInFormation::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexFollowLeaderInFormation, 0x870c3c, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedConstructorInstall(0x6949A0, "1", {}, CPedGroup*, CPed*, const CVector&, float);
    RH_ScopedDestructorInstall(0x694A40);
    RH_ScopedInstall(CreateSubTask, 0x6962A0);

    RH_ScopedVMTInstall(Clone, 0x695740);
    RH_ScopedVMTInstall(GetTaskType, 0x694A30);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x696820);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x6968E0);
    RH_ScopedVMTInstall(ControlSubTask, 0x696940);
}

// 0x6949A0
CTaskComplexFollowLeaderInFormation::CTaskComplexFollowLeaderInFormation(CPedGroup* pedGroup, CPed* leader, const CVector& offset, float maxDist) :
    m_Group{ pedGroup },
    m_Leader{ leader },
    m_Offset{ offset },
    m_MoveState{ PEDMOVE_WALK },
    m_MaxDist{ maxDist } {
}

// For 0x695740
CTaskComplexFollowLeaderInFormation::CTaskComplexFollowLeaderInFormation(const CTaskComplexFollowLeaderInFormation& o) :
    CTaskComplexFollowLeaderInFormation{
        o.m_Group,
        o.m_Leader,
        o.m_Offset,
        o.m_MaxDist
    } {
}

// 0x6962A0
CTask* CTaskComplexFollowLeaderInFormation::CreateSubTask(eTaskType taskType, CPed* ped) {
    switch (taskType) {
    case TASK_SIMPLE_STAND_STILL: // 0x6962F4
        return new CTaskSimpleStandStill{ 500 };
    case TASK_SIMPLE_PAUSE: // 0x696339
        return new CTaskSimplePause{ 2'000 };
    case TASK_COMPLEX_ENTER_CAR_AS_PASSENGER: // 0x696375
        return new CTaskComplexEnterCarAsPassenger{ m_Leader->m_pVehicle };
    case TASK_COMPLEX_LEAVE_CAR: // 0x6963C6
        return new CTaskComplexLeaveCar{ ped->m_pVehicle, TARGET_DOOR_FRONT_LEFT, 0, true, false };
    case TASK_SIMPLE_CAR_DRIVE: // 0x69640C
        return new CTaskSimpleCarDrive{ ped->m_pVehicle };
    case TASK_COMPLEX_SEEK_ENTITY: { // 0x696493
        auto* const tSeek = new CTaskComplexSeekEntity<CEntitySeekPosCalculatorXYOffset>{
            m_Leader,
            50'000,
            1'000,
            1.f,
            2.f,
            2.f,
            false,
            true
        };
        tSeek->GetSeekPosCalculator().SetOffset(m_Offset);
        tSeek->SetMoveState(PEDMOVE_SPRINT);
        return tSeek;
    }
    case TASK_SIMPLE_ACHIEVE_HEADING: { // 0x69650E
        auto&       membership   = m_Group->GetMembership();
        auto* const randomMember = membership.GetMember(CGeneral::GetRandomNumberInRange(0, (int32)membership.CountMembers()));
        auto* const lookAtPed    = randomMember == ped ? membership.GetLeader() : randomMember;
        if (lookAtPed && CGeneral::GetRandomNumberInRange(0, 50) == 20) { // 0x69655B
            const CVector dir = (lookAtPed->GetPosition() - ped->GetPosition()).Normalized();

            // NOTE: Not `dir.Heading()`, the original replaces `y == 0` with a small value here
            return new CTaskSimpleAchieveHeading{ CGeneral::LimitRadianAngle(CGeneral::GetRadianAngleBetweenPoints(dir.x, dir.y, 0.f, 0.f)) };
        }
        return new CTaskSimpleAchieveHeading{ ped->m_fCurrentRotation }; // 0x69661F
    }
    case TASK_FINISHED: // 0x69647D
        return nullptr;
    default:
        NOTSA_UNREACHABLE_CASE(taskType);
    }
}

// 0x696820
CTask* CTaskComplexFollowLeaderInFormation::CreateNextSubTask(CPed* ped) {
    const auto leaderMoveState = m_Leader->m_nMoveState;
    switch (const auto tt = m_pSubTask->GetTaskType()) {
    case TASK_COMPLEX_SEEK_ENTITY: // 0x69688D
        return CreateSubTask(
            leaderMoveState == PEDMOVE_NONE || leaderMoveState == PEDMOVE_STILL
                ? TASK_SIMPLE_STAND_STILL
                : TASK_COMPLEX_SEEK_ENTITY,
            ped
        );
    case TASK_SIMPLE_STAND_STILL: // 0x696896
    case TASK_COMPLEX_LEAVE_CAR:  // 0x6968B6
    case TASK_SIMPLE_CAR_DRIVE:   // 0x6968B6
        return CreateSubTask(TASK_COMPLEX_SEEK_ENTITY, ped);
    case TASK_COMPLEX_ENTER_CAR_AS_PASSENGER: // 0x696852
        return CreateFirstSubTask(ped);
    case TASK_SIMPLE_PAUSE: // 0x696866
        return CreateSubTask(TASK_SIMPLE_PAUSE, ped);
    case TASK_SIMPLE_ACHIEVE_HEADING: // 0x6968C9
        return nullptr;
    default:
        NOTSA_UNREACHABLE_CASE(tt);
    }
}

// 0x6968E0
CTask* CTaskComplexFollowLeaderInFormation::CreateFirstSubTask(CPed* ped) {
    if (ped->IsInVehicle()) {
        return CreateSubTask(
            m_Leader->IsInVehicle(ped->m_pVehicle)
                ? TASK_SIMPLE_CAR_DRIVE
                : TASK_COMPLEX_LEAVE_CAR,
            ped
        );
    }
    return CreateSubTask(TASK_COMPLEX_SEEK_ENTITY, ped);
}

// 0x696940
CTask* CTaskComplexFollowLeaderInFormation::ControlSubTask(CPed* ped) {
    if (!m_Leader) {
        return nullptr;
    }

    const auto subTaskType = m_pSubTask->GetTaskType();
    if (notsa::isa<CTaskSimpleCarDrive>(m_pSubTask)) { // 0x696976
        return m_pSubTask;
    }

    const auto pedToLeaderDistSq = (ped->GetPosition() - m_Leader->GetPosition()).SquaredMagnitude();
    if (m_Leader->IsInVehicle()) { // 0x696981
        auto* const leaderVeh = m_Leader->m_pVehicle;
        switch (subTaskType) {
        case TASK_COMPLEX_SEEK_ENTITY: { // 0x6969B8
            notsa::cast<CTaskComplexSeekEntity<CEntitySeekPosCalculatorXYOffset>>(m_pSubTask)->SetMaxEntityDist2D(leaderVeh->GetBoundRadius());
            if (m_MaxDist <= 0.f || pedToLeaderDistSq <= sq(m_MaxDist)) {
                return m_pSubTask;
            }
            if (!m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_LEISURE)) {
                return m_pSubTask;
            }
            CTaskSimpleStandStill{}.ProcessPed(ped);
            return new CTaskSimplePause{ 2'000 };
        }
        case TASK_SIMPLE_STAND_STILL: { // 0x696AD1
            if (CPedGroups::GetPedsGroup(ped)) {
                return m_pSubTask;
            }
            if (pedToLeaderDistSq < sq(leaderVeh->GetBoundRadius())) { // 0x696B3F
                // NOTE: Not `HasSpaceForAPassenger()`, that one leaves one more seat free
                if (leaderVeh->m_nNumPassengers >= leaderVeh->m_nMaxPassengers) {
                    return m_pSubTask;
                }
                return m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_LEISURE)
                    ? new CTaskComplexEnterCarAsPassenger{ leaderVeh }
                    : m_pSubTask;
            }
            if (pedToLeaderDistSq < sq(m_MaxDist) && m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_LEISURE)) { // 0x696BAE
                return CreateSubTask(TASK_COMPLEX_SEEK_ENTITY, ped);
            }
            return m_pSubTask;
        }
        case TASK_SIMPLE_PAUSE:
        case TASK_COMPLEX_LEAVE_CAR:
        case TASK_COMPLEX_ENTER_CAR_AS_PASSENGER:
        case TASK_SIMPLE_ACHIEVE_HEADING:
            return m_pSubTask;
        default:
            NOTSA_UNREACHABLE_CASE(subTaskType);
        }
    }

    switch (subTaskType) {
    case TASK_COMPLEX_ENTER_CAR_AS_PASSENGER: // 0x696BF7
        return m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_LEISURE)
            ? CreateFirstSubTask(ped)
            : m_pSubTask;
    case TASK_SIMPLE_PAUSE:       // 0x696C31
    case TASK_SIMPLE_STAND_STILL: // 0x696C31
        return pedToLeaderDistSq < sq(m_MaxDist)
            ? CreateSubTask(TASK_COMPLEX_SEEK_ENTITY, ped)
            : m_pSubTask;
    case TASK_COMPLEX_SEEK_ENTITY:
    case TASK_COMPLEX_LEAVE_CAR:
    case TASK_SIMPLE_ACHIEVE_HEADING:
        return m_pSubTask;
    default:
        NOTSA_UNREACHABLE_CASE(subTaskType);
    }
}
