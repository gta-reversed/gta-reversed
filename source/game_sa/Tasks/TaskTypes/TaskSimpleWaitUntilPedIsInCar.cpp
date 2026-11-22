#include "StdInc.h"
#include "TaskSimpleWaitUntilPedIsInCar.h"

void CTaskSimpleWaitUntilPedIsInCar::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleWaitUntilPedIsInCar, 0x86f0b8, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x649620);
    RH_ScopedInstall(Destructor, 0x6496A0);

    RH_ScopedVMTInstall(Clone, 0x64A520);
    RH_ScopedVMTInstall(GetTaskType, 0x649680);
    RH_ScopedVMTInstall(MakeAbortable, 0x649690);
    RH_ScopedVMTInstall(ProcessPed, 0x649700);
}

// 0x649620
CTaskSimpleWaitUntilPedIsInCar::CTaskSimpleWaitUntilPedIsInCar(CPed* pedToWaitFor) :
    m_pedToWaitFor{pedToWaitFor}
{
    CEntity::SafeRegisterRef(m_pedToWaitFor);
}

// 0x6496A0
CTaskSimpleWaitUntilPedIsInCar::~CTaskSimpleWaitUntilPedIsInCar() {
    CEntity::SafeCleanUpRef(m_pedToWaitFor);
}


// 0x649700
bool CTaskSimpleWaitUntilPedIsInCar::ProcessPed(CPed* ped) {
    ped->SetMoveState(PEDMOVE_STILL);

    if (!m_pedToWaitFor) {
        return true;
    }

    if (m_pedToWaitFor->GetTaskManager().Find<TASK_COMPLEX_GO_TO_CAR_DOOR_AND_STAND_STILL>()) {
        m_bIsWalkingToDoor = true;
    } else {
        if (m_bIsWalkingToDoor) {
            return true;
        }
        if (m_pedToWaitFor->GetTaskManager().Find<TASK_SIMPLE_CAR_GET_IN>()) {
            return true;
        }
        if (!m_pedToWaitFor->IsPlayer() && m_pedToWaitFor->bStuckUnderCar) {
            return true;
        }
    }

    return m_pedToWaitFor->bInVehicle;
}
