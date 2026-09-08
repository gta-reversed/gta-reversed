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
    auto& scanner = ped->GetIntelligence()->GetEventScanner().m_pedAcquaintanceScanner;
    scanner.m_bScanAllowedInVehicle    = m_bScanAllowedInVehicle;
    scanner.m_bScanAllowedScriptPed    = m_bScanAllowedScriptPed;
    scanner.m_bScanAllowedScriptedTask = m_bScanAllowedScriptedTask;
    return true;
}

void CTaskSimpleTogglePedThreatScanner::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleTogglePedThreatScanner, 0x85a334, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x492D10);
    RH_ScopedInstall(Destructor, 0x492D40);

    RH_ScopedVMTInstall(Clone, 0x492D50);
    RH_ScopedVMTInstall(GetTaskType, 0x492DD0);
    RH_ScopedVMTInstall(MakeAbortable, 0x492DE0);
    RH_ScopedVMTInstall(ProcessPed, 0x6337A0);
}
