#pragma once

#include "TaskComplexEnterCar.h"

class NOTSA_EXPORT_VTABLE CTaskComplexOpenPassengerDoor : public CTaskComplexEnterCar {
public:
    static constexpr auto Type = TASK_COMPLEX_CAR_OPEN_PASSENGER_DOOR;

    CTaskComplexOpenPassengerDoor(CVehicle* vehicle, int32 targetSeat) :
        CTaskComplexEnterCar(vehicle, false, true, false, false) {
        m_TargetSeat = targetSeat;
    }
    ~CTaskComplexOpenPassengerDoor() override = default;

    eTaskType GetTaskType() const override { return Type; }                                             // 0x640410
    CTask*    Clone() const override { return new CTaskComplexOpenPassengerDoor(m_Car, m_TargetSeat); } // 0x6438E0

public:
    static void InjectHooks();
};
