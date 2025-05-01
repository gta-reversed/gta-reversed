#include "StdInc.h"
#include "EventInWater.h"


CEventInWater::CEventInWater(float acceleration) {
    m_acceleration = acceleration;
}

bool CEventInWater::AffectsPed(CPed* ped) {
    CTask* task = ped->GetTaskManager().GetActiveTask();
    if (!ped->IsPlayer() && task && task->GetTaskType() == TASK_COMPLEX_IN_WATER)
        return false;
    return ped->IsAlive();
}

bool CEventInWater::TakesPriorityOver(const CEvent& refEvent) {
    switch (refEvent.GetEventType()) {
    case EVENT_KNOCK_OFF_BIKE:
    case EVENT_DAMAGE:
    case EVENT_STUCK_IN_AIR:
        if (m_acceleration > 1.0f) {
            return true;
        }
        break;
    }
    return CEvent::TakesPriorityOver(refEvent);
}

