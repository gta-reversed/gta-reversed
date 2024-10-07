#include "StdInc.h"

#include "GroupEventHandler.h"
#include "EventHandler.h"
#include "IKChainManager_c.h"

#include "Tasks/TaskTypes/TaskComplexStareAtPed.h"
#include "Tasks/TaskTypes/TaskSimpleNone.h"
#include "Tasks/TaskTypes/TaskComplexLeaveAnyCar.h"
#include "Tasks/TaskTypes/TaskComplexGangJoinRespond.h"
#include "Tasks/TaskTypes/TaskSimpleGoToPoint.h"
#include "Tasks/TaskTypes/TaskComplexEnterCarAsPassenger.h"
#include "Tasks/TaskTypes/TaskComplexEnterCarAsPassengerWait.h"
#include "Tasks/TaskTypes/TaskComplexLeaveCar.h"
#include "Tasks/TaskTypes/TaskComplexLeaveCarAsPassengerWait.h"
#include "Tasks/TaskTypes/TaskComplexSmartFleeEntity.h"
#include "Tasks/TaskTypes/TaskSimpleWaitUntilLeaderAreaCodesMatch.h"
#include "Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h"
#include "Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorStandard.h"
#include "Tasks/TaskTypes/TaskGoToVehicleAndLean.h"
#include "Tasks/TaskTypes/TaskComplexKillPedOnFoot.h"
#include "Tasks/TaskTypes/TaskGangHassleVehicle.h"
#include "Tasks/TaskTypes/TaskGangHasslePed.h"
#include "Tasks/TaskTypes/TaskComplexSignalAtPed.h"
#include "Tasks/TaskTypes/TaskComplexPartnerGreet.h"
#include "Tasks/TaskTypes/TaskComplexPartnerDeal.h"

#include "Tasks/Allocators/TaskAllocator.h"
#include "Tasks/Allocators/TaskAllocatorPlayerCommandAttack.h"
#include "Tasks/Allocators/TaskAllocatorKillThreatsBasic.h"
#include "Tasks/Allocators/TaskAllocatorKillThreatsBasicRandomGroup.h"
#include "Tasks/Allocators/TaskAllocatorKillThreatsDriveby.h"

#include "Events/EventVehicleDamage.h"
#include "Events/EventGunShot.h"
#include "Events/EventSexyPed.h"
#include "Events/EventAcquaintancePed.h"
#include "Events/EventPlayerCommandToGroup.h"
#include "Events/EventPlayerCommandToGroupAttack.h"
#include "Events/EventPlayerCommandToGroupGather.h"
#include "Events/EventGunAimedAt.h"
#include "Events/EventDraggedOutCar.h"
#include "Events/EventDanger.h"
#include "Events/EventDamage.h"
#include "Events/EventLeanOnVehicle.h"
#include "Events/EventSeenCop.h"
#include "Events/EventNewGangMember.h"
#include "Events/EventLeaderEntryExit.h"
#include "Events/EventLeaderExitedCarAsDriver.h"

void CGroupEventHandler::InjectHooks() {
    RH_ScopedClass(CGroupEventHandler);
    RH_ScopedCategory("Events");

    RH_ScopedInstall(IsKillTaskAppropriate, 0x5F7A60);
    RH_ScopedInstall(ComputeWalkAlongsideResponse, 0x5FA910);
    RH_ScopedInstall(ComputeStareResponse, 0x5F9BD0);
    RH_ScopedInstall(ComputeResponseVehicleDamage, 0x5FC070);
    RH_ScopedInstall(ComputeResponseShotFired, 0x5FBDF0);
    RH_ScopedInstall(ComputeResponseSexyPed, 0x5FB390);
    RH_ScopedInstall(ComputeResponseSeenCop, 0x5FBCB0);
    RH_ScopedInstall(ComputeResponsePlayerCommand, 0x5FB470);
    RH_ScopedInstall(ComputeResponsePedThreat, 0x5FBB90);
    RH_ScopedInstall(ComputeResponsePedFriend, 0x5FB2D0);
    RH_ScopedInstall(ComputeResponseNewGangMember, 0x5F9840);
    RH_ScopedInstall(ComputeResponseLeaderExitedCar, 0x5F90A0);
    RH_ScopedInstall(ComputeResponsLeaderQuitEnteringCar, 0x5F9530, { .reversed = false });
    RH_ScopedInstall(ComputeResponseLeaderEnteredCar, 0x5F8900, { .reversed = false });
    RH_ScopedInstall(ComputeResponseLeaderEnterExit, 0x5F9710);
    RH_ScopedInstall(ComputeResponseGunAimedAt, 0x5FBD10);
    RH_ScopedInstall(ComputeResponseGather, 0x5F99F0);
    RH_ScopedInstall(ComputeResponseDraggedOutCar, 0x5FBE70);
    RH_ScopedInstall(ComputeResponseDanger, 0x5FB540);
    RH_ScopedInstall(ComputeResponseDamage, 0x5FBF50);
    RH_ScopedInstall(ComputeMemberResponses, 0x5FAA50);
    RH_ScopedInstall(ComputeLeanOnVehicleResponse, 0x5F9B20);
    RH_ScopedInstall(ComputeKillThreatsBasicResponse, 0x5FB590);
    RH_ScopedInstall(ComputeKillPlayerBasicResponse, 0x5FB670);
    RH_ScopedInstall(ComputeHassleThreatResponse, 0x5F9D50);
    RH_ScopedInstall(ComputeHassleSexyPedResponse, 0x5FA020);
    RH_ScopedInstall(ComputeHandSignalResponse, 0x5FA820);
    RH_ScopedInstall(ComputeGreetResponse, 0x5FA550);
    RH_ScopedInstall(ComputeFleePedResponse, 0x5FA130);
    RH_ScopedInstall(ComputeEventResponseTasks, 0x5FC200);
    RH_ScopedInstall(ComputeDrivebyResponse, 0x5F7A00);
    RH_ScopedInstall(ComputeDoDealResponse, 0x5FA290);
}

