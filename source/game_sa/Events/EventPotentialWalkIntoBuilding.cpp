#include "StdInc.h"
#include "EventPotentialWalkIntoBuilding.h"


CEventPotentialWalkIntoBuilding::CEventPotentialWalkIntoBuilding() : CEventBuildingCollision(0, 0.0f, m_building, &m_impactNormal, &m_impactPos, m_pieceType) {}

bool CEventPotentialWalkIntoBuilding::AffectsPed(CPed* ped) {
    if (ped->IsPlayer() || !ped->IsAlive() || ped->m_pAttachedTo)
        return false;

    if (m_pieceType == 1)
        return false;

    if (ped->GetEventHandler().GetCurrentEventType() != EVENT_BUILDING_COLLISION || m_building != ped->GetEventHandlerHistory().GetCurrentEvent()->GetSourceEntity()                                                                                     // +20
    && (ped->GetEventHandler().GetCurrentEventType() != EVENT_BUILDING_COLLISION || m_building != ped->GetEventHandlerHistory().GetCurrentEvent()->GetSourceEntity())
    ) {
        auto* task = ped->GetTaskManager().GetActiveTask();
        if (!task || task->GetTaskType() != TASK_COMPLEX_REACT_TO_GUN_AIMED_AT) {
            return true;
        }
    }
    return false;
}

