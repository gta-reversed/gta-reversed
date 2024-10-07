#include "StdInc.h"

#include "TaskComplexGoToPointAndStandStillTimed.h"
#include "PedPlacement.h"

void CTaskComplexGoToPointAndStandStillTimed::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexGoToPointAndStandStillTimed, 0x86FDD4, 11);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedInstall(Constructor, 0x6685E0);
    RH_ScopedVMTInstall(Clone, 0x66CF30);
    RH_ScopedVMTInstall(StopTimer, 0x6686A0);
    RH_ScopedVMTInstall(MakeAbortable, 0x668640);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x66DC90);
    RH_ScopedVMTInstall(ControlSubTask, 0x66DCE0);
}

CTaskComplexGoToPointAndStandStillTimed::CTaskComplexGoToPointAndStandStillTimed(eMoveState moveState, const CVector& targetPoint, float fRadius, float fMoveStateRadius, int32 time)
    : CTaskComplexGoToPointAndStandStill(moveState, targetPoint, fRadius, fMoveStateRadius, false, false)
{
    m_nTime = time;
}

CTaskComplexGoToPointAndStandStillTimed::~CTaskComplexGoToPointAndStandStillTimed() {
    // nothing here
}

// 0x66CF30
CTask* CTaskComplexGoToPointAndStandStillTimed::Clone() const {
    return new CTaskComplexGoToPointAndStandStillTimed(m_moveState, m_vecTargetPoint, m_fRadius, m_fMoveStateRadius, m_nTime);
}

// 0x6686A0
void CTaskComplexGoToPointAndStandStillTimed::StopTimer(const CEvent* event) {
    if (!CEventHandler::IsTemporaryEvent(*event))
        m_timer.Pause();
}

// 0x668640
bool CTaskComplexGoToPointAndStandStillTimed::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    bool bSubTaskAbortable = m_pSubTask->MakeAbortable(ped, priority, event);
    if (bSubTaskAbortable && priority == ABORT_PRIORITY_URGENT && (!event || !CEventHandler::IsTemporaryEvent(*event))) {
        m_timer.Pause();
    }
    return bSubTaskAbortable;
}

// 0x66DC90
CTask* CTaskComplexGoToPointAndStandStillTimed::CreateFirstSubTask(CPed* ped) {
    m_timer.Start(m_nTime);
    return CTaskComplexGoToPointAndStandStill::CreateFirstSubTask(ped);
}

// 0x66DCE0
CTask* CTaskComplexGoToPointAndStandStillTimed::ControlSubTask(CPed* ped) {
    if (m_timer.Reset()) {
        if (m_timer.IsOutOfTime() && m_pSubTask->GetTaskType() != TASK_SIMPLE_STAND_STILL && CPedPlacement::FindZCoorForPed(m_vecTargetPoint)) {
            ped->SetPosn(m_vecTargetPoint);
        }
    }
    return CTaskComplexGoToPointAndStandStill::ControlSubTask(ped);
}