bool IsPedInPlayersGroup(CPedGroup* pg, CPed* p) {
    const auto playerGrp = &FindPlayerPed(-1)->GetPlayerGroup();
    if (pg == playerGrp) {
        return true;
    }
    if (playerGrp->GetMembership().IsMember(p)) {
        return true;
    }
    return false;
}

void MaybeAdjustTaskOfGroupThreatEvent(const CEventEditableResponse& e, CPedGroup* pg, CPed* originator, CPed* srcPed) {
    if (pg->m_bIsMissionGroup && srcPed->IsPlayer()) {
        if (const auto l = pg->GetMembership().GetLeader()) {
            if (!l->GetActiveWeapon().IsTypeMelee() && !l->GetIntelligence()->IsFriendlyWith(*originator)) {
                const_cast<CEventEditableResponse*>(&e)->m_taskId = TASK_GROUP_KILL_THREATS_BASIC; // nice R*
            }
        }
    }
}

// 0x5F7A60
bool CGroupEventHandler::IsKillTaskAppropriate(CPedGroup* g, CPed* threat) {
    if (g->m_bIsMissionGroup || threat->GetActiveWeapon().IsTypeMelee()) {
        return true;
    }
    for (auto& m : g->GetMembership().GetMembers()) {
        if (!m.GetActiveWeapon().IsTypeMelee()) {
            return true;
        }
    }
    return false;
}

// 0x5FA910
CTaskAllocator* CGroupEventHandler::ComputeWalkAlongsideResponse(CPedGroup* pg, CPed* ped1, CPed* ped2) {
    NOTSA_UNREACHABLE(); // Unused
}

// 0x5F9BD0
CTaskAllocator* CGroupEventHandler::ComputeStareResponse(CPedGroup* pg, CPed* stareAt, CPed* originatorPed, int32 timeout, int32 timeoutBias) {
    if (!stareAt) {
        return nullptr;
    }
    if (!pg->GetMembership().GetLeader()) {
        return nullptr;
    }
    if ((stareAt->GetPosition() - pg->GetMembership().GetLeader()->GetPosition()).SquaredMagnitude2D() >= sq(8.f)) {
        return nullptr;
    }
    for (auto& m : pg->GetMembership().GetMembers()) {
        pg->GetIntelligence().SetEventResponseTask(
            &m,
            CTaskComplexStareAtPed{
                pg,
                stareAt,
                timeout + CGeneral::GetRandomNumberInRange(-timeoutBias, timeoutBias)
            }
        );
    }
    return nullptr;
}

// 0x5FC070
CTaskAllocator* CGroupEventHandler::ComputeResponseVehicleDamage(const CEventVehicleDamage& e, CPedGroup* pg, CPed* originator) {
    if (!e.m_attacker || !e.m_attacker->IsPed()) {
        return nullptr;
    }
    const auto threat = e.m_attacker->AsPed();
    switch (e.m_taskId) {
    case TASK_GROUP_KILL_THREATS_BASIC:  return ComputeKillThreatsBasicResponse(pg, threat, originator, true);
    case TASK_GROUP_KILL_PLAYER_BASIC:   return ComputeKillPlayerBasicResponse(pg, threat, originator, true); 
    case TASK_GROUP_FLEE_THREAT:         return ComputeFleePedResponse(pg, threat, originator, true);         
    case TASK_GROUP_USE_MEMBER_DECISION: return ComputeMemberResponses(e, pg, originator);                      
    case TASK_GROUP_DRIVEBY:             return ComputeDrivebyResponse(pg, threat, originator);
    }
    return nullptr;
}

