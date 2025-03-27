#include "StdInc.h"
#include "TaskComplexShuffleSeats.h"
#include "TaskSimpleCarSetPedInAsDriver.h"
#include "TaskSimpleCarShuffle.h"
#include "TaskSimpleCarSetPedInAsPassenger.h"
#include "TaskSimpleCarSetPedOut.h"

void CTaskComplexShuffleSeats::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexShuffleSeats, 0x86ea40, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x63D130);
    RH_ScopedInstall(Destructor, 0x63D1A0);

    RH_ScopedInstall(CreateSubTask, 0x63D240);
    RH_ScopedInstall(CreateTaskUtilityLineUpPedWithCar, 0x63D610);
    RH_ScopedVMTInstall(Clone, 0x63DEB0);
    RH_ScopedVMTInstall(GetTaskType, 0x63D190);
    RH_ScopedVMTInstall(MakeAbortable, 0x63D220);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x63D420);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x643700);
    RH_ScopedVMTInstall(ControlSubTask, 0x63D5C0);
}

// 0x63D130
CTaskComplexShuffleSeats::CTaskComplexShuffleSeats(CVehicle* veh) :
    m_Veh{veh}
{
}

CTaskComplexShuffleSeats::CTaskComplexShuffleSeats(const CTaskComplexShuffleSeats& o) :
    CTaskComplexShuffleSeats{o.m_Veh}
{
}

// 0x63D1A0
CTaskComplexShuffleSeats::~CTaskComplexShuffleSeats() {
    delete m_TaskUtilityLineUpPedWithCar;
}

// 0x63D240
CTask* CTaskComplexShuffleSeats::CreateSubTask(eTaskType taskType, CPed* ped) {
    switch (taskType) {
    case TASK_SIMPLE_CAR_SET_PED_IN_AS_DRIVER:
        return new CTaskSimpleCarSetPedInAsDriver{m_Veh, m_TaskUtilityLineUpPedWithCar};
    case TASK_SIMPLE_CAR_SHUFFLE:
        return new CTaskSimpleCarShuffle{ m_Veh, m_OriginDoor, m_TaskUtilityLineUpPedWithCar };
    case TASK_SIMPLE_CAR_SET_PED_IN_AS_PASSENGER: {
        if (m_Veh->m_pDriver == ped) {
            m_Veh->RemoveDriver(ped);
        } else {
            m_Veh->RemovePassenger(ped);
        }
        return new CTaskSimpleCarSetPedInAsPassenger{ m_Veh, (eTargetDoor)m_TargetDoor, false, m_TaskUtilityLineUpPedWithCar };
    }
    case TASK_SIMPLE_CAR_SET_PED_OUT:
        return new CTaskSimpleCarSetPedOut{ m_Veh, (eTargetDoor)CCarEnterExit::ComputeTargetDoorToExit(m_Veh, ped), true };
    case TASK_FINISHED: {
        if (!ped->bInVehicle) {
            ped->m_bUsesCollision = true;
        }
        return nullptr;
    }
    default:
        NOTSA_UNREACHABLE();
    }
}

// 0x63D610
void CTaskComplexShuffleSeats::CreateTaskUtilityLineUpPedWithCar(CPed* ped) {
    m_OriginDoor = [this, ped] {
        if (m_Veh->IsDriver(ped)) {
            return TARGET_DOOR_DRIVER;
        }
        if (const auto optPsgrIdx = m_Veh->GetPassengerIndex(ped)) {
            switch (*optPsgrIdx) {
            case 0: return TARGET_DOOR_FRONT_RIGHT;
            case 1: return TARGET_DOOR_REAR_LEFT;
            }
        }
        return TARGET_DOOR_REAR_RIGHT;
    }();

    m_TaskUtilityLineUpPedWithCar = new CTaskUtilityLineUpPedWithCar{
        CCarEnterExit::GetPositionToOpenCarDoor(m_Veh, m_OriginDoor) - ped->GetPosition(),
        600,
        0,
        m_OriginDoor
    };
}

// 0x63D220
bool CTaskComplexShuffleSeats::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
    return priority == ABORT_PRIORITY_IMMEDIATE && m_pSubTask->MakeAbortable(ped, priority, event);
}

// 0x63D420
CTask* CTaskComplexShuffleSeats::CreateNextSubTask(CPed* ped) {
    return CreateSubTask([this, ped]{
        if (m_bAbort) {
            switch (m_pSubTask->GetTaskType()) {
            case TASK_SIMPLE_CAR_SET_PED_IN_AS_PASSENGER:
            case TASK_SIMPLE_CAR_SHUFFLE:
            case TASK_SIMPLE_CAR_SET_PED_IN_AS_DRIVER:
                return TASK_SIMPLE_CAR_SET_PED_OUT;
            case TASK_NONE:
            case TASK_SIMPLE_CAR_SET_PED_OUT:
                return TASK_FINISHED;
            default:
                NOTSA_UNREACHABLE();
            }
        }

        switch (m_pSubTask->GetTaskType()) {
        case TASK_SIMPLE_CAR_SET_PED_IN_AS_DRIVER:
        case TASK_SIMPLE_CAR_SET_PED_IN_AS_PASSENGER:
        case TASK_NONE:
            return TASK_FINISHED;
        case TASK_SIMPLE_CAR_SHUFFLE: {
            m_TargetDoor = [this, ped] {
                switch (m_OriginDoor) {
                case TARGET_DOOR_FRONT_RIGHT: return TARGET_DOOR_DRIVER;
                case TARGET_DOOR_REAR_RIGHT:  return TARGET_DOOR_REAR_LEFT;
                case TARGET_DOOR_DRIVER:      return TARGET_DOOR_FRONT_RIGHT;
                case TARGET_DOOR_REAR_LEFT:   return TARGET_DOOR_REAR_RIGHT;
                default:                      NOTSA_UNREACHABLE();
                }
            }();
            if (m_TargetDoor == TARGET_DOOR_DRIVER) { // Inverted
                return m_Veh->m_pDriver
                    ? TASK_FINISHED
                    : TASK_SIMPLE_CAR_SET_PED_IN_AS_DRIVER;
            } else {
                return m_Veh->m_apPassengers[CCarEnterExit::ComputePassengerIndexFromCarDoor(m_Veh, m_TargetDoor)] // Inverted
                    ? TASK_FINISHED
                    : TASK_SIMPLE_CAR_SET_PED_IN_AS_PASSENGER;
            }
        }
        default:
            NOTSA_UNREACHABLE();
        }
    }(), ped);
}

// 0x643700
CTask* CTaskComplexShuffleSeats::CreateFirstSubTask(CPed* ped) {
    return CreateSubTask([this, ped] {
        if (m_Veh && CCarEnterExit::IsVehicleHealthy(m_Veh) && CCarEnterExit::IsPedHealthy(ped)) {
            CreateTaskUtilityLineUpPedWithCar(ped);
            return TASK_SIMPLE_CAR_SHUFFLE;
        } 
        return TASK_NONE;
    }(), ped);
}

// 0x63D5C0
CTask* CTaskComplexShuffleSeats::ControlSubTask(CPed* ped) {
    if (!m_Veh || !CCarEnterExit::IsVehicleHealthy(m_Veh) || !CCarEnterExit::IsPedHealthy(ped)) {
        m_bAbort = true;
        m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_LEISURE);
    }
    return m_pSubTask;
}
