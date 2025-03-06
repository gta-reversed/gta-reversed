/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#include "StdInc.h"

#include "PedIntelligence.h"

#include "IKChainManager_c.h"
#include "PedType.h"
#include "TaskComplexBeInGroup.h"
#include "TaskSimpleCarDriveTimed.h"
#include "TaskSimpleStandStill.h"
#include "TaskComplexFacial.h"
#include "TaskComplexEnterCar.h"
#include "TaskSimpleGoTo.h"
#include "TaskComplexKillPedOnFoot.h"
#include "TaskSimplePlayerOnFoot.h"
#include "TaskComplexWander.h"
#include "TaskSimpleUseGun.h"
#include "TaskSimpleHoldEntity.h"
#include "TaskSimpleDuck.h"
#include "TaskSimpleClimb.h"
#include "CollisionEventScanner.h"
#include "PlayerRelationshipRecorder.h"
#include "TaskComplexEnterCarAsDriver.h"
#include "TaskComplexEnterCarAsPassenger.h"
#include "TaskSimpleCarDrive.h"
#include "TaskSimpleFight.h"
#include "TaskSimpleUseGun.h"
#include "TaskSimpleThrowProjectile.h"
#include "TaskSimpleJetPack.h"
#include "TaskSimpleInAir.h"
#include "TaskSimpleHoldEntity.h"
#include "TaskSimpleSwim.h"
#include "TaskSimplePutDownEntity.h"
#include <TaskComplexGoToCarDoorAndStandStill.h>
#include "TaskSimplePickUpEntity.h"
#include <extensions/enumerate.hpp>


float& CPedIntelligence::STEALTH_KILL_RANGE = *reinterpret_cast<float*>(0x8D2398); // 2.5f
float& CPedIntelligence::LIGHT_AI_LEVEL_MAX = *reinterpret_cast<float*>(0x8D2380); // 0.3f
float& CPedIntelligence::flt_8D2384 = *reinterpret_cast<float*>(0x8D2384); // 30.0f
float& CPedIntelligence::flt_8D2388 = *reinterpret_cast<float*>(0x8D2388); // 50.0f

void CPedIntelligence::InjectHooks()
{
    RH_ScopedClass(CPedIntelligence);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Constructor, 0x607140);
    RH_ScopedInstall(Destructor, 0x607300);

    RH_ScopedInstall(GetPedEntities, 0x4893E0);
    RH_ScopedInstall(SetPedDecisionMakerType, 0x600B50);
    RH_ScopedInstall(SetPedDecisionMakerTypeInGroup, 0x600BB0);
    RH_ScopedInstall(RestorePedDecisionMakerType, 0x600BC0);
    RH_ScopedInstall(SetHearingRange, 0x600BE0);
    RH_ScopedInstall(SetSeeingRange, 0x600BF0);
    RH_ScopedInstall(IsInSeeingRange, 0x600C60);
    RH_ScopedInstall(FindRespectedFriendInInformRange, 0x600CF0);
    RH_ScopedInstall(IsRespondingToEvent, 0x600DB0);
    RH_ScopedInstall(AddTaskPhysResponse, 0x600DC0);
    RH_ScopedInstall(AddTaskEventResponseTemp, 0x600DE0);
    RH_ScopedInstall(AddTaskEventResponseNonTemp, 0x600E00);
    RH_ScopedInstall(AddTaskPrimaryMaybeInGroup, 0x600E20);
    RH_ScopedInstall(FindTaskByType, 0x600EE0);
    RH_ScopedInstall(GetTaskFighting, 0x600F30);
    RH_ScopedInstall(GetTaskUseGun, 0x600F70);
    RH_ScopedInstall(GetTaskThrow, 0x600FB0);
    RH_ScopedInstall(GetTaskHold, 0x600FF0);
    RH_ScopedInstall(GetTaskSwim, 0x601070);
    RH_ScopedInstall(GetTaskDuck, 0x6010A0);
    RH_ScopedInstall(GetTaskJetPack, 0x601110);
    RH_ScopedInstall(GetTaskInAir, 0x601150);
    RH_ScopedInstall(GetTaskClimb, 0x601180);
    RH_ScopedInstall(GetUsingParachute, 0x6011B0);
    RH_ScopedInstall(SetTaskDuckSecondary, 0x601230);
    RH_ScopedInstall(ClearTaskDuckSecondary, 0x601390);
    RH_ScopedInstall(ClearTasks, 0x601420);
    RH_ScopedInstall(FlushImmediately, 0x601640);
    RH_ScopedInstall(GetEffectInUse, 0x6018D0);
    RH_ScopedInstall(SetEffectInUse, 0x6018E0);
    RH_ScopedInstall(ProcessAfterProcCol, 0x6018F0);
    RH_ScopedInstall(ProcessAfterPreRender, 0x6019B0);
    RH_ScopedInstall(ProcessEventHandler, 0x601BB0);
    RH_ScopedInstall(IsFriendlyWith, 0x601BC0);
    RH_ScopedInstall(IsThreatenedBy, 0x601C30);
    RH_ScopedInstall(Respects, 0x601C90);
    RH_ScopedInstall(IsInACarOrEnteringOne, 0x601CC0);
    RH_ScopedInstall(AreFriends, 0x601D10);
    RH_ScopedInstall(GetMoveStateFromGoToTask, 0x601D70);
    RH_ScopedInstall(FlushIntelligence, 0x601DA0);
    RH_ScopedInstall(TestForStealthKill, 0x601E00);
    RH_ScopedInstall(RecordEventForScript, 0x602050);
    RH_ScopedInstall(IsInterestingEntity, 0x6020A0);
    RH_ScopedInstall(LookAtInterestingEntities, 0x6020D0);
    RH_ScopedInstall(IsPedGoingForCarDoor, 0x602350);
    RH_ScopedInstall(CanSeeEntityWithLights, 0x605550);
    RH_ScopedInstall(ProcessStaticCounter, 0x605650);
    RH_ScopedInstall(ProcessFirst, 0x6073A0);
    RH_ScopedInstall(Process, 0x608260);
    RH_ScopedInstall(GetActivePrimaryTask, 0x4B85B0);
}

