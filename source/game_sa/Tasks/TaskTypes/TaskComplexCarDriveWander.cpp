#include "StdInc.h"

#include "TaskComplexCarDriveWander.h"
#include "CarCtrl.h"

// 0x63CB10
CTaskComplexCarDriveWander::CTaskComplexCarDriveWander(CVehicle* vehicle, eCarDrivingStyle carDrivingStyle, float fSpeed) :
    CTaskComplexCarDrive(vehicle, fSpeed, MODEL_INVALID, carDrivingStyle) {
    // NOP
}

// 0x63CB60
void CTaskComplexCarDriveWander::SetUpCar() {
    m_OriginalDrivingStyle = m_Veh->m_autoPilot.m_nCarDrivingStyle;
    m_OriginalMission      = m_Veh->m_autoPilot.m_nCarMission;
    m_OriginalSpeed        = m_Veh->m_autoPilot.m_nCruiseSpeed;
    m_bIsCarSetUp          = true;
    if (!CCarCtrl::bCarIsBeingCreated) {
        CCarCtrl::JoinCarWithRoadSystem(m_Veh);
        m_Veh->SetStatus(STATUS_PHYSICS);
        m_Veh->m_autoPilot.SetCarMission(MISSION_CRUISE);
        m_Veh->m_autoPilot.SetCruiseSpeed((uint8)m_CruiseSpeed);
        m_Veh->m_autoPilot.m_speed            = m_Veh->m_autoPilot.m_nCruiseSpeed;
        m_Veh->m_autoPilot.m_nCarDrivingStyle = static_cast<eCarDrivingStyle>(m_CarDrivingStyle);
    }
    m_Veh->vehicleFlags.bEngineOn            = !m_Veh->vehicleFlags.bEngineBroken;
    m_Veh->m_autoPilot.m_nTimeToStartMission = CTimer::GetTimeInMS();
}

// 0x643240
CTask* CTaskComplexCarDriveWander::CreateSubTaskCannotGetInCar(CPed* ped) {
    return CTaskComplexCarDrive::CreateSubTask(TASK_COMPLEX_WANDER, ped);
}

void CTaskComplexCarDriveWander::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexCarDriveWander, 0x86e96c, 14);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedConstructorInstall(0x63CB10, "1", {}, CVehicle*, eCarDrivingStyle, float);
    RH_ScopedDestructorInstall(0x63CB50);

    RH_ScopedVMTInstall(Clone, 0x63DD00);
    RH_ScopedVMTInstall(GetTaskType, 0x63CB40);
    RH_ScopedVMTInstall(SetUpCar, 0x63CB60);
    RH_ScopedVMTInstall(CreateSubTaskCannotGetInCar, 0x643240);
}
