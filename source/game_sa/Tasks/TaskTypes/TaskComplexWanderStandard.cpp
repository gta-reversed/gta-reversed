#include "StdInc.h"

#include "TaskComplexWanderStandard.h"
#include "TaskComplexPartnerChat.h"
#include "TaskComplexBeInCouple.h"
#include "EventSexyVehicle.h"
#include "EventChatPartner.h"
#include "EventAcquaintancePedDislike.h"

// 0x48E4F0
CTaskComplexWanderStandard::CTaskComplexWanderStandard(eMoveState MoveState, uint8 Dir, bool bWanderSensibly) :
    CTaskComplexWander(MoveState, Dir, bWanderSensibly),
    m_nMinNextScanTime{ 0 } {
}

// 0x672600
void CTaskComplexWanderStandard::ScanForStuff(CPed* ped) {
    if (!m_TaskTimer.m_bStarted) {
        m_TaskTimer.m_nStartTime = CTimer::GetTimeInMS();
        m_TaskTimer.m_nInterval  = 50;
        m_TaskTimer.m_bStarted   = true;
    }

    if (CTimer::GetTimeInMS() < m_nMinNextScanTime) {
        return;
    }

    if (m_TaskTimer.m_bStarted) { // V547 Expression 'm_TaskTimer.m_bStarted' is always true.
        if (m_TaskTimer.m_bStopped) {
            m_TaskTimer.m_nStartTime = CTimer::GetTimeInMS();
            m_TaskTimer.m_bStopped   = false;
        }

        if (CTimer::GetTimeInMS() >= m_TaskTimer.m_nStartTime + m_TaskTimer.m_nInterval) {
            m_TaskTimer.m_nInterval  = 50;
            m_TaskTimer.m_nStartTime = CTimer::GetTimeInMS();
            m_TaskTimer.m_bStarted   = true;
            if (!LookForGangMembers(ped) && !LookForChatPartners(ped)) {
                CTaskComplexWanderStandard::LookForSexyCars(ped);
            }
        }
    }
}

// 0x66AED0
bool CTaskComplexWanderStandard::WillChat(const CPed& first, const CPed& second) {
    const auto CanChat = [](const CPed& ped) {
        return ped.m_nPedType != PED_TYPE_CRIMINAL
            && ped.m_nPedType != PED_TYPE_COP
            && !ped.IsPlayer()
            && !ped.IsGangster();
    };
    return CanChat(first) && CanChat(second) && CPedIntelligence::AreFriends(first, second);
}

// 0x66AF60
void CTaskComplexWanderStandard::SetNextMinScanTime(CPed* ped) {
    constexpr auto SCAN_DELAY_MS = 100'000u;

    auto* const tWander          = notsa::dyn_cast_if_present<CTaskComplexWander>(ped->GetTaskManager().GetActiveTask());
    if (!tWander || tWander->GetWanderType() != GetWanderType()) {
        return;
    }
    static_cast<CTaskComplexWanderStandard*>(tWander)->SetMinNextScanTime(CTimer::GetTimeInMS() + SCAN_DELAY_MS);
}

// 0x66AFD0
bool CTaskComplexWanderStandard::LookForSexyCars(CPed* ped) {
    constexpr auto MIN_MONETARY_VALUE = 40'000u;
    constexpr auto MIN_HEALTH         = 500.f;
    constexpr auto MAX_DIST           = 5.f;

    for (auto& veh : ped->GetIntelligence()->GetVehicleScanner().GetEntities<CVehicle>()) {
        if (&veh == ped->m_pVehicle || veh.m_pHandlingData->m_nMonetaryValue <= MIN_MONETARY_VALUE || veh.GetHealth() <= MIN_HEALTH) {
            continue;
        }
        const auto dir = veh.GetPosition() - ped->GetPosition();
        if (dir.SquaredMagnitude() >= sq(MAX_DIST) || dir.Dot(ped->GetForward()) <= 0.f) { // Close and in front of us
            continue;
        }
        if (!CWorld::GetIsLineOfSightClear(ped->GetPosition(), veh.GetPosition(), true, false, false, true, false, false, false)) {
            continue;
        }
        ped->GetEventGroup().Add(CEventSexyVehicle{ &veh });
        SetNextMinScanTime(ped);
        return true;
    }
    return false;
}

