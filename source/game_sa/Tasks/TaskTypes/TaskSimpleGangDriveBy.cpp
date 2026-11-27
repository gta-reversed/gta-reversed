#include "StdInc.h"

#include "TaskSimpleGangDriveBy.h"
#include "GameLogic.h"

constexpr float DRIVEBY_ANIM_LOOP_START = 0.2667f; // 0x8D2E7C - Aiming loops back to this time
constexpr float DRIVEBY_ANIM_LOOP_END   = 0.4333f; // 0x8D2E80
constexpr float DRIVEBY_ANIM_FIRE_TIME  = 0.3f;    // 0x8D2E84

//! Direction the ped leans out to (NOTSA)
enum eDriveByDirection : int8 {
    DRIVEBY_DIR_FWD   = 0,
    DRIVEBY_DIR_LEFT  = 1,
    DRIVEBY_DIR_BACK  = 2,
    DRIVEBY_DIR_RIGHT = 3,
};

// 0x6217D0
CTaskSimpleGangDriveBy::CTaskSimpleGangDriveBy(CEntity* target, const CVector* targetPos, float abortRange, int8 frequencyPercentage, eDrivebyStyle drivebyStyle, bool seatRHS) :
    m_bSeatRHS{ seatRHS },
    m_nDrivebyStyle{ drivebyStyle },
    m_fAbortRange{ abortRange },
    m_pTargetEntity{ target },
    m_nFrequencyPercentage{ frequencyPercentage },
    m_vecCoords{ targetPos ? *targetPos : CVector{} } {
}

// 0x6236D0
CTaskSimpleGangDriveBy::CTaskSimpleGangDriveBy(const CTaskSimpleGangDriveBy& o) :
    CTaskSimpleGangDriveBy{
        o.m_pTargetEntity,
        &o.m_vecCoords,
        o.m_fAbortRange,
        o.m_nFrequencyPercentage,
        o.m_nDrivebyStyle,
        o.m_bSeatRHS
    }
{
    m_bFromScriptCommand = o.m_bFromScriptCommand;
}

// 0x6218C0
CTaskSimpleGangDriveBy::~CTaskSimpleGangDriveBy() {
    if (m_bAnimsReferenced) {
        CAnimManager::RemoveAnimBlockRef(CAnimManager::GetAnimationBlockIndex(m_nRequiredAnimGroup));
    }
    if (m_pAnimAssoc) {
        m_pAnimAssoc->SetDefaultDeleteCallback();
    }
}

// 0x621BE0
void CTaskSimpleGangDriveBy::DeleteAnimCB(CAnimBlendAssociation* anim, void* data) {
    auto* const self = notsa::cast<CTaskSimpleGangDriveBy>(static_cast<CTask*>(data));
    if (self->m_pAnimAssoc == anim) {
        self->m_pAnimAssoc = nullptr;
    }
}

// NOTSA
static AnimationId GetDriveByAnimId(eDriveByDirection dir, bool seatRHS) {
    switch (dir) {
    case DRIVEBY_DIR_FWD:   return seatRHS ? ANIM_ID_DRIVEBYRHS_FWD : ANIM_ID_DRIVEBYLHS_FWD;
    case DRIVEBY_DIR_LEFT:  return seatRHS ? ANIM_ID_DRIVEBYTOP_RHS : ANIM_ID_DRIVEBYLHS;
    case DRIVEBY_DIR_BACK:  return seatRHS ? ANIM_ID_DRIVEBYRHS_BWD : ANIM_ID_DRIVEBYLHS_BWD;
    case DRIVEBY_DIR_RIGHT: return seatRHS ? ANIM_ID_DRIVEBYRHS : ANIM_ID_DRIVEBYTOP_LHS;
    default:                NOTSA_UNREACHABLE_CASE(dir);
    }
}

// NOTSA - The direction (`eDriveByDirection`) closest to `angle`
static int8 GetDriveByDirection(float angle) {
    const float fromFrontRight = angle + FRAC_PI_4;
    return (int8)(int32)((fromFrontRight < 0.f ? fromFrontRight + TWO_PI : fromFrontRight) * FRAC_2_PI);
}

