#pragma once

#include <concepts>

#include "TaskComplex.h"
#include "World.h"
#include "Ped.h"
#include "General.h"
#include "TaskTimer.h"
#include "eMoveState.h"
#include "extensions/utility.hpp"
#include "PedPlacement.h"
#include "TaskSimpleCarDriveTimed.h"
#include "TaskSimpleTired.h"
#include "TaskComplexFollowNodeRoute.h"
#include "TaskComplexGoToPointAndStandStill.h"
#include "TaskSimpleCarDrive.h"
#include "TaskComplexLeaveCar.h"
#include "TaskComplexSequence.h"
#include "TaskComplexTurnToFaceEntityOrCoord.h"
#include "TaskSimpleStandStill.h"
#include "TaskSimpleTired.h"
#include "PosCalculators/EntitySeekPosCalculator.h"
#include "PosCalculators/EntitySeekPosCalculatorStandard.h"
#include "PosCalculators/EntitySeekPosCalculatorXYOffset.h"

template <std::derived_from<CEntitySeekPosCalculator> T_PosCalc = CEntitySeekPosCalculatorStandard>
class NOTSA_EXPORT_VTABLE CTaskComplexSeekEntity : public CTaskComplex {
    CEntity* m_entityToSeek{};
    int32 m_seekInterval{};
    int32 m_scanInterval{};
    float m_maxEntityDist2D{};
    float m_moveStateRadius{};
    float m_minEntityDist2D{};
    float m_followNodeThresholdHeightChange{};
    CTaskTimer m_scanTimer{};
    CTaskTimer m_seekTimer{};
    T_PosCalc m_entitySeekPosCalculator{};

    // Everything here mustn't be accesed without the proper `T_PosCalc` type!
    eMoveState m_moveState{ PEDMOVE_RUN };
    bool m_bPlayTiredAnim : 1{};
    bool m_bFaceEntityWhenDone : 1{};
    bool m_bAchievedSeekEntity : 1{}; /// True when the distance between the `GetSeekPos()` and the `ped` is less than `m_maxEntityDist2D`
    bool m_bTrackingEntity : 1{};
public:
    /*!
    * NOTE: Since this task is templated but uses the same task type for all templated tasks
    *       our template magic stuff in TaskManager might not work properly with it,
    *       because at runtime all templated tasks will have the same type.
    *       Accessing anything before `m_entitySeekPosCalculator` is fine either way
    *       but anything after it depends on the actual `T_PosCalc` used [which can't [easily] be figured out at runtime]
    */
    static constexpr auto Type = eTaskType::TASK_COMPLEX_SEEK_ENTITY;

    static void InjectHooks() {
        RH_ScopedCategory("Tasks/TaskTypes/SeekEntity");
        if constexpr (std::is_same_v<T_PosCalc, CEntitySeekPosCalculatorXYOffset>) {
            RH_ScopedVirtualClass(CTaskComplexSeekEntity<CEntitySeekPosCalculatorXYOffset>, 0x86FBB4, 11);

            RH_ScopedInstall(Constructor, 0x661DC0);
            RH_ScopedInstall(Destructor, 0x661F30);

            RH_ScopedInstall(CreateSubTask, 0x496DC0);

            RH_ScopedVMTInstall(Clone, 0x664AF0);
            RH_ScopedVMTInstall(GetTaskType, 0x661EA0);
            RH_ScopedVMTInstall(MakeAbortable, 0x661EB0);
            RH_ScopedVMTInstall(CreateNextSubTask, 0x665080);
            RH_ScopedVMTInstall(CreateFirstSubTask, 0x665600);
            RH_ScopedVMTInstall(ControlSubTask, 0x665760);
        }
        // TODO: Add other specializations
    }

    CTaskComplexSeekEntity(
        CEntity*  entity,
        int32     seekInterval,
        int32     scanInterval,
        float     maxEntityDist2D,
        float     moveStateRadius,
        float     followNodeThresholdHeightChange,
        bool      bPlayTiredAnim,
        bool      bFaceEntityWhenDone,
        T_PosCalc seekPosCalculator = {} // NOTSA
    ) :
        m_entityToSeek{ entity },
        m_seekInterval{ seekInterval },
        m_scanInterval{ scanInterval },
        m_maxEntityDist2D{ maxEntityDist2D },
        m_moveStateRadius{ moveStateRadius },
        m_followNodeThresholdHeightChange{ followNodeThresholdHeightChange },
        m_bPlayTiredAnim{ bPlayTiredAnim },
        m_bFaceEntityWhenDone{ bFaceEntityWhenDone },
        m_entitySeekPosCalculator{ seekPosCalculator }
    {
        CEntity::SafeRegisterRef(m_entityToSeek);
    }