// 0x66FDA0
bool CTaskComplexWanderStandard::LookForChatPartners(CPed* ped) {
    constexpr auto MAX_PLAYER_VEH_SPEED_SQ = 0.04f;
    constexpr auto MAX_DIST                = 10.f;

    if (!g_surfaceInfos.IsPavement(ped->m_nContactSurface) || m_nMoveState > PEDMOVE_WALK) {
        return false;
    }
    if (const auto* const player = FindPlayerPed(); player->IsInVehicle() && player->m_pVehicle->GetMoveSpeed().SquaredMagnitude() > MAX_PLAYER_VEH_SPEED_SQ) {
        return false;
    }
    if (CStreaming::IsVeryBusy() || CGameLogic::LaRiotsActiveHere()) {
        return false;
    }

    const auto IsBusy = [](CPed& p) {
        return p.GetTaskManager().Find<CTaskComplexPartnerChat>(false)
            || p.GetEventGroup().GetEventOfType(EVENT_CHAT_PARTNER)
            || p.GetTaskManager().Find<CTaskComplexBeInCouple>(false);
    };

    for (auto& partner : ped->GetIntelligence()->GetPedScanner().GetEntities<CPed>()) {
        if (!g_surfaceInfos.IsPavement(partner.m_nContactSurface)) {
            continue;
        }
        const auto* const tPartnerActive = partner.GetTaskManager().GetActiveTask();
        if (!tPartnerActive || tPartnerActive->GetTaskType() != GetTaskType()) {
            continue;
        }
        if (IsBusy(*ped) || IsBusy(partner) || !WillChat(*ped, partner)) {
            continue;
        }
        const auto dir = partner.GetPosition() - ped->GetPosition();
        if (dir.SquaredMagnitude() >= sq(MAX_DIST)) {
            continue;
        }
        if (dir.Dot(ped->GetForward()) <= 0.f || dir.Dot(partner.GetForward()) >= 0.f) { // Facing each other
            continue;
        }
        if (!CWorld::GetIsLineOfSightClear(ped->GetPosition(), partner.GetPosition(), true, false, false, true, false, false, false)) {
            continue;
        }
        ped->GetEventGroup().Add(CEventChatPartner{ true, &partner });
        partner.GetEventGroup().Add(CEventChatPartner{ false, ped });
        SetNextMinScanTime(ped);
        SetNextMinScanTime(&partner);
        return true;
    }
    return false;
}

// 0x670100
bool CTaskComplexWanderStandard::LookForGangMembers(CPed* ped) {
    constexpr auto MAX_DIST = 5.f;

    if (CPedGroups::GetPedsGroup(ped)) {
        return false;
    }

    CVector targetPos;
    ComputeTargetPos(ped, targetPos, m_NextNode);
    const auto usToTarget = ped->GetPosition() - targetPos;
    for (auto& other : ped->GetIntelligence()->GetPedScanner().GetEntities<CPed>()) {
        const auto otherToTarget = other.GetPosition() - targetPos;
        if (otherToTarget.Dot(usToTarget) <= 0.f || otherToTarget.SquaredMagnitude() >= sq(MAX_DIST)) { // Close to our target, on our side of it
            continue;
        }
        auto* const grp = CPedGroups::GetPedsGroup(&other);
        if (!grp || grp->GetMembership().CountMembersExcludingLeader() <= 0) {
            continue;
        }
        ped->GetEventGroup().Add(CEventAcquaintancePedDislike{ &other, TASK_SMART_FLEE_ENTITY_WALKING });
        SetNextMinScanTime(ped);
        return true;
    }
    return false;
}

void CTaskComplexWanderStandard::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexWanderStandard, 0x85a200, 15);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedConstructorInstall(0x48E4F0, "1", {}, eMoveState, uint8, bool);
    RH_ScopedDestructorInstall(0x48E600);

    RH_ScopedInstall(WillChat, 0x66AED0);
    RH_ScopedInstall(SetNextMinScanTime, 0x66AF60);
    RH_ScopedInstall(LookForSexyCars, 0x66AFD0);
    RH_ScopedInstall(LookForChatPartners, 0x66FDA0);
    RH_ScopedInstall(LookForGangMembers, 0x670100);
    RH_ScopedVMTInstall(Clone, 0x48E530);
    RH_ScopedVMTInstall(GetWanderType, 0x48E5D0);
    RH_ScopedVMTInstall(ScanForStuff, 0x672600);
}
