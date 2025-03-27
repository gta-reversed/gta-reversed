#pragma once


#include "TaskTimer.h"
#include "AttractorScanner.h"
#include "EventScanner.h"


class CPedAcquaintanceScanner {
public:
    static float& ms_fThresholdDotProduct;

    CTaskTimer m_timer;
    bool m_bScanAllowedScriptPed;
    bool m_bScanAllowedInVehicle;
    bool m_bScanAllowedScriptedTask;
};

