#include "StdInc.h"

#include "TaskComplexKillPedOnFootArmed.h"
#include "TaskComplexGoToPointAndStandStill.h"
#include "SeekEntity/TaskComplexSeekEntityStandard.h"
#include "TaskSimpleGoToPoint.h"
#include "TaskSimpleDuck.h"
#include "TaskSimpleGunControl.h"
#include "TaskSimpleThrowControl.h"
#include "TaskSimpleUseGun.h"
#include "TaskSimplePause.h"
#include "TaskSimpleStandStill.h"
#include "Cover.h"
#include "LoadMonitor.h"

constexpr auto STAND_STILL_TIME_MS = 20'000; // 0x86DB24
constexpr auto DUCK_IN_COVER_MS    = 6'000;

void CTaskComplexKillPedOnFootArmed::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexKillPedOnFootArmed, 0x86d918, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedConstructorInstall(0x621190, "1", {}, CPed*, uint32, uint32, uint32, int8);
    RH_ScopedDestructorInstall(0x621250);

    RH_ScopedInstall(LineOfSightClearForAttack, 0x621500);
    RH_ScopedInstall(IsPedInLeaderFiringLine, 0x621300);
    RH_ScopedInstall(CreateSubTask, 0x626FC0);

    RH_ScopedVMTInstall(Clone, 0x6234C0);
    RH_ScopedVMTInstall(GetTaskType, 0x621240);
    RH_ScopedVMTInstall(MakeAbortable, 0x6212B0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x62C190);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x62BF00);
    RH_ScopedVMTInstall(ControlSubTask, 0x62CCE0);
}

// 0x621190
CTaskComplexKillPedOnFootArmed::CTaskComplexKillPedOnFootArmed(
    CPed*  target,
    uint32 duckingConditions,
    uint32 lengthOfDuck,
    uint32 duckChancePerc,
    int8   competence
) :
    m_Target{ target },
    m_DuckingConditions{ duckingConditions },
    m_LengthOfDuck{ lengthOfDuck },
    m_DuckChancePerc{ duckChancePerc },
    m_Competence{ competence } {
}

// NOTSA
CTaskComplexKillPedOnFootArmed::CTaskComplexKillPedOnFootArmed(const CTaskComplexKillPedOnFootArmed& o) :
    CTaskComplexKillPedOnFootArmed{
        o.m_Target,
        o.m_DuckingConditions,
        o.m_LengthOfDuck,
        o.m_DuckChancePerc,
        o.m_Competence
    } {
    m_AimImmediate = o.m_AimImmediate;
}

// 0x621500
bool CTaskComplexKillPedOnFootArmed::LineOfSightClearForAttack(CPed* ped) {
    // The result of the last check is reused for a while (for a shorter time in interiors, or if a mission ped is after the player)
    const bool   isAfterPlayer    = m_Target && m_Target->IsPlayer() && (CGame::currArea != AREA_CODE_NORMAL_WORLD || ped->IsCreatedBy(PED_MISSION)); // 0x621548
    const bool   isRecheckedOften = ped->GetAreaCode() != AREA_CODE_NORMAL_WORLD || isAfterPlayer;
    const uint32 recheckInterval  = isRecheckedOften ? 5'000 : 10'000;
    const float  maxMovedDistSq   = isRecheckedOften && g_LoadMonitor.GetProcLevel() < eProcessingLevel::HIGH ? sq(2.f) : sq(3.f);
    const uint32 now              = CTimer::GetTimeInMS();

    if (now - m_LosClearTime < recheckInterval) { // 0x62158A
        return true;
    }

    // Still blocked if neither the target, nor the ped has moved since
    if (now - m_LosBlockedTime <= recheckInterval) { // 0x62159D
        if (!m_Target || CVector::DistSqr(m_Target->GetPosition(), m_LosBlockedTargetPos) <= maxMovedDistSq) {
            if (CVector::DistSqr(ped->GetPosition(), m_LosBlockedOurPos) <= maxMovedDistSq) {
                return false;
            }
        }
    }

    const auto GetHeadPos = [](CPed* headOf) {
        CVector pos{ 0.1f, 0.f, 0.f };
        headOf->GetTransformedBonePosition(pos, BONE_HEAD);
        return pos;
    };
    const CVector origin    = ped->bIsDucking ? ped->GetPosition() + CVector{ 0.f, 0.f, 0.25f } : GetHeadPos(ped); // 0x62163A
    const CVector targetPos = GetHeadPos(m_Target);

    // 0x6216D8 - The vehicle the target is in doesn't block the LOS
    auto* const targetVeh    = m_Target->GetVehicleIfInOne();
    const bool  hadCollision = targetVeh && targetVeh->GetUsesCollision();
    if (targetVeh) {
        targetVeh->SetUsesCollision(false);
    }
    const bool isClear = CWorld::GetIsLineOfSightClear(origin, targetPos, true, true, false, true, false, true, false);
    if (targetVeh) {
        targetVeh->SetUsesCollision(hadCollision);
    }

    if (isClear) { // 0x62172B
        m_LosClearTime   = now;
        m_LosBlockedTime = 0;
    } else { // 0x621746
        m_LosClearTime        = 0;
        m_LosBlockedTime      = now;
        m_LosBlockedTargetPos = m_Target->GetPosition();
        m_LosBlockedOurPos    = ped->GetPosition();
    }
    return isClear;
}

