#include "StdInc.h"

#include "TaskComplexDriveToPoint.h"
#include "TaskComplexGoToPointAnyMeans.h"

// 0x63CE00
CTaskComplexDriveToPoint::CTaskComplexDriveToPoint(CVehicle* vehicle, const CVector& point, float speed, int32 arg4, eModelID carModelIndexToCreate, float radius, eCarDrivingStyle drivingStyle) :
      CTaskComplexCarDrive(vehicle, speed, carModelIndexToCreate, drivingStyle),
      m_Point{ point },
      field_30{ arg4 },
      m_Radius{ radius },
      field_38{ false }
{

}

// 0x63CE80
CTask* CTaskComplexDriveToPoint::CreateSubTaskCannotGetInCar(CPed* ped) {
    return new CTaskComplexGoToPointAnyMeans(PEDMOVE_RUN, m_Point, 0.5f, m_DesiredCarModel);
}

// 0x63CF00
void CTaskComplexDriveToPoint::SetUpCar() {
    m_OriginalDrivingStyle = m_Veh->m_autoPilot.m_nCarDrivingStyle;
    m_OriginalMission         = m_Veh->m_autoPilot.m_nCarMission;
    m_OriginalSpeed              = m_Veh->m_autoPilot.m_nCruiseSpeed;

    m_bIsCarSetUp = true;

    if (m_CruiseSpeed > 0.0f) {
        assert(m_CruiseSpeed < 255.0f);
        m_Veh->m_autoPilot.m_nCruiseSpeed = (uint8)m_CruiseSpeed;
    }
    m_Veh->m_autoPilot.m_nCarDrivingStyle    = static_cast<eCarDrivingStyle>(m_CarDrivingStyle);
    m_Veh->m_autoPilot.m_nTimeToStartMission = CTimer::GetTimeInMS();
}

// 0x645420
CTask* CTaskComplexDriveToPoint::Drive(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x645420, CTaskComplexDriveToPoint*, CPed*>(this, ped); // untested

    auto dist = DistanceBetweenPoints(m_Veh->GetPosition(), m_Point);
    if (dist < m_Radius) {
        m_Veh->m_autoPilot.m_nCarMission = MISSION_NONE;
        field_38 = true;
        return CTaskComplexCarDrive::CreateSubTask(TASK_FINISHED, ped);
    }

    if (dist >= 3.0f || m_Veh->m_autoPilot.m_nCarMission) {
        if (!m_Veh->m_autoPilot.m_nCruiseSpeed) {
            assert(m_CruiseSpeed < 255.0f);
            m_Veh->m_autoPilot.m_nCruiseSpeed = (uint8)m_CruiseSpeed;
        }

        if (IsTargetBlocked(ped)) {
            field_38 = true;
            return CTaskComplexCarDrive::CreateSubTask(TASK_FINISHED, ped);
        }

        switch (field_30) {
        case field_30_enum::DEFAULT:
            CCarAI::GetCarToGoToCoors(m_Veh, &m_Point, m_CarDrivingStyle, false);
            return m_pSubTask;
        case field_30_enum::ACCURATE:
            CCarAI::GetCarToGoToCoorsAccurate(m_Veh, &m_Point, m_CarDrivingStyle, false);
            return m_pSubTask;
        case field_30_enum::STRAIGHT_LINE:
            CCarAI::GetCarToGoToCoorsStraightLine(m_Veh, &m_Point, m_CarDrivingStyle, false);
            return m_pSubTask;
        case field_30_enum::RACING:
            CCarAI::GetCarToGoToCoorsRacing(m_Veh, &m_Point, m_CarDrivingStyle, false);
            return m_pSubTask;
        default:
            return m_pSubTask;
        }
    }

    field_38 = true;
    return CTaskComplexCarDrive::CreateSubTask(TASK_FINISHED, ped);
}

// 0x6452C0
bool CTaskComplexDriveToPoint::IsTargetBlocked(CPed* ped) const {
    return plugin::CallMethodAndReturn<bool, 0x6452C0, const CTaskComplexDriveToPoint*, CPed*>(this, ped); // untested

    if (DistanceBetweenPointsSquared(ped->GetPosition(), m_Point) > sq(6.0f)) {
        return false;
    }

    auto intel = ped->GetIntelligence();
    return IsTargetBlocked(ped, intel->GetPedEntities(), 16) || IsTargetBlocked(ped, intel->GetVehicleEntities(), 16);
}

// 0x6432A0
bool CTaskComplexDriveToPoint::IsTargetBlocked(CPed* ped, CEntity** entities, int32 numEntities) const {
    return plugin::CallMethodAndReturn<bool, 0x6432A0, const CTaskComplexDriveToPoint*, CPed*, CEntity**, int32>(this, ped, entities, numEntities); // untested

    if (!ped->m_pVehicle)
        return false;

    const auto& vehPos = ped->m_pVehicle->GetPosition();
    auto dist = vehPos - m_Point;

    for (auto i = 0; i < numEntities; ++i) {
        CEntity* entity = entities[i];
        if (!entity || entity == ped->m_pVehicle) {
            continue;
        }

        const auto& vehicleRadius = ped->m_pVehicle->GetModelInfo()->GetColModel()->GetBoundRadius();
        if (DistanceBetweenPointsSquared(vehPos, entity->GetPosition()) > sq(vehicleRadius)) {
            continue;
        }

        const auto& entityRadius = entity->GetModelInfo()->GetColModel()->GetBoundRadius();
        if ((entityRadius + vehicleRadius) * (entityRadius + vehicleRadius) * 1.5f > dist.SquaredMagnitude()) {
            return true;
        }
    }
    return false;
}

void CTaskComplexDriveToPoint::GoToPoint(const CVector& point) {
    m_Point = point;
}
