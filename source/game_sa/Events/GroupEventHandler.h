#pragma once

class CPed;
class CEvent;
class CEventGroupEvent;
class CTaskAllocator;

class CEventVehicleDamage;
class CEventGunShot;
class CEventSexyPed;
class CEventSeenCop;
class CEventPlayerCommandToGroup;
class CEventAcquaintancePed;
class CEventNewGangMember;
class CEventLeaderExitedCarAsDriver;
class CEventLeaderEntryExit;
class CEventGunAimedAt;
class CEventPlayerCommandToGroupGather;
class CEventDraggedOutCar;
class CEventDanger;
class CEventDamage;
class CEventEditableResponse;
class CEventLeanOnVehicle;

class CGroupEventHandler {
public:
    static void InjectHooks();

    static bool IsKillTaskAppropriate(CPedGroup* pg, CPed* ped);

    static CTaskAllocator* ComputeWalkAlongsideResponse(CPedGroup* pg, CPed* ped1, CPed* ped2);
    static CTaskAllocator* ComputeStareResponse(CPedGroup* pg, CPed* stareAt, CPed* originator, int32 timeout, int32 timeoutBias);

    static CTaskAllocator* ComputeResponseVehicleDamage(const CEventVehicleDamage& event, CPedGroup* pg, CPed* originator);
    static CTaskAllocator* ComputeResponseShotFired(const CEventGunShot& event, CPedGroup* pg, CPed* originator);
    static CTaskAllocator* ComputeResponseSexyPed(const CEventSexyPed& event, CPedGroup* pg, CPed* originator);
    static CTaskAllocator* ComputeResponseSeenCop(const CEventSeenCop& event, CPedGroup* pg, CPed* originator);
    static CTaskAllocator* ComputeResponsePlayerCommand(const CEventPlayerCommandToGroup& event, CPedGroup* pg, CPed* originator);
    static CTaskAllocator* ComputeResponsePedThreat(const CEventAcquaintancePed& event, CPedGroup* pg, CPed* originator);
    static CTaskAllocator* ComputeResponsePedFriend(const CEventAcquaintancePed& e, CPedGroup* pg, CPed* originator);
    static CTaskAllocator* ComputeResponseNewGangMember(const CEventNewGangMember& e, CPedGroup* pg, CPed* originator);
    static CTaskAllocator* ComputeResponseLeaderExitedCar(const CEventEditableResponse& e, CPedGroup* pg, CPed* originator);
    static CTaskAllocator* ComputeResponseLeaderEnteredCar(const CEvent& e, CPedGroup* pg, CPed* originator);
    static CTaskAllocator* ComputeResponseLeaderEnterExit(const CEventLeaderEntryExit& e, CPedGroup* pg, CPed* originator);
    static CTaskAllocator* ComputeResponseGunAimedAt(const CEventGunAimedAt& e, CPedGroup* pg, CPed* originator);
    static CTaskAllocator* ComputeResponseGather(const CEventPlayerCommandToGroupGather& e, CPedGroup* pg, CPed* originator);
    static CTaskAllocator* ComputeResponseDraggedOutCar(const CEventDraggedOutCar& e, CPedGroup* pg, CPed* originator);
    static CTaskAllocator* ComputeResponseDanger(const CEventDanger& e, CPedGroup* pg, CPed* originator);
    static CTaskAllocator* ComputeResponseDamage(const CEventDamage& e, CPedGroup* pg, CPed* originator);
    static CTaskAllocator* ComputeResponsLeaderQuitEnteringCar(const CEvent&, CPedGroup* pg, CPed* originator);
    static CTaskAllocator* ComputeMemberResponses(const CEventEditableResponse& e, CPedGroup* pg, CPed* originator);
    static CTaskAllocator* ComputeLeanOnVehicleResponse(const CEventLeanOnVehicle& e, CPedGroup* pg, CPed* originator);

    static CTaskAllocator* ComputeKillThreatsBasicResponse(CPedGroup* pg, CPed* threat, CPed* originator, bool bDamageOriginator);
    static CTaskAllocator* ComputeKillPlayerBasicResponse(CPedGroup* pg, CPed* threat, CPed* originator, bool bDamageOriginator);
    static CTaskAllocator* ComputeHassleThreatResponse(CPedGroup* pg, CPed* threat, CPed* originator, bool bDamageOriginator);
    static CTaskAllocator* ComputeHassleSexyPedResponse(CPedGroup* pg, CPed* sexyPed, CPed* originator);
    static CTaskAllocator* ComputeHandSignalResponse(CPedGroup* pg, CPed* signalAt, CPed* originator);
    static CTaskAllocator* ComputeGreetResponse(CPedGroup* pg, CPed* toGreet, CPed* originator);
    static CTaskAllocator* ComputeDoDealResponse(CPedGroup* pg, CPed* dealWith, CPed* originator);
    static CTaskAllocator* ComputeFleePedResponse(CPedGroup* pg, CPed* threat, CPed* originator, bool bDamageOriginator);
    static CTaskAllocator* ComputeDrivebyResponse(CPedGroup* pg, CPed* threat, CPed* originator);

    static CTaskAllocator* ComputeEventResponseTasks(const CEventGroupEvent& groupEvent, CPedGroup* pg);
};

static constexpr auto fEntityPosChangeThreshold = 1.f;
