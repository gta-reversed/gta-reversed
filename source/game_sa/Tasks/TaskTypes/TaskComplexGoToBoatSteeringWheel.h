#pragma once

#include "Base.h"
#include "TaskComplex.h"

class NOTSA_EXPORT_VTABLE CTaskComplexGoToBoatSteeringWheel : public CTaskComplex {
public:
    CVector   m_TargetPos{};
    CVehicle* m_Vehicle{};
    bool      m_bAchievedPoint{};
    uint32    m_FirstSubTaskStartTime{};

public:
    static constexpr auto Type = TASK_COMPLEX_GO_TO_BOAT_STEERING_WHEEL;

    explicit CTaskComplexGoToBoatSteeringWheel(CVehicle* vehicle);
    ~CTaskComplexGoToBoatSteeringWheel() override;

    CTask* CreateSubTask(eTaskType taskType);
    void ComputeTargetPos();
    bool HasAchievedPoint() const { return m_bAchievedPoint; }

    eTaskType GetTaskType() const override { return Type; }
    CTask* Clone() const override { return new CTaskComplexGoToBoatSteeringWheel(m_Vehicle); } // 0x64A350
    CTask* CreateNextSubTask(CPed* ped) override;
    CTask* CreateFirstSubTask(CPed* ped) override;
    CTask* ControlSubTask(CPed* ped) override;
private:
    friend void InjectHooksMain();
    static void InjectHooks();
    CTaskComplexGoToBoatSteeringWheel* Constructor(CVehicle* veh) { this->CTaskComplexGoToBoatSteeringWheel::CTaskComplexGoToBoatSteeringWheel(veh); return this; }
    CTaskComplexGoToBoatSteeringWheel* Destructor() { this->CTaskComplexGoToBoatSteeringWheel::~CTaskComplexGoToBoatSteeringWheel(); return this; }
};
VALIDATE_SIZE(CTaskComplexGoToBoatSteeringWheel, 0x24);