// 0x607140
CPedIntelligence::CPedIntelligence(CPed* ped) :
    m_pPed{ ped },
    m_TaskMgr{ CTaskManager(ped) },
    m_eventHandler{ CEventHandler(ped) },
    m_eventGroup{ CEventGroup(ped) }
{
    m_nDecisionMakerType                  = DM_EVENT_UNDEFINED;
    m_nDecisionMakerTypeInGroup           = -1;
    m_fHearingRange                       = 15.0f;
    m_fSeeingRange                        = 15.0f;
    m_nDmNumPedsToScan                    = 3;
    m_fDmRadius                           = 15.0f;
    m_FollowNodeThresholdDistance                              = 30.0f;
    m_NextEventResponseSequence                              = -1;
    m_nEventId                            = 0;
    m_nEventPriority                      = 0;
    field_188                             = 0;
    m_collisionScanner.m_bAlreadyHitByCar = false;
    m_AnotherStaticCounter                = 0;
    m_StaticCounter                       = 0;
    if (IsPedTypeGang(ped->m_nPedType)) {
        m_fSeeingRange = 40.f;
        m_fHearingRange = 40.f;
    }
    std::ranges::fill(m_apInterestingEntities, nullptr);
}

// 0x607300
CPedIntelligence::~CPedIntelligence() {
    GetPlayerRelationshipRecorder().ClearRelationshipWithPlayer(m_pPed);
}

// 0x600B50
void CPedIntelligence::SetPedDecisionMakerType(int32 newType) {
    int32 oldType = m_nDecisionMakerType;
    if (oldType)
    {
        if (!newType)
        {
            m_nDecisionMakerTypeInGroup = oldType;
        }
        m_nDecisionMakerType = newType;
    }
    else
    {
        m_nDecisionMakerTypeInGroup = newType;
    }
    if (m_nDecisionMakerType == DM_EVENT_PED_ENTERED_MY_VEHICLE)
    {
        m_fDmRadius = 5.0f;
        m_nDmNumPedsToScan = 15;
    }
}

// 0x600BB0
void CPedIntelligence::SetPedDecisionMakerTypeInGroup(int32 newType) {
    m_nDecisionMakerTypeInGroup = newType;
}

// 0x600BC0
void CPedIntelligence::RestorePedDecisionMakerType() {
    if (!m_nDecisionMakerType) { // todo: DM_EVENT_DRAGGED_OUT_CAR
        m_nDecisionMakerType = m_nDecisionMakerTypeInGroup;
    }
}

// 0x600BE0
void CPedIntelligence::SetHearingRange(float range) {
    m_fHearingRange = range;
}

// 0x600BF0
void CPedIntelligence::SetSeeingRange(float range) {
    m_fSeeingRange = range;
}

// Unused
// 0x600C00
bool CPedIntelligence::IsInHearingRange(const CVector& posn) {
    return plugin::CallMethodAndReturn<bool, 0x600C00, CPedIntelligence*, const CVector&>(this, posn);
}

// 0x600C60
bool CPedIntelligence::IsInSeeingRange(const CVector& posn) const {
    CVector distance = posn - m_pPed->GetPosition();
    if (m_fSeeingRange * m_fSeeingRange > distance.SquaredMagnitude()) {
        if (DotProduct(distance, m_pPed->GetForward()) > 0.0f)
            return true;
    }
    return false;
}

// 0x600CF0
bool CPedIntelligence::FindRespectedFriendInInformRange() {
    auto respect = m_pPed->GetAcquaintance().GetAcquaintances(ACQUAINTANCE_RESPECT);
    for (uint32 pedScanIndex = 0; pedScanIndex < m_nDmNumPedsToScan; pedScanIndex++) {
        CPed* pedEntity = GetPedEntities()[pedScanIndex]->AsPed();
        if (pedEntity) {
            if (CPedType::GetPedFlag(pedEntity->m_nPedType) & respect) {
                CVector distance = m_pPed->GetPosition() - pedEntity->GetPosition();
                if ((m_fDmRadius * m_fDmRadius) > distance.SquaredMagnitude())
                    return true;
            }
        }
    }
    return false;
}

// 0x600DB0
bool CPedIntelligence::IsRespondingToEvent(eEventType eventType) {
    return m_eventHandler.GetHistory().IsRespondingToEvent(eventType);
}

// (CTask *task, bool arg2)
// 0x600DC0
void CPedIntelligence::AddTaskPhysResponse(CTask* task, int32 unUsed) {
    m_TaskMgr.SetTask(task, TASK_PRIMARY_PHYSICAL_RESPONSE, unUsed);
}

// (CTask *task, bool arg2)
// 0x600DE0
void CPedIntelligence::AddTaskEventResponseTemp(CTask* task, int32 unUsed) {
    m_TaskMgr.SetTask(task, TASK_PRIMARY_EVENT_RESPONSE_TEMP, unUsed);
}

