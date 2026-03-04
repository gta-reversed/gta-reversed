#pragma once

#include "Base.h"

#include "Accident.h"

static constexpr size_t NUM_ACCIDENTS = 16;

class CAccidentManager {
private:
    CAccident m_Accidents[NUM_ACCIDENTS]{};

public:
    CAccidentManager()  = default;
    ~CAccidentManager() = default;

    void ReportAccident(CPed* injuredPed);
    int32 ComputeNoOfFreeAccidents() const;

    CAccident* GetNearestFreeAccident(const CVector& pos, bool corpseMustHaveHead);
    CAccident* GetNearestFreeAccidentExceptThisOne(const CVector& pos, CAccident* exclude, bool corpseMustHaveHead);

private:
    friend void InjectHooksMain();
    static void InjectHooks();
};

VALIDATE_SIZE(CAccidentManager, 0x80);

CAccidentManager* GetAccidentManager();
