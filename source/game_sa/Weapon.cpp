/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#include "StdInc.h"

#include "Weapon.h"

#include "WeaponInfo.h"
#include "CreepingFire.h"
#include "BulletInfo.h"
#include "InterestingEvents.h"
#include "Shadows.h"
#include "Birds.h"

float& CWeapon::ms_fExtinguisherAimAngle = *(float*)0x8D610C;
bool& CWeapon::bPhotographHasBeenTaken = *(bool*)0xC8A7C0;
bool& CWeapon::ms_bTakePhoto = *(bool*)0xC8A7C1;
CColModel& CWeapon::ms_PelletTestCol = *(CColModel*)0xC8A7DC;
float& fPlayerAimScale = *(float*)0x8D6110;
float& fPlayerAimScaleDist = *(float*)0x8D6114;
float& fPlayerAimRotRate = *(float*)0x8D6118;
float& SHOTGUN_SPREAD_RATE = *(float*)0x8D611C;
uint32& SHOTGUN_NUM_PELLETS = *(uint32*)0x8D6120;
uint32& SPAS_NUM_PELLETS = *(uint32*)0x8D6124;
float& PELLET_COL_SCALE_RATIO_MULT = *(float*)0x8D6128;
float* fReloadAnimSampleFraction = (float*)0x8D612C;

void CWeapon::InjectHooks() {
    RH_ScopedClass(CWeapon);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Constructor, 0x73B430);

    RH_ScopedInstall(Shutdown, 0x73A380);
    RH_ScopedInstall(Reload, 0x73AEB0);
    RH_ScopedInstall(IsTypeMelee, 0x73B1C0);
    RH_ScopedInstall(IsType2Handed, 0x73B1E0);
    RH_ScopedInstall(IsTypeProjectile, 0x73B210);
    RH_ScopedInstall(HasWeaponAmmoToBeUsed, 0x73B2A0);
    RH_ScopedInstall(InitialiseWeapons, 0x73A300);
    RH_ScopedInstall(ShutdownWeapons, 0x73A330);
    RH_ScopedInstall(UpdateWeapons, 0x73A360);
    RH_ScopedInstall(AddGunshell, 0x73A3E0);
    RH_ScopedInstall(ProcessLineOfSight, 0x73B300);
    RH_ScopedInstall(StopWeaponEffect, 0x73B360);
    RH_ScopedInstall(TargetWeaponRangeMultiplier, 0x73B380);
    RH_ScopedInstall(Initialise, 0x73B4A0);
    RH_ScopedInstall(DoWeaponEffect, 0x73E690);
    RH_ScopedInstall(FireSniper, 0x73AAC0);
    RH_ScopedInstall(TakePhotograph, 0x73C1F0);
    RH_ScopedInstall(DoDoomAiming, 0x73CDC0);
    RH_ScopedInstall(GenerateDamageEvent, 0x73A530);
    RH_ScopedInstall(FireInstantHitFromCar2, 0x73CBA0);
    RH_ScopedInstall(Update, 0x73DB40);
    RH_ScopedInstall(SetUpPelletCol, 0x73C710, { .reversed = false });
    RH_ScopedInstall(FireAreaEffect, 0x73E800);
    RH_ScopedInstall(FireInstantHitFromCar, 0x73EC40, { .reversed = false });
    RH_ScopedInstall(FireFromCar, 0x73FA20);
    RH_ScopedInstall(FireInstantHit, 0x73FB10, { .reversed = false });
    RH_ScopedInstall(FireProjectile, 0x741360, { .reversed = false });
    RH_ScopedInstall(DoBulletImpact, 0x73B550, { .reversed = false });
    RH_ScopedInstall(LaserScopeDot, 0x73A8D0);
    RH_ScopedInstall(FireM16_1stPerson, 0x741C00);
    RH_ScopedInstall(Fire, 0x742300);
    RH_ScopedGlobalInstall(DoTankDoomAiming, 0x73D1E0, { .reversed = false });
    RH_ScopedGlobalInstall(DoDriveByAutoAiming, 0x73D720, { .reversed = false });
    RH_ScopedGlobalInstall(FindNearestTargetEntityWithScreenCoors, 0x73E240, { .reversed = false });
    RH_ScopedGlobalInstall(EvaluateTargetForHeatSeekingMissile, 0x73E560, { .reversed = false });
    RH_ScopedGlobalInstall(CheckForShootingVehicleOccupant, 0x73F480, { .reversed = false });
    RH_ScopedGlobalInstall(PickTargetForHeatSeekingMissile, 0x73F910, { .reversed = false });
    RH_ScopedOverloadedInstall(CanBeUsedFor2Player, "Static", 0x73B240, bool(*)(eWeaponType));
    RH_ScopedOverloadedInstall(CanBeUsedFor2Player, "Method", 0x73DEF0, bool(CWeapon::*)(), { .reversed = false });
}

// 0x73B430
CWeapon::CWeapon(eWeaponType weaponType, int32 ammo) {
    m_nType = weaponType;
    m_nState = eWeaponState::WEAPONSTATE_READY;
    m_nAmmoInClip = 0;
    m_nTotalAmmo = std::min(ammo, 99999);

    Reload();

    m_nTimeForNextShot = 0;
    field_14 = 0;
    m_pFxSystem = nullptr;
    m_bNoModel = false;
}

CWeapon* CWeapon::Constructor(eWeaponType weaponType, int32 ammo) {
    this->CWeapon::CWeapon(weaponType, ammo);
    return this;
}

// 0x73B4A0
void CWeapon::Initialise(eWeaponType weaponType, int32 ammo, CPed* owner) {
    m_nType = weaponType;
    m_nState = eWeaponState::WEAPONSTATE_READY;
    m_nAmmoInClip = 0;
    m_nTotalAmmo = std::min(ammo, 99999);

    Reload(owner);

    m_nTimeForNextShot = 0;

    int32 model1 = CWeaponInfo::GetWeaponInfo(weaponType, eWeaponSkill::STD)->m_nModelId1;
    int32 model2 = CWeaponInfo::GetWeaponInfo(weaponType, eWeaponSkill::STD)->m_nModelId2;

    if (model1 != -1)
        CModelInfo::GetModelInfo(model1)->AddRef();

    if (model2 != -1)
        CModelInfo::GetModelInfo(model2)->AddRef();

    m_pFxSystem = nullptr;
    m_bNoModel = false;
}

// 0x73A300
void CWeapon::InitialiseWeapons() {
    CWeaponInfo::Initialise();
    CShotInfo::Initialise();
    CExplosion::Initialise();
    CProjectileInfo::Initialise();
    CBulletInfo::Initialise();

    bPhotographHasBeenTaken = false;
    ms_bTakePhoto = false;
}

// 0x73A330
void CWeapon::ShutdownWeapons() {
    CWeaponInfo::Shutdown();
    CShotInfo::Shutdown();
    CExplosion::Shutdown();
    CProjectileInfo::Shutdown();
    CBulletInfo::Shutdown();

    ms_PelletTestCol.RemoveCollisionVolumes();
}

