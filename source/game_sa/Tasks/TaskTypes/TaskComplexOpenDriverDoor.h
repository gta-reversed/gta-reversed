#pragma once

#include "TaskComplexEnterCar.h"
class CVehicle;

class NOTSA_EXPORT_VTABLE CTaskComplexOpenDriverDoor : public CTaskComplexEnterCar {
public:
    static constexpr auto Type = TASK_COMPLEX_CAR_OPEN_DRIVER_DOOR;

    CTaskComplexOpenDriverDoor(CVehicle* vehicle);
    CTaskComplexOpenDriverDoor(const CTaskComplexOpenDriverDoor&);
    ~CTaskComplexOpenDriverDoor() override = default;

    eTaskType GetTaskType() const override { return Type; }                              // 0x6403C0
    CTask*    Clone() const override { return new CTaskComplexOpenDriverDoor{ *this }; } // 0x643870

public:
    static void InjectHooks();
};
