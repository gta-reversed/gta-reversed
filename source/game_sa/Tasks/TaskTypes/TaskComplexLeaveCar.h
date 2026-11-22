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

    [[deprecated]] explicit CTaskComplexLeaveCar(CVehicle* targetVehicle, int32 nTargetDoor, int32 nDelayTime); // TODO: Add `bool die` to the ctor below and remove this one
    explicit CTaskComplexLeaveCar(CVehicle* targetVehicle, int32 nTargetDoor /*= 0*/, int32 nDelayTime /*= 0*/, bool bSensibleLeaveCar /*= true */, bool bForceGetOut /*= false */);
    CTaskComplexLeaveCar(const CTaskComplexLeaveCar& o);
    ~CTaskComplexLeaveCar() override;

    void ComputeTargetDoor(CPed* ped);

    eTaskType GetTaskType() const override { return Type; }
    CTask*    Clone() const override { return new CTaskComplexLeaveCar{ *this }; } // 0x63D9E0
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override;
    CTask*    CreateSubTask(eTaskType taskType, CPed* ped);

public:
    static void InjectHooks();

public:
    CVehicle::Ref                                 m_pTargetVehicle{};
    int32                                         m_nTargetDoor{};
    int32                                         m_nDelayTime{};
    bool                                          m_bSensibleLeaveCar{};
    bool                                          m_bForceGetOut{};
    bool                                          m_bDie{};
    std::unique_ptr<CTaskUtilityLineUpPedWithCar> m_pTaskUtilityLineUpPedWithCar{};
    uint8                                         m_nDoorFlagsSet{};
    uint8                                         m_nNumGettingInSet{};
    int32                                         m_nDieAnimID{ANIM_ID_KO_SHOT_FRONT_0};
    float                                         m_fDieAnimBlendDelta{4.f};
    float                                         m_fDieAnimSpeed{1.f};
    bool                                          m_bIsInAir{};
};
VALIDATE_SIZE(CTaskComplexLeaveCar, 0x34);
