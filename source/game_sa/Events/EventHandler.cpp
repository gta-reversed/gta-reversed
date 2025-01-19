#include "StdInc.h"

#include "EventHandler.h"
#include "InterestingEvents.h"
#include "IKChainManager_c.h"
#include "PedStats.h"

#include "Tasks/TaskTypes/TaskComplexFacial.h"
#include "Tasks/TaskTypes/TaskSimpleUseGun.h"
#include "Tasks/TaskTypes/TaskSimpleStandStill.h"
#include "Tasks/TaskTypes/TaskComplexInAirAndLand.h"
#include "Tasks/TaskTypes/TaskComplexStuckInAir.h"
#include "Tasks/TaskTypes/TaskSimpleWaitUntilAreaCodesMatch.h"
#include "Tasks/TaskTypes/TaskComplexUseEffect.h"
#include "Tasks/TaskTypes/TaskComplexKillPedOnFoot.h"
#include "Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h"
#include "Tasks/TaskTypes/TaskComplexWalkRoundObject.h"
#include "Tasks/TaskTypes/TaskSimpleGoTo.h"
#include "Tasks/TaskTypes/TaskSimpleHitHead.h"
#include "Tasks/TaskTypes/TaskComplexEnterCarAsPassengerWait.h"
#include "Tasks/TaskTypes/TaskComplexWalkRoundBuildingAttempt.h"
#include "Tasks/TaskTypes/TaskComplexMoveBackAndJump.h"
#include "Tasks/TaskTypes/TaskComplexJump.h"
#include "Tasks/TaskTypes/TaskComplexGangFollower.h"
#include "Tasks/TaskTypes/TaskSimpleClimb.h"
#include "Tasks/TaskTypes/TaskComplexLeaveCar.h"
#include "Tasks/TaskTypes/TaskComplexPartnerChat.h"
#include "Tasks/TaskTypes/TaskComplexPartnerDeal.h"
#include "Tasks/TaskTypes/TaskComplexPartnerGreet.h"
#include "Tasks/TaskTypes/TaskComplexCarDriveMission.h"
#include "Tasks/TaskTypes/TaskComplexSmartFleeEntity.h"
#include "Tasks/TaskTypes/TaskComplexInvestigateDeadPed.h"
#include "Tasks/TaskTypes/TaskSimpleDuck.h"
#include "Tasks/TaskTypes/TaskSimpleDead.h"
#include "Tasks/TaskTypes/TaskComplexGangJoinRespond.h"
#include "Tasks/TaskTypes/TaskComplexEnterCarAsDriver.h"
#include "Tasks/TaskTypes/TaskComplexEnterCarAsPassenger.h"
#include "Tasks/TaskTypes/TaskSimplePause.h"
#include "Tasks/TaskTypes/TaskSimpleAchieveHeading.h"
#include "Tasks/TaskTypes/TaskSimpleShakeFist.h"
#include "Tasks/TaskTypes/TaskComplexStealCar.h"
#include "Tasks/TaskTypes/TaskComplexDragPedFromCar.h"
#include "Tasks/TaskTypes/TaskComplexKillCriminal.h"
#include "Tasks/TaskTypes/TaskComplexDestroyCar.h"
#include "Tasks/TaskTypes/TaskComplexExtinguishFires.h"
#include "Tasks/TaskTypes/TaskComplexUseClosestFreeScriptedAttractor.h"
#include "Tasks/TaskTypes/TaskComplexUseClosestFreeScriptedAttractorRun.h"
#include "Tasks/TaskTypes/TaskComplexUseClosestFreeScriptedAttractorSprint.h"
#include "Tasks/TaskTypes/TaskSimpleCower.h"
#include "Tasks/TaskTypes/TaskSimpleHandsUp.h"
#include "Tasks/TaskTypes/TaskComplexReactToGunAimedAt.h"
#include "Tasks/TaskTypes/TaskComplexKillPedOnFootStealth.h"
#include "Tasks/TaskTypes/TaskComplexCarDriveMission.h"
#include "Tasks/TaskTypes/TaskComplexFleeEntity.h"
#include "Tasks/TaskTypes/TaskComplexFleeAnyMeans.h"
#include "Tasks/TaskTypes/TaskComplexDie.h"
#include "Tasks/TaskTypes/TaskSimpleStealthKill.h"
#include "Tasks/TaskTypes/TaskComplexFallToDeath.h"
#include "Tasks/TaskTypes/TaskSimpleFall.h"
#include "Tasks/TaskTypes/TaskSimpleGetUp.h"
#include "Tasks/TaskTypes/TaskSimpleGunControl.h"
#include "Tasks/TaskTypes/TaskSimpleChoking.h"
#include "Tasks/TaskTypes/TaskSimpleBeHit.h"
#include "Tasks/TaskTypes/TaskComplexFallAndGetUp.h"
#include "Tasks/TaskTypes/TaskComplexInWater.h"
#include "Tasks/TaskTypes/Interior/TaskInteriorUseInfo.h"
#include "Tasks/TaskTypes/TaskComplexDiveFromAttachedEntityAndGetUp.h"
#include "Tasks/TaskTypes/TaskComplexWalkRoundObject.h"
#include "Tasks/TaskTypes/TaskSimplePlayerOnFire.h"
#include "Tasks/TaskTypes/TaskComplexOnFire.h"
#include "Tasks/TaskTypes/TaskComplexPassObject.h"
#include "Tasks/TaskTypes/TaskComplexLeaveCarAndFlee.h"
#include "Tasks/TaskTypes/TaskComplexLeaveCarAndWander.h"
#include "Tasks/TaskTypes/TaskComplexProstituteSolicit.h"
#include "Tasks/TaskTypes/TaskComplexInvestigateDisturbance.h"
#include "Tasks/TaskTypes/TaskComplexSeekEntityAiming.h"
#include "Tasks/TaskTypes/TaskSimpleGangDriveBy.h"
#include "Tasks/TaskTypes/TaskComplexDriveWander.h"
#include "Tasks/TaskTypes/TaskComplexFollowLeaderInFormation.h"
#include "Tasks/TaskTypes/TaskSimpleSay.h"
#include "Tasks/TaskTypes/TaskComplexHitResponse.h"
#include "Tasks/TaskTypes/TaskComplexAvoidOtherPedWhileWandering.h"
#include "Tasks/TaskTypes/TaskComplexPartnerShove.h"
#include "Tasks/TaskTypes/TaskComplexSmartFleePoint.h"
#include "Tasks/TaskTypes/TaskComplexFleePoint.h"
#include "Tasks/TaskTypes/TaskComplexWalkRoundFire.h"
#include "Tasks/TaskTypes/TaskComplexGetUpAndStandStill.h"
#include "Tasks/TaskTypes/TaskComplexWander.h"
#include "Tasks/TaskTypes/TaskGangHasslePed.h"
#include "Tasks/TaskTypes/TaskComplexSignalAtPed.h"
#include "Tasks/TaskTypes/TaskComplexArrestPed.h"
#include "Tasks/TaskTypes/TaskComplexHitPedWithCar.h"
#include "Tasks/TaskTypes/TaskComplexEvasiveStep.h"
#include "Tasks/TaskTypes/TaskComplexWalkRoundCar.h"
#include "Tasks/TaskTypes/TaskComplexRoadRage.h"
#include "Tasks/TaskTypes/TaskComplexLeaveAnyCar.h"
#include "Tasks/TaskTypes/TaskComplexScreamInCarThenLeave.h"

#include "Events/Event.h"
#include "Events/EventCarUpsideDown.h"
#include "Events/EventDamage.h"
#include "Events/EventAreaCodes.h"
#include "Events/EventAttractor.h"
#include "Events/EventBuildingCollision.h"
#include "Events/EventChatPartner.h"
#include "Events/EventCopCarBeingStolen.h"
#include "Events/EventCreatePartnerTask.h"
#include "Events/EventDanger.h"
#include "Events/EventDeadPed.h"
#include "Events/EventDeath.h"
#include "Events/EventDontJoinPlayerGroup.h"
#include "Events/EventDraggedOutCar.h"
#include "Events/EventFireNearby.h"
#include "Events/EventDamage.h"
#include "Events/EventGotKnockedOverByCar.h"
#include "Events/EventGunAimedAt.h"
#include "Events/EventHighAngerAtPlayer.h"
#include "Events/EventInWater.h"
#include "Events/EventInteriorUseInfo.h"
#include "Events/EventLowAngerAtPlayer.h"
#include "Events/EventHealthLow.h"
#include "Events/EventPotentialWalkIntoObject.h"
#include "Events/EventObjectCollision.h"
#include "Events/EventPotentialWalkIntoFire.h"
#include "Events/EventOnFire.h"
#include "Events/EventPassObject.h"
#include "Events/EventAcquaintancePed.h"
#include "Events/EventSoundQuiet.h"
#include "Events/EventAcquaintancePedHateBadlyLit.h"
#include "Events/EventAcquaintancePedHate.h"
#include "Events/EventPedToChase.h"
#include "Events/EventPedToFlee.h"
#include "Events/EventPlayerCollisionWithPed.h"
#include "Events/EventPotentialWalkIntoPed.h"
#include "Events/EventHealthReallyLow.h"
#include "Events/EventRevived.h"
#include "Events/EventScriptCommand.h"
#include "Events/EventSeenCop.h"
#include "Events/EventSeenPanickedPed.h"
#include "Events/EventSexyPed.h"
#include "Events/EventSexyVehicle.h"
#include "Events/EventGunShot.h"
#include "Events/EventGunShotWhizzedBy.h"
#include "Events/EventSignalAtPed.h"
#include "Events/EventSpecial.h"
#include "Events/EventVehicleCollision.h"
#include "Events/EventVehicleDamage.h"
#include "Events/EventVehicleDied.h"
#include "Events/EventVehicleHitAndRun.h"
#include "Events/EventVehicleOnFire.h"
#include "Events/EventPotentialGetRunOver.h"
#include "Events/EventPotentialWalkIntoVehicle.h"
#include "Events/EventHitByWaterCannon.h"
#include "Events/EventPedEnteredMyVehicle.h"
#include "Events/EventVehicleToSteal.h"

constexpr auto fSafeDistance = 60.f;

/*****
 * Refactor idea:
 * `Compute*` functions could just return:
 * `Response::Event ComputeBuildingCollisionResponse();`
 * So `SetEventResponseTask` and `ComputeEventResponseTask` could completely be replaced
 * And we could remove the ugly lambda wrapping from all the `Compute*` functions.
 *****
namespace Response {
enum class ResponseType {
    PHYSICAL,
    EVENT,
    SECONDARY_AIM,
    SECONDARY_SAY,
    SECONDARY_PARTIAL_ANIM,
};
template<ResponseType RespType>
struct Base {
    static inline constexpr auto Type = RespType;

    CTask* task;
};
struct Physical : Base<ResponseType::PHYSICAL> {};
struct Primary: Base<ResponseType::EVENT>{};
struct SecondaryAim: Base<ResponseType::SECONDARY_AIM>{};
struct SecondarySay : Base<ResponseType::SECONDARY_SAY>{};
struct SecondaryPartialAnim : Base<ResponseType::SECONDARY_PARTIAL_ANIM>{};

template<typename... Ts>
void ProcessEventResponse( // TODO: Make this a method
    CTask*& physical,
    CTask*& event,
    CTask*& secondaryAim,
    CTask*& secondarySay,
    CTask*& secondaryPartialAnim,
    std::tuple<Ts...> responses
) {
    std::apply([]<typename T>(T r){
        using enum ResponseType;
        switch (T::Type) {
        case PHYSICAL:               physical = r;             break;
        case EVENT:                  event = r;                break;
        case SECONDARY_AIM:          secondaryAim = r;         break;
        case SECONDARY_SAY:          secondarySay = r;         break;
        case SECONDARY_PARTIAL_ANIM: secondaryPartialAnim = r; break;
        default:                     NOTSA_UNREACHABLE();
        }
    }, responses);
}

template<typename T>
void Process( // TODO: Make this a method
    CTask*& physical,
    CTask*& event,
    CTask*& secondaryAim,
    CTask*& secondarySay,
    CTask*& secondaryPartialAnim,
    T       response
) {
    return Process(
        physical
        event
        secondaryAim
        secondarySay
        secondaryPartialAnim
        std::make_tuple(response)
    );
}
};*/

void CEventHandler::InjectHooks() {
    RH_ScopedClass(CEventHandler);
    RH_ScopedCategory("Events");

    RH_ScopedInstall(Constructor, 0x4C3E80);
    RH_ScopedInstall(Destructor, 0x4C3EC0);

    RH_ScopedInstall(Flush, 0x4C3790);
    RH_ScopedInstall(FlushImmediately, 0x4C3820);
    RH_ScopedInstall(GetCurrentEventType, 0x4B8CC0);
    RH_ScopedInstall(HandleEvents, 0x4C3F10);
    RH_ScopedInstall(IsKillTaskAppropriate, 0x4BC3E0);
    RH_ScopedInstall(IsTemporaryEvent, 0x4BC370);
    // RH_ScopedInstall(RecordActiveEvent, 0x0, { .reversed = false });
    // RH_ScopedInstall(RecordPassiveEvent, 0x0, { .reversed = false });
    RH_ScopedInstall(RegisterKill, 0x4B9340);
    RH_ScopedInstall(SetEventResponseTask, 0x4BC600);
    RH_ScopedInstall(ComputeAreaCodesResponse, 0x4BBF50);
    RH_ScopedInstall(ComputeAttractorResponse, 0x4B9BE0);
    // RH_ScopedInstall(ComputeBuildingCollisionPassiveResponse, 0x0, { .reversed = false });
    RH_ScopedInstall(ComputeBuildingCollisionResponse, 0x4BF2B0);
    RH_ScopedInstall(ComputeCarUpsideDownResponse, 0x4BBC30);
    RH_ScopedInstall(ComputeChatPartnerResponse, 0x4B98E0);
    RH_ScopedInstall(ComputeCopCarBeingStolenResponse, 0x4BB740);
    RH_ScopedInstall(ComputeCreatePartnerTaskResponse, 0x4BB130);
    RH_ScopedInstall(ComputeDamageResponse, 0x4C0170);
    RH_ScopedInstall(ComputeDangerResponse, 0x4BC230);
    RH_ScopedInstall(ComputeDeadPedResponse, 0x4B9470);
    RH_ScopedInstall(ComputeDeathResponse, 0x4B9400);
    RH_ScopedInstall(ComputeDontJoinGroupResponse, 0x4BC1D0);
    RH_ScopedInstall(ComputeDraggedOutCarResponse, 0x4BCC30);
    RH_ScopedInstall(ComputeFireNearbyResponse, 0x4BBFB0);
    RH_ScopedInstall(ComputeGotKnockedOverByCarResponse, 0x4C3430);
    RH_ScopedInstall(ComputeGunAimedAtResponse, 0x4C2840);
    RH_ScopedInstall(ComputeHighAngerAtPlayerResponse, 0x4BAC10);
    RH_ScopedInstall(ComputeInWaterResponse, 0x4BAF80);
    RH_ScopedInstall(ComputeInteriorUseInfoResponse, 0x4BAFE0);
    RH_ScopedInstall(ComputeKnockOffBikeResponse, 0x4B9FF0, { .reversed = false });
    RH_ScopedInstall(ComputeLowAngerAtPlayerResponse, 0x4BAAD0);
    RH_ScopedInstall(ComputeLowHealthResponse, 0x4BA990);
    RH_ScopedInstall(ComputeObjectCollisionPassiveResponse, 0x4BBB90);
    RH_ScopedInstall(ComputeObjectCollisionResponse, 0x4B92B0);
    RH_ScopedInstall(ComputeOnEscalatorResponse, 0x4BC150);
    RH_ScopedInstall(ComputeOnFireResponse, 0x4BAD50);
    RH_ScopedInstall(ComputePassObjectResponse, 0x4BB0C0);
    RH_ScopedInstall(ComputePedCollisionWithPedResponse, 0x4BDB80, { .reversed = false });
    RH_ScopedInstall(ComputePedCollisionWithPlayerResponse, 0x4BE7D0, { .reversed = false });
    RH_ScopedInstall(ComputePedEnteredVehicleResponse, 0x4C1590);
    RH_ScopedInstall(ComputePedFriendResponse, 0x4B9DD0);
    RH_ScopedInstall(ComputePedSoundQuietResponse, 0x4B9D40);
    RH_ScopedInstall(ComputePedThreatBadlyLitResponse, 0x4B9C90);
    RH_ScopedInstall(ComputePedThreatResponse, 0x4C19A0);
    RH_ScopedInstall(ComputePedToChaseResponse, 0x4C1910);
    RH_ScopedInstall(ComputePedToFleeResponse, 0x4B9B50);
    RH_ScopedInstall(ComputePersonalityResponseToDamage, 0x4BF9B0);
    RH_ScopedInstall(ComputePlayerCollisionWithPedResponse, 0x4B8CE0);
    RH_ScopedInstall(ComputePlayerWantedLevelResponse, 0x4BB280);
    RH_ScopedInstall(ComputePotentialPedCollideResponse, 0x4C2610);
    RH_ScopedInstall(ComputePotentialWalkIntoFireResponse, 0x4BBCD0);
    RH_ScopedInstall(ComputeReallyLowHealthResponse, 0x4BAA30);
    RH_ScopedInstall(ComputeReviveResponse, 0x4B97B0);
    RH_ScopedInstall(ComputeScriptCommandResponse, 0x4BA7C0);
    RH_ScopedInstall(ComputeSeenCopResponse, 0x4BC050);
    RH_ScopedInstall(ComputeSeenPanickedPedResponse, 0x4C35F0);
    RH_ScopedInstall(ComputeSexyPedResponse, 0x4B99F0);
    RH_ScopedInstall(ComputeSexyVehicleResponse, 0x4B9AA0);
    RH_ScopedInstall(ComputeShotFiredResponse, 0x4BC710);
    RH_ScopedInstall(ComputeShotFiredWhizzedByResponse, 0x4BBE30);
    RH_ScopedInstall(ComputeSignalAtPedResponse, 0x4BB050);
    RH_ScopedInstall(ComputeSpecialResponse, 0x4BB800);
    RH_ScopedInstall(ComputeVehicleCollisionResponse, 0x4BD6A0);
    RH_ScopedInstall(ComputeVehicleDamageResponse, 0x4C2FC0);
    RH_ScopedInstall(ComputeVehicleDiedResponse, 0x4BA8B0);
    // RH_ScopedInstall(ComputeVehicleHitAndRunResponse, 0x0, { .reversed = false });
    RH_ScopedInstall(ComputeVehicleOnFireResponse, 0x4BB2E0);
    RH_ScopedInstall(ComputeVehiclePotentialCollisionResponse, 0x4C0BD0);
    RH_ScopedInstall(ComputeVehiclePotentialPassiveCollisionResponse, 0x4B96D0);
    RH_ScopedInstall(ComputeVehicleToStealResponse, 0x4B9F80);
    RH_ScopedInstall(ComputeWaterCannonResponse, 0x4BAE30);

    RH_ScopedOverloadedInstall(ComputeEventResponseTask, "0", 0x4C3870, void (CEventHandler::*)(CEvent*, CTask*));
    RH_ScopedOverloadedInstall(ComputeEventResponseTask, "Ped", 0x4C4220, CTask*(*)(const CPed&, const CEvent&));
}