// 0x621960
void CTaskSimpleGangDriveBy::SetupStaticAnimForPlayer(CPlayerPed* player) {
    auto& cam = TheCamera.GetActiveCam();
    if (cam.m_nMode != MODE_AIMWEAPON_FROMCAR && cam.m_nMode != MODE_TWOPLAYER_IN_CAR_AND_SHOOTING) {
        m_bInRangeToShoot = false;
        m_bInWeaponRange  = true;
    } else {
        m_bInRangeToShoot = true;
        m_bInWeaponRange  = true;

        CVector aim       = cam.m_vecFront;
        if (cam.m_nMode == MODE_TWOPLAYER_IN_CAR_AND_SHOOTING) {
            cam.Get_TwoPlayer_AimVector(aim);
        }
        switch (const auto dir = GetDriveByDirection(CGeneral::LimitRadianAngle(CVector2D{ aim }.Heading() - player->m_pVehicle->GetHeading()))) {
        case DRIVEBY_DIR_FWD:
        case DRIVEBY_DIR_LEFT:
        case DRIVEBY_DIR_BACK:
        case DRIVEBY_DIR_RIGHT:
            m_nRequiredAnimID = GetDriveByAnimId((eDriveByDirection)dir, m_bSeatRHS);
            break;
        default: // 0x621a5d - Anything else keeps the anim
            break;
        }
    }
    if (!CGameLogic::IsCoopGameGoingOn()) {
        TheCamera.SetNewPlayerWeaponMode(MODE_AIMWEAPON_FROMCAR);
    }
    if (auto* const pd = player->GetPlayerData()) {
        pd->m_bFreeAiming = true;
    }
    if (player->IsPlayer()) {
        player->GetPlayerInfoForThisPlayerPed()->m_bCanDoDriveBy = false;
    }
}

// 0x621B10
bool CTaskSimpleGangDriveBy::CheckIfThereNoObstaclesForFiring(CPed* ped, CVector* posn) {
    if (CTimer::GetTimeInMS() > m_nLOSCheckTime) {
        m_nLOSBlocked = false;
        if (m_pTargetEntity) {
            CWorld::pIgnoreEntity = m_pTargetEntity;
            if (!CWorld::GetIsLineOfSightClear(ped->GetPosition() + CVector{ 0.f, 0.f, 0.5f }, *posn, true, false, false, false, false, true, false)) {
                m_nLOSBlocked = true;
            }
            CWorld::pIgnoreEntity = nullptr;
        }
        m_nLOSCheckTime = CTimer::GetTimeInMS() + (int32)CGeneral::GetRandomNumberInRange(0.f, 500.f) + 1'750;
    }
    return !m_nLOSBlocked;
}

