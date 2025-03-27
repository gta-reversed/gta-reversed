/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "TaskComplexEnterCar.h"
#include "Ped.h" // eMoveState

class NOTSA_EXPORT_VTABLE CTaskComplexEnterCarAsDriver : public CTaskComplexEnterCar {
public:
    static constexpr auto Type = TASK_COMPLEX_ENTER_CAR_AS_DRIVER;

    explicit CTaskComplexEnterCarAsDriver(CVehicle* targetVehicle);
    CTaskComplexEnterCarAsDriver(CVehicle* targetVehicle, eMoveState moveState);
    CTaskComplexEnterCarAsDriver(const CTaskComplexEnterCarAsDriver&);
    ~CTaskComplexEnterCarAsDriver() override = default;

    eTaskType GetTaskType() const override { return Type; }
    CTask*    Clone()       const override { return new CTaskComplexEnterCarAsDriver{*this}; }
};
VALIDATE_SIZE(CTaskComplexEnterCarAsDriver, 0x50);
