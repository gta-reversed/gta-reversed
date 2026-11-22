#pragma once

#include "eCrimeType.h"

class CPed;

class CCrime {
public:
    static void InjectHooks();

    static void ReportCrime(eCrimeType crimeType, CEntity* entity, CPed* ped2);
    static float FindImmediateDetectionRange(eCrimeType CrimeType); // Android
};
