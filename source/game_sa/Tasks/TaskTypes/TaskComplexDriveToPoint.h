#pragma once

#include "TaskComplexCarDrive.h"
class CVehicle;

enum field_30_enum : int32 {
    DEFAULT,
    ACCURATE,
    STRAIGHT_LINE,
    RACING,
};

class NOTSA_EXPORT_VTABLE CTaskComplexDriveToPoint : public CTaskComplexCarDrive {
public:
    CVector m_Point;
    field_30_enum field_30;
    float m_Radius;
    bool field_38;

public:
    static constexpr auto Type = TASK_COMPLEX_CAR_DRIVE_TO_POINT;

    CTaskComplexDriveToPoint(CVehicle* vehicle, const CVector& point, float speed, int32 arg4, eModelID carModelIndexToCreate, float radius, eCarDrivingStyle drivingStyle);
    ~CTaskComplexDriveToPoint() override = default;

    eTaskType GetTaskType() const override { return Type;}
    CTask* Clone() const override { return new CTaskComplexDriveToPoint(m_Veh, m_Point, m_CruiseSpeed, field_30, m_DesiredCarModel, m_Radius, static_cast<eCarDrivingStyle>(m_CarDrivingStyle)); }

    void SetUpCar() override;
    CTask* CreateSubTaskCannotGetInCar(CPed* ped) override;
    CTask* Drive(CPed* ped) override;


    bool IsTargetBlocked(CPed* ped) const;
    bool IsTargetBlocked(CPed* ped, CEntity** entities, int32 numEntities) const;

    /// NOTSA
    void GoToPoint(const CVector& point);
};
VALIDATE_SIZE(CTaskComplexDriveToPoint, 0x3C);