// 0x73A380
void CWeapon::Shutdown() {
    for (const auto m : CWeaponInfo::GetWeaponInfo(m_nType)->GetModels()) {
        if (m != MODEL_INVALID) {
            CModelInfo::GetModelInfo(m)->RemoveRef();
        }
    }
    m_nType            = eWeaponType::WEAPON_UNARMED;
    m_nState           = eWeaponState::WEAPONSTATE_READY;
    m_nTotalAmmo       = 0;
    m_nAmmoInClip      = 0;
    m_nTimeForNextShot = 0;
}

// 0x73A3E0
void CWeapon::AddGunshell(CEntity* creator, CVector& position, const CVector2D& direction, float size) {
    if (!creator || !creator->GetIsOnScreen()) {
        return;
    }

    // originally squared
    if (DistanceBetweenPoints(creator->GetPosition(), TheCamera.GetPosition()) > 10.0f) {
        return;
    }

    CVector velocity(direction.x, direction.y, CGeneral::GetRandomNumberInRange(0.4f, 1.6f));

    FxPrtMult_c fxprt(0.5f, 0.5f, 0.5f, 1.0f, size, 1.0f, 1.0f);

    switch (m_nType) {
    case eWeaponType::WEAPON_SPAS12_SHOTGUN:
    case eWeaponType::WEAPON_SHOTGUN:
        fxprt.SetColor(0.6f, 0.1f, 0.1f);
    }

    g_fx.m_GunShell->AddParticle(&position, &velocity, 0.0f, &fxprt, -1.0f, 1.2f, 0.6f, 0);
}

// 0x73A530
bool CWeapon::GenerateDamageEvent(CPed* victim, CEntity* creator, eWeaponType weaponType, int32 damageFactor, ePedPieceTypes pedPiece, uint8 direction) {
    CPedDamageResponseCalculator pedDmgRespCalc{
        creator,
        (float)damageFactor,
        weaponType,
        pedPiece,
        false
    };

    CEventDamage eventDmg{
        creator,
        CTimer::GetTimeInMS(),
        weaponType,
        pedPiece,
        direction,
        false,
        victim->bInVehicle
    };

    if (   victim->m_fHealth <= 0.f
        && CLocalisation::Blood()
        && CLocalisation::KickingWhenDown()
        && victim->GetTaskManager().GetSimplestActiveTask()->GetTaskType() == TASK_SIMPLE_DEAD
    ) {
        const auto floorHitAnim = CAnimManager::BlendAnimation(
            victim->m_pRwClump,
            ANIM_GROUP_DEFAULT,
            RpAnimBlendClumpGetFirstAssociation(victim->m_pRwClump, ANIMATION_800) ? ANIM_ID_FLOOR_HIT_F : ANIM_ID_FLOOR_HIT
        );
        if (floorHitAnim) {
            floorHitAnim->Start();
        }
        return true;
    }

    if (!victim->IsAlive()) {
        return true;
    }

    if (!eventDmg.AffectsPed(victim)) {
        return false;
    }

    if (creator == FindPlayerPed()) {
        CCrime::ReportCrime(CRIME_DAMAGED_PED, victim, static_cast<CPed*>(creator));
    }

    pedDmgRespCalc.ComputeDamageResponse(
        victim,
        eventDmg.m_damageResponse,
        true
    );

    bool ret = true;
    if ((CWeaponInfo::GetWeaponInfo(weaponType)->m_nWeaponFire == eWeaponFire::WEAPON_FIRE_MELEE || weaponType == WEAPON_FALL && creator && creator->GetType() == ENTITY_TYPE_OBJECT) && !victim->bInVehicle) {
        eventDmg.ComputeAnim(victim, true);
        switch (eventDmg.m_nAnimID) {
        case ANIM_ID_SHOT_PARTIAL:
        case ANIM_ID_SHOT_LEFTP:
        case ANIM_ID_SHOT_PARTIAL_B:
        case ANIM_ID_SHOT_RIGHTP: { //> 0x73A769 - Inverted
            auto anim = RpAnimBlendClumpGetAssociation(victim->m_pRwClump, eventDmg.m_nAnimID);
            if (!anim) {
                anim = CAnimManager::AddAnimation(
                    victim->m_pRwClump,
                    (AssocGroupId)eventDmg.m_nAnimGroup,
                    (AnimationId)eventDmg.m_nAnimID
                );
            }
            anim->m_fBlendAmount = 0.f;
            anim->m_fBlendDelta = eventDmg.m_fAnimBlend;
            anim->m_fSpeed = eventDmg.m_fAnimSpeed;
            anim->Start();
            break;
        }
        case ANIM_ID_NO_ANIMATION_SET:
            break;
        case ANIM_ID_DOOR_LHINGE_O:
            ret = false;
            break;
        default: { //< 0x73A7B5
            const auto a = CAnimManager::BlendAnimation(
                victim->m_pRwClump,
                (AssocGroupId)eventDmg.m_nAnimGroup,
                (AnimationId)eventDmg.m_nAnimID,
                eventDmg.m_fAnimBlend
            );
            a->m_fSpeed = eventDmg.m_fAnimSpeed;
            a->SetFlag(ANIMATION_STARTED);
            break;
        }
        }
    }
    bool isStealth = false;
    if (creator && creator->IsPed()) {
        const auto pcreator = creator->AsPed();
        if (pcreator->GetTaskManager().GetActiveTask()->GetTaskType() == TASK_SIMPLE_STEALTH_KILL || weaponType == WEAPON_PISTOL_SILENCED) {
            isStealth = true;
        }
    }
    eventDmg.m_b05 = isStealth;
    if (!victim->bInVehicle || victim->m_fHealth <= 0.f || !victim->GetTaskManager().GetActiveTask() || victim->GetTaskManager().GetActiveTask()->GetTaskType() != TASK_SIMPLE_GANG_DRIVEBY) {
        victim->GetEventGroup().Add(eventDmg);
    }
    return ret;

}

// 0x73A8D0
bool CWeapon::LaserScopeDot(CVector* outCoord, float* outSize) {
    /* UNUSED */
    NOTSA_UNREACHABLE();
}

