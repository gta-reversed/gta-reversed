#include "StdInc.h"
#include "TaskSimpleGunControl.h"
#include "TaskSimpleUseGun.h"
#include "TaskSimpleDuck.h"

void CTaskSimpleGunControl::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleGunControl, 0x86d76c, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x61F3F0);

    RH_ScopedInstall(Destructor, 0x61F4D0);

    RH_ScopedVMTInstall(Clone, 0x622FB0);
    RH_ScopedVMTInstall(GetTaskType, 0x61F4C0);
    RH_ScopedVMTInstall(MakeAbortable, 0x61F530);
    RH_ScopedVMTInstall(ProcessPed, 0x625270);
}

// Not the actual ctor used originally
// because that took pointers instead for the CVector's, but that's stupid, because
// they just 0 inited it otherwise, thus we can just default init the args
// instead of passing nullptr, and it's going to be the same effect.
// so, tldr, instead of `nullptr` for the CVector&'s pass in `{}`
CTaskSimpleGunControl::CTaskSimpleGunControl(CEntity* targetEntity, CVector const& targetPos, CVector const& moveTarget, eGunCommand firingTask, int16 burstAmmoCnt, int32 leisureDurMs) :
    m_targetPos{targetPos},
    m_moveTarget{moveTarget},
    m_targetEntity{targetEntity},
    m_leisureDurMs{leisureDurMs},
    m_firingTask{firingTask},
    m_burstAmmoCnt{burstAmmoCnt},
    m_attackIntervalMult{1.f}
{
    CEntity::SafeRegisterRef(m_targetEntity);
}

CTaskSimpleGunControl::CTaskSimpleGunControl(const CTaskSimpleGunControl& o) :
    CTaskSimpleGunControl{o.m_targetEntity, o.m_targetPos, o.m_moveTarget, o.m_firingTask, o.m_burstAmmoCnt, o.m_leisureDurMs}
{
    m_aimImmidiately = o.m_aimImmidiately;
}

// 0x61F4D0
CTaskSimpleGunControl::~CTaskSimpleGunControl() {
    CEntity::SafeCleanUpRef(m_targetEntity);
}

// 0x61F530
bool CTaskSimpleGunControl::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
    const auto urgent = priority == ABORT_PRIORITY_URGENT;

    if (urgent && event) {
        if (event->GetEventPriority() < 22) {
            return false;
        }

        switch (event->GetEventType()) {
        case EVENT_VEHICLE_COLLISION:
        case EVENT_PED_COLLISION_WITH_PED:
        case EVENT_PED_COLLISION_WITH_PLAYER:
        case EVENT_PLAYER_COLLISION_WITH_PED:
        case EVENT_OBJECT_COLLISION:
        case EVENT_BUILDING_COLLISION:
            return false;
        default:
            break;
        }
        
    }

    if (const auto useGun = ped->GetIntelligence()->GetTaskUseGun()) {
        useGun->m_CountDownFrames = urgent ? 2 : 10;
        useGun->m_HasFiredGun = false;
    }

    ForceStopDuckingMove(ped);

    return true;
}

