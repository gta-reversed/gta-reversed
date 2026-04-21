/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "TaskTimer.h"
#include "AttractorScanner.h"

class CPed;

class CPedAcquaintanceScanner {
public:
    static inline auto& ms_fThresholdDotProduct = StaticRef<float>(0xC0B034);

    CTaskTimer m_timer;
    bool m_bScanAllowedScriptPed;
    bool m_bScanAllowedInVehicle;
    bool m_bScanAllowedScriptedTask;
};

class CVehiclePotentialCollisionScanner {
public:
    CTaskTimer m_timer;
    void ScanForVehiclePotentialCollisionEvents(const CPed& ped, CEntity** entities, int32 count);
};

class CObjectPotentialCollisionScanner {
public:
    CTaskTimer m_timer;
};

class CSexyPedScanner {
public:
    CTaskTimer m_timer;
};

class CNearbyFireScanner {
public:
    CTaskTimer m_timer;
};

VALIDATE_SIZE(CPedAcquaintanceScanner, 0x10);
VALIDATE_SIZE(CVehiclePotentialCollisionScanner, 0xC);
VALIDATE_SIZE(CObjectPotentialCollisionScanner, 0xC);
VALIDATE_SIZE(CSexyPedScanner, 0xC);
VALIDATE_SIZE(CNearbyFireScanner, 0xC);

class CEventScanner {
public:
    uint32                            m_nNextScanTime;
    CVehiclePotentialCollisionScanner m_vehiclePotentialCollisionScanner;
    CObjectPotentialCollisionScanner  m_objectPotentialCollisionScanner;
    CAttractorScanner                 m_attractorScanner;
    CPedAcquaintanceScanner           m_pedAcquaintanceScanner;
    CSexyPedScanner                   m_sexyPedScanner;
    CNearbyFireScanner                m_nearbyFireScanner;

    static inline uint32 m_sDeadPedWalkingTimer = *(uint32*)0xC0B038;

public:
    CEventScanner();
    ~CEventScanner() = default;

    void Clear();
    void ScanForEvents(CPed& ped);
    void ScanForEventsNow(const CPed& ped, bool bDontScan);
};

VALIDATE_SIZE(CEventScanner, 0xD4);
