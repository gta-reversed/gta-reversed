#include "StdInc.h"

#include "TaskComplexWanderCriminal.h"
#include "Events/EventVehicleToSteal.h"
#include "CarEnterExit.h"

// 0x48E610
CTaskComplexWanderCriminal::CTaskComplexWanderCriminal(eMoveState MoveState, uint8 Dir, bool bWanderSensibly) :
    CTaskComplexWander(MoveState, Dir, bWanderSensibly) {
}

// 0x670350
void CTaskComplexWanderCriminal::ScanForStuff(CPed* ped) {
    constexpr int32 SCAN_INTERVAL_MS = 50;

    if (!m_TaskTimer.IsStarted()) {
        m_TaskTimer.Start(SCAN_INTERVAL_MS);
        m_MinNextScanTimeMs = CTimer::GetTimeInMS() + CGeneral::GetRandomNumberInRange(0, 30'000);
    }

    if (m_TaskTimer.IsOutOfTime()) { // 0x6703B9
        m_TaskTimer.Start(SCAN_INTERVAL_MS);
        if (CTimer::GetTimeInMS() >= m_MinNextScanTimeMs) {
            LookForCarsToSteal(ped);
        }
    }
}

// 0x66B4F0
void CTaskComplexWanderCriminal::LookForCarsToSteal(CPed* ped) {
    const auto [closestVeh, closestDistSq] = notsa::SpatialQuery(
        ped->GetIntelligence()->GetVehicleScanner().GetEntities<CVehicle>()
            | rngv::filter([ped](CVehicle& veh) {
                  return CCarEnterExit::IsVehicleStealable(&veh, ped);
              })
            | rngv::transform([](CVehicle& veh) {
                  return &veh;
              }),
        ped->GetPosition(),
        static_cast<CVehicle*>(nullptr)
    );
    if (!closestVeh) {
        return;
    }

    m_MinNextScanTimeMs = CTimer::GetTimeInMS() + 30'000;
    if (CGeneral::GetRandomNumberInRange(0.0f, 1.0f) < 0.2f) { // 0x66B5C2
        ped->GetEventGroup().Add(CEventVehicleToSteal{ closestVeh });
    }
}

void CTaskComplexWanderCriminal::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexWanderCriminal, 0x85a23c, 15);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedConstructorInstall(0x48E610, "1", {}, eMoveState, uint8, bool);
    RH_ScopedDestructorInstall(0x48E720);

    RH_ScopedInstall(LookForCarsToSteal, 0x66B4F0);
    RH_ScopedVMTInstall(Clone, 0x48E650);
    RH_ScopedVMTInstall(GetWanderType, 0x48E6F0);
    RH_ScopedVMTInstall(ScanForStuff, 0x670350);
}