// 0x625270
bool CTaskSimpleGunControl::ProcessPed(CPed* ped) {
    if (m_isFinished) {
        if (const auto useGun = ped->GetIntelligence()->GetTaskUseGun()) {
            useGun->m_CountDownFrames = std::min<uint8>(useGun->m_CountDownFrames, 4u);
            useGun->m_HasFiredGun = false;
        }
        ForceStopDuckingMove(ped);      
        return true;
    }

    switch (m_firingTask) {
    case eGunCommand::END_LEISURE:
    case eGunCommand::END_NOW:
        break;

    default: {
        if (const auto attackTask = ped->GetTaskManager().GetTaskSecondary(TASK_SECONDARY_ATTACK)) {
            if (const auto useGun = CTask::DynCast<CTaskSimpleUseGun>(attackTask)) { // Inverted
                if (m_isFirstTime) {
                    m_nextAtkTimeMs = 0;
                    m_isFirstTime = false;
                    useGun->Reset(ped, m_targetEntity, m_targetPos, eGunCommand::AIM, m_burstAmmoCnt);
                }
            } else {
                attackTask->MakeAbortable(ped);
                return false;
            }
        } else {
            const auto useGun = new CTaskSimpleUseGun{
                m_targetEntity,
                m_targetPos,
                eGunCommand::AIM,
                (uint16)m_burstAmmoCnt,
                m_aimImmidiately
            };
            ped->GetTaskManager().SetTaskSecondary(useGun, TASK_SECONDARY_ATTACK);
            m_aimImmidiately = false;
        m_nextAtkTimeMs = 0;
        m_isFirstTime = false;
        }
        break;
    }
    }

    const auto useGunTask = ped->GetIntelligence()->GetTaskUseGun();

    // Moved from 0x6254D3
    const auto& winfo = [&, this] {
        if (!useGunTask || !useGunTask->m_WeaponInfo) {
            return ped->GetActiveWeapon().GetWeaponInfo(ped);
        }
        return *useGunTask->m_WeaponInfo;
    }();

    // Play gunstand animation if necessary
    if ([&, this]{
        if (ped->IsGangster()) { // 0x625437
            return true;
        }

        if (useGunTask) { // 0x625459
            if (const auto wi = useGunTask->m_WeaponInfo) {
                if (wi->flags.bAimWithArm) {
                    return true;
                }
            }
        }

        return false;
    }()) {
        // 0x62546E
        const auto anim = RpAnimBlendClumpGetAssociation(ped->m_pRwClump, ANIM_ID_GANG_GUNSTAND);
        if (!anim || (anim->m_BlendAmount < 1.f && anim->m_BlendDelta <= 0.f)) {
            CAnimManager::BlendAnimation(ped->m_pRwClump, ANIM_GROUP_DEFAULT, ANIM_ID_GANG_GUNSTAND);
        }
    }

    // Calculate next gun command
    auto [nextGunCmd, dontCheckLeisureDur] = [&, this]() -> std::pair<eGunCommand, bool> {
        using enum eGunCommand;

        if (ped->GetActiveWeapon().m_State == WEAPONSTATE_RELOADING && winfo.flags.bReload) {
            return { RELOAD, false }; // 0x625517
        }

        switch (m_firingTask) {
        case NONE:
            return { AIM, false };
        case AIM: {
            if (!m_nextAtkTimeMs) {
                return { FIREBURST, false };
            }
            return { AIM, true };
        }
        case FIRE:
            return { m_nextAtkTimeMs ? AIM : FIREBURST, false }; // Inverted ternary

        case FIREBURST: {
            if (CTimer::GetTimeInMS() < m_nextAtkTimeMs) {
                if (m_nextAtkTimeMs == (uint32)-1) { // 0x625600
                    if (useGunTask && (eGunCommand)useGunTask->m_LastCmd != FIREBURST) {
                        m_nextAtkTimeMs = 0;
                    }
                }
                return { AIM, false };
            }


            m_nextAtkTimeMs = (uint32)-1;
            m_burstAmmoCnt = winfo.m_nAmmoClip;
            if (m_burstAmmoCnt > 1) {
                if (ped->m_nWeaponShootingRate < 100u) {
                    m_burstAmmoCnt = std::min((int16)(4 * ped->m_nWeaponShootingRate / 30), m_burstAmmoCnt);
                    m_burstAmmoCnt = std::max(m_burstAmmoCnt - CGeneral::GetRandomNumberInRange(0, 2), 1);
                }
            }

            if (useGunTask) {
                useGunTask->m_BurstLength = m_burstAmmoCnt;
            }

            return { FIREBURST, false };
        }
        case RELOAD: {
            if (!useGunTask || (eGunCommand)useGunTask->m_LastCmd != RELOAD) {
                return { FIREBURST, false };
            }
            m_firingTask = END_LEISURE;
            return { END_LEISURE, false };
        }
        case PISTOLWHIP:
            return { FIREBURST, false };
        case END_LEISURE:
            return { END_LEISURE, true };
        default:
            NOTSA_UNREACHABLE();
        }
    }();

    if (m_leisureDurMs >= 0) {
        m_leisureDurMs = std::max(m_leisureDurMs - (int32)CTimer::GetTimeStepInMS(), 0);
    }

    if (dontCheckLeisureDur || !m_leisureDurMs) { // 0x625681
        using enum eGunCommand;

            if (useGunTask && (m_firingTask != END_LEISURE || !useGunTask->m_IsFinished)) {
                if (useGunTask->m_HasFiredGun) {
                    switch ((eGunCommand)useGunTask->m_LastCmd) {
                    case FIRE:
                    case FIREBURST:
                        break;
                default: {
                    useGunTask->m_CountDownFrames = 2;
                    useGunTask->m_HasFiredGun = false;
                    }
                }
            }
            m_isFinished = true;
        }

        if (m_isFinished) {
            ForceStopDuckingMove(ped);
            return true;
        }

        if (!m_leisureDurMs && useGunTask) {
            m_firingTask = END_LEISURE;
            useGunTask->ControlGun(ped, m_targetEntity, END_LEISURE);
        }
    }

    // Calculate next attack time - 0x625719
    if (!m_nextAtkTimeMs) {
        float period = (float)(winfo.m_nWeaponFire == eWeaponFire::WEAPON_FIRE_PROJECTILE
            ? DEFAULT_GUN_ATTACK_PERIOD * 2
            : DEFAULT_GUN_ATTACK_PERIOD);

        period *= CGeneral::GetRandomNumberInRange(0.75f, 1.25f);
        period /= (float)ped->m_nWeaponShootingRate * m_attackIntervalMult * 0.04f;

        m_nextAtkTimeMs = CTimer::GetTimeInMS() + (uint32)period;
    }

    bool isRightArmBlockedForPistolWhip{};
    std::tie(nextGunCmd, isRightArmBlockedForPistolWhip) = [&, this]() -> std::tuple<eGunCommand, bool> {
        if (nextGunCmd == eGunCommand::FIREBURST && CTaskSimpleUseGun::RequirePistolWhip(ped, m_targetEntity)) { // 0x6257A1 
            return { eGunCommand::PISTOLWHIP, false };
        } else if (ped->IsRightArmBlockedNow()) { // 0x6257E2
            return { eGunCommand::AIM, true };
        }
        return { nextGunCmd, false };
    }();

    if (useGunTask) {
        useGunTask->ControlGun(ped, m_targetEntity, nextGunCmd);
    }

    CVector pedToTarget{};
    if (!GetPedToTarget(ped, pedToTarget)) {
        return false;
    }

    ped->m_fAimingRotation = pedToTarget.Heading();

    if ([&, this] {
        if (!useGunTask) {
            return false;
        }

        if (ped->bStayInSamePlace) {
            return false;
        }

        if (!isRightArmBlockedForPistolWhip) {
            if (pedToTarget.SquaredMagnitude() >= 6.f) {
                if ((eGunCommand)useGunTask->m_LastCmd == eGunCommand::FIREBURST) {
                    if (winfo.flags.bMoveFire) {
                        return false;
                    }
                }
            }
        }
        return true;
    }()) {
        useGunTask->ControlGunMove({ 0.f, 1.f });
    } else if (m_targetEntity) {
        if (!ped->bIgnoreWeaponRange) {
            if (pedToTarget.SquaredMagnitude() > sq(winfo.m_fWeaponRange)) {
                m_leisureDurMs = 0;
            }
        }
    }

    return false;
}

void CTaskSimpleGunControl::ForceStopDuckingMove(CPed* const ped) {
    if (ped->bIsDucking) {
        if (const auto duck = ped->GetIntelligence()->GetTaskDuck()) {
            duck->ForceStopMove();
        }
    }
}

bool CTaskSimpleGunControl::GetPedToTarget(const CPed* ped, CVector& out) {
    if (m_targetEntity) {
        out = m_targetEntity->GetPosition() - ped->GetPosition();
        return true;
    }

    if (!CVector2D{ m_targetPos }.IsZero()) {
        out = m_targetPos - ped->GetPosition();
        return true;
    }
     
    return false;
}