// 0x5FBDF0
CTaskAllocator* CGroupEventHandler::ComputeResponseShotFired(const CEventGunShot& e, CPedGroup* pg, CPed* originator) {
    if (!e.m_firedBy || !e.m_firedBy->IsPed()) {
        return nullptr;
    }
    const auto threat = e.m_firedBy->AsPed();
    switch (e.m_taskId) {
    case TASK_GROUP_KILL_THREATS_BASIC:  return ComputeKillThreatsBasicResponse(pg, threat, originator, false);
    case TASK_GROUP_FLEE_THREAT:         return ComputeFleePedResponse(pg, threat, originator, false);
    case TASK_GROUP_USE_MEMBER_DECISION: return ComputeMemberResponses(e, pg, originator);
    }
    return nullptr;
}

// 0x5FB390
CTaskAllocator* CGroupEventHandler::ComputeResponseSexyPed(const CEventSexyPed& e, CPedGroup* pg, CPed* originator) {
    if (!e.m_SexyPed) {
        return nullptr;
    }
    if (pg->GetMembership().IsMember(e.m_SexyPed)) {
        return nullptr;
    }
    if (IsPedInPlayersGroup(pg, e.m_SexyPed)) {
        return nullptr;
    }
    switch (e.m_taskId) {
    case TASK_GROUP_STARE_AT_PED:    return ComputeStareResponse(pg, e.m_SexyPed, originator, CGeneral::GetRandomNumberInRange(3000, 5000), 1000);
    case TASK_GROUP_HASSLE_SEXY_PED: return ComputeHassleSexyPedResponse(pg, e.m_SexyPed, originator);
    }
    return nullptr;
}

// 0x5FBCB0
CTaskAllocator* CGroupEventHandler::ComputeResponseSeenCop(const CEventSeenCop& e, CPedGroup* pg, CPed* originator) {
    switch (e.m_taskId) {
    case TASK_GROUP_KILL_THREATS_BASIC: return ComputeKillThreatsBasicResponse(pg, e.m_AcquaintancePed, originator, false);
    case TASK_GROUP_FLEE_THREAT:        return ComputeFleePedResponse(pg, e.m_AcquaintancePed, originator, false);
    case TASK_GROUP_HAND_SIGNAL:        return ComputeHandSignalResponse(pg, e.m_AcquaintancePed, originator);
    }
    return nullptr;
}

// 0x5FB470
CTaskAllocator* CGroupEventHandler::ComputeResponsePlayerCommand(const CEventPlayerCommandToGroup& e, CPedGroup* pg, CPed* originator) {
    switch (e.m_command) {
    case ePlayerGroupCommand::PLAYER_GROUP_COMMAND_GATHER:
        return ComputeResponseGather(static_cast<const CEventPlayerCommandToGroupGather&>(e), pg, originator);
    case ePlayerGroupCommand::PLAYER_GROUP_COMMAND_ATTACK:
        return new CTaskAllocatorPlayerCommandAttack{
            e.m_target,
            e.m_target->GetGroupId(),
            e.m_target->m_nPedType
        };
    }
    return nullptr;
}

// 0x5FBB90
CTaskAllocator* CGroupEventHandler::ComputeResponsePedThreat(const CEventAcquaintancePed& e, CPedGroup* pg, CPed* originator) {
    if (!e.m_AcquaintancePed) {
        return nullptr;
    }
    if (pg->GetMembership().IsMember(e.m_AcquaintancePed)) {
        return nullptr;
    }
    switch (e.m_taskId) {
    case TASK_GROUP_KILL_THREATS_BASIC:    return ComputeKillThreatsBasicResponse(pg, e.m_AcquaintancePed, originator, false);
    case TASK_GROUP_STARE_AT_PED:          return ComputeStareResponse(pg, e.m_AcquaintancePed, originator, 99'999'999, false); // 1.15740739583 days
    case TASK_GROUP_FLEE_THREAT:           return ComputeFleePedResponse(pg, e.m_AcquaintancePed, originator, false);
    case TASK_GROUP_HASSLE_THREAT:         return ComputeHassleThreatResponse(pg, e.m_AcquaintancePed, originator, true);
    case TASK_GROUP_USE_MEMBER_DECISION:   return ComputeMemberResponses(e, pg, originator);
    case TASK_GROUP_DRIVEBY:               return ComputeDrivebyResponse(pg, e.m_AcquaintancePed, originator);
    case TASK_GROUP_HASSLE_THREAT_PASSIVE: return ComputeHassleThreatResponse(pg, e.m_AcquaintancePed, originator, false);
    }
    return nullptr;
}

