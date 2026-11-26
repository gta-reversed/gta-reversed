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
#include <memory>

class NOTSA_EXPORT_VTABLE CTaskComplexLeaveCar : public CTaskComplex {
public:
    static constexpr auto Type = TASK_COMPLEX_LEAVE_CAR;

    [[deprecated]] explicit CTaskComplexLeaveCar(CVehicle* targetVehicle, int32 targetDoor, int32 delayTime); // TODO: Add `bool die` to the ctor below and remove this one
    explicit CTaskComplexLeaveCar(CVehicle* targetVehicle, int32 targetDoor /*= 0*/, int32 delayTime /*= 0*/, bool sensibleLeaveCar /*= true */, bool forceGetOut /*= false */);
    CTaskComplexLeaveCar(const CTaskComplexLeaveCar& o); // NOTSA
    ~CTaskComplexLeaveCar() override;

    eTaskType GetTaskType() const override { return Type; }
    CTask*    Clone() const override { return new CTaskComplexLeaveCar{ *this }; } // 0x63D9E0
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override;

    CTask* CreateSubTask(eTaskType taskType, CPed* ped);
    void   ComputeTargetDoor(CPed* ped);
    void   SetVehicleFlags(CPed* ped);
    void   CreateTaskUtilityLineUpPedWithCar(CPed* ped);
    void   ClearVehicleFlags(); // NOTSA

    static void GetDoorUsage(const CVehicle* veh, int32 door, bool& isGettingIn, bool& isGettingOut);

    static void InjectHooks();

protected:
    CVehicle::Ref                                 m_TargetVehicle{};
    int32                                         m_TargetDoor{};
    int32                                         m_DelayTime{};
    bool                                          m_SensibleLeaveCar{};
    bool                                          m_ForceGetOut{};
    bool                                          m_Die{};
    std::unique_ptr<CTaskUtilityLineUpPedWithCar> m_LineUpUtility{};
    uint8                                         m_DoorFlagsSet{};    //!< Getting-out flags this task has set on the vehicle
    uint8                                         m_NumGettingInSet{}; //!< How much this task has added to the vehicle's `m_nNumGettingIn`
    AnimationId                                   m_DieAnimID{ ANIM_ID_KO_SHOT_FRONT_0 };
    float                                         m_DieAnimBlendDelta{ 4.f };
    float                                         m_DieAnimSpeed{ 1.f };
    bool                                          m_IsInAir{};
};
VALIDATE_SIZE(CTaskComplexLeaveCar, 0x34);
