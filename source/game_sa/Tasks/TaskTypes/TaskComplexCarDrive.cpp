#include "StdInc.h"

#include "TaskComplexCarDrive.h"
#include "TaskComplexEnterCarAsPassenger.h"
#include "TaskComplexEnterCarAsDriver.h"
#include "TaskSimpleCarDrive.h"
#include "TaskComplexWander.h"
#include "TaskComplexEnterAnyCarAsDriver.h"
#include "TaskComplexLeaveAnyCar.h"
#include "TaskSimpleCreateCarAndGetIn.h"

void CTaskComplexCarDrive::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexCarDrive, 0x86E934, 14);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor_0, 0x63C9D0);
    RH_ScopedInstall(Constructor_1, 0x63C940);
    RH_ScopedInstall(Destructor, 0x63CA40);

    RH_ScopedInstall(CreateSubTask, 0x642FA0);

    RH_ScopedInstall(CreateSubTaskCannotGetInCar, 0x643200);
    RH_ScopedVMTInstall(SetUpCar, 0x63CAE0);
    RH_ScopedVMTInstall(Drive, 0x63CAD0);
    RH_ScopedVMTInstall(Clone, 0x63DC90);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x644E20);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x645100);
    RH_ScopedVMTInstall(ControlSubTask, 0x645240);
}

// 0x63C9D0
// asDriver stuff is NOTSA
CTaskComplexCarDrive::CTaskComplexCarDrive(CVehicle* vehicle, bool asDriver) :
    m_bAsDriver{asDriver},
    m_Veh{vehicle}
{
    CEntity::SafeRegisterRef(m_Veh);
}

// 0x63C940
CTaskComplexCarDrive::CTaskComplexCarDrive(CVehicle* vehicle, float speed, eModelID carModelIndexToCreate, eCarDrivingStyle carDrivingStyle) :
    m_CruiseSpeed{speed},
    m_DesiredCarModel{carModelIndexToCreate},
    m_Veh{vehicle},
    m_CarDrivingStyle{static_cast<uint32>(carDrivingStyle)}
{
    CEntity::SafeRegisterRef(m_Veh);
}

CTaskComplexCarDrive::CTaskComplexCarDrive(const CTaskComplexCarDrive& o) :
    CTaskComplexCarDrive{ m_Veh, m_CruiseSpeed, m_DesiredCarModel, static_cast<eCarDrivingStyle>(m_CarDrivingStyle) }
{
    m_bAsDriver = o.m_bAsDriver;
}

// 0x63CA40
CTaskComplexCarDrive::~CTaskComplexCarDrive() {
    if (m_Veh) {
        if (m_bIsCarSetUp) {
            m_Veh->m_autoPilot.m_nCarDrivingStyle = static_cast<eCarDrivingStyle>(m_OriginalDrivingStyle);
            m_Veh->m_autoPilot.m_nCarMission      = static_cast<eCarMission>(m_OriginalMission);
            m_Veh->m_autoPilot.m_nCruiseSpeed     = m_OriginalSpeed;
        }
        CEntity::SafeCleanUpRef(m_Veh);
    }
}

// 0x644E20
CTask* CTaskComplexCarDrive::CreateNextSubTask(CPed* ped) {
    const auto Create = [this, ped](eTaskType tt) {
        return CreateSubTask(tt, ped);
    };
    switch (m_pSubTask->GetTaskType()) {
    case TASK_COMPLEX_ENTER_CAR_AS_PASSENGER: // 0x644E93
        return Create(ped->IsInVehicle() ? TASK_SIMPLE_CAR_DRIVE : TASK_FINISHED);
    case TASK_COMPLEX_ENTER_CAR_AS_DRIVER: // 0x644E65
        return Create(ped->IsInVehicle() ? TASK_SIMPLE_CAR_DRIVE : TASK_COMPLEX_ENTER_ANY_CAR_AS_DRIVER);
    case TASK_SIMPLE_CAR_DRIVE: // 0x644E56
        return CreateFirstSubTask(ped);
    case TASK_COMPLEX_LEAVE_ANY_CAR: { // 0x644EC8
        if (m_Veh) {
            return m_bAsDriver
                ? Create(TASK_COMPLEX_ENTER_CAR_AS_DRIVER)
                : Create(TASK_COMPLEX_ENTER_CAR_AS_PASSENGER);
        } else {
            return m_bAsDriver
                ? Create(TASK_COMPLEX_ENTER_ANY_CAR_AS_DRIVER)
                : Create(TASK_FINISHED);
        }
    }
    case TASK_COMPLEX_ENTER_ANY_CAR_AS_DRIVER: { // 0x644F39
        if (ped->IsInVehicle()) {
            CEntity::ChangeEntityReference(m_Veh, ped->m_pVehicle);
            return Create(TASK_SIMPLE_CAR_DRIVE);
        }
        return m_DesiredCarModel == MODEL_INVALID
            ? CreateSubTaskCannotGetInCar(ped)
            : Create(TASK_SIMPLE_CREATE_CAR_AND_GET_IN);
    }
    case TASK_SIMPLE_CREATE_CAR_AND_GET_IN: { // 0x644FA3
        if (ped->IsInVehicle()) {
            CEntity::ChangeEntityReference(m_Veh, ped->m_pVehicle);
            return Create(TASK_SIMPLE_CAR_DRIVE);
        }
        return CreateSubTaskCannotGetInCar(ped);
    }
    default:
        NOTSA_UNREACHABLE();
    }
}