// 0x627600
void CTaskSimpleGangDriveBy::SetupStaticAnimForNPC(CPed* ped) {
    const float range      = m_pWeaponInfo->m_fWeaponRange;

    const CVector toTarget = m_pTargetEntity
        ? m_pTargetEntity->GetMatrix().TransformPoint(m_vecCoords) - ped->GetPosition()
        : m_vecCoords - ped->GetPosition();
    const float   dist     = toTarget.Magnitude();
    const float   angle    = CGeneral::LimitRadianAngle(CVector2D{ toTarget }.Heading() - ped->m_pVehicle->GetHeading());

    m_bInRangeToShoot      = false;
    m_bInWeaponRange       = dist < range;
    if (!m_bReachedAbortRange) {
        if (dist < m_fAbortRange) {
            m_bReachedAbortRange = true;
        }
    } else if (dist > m_fAbortRange) { // 0x62777f
        m_nNextCommand = COMMAND_END;
        return;
    }
    if (auto* const target = m_pTargetEntity.Get()) {
        if (target->GetIsTypePed() && target->AsPed()->m_fHealth <= 0.f) { // 0x62779f
            m_nNextCommand = COMMAND_END;
            return;
        }
        if (target->GetIsTypeVehicle() && target->AsVehicle()->GetHealth() <= 0.f) { // 0x6277b7
            m_nNextCommand = COMMAND_END;
            return;
        }
    }

    CVector targetPos = ped->GetPosition() + toTarget;
    if (!CheckIfThereNoObstaclesForFiring(ped, &targetPos)) {
        m_bInRangeToShoot = false;
        return;
    }

    // 0x627825 - The direction(s) the target is in
    int8 closestDir = -1;
    int8 sideDir    = 0; // -1 = left, 1 = right
    int8 fwdBackDir = 0; // -1 = front, 1 = back
    switch (m_nDrivebyStyle) {
    case eDrivebyStyle::FIXED_LHS:
    case eDrivebyStyle::FIXED_RHS:
    case eDrivebyStyle::FIXED_FWD:
    case eDrivebyStyle::FIXED_BAK:
        break;
    default: {
        closestDir = GetDriveByDirection(angle);
        if (angle > DegreesToRadians(15.f) && angle < DegreesToRadians(165.f)) {
            sideDir = -1;
        } else if (angle < -DegreesToRadians(15.f) && angle > -DegreesToRadians(165.f)) {
            sideDir = 1;
        }
        if (angle < DegreesToRadians(75.f) && angle > -DegreesToRadians(75.f)) {
            fwdBackDir = -1;
        } else if (angle > DegreesToRadians(105.f) && angle < -DegreesToRadians(105.f)) { // NOTE/BUG: Never true (`&&` instead of `||`), so `AI_FWD_BAK` never picks the back while no anim is set
            fwdBackDir = 1;
        }
        break;
    }
    }

    // 0x627922 - The direction to use for the style
    const bool noAnim           = m_nRequiredAnimID == ANIM_ID_NO_ANIMATION_SET;
    const auto GetClosestIfSide = [&]() -> int8 {
        return closestDir == DRIVEBY_DIR_LEFT || closestDir == DRIVEBY_DIR_RIGHT ? closestDir : -1;
    };
    const auto GetClosestIfFwdBack = [&]() -> int8 {
        return closestDir == DRIVEBY_DIR_FWD || closestDir == DRIVEBY_DIR_BACK ? closestDir : -1;
    };
    const auto dir = [&]() -> int8 {
        switch (m_nDrivebyStyle) {
        case eDrivebyStyle::FIXED_LHS:      return DRIVEBY_DIR_LEFT;
        case eDrivebyStyle::FIXED_RHS:      return DRIVEBY_DIR_RIGHT;
        case eDrivebyStyle::START_FROM_LHS: return noAnim ? DRIVEBY_DIR_LEFT : GetClosestIfSide();
        case eDrivebyStyle::START_FROM_RHS: return noAnim ? DRIVEBY_DIR_RIGHT : GetClosestIfSide();
        case eDrivebyStyle::AI_SIDE:        { // 0x627952
            if (!noAnim) {
                return GetClosestIfSide();
            }
            return sideDir == -1 ? DRIVEBY_DIR_LEFT
                : sideDir == 1   ? DRIVEBY_DIR_RIGHT
                                 : -1;
        }
        case eDrivebyStyle::FIXED_FWD:  return DRIVEBY_DIR_FWD;
        case eDrivebyStyle::FIXED_BAK:  return DRIVEBY_DIR_BACK;
        case eDrivebyStyle::AI_FWD_BAK: { // 0x62797f
            if (!noAnim) {
                return GetClosestIfFwdBack();
            }
            return fwdBackDir == -1 ? DRIVEBY_DIR_FWD
                : fwdBackDir == 1   ? DRIVEBY_DIR_BACK
                                    : -1;
        }
        case eDrivebyStyle::AI_ALL_DIRN: return closestDir;
        default:                         return -1;
        }
    }();

    // 0x6279b1 - The anim of the direction; in range to shoot if the target is in its arc too
    const auto SetAnim = [&](eDriveByDirection d, bool inArc) {
        m_nRequiredAnimID = GetDriveByAnimId(d, m_bSeatRHS);
        if (dist < range && inArc) {
            m_bInRangeToShoot = true;
        }
    };
    switch (dir) {
    case DRIVEBY_DIR_FWD:   return SetAnim(DRIVEBY_DIR_FWD, angle >= -FRAC_PI_4 && angle <= FRAC_PI_4);
    case DRIVEBY_DIR_LEFT:  return SetAnim(DRIVEBY_DIR_LEFT, angle >= FRAC_PI_4 && angle <= DegreesToRadians(135.f));
    case DRIVEBY_DIR_BACK:  return SetAnim(DRIVEBY_DIR_BACK, angle >= -DegreesToRadians(135.f) || angle <= DegreesToRadians(135.f)); // NOTE: Always true (except NaN), unlike the other arcs
    case DRIVEBY_DIR_RIGHT: return SetAnim(DRIVEBY_DIR_RIGHT, angle >= -DegreesToRadians(135.f) && angle <= -FRAC_PI_4);
    default:                return; // 0x6279b7 - No direction: the anim stays
    }
}

