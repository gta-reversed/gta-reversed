/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "EventHandlerHistory.h"

class CPed;

class CTask;
class CTaskInteriorUseInfo;

class CEvent;
class CEventCarUpsideDown;
class CEventDamage;
class CEventAreaCodes;
class CEventAttractor;
class CEventBuildingCollision;
class CEventChatPartner;
class CEventCopCarBeingStolen;
class CEventCreatePartnerTask;
class CEventDanger;
class CEventDeadPed;
class CEventDeath;
class CEventDontJoinPlayerGroup;
class CEventDraggedOutCar;
class CEventFireNearby;
class CEventDamage;
class CEventGotKnockedOverByCar;
class CEventGunAimedAt;
class CEventHighAngerAtPlayer;
class CEventInWater;
class CEventInteriorUseInfo;
class CEventLowAngerAtPlayer;
class CEventHealthLow;
class CEventPotentialWalkIntoObject;
class CEventObjectCollision;
class CEventPotentialWalkIntoFire;
class CEventOnFire;
class CEventPassObject;
class CEventAcquaintancePed;
class CEventSoundQuiet;
class CEventAcquaintancePedHateBadlyLit;
class CEventAcquaintancePedHate;
class CEventPedToChase;
class CEventPedToFlee;
class CEventPlayerCollisionWithPed;
class CEventPotentialWalkIntoPed;
class CEventHealthReallyLow;
class CEventRevived;
class CEventScriptCommand;
class CEventSeenCop;
class CEventSeenPanickedPed;
class CEventSexyPed;
class CEventSexyVehicle;
class CEventGunShot;
class CEventGunShotWhizzedBy;
class CEventSignalAtPed;
class CEventSpecial;
class CEventVehicleCollision;
class CEventVehicleDamage;
class CEventVehicleDied;
class CEventVehicleHitAndRun;
class CEventVehicleOnFire;
class CEventPotentialGetRunOver;
class CEventPotentialWalkIntoVehicle;
class CEventHitByWaterCannon;
class CEventPedEnteredMyVehicle;
class CEventVehicleToSteal;

class CEventHandler {
public:
    static void InjectHooks();

    explicit CEventHandler(CPed* ped);
    ~CEventHandler() = default;

    void ResetResponse();
    void ResetHistory() { m_History.ClearAllEvents(); }
    void Flush();
    void FlushImmediately();

    eEventType GetCurrentEventType() const;
    void HandleEvents();
    bool IsKillTaskAppropriate(CPed* ped1, CPed* ped2, const CEvent& event);
    static bool IsTemporaryEvent(const CEvent& event);
    void RecordActiveEvent(const CEvent& event);
    void RecordPassiveEvent(const CEvent& event);
    static void RegisterKill(const CPed* ped, const CEntity* entity, eWeaponType weaponType, bool a4);
    void SetEventResponseTask(const CEvent& event);