// (CTask *task,bool arg2)
// 0x600E00
void CPedIntelligence::AddTaskEventResponseNonTemp(CTask* task, int32 unUsed) {
    m_TaskMgr.SetTask(task, TASK_PRIMARY_EVENT_RESPONSE_NONTEMP, unUsed);
}

// 0x600E20
void CPedIntelligence::AddTaskPrimaryMaybeInGroup(CTask* task, bool bAffectsPed) {
    CPedGroup* pegGroup = CPedGroups::GetPedsGroup(m_pPed);
    if (m_pPed->IsPlayer() || !pegGroup)
    {
        CEventScriptCommand eventScriptCommand(TASK_PRIMARY_PRIMARY, task, bAffectsPed);
        m_eventGroup.Add(&eventScriptCommand, false);
    }
    else
    {
        pegGroup->GetIntelligence().SetScriptCommandTask(m_pPed, task);
        delete task;
    }
}

// 0x600EE0
CTask* CPedIntelligence::FindTaskByType(eTaskType type) {
    for (const auto idx : { TASK_PRIMARY_DEFAULT, TASK_PRIMARY_PRIMARY, TASK_PRIMARY_EVENT_RESPONSE_TEMP, TASK_PRIMARY_EVENT_RESPONSE_NONTEMP }) {
        if (const auto task = m_TaskMgr.FindTaskByType(idx, type)) {
            return task;
        }
    }
    return nullptr;
}

// 0x600F30
CTaskSimpleFight* CPedIntelligence::GetTaskFighting() {
    return CTask::DynCast<CTaskSimpleFight>(m_TaskMgr.GetTaskSecondary(TASK_SECONDARY_ATTACK));
}

// 0x600F70
CTaskSimpleUseGun* CPedIntelligence::GetTaskUseGun() {
    return CTask::DynCast<CTaskSimpleUseGun>(m_TaskMgr.GetTaskSecondary(TASK_SECONDARY_ATTACK));
}

// 0x600FB0
CTaskSimpleThrowProjectile* CPedIntelligence::GetTaskThrow() {
    return CTask::DynCast<CTaskSimpleThrowProjectile>(m_TaskMgr.GetTaskSecondary(TASK_SECONDARY_ATTACK));
}

// 0x600FF0
CTaskSimpleHoldEntity* CPedIntelligence::GetTaskHold(bool bIgnoreCheckingForSimplestActiveTask) {
    if (const auto task = CTask::DynCast<CTaskSimpleHoldEntity>(m_TaskMgr.GetTaskSecondary(TASK_SECONDARY_PARTIAL_ANIM))) {
        return task;
    }

    if (!bIgnoreCheckingForSimplestActiveTask) {
        if (const auto task = m_TaskMgr.GetSimplestActiveTask()) {
            if (notsa::isa<CTaskSimplePickUpEntity, CTaskSimplePutDownEntity>(task)) {
                return static_cast<CTaskSimpleHoldEntity*>(task);
            }
        }
    }

    return nullptr;
}

// 0x601070
CTaskSimpleSwim* CPedIntelligence::GetTaskSwim() {
    return CTask::DynCast<CTaskSimpleSwim>(m_TaskMgr.GetSimplestActiveTask());
}

// 0x6010A0
CTaskSimpleDuck* CPedIntelligence::GetTaskDuck(bool bIgnoreCheckingForSimplestActiveTask) {
    if (const auto task = CTask::DynCast<CTaskSimpleDuck>(m_TaskMgr.GetTaskSecondary(TASK_SECONDARY_DUCK))) {
        return task;
    }

    auto* secondaryTask = m_TaskMgr.GetTaskSecondary(TASK_SECONDARY_DUCK);
    if (secondaryTask && secondaryTask->GetTaskType() == TASK_SIMPLE_DUCK) {
        return (CTaskSimpleDuck*)secondaryTask;
    }

    if (!bIgnoreCheckingForSimplestActiveTask) {
        if (const auto task = CTask::DynCast<CTaskSimpleDuck>(m_TaskMgr.GetSimplestActiveTask())) {
            return task;
        }
    }

    return nullptr;
}

// 0x601110
CTaskSimpleJetPack* CPedIntelligence::GetTaskJetPack() {
    if (m_pPed->IsPlayer()) {
        return CTask::DynCast<CTaskSimpleJetPack>(m_TaskMgr.GetSimplestActiveTask());
    }
    return nullptr;
}

// 0x601150
CTaskSimpleInAir* CPedIntelligence::GetTaskInAir() {
    return CTask::DynCast<CTaskSimpleInAir>(m_TaskMgr.GetSimplestActiveTask());
}

// 0x601180
CTaskSimpleClimb* CPedIntelligence::GetTaskClimb() {
    return CTask::DynCast<CTaskSimpleClimb>(m_TaskMgr.GetSimplestActiveTask());
}

// @sa [@addr unk]
CTaskSimpleDuck* CPedIntelligence::GetTaskSecondaryDuck() {
    return CTask::DynCast<CTaskSimpleDuck>(m_TaskMgr.GetTaskSecondary(TASK_SECONDARY_DUCK));
}

