#include "StdInc.h"

#include "TaskComplexObserveTrafficLightsAndAchieveHeading.h"

#include "TaskComplexObserveTrafficLights.h"
#include "TaskSimpleAchieveHeading.h"

// 0x631910
CTaskComplexObserveTrafficLightsAndAchieveHeading::CTaskComplexObserveTrafficLightsAndAchieveHeading(int32 durationInMs, float targetHeading) : CTaskComplex() {
    m_nDurationInMs = durationInMs;
    m_fTargetHeading = targetHeading;
}

// 0x636490
CTask* CTaskComplexObserveTrafficLightsAndAchieveHeading::Clone() const {
    return new CTaskComplexObserveTrafficLightsAndAchieveHeading(m_nDurationInMs, m_fTargetHeading);
}

// 0x631AD0
CTask* CTaskComplexObserveTrafficLightsAndAchieveHeading::ControlSubTask(CPed* ped) {
    return m_pSubTask;
}

// 0x631AC0
CTask* CTaskComplexObserveTrafficLightsAndAchieveHeading::CreateFirstSubTask(CPed* ped) {
    return CreateSubTask(TASK_SIMPLE_ACHIEVE_HEADING);
}

// 0x631950
bool CTaskComplexObserveTrafficLightsAndAchieveHeading::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    return (priority == ABORT_PRIORITY_URGENT || priority == ABORT_PRIORITY_IMMEDIATE) && m_pSubTask->MakeAbortable(ped, priority, event);
}

// 0x631A70
CTask* CTaskComplexObserveTrafficLightsAndAchieveHeading::CreateNextSubTask(CPed* ped) {
    switch (m_pSubTask->GetTaskType()) {
    case TASK_COMPLEX_OBSERVE_TRAFFIC_LIGHTS:
        return CreateSubTask(TASK_FINISHED);
    case TASK_SIMPLE_ACHIEVE_HEADING:
        return CreateSubTask(TASK_COMPLEX_OBSERVE_TRAFFIC_LIGHTS);
    default:
        return nullptr;
    }
}

// 0x631990
CTask* CTaskComplexObserveTrafficLightsAndAchieveHeading::CreateSubTask(eTaskType taskType) const {
    switch (taskType) {
    case TASK_COMPLEX_OBSERVE_TRAFFIC_LIGHTS:
        return new CTaskComplexObserveTrafficLights();
    case TASK_SIMPLE_ACHIEVE_HEADING:
        return new CTaskSimpleAchieveHeading(m_fTargetHeading, 0.5f, 0.2f);
    case TASK_FINISHED:
        return nullptr;
    default:
        return nullptr;
    }
}
