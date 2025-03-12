#include "StdInc.h"

#include "TaskComplexWalkAlongsidePed.h"
#include "TaskComplexFollowNodeRoute.h"
#include "TaskComplexLeaveCar.h"
#include "TaskSimpleGoToPoint.h"
#include "TaskSimpleStandStill.h"
#include "TaskComplexSeekEntity.h"
#include "SeekEntity/PosCalculators/EntitySeekPosCalculatorXYOffset.h"

void CTaskComplexWalkAlongsidePed::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexWalkAlongsidePed, 0x8706cc, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x683240);
    RH_ScopedInstall(Destructor, 0x6832C0);

    RH_ScopedInstall(CreateSubTask, 0x685000);
    RH_ScopedVMTInstall(Clone, 0x6838E0);
    RH_ScopedVMTInstall(GetTaskType, 0x6832B0);
    RH_ScopedVMTInstall(MakeAbortable, 0x683320);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x685680);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x685630);
    RH_ScopedVMTInstall(ControlSubTask, 0x685170);
}

// 0x683240
CTaskComplexWalkAlongsidePed::CTaskComplexWalkAlongsidePed(CPed* ped, float giveUpDist) :
    m_TargetPed{ped},
    m_GiveUpDist{giveUpDist}
{
}

// 0x6838E0
CTaskComplexWalkAlongsidePed::CTaskComplexWalkAlongsidePed(const CTaskComplexWalkAlongsidePed& o) :
    CTaskComplexWalkAlongsidePed{o.m_TargetPed, o.m_GiveUpDist}
{
}

// 0x685000
CTask* CTaskComplexWalkAlongsidePed::CreateSubTask(eTaskType tt, CPed* ped) {
    switch (tt) {
    case TASK_COMPLEX_LEAVE_CAR:
        return new CTaskComplexLeaveCar{ped->m_pVehicle, 0, 0, true, false};
    case TASK_SIMPLE_STAND_STILL:
        return new CTaskSimpleStandStill{10};
    case TASK_COMPLEX_SEEK_ENTITY: {
        const auto t = new CTaskComplexSeekEntity<CEntitySeekPosCalculatorXYOffset>{
            m_TargetPed,
            50'000,
            1'000,
            0.5f,
            5.f,
            2.f,
            false,
            false,
            CEntitySeekPosCalculatorXYOffset{m_Offset}
        };
        t->SetMoveState(PEDMOVE_SPRINT);
        t->SetIsTrackingEntity(true);
        return t;
    }
    default:
        NOTSA_UNREACHABLE("Invalid TaskType {}", tt);
    }
}

// 0x685680
CTask* CTaskComplexWalkAlongsidePed::CreateNextSubTask(CPed* ped) {
    if (!m_TargetPed) {
        return nullptr;
    }
    switch (m_pSubTask->GetTaskType()) {
    case TASK_SIMPLE_STAND_STILL:
    case TASK_COMPLEX_LEAVE_CAR:
        return CreateSubTask(TASK_COMPLEX_SEEK_ENTITY, ped);
    case TASK_COMPLEX_SEEK_ENTITY:
        return CreateSubTask(TASK_SIMPLE_STAND_STILL, ped);
    }
    return nullptr;
}

// 0x685630
CTask* CTaskComplexWalkAlongsidePed::CreateFirstSubTask(CPed* ped) {
    if (!m_TargetPed) {
        return nullptr;
    }
    return CreateSubTask(ped->IsInVehicle() ? TASK_COMPLEX_LEAVE_CAR : TASK_COMPLEX_SEEK_ENTITY, ped);
}