// 0x627B20
bool CTaskSimpleGangDriveBy::BlendAnimation(CPed* ped) {
    auto* const block = [this] {
        auto* const b = CAnimManager::GetAnimationBlock(m_nRequiredAnimGroup);
        return b ? b : CAnimManager::GetAnimationBlock(CAnimManager::GetAnimBlockName(m_nRequiredAnimGroup));
    }();
    const auto blockIdx = CAnimManager::GetAnimationBlockIndex(block);
    if (!block->IsLoaded) {
        CStreaming::RequestModel(IFPToModelId(blockIdx), STREAMING_KEEP_IN_MEMORY);
        return false;
    }
    if (!m_bAnimsReferenced) {
        CAnimManager::AddAnimBlockRef(blockIdx);
        m_bAnimsReferenced = true;
    }
    if (m_nRequiredAnimID == ANIM_ID_NO_ANIMATION_SET && !m_pAnimAssoc) {
        return false;
    }

    const auto ShouldBlend = [this] {
        if (!m_pAnimAssoc) {
            return m_nAttackTimer <= 0 && m_bInWeaponRange;
        }
        if (m_pAnimAssoc->GetAnimId() == m_nRequiredAnimID) {
            return false;
        }
        switch (m_nNextCommand) {
        case COMMAND_AIM:
        case COMMAND_FIRE:
        case COMMAND_FIRE_BURST:
            return m_nLastCommand < COMMAND_UNK_4;
        default:
            return false;
        }
    };
    if (ShouldBlend()) { // 0x627bd9
        const bool hadAnim = m_pAnimAssoc != nullptr;
        if (hadAnim) {
            m_pAnimAssoc->SetDefaultDeleteCallback();
            m_pAnimAssoc->SetFlag(ANIMATION_IS_PLAYING, false);
        }
        m_pAnimAssoc = CAnimManager::BlendAnimation(ped->GetRpClump(), m_nRequiredAnimGroup, (AnimationId)m_nRequiredAnimID, 4.f);
        m_pAnimAssoc->SetDeleteCallback(DeleteAnimCB, this);
        if (m_nNextCommand == COMMAND_FIRE_BURST && m_nBurstShots <= 1) {
            m_nBurstShots = (int8)CGeneral::GetRandomNumberInRange(2, (int32)(int16)m_pWeaponInfo->m_nAmmoClip);
        }
        if (hadAnim) { // 0x627c48
            m_pAnimAssoc->SetCurrentTime(DRIVEBY_ANIM_LOOP_START);
            m_pAnimAssoc->SetFlag(ANIMATION_IS_PLAYING, false);
            m_nLastCommand = COMMAND_AIM;
            m_nNextCommand = COMMAND_NONE;
            if (!ped->IsPlayer()) {
                m_nAttackTimer = 100;
            }
        } else {
            m_nLastCommand = m_nNextCommand;
            m_nNextCommand = COMMAND_NONE;
        }
    }
    return m_pAnimAssoc && m_pAnimAssoc->GetBlendAmount() > 0.9f && m_pAnimAssoc->GetBlendDelta() >= 0.f;
}

// 0x627CC0
int8 CTaskSimpleGangDriveBy::FireGun(CPed* ped) {
    return plugin::CallMethodAndReturn<int8, 0x627CC0, CTaskSimpleGangDriveBy*, CPed*>(this, ped);
}

// 0x628350
int8 CTaskSimpleGangDriveBy::ProcessAiming(CPed* ped) {
    return plugin::CallMethodAndReturn<int8, 0x628350, CTaskSimpleGangDriveBy*, CPed*>(this, ped);
}

