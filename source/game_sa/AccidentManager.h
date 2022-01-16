#pragma once

#include "Accident.h"

const int32 NUM_ACCIDENTS = 16;

class CAccidentManager {
public:
    CAccident m_Accidents[NUM_ACCIDENTS];

    static CAccidentManager*& gAccidentManager;

public:
    static void InjectHooks();

    static CAccidentManager* GetInstance();

    void       ReportAccident(CPed* ped);
    int32      GetNumberOfFreeAccidents();
    CAccident* GetNearestFreeAccidentExceptThisOne(CVector& posn, CAccident* thisOne, bool bIgnoreHeadless);
    CAccident* GetNearestFreeAccident(CVector& posn, bool bIgnoreHeadless);
};

VALIDATE_SIZE(CAccidentManager, 0x80);
