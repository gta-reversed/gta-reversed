#include "StdInc.h"

#include "TaskComplexKillPedOnFootArmed.h"
#include "TaskSimpleGoToPoint.h"
#include "TaskSimpleDuck.h"
#include "TaskSimpleGunControl.h"
#include "TaskSimpleThrowControl.h"
#include "TaskSimpleStandStill.h"
#include "TaskSimplePause.h"
#include "TaskSimpleUseGun.h"
#include "TaskSimpleSlideToCoord.h" // STAND_STILL_TIME (0x86DB24)
#include "TaskComplexSeekEntityStandard.h"
#include "TaskComplexGoToPointAndStandStill.h"
#include "Cover.h"
#include "World.h"
#include <extensions/utility.hpp>

void CTaskComplexKillPedOnFootArmed::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexKillPedOnFootArmed, 0x86d918, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x621190);
    RH_ScopedInstall(Destructor, 0x621250);

    RH_ScopedInstall(LineOfSightClearForAttack, 0x621500);
    RH_ScopedInstall(IsPedInLeaderFiringLine, 0x621300);
    RH_ScopedInstall(CreateSubTask, 0x626FC0);

    RH_ScopedVMTInstall(Clone, 0x6234C0);
    RH_ScopedVMTInstall(GetTaskType, 0x621240);
    RH_ScopedVMTInstall(MakeAbortable, 0x6212B0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x62C190, {.reversed = false});
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x62BF00);
    RH_ScopedVMTInstall(ControlSubTask, 0x62CCE0);
}

// 0x621190
CTaskComplexKillPedOnFootArmed::CTaskComplexKillPedOnFootArmed(
    CPed*  target,
    uint32 duckingConditions,
    uint32 duckTime,
    uint32 duckChancePerc,
    int8   competence
) :
    m_target{ target },
    m_duckingConditions{ duckingConditions },
    m_lengthOfDuck{ duckTime },
    m_duckChancePerc{ duckChancePerc },
    m_competence{ competence }
{
    CEntity::SafeRegisterRef(m_target);
}

// notsa
CTaskComplexKillPedOnFootArmed::CTaskComplexKillPedOnFootArmed(const CTaskComplexKillPedOnFootArmed& o) :
    CTaskComplexKillPedOnFootArmed{
        o.m_target,
        o.m_duckingConditions,
        o.m_lengthOfDuck,
        o.m_duckChancePerc,
        o.m_competence
    }
{
    m_aimImmediate = o.m_aimImmediate;
}

// 0x621250
CTaskComplexKillPedOnFootArmed::~CTaskComplexKillPedOnFootArmed() {
    CEntity::SafeCleanUpRef(m_target);
}

// 0x621500
bool CTaskComplexKillPedOnFootArmed::LineOfSightClearForAttack(CPed* ped) { // ped is the task owner ped
    /*
    * TODO:
    * here's some code to calculate the values to use (based on some variables)
    * nothing special, but it's messy, and it's late, so I won't bother
    */
    const auto reqDistDeltaSq = sq(2.f);
    const auto reqTimeDelta   = 5000;

    //> If the LOS was recently clear, let's consider it still is
    if (CTimer::GetTimeInMS() - m_losClearTime < reqTimeDelta) {
        return true;
    }

    //> Perhaps check if the entities are still kinda around the same position as the last time the LOS was blocked...
    if (CTimer::GetTimeInMS() - m_losBlockedTime < reqTimeDelta) {
        if (reqDistDeltaSq >= (m_target->GetPosition() - m_losBlockedTargetPos).SquaredMagnitude()) {
            if (reqDistDeltaSq >= (ped->GetPosition() - m_losBlockedOurPos).SquaredMagnitude()) {
                return false;
            }
        }
    }

    //> 0x6216DD
    // Temporarily disable the target ped vehicle's collision (To ignore it)
    // Perhaps, `CWorld::pIgnoreEntity` could be used?
    const auto targetVeh = m_target->GetVehicleIfInOne();
    const notsa::ScopeGuard restore{[targetVeh, had = targetVeh && targetVeh->GetUsesCollision()] {
        if (targetVeh) {
            targetVeh->SetUsesCollision(had);
        }
    }};

    if (targetVeh) {
        targetVeh->SetUsesCollision(false);
    }

    const auto GetPedHeadPos = [](CPed* headOf) {
        CVector inout{ 0.1f, 0.f, 0.f };
        headOf->GetTransformedBonePosition(inout, BONE_HEAD);
        return inout;
    };

    if (CWorld::GetIsLineOfSightClear(
        ped->bIsDucking
            ? GetPedHeadPos(ped)
            : ped->GetPosition() + CVector{0.f, 0.f, 0.25f},
        GetPedHeadPos(m_target), // Always use head position
        true,
        true,
        false,
        true,
        false,
        true,
        false
    )) {
        m_losClearTime   = CTimer::GetTimeInMS();
        m_losBlockedTime = 0;
        return true;
    } else {
        m_losBlockedTime      = CTimer::GetTimeInMS();
        m_losClearTime        = 0;
        m_losBlockedOurPos    = ped->GetPosition();
        m_losBlockedTargetPos = m_target->GetPosition();
        return false;
    }
}

