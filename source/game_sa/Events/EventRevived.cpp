#include "StdInc.h"
#include "EventRevived.h"


bool CEventRevived::AffectsPed(CPed* ped) {
    return !ped->IsCreatedByMission() && !ped->IsAlive();
}

