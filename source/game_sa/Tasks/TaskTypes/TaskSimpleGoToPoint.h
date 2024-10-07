#pragma once

#include "TaskSimpleGoTo.h"

class NOTSA_EXPORT_VTABLE CTaskSimpleGoToPoint : public CTaskSimpleGoTo {
public:
    union {
        struct {
            uint8 m_bMoveTowardsTargetPoint : 1;
            uint8 m_b02 : 1;
            uint8 m_b03 : 1;
            uint8 m_b04 : 1;
            uint8 m_b05 : 1;
        } gotoPointFlags;
        uint8 m_GoToPointFlags;
    };
    CVector m_vecLastPedPos;

public:
    static constexpr auto Type = TASK_SIMPLE_GO_TO_POINT;

    CTaskSimpleGoToPoint(eMoveState moveState, const CVector& targetPoint, float fRadius = 0.5f, bool bMoveTowardsTargetPoint = false, bool a6 = false);
    ~CTaskSimpleGoToPoint() override = default;

    eTaskType GetTaskType() const override { return Type; }
    CTask* Clone() const override { return new CTaskSimpleGoToPoint(m_moveState, m_vecTargetPoint, m_fRadius, gotoPointFlags.m_bMoveTowardsTargetPoint, gotoPointFlags.m_b04); } // 0x66CC60
    bool MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    bool ProcessPed(CPed* ped) override;

    // bDontCheckRadius is always false
    void UpdatePoint(const CVector& targetPosition, float fRadius = 0.5f, bool bDontCheckRadius = false);

    bool WasTaskSuccessful(CPed* ped) const;

    static void InjectHooks();
    auto Constructor(eMoveState moveState, const CVector& targetPoint, float fRadius, bool bMoveTowardsTargetPoint, bool a6) { this->CTaskSimpleGoToPoint::CTaskSimpleGoToPoint(moveState, targetPoint, fRadius, bMoveTowardsTargetPoint, a6); return this; }
};
VALIDATE_SIZE(CTaskSimpleGoToPoint, 0x30);
