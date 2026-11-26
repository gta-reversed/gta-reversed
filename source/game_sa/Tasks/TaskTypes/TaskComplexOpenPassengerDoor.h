#pragma once

#include "TaskComplexEnterCar.h"

class NOTSA_EXPORT_VTABLE CTaskComplexOpenPassengerDoor : public CTaskComplexEnterCar {
public:
    static constexpr auto Type = TASK_COMPLEX_CAR_OPEN_PASSENGER_DOOR;

    CTaskComplexOpenPassengerDoor(CVehicle* vehicle, int32 targetSeat) :
        CTaskComplexEnterCar(vehicle, false, true, false, false) {
        m_TargetSeat = targetSeat;
    }
    // NOTSA
    CTaskComplexOpenPassengerDoor(const CTaskComplexOpenPassengerDoor& o) :
        CTaskComplexOpenPassengerDoor{ o.m_Car, o.m_TargetSeat } {
        m_MoveState = o.m_MoveState;
    }
    ~CTaskComplexOpenPassengerDoor() override = default;

    eTaskType GetTaskType() const override { return Type; }                         // 0x640410
    CTask*    Clone() const override { return new CTaskComplexOpenPassengerDoor{ *this }; } // 0x6438E0

public:
    static void InjectHooks();
};