// 0x621300
bool CTaskComplexKillPedOnFootArmed::IsPedInLeaderFiringLine(CPed* ped) {
    auto* const group = ped->GetGroup();
    if (!group) {
        return false;
    }

    auto* const leader = group->GetMembership().GetLeader();
    if (!leader || !leader->IsPlayer() || !leader->m_pTargetedObject || leader->GetActiveWeapon().IsTypeMelee()) {
        return false;
    }

    const CVector& leaderPos      = leader->GetPosition();
    const CVector  leaderToTarget = leader->m_pTargetedObject->GetPosition() - leaderPos;
    const CVector  leaderToPed    = ped->GetPosition() - leaderPos;

    // 0x621394 - The ped has to be within 2 units of the leader's (2D) line of fire...
    const CVector fireDir = CVector{ CVector2D{ leaderToTarget } }.Normalized();
    if (CVector2D::DistSqr(leaderPos + fireDir * fireDir.Dot(leaderToPed), ped->GetPosition()) >= sq(2.f)) {
        return false;
    }

    // 0x621452 - ...in front of the leader, and closer than 10 units to him
    return leaderToTarget.Dot(leaderToPed) > 0.f && leaderToPed.SquaredMagnitude() < sq(10.f);
}

// NOTSA
CTask* CTaskComplexKillPedOnFootArmed::CreateSeekSubTask(float maxTargetDist2D) {
    return new CTaskComplexSeekEntityStandard{
        m_Target,
        50'000,
        1'000,
        maxTargetDist2D,
        2.f,
        2.f,
        true,
        true
    };
}

