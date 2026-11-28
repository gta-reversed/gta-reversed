#pragma once

#include "TaskComplex.h"
#include "Vector.h"
#include "TaskTimer.h"

class CVehicle;
enum eMoveState : uint32;

class NOTSA_EXPORT_VTABLE CTaskComplexGoToPointAnyMeans : public CTaskComplex {
public:
    static constexpr auto Type = TASK_COMPLEX_GO_TO_POINT_ANY_MEANS;

    CTaskComplexGoToPointAnyMeans(int32 moveState, const CVector& posn, float radius, int32 modelId);
    CTaskComplexGoToPointAnyMeans(int32 moveState, const CVector& posn, CVehicle* vehicle, float radius, int32 modelId);
    CTaskComplexGoToPointAnyMeans(const CTaskComplexGoToPointAnyMeans& o); // NOTSA
    ~CTaskComplexGoToPointAnyMeans() override;

    eTaskType GetTaskType() const override { return Type; } // 0x66B780
    CTask*    Clone() const override { return new CTaskComplexGoToPointAnyMeans{ *this }; } // 0x66D1E0
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override;

    CTask* CreateSubTask(int32 taskType, CPed* ped);

    static void InjectHooks();

private:
    CVector    m_Pos{};
    eMoveState m_MoveState{};
    float      m_Radius{};
    CVehicle*  m_Vehicle{}; //!< NOTE: Not a `CVehicle::Ref`, see `ControlSubTask`
    int32      m_ModelId{};
    CTaskTimer m_Timer{};
};
VALIDATE_SIZE(CTaskComplexGoToPointAnyMeans, 0x34);
