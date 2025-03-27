#include "StdInc.h"
#include "EventFireNearby.h"


CEventFireNearby::CEventFireNearby(const CVector& position) : CEventEditableResponse() {
    m_position = position;
}

bool CEventFireNearby::AffectsPed(CPed* ped) {
    return !ped->GetTaskManager().Has<TASK_COMPLEX_EXTINGUISH_FIRES>() && ped->IsAlive();
}