// 0x645100
CTask* CTaskComplexCarDrive::CreateFirstSubTask(CPed* ped) {
    return CreateSubTask([this, ped] {
        if (!m_Veh) {
            if (ped->IsInVehicle()) {
                CEntity::ChangeEntityReference(m_Veh, ped->m_pVehicle);
                return TASK_SIMPLE_CAR_DRIVE;
            }
            return m_bAsDriver
                ? TASK_COMPLEX_ENTER_ANY_CAR_AS_DRIVER
                : TASK_FINISHED;
        }
        if (ped->IsInVehicle()) {
            return ped->m_pVehicle == m_Veh
                ? TASK_SIMPLE_CAR_DRIVE
                : TASK_COMPLEX_LEAVE_ANY_CAR;
        }
        if (!m_Veh->IsBike()) {
            if (CUpsideDownCarCheck{}.IsCarUpsideDown(m_Veh)) {
                return m_bAsDriver
                    ? TASK_COMPLEX_ENTER_ANY_CAR_AS_DRIVER
                    : TASK_FINISHED;
            }
        }
        return m_bAsDriver
            ? TASK_COMPLEX_ENTER_CAR_AS_DRIVER
            : TASK_COMPLEX_ENTER_CAR_AS_PASSENGER;
    }(), ped);
}

// 0x645240
CTask* CTaskComplexCarDrive::ControlSubTask(CPed* ped) {
    if (ped->IsInVehicle()) {
        switch (m_pSubTask->GetTaskType()) {
        case TASK_SIMPLE_CAR_DRIVE:
            return Drive(ped);
        case TASK_COMPLEX_GO_TO_POINT_ANY_MEANS:
            if (ped->IsInVehicle()) {
                CEntity::ChangeEntityReference(m_Veh, ped->m_pVehicle);
                return CreateSubTask(TASK_SIMPLE_CAR_DRIVE, ped);
            }
        }
    }
    return m_pSubTask;
}

// 0x63CAE0
void CTaskComplexCarDrive::SetUpCar() {
    m_OriginalDrivingStyle = m_Veh->m_autoPilot.m_nCarDrivingStyle;
    m_OriginalMission      = m_Veh->m_autoPilot.m_nCarMission;
    m_OriginalSpeed        = m_Veh->m_autoPilot.m_nCruiseSpeed;
    m_bIsCarSetUp          = true;
}

// 0x642FA0
CTask* CTaskComplexCarDrive::CreateSubTask(eTaskType taskType, CPed* ped) {
    switch (taskType) {
    case TASK_COMPLEX_ENTER_CAR_AS_PASSENGER:
        return new CTaskComplexEnterCarAsPassenger{ m_Veh };
    case TASK_COMPLEX_ENTER_CAR_AS_DRIVER:
        return new CTaskComplexEnterCarAsDriver{ m_Veh };
    case TASK_SIMPLE_CAR_DRIVE:
        SetUpCar();
        return new CTaskSimpleCarDrive{ m_Veh };
    case TASK_COMPLEX_LEAVE_ANY_CAR:
        return new CTaskComplexLeaveAnyCar{ 0, true, false };
    case TASK_COMPLEX_ENTER_ANY_CAR_AS_DRIVER:
        return new CTaskComplexEnterAnyCarAsDriver{};
    case TASK_SIMPLE_CREATE_CAR_AND_GET_IN:
        return new CTaskSimpleCreateCarAndGetIn{ ped->GetPosition(), m_DesiredCarModel };
    case TASK_COMPLEX_WANDER:
        return CTaskComplexWander::GetWanderTaskByPedType(ped);
    case TASK_FINISHED:
        return nullptr;
    default:
        NOTSA_UNREACHABLE();
    }
}
