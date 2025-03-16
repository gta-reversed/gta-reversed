#pragma once

#include "TaskComplexCarDrive.h"
#include "eCarMission.h"
#include "eCarDrivingStyle.h"

class CVehicle;

class NOTSA_EXPORT_VTABLE CTaskComplexCarDriveMission : public CTaskComplexCarDrive {
public:
    CVehicle*   m_pTargetVehicle;
    eCarMission m_nCarMission;

public:
    static constexpr auto Type = TASK_COMPLEX_CAR_DRIVE_MISSION;

    CTaskComplexCarDriveMission(CVehicle* vehicle, CEntity* targetVehicle, eCarMission carDriveMission, eCarDrivingStyle carDrivingStyle, float fSpeed);
    ~CTaskComplexCarDriveMission() override;

    CTask* Clone() const override { return new CTaskComplexCarDriveMission(m_Veh, m_pTargetVehicle, m_nCarMission, (eCarDrivingStyle)m_CarDrivingStyle, m_CruiseSpeed); }
    eTaskType GetTaskType() const override { return Type; }
    void SetUpCar() override;

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CTaskComplexCarDriveMission* Constructor(CVehicle* vehicle, CEntity* targetVehicle, eCarMission carDriveMission, eCarDrivingStyle carDrivingStyle, float fSpeed) { this->CTaskComplexCarDriveMission::CTaskComplexCarDriveMission(vehicle, targetVehicle, carDriveMission, carDrivingStyle, fSpeed); return this; }
    CTaskComplexCarDriveMission* Destructor() { this->CTaskComplexCarDriveMission::~CTaskComplexCarDriveMission(); return this; }
};
VALIDATE_SIZE(CTaskComplexCarDriveMission, 0x2C);

class NOTSA_EXPORT_VTABLE CTaskComplexCarDriveMissionFleeScene : public CTaskComplexCarDriveMission {
public:
    explicit CTaskComplexCarDriveMissionFleeScene(CVehicle* vehicle) : CTaskComplexCarDriveMission(vehicle, nullptr, MISSION_CRUISE, DRIVING_STYLE_AVOID_CARS, 40.0f) { }; // 0x4B8920
    ~CTaskComplexCarDriveMissionFleeScene() override = default; // 0x4B89F0

    static constexpr auto Type = TASK_COMPLEX_CAR_DRIVE_MISSION_FLEE_SCENE;
    eTaskType GetTaskType() const override { return Type; } // 0x4B89C0
    CTask* Clone() const override { return new CTaskComplexCarDriveMissionFleeScene(m_Veh); } // 0x4B8950
};

class NOTSA_EXPORT_VTABLE CTaskComplexCarDriveMissionKillPed : public CTaskComplexCarDriveMission {
public:
    CTaskComplexCarDriveMissionKillPed(CVehicle* vehicle, CEntity* targetVehicle) : CTaskComplexCarDriveMission(vehicle, targetVehicle, MISSION_36, DRIVING_STYLE_PLOUGH_THROUGH, 40.0f) { } // 0x4B8A00
    ~CTaskComplexCarDriveMissionKillPed() override = default; // 0x4B8AD0

    static constexpr auto Type = TASK_COMPLEX_CAR_DRIVE_MISSION_KILL_PED;
    eTaskType GetTaskType() const override { return Type; } // 0x4B8AA0
    CTask* Clone() const override { return new CTaskComplexCarDriveMissionKillPed(m_Veh, m_pTargetVehicle); } // 0x4B8A30
};
