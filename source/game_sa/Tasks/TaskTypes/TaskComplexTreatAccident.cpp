#include "StdInc.h"

#include "TaskComplexTreatAccident.h"

#include "TaskSimpleGiveCPR.h"
#include "TaskSimpleNone.h"
#include "TaskSimpleAchieveHeading.h"
#include "IKChainManager_c.h"

void CTaskComplexTreatAccident::InjectHooks()
{
    RH_ScopedVirtualClass(CTaskComplexTreatAccident, 0x86F4EC, 11);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedInstall(Constructor, 0x658AB0);
    RH_ScopedInstall(CreateSubTask, 0x659E90);
    RH_ScopedInstall(ComputeHeading, 0x658AF0);
    RH_ScopedVMTInstall(Clone, 0x659A90);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x65A8F0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x65A830);
    RH_ScopedVMTInstall(ControlSubTask, 0x658B90);
}

// 0x658AB0
CTaskComplexTreatAccident* CTaskComplexTreatAccident::Constructor(CAccident* pAcc)
{
    this->CTaskComplexTreatAccident::CTaskComplexTreatAccident(pAcc);
    return this;
}

// 0x658AB0
CTaskComplexTreatAccident::CTaskComplexTreatAccident(CAccident* accident)
{
    m_pAccident = accident;
}

// 0x65A830
CTask* CTaskComplexTreatAccident::CreateNextSubTask(CPed* ped)
{
    eTaskType subTaskId = m_pSubTask->GetTaskType();

    if (subTaskId == TASK_SIMPLE_GIVE_CPR || subTaskId == TASK_NONE)
        return CreateSubTask(TASK_FINISHED, ped);

    if (subTaskId == TASK_SIMPLE_ACHIEVE_HEADING)
    {
        CPed* targetPed = m_pAccident->m_pPed;

        if (targetPed && !targetPed->bFadeOut)
        {
            targetPed->m_nDeathTimeMS = CTimer::GetTimeInMS();
            auto newSubTask = CreateSubTask(TASK_SIMPLE_GIVE_CPR, ped);
            ped->m_fCurrentRotation = ComputeHeading(ped);
            ped->m_fAimingRotation = ped->m_fCurrentRotation;
            return newSubTask;
        }
        else
            return CreateSubTask(TASK_FINISHED, ped);
    }

    return nullptr;
}

// 0x65A8F0
CTask* CTaskComplexTreatAccident::CreateFirstSubTask(CPed* ped)
{
    CPed* targetPed = m_pAccident->m_pPed;

    if (targetPed && !targetPed->bFadeOut)
    {
        targetPed->m_nDeathTimeMS = CTimer::GetTimeInMS();
        ped->Say(CTX_GLOBAL_VICTIM, 0, 1.0F, false, false, false);
        g_ikChainMan.LookAt("TaskTreatAccident", ped, targetPed, 5000, BONE_HEAD, nullptr, true, 0.25F, 500, 3, false);
        return CreateSubTask(TASK_SIMPLE_ACHIEVE_HEADING, ped);
    }
    else
        return CreateSubTask(TASK_NONE, ped);
}

// 0x658B90
CTask* CTaskComplexTreatAccident::ControlSubTask(CPed* ped)
{
    return m_pSubTask;
}

// 0x659A90
CTask* CTaskComplexTreatAccident::Clone() const {
    return new CTaskComplexTreatAccident(m_pAccident);
}

// 0x659E90
CTask* CTaskComplexTreatAccident::CreateSubTask(eTaskType taskType, CPed* ped)
{
    switch (taskType)
    {
    case TASK_SIMPLE_GIVE_CPR:
        return new CTaskSimpleGiveCPR(m_pAccident);
    case TASK_NONE:
        return new CTaskSimpleNone();
    case TASK_SIMPLE_ACHIEVE_HEADING:
        return new CTaskSimpleAchieveHeading(ComputeHeading(ped), 0.5F, 0.2F);
    default:
        return nullptr;
    }
}

// 0x658AF0
float CTaskComplexTreatAccident::ComputeHeading(CPed* ped) {
    const auto injuredPedPos = (m_pAccident->m_pPed->GetBonePosition(BONE_HEAD) + m_pAccident->m_pPed->GetBonePosition(BONE_PELVIS)) * 0.5f;
    return (injuredPedPos - ped->GetPosition()).Heading(true);
}
