#pragma once

#include "TaskComplex.h"
#include "TaskTimer.h"

class CTaskComplexStealCar;
class CPed;
class CVehicle;
class CTaskComplexCarDriveMissionFleeScene;

class NOTSA_EXPORT_VTABLE CTaskComplexStealCar : public CTaskComplex {

public:
    CVehicle*  m_veh{};             /// The vehicle to steal
    CTaskTimer m_enterTimer{};      /// Started when `TASK_COMPLEX_ENTER_CAR_AS_DRIVER` is created, interval is 10'000
    CPed*      m_originalriver{};   /// The original driver

public:
    static void InjectHooks();

    static constexpr auto Type = eTaskType::TASK_COMPLEX_STEAL_CAR;

    CTaskComplexStealCar(CVehicle* toSteal);
    CTaskComplexStealCar(const CTaskComplexStealCar&);
    ~CTaskComplexStealCar();

    CTask* CreateSubTask(eTaskType taskType, CPed* ped);

    CTask*    Clone() const override { return new CTaskComplexStealCar{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override;

private: // Wrappers for hooks
    // 0x63B7D0
    CTaskComplexStealCar* Constructor(CVehicle* toSteal) {
        this->CTaskComplexStealCar::CTaskComplexStealCar(toSteal);
        return this;
    }
    // 0x63B850
    CTaskComplexStealCar* Destructor() {
        this->CTaskComplexStealCar::~CTaskComplexStealCar();
        return this;
    }
};
