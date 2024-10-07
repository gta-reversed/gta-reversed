#include "StdInc.h"
#include "TaskComplexCarDriveMission.h"
#include "CarCtrl.h"

void CTaskComplexCarDriveMission::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexCarDriveMission, 0x86E9A4, 14);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x63CC30);
    RH_ScopedInstall(Destructor, 0x63CCC0);
    RH_ScopedVMTInstall(Clone, 0x63DD70);
    RH_ScopedVMTInstall(GetTaskType, 0x63CCB0);
    RH_ScopedVMTInstall(SetUpCar, 0x63CD20);
}

// 0x63CC30
CTaskComplexCarDriveMission::CTaskComplexCarDriveMission(CVehicle* vehicle, CEntity* targetVehicle, eCarMission carDriveMission, eCarDrivingStyle carDrivingStyle, float fSpeed) :
    CTaskComplexCarDrive{ vehicle, fSpeed, MODEL_INVALID, carDrivingStyle }
{
    // assert(targetVehicle->IsVehicle());
    m_pTargetVehicle = static_cast<CVehicle*>(targetVehicle);
    m_nCarMission    = carDriveMission;
    CEntity::SafeRegisterRef(m_pTargetVehicle);
}

// 0x63CCC0
CTaskComplexCarDriveMission::~CTaskComplexCarDriveMission() {
    CEntity::SafeCleanUpRef(m_pTargetVehicle);
}

// 0x63CD20
void CTaskComplexCarDriveMission::SetUpCar() {
    CTaskComplexCarDrive::SetUpCar();

    CCarCtrl::JoinCarWithRoadSystem(m_Veh);
    m_Veh->m_nStatus = STATUS_PHYSICS;

    auto& autopilot = m_Veh->m_autoPilot;
    autopilot.m_nCarMission         = m_nCarMission;
    autopilot.m_nCruiseSpeed        = (uint32)m_CruiseSpeed;
    autopilot.m_speed               = (float)autopilot.m_nCruiseSpeed;
    autopilot.m_nCarDrivingStyle    = (eCarDrivingStyle)m_CarDrivingStyle;
    autopilot.m_pTargetCar          = m_pTargetVehicle;
    autopilot.m_nTimeToStartMission = CTimer::GetTimeInMS();

    CEntity::SafeRegisterRef(autopilot.m_pTargetCar);

    m_Veh->vehicleFlags.bEngineOn = m_Veh->vehicleFlags.bEngineBroken ? false : true;
}