// 0x6011B0
bool CPedIntelligence::GetUsingParachute() {
    CWeapon* activeWeapon = &m_pPed->GetActiveWeapon();
    if (activeWeapon->m_Type != WEAPON_PARACHUTE) {
        return false;
    }

    if (m_pPed->bIsStanding) {
        return false;
    }

    if (m_pPed->physicalFlags.bSubmergedInWater) {
        return false;
    }

    auto animAssoc = RpAnimBlendClumpGetFirstAssociation(m_pPed->m_pRwClump, ANIMATION_IS_PARTIAL);
    if (!animAssoc) {
        return false;
    }

    int32 blockID = animAssoc->m_BlendHier->m_nAnimBlockId;
    if (_stricmp(CAnimManager::GetAnimBlocks()[blockID].Name, "parachute") != 0) {
        while (true) {
            animAssoc = RpAnimBlendGetNextAssociation(animAssoc);
            if (!animAssoc) {
                return false;
            }
        }
    }
    return true;
}

// 0x601230
void CPedIntelligence::SetTaskDuckSecondary(uint16 nLengthOfDuck) {
    if (const auto duck = GetTaskDuck()) {
        if (duck->m_DuckControlType == DUCK_SCRIPT_CONTROLLED) {
            return;
        }
    }

    m_TaskMgr.SetTaskSecondary(new CTaskSimpleDuck(DUCK_TASK_CONTROLLED, nLengthOfDuck), TASK_SECONDARY_DUCK);

    if (const auto useGun = GetTaskUseGun()) {
        useGun->ClearAnim(m_pPed);
    }

    CTask::Cast<CTaskSimpleDuck>(m_TaskMgr.GetTaskSecondary(TASK_SECONDARY_DUCK))->ProcessPed(m_pPed);
}

// 0x601390
void CPedIntelligence::ClearTaskDuckSecondary() {
    CTaskManager* taskManager = &m_TaskMgr;
    CTask* secondaryDuck = taskManager->GetTaskSecondary(TASK_SECONDARY_DUCK);
    if (!secondaryDuck)
        return;

    secondaryDuck->MakeAbortable(m_pPed, ABORT_PRIORITY_LEISURE, nullptr);
    CPlayerPedData* playerData = m_pPed->m_pPlayerData;
    if (playerData) {
        playerData->m_fMoveBlendRatio = 0.0f;
    } else {
        m_pPed->SetMoveState(PEDMOVE_STILL);
    }

    if (CTask* task = taskManager->GetTaskSecondary(TASK_SECONDARY_ATTACK)) {
        if (task->GetTaskType() == TASK_SIMPLE_USE_GUN) {
            if (taskManager->GetTaskSecondary(TASK_SECONDARY_ATTACK)) {
                CTaskSimpleUseGun* taskUseGun = GetTaskUseGun();
                taskUseGun->ClearAnim(m_pPed);
            }
        }
    }
}

// 0x601420
void CPedIntelligence::ClearTasks(bool bClearPrimaryTasks, bool bClearSecondaryTasks) {
    if (bClearPrimaryTasks)
    {
        if (m_pPed->bInVehicle && m_pPed->m_pVehicle)
        {
            if (!m_eventGroup.HasScriptCommandOfTaskType(TASK_SIMPLE_CAR_DRIVE))
            {
                CTask* driveTask = nullptr;
                if (m_TaskMgr.GetTaskPrimary(TASK_PRIMARY_DEFAULT)->GetTaskType() == TASK_SIMPLE_CAR_DRIVE)
                {
                    driveTask = static_cast<CTask*>(new CTaskSimpleCarDriveTimed(m_pPed->m_pVehicle, 0));
                }
                else
                {
                    driveTask = new CTaskSimpleCarDrive(m_pPed->m_pVehicle, nullptr, false);
                }
                CEventScriptCommand eventScriptCommand(TASK_PRIMARY_PRIMARY, driveTask, false);
                m_eventGroup.Add(&eventScriptCommand, false);
            }
        }
        else if (!m_eventGroup.HasScriptCommandOfTaskType(TASK_SIMPLE_STAND_STILL))
        {
            auto taskSimpleStandStill = new CTaskSimpleStandStill(0, false, false, 8.0f);
            CEventScriptCommand eventScriptCommand(TASK_PRIMARY_PRIMARY, taskSimpleStandStill, false);
            m_eventGroup.Add(&eventScriptCommand, false);
        }

        m_eventHandler.HandleEvents();
        m_TaskMgr.ManageTasks();
        CPedScriptedTaskRecord::Process();
    }

    if (!bClearSecondaryTasks)
        return;

    for (const auto [idx, task] : notsa::enumerate(m_TaskMgr.GetSecondaryTasks())) {
        if (!task) {
            continue;
        }

        if ((eSecondaryTask)idx == TASK_SECONDARY_FACIAL_COMPLEX) {
            continue;
        }

        if (task->MakeAbortable(m_pPed)) {
            m_TaskMgr.SetTaskSecondary(nullptr, (eSecondaryTask)idx);
        } else {
            task->MakeAbortable(m_pPed, ABORT_PRIORITY_LEISURE, nullptr);
        }
    }
}