// 0x62D290
bool CTaskSimpleGangDriveBy::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    switch (priority) {
    case ABORT_PRIORITY_URGENT:
    case ABORT_PRIORITY_IMMEDIATE:
        break;
    default:
        m_nNextCommand = COMMAND_ABORT_REQUESTED;
        return false;
    }
    if (const auto* const eDamage = notsa::dyn_cast_if_present<CEventDamage>(event); eDamage && !eDamage->HasKilledPed()) {
        return false;
    }

    m_nNextCommand = COMMAND_END;
    if (BlendAnimation(ped)) {
        m_nLastCommand = m_nNextCommand;
        if (priority == ABORT_PRIORITY_IMMEDIATE) {
            m_pAnimAssoc->SetBlendDelta(-1000.f);
        } else if (!m_pAnimAssoc->IsPlaying()) {
            m_pAnimAssoc->SetFlag(ANIMATION_IS_PLAYING);
        }
    }
    if (auto* const pd = ped->GetPlayerData()) {
        pd->m_bFreeAiming = true;
    }
    if (ped->IsPlayer()) {
        ped->AsPlayer()->GetPlayerInfoForThisPlayerPed()->m_bCanDoDriveBy = true;
    }
    if (m_pAnimAssoc) {
        if (!m_pAnimAssoc->IsPlaying()) { // 0x62d348
            const auto delta = m_pAnimAssoc->GetBlendDelta();
            if (delta > 0.f || (m_pAnimAssoc->GetBlendAmount() > 0.f && delta >= 0.f)) {
                m_pAnimAssoc->SetBlendDelta(-8.f);
            }
        }
        m_pAnimAssoc->SetDefaultDeleteCallback();
        m_pAnimAssoc = nullptr;
    }
    m_bIsFinished = true;
    return true;
}

