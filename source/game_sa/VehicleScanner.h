#pragma once

#include "EntityScanner.h"
#include "Vehicle.h"

class CPed;
class CVehicle;

class CVehicleScanner : public CEntityScanner {
public:
    void ScanForVehiclesInRange(CPed& ped);
    CVehicle* GetClosestVehicleInRange() const;
};
