#include "StdInc.h"
#include "EventInWater.h"

CEventInWater::CEventInWater(float acceleration) :
    m_acceleration{ acceleration } {
}

bool CEventInWater::AffectsPed(CPed* ped) {
    CTask* task = ped->GetTaskManager().GetActiveTask();
    if (!ped->IsPlayer() && task && task->GetTaskType() == TASK_COMPLEX_IN_WATER) {
        return false;
    }
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

void CEventInWater::InjectHooks() {
    RH_ScopedVirtualClass(CEventInWater, 0x85b4c8, 16);
    RH_ScopedCategory("Events");

    RH_ScopedConstructorInstall(0x4B1370, "1", {}, float);
    RH_ScopedDestructorInstall(0x4B13C0);

    RH_ScopedVMTInstall(GetEventType, 0x4B1390);
    RH_ScopedVMTInstall(GetEventPriority, 0x4B13B0);
    RH_ScopedVMTInstall(GetLifeTime, 0x4B13A0);
    RH_ScopedVMTInstall(Clone, 0x4B7810);
    RH_ScopedVMTInstall(AffectsPed, 0x4B13D0);
    RH_ScopedVMTInstall(TakesPriorityOver, 0x4B1420);
}
