#include "StdInc.h"

#include "TaskComplexEnterCarAsPassengerTimed.h"
#include "TaskSimpleCarSetPedInAsPassenger.h"
#include "TaskComplexEnterCarAsPassenger.h"

void CTaskComplexEnterCarAsPassengerTimed::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexEnterCarAsPassengerTimed, 0x86e74c, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x63B030);
    RH_ScopedInstall(Destructor, 0x63B0C0);

    RH_ScopedVMTInstall(Clone, 0x63D7E0);
    RH_ScopedVMTInstall(GetTaskType, 0x63B0B0);
    RH_ScopedVMTInstall(StopTimer, 0x63B2E0);
    RH_ScopedVMTInstall(MakeAbortable, 0x63B120);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x63B190);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x640660);
    RH_ScopedVMTInstall(ControlSubTask, 0x63B1A0);
}

// 0x63B030
CTaskComplexEnterCarAsPassengerTimed::CTaskComplexEnterCarAsPassengerTimed(CVehicle* vehicle, uint32 targetSeat, uint32 timeMs, bool bCarryOnAfterFallingOff) :
    m_TargetCar{ vehicle },
    m_TargetSeat{ targetSeat },
    m_Time{ timeMs },
    m_bCarryOnAfterFallingOff{ bCarryOnAfterFallingOff }
{   
}

// 0x63D7E0
CTaskComplexEnterCarAsPassengerTimed::CTaskComplexEnterCarAsPassengerTimed(const CTaskComplexEnterCarAsPassengerTimed& o) :
    CTaskComplexEnterCarAsPassengerTimed{o.m_TargetCar, o.m_TargetSeat, o.m_Time, o.m_bCarryOnAfterFallingOff}
{
    m_MoveState = o.m_MoveState;
}

// 0x63B2E0
void CTaskComplexEnterCarAsPassengerTimed::StopTimer(const CEvent* event) {
    if (!CEventHandler::IsTemporaryEvent(*event)) {
        m_Timer.Pause();
    }
}

// 0x63B120
bool CTaskComplexEnterCarAsPassengerTimed::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
    if (!m_pSubTask->MakeAbortable(ped, priority, event)) {
        return false;
    }
    switch (priority) {
    case ABORT_PRIORITY_IMMEDIATE:
    case ABORT_PRIORITY_URGENT: {
        if (!event || !CEventHandler::IsTemporaryEvent(*event)) {
            m_Timer.Pause();
        }
    }
    }
    return true;
}

// 0x640660
CTask* CTaskComplexEnterCarAsPassengerTimed::CreateFirstSubTask(CPed* ped) {
    if (!m_TargetCar) {
        return nullptr;
    }

    m_Timer.Start(m_Time);

    const auto tEnterCar = new CTaskComplexEnterCarAsPassenger{m_TargetCar, (int32)m_TargetSeat, m_bCarryOnAfterFallingOff};
    tEnterCar->SetMoveState(m_MoveState);
    return tEnterCar;
}

// 0x63B1A0
CTask* CTaskComplexEnterCarAsPassengerTimed::ControlSubTask(CPed* ped) {
    if (!m_TargetCar) {
        return nullptr;
    }
    if (m_Timer.IsOutOfTime() && m_pSubTask->MakeAbortable(ped) && !ped->bInVehicle) {
        if (   m_TargetSeat == 0 && m_TargetCar->m_nNumPassengers < m_TargetCar->m_nMaxPassengers
            || !m_TargetCar->GetPassengers()[CCarEnterExit::ComputePassengerIndexFromCarDoor(m_TargetCar, m_TargetSeat)]
        ) {
            return new CTaskSimpleCarSetPedInAsPassenger{m_TargetCar, (eTargetDoor)m_TargetSeat, true};
        }
    }
    return m_pSubTask;
}
