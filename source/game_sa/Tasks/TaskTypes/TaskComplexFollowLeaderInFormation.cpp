#include "StdInc.h"

#include "TaskComplexFollowLeaderInFormation.h"

void CTaskComplexFollowLeaderInFormation::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexFollowLeaderInFormation, 0x870c3c, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x6949A0);
    RH_ScopedInstall(Destructor, 0x694A40);
    RH_ScopedInstall(CreateSubTask, 0x6962A0, { .Reversed = false });

    RH_ScopedVMTInstall(Clone, 0x695740);
    RH_ScopedVMTInstall(GetTaskType, 0x694A30);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x696820);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x6968E0);
    RH_ScopedVMTInstall(ControlSubTask, 0x696940, { .Reversed = false });
}

// 0x6949A0
CTaskComplexFollowLeaderInFormation::CTaskComplexFollowLeaderInFormation(CPedGroup* pedGroup, CPed* ped, const CVector& posn, float dist) :
    m_Group{pedGroup},
    m_Leader{ped},
    m_Pos{posn},
    m_MoveState{PEDMOVE_WALK},
    m_Dist{dist}
{
    CEntity::SafeRegisterRef(m_Leader);
}

// For 0x695740
CTaskComplexFollowLeaderInFormation::CTaskComplexFollowLeaderInFormation(const CTaskComplexFollowLeaderInFormation& o) :
    CTaskComplexFollowLeaderInFormation{
        o.m_Group,
        o.m_Leader,
        o.m_Pos,
        o.m_Dist
    }
{
}

// 0x694A40
CTaskComplexFollowLeaderInFormation::~CTaskComplexFollowLeaderInFormation() {
    CEntity::SafeCleanUpRef(m_Leader);
}

// 0x696820
CTask* CTaskComplexFollowLeaderInFormation::CreateNextSubTask(CPed* ped) {
    const auto leaderMoveState = m_Leader->m_nMoveState;
    switch (m_pSubTask->GetTaskType()) {
    case TASK_COMPLEX_SEEK_ENTITY:
        return CreateSubTask(
            leaderMoveState == PEDMOVE_NONE || leaderMoveState == PEDMOVE_STILL
                ? TASK_SIMPLE_STAND_STILL
                : TASK_COMPLEX_SEEK_ENTITY,
            ped
        );
    case TASK_SIMPLE_STAND_STILL:
    case TASK_COMPLEX_LEAVE_CAR:
    case TASK_SIMPLE_CAR_DRIVE:
        return CreateSubTask(TASK_COMPLEX_SEEK_ENTITY, ped);
    case TASK_COMPLEX_ENTER_CAR_AS_PASSENGER:
        return CreateFirstSubTask(ped);
    case TASK_SIMPLE_PAUSE:
        return CreateSubTask(TASK_SIMPLE_PAUSE, ped);
    default:
        return nullptr;
    }
}

// 0x6968E0
CTask* CTaskComplexFollowLeaderInFormation::CreateFirstSubTask(CPed* ped) {
    if (ped->bInVehicle && ped->m_pVehicle) {
        return CreateSubTask(
            m_Leader->bInVehicle && m_Leader->m_pVehicle == ped->m_pVehicle
                ? TASK_SIMPLE_CAR_DRIVE
                : TASK_COMPLEX_LEAVE_CAR,
            ped
        );
    }
    return CreateSubTask(TASK_COMPLEX_SEEK_ENTITY, ped);
}

// 0x696940
CTask* CTaskComplexFollowLeaderInFormation::ControlSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x696940, CTaskComplexFollowLeaderInFormation*, CPed*>(this, ped);
}

CTask* CTaskComplexFollowLeaderInFormation::CreateSubTask(eTaskType taskType, CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x6962A0, CTaskComplexFollowLeaderInFormation*, eTaskType, CPed*>(this, taskType, ped);
}