// 0x4C3E80
CEventHandler::CEventHandler(CPed* ped) :
    m_Ped{ped}
{
}

// 0x4C3F10
void CEventHandler::HandleEvents() {
    m_History.TickStoredEvent(m_Ped);

    const auto lastAbortedTask  = m_History.GetAbortedTask();

    const auto pedTM                   = &m_Ped->GetTaskManager();
    const auto primaryTask             = m_Ped->GetIntelligence()->GetActivePrimaryTask();
    const auto tempEventRespTask       = pedTM->GetTemporaryEventResponseTask();
    const auto presistentEventRespTask = pedTM->GetPresistentEventResponseTask();

    const auto pedEG                     = &m_Ped->GetEventGroup();
    const auto pedEGHighestPriorityEvent = pedEG->GetHighestPriorityEvent();

    pedEG->TickEvents();

    if (!lastAbortedTask) {
        m_History.RecordAbortedTask(primaryTask && lastAbortedTask == primaryTask ? primaryTask : nullptr);
    }

    if (!tempEventRespTask) {
        if (!presistentEventRespTask) {
            m_History.RecordAbortedTask(nullptr);
        }
        m_History.ClearTempEvent();
    }

    if (!presistentEventRespTask) {
        m_History.ClearNonTempEvent();
    }

    if (const auto priorityEvent = pedEG->GetHighestPriorityEvent()) {
        if (priorityEvent->GetEventType() == EVENT_DAMAGE) { // 0x4C4004
            m_Ped->SetIsStatic(false);
        }

        if (const auto currEvent = m_History.GetCurrentEvent()) { // 0x4C4015
            if (currEvent->GetEventType() == pedEGHighestPriorityEvent->GetEventType()) {
                pedEG->Remove(pedEGHighestPriorityEvent);
                pedEG->RemoveInvalidEvents(false);
                pedEG->Reorganise();
                if (!primaryTask) {
                    m_History.RecordAbortedTask(nullptr);
                    m_History.ClearAllEvents();
                }
                return;
            }
        }

        if (m_History.TakesPriorityOverCurrentEvent(*priorityEvent)) { // 0x4C40DE
            const auto activeTask = pedTM->GetActiveTask();

            const auto hasStoppedTimers = [&]{
                if (!activeTask || activeTask->MakeAbortable(m_Ped, ABORT_PRIORITY_URGENT, priorityEvent)) {
                    pedTM->StopTimers(priorityEvent);
                    return true;
                }
                return false;
            }();

            if (primaryTask) {
                if (!hasStoppedTimers) { // 0x4C41A9
                    priorityEvent->UnTick();
                    activeTask->MakeAbortable(m_Ped, ABORT_PRIORITY_LEISURE, priorityEvent);
                    m_History.RecordAbortedTask(activeTask);
                    pedEG->RemoveInvalidEvents(false);
                    pedEG->Reorganise();
                    return;
                }
                m_History.RecordAbortedTask(nullptr);
            } else if (lastAbortedTask) { // 0x4C4176
                m_History.RecordAbortedTask(nullptr);
            } else if (!hasStoppedTimers) {
                priorityEvent->UnTick();
                pedEG->RemoveInvalidEvents(false);
                pedEG->Reorganise();
                return;
            }

            ComputeEventResponseTask(priorityEvent, lastAbortedTask);
            if (m_EventResponseTask) {
                m_History.RecordCurrentEvent(m_Ped, *priorityEvent);
            }
            SetEventResponseTask(*priorityEvent);
            pedEG->Remove(priorityEvent);
        }

        pedEG->RemoveInvalidEvents(false);
        pedEG->Reorganise();
    } else if (!primaryTask) {
        m_History.RecordAbortedTask(nullptr);
        m_History.ClearAllEvents();
    }
}

void CEventHandler::RecordActiveEvent(const CEvent& event) {
    assert(false);
}

void CEventHandler::RecordPassiveEvent(const CEvent& event) {
    assert(false);
}

// 0x4B9340
void CEventHandler::RegisterKill(const CPed* ped, const CEntity* inflictedBy, eWeaponType weaponUsed, bool wasHeadShot) {
    if (!ped) {
        return;
    }
    if (inflictedBy && inflictedBy->IsPed() && inflictedBy->AsPed()->IsPlayer() && inflictedBy != ped) {
        const auto pi = &FindPlayerInfo();
        pi->m_nHavocCaused += 10;
        pi->m_fCurrentChaseValue += 5.f;
        CDarkel::RegisterKillByPlayer(*ped, weaponUsed, wasHeadShot, inflictedBy->AsPed()->m_nPedType);
        CPedGroups::RegisterKillByPlayer();
    } else if (inflictedBy && inflictedBy->IsVehicle() && inflictedBy == FindPlayerVehicle()) {
        CStats::IncrementStat(STAT_PEOPLE_YOUVE_WASTED);
    } else {
        CDarkel::RegisterKillNotByPlayer(ped);
    }
}

