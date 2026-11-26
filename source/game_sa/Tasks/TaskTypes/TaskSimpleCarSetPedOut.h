/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "TaskSimple.h"
#include "Vehicle.h"
#include <extensions/EntityRef.hpp>

class NOTSA_EXPORT_VTABLE CTaskSimpleCarSetPedOut : public CTaskSimple {
public:
    static constexpr auto Type = TASK_SIMPLE_CAR_SET_PED_OUT;

    CTaskSimpleCarSetPedOut(CVehicle* targetVehicle, eTargetDoor nTargetDoor, bool bSwitchOffEngine, bool warpingOutOfCar /*notsa arg*/);
    CTaskSimpleCarSetPedOut(CVehicle* targetVehicle, eTargetDoor nTargetDoor, bool bSwitchOffEngine); /* Original constructor, kept for hooks */
    CTaskSimpleCarSetPedOut(const CTaskSimpleCarSetPedOut&);
    ~CTaskSimpleCarSetPedOut() override = default;

    static void PositionPedOutOfCollision(CPed* ped, CVehicle* veh, int32 door);

    eTaskType GetTaskType() const override { return Type; }
    CTask*    Clone() const override { return new CTaskSimpleCarSetPedOut{ *this }; }
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override { return false; }
    bool      ProcessPed(CPed* ped) override;

public:
    static void InjectHooks();

public:
    CVehicle::Ref m_pTargetVehicle{};
    eTargetDoor   m_nTargetDoor{};
    bool          m_bSwitchOffEngine{};
    bool          m_bWarpingOutOfCar{};
    bool          m_bFallingOutOfCar{}; /// Jumping or falling off car or bike
    bool          m_bKnockedOffBike{};
    uint8         m_nDoorFlagsToClear{};
    uint8         m_nNumGettingInToClear{};
};
VALIDATE_SIZE(CTaskSimpleCarSetPedOut, 0x18);