// 0x73AAC0
bool CWeapon::FireSniper(CPed* shooter, CEntity* victim, CVector* target) {
    const CCam& activeCam = CCamera::GetActiveCamera();

    if (FindPlayerPed() == shooter) {
        switch (activeCam.m_nMode) {
        case MODE_M16_1STPERSON:
        case MODE_SNIPER:
        case MODE_CAMERA:
        case MODE_ROCKETLAUNCHER:
        case MODE_ROCKETLAUNCHER_HS:
        case MODE_M16_1STPERSON_RUNABOUT:
        case MODE_SNIPER_RUNABOUT:
        case MODE_ROCKETLAUNCHER_RUNABOUT:
        case MODE_ROCKETLAUNCHER_RUNABOUT_HS:
            break;
        default:
            return false;
        }
    }

    // todo: make sense of literals.
    float vecFrontZ_Y = activeCam.m_vecFront.z * 0.145f - activeCam.m_vecFront.y * 0.98940003f;

    if (vecFrontZ_Y > 0.99699998f)
        CCoronas::MoonSize = (CCoronas::MoonSize + 1) % 8;

    CVector velocity = activeCam.m_vecFront;
    velocity.Normalise();
    velocity *= 16.0f;

    CBulletInfo::AddBullet(shooter, m_nType, activeCam.m_vecSource, velocity);

    // recoil effect for players
    if (shooter->IsPlayer()) {
        CVector creatorPos = FindPlayerCoors();
        CPad* creatorPad = CPad::GetPad(shooter->m_nPedType);

        creatorPad->StartShake_Distance(240, 128, creatorPos);
        CamShakeNoPos(&TheCamera, 0.2f);
    }

    if (shooter->m_nType == ENTITY_TYPE_PED) {
        CCrime::ReportCrime(CRIME_FIRE_WEAPON, shooter, shooter);
    } else if (shooter->m_nType == ENTITY_TYPE_VEHICLE && shooter->field_460) {
        CCrime::ReportCrime(CRIME_FIRE_WEAPON, shooter, shooter->field_460);
    }

    CVector targetPoint = velocity * 40.0f + activeCam.m_vecSource;
    bool hasNoSound = m_nType == eWeaponType::WEAPON_PISTOL_SILENCED || m_nType == eWeaponType::WEAPON_TEARGAS;
    CEventGroup* eventGroup = GetEventGlobalGroup();

    CEventGunShot gs(shooter, activeCam.m_vecSource, targetPoint, hasNoSound);
    eventGroup->Add(static_cast<CEvent*>(&gs), false);

    CEventGunShotWhizzedBy gsw(shooter, activeCam.m_vecSource, targetPoint, hasNoSound);
    eventGroup->Add(static_cast<CEvent*>(&gsw), false);

    g_InterestingEvents.Add(CInterestingEvents::EType::INTERESTING_EVENT_22, shooter);

    return true;
}

// 0x73AEB0
void CWeapon::Reload(CPed* owner) {
    if (!m_nTotalAmmo)
        return;

    uint32 ammo = GetWeaponInfo(owner).m_nAmmoClip;
    m_nAmmoInClip = std::min(ammo, m_nTotalAmmo);
}

// 0x73B1C0
bool CWeapon::IsTypeMelee() {
    auto weaponInfo = CWeaponInfo::GetWeaponInfo(m_nType, eWeaponSkill::STD);
    return weaponInfo->m_nWeaponFire == eWeaponFire::WEAPON_FIRE_MELEE;
}

// 0x73B1E0
bool CWeapon::IsType2Handed() {
    switch (m_nType) {
    case eWeaponType::WEAPON_M4:
    case eWeaponType::WEAPON_AK47:
    case eWeaponType::WEAPON_SPAS12_SHOTGUN:
    case eWeaponType::WEAPON_SHOTGUN:
    case eWeaponType::WEAPON_SNIPERRIFLE:
    case eWeaponType::WEAPON_FLAMETHROWER:
    case eWeaponType::WEAPON_COUNTRYRIFLE:
        return true;
    }

    return false;
}

// 0x73B210
bool CWeapon::IsTypeProjectile() {
    switch (m_nType) {
    case eWeaponType::WEAPON_GRENADE:
    case eWeaponType::WEAPON_REMOTE_SATCHEL_CHARGE:
    case eWeaponType::WEAPON_TEARGAS:
    case eWeaponType::WEAPON_MOLOTOV:
    case eWeaponType::WEAPON_FREEFALL_BOMB:
        return true;
    }

    return false;
}

// 0x73B240
bool CWeapon::CanBeUsedFor2Player(eWeaponType weaponType) {
    switch (weaponType) {
    case eWeaponType::WEAPON_CHAINSAW:
    case eWeaponType::WEAPON_SNIPERRIFLE:
    case eWeaponType::WEAPON_RLAUNCHER:
    case eWeaponType::WEAPON_PARACHUTE:
        return false;
    }

    return true;
}

// 0x73B2A0
bool CWeapon::HasWeaponAmmoToBeUsed() {
    switch (m_nType) {
    case eWeaponType::WEAPON_UNARMED:
    case eWeaponType::WEAPON_BRASSKNUCKLE:
    case eWeaponType::WEAPON_GOLFCLUB:
    case eWeaponType::WEAPON_NIGHTSTICK:
    case eWeaponType::WEAPON_KNIFE:
    case eWeaponType::WEAPON_BASEBALLBAT:
    case eWeaponType::WEAPON_KATANA:
    case eWeaponType::WEAPON_CHAINSAW:
    case eWeaponType::WEAPON_DILDO1:
    case eWeaponType::WEAPON_DILDO2:
    case eWeaponType::WEAPON_VIBE1:
    case eWeaponType::WEAPON_VIBE2:
    case eWeaponType::WEAPON_FLOWERS:
    case eWeaponType::WEAPON_PARACHUTE:
        return true;
    }

    return m_nTotalAmmo != 0;
}

// 0x73B300
bool CWeapon::ProcessLineOfSight(const CVector& startPoint, const CVector& endPoint, CColPoint& outColPoint, CEntity*& outEntity, eWeaponType weaponType, CEntity* arg5,
                                 bool buildings, bool vehicles, bool peds, bool objects, bool dummies, bool arg11, bool doIgnoreCameraCheck) {
    CBirds::HandleGunShot(&startPoint, &endPoint);
    CShadows::GunShotSetsOilOnFire(startPoint, endPoint);

    return CWorld::ProcessLineOfSight(startPoint, endPoint, outColPoint, outEntity, buildings, vehicles, peds, objects, dummies, false, doIgnoreCameraCheck, true);
}

// 0x73B360
void CWeapon::StopWeaponEffect() {
    if (!m_pFxSystem || m_nType == eWeaponType::WEAPON_MOLOTOV)
        return;

    m_pFxSystem->Kill();
    m_pFxSystem = nullptr;
}

// 0x73B380
float CWeapon::TargetWeaponRangeMultiplier(CEntity* victim, CEntity* weaponOwner) {
    if (!victim || !weaponOwner)
        return 1.0f;

    switch (victim->m_nType) {
    case ENTITY_TYPE_VEHICLE: {
        if (!victim->AsVehicle()->IsBike())
            return 3.0f;
        break;
    }
    case ENTITY_TYPE_PED: {
        CPed* pedVictim = victim->AsPed();

        if (pedVictim->m_pVehicle && !pedVictim->m_pVehicle->IsBike()) {
            return 3.0f;
        }

        if (CEntity* attachedTo = pedVictim->m_pAttachedTo) {
            if (attachedTo->IsVehicle() && !attachedTo->AsVehicle()->IsBike())
                return 3.0f;
        }
        break;
    }
    }

    if (!weaponOwner->IsPed() || !weaponOwner->AsPed()->IsPlayer())
        return 1.0f;

    switch (CCamera::GetActiveCamera().m_nMode) {
    case MODE_TWOPLAYER_IN_CAR_AND_SHOOTING:
        return 2.0f;
    case MODE_HELICANNON_1STPERSON:
        return 3.0f;
    }

    return 1.0f;
}

// 0x73B550
void CWeapon::DoBulletImpact(CEntity* owner, CEntity* victim, const CVector& startPoint, const CVector& endPoint, const CColPoint& colPoint, int32 arg5) {
    plugin::CallMethod<0x73B550, CWeapon*, CEntity*, CEntity*, const CVector&, const CVector&, const CColPoint&, int32>(this, owner, victim, startPoint, endPoint, colPoint, arg5);
}