// 0x621300
bool CTaskComplexKillPedOnFootArmed::IsPedInLeaderFiringLine(CPed* ped) {
    const auto pedGrp = ped->GetGroup();
    if (!pedGrp) {
        return false;
    }

    const auto grpLeaderPlyr = pedGrp->GetMembership().GetLeader();
    if (!grpLeaderPlyr || !grpLeaderPlyr->IsPlayer()) {
        return false;
    }

    if (!grpLeaderPlyr->m_pTargetedObject || grpLeaderPlyr->GetActiveWeapon().IsTypeMelee()) {
        return false;
    }

    const auto &leaderPos2D      = grpLeaderPlyr->GetPosition2D();
    const auto &leaderPos        = grpLeaderPlyr->GetPosition();
    const auto leaderToPed       = ped->GetPosition() - leaderPos, // 0x6213BD
               leaderToTargetDir = (grpLeaderPlyr->m_pTargetedObject->GetPosition() - leaderPos).Normalized(); // 0x621394

    /* clang-format off
     * --[projPointOnLeaderToTargetRay2D]-->[leaderPos]---[leaderToTargetDir]-->[Target]
     *                                          /
     *                                         /
     *                                        /
     *                                       /
     *                                 [leaderToPed]
     *                                     /
     *                                    /
     *                                  \|/
     *                                [Ped]
     * clang-format on */

    //> 0x6213AE
    const auto projPointOnLeaderToTargetRay2D = leaderPos + CVector2D{ leaderToPed }.ProjectOnToNormal(leaderToTargetDir);
    if ((projPointOnLeaderToTargetRay2D - ped->GetPosition2D()).SquaredMagnitude() >= sq(2.f)) {
        return false;
    }

    //> 0x0621482
    if (leaderToTargetDir.Dot(leaderToPed) <= 0.f) { // Ped is "behind" leader (Like on the ASCII art above)
        return false;
    }

    //> 0x6214CD
    if (leaderToPed.SquaredMagnitude() >= sq(10.f)) {
        return false;
    }

    return true;
}

