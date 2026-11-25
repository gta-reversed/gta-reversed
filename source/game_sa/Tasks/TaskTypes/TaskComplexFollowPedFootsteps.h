#pragma once

#include "TaskComplex.h"
#include "Ped.h"

class CPointRoute;

class NOTSA_EXPORT_VTABLE CTaskComplexFollowPedFootsteps : public CTaskComplex {
public:
    static constexpr auto Type = TASK_COMPLEX_FOLLOW_PED_FOOTSTEPS;

    explicit CTaskComplexFollowPedFootsteps(CPed* ped);
    ~CTaskComplexFollowPedFootsteps() override;

    CTask* Clone() const override {
        return new CTaskComplexFollowPedFootsteps(m_targetPed);
    }
    eTaskType      GetTaskType() const override { return Type; }
    bool           MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override { return m_pSubTask->MakeAbortable(ped, priority, event); }
    CTask*         CreateNextSubTask(CPed* ped) override;
    CTask*         CreateFirstSubTask(CPed* ped) override;
    CTask*         ControlSubTask(CPed* ped) override;
    virtual CTask* CreateSubTask(eTaskType taskType, CPed* ped);

public:
    static void InjectHooks();

public:
    CPed::Ref    m_targetPed{};
    uint32       m_subTaskCreateCheckTimer{};
    uint32       m_lineOfSightCheckTimer{};
    CPointRoute* m_pointRoute{};
    int16        m_moveState{ PEDMOVE_WALK };
    uint8        m_updateGoToPoint : 1 {};
};
VALIDATE_SIZE(CTaskComplexFollowPedFootsteps, 0x20);
