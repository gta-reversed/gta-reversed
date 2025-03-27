#pragma once

#include "TaskComplex.h"
#include "Vector.h"

class CVehicle;
enum eMoveState : uint32;

class NOTSA_EXPORT_VTABLE CTaskComplexGoToPointAnyMeans : public CTaskComplex {
public:
    CVector    m_Pos;
    eMoveState m_MoveState;
    float      m_fRadius;
    CVehicle*  m_Vehicle;
    int32      m_nModelId;
    uint32     m_nStartTimeInMs;
    uint32     m_nTimeOffsetInMs;
    bool       m_bRefreshTime;
    bool       m_bResetStartTime;

public:
    static constexpr auto Type = TASK_COMPLEX_GO_TO_POINT_ANY_MEANS;

    CTaskComplexGoToPointAnyMeans(int32 moveState, const CVector& posn, float radius, int32 modelId);
    CTaskComplexGoToPointAnyMeans(int32 moveState, const CVector& posn, CVehicle* vehicle, float radius, int32 modelId);
    ~CTaskComplexGoToPointAnyMeans() override;

    eTaskType GetTaskType() const override { return Type; } // 0x66B780
    CTask* Clone() const override { return new CTaskComplexGoToPointAnyMeans(m_MoveState, m_Pos, m_Vehicle, m_fRadius, m_nModelId); } // 0x66D1E0
    CTask* CreateNextSubTask(CPed* ped) override;
    CTask* CreateFirstSubTask(CPed* ped) override;
    CTask* ControlSubTask(CPed* ped) override;

    CTask* CreateSubTask(int32 taskType, CPed* ped);

private:
    friend void InjectHooksMain();
    static void InjectHooks();
    CTaskComplexGoToPointAnyMeans* Constructor(int32 a2, CVector const& posn, float a4, int32 a5) { this->CTaskComplexGoToPointAnyMeans::CTaskComplexGoToPointAnyMeans(a2, posn, a4, a5); return this; }
    CTaskComplexGoToPointAnyMeans* Constructor(int32 moveState, CVector const& posn, CVehicle* vehicle, float radius, int32 a6) { this->CTaskComplexGoToPointAnyMeans::CTaskComplexGoToPointAnyMeans(moveState, posn, vehicle, radius, a6); return this; }
    CTaskComplexGoToPointAnyMeans* Destructor() { this->CTaskComplexGoToPointAnyMeans::~CTaskComplexGoToPointAnyMeans(); return this; }
};
VALIDATE_SIZE(CTaskComplexGoToPointAnyMeans, 0x34);
