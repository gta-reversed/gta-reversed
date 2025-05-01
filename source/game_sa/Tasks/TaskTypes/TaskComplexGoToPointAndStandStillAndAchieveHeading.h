#pragma once

#include "TaskComplex.h"

class CVector;

class NOTSA_EXPORT_VTABLE CTaskComplexGoToPointAndStandStillAndAchieveHeading : public CTaskComplex {
private:
    eMoveState m_MoveState;
    CVector    m_TargetPos;
    float      m_Radius;
    float      m_Angle;
    float      m_ChangeRateMult; // var naming?
    float      m_MaxHeading; // var naming?
    uint8      m_nFlags;

public:
    static constexpr auto Type = TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL_AND_ACHIEVE_HEADING;

    CTaskComplexGoToPointAndStandStillAndAchieveHeading(eMoveState moveState, const CVector& targetPos, float angle, float radius, float changeRateMult, float maxHeading);
    ~CTaskComplexGoToPointAndStandStillAndAchieveHeading() override = default; // 0x668D40

    eTaskType GetTaskType() const override { return Type; }
    CTask* Clone() const override { return new CTaskComplexGoToPointAndStandStillAndAchieveHeading(m_MoveState, m_TargetPos, m_Angle, m_Radius, m_ChangeRateMult, m_MaxHeading); } // 0x66CFD0
    CTask* CreateNextSubTask(CPed* ped) override;
    CTask* CreateFirstSubTask(CPed* ped) override;
    CTask* ControlSubTask(CPed* ped) override;
    CTask* CreateSubTask(eTaskType taskType);

private:
    friend void InjectHooksMain();
    static void InjectHooks();
    CTaskComplexGoToPointAndStandStillAndAchieveHeading* Constructor(eMoveState a2, CVector const& a3, float a4, float a5, float a6, float a7) { this->CTaskComplexGoToPointAndStandStillAndAchieveHeading::CTaskComplexGoToPointAndStandStillAndAchieveHeading(a2, a3, a4, a5, a6, a7); return this; }
    CTaskComplexGoToPointAndStandStillAndAchieveHeading* Destructor() { this->CTaskComplexGoToPointAndStandStillAndAchieveHeading::~CTaskComplexGoToPointAndStandStillAndAchieveHeading(); return this; }
};
