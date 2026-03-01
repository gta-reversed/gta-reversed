#pragma once


#include "TaskTimer.h"
#include "AttractorScanner.h"
#include "EventScanner.h"

class CVehiclePotentialCollisionScanner {
public:
    CTaskTimer m_timer;
    void ScanForVehiclePotentialCollisionEvents(const CPed& ped, CEntity** entities, int32 count);
};

