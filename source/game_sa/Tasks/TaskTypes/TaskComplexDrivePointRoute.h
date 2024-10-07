#pragma once

#include "eCarDrivingStyle.h"
#include "TaskComplex.h"
#include "PedGeometryAnalyser.h"

class CVehicle;
class CPed;
class CPointRoute;

class NOTSA_EXPORT_VTABLE CTaskComplexDrivePointRoute : public CTaskComplex {
public:
    CVehicle*        m_veh{};
    CPointRoute*     m_route{};
    float            m_cruiseSpeed{};
    uint32           m_mode{};
    eModelID         m_desiredCarModel{};
    float            m_targetRadius{};
    eCarDrivingStyle m_drivingStyle{};
    uint8            m_zeropad[3]{}; /// Important padding, as originally `m_drivingStyle` was 4 bytes wide (now it's 1)
    uint32           m_currPtIdx{};

public:
    static void InjectHooks();

    static constexpr auto Type = eTaskType::TASK_COMPLEX_CAR_DRIVE_POINT_ROUTE;

    CTaskComplexDrivePointRoute(CVehicle* vehicle, CPointRoute const& route, float cruiseSpeed, uint32 mode, eModelID carModelIdx, float targetRadius, eCarDrivingStyle drivingStyle);
    CTaskComplexDrivePointRoute(const CTaskComplexDrivePointRoute&);
    ~CTaskComplexDrivePointRoute();

    /// NOTSA: Create `CTaskComplexDriveToPoint` for the current point.
    CTask* CreateTaskForCurrentPoint() const;

    CTask*    Clone() const override { return new CTaskComplexDrivePointRoute{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override { return CreateTaskForCurrentPoint(); }
    CTask*    ControlSubTask(CPed* ped) override { return m_pSubTask; }


private: // Wrappers for hooks
    // 0x6433E0
    auto Constructor(CVehicle* vehicle, CPointRoute const& route, float cruiseSpeed, uint32 mode, eModelID carModelIdx, float targetRadius, eCarDrivingStyle drivingStyle) {
        this->CTaskComplexDrivePointRoute::CTaskComplexDrivePointRoute(vehicle, route, cruiseSpeed, mode, carModelIdx, targetRadius, drivingStyle);
        return this;
    }

    // 0x643490
    auto Destructor() {
        this->CTaskComplexDrivePointRoute::~CTaskComplexDrivePointRoute();
        return this;
    }
};