// 0x626FC0
CTask* CTaskComplexKillPedOnFootArmed::CreateSubTask(eTaskType taskType, CPed* ped) {
    const auto& ourWepInfo = ped->GetActiveWeapon().GetWeaponInfo(ped);
    switch (taskType) {
    case TASK_COMPLEX_SEEK_ENTITY: {
        // Seek/retry approach (0x627115): fresh seek at 6.0f (0x40C00000); LOS blocked 3s..8s slows the seek (6.0f - (dt - 3000.0f) * 0.001f).
        const auto dt = (float)(int32)(CTimer::GetTimeInMS() - m_losBlockedTime);
        if (m_losBlockedTime && dt >= 3000.0f) {
            if (dt <= 8000.0f) {
                return new CTaskComplexSeekEntityStandard{ m_target, 50000, 1000, 6.0f - (dt - 3000.0f) * 0.001f, 2.0f, 2.0f, true, true };
            }
            // Very-long-blocked sidestep probe (0x6271FD): perpendicular to the target->us direction, scaled 1.5f.
            // probeA = targetPos + perp, probeB = targetPos - perp; first with clear LOS (target->probe) wins.
            CVector dir = m_target->GetPosition() - ped->GetPosition();
            dir.z = 0.0f;
            dir.Normalise();
            const CVector perp{ -dir.y * 1.5f, dir.x * 1.5f, 0.0f };
            const CVector& tgtPos = m_target->GetPosition();
            const CVector probeA = tgtPos + perp;
            const CVector probeB = tgtPos - perp;
            if (CWorld::GetIsLineOfSightClear(tgtPos, probeA, true, true, false, false, true, false, false)) {
                return new CTaskComplexGoToPointAndStandStill{ PEDMOVE_RUN, probeA, 0.5f, 2.0f, false, false };
            }
            if (CWorld::GetIsLineOfSightClear(tgtPos, probeB, true, true, false, false, true, false, false)) {
                return new CTaskComplexGoToPointAndStandStill{ PEDMOVE_RUN, probeB, 0.5f, 2.0f, false, false };
            }
            return new CTaskComplexSeekEntityStandard{ m_target, 50000, 1000, 1.0f, 2.0f, 2.0f, true, true };
        }
        return new CTaskComplexSeekEntityStandard{ m_target, 50000, 1000, 6.0f, 2.0f, 2.0f, true, true };
    }
    case TASK_SIMPLE_PAUSE: { // 0x626FC0: stand still for a beat, then a short pause
        CTaskSimpleStandStill still{ 0, false, false, 8.0f };
        still.ProcessPed(ped);
        return new CTaskSimplePause{ 100 };
    }
    case TASK_SIMPLE_STAND_STILL: {
        // 0x626FC0: time is the `STAND_STILL_TIME` static (`DAT_0086db24`), blend is 8.0f (`0x41000000`)
        return new CTaskSimpleStandStill{ STAND_STILL_TIME, false, false, 8.0f };
    }
    case TASK_SIMPLE_DUCK: {
        return new CTaskSimpleDuck{ DUCK_STANDALONE, (uint16)m_lengthOfDuck, -1 };
    }
    case TASK_SIMPLE_GUN_CTRL: {
        if (ourWepInfo.flags.bThrow) {
            const auto task = new CTaskSimpleThrowControl{ m_target, nullptr };
            m_lastAttackTime = CTimer::GetTimeInMS();
            m_losBlockedTime = 0;
            return task;
        }
        auto firingTask = eGunCommand::FIREBURST;
        if (!LineOfSightClearForAttack(ped)) {
            firingTask = (eGunCommand)0;
        }
        const auto task = new CTaskSimpleGunControl{ m_target, {}, {}, firingTask, 5, -1 };
        task->m_aimImmidiately = m_aimImmediate;
        m_aimImmediate = false;
        m_lastAttackTime = CTimer::GetTimeInMS();
        m_shootTimer = CTimer::GetTimeInMS() + CGeneral::GetRandomNumberInRange(4000, 8000);
        if (ped->GetGroup()) {
            ped->Say(CTX_GLOBAL_SURROUNDED);
        }
        m_losBlockedTime = 0;
        return task;
    }
    default:
        return nullptr;
    }
}

// 0x6212B0
bool CTaskComplexKillPedOnFootArmed::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
    switch (priority) {
    case ABORT_PRIORITY_URGENT: {
        if (const auto aimedAtEvent = notsa::dyn_cast_if_present<const CEventGunAimedAt>(event)) {
            if (aimedAtEvent->m_AimedBy == m_target) {
                return false;
            }
        }
        break;
    }
    case ABORT_PRIORITY_IMMEDIATE:
        break;
    case ABORT_PRIORITY_LEISURE:
    default:
        return false;
    }
    return m_pSubTask->MakeAbortable(ped, priority, event);
}