// 0x5FB2D0
CTaskAllocator* CGroupEventHandler::ComputeResponsePedFriend(const CEventAcquaintancePed& e, CPedGroup* pg, CPed* originator) {
    if (!e.m_AcquaintancePed) {
        return nullptr;
    }
    if (IsPedInPlayersGroup(pg, e.m_AcquaintancePed)) {
        return nullptr;
    }
    switch (e.m_taskId) {
    case TASK_GROUP_PARTNER_DEAL:  return ComputeDoDealResponse(pg, e.m_AcquaintancePed, originator);
    case TASK_GROUP_PARTNER_GREET: return ComputeGreetResponse(pg, e.m_AcquaintancePed, originator);
    }
    return nullptr;
}

// 0x5F9840
CTaskAllocator* CGroupEventHandler::ComputeResponseNewGangMember(const CEventNewGangMember& e, CPedGroup* pg, CPed* originator) {
    if (!e.m_member || pg->GetMembership().IsMember(e.m_member)) {
        return nullptr;
    }
    CTaskComplexSequence tseq{
        new CTaskComplexLeaveAnyCar{0, false, true},
        new CTaskComplexGangJoinRespond{true}
    };
    if (pg->GetMembership().GetLeader() == FindPlayerPed()) {
        tseq.AddTask(new CTaskSimpleGoToPoint{
            eMoveState::PEDMOVE_RUN,
            FindPlayerPed()->GetPosition(),
            3.f,
            true
        });
    }
    pg->GetIntelligence().SetEventResponseTask(e.m_member, tseq);
    return nullptr;
}

// 0x5F90A0
CTaskAllocator* CGroupEventHandler::ComputeResponseLeaderExitedCar(const CEventEditableResponse&, CPedGroup* pg, CPed* originator) {
    const auto leader = pg->GetMembership().GetLeader();
    for (auto&& [i, m] : notsa::enumerate(pg->GetMembership().GetFollowers())) {
        if (m.m_pVehicle && m.bInVehicle && m.m_pVehicle == leader->m_pVehicle) { // Already in the leader's vehicle
            continue; 
        }
        CVehicle* mveh{};
        if (const auto t = m.GetTaskManager().Find<CTaskComplexEnterCarAsPassengerWait>(false); !t || !(mveh = t->GetCar())) {
            if (const auto t = m.GetTaskManager().Find<CTaskComplexEnterCarAsPassenger>(false); !t || !(mveh = t->GetTargetCar())) {
                continue;
            }
        }
        const auto SetTask = [&](const auto& task) {
            pg->GetIntelligence().SetEventResponseTask(&m, task);
        };
        const auto isVehOnFire = mveh->m_pFireParticle && mveh->m_pFireParticle->GetPlayStatus() == eFxSystemPlayStatus::FX_PLAYING;
        if (notsa::contains<int>({ 15, 16 }, mveh->m_pHandlingData->m_nAnimGroup)) { // TODO: Enums
            if (isVehOnFire) { // INVERTED - 0x5F9483 
                SetTask(CTaskComplexSequence{
                    new CTaskComplexLeaveCarAsPassengerWait{mveh},
                    new CTaskComplexSmartFleeEntity{
                        mveh,
                        false,
                        15.f,
                        1'000'000,
                        1000,
                        fEntityPosChangeThreshold
                    }
                });
            } else { // 0x5F937C
                SetTask(CTaskComplexLeaveCarAsPassengerWait{
                    mveh
                });
            }
        } else {
            // NOTSA/NOTE:
            // To make this behave the same as the OG code we'd have to either:
            // - Use `pg->GetMembership() * 500`
            // - Use an `for i` loop [to iterate the array]
            // But I'm way too fancy to do that, so...
            if (isVehOnFire) { // INVERTED
                SetTask(CTaskComplexSequence{ // 0x5F9193
                    new CTaskComplexLeaveCar{
                        mveh,
                        eTargetDoor::TARGET_DOOR_FRONT_LEFT,
                        CGeneral::GetRandomNumberInRange(-250, 250) + (i + 1) * 500, // SEE NOTE
                        false,
                        false
                    },
                    new CTaskComplexSmartFleeEntity{
                        mveh,
                        false,
                        15.f,
                        1'000'000,
                        1'000,
                        fEntityPosChangeThreshold
                    }
                });
            } else { // 0x5F92E7
                SetTask(CTaskComplexLeaveCar{
                     mveh,
                    eTargetDoor::TARGET_DOOR_FRONT_LEFT,
                    CGeneral::GetRandomNumberInRange(-250, 250) + i * 500, // SEE NOTE
                    false,
                    false
                });
            }
        }
    }
    return nullptr;
}

