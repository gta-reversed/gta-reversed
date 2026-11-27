/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "TaskSimple.h"
#include "Vehicle.h"
#include "TaskUtilityLineUpPedWithCar.h"

class CAnimBlendAssociation;
class CTaskUtilityLineUpPedWithCar;

class NOTSA_EXPORT_VTABLE CTaskSimpleCarSetPedInAsPassenger : public CTaskSimple {
public:
    static constexpr auto Type = TASK_SIMPLE_CAR_SET_PED_IN_AS_PASSENGER;

    CTaskSimpleCarSetPedInAsPassenger(CVehicle* targetVehicle, eTargetDoor targetDoor, bool warpingInToCar /* notsa arg */, CTaskUtilityLineUpPedWithCar* utility = nullptr);
    CTaskSimpleCarSetPedInAsPassenger(CVehicle* targetVehicle, eTargetDoor targetDoor, CTaskUtilityLineUpPedWithCar* utility = nullptr); /* Original constructor, used for hooks */
    CTaskSimpleCarSetPedInAsPassenger(const CTaskSimpleCarSetPedInAsPassenger&);
    ~CTaskSimpleCarSetPedInAsPassenger() override = default;

    eTaskType GetTaskType() const override { return Type; }
    CTask*    Clone() const override { return new CTaskSimpleCarSetPedInAsPassenger{ *this }; } // 0x649D90
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override { return false; }
    bool      ProcessPed(CPed* ped) override;

    static void InjectHooks();

private:
    bool                          m_IsFinished{};
    CAnimBlendAssociation*        m_Anim{};
    CVehicle::Ref                 m_TargetVehicle{};
    eTargetDoor                   m_TargetDoor{};
    CTaskUtilityLineUpPedWithCar* m_Utility{};
    bool                          m_WarpingInToCar{};
    uint8                         m_DoorFlagsToClear{};
    uint8                         m_NumGettingInToClear{};
};
VALIDATE_SIZE(CTaskSimpleCarSetPedInAsPassenger, 0x20);
