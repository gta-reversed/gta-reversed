#include "StdInc.h"
#include "TaskSimpleWaitForBus.h"

void CTaskSimpleWaitForBus::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleWaitForBus, 0x86e1b8, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x632A50);
    RH_ScopedInstall(Destructor, 0x632A90);

    RH_ScopedVMTInstall(Clone, 0x636AD0);
    RH_ScopedVMTInstall(GetTaskType, 0x632A70);
    RH_ScopedVMTInstall(MakeAbortable, 0x632A80);
    RH_ScopedVMTInstall(ProcessPed, 0x632AA0);
}

CTaskSimpleWaitForBus::CTaskSimpleWaitForBus(const CTaskSimpleWaitForBus& o) :
    CTaskSimpleWaitForBus{}
{
}

// 0x632AA0
bool CTaskSimpleWaitForBus::ProcessPed(CPed* ped) {
    m_bus = [this, ped]() -> CVehicle* {
        for (auto& veh : ped->GetIntelligence()->GetVehicleScanner().GetEntities<CVehicle>()) {
            if (veh.vehicleFlags.bIsBus) {
                if (veh.HasSpaceForAPassenger()) {
                    if (sq(0.005f) >= veh.m_vecMoveSpeed.SquaredMagnitude()) {
                        return &veh;
                    }
                }
            }
        }
        return nullptr;
    }();

    return true;
}