// NOTSA - Code from 0x627110 (`CreateSubTask`)
CTask* CTaskComplexKillPedOnFootArmed::CreateMoveInCloserSubTask(CPed* ped) {
    if (!m_NeedToMoveInCloserTime) {
        return CreateSeekSubTask(6.f);
    }

    // The longer the ped is unable to get a clear LOS, the closer it goes
    const auto triedForMs = (float)(CTimer::GetTimeInMS() - m_NeedToMoveInCloserTime);
    if (triedForMs < 3'000.f) { // 0x62713A
        return CreateSeekSubTask(6.f);
    }
    if (triedForMs <= 8'000.f) { // 0x627389
        return CreateSeekSubTask(6.f - (triedForMs - 3'000.f) * 0.001f);
    }

    // 0x6271A7 - After 8 seconds try to get to the target's side instead
    const CVector& targetPos   = m_Target->GetPosition();
    const CVector  pedToTarget = CVector{ CVector2D{ targetPos - ped->GetPosition() } }.Normalized();
    const CVector  toSide      = CVector{ -pedToTarget.y, pedToTarget.x, 0.f } * 1.5f;
    for (const CVector sidePos : { targetPos + toSide, targetPos - toSide }) {
        if (CWorld::GetIsLineOfSightClear(targetPos, sidePos, true, true, false, false, true, false, false)) {
            return new CTaskComplexGoToPointAndStandStill{ PEDMOVE_RUN, sidePos, 0.5f, 2.f, false, false };
        }
    }
    return CreateSeekSubTask(1.f); // 0x627341
}

// 0x626FC0
CTask* CTaskComplexKillPedOnFootArmed::CreateSubTask(eTaskType taskType, CPed* ped) {
    switch (taskType) {
    case TASK_SIMPLE_DUCK: // 0x627037
        return new CTaskSimpleDuck{ DUCK_STANDALONE, (uint16)(m_LengthOfDuck) };
    case TASK_SIMPLE_STAND_STILL: // 0x627073
        return new CTaskSimpleStandStill{ STAND_STILL_TIME_MS };
    case TASK_SIMPLE_PAUSE: { // 0x6270AB
        CTaskSimpleStandStill{}.ProcessPed(ped);
        return new CTaskSimplePause{ 100 };
    }
    case TASK_COMPLEX_SEEK_ENTITY: { // 0x627110
        auto* const tMoveInCloser = CreateMoveInCloserSubTask(ped);
        if (m_Target && m_Target->IsPlayer()) { // 0x6273DB
            if (ped->IsCop()) {
                if (ped->Say(FindPlayerWanted()->m_NumCopsInPursuit > 1 ? CTX_GLOBAL_CHASE_FOOT : CTX_GLOBAL_SOLO) >= 0) {
                    m_Target->Say(CTX_GLOBAL_CHASED, 3'500);
                }
            } else if (ped->GetGroup()) { // 0x62748B
                ped->Say(CTX_GLOBAL_CHASE_FOOT);
            }
        }
        return tMoveInCloser;
    }
    case TASK_SIMPLE_GUN_CTRL: {
        // 0x6274C6
        const auto StartAttack = [this](CTask* tAttack) { // 0x6275AC
            m_LastAttackTime = CTimer::GetTimeInMS();
            m_LosBlockedTime = 0;
            return tAttack;
        };
        if (ped->GetActiveWeapon().GetWeaponInfo(ped).flags.bThrow) {
            return StartAttack(new CTaskSimpleThrowControl{ m_Target, nullptr });
        }
        // 0x627508
        auto* const tGunCtrl = new CTaskSimpleGunControl{
            m_Target,
            CVector{},
            CVector{},
            LineOfSightClearForAttack(ped) ? eGunCommand::FIREBURST : eGunCommand::NONE,
            5,
            -1
        };
        tGunCtrl->m_aimImmidiately = std::exchange(m_AimImmediate, false);
        m_ShootTimer               = CTimer::GetTimeInMS() + CGeneral::GetRandomNumberInRange(4'000, 8'000);
        if (ped->GetGroup()) {
            ped->Say(CTX_GLOBAL_SURROUNDED);
        }
        return StartAttack(tGunCtrl);
    }
    default:
        NOTSA_UNREACHABLE_CASE(taskType);
    }
}

// 0x6212B0
bool CTaskComplexKillPedOnFootArmed::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    switch (priority) {
    case ABORT_PRIORITY_URGENT: {
        if (const auto* const aimedAtEvent = notsa::dyn_cast_if_present<const CEventGunAimedAt>(event)) {
            if (aimedAtEvent->m_AimedBy == m_Target) {
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
    if (!m_Target) {
        return nullptr;
    }

    const uint32 now = CTimer::GetTimeInMS();
    switch (const auto tt = m_pSubTask->GetTaskType()) {
    case TASK_SIMPLE_GO_TO_POINT: { // 0x62C3DF - Got into cover
        auto* const tNext        = CreateSubTask(LineOfSightClearForAttack(ped) ? TASK_SIMPLE_GUN_CTRL : TASK_SIMPLE_PAUSE, ped);
        m_NeedToMoveInCloserTime = 0;
        return tNext;
    }
    case TASK_COMPLEX_SEEK_ENTITY:
    case TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL: {
        // 0x62C4B4
        if (LineOfSightClearForAttack(ped)) {
            auto* const tGunCtrl     = CreateSubTask(TASK_SIMPLE_GUN_CTRL, ped);
            m_NeedToMoveInCloserTime = 0;
            return tGunCtrl;
        }
        auto* const tPause = CreateSubTask(TASK_SIMPLE_PAUSE, ped);
        if (!m_NeedToMoveInCloserTime) {
            m_NeedToMoveInCloserTime = now;
        }
        return tPause;
    }
    case TASK_SIMPLE_PAUSE:
    case TASK_SIMPLE_STAND_STILL: {
        // 0x62C2DE
        if (ped->bStayInSamePlace || ped->bKindaStayInSamePlace) { // 0x62C339
            const bool  isInRange    = CVector::DistSqr(m_Target->GetPosition(), ped->GetPosition()) < sq(ped->GetActiveWeapon().GetWeaponInfo(ped).m_fTargetRange);
            auto* const tNext        = CreateSubTask(isInRange && LineOfSightClearForAttack(ped) ? TASK_SIMPLE_GUN_CTRL : TASK_SIMPLE_PAUSE, ped);
            m_NeedToMoveInCloserTime = 0;
            return tNext;
        }
        ped->Say(CTX_GLOBAL_MOVE_IN);
        auto* const tSeek = CreateSubTask(TASK_COMPLEX_SEEK_ENTITY, ped);
        if (!m_NeedToMoveInCloserTime) {
            m_NeedToMoveInCloserTime = now;
        }
        return tSeek;
    }
    case TASK_SIMPLE_DUCK: { // 0x62C2A9
        auto* const tNext        = CreateSubTask(LineOfSightClearForAttack(ped) ? TASK_SIMPLE_GUN_CTRL : TASK_SIMPLE_PAUSE, ped);
        ped->bIsDucking          = false;
        m_NeedToMoveInCloserTime = 0;
        return tNext;
    }
    case TASK_FINISHED: // 0x62CCCA
        m_NeedToMoveInCloserTime = 0;
        return nullptr;
    case TASK_COMPLEX_GO_TO_POINT_SHOOTING: // 0x62C411
    case TASK_SIMPLE_GUN_CTRL:              // 0x62C411
    case TASK_SIMPLE_THROW_CTRL:            // 0x62C411
        break;
    default:
        NOTSA_UNREACHABLE_CASE(tt);
    }

    // 0x62C411 - Done attacking, decide what to do next
    // NOTE: The random checks are the original's (the low bits of `rand()`), `CGeneral::RandomBool` draws differently

    const auto Finish = [&](CTask* tNext) { // 0x62CC6C
        const auto IsCoverBehindWall = [](const CCoverPoint* cpt) {
            return cpt && (cpt->GetUsage() == CCoverPoint::eUsage::WALLTOLEFT || cpt->GetUsage() == CCoverPoint::eUsage::WALLTORIGHT);
        };
        if (CGeneral::GetRandomNumber() % 2 != 0 && m_Competence > 0 && !ped->bNotAllowedToDuck && !IsCoverBehindWall(ped->m_pCoverPoint)) {
            ped->GetIntelligence()->SetTaskDuckSecondary(DUCK_IN_COVER_MS);
        }
        m_NeedToMoveInCloserTime = 0;
        return tNext;
    };
    const auto FinishWithAttack = [&] { // 0x62CC42
        return Finish(CreateSubTask(TASK_SIMPLE_GUN_CTRL, ped));
    };
    const auto FinishWithPause = [&] { // 0x62CC55
        auto* const tPause = CreateSubTask(TASK_SIMPLE_PAUSE, ped);
        m_LastAttackTime   = now;
        return Finish(tPause);
    };

    const auto&    weaponInfo    = ped->GetActiveWeapon().GetWeaponInfo(ped);
    const float    weaponRange   = weaponInfo.m_fTargetRange;
    const CVector& targetPos     = m_Target->GetPosition();
    const float    targetDist    = CVector::Dist(targetPos, ped->GetPosition());
    const bool     isTargetMelee = m_Target->GetActiveWeapon().IsTypeMelee();

    if (ped->bStayInSamePlace) { // 0x62C417
        return FinishWithPause();
    }

    if (targetDist < 3.f) { // 0x62C423
        return FinishWithAttack();
    }

    if (targetDist > std::min(weaponRange * 0.8f, 23.f)) { // 0x62C438 - Too far, move in
        if (ped->bKindaStayInSamePlace) {
            return FinishWithPause();
        }
        return Finish(CreateSeekSubTask(std::min(weaponRange * 0.6f, 20.f))); // 0x62C476
    }

    // 0x62C551 - The last attack was stopped by something in the way
    if (auto* const tGunCtrl = notsa::dyn_cast<CTaskSimpleGunControl>(m_pSubTask); tGunCtrl && tGunCtrl->m_isLOSBlocked) {
        if (!ped->bStayInSamePlace && !ped->bKindaStayInSamePlace) {
            ped->Say(CTX_GLOBAL_MOVE_IN);
            return Finish(CreateSeekSubTask(2.f));
        }
    }

    // 0x62C5F8 - Use the cover point the ped is at
    if (!isTargetMelee && ped->m_pCoverPoint) {
        CVector coverPos{};
        if (CCover::DoesCoverPointStillProvideCover(ped->m_pCoverPoint, targetPos) && CCover::FindCoordinatesCoverPoint(*ped->m_pCoverPoint, ped, targetPos, coverPos)) {
            // 0x62C676 - The ped is at the cover point | NOTE: Using DistSqr here
            if (CVector2D::DistSqr(ped->GetPosition(), coverPos) < sq(0.75f)) {
                if (ped->m_pCoverPoint->GetUsage() == CCoverPoint::eUsage::LOWCOVER) { // 0x62C6CF - Duck behind it
                    if (targetDist < 12.f && !ped->bNotAllowedToDuck && CGeneral::GetRandomNumber() % 4 == 0 && !ped->GetTaskManager().GetTaskSecondary(TASK_SECONDARY_DUCK)) {
                        ped->Say(CTX_GLOBAL_DUCK);
                        return Finish(new CTaskSimpleDuck{ DUCK_STANDALONE, 2'500 });
                    }
                } else if (CGeneral::GetRandomNumber() % 2 == 0 && !weaponInfo.flags.bThrow) { // 0x62C772 - Step out from behind the wall, shoot, then step back
                    auto* const tGunCtrl = new CTaskSimpleGunControl{
                        m_Target,
                        CVector{},
                        CVector{},
                        eGunCommand::FIREBURST,
                        5,
                        -1
                    };
                    tGunCtrl->m_aimImmidiately = std::exchange(m_AimImmediate, false);
                    m_ShootTimer               = now + 5'000;
                    m_LastAttackTime           = now;
                    m_StrafeEndTime            = now + 2'500;
                    m_StrafeDir                = ped->m_pCoverPoint->GetUsage() == CCoverPoint::eUsage::WALLTOLEFT ? eStrafeDir::LEFT : eStrafeDir::RIGHT;
                    m_StrafeBack               = true;
                    ped->GetIntelligence()->ClearTaskDuckSecondary();
                    return Finish(tGunCtrl);
                }
            }
            ped->ReleaseCoverPoint(); // 0x62C838
        }
    }

    // 0x62C849 - Look for a new cover point, and run to it
    if (m_Competence > 0 && targetDist > 6.f && !isTargetMelee && CGeneral::GetRandomNumber() % 2 != 0) {
        ped->ReleaseCoverPoint();
        if (ped->m_pCoverPoint = CCover::FindAndReserveCoverPoint(ped, targetPos, m_Competence == 2)) {
            CVector    coverPos{}; // NOTE/BUG: Originally uninitialized, and used even if the coordinates couldn't be found
            const bool hasCoverPos = CCover::FindCoordinatesCoverPoint(*ped->m_pCoverPoint, ped, targetPos, coverPos) || !notsa::bugfixes::GenericUB;
            coverPos.z += 1.f;
            if (hasCoverPos && CVector2D::DistSqr(coverPos, targetPos) < sq(weaponRange * 0.75f)) { // 0x62C904 | NOTE: Using DistSqr here
                if (CWorld::GetIsLineOfSightClear(coverPos, ped->GetPosition(), true, true, false, false, false, false, false)) {
                    if (ped->GetGroup()) {
                        ped->Say(CTX_GLOBAL_COVER_ME);
                    }
                    ped->GetIntelligence()->SetTaskDuckSecondary(DUCK_IN_COVER_MS);
                    return Finish(new CTaskSimpleGoToPoint{ PEDMOVE_RUN, coverPos, 0.5f, true });
                }
            }
            ped->ReleaseCoverPoint(); // 0x62CA3A
        }
    }

    // 0x62C9E5 - Move in closer, or forwards for a bit
    CTask* tMoveIn{};
    if (targetDist > 10.f) {
        switch (CGeneral::GetRandomNumber() % 4) {
        case 0: { // 0x62CA43
            if (!ped->bKindaStayInSamePlace) {
                ped->Say(CTX_GLOBAL_MOVE_IN);
                tMoveIn = CreateSeekSubTask(targetDist - 4.f);
            }
            break;
        }
        case 1: { // 0x62CA0E
            if (!ped->bKindaStayInSamePlace) {
                m_StrafeDir     = eStrafeDir::FORWARD;
                m_StrafeEndTime = now + 2'000;
                m_StrafeBack    = false;
            }
            break;
        }
        }
    }

    // 0x62CACA - Strafe to a (random) side that isn't blocked
    if (targetDist > 5.f && CGeneral::GetRandomNumber() % 4 == 0) {
        const bool    isToRight = CGeneral::GetRandomNumber() % 2 != 0;
        const CVector pedPos    = ped->GetPosition();
        const CVector toSide    = ped->GetRight() * (isToRight ? 2.5f : -2.5f);
        const bool    isBlocked = !CWorld::GetIsLineOfSightClear(pedPos, pedPos + toSide, true, true, false, true, false, false, false);
        m_StrafeDir             = isToRight != isBlocked ? eStrafeDir::RIGHT : eStrafeDir::LEFT;
        m_StrafeEndTime         = now + 2'000;
        m_StrafeBack            = false;
    }

    if (tMoveIn) { // 0x62CC04
        return Finish(tMoveIn);
    }

    if (LineOfSightClearForAttack(ped)) { // 0x62CC08
        return FinishWithAttack();
    }

    if (CGeneral::GetRandomNumber() % 4 == 0) { // 0x62CC14
        m_StrafeDir     = CGeneral::GetRandomNumber() < RAND_MAX / 2 ? eStrafeDir::RIGHT : eStrafeDir::LEFT;
        m_StrafeEndTime = now + 2'000;
        m_StrafeBack    = false;
    }
    return FinishWithPause();
}

// 0x62BF00
CTask* CTaskComplexKillPedOnFootArmed::CreateFirstSubTask(CPed* ped) {
    m_NewTarget = false;

    if (!m_Target) {
        return nullptr;
    }

    const CVector& targetPos    = m_Target->GetPosition();
    const float    targetDistSq = CVector::DistSqr(targetPos, ped->GetPosition());

    // 0x62BFCC - Run into cover first | NOTE: Using squared distances here
    const bool isFarEnough = targetDistSq > sq(30.f) || (targetDistSq > sq(6.f) && !m_Target->GetActiveWeapon().IsTypeMelee());
    if (!ped->bStayInSamePlace && CGeneral::GetRandomNumber() % 2 != 0 && m_Competence > 0 && isFarEnough) {
        ped->ReleaseCoverPoint();
        if (ped->m_pCoverPoint = CCover::FindAndReserveCoverPoint(ped, targetPos, false)) {
            CVector    coverPos{}; // NOTE/BUG: Originally uninitialized, and used even if the coordinates couldn't be found
            const bool hasCoverPos = CCover::FindCoordinatesCoverPoint(*ped->m_pCoverPoint, ped, targetPos, coverPos) || !notsa::bugfixes::GenericUB;
            coverPos.z += 1.f;
            if (hasCoverPos && CWorld::GetIsLineOfSightClear(coverPos, ped->GetPosition(), true, true, false, false, false, false, false)) { // 0x62C08F
                ped->GetIntelligence()->SetTaskDuckSecondary(DUCK_IN_COVER_MS);
                return new CTaskSimpleGoToPoint{ PEDMOVE_RUN, coverPos, 0.5f, true };
            }
            ped->ReleaseCoverPoint(); // 0x62C142
        }
    }

    const auto& weaponInfo = ped->GetActiveWeapon().GetWeaponInfo(ped);
    if (targetDistSq <= sq(weaponInfo.m_fTargetRange) * 0.25f && LineOfSightClearForAttack(ped)) { // 0x62C104
        return CreateSubTask(TASK_SIMPLE_GUN_CTRL, ped);
    }
    if (!ped->bStayInSamePlace && !ped->bKindaStayInSamePlace) { // 0x62C12C
        return CreateSubTask(TASK_COMPLEX_SEEK_ENTITY, ped);
    }
    return CreateSubTask(LineOfSightClearForAttack(ped) || weaponInfo.flags.bThrow ? TASK_SIMPLE_PAUSE : TASK_SIMPLE_GUN_CTRL, ped); // 0x62C14B
}

// 0x62CCE0
CTask* CTaskComplexKillPedOnFootArmed::ControlSubTask(CPed* ped) {
    if (m_NewTarget) {
        return CreateFirstSubTask(ped);
    }

    if (!m_Target || m_Target->m_fHealth <= 0.f) {
        return nullptr;
    }

    const uint32   now       = CTimer::GetTimeInMS();
    const CVector& targetPos = m_Target->GetPosition();
    auto* const    intel     = ped->GetIntelligence();

    // 0x62CD55
    const auto IsDuckingByChance = [this] {
        return (uint32)(CGeneral::GetRandomNumberInRange(0, 100)) < m_DuckChancePerc;
    };
    if ((m_DuckingConditions & DUCK_PERIODICALLY) && now > m_LastDuckTime + m_LengthOfDuck + 2'000 && !ped->bIsDucking) { // 0x62CD55
        if (CVector::DistSqr(targetPos, ped->GetPosition()) < sq(20.f) && IsDuckingByChance()) {
            intel->SetTaskDuckSecondary((uint16)(m_LengthOfDuck));
        }
        m_LastDuckTime = now;
    } else if ((m_DuckingConditions & DUCK_WHEN_SHOT_AT_BY_PLAYER) && m_ShotFiredByPlayer && IsDuckingByChance()) { // 0x62CDF0
        if (auto* const tDuck = intel->GetTaskDuck()) {
            tDuck->SetDuckTimer((uint16)(m_LengthOfDuck));
        } else {
            intel->SetTaskDuckSecondary((uint16)(m_LengthOfDuck));
        }
    }

    // 0x62CE4E
    auto* const tDuck = intel->GetTaskDuck();
    if (tDuck) {
        tDuck->m_bIsInControl = true;
    }

    const auto& weaponInfo = ped->GetActiveWeapon().GetWeaponInfo(ped);
    const bool  canMove    = !ped->bStayInSamePlace;
    switch (const auto tt = m_pSubTask->GetTaskType()) {
    case TASK_COMPLEX_SEEK_ENTITY: {
        // 0x62D12D
        const bool isTargetClose  = CVector::DistSqr(targetPos, ped->GetPosition()) < sq(weaponInfo.m_fTargetRange * 0.5f); // NOTE: Using DistSqr here
        const bool isTimeToAttack = m_Target->GetMoveSpeed().Dot(ped->GetForward()) < 0.f || now - m_LastAttackTime > 2'000;
        const bool isInAttackPos  = !canMove || (isTargetClose && isTimeToAttack);
        const bool startAttack    = IsPedInLeaderFiringLine(ped)
            || (isInAttackPos && LineOfSightClearForAttack(ped))
            || (m_Target->physicalFlags.bSubmergedInWater && LineOfSightClearForAttack(ped));
        if (startAttack) { // 0x62D1FF
            return CreateSubTask(TASK_SIMPLE_GUN_CTRL, ped);
        }
        if (ped->GetGroup()) { // 0x62D21A
            ped->Say(CTX_GLOBAL_COVER_ME);
        }
        return m_pSubTask;
    }
    case TASK_SIMPLE_GUN_CTRL: { // 0x62CE8D
        const float targetDist = CVector::Dist(targetPos, ped->GetPosition());

        // Stop shooting if the target is too far (to move in closer), or it can't be seen anymore
        const bool endAttack = targetDist > weaponInfo.m_fTargetRange
            || (targetDist > weaponInfo.m_fTargetRange * 0.5f && canMove && now > m_LastAttackTime + 2'500)
            || (targetDist > 4.f && canMove && now > m_ShootTimer)
            || !LineOfSightClearForAttack(ped);
        if (endAttack) { // 0x62CF32
            auto* const tGunCtrl = notsa::cast<CTaskSimpleGunControl>(m_pSubTask);
            if (tGunCtrl->m_firingTask != eGunCommand::END_LEISURE) {
                tGunCtrl->m_firingTask    = eGunCommand::END_LEISURE;
                tGunCtrl->m_nextAtkTimeMs = 0;
            }
        }

        auto* const tUseGun = intel->GetTaskUseGun(); // 0x62CF44
        if (!tUseGun) {
            return m_pSubTask;
        }

        if (ped->GetGroup()) {
            ped->Say(CTX_GLOBAL_SURROUNDED);
        }

        const auto GetMoveCmd = [&]() -> CVector2D { // 0x62CF7F
            if (m_Target->physicalFlags.bSubmergedInWater) {
                return { 0.f, 0.f };
            }
            if (now < m_StrafeEndTime && canMove) { // 0x62CFC2
                switch (m_StrafeDir) {
                case eStrafeDir::LEFT:    return { -1.f, 0.f };
                case eStrafeDir::RIGHT:   return { 1.f, 0.f };
                case eStrafeDir::FORWARD: return { 0.f, -1.f };
                case eStrafeDir::BACK:    return { 0.f, 1.f };
                default:                  NOTSA_UNREACHABLE();
                }
            }
            if (targetDist < 4.f && canMove) { // 0x62D00F - Too close, back off
                return { 0.f, 1.f };
            }
            if (m_StrafeBack) { // 0x62D03A
                m_StrafeDir = [this] {
                    switch (m_StrafeDir) {
                    case eStrafeDir::LEFT:    return eStrafeDir::RIGHT;
                    case eStrafeDir::RIGHT:   return eStrafeDir::LEFT;
                    case eStrafeDir::FORWARD: return eStrafeDir::BACK;
                    case eStrafeDir::BACK:    return eStrafeDir::FORWARD;
                    default:                  NOTSA_UNREACHABLE();
                    }
                }();
                m_StrafeBack    = false;
                m_StrafeEndTime = now + 2'500;
            }
            return { 0.f, 0.f };
        };
        const CVector2D moveCmd = GetMoveCmd();

        if (!tDuck) { // 0x62D113
            tUseGun->ControlGunMove(moveCmd);
            return m_pSubTask;
        }

        // 0x62D086 - Roll sideways once every 3 seconds at most
        if (m_StrafeDir == eStrafeDir::LEFT || m_StrafeDir == eStrafeDir::RIGHT) {
            if (now <= m_LastRollTime + 3'000) {
                tDuck->ControlDuckMove({ 0.f, 0.f });
                return m_pSubTask;
            }
            m_LastRollTime = now;
        }
        tDuck->ControlDuckMove(moveCmd);
        return m_pSubTask;
    }
    case TASK_SIMPLE_PAUSE:                        // 0x62D253
    case TASK_SIMPLE_STAND_STILL:                  // 0x62D253
    case TASK_SIMPLE_DUCK:                         // 0x62D253
    case TASK_SIMPLE_GO_TO_POINT:                  // 0x62D253
    case TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL: // 0x62D253
    case TASK_SIMPLE_THROW_CTRL:                   // 0x62D253
        return m_pSubTask;
    default:
        NOTSA_UNREACHABLE_CASE(tt);
    }
}
