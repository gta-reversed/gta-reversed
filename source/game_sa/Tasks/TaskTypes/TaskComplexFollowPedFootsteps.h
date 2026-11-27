#pragma once

#include "TaskComplex.h"
#include "Ped.h"
#include "Enums/eMoveState.h"

class CPointRoute;

class NOTSA_EXPORT_VTABLE CTaskComplexFollowPedFootsteps : public CTaskComplex {
public:
    static constexpr auto Type = TASK_COMPLEX_FOLLOW_PED_FOOTSTEPS;

    explicit CTaskComplexFollowPedFootsteps(CPed* ped);
    ~CTaskComplexFollowPedFootsteps() override;

    CTask* Clone() const override {
        return new CTaskComplexFollowPedFootsteps(m_TargetPed);
    }
    eTaskType      GetTaskType() const override { return Type; }
    bool           MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override { return m_pSubTask->MakeAbortable(ped, priority, event); }
    CTask*         CreateNextSubTask(CPed* ped) override;
    CTask*         CreateFirstSubTask(CPed* ped) override;
    CTask*         ControlSubTask(CPed* ped) override;
    virtual CTask* CreateSubTask(eTaskType taskType, CPed* ped);

    auto GetTargetPed() const { return m_TargetPed.Get(); }

    static void InjectHooks();

private:
    CPed::Ref     m_TargetPed{};
    uint32        m_SubTaskCreateCheckTimer{}; //!< Last time a footstep of the target was recorded
    uint32        m_LineOfSightCheckTimer{};
    CPointRoute*  m_PointRoute{}; //!< The target's recorded footsteps
    eMoveStateS16 m_MoveState{ PEDMOVE_WALK };
    bool          m_HasLineOfSight : 1 {}; //!< Result of the last line of sight check to the target
};
VALIDATE_SIZE(CTaskComplexFollowPedFootsteps, 0x20);