// 0x62C190
CTask* CTaskComplexKillPedOnFootArmed::CreateNextSubTask(CPed* ped) {
    if (!m_target) {
        return nullptr;
    }
    const bool isOurWeaponMelee = ped->GetActiveWeapon().IsTypeMelee();
    const auto& ourWepInfo = ped->GetActiveWeapon().GetWeaponInfo(ped);
    const float maxRange = ourWepInfo.m_fTargetRange;
    const float dist = (m_target->GetPosition() - ped->GetPosition()).Magnitude();
    switch (m_pSubTask->GetTaskType()) {
    case TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL: { // 0x62C4B4
        if (LineOfSightClearForAttack(ped)) {
            return CreateSubTask(TASK_SIMPLE_GUN_CTRL, ped);
        }
        return CreateSubTask(TASK_SIMPLE_PAUSE, ped);
    }
    case TASK_SIMPLE_GO_TO_POINT: { // 900
        if (LineOfSightClearForAttack(ped)) {
            return CreateSubTask(TASK_SIMPLE_GUN_CTRL, ped);
        }
        return CreateSubTask(TASK_SIMPLE_PAUSE, ped);
    }
    case TASK_SIMPLE_PAUSE: // 0xCA
    case TASK_SIMPLE_STAND_STILL: { // 0xCB
        if (!ped->bStayInSamePlace && !ped->bKindaStayInSamePlace) {
            ped->Say(CTX_GLOBAL_MOVE_IN); // 0x90
            if (const auto seek = CreateSubTask(TASK_COMPLEX_SEEK_ENTITY, ped)) {
                if (!m_losBlockedTime) {
                    m_losBlockedTime = CTimer::GetTimeInMS();
                }
                return seek;
            }
            return nullptr;
        }
        // 0x62C339/0x62C3DF: dist to target <= weapon range => gun ctrl without LOS probe
        const auto losTaskType = dist <= maxRange
            ? TASK_SIMPLE_GUN_CTRL
            : LineOfSightClearForAttack(ped) ? TASK_SIMPLE_GUN_CTRL : TASK_SIMPLE_PAUSE;
        if (losTaskType == TASK_SIMPLE_PAUSE) {
            return CreateSubTask(TASK_SIMPLE_PAUSE, ped);
        }
        return CreateSubTask(TASK_SIMPLE_GUN_CTRL, ped);
    }
    case TASK_SIMPLE_DUCK: { // 0x19F
        const auto task = LineOfSightClearForAttack(ped)
            ? CreateSubTask(TASK_SIMPLE_GUN_CTRL, ped)
            : CreateSubTask(TASK_SIMPLE_PAUSE, ped);
        ped->bKindaStayInSamePlace = false; // 0x62C3CB: `& 0xfbffffff`
        m_losBlockedTime = 0;
        return task;
    }
    case TASK_COMPLEX_GO_TO_POINT_SHOOTING: // 0x62C411: bStayInSamePlace set => pause; in range => gun ctrl, else range-gated seek/gun logic
    case TASK_SIMPLE_GUN_CTRL:
    case TASK_SIMPLE_THROW_CTRL:
    case TASK_SIMPLE_GANG_DRIVEBY: {
        if (ped->bStayInSamePlace) {
            return CreateSubTask(TASK_SIMPLE_PAUSE, ped);
        }
        if (dist < 3.0f) { // [0x858B3C]
            return CreateSubTask(TASK_SIMPLE_GUN_CTRL, ped);
        }
        float desiredRange = maxRange * 0.8f; // [0x858C98]
        if (desiredRange > 23.0f) {           // [0x858F84]
            desiredRange = 23.0f;
        }
        if (desiredRange < dist) {
            if (ped->bKindaStayInSamePlace) { // 0x400000
                return CreateSubTask(TASK_SIMPLE_PAUSE, ped);
            }
            float seekSpeed = maxRange * 0.6f; // [0x858CC8]
            if (seekSpeed > 20.0f) {           // [0x858BA4]: 20.0f floor expressed as min clamp
                seekSpeed = 20.0f;
            }
            if (const auto seek = new CTaskComplexSeekEntityStandard{ m_target, 50000, 1000, seekSpeed, 2.0f, 2.0f, true, true }) {
                return seek;
            }
        }
        if (const auto subType = m_pSubTask->GetTaskType();
            subType == TASK_SIMPLE_GUN_CTRL && notsa::cast<CTaskSimpleGunControl>(m_pSubTask)->m_isFirstTime
            && !ped->bStayInSamePlace && !ped->bKindaStayInSamePlace
        ) {
            ped->Say(CTX_GLOBAL_MOVE_IN);
            if (const auto seek = new CTaskComplexSeekEntityStandard{ m_target, 50000, 1000, 2.0f, 2.0f, 2.0f, true, true }) {
                return seek;
            }
        }
        if (!isOurWeaponMelee && ped->m_pCoverPoint) {
            if (CCover::DoesCoverPointStillProvideCover(ped->m_pCoverPoint, m_target->GetPosition())) {
                CVector coverPos{};
                if (CCover::FindCoordinatesCoverPoint(*ped->m_pCoverPoint, ped, m_target->GetPosition(), coverPos)) {
                    if ((coverPos - ped->GetPosition()).Magnitude2D() < 0.75f) { // [0x858F34]
                        if (ped->m_pCoverPoint->GetType() == CCoverPoint::eType::NONE) {
                            if (dist < 12.0f // [0x858CCC]
                                && !ped->bCrouchWhenShooting
                                && CGeneral::GetRandomNumber() % 4 == 0
                                && !ped->GetTaskManager().GetTaskSecondary(TASK_SECONDARY_DUCK)
                            ) {
                                ped->Say(CTX_GLOBAL_DUCK);
                                if (const auto duck = new CTaskSimpleDuck{ DUCK_STANDALONE, 2500, -1 }) {
                                    return duck;
                                }
                                ped->ReleaseCoverPoint();
                            }
                        } else if (CGeneral::GetRandomNumber() % 2 == 0 && !ourWepInfo.flags.bThrow) {
                            const auto task = new CTaskSimpleGunControl{ m_target, {}, {}, eGunCommand::FIREBURST, 5, -1 };
                            task->m_aimImmidiately = m_aimImmediate;
                            m_aimImmediate = false;
                            m_lastAttackTime = CTimer::GetTimeInMS();
                            m_shootTimer = CTimer::GetTimeInMS() + 5000;
                            m_lastStrafeTime = CTimer::GetTimeInMS() + 2500;
                            m_bStrafeBack = true;
                            m_strafeDir = eStrafeDir::LEFT;
                            if (ped->m_pCoverPoint->GetType() != CCoverPoint::eType::NONE) {
                                m_bStrafeBack = false;
                            }
                            ped->GetIntelligence()->ClearTaskDuckSecondary();
                            if (task) {
                                return task;
                            }
                            ped->ReleaseCoverPoint();
                        }
                    }
                    ped->ReleaseCoverPoint();
                }
            }
        }
        if (m_competence > 0 && dist > 6.0f // [0x858B44]
            && !isOurWeaponMelee && CGeneral::GetRandomNumber() % 2 != 0
        ) {
            ped->ReleaseCoverPoint();
            if (const auto cover = CCover::FindAndReserveCoverPoint(ped, m_target->GetPosition(), m_competence == 2)) {
                ped->m_pCoverPoint = cover;
                CVector coverPos{};
                CCover::FindCoordinatesCoverPoint(*cover, ped, m_target->GetPosition(), coverPos);
                coverPos.z += 1.0f;
                if (((m_target->GetPosition() - ped->GetPosition()).Magnitude() < maxRange * 0.75f) // [0x858F34]
                    && CWorld::GetIsLineOfSightClear(coverPos, ped->GetPosition(), true, true, false, false, false, false, false)
                ) {
                    if (ped->GetGroup()) {
                        ped->Say(CTX_GLOBAL_COVER_ME);
                    }
                    ped->GetIntelligence()->SetTaskDuckSecondary(6000);
                    if (const auto go = new CTaskSimpleGoToPoint{ PEDMOVE_RUN, coverPos, 0.5f, true, false }) {
                        return go;
                    }
                    ped->ReleaseCoverPoint();
                } else {
                    ped->ReleaseCoverPoint();
                }
            }
        }
        if (dist > 10.0f) { // [0x85862C]
            if (CGeneral::GetRandomNumber() % 4 == 0) {
                if (!ped->bKindaStayInSamePlace) { // 0x400000 clear
                    ped->Say(CTX_GLOBAL_MOVE_IN);
                    if (const auto seek = new CTaskComplexSeekEntityStandard{ m_target, 50000, 1000, dist - 4.0f /* [0x858B90] */, 2.0f, 2.0f, true, true }) {
                        return seek;
                    }
                }
            } else if (CGeneral::GetRandomNumber() % 4 == 1 && !ped->bKindaStayInSamePlace) {
                m_strafeDir = eStrafeDir::BACK;
                m_lastStrafeTime = CTimer::GetTimeInMS() + 2000;
                m_bStrafeBack = false;
            }
        }
        if (dist > 5.0f /* [0x858C80] */ && CGeneral::GetRandomNumber() % 4 == 0) { // sidestep direction probe
            m_strafeDir = eStrafeDir::LEFT;
            if (CGeneral::GetRandomNumber() % 2 != 0) {
                m_strafeDir = eStrafeDir::RIGHT;
            }
            const CVector right = ped->GetRight();
            CVector probe = ped->GetPosition() + right * (m_strafeDir == eStrafeDir::RIGHT ? 8.0f : -8.0f); // 0x41000000
            if (!CWorld::GetIsLineOfSightClear(ped->GetPosition(), probe, true, true, false, true, false, false, false)) {
                m_strafeDir = m_strafeDir == eStrafeDir::RIGHT ? eStrafeDir::LEFT : eStrafeDir::RIGHT;
            }
            m_lastStrafeTime = CTimer::GetTimeInMS() + 2000;
            m_bStrafeBack = false;
        }
        if (LineOfSightClearForAttack(ped)) {
            return CreateSubTask(TASK_SIMPLE_GUN_CTRL, ped);
        }
        if (CGeneral::GetRandomNumber() % 4 == 0) {
            m_strafeDir = (eStrafeDir)(CGeneral::GetRandomNumber() < 0x3FFF);
            m_lastStrafeTime = CTimer::GetTimeInMS() + 2000;
            m_bStrafeBack = false;
        }
        return CreateSubTask(TASK_SIMPLE_PAUSE, ped);
    }
    case TASK_COMPLEX_SEEK_ENTITY: { // 0x62C4B4 fallthrough twin of 0x387
        if (LineOfSightClearForAttack(ped)) {
            return CreateSubTask(TASK_SIMPLE_GUN_CTRL, ped);
        }
        return CreateSubTask(TASK_SIMPLE_PAUSE, ped);
    }
    case TASK_FINISHED:
        m_losBlockedTime = 0;
        return nullptr;
    default:
        m_losBlockedTime = 0;
        return nullptr;
    }
}