// 0x601640
void CPedIntelligence::FlushImmediately(bool bSetPrimaryDefaultTask) {
    const auto tComplexBeInGroupClone = CTask::CloneIfIs<CTaskComplexBeInGroup>(m_TaskMgr.GetTaskPrimary(TASK_PRIMARY_PRIMARY));

    // Save hold task
    int32 objectType = -1;
    bool bIsEntityVisible = false;
    CObject* objectToHold = nullptr;
    CTaskSimpleHoldEntity* taskSimpleHoldEntityCloned = nullptr;
    if (const auto tSimpleHoldEntity = CTask::DynCast<CTaskSimpleHoldEntity>(m_TaskMgr.GetTaskSecondary(TASK_SECONDARY_PARTIAL_ANIM))) {
        objectToHold = (CObject*)tSimpleHoldEntity->m_pEntityToHold;
        if (objectToHold) {
            if (objectToHold->IsObject()) {
                objectType = objectToHold->m_nObjectType;
                bIsEntityVisible = objectToHold->m_bIsVisible;
            }
            taskSimpleHoldEntityCloned = (CTaskSimpleHoldEntity*)tSimpleHoldEntity->Clone();
        }
    }

    // Save facial task
    const auto tComplexFacialCloned = CTask::CloneIfIs<CTaskComplexFacial>(m_TaskMgr.GetTaskSecondary(TASK_SECONDARY_FACIAL_COMPLEX));

    // Now actually flush the subsystems
    m_eventGroup.Flush(true);
    m_eventHandler.FlushImmediately();
    m_TaskMgr.FlushImmediately();
    CPedScriptedTaskRecord::Process();

    // Restore group primary task
    if (tComplexBeInGroupClone) {
        auto pedGroup = CPedGroups::GetPedsGroup(m_pPed);
        if (!pedGroup || m_pPed->IsPlayer()) {
            delete tComplexBeInGroupClone;
        } else {
            pedGroup->GetIntelligence().ComputeDefaultTasks(m_pPed);
            m_TaskMgr.SetTask(tComplexBeInGroupClone, TASK_PRIMARY_PRIMARY, false);
        }
    }

    // Restore hold task
    if (taskSimpleHoldEntityCloned) {
        if (objectType != -1) {
            objectToHold->m_nObjectType = objectType;
            objectToHold->m_bIsVisible  = bIsEntityVisible;
        }
        m_TaskMgr.SetTaskSecondary(taskSimpleHoldEntityCloned, TASK_SECONDARY_PARTIAL_ANIM);
        taskSimpleHoldEntityCloned->ProcessPed(m_pPed);
    }

    // Restore facial
    if (tComplexFacialCloned) {
        m_TaskMgr.SetTaskSecondary(tComplexFacialCloned, TASK_SECONDARY_FACIAL_COMPLEX);
    }

    // And set a default primary task if necessary
    if (bSetPrimaryDefaultTask) {
        m_TaskMgr.SetTask(
            [this]() -> CTask* {
                if (m_pPed->IsPlayer()) {
                    return new CTaskSimplePlayerOnFoot();
                } else if (m_pPed->m_nCreatedBy != PED_MISSION) {
                    return CTaskComplexWander::GetWanderTaskByPedType(m_pPed);
                } else {
                    return new CTaskSimpleStandStill(0, true, false, 8.0f);
                }
            }(),
            TASK_PRIMARY_DEFAULT,
            false
        );
    }
}

// 0x6018D0
C2dEffect* CPedIntelligence::GetEffectInUse() const {
    return m_eventScanner.m_attractorScanner.m_pEffectInUse;
}

// 0x6018E0
void CPedIntelligence::SetEffectInUse(C2dEffect* effect) {
    m_eventScanner.m_attractorScanner.m_pEffectInUse = effect;
}

// 0x6018F0
void CPedIntelligence::ProcessAfterProcCol() {
    g_LoadMonitor.StartTimer(0);

    auto* activeSimplestTask = m_TaskMgr.GetSimplestActiveTask();
    if (activeSimplestTask && activeSimplestTask->IsSimple()) {
        bool bPositionSet = static_cast<CTaskSimple*>(activeSimplestTask)->SetPedPosition(m_pPed);
        if (!bPositionSet) {
            auto* simplestTask = m_TaskMgr.GetLastTaskOf(TASK_PRIMARY_DEFAULT);
            if (simplestTask && simplestTask->IsSimple()) {
                bPositionSet = simplestTask->AsSimple()->SetPedPosition(m_pPed);
            }
        }

        if (bPositionSet) {
            m_pPed->UpdateRW();
            m_pPed->UpdateRwFrame();
        }
    }

    m_pPed->bCalledPreRender = 0;

    g_LoadMonitor.EndTimer(0);
}

