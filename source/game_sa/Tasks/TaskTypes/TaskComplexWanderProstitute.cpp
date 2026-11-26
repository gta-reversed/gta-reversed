#include "StdInc.h"

#include "TaskComplexWanderProstitute.h"
#include "TaskComplexProstituteSolicit.h"
#include "Events/EventAcquaintancePedRespect.h"

// 0x672690
CTaskComplexWanderProstitute::CTaskComplexWanderProstitute(eMoveState MoveState, uint8 Dir, bool bWanderSensibly) :
    CTaskComplexWanderStandard(MoveState, Dir, bWanderSensibly) {
}

constexpr uint32 SCAN_INTERVAL_MS   = 2'000;
constexpr uint32 MAX_MONETARY_VALUE = 50'000;
constexpr float  MAX_CAR_SPEED      = 2.f;

// 0x672700
void CTaskComplexWanderProstitute::ScanForStuff(CPed* ped) {
    CTaskComplexWanderStandard::ScanForStuff(ped);

    if (CTimer::GetTimeInMS() <= m_NextScanTimeMs) {
        return;
    }
    m_NextScanTimeMs    = CTimer::GetTimeInMS() + SCAN_INTERVAL_MS;

    const auto& scanner = ped->GetIntelligence()->GetPedScanner();
    if (rng::any_of(scanner.GetEntities<CPed>(), &CPed::IsCop)) {
        return;
    }

    for (auto& client : scanner.GetEntities<CPed>()) {
        // NOTE: Checks `m_pVehicle` only, not `IsInVehicle()`
        if (!client.GetPlayerData() || !client.m_pVehicle || client.GetPlayerData()->m_pLastProstituteShagged == ped) {
            continue;
        }
        if (client.m_pVehicle->m_pHandlingData->m_nMonetaryValue <= CGeneral::GetRandomNumberInRange(0u, MAX_MONETARY_VALUE)) {
            continue;
        }
        if ((client.m_pVehicle->GetMoveSpeed() * 50.f).SquaredMagnitude() >= sq(MAX_CAR_SPEED)) {
            continue;
        }
        if (!CTaskComplexProstituteSolicit::IsTaskValid(ped, &client)) {
            continue;
        }
        ped->GetEventGroup().Add(CEventAcquaintancePedRespect{ &client, TASK_COMPLEX_PROSTITUTE_SOLICIT });
    }
}

void CTaskComplexWanderProstitute::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexWanderProstitute, 0x870148, 15);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedConstructorInstall(0x672690, "1", {}, eMoveState, uint8, bool);

    RH_ScopedVMTInstall(Clone, 0x673C80);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x674920);
    RH_ScopedVMTInstall(GetWanderType, 0x6726C0);
    RH_ScopedVMTInstall(ScanForStuff, 0x672700);
}
