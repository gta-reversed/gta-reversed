#pragma once

#include "TaskComplex.h"
#include "TaskTimer.h"
#include "Vector.h"

class CColSphere;

class NOTSA_EXPORT_VTABLE CTaskComplexAvoidOtherPedWhileWandering : public CTaskComplex {
public:
    CPed*      m_OtherPed{};
    CVector    m_StartPoint{};
    CVector    m_TargetPt{};
    CVector    m_DetourTarget{};
    CVector    m_NewTarget{};
    CTaskTimer m_Timer{};
    CTaskTimer m_DontQuitYetTimer{};
    eMoveState m_MoveState{};
    bool       m_bDoingIK : 1{};
    bool       m_bWantsToQuit : 1{};
    bool       m_bMovingTarget : 1{};
    
public:
    static constexpr auto Type = TASK_COMPLEX_AVOID_OTHER_PED_WHILE_WANDERING;

    CTaskComplexAvoidOtherPedWhileWandering(
        CPed* ped,
        const CVector& targetPoint,
        eMoveState moveState,

        //VVV NOTSA ARGS VVV//
        bool bMovingTarget = false
    );
    ~CTaskComplexAvoidOtherPedWhileWandering() override;

    CTask* Clone() const override;
    CTask* ControlSubTask(CPed* ped) override;
    eTaskType GetTaskType() const override { return Type; }
    bool MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
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
    CTaskComplexAvoidOtherPedWhileWandering* Constructor(CPed* ped, const CVector& targetPoint, eMoveState moveState) {
        this->CTaskComplexAvoidOtherPedWhileWandering::CTaskComplexAvoidOtherPedWhileWandering(ped, targetPoint, moveState);
        return this;
    }

private:
    friend void InjectHooksMain();
    static void InjectHooks();
};

VALIDATE_SIZE(CTaskComplexAvoidOtherPedWhileWandering, 0x60);
