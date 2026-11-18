#pragma once

#include "TaskComplexEnterCar.h"
#include "Enums/eMoveState.h"

class NOTSA_EXPORT_VTABLE CTaskComplexEnterCarAsDriver : public CTaskComplexEnterCar {
public:
    static constexpr auto Type = TASK_COMPLEX_ENTER_CAR_AS_DRIVER;

    explicit CTaskComplexEnterCarAsDriver(CVehicle* targetVehicle);
    CTaskComplexEnterCarAsDriver(CVehicle* targetVehicle, eMoveState moveState);
    CTaskComplexEnterCarAsDriver(const CTaskComplexEnterCarAsDriver&);
    ~CTaskComplexEnterCarAsDriver() override = default;

    eTaskType GetTaskType() const override { return Type; }
    CTask*    Clone() const override { return new CTaskComplexEnterCarAsDriver{ *this }; }

public:
    static void InjectHooks();
};
VALIDATE_SIZE(CTaskComplexEnterCarAsDriver, 0x50);