// 0x6019B0
void CPedIntelligence::ProcessAfterPreRender() {
    g_LoadMonitor.StartTimer(0);

    CTask* secondaryTask = m_TaskMgr.GetTaskSecondary(TASK_SECONDARY_PARTIAL_ANIM);
    if (secondaryTask && secondaryTask->IsSimple())
    {
        if (static_cast<CTaskSimple*>(secondaryTask)->SetPedPosition(m_pPed))
        {
            CTask* secondaryAttackTask = m_TaskMgr.GetTaskSecondary(TASK_SECONDARY_ATTACK);
            if (secondaryAttackTask && secondaryAttackTask->GetTaskType() == TASK_SIMPLE_HOLD_ENTITY)
            {
                static_cast<CTaskSimpleUseGun*>(secondaryAttackTask)->SetPedPosition(m_pPed);
            }
        }
    }

    CTask* secondaryAttackTask = m_TaskMgr.GetTaskSecondary(TASK_SECONDARY_ATTACK);
    if (secondaryAttackTask && secondaryAttackTask->GetTaskType() == TASK_SIMPLE_USE_GUN)
    {
        static_cast<CTaskSimpleUseGun*>(secondaryAttackTask)->SetPedPosition(m_pPed);
    }

    CWeapon* activeWeapon = &m_pPed->GetActiveWeapon();
    if (activeWeapon->m_Type == WEAPON_MOLOTOV && activeWeapon->m_FxSystem)
    {
        RpHAnimHierarchy* animHierarchy = GetAnimHierarchyFromSkinClump(m_pPed->m_pRwClump);
        int32 animIDIndex = RpHAnimIDGetIndex(animHierarchy, 24); // 24 = BONE_R_HAND? - "BONE_R" xDDD
        RwMatrix* matrixArray = RpHAnimHierarchyGetMatrixArray(animHierarchy);

        RwV3d pointIn = { 0.05f, 0.05f,  0.14f };
        RwV3d pointOut;
        RwV3dTransformPoint(&pointOut, &pointIn, &matrixArray[animIDIndex]);

        RwMatrix matrix;
        memcpy(&matrix, m_pPed->GetModellingMatrix(), sizeof(matrix));
        matrix.pos = pointOut;
        RwMatrixUpdate(&matrix);
        activeWeapon->m_FxSystem->SetMatrix(&matrix);
    }

    if (m_pPed->bInVehicle)
    {
        CVehicle* vehicle = m_pPed->m_pVehicle;
        if (vehicle && vehicle->IsBike()) {
            vehicle->AsBike()->FixHandsToBars(m_pPed);
        }
    }

    g_LoadMonitor.EndTimer(0);
}

// 0x601BB0
void CPedIntelligence::ProcessEventHandler() {
    m_eventHandler.HandleEvents();
}

// 0x601BC0
bool CPedIntelligence::IsFriendlyWith(const CPed& ped) const {
    auto respect = m_pPed->GetAcquaintance().GetRespect();
    auto like    = m_pPed->GetAcquaintance().GetLike();
    auto pedFlag = CPedType::GetPedFlag(ped.m_nPedType);
    return m_pPed->m_nPedType == ped.m_nPedType || pedFlag & respect || pedFlag & like;
}

// 0x601C30
bool CPedIntelligence::IsThreatenedBy(const CPed& ped) const {
    auto dislike     = m_pPed->GetAcquaintance().GetDislike();
    auto hate        = m_pPed->GetAcquaintance().GetHate();
    auto pedTypeFlag = CPedType::GetPedFlag(ped.m_nPedType);
    return pedTypeFlag & dislike || pedTypeFlag & hate;
}

// 0x601C90
bool CPedIntelligence::Respects(CPed* ped) const {
    auto respect = m_pPed->GetAcquaintance().GetRespect();
    auto pedFlag = CPedType::GetPedFlag(ped->m_nPedType);
    return pedFlag & respect;
}

// 0x601CC0
bool CPedIntelligence::IsInACarOrEnteringOne() {
    if (const auto task = m_TaskMgr.Find<CTaskComplexEnterCarAsDriver>()) {
        return !!task->GetTargetCar();
    }

    if (const auto task = m_TaskMgr.Find<CTaskComplexEnterCarAsPassenger>()) {
        return !!task->GetTargetCar();
    }

    if (const auto task = m_TaskMgr.Find<CTaskSimpleCarDrive>()) {
        return !!task->GetVehicle();
    }

    return false;
}

// 0x601D10
bool CPedIntelligence::AreFriends(const CPed& ped1, const CPed& ped2) {
    return ped1.GetIntelligence()->IsFriendlyWith(ped2) || ped2.GetIntelligence()->IsFriendlyWith(ped1);
}

// unused
// 0x601D50
bool CPedIntelligence::IsPedGoingSomewhereOnFoot() {
    const auto tSimplestActive = m_TaskMgr.GetSimplestActiveTask();
    return tSimplestActive && CTask::IsGoToTask(tSimplestActive);
}

// 0x601D70 
eMoveState CPedIntelligence::GetMoveStateFromGoToTask() {
    const auto tSimplestActive = m_TaskMgr.GetSimplestActiveTask();
    return tSimplestActive && CTask::IsGoToTask(tSimplestActive)
        ? static_cast<CTaskSimpleGoTo*>(tSimplestActive)->m_moveState
        : PEDMOVE_STILL;
}

// 0x601DA0
void CPedIntelligence::FlushIntelligence() {
    m_TaskMgr.Flush();
    m_eventHandler.ResetHistory();
    m_eventHandler.ResetResponse();
    m_eventGroup.Flush(false);
    m_vehicleScanner.Clear();
    m_pedScanner.Clear();
    m_eventScanner.m_attractorScanner.Clear();
}