    CTaskComplexSeekEntity(const CTaskComplexSeekEntity& o) :
        m_entityToSeek{ o.m_entityToSeek },
        m_seekInterval{ o.m_seekInterval },
        m_scanInterval{ o.m_scanInterval },
        m_maxEntityDist2D{ o.m_maxEntityDist2D },
        m_moveStateRadius{ o.m_moveStateRadius },
        m_followNodeThresholdHeightChange{ o.m_followNodeThresholdHeightChange },
        m_bPlayTiredAnim{ o.m_bPlayTiredAnim },
        m_bFaceEntityWhenDone{ true },

        m_entitySeekPosCalculator{ o.m_entitySeekPosCalculator },
        m_moveState{ o.m_moveState },
        m_bTrackingEntity{ o.m_bTrackingEntity },
        m_minEntityDist2D{ o.m_minEntityDist2D }
    {
        CEntity::SafeRegisterRef(m_entityToSeek);
    }

    ~CTaskComplexSeekEntity() override {
        CEntity::SafeCleanUpRef(m_entityToSeek);
    }

    CTask* CreateSubTask(eTaskType type, CPed* ped) {
        switch (type) {
        case TASK_FINISHED:
            return nullptr;
        case TASK_COMPLEX_TURN_TO_FACE_ENTITY:
            return new CTaskComplexTurnToFaceEntityOrCoord{ m_entityToSeek };
        case TASK_COMPLEX_FOLLOW_NODE_ROUTE:
            return new CTaskComplexFollowNodeRoute{
                m_moveState,
                GetSeekPos(ped),
                m_maxEntityDist2D,
                m_moveStateRadius,
                m_followNodeThresholdHeightChange,
                false,
                -1,
                true
            };
        case TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL:
            return new CTaskComplexGoToPointAndStandStill{
                m_moveState,
                GetSeekPos(ped),
                m_maxEntityDist2D,
                m_moveStateRadius,
                false,
                false,
                m_bTrackingEntity
            };
        case TASK_SIMPLE_CAR_DRIVE_TIMED:
            return new CTaskSimpleCarDriveTimed{ ped->m_pVehicle, 2000 };
        case TASK_COMPLEX_LEAVE_CAR:
            return new CTaskComplexLeaveCar{ ped->m_pVehicle, TARGET_DOOR_FRONT_LEFT, 0, true, false };
        case TASK_COMPLEX_SEQUENCE: {
            const auto seq = new CTaskComplexSequence{};
            if (m_bFaceEntityWhenDone) {
                seq->AddTask(new CTaskComplexTurnToFaceEntityOrCoord{ m_entityToSeek });
            }
            seq->AddTask(new CTaskSimpleStandStill{ 100 });
            return seq;
        }
        case TASK_SIMPLE_STAND_STILL:
            return new CTaskSimpleStandStill{ 2000 };
        case TASK_SIMPLE_TIRED:
            return new CTaskSimpleTired{ 1000u };
        default:
            NOTSA_UNREACHABLE();
        }
    }

    CTask* Clone() const override {
        return new CTaskComplexSeekEntity{ *this };
    }

    eTaskType GetTaskType() const override {
        return eTaskType::TASK_COMPLEX_SEEK_ENTITY;
    }

    bool MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override {
        if (priority == ABORT_PRIORITY_LEISURE) {
            m_scanInterval = -1;
            m_seekTimer.SetAsOutOfTime();
        }

        if (m_pSubTask->MakeAbortable(ped, priority, event)) {
            m_scanTimer.SetAsOutOfTime();
            return true;
        }

        return false;
    }

