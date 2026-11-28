#pragma once

#include "TaskComplex.h"
#include "Vector.h"

class CEntity;

class NOTSA_EXPORT_VTABLE CTaskComplexTrackEntity : public CTaskComplex {
public:
    constexpr static auto Type = eTaskType::TASK_COMPLEX_TRACK_ENTITY;

    CTaskComplexTrackEntity(
        CEntity* pEntity,
        CVector  offsetPos,
        bool     useSprint,
        int32    giveUpTime,
        float    nodeRouteDist,
        float    giveUpDist,
        bool     isLocalOffset
    );
    CTaskComplexTrackEntity(const CTaskComplexTrackEntity&);
    ~CTaskComplexTrackEntity() override = default;

    void SetOffsetPos(CVector pos) { m_OffsetPos = pos; } // 0x65F760
    void CalcTargetPos(CPed* ped);
    void CalcMoveRatio(CPed* ped);

    CTask*    Clone() const override { return new CTaskComplexTrackEntity{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override;

public:
    static void InjectHooks();

public:
    notsa::EntityRef<> m_EntityToTrack{};        //!< The entity to track, if it becomes invalid, the task will be aborted
    CVector            m_OffsetPos{};            //!< Offset from the entity's position to track - Either world space or local
    bool               m_UseSprint{};            //!< If true, the ped will sprint to the target position, otherwise it will run
    int32              m_GiveUpTime{};           //!< Time in milliseconds to give up tracking the entity if it becomes invalid or unreachable
    float              m_NodeRouteDist{};        //!< Distance in world units to the target position at which the ped will start following a node route instead of going directly to the target position
    float              m_GiveUpDist{};           //!< Distance in world units to the target position at which the ped will give up tracking the entity
    bool               m_IsLocalOffset{};        //!< See `m_OffsetPos` member variable
    CTaskTimer         m_GiveUpTimer{};          //!< Timer to track when to give up tracking the entity
    float              m_MoveBlendRatio{ -1.f }; //!< Move blend ratio to use when moving towards the target position, if negative, it will be calculated based on the ped's move state
    float              m_TargetBlendRatio{};     //!< Target move blend ratio to use when moving towards the target position, if negative, it will be calculated based on the ped's move state
    CVector            m_TargetPos{};            //!< Target position to move towards, calculated based on the entity's position and the offset
    float              m_DistToTargetSq{};       //!< SqDist from the ped's position to the target position, updated every frame
    bool               m_HasEntityStopped{};     //!< Unused
};
VALIDATE_SIZE(CTaskComplexTrackEntity, 0x58);