// 0x601E00
bool CPedIntelligence::TestForStealthKill(CPed* target, bool bFullTest) {
    if (target->bInVehicle)
        return false;

    if (target->bIsDucking || target->m_fHealth < 1.0f)
        return false;

    if (target->GetBonePosition(BONE_HEAD).z < target->GetPosition().z)
        return false;

    if (bFullTest)
        return true;

    if (target->m_nMoveState >= PEDMOVE_RUN)
        return false;

    CVector distance = target->GetPosition() - m_pPed->GetPosition();
    if (STEALTH_KILL_RANGE * STEALTH_KILL_RANGE < distance.SquaredMagnitude())
        return false;

    if (DotProduct(distance, target->GetForward()) <= 0.0f)
        return false;

    if (const auto tKillPedOnFoot = CTask::DynCast<CTaskComplexKillPedOnFoot>(target->GetTaskManager().GetActiveTask())) {
        if (tKillPedOnFoot->m_target == m_pPed) {
            return false;
        }
    }

    CEvent* currentEvent = target->GetEventHandlerHistory().GetCurrentEvent();
    if (currentEvent && currentEvent->GetSourceEntity() == m_pPed)
    {
        // todo: IsThreatenedBy ???
        auto hate = target->GetAcquaintance().GetAcquaintances(ACQUAINTANCE_HATE);
        auto dislike = target->GetAcquaintance().GetAcquaintances(ACQUAINTANCE_DISLIKE);
        auto pedFlag = CPedType::GetPedFlag(m_pPed->m_nPedType);
        bool bAcquaintancesFlagSet = (
            (hate && (pedFlag & hate)) ||
            (dislike && (pedFlag & dislike))
        );
        if (bAcquaintancesFlagSet && target->GetGroup()) {
            const auto oe = target->GetGroup()->GetIntelligence().GetOldEvent();
            if (oe && oe->GetSourceEntity() == m_pPed && bAcquaintancesFlagSet) {
                return false;
            }
        }
    }
    return true;
}

// 0x602050
void CPedIntelligence::RecordEventForScript(int32 eventId, int32 eventPriority) {
    if (eventId != EVENT_SCRIPT_COMMAND && (!eventId || eventPriority > m_nEventPriority)) {
        m_nEventId = eventId;
        m_nEventPriority = eventPriority;
    }
}

// Unused
// typo: Entities
// 0x602080
bool CPedIntelligence::HasInterestingEntites() {
    return plugin::CallMethodAndReturn<bool, 0x602080, CPedIntelligence*>(this);
}

// 0x6020A0
bool CPedIntelligence::IsInterestingEntity(CEntity* entity) {
    for (CEntity* interestingEntity : m_apInterestingEntities) {
        if (interestingEntity == entity) {
            return true;
        }
    }
    return false;
}

// 0x6020D0
void CPedIntelligence::LookAtInterestingEntities() {
    if (m_pPed->bDontAcceptIKLookAts != false)
        return;

    bool bInterestingEntityExists = false;
    for (CEntity* interestingEntity : m_apInterestingEntities) {
        if (interestingEntity) {
            bInterestingEntityExists = true;
            break;
        }
    }

    if (!bInterestingEntityExists)
        return;

    if (g_ikChainMan.IsLooking(m_pPed) || !m_pPed->GetIsOnScreen() || CGeneral::GetRandomNumberInRange(0, 100) != 50)
        return;

    CEntity* outEntities[1024];
    int16 outCount = -1;
    CWorld::FindObjectsInRange(m_pPed->GetPosition(), 15.0f, false, &outCount, 1024, outEntities, false, true, true, true, false);
    if (outCount <= 0)
        return;

    int32 interestingEntityCount = 0;
    for (int32 i = 0; i < outCount; i++) {
        CEntity* entity = outEntities[i];
        if (IsInterestingEntity(entity)) {
            outEntities[interestingEntityCount] = entity;
            interestingEntityCount++;
        }
    }

    if (!interestingEntityCount)
        return;

    const int32 randomInterestingEntityIndex = CGeneral::GetRandomNumberInRange(0, interestingEntityCount);
    const int32 randomTime = CGeneral::GetRandomNumberInRange(3000, 5000);
    CPed* interestingEntity1 = (CPed*)outEntities[randomInterestingEntityIndex];

    CVector position = { 0.0f, 0.0f, 0.0f };
    g_ikChainMan.LookAt(
        "InterestingEntities",
        m_pPed,
        interestingEntity1,
        randomTime,
        BONE_UNKNOWN,
        &position,
        false,
        0.25f,
        500,
        3,
        false
    );
}

// unused
// 0x602320
void CPedIntelligence::RemoveAllInterestingEntities() {
    plugin::CallMethod<0x602320, CPedIntelligence*>(this);
}

// 0x602350
bool CPedIntelligence::IsPedGoingForCarDoor() {
    auto* task = m_TaskMgr.GetSimplestActiveTask();
    for (auto i = 0; task && i < 3; i++, task = task->GetParent()) {
        if (notsa::isa<CTaskComplexGoToCarDoorAndStandStill>(task)) {
            return true;
        }
    }
    return false;
}

// should be (const CEntity* entity, bool unused)
// 0x605550
float CPedIntelligence::CanSeeEntityWithLights(CEntity* entity, int32 unUsed) {
    if (!entity->IsPed())
        return LIGHT_AI_LEVEL_MAX;

    CPed* ped = entity->AsPed();
    if (!ped->IsPlayer())
        return LIGHT_AI_LEVEL_MAX;

    float fLightingTotal = ped->GetLightingTotal();
    if (fLightingTotal > LIGHT_AI_LEVEL_MAX)
        return LIGHT_AI_LEVEL_MAX;

    const float fLightLevelSquared = LIGHT_AI_LEVEL_MAX * LIGHT_AI_LEVEL_MAX;
    float fMagnitude = (ped->GetPosition() - m_pPed->GetPosition()).Magnitude() - 0.7f;
    float result = fLightingTotal * fLightingTotal - fMagnitude / 30.0f * fLightLevelSquared;
    if (result <= 0.0f) {
        result = result * result - fMagnitude / 50.0f * fLightLevelSquared;
        if (result <= 0.0f)
            result = 0.0f;
        else
            result *= -1.0f;
    }
    return result;
}