/*!
* @addr 0x73C1F0
* @brief Marks all peds and objects that are in range (125 units) and in frame (on the screen - 0.1 relative border) as photographed.
*
* @param owner Camera owner - unused.
* @param point Pos of the camflash effect
*/
bool CWeapon::TakePhotograph(CEntity* owner, CVector* point) {
    UNUSED(owner);

    if (point) {
        if (const auto fx = g_fxMan.CreateFxSystem("camflash", point, nullptr, false)) {
            fx->PlayAndKill();
        }
    }

    if (CCamera::GetActiveCamera().m_nMode != MODE_CAMERA) {
        return false;
    }

    CPickups::PictureTaken();
    bPhotographHasBeenTaken = true;
    ms_bTakePhoto = true;
    CStats::IncrementStat(STAT_PHOTOGRAPHS_TAKEN, 1.0f);

    // NOTSA - Optimization
    const auto& camMat = TheCamera.GetMatrix();
    const auto& camPos = camMat.GetPosition();

    const auto IsPosInRange = [&](const CVector& worldPos) {
        return (camPos - worldPos).SquaredMagnitude() >= sq(125.f); // NOTSA: Using squared mag.
    };

    // Check is in position in camera's frame
    const auto IsPosInCamFrame = [](const CVector& worldPos) {
        CVector pedHeadPos_Screen;
        if (float _w, _h; !CSprite::CalcScreenCoors(worldPos, &pedHeadPos_Screen, &_w, &_h, false, true)) {
            return false;
        }

        // TODO/BUG: Possibly buggy on bigger screens, because the border becomes too big (because of the relative multiplier) maybe?
        if (   (SCREEN_WIDTH * 0.1f >= pedHeadPos_Screen.x || pedHeadPos_Screen.x >= SCREEN_WIDTH * 0.9f)
            || (SCREEN_HEIGHT * 0.1f >= pedHeadPos_Screen.y || pedHeadPos_Screen.y >= SCREEN_HEIGHT * 0.9f)
        ) {
            return false;
        }

        return true;
    };

    const auto CheckIsLOSBlocked = [&, camFwd = camMat.GetForward()](const CVector& target, CEntity* ignore) {
        CColPoint _cp; // Unused
        CEntity* hitEntity{};
        if (!CWorld::ProcessLineOfSight(
            camPos + camFwd * 2.f,
            target,
            _cp,
            hitEntity,
            true,
            true,
            true,
            true,
            true,
            true,
            false,
            false
        ) || hitEntity == ignore) { // TODO: Here we could set CWorld::pIgnoreEntity to `&ped`, instead of this check.
            return false;
        }
        return true;
    };

    for (auto& ped : GetPedPool()->GetAllValid()) {
        if (IsPosInRange(ped.GetPosition())) {
            continue;
        }

        const auto pedHeadPos = ped.GetBonePosition(BONE_HEAD);

        if (!IsPosInCamFrame(pedHeadPos)) {
            continue;
        }

        if (!CheckIsLOSBlocked(
            pedHeadPos + Normalized(camPos - pedHeadPos) * 1.5f, // Point from ped's head towards camera
            &ped
        )) {
            ped.bHasBeenPhotographed = true;
        }
    }

    for (auto& obj : GetObjectPool()->GetAllValid()) {
        const auto& objPos = obj.GetPosition();

        if (!IsPosInRange(objPos) || !IsPosInCamFrame(objPos)) {
            continue;
        }

        if (!CheckIsLOSBlocked(objPos, &obj)) {
            obj.objectFlags.bIsPhotographed = true;
        }
    }

    return true;
}

// 0x73C710
void CWeapon::SetUpPelletCol(int32 numPellets, CEntity* owner, CEntity* victim, CVector& point, CColPoint& colPoint, CMatrix& outMatrix) {
    plugin::CallMethod<0x73C710, CWeapon*, int32, CEntity*, CEntity*, CVector&, CColPoint&, CMatrix&>(this, numPellets, owner, victim, point, colPoint, outMatrix);
}

// 0x73CBA0
void CWeapon::FireInstantHitFromCar2(CVector startPoint, CVector endPoint, CVehicle* vehicle, CEntity* owner) {
    const auto player = FindPlayerPed();
    //CWeaponInfo::GetWeaponInfo(m_nType, eWeaponSkill::STD); // useless
    CCrime::ReportCrime(CRIME_FIRE_WEAPON, player, player);

    GetEventGlobalGroup()->Add(CEventGunShot{
        notsa::coalesce<CEntity*>(owner, vehicle),
        startPoint,
        endPoint,
        m_nType == WEAPON_PISTOL_SILENCED || m_nType == WEAPON_TEARGAS
    });
    g_InterestingEvents.Add(CInterestingEvents::EType::INTERESTING_EVENT_22, owner);

    CPointLights::AddLight(PLTYPE_POINTLIGHT, startPoint, {}, 3.0f, 0.25f, 0.22f, 0.0f, 0, false, nullptr);
    CWorld::bIncludeBikers = true;
    CWorld::pIgnoreEntity = vehicle;
    CBirds::HandleGunShot(&startPoint, &endPoint);
    CShadows::GunShotSetsOilOnFire(startPoint, endPoint);

    CEntity* victim{};
    CColPoint cpImpact{};
    CWorld::ProcessLineOfSight(&startPoint, &endPoint, cpImpact, victim, true, true, true, true, true, false, false, true);
    CWorld::ResetLineTestOptions();
    DoBulletImpact(owner, victim, startPoint, endPoint, cpImpact, 0);
}

