#include "StdInc.h"

#include "TaskSimpleTogglePedThreatScanner.h"

// 0x492D10
CTaskSimpleTogglePedThreatScanner::CTaskSimpleTogglePedThreatScanner(bool bScanAllowedScriptPed, bool bScanAllowedInVehicle, bool bScanAllowedScriptedTask)
    : CTaskSimple()
{
    m_bScanAllowedScriptPed = bScanAllowedScriptPed;
    m_bScanAllowedInVehicle = bScanAllowedInVehicle;
    m_bScanAllowedScriptedTask = bScanAllowedScriptedTask;
}

// 0x6337A0
bool CTaskSimpleTogglePedThreatScanner::ProcessPed(CPed* ped) {
    auto& scanner = ped->GetIntelligence()->GetEventScanner().GetPedAcquaintanceScanner();
    scanner.m_bScanAllowedInVehicle    = m_bScanAllowedInVehicle;
    scanner.m_bScanAllowedScriptPed    = m_bScanAllowedScriptPed;
    scanner.m_bScanAllowedScriptedTask = m_bScanAllowedScriptedTask;
    return true;
}
