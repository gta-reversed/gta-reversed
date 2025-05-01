#include "StdInc.h"
#include "TaskComplexEvasiveDiveAndGetUp.h"
#include "TaskSimpleEvasiveDive.h"
#include "TaskSimplePause.h"
#include "TaskSimpleGetUp.h"
#include "TaskSimpleAchieveHeading.h"

void CTaskComplexEvasiveDiveAndGetUp::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexEvasiveDiveAndGetUp, 0x86f268, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x6536B0);
    RH_ScopedInstall(Destructor, 0x653740);

    RH_ScopedInstall(CreateSubTask, 0x653820);
    RH_ScopedVMTInstall(Clone, 0x655940);
    RH_ScopedVMTInstall(GetTaskType, 0x653730);
    RH_ScopedVMTInstall(MakeAbortable, 0x6537A0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x656000);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x6560A0);
    RH_ScopedVMTInstall(ControlSubTask, 0x653810);
}

// 0x6536B0
CTaskComplexEvasiveDiveAndGetUp::CTaskComplexEvasiveDiveAndGetUp(CVehicle* evadeVeh, int32 unconsciousTime, const CVector& diveDir, bool bAchieveHeadingFirst) :
    m_UnconsciousTime{unconsciousTime},
    m_bAchieveHeadingFirst{bAchieveHeadingFirst},
    m_DiveDir{diveDir},
    m_EvadeVeh{evadeVeh}
{
}

// 0x655940
CTaskComplexEvasiveDiveAndGetUp::CTaskComplexEvasiveDiveAndGetUp(const CTaskComplexEvasiveDiveAndGetUp& o) :
    CTaskComplexEvasiveDiveAndGetUp{ o.m_EvadeVeh, o.m_UnconsciousTime, o.m_DiveDir, o.m_bAchieveHeadingFirst }
{
}

// 0x653820
CTask* CTaskComplexEvasiveDiveAndGetUp::CreateSubTask(eTaskType taskType) {
    switch (taskType) {
    case TASK_SIMPLE_EVASIVE_DIVE:
        return new CTaskSimpleEvasiveDive{ m_EvadeVeh };
    case TASK_SIMPLE_PAUSE:
        return new CTaskSimplePause{ m_UnconsciousTime };
    case TASK_SIMPLE_GET_UP:
        return new CTaskSimpleGetUp{};
    case TASK_SIMPLE_ACHIEVE_HEADING:
        return new CTaskSimpleAchieveHeading{ m_DiveDir.Heading(), 2.f };
    case TASK_FINISHED:
        return nullptr;
    default:
        NOTSA_UNREACHABLE("Invalid TaskType: {}", taskType);
    }
}

// 0x6537A0
bool CTaskComplexEvasiveDiveAndGetUp::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
    const auto subTaskAborted = m_pSubTask->MakeAbortable(ped, priority, event);
    if (subTaskAborted && priority == ABORT_PRIORITY_IMMEDIATE) {
        if (const auto anim = RpAnimBlendClumpGetAssociation(ped->m_pRwClump, ANIM_ID_EV_DIVE)) {
            anim->SetBlendDelta(-1000.f);
        }
    }
    return subTaskAborted;
}

// 0x656000
CTask* CTaskComplexEvasiveDiveAndGetUp::CreateNextSubTask(CPed* ped) {
    switch (const auto subtt = m_pSubTask->GetTaskType()) {
    case TASK_SIMPLE_ACHIEVE_HEADING:
        return CreateSubTask(TASK_SIMPLE_EVASIVE_DIVE);
    case TASK_SIMPLE_EVASIVE_DIVE:
        return CreateSubTask(TASK_SIMPLE_PAUSE);
    case TASK_SIMPLE_PAUSE: {
        ped->m_fCurrentRotation = ped->m_fAimingRotation = CGeneral::LimitRadianAngle(ped->m_fCurrentRotation - HALF_PI);
        return CreateSubTask(TASK_SIMPLE_GET_UP);
    }
    case TASK_SIMPLE_GET_UP:
        return CreateSubTask(TASK_FINISHED);
    default:
        NOTSA_UNREACHABLE("Invalid SubTask: {}", subtt);
    }
}

// 0x6560A0
CTask* CTaskComplexEvasiveDiveAndGetUp::CreateFirstSubTask(CPed* ped) {
    return CreateSubTask(m_bAchieveHeadingFirst ? TASK_SIMPLE_ACHIEVE_HEADING : TASK_SIMPLE_EVASIVE_DIVE);
}