/*!
* @addr 0x73CDC0
* @brief Find closest entity in range that is visible to `owner` (Eg.: Is in [-PI/8, PI/8] angle) and modify `end->z` to be pointing at it. idk..
*
* @param end out Z axis is modified
*/
void CWeapon::DoDoomAiming(CEntity* owner, CVector* start, CVector* end) {
    int16 inRangeCount{};
    std::array<CEntity*, 16> objInRange{};
    CWorld::FindObjectsInRange(*start, (*start - *end).Magnitude(), true, &inRangeCount, (int16)objInRange.size(), objInRange.data(), false, true, true, false, false);

    CEntity* closestEntity{};
    float    closestDist{ 10'000 };
    for (auto entity : std::span{ objInRange.begin(), (size_t)inRangeCount }) {
        if (entity == owner || owner->AsPed()->CanSeeEntity(entity, PI / 8.f)) { // todo: add check owner->IsPed() NOTSA
            continue;
        }

        switch (entity->GetStatus()) {
        case STATUS_TRAIN_MOVING:
        case STATUS_TRAIN_NOT_MOVING:
        case STATUS_WRECKED:
            continue;
        }

        const auto dir = entity->GetPosition() - owner->GetPosition();
        if (const auto dist2D = dir.Magnitude2D(); std::abs(dir.z) * 1.5f < dist2D) {
            const auto dist3D = std::hypot(dist2D, dir.z);
            if (dist3D < closestDist) {
                closestEntity = entity;
                closestDist = dist3D;
            }
        }
    }

    if (closestDist < 9000.f) {
        // assert(closestEntity); // We should have one, because by default `closestDist` is FLT_MAX (originally 10 000)

        {
            CEntity*  _hitEntity{}; // Unused
            CColPoint _cp;          // Unused
            if (CWorld::ProcessLineOfSight(*start, closestEntity->GetPosition(), _cp, _hitEntity, true, false, false, false, false, false, false, true)) {
                return;
            }
        }

        float targetZ = closestEntity->GetPosition().z + 0.3f;
        if (closestEntity->IsPed() && closestEntity->AsPed()->bIsDucking) {
            targetZ -= 0.8f; // Effectively only -0.5 relative to the original Z
        }
        const auto t = (*start - *end).Magnitude2D() / (*start - closestEntity->GetPosition()).Magnitude2D();
        end->z = start->z + (targetZ - start->z) * t; // Re-ordered a little
    }
}

// 0x73D1E0
void CWeapon::DoTankDoomAiming(CEntity* vehicle, CEntity* owner, CVector* startPoint, CVector* endPoint) {
    plugin::Call<0x73D1E0, CEntity*, CEntity*, CVector*, CVector*>(vehicle, owner, startPoint, endPoint);
}

// 0x73D720
void CWeapon::DoDriveByAutoAiming(CEntity* owner, CVehicle* vehicle, CVector* startPoint, CVector* endPoint, bool canAimVehicles) {
    plugin::Call<0x73D720, CEntity*, CVehicle*, CVector*, CVector*, bool>(owner, vehicle, startPoint, endPoint, canAimVehicles);
}

// 0x73DB40
void CWeapon::Update(CPed* owner) {
    const auto wi = &GetWeaponInfo(owner);
    const auto ao = &wi->GetAimingOffset();

    const auto ProcessReloadAudioIf = [&](auto Pred) {
        const auto ProcessOne = [&](uint32 delay, eAudioEvents ae) {
            if (Pred(delay, ae)) {
                owner->m_weaponAudio.AddAudioEvent(ae);
            }
        };
        ProcessOne(owner->bIsDucking ? ao->CrouchRLoadA : ao->RLoadA, AE_WEAPON_RELOAD_A);
        ProcessOne(owner->bIsDucking ? ao->CrouchRLoadB : ao->RLoadB, AE_WEAPON_RELOAD_B);
    };

    switch (m_nState) {
    case WEAPONSTATE_FIRING: {
        if (owner && notsa::contains({ WEAPON_SPAS12_SHOTGUN, WEAPON_SHOTGUN }, m_nType)) { // 0x73DBA5    
            ProcessReloadAudioIf([&](uint32 rload, eAudioEvents ae) {
                if (!rload) {
                    return false;
                }
                const auto nextShotEnd = m_nTimeForNextShot + rload;
                return CTimer::GetPreviousTimeInMS() < nextShotEnd && CTimer::GetTimeInMS() >= nextShotEnd;
            });
        }
        if (CTimer::GetTimeInMS() > m_nTimeForNextShot) {
            m_nState = wi->m_nWeaponFire == eWeaponFire::WEAPON_FIRE_MELEE || m_nTotalAmmo != 0
                ? eWeaponState::WEAPONSTATE_READY
                : eWeaponState::WEAPONSTATE_OUT_OF_AMMO;
        }
        break;
    }
    case WEAPONSTATE_RELOADING: {
        if (owner && m_nType < WEAPON_LAST_WEAPON) {
            const auto DoPlayAnimlessReloadAudio = [&] {
                ProcessReloadAudioIf([
                    &,
                    shootDelta = m_nTimeForNextShot - wi->GetWeaponReloadTime()
                ](uint32 rload, eAudioEvents ae) {
                    const auto audioTimeMs = rload + shootDelta;
                    return CTimer::GetPreviousTimeInMS() < audioTimeMs && CTimer::GetTimeInMS() >= audioTimeMs;
                });
            };
            if (wi->flags.bReload && (!owner->IsPlayer() || !FindPlayerInfo().m_bFastReload)) { // 0x73DCCE
                auto animRLoad = RpAnimBlendClumpGetAssociation(
                    owner->m_pRwClump,
                    ANIM_ID_RELOAD //(wi->m_nFlags & 0x1000) != 0 ? ANIM_ID_RELOAD : ANIM_ID_WALK // Always going to be `ANIM_ID_RELOAD`
                );
                if (!animRLoad) {
                    animRLoad = RpAnimBlendClumpGetAssociation(owner->m_pRwClump, wi->GetCrouchReloadAnimationID());
                }
                if (animRLoad) { // 0x73DD30
                    ProcessReloadAudioIf([&](uint32 rloadMs, eAudioEvents ae) {
                        const auto rloadS = (float)rloadMs / 1000.f;
                        return rloadS <= animRLoad->m_fCurrentTime && animRLoad->m_fCurrentTime - animRLoad->m_fTimeStep < rloadS;
                    });
                    if (CTimer::GetTimeInMS() > m_nTimeForNextShot) {
                        if (animRLoad->GetTimeProgress() < 0.9f) {
                            m_nTimeForNextShot = CTimer::GetTimeInMS();
                        }
                    }
                } else if (owner->GetIntelligence()->GetTaskUseGun()) { // 0x73DDF9
                    if (CTimer::GetTimeInMS() > m_nTimeForNextShot) {
                        m_nTimeForNextShot = CTimer::GetTimeInMS();
                    }
                } else { // 0x73DE16
                    DoPlayAnimlessReloadAudio();
                }
            } else {
                DoPlayAnimlessReloadAudio();
            }
        }
        //> 0x73DEA4
        if (CTimer::GetTimeInMS() > m_nTimeForNextShot) {
            Reload(owner);
            m_nState = WEAPONSTATE_READY;
        }
        KillFx();
        break;
    }
    case WEAPONSTATE_MELEE_MADECONTACT: {
        m_nState = WEAPONSTATE_READY;
        KillFx();
        break;
    }
    default: {
        KillFx();
        break;
    }
    }
}

// 0x73A360
void CWeapon::UpdateWeapons() {
    ZoneScoped;

    CShotInfo::Update();
    CExplosion::Update();
    CProjectileInfo::Update();
    CBulletInfo::Update();
}

// 0x73DEF0
bool CWeapon::CanBeUsedFor2Player() {
    return CanBeUsedFor2Player(m_nType);
}

// 0x73E240
void CWeapon::FindNearestTargetEntityWithScreenCoors(float screenX, float screenY, float range, CVector point, float* outX, float* outY) {
    plugin::Call<0x73E240, float, float, float, CVector, float*, float*>(screenX, screenY, range, point, outX, outY);
}

// 0x73E560
float CWeapon::EvaluateTargetForHeatSeekingMissile(CEntity* entity, CVector& posn, CVector& direction, float distanceMultiplier, bool fromVehicle, CEntity* lastEntity) {
    return plugin::CallAndReturn<float, 0x73E560, CEntity*, CVector&, CVector&, float, bool, CEntity*>(entity, posn, direction, distanceMultiplier, fromVehicle, lastEntity);
}

// 0x73E690
void CWeapon::DoWeaponEffect(CVector origin, CVector dir) {
    char fxName[32]{};
    switch (m_nType) {
    case eWeaponType::WEAPON_FLAMETHROWER:
        strcpy_s(fxName, "flamethrower");
        break;
    case eWeaponType::WEAPON_EXTINGUISHER:
        strcpy_s(fxName, "extinguisher");
        break;
    case eWeaponType::WEAPON_SPRAYCAN:
        strcpy_s(fxName, "spraycan");
        break;
    default:
        return StopWeaponEffect();
    }

    RwMatrix* mat = RwMatrixCreate();
    g_fx.CreateMatFromVec(mat, &origin, &dir);

    if (m_pFxSystem) {
        m_pFxSystem->SetMatrix(mat);
    } else {
        CVector posn{};
        m_pFxSystem = g_fxMan.CreateFxSystem(fxName, &posn, mat, false);

        if (!m_pFxSystem) {
            RwMatrixDestroy(mat);
            return;
        }

        m_pFxSystem->CopyParentMatrix();
        m_pFxSystem->Play();
        m_pFxSystem->SetMustCreatePrts(true);
    }
    m_pFxSystem->SetConstTime(1, 1.0f);

    RwMatrixDestroy(mat);
}

// 0x73E800
bool CWeapon::FireAreaEffect(CEntity* firingEntity, const CVector& origin, CEntity* targetEntity, CVector* target) {
    const auto wi = &GetWeaponInfo(); // TODO/NOTE: Why not `GetWeaponInfo(firingEntity)`?
    const auto [shotDir, shotPt] = [&]() -> std::pair<CVector, CVector> {
        if (!targetEntity && !target) {
            if (firingEntity == FindPlayerPed() && TheCamera.m_aCams[0].Using3rdPersonMouseCam()) {
                CVector camPos, camTargetPos;
                TheCamera.Find3rdPersonCamTargetVector(wi->m_fWeaponRange, origin, &camPos, &camTargetPos);
                return {
                    (camTargetPos - camPos) / wi->m_fWeaponRange, // Scale to a unit vector
                    camTargetPos
                }; 
            } else {
                // NOTE: Moved here from `0x73E83F`
                // NOTE: Original code used degs instead of radians
                //       and then converted back to radians... So we're gonna stick to radians only ;)
                const auto heading = [&] { // 0x73E83F
                    if (targetEntity) {
                        return (targetEntity->GetPosition() - origin).Heading();
                    }
                    if (target) {
                        return (*target - origin).Heading();
                    }
                    return firingEntity->GetHeading();
                }();
                CVector dir{
                    -std::sin(heading),
                    std::cos(heading),
                    0.f
                };
                if (firingEntity->IsPed()) {
                    if (const auto pd = firingEntity->AsPed()->m_pPlayerData) {
                        dir.z = -std::tan(pd->m_fLookPitch);
                    }
                }
                return { dir, origin + dir };
            }
        } else {
            const auto ptTarget = target
                ? *target
                : targetEntity->IsPed()
                    ? targetEntity->AsPed()->GetBonePosition(BONE_SPINE1)
                    : targetEntity->GetPosition();
            return { (ptTarget - origin).Normalized(), ptTarget };
        }
    }();
    CShotInfo::AddShot(firingEntity, m_nType, origin, shotPt);
    DoWeaponEffect(origin, shotDir);
    if (m_nType == WEAPON_FLAMETHROWER && CGeneral::RandomBool(1.f / 3.f * 100.f)) {
        if (CCreepingFire::TryToStartFireAtCoors(
            shotDir * CVector::Random(3.5f, 6.f) + origin + CVector{0.f, 0.f, 0.5f},
            0,
            true,
            false,
            2.3f)
        ) {
            CStats::IncrementStat(STAT_FIRES_STARTED);
        }
    }
    CCrime::ReportCrime(CRIME_FIRE_WEAPON, nullptr, firingEntity->AsPed());
    return true;
}

// 0x73EC40
bool CWeapon::FireInstantHitFromCar(CVehicle* vehicle, bool leftSide, bool rightSide) {
    return plugin::CallMethodAndReturn<bool, 0x73EC40, CWeapon*, CVehicle*, bool, bool>(this, vehicle, leftSide, rightSide);
}

// 0x73F480
bool CWeapon::CheckForShootingVehicleOccupant(CEntity** pCarEntity, CColPoint* colPoint, eWeaponType weaponType, const CVector& origin, const CVector& target) {
    return plugin::CallAndReturn<bool, 0x73F480, CEntity**, CColPoint*, eWeaponType, const CVector&, const CVector&>(pCarEntity, colPoint, weaponType, origin, target);
}

// 0x73F910
CEntity* CWeapon::PickTargetForHeatSeekingMissile(CVector origin, CVector direction, float distanceMultiplier, CEntity* ignoreEntity, bool fromVehicle, CEntity* lastEntity) {
    return plugin::CallAndReturn<CEntity*, 0x73F910, CVector, CVector, float, CEntity*, bool, CEntity*>(origin, direction, distanceMultiplier, ignoreEntity, fromVehicle, lastEntity);
}

// 0x73FA20
bool CWeapon::FireFromCar(CVehicle* vehicle, bool leftSide, bool rightSide) {
    if (m_nState != WEAPONSTATE_READY || m_nAmmoInClip <= 0) {
        return false;
    }
    if (!CWeapon::FireInstantHitFromCar(vehicle, leftSide, rightSide)) {
        return notsa::IsFixBugs() ? false : true;
    }
    if (const auto d = vehicle->m_pDriver) {
        d->m_weaponAudio.AddAudioEvent(AE_WEAPON_FIRE);
    }
    if (!CCheat::IsActive(CHEAT_INFINITE_AMMO)) {
        if (m_nAmmoInClip) { // NOTE: I'm pretty sure this is redundant
            m_nAmmoInClip--;
        }
        if (  m_nTotalAmmo < 25'000 && m_nTotalAmmo > 0
            && (vehicle->GetStatus() != STATUS_PLAYER || CStats::GetPercentageProgress() < 100.f)
        ) {
            m_nTotalAmmo--;
        }
    }
    m_nState = WEAPONSTATE_FIRING;
    if (m_nAmmoInClip) {
        m_nTimeForNextShot = CTimer::GetTimeInMS() + 1000; // NOTE: Shoot delay can be adjusted here
    } else if (m_nTotalAmmo) {
        m_nState = WEAPONSTATE_RELOADING;
        m_nTimeForNextShot = CTimer::GetTimeInMS() + GetWeaponInfo().GetWeaponReloadTime();
    }
    return true;
}

// 0x73FB10
bool CWeapon::FireInstantHit(CEntity* firingEntity, CVector* origin, CVector* muzzlePosn, CEntity* targetEntity, CVector* target, CVector* originForDriveBy, bool arg6, bool muzzle) {
    return plugin::CallMethodAndReturn<bool, 0x73FB10, CWeapon*, CEntity*, CVector*, CVector*, CEntity*, CVector*, CVector*, bool, bool>(this, firingEntity, origin, muzzlePosn, targetEntity, target, originForDriveBy, arg6, muzzle);
}

// 0x741360
bool CWeapon::FireProjectile(CEntity* firingEntity, CVector* origin, CEntity* targetEntity, CVector* target, float force) {
    return plugin::CallMethodAndReturn<bool, 0x741360, CWeapon*, CEntity*, CVector*, CEntity*, CVector*, float>(this, firingEntity, origin, targetEntity, target, force);
}

// 0x741C00
bool CWeapon::FireM16_1stPerson(CPed* owner) {
    const auto cam = &TheCamera.GetActiveCam();

    switch (cam->m_nMode) {
    case MODE_M16_1STPERSON:
    case MODE_SNIPER:
    case MODE_CAMERA:
    case MODE_ROCKETLAUNCHER:
    case MODE_ROCKETLAUNCHER_HS:
    case MODE_M16_1STPERSON_RUNABOUT:
    case MODE_SNIPER_RUNABOUT:
    case MODE_ROCKETLAUNCHER_RUNABOUT:
    case MODE_ROCKETLAUNCHER_RUNABOUT_HS:
    case MODE_HELICANNON_1STPERSON:
        break;
    default:
        return false;
    }

    const auto wi = &GetWeaponInfo(); // NOTE: Why not `GetWeaponInfo(owner)`

    CWorld::bIncludeDeadPeds = true;
    CWorld::bIncludeCarTyres = true;
    CWorld::bIncludeBikers   = true;

    const auto camOriginPos = cam->m_vecSource;
    const auto camTargetPos = camOriginPos + cam->m_vecFront * 3.f;

    CBirds::HandleGunShot(&camOriginPos, &camTargetPos);
    CShadows::GunShotSetsOilOnFire(camOriginPos, camTargetPos);

    CColPoint shotCP;
    CEntity*  shotHitEntity;
    if (CWorld::ProcessLineOfSight(camOriginPos, camTargetPos, shotCP, shotHitEntity, true, true, true, true, true, false, false, true)) {
        CheckForShootingVehicleOccupant(&shotHitEntity, &shotCP, m_nType, camOriginPos, camTargetPos);
    }

    CWorld::bIncludeDeadPeds = false;
    CWorld::bIncludeCarTyres = false;
    CWorld::bIncludeBikers   = false;
    CWorld::pIgnoreEntity    = nullptr;

    //> 0x741DC4 - Check if hit entity is within range
    if (shotHitEntity) {
        if (TargetWeaponRangeMultiplier(shotHitEntity, owner) * wi->m_fWeaponRange >= (camOriginPos - shotCP.m_vecPoint).SquaredMagnitude2D()) {
            shotHitEntity = nullptr;
        }
    }

    DoBulletImpact(owner, shotHitEntity, &camOriginPos, &camTargetPos, shotCP, false);

    //> 0x741E48 - Visual/physical feedback for the player(s)
    if (owner->IsPlayer()) {
        auto intensity = [&]{
            switch (m_nType) {
            case WEAPON_AK47:
                return 0.00015f;
            case WEAPON_M4:
                return 0.0003f;
            default:
                return 0.0002f;
            }
        }();
        if (FindPlayerPed()->bIsDucking || FindPlayerPed()->m_pAttachedTo) {
            intensity *= 0.3f;
        }

        // Move the camera around a little
        cam->m_fHorizontalAngle += (float)CGeneral::GetRandomNumberInRange(-64, 64);
        cam->m_fVerticalAngle += (float)CGeneral::GetRandomNumberInRange(-64, 64);

        // Do pad shaking
        const auto shakeFreq = (uint8)lerp(130.f, 210.f, std::clamp((20.f - (wi->m_fAnimLoopEnd - wi->m_fAnimLoopStart) * 900.f) / 80.f, 0.f, 1.f));
        CPad::GetPad(owner->GetPadNumber())->StartShake(
            (int16)(CTimer::GetTimeStep() * 20'000.f / (float)shakeFreq),
            shakeFreq,
            0
        );
    }

    return true;
}

// 0x742300
bool CWeapon::Fire(CEntity* firedBy, CVector* startPosn, CVector* barrelPosn, CEntity* targetEnt, CVector* targetPosn, CVector* altPosn) {
    const auto firedByPed = firedBy && firedBy->IsPed()
        ? firedBy->AsPed()
        : nullptr;
    const auto wi = &GetWeaponInfo(firedByPed);

    CVector point{ 0.f, 0.f, 0.6f };


    const auto fxPos = startPosn
        ? startPosn
        : &point;
    const auto shotOrigin = startPosn
        ? barrelPosn
        : &point;
    if (!startPosn) {
        point     = firedBy->GetMatrix() * point;
        startPosn = &point;
    }

    if (field_14) {
        const auto r = 0.15f;

        const auto h = firedBy->GetHeading();
        fxPos->x -= std::sin(h) * r;
        fxPos->y += std::cos(h) * r;
    }

    switch (m_nState) {
    case WEAPONSTATE_READY:
    case WEAPONSTATE_FIRING:
        break;
    default:
        return false;
    }

    if (!m_nAmmoInClip) {
        if (!m_nTotalAmmo) {
            return false;
        }
        m_nAmmoInClip = std::min<uint32>(m_nTotalAmmo, wi->m_nAmmoClip);
    }

    const auto [hasFired, delayNextShot] = [&]() -> std::pair<bool, bool> {
        switch (m_nType) {
        case WEAPON_GRENADE:
        case WEAPON_TEARGAS:
        case WEAPON_MOLOTOV:
        case WEAPON_REMOTE_SATCHEL_CHARGE: { // 0x74268B
            if (targetPosn) {
                return {
                    FireProjectile( // 0x742705
                        firedBy,
                        shotOrigin,
                        targetEnt,
                        targetPosn,
                        std::clamp(((firedBy->GetPosition() - *targetPosn).Magnitude() - 10.f) / 10.f, 0.2f, 1.f)
                    ),
                    true
                };
            } else if (firedBy == FindPlayerPed()) { // 0x74271F
                return {
                    FireProjectile(
                        firedBy,
                        shotOrigin,
                        targetEnt,
                        nullptr,
                        firedBy->AsPed()->m_pPlayerData->m_fAttackButtonCounter * 0.0375f
                    ),
                    true
                };
            }
            return {
                FireProjectile( // 0x74274E
                    firedBy,
                    shotOrigin,
                    targetEnt,
                    nullptr,
                    0.3f
                ),
                true
            };
        }
        case WEAPON_PISTOL:
        case WEAPON_PISTOL_SILENCED:
        case WEAPON_DESERT_EAGLE:
        case WEAPON_MICRO_UZI:
        case WEAPON_MP5:
        case WEAPON_AK47:
        case WEAPON_M4:
        case WEAPON_TEC9:
        case WEAPON_COUNTRYRIFLE:
        case WEAPON_MINIGUN: { // 0x7424FE
            if (   firedByPed
                && firedByPed->m_nPedType == PED_TYPE_PLAYER1
                && notsa::contains({ MODE_M16_1STPERSON, MODE_HELICANNON_1STPERSON }, TheCamera.m_PlayerWeaponMode.m_nMode)
            ) {
                return { FireM16_1stPerson(firedByPed), true };
            }
            const auto fired = FireInstantHit(firedBy, startPosn, shotOrigin, targetEnt, targetPosn, altPosn, false, true);
            if (firedByPed) { // 0x74255B
                if (!firedByPed->bInVehicle) {
                    return { fired, false };
                }
                if (const auto t = firedByPed->GetTaskManager().GetActiveTask()) {
                    return { fired, t->GetTaskType() == TASK_SIMPLE_GANG_DRIVEBY };
                }
            }
            return { fired, true };
        }
        case WEAPON_SHOTGUN:
        case WEAPON_SAWNOFF_SHOTGUN:
        case WEAPON_SPAS12_SHOTGUN:
            return {
                FireInstantHit( // 0x742495
                    firedBy,
                    startPosn,
                    shotOrigin,
                    targetEnt,
                    targetPosn,
                    altPosn,
                    false,
                    true
                ),
                true
            };
        case WEAPON_SNIPERRIFLE: { // 0x7424AC
            if (firedByPed && firedByPed->m_nPedType == PED_TYPE_PLAYER1 && TheCamera.m_PlayerWeaponMode.m_nMode == MODE_SNIPER) {
                return {
                    FireSniper(firedByPed, targetEnt, targetPosn),
                    true
                }; 
            }
            return {
                FireInstantHit(
                    firedBy,
                    startPosn,
                    shotOrigin,
                    targetEnt,
                    targetPosn,
                    nullptr,
                    false,
                    true
                ),
                true
            };
        }
        case WEAPON_RLAUNCHER:
        case WEAPON_RLAUNCHER_HS: { // 0x7425B3
            if (firedByPed) {
                const auto CanFire = [&](CVector origin, CVector end) {
                    return (origin - end).SquaredMagnitude() <= sq(8.f) && !firedBy->IsPed();
                };
                if (   targetEnt  && !CanFire(firedBy->GetPosition(), targetEnt->GetPosition())
                    || targetPosn && !CanFire(firedBy->GetPosition(), targetPosn)
                ) {
                    return { false, true };
                }
            }
            return {
                FireProjectile(
                    firedBy,
                    shotOrigin,
                    targetEnt,
                    targetPosn
                ),
                true
            };
        }
        case WEAPON_FLAMETHROWER:
        case WEAPON_SPRAYCAN:
        case WEAPON_EXTINGUISHER:
            return {
                FireAreaEffect(
                    firedBy,
                    shotOrigin,
                    targetEnt,
                    targetPosn
                ),
                true
            };
        case WEAPON_DETONATOR: {
            assert(firedByPed);
            CWorld::UseDetonator(firedByPed);
            m_nAmmoInClip = m_nTotalAmmo  = 1;
            return { true, true };
        }
        case WEAPON_CAMERA:
            return {
                TakePhotograph(firedBy, shotOrigin),
                true
            };
        default:
            NOTSA_UNREACHABLE();
        }
    }();

    // 0x74279A
    if (hasFired) {
        // 0x7427B3
        const bool isPlayerFiring = firedByPed && m_nType != WEAPON_CAMERA && firedByPed->IsPlayer();
        if (firedByPed) {
            if (m_nType != WEAPON_CAMERA) {
                firedByPed->bFiringWeapon = true;
            }
            firedByPed->m_weaponAudio.AddAudioEvent(AE_WEAPON_FIRE);
            if (isPlayerFiring && targetEnt && targetEnt->IsPed() && m_nType != WEAPON_PISTOL_SILENCED) {
                firedByPed->Say(182, 200); // 0x74280E
            }
        }

        // 0x74282C
        if (m_nType == WEAPON_REMOTE_SATCHEL_CHARGE) {
            firedByPed->GiveWeapon(WEAPON_DETONATOR, true, true);
            if (firedByPed->GetWeapon(WEAPON_REMOTE_SATCHEL_CHARGE).m_nTotalAmmo <= 1) {
                firedByPed->GetWeapon(WEAPON_DETONATOR).m_nState = eWeaponState::WEAPONSTATE_READY;
                firedByPed->SetCurrentWeapon(WEAPON_DETONATOR);
            }
        }

        //> 0x74286D - Increase stats
        if (isPlayerFiring) {
            switch (m_nType)
            {
            case WEAPON_GRENADE:
            case WEAPON_MOLOTOV:
            case WEAPON_ROCKET:
            case WEAPON_RLAUNCHER:
            case WEAPON_RLAUNCHER_HS:
            case WEAPON_REMOTE_SATCHEL_CHARGE:
            case WEAPON_DETONATOR:
                CStats::IncrementStat(STAT_KGS_OF_EXPLOSIVES_USED);
                break;
            case WEAPON_PISTOL:
            case WEAPON_PISTOL_SILENCED:
            case WEAPON_DESERT_EAGLE:
            case WEAPON_SHOTGUN:
            case WEAPON_SAWNOFF_SHOTGUN:
            case WEAPON_SPAS12_SHOTGUN:
            case WEAPON_MICRO_UZI:
            case WEAPON_MP5:
            case WEAPON_AK47:
            case WEAPON_M4:
            case WEAPON_TEC9:
            case WEAPON_COUNTRYRIFLE:
            case WEAPON_SNIPERRIFLE:
            case WEAPON_MINIGUN:
                CStats::IncrementStat(STAT_BULLETS_FIRED);
                break;
            default:
                break;
            }
        }

        // 0x7428A6
        if (!CCheat::IsActive(CHEAT_INFINITE_AMMO)) {
            if (m_nAmmoInClip) {
                m_nAmmoInClip--;
            }
            if (m_nTotalAmmo > 0) {
                if (isPlayerFiring
                        ? m_nType == WEAPON_DETONATOR || CStats::GetPercentageProgress() < 100.f
                        : m_nTotalAmmo < 25'000
                ) {
                    m_nTotalAmmo--;    
                }
            }
        }

        m_nState = WEAPONSTATE_FIRING;

        if (!m_nAmmoInClip) { // 0x7428FB
            if (m_nTotalAmmo) {
                m_nState = WEAPONSTATE_RELOADING;
                m_nTimeForNextShot = firedBy == FindPlayerPed() && FindPlayerInfo().m_bFastReload
                    ? wi->GetWeaponReloadTime()
                    : wi->GetWeaponReloadTime() / 4;
                m_nTimeForNextShot += CTimer::GetTimeInMS();
            } else if (TheCamera.GetActiveCam().m_nMode == MODE_CAMERA) {
                CPad::GetPad()->Clear(false, true);
            }
            return true;
        }
        m_nTimeForNextShot = delayNextShot
            ? m_nType == WEAPON_CAMERA
                ? 1100
                : (uint32)((wi->m_fAnimLoopEnd - wi->m_fAnimLoopStart) * 900.f)
            : 0;
        m_nTimeForNextShot += CTimer::GetTimeInMS();
    }
    // 0x7429F2
    if (m_nType == WEAPON_UNARMED || m_nType == WEAPON_BASEBALLBAT) {
        return true;
    }
    return hasFired;
}

CWeaponInfo& CWeapon::GetWeaponInfo(CPed* owner) const {
    return GetWeaponInfo(owner ? owner->GetWeaponSkill(m_nType) : eWeaponSkill::STD);
}

CWeaponInfo& CWeapon::GetWeaponInfo(eWeaponSkill skill) const {
    return *CWeaponInfo::GetWeaponInfo(m_nType, skill);
}

void CWeapon::KillFx() {
    if (m_pFxSystem && m_nType != WEAPON_MOLOTOV) {
        m_pFxSystem->Kill();
        m_pFxSystem = nullptr;
    }
}

// 0x73AF00
void FireOneInstantHitRound(CVector* startPoint, CVector* endPoint, int32 intensity) {
    plugin::Call<0x73AF00, CVector*, CVector*, int32>(startPoint, endPoint, intensity);
}
