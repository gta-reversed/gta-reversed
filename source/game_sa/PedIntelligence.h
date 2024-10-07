/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "eEventType.h"
#include "Enums/eMoveState.h"
#include "TaskManager.h"
#include "EventHandler.h"
#include "EventGroup.h"
#include "TaskTimer.h"
#include "EventScanner.h"
#include "PedStuckChecker.h"
#include "VehicleScanner.h"
#include "PedScanner.h"
#include "MentalHealth.h"
#include "PedScanner.h"
#include "Collision/CollisionEventScanner.h"
#include "Enums/eMoveState.h"

class CPed;
class CEntity;
class CPlayerPed;

class CTaskSimpleUseGun;
class CTaskSimpleFight;
class CTaskSimpleHoldEntity;
class CTaskSimpleThrowProjectile;
class CTaskSimpleSwim;
class CTaskSimpleDuck;
class CTaskSimpleClimb;
class CTaskSimpleJetPack;
class CTaskSimpleInAir;

class CPedIntelligence {
public:
    CPed*                  m_pPed;
    CTaskManager           m_TaskMgr;
    CEventHandler          m_eventHandler;
    CEventGroup            m_eventGroup;
    int32                  m_nDecisionMakerType;
    int32                  m_nDecisionMakerTypeInGroup;
    float                  m_fHearingRange;
    float                  m_fSeeingRange;
    uint32                 m_nDmNumPedsToScan;
    float                  m_fDmRadius;
    float                  m_FollowNodeThresholdDistance;
    char                   m_NextEventResponseSequence;
    uint8                  m_nEventId;
    uint8                  m_nEventPriority;
    char                   field_D3;
    CVehicleScanner        m_vehicleScanner;
    CPedScanner            m_pedScanner;
    CMentalState           m_mentalState;
    char                   field_188;
    CEventScanner          m_eventScanner;
    CCollisionEventScanner m_collisionScanner;
    CPedStuckChecker       m_pedStuckChecker;
    int32                  m_AnotherStaticCounter;
    int32                  m_StaticCounter;
    CVector                m_vecLastPedPosDuringDamageEntity;
    CEntity*               m_apInterestingEntities[3];

    static float& STEALTH_KILL_RANGE;
    static float& LIGHT_AI_LEVEL_MAX;
    static float& flt_8D2384;
    static float& flt_8D2388;

public:
    static void InjectHooks();

    static void* operator new(unsigned size);
    static void operator delete(void* object);

    CPedIntelligence(CPed* ped);
    ~CPedIntelligence();

    void SetPedDecisionMakerType(int32 newType);
    auto GetPedDecisionMakerType() const { return m_nDecisionMakerType; }
    void SetPedDecisionMakerTypeInGroup(int32 newType);
    void RestorePedDecisionMakerType();
    void SetHearingRange(float range);
    void SetSeeingRange(float range);
    bool IsInHearingRange(const CVector& posn);
    bool IsInSeeingRange(const CVector& posn) const;
    bool FindRespectedFriendInInformRange();
    bool IsRespondingToEvent(eEventType eventType);
    void AddTaskPhysResponse(CTask* task, int32 unUsed = 1);
    void AddTaskEventResponseTemp(CTask* task, int32 unUsed);
    void AddTaskEventResponseNonTemp(CTask* task, int32 unUsed);
    void AddTaskPrimaryMaybeInGroup(CTask* task, bool bAffectsPed);
    CTask* FindTaskByType(eTaskType taskId);
    CTaskSimpleFight* GetTaskFighting();
    CTaskSimpleUseGun* GetTaskUseGun();
    CTaskSimpleThrowProjectile* GetTaskThrow();
    CTaskSimpleHoldEntity* GetTaskHold(bool bIgnoreCheckingForSimplestActiveTask = true);
    CTaskSimpleSwim* GetTaskSwim();
    CTaskSimpleDuck* GetTaskDuck(bool bIgnoreCheckingForSimplestActiveTask = true);
    CTaskSimpleJetPack* GetTaskJetPack();
    CTaskSimpleInAir* GetTaskInAir();
    CTaskSimpleClimb* GetTaskClimb();
    CTaskSimpleDuck* GetTaskSecondaryDuck();
    bool GetUsingParachute();
    void SetTaskDuckSecondary(uint16 nLengthOfDuck);
    void ClearTaskDuckSecondary();
    void ClearTasks(bool bClearPrimaryTasks, bool bClearSecondaryTasks);
    void FlushImmediately(bool bSetPrimaryDefaultTask);
    C2dEffect* GetEffectInUse() const;
    void SetEffectInUse(C2dEffect* effect);
    void ProcessAfterProcCol();
    void ProcessAfterPreRender();
    void ProcessEventHandler();
    bool IsFriendlyWith(const CPed& ped) const;
    bool IsThreatenedBy(const CPed& ped) const;
    bool Respects(CPed* ped) const;
    bool IsInACarOrEnteringOne();
    static bool AreFriends(const CPed& ped1, const CPed& ped2);
    bool IsPedGoingSomewhereOnFoot();
    eMoveState GetMoveStateFromGoToTask();
    void FlushIntelligence();
    bool TestForStealthKill(CPed* pTarget, bool bFullTest);
    void RecordEventForScript(int32 eventId, int32 eventPriority);
    bool HasInterestingEntites();
    bool IsInterestingEntity(CEntity* entity);
    void LookAtInterestingEntities();
    void RemoveAllInterestingEntities();
    bool IsPedGoingForCarDoor();
    float CanSeeEntityWithLights(CEntity* entity, int32 unUsed);
    void ProcessStaticCounter();
    void ProcessFirst();
    void Process();
    CTask* GetActivePrimaryTask() const;
    float GetPedFOVRange() const;
    void IncrementAngerAtPlayer(uint8 anger);

    void SetDmRadius(float r) { m_fDmRadius = r; }
    void SetNumPedsToScan(uint32 n) { m_nDmNumPedsToScan = n; }

    // NOTSA
    bool IsUsingGun() {
        if (GetTaskUseGun()) {
            return true;
        }
        if (auto simplestTask = m_TaskMgr.GetSimplestActiveTask()) {
            if (simplestTask->GetTaskType() == TASK_SIMPLE_GANG_DRIVEBY) {
                return true;
            }
        }
        return false;
    }

    //! Get the vehicle the ped is entering now (If any)
    CVehicle* GetEnteringVehicle();

    CEventHandler&   GetEventHandler()    { return m_eventHandler; }
    CEventGroup&     GetEventGroup()      { return m_eventGroup; }
    CEventScanner&   GetEventScanner()    { return m_eventScanner; }
    CPedScanner&     GetPedScanner()      { return m_pedScanner; }
    CVehicleScanner& GetVehicleScanner()  { return m_vehicleScanner; }
    CEntity**        GetPedEntities()     { return m_pedScanner.m_apEntities.data(); }     // 0x4893E0
    CEntity*         GetPedEntity(uint32 index) { return GetPedEntities()[index]; } // todo: GetPedEntity or degrades readability?
    CEntity**        GetVehicleEntities() { return m_vehicleScanner.m_apEntities.data(); }

private:
    CPedIntelligence* Constructor(CPed* ped);
    CPedIntelligence* Destructor();
};

VALIDATE_SIZE(CPedIntelligence, 0x294);
VALIDATE_OFFSET(CPedIntelligence, m_AnotherStaticCounter, 0x274);
