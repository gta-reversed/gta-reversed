#pragma once

#include "TaskSimpleAnim.h"
#include "TaskTimer.h"
#include "AnimationEnums.h"


class NOTSA_EXPORT_VTABLE CTaskSimpleRunTimedAnim : public CTaskSimpleAnim {
public:
    CTaskSimpleRunTimedAnim(
        AssocGroupId groupId,
        AnimationId  animId,
        float        blendDelta,
        float        blendOutDelta,
        uint32       durationMs,
        bool         holdLastFrame
    );
    CTaskSimpleRunTimedAnim(
        AssocGroupId groupId,
        AnimationId  animId,
        float        blendDelta,
        float        blendOutDelta,
        uint32       durationMs,
        eTaskType    taskType,
        const char*  taskName,
        bool         holdLastFrame
    );
    CTaskSimpleRunTimedAnim(const CTaskSimpleRunTimedAnim& o);
    ~CTaskSimpleRunTimedAnim() = default;

    CTask*    Clone() const override { return new CTaskSimpleRunTimedAnim{ *this }; }
    eTaskType GetTaskType() const override { return m_TaskType; }
    bool      ProcessPed(CPed* ped) override;

protected:
    void StartAnim(CPed* ped);

public:
    static void InjectHooks();

protected:
    AssocGroupId               m_AnimGroupId{};                      //!< The group ID of the animation to run
    AnimationId                m_AnimId{};                           //!< The animation ID to run
    float                      m_BlendDelta{};                       //!< How fast the animation should blend in
    float                      m_BlendOutDelta{};                    //!< Unused
    uint32                     m_RunDurationMs{};                    //!< How long the animation should run for, in milliseconds
    CTaskTimer                 m_RunTimer{};                         //!< Timer for how long the animation should run
    notsa::WEnumU16<eTaskType> m_TaskType{ TASK_SIMPLE_TIMED_ANIM }; //!< The type of the task (Set by derived tasks)
};