// 0x62BF00
CTask* CTaskComplexKillPedOnFootArmed::CreateFirstSubTask(CPed* ped) {
    if (!m_target) {
        return nullptr;
    }

    const auto &ourPos    = ped->GetPosition(),
               &targetPos = m_target->GetPosition();

    const auto targetToOurPedDistSq = (ourPos - targetPos).SquaredMagnitude();

    if (   !ped->bStayInSamePlace
        && CGeneral::RandomBool(50)
        && m_competence > 0
        && (targetToOurPedDistSq >= sq(30.f) || targetToOurPedDistSq >= sq(6.f) && !m_target->GetActiveWeapon().IsTypeMelee()) 
    ) {
        ped->ReleaseCoverPoint();
        if (ped->m_pCoverPoint = CCover::FindAndReserveCoverPoint(ped, targetPos, false)) {
            CVector coverPos{};
            if (!notsa::IsFixBugs() || CCover::FindCoordinatesCoverPoint(*ped->m_pCoverPoint, ped, targetPos, coverPos)) {
                if (CWorld::GetIsLineOfSightClear(coverPos, ourPos, true, true, false, false, false, false, false)) {
                    ped->GetIntelligence()->SetTaskDuckSecondary(6000);
                    if (const auto task = new CTaskSimpleGoToPoint{
                        PEDMOVE_RUN,
                        coverPos,
                        0.5f,
                        true
                    }) { // I can't believe my eyes.... error handling?
                        return task;
                    }
                } else {
                    ped->ReleaseCoverPoint();
                }
            } else {
                NOTSA_LOG_WARN("Can't find cover point's coordinates!"); // Originally the game has left `coverPos` uninitialized in this case
            }
        }
    }

    return CreateSubTask([&, this] {
        const auto& ourActiveWep = ped->GetActiveWeapon().GetWeaponInfo(ped);

        if (targetToOurPedDistSq <= sq(ourActiveWep.m_fTargetRange / 4.f) && LineOfSightClearForAttack(ped)) {
            return TASK_SIMPLE_GUN_CTRL;
        }

        if (!ped->bStayInSamePlace && !ped->bKindaStayInSamePlace) {
            return TASK_COMPLEX_SEEK_ENTITY;
        }

        if (LineOfSightClearForAttack(ped) || ourActiveWep.flags.bThrow) {
            return TASK_SIMPLE_PAUSE;
        }

        return TASK_SIMPLE_GUN_CTRL;
    }(), ped);
}