    CTask* CreateNextSubTask(CPed* ped) override {
        switch (m_pSubTask->GetTaskType()) {
        case TASK_COMPLEX_SEQUENCE:
        case TASK_COMPLEX_TURN_TO_FACE_ENTITY:
            return CreateSubTask(TASK_FINISHED, ped);
        case TASK_COMPLEX_FOLLOW_NODE_ROUTE:
        case TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL: {
            if (const auto dist2DSq = (ped->GetPosition() - GetSeekPos(ped)).SquaredMagnitude2D(); dist2DSq > sq(m_maxEntityDist2D)) {
                return CreateSubTaskWhenPedIsTooFarFromEntity(ped, dist2DSq);
            }
            m_bAchievedSeekEntity = true;
            return CreateSubTask(m_bFaceEntityWhenDone ? TASK_COMPLEX_TURN_TO_FACE_ENTITY : TASK_FINISHED, ped); // Ternary inverted
        }
        case TASK_SIMPLE_CAR_DRIVE_TIMED:
            return CreateFirstSubTask(ped);
        case TASK_COMPLEX_LEAVE_CAR:
            return CreateSubTask(ped->bInVehicle ? TASK_FINISHED : TASK_SIMPLE_CAR_DRIVE_TIMED, ped);
        case TASK_SIMPLE_STAND_STILL:
            return CreateSubTask(TASK_SIMPLE_STAND_STILL, ped);
        case TASK_SIMPLE_TIRED: {
            CTaskSimpleStandStill task{ -1 };
            task.ProcessPed(ped);
            return CreateSubTask(TASK_FINISHED, ped);
        }
        default:
            NOTSA_UNREACHABLE();
        }
    }

    CTask* CreateFirstSubTask(CPed* ped) override {
        m_scanTimer.Start(m_scanInterval);
        m_seekTimer.Start(m_seekInterval);

        if (!m_entityToSeek) {
            return CreateSubTask(TASK_SIMPLE_TIRED, ped);
        }

        if (ped->IsInVehicle()) {
            return CreateSubTask(TASK_COMPLEX_LEAVE_CAR, ped);
        }
 
        if (const auto dist2DSq = (ped->GetPosition2D() - GetSeekPos2D(ped)).SquaredMagnitude(); dist2DSq > sq(m_maxEntityDist2D)) {
            return CreateSubTaskWhenPedIsTooFarFromEntity(ped, dist2DSq);
        }

        m_bAchievedSeekEntity = true;
        ped->SetMoveState(PEDMOVE_STILL);
        return CreateSubTask(TASK_COMPLEX_SEQUENCE, ped);
    }

    CTask* ControlSubTask(CPed* ped) override {
        // Boat in contact with both peds (that is, in case m_entityToSeek is a ped)
        const auto commonBoat = [this, ped]() -> CBoat* {
            if (!m_entityToSeek || !m_entityToSeek->IsPed()) {
                return nullptr;
            }

            const auto toSeekPed = m_entityToSeek->AsPed();

            if (!CPedGroups::AreInSameGroup(toSeekPed, ped)) {
                return nullptr;
            }

            if (toSeekPed->physicalFlags.bSubmergedInWater) {
                return nullptr;
            }

            for (const auto entity : { toSeekPed->m_standingOnEntity, toSeekPed->m_pContactEntity }) {
                if (entity && entity->IsVehicle()) {
                    if (ped->m_standingOnEntity == entity || ped->m_pContactEntity == entity) {
                        if (entity->AsVehicle()->IsBoat()) {
                            return entity->AsBoat();
                        }
                    }
                }
            }

            return nullptr;
        }();

        if (ped->m_pAttachedTo && !commonBoat) {
            ped->DettachPedFromEntity();
        }

        const auto subTaskType = m_pSubTask->GetTaskType();

        if (
               ped->m_pAttachedTo // From 0x496821
            || !commonBoat
            || subTaskType == TASK_SIMPLE_STAND_STILL
            || m_pSubTask->MakeAbortable(ped)
        ) {
            notsa::ScopeGuard makePedTalkOnReturn{ [this, ped] {
                if (m_entityToSeek && m_entityToSeek->IsPed()) {
                    if (m_entityToSeek->AsPed()->IsPlayer()) { // Entity to seek is a player
                        if (FindPlayerPed()->GetPlayerGroup().GetMembership().IsFollower(ped)) { // And ped is part of the player's group
                            if (ped->IsJoggingOrFaster()) {
                                if (((uint16)CTimer::GetFrameCounter() + ped->m_nRandomSeed) % 16384 == 0) {
                                    ped->Say(CTX_GLOBAL_FOLLOW_CONSTANT);
                                }
                            }
                        }
                    }
                }
            }};


            switch (subTaskType) {
            case TASK_COMPLEX_FOLLOW_NODE_ROUTE:
            case TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL:
                break;
            default:
                return nullptr;
            }

            if (!m_entityToSeek) {
                return m_pSubTask->MakeAbortable(ped) ? nullptr : m_pSubTask;
            }

            if (m_seekTimer.IsOutOfTime()) {
                if (!m_pSubTask->MakeAbortable(ped)) {
                    return m_pSubTask;
                }

                if (!m_bPlayTiredAnim) {
                    return nullptr;
                }

                return new CTaskSimpleTired{ 1000u };
            }

            if (!m_scanTimer.IsOutOfTime()) {
                return m_pSubTask;
            }

            m_scanTimer.Start(m_scanInterval);

            const auto seekPos = GetSeekPos(ped);
            const auto pedToSeekPosDist2dSq = (ped->GetPosition() - seekPos).SquaredMagnitude2D();

            if (subTaskType == TASK_COMPLEX_FOLLOW_NODE_ROUTE) {
                if (m_minEntityDist2D == 0.f || pedToSeekPosDist2dSq >= sq(m_minEntityDist2D - 1.f)) {
                    static_cast<CTaskComplexFollowNodeRoute*>(m_pSubTask)->SetTarget(ped, seekPos, m_maxEntityDist2D, m_moveStateRadius, m_followNodeThresholdHeightChange, false);
                } else if (m_pSubTask->MakeAbortable(ped)) {
                    return CreateSubTask(TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL, ped);
                }
            } else {
                const auto subTaskStandStill = CTask::Cast<CTaskComplexGoToPointAndStandStill>(m_pSubTask);
                if (subTaskStandStill->m_bWasSuccessful) {
                    return m_pSubTask;      
                } else if (m_minEntityDist2D == 0.f || pedToSeekPosDist2dSq <= sq(m_minEntityDist2D + 1.f)) {
                    subTaskStandStill->GoToPoint(seekPos, m_maxEntityDist2D, m_moveStateRadius, false);
                } else if (m_pSubTask->MakeAbortable(ped)) {
                    return CreateSubTask(TASK_COMPLEX_FOLLOW_NODE_ROUTE, ped);
                }
            }
            return m_pSubTask;
        }

        // Process ped to boat attachment

        auto& boatPos      = commonBoat->GetPosition();
        auto  pedToBoat    = ped->GetPosition() - boatPos;
        auto  pedToBoatDir = Normalized(pedToBoat);

        auto newPedPos = boatPos - (pedToBoat - pedToBoatDir * 0.25f);
        (void)CPedPlacement::FindZCoorForPed(newPedPos); // Omitting `if` check here, it's useless
    
        ped->AttachPedToEntity(
            commonBoat,
            Invert(commonBoat->GetMatrix()).TransformVector(newPedPos - boatPos),
            (int16)CGeneral::LimitRadianAngle(CGeneral::GetRadianAngleBetweenPoints(pedToBoatDir, {})),
            0.2f,
            ped->GetActiveWeapon().m_Type
        );

        return new CTaskSimpleStandStill{ 2000 };
    }