    void ComputeAreaCodesResponse(CEventAreaCodes* e, CTask* tactive, CTask* tsimplest);
    void ComputeAttractorResponse(CEventAttractor* e, CTask* tactive, CTask* tsimplest);
    void ComputeBuildingCollisionPassiveResponse(CEvent* e, CTask* tactive, CTask* tsimplest);
    void ComputeBuildingCollisionResponse(CEventBuildingCollision* e, CTask* tactive, CTask* tsimplest);
    void ComputeCarUpsideDownResponse(CEventCarUpsideDown* e, CTask* tactive, CTask* tsimplest);
    void ComputeChatPartnerResponse(CEventChatPartner* e, CTask* tactive, CTask* tsimplest);
    void ComputeCopCarBeingStolenResponse(CEventCopCarBeingStolen* e, CTask* tactive, CTask* tsimplest);
    void ComputeCreatePartnerTaskResponse(CEventCreatePartnerTask* e, CTask* tactive, CTask* tsimplest);
    void ComputeDamageResponse(CEventDamage* e, CTask* tactive, CTask* tsimplest, CTask* abortedTaskEventResponse);
    void ComputeDangerResponse(CEventDanger* e, CTask* tactive, CTask* tsimplest);
    void ComputeDeadPedResponse(CEventDeadPed* e, CTask* tactive, CTask* tsimplest);
    void ComputeDeathResponse(CEventDeath* e, CTask* tactive, CTask* tsimplest);
    void ComputeDontJoinGroupResponse(CEventDontJoinPlayerGroup* e, CTask* tactive, CTask* tsimplest);
    void ComputeDraggedOutCarResponse(CEventDraggedOutCar* e, CTask* tactive, CTask* tsimplest);
    void ComputeFireNearbyResponse(CEventFireNearby* e, CTask* tactive, CTask* tsimplest);
    void ComputeGotKnockedOverByCarResponse(CEventGotKnockedOverByCar* e, CTask* tactive, CTask* tsimplest);
    void ComputeGunAimedAtResponse(CEventGunAimedAt* e, CTask* tactive, CTask* tsimplest);
    void ComputeHighAngerAtPlayerResponse(CEventHighAngerAtPlayer* e, CTask* tactive, CTask* tsimplest);
    void ComputeInWaterResponse(CEventInWater* e, CTask* tactive, CTask* tsimplest);
    void ComputeInteriorUseInfoResponse(CEventInteriorUseInfo* e, CTaskInteriorUseInfo* tactive, CTask* tsimplest);
    void ComputeKnockOffBikeResponse(CEvent* e, CTask* tactive, CTask* tsimplest);
    void ComputeLowAngerAtPlayerResponse(CEventLowAngerAtPlayer* e, CTask* tactive, CTask* tsimplest);
    void ComputeLowHealthResponse(CEventHealthLow* e, CTask* tactive, CTask* tsimplest);
    void ComputeObjectCollisionPassiveResponse(CEventPotentialWalkIntoObject* e, CTask* tactive, CTask* tsimplest);
    void ComputeObjectCollisionResponse(CEventObjectCollision* e, CTask* tactive, CTask* tsimplest);
    void ComputeOnEscalatorResponse(CEvent* e, CTask* tactive, CTask* tsimplest);
    void ComputeOnFireResponse(CEventOnFire* e, CTask* tactive, CTask* tsimplest);
    void ComputePassObjectResponse(CEventPassObject* e, CTask* tactive, CTask* tsimplest);
    void ComputePedCollisionWithPedResponse(CEvent* e, CTask* tactive, CTask* tsimplest);
    void ComputePedCollisionWithPlayerResponse(CEvent* e, CTask* tactive, CTask* tsimplest);
    void ComputePedEnteredVehicleResponse(CEventPedEnteredMyVehicle* e, CTask* tactive, CTask* tsimplest);
    void ComputePedFriendResponse(CEventAcquaintancePed* e, CTask* tactive, CTask* tsimplest);
    void ComputePedSoundQuietResponse(CEventSoundQuiet* e, CTask* tactive, CTask* tsimplest);
    void ComputePedThreatBadlyLitResponse(CEventAcquaintancePedHateBadlyLit* e, CTask* tactive, CTask* tsimplest);
    void ComputePedThreatResponse(CEventAcquaintancePedHate* e, CTask* tactive, CTask* tsimplest);
    void ComputePedToChaseResponse(CEventPedToChase* e, CTask* tactive, CTask* tsimplest);
    void ComputePedToFleeResponse(CEventPedToFlee* e, CTask* tactive, CTask* tsimplest);
    void ComputePersonalityResponseToDamage(CEventDamage* e, CPed* src);
    void ComputePlayerCollisionWithPedResponse(CEventPlayerCollisionWithPed* e, CTask* tactive, CTask* tsimplest);
    void ComputePlayerWantedLevelResponse(CEvent* e, CTask* tactive, CTask* tsimplest);
    void ComputePotentialPedCollideResponse(CEventPotentialWalkIntoPed* e, CTask* tactive, CTask* tsimplest);
    void ComputePotentialWalkIntoFireResponse(CEventPotentialWalkIntoFire* e, CTask* tactive, CTask* tsimplest);
    void ComputeReallyLowHealthResponse(CEventHealthReallyLow* e, CTask* tactive, CTask* tsimplest);
    void ComputeReviveResponse(CEventRevived* e, CTask* tactive, CTask* tsimplest);
    void ComputeScriptCommandResponse(CEventScriptCommand* e, CTask* tactive, CTask* tsimplest);
    void ComputeSeenCopResponse(CEventSeenCop* e, CTask* tactive, CTask* tsimplest);
    void ComputeSeenPanickedPedResponse(CEventSeenPanickedPed* e, CTask* tactive, CTask* tsimplest);
    void ComputeSexyPedResponse(CEventSexyPed* e, CTask* tactive, CTask* tsimplest);
    void ComputeSexyVehicleResponse(CEventSexyVehicle* e, CTask* tactive, CTask* tsimplest);
    void ComputeShotFiredResponse(CEventGunShot* e, CTask* tactive, CTask* tsimplest);
    void ComputeShotFiredWhizzedByResponse(CEventGunShotWhizzedBy* e, CTask* tactive, CTask* tsimplest);
    void ComputeSignalAtPedResponse(CEventSignalAtPed* e, CTask* tactive, CTask* tsimplest);
    void ComputeSpecialResponse(CEventSpecial* e, CTask* tactive, CTask* tsimplest);
    void ComputeVehicleCollisionResponse(CEventVehicleCollision* e, CTask* tactive, CTask* tsimplest);
    void ComputeVehicleDamageResponse(CEventVehicleDamage* e, CTask* tactive, CTask* tsimplest);
    void ComputeVehicleDiedResponse(CEventVehicleDied* e, CTask* tactive, CTask* tsimplest);
    void ComputeVehicleHitAndRunResponse(CEventVehicleHitAndRun* e, CTask* tactive, CTask* tsimplest);
    void ComputeVehicleOnFireResponse(CEventVehicleOnFire* e, CTask* tactive, CTask* tsimplest);
    void ComputeVehiclePotentialCollisionResponse(CEventPotentialGetRunOver* e, CTask* tactive, CTask* tsimplest);
    void ComputeVehiclePotentialPassiveCollisionResponse(CEventPotentialWalkIntoVehicle* e, CTask* tactive, CTask* tsimplest);
    void ComputeVehicleToStealResponse(CEventVehicleToSteal* e, CTask* tactive, CTask* tsimplest);
    void ComputeWaterCannonResponse(CEventHitByWaterCannon* e, CTask* tactive, CTask* tsimplest);

    void ComputeEventResponseTask(CEvent* event, CTask* tAbortedTaskEventResponse);
    static CTask* ComputeEventResponseTask(const CPed& ped, const CEvent& e);

    auto& GetHistory() { return m_History; }

private:
    // 0x4C3E80
    CEventHandler* Constructor(CPed * ped) {
        this->CEventHandler::CEventHandler(ped);
        return this;
    }

    // 0x4C3EC0
    CEventHandler* Destructor() {
        this->CEventHandler::~CEventHandler();
        return this;
    }

private:
    CPed*                m_Ped{};
    CEventHandlerHistory m_History{};
    CTask*               m_PhysicalResponseTask{};
    CTask*               m_EventResponseTask{};
    CTask*               m_AttackTask{};
    CTask*               m_SayTask{};
    CTask*               m_PartialAnimTask{};
};
VALIDATE_SIZE(CEventHandler, 0x34);
