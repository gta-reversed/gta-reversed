#include "StdInc.h"

#include "TaskComplexDriveToPoint.h"
#include "TaskComplexGoToPointAnyMeans.h"
#include <reversiblebugfixes/Bugs.hpp>

// 0x63CE00
CTaskComplexDriveToPoint::CTaskComplexDriveToPoint(CVehicle* vehicle, const CVector& point, float speed, int32 arg4, eModelID carModelIndexToCreate, float radius, eCarDrivingStyle drivingStyle) :
    CTaskComplexCarDrive(vehicle, speed, carModelIndexToCreate, drivingStyle),
    m_Point{ point },
    field_30{ arg4 },
    m_Radius{ radius },
    field_38{ false } {
}

// 0x63CE80
CTask* CTaskComplexDriveToPoint::CreateSubTaskCannotGetInCar(CPed* ped) {
    return new CTaskComplexGoToPointAnyMeans(PEDMOVE_RUN, m_Point, 0.5f, m_DesiredCarModel);
}

// 0x63CF00
void CTaskComplexDriveToPoint::SetUpCar() {
    m_OriginalDrivingStyle = m_Veh->m_autoPilot.m_nCarDrivingStyle;
    m_OriginalMission      = m_Veh->m_autoPilot.m_nCarMission;
    m_OriginalSpeed        = m_Veh->m_autoPilot.m_nCruiseSpeed;

    m_bIsCarSetUp          = true;

    if (m_CruiseSpeed > 0.0f) {
        assert(m_CruiseSpeed < 255.0f);
        m_Veh->m_autoPilot.SetCruiseSpeed((uint8)m_CruiseSpeed);
    }
    m_Veh->m_autoPilot.m_nCarDrivingStyle    = static_cast<eCarDrivingStyle>(m_CarDrivingStyle);
    m_Veh->m_autoPilot.m_nTimeToStartMission = CTimer::GetTimeInMS();
}

// 0x645420
CTask* CTaskComplexDriveToPoint::Drive(CPed* ped) {
    const auto dist = DistanceBetweenPoints(m_Point, m_Veh->GetPosition());
    if (dist < m_Radius) {
        m_Veh->m_autoPilot.SetCarMission(MISSION_NONE);
        field_38 = true;
        return CreateSubTask(TASK_FINISHED, ped);
    }

    if (dist < 3.0f && m_Veh->m_autoPilot.m_nCarMission == MISSION_NONE) {
        field_38 = true;
        return CreateSubTask(TASK_FINISHED, ped);
    }

    if (!m_Veh->m_autoPilot.m_nCruiseSpeed) {
        m_Veh->m_autoPilot.m_nCruiseSpeed = (uint8)((int32)(m_CruiseSpeed)); // NOTE: Truncated, not clamped (`-1.0f` gives `255`)
    }

    if (IsTargetBlocked(ped)) {
        field_38 = true;
        return CreateSubTask(TASK_FINISHED, ped);
    }

    switch (field_30) {
    case field_30_enum::DEFAULT:       CCarAI::GetCarToGoToCoors(m_Veh, m_Point, m_CarDrivingStyle, false); break;
    case field_30_enum::ACCURATE:      CCarAI::GetCarToGoToCoorsAccurate(m_Veh, m_Point, m_CarDrivingStyle, false); break;
    case field_30_enum::STRAIGHT_LINE: CCarAI::GetCarToGoToCoorsStraightLine(m_Veh, m_Point, m_CarDrivingStyle, false); break;
    case field_30_enum::RACING:        CCarAI::GetCarToGoToCoorsRacing(m_Veh, m_Point, m_CarDrivingStyle, false); break;
    }
    return m_pSubTask;
}

// 0x6452C0
bool CTaskComplexDriveToPoint::IsTargetBlocked(CPed* ped) const {
    if (CVector::DistSqr(ped->GetPosition(), m_Point) > sq(6.0f)) {
        return false;
    }
    auto* const intel = ped->GetIntelligence();
    return IsTargetBlocked(ped, intel->GetPedEntities(), 16)
        || IsTargetBlocked(ped, intel->GetVehicleEntities(), 16);
}

// 0x6432A0
bool CTaskComplexDriveToPoint::IsTargetBlocked(CPed* ped, CEntity** entities, int32 numEntities) const {
    auto* const veh = ped->m_pVehicle;
    if (!veh) {
        return false;
    }

    const auto vehRadius = veh->GetModelInfo()->GetColModel()->GetBoundRadius();
    const auto vehDistSq = CVector::DistSqr(veh->GetPosition(), m_Point);
    for (auto* const entity : std::span{ entities, (size_t)(numEntities) }) {
        if (!entity || entity == veh) {
            continue;
        }

        // Entity must be (roughly) on the target point
        const auto entityRadius = entity->GetModelInfo()->GetColModel()->GetBoundRadius();
        if (CVector::DistSqr(entity->GetPosition(), m_Point) >= sq(entityRadius)) {
            continue;
        }

        // And we must be close enough to it
        const auto maxDistSq = notsa::bugfixes::CTaskComplexDriveToPoint_IsTargetBlocked_DistScale
            ? sq((entityRadius + vehRadius) * 1.5f)
            : sq(entityRadius + vehRadius) * 1.5f; // The original scales the squared distance (= dist < (entityRadius + vehRadius) * sqrt(1.5))
        if (vehDistSq < maxDistSq) {
            return true;
        }
    }
    return false;
}

void CTaskComplexDriveToPoint::GoToPoint(const CVector& point) {
    m_Point = point;
}

void CTaskComplexDriveToPoint::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexDriveToPoint, 0x86e9dc, 14);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedConstructorInstall(0x63CE00, "1", {}, CVehicle *, CVector const&, float, int32, eModelID, float, eCarDrivingStyle);
    RH_ScopedDestructorInstall(0x63CE70);

    RH_ScopedOverloadedInstall(IsTargetBlocked, "Any", 0x6452C0, bool(CTaskComplexDriveToPoint::*)(CPed*) const);
    RH_ScopedOverloadedInstall(IsTargetBlocked, "Array", 0x6432A0, bool(CTaskComplexDriveToPoint::*)(CPed*, CEntity**, int32) const);
    RH_ScopedVMTInstall(Clone, 0x63DDE0);
    RH_ScopedVMTInstall(GetTaskType, 0x63CE60);
    RH_ScopedVMTInstall(SetUpCar, 0x63CF00);
    RH_ScopedVMTInstall(CreateSubTaskCannotGetInCar, 0x63CE80);
    RH_ScopedVMTInstall(Drive, 0x645420);
}