// 0x62D3B0
bool CTaskSimpleGangDriveBy::ProcessPed(CPed* ped) {
    if (const auto* const wi = CWeaponInfo::GetWeaponInfo(ped); wi && wi->GetFireType() != WEAPON_FIRE_INSTANT_HIT) {
        return true;
    }
    if (ped->m_pVehicle && !ped->m_pVehicle->CanPedLeanOut(ped)) {
        return true;
    }
    if (m_bIsFinished) {
        return true;
    }
    if (!m_nLOSCheckTime) {
        m_nLOSCheckTime = CTimer::GetTimeInMS() + CGeneral::GetRandomNumberInRange(0, 1'000);
    }
    if (!m_pWeaponInfo) {
        m_pWeaponInfo = CWeaponInfo::GetWeaponInfo(ped);
    }
    if (!ped->IsInVehicle() || ped->m_fHealth < 1.f) { // 0x62d9ab
        MakeAbortable(ped, ABORT_PRIORITY_URGENT);
        return true;
    }
    if (!ped->IsPlayer()) {
        ped->bTestForShotInVehicle = true;
    }

    auto* const veh = ped->m_pVehicle;
    if (m_nRequiredAnimGroup == ANIM_GROUP_DEFAULT) { // 0x62d4b7 - Pick the anim group (and the side for bikes / quads)
        if (veh->IsSubQuad()) {
            m_nRequiredAnimGroup = ANIM_GROUP_QUAD_DBZ;
            m_bSeatRHS           = false;
        } else if (veh->IsBike()) { // 0x62d4d5
            m_nRequiredAnimGroup = veh->AsBike()->m_RideAnimData.AnimGroup == ANIM_GROUP_BIKES && veh->IsDriver(ped)
                ? ANIM_GROUP_COP_DBZ
                : ANIM_GROUP_BIKE_DBZ;
            m_bSeatRHS           = false;
        } else {
            if (veh->IsSubAutomobile()) { // 0x62d504 - Open the window of a passenger's door
                const auto& passengers = veh->m_apPassengers;
                const auto  door       = [&]() -> std::optional<eCarNodes> {
                    if (ped == passengers[0]) {
                        m_bSeatRHS = true;
                        return CAR_DOOR_RF;
                    }
                    if (ped == passengers[1]) {
                        m_bSeatRHS = false;
                        return CAR_DOOR_LR;
                    }
                    if (ped == passengers[2]) {
                        m_bSeatRHS = true;
                        return CAR_DOOR_RR;
                    }
                    return std::nullopt;
                }();
                if (door) {
                    veh->SetWindowOpenFlag(*door);
                }
            }
            m_nRequiredAnimGroup = ANIM_GROUP_DRIVEBYS;
        }
        if (!ped->IsPlayer() && !ped->m_pWeaponObject) { // 0x62d551
            ped->AddWeaponModel(CWeaponInfo::GetWeaponInfo(ped->GetActiveWeapon().GetType(), eWeaponSkill::STD)->m_nModelId1);
        }
    }

    if (ped->IsPlayer()) {
        SetupStaticAnimForPlayer(ped->AsPlayer());
    } else {
        SetupStaticAnimForNPC(ped);
    }
    if (m_nRequiredAnimGroup == ANIM_GROUP_COP_DBZ && veh->IsBike()) { // 0x62d5a7
        veh->AsBike()->m_nFixLeftHand = true;
    }

    if (BlendAnimation(ped)) {
        auto* const anim          = m_pAnimAssoc;
        const auto  HasJustPassed = [anim](float t) { // NOTE: `m_TimeStep` has no accessor on master
            return anim->GetCurrentTime() > t && anim->GetCurrentTime() - anim->m_TimeStep <= t;
        };

        // 0x62d5d3 - Fire at the fire time of the anim
        if (anim->IsPlaying() && HasJustPassed(DRIVEBY_ANIM_FIRE_TIME)) {
            if (m_nLastCommand == COMMAND_FIRE || m_nLastCommand == COMMAND_FIRE_BURST) {
                FireGun(ped);
            }
        }

        // 0x62d612 - Start / stop the anim
        const bool isPlaying     = anim->IsPlaying();
        const auto IsFireCommand = [](char cmd) {
            return cmd == COMMAND_FIRE || cmd == COMMAND_FIRE_BURST;
        };
        if (!isPlaying && m_nAttackTimer <= 0 && IsFireCommand(m_nNextCommand) && (anim->GetHier()->GetTotalTime() > anim->GetCurrentTime() || anim->GetBlendDelta() >= 0.f)) { // 0x62d656
            anim->SetFlag(ANIMATION_IS_PLAYING);
            // NOTE: `m_nAmmoClip` is read as a signed 16 bit value
            m_nBurstShots  = (int8)CGeneral::GetRandomNumberInRange(1, (int32)((float)(int16)m_pWeaponInfo->m_nAmmoClip * 0.5f));
            m_nLastCommand = m_nNextCommand;
            m_nNextCommand = COMMAND_NONE;
        } else if (!isPlaying && m_nNextCommand == COMMAND_END) { // 0x62d698
            anim->SetFlag(ANIMATION_IS_PLAYING);
            m_nLastCommand = m_nNextCommand;
            m_nNextCommand = COMMAND_END;
        } else if (!isPlaying && anim->GetAnimId() != m_nRequiredAnimID) { // 0x62d6b5
            anim->SetFlag(ANIMATION_IS_PLAYING);
        } else if (m_nLastCommand == COMMAND_AIM && isPlaying && anim->GetAnimId() == m_nRequiredAnimID && HasJustPassed(DRIVEBY_ANIM_LOOP_START)) { // 0x62d6f1
            anim->SetFlag(ANIMATION_IS_PLAYING, false);
            anim->SetCurrentTime(DRIVEBY_ANIM_LOOP_START);
        }

        // 0x62d703 - The end of the anim's loop
        if (m_pAnimAssoc->GetAnimId() == m_nRequiredAnimID && HasJustPassed(DRIVEBY_ANIM_LOOP_END)) {
            if (ped->GetActiveWeapon().GetState() == WEAPONSTATE_RELOADING && m_nNextCommand <= COMMAND_FIRE) { // 0x62d758
                m_nNextCommand = COMMAND_FIRE;
                m_nAttackTimer = 2'000;
            } else if ((IsFireCommand(m_nNextCommand) || (m_nLastCommand == COMMAND_FIRE_BURST && m_nBurstShots > 0)) && m_bInRangeToShoot) { // 0x62d782
                anim->SetCurrentTime(DRIVEBY_ANIM_LOOP_START);
                m_pAnimAssoc->SetFlag(ANIMATION_IS_PLAYING);
                m_nLastCommand = std::max(m_nLastCommand, m_nNextCommand);
                m_nNextCommand = COMMAND_NONE;
                if (m_nLastCommand == COMMAND_FIRE_BURST) {
                    m_nBurstShots--;
                } else {
                    m_nBurstShots = 0;
                }
            } else if (m_nNextCommand == COMMAND_AIM) { // 0x62d7b8
                anim->SetCurrentTime(DRIVEBY_ANIM_LOOP_START);
                m_pAnimAssoc->SetFlag(ANIMATION_IS_PLAYING, false);
                m_nLastCommand = COMMAND_AIM;
                m_nNextCommand = COMMAND_NONE;
                m_nAttackTimer = -1;
            }
        }
    }

    // 0x62d7d8
    if (m_nLastCommand != COMMAND_NONE && m_nLastCommand < COMMAND_UNK_4 && m_pAnimAssoc) {
        if (m_pAnimAssoc->GetBlendAmount() > 0.5f || (m_pAnimAssoc->GetBlendDelta() > 0.f && !m_pAnimAssoc->IsPlaying())) {
            ProcessAiming(ped);
        }
    }

    // 0x62d818
    m_nAttackTimer = m_nAttackTimer < 0
        ? (int16)CGeneral::GetRandomNumberInRange(200, (100 - m_nFrequencyPercentage) * 100)
        : (int16)(m_nAttackTimer - (int32)CTimer::GetTimeStepInMS());

    // 0x62d860 - The next command
    if (ped->IsPlayer() && m_nNextCommand < COMMAND_UNK_4) {
        m_nAttackTimer = 0;
        m_nNextCommand = ped->AsPlayer()->GetPadFromPlayer()->GetWeapon(ped) ? COMMAND_FIRE : COMMAND_AIM;
    } else if (m_nNextCommand == COMMAND_NONE) {
        switch (m_nLastCommand) {
        case COMMAND_AIM: { // 0x62d8a9
            if (!m_nFrequencyPercentage && m_bInWeaponRange) {
                m_nNextCommand = COMMAND_AIM;
            } else if (m_bInRangeToShoot) {
                m_nNextCommand = COMMAND_FIRE_BURST;
            } else if (m_nAttackTimer <= 0) {
                m_nAttackTimer = (int16)((int32)CTimer::GetTimeStepInMS() * 2);
            }
            break;
        }
        case COMMAND_ABORT_REQUESTED:
        case COMMAND_END: // 0x62d8e9
            m_nNextCommand = COMMAND_END;
            break;
        default: // 0x62d8ef
            if (m_bInWeaponRange) {
                m_nNextCommand = COMMAND_AIM;
            }
            break;
        }
    }

    // 0x62d8fa
    if (m_nLastCommand != COMMAND_UNK_4 && !m_pAnimAssoc && (m_nLastCommand == COMMAND_END || m_nNextCommand == COMMAND_END)) {
        m_bIsFinished = true;
    }
    if (ped->IsPlayer()) {
        ped->GetActiveWeapon().Update(ped);
    }

    // 0x62d939 - Complain about the speed of the vehicle
    const auto speedSq = CVector2D{ veh->GetMoveSpeed() }.SquaredMagnitude();
    if (speedSq > 0.5f) {
        ped->Say(CTX_GLOBAL_CAR_DRIVEBY_TOO_FAST);
    } else if (speedSq < 0.01f) {
        ped->Say(CTX_GLOBAL_CAR_DRIVEBY_BURN_RUBBER);
    }
    return false;
}

void CTaskSimpleGangDriveBy::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleGangDriveBy, 0x86d944, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedConstructorInstall(0x6217D0, "1", {}, CEntity*, const CVector*, float, char, eDrivebyStyle, bool);
    RH_ScopedDestructorInstall(0x6218C0);

    RH_ScopedInstall(SetupStaticAnimForPlayer, 0x621960);
    RH_ScopedInstall(CheckIfThereNoObstaclesForFiring, 0x621B10);
    RH_ScopedInstall(SetupStaticAnimForNPC, 0x627600);
    RH_ScopedInstall(BlendAnimation, 0x627B20);
    RH_ScopedInstall(FireGun, 0x627CC0, { .Reversed = false });
    RH_ScopedInstall(ProcessAiming, 0x628350, { .Reversed = false });
    RH_ScopedInstall(DeleteAnimCB, 0x621BE0);
    RH_ScopedVMTInstall(Clone, 0x6236D0);
    RH_ScopedVMTInstall(GetTaskType, 0x6218B0);
    RH_ScopedVMTInstall(MakeAbortable, 0x62D290);
    RH_ScopedVMTInstall(ProcessPed, 0x62D3B0);
}
