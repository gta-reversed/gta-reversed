#pragma once

#include "TaskSimpleCarDrive.h"

class CVehicle;

class NOTSA_EXPORT_VTABLE CTaskSimpleCarDriveTimed : public CTaskSimpleCarDrive {
public:
    int32      m_nTime;
    CTaskTimer m_nTimer;

public:
    static constexpr auto Type = TASK_SIMPLE_CAR_DRIVE_TIMED;

    CTaskSimpleCarDriveTimed() = delete;
    CTaskSimpleCarDriveTimed(CVehicle* vehicle, int32 nTime = -1);
    ~CTaskSimpleCarDriveTimed() override = default;

    eTaskType GetTaskType() const override { return Type; }
    CTask* Clone() const override { return new CTaskSimpleCarDriveTimed(m_pVehicle, m_nTime); }
    bool ProcessPed(class CPed* ped) override;

private:
    friend void InjectHooksMain();
    static void InjectHooks() {};
    CTaskSimpleCarDriveTimed* Constructor(CVehicle* vehicle, int32 nTime);
};
VALIDATE_SIZE(CTaskSimpleCarDriveTimed, 0x70);