// 0x62CCE0
CTask* CTaskComplexKillPedOnFootArmed::ControlSubTask(CPed* ped) {
    if (m_newTarget) {
        return CreateFirstSubTask(ped);
    }

    if (!m_target || m_target->m_fHealth <= 0.f) {
        return nullptr;
    }
    const auto ogSubTask = m_pSubTask;

    const auto &ourPos    = ped->GetPosition(),
               &targetPos = m_target->GetPosition();

    const auto targetToOurPedDistSq = (ourPos - targetPos).SquaredMagnitude();

    //> 0x62CD55
    if (   (m_duckingConditions & 4) == 0
        || CTimer::GetTimeInMS() <= m_lastDuckTime + m_lengthOfDuck + 2000
        || ped->bIsDucking
    ) {
        if (   m_duckingConditions & 1
            && m_bShotFiredByPlayer
            && CGeneral::RandomBool((float)m_duckChancePerc)
        ) {
            if (const auto quack = ped->GetIntelligence()->GetTaskDuck()) {
                quack->SetDuckTimer((uint16)m_lengthOfDuck);
            } else {
                ped->GetIntelligence()->SetTaskDuckSecondary((uint16)m_lengthOfDuck);
#ifdef FIX_BUGS
                m_lastDuckTime = CTimer::GetTimeInMS(); // Not actually sure if this is necessary tbh
#endif
            }
        }
    } else {
        if (CGeneral::RandomBool((float)m_duckChancePerc)) {
            if (targetToOurPedDistSq <= sq(20.f)) {
                ped->GetIntelligence()->SetTaskDuckSecondary((uint16)m_lengthOfDuck);
            }
        }
        m_lastDuckTime = CTimer::GetTimeInMS();
    }

    //> 0x62CE56
    const auto quack = ped->GetIntelligence()->GetTaskDuck();
    if (quack) {
        quack->m_bIsInControl = true;
    }

    //> 0x62CE6E
    if (m_pSubTask != ogSubTask) { // ?????
        return ogSubTask;
    }

    const auto& ourwi = ped->GetActiveWeapon().GetWeaponInfo(ped);
    switch (m_pSubTask->GetTaskType()) { //> 0x62CE7E
    case TASK_COMPLEX_SEEK_ENTITY: {
        if ([&, this]{
            if (IsPedInLeaderFiringLine(ped)) {
                return true;
            }
            if (m_target->physicalFlags.bSubmergedInWater && LineOfSightClearForAttack(ped)) {
                return true;
            }
            if (ped->bStayInSamePlace) {
                return LineOfSightClearForAttack(ped);
            } else if (sq(ourwi.m_fTargetRange / 2.f) >= targetToOurPedDistSq) {
                if (CTimer::GetTimeInMS() - m_lastAttackTime >= 2000 || m_target->GetMoveSpeed().Dot(ped->GetForward()) < 0.f) {
                    return LineOfSightClearForAttack(ped);
                }
            }
            return false;
        }()) {
            return CreateSubTask(TASK_SIMPLE_GUN_CTRL, ped);
        } else {
            if (ped->GetGroup()) {
                ped->Say(CTX_GLOBAL_COVER_ME);
            }
        }
        break;
    }
    case TASK_SIMPLE_GUN_CTRL: { //> 0x62CE87
        if (   targetToOurPedDistSq >= sq(ourwi.m_fTargetRange)
            || targetToOurPedDistSq >= sq(ourwi.m_fTargetRange / 2.f) && !ped->bStayInSamePlace && CTimer::GetTimeInMS() - m_lastAttackTime >= 2000
            || targetToOurPedDistSq >= sq(4.f) && !ped->bStayInSamePlace && CTimer::GetTimeInMS() >= m_shootTimer
            || !LineOfSightClearForAttack(ped)
        ) {
            const auto gctrl = notsa::cast<CTaskSimpleGunControl>(m_pSubTask);
            if (gctrl->m_firingTask != eGunCommand::END_LEISURE) {
                gctrl->m_nextAtkTimeMs = 0;
                gctrl->m_firingTask    = eGunCommand::END_LEISURE;
            }
        }

        const auto ugun = ped->GetIntelligence()->GetTaskUseGun();
        if (!ugun) {
            break;
        }

        if (ped->GetGroup()) {
            ped->Say(CTX_GLOBAL_SURROUNDED);
        }

        //> 0x62CF88
        const auto actionDir = [&, this]() -> CVector2D {
            if (m_target->physicalFlags.bSubmergedInWater) {
                return { 0.f, 0.f };
            }
            if (CTimer::GetTimeInMS() >= m_lastStrafeTime || !ped->bStayInSamePlace) {
                if (targetToOurPedDistSq <= sq(4.f) && ped->bStayInSamePlace) {
                    return { 0.f, 1.f }; // Forwards
                }
                if (m_bStrafeBack) {
                    m_bStrafeBack    = false;
                    m_lastStrafeTime = CTimer::GetTimeInMS() + 2500;
                    m_strafeDir      = [&, this] { // Strafe back to where we came from the last time
                        switch (m_strafeDir)
                        {
                        case eStrafeDir::LEFT:    return eStrafeDir::RIGHT;
                        case eStrafeDir::RIGHT:   return eStrafeDir::LEFT;
                        case eStrafeDir::FORWARD: return eStrafeDir::BACK;
                        case eStrafeDir::BACK:    return eStrafeDir::FORWARD;
                        default:                  NOTSA_UNREACHABLE();
                        }
                    }();
                }
                return { 0.f, 0.f };
            } else {
                switch (m_strafeDir)
                {
                case eStrafeDir::LEFT:    return { -1.f,  0.f };
                case eStrafeDir::RIGHT:   return {  1.f,  0.f };
                case eStrafeDir::FORWARD: return {  0.f, -1.f };
                case eStrafeDir::BACK:    return {  0.f,  1.f };
                default:                  NOTSA_UNREACHABLE();
                }
            }
        }();

        //> 0x62D080
        if (quack) {
            quack->ControlDuckMove([&, this] {
                if (notsa::contains(std::to_array({ eStrafeDir::LEFT, eStrafeDir::RIGHT }), m_strafeDir)) {
                    if (m_lastRollTime + 3000 >= CTimer::GetTimeInMS()) {
                        return CVector2D{ 0.f, 0.f };
                    } else {
                        m_lastRollTime = CTimer::GetTimeInMS();
                    }
                }
                return actionDir;
            }());
        } else {
            ugun->ControlGunMove(actionDir);
        }

        break;
    }
    }

    return m_pSubTask;
}