// 0x5F8900
CTaskAllocator* CGroupEventHandler::ComputeResponseLeaderEnteredCar(const CEvent& e, CPedGroup* pg, CPed* originator) {
    return plugin::CallAndReturn<CTaskAllocator*, 0x5F8900, const CEvent&, CPedGroup*, CPed*>(e, pg, originator);
}

// 0x5F9710
CTaskAllocator* CGroupEventHandler::ComputeResponseLeaderEnterExit(const CEventLeaderEntryExit& e, CPedGroup* pg, CPed* originator) {
    const auto pgms   = &pg->GetMembership();
    const auto leader = pgms->GetLeader();
    if (!leader) {
        return nullptr;
    }
    for (auto& m : pgms->GetFollowers()) {
        if (m.bInVehicle) {
            continue;
        }
        CTaskSimpleWaitUntilLeaderAreaCodesMatch task{leader};
        if (task.ProcessPed(&m)) {
            m.m_bUsesCollision = true;
        } else {
            pg->GetIntelligence().SetEventResponseTask(&m, task);
        }
    }
    return nullptr;
}

// 0x5FBD10
CTaskAllocator* CGroupEventHandler::ComputeResponseGunAimedAt(const CEventGunAimedAt& e, CPedGroup* pg, CPed* originator) {
    const auto src = e.GetSourceEntity();
    if (!src || !src->IsPed()) {
        return nullptr;
    }
    const auto srcPed = src->AsPed();
    MaybeAdjustTaskOfGroupThreatEvent(e, pg, originator, srcPed);
    switch (e.m_taskId) {
    case TASK_GROUP_KILL_THREATS_BASIC:  return ComputeKillThreatsBasicResponse(pg, srcPed, originator, false);
    case TASK_GROUP_FLEE_THREAT:         return ComputeFleePedResponse(pg, srcPed, originator, false);
    case TASK_GROUP_USE_MEMBER_DECISION: return ComputeMemberResponses(e, pg, originator);
    }
    return nullptr;
}

// 0x5F99F0
CTaskAllocator* CGroupEventHandler::ComputeResponseGather(const CEventPlayerCommandToGroupGather& e, CPedGroup* pg, CPed* originator) {
    for (auto& m : pg->GetMembership().GetFollowers()) {
        pg->GetIntelligence().SetEventResponseTask(
            &m,
            CTaskComplexSeekEntity<CEntitySeekPosCalculatorStandard>{
                originator,
                50'000,
                1'000,
                7.5f,
                2.f,
                2.f,
                true,
                true,
                CEntitySeekPosCalculatorStandard{}
            }
        );
    }
    return nullptr;
}

// 0x5FBE70
CTaskAllocator* CGroupEventHandler::ComputeResponseDraggedOutCar(const CEventDraggedOutCar& e, CPedGroup* pg, CPed* originator) {
    if (!e.m_CarJacker) {
        return nullptr;
    }
    assert(!e.m_CarJacker->IsPed()); // Original code just `returns nullptr` in this case, but but since `m_CarJacker` is typed as `CPed*` it *should* be at least a `CPed*`
    switch (e.m_taskId) {
    case TASK_GROUP_KILL_THREATS_BASIC:
        return e.m_CarJacker->IsPlayer() && originator && originator->GetIntelligence()->Respects(e.m_CarJacker) && !pg->m_bIsMissionGroup
            ? ComputeFleePedResponse(pg, e.m_CarJacker, originator, false)
            : ComputeKillThreatsBasicResponse(pg, e.m_CarJacker, originator, false);
    case TASK_GROUP_FLEE_THREAT:         return ComputeFleePedResponse(pg, e.m_CarJacker, originator, false);
    case TASK_GROUP_USE_MEMBER_DECISION: return ComputeMemberResponses(e, pg, originator);
    case TASK_GROUP_EXIT_CAR:            return ComputeResponseLeaderExitedCar(static_cast<const CEventEditableResponse&>(e), pg, originator);
    }
    return nullptr;
}

// 0x5FB540
CTaskAllocator* CGroupEventHandler::ComputeResponseDanger(const CEventDanger& e, CPedGroup* pg, CPed* originator) {
    const auto esrc = e.GetSourceEntity();
    if (!esrc || !esrc->IsPed()) {
        return nullptr;
    }
    switch (e.m_taskId) {
    case TASK_GROUP_FLEE_THREAT: return ComputeFleePedResponse(pg, esrc->AsPed(), originator, false);
    }
    return nullptr;
}

