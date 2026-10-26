#pragma once

class CEntityScanner;
class CPed;
class CObject;

class CObjectScanner : public CEntityScanner {
public:
    CObjectScanner() : CEntityScanner() { } // 0x606010
    ~CObjectScanner() = default;            // 0x606050

    void ScanForObjectsInRange(CPed& ped);
    CObject* GetClosestObjectInRange() const;
};
