#pragma once

#include "TaskComplex.h"
#include "TaskTimer.h"
#include "Vector.h"

class CColSphere;

class CTaskComplexAvoidOtherPedWhileWandering : public CTaskComplex {
public:
    CPed*      m_ped;
    CVector    field_10;
    CVector    field_1C;
    CVector    field_28;
    int32      field_34;
    int32      field_38;
    int32      field_3C;
    CTaskTimer field_40;
    CTaskTimer field_4C;
    int32      m_moveState; // see eMoveState
    int8       m_flag1 : 1;
    int8       m_flag2 : 1;
    int8       m_flag3 : 1;
    int8       m_flag4 : 1;
    char       field_5D[3];

public:
    CTaskComplexAvoidOtherPedWhileWandering(CPed* ped, const CVector& targetPoint, int32 moveState);
    ~CTaskComplexAvoidOtherPedWhileWandering();

    CTask* Clone() override;
    CTask* ControlSubTask(CPed* ped) override;
    eTaskType GetTaskType() override { return TASK_COMPLEX_AVOID_OTHER_PED_WHILE_WANDERING; }
    bool MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) override;
    CTask* CreateNextSubTask(CPed* ped) override;
    CTask* CreateFirstSubTask(CPed* ped) override;

    void QuitIK(CPed* ped);
    uint8 NearbyPedsInSphere(CColSphere* colSphere, CPed* ped);
    void SetUpIK(CPed* ped);
    bool ComputeSphere(CPed* ped, CColSphere* colSphere, CPed** pedsToCheck, CPed** pedsInSphere);
    void ComputeAvoidSphere(CPed* ped, CColSphere* colSphere);
    bool ComputeDetourTarget(CPed* ped, CColSphere* colSphere);
    bool ComputeDetourTarget(CPed* ped);

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CTaskComplexAvoidOtherPedWhileWandering* Constructor(CPed* ped, const CVector& targetPoint, int32 moveState);
};

VALIDATE_SIZE(CTaskComplexAvoidOtherPedWhileWandering, 0x60);