// 0x5FBF50
CTaskAllocator* CGroupEventHandler::ComputeResponseDamage(const CEventDamage& e, CPedGroup* pg, CPed* originator) {
    const auto src = e.GetSourceEntity();
    if (!src || !src->IsPed()) {
        return nullptr;
    }
    const auto srcPed = src->AsPed();
    MaybeAdjustTaskOfGroupThreatEvent(e, pg, originator, srcPed);
    switch (e.m_taskId) {
    case TASK_GROUP_KILL_THREATS_BASIC:  return ComputeKillThreatsBasicResponse(pg, srcPed, originator,  true);
    case TASK_GROUP_KILL_PLAYER_BASIC:   return ComputeKillPlayerBasicResponse(pg, srcPed, originator, true);
    case TASK_GROUP_FLEE_THREAT:         return ComputeFleePedResponse(pg, srcPed, originator, true);
    case TASK_GROUP_USE_MEMBER_DECISION: return ComputeMemberResponses(e, pg, originator);
    }
    return nullptr;
}

// 0x5F9530
CTaskAllocator* CGroupEventHandler::ComputeResponsLeaderQuitEnteringCar(const CEvent& e, CPedGroup* pg, CPed* originator) {
    return plugin::CallAndReturn<CTaskAllocator*, 0x5F9530, const CEvent&, CPedGroup*, CPed*>(e, pg, originator);
}

// 0x5FAA50
CTaskAllocator* CGroupEventHandler::ComputeMemberResponses(const CEventEditableResponse& e, CPedGroup* pg, CPed* originator) {
    const std::unique_ptr<CEventEditableResponse> ce{ static_cast<CEventEditableResponse*>(const_cast<CEventEditableResponse&>(e).Clone()) };
    for (auto& m : pg->GetMembership().GetFollowers()) {
        if (!m.IsAlive()) {
            continue;
        }
        if (ce->HasEditableResponse()) {
            ce->m_taskId = TASK_NONE;
            ce->ComputeResponseTaskType(&m, true);
        } else if (const auto rt = std::unique_ptr<CTask>(CEventHandler::ComputeEventResponseTask(m, *ce))) {
            pg->GetIntelligence().SetEventResponseTask(&m, *rt);
        }
    }
    return nullptr;
}

// 0x5F9B20
CTaskAllocator* CGroupEventHandler::ComputeLeanOnVehicleResponse(const CEventLeanOnVehicle& e, CPedGroup* pg, CPed* originator) {
    const auto leader = pg->GetMembership().GetLeader();
    if (!leader) {
        return nullptr;
    }
    pg->GetIntelligence().SetEventResponseTask(
        leader,
        CTaskGoToVehicleAndLean{e.m_vehicle, e.m_leanAnimDurationInMs}
    );
    return nullptr;
}

// 0x5FB590
CTaskAllocator* CGroupEventHandler::ComputeKillThreatsBasicResponse(CPedGroup* pg, CPed* threat, CPed* originator, bool bDamageOriginator) {
    if (!IsKillTaskAppropriate(pg, threat)) {
        return ComputeFleePedResponse(pg, threat, originator, false);
    }
    if (pg->m_bIsMissionGroup) {
        return new CTaskAllocatorKillThreatsBasic{threat};
    }
    return new CTaskAllocatorKillThreatsBasicRandomGroup{threat};
}

// 0x5FB670
CTaskAllocator* CGroupEventHandler::ComputeKillPlayerBasicResponse(CPedGroup* pg, CPed* threat, CPed* originator, bool bDamageOriginator) {
    if (!threat) {
        return nullptr;
    }
    if (!IsKillTaskAppropriate(pg, threat)) {
        return ComputeFleePedResponse(pg, threat, originator, false);
    }
    for (auto& m : pg->GetMembership().GetMembers()) {
        pg->GetIntelligence().SetEventResponseTask(
            &m,
            CTaskComplexKillPedOnFoot{ threat }
        );
    }
    return nullptr;
}

// 0x5F9D50
CTaskAllocator* CGroupEventHandler::ComputeHassleThreatResponse(CPedGroup* pg, CPed* threat, CPed* originator, bool bBeAggressive) {
    if (!threat) {
        return nullptr;
    }
    const auto leader = pg->GetMembership().GetLeader();
    if (!leader) {
        return nullptr;
    }
    if ((leader->GetPosition() - threat->GetPosition()).SquaredMagnitude() >= sq(12.f)) {
        return nullptr;
    }
    for (auto& m : pg->GetMembership().GetMembers()) {
        const auto SetTask = [&](const auto& t) {
            pg->GetIntelligence().SetEventResponseTask(&m, t);
        };
        if (threat->IsInVehicle() && threat->m_pVehicle->IsSubAutomobile()) {
            if (!bBeAggressive) {
                if (threat->m_pVehicle->GetSpareHasslePosId() != -1) {
                    SetTask(CTaskGangHassleVehicle{
                        threat->m_pVehicle,
                        -1,
                        0,
                        0.25f,
                        0.65f
                    });
                }
            }
        } else {
            SetTask(CTaskGangHasslePed{
                threat,
                bBeAggressive ? 2 : 1, // TODO: Enum
                12'000,
                20'000
            });
        }
    }
    return nullptr;
}