// 0x4BC600
void CEventHandler::SetEventResponseTask(const CEvent& event) {
    const auto i  = m_Ped->GetIntelligence();
    const auto tm = &i->m_TaskMgr;

    if (m_PhysicalResponseTask) {
        i->AddTaskPhysResponse(m_PhysicalResponseTask, true);
    }

    if (m_EventResponseTask) {
        if (IsTemporaryEvent(event)) {
            i->AddTaskEventResponseTemp(m_EventResponseTask, false);
        } else {
            i->AddTaskEventResponseTemp(nullptr, 0);
            i->AddTaskEventResponseNonTemp(m_EventResponseTask, false);
        }
    }

    if (m_AttackTask) {
        if (const auto tAttack = tm->GetTaskSecondary(TASK_SECONDARY_ATTACK)) {
            tAttack->MakeAbortable(m_Ped);
        }
        tm->SetTaskSecondary(m_AttackTask, TASK_SECONDARY_ATTACK);
    }

    if (m_SayTask) {
        tm->SetTaskSecondary(m_SayTask, TASK_SECONDARY_SAY);
    }

    if (m_PartialAnimTask) {
        tm->SetTaskSecondary(m_PartialAnimTask, TASK_SECONDARY_PARTIAL_ANIM);
    }

    if (event.HasEditableResponse()) {
        const auto eResponse = &static_cast<const CEventEditableResponse&>(event);
        if (const auto tfacial = tm->GetTaskSecondaryFacial()) {
            if (eResponse->GetFacialExpressionType() != eFacialExpression::NONE) {
                tfacial->SetRequest(eResponse->GetFacialExpressionType(), 10'000);
            }
        }
    }
}

// @addr unk
// NOTE/BUG: Weird, no `delete`s?
void CEventHandler::ResetResponse() {
    m_PhysicalResponseTask = nullptr;
    m_EventResponseTask    = nullptr;
    m_AttackTask           = nullptr;
    m_SayTask              = nullptr;
    m_PartialAnimTask      = nullptr;
}

// 0x4C3790
void CEventHandler::Flush() {
    delete m_PhysicalResponseTask;
    m_PhysicalResponseTask = nullptr;

    delete m_EventResponseTask;
    m_EventResponseTask = nullptr;

    delete m_AttackTask;
    m_AttackTask = nullptr;

    delete m_SayTask;
    m_SayTask = nullptr;

    delete m_PartialAnimTask;
    m_PartialAnimTask = nullptr;

    m_History.Flush();
}

// 0x4C3820
void CEventHandler::FlushImmediately() {
    m_History.Flush();
}

// 0x4B8CC0
eEventType CEventHandler::GetCurrentEventType() const {
    if (const auto e = m_History.GetCurrentEvent()) {
        return e->GetEventType();
    }
    return EVENT_NONE;
}

// 0x4BC370
bool CEventHandler::IsTemporaryEvent(const CEvent& event) {
    auto eventType = event.GetEventType();
    switch (eventType) {
    case EVENT_VEHICLE_COLLISION:
    case EVENT_PED_COLLISION_WITH_PED:
    case EVENT_PED_COLLISION_WITH_PLAYER:
    case EVENT_PLAYER_COLLISION_WITH_PED:
    case EVENT_OBJECT_COLLISION:
    case EVENT_BUILDING_COLLISION:
    case EVENT_IN_AIR:
    case EVENT_POTENTIAL_WALK_INTO_VEHICLE:
    case EVENT_POTENTIAL_WALK_INTO_BUILDING:
    case EVENT_POTENTIAL_WALK_INTO_OBJECT:
    case EVENT_POTENTIAL_WALK_INTO_PED:
    case EVENT_POTENTIAL_WALK_INTO_FIRE:
    case EVENT_AREA_CODES:
    case EVENT_STUCK_IN_AIR:
    case EVENT_POTENTIAL_GET_RUN_OVER:
    case EVENT_ON_ESCALATOR:
    case EVENT_FIRE_NEARBY:
        return true;
    default:
        return false;
    }
}

// 0x4BC3E0
bool CEventHandler::IsKillTaskAppropriate(CPed* ped1, CPed* ped2, const CEvent& event) {
    if (ped1->IsCreatedByMission()) {
        return true;
    }
    if (!ped1->GetActiveWeapon().IsTypeMelee() || ped2->GetActiveWeapon().IsTypeMelee()) {
        return true;
    }
    return false;
}

// 0x4BBF50
void CEventHandler::ComputeAreaCodesResponse(CEventAreaCodes* e, CTask* tactive, CTask* tsimplest) {
    if (!e->m_ped) {
        return;
    }
    m_EventResponseTask = new CTaskSimpleWaitUntilAreaCodesMatch{e->m_ped};
}

// 0x4B9BE0
void CEventHandler::ComputeAttractorResponse(CEventAttractor* e, CTask* tactive, CTask* tsimplest) {
    if (e->GetEventType() == EVENT_ATTRACTOR && !e->m_entity) {
        return;
    }
    if (e->m_taskId == TASK_NONE) {
        m_EventResponseTask = nullptr;
    } else if (e->m_taskId == TASK_COMPLEX_USE_EFFECT) {
        if (GetPedAttractorManager()->HasEmptySlot(e->m_2dEffect, e->m_entity)) { // inverted
            m_EventResponseTask = new CTaskComplexUseEffect{e->m_2dEffect, e->m_entity};
        }
    }
}

// 0x0
// EVENT_POTENTIAL_WALK_INTO_BUILDING
// ANDROID IDB 0x3025F8 (1.0)
void CEventHandler::ComputeBuildingCollisionPassiveResponse(CEvent* e, CTask* tactive, CTask* tsimplest) {
    // NOP
}

// 0x4BF2B0
void CEventHandler::ComputeBuildingCollisionResponse(CEventBuildingCollision* e, CTask* tactive, CTask* tsimplest) {
    m_EventResponseTask = [&]() -> CTask* {
        if (!tactive || !e->m_building || e->m_moveState == PEDMOVE_STILL) {
            return nullptr;
        }

        const auto tm = &m_Ped->GetTaskManager();

        const auto tKillPedOnFoot    = tm->Find<CTaskComplexKillPedOnFoot>(false);
        const auto tSeekEntity       = tm->Find<CTaskComplexSeekEntity<>>(false); // Hackery, because we don't know what `T_PosCalc` actually is...
        const auto tEnterCarPsgrWait = tm->Find<CTaskComplexEnterCarAsPassengerWait>(false);
        const auto tGoToPoint        = CTask::IsGoToTask(tsimplest)
            ? static_cast<CTaskSimpleGoTo*>(tsimplest)
            : nullptr;

        const auto isHeadOnCollision = e->IsHeadOnCollision(m_Ped) ;

        if (e->CanTreatBuildingAsObject(e->m_building)) { // 0x4BF36E
            if (tGoToPoint) {
                return new CTaskComplexWalkRoundObject{
                    e->m_moveState,
                    tGoToPoint->m_vecTargetPoint,
                    e->m_building
                };
            }
            return nullptr;
        }

        if (m_Ped->bIsStanding && e->m_impactNormal.z > COS_45) { // 0x4BF3FE
            return nullptr;
        }

        if (!tGoToPoint && (!tKillPedOnFoot || !tKillPedOnFoot->m_target)) { // 0x4BF41E
            if (isHeadOnCollision && !CPedGeometryAnalyser::CanPedJumpObstacle(*m_Ped, *e->m_building, e->m_impactNormal, e->m_impactPos) && e->m_moveState > eMoveState::PEDMOVE_WALK) {
                return new CTaskSimpleHitHead{};
            }
        }

        if (isHeadOnCollision) { // 0x4BF4D1
            if (!tSeekEntity && !tKillPedOnFoot && !tEnterCarPsgrWait && !CGeneral::RandomBool(1.f / 15.f * 100.f)) {
                return new CTaskComplexWalkRoundBuildingAttempt{
                    (eMoveState)e->m_moveState,
                    tGoToPoint->m_vecTargetPoint,
                    e->m_impactPos,
                    e->m_impactNormal,
                    isHeadOnCollision
                };
            }

            if (CPedGeometryAnalyser::CanPedJumpObstacle(*m_Ped, *e->m_building, e->m_impactNormal, e->m_impactPos)) { // 0x4BF540
                if (e->m_moveState > eMoveState::PEDMOVE_WALK) {
                    return new CTaskComplexMoveBackAndJump{};
                }
                return new CTaskComplexJump{COMPLEX_JUMP_TYPE_JUMP};
            }

            if (tKillPedOnFoot) { // 0x4BF5C2
                if (!tKillPedOnFoot->m_target) {
                    return nullptr;
                }
                if (CPedGeometryAnalyser::CanPedTargetPed(*m_Ped, *tKillPedOnFoot->m_target, true)) {
                    return nullptr;
                }
                return new CTaskComplexWalkRoundBuildingAttempt{
                    (eMoveState)e->m_moveState,
                    tKillPedOnFoot->m_target,
                    {},
                    e->m_impactPos,
                    e->m_impactNormal,
                    isHeadOnCollision
                };
            }

            CEntity* targetEntity{};
            if (tSeekEntity) { // 0x4BF660
                targetEntity = tSeekEntity->GetEntityToSeek();
            } else if (!tEnterCarPsgrWait) { // 0x4BF664
                return new CTaskComplexWalkRoundBuildingAttempt{
                    (eMoveState)e->m_moveState,
                    tGoToPoint->m_vecTargetPoint, // For this code path to be reachable the code at 0x4BF47F can't be reachable, thus we use `tGoToPoint->m_vecTargetPoint`
                    e->m_impactPos,
                    e->m_impactNormal,
                    isHeadOnCollision
                };
            } else if (const auto car = tEnterCarPsgrWait->GetCar()) {
                targetEntity = car->m_pDriver;
            } else {
                return nullptr;
            }

            if (!targetEntity) { // 0x4BF6A6
                return nullptr;
            }

            if (CTaskComplexGangFollower::ms_bUseClimbing) { // 0x4BF6B3
                if (CPedGroups::IsInPlayersGroup(m_Ped)) {
                    CVector climbPos;
                    float   climbGrabHeading;
                    uint8   climbSurfaceType;
                    if (const auto entityToClimb = CTaskSimpleClimb::TestForClimb(m_Ped, climbPos, climbGrabHeading, climbSurfaceType, true)) {
                        return new CTaskSimpleClimb{
                            entityToClimb,
                            climbPos,
                            climbGrabHeading,
                            climbSurfaceType,
                            climbPos.z - m_Ped->GetPosition().z < CTaskSimpleClimb::ms_fMinForStretchGrab ? CLIMB_PULLUP : CLIMB_GRAB,
                            true
                        };
                    }
                }
            }

            if (targetEntity->IsPed()) {
                if (!g_ikChainMan.IsLooking(m_Ped)) { // 0x4BF77F
                    g_ikChainMan.LookAt(
                        "CompBldgCollResp",
                        m_Ped,
                        targetEntity,
                        3'000,
                        BONE_HEAD,
                        nullptr,
                        true,
                        0.25f,
                        500,
                        3,
                        false
                    );
                }
                /* nop GetTaskSecondary */
            }

            return new CTaskComplexWalkRoundBuildingAttempt{ // 0x4BF814
                (eMoveState)e->m_moveState,
                targetEntity,
                {},
                e->m_impactPos,
                e->m_impactNormal,
                isHeadOnCollision
            };
        } else {
            const auto DoWalkAroundBuldingTask = [&](CEntity* aroundEntity) {
                if (aroundEntity) {
                    return new CTaskComplexWalkRoundBuildingAttempt{
                        (eMoveState)e->m_moveState,
                        aroundEntity,
                        {},
                        e->m_impactPos,
                        e->m_impactNormal,
                        false
                    }; 
                }
                return new CTaskComplexWalkRoundBuildingAttempt{
                    (eMoveState)e->m_moveState,
                    tGoToPoint->m_vecTargetPoint, // For this code path to be reachable the code at 0x4BF47F can't be reachable, thus we use `tGoToPoint->m_vecTargetPoint`
                    e->m_impactPos,
                    e->m_impactNormal,
                    false
                };
            };
            if (tKillPedOnFoot) { // 0x004BF825
                return DoWalkAroundBuldingTask(tKillPedOnFoot->m_target);
            } else if (tSeekEntity) { // 0x4BF8C3
                return DoWalkAroundBuldingTask(tSeekEntity->GetEntityToSeek());
            }
        }
        return nullptr;
    }();
}

// 0x4BBC30
void CEventHandler::ComputeCarUpsideDownResponse(CEventCarUpsideDown* e, CTask* tactive, CTask* tsimplest) {
    m_EventResponseTask = [&]() -> CTask* {
        if (!m_Ped->IsInVehicle() || m_Ped->m_pVehicle != e->m_vehicle) {
            return nullptr;
        }
        return new CTaskComplexLeaveCar{
            e->m_vehicle,
            0,
            CGeneral::GetRandomNumberInRange(100, 400),
            true,
            false
        };
    }();
}

// 0x4B98E0
void CEventHandler::ComputeChatPartnerResponse(CEventChatPartner* e, CTask* tactive, CTask* tsimplest) {
    m_EventResponseTask = [&]() -> CTask* {
        if (!e->m_partner || m_Ped->GetTaskManager().Has<CTaskComplexPartnerChat>(false)) {
            return nullptr;
        }
        return new CTaskComplexPartnerChat{
            "EventChatPartnerResponse",
            e->m_partner,
            e->m_leadSpeaker,
            0.5f,
            4,
            true,
            false,
            {}
        };
    }();
}

// 0x4BB740
void CEventHandler::ComputeCopCarBeingStolenResponse(CEventCopCarBeingStolen* e, CTask* tactive, CTask* tsimplest) {
    m_EventResponseTask = [&]() -> CTask* {
        if (!e->m_hijacker || !e->m_vehicle || m_Ped->GetVehicleIfInOne() != e->m_vehicle) {
            return nullptr;
        }
        if (e->m_hijacker->IsPlayer()) {
            FindPlayerWanted()->SetWantedLevelNoDrop(1);
        }
        return new CTaskComplexLeaveCar{ e->m_vehicle, 0, 0, true, false };
    }();
}

// 0x4BB130
void CEventHandler::ComputeCreatePartnerTaskResponse(CEventCreatePartnerTask* e, CTask* tactive, CTask* tsimplest) {
    m_EventResponseTask = [&]() -> CTask* {
        if (!e->m_partner) {
            return nullptr;
        }
        switch (e->m_partnerType) {
        case 0:
            return new CTaskComplexPartnerDeal{
                "CompPartnerTaskResp",
                e->m_partner,
                e->m_isLeadSpeaker,
                e->m_meetDist,
                {}
            };
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6: 
            return new CTaskComplexPartnerGreet{
                "CompPartnerTaskResp",
                e->m_partner,
                e->m_isLeadSpeaker,
                e->m_meetDist,
                e->m_partnerType - 1,
                {}
            };
        default:
            return nullptr;
        }
    }();
}

// 0x4C0170
void CEventHandler::ComputeDamageResponse(CEventDamage* e, CTask* tactive, CTask* tsimplest, CTask* abortedTaskEventResponse) {
    UNUSED(abortedTaskEventResponse);

    m_EventResponseTask = [&]() -> CTask* {
        const auto DoComputeDamageAndResponseForPersonality = [&] {
            const auto ce = m_History.GetCurrentEvent();
            if (!ce || ce->GetEventType() != EVENT_DAMAGE || !e->IsSameEventForAI(static_cast<CEventDamage&>(*ce))) {
                if (!m_Ped->GetTaskManager().Has<TASK_COMPLEX_REACT_TO_ATTACK>() && !m_Ped->IsPlayer()) {
                    if (const auto esrc = e->GetSourceEntity()) {
                        if (esrc->IsPed()) {
                            ComputePersonalityResponseToDamage(e, esrc->AsPed());
                        }
                    }
                }
            }
            return m_EventResponseTask;
        };

        if (!e->m_bAddToEventGroup) {
            // NOTE(Pirulax):
            // Originally the code here used `e->m_pSourceEntity`, but that was probably just a mistake,
            // as the other code piece [copy paste of it] used `e->GetSourceEntity()`.
            return DoComputeDamageAndResponseForPersonality();
        }

        e->ProcessDamage(m_Ped);

        switch (e->m_weaponType) {
        case WEAPON_RAMMEDBYCAR:
        case WEAPON_RUNOVERBYCAR:
            g_InterestingEvents.Add(CInterestingEvents::INTERESTING_EVENT_15, m_Ped);
        }

        if (const auto v = m_Ped->GetVehicleIfInOne()) {
            if (v->IsBike() || v->IsSubQuad()) {
                if (!e->m_bFallDown && !e->HasKilledPed()) {
                    assert(e->m_pSourceEntity->IsPed());
                    ComputePersonalityResponseToDamage(e, e->m_pSourceEntity->AsPed());
                } else {
                    ComputeKnockOffBikeResponse(e, tactive, tsimplest); // 0x4C02DE
                }
                return m_EventResponseTask;
            }
        }

        if (e->HasKilledPed() || m_Ped->m_fHealth < 1.f || (m_Ped->bInVehicle && e->m_bFallDown)) { // 0x4C0315
            if (const auto v = m_Ped->GetVehicleIfInOne()) {
                if (v->IsBike() || v->IsSubQuad()) { // 0x4C07E5
                    ComputeKnockOffBikeResponse(e, tactive, tsimplest);
                    return m_EventResponseTask;
                }
            }

            // TODO(Pirulax):
            // Eventually remove these lambdas.
            // I'm pretty sure this code can be linearized, but first I want to make sure it actually works :D
            const auto DoDie = [&](bool bFallingToDeath = false, eDirection fallToDeathDir = eDirection::FORWARD, bool bFallToDeathOverRailing = false) { // 0x4C0AA2
                g_InterestingEvents.Add(CInterestingEvents::INTERESTING_EVENT_28, m_Ped);
                if (const auto tPhyResp = m_Ped->GetTaskManager().GetTaskPrimary(TASK_PRIMARY_PHYSICAL_RESPONSE)) {
                    if (tPhyResp->GetTaskType() != TASK_SIMPLE_CHOKING || !notsa::contains({ WEAPON_SPRAYCAN, WEAPON_EXTINGUISHER, WEAPON_TEARGAS }, e->m_weaponType)) {
                        m_Ped->GetIntelligence()->AddTaskPhysResponse(nullptr);
                    }
                }

                if (e->m_pSourceEntity) {
                    RegisterKill(m_Ped, e->m_pSourceEntity, e->m_weaponType, e->WasHeadShot());
                }

                return new CTaskComplexDie{
                    e->m_weaponType,
                    e->m_nAnimGroup,
                    e->m_nAnimID,
                    e->m_fAnimBlend,
                    e->m_fAnimSpeed,
                    tactive && tactive->GetTaskType() == TASK_SIMPLE_STEALTH_KILL && !static_cast<CTaskSimpleStealthKill*>(tactive)->m_bKeepTargetAlive,
                    bFallingToDeath,
                    fallToDeathDir,
                    bFallToDeathOverRailing
                };
            };

            const auto DoDieMaybeFall = [&]{ // 0x4C0A1C
                if (notsa::contains({ ANIM_ID_NO_ANIMATION_SET, ANIM_ID_DOOR_LHINGE_O }, e->m_nAnimID)) {
                    e->ComputeDeathAnim(m_Ped, false);
                }
                if (!m_Ped->IsPlayer() && e->m_pSourceEntity) {
                    if (notsa::contains({
                        WEAPON_PISTOL,
                        WEAPON_PISTOL_SILENCED,
                        WEAPON_DESERT_EAGLE,
                        WEAPON_SHOTGUN,
                        WEAPON_SAWNOFF_SHOTGUN,
                        WEAPON_SPAS12_SHOTGUN,
                        WEAPON_MICRO_UZI,
                        WEAPON_MP5,
                        WEAPON_AK47,
                        WEAPON_M4,
                        WEAPON_TEC9,
                        WEAPON_COUNTRYRIFLE,
                        WEAPON_MINIGUN
                    }, e->m_weaponType)) {
                        if (e->m_pSourceEntity->IsPed() && e->m_pSourceEntity->AsPed()->IsCreatedByMission()) {
                            int32 fallToDeathDir;
                            bool  bFallToDeathOverRailing;
                            bool  bFallToDeath = CTaskComplexFallToDeath::CalcFall(m_Ped, fallToDeathDir, bFallToDeathOverRailing);
                            return DoDie(bFallToDeath, (eDirection)(fallToDeathDir), bFallToDeathOverRailing);
                        }
                    }
                }
                return DoDie();
            };

            if (!notsa::contains({ ANIM_ID_NO_ANIMATION_SET, ANIM_ID_DOOR_LHINGE_O }, e->m_nAnimID)) {
                return DoDie();
            }

            if (!tsimplest) {
                return DoDieMaybeFall();
            }

            switch (tsimplest->GetTaskType()) {
            case TASK_SIMPLE_FALL: {
                if (const auto dr = &e->m_damageResponse; e->m_weaponType == WEAPON_FALL && dr->m_fDamageArmor + dr->m_fDamageHealth >= 20.f) { // 0x4C083E - Add blood
                    for (auto i = 10; i-- > 0;) {
                        g_fx.AddBlood(
                            m_Ped->GetPosition() + CVector::Random({-0.5f, -0.5f, 0.f}, {0.5f, 0.5f, 0.f}),
                            {0.f, 0.f, 2.f},
                            10,
                            m_Ped->m_fContactSurfaceBrightness
                        );
                    }
                }
                const auto tfall = CTask::Cast<CTaskSimpleFall>(tsimplest);
                if (const auto a = tfall->m_pAnim) { // 0x4C08ED
                    if (a->m_BlendAmount > 0.5f && a->m_BlendDelta >= 0.f && a->m_CurrentTime < a->m_BlendHier->m_fTotalTime) {
                        e->m_nAnimGroup = a->m_AnimGroupId;
                        e->m_nAnimID    = a->m_AnimId;
                        e->m_fAnimBlend = 4.f;
                        e->m_fAnimSpeed = 1.f;
                        return DoDieMaybeFall();
                    }
                }
                if (!tfall->m_bIsFinished) {
                    return DoDieMaybeFall();
                }
                e->m_fAnimBlend = 4.f;
                e->m_fAnimSpeed = 1.f;
                e->m_nAnimGroup = ANIM_GROUP_DEFAULT;
                if (const auto a = RpAnimBlendClumpGetFirstAssociation(m_Ped->m_pRwClump, ANIMATION_800)) { // 0x4C094E
                    e->m_nAnimID = ANIM_ID_FLOOR_HIT_F;
                    return DoDieMaybeFall();
                }
                break;
            }
            case TASK_SIMPLE_GET_UP: { // 0x4C0987
                const auto tgup = CTask::Cast<CTaskSimpleGetUp>(tsimplest);
                if (!tgup->m_Anim || tgup->m_Anim->GetTimeProgress() >= 0.5f) {
                    if (tgup->m_bHasPedGotUp) {
                        return DoDieMaybeFall();
                    }
                    e->m_nAnimID = RpAnimBlendClumpGetFirstAssociation(m_Ped->m_pRwClump, ANIMATION_800)
                        ? ANIM_ID_FLOOR_HIT_F
                        : ANIM_ID_FLOOR_HIT;
                    
                } else if (!tgup->m_bHasPedGotUp) {
                    e->m_nAnimID = tgup->m_Anim->m_AnimId == ANIM_ID_GETUP_FRONT
                        ? ANIM_ID_FLOOR_HIT_F
                        : ANIM_ID_FLOOR_HIT;
                }
                e->m_nAnimGroup = ANIM_GROUP_DEFAULT;
                e->m_fAnimBlend = 4.f;
                e->m_fAnimSpeed = 1.f;
                return DoDieMaybeFall();
            }
            default:
                return DoDieMaybeFall();
            }
        }

        if (!e->m_pSourceEntity) {
            return nullptr;
        }

        if (CWeaponInfo::TypeIsWeapon(e->m_weaponType)) {
            if (m_Ped->bInVehicle) { // 0x4C0332
                return DoComputeDamageAndResponseForPersonality();
            }

            const auto eventSrcPed = e->m_pSourceEntity->IsPed()
                ? e->m_pSourceEntity->AsPed()
                : nullptr;

            if (eventSrcPed) { // 0x4C0338
                if (const auto killerStealthTask = eventSrcPed->GetTaskManager().GetActiveTaskAs<CTaskSimpleStealthKill>()) {
                    m_PhysicalResponseTask = new CTaskSimpleStealthKill{false, eventSrcPed, killerStealthTask->m_animGrpId};
                    m_Ped->SetPedState(PEDSTATE_DIE_BY_STEALTH);
                    return nullptr;
                }
            }

            if (e->m_nAnimID == ANIM_ID_NO_ANIMATION_SET) {
                e->ComputeAnim(m_Ped, false);
            }

            // Do choking if it applies to this weapon type
            if (notsa::contains({ WEAPON_SPRAYCAN, WEAPON_EXTINGUISHER, WEAPON_TEARGAS }, e->m_weaponType)) { // 0x4C03F8 - TODO: `WeaponIsChocking`
                if (!notsa::IsFixBugs() || eventSrcPed) { // Original code just crashed at this point
                    const auto t = eventSrcPed->GetIntelligence()->GetTaskUseGun();
                    if (!t || t->GetLastGunCommand() != eGunCommand::PISTOLWHIP) {
                        const auto bIsTearGas = e->m_weaponType == WEAPON_TEARGAS;
                        if (const auto t = CTask::DynCast<CTaskSimpleChoking>(m_Ped->GetTaskManager().GetTaskPrimary(TASK_PRIMARY_PHYSICAL_RESPONSE))) {
                            t->UpdateChoke(m_Ped, eventSrcPed, bIsTearGas);
                        } else {
                            m_PhysicalResponseTask = new CTaskSimpleChoking{eventSrcPed, bIsTearGas};
                        }
                        return DoComputeDamageAndResponseForPersonality();
                    }
                }
            }

            // Otherwise
            if (!e->m_bFallDown && !notsa::contains({ ANIM_ID_NO_ANIMATION_SET, ANIM_ID_DOOR_LHINGE_O }, e->m_nAnimID)) {
                if (CAnimManager::GetAnimAssociation(e->m_nAnimGroup, e->m_nAnimID)->m_Flags & ANIMATION_DONT_ADD_TO_PARTIAL_BLEND) { // 0x4C04B7
                    if (!e->GetAnimAdded()) {
                        if (!notsa::contains({ANIM_ID_SHOT_PARTIAL, ANIM_ID_SHOT_LEFTP, ANIM_ID_SHOT_PARTIAL_B, ANIM_ID_SHOT_RIGHTP}, e->m_nAnimID)) {
                            const auto a = CAnimManager::BlendAnimation(m_Ped->m_pRwClump, e->m_nAnimGroup, e->m_nAnimID, e->m_fAnimBlend);
                            a->SetSpeed(e->m_fAnimSpeed);
                            a->SetCurrentTime(0.f);
                            e->m_bAnimAdded = true;
                        } else {
                            auto a = RpAnimBlendClumpGetAssociation(m_Ped->m_pRwClump, e->GetAnimId()); // 0x4C04DE
                            if (!a) {
                                a = CAnimManager::BlendAnimation(m_Ped->m_pRwClump, e->GetAnimGroup(), e->GetAnimId());
                            }
                            a->SetBlend(0.f, e->m_fAnimBlend);
                            a->SetSpeed(e->m_fAnimSpeed);
                            a->SetCurrentTime(0.f);
                        }
                    }
                } else { // 0x4C0593 
                    const auto tBeHit = new CTaskSimpleBeHit{
                        eventSrcPed,
                        e->m_pedPieceType,
                        e->m_ucDirection,
                        (int32)e->GetDamageResponse().GetTotalDamage()
                    };
                    m_PhysicalResponseTask = tBeHit;

                    tBeHit->m_nAnimId    = e->GetAnimId();
                    tBeHit->m_nAnimGroup = e->GetAnimGroup();
                    tBeHit->m_bAnimAdded = e->GetAnimAdded();
                }
            } else {
                m_PhysicalResponseTask = new CTaskComplexFallAndGetUp{
                    e->GetAnimId(),
                    e->GetAnimGroup(),
                    (int32)(1000.f / ((float)m_Ped->m_nWeaponShootingRate / 40.f))
                };
            }

            return DoComputeDamageAndResponseForPersonality();
        }

        if (!m_Ped->bInVehicle) { // 0x4C06D9
            if (!notsa::contains({ ANIM_ID_NO_ANIMATION_SET, ANIM_ID_DOOR_LHINGE_O }, e->m_nAnimID)) {
                if (notsa::contains({ WEAPON_RAMMEDBYCAR, WEAPON_RUNOVERBYCAR, WEAPON_FALL }, e->m_weaponType)) {
                    m_PhysicalResponseTask = new CTaskComplexFallAndGetUp{ // 0x4C0748
                        e->GetAnimId(),
                        e->GetAnimGroup(),
                        m_Ped->IsPlayer() ? 500 : 1000
                    };
                    const auto knockedOverByVeh = e->m_pSourceEntity->IsVehicle()
                        ? e->m_pSourceEntity->AsVehicle()
                        : e->m_pSourceEntity->IsPed()
                            ? e->m_pSourceEntity->AsPed()->GetVehicleIfInOne()
                            : nullptr;
                    if (knockedOverByVeh) {
                        m_Ped->GetEventGroup().Add(CEventGotKnockedOverByCar{knockedOverByVeh});
                    }
                }
            }
        }

        return nullptr;
    }();
}

// 0x4BC230
void CEventHandler::ComputeDangerResponse(CEventDanger* e, CTask* tactive, CTask* tsimplest) {
    m_EventResponseTask = [&]() -> CTask* {
        if (!e->m_dangerFrom) {
            return nullptr;
        }
        switch (e->m_taskId) {
        case TASK_COMPLEX_CAR_DRIVE_MISSION_FLEE_SCENE: {
            if (m_Ped->bInVehicle) {
                if (!m_Ped->m_pVehicle->IsDriver(m_Ped)) {
                    return nullptr;
                }
                return new CTaskComplexCarDriveMissionFleeScene{m_Ped->m_pVehicle};
            }
            [[fallthrough]];
        }
        case TASK_COMPLEX_SMART_FLEE_ENTITY: {
            return new CTaskComplexSmartFleeEntity{ e->m_dangerFrom, true, 100'000.f };
        }
        }
        return nullptr;
    }();
}

// 0x4B9470
void CEventHandler::ComputeDeadPedResponse(CEventDeadPed* e, CTask* tactive, CTask* tsimplest) {
    m_EventResponseTask = [&]() -> CTask* {
        if (!e->GetDeadPed()) {
            return nullptr;
        }
        if (m_Ped->IsPlayer()) {
            if ((m_Ped->GetPosition() - e->GetDeadPed()->GetPosition()).SquaredMagnitude2D() >= sq(0.75f)) {
                return nullptr;
            }
            if (m_Ped->GetIntelligence()->GetTaskFighting()) {
                return nullptr;
            }
            if (g_ikChainMan.IsLooking(m_Ped)) {
                return nullptr;
            }
            const auto c = &TheCamera.GetActiveCam();
            switch (c->m_nMode) {
            case MODE_SNIPER:
            case MODE_CAMERA:
            case MODE_ROCKETLAUNCHER:
            case MODE_ROCKETLAUNCHER_HS:
            case MODE_M16_1STPERSON:
            case MODE_1STPERSON:
            case MODE_HELICANNON_1STPERSON:
                return nullptr;
            }
            if (c->GetWeaponFirstPersonOn()) {
                return nullptr;
            }
            g_ikChainMan.LookAt(
                "CompDeadPedResp",
                m_Ped,
                e->GetDeadPed(),
                500,
                BONE_HEAD,
                nullptr,
                true,
                0.25f,
                500,
                3,
                false
            );
            return nullptr;
        }
        switch (e->m_taskId) {
        case TASK_COMPLEX_SMART_FLEE_ENTITY:
            return new CTaskComplexSmartFleeEntity{e->GetDeadPed(), true, 100'000.f};
        case TASK_COMPLEX_INVESTIGATE_DEAD_PED:
            return new CTaskComplexInvestigateDeadPed{e->GetDeadPed()};
        case TASK_SIMPLE_DUCK_FOREVER:
            return new CTaskSimpleDuck{ DUCK_STANDALONE, 57599u, -1 };
        default:
            return nullptr;
        }
    }();
}

// 0x4B9400
void CEventHandler::ComputeDeathResponse(CEventDeath* e, CTask* tactive, CTask* tsimplest) {
    m_EventResponseTask = new CTaskSimpleDead{e->GetDeathTime(), e->HasDrowned()};
}

// 0x4BC1D0
void CEventHandler::ComputeDontJoinGroupResponse(CEventDontJoinPlayerGroup* e, CTask* tactive, CTask* tsimplest) {
    m_EventResponseTask = new CTaskComplexGangJoinRespond{false};
}

// 0x4BCC30
void CEventHandler::ComputeDraggedOutCarResponse(CEventDraggedOutCar* e, CTask* tactive, CTask* tsimplest) {
    // Pirulax: You gotta love when 500+ sloc is compressed to 100 :D
    m_EventResponseTask = [&]() -> CTask* {
        if (m_Ped->IsPlayer() || !e->m_Vehicle) {
            return nullptr;
        }

        if (e->m_Vehicle->GetAnimGroupId() == ANIM_GROUP_COACHCARANIMS) { // 0x4BCC7E
            return new CTaskComplexSmartFleeEntity{e->m_CarJacker, false, fSafeDistance};
        }

        const auto CreateFinalSeq = [&](CTask* response) -> CTask* {
            const auto leaveCarTask = e->m_Vehicle->IsDriver(m_Ped) || e->m_Vehicle->m_apPassengers[0] == m_Ped // 0x4BCCF2
                ? new CTaskComplexLeaveCar{ e->m_Vehicle, 0, 0, false, true }
                : nullptr;

            if (leaveCarTask && response) { // 0x4BD63A
                return new CTaskComplexSequence{ leaveCarTask, response };
            }

            return leaveCarTask
                ? leaveCarTask // NOTE: Slightly changed the code. Originally it boiled down to `new CTaskComplexSequence{ leaveCarTask, nullptr }`. This way we save an allocation.
                : response;    // `response` might be null, that's fine
        };

        const auto CreateStealCarBackSeq = [&](bool dragOutPedIfPassenger, bool killJacker) {
            const auto s = new CTaskComplexSequence{};

            // Make important life decisions
            if (CGeneral::RandomBool(50.f)) {
                s->AddTask(new CTaskSimplePause{CGeneral::GetRandomNumberInRange(500, 1'500)});
            } else {
                s->AddTasks(
                    new CTaskSimpleAchieveHeading{m_Ped, e->m_CarJacker},
                    new CTaskSimpleShakeFist{}
                );
            }

            // Maybe kill the fucker
            if (killJacker) {
                s->AddTask(new CTaskComplexKillPedOnFoot{e->m_CarJacker});
            }

            // Talk a little
            m_Ped->Say(CTX_GLOBAL_JACKED_ON_STREET);

            // Try stealing the car back
            if (e->m_IsDriverSeat) {
                s->AddTask(new CTaskComplexStealCar{e->m_Vehicle});
            } else {
                if (dragOutPedIfPassenger) { // TODO/BUG: I really wonder if this is just a bug [that is, that they didn't include this in both [0x4BCF68 and 0x4BD3FA] places]
                    s->AddTask(new CTaskComplexDragPedFromCar{e->m_CarJacker});
                } 
                s->AddTask(new CTaskComplexEnterCarAsPassenger{e->m_Vehicle});
            }

            return CreateFinalSeq(s);
        };
        switch (e->m_taskId) {
        case TASK_NONE: // 0x4BCD68
            return CreateFinalSeq(nullptr); // Nothing to do
        case TASK_SIMPLE_HIT_HEAD: { // 0x4BCD74
            if (e->m_IsDriverSeat) {
                return CreateFinalSeq(new CTaskComplexEnterCarAsDriver{ e->m_Vehicle });
            }
            return CreateFinalSeq(new CTaskComplexEnterCarAsPassenger{ e->m_Vehicle });
        }
        case TASK_COMPLEX_EVASIVE_STEP: { // 0x4BCD85
            return CreateStealCarBackSeq(false, true);
        }
        case TASK_COMPLEX_KILL_CRIMINAL: { // 0x4BD111
            if (!e->m_CarJacker || e->m_CarJacker->IsPlayer()) {
                return CreateFinalSeq(nullptr);
            }
            return CreateFinalSeq(new CTaskComplexKillCriminal{e->m_CarJacker});
        }
        case TASK_COMPLEX_DESTROY_CAR: { // 0x4BD173
            return CreateFinalSeq(new CTaskComplexDestroyCar{e->m_Vehicle});
        }
        case TASK_COMPLEX_KILL_PED_ON_FOOT: { // 0x4BD1B4
            assert(e->m_CarJacker); // There's some code after fleeing, but it makes no sense since `e->m_CarJacker` would be null. Pretty sure they forgot a `break`.
            if (IsKillTaskAppropriate(m_Ped, e->m_CarJacker, *e)) {
                return CreateStealCarBackSeq(true, false);
            }
            return CreateFinalSeq(new CTaskComplexSmartFleeEntity{e->m_CarJacker, true, 100'000.f});
        }
        default:
            return nullptr;
        }
    }();
}

// 0x4BBFB0
void CEventHandler::ComputeFireNearbyResponse(CEventFireNearby* e, CTask* tactive, CTask* tsimplest) {
    m_EventResponseTask = [&]() -> CTask* {
        switch (e->m_taskId) {
        case TASK_COMPLEX_EXTINGUISH_FIRES:
            return new CTaskComplexExtinguishFires{};
        }
        return nullptr;
    }();
}

// 0x4C3430
void CEventHandler::ComputeGotKnockedOverByCarResponse(CEventGotKnockedOverByCar* e, CTask* tactive, CTask* tsimplest) {
    m_EventResponseTask = [&]() -> CTask* {
        if (!e->m_vehicle) {
            return nullptr;
        }
        const auto drvr = e->m_vehicle->m_pDriver;
        if (!drvr) {
            return nullptr;
        }
        switch (e->m_taskId) {
        case TASK_COMPLEX_KILL_PED_ON_FOOT: { // 0x4C346E
            if (IsKillTaskAppropriate(m_Ped, drvr, *e)) {
                return new CTaskComplexKillPedOnFoot{drvr};
            }
            return new CTaskComplexSmartFleeEntity{drvr, true, 100'000.f};
        }
        case TASK_COMPLEX_FLEE_ENTITY: { // 0x4C347B
            // There was an incorrect (it was inverted) null check at `0x4C3496`
            // The opposite case (when `drvr == nullptr` was a no-op anyways because `CTaskComplexSmartFleeEntity` does nothing when `fleeEntity == nullptr`)
            return notsa::IsFixBugs()
                ? new CTaskComplexSmartFleeEntity{drvr, true, 100'000.f}
                : nullptr; 
        }
        case TASK_COMPLEX_KILL_CRIMINAL: {
            return new CTaskComplexKillCriminal{drvr};
        }
        }
        return nullptr;
    }();
}

// 0x4C2840
void CEventHandler::ComputeGunAimedAtResponse(CEventGunAimedAt* e, CTask* tactive, CTask* tsimplest) {
    m_EventResponseTask = [&]() -> CTask* {
        if (m_Ped->IsPlayer()) {
            return nullptr;
        }

        const auto IsFleeTask = [](CTask* t) {
            return notsa::contains({TASK_COMPLEX_SMART_FLEE_ENTITY, TASK_COMPLEX_SMART_FLEE_POINT}, t->GetTaskType());
        };
        if (IsFleeTask(tactive) || IsFleeTask(tsimplest)) {
            return new CTaskComplexSmartFleeEntity{e->m_AimedBy, false, fSafeDistance}; // 0x4C28F5
        }

        if (m_Ped->IsCreatedBy(PED_GAME) && !m_Ped->bInVehicle && !m_Ped->GetActiveWeapon().IsTypeMelee()) {
            if (e->m_AimedBy && e->m_AimedBy->IsPlayer() && !m_Ped->GetIntelligence()->IsFriendlyWith(*e->m_AimedBy)) {
                e->m_taskId = TASK_COMPLEX_KILL_PED_ON_FOOT; // 0x4C296F
            }
        }

        switch (e->m_taskId) {
        case TASK_COMPLEX_USE_CLOSEST_FREE_SCRIPTED_ATTRACTOR: // 0x4C2A7A
            return new CTaskComplexUseClosestFreeScriptedAttractor{};
        case TASK_COMPLEX_USE_CLOSEST_FREE_SCRIPTED_ATTRACTOR_RUN:
            return new CTaskComplexUseClosestFreeScriptedAttractorRun{};
        case TASK_COMPLEX_USE_CLOSEST_FREE_SCRIPTED_ATTRACTOR_SPRINT:
            return new CTaskComplexUseClosestFreeScriptedAttractorSprint{};
        case TASK_SIMPLE_COWER:
            return new CTaskSimpleCower{};
        case TASK_SIMPLE_HANDS_UP:
            return new CTaskSimpleHandsUp{5'000};
        case TASK_SIMPLE_DUCK: {
            if (const auto t = m_Ped->GetIntelligence()->GetTaskDuck()) {
                return t;
            }
            return new CTaskSimpleDuck{DUCK_STANDALONE, 5'000};
        }
        case TASK_SIMPLE_DUCK_FOREVER:
            return new CTaskSimpleDuck{DUCK_STANDALONE, 0x967Fu}; // are they trying to tell us something? "Fu" :D
        case TASK_COMPLEX_REACT_TO_GUN_AIMED_AT: // 0x4C2B2C
            return new CTaskComplexReactToGunAimedAt{e->m_AimedBy}; 
        case TASK_COMPLEX_KILL_PED_ON_FOOT_STEALTH: // 0x4C2E30
            return new CTaskComplexKillPedOnFootStealth{e->m_AimedBy};
        case TASK_COMPLEX_KILL_CRIMINAL:
            return new CTaskComplexKillCriminal{e->m_AimedBy};
        case TASK_COMPLEX_KILL_PED_ON_FOOT: { // 0x4C2E3D
            if (IsKillTaskAppropriate(m_Ped, e->m_AimedBy, *e)) {
                return new CTaskComplexKillPedOnFoot{e->m_AimedBy};
            }
            return new CTaskComplexFleeEntity{e->m_AimedBy, false};
        }
        case TASK_COMPLEX_FLEE_ANY_MEANS: // 0x4C2B36
            return new CTaskComplexFleeAnyMeans{e->m_AimedBy, true, fSafeDistance};
        case TASK_COMPLEX_CAR_DRIVE_MISSION_FLEE_SCENE: {// 0x4C2BF
            if (const auto v = m_Ped->m_pVehicle) {
                if (v->IsDriver(m_Ped)) {
                    return new CTaskComplexCarDriveMissionFleeScene{v}; // 0x4C2C39
                }
            }
            if (!m_Ped->bWantedByPolice || !e->m_AimedBy->IsCop()) {
                return new CTaskComplexFleeEntity{e->m_AimedBy, false};
            }
            return new CTaskComplexFleeAnyMeans{e->m_AimedBy, true, fSafeDistance};
        }
        case TASK_COMPLEX_FLEE_ENTITY: // 0x4C2BEE
            return new CTaskComplexFleeEntity{e->m_AimedBy, false};
        case TASK_COMPLEX_SMART_FLEE_ENTITY: { // 0x4C2B5C
            if (!m_Ped->bWantedByPolice || !e->m_AimedBy->IsCop()) {
                return new CTaskComplexFleeEntity{e->m_AimedBy, false};
            }
            return new CTaskComplexFleeAnyMeans{e->m_AimedBy, true, fSafeDistance};
        }
        }
        NOTSA_UNREACHABLE();
    }();
}

// 0x4BAC10
void CEventHandler::ComputeHighAngerAtPlayerResponse(CEventHighAngerAtPlayer* e, CTask* tactive, CTask* tsimplest) {
    m_EventResponseTask = [&]() -> CTask* {
        switch (e->m_taskId) {
        case TASK_COMPLEX_KILL_PED_ON_FOOT:
            return new CTaskComplexKillPedOnFoot{FindPlayerPed()};
        case TASK_COMPLEX_SMART_FLEE_ENTITY:
            return new CTaskComplexSmartFleeEntity{FindPlayerPed(), false, 60.f};
        case TASK_NONE:
            return nullptr;
        case TASK_SIMPLE_DUCK_FOREVER:
            return new CTaskSimpleDuck{DUCK_STANDALONE, 0xE0FFu, -1};
        default:
            NOTSA_UNREACHABLE();
        }
    }();
}

// 0x4BAF80
void CEventHandler::ComputeInWaterResponse(CEventInWater* e, CTask* tactive, CTask* tsimplest) {
    m_EventResponseTask = new CTaskComplexInWater{};
}

// 0x4BAFE0
void CEventHandler::ComputeInteriorUseInfoResponse(CEventInteriorUseInfo* e, CTaskInteriorUseInfo* tUseInfo, CTask* tsimplest) {
    m_EventResponseTask = tUseInfo->Clone();
}

// 0x4B9FF0
void CEventHandler::ComputeKnockOffBikeResponse(CEvent* e, CTask* tactive, CTask* tsimplest) {
    plugin::CallMethod<0x4B9FF0, CEventHandler*, CEvent*, CTask*, CTask*>(this, e, tactive, tsimplest);
}

// 0x4BAAD0
void CEventHandler::ComputeLowAngerAtPlayerResponse(CEventLowAngerAtPlayer* e, CTask* tactive, CTask* tsimplest) {
    m_EventResponseTask = [&]() -> CTask* { // Same code as `ComputeHighAngerAtPlayerResponse`
        switch (e->m_taskId) {
        case TASK_COMPLEX_KILL_PED_ON_FOOT:
            return new CTaskComplexKillPedOnFoot{FindPlayerPed()};
        case TASK_COMPLEX_SMART_FLEE_ENTITY:
            return new CTaskComplexSmartFleeEntity{FindPlayerPed(), false, 60.f};
        case TASK_NONE:
            return nullptr;
        case TASK_SIMPLE_DUCK_FOREVER:
            return new CTaskSimpleDuck{DUCK_STANDALONE, 0xE0FFu, -1};
        default:
            NOTSA_UNREACHABLE();
        }
    }();
}

// 0x4BA990
void CEventHandler::ComputeLowHealthResponse(CEventHealthLow* e, CTask* tactive, CTask* tsimplest) {
    m_EventResponseTask = [&]() -> CTask* {
        switch (e->m_taskId) {
        case TASK_COMPLEX_DIVE_FROM_ATTACHED_ENTITY_AND_GET_UP:
            return new CTaskComplexDiveFromAttachedEntityAndGetUp{};
        case TASK_NONE:
            return nullptr;
        default:
            NOTSA_UNREACHABLE();
        }
    }();
}

// 0x4BBB90
void CEventHandler::ComputeObjectCollisionPassiveResponse(CEventPotentialWalkIntoObject* e, CTask* tactive, CTask* tsimplest) {
    m_EventResponseTask = [&]() -> CTask* {
        if (!e->m_object || m_Ped->bInVehicle || e->m_moveState == PEDMOVE_STILL) {
            return nullptr;
        }
        if (!tsimplest || !CTask::IsGoToTask(tsimplest)) {
            return nullptr;
        }
        const auto tGoTo = static_cast<CTaskSimpleGoTo*>(tsimplest);
        return new CTaskComplexWalkRoundObject{
            e->m_moveState,
            tGoTo->m_vecTargetPoint,
            e->m_object
        };
    }();
}

// 0x4B92B0
void CEventHandler::ComputeObjectCollisionResponse(CEventObjectCollision* e, CTask* tactive, CTask* tsimplest) { // Same code as `ComputeObjectCollisionPassiveResponse`
    m_EventResponseTask = [&]() -> CTask* {
        if (!e->m_object || m_Ped->bInVehicle || e->m_moveState == PEDMOVE_STILL) {
            return nullptr;
        }
        if (!tsimplest || !CTask::IsGoToTask(tsimplest)) {
            return nullptr;
        }
        const auto tGoTo = static_cast<CTaskSimpleGoTo*>(tsimplest);
        return new CTaskComplexWalkRoundObject{
            e->m_moveState,
            tGoTo->m_vecTargetPoint,
            e->m_object
        };
    }();
}

// 0x4BC150
void CEventHandler::ComputeOnEscalatorResponse(CEvent* e, CTask* tactive, CTask* tsimplest) {
    m_EventResponseTask = new CTaskSimpleStandStill{0, true};
}

// 0x4BAD50
void CEventHandler::ComputeOnFireResponse(CEventOnFire* e, CTask* tactive, CTask* tsimplest) {
    if (m_Ped->IsPlayer()) {
        if (!m_Ped->GetTaskManager().GetTaskSecondary(TASK_SECONDARY_PARTIAL_ANIM)) {
            m_PartialAnimTask = new CTaskSimplePlayerOnFire{};
        }
    } else {
        m_EventResponseTask = new CTaskComplexOnFire{};
    }
}

// 0x4BB0C0
void CEventHandler::ComputePassObjectResponse(CEventPassObject* e, CTask* tactive, CTask* tsimplest) {
    m_EventResponseTask = new CTaskComplexPassObject{e->m_giver, e->m_dontPassObject};
}

// 0x4BDB80
void CEventHandler::ComputePedCollisionWithPedResponse(CEvent* e, CTask* tactive, CTask* tsimplest) {
    plugin::CallMethod<0x4BDB80, CEventHandler*, CEvent*, CTask*, CTask*>(this, e, tactive, tsimplest);
}

// 0x4BE7D0
void CEventHandler::ComputePedCollisionWithPlayerResponse(CEvent* e, CTask* tactive, CTask* tsimplest) {
    plugin::CallMethod<0x4BE7D0, CEventHandler*, CEvent*, CTask*, CTask*>(this, e, tactive, tsimplest);
}

// 0x4C1590
void CEventHandler::ComputePedEnteredVehicleResponse(CEventPedEnteredMyVehicle* e, CTask* tactive, CTask* tsimplest) {
    m_EventResponseTask = [&]() -> CTask* {
        if (!e->m_Vehicle || !e->m_PedThatEntered || !m_Ped->bInVehicle) {
            return nullptr;
        }
        /* m_ped->m_pVehicle->IsDriver(m_ped) */
        if (e->m_Vehicle->m_pHandlingData->GetAnimGroupId() == ANIM_GROUP_COACHCARANIMS) {
            return new CTaskComplexSmartFleeEntity{ e->m_PedThatEntered, false, 60.f };
        }
        g_InterestingEvents.Add(CInterestingEvents::INTERESTING_EVENT_20, e->m_Vehicle);
        const auto LeaveCarAndFlee = [&]{
            return new CTaskComplexLeaveCarAndFlee{
                e->m_Vehicle,
                e->m_Vehicle->GetPosition(),
                e->m_TargetDoor,
                CGeneral::GetRandomNumberInRange(300, 600)
            };
        };
        switch (e->m_taskId) {
        case TASK_COMPLEX_KILL_CRIMINAL: { // 0x4C17ED
            if (e->m_PedThatEntered->IsPlayer()) {
                return nullptr;
            }
            return new CTaskComplexKillCriminal{ e->m_PedThatEntered };
        }
        case TASK_COMPLEX_DESTROY_CAR: // 0x4C1849
            return new CTaskComplexDestroyCar{ e->m_Vehicle };
        case TASK_COMPLEX_KILL_PED_ON_FOOT: { // 0x4C186A
            if (!IsKillTaskAppropriate(m_Ped, e->m_PedThatEntered, *e)) {
                return LeaveCarAndFlee();
            }
            return new CTaskComplexKillPedOnFoot{ e->m_PedThatEntered };
        }
        case TASK_COMPLEX_SCREAM_IN_CAR_THEN_LEAVE: { // 0x4C1759
            if (e->m_PedThatEntered->IsPlayer() && CTheScripts::IsPlayerOnAMission()) {
                return LeaveCarAndFlee();
            }
            return new CTaskComplexScreamInCarThenLeave{ e->m_Vehicle, e->m_TargetDoor };
        }
        case TASK_COMPLEX_LEAVE_CAR: // 0x4C16CE
            return new CTaskComplexLeaveCar{ e->m_Vehicle, e->m_TargetDoor, CGeneral::GetRandomNumberInRange(300, 600), false, true };
        case TASK_COMPLEX_LEAVE_CAR_AND_FLEE: // 0x4C18C9
            return LeaveCarAndFlee();
        case TASK_COMPLEX_LEAVE_CAR_AND_WANDER: // 0x4C1744
            return new CTaskComplexLeaveCarAndWander{ e->m_Vehicle, e->m_TargetDoor, CGeneral::GetRandomNumberInRange(300, 600), false };
        case TASK_NONE: // 0x4C16F1
            return nullptr;
        default:
            NOTSA_UNREACHABLE(); // Not sure
        }
    }();
}

// 0x4B9DD0
void CEventHandler::ComputePedFriendResponse(CEventAcquaintancePed* e, CTask* tactive, CTask* tsimplest) {
    m_EventResponseTask = [&]() -> CTask* {
        switch (e->m_taskId) {
        case TASK_COMPLEX_USE_CLOSEST_FREE_SCRIPTED_ATTRACTOR:
            return new CTaskComplexUseClosestFreeScriptedAttractor{};
        case TASK_COMPLEX_USE_CLOSEST_FREE_SCRIPTED_ATTRACTOR_RUN:
            return new CTaskComplexUseClosestFreeScriptedAttractorRun{};
        case TASK_COMPLEX_USE_CLOSEST_FREE_SCRIPTED_ATTRACTOR_SPRINT:
            return new CTaskComplexUseClosestFreeScriptedAttractorSprint{};
        case TASK_COMPLEX_PROSTITUTE_SOLICIT: {
            if (m_Ped->GetTaskManager().Find<CTaskComplexProstituteSolicit>(false)) {
                return nullptr;
            }
            if (m_Ped->bInVehicle) {
                return nullptr;
            }
            return new CTaskComplexProstituteSolicit{e->m_AcquaintancePed};
        }
        case TASK_NONE:
            return nullptr;
        default:
            NOTSA_UNREACHABLE();
        }
    }();
}

// 0x4B9D40
void CEventHandler::ComputePedSoundQuietResponse(CEventSoundQuiet* e, CTask* tactive, CTask* tsimplest) {
    m_EventResponseTask = [&]() -> CTask* {
        if (!e->GetSourceEntity()) {
            return nullptr;
        }
        switch (e->m_taskId) {
        case TASK_NONE:
            return nullptr;
        case TASK_COMPLEX_INVESTIGATE_DISTURBANCE:
            return new CTaskComplexInvestigateDisturbance{e->m_position, e->GetSourceEntity()};
        default:
            NOTSA_UNREACHABLE();
        }
    }();
}

// 0x4B9C90
void CEventHandler::ComputePedThreatBadlyLitResponse(CEventAcquaintancePedHateBadlyLit* e, CTask* tactive, CTask* tsimplest) {
    m_EventResponseTask = [&]() -> CTask* {
        if (!e->m_AcquaintancePed) {
            return nullptr;
        }
        switch (e->m_taskId) {
        case TASK_NONE:
            return nullptr;
        case TASK_COMPLEX_INVESTIGATE_DISTURBANCE:
            return new CTaskComplexInvestigateDisturbance{e->m_point, e->m_AcquaintancePed};
        default:
            NOTSA_UNREACHABLE();
        }
    }();
}

// 0x4C19A0
void CEventHandler::ComputePedThreatResponse(CEventAcquaintancePedHate* e, CTask* tactive, CTask* tsimplest) {
    m_EventResponseTask = [&]() -> CTask* {
        if (!e->m_AcquaintancePed) {
            return nullptr;
        }
        switch (e->m_taskId) {
        case TASK_COMPLEX_SEEK_ENTITY_AIMING: // 0x4C215A
            return new CTaskComplexSeekEntityAiming{e->m_AcquaintancePed, 10.f, 5.f};
        case TASK_SMART_FLEE_ENTITY_WALKING:
            return new CTaskComplexSmartFleeEntity{e->m_AcquaintancePed, false, 60.f};
        case TASK_COMPLEX_KILL_PED_ON_FOOT_TIMED: // 0x4C1E7A [yeah, this is from down below]
        case TASK_COMPLEX_KILL_PED_ON_FOOT: { // 0x4C1D71
            if (IsKillTaskAppropriate(m_Ped, e->m_AcquaintancePed, *e)) {
                return new CTaskComplexKillPedOnFoot{
                    e->m_AcquaintancePed,
                    e->m_taskId == TASK_COMPLEX_KILL_PED_ON_FOOT ? -1 : 10'000
                };
            }
            if (m_Ped->bWantedByPolice && e->m_AcquaintancePed->IsCop()) {
                return new CTaskComplexFleeAnyMeans{e->m_AcquaintancePed, true, 60.f};
            }
            if (!m_Ped->IsInVehicle() || !m_Ped->m_pVehicle->IsDriver(m_Ped)) {
                return new CTaskComplexSmartFleeEntity{e->m_AcquaintancePed, false, 60.f};
            }
            return new CTaskComplexCarDriveMissionFleeScene{m_Ped->m_pVehicle};
        }
        case TASK_SIMPLE_GUN_CTRL: // 0x4C2123
            return new CTaskSimpleGunControl{ 
                e->m_AcquaintancePed,
                {},
                {},
                eGunCommand::NONE,
                1,
                10'000
            };
        case TASK_SIMPLE_GANG_DRIVEBY: {
            if (!m_Ped->IsInVehicle() || !m_Ped->m_pVehicle->IsDriver(m_Ped) || !m_Ped->GetTaskManager().Find<CTaskComplexCarDrive>()) {
                return nullptr;
            }
            const auto pedActiveWI = &m_Ped->GetActiveWeapon().GetWeaponInfo(m_Ped);
            if (pedActiveWI->GetFireType() != WEAPON_FIRE_INSTANT_HIT) {
                return nullptr;
            }
            const auto maxFireRange = std::max(pedActiveWI->m_fTargetRange, m_Ped->GetIntelligence()->GetPedFOVRange() + 5.f);
            if ((e->m_AcquaintancePed->GetPosition() - m_Ped->GetPosition()).SquaredMagnitude() >= sq(maxFireRange)) { // out of shooting range
                return nullptr;
            }
            return new CTaskSimpleGangDriveBy{ // 0x4C229B
                e->m_AcquaintancePed,
                nullptr,
                maxFireRange,
                90,
                eDrivebyStyle::AI_ALL_DIRN,
                false
            };
        }
        case TASK_COMPLEX_KILL_PED_ON_FOOT_STEALTH: // 0x4C22C6
          return new CTaskComplexKillPedOnFootStealth{e->m_AcquaintancePed};
        case TASK_COMPLEX_KILL_PED_ON_FOOT_KINDA_STAND_STILL: {
            if (IsKillTaskAppropriate(m_Ped, e->m_AcquaintancePed, *e)) {
                m_Ped->bKindaStayInSamePlace = true;
                return new CTaskComplexKillPedOnFoot{e->m_AcquaintancePed};
            }
            if (m_Ped->bWantedByPolice && e->m_AcquaintancePed->IsCop()) {
                return new CTaskComplexFleeAnyMeans{e->m_AcquaintancePed, true, 60.f};
            }
            return new CTaskComplexSmartFleeEntity{e->m_AcquaintancePed, false, 60.f}; // 0x4C20F0
        }
        case TASK_COMPLEX_KILL_PED_AND_REENTER_CAR: // 0x4C1D64
            if (m_Ped->m_pVehicle) {
                return new CTaskComplexSequence{
                    new CTaskComplexKillPedOnFoot{e->m_AcquaintancePed},
                    new CTaskComplexCarDriveWander{m_Ped->m_pVehicle}
                };
            }
            return new CTaskComplexKillPedOnFoot{e->m_AcquaintancePed};
        case TASK_COMPLEX_KILL_CRIMINAL: { // 0x4C1D64
            if (e->m_AcquaintancePed->IsPlayer()) {
                return nullptr;
            }
            return new CTaskComplexKillCriminal{e->m_AcquaintancePed, true};
        }
        case TASK_COMPLEX_FLEE_ANY_MEANS: // 0x4C19D3
            return new CTaskComplexFleeAnyMeans{e->m_AcquaintancePed, true, 60.f};
        case TASK_COMPLEX_SEQUENCE: {
            const auto seqIdx = m_Ped->GetIntelligence()->m_NextEventResponseSequence;
            if (seqIdx < 0) {
                return nullptr;
            }
            const auto seq = &CTaskSequences::ms_taskSequence[seqIdx];
            if (!seq->m_Tasks[0]) {
                return nullptr;
            }
            return new CTaskComplexUseSequence{seqIdx};
        }
        case TASK_COMPLEX_USE_CLOSEST_FREE_SCRIPTED_ATTRACTOR:
            return new CTaskComplexUseClosestFreeScriptedAttractor{};
        case TASK_COMPLEX_USE_CLOSEST_FREE_SCRIPTED_ATTRACTOR_RUN:
            return new CTaskComplexUseClosestFreeScriptedAttractorRun{};
        case TASK_COMPLEX_USE_CLOSEST_FREE_SCRIPTED_ATTRACTOR_SPRINT:
            return new CTaskComplexUseClosestFreeScriptedAttractorSprint{};
        case TASK_SIMPLE_COWER:
            return new CTaskSimpleCower{};
        case TASK_SIMPLE_HANDS_UP: // 0x4C1B39
            return new CTaskSimpleHandsUp{5'000};
        case TASK_COMPLEX_FLEE_ENTITY:
            return new CTaskComplexSmartFleeEntity{e->m_AcquaintancePed, false, 60.f};
        case TASK_COMPLEX_SMART_FLEE_ENTITY: { // 0x4C1C4B
            if (m_Ped->bWantedByPolice && e->m_AcquaintancePed->IsCop()) {
                return new CTaskComplexSmartFleeEntity{
                    e->m_AcquaintancePed,
                    false,
                    60.f,
                    1'000'000,
                    1'000,
                    1.f,
                    e->GetEventType() == EVENT_ACQUAINTANCE_PED_DISLIKE ? eMoveState::PEDMOVE_WALK : eMoveState::PEDMOVE_SPRINT
                };
            }
            return new CTaskComplexFleeAnyMeans{e->m_AcquaintancePed, true, 60.f};
        }
        case TASK_SIMPLE_DUCK_FOREVER: // 0x4C1B5E
            return new CTaskSimpleDuck{DUCK_STANDALONE, 0x967Fu};
        case TASK_SIMPLE_DUCK: { // 0x4C1BA1
            if (const auto t = m_Ped->GetIntelligence()->GetTaskDuck()) {
                return t;
            }
            return new CTaskSimpleDuck{DUCK_STANDALONE, 5'000};
        }
        case TASK_COMPLEX_LEAVE_CAR :
            return new CTaskComplexLeaveCar{m_Ped->m_pVehicle, 0, 0, false, 1};
        case TASK_NONE:
            return nullptr;
        default:
            NOTSA_UNREACHABLE();
        }
    }();
}

// 0x4C1910
void CEventHandler::ComputePedToChaseResponse(CEventPedToChase* e, CTask* tactive, CTask* tsimplest) {
    m_EventResponseTask = [&]() -> CTask* {
        if (!e->m_ped) {
            return nullptr;
        }
        return new CTaskComplexSeekEntity<>{
            e->m_ped,
            30'000,
            1'000,
            1.f,
            2.f,
            2.f,
            true,
            true
        };
    }();
}

// 0x4B9B50
void CEventHandler::ComputePedToFleeResponse(CEventPedToFlee* e, CTask* tactive, CTask* tsimplest) {
    m_EventResponseTask = [&]() -> CTask* {
        if (!e->m_ped) {
            return nullptr;
        }
        m_Ped->Say(CTX_GLOBAL_CRIMINAL_PLEAD);
        return new CTaskComplexSmartFleeEntity{e->m_ped, true, 100.f, -1};
    }();
}

// 0x4BF9B0
void CEventHandler::ComputePersonalityResponseToDamage(CEventDamage* e, CPed* src) {
    m_EventResponseTask = [&]() -> CTask* {
        switch (e->m_taskId) {
        case TASK_COMPLEX_KILL_PED_ON_FOOT_STEALTH: // 0x4BFF02
            return new CTaskComplexKillPedOnFootStealth{src};
        case TASK_COMPLEX_KILL_CRIMINAL: { // 0x4BFF17
            if (src->IsPlayer()) {
                return nullptr;
            }
            if (IsKillTaskAppropriate(m_Ped, src, *e)) {
                return new CTaskComplexKillCriminal{src, false};
            }
            return new CTaskComplexSmartFleeEntity{src, false, 60.f};
        }
        case TASK_COMPLEX_KILL_PED_ON_FOOT: { // 0x4BFFE3
            // I've copy pasted this piece of code at least 3 times now
            if (IsKillTaskAppropriate(m_Ped, src, *e)) {
                return new CTaskComplexKillPedOnFoot{src};
            }
            if (m_Ped->bWantedByPolice && src->IsCop()) {
                return new CTaskComplexFleeAnyMeans{src, true, 60.f};
            }
            if (!m_Ped->IsInVehicle() || !m_Ped->m_pVehicle->IsDriver(m_Ped)) {
                return new CTaskComplexSmartFleeEntity{src, false, 60.f};
            }
            return new CTaskComplexCarDriveMissionFleeScene{m_Ped->m_pVehicle};
        }
        case TASK_COMPLEX_FLEE_ANY_MEANS: // 0x4BFC28
            return new CTaskComplexFleeAnyMeans{src, true, 60.f};
        case TASK_COMPLEX_SMART_FLEE_ENTITY: // 0x4BFC58
            m_Ped->Say(CTX_GLOBAL_RUN_FROM_FIGHT);
            [[fallthrough]];
        case TASK_COMPLEX_FLEE_ENTITY: { // 0x4BFD1B
            if (m_Ped->bWantedByPolice && src->IsCop()) {
                return new CTaskComplexFleeAnyMeans{src, true, 60.f};
            }
            return new CTaskComplexSmartFleeEntity{src, false, 60.f};
        }
        case TASK_COMPLEX_CAR_DRIVE_MISSION_KILL_PED: { // 0x4BFDD7
            if (m_Ped->IsInVehicle()) {
                return new CTaskComplexCarDriveMissionKillPed{m_Ped->m_pVehicle, src};
            }
            return new CTaskComplexKillPedOnFoot{src};
        }
        case TASK_COMPLEX_CAR_DRIVE_MISSION_FLEE_SCENE: { // 0x4BF9DD
            if (m_Ped->IsInVehicle()) {
                return new CTaskComplexCarDriveMissionKillPed{m_Ped->m_pVehicle, src};
            }
            return new CTaskComplexFleeAnyMeans{src, true, 60.f};
        }
        case TASK_COMPLEX_DIVE_FROM_ATTACHED_ENTITY_AND_GET_UP: // 0x4BFAE2
            return new CTaskComplexDiveFromAttachedEntityAndGetUp{};
        case TASK_SIMPLE_DUCK_FOREVER: // 0x4BFAEE
            return new CTaskSimpleDuck{DUCK_STANDALONE, 0xE0FF};
        case TASK_SIMPLE_DUCK: { // 0x4BFB29
            if (const auto t = m_Ped->GetIntelligence()->GetTaskDuck()) {
                return t;
            }
            return new CTaskSimpleDuck{DUCK_STANDALONE, CGeneral::GetRandomNumberInRange<uint16>(2'000, 5'000)};
        }
        case TASK_COMPLEX_USE_CLOSEST_FREE_SCRIPTED_ATTRACTOR:
            return new CTaskComplexUseClosestFreeScriptedAttractor{};
        case TASK_COMPLEX_USE_CLOSEST_FREE_SCRIPTED_ATTRACTOR_RUN:
            return new CTaskComplexUseClosestFreeScriptedAttractorRun{};
        case TASK_COMPLEX_USE_CLOSEST_FREE_SCRIPTED_ATTRACTOR_SPRINT:
            return new CTaskComplexUseClosestFreeScriptedAttractorSprint{};
        case TASK_SIMPLE_BE_DAMAGED: // 0x4BFA09
        case TASK_NONE:
            return nullptr;
        default:
            NOTSA_UNREACHABLE();
        }
    }();
}

// 0x4B8CE0
void CEventHandler::ComputePlayerCollisionWithPedResponse(CEventPlayerCollisionWithPed* e, CTask* tactive, CTask* tsimplest) {
    std::tie(m_EventResponseTask, m_SayTask) = [&]() -> std::pair<CTask*, CTask*> {
        const auto plyr = m_Ped->AsPlayer();
        if (!e->m_victim) {
            return {nullptr, nullptr};
        }

        if (e->m_victim->GetTaskManager().GetActiveTaskAs<CTaskComplexFollowLeaderInFormation>()) { // 0x4B8DAE - Moved here to be able to early out
            return {nullptr, new CTaskSimpleSay{CTX_GLOBAL_BUMP}};
        }

        const auto victimToPlayer             = (e->m_victim->GetPosition() - plyr->GetPosition());
        const auto isCollisionInFrontOfPlayer = !std::signbit(victimToPlayer.Dot(plyr->GetForward()));
        const auto isCollisionBehindVictim    = std::signbit(victimToPlayer.Dot(e->m_victim->GetForward()));

        const auto DoLookAt = [&](CPed* looker, int32 time) {
            g_ikChainMan.LookAt(
                "CompPlayerCollPedResp",
                looker,
                looker == e->m_victim ? plyr : e->m_victim,
                time,
                BONE_HEAD,
                nullptr,
                true,
                0.25f,
                500,
                3,
                false
            );
        };

        if (e->m_movestate == PEDMOVE_WALK) { // 0x4B8DF1
            if (e->m_victimMoveState == PEDMOVE_WALK) { // Both peds were walking
                if (isCollisionInFrontOfPlayer || isCollisionBehindVictim) { // 0x4B8E05 and 0x4B8E5F
                    DoLookAt(isCollisionInFrontOfPlayer ? plyr : e->m_victim, 2000);
                    if (isCollisionBehindVictim) {
                        plyr->AnnoyPlayerPed(false);
                    }
                }
                return {nullptr, new CTaskSimpleSay{CTX_GLOBAL_BUMP}}; // Just say something, and move on
            }
        } else if (!notsa::contains({PEDMOVE_RUN, PEDMOVE_SPRINT}, e->GetMoveState()) || !notsa::contains({PEDMOVE_WALK, PEDMOVE_RUN, PEDMOVE_SPRINT}, e->GetVictimMoveState())) { // 0x4B8EDC - One of the peds was walking and the othe was running/sprinting
            if (e->m_movestate == PEDMOVE_STILL && notsa::contains({ PEDMOVE_WALK, PEDMOVE_RUN, PEDMOVE_SPRINT }, e->GetVictimMoveState()) && isCollisionBehindVictim) {
                DoLookAt(plyr, 2'000);
                return {
                    e->m_victimMoveState != PEDMOVE_WALK
                        ? new CTaskComplexHitResponse{(eDirection)CPedGeometryAnalyser::ComputePedHitSide(*e->m_victim, *plyr)}
                        : nullptr,
                    new CTaskSimpleSay{CTX_GLOBAL_BUMP}
                };
            } else if (notsa::contains({PEDMOVE_RUN, PEDMOVE_SPRINT}, e->GetMoveState())) { // 0x4B8FBF
                if (notsa::contains({PEDMOVE_NONE, PEDMOVE_STILL}, e->GetVictimMoveState()) && isCollisionInFrontOfPlayer && e->GetMoveState() == PEDMOVE_SPRINT) { // 0x4B8FCD
                    DoLookAt(plyr, 2'000);
                    return {nullptr, new CTaskSimpleSay{CTX_GLOBAL_BUMP}};
                }
            }
        } else if (!isCollisionInFrontOfPlayer && isCollisionBehindVictim || !isCollisionBehindVictim) { // 0x4B9057 and 0x4B905F
            DoLookAt(plyr, 2'000);
            plyr->AnnoyPlayerPed(false);
            return {nullptr, new CTaskSimpleSay{CTX_GLOBAL_BUMP}};
        } else if (e->m_victimMoveState != PEDMOVE_WALK) { // 0x4B9068
            const auto plyrHitSide = CPedGeometryAnalyser::ComputePedHitSide(*e->m_victim, *plyr);
            plyr->AnnoyPlayerPed(false);
            if (   (plyr->m_pStats->m_fDefendWeakness <= 1.5f || e->m_victim->m_pStats->m_fDefendWeakness <= 1.5f) && plyr->m_pStats->m_fDefendWeakness <= 1.5f // 0x4B9073
                || plyr->m_pStats->m_fDefendWeakness <=e->m_victim->m_pStats->m_fDefendWeakness
            ) { 
                // 0x4B90A6
                DoLookAt(plyr, 2'000);
                return {
                    e->m_movestate == PEDMOVE_SPRINT
                        ? new CTaskComplexHitResponse{(eDirection)plyrHitSide}
                        : nullptr,
                    new CTaskSimpleSay{CTX_GLOBAL_BUMP}
                };
            }
            return { // 0x4B90DF
                new CTaskComplexFallAndGetUp{plyrHitSide, false},
                nullptr
            };
        }
        return {nullptr, nullptr};
    }();
}

// 0x4BB280
void CEventHandler::ComputePlayerWantedLevelResponse(CEvent* e, CTask* tactive, CTask* tsimplest) {
    NOTSA_UNREACHABLE(); // This event doesn't seem to exist
}

// 0x4C2610
void CEventHandler::ComputePotentialPedCollideResponse(CEventPotentialWalkIntoPed* e, CTask* tactive, CTask* tsimplest) {
    m_EventResponseTask = [&]() -> CTask* {
        if (!e->m_ped) {
            return nullptr;
        }
        const auto moveState = tsimplest && CTask::IsGoToTask(tsimplest)
            ? e->m_moveState
            : PEDMOVE_WALK;
        switch (e->m_taskId) {
        case TASK_NONE: // 0x4C281B
            return nullptr;
        case TASK_COMPLEX_AVOID_OTHER_PED_WHILE_WANDERING: { // 0x4C2707
            const auto tGangFollower = m_Ped->GetTaskManager().Find<CTaskComplexGangFollower>();
            const auto wanderingMoveState = [&]{
                if (moveState <= PEDMOVE_WALK) {
                    return moveState;
                }
                if (!tGangFollower || !tGangFollower->m_Leader) {
                    return moveState;
                }
                const auto gangLeader = tGangFollower->m_Leader;
                const auto leaderMoveState = gangLeader->IsPlayer()
                    ? gangLeader->m_nMoveState
                    : gangLeader->GetIntelligence()->GetMoveStateFromGoToTask();
                if (leaderMoveState >= PEDMOVE_RUN) {
                    return moveState;
                }
                const auto tSeekEntity = m_Ped->GetTaskManager().Find<CTaskComplexSeekEntity<>>();
                if (!tSeekEntity) {
                    return moveState;
                }
                const auto pedToLeaderDistSq = (gangLeader->GetPosition() - m_Ped->GetPosition()).SquaredMagnitude();
#ifdef FIX_BUGS // Can't do substraction and shit with squared values, it won't work too well
                if (std::sqrt(pedToLeaderDistSq) - tSeekEntity->GetMoveStateRadius() < tSeekEntity->GetMoveStateRadius()) {
#else
                if (pedToLeaderDistSq - sq(tSeekEntity->GetMoveStateRadius()) < sq(tSeekEntity->GetMoveStateRadius())) {
#endif
                    return PEDMOVE_WALK;
                }
                return moveState;
            }();
            return new CTaskComplexAvoidOtherPedWhileWandering{e->m_ped, e->m_targetPoint, wanderingMoveState, tGangFollower != nullptr};
        }
        case TASK_COMPLEX_PARTNER_SHOVE:
            return new CTaskComplexPartnerShove{
                "CompPotPedCollResp",
                e->m_ped,
                true,
                0.5f,
                1,
                {}
            };
        default:
            NOTSA_UNREACHABLE();
        }
    }();  
}

// 0x4BBCD0
void CEventHandler::ComputePotentialWalkIntoFireResponse(CEventPotentialWalkIntoFire* e, CTask* tactive, CTask* tsimplest) {
    m_EventResponseTask = [&]() -> CTask* {
        if (!tsimplest || !CTask::IsGoToTask(tsimplest)) {
            return nullptr;
        }
        switch (e->m_taskId) {
        case TASK_COMPLEX_SMART_FLEE_POINT:
            return new CTaskComplexSmartFleePoint{e->m_firePos, false, 60.f, 1'000'000};
        case TASK_COMPLEX_WALK_ROUND_FIRE:
            return new CTaskComplexWalkRoundFire{
                e->m_moveState,
                e->m_firePos,
                e->m_radius,
                static_cast<CTaskSimpleGoTo*>(tsimplest)->m_vecTargetPoint
            };
        case TASK_COMPLEX_FLEE_POINT:
            return new CTaskComplexFleePoint{e->m_firePos, false, 60.f, 1'000'000};
        case TASK_NONE:
            return nullptr;
        default:
            NOTSA_UNREACHABLE();
        }
    }();
}

// 0x4BAA30
void CEventHandler::ComputeReallyLowHealthResponse(CEventHealthReallyLow* e, CTask* tactive, CTask* tsimplest) {
    m_EventResponseTask = [&]() -> CTask* {
        switch (e->m_taskId) {
        case TASK_COMPLEX_DIVE_FROM_ATTACHED_ENTITY_AND_GET_UP:
            return new CTaskComplexDiveFromAttachedEntityAndGetUp{0};
        case TASK_NONE:
            return nullptr;
        default:
            NOTSA_UNREACHABLE();
        }
    }();    
}

// 0x4B97B0
void CEventHandler::ComputeReviveResponse(CEventRevived* e, CTask* tactive, CTask* tsimplest) {
    std::tie(m_EventResponseTask, m_SayTask) = [&]() -> std::pair<CTask*, CTask*> {
        m_Ped->m_fHealth                = 100.f;
        m_Ped->m_bUsesCollision         = true;
        m_Ped->bKnockedUpIntoAir        = false;
        m_Ped->bKnockedOffBike          = false;
        m_Ped->bKilledByStealth         = false;
        m_Ped->physicalFlags.bDestroyed = false;

        m_Ped->SetPedState(PEDSTATE_IDLE);
        m_Ped->RestartNonPartialAnims();

        const auto tm = &m_Ped->GetTaskManager();
        if (!tm->GetTaskPrimary(TASK_PRIMARY_DEFAULT) && !tm->GetTaskPrimary(TASK_PRIMARY_PRIMARY)) {
            tm->SetTask(
                CTaskComplexWander::GetWanderTaskByPedType(m_Ped),
                TASK_PRIMARY_PRIMARY
            );
        }
        
        return {
            new CTaskComplexGetUpAndStandStill{},
            new CTaskSimpleSay{CTX_GLOBAL_SAVED}
        };
    }();
}

// 0x4BA7C0
void CEventHandler::ComputeScriptCommandResponse(CEventScriptCommand* e, CTask* tactive, CTask* tsimplest) {
    const auto tm = &m_Ped->GetTaskManager();
    const auto taskIdx = e->m_primaryTaskIndex == TASK_PRIMARY_PRIMARY
        ? TASK_PRIMARY_PRIMARY
        : TASK_PRIMARY_DEFAULT;
    const auto t = tm->GetTaskPrimary(taskIdx);
    if (t && !t->MakeAbortable(m_Ped, ABORT_PRIORITY_URGENT, e)) {
        t->MakeAbortable(m_Ped, ABORT_PRIORITY_LEISURE, e);
        const auto ce = static_cast<CEventScriptCommand*>(m_Ped->GetEventGroup().Add(e));
        if (const auto tr = CPedScriptedTaskRecord::GetRecordAssociatedWithEvent(e)) {
            tr->AssociateWithEvent(ce);
        }
    } else {
        const auto ct = e->CloneScriptTask();
        tm->ClearTaskEventResponse();
        tm->SetTask(ct, taskIdx, true);
        if (const auto tr = CPedScriptedTaskRecord::GetRecordAssociatedWithEvent(e)) {
            tr->AssociateWithTask(ct);
        }
    }
}

// 0x4BC050
void CEventHandler::ComputeSeenCopResponse(CEventSeenCop* e, CTask* tactive, CTask* tsimplest) {
    m_EventResponseTask = [&]() -> CTask* {
        if (!e->m_AcquaintancePed) {
            return nullptr;
        }
        switch (e->m_taskId) {
        case TASK_FINISHED:
            return nullptr;
        case TASK_COMPLEX_SMART_FLEE_ENTITY:
            return new CTaskComplexSmartFleeEntity{ e->m_AcquaintancePed, false, 60.f };
        case TASK_COMPLEX_KILL_PED_ON_FOOT:
            return new CTaskComplexKillPedOnFoot{ e->m_AcquaintancePed };
        default:
            NOTSA_UNREACHABLE(); // Not sure
        }
    }();
}

// 0x4C35F0
void CEventHandler::ComputeSeenPanickedPedResponse(CEventSeenPanickedPed* e, CTask* tactive, CTask* tsimplest) {
    m_EventResponseTask = [&]() -> CTask* {
        const auto thisEventSrc = e->GetSourceEntity();
        if (!thisEventSrc) {
            return nullptr;
        }
        const auto currEvnt = m_History.GetCurrentEvent();
        if (!currEvnt) {
            return nullptr;
        }
        const auto currEvntSrc = currEvnt->GetSourceEntity();
        if (!currEvntSrc) {
            return nullptr;
        }
        switch (e->m_taskId) {
        case TASK_SIMPLE_DUCK_FOREVER:
            return new CTaskSimpleDuck{ DUCK_STANDALONE, 57599u, -1 };
        case TASK_COMPLEX_FLEE_ENTITY:
            return new CTaskComplexFleeEntity{ currEvntSrc, true, 45.f };
        case TASK_COMPLEX_SMART_FLEE_ENTITY:
            return new CTaskComplexSmartFleeEntity{ currEvntSrc, false, 45.f };
        default:
            NOTSA_UNREACHABLE(); // Not sure
        }
    }();
}

// 0x4B99F0
void CEventHandler::ComputeSexyPedResponse(CEventSexyPed* e, CTask* tactive, CTask* tsimplest) {
    m_EventResponseTask = [&]() -> CTask* {
        if (!e->m_SexyPed) {
            return nullptr;
        }
        switch (e->m_taskId) {
        case TASK_NONE:
            return nullptr;
        case TASK_COMPLEX_GANG_HASSLE_PED:
            return new CTaskGangHasslePed{ e->m_SexyPed, 0, 10'000, 30'000 };
        default:
            NOTSA_UNREACHABLE(); // Not sure
        }
    }();
}

// 0x4B9AA0
void CEventHandler::ComputeSexyVehicleResponse(CEventSexyVehicle* e, CTask* tactive, CTask* tsimplest) {
    m_EventResponseTask = [&]() -> CTask* {
        if (!e->m_vehicle) {
            return nullptr;
        }
        g_InterestingEvents.Add(CInterestingEvents::EType::INTERESTING_EVENT_8, e->m_vehicle);
        g_ikChainMan.LookAt("CompSexyVhclResp", m_Ped, e->m_vehicle, 5000, BONE_UNKNOWN, nullptr, true, 0.25f, 500, 3, false);
        return new CTaskSimpleStandStill(5000, false, false, 8.0f);
    }();
}

// 0x4BC710
void CEventHandler::ComputeShotFiredResponse(CEventGunShot* e, CTask* tactive, CTask* tsimplest) {
    m_EventResponseTask = [&]() -> CTask* {
        if (!e->m_firedBy) {
            return nullptr;
        }
        const auto firedByPed = e->m_firedBy->IsPed()
            ? e->m_firedBy->AsPed()
            : nullptr;
        switch (e->m_taskId) {
        case TASK_COMPLEX_TURN_TO_FACE_ENTITY: // 0x4BCAE4
            return new CTaskComplexTurnToFaceEntityOrCoord{e->m_firedBy, 0.5f};
        case TASK_COMPLEX_KILL_PED_ON_FOOT: { // 0x4BC8F6
            if (!firedByPed || firedByPed->m_nPedType == m_Ped->m_nPedType) {
                return nullptr;
            }
            if (IsKillTaskAppropriate(m_Ped, firedByPed, *e)) {
                return new CTaskComplexKillPedOnFoot{firedByPed}; // 0x4BC950
            }
            if (m_Ped->IsInVehicle()) {
                if (m_Ped->m_pVehicle->IsDriver(m_Ped)) {
                    return new CTaskComplexCarDriveMissionFleeScene{ m_Ped->m_pVehicle }; // 0x4BC997
                }
                if (m_Ped->m_pVehicle->m_pDriver) {
                    return nullptr; // 0x4BC9CC
                }
            }
            return new CTaskComplexSmartFleeEntity{e->m_firedBy, false, 60.f};
        }
        case TASK_COMPLEX_KILL_PED_ON_FOOT_STEALTH: // 0x4BCB16
            return new CTaskComplexKillPedOnFootStealth{ firedByPed };
        case TASK_COMPLEX_KILL_CRIMINAL: { // 0x4BCA16
            if (!firedByPed || firedByPed->m_nPedType == m_Ped->m_nPedType || firedByPed->IsPlayer()) {
                return nullptr;
            }
            if (IsKillTaskAppropriate(m_Ped, firedByPed, *e)) { // 0x4BCA51 - Inverted
                return new CTaskComplexKillCriminal{ firedByPed, false }; // 0x4BCA7F
            }
            return new CTaskComplexSmartFleeEntity{ e->m_firedBy, false, 60.f };
        }
        case TASK_COMPLEX_SMART_FLEE_ENTITY: // 0x4BC8CE
            return new CTaskComplexSmartFleeEntity{ e->m_firedBy, true, 60.f };
        case TASK_COMPLEX_CAR_DRIVE_MISSION_FLEE_SCENE: { // 0x4BC7EB
            const auto v = m_Ped->m_pVehicle;
            if (v && v->IsDriver(m_Ped)) {
                return new CTaskComplexCarDriveMissionFleeScene{ v }; // 0x4BC835
            }
            return new CTaskComplexSmartFleeEntity{ v, false, 60.f }; // 0x4BC885
        }
        case TASK_SIMPLE_DUCK_FOREVER:
            return new CTaskSimpleDuck{ DUCK_STANDALONE, 0x967Fu, -1 };
        case TASK_NONE:
            return nullptr;
        case TASK_SIMPLE_DUCK: {
            if (m_Ped->GetIntelligence()->GetTaskDuck(true)) {
                return new CTaskSimpleDuck{ DUCK_STANDALONE, 3000, -1 };
            }
            return nullptr;
        }
        default:
            NOTSA_UNREACHABLE(); // not sure
        }
    }();
}

// 0x4BBE30
void CEventHandler::ComputeShotFiredWhizzedByResponse(CEventGunShotWhizzedBy* e, CTask* tactive, CTask* tsimplest) {
    m_EventResponseTask = [&]() -> CTask* {
        const auto esrc = e->GetSourceEntity();
        if (!esrc) {
            return nullptr;
        }
        switch (e->m_taskId) {
        case TASK_NONE:
            return nullptr; // 0x4BBE91
        case TASK_SIMPLE_DUCK_WHILE_SHOTS_WHIZZING: { // 0x4BBE68
            const auto ProcessTaskDuck = [this](CTaskSimpleDuck* tDuck) {
                if (tDuck->m_ShotWhizzingCounter >= 0) {
                    tDuck->RestartTask(m_Ped);
                }
            };

            if (tsimplest) {
                if (const auto tDuck = CTask::DynCast<CTaskSimpleDuck>(tsimplest)) {
                    ProcessTaskDuck(tDuck);
                    return nullptr;
                }
            }

            if (const auto tDuck = m_Ped->GetIntelligence()->GetTaskDuck()) {
                ProcessTaskDuck(tDuck);
                return nullptr;
            }

            return new CTaskSimpleDuck{ DUCK_STANDALONE_WEAPON_CROUCH, CGeneral::GetRandomNumberInRange<uint16>(3'000, 5'000), 1'000 };
        }
        default:
            NOTSA_UNREACHABLE();
        }
    }();
}

// 0x4BB050
void CEventHandler::ComputeSignalAtPedResponse(CEventSignalAtPed* e, CTask* tactive, CTask* tsimplest) {
    m_EventResponseTask = [&]() -> CTask* {
        return new CTaskComplexSignalAtPed{ e->m_SignalAt, e->m_InitialPause, e->m_bPlayAnimAtEnd };
    }();
}

// 0x4BB800
void CEventHandler::ComputeSpecialResponse(CEventSpecial* e, CTask* tactive, CTask* tsimplest) {
    m_EventResponseTask = [&]() -> CTask* {
        switch (e->m_taskId) {
        case TASK_COMPLEX_KILL_PED_ON_FOOT:
            return new CTaskComplexKillPedOnFoot{ FindPlayerPed() }; // 0x4BBB0F
        case TASK_COMPLEX_SMART_FLEE_ENTITY:
            return new CTaskComplexSmartFleeEntity{ FindPlayerPed(), false, 60.f }; // 0x4BBA54
        case TASK_COMPLEX_SMART_FLEE_POINT:
            return new CTaskComplexSmartFleePoint{ FindPlayerPed()->GetPosition(), false, 1000.f, 100'000 }; // 0x4BBAA6
        case TASK_COMPLEX_CAR_DRIVE_MISSION_FLEE_SCENE:
            return new CTaskComplexCarDriveMissionFleeScene{nullptr}; // 0x4BBAD2 - Makes no sense
        case TASK_COMPLEX_ARREST_PED:
            return new CTaskComplexArrestPed{ FindPlayerPed() }; // 0x4BBB65
        case TASK_COMPLEX_USE_CLOSEST_FREE_SCRIPTED_ATTRACTOR:
            return new CTaskComplexUseClosestFreeScriptedAttractor{};
        case TASK_COMPLEX_USE_CLOSEST_FREE_SCRIPTED_ATTRACTOR_RUN:
            return new CTaskComplexUseClosestFreeScriptedAttractorRun{};
        case TASK_COMPLEX_USE_CLOSEST_FREE_SCRIPTED_ATTRACTOR_SPRINT:
            return new CTaskComplexUseClosestFreeScriptedAttractorSprint{};
        case TASK_COMPLEX_LEAVE_CAR_AND_WANDER: {
            if (const auto v = m_Ped->m_pVehicle) {
                return new CTaskComplexLeaveCarAndWander{ v }; // 0x4BB9D7
            }
            return nullptr;
        }
        case TASK_COMPLEX_LEAVE_CAR: {
            if (const auto v = m_Ped->m_pVehicle) {
                return new CTaskComplexLeaveCar{ v, TARGET_DOOR_FRONT_LEFT, 0, true, false }; // 0x4BB98E
            }
            return nullptr;
        }
        case TASK_COMPLEX_LEAVE_CAR_AND_FLEE: {
            if (const auto v = m_Ped->m_pVehicle) { 
                return new CTaskComplexLeaveCarAndFlee{ v, FindPlayerPed()->GetPosition(), TARGET_DOOR_FRONT_LEFT, 0, false }; // 0x4BB98E
            }
            return nullptr;
        }
        default:
            NOTSA_UNREACHABLE(); // not sure
        }
    }();
}

// 0x4BD6A0
void CEventHandler::ComputeVehicleCollisionResponse(CEventVehicleCollision* e, CTask* tactive, CTask* tsimplest) {
    m_EventResponseTask = [&]() -> CTask* {
        if (!e->m_vehicle) {
            return nullptr;
        }

        m_Ped->bPushedAlongByCar = false;

        if (e->m_evadeType == VEHICLE_EVADE_BY_HITSIDE) {
            return new CTaskComplexEvasiveStep{e->m_vehicle, CTaskComplexHitPedWithCar::ComputeEvasiveStepMoveDir(m_Ped, e->m_vehicle)}; // 0x4BD74A
        }

        const auto IncrementAngerIfPlayerIsDriver = [&]() {
            if (const auto d = e->m_vehicle->m_pDriver) {
                if (d->IsPlayer()) {
                    d->GetIntelligence()->IncrementAngerAtPlayer(2);
                }
            }
        };

        const auto eVehVelSq = e->m_vehicle->GetMoveSpeed().SquaredMagnitude();
        if (eVehVelSq >= sq(0.5f)) { // 0x4BD754
            if (m_Ped->m_pEntityIgnoredCollision != e->m_vehicle) { // Moved up here from 0x4BD861
                m_Ped->bPushedAlongByCar = true;
            }
            float eDmgIntensity = e->m_fDamageIntensity;
            if (m_Ped->IsPlayer()) {
                if (m_Ped->bIsStanding) {
                    const auto fwdImpactDot = (m_Ped->GetForward() * m_Ped->m_vecAnimMovingShiftLocal.y).Dot(e->m_impactNormal);
                    if (fwdImpactDot < 0.f) { // Impact is opposite to our forwards vector
                        eDmgIntensity = std::max(0.f, fwdImpactDot * m_Ped->m_fMass + eDmgIntensity); // 0x4BD7B4
                    }
                }
                eDmgIntensity = std::min(20.f, eDmgIntensity);
            }
            IncrementAngerIfPlayerIsDriver();
            return new CTaskComplexHitPedWithCar{ e->m_vehicle, eDmgIntensity };
        }

        if ((!m_Ped->IsPlayer() || m_Ped->GetTaskManager().GetTaskPrimary(TASK_PRIMARY_PRIMARY)) && tsimplest && CTask::IsGoToTask(tsimplest)) {
            if (e->m_vehicle == m_Ped->m_standingOnEntity) {
                if (std::abs(m_Ped->m_fCurrentRotation - m_Ped->m_fCurrentRotation) < 0.01f && CPedGeometryAnalyser::CanPedJumpObstacle(*m_Ped, *e->m_vehicle)) {
                    return new CTaskComplexJump{ COMPLEX_JUMP_TYPE_JUMP }; // 0x4BD925
                }
            }    
            return new CTaskComplexWalkRoundCar{ // 0x4BD997
                m_Ped->GetGroup()
                    ? PEDMOVE_RUN
                    : (eMoveState)e->m_moveState,
                static_cast<CTaskSimpleGoTo*>(tsimplest)->m_vecTargetPoint,
                e->m_vehicle,
                m_Ped->GetIntelligence()->IsPedGoingForCarDoor(),
                e->m_DirectionToWalkRoundCar
            };   
        }

        if (m_Ped->IsPlayer() && !m_Ped->bIsInTheAir) { // 0x4BD9A3   
            if (m_Ped->m_pPlayerData->m_fMoveBlendRatio != 0.f || !e->m_vehicle->m_pDriver) { // 0x4BDAD2
                g_ikChainMan.LookAt(
                    "CompVehicleCollResp",
                    m_Ped,
                    e->m_vehicle,
                    800,
                    BONE_UNKNOWN,
                    nullptr,
                    true,
                    0.25f,
                    500,
                    3,
                    false
                );
                return nullptr;
            }
            m_Ped->AsPlayer()->AnnoyPlayerPed(false);
            g_ikChainMan.LookAt(
                "CompVehicleCollResp",
                m_Ped,
                e->m_vehicle,
                1300,
                BONE_UNKNOWN,
                nullptr,
                true,
                0.25f,
                500,
                3,
                false
            );
            switch (m_Ped->GetActiveWeapon().GetType()) {
            case WEAPON_GOLFCLUB:
            case WEAPON_KNIFE:
            case WEAPON_BRASSKNUCKLE:
            case WEAPON_UNARMED:
                return new CTaskSimpleShakeFist{};
            }
        } else {
            if (eVehVelSq >= sq(0.001f)) {
                IncrementAngerIfPlayerIsDriver();
                return new CTaskComplexHitPedWithCar{ e->m_vehicle, e->m_fDamageIntensity };
            }
        }
        return nullptr;
    }();
}

// 0x4C2FC0
void CEventHandler::ComputeVehicleDamageResponse(CEventVehicleDamage* e, CTask* tactive, CTask* tsimplest) {
    m_EventResponseTask = [&]() -> CTask* {
        if (!m_Ped->m_pVehicle || m_Ped->m_pVehicle != e->m_vehicle) {
            return nullptr;
        }
        switch (e->m_taskId) {
        case TASK_COMPLEX_KILL_PED_ON_FOOT: { // 0x4C31C5
            if (!e->m_attacker) { 
                return nullptr;
            }
            switch (e->m_attacker->GetType()) {
            case ENTITY_TYPE_PED: {
                if (IsKillTaskAppropriate(m_Ped, e->m_attacker->AsPed(), *e)) {
                    return new CTaskComplexKillPedOnFoot{ e->m_attacker->AsPed() }; // 0x4C33DA
                }
                if (!notsa::IsFixBugs() || m_Ped->m_pVehicle) { // Not a bug per-se, as it's harmless, but we can save an `new` call
                    if (m_Ped->m_pVehicle && m_Ped->m_pVehicle->IsDriver(m_Ped)) {
                        return new CTaskComplexCarDriveMissionFleeScene{ m_Ped->m_pVehicle }; // 0x4C332B
                    } else {
                        return new CTaskComplexSmartFleeEntity{ m_Ped->m_pVehicle, false, 60.f }; // 0x4C33A6
                    }
                }
                return nullptr;
            }
            case ENTITY_TYPE_VEHICLE:
                return new CTaskComplexKillPedOnFoot{ e->m_attacker->AsVehicle()->m_pDriver }; // `driver == null` is not UB
            }
            return nullptr;
        }
        case TASK_COMPLEX_ROAD_RAGE: { // 0x4C31CE
            if (!e->m_attacker) { 
                return nullptr;
            }
            switch (e->m_attacker->GetType()) {
            case ENTITY_TYPE_PED:
                return new CTaskComplexRoadRage{ e->m_attacker->AsPed() };
            case ENTITY_TYPE_VEHICLE:
                return new CTaskComplexRoadRage{ e->m_attacker->AsVehicle()->m_pDriver }; // `driver == null` is not UB
            }
            return nullptr;
        }
        case TASK_COMPLEX_KILL_CRIMINAL: { // 0x4C31E8
            if (!e->m_attacker) { 
                return nullptr;
            }
            switch (e->m_attacker->GetType()) {
            case ENTITY_TYPE_PED: {
                if (!e->m_attacker->AsPed()->IsPlayer()) {
                    return new CTaskComplexKillCriminal{ e->m_attacker->AsPed() };
                }
                return nullptr;
            }
            case ENTITY_TYPE_VEHICLE: {
                const auto attacker = e->m_attacker->AsVehicle();
                if (!attacker->m_pDriver || !attacker->m_pDriver->IsPlayer()) {
                    return new CTaskComplexKillCriminal{ attacker->m_pDriver };
                }
                return nullptr;
            }
            }
            return nullptr;
        }
        case TASK_COMPLEX_CAR_DRIVE_MISSION_KILL_PED: { // 0x4C3111
            if (!e->m_attacker) { 
                return nullptr;
            }
            switch (e->m_attacker->GetType()) {
            case ENTITY_TYPE_PED:
            case ENTITY_TYPE_VEHICLE: {
                if (   e->GetSourceEntity() == FindPlayerPed() // Interesting...
                    && CTheScripts::IsPlayerOnAMission()
                    && m_Ped->IsCreatedBy(PED_GAME)
                    && e->m_weaponType == WEAPON_RAMMEDBYCAR
                ) { 
                    return new CTaskComplexCarDriveMissionFleeScene{ m_Ped->m_pVehicle }; // 0x4C3171
                }
                return new CTaskComplexCarDriveMissionKillPed{ m_Ped->m_pVehicle, e->m_attacker }; // 0x4C31B6
            }
            }
            return nullptr;
        }
        case TASK_COMPLEX_LEAVE_CAR_AND_FLEE: // 0x4C3022
            return new CTaskComplexLeaveCarAndFlee{ m_Ped->m_pVehicle, m_Ped->GetPosition() };
        case TASK_COMPLEX_CAR_DRIVE_MISSION_FLEE_SCENE: { // 0x4C3027
            if (!m_Ped->m_pVehicle->IsDriver(m_Ped)) {
                return new CTaskComplexSmartFleeEntity{ m_Ped->m_pVehicle, false, 60.f }; // 0x4C302E
            }
            return new CTaskComplexCarDriveMissionFleeScene{ m_Ped->m_pVehicle }; 
        }
        case TASK_COMPLEX_SEQUENCE: {
            const auto nextSeqIdx = m_Ped->GetIntelligence()->m_NextEventResponseSequence;
            if (nextSeqIdx >= 0 && CTaskSequences::Get(nextSeqIdx)->IsActive()) {
                return new CTaskComplexUseSequence{ nextSeqIdx }; // 0x4C3105
            }
            return nullptr;
        }
        }
        return nullptr;
    }();
}

// 0x4BA8B0
void CEventHandler::ComputeVehicleDiedResponse(CEventVehicleDied* e, CTask* tactive, CTask* tsimplest) {
    m_EventResponseTask = [&]() -> CTask* {
        if (!e->m_vehicle) {
            return nullptr;
        }
        m_Ped->physicalFlags.bDestroyed = true;
        m_Ped->m_fHealth = 0.f;
        if (m_Ped->IsInVehicle() && (m_Ped->m_pVehicle->IsBike() || m_Ped->m_pVehicle->IsSubQuad())) {
            ComputeKnockOffBikeResponse(e, tactive, tsimplest);
            return m_EventResponseTask;
        }
        return new CTaskComplexDie{ WEAPON_EXPLOSION, ANIM_GROUP_DEFAULT, ANIM_ID_KO_SHOT_FRONT_0 };
    }();
}

// 0x?
void CEventHandler::ComputeVehicleHitAndRunResponse(CEventVehicleHitAndRun* e, CTask* tactive, CTask* tsimplest) {
    NOTSA_UNREACHABLE();
}

// 0x4BB2E0
void CEventHandler::ComputeVehicleOnFireResponse(CEventVehicleOnFire* e, CTask* tactive, CTask* tsimplest) {
    m_EventResponseTask = [&]() -> CTask* {
        if (!e->m_vehicle) {
            return nullptr;
        }
        if ((e->m_vehicle->GetStatus() == STATUS_WRECKED || e->m_vehicle->m_fHealth <= 0.f) && m_Ped->IsInVehicle()) {
            if (m_Ped->IsInVehicle() && (m_Ped->m_pVehicle->IsBike() || m_Ped->m_pVehicle->IsSubQuad())) {
                ComputeKnockOffBikeResponse(e, tactive, tsimplest);
                return m_EventResponseTask;
            }
            return new CTaskComplexDie{ WEAPON_EXPLOSION, ANIM_GROUP_DEFAULT, ANIM_ID_KO_SHOT_FRONT_0 };
        }
        switch (e->m_taskId) {
        case TASK_COMPLEX_LEAVE_CAR: {
            return new CTaskComplexLeaveAnyCar{ 0, 0, false };
        case TASK_COMPLEX_LEAVE_CAR_AND_FLEE:
            return new CTaskComplexSequence{
                new CTaskComplexLeaveAnyCar{ 0, 0, true }, // 0x4BB43D
                new CTaskComplexSmartFleeEntity{ e->m_vehicle, false, 15.f } // 0x4BB48B
            };
        case TASK_COMPLEX_LEAVE_CAR_AND_WANDER:
            return new CTaskComplexSequence{ // Why use a sequence here?
                new CTaskComplexLeaveAnyCar{ 0, 0, false } // 0x4BB506
            };
        case TASK_COMPLEX_CAR_DRIVE_MISSION_FLEE_SCENE: {
            if (m_Ped->m_pVehicle && m_Ped->m_pVehicle->IsDriver(m_Ped)) {
                return new CTaskComplexCarDriveMissionFleeScene{ m_Ped->m_pVehicle };
            }
            return new CTaskComplexSmartFleeEntity{ m_Ped->m_pVehicle, false, 60.f };
        }
        case TASK_COMPLEX_FLEE_ENTITY:
            return new CTaskComplexFleeEntity{ e->m_vehicle, false, 15.f };
        case TASK_COMPLEX_SMART_FLEE_ENTITY:
            return new CTaskComplexSmartFleeEntity{ m_Ped->m_pVehicle, false, 15.f };
        default:
            NOTSA_UNREACHABLE();
        }
        }
    }();
}

// 0x4C0BD0
void CEventHandler::ComputeVehiclePotentialCollisionResponse(CEventPotentialGetRunOver* e, CTask* tactive, CTask* tsimplest) {
    std::tie(m_EventResponseTask, m_PartialAnimTask) = [&]() -> std::pair<CTask*, CTask*> {
        if (!e->m_Vehicle || m_Ped->bInVehicle) {
            return {};
        }
        const CVector pedPos{m_Ped->GetPosition()};

        CVector vehBB[4];
        CPedGeometryAnalyser::ComputeEntityBoundingBoxCorners(pedPos.z, *e->m_Vehicle, vehBB);

        CVector vehBBPlanes[4];
        float vehBBPlanesDot[4];
        CPedGeometryAnalyser::ComputeEntityBoundingBoxPlanesUncachedAll(pedPos.z, *e->m_Vehicle, &vehBBPlanes, vehBBPlanesDot);

        CVector dirToAvoidVehicle;
        CPedGeometryAnalyser::ComputeMoveDirToAvoidEntity(*m_Ped, *e->m_Vehicle, dirToAvoidVehicle);

        // 0x4C0DA2 - 0x4C0E1D
        const auto vehFwdSpeedSq       = e->m_Vehicle->GetMoveSpeed().Dot(e->m_Vehicle->GetForward());
        const auto vehFwdSpeedPerSec = vehFwdSpeedSq * CTimer::GetTimestepPerSecond();
        const auto vehBBCenter  = vehFwdSpeedSq > 0.f
            ? (vehBB[3] + vehBB[0]) / 2.f   // Front side
            : (vehBB[2] + vehBB[1]) / 2.f;  // Back side
        const auto pedToVehBBCenter         = pedPos - vehBBCenter;
        const auto pedToVehBBCenterFwdDot   = pedToVehBBCenter.Dot(e->m_Vehicle->GetForward());
        const auto pedToVehBBCenterRightDot = pedToVehBBCenter.Dot(e->m_Vehicle->GetRight());

        if (m_Ped->IsPlayer()) {
            const auto plyrPed = m_Ped->AsPlayer();
            if (   std::abs(pedToVehBBCenterFwdDot) < 3.f
                && std::abs(pedToVehBBCenterFwdDot) > std::abs(vehFwdSpeedPerSec)
                && plyrPed->m_pPlayerData->m_fMoveBlendRatio < 1.f
                && g_ikChainMan.IsLooking(plyrPed)
            ) { // 0x4C0E84
                plyrPed->AnnoyPlayerPed(false);
                g_ikChainMan.LookAt(
                    "CompVhclPotCollResp",
                    plyrPed,
                    e->m_Vehicle,
                    1500,
                    BONE_UNKNOWN,
                    nullptr,
                    true,
                    0.25f,
                    500,
                    3,
                    false
                );
                switch (plyrPed->GetActiveWeapon().GetType()) {
                case WEAPON_UNARMED:
                case WEAPON_BASEBALLBAT:
                case WEAPON_PISTOL:
                case WEAPON_PISTOL_SILENCED:
                case WEAPON_MICRO_UZI:
                    return { nullptr, new CTaskSimpleShakeFist{} }; // 0x4C0F14
                }
                return {};
            };
        } else if (std::abs(pedToVehBBCenterFwdDot) <= std::abs(2.f * vehFwdSpeedPerSec)) { // 0x4C0F3D
            if (std::abs(vehFwdSpeedSq) <= 0.05f) {
                return {}; // Too slow
            }
            const auto rndChance = CGeneral::GetRandomNumberInRange(0.f, 1.f);
            const auto [chanceToEvadeBack, chanceForHornNeeded, chanceToEvadeFront, minPedDistToBBCenterX] = [&]() -> std::array<float, 4> {
                if (std::abs(vehFwdSpeedSq) <= 0.3f) {
                    return {0.02f, 0.05f, 0.1f, 0.7f};
                }
                return {0.05f, 0.1f, 0.2f, 0.175f};
            }();
            const auto vehBBMaxX = e->m_Vehicle->GetColModel()->GetBoundingBox().m_vecMax.x;
            if ((e->m_Vehicle->GetPosition() - pedPos).Dot(e->m_Vehicle->GetForward()) > 0.f) { // 0x4C0F6D - Is the vehicle going forwards?
                if (vehBBMaxX - minPedDistToBBCenterX < std::abs(pedToVehBBCenterRightDot)) { // 
                    if (rndChance >= chanceToEvadeFront * 0.5f) { // Inverted
                        return { new CTaskComplexEvasiveStep{ e->m_Vehicle, dirToAvoidVehicle }, nullptr };
                    } else { // 0x4C0FD2
                        m_Ped->m_fAimingRotation = (e->m_Vehicle->GetPosition2D() - m_Ped->GetPosition2D()).Heading();
                        return { new CTaskSimpleShakeFist{}, nullptr };
                    }
                } else if (rndChance < chanceToEvadeFront) { // 0x4C10DD
                    const auto pedHeading = (e->m_Vehicle->GetPosition2D() - m_Ped->GetPosition2D()).Heading();
                    m_Ped->m_fCurrentRotation = pedHeading;
                    m_Ped->m_fAimingRotation  = pedHeading;
                    m_Ped->SetHeading(pedHeading);

                    return { new CTaskSimpleHandsUp{ 3000 }, nullptr }; // 0x4C117B
                }
            } else {
                if (rndChance > chanceForHornNeeded && !e->m_Vehicle->m_nHornCounter) {
                    return {};
                }
                if (rndChance > chanceToEvadeBack) {
                    return { new CTaskComplexEvasiveStep{ e->m_Vehicle, dirToAvoidVehicle }, nullptr }; // 0x4C121E
                }
            }
            return { new CTaskComplexEvasiveDiveAndGetUp{ e->m_Vehicle, 0, dirToAvoidVehicle, true }, nullptr }; // 0x4C11BA
        } else if (e->m_Vehicle->GetStatus() == STATUS_PLAYER && m_Ped->IsCreatedBy(PED_GAME)) {
            const auto currPedEvent = m_Ped->GetIntelligence()->GetEventHandler().GetHistory().GetCurrentEvent();
            if (e->m_Vehicle->m_nHornCounter && (!currPedEvent || currPedEvent->GetEventType() != EVENT_POTENTIAL_GET_RUN_OVER)) { // 0x4C12A0
                const auto rndNum = CGeneral::GetRandomNumberInRange(0, 1000);
                if (rndNum < 25 && !m_Ped->IsCop()) { // 0x4C12C0
                    return { new CTaskComplexFleeEntity{ e->m_Vehicle, false, 60.f, CGeneral::GetRandomNumberInRange(750, 1250)}, nullptr };
                } else if (rndNum < 400) {
                    if (rndNum < 100) { // 0x4C1335
                        return { // Linus Torvalds would nuke this code with 6 levels of indentation xD
                            new CTaskComplexSequence{
                                new CTaskSimpleAchieveHeading{ (e->m_Vehicle->GetPosition2D() - m_Ped->GetPosition2D()).Heading() },
                                new CTaskSimpleShakeFist{}
                            },
                            nullptr
                        };
                    } else {
                        g_ikChainMan.LookAt(
                            "CompVhclPotCollResp",
                            m_Ped,
                            e->m_Vehicle,
                            1300,
                            BONE_UNKNOWN,
                            nullptr,
                            true,
                            0.25f,
                            500,
                            3,
                            false
                        );
                        if (rndNum < 200) {
                            return { new CTaskSimpleAchieveHeading{ (e->m_Vehicle->GetPosition2D() - m_Ped->GetPosition2D()).Heading() }, nullptr };
                        }
                    }
                }
            }
        }
        return {};
    }();
}

// 0x4B96D0
void CEventHandler::ComputeVehiclePotentialPassiveCollisionResponse(CEventPotentialWalkIntoVehicle* e, CTask* tactive, CTask* tsimplest) {
    m_EventResponseTask = [&]() -> CTask* {
        if (!e->m_vehicle || !m_Ped->bInVehicle || !CTask::IsGoToTask(tsimplest)) {
            return nullptr;
        }
        const auto tGoTo = static_cast<CTaskSimpleGoTo*>(tsimplest);
        if (tGoTo->m_moveState == PEDMOVE_STILL) {
            return nullptr;
        }
        return new CTaskComplexWalkRoundCar{
            m_Ped->GetGroup()
            ? PEDMOVE_RUN
            : (eMoveState)e->m_moveState,
            static_cast<CTaskSimpleGoTo*>(tsimplest)->m_vecTargetPoint,
            e->m_vehicle,
            m_Ped->GetIntelligence()->IsPedGoingForCarDoor(),
        };
    }();
}

// 0x4B9F80
void CEventHandler::ComputeVehicleToStealResponse(CEventVehicleToSteal* e, CTask* tactive, CTask* tsimplest) {
    m_EventResponseTask = [&]() -> CTask* {
        if (!m_Ped->bInVehicle || !e->m_vehicle) {
            return nullptr;
        }
        return new CTaskComplexStealCar{ e->m_vehicle };
    }();
}

// 0x4BAE30
void CEventHandler::ComputeWaterCannonResponse(CEventHitByWaterCannon* e, CTask* tactive, CTask* tsimplest) {
    m_EventResponseTask = [&]() -> CTask* {
        m_Ped->Say(CTX_GLOBAL_PAIN_HIGH);
        m_Ped->ApplyMoveForce({ 0.f, 0.f, CTimer::GetTimeStep() * 2.f });
        m_Ped->SetMoveSpeedXY((CVector2D{e->m_moveSpeed} * 0.6f + CVector2D{m_Ped->GetMoveSpeed()}) / 2.f);
        if (const auto speed = m_Ped->GetMoveSpeed().Magnitude2D(); speed >= 0.2f) {
            m_Ped->SetMoveSpeedXY(0.2f / speed * CVector2D{m_Ped->GetMoveSpeed()});
        }
        return new CTaskComplexFallAndGetUp{ CPedGeometryAnalyser::ComputePedHitSide(*m_Ped, e->m_moveSpeed), 0 };
    }();
}

// 0x4C3870
void CEventHandler::ComputeEventResponseTask(CEvent* e, CTask* pAbortedTaskEventResponse) {
    m_PhysicalResponseTask = nullptr;
    m_EventResponseTask    = nullptr;
    m_AttackTask           = nullptr;
    m_SayTask              = nullptr;
    m_PartialAnimTask      = nullptr;

    const auto tactive = m_Ped->GetTaskManager().GetActiveTask();
    const auto tsimplest = tactive
        ? m_Ped->GetTaskManager().GetSimplestActiveTask()
        : nullptr;
    
    //DEV_LOG("event: {} tactive: {} tsimplest: {}", (int32)event->GetEventType(), (int32)tactive->GetTaskType(), (int32)tsimplest->GetTaskType()); // NOTSA

    switch (e->GetEventType()) {
    case EVENT_VEHICLE_COLLISION:
        ComputeVehicleCollisionResponse(static_cast<CEventVehicleCollision*>(e), tactive, tsimplest);
        break;
    case EVENT_PED_COLLISION_WITH_PED:
        ComputePedCollisionWithPedResponse(e, tactive, tsimplest);
        break;
    case EVENT_PED_COLLISION_WITH_PLAYER:
        ComputePedCollisionWithPlayerResponse(e, tactive, tsimplest);
        break;
    case EVENT_PLAYER_COLLISION_WITH_PED:
        ComputePlayerCollisionWithPedResponse(static_cast<CEventPlayerCollisionWithPed*>(e), tactive, tsimplest);
        break;
    case EVENT_OBJECT_COLLISION:
        ComputeObjectCollisionResponse(static_cast<CEventObjectCollision*>(e), tactive, tsimplest);
        break;
    case EVENT_BUILDING_COLLISION:
        ComputeBuildingCollisionResponse(static_cast<CEventBuildingCollision*>(e), tactive, tsimplest);
        break;
    case EVENT_DRAGGED_OUT_CAR:
        ComputeDraggedOutCarResponse(static_cast<CEventDraggedOutCar*>(e), tactive, tsimplest);
        break;
    case EVENT_KNOCK_OFF_BIKE:
        ComputeKnockOffBikeResponse(e, tactive, tsimplest);
        break;
    case EVENT_DAMAGE:
        ComputeDamageResponse(static_cast<CEventDamage*>(e), tactive, tsimplest, pAbortedTaskEventResponse);
        break;
    case EVENT_DEATH:
        ComputeDeathResponse(static_cast<CEventDeath*>(e), tactive, tsimplest);
        break;
    case EVENT_DEAD_PED:
        ComputeDeadPedResponse(static_cast<CEventDeadPed*>(e), tactive, tsimplest);
        break;
    case EVENT_POTENTIAL_GET_RUN_OVER:
        ComputeVehiclePotentialCollisionResponse(static_cast<CEventPotentialGetRunOver*>(e), tactive, tsimplest);
        break;
    case EVENT_POTENTIAL_WALK_INTO_PED:
        ComputePotentialPedCollideResponse(static_cast<CEventPotentialWalkIntoPed*>(e), tactive, tsimplest);
        break;
    case EVENT_SHOT_FIRED:
        ComputeShotFiredResponse(static_cast<CEventGunShot*>(e), tactive, tsimplest);
        break;
    case EVENT_COP_CAR_BEING_STOLEN:
        ComputeCopCarBeingStolenResponse(static_cast<CEventCopCarBeingStolen*>(e), tactive, tsimplest);
        break;
    case EVENT_PED_ENTERED_MY_VEHICLE:
        ComputePedEnteredVehicleResponse(static_cast<CEventPedEnteredMyVehicle*>(e), tactive, tsimplest);
        break;
    case EVENT_REVIVE:
        ComputeReviveResponse(static_cast<CEventRevived*>(e), tactive, tsimplest);
        break;
    case EVENT_CHAT_PARTNER:
        ComputeChatPartnerResponse(static_cast<CEventChatPartner*>(e), tactive, tsimplest);
        break;
    case EVENT_SEXY_PED:
        ComputeSexyPedResponse(static_cast<CEventSexyPed*>(e), tactive, tsimplest);
        break;
    case EVENT_SEXY_VEHICLE:
        ComputeSexyVehicleResponse(static_cast<CEventSexyVehicle*>(e), tactive, tsimplest);
        break;
    case EVENT_PED_TO_CHASE:
        ComputePedToChaseResponse(static_cast<CEventPedToChase*>(e), tactive, tsimplest);
        break;
    case EVENT_PED_TO_FLEE:
        ComputePedToFleeResponse(static_cast<CEventPedToFlee*>(e), tactive, tsimplest);
        break;
    case EVENT_ATTRACTOR:
    case EVENT_SCRIPTED_ATTRACTOR:
        ComputeAttractorResponse(static_cast<CEventAttractor*>(e), tactive, tsimplest);
        break;
    case EVENT_VEHICLE_TO_STEAL:
        ComputeVehicleToStealResponse(static_cast<CEventVehicleToSteal*>(e), tactive, tsimplest);
        break;
    case EVENT_GUN_AIMED_AT:
        ComputeGunAimedAtResponse(static_cast<CEventGunAimedAt*>(e), tactive, tsimplest);
        break;
    case EVENT_SCRIPT_COMMAND:
        ComputeScriptCommandResponse(static_cast<CEventScriptCommand*>(e), tactive, tsimplest);
        break;
    case EVENT_IN_AIR:
        if (!m_Ped->bIsStanding) {
            m_EventResponseTask = new CTaskComplexInAirAndLand(false, false);
        }
        break;
    case EVENT_VEHICLE_DIED:
        ComputeVehicleDiedResponse(static_cast<CEventVehicleDied*>(e), tactive, tsimplest);
        break;
    case EVENT_ACQUAINTANCE_PED_HATE:
    case EVENT_ACQUAINTANCE_PED_DISLIKE:
        ComputePedThreatResponse(static_cast<CEventAcquaintancePedHate*>(e), tactive, tsimplest);
        break;
    case EVENT_ACQUAINTANCE_PED_LIKE:
    case EVENT_ACQUAINTANCE_PED_RESPECT:
        ComputePedFriendResponse(static_cast<CEventAcquaintancePed*>(e), tactive, tsimplest);
        break;
    case EVENT_VEHICLE_DAMAGE_WEAPON:
    case EVENT_VEHICLE_DAMAGE_COLLISION:
        ComputeVehicleDamageResponse(static_cast<CEventVehicleDamage*>(e), tactive, tsimplest);
        break;
    case EVENT_SPECIAL:
        ComputeSpecialResponse(static_cast<CEventSpecial*>(e), tactive, tsimplest);
        break;
    case EVENT_GOT_KNOCKED_OVER_BY_CAR:
        ComputeGotKnockedOverByCarResponse(static_cast<CEventGotKnockedOverByCar*>(e), tactive, tsimplest);
        break;
    case EVENT_POTENTIAL_WALK_INTO_OBJECT:
        ComputeObjectCollisionPassiveResponse(static_cast<CEventPotentialWalkIntoObject*>(e), tactive, tsimplest);
        break;
    case EVENT_CAR_UPSIDE_DOWN:
        ComputeCarUpsideDownResponse(static_cast<CEventCarUpsideDown*>(e), tactive, tsimplest);
        break;
    case EVENT_POTENTIAL_WALK_INTO_FIRE:
        ComputePotentialWalkIntoFireResponse(static_cast<CEventPotentialWalkIntoFire*>(e), tactive, tsimplest);
        break;
    case EVENT_SHOT_FIRED_WHIZZED_BY:
        ComputeShotFiredWhizzedByResponse(static_cast<CEventGunShotWhizzedBy*>(e), tactive, tsimplest);
        break;
    case EVENT_LOW_ANGER_AT_PLAYER:
        ComputeLowAngerAtPlayerResponse(static_cast<CEventLowAngerAtPlayer*>(e), tactive, tsimplest);
        break;
    case EVENT_HIGH_ANGER_AT_PLAYER:
        ComputeHighAngerAtPlayerResponse(static_cast<CEventHighAngerAtPlayer*>(e), tactive, tsimplest);
        break;
    case EVENT_HEALTH_REALLY_LOW:
        ComputeReallyLowHealthResponse(static_cast<CEventHealthReallyLow*>(e), tactive, tsimplest);
        break;
    case EVENT_HEALTH_LOW:
        ComputeLowHealthResponse(static_cast<CEventHealthLow*>(e), tactive, tsimplest);
        break;
    case EVENT_POTENTIAL_WALK_INTO_VEHICLE:
        ComputeVehiclePotentialPassiveCollisionResponse(static_cast<CEventPotentialWalkIntoVehicle*>(e), tactive, tsimplest);
        break;
    case EVENT_ON_FIRE:
        ComputeOnFireResponse(static_cast<CEventOnFire*>(e), tactive, tsimplest);
        break;
    case EVENT_FIRE_NEARBY:
        ComputeFireNearbyResponse(static_cast<CEventFireNearby*>(e), tactive, tsimplest);
        break;
    case EVENT_SOUND_QUIET:
        ComputePedSoundQuietResponse(static_cast<CEventSoundQuiet*>(e), tactive, tsimplest);
        break;
    case EVENT_ACQUAINTANCE_PED_HATE_BADLY_LIT:
        ComputePedThreatBadlyLitResponse(static_cast<CEventAcquaintancePedHateBadlyLit*>(e), tactive, tsimplest);
        break;
    case EVENT_WATER_CANNON:
        ComputeWaterCannonResponse(static_cast<CEventHitByWaterCannon*>(e), tactive, tsimplest);
        break;
    case EVENT_SEEN_PANICKED_PED:
        ComputeSeenPanickedPedResponse(static_cast<CEventSeenPanickedPed*>(e), tactive, tsimplest);
        break;
    case EVENT_IN_WATER:
        ComputeInWaterResponse(static_cast<CEventInWater*>(e), tactive, tsimplest);
        break;
    case EVENT_AREA_CODES:
        ComputeAreaCodesResponse(static_cast<CEventAreaCodes*>(e), tactive, tsimplest);
        break;
    case EVENT_PLAYER_WANTED_LEVEL:
        ComputePlayerWantedLevelResponse(e, tactive, tsimplest);
        break;
    case EVENT_CREATE_PARTNER_TASK:
        ComputeCreatePartnerTaskResponse(static_cast<CEventCreatePartnerTask*>(e), tactive, tsimplest);
        break;
    case EVENT_SEEN_COP:
        ComputeSeenCopResponse(static_cast<CEventSeenCop*>(e), tactive, tsimplest);
        break;
    case EVENT_ON_ESCALATOR:
        ComputeOnEscalatorResponse(e, tactive, tsimplest);
        break;
    case EVENT_DANGER:
        ComputeDangerResponse(static_cast<CEventDanger*>(e), tactive, tsimplest);
        break;
    case EVENT_VEHICLE_ON_FIRE:
        ComputeVehicleOnFireResponse(static_cast<CEventVehicleOnFire*>(e), tactive, tsimplest);
        break;
    case EVENT_INTERIOR_USE_INFO:
        ComputeInteriorUseInfoResponse(static_cast<CEventInteriorUseInfo*>(e), CTask::DynCast<CTaskInteriorUseInfo>(tactive), tsimplest);
        break;
    case EVENT_SIGNAL_AT_PED:
        ComputeSignalAtPedResponse(static_cast<CEventSignalAtPed*>(e), tactive, tsimplest);
        break;
    case EVENT_PASS_OBJECT:
        ComputePassObjectResponse(static_cast<CEventPassObject*>(e), tactive, tsimplest);
        break;
    case EVENT_STUCK_IN_AIR:
        m_EventResponseTask = new CTaskComplexStuckInAir();
        break;
    case EVENT_DONT_JOIN_GROUP:
        ComputeDontJoinGroupResponse(static_cast<CEventDontJoinPlayerGroup*>(e), tactive, tsimplest);
        break;
    default:
        return;
    }
}

// 0x4C4220
CTask* CEventHandler::ComputeEventResponseTask(const CPed& ped, const CEvent& e) {
    CTask* task = nullptr;

    CEventHandler eh{const_cast<CPed*>(&ped)};
    eh.ComputeEventResponseTask(const_cast<CEvent*>(&e), nullptr);
    if (eh.m_EventResponseTask) {
        task = eh.m_EventResponseTask;
        eh.m_EventResponseTask = nullptr;
    }
    eh.Flush();
    return task;
}
