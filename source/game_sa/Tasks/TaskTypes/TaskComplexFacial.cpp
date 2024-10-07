#include "StdInc.h"

#include "TaskComplexFacial.h"
#include "TaskSimplePause.h"

void CTaskComplexFacial::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexFacial, 0x870ADC, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x690D20);

    RH_ScopedInstall(SetRequest, 0x691230);
    RH_ScopedInstall(StopAll, 0x691250);
    RH_ScopedVMTInstall(Clone, 0x6928B0);
    RH_ScopedVMTInstall(GetTaskType, 0x690D80);
    RH_ScopedVMTInstall(MakeAbortable, 0x690DA0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x690DC0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x690F30);
    RH_ScopedVMTInstall(ControlSubTask, 0x690FC0);
}

// 0x691250
void CTaskComplexFacial::StopAll() {
    if (m_pSubTask->GetTaskType() == TASK_SIMPLE_FACIAL) {
        m_IsAborting = true;
    }
}

// 0x691230
void CTaskComplexFacial::SetRequest(eFacialExpression typeA, int32 durA, eFacialExpression typeB, int32 durB) {
    m_RequestA  = typeA;
    m_DurationA = durA;

    m_RequestB  = typeB;
    m_DurationB = durB;
}

// 0x690DA0
bool CTaskComplexFacial::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    return priority == ABORT_PRIORITY_IMMEDIATE && m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_IMMEDIATE, event);
}

// 0x690DC0
CTask* CTaskComplexFacial::CreateNextSubTask(CPed* ped) {
    if (!ped->IsAlive() && m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_IMMEDIATE)) {
        return new CTaskSimplePause{ 5'000 };
    }
    if (m_RequestB == eFacialExpression::NONE) {
        if (!CTask::IsA<CTaskSimpleFacial>(m_pSubTask) && m_IsChewing) {
            return new CTaskSimpleFacial{ eFacialExpression::CHEWING, 5'000 };
        }
        return new CTaskSimplePause{ 5'000 };
    }
    return new CTaskSimpleFacial{ std::exchange(m_RequestB, eFacialExpression::NONE), m_DurationB }; // Play the next facial expression
}

// 0x690F30
CTask* CTaskComplexFacial::CreateFirstSubTask(CPed* ped) {
    if (ped->IsPlayer()) {
        m_IsChewing = false;
    }
    return new CTaskSimplePause{ 5'000 };
}

// 0x690FC0
CTask* CTaskComplexFacial::ControlSubTask(CPed* ped) {
    if (!ped->IsAlive() && m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_IMMEDIATE)) {
        return new CTaskSimplePause{ 5'000 };
    }

    if (m_IsAborting && m_pSubTask->MakeAbortable(ped)) {
        m_IsAborting       = false;
        m_TalkingLastFrame = false;
        return new CTaskSimplePause{ 5'000 };
    }

    if (RpAnimBlendClumpGetAssociation(ped->m_pRwClump, ANIM_ID_IDLE_CHAT)) {
        m_RequestB         = eFacialExpression::NONE;
        m_TalkingLastFrame = true;
        return RpAnimBlendClumpGetAssociation(ped->m_pRwClump, ANIM_ID_FACTALK)
            ? m_pSubTask
            : new CTaskSimpleFacial{ eFacialExpression::TALKING, 0 };
    }

    if (m_TalkingLastFrame && m_pSubTask->MakeAbortable(ped)) {
        m_TalkingLastFrame = false;
        return new CTaskSimplePause{ 5'000 };
    }

    if (m_RequestA != eFacialExpression::NONE) { // Check for a new request...
        const auto requestedFacialType = std::exchange(m_RequestA, eFacialExpression::NONE);
        if (const auto st = CTask::DynCast<CTaskSimpleFacial>(m_pSubTask)) { // There's a sub-task already running, just change it
            if (st->GetFacialType() == requestedFacialType) {
                st->StartTimer(m_DurationA);
                return m_pSubTask;
            }
        }
        return new CTaskSimpleFacial{ requestedFacialType, m_DurationA }; // Otherwise create the task
    }

    return m_pSubTask;
}
