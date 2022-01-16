#include "StdInc.h"

#include "TaskComplexObserveTrafficLights.h"
#include "TaskSimpleRunAnim.h"
#include "TaskSimpleStandStill.h"

// 0x631790
CTaskComplexObserveTrafficLights::CTaskComplexObserveTrafficLights() : CTaskComplex() {
    dwordC = 0;
    dword10 = 0;
    byte14 = 0;
    byte15 = 0;
}

// 0x6318E0
CTask* CTaskComplexObserveTrafficLights::ControlSubTask(CPed* ped) {
    if (CTrafficLights::LightForPeds() || !m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_LEISURE, nullptr)) {
        return m_pSubTask;
    } else {
        return nullptr;
    }
}

// 0x6317E0
CTask* CTaskComplexObserveTrafficLights::CreateNextSubTask(CPed* ped) {
    if (!m_pSubTask || m_pSubTask->GetTaskType() == TASK_SIMPLE_STAND_STILL)
    {
        return new CTaskSimpleRunAnim(ped->m_nAnimGroup, ANIM_ID_ROADCROSS, 4.0f, false);
    }
    else if (m_pSubTask->GetTaskType() == TASK_SIMPLE_ANIM)
    {
        return new CTaskSimpleStandStill(CGeneral::GetRandomNumberInRange(3000, 5000), false, false, 8.0f);
    }
    return nullptr;
}