// 0x685170
CTask* CTaskComplexWalkAlongsidePed::ControlSubTask(CPed* ped) {
    if (!m_TargetPed) {
        return m_pSubTask->MakeAbortable(ped)
            ? nullptr
            : m_pSubTask;
    }

    const auto targetPedPos = m_TargetPed->GetPosition();
    const auto pedPos       = ped->GetPosition();

    const auto pedWalkAnim       = RpAnimBlendClumpGetAssociation(ped->m_pRwClump, ANIM_ID_WALK);
    const auto targetPedWalkAnim = RpAnimBlendClumpGetAssociation(ped->m_pRwClump, ANIM_ID_WALK);

    const auto isTargetMoving = notsa::contains({PEDMOVE_WALK, PEDMOVE_RUN, PEDMOVE_SPRINT}, m_TargetPed->GetIntelligence()->GetMoveStateFromGoToTask());

    const auto pedToTarget         = m_TargetPed->GetPosition2D() - ped->GetPosition2D();
    const auto targetToPedDist2DSq = pedToTarget.SquaredMagnitude();

    if (targetToPedDist2DSq >= sq(m_GiveUpDist)) { // 0x68527B
        return nullptr;
    }

    const auto tSubTaskSeekEntity = CTask::DynCast<CTaskComplexSeekEntity<CEntitySeekPosCalculatorXYOffset>>(m_pSubTask);
    if (!tSubTaskSeekEntity) { // 0x68528B
        ped->bMoveAnimSpeedHasBeenSetByTask         = false;
        m_TargetPed->bMoveAnimSpeedHasBeenSetByTask = false;
        return m_pSubTask;
    }

    tSubTaskSeekEntity->GetSeekPosCalculator().SetOffset(m_Offset);
    tSubTaskSeekEntity->SetEntityMinDist2D(3.f);
    
    const auto pedTaskFollowNodeRoute = ped->GetTaskManager().Find<CTaskComplexFollowNodeRoute>();
    const auto pedTaskGoToPoint       = ped->GetTaskManager().Find<CTaskSimpleGoToPoint>();

    bool bChangedAnimSpeed = false;
    if (pedTaskGoToPoint && !pedTaskFollowNodeRoute) { // 0x685308 (Inverted)
        auto targetPtForPed = m_TargetPed->GetMatrix().TransformPoint(m_Offset);
        if (isTargetMoving) { // 0x68535C
            if (m_TargetPed->GetPosition().Dot(m_TargetPed->GetForward()) - targetPtForPed.Dot(m_TargetPed->GetForward()) < 0.f) { // Target pt is behind target ped (I think?)
                if (pedWalkAnim) { // 0x6853C6
                    const auto pedToTargetPtDist2DSq = (ped->GetPosition2D() - CVector2D{targetPtForPed}).SquaredMagnitude();

                    if (pedToTargetPtDist2DSq < 2.f) { // 0x6853DB
                        targetPtForPed += m_TargetPed->GetForward() * 2.f;
                    }

                    const auto IncreaseWalkAnimSpeed = [](CAnimBlendAssociation* walkAnim) {
                        walkAnim->SetSpeed(std::min(1.25f, walkAnim->m_Speed + 0.0125f));
                    };
                    const auto DecreaseWalkAnimSpeed = [](CAnimBlendAssociation* walkAnim) {
                        walkAnim->SetSpeed(std::max(0.85f, walkAnim->m_Speed - 0.0125f));
                    };

                    if (pedToTargetPtDist2DSq >= sq(0.25f)) {
                        if (pedToTargetPtDist2DSq <= sq(0.25f)) {
                            NOTSA_UNREACHABLE("Never say never"); // 0x6854BF - I'm quite sure they fucked something up, cause this only runs if `pedToTargetPtDist2DSq == sq(0.25f)`
                        }
                        IncreaseWalkAnimSpeed(pedWalkAnim);
                        if (targetPedWalkAnim) {
                            DecreaseWalkAnimSpeed(targetPedWalkAnim);
                        }
                    } else {
                        DecreaseWalkAnimSpeed(pedWalkAnim);
                        if (targetPedWalkAnim) {
                            IncreaseWalkAnimSpeed(targetPedWalkAnim);
                        }
                    }

                    bChangedAnimSpeed = true;
                }
            }
        }

        pedTaskGoToPoint->UpdatePoint(targetPtForPed); // 0x685586
    }

    if (ped->GetIntelligence()->m_AnotherStaticCounter > 8 && targetToPedDist2DSq <= sq(2.f) && m_pSubTask->MakeAbortable(ped)) { // 0x6855C2
        m_Offset = CVector{CVector2D{pedToTarget}};
        return CreateSubTask(TASK_SIMPLE_STAND_STILL, ped);
    }

    m_TargetPed->bMoveAnimSpeedHasBeenSetByTask = ped->bMoveAnimSpeedHasBeenSetByTask = bChangedAnimSpeed;

    return m_pSubTask;
}
