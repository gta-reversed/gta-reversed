#pragma once

class CEntityScanner;

class CPedScanner : public CEntityScanner {
public:
    void ScanForPedsInRange(CPed& ped);
    CPed* GetClosestPedInRange() const;
};
