/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "TaskComplex.h"
#include "Vehicle.h"
#include "TaskUtilityLineUpPedWithCar.h"

class NOTSA_EXPORT_VTABLE CTaskComplexLeaveCar : public CTaskComplex {
public:
    CVehicle*                     m_pTargetVehicle;
    int32                         m_nTargetDoor;
    int32                         m_nDelayTime;
    bool                          m_bSensibleLeaveCar;
    bool                          m_bForceGetOut;
    bool                          m_bDie;
    CTaskUtilityLineUpPedWithCar* m_pTaskUtilityLineUpPedWithCar;
    uint8                         m_nDoorFlagsSet;
    uint8                         m_nNumGettingInSet;
    int32                         m_nDieAnimID;
    float                         m_fDieAnimBlendDelta;
    float                         m_fDieAnimSpeed;
    bool                          m_bIsInAir;

public:
    static constexpr auto Type = TASK_COMPLEX_LEAVE_CAR;

    explicit CTaskComplexLeaveCar(CVehicle* targetVehicle, int32 nTargetDoor, int32 nDelayTime);
    explicit CTaskComplexLeaveCar(CVehicle* targetVehicle, int32 nTargetDoor, int32 nDelayTime, bool bSensibleLeaveCar, bool bForceGetOut);
    CTaskComplexLeaveCar(const CTaskComplexLeaveCar& o); // NOTSA
    ~CTaskComplexLeaveCar() override;

    eTaskType GetTaskType() const override { return Type; }
    CTask* Clone() const override { return new CTaskComplexLeaveCar{ *this }; } // 0x63D9E0
    bool   MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    CTask* CreateNextSubTask(CPed* ped) override;
    CTask* CreateFirstSubTask(CPed* ped) override;
    CTask* ControlSubTask(CPed* ped) override;
    CTask* CreateSubTask(eTaskType taskType, CPed* ped);

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CTaskComplexLeaveCar* Constructor(CVehicle* targetVehicle, int32 nTargetDoor, int32 nDelayTime, bool bSensibleLeaveCar, bool bForceGetOut);
};
VALIDATE_SIZE(CTaskComplexLeaveCar, 0x34);
