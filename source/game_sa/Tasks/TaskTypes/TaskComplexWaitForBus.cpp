#include "StdInc.h"
#include "TaskComplexWaitForBus.h"
#include "TaskSimpleWaitForBus.h"
#include "TaskComplexEnterCarAsPassenger.h"


// For 0x636A60
CTaskComplexWaitForBus::CTaskComplexWaitForBus(const CTaskComplexWaitForBus& o) :
    CTaskComplexWaitForBus{}
{
}

// 0x632940
CTask* CTaskComplexWaitForBus::CreateNextSubTask(CPed* ped) {
    switch (m_pSubTask->GetTaskType()) {
    case TASK_SIMPLE_WAIT_FOR_BUS: {
        m_BusToWaitFor = notsa::cast<CTaskSimpleWaitForBus>(m_pSubTask)->m_bus;
        return new CTaskComplexEnterCarAsPassenger{ m_BusToWaitFor, 0, false };
    }
    case TASK_COMPLEX_ENTER_CAR_AS_PASSENGER: {
        ped->m_bRemoveFromWorld = true;
        ped->bDontDragMeOutCar = true;

        if (const auto player = FindPlayerPed()) {
            if (m_BusToWaitFor->IsPassenger(player) || m_BusToWaitFor->IsDriver(player)) { // NOTE: Why complicate it so much? `ped->IsInVehicle(player)` ?
                assert(player->IsInVehicle(m_BusToWaitFor)); // This should always hold true. If so, replace above with this
                player->GetPlayerData()->m_nBusFaresCollected += 5; // TODO: Fix bugs.
            }
        }

        return nullptr; // Got on the bus, so task is finito
    }
    default:
        NOTSA_UNREACHABLE();
    }
}

// 0x638940
CTask* CTaskComplexWaitForBus::CreateFirstSubTask(CPed* ped) {
    return new CTaskSimpleWaitForBus{};
}

void CTaskComplexWaitForBus::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexWaitForBus, 0x86e18c, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedConstructorInstall(0x632900, "0", {});
    RH_ScopedDestructorInstall(0x632930);

    RH_ScopedVMTInstall(Clone, 0x636A60);
    RH_ScopedVMTInstall(GetTaskType, 0x632920);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x632940);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x638940);
    RH_ScopedVMTInstall(ControlSubTask, 0x632A40);
}