// 0x5FA020
CTaskAllocator* CGroupEventHandler::ComputeHassleSexyPedResponse(CPedGroup* pg, CPed* sexyPed, CPed* originator) {
    if (!sexyPed) {
        return nullptr;
    }
    for (auto& m : pg->GetMembership().GetFollowers()) {
        if (!CGeneral::RandomBool(25.f)) {
            continue;
        }
        pg->GetIntelligence().SetEventResponseTask(
            &m,
            CTaskGangHasslePed{sexyPed, 0, 8'000, 12'000}
        );
    }
    return nullptr;
}

// 0x5FA820
CTaskAllocator* CGroupEventHandler::ComputeHandSignalResponse(CPedGroup* pg, CPed* signalAt, CPed* originator) {
    if (!signalAt) {
        return nullptr;
    }
    for (auto& m : pg->GetMembership().GetMembers()) {
        if (m.IsPlayer()) {
            continue;
        }
        pg->GetIntelligence().SetEventResponseTask(
            &m,
            CTaskComplexSignalAtPed{signalAt}
        );
    }
    return nullptr;
}

// 0x5FA550
CTaskAllocator* CGroupEventHandler::ComputeGreetResponse(CPedGroup* pg, CPed* toGreet, CPed* originator) {
    if (!toGreet) {
        return nullptr;
    }
    if (!toGreet->GetTaskManager().Has<TASK_COMPLEX_WANDER>()) {
        return nullptr;
    }
    const auto leader = pg->GetMembership().GetLeader();
    if (!leader) {
        return nullptr;
    }
    const auto [closestToGreeter, closestToGreeterDistSq] = pg->GetMembership().GetMemberClosestTo(toGreet);
    if (!closestToGreeter || closestToGreeterDistSq < sq(4.f) && closestToGreeterDistSq > sq(10.f)) {
        return nullptr;
    }
    const auto partnerType = CGeneral::GetRandomNumberInRange(0, 6);
    pg->GetIntelligence().SetEventResponseTask(
        closestToGreeter,
        CTaskComplexPartnerGreet{
            "CompGreetResp",
            toGreet,
            true,
            0.f,
            partnerType,
            {}
        }
    );
    toGreet->GetEventGroup().Add(CEventCreatePartnerTask{
        partnerType + 1,
        closestToGreeter,
        false,
        0.f // Seems odd
    });
    for (auto& m : pg->GetMembership().GetMembers()) {
        if (&m == closestToGreeter) {
            continue;
        }
        g_ikChainMan.LookAt(
            "CompGreetResp",
            &m,
            closestToGreeter,
            CGeneral::GetRandomNumberInRange(3000, 6000),
            BONE_HEAD,
            nullptr,
            true,
            0.25f,
            500,
            3,
            0
        );
    }
    return nullptr;
}

// 0x5FA290
CTaskAllocator* CGroupEventHandler::ComputeDoDealResponse(CPedGroup* pg, CPed* dealWith, CPed* originator) { // Pretty much copy-paste of `ComputeGreetResponse`
    if (!dealWith) {
        return nullptr;
    }
    if (!dealWith->GetTaskManager().Has<TASK_COMPLEX_WANDER>()) {
        return nullptr;
    }
    const auto leader = pg->GetMembership().GetLeader();
    if (!leader) {
        return nullptr;
    }
    const auto [closestToGreeter, closestToGreeterDistSq] = pg->GetMembership().GetMemberClosestTo(dealWith);
    if (!closestToGreeter || closestToGreeterDistSq < sq(4.f) && closestToGreeterDistSq > sq(10.f)) {
        return nullptr;
    }
    const auto partnerType = CGeneral::GetRandomNumberInRange(0, 6);
    pg->GetIntelligence().SetEventResponseTask(
        closestToGreeter,
        CTaskComplexPartnerDeal{
            "CompDoDealResp",
            dealWith,
            true,
            0.f,
            {}
        }
    );
    dealWith->GetEventGroup().Add(CEventCreatePartnerTask{
        partnerType + 1,
        closestToGreeter,
        false,
        0.f // Seems odd
    });
    for (auto& m : pg->GetMembership().GetMembers()) {
        if (&m == closestToGreeter) {
            continue;
        }
        g_ikChainMan.LookAt(
            "CompGreetResp",
            &m,
            closestToGreeter,
            CGeneral::GetRandomNumberInRange(3000, 6000),
            BONE_HEAD,
            nullptr,
            true,
            0.25f,
            500,
            3,
            0
        );
    }
    return nullptr;
}

