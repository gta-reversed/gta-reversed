#include "StdInc.h"

#include "EventScanner.h"

// 0x605300
CEventScanner::CEventScanner() {
    m_nNextScanTime = CTimer::GetTimeInMS() + CGeneral::GetRandomNumberInRange(3000u); // Originally should be -3000.0f (float value)
}

void CEventScanner::Clear() {
    m_attractorScanner.Clear();
}

// 0x607E30
void CEventScanner::ScanForEvents(CPed& ped) {
    plugin::CallMethod<0x607E30, CEventScanner*, CPed&>(this, ped);
}

// 0x6053D0
void CEventScanner::ScanForEventsNow(const CPed& ped, bool bDontScan) {
    if (bDontScan)
        return;

    auto scanner = &m_vehiclePotentialCollisionScanner;
    if (scanner->m_timer.m_bStarted) { // todo: inlined?
        scanner->m_timer.m_nStartTime = CTimer::GetTimeInMS();
        scanner->m_timer.m_nInterval = -1;
        scanner->m_timer.m_bStarted = true;
    }
    scanner->ScanForVehiclePotentialCollisionEvents(ped, ped.GetIntelligence()->GetVehicleEntities(), 16);
}

// 0x603720
void CVehiclePotentialCollisionScanner::ScanForVehiclePotentialCollisionEvents(const CPed& ped, CEntity** entities, int32 count) {
    plugin::CallMethod<0x603720, CVehiclePotentialCollisionScanner*, const CPed&, CEntity**, int32>(this, ped, entities, count);
}