    // NOTSA Helpers
    CVector GetSeekPos(CPed* ped) {
        CVector seekPos{};
        m_entitySeekPosCalculator.ComputeEntitySeekPos(*ped, *m_entityToSeek, seekPos);
        return seekPos;
    }

    CVector2D GetSeekPos2D(CPed* ped) {
        return CVector2D{GetSeekPos(ped)};
    }

    void SetMoveState(eMoveState ms) {
        m_moveState = ms;
    }

    void SetIsTrackingEntity(bool v) {
        m_bTrackingEntity = v;
    }

    auto& GetSeekPosCalculator() {
        return m_entitySeekPosCalculator;
    }

    void SetEntityMinDist2D(float v) {
        m_minEntityDist2D = v;
    }

    auto GetEntityToSeek() const {
        return m_entityToSeek;
    }

    auto GetMoveStateRadius() const {
        return m_moveStateRadius;
    }

private:
    CTask* CreateSubTaskWhenPedIsTooFarFromEntity(CPed* ped, float pedToSeekPosDist2DSq) {
        return CreateSubTask(
            m_minEntityDist2D == 0.f || pedToSeekPosDist2DSq > sq(m_minEntityDist2D) ? TASK_COMPLEX_FOLLOW_NODE_ROUTE : TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL,
            ped
        );
    }

protected: // Wrappers for hooks
    // 0x493730
    CTaskComplexSeekEntity* Constructor(
        CEntity* entity,
        int32    taskTimer2Interval,
        int32    taskTimer1Interval,
        float    maxEntityDist2D,
        float    unk1,
        float    unk2,
        bool     flag0,
        bool     flag1
    ) {
        this->CTaskComplexSeekEntity::CTaskComplexSeekEntity(entity, taskTimer1Interval, taskTimer2Interval, maxEntityDist2D, unk1, unk2, flag0, flag1);
        return this;
    }

    // 0x493890
    CTaskComplexSeekEntity* Destructor() {
        this->CTaskComplexSeekEntity::~CTaskComplexSeekEntity();
        return this;
    }
};
