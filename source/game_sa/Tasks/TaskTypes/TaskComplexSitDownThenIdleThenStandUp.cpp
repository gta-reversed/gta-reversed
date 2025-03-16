#include "StdInc.h"
#include "TaskComplexSitDownThenIdleThenStandUp.h"
#include "TaskSimpleSitDown.h"
#include "TaskSimpleSitIdle.h"
#include "TaskSimpleStandUp.h"
#include "TaskSimpleStandStill.h"

void CTaskComplexSitDownThenIdleThenStandUp::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexSitDownThenIdleThenStandUp, 0x86df60, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x631460);
    RH_ScopedInstall(Destructor, 0x6314A0);

    RH_ScopedInstall(CreateSubTask, 0x631550);

    RH_ScopedVMTInstall(Clone, 0x636390);
    RH_ScopedVMTInstall(GetTaskType, 0x631490);
    RH_ScopedVMTInstall(MakeAbortable, 0x631510);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x637DB0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x6316A0);
    RH_ScopedVMTInstall(ControlSubTask, 0x631730);
}

// 0x631460
CTaskComplexSitDownThenIdleThenStandUp::CTaskComplexSitDownThenIdleThenStandUp(int32 durationMs, bool bSitOnStep, bool bInstant) :
    m_durationMs{durationMs},
    m_doInstantly{bInstant},
    m_sitOnStep{bSitOnStep}
{
}

CTaskComplexSitDownThenIdleThenStandUp::CTaskComplexSitDownThenIdleThenStandUp(const CTaskComplexSitDownThenIdleThenStandUp& o) :
    CTaskComplexSitDownThenIdleThenStandUp{ o.m_durationMs, o.m_sitOnStep, o.m_doInstantly }
{
}

// 0x6314A0
CTaskComplexSitDownThenIdleThenStandUp::~CTaskComplexSitDownThenIdleThenStandUp() {
    if (m_animBlockRefAdded) {
        CAnimManager::RemoveAnimBlockRef(CAnimManager::GetAnimationBlockIndex("attractors"));
        m_animBlockRefAdded = false;
    }
}

// 0x631550
CTask* CTaskComplexSitDownThenIdleThenStandUp::CreateSubTask(eTaskType taskType) {
    switch (taskType) {
    case TASK_FINISHED:
        return nullptr;
    case TASK_SIMPLE_SIT_DOWN:
        return new CTaskSimpleSitDown{ m_sitOnStep };
    case TASK_SIMPLE_SIT_IDLE:
        return new CTaskSimpleSitIdle{ m_durationMs, m_sitOnStep };
    case TASK_SIMPLE_STAND_UP:
        return new CTaskSimpleStandUp{ m_sitOnStep };
    default:
        NOTSA_UNREACHABLE();
    }
}

// 0x631510
bool CTaskComplexSitDownThenIdleThenStandUp::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
    if (priority == ABORT_PRIORITY_IMMEDIATE) {
        return m_pSubTask->MakeAbortable(ped, priority, event);
    }

    m_durationMs = 0;
    m_pSubTask->MakeAbortable(ped, priority, event);

    return false;
}

// 0x637DB0
CTask* CTaskComplexSitDownThenIdleThenStandUp::CreateNextSubTask(CPed* ped) {
    switch (m_pSubTask->GetTaskType()) {
    case TASK_SIMPLE_STAND_STILL: {
        if (m_sitOnStep && !m_animBlockRefAdded) {
            return new CTaskSimpleStandStill{ 100 };
        }
        return CreateSubTask(
            m_doInstantly
                ? TASK_SIMPLE_SIT_IDLE
                : TASK_SIMPLE_SIT_DOWN
        );
    }
    case TASK_SIMPLE_SIT_DOWN:
        return CreateSubTask(TASK_SIMPLE_SIT_IDLE);
    case TASK_SIMPLE_SIT_IDLE:
        return CreateSubTask(TASK_SIMPLE_STAND_UP);
    case TASK_SIMPLE_STAND_UP:
        return nullptr;
    default:
        NOTSA_UNREACHABLE();
    }
}

// 0x6316A0
CTask* CTaskComplexSitDownThenIdleThenStandUp::CreateFirstSubTask(CPed* ped) {
    return new CTaskSimpleStandStill{ 100 };
}

// 0x631730
CTask* CTaskComplexSitDownThenIdleThenStandUp::ControlSubTask(CPed* ped) {
    if (!m_animBlockRefAdded && m_sitOnStep) {
        const auto idx = CAnimManager::GetAnimationBlockIndex("attractors");
        if (CAnimManager::GetAnimBlocks()[idx].IsLoaded) {
            CAnimManager::AddAnimBlockRef(idx);
            m_animBlockRefAdded = true;
        } else {
            CStreaming::RequestModel(IFPToModelId(idx), STREAMING_KEEP_IN_MEMORY);
        }
    }
    return m_pSubTask;
}