// 0x605650
void CPedIntelligence::ProcessStaticCounter() {
    CTask* task = m_TaskMgr.GetSimplestActiveTask();
    if (!task || !CTask::IsGoToTask(task)) {
        if (m_StaticCounter == -2) {
            m_StaticCounter = 4;
        } else {
            m_StaticCounter++;
        }
        m_AnotherStaticCounter = 0;
        return;
    }

    int32 moveState = static_cast<CTaskSimpleGoTo*>(task)->m_moveState;
    if (moveState != PEDMOVE_WALK && moveState != PEDMOVE_RUN && moveState != PEDMOVE_SPRINT) {
        if (m_StaticCounter == -2) {
            m_StaticCounter = 4;
        } else {
            m_StaticCounter++;
        }
        m_AnotherStaticCounter = 0;
        return;
    }

    if (m_pPed->m_pDamageEntity) {
        if (m_StaticCounter > 4) {
            m_vecLastPedPosDuringDamageEntity = m_pPed->GetPosition();
        }
        m_StaticCounter = 0;
    } else {
        if (m_StaticCounter == -2) {
            m_StaticCounter = 4;
        } else {
            m_StaticCounter++;
        }
    }

    if (m_StaticCounter > 4) {
        m_AnotherStaticCounter = 0;
        return;
    }

    if ((m_pPed->GetPosition() - m_vecLastPedPosDuringDamageEntity).SquaredMagnitude() < 1.0f / 16.0f) {
        m_AnotherStaticCounter++;
    } else {
        m_AnotherStaticCounter = 0;
        m_vecLastPedPosDuringDamageEntity = m_pPed->GetPosition();
    }
}

// 0x6073A0
void CPedIntelligence::ProcessFirst() {
    g_LoadMonitor.StartTimer(0);

    ProcessStaticCounter();
    if (!m_pedStuckChecker.TestPedStuck(m_pPed, &m_eventGroup))
        m_collisionScanner.ScanForCollisionEvents(m_pPed, &m_eventGroup);

    if (m_pPed->m_fDamageIntensity > 0.0f)
    {
        CEntity* damageEntity = m_pPed->m_pDamageEntity;
        if (damageEntity && !damageEntity->IsPed()) {
            if (DotProduct(m_pPed->m_vecLastCollisionImpactVelocity, m_pPed->GetForward()) < -0.5f)
                m_pPed->bPedHitWallLastFrame = true;
        }
    }

    if (m_pPed->bInVehicle)
    {
        CVehicle* vehicle = m_pPed->m_pVehicle;
        if (vehicle && vehicle->IsBike()) {
            auto* bike = vehicle->AsBike();
            bike->m_nFixLeftHand = false;
            bike->m_nFixRightHand = false;
        }
    }
    m_pPed->bMoveAnimSpeedHasBeenSetByTask = false;

    g_LoadMonitor.EndTimer(0);
}

// 0x608260
void CPedIntelligence::Process() {
    g_LoadMonitor.StartTimer(0);

    m_vehicleScanner.ScanForVehiclesInRange(*m_pPed);
    m_pedScanner.ScanForPedsInRange(*m_pPed);
    m_eventScanner.ScanForEvents(*m_pPed);
    m_eventHandler.HandleEvents();
    m_TaskMgr.ManageTasks();
    GetPlayerRelationshipRecorder().RecordRelationshipWithPlayer(m_pPed);
    LookAtInterestingEntities();

    g_LoadMonitor.EndTimer(0);
}

// 0x4B85B0
CTask* CPedIntelligence::GetActivePrimaryTask() const {
    for (const auto i : { TASK_PRIMARY_PHYSICAL_RESPONSE, TASK_PRIMARY_EVENT_RESPONSE_TEMP, TASK_PRIMARY_EVENT_RESPONSE_NONTEMP }) {
        if (CTask* task = m_TaskMgr.GetTaskPrimary(TASK_PRIMARY_PHYSICAL_RESPONSE)) {
            return task;
        }
    }
    return nullptr;
}

// 0x4ABEA0
float CPedIntelligence::GetPedFOVRange() const {
    return std::max(m_fHearingRange, m_fSeeingRange);
}

// 0x421050
void CPedIntelligence::IncrementAngerAtPlayer(uint8 anger) {
    if (!m_mentalState.m_AngerTimer.IsOutOfTime()) {
        return;
    }
    m_mentalState.m_AngerTimer.Start(3000);
    m_mentalState.m_AngerAtPlayer += anger;
}

// 0x6074A0
void* CPedIntelligence::operator new(unsigned size) {
    return GetPedIntelligencePool()->New();
}

// 0x6074E0
void CPedIntelligence::operator delete(void* object) {
    GetPedIntelligencePool()->Delete(static_cast<CPedIntelligence*>(object));
}

// NOTSA
CVehicle* CPedIntelligence::GetEnteringVehicle() {
    for (const auto taskt : { TASK_COMPLEX_ENTER_CAR_AS_DRIVER, TASK_COMPLEX_ENTER_CAR_AS_PASSENGER }) {
        if (const auto task = FindTaskByType(taskt)) {
            return static_cast<CTaskComplexEnterCar*>(task)->GetTargetCar();
        }
    }
    return nullptr;
}

// 0x607140
CPedIntelligence* CPedIntelligence::Constructor(CPed* ped) {
    this->CPedIntelligence::CPedIntelligence(ped);
    return this;
}

// 0x607300
CPedIntelligence* CPedIntelligence::Destructor() {
    this->CPedIntelligence::~CPedIntelligence();
    return this;
}
