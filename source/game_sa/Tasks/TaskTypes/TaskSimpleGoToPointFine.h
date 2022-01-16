#pragma once

#include "TaskSimpleGoTo.h"

class CTaskSimpleGoToPointFine : public CTaskSimpleGoTo {
public:
    float m_fMoveRatio;

public:
    CTaskSimpleGoToPointFine(float moveRatio, CVector targetPoint, float fRadius, CEntity* entity);
    ~CTaskSimpleGoToPointFine() override = default;

    CTask*    Clone() override;
    eTaskType GetTaskType() override { return TASK_SIMPLE_GO_TO_POINT_FINE; }
    bool      MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) override;
    bool      ProcessPed(CPed* ped) override;

    void  Finish(CPed* ped);

    void  SetBlendedMoveAnim(CPed* ped);
    void  SetTargetPos(CVector posn);
    void  SetMoveRatio(float value) { m_fMoveRatio = value; } // 0x65F3A0
    static float BaseRatio(eMoveState moveState);

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CTaskSimpleGoToPointFine* Constructor(float moveRatio, CVector targetPoint, float fRadius, CEntity* entity);

    CTask* Clone_Reversed();
    bool   MakeAbortable_Reversed(CPed* ped, eAbortPriority priority, const CEvent* event);
    bool   ProcessPed_Reversed(CPed* ped);
};

VALIDATE_SIZE(CTaskSimpleGoToPointFine, 0x24);