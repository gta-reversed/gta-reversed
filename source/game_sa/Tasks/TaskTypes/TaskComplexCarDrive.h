#pragma once

#include "TaskComplex.h"
#include "eCarMission.h"
#include "eCarDrivingStyle.h"

class CVehicle;

class NOTSA_EXPORT_VTABLE CTaskComplexCarDrive : public CTaskComplex {
public:
    static constexpr auto Type = TASK_COMPLEX_CAR_DRIVE;

    CTaskComplexCarDrive(CVehicle* vehicle, bool asDriver = true);
    CTaskComplexCarDrive(CVehicle* vehicle, float speed, eModelID carModelIndexToCreate = MODEL_INVALID, eCarDrivingStyle carDrivingStyle = DRIVING_STYLE_STOP_FOR_CARS);
    CTaskComplexCarDrive(const CTaskComplexCarDrive&);
    ~CTaskComplexCarDrive() override;

    CTask*    Clone() const override { return new CTaskComplexCarDrive{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override;

    virtual void   SetUpCar();
    virtual CTask* CreateSubTaskCannotGetInCar(CPed* ped) { return CreateSubTask(TASK_FINISHED, ped); }
    virtual CTask* Drive(CPed* ped) { return m_pSubTask; }

    CTask* CreateSubTask(eTaskType taskType, CPed* ped);

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CTaskComplexCarDrive* Constructor_0(CVehicle* vehicle) { this->CTaskComplexCarDrive::CTaskComplexCarDrive(vehicle); return this; }
    CTaskComplexCarDrive* Constructor_1(CVehicle* vehicle, float speed, eModelID carModelIndexToCreate, eCarDrivingStyle carDrivingStyle) { this->CTaskComplexCarDrive::CTaskComplexCarDrive(vehicle, speed, carModelIndexToCreate, carDrivingStyle); return this; }
    CTaskComplexCarDrive* Destructor() { this->CTaskComplexCarDrive::~CTaskComplexCarDrive(); return this; }

public:
    CVehicle* m_Veh{};
    float     m_CruiseSpeed{};
    eModelID  m_DesiredCarModel{MODEL_INVALID};
    uint32    m_CarDrivingStyle{DRIVING_STYLE_STOP_FOR_CARS};
    bool      m_bAsDriver{};  
    int8      m_OriginalDrivingStyle{};
    int8      m_OriginalMission{};
    uint8     m_OriginalSpeed{};
    bool      m_bIsCarSetUp{};
};
VALIDATE_SIZE(CTaskComplexCarDrive, 0x24);
