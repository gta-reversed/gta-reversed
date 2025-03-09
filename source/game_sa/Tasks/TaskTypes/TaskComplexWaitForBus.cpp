#include "StdInc.h"
#include "TaskComplexWaitForBus.h"
#include "TaskSimpleWaitForBus.h"
#include "TaskComplexEnterCarAsPassenger.h"

void CTaskComplexWaitForBus::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexWaitForBus, 0x86e18c, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x632900);

    RH_ScopedInstall(Destructor, 0x632930);

    RH_ScopedVMTInstall(Clone, 0x636A60, {.reversed = false});
    RH_ScopedVMTInstall(GetTaskType, 0x632920, {.reversed = false});
    RH_ScopedVMTInstall(CreateNextSubTask, 0x632940, {.reversed = false});
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x638940, {.reversed = false});
    RH_ScopedVMTInstall(ControlSubTask, 0x632A40, {.reversed = false});
}

CTaskComplexWaitForBus::CTaskComplexWaitForBus(const CTaskComplexWaitForBus& o) :
    CTaskComplexWaitForBus{}
{
}

// 0x632940
CTask* CTaskComplexWaitForBus::CreateNextSubTask(CPed* ped) {
    switch (m_pSubTask->GetTaskType()) {
    case TASK_SIMPLE_WAIT_FOR_BUS: {
        m_bus = notsa::cast<CTaskSimpleWaitForBus>(m_pSubTask)->m_bus;
        return new CTaskComplexEnterCarAsPassenger{ m_bus, 0, false };
    }
    case TASK_COMPLEX_ENTER_CAR_AS_PASSENGER: {
        ped->m_bRemoveFromWorld = true;
        ped->bDontDragMeOutCar = true;


        if (const auto player = FindPlayerPed()) {
            if (m_bus->IsPassenger(player) || m_bus->IsDriver(player)) { // NOTE: Why complicate it so much? `ped->IsInVehicle(player)` ?
                assert(player->IsInVehicle(m_bus)); // This should always hold true. If so, replace above with this
                player->m_pPlayerData += 5;
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
