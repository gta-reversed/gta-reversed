#pragma once

#include "TaskSimpleCarDrive.h"

class CVehicle;

class NOTSA_EXPORT_VTABLE CTaskSimpleCarDriveTimed : public CTaskSimpleCarDrive {
public:
    static constexpr auto Type = TASK_SIMPLE_CAR_DRIVE_TIMED;

    CTaskSimpleCarDriveTimed() = delete;
    CTaskSimpleCarDriveTimed(CVehicle* vehicle, int32 time = -1);
    ~CTaskSimpleCarDriveTimed() override = default;

    eTaskType GetTaskType() const override { return Type; }
    CTask*    Clone() const override { return new CTaskSimpleCarDriveTimed(m_Vehicle, m_Time); }
    bool      ProcessPed(class CPed* ped) override;

    static void InjectHooks();

private:
    int32      m_Time{};
    CTaskTimer m_Timer{};
};
VALIDATE_SIZE(CTaskSimpleCarDriveTimed, 0x70);
