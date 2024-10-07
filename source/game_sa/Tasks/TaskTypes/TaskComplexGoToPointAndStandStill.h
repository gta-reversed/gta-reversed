#pragma once

#include "TaskComplex.h"
#include "Vector.h"

class CTaskSimpleGoToPoint;

class NOTSA_EXPORT_VTABLE CTaskComplexGoToPointAndStandStill : public CTaskComplex {
public:
    eMoveState m_moveState;
    CVector    m_vecTargetPoint;
    float      m_fRadius;          ///< Ped stops moving after it is within the radius of the point.
    float      m_fMoveStateRadius; ///< The ped will either sprint, run, or walk depending on far it is from the radius
    struct {
        bool m_bMustOverShootTarget : 1 {}; // 0x1
        bool m_bStopExactly : 1 {};         // 0x2
        bool m_bNewTarget : 1 {};           // 0x4
        bool m_bTrackingEntity : 1 {};      // 0x8
        bool m_bWasSuccessful : 1 {};       // 0x10
    };

public:
    static constexpr auto Type = TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL;

    CTaskComplexGoToPointAndStandStill(
        eMoveState moveState,
        const CVector& targetPoint,
        float fRadius = 1.5f,
        float fMoveStateRadius = 2.f,
        bool bMustOverShootTarget = false,
        bool bStopExactly = false,
        /* \/\/\/ NOTSA ARGS \/\/\/ */
        bool bTrackingEntity = false
    );
    ~CTaskComplexGoToPointAndStandStill() override;

    eTaskType GetTaskType() const override { return Type; }
    CTask*    Clone() const override;
    CTask* CreateNextSubTask(CPed* ped) override;
    CTask* CreateFirstSubTask(CPed* ped) override;
    CTask* ControlSubTask(CPed* ped) override;

    void   GoToPoint(const CVector& targetPoint, float fRadius = 1.5f, float fMoveStateRadius = 2.f, bool bUpdateTargetEvenIfItsTheSame = false);
    void   SelectMoveState(CTaskSimpleGoToPoint* pGotoPointTask, CPed* ped, float fMoveStateRadius, float fRunOrSprintRadius);
    CTask* CreateSubTask(eTaskType taskType, CPed* ped);

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    // 0x668120
    CTaskComplexGoToPointAndStandStill* Constructor(eMoveState moveState, const CVector& targetPoint, float fRadius, float fMoveStateRadius, bool bUnknown, bool bGoToPoint) {
        this->CTaskComplexGoToPointAndStandStill::CTaskComplexGoToPointAndStandStill(moveState, targetPoint, fRadius, fMoveStateRadius, bUnknown, bGoToPoint);
        return this;
    }
};
VALIDATE_SIZE(CTaskComplexGoToPointAndStandStill, 0x28);
