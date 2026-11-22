#pragma once

#include "TaskComplex.h"
#include "Vector.h"
#include "Enums/eMoveState.h"

class NOTSA_EXPORT_VTABLE CTaskComplexGoToPointAndStandStillAndAchieveHeading : public CTaskComplex {
public:
    static constexpr auto Type = TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL_AND_ACHIEVE_HEADING;

    CTaskComplexGoToPointAndStandStillAndAchieveHeading(
        eMoveState     moveState,
        const CVector& targetPos,
        float          desirdHeading,
        float          targetRadius,
        float          changeRateMult,
        float          headingTolerance
    );
    CTaskComplexGoToPointAndStandStillAndAchieveHeading(const CTaskComplexGoToPointAndStandStillAndAchieveHeading&);
    ~CTaskComplexGoToPointAndStandStillAndAchieveHeading() override = default; // 0x668D40

    eTaskType GetTaskType() const override { return Type; }
    CTask*    Clone() const override { return new CTaskComplexGoToPointAndStandStillAndAchieveHeading{ *this }; } // 0x66CFD0
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override;
    CTask*    CreateSubTask(eTaskType taskType);

public:
    static void InjectHooks();

private:
    eMoveState m_MoveState;
    CVector    m_TargetPos;
    float      m_TargetRadius;
    float      m_DesiredHeading;
    float      m_HeadingChangeRateMult;
    float      m_HeadingTolerance;
    bool       m_HasNewTarget : 1 { true };
};
