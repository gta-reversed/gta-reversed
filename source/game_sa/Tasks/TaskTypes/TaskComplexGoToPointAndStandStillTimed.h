#pragma once

#include "TaskComplexGoToPointAndStandStill.h"
#include "TaskTimer.h"

class CTaskComplexGoToPointAndStandStillTimed : public CTaskComplexGoToPointAndStandStill {
public:
    int32      m_nTime;
    CTaskTimer m_timer;

public:
    CTaskComplexGoToPointAndStandStillTimed(int32 moveState, const CVector& targetPoint, float fRadius, float fMoveStateRadius, int32 time);
    ~CTaskComplexGoToPointAndStandStillTimed();

    CTask* Clone() override;
    void StopTimer(const CEvent* event) override;
    bool MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) override;
    CTask* CreateFirstSubTask(CPed* ped) override;
    CTask* ControlSubTask(CPed* ped) override;

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CTaskComplexGoToPointAndStandStillTimed* Constructor(int32 moveState, const CVector& targetPoint, float fRadius, float fMoveStateRadius, int32 time);
    CTask* Clone_Reversed();
    void StopTimer_Reversed(const CEvent* event);
    bool MakeAbortable_Reversed(CPed* ped, eAbortPriority priority, const CEvent* event);
    CTask* CreateFirstSubTask_Reversed(CPed* ped);
    CTask* ControlSubTask_Reversed(CPed* ped);
};

VALIDATE_SIZE(CTaskComplexGoToPointAndStandStillTimed, 0x38);