// 0x5FA130
CTaskAllocator* CGroupEventHandler::ComputeFleePedResponse(CPedGroup* pg, CPed* threat, CPed* originator, bool bDamageOriginator) {
    if (!threat) {
        return nullptr;
    }
    const auto leader = pg->GetMembership().GetLeader();
    if (!leader) {
        return nullptr;
    }
    /* rand(); */
    for (auto& m : pg->GetMembership().GetMembers()) {
        pg->GetIntelligence().SetEventResponseTask(
            threat,
            CTaskComplexSmartFleeEntity{
                &m,
                false,
                60.f,
                10'000,
                1'000,
                fEntityPosChangeThreshold
            }
        );
    }
    return nullptr;
}

// 0x5F7A00
CTaskAllocator* CGroupEventHandler::ComputeDrivebyResponse(CPedGroup* pg, CPed* threat, CPed* originator) {
    return new CTaskAllocatorKillThreatsDriveby{threat};
}

// 0x5FC200
CTaskAllocator* CGroupEventHandler::ComputeEventResponseTasks(const CEventGroupEvent& ge, CPedGroup* g) {
    const auto& p = ge.m_ped;
    const auto& e = ge.m_event;
    switch (e->GetEventType()) {
    case EVENT_DRAGGED_OUT_CAR:
        return ComputeResponseDraggedOutCar(static_cast<const CEventDraggedOutCar&>(*e), g, p);
    case EVENT_DAMAGE:
        return ComputeResponseDamage(static_cast<const CEventDamage&>(*e), g, p);
    case EVENT_SHOT_FIRED:
        return ComputeResponseShotFired(static_cast<const CEventGunShot&>(*e), g, p);
    case EVENT_SEXY_PED:
        return ComputeResponseSexyPed(static_cast<const CEventSexyPed&>(*e), g, p);
    case EVENT_GUN_AIMED_AT:
        return ComputeResponseGunAimedAt(static_cast<const CEventGunAimedAt&>(*e), g, p);
    case EVENT_ACQUAINTANCE_PED_HATE:
    case EVENT_ACQUAINTANCE_PED_DISLIKE:
        return ComputeResponsePedThreat(static_cast<const CEventAcquaintancePed&>(*e), g, p);
    case EVENT_ACQUAINTANCE_PED_LIKE:
    case EVENT_ACQUAINTANCE_PED_RESPECT:
        return ComputeResponsePedFriend(static_cast<const CEventAcquaintancePed&>(*e), g, p);
    case EVENT_VEHICLE_DAMAGE_WEAPON:
    case EVENT_VEHICLE_DAMAGE_COLLISION:
        return ComputeResponseVehicleDamage(static_cast<const CEventVehicleDamage&>(*e), g, p);
    case EVENT_LEADER_ENTERED_CAR_AS_DRIVER:
        return ComputeResponseLeaderEnteredCar(*e, g, p);
    case EVENT_LEADER_EXITED_CAR_AS_DRIVER:
        return ComputeResponseLeaderExitedCar(static_cast<const CEventLeaderExitedCarAsDriver&>(*e), g, p);
    case EVENT_LEADER_QUIT_ENTERING_CAR_AS_DRIVER:
        return ComputeResponsLeaderQuitEnteringCar(*e, g, p);
    case EVENT_PLAYER_COMMAND_TO_GROUP:
    case EVENT_PLAYER_COMMAND_TO_GROUP_GATHER:
        return ComputeResponsePlayerCommand(static_cast<const CEventPlayerCommandToGroup&>(*e), g, p);
    case EVENT_SEEN_COP:
        return ComputeResponseSeenCop(static_cast<const CEventSeenCop&>(*e), g, p);
    case EVENT_DANGER:
        return ComputeResponseDanger(static_cast<const CEventDanger&>(*e), g, p);
    case EVENT_LEADER_ENTRY_EXIT:
        return ComputeResponseLeaderEnterExit(static_cast<const CEventLeaderEntryExit&>(*e), g, p);
    case EVENT_NEW_GANG_MEMBER:
        return ComputeResponseNewGangMember(static_cast<const CEventNewGangMember&>(*e), g, p);
    case EVENT_LEAN_ON_VEHICLE:
        return ComputeLeanOnVehicleResponse(static_cast<const CEventLeanOnVehicle&>(*e), g, p);
    default:
        return nullptr;
    }
}
