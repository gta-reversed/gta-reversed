#include "StdInc.h"

#include "EventDamage.h"

#include "PedType.h"
#include "TaskComplexKillPedOnFoot.h"

#include "TaskSimpleFight.h"
#include "TaskSimpleUseGun.h"

void CEventDamage::InjectHooks() {
    RH_ScopedVirtualClass(CEventDamage, 0x85AD78, 17);
    RH_ScopedCategory("Events");

    RH_ScopedOverloadedInstall(Constructor, "", 0x4B33B0, CEventDamage*(CEventDamage::*)(const CEventDamage&));
    RH_ScopedOverloadedInstall(Constructor, "1", 0x4AD830, CEventDamage * (CEventDamage::*)(CEntity*, uint32, eWeaponType, ePedPieceTypes, uint8, bool, bool));
    RH_ScopedVMTInstall(GetEventType, 0x4AD910);
    RH_ScopedVMTInstall(GetEventPriority, 0x4AD950);
    RH_ScopedVMTInstall(GetLifeTime, 0x4AD920);
    RH_ScopedVMTInstall(AffectsPed, 0x4B35A0);
    RH_ScopedVMTInstall(AffectsPedGroup, 0x4B38D0);
    RH_ScopedVMTInstall(IsCriminalEvent, 0x4ADA90);
    RH_ScopedVMTInstall(ReportCriminalEvent, 0x4B3440);
    RH_ScopedVMTInstall(GetSourceEntity, 0x4ADA70);
    RH_ScopedVMTInstall(TakesPriorityOver, 0x4ADB00);
    RH_ScopedVMTInstall(GetLocalSoundLevel, 0x4AD930);
    RH_ScopedVMTInstall(DoInformVehicleOccupants, 0x4ADAE0);
    RH_ScopedVMTInstall(CanBeInterruptedBySameEvent, 0x4AD940);
    RH_ScopedVMTInstall(CloneEditable, 0x4B5D40);
    RH_ScopedInstall(From, 0x4AD9C0);
    RH_ScopedInstall(ProcessDamage, 0x4B3A20);
    RH_ScopedInstall(ComputeBodyPartToRemove, 0x4ADC10);
    RH_ScopedInstall(ComputeDeathAnim, 0x4B3A60);
    RH_ScopedInstall(ComputeDamageAnim, 0x4B3FC0);
}

// 0x4B33B0
CEventDamage::CEventDamage(const CEventDamage& event) : CEventEditableResponse() {
    From(event);
}

// 0x4AD830
CEventDamage::CEventDamage(CEntity* source, uint32 startTime, eWeaponType weaponType, ePedPieceTypes pieceHit, uint8 direction, bool bJumpedOutOfMovingCar, bool bPedInVehicle) : CEventEditableResponse() {
    m_pSourceEntity         = source;
    m_nStartTime            = startTime;
    m_weaponType            = weaponType;
    m_pedPieceType          = pieceHit;
    m_ucDirection           = direction;
    m_bWitnessedInVehicle   = bPedInVehicle;
    m_bJumpedOutOfMovingCar = bJumpedOutOfMovingCar;
    m_nAnimGroup            = ANIM_GROUP_DEFAULT;
    m_nAnimID               = ANIM_ID_NO_ANIMATION_SET;
    m_fAnimBlend            = 8.0f;
    m_fAnimSpeed            = 1.0f;

    CEntity::SafeRegisterRef(m_pSourceEntity);
    m_bWitnessedInVehicle = true; // if we're setting this to true, then why do we have bPedInVehicle parameter in this constructor? bug?
}

CEventDamage::~CEventDamage() {
    CEntity::SafeCleanUpRef(m_pSourceEntity);
}

CEventDamage* CEventDamage::Constructor(const CEventDamage& event) {
    this->CEventDamage::CEventDamage(event);
    return this;
}

CEventDamage* CEventDamage::Constructor(CEntity* source, uint32 startTime, eWeaponType weaponType, ePedPieceTypes pieceHit, uint8 direction, bool a7, bool bPedInVehicle) {
    this->CEventDamage::CEventDamage(source, startTime, weaponType, pieceHit, direction, a7, bPedInVehicle);
    return this;
}

// 0x4B35A0
bool CEventDamage::AffectsPed(CPed* ped) {
    auto* pedSourceEntity = GetSourceEntity()->AsPed();
    if (m_damageResponse.m_bDamageCalculated && !m_damageResponse.m_bCheckIfAffectsPed)
        return true;

    if (!ped->IsAlive() || ped == FindPlayerPed() && !FindPlayerPed()->m_pPlayerData->m_bCanBeDamaged)
        return false;

    if (m_weaponType == WEAPON_DROWNING && !ped->bDrownsInWater)
        return false;

    if (ped == FindPlayerPed()) {
        if (m_pSourceEntity) {
            if (m_pSourceEntity->IsPed() && pedSourceEntity->m_nPedType == PED_TYPE_GANG2 && m_weaponType >= WEAPON_GRENADE) {
                const auto task = pedSourceEntity->GetTaskManager().Find<CTaskComplexKillPedOnFoot>();
                if (!task || task->m_target != ped){
                    return false;
                }
            }
        }
    }

    if (ped == FindPlayerPed()) {
        if (FindPlayerInfo().m_bFireProof) {
            if (m_weaponType == WEAPON_MOLOTOV || m_weaponType == WEAPON_FLAMETHROWER)
                return false;
        }
    }
    if (ped->physicalFlags.bInvulnerable) {
        if (pedSourceEntity != FindPlayerPed()) {
            if (m_weaponType != WEAPON_DROWNING && m_weaponType != WEAPON_EXPLOSION && ped->m_fHealth > 0.0f)
                return false;
        }
    }
    if (ped->bHasBulletProofVest) {
        if (CWeaponInfo::GetWeaponInfo(m_weaponType, eWeaponSkill::STD)->m_nWeaponFire == WEAPON_FIRE_INSTANT_HIT) {
            if (m_pedPieceType == PED_PIECE_TORSO || m_pedPieceType == PED_PIECE_ASS)
                return false;
        }
    }

    if (!m_bAddToEventGroup) {
        if (m_weaponType == WEAPON_PISTOL_SILENCED) {
            if (!m_pSourceEntity)
                return false;
            if (!m_pSourceEntity->IsPed())
                return false;
            if (!m_bWitnessedInVehicle)
                return false;
            if (!ped->bInVehicle)
                return false;
        }
        if (m_pSourceEntity) {
            if (m_pSourceEntity->IsPed()) {
                CTask* activeTask = pedSourceEntity->GetTaskManager().GetActiveTask();
                if (activeTask && activeTask->GetTaskType() == TASK_SIMPLE_STEALTH_KILL) {
                    CVector vecDirection = m_pSourceEntity->GetPosition() - ped->GetPosition();
                    vecDirection.Normalise();
                    if (ped->m_pIntelligence->CanSeeEntityWithLights(m_pSourceEntity, 0) <= 0.0f
                        || DotProduct(&vecDirection, &ped->GetForward()) < CPedAcquaintanceScanner::ms_fThresholdDotProduct)
                    {
                        return false;
                    }
                }
            }
        }
    }
    if (m_pSourceEntity) {
        if (    m_pSourceEntity->IsPed()
            && !pedSourceEntity->IsPlayer()
            && CPedGroups::AreInSameGroup(ped, pedSourceEntity)
            && m_weaponType != WEAPON_EXPLOSION
        ) {
            return false;
        }
    }
    bool bAffectsPed = ped->CanPhysicalBeDamaged(m_weaponType, nullptr);
    if (    m_weaponType == WEAPON_FALL
        && (ped->physicalFlags.bCollisionProof || ped->m_pAttachedTo || ped->m_fHealth > 0.0f && ped->m_pIntelligence->GetTaskJetPack())
    ) {
        bAffectsPed = false;
    }
    return bAffectsPed;

}

// 0x4B38D0
bool CEventDamage::AffectsPedGroup(CPedGroup* pedGroup) {
    if (m_weaponType == WEAPON_PISTOL_SILENCED)
        return false;

    if (!m_pSourceEntity)
        return true;

    if (!m_pSourceEntity->IsPed())
        return true;

    auto* ped = m_pSourceEntity->AsPed();
    CTask* activeTask = ped->GetTaskManager().GetActiveTask();
    if (!activeTask)
        return true;

    if (activeTask->GetTaskType() != TASK_SIMPLE_STEALTH_KILL)
        return true;

    for (auto memberId = 0; memberId < 8; memberId++) { // todo: magic number
        CPed* groupMember = pedGroup->GetMembership().GetMember(memberId);
        if (groupMember) {
            CVector vecDirection = m_pSourceEntity->GetPosition() - groupMember->GetPosition();
            vecDirection.Normalise();
            if (groupMember->m_pIntelligence->CanSeeEntityWithLights(m_pSourceEntity, 0) > 0.0f) {
                if (DotProduct(&vecDirection, &groupMember->GetForward()) > CPedAcquaintanceScanner::ms_fThresholdDotProduct)
                    return true;
            }
        }
    }
    return false;
}

// 0x4ADA90
bool CEventDamage::IsCriminalEvent() {
    if (m_pSourceEntity) {
        auto* vehicle = m_pSourceEntity->AsVehicle();
        auto* ped     = m_pSourceEntity->AsPed();

        return (   m_pSourceEntity->IsPed()
                && ped->IsPlayer()
                || m_pSourceEntity->IsVehicle()
                && vehicle->m_pDriver == FindPlayerPed()
        );
    }
    return false;
}

// 0x4B3440
void CEventDamage::ReportCriminalEvent(CPed* ped) {
    if (IsCriminalEvent() && m_pSourceEntity) {
        bool bPoliceCareAboutCrime = CPedType::PoliceDontCareAboutCrimesAgainstPedType(ped->m_nPedType);
        if (m_weaponType <= WEAPON_CHAINSAW) {
            auto crimeType = ped->m_nPedType == PED_TYPE_COP ? CRIME_DAMAGED_COP : CRIME_DAMAGED_PED;
            FindPlayerWanted()->RegisterCrime(crimeType, m_pSourceEntity->GetPosition(), ped, bPoliceCareAboutCrime);
            return;
        }

        if (m_weaponType <= WEAPON_DETONATOR || m_weaponType == WEAPON_SPRAYCAN) {
            auto crimeType = ped->m_nPedType == PED_TYPE_COP ? CRIME_DAMAGE_COP_CAR : CRIME_DAMAGE_CAR;
            FindPlayerWanted()->RegisterCrime(crimeType, m_pSourceEntity->GetPosition(), ped, bPoliceCareAboutCrime);
            return;
        }

        if (m_weaponType == WEAPON_RAMMEDBYCAR || m_weaponType == WEAPON_RUNOVERBYCAR) {
            auto crimeType = ped->m_nPedType == PED_TYPE_COP ? CRIME_KILL_COP_PED_WITH_CAR : CRIME_KILL_PED_WITH_CAR;
            FindPlayerWanted()->RegisterCrime(crimeType, m_pSourceEntity->GetPosition(), ped, bPoliceCareAboutCrime);
            return;
        }
    }
}

// 0x4ADA70
CEntity* CEventDamage::GetSourceEntity() const {
    if (m_pSourceEntity && m_pSourceEntity->IsVehicle()) {
        auto* vehicle = m_pSourceEntity->AsVehicle();
        if (vehicle->m_pDriver)
            return vehicle->m_pDriver;
    }
    return m_pSourceEntity;
}

// 0x4ADB00
bool CEventDamage::TakesPriorityOver(const CEvent& refEvent) {
    if (refEvent.GetEventType() == EVENT_IN_WATER && m_damageResponse.m_bHealthZero && m_bAddToEventGroup
        || refEvent.GetEventType() == EVENT_ON_FIRE && m_damageResponse.m_bHealthZero && m_bAddToEventGroup
        || refEvent.GetEventType() == EVENT_KNOCK_OFF_BIKE && m_damageResponse.m_bHealthZero && m_bAddToEventGroup) {
        return true;
    }

    auto* ped = m_pSourceEntity->AsPed();
    if (m_pSourceEntity && m_pSourceEntity->IsPed() && ped->IsPlayer() && refEvent.GetEventType() == EVENT_DAMAGE) {
        if (refEvent.GetSourceEntity() == m_pSourceEntity && (!m_damageResponse.m_bHealthZero || !m_bAddToEventGroup))
            return CEvent::TakesPriorityOver(refEvent);
        return true;
    }

    if (refEvent.GetEventType() == EVENT_DAMAGE) {
        if (GetSourceEntity() != refEvent.GetSourceEntity())
            return true;
    }
    return CEvent::TakesPriorityOver(refEvent);
}

// 0x4ADAE0
bool CEventDamage::DoInformVehicleOccupants(CPed* ped) {
    if (GetSourceEntity())
        return m_weaponType != WEAPON_DROWNING;
    return false;
}

// 0x4B5D40
CEventEditableResponse* CEventDamage::CloneEditable() const noexcept {
    auto* clonedEvent      = new CEventDamage(*this);
    clonedEvent->m_nAnimID = m_nAnimID;
    clonedEvent->m_bStealthMode     = m_bStealthMode;
    return clonedEvent;
}

// 0x4AD9C0
void CEventDamage::From(const CEventDamage& event) {
    m_pSourceEntity = event.m_pSourceEntity;
    CEntity::SafeRegisterRef(m_pSourceEntity);

    m_nStartTime            = event.m_nStartTime;
    m_weaponType            = event.m_weaponType;
    m_pedPieceType          = event.m_pedPieceType;
    m_ucDirection           = event.m_ucDirection;
    m_damageResponse        = event.m_damageResponse;
    m_nAnimGroup            = event.m_nAnimGroup;
    m_nAnimID               = event.m_nAnimID;
    m_fAnimBlend            = event.m_fAnimBlend;
    m_fAnimSpeed            = event.m_fAnimSpeed;
    m_bJumpedOutOfMovingCar = event.m_bJumpedOutOfMovingCar;
    m_bFallDown             = event.m_bFallDown;
    m_bAnimAdded            = event.m_bAnimAdded;
    m_bWitnessedInVehicle   = event.m_bWitnessedInVehicle;
    m_bStealthMode          = event.m_bStealthMode;
}

// 0x4B3A20
void CEventDamage::ProcessDamage(CPed* ped) {
    int32 pedNode = 0;
    ComputeBodyPartToRemove(pedNode);
    if (pedNode)
        ped->RemoveBodyPart((ePedNode)pedNode, m_ucDirection);
}

// todo: ePedNode
// 0x4ADC10
void CEventDamage::ComputeBodyPartToRemove(int32& boneFrameId) {
    boneFrameId = 0;
    switch (m_weaponType) {
    case WEAPON_UNARMED:
    case WEAPON_BRASSKNUCKLE:
    case WEAPON_GOLFCLUB:
    case WEAPON_NIGHTSTICK:
    case WEAPON_KNIFE:
    case WEAPON_BASEBALLBAT:
    case WEAPON_SHOVEL:
    case WEAPON_POOL_CUE:
    case WEAPON_KATANA:
    case WEAPON_CHAINSAW:
    case WEAPON_DILDO1:
    case WEAPON_DILDO2:
    case WEAPON_VIBE1:
    case WEAPON_VIBE2:
    case WEAPON_FLOWERS:
    case WEAPON_CANE:
    case WEAPON_PARACHUTE: {
        if (m_damageResponse.m_bForceDeath)
            boneFrameId = PED_NODE_HEAD;
        break;
    }
    case WEAPON_GRENADE:
    case WEAPON_RLAUNCHER:
    case WEAPON_RLAUNCHER_HS:
    case WEAPON_EXPLOSION: {
        if (m_damageResponse.m_bForceDeath) {
            int32 boneArray[5] = {
                PED_NODE_LEFT_ARM,
                PED_NODE_RIGHT_LEG,
                PED_NODE_HEAD,
                PED_NODE_RIGHT_ARM,
                PED_NODE_LEFT_LEG
            };
            boneFrameId = boneArray[CGeneral::GetRandomNumberInRange(0, 5)];
        }
        break;
    }
    case WEAPON_TEARGAS:
    case WEAPON_MOLOTOV:
    case WEAPON_ROCKET:
    case WEAPON_ROCKET_HS:
    case WEAPON_FREEFALL_BOMB:
    case WEAPON_PISTOL:
    case WEAPON_PISTOL_SILENCED:
    case WEAPON_MICRO_UZI:
    case WEAPON_MP5:
    case WEAPON_TEC9:
    case WEAPON_FLAMETHROWER:
    case WEAPON_REMOTE_SATCHEL_CHARGE:
    case WEAPON_DETONATOR:
    case WEAPON_SPRAYCAN:
    case WEAPON_EXTINGUISHER:
    case WEAPON_CAMERA:
    case WEAPON_NIGHTVISION:
    case WEAPON_INFRARED:
    case WEAPON_LAST_WEAPON:
    case WEAPON_ARMOUR:
    case WEAPON_RAMMEDBYCAR:
    case WEAPON_RUNOVERBYCAR:
        return;
    case WEAPON_DESERT_EAGLE:
    case WEAPON_SHOTGUN:
    case WEAPON_SAWNOFF_SHOTGUN:
    case WEAPON_SPAS12_SHOTGUN:
    case WEAPON_AK47:
    case WEAPON_M4:
    case WEAPON_COUNTRYRIFLE:
    case WEAPON_SNIPERRIFLE:
    case WEAPON_MINIGUN:
        if (m_damageResponse.m_bForceDeath) {
            switch (m_pedPieceType)
            {
            case PED_PIECE_LEFT_ARM:
                boneFrameId = PED_NODE_LEFT_ARM;
                break;
            case PED_PIECE_RIGHT_ARM:
                boneFrameId = PED_NODE_RIGHT_ARM;
                break;
            case PED_PIECE_LEFT_LEG:
                boneFrameId = PED_NODE_LEFT_LEG;
                break;
            case PED_PIECE_RIGHT_LEG:
                boneFrameId = PED_NODE_RIGHT_LEG;
                break;
            case PED_PIECE_HEAD:
                boneFrameId = PED_NODE_HEAD;
                break;
            }
        }
        break;
    }
}

// 0x4B3A60
void CEventDamage::ComputeDeathAnim(CPed* ped, bool bMakeActiveTaskAbortable) {
    m_nAnimGroup = ANIM_GROUP_DEFAULT;
    m_nAnimID    = ANIM_ID_KO_SHOT_FRONT_0;
    m_fAnimBlend = 4.0f;
    m_fAnimSpeed = 1.0f;
    float fForceFactor = 0.0f;
    CTask* activeTask = ped->GetTaskManager().GetActiveTask();
    if (ped->bInVehicle || bMakeActiveTaskAbortable && activeTask && !activeTask->MakeAbortable(ped, ABORT_PRIORITY_URGENT, this))
    {
        m_nAnimID = ANIM_ID_NO_ANIMATION_SET;
    }
    else if (m_weaponType == WEAPON_DROWNING || ped->physicalFlags.bSubmergedInWater && !ped->bIsStanding)
    {
        m_nAnimID = ANIM_ID_DROWN;
    }
    else
    {
        auto* pedSourceEntity = m_pSourceEntity->AsPed();
        CTaskSimpleFight* taskFight = nullptr;
        if (m_pSourceEntity && m_pSourceEntity->IsPed())
            taskFight = pedSourceEntity->m_pIntelligence->GetTaskFighting();

        const auto bonePosition = ped->GetBonePosition(BONE_HEAD, false);
        CTask* pSimplestActiveTask = ped->GetTaskManager().GetSimplestActiveTask();
        if (ped->GetPosition().z - 0.2f > bonePosition.z
            && pSimplestActiveTask && (pSimplestActiveTask->GetTaskType() == TASK_SIMPLE_FALL || pSimplestActiveTask->GetTaskType() == TASK_SIMPLE_GET_UP))
        {
            m_bFallDown = true;
            m_nAnimID = RpAnimBlendClumpGetFirstAssociation(ped->m_pRwClump, ANIMATION_IS_FRONT) ? ANIM_ID_FLOOR_HIT_F : ANIM_ID_FLOOR_HIT;
        }
        else
        {
            if (   !m_ucDirection
                && taskFight
                && taskFight->IsComboSet()
                && taskFight->GetCurrentComboSet() >= eMeleeCombo::UNARMED_1
                && taskFight->GetMove() <= eMeleeMove::ATTACK3
            ) {
                m_nAnimGroup = taskFight->GetComboAnimGroupID();
                m_nAnimID = (AnimationId)(+taskFight->GetMove() + ANIM_ID_FIGHT_HIT_1);
                m_fAnimBlend = 16.0f;
                fForceFactor = 1.0f;
            }
            else
            {
                bool bKnockOutAnim = false;
                bool bKnockOutShotFront = false;
                bool bKnockOutShotFrontExtraForce = false;
                switch (m_weaponType) {
                case WEAPON_UNARMED:
                case WEAPON_BRASSKNUCKLE:
                case WEAPON_NIGHTSTICK:
                case WEAPON_CHAINSAW:
                case WEAPON_PARACHUTE:
                    fForceFactor = 0.5f;
                    bKnockOutShotFrontExtraForce = true;
                    break;
                case WEAPON_GOLFCLUB:
                case WEAPON_BASEBALLBAT:
                case WEAPON_SHOVEL:
                case WEAPON_POOL_CUE:
                case WEAPON_KATANA:
                case WEAPON_DILDO1:
                    fForceFactor = 1.5f;
                    bKnockOutShotFrontExtraForce = true;
                    break;
                case WEAPON_KNIFE:
                case WEAPON_DILDO2:
                case WEAPON_VIBE1:
                case WEAPON_VIBE2:
                case WEAPON_FLOWERS:
                case WEAPON_CANE:
                    bKnockOutShotFrontExtraForce = true;
                    break;
                case WEAPON_GRENADE:
                case WEAPON_RLAUNCHER:
                case WEAPON_RLAUNCHER_HS:
                case WEAPON_REMOTE_SATCHEL_CHARGE:
                case WEAPON_EXPLOSION:
                    bKnockOutAnim = true;
                    break;
                case WEAPON_TEARGAS:
                case WEAPON_MOLOTOV:
                case WEAPON_FLAMETHROWER:
                case WEAPON_SPRAYCAN:
                case WEAPON_EXTINGUISHER:
                    m_nAnimID = ANIM_ID_KO_SHOT_FRONT_0;
                    break;
                case WEAPON_ROCKET:
                case WEAPON_ROCKET_HS:
                case WEAPON_FREEFALL_BOMB:
                case WEAPON_DETONATOR:
                case WEAPON_CAMERA:
                case WEAPON_NIGHTVISION:
                case WEAPON_INFRARED:
                case WEAPON_LAST_WEAPON:
                case WEAPON_ARMOUR:
                    break;
                case WEAPON_PISTOL:
                case WEAPON_PISTOL_SILENCED:
                case WEAPON_MICRO_UZI:
                case WEAPON_MP5:
                case WEAPON_TEC9:
                case WEAPON_COUNTRYRIFLE:
                case WEAPON_SNIPERRIFLE:
                case WEAPON_UZI_DRIVEBY:
                    bKnockOutShotFront = true;
                    break;
                case WEAPON_DESERT_EAGLE:
                    fForceFactor = 1.0f;
                    bKnockOutShotFront = true;
                    break;
                case WEAPON_SHOTGUN:
                case WEAPON_SAWNOFF_SHOTGUN:
                case WEAPON_SPAS12_SHOTGUN:
                case WEAPON_MINIGUN:
                    fForceFactor = 2.0f;
                    bKnockOutShotFront = true;
                    break;
                case WEAPON_AK47:
                case WEAPON_M4:
                    fForceFactor = 0.5f;
                    bKnockOutShotFront = true;
                    break;
                case WEAPON_RAMMEDBYCAR:
                case WEAPON_RUNOVERBYCAR:
                    switch (m_ucDirection) // todo: m_ucDirection enum
                    {
                    case 0:
                        if (m_pedPieceType == PED_PIECE_LEFT_ARM)
                            m_nAnimID = ANIM_ID_KD_LEFT;
                        else
                            m_nAnimID = m_pedPieceType
                                ? ANIM_ID_KO_SKID_BACK
                                : ANIM_ID_KD_RIGHT;
                        break;
                    case 1:
                        m_nAnimID = ANIM_ID_KO_SPIN_R;
                        break;
                    case 2:
                        if (m_pedPieceType == PED_PIECE_LEFT_ARM)
                            m_nAnimID = ANIM_ID_KD_LEFT;
                        else
                            m_nAnimID = m_pedPieceType == PED_PIECE_RIGHT_ARM
                                ? ANIM_ID_KD_RIGHT
                                : ANIM_ID_KO_SKID_FRONT; 
                        break;
                    case 3:
                        m_nAnimID = ANIM_ID_KO_SPIN_L;
                        break;
                    }
                    break;
                case WEAPON_DROWNING:
                    m_nAnimID = ANIM_ID_DROWN;
                    break;
                case WEAPON_FALL:
                    m_nAnimID = ANIM_ID_KO_SHOT_FRONT_2;
                    break;
                }

                if (bKnockOutShotFrontExtraForce) {
                    if (m_damageResponse.m_bForceDeath) {
                        m_nAnimID = ANIM_ID_KO_SHOT_FACE;
                        fForceFactor = 0.0f;
                    }
                    else {
                        bKnockOutAnim = true;
                    }
                    if (m_pSourceEntity) {
                        if (m_pSourceEntity->IsPed() && pedSourceEntity->m_pPlayerData) {
                            if (pedSourceEntity->m_pPlayerData->m_bAdrenaline)
                                fForceFactor = fForceFactor * 5.0f;
                            else
                                fForceFactor = CStats::GetFatAndMuscleModifier(STAT_MOD_4) * fForceFactor;
                        }
                    }
                }
                if (bKnockOutShotFront) {
                    if (m_damageResponse.m_bForceDeath) {
                        m_nAnimID = ANIM_ID_KO_SHOT_FRONT_0;
                        fForceFactor = 0.0f;
                    }
                    else if (fForceFactor <= 0.0f) {
                        m_nAnimID = ANIM_ID_KO_SHOT_FRONT_0;
                    }
                    else {
                        bKnockOutAnim = true;
                    }
                }
                if (bKnockOutAnim) {
                    switch (m_ucDirection)
                    {
                    case 0:
                        m_nAnimID = ANIM_ID_KO_SKID_FRONT;
                        break;
                    case 1:
                        m_nAnimID = ANIM_ID_KO_SPIN_R;
                        break;
                    case 2:
                        m_nAnimID = ANIM_ID_KO_SKID_BACK;
                        break;
                    case 3:
                    default:
                        m_nAnimID = ANIM_ID_KO_SPIN_L;
                        break;
                    }
                }
            }
            if (CCheat::IsActive(CHEAT_MEGA_PUNCH)) {
                if (!m_weaponType || m_weaponType == WEAPON_BRASSKNUCKLE || m_weaponType == WEAPON_PARACHUTE)
                    fForceFactor = fForceFactor * 20.0f;
            }
            if (fForceFactor > 0.0f && m_pSourceEntity) {
                fForceFactor *= -5.0f;
                ped->bWasStanding = false;
                ped->bIsStanding = false;
                CVector vecForce = m_pSourceEntity->GetPosition() - ped->GetPosition();
                vecForce.Normalise();
                vecForce *= fForceFactor;
                vecForce.z = 5.0f;
                ped->ApplyMoveForce(vecForce);
            }
        }
    }
}

// 0x4B3FC0
void CEventDamage::ComputeDamageAnim(CPed* ped, bool bMakeActiveTaskAbortable) {
    if (ped->bInVehicle && ped->m_pVehicle) {
        CWeaponInfo* pWeaponInfo = CWeaponInfo::GetWeaponInfo((eWeaponType)this->m_weaponType, eWeaponSkill::STD);
        if (!pWeaponInfo->m_nWeaponFire
            && m_weaponType
            && m_weaponType != WEAPON_BRASSKNUCKLE
            && m_weaponType != WEAPON_PARACHUTE
            || pWeaponInfo->m_nDamage > 50)
        {
            m_bFallDown = true;
        }
        m_nAnimID = ANIM_ID_NO_ANIMATION_SET;
        return;
    }

    bool bHeavyHit = false;
    if (m_weaponType == WEAPON_SPAS12_SHOTGUN || m_weaponType == WEAPON_SHOTGUN || m_weaponType == WEAPON_SAWNOFF_SHOTGUN
        || m_weaponType == WEAPON_DESERT_EAGLE || m_weaponType == WEAPON_COUNTRYRIFLE) {
        bHeavyHit = true;
    }
    if (ped->bUpperBodyDamageAnimsOnly)
        bHeavyHit = false;

    float fPedStrength = 1.0f;
    float fHealthThreshold = 15.0f;
    float fForceFactor = 0.0f;
    CTaskSimpleFight* sourceEntityTaskFight = nullptr;
    CTaskSimpleUseGun* sourceEntityTaskUseGun = nullptr;
    if (m_pSourceEntity && m_pSourceEntity->IsPed()) {
        auto* pedSourceEntity = m_pSourceEntity->AsPed();
        sourceEntityTaskFight = pedSourceEntity->m_pIntelligence->GetTaskFighting();
        if (!sourceEntityTaskFight)
            sourceEntityTaskUseGun = pedSourceEntity->m_pIntelligence->GetTaskUseGun();
        if (pedSourceEntity->m_pPlayerData) {
            if (pedSourceEntity->m_pPlayerData->m_bAdrenaline) {
                fPedStrength = 2.0f;
            }
            else {
                fPedStrength = (CStats::GetFatAndMuscleModifier(STAT_MOD_4) + 1.0f) * 0.5f;
                fHealthThreshold = 10.0f * 0.5f + 10.0f;
            }
        }
    }

    CTask* pSimplestActiveTask = ped->GetTaskManager().GetSimplestActiveTask();

    const auto headPos = ped->GetBonePosition(BONE_HEAD, false);
    if (headPos.z < ped->GetPosition().z && !ped->IsPlayer()
        && pSimplestActiveTask
        && (pSimplestActiveTask->GetTaskType() == TASK_SIMPLE_FALL|| pSimplestActiveTask->GetTaskType() == TASK_SIMPLE_GET_UP))
    {
        m_bFallDown = true;
        m_nAnimID = RpAnimBlendClumpGetFirstAssociation(ped->m_pRwClump, ANIMATION_IS_FRONT) ? ANIM_ID_FLOOR_HIT_F : ANIM_ID_FLOOR_HIT;
    }
    else if (m_pedPieceType == PED_PIECE_TORSO) {
        bool bMultiplyForceWithPedStrength = false;
        if (sourceEntityTaskUseGun && sourceEntityTaskUseGun->GetLastGunCommand() == eGunCommand::PISTOLWHIP) {
            if (m_ucDirection)
                bMultiplyForceWithPedStrength = true;
            if (ped->IsPlayer())
                bMultiplyForceWithPedStrength = false;
        }
        else {
            if (bHeavyHit && !ped->IsPlayer())
                bMultiplyForceWithPedStrength = true;
        }
        if (ped->bUpperBodyDamageAnimsOnly || ped->bIsDucking || ped->m_pAttachedTo || !bMultiplyForceWithPedStrength || !m_pSourceEntity) {
            if (m_weaponType >= WEAPON_CHAINSAW || ped->m_fHealth >= fHealthThreshold) {
                if (m_weaponType != WEAPON_FALL ||  !m_pSourceEntity  || !m_pSourceEntity->IsObject()) {
                    if (sourceEntityTaskFight && sourceEntityTaskFight->GetMove() == eMeleeMove::MOVING && !ped->IsPlayer() && ped->m_nMoveState > PEDMOVE_WALK)
                        m_bFallDown = true;
                }
                else {
                    m_bFallDown = true;
                }
            }
            else {
                m_bFallDown = true;
                fForceFactor = fPedStrength * 5.0f;
            }
        }
        else {
            m_bFallDown = true;
            fForceFactor = 5.0f;
        }
    }

    bool bPlayHitAnim = true;
    bool bNoComboHit = false;
    if (m_bFallDown && (m_ucDirection || !sourceEntityTaskFight || !sourceEntityTaskFight->IsComboSet() && !sourceEntityTaskFight->IsHitComboSet())) {
        bNoComboHit = true;
        bPlayHitAnim = false;
    }
    else {
        m_bFallDown = false;
        if (!m_ucDirection) {
            if (   sourceEntityTaskFight
                && sourceEntityTaskFight->GetCurrentComboSet() >= eMeleeCombo::UNARMED_1
                && sourceEntityTaskFight->GetMove() <= eMeleeMove::MOVING
            ) {
                m_nAnimGroup = sourceEntityTaskFight->GetComboAnimGroupID();
                m_nAnimID = (AnimationId)(+sourceEntityTaskFight->GetMove() + ANIM_ID_FIGHT_HIT_1);
                m_fAnimBlend = 16.0f;
                if (sourceEntityTaskFight->IsComboSet())
                    m_bFallDown = true;
                bPlayHitAnim = false;
            }
            else if (sourceEntityTaskUseGun && sourceEntityTaskUseGun->m_LastCmd == eGunCommand::PISTOLWHIP) {
                m_nAnimGroup = CTaskSimpleFight::m_aComboData[0].AnimGroup;
                m_nAnimID = ANIM_ID_FIGHT_HIT_2;
                m_fAnimBlend = 16.0f;
                bPlayHitAnim = false;
            }
        }
    }

    if (bPlayHitAnim) {
        if (m_weaponType < WEAPON_PISTOL || m_weaponType > WEAPON_COUNTRYRIFLE || bHeavyHit && !ped->IsPlayer()
            || (!ped->IsPlayer() || bHeavyHit)
            && ped->m_nMoveState <= PEDMOVE_STILL && !ped->bIsDucking && !ped->bUpperBodyDamageAnimsOnly)
        {
            bool bPlayBodyPartHitAnim = true;
            if (ped->IsPlayer() && m_weaponType >= WEAPON_PISTOL) {
              CPlayerPedData * playerData = ped->m_pPlayerData;
                if (CTimer::GetTimeInMS() <= playerData->m_nHitAnimDelayTimer || ped->m_nPedState == PEDSTATE_DRIVING) {
                    m_nAnimID = ANIM_ID_NO_ANIMATION_SET;
                    bPlayBodyPartHitAnim = false;
                }
                else {
                    playerData->m_nHitAnimDelayTimer = CTimer::GetTimeInMS() + 2500; // originally 2500.0f
                }
            }
            if (bPlayBodyPartHitAnim) {
                if (m_weaponType < WEAPON_PISTOL || m_weaponType > WEAPON_LAST_WEAPON) {
                    m_nAnimID = (AnimationId)(m_ucDirection + ANIM_ID_HIT_FRONT);
                    if (m_ucDirection || m_damageResponse.m_fDamageArmor + m_damageResponse.m_fDamageHealth <= 20.0f) {
                        if (m_ucDirection == 2 && m_weaponType <= WEAPON_CANE)
                            m_nAnimID = ANIM_ID_HIT_BEHIND;
                    }
                    else {
                        m_nAnimID = ANIM_ID_HIT_WALK;
                    }
                }
                else {
                    int32 currentEventAnimId = -1;
                    CEvent* pCurrentEvent = ped->m_pIntelligence->m_eventHandler.GetHistory().GetCurrentEvent();
                    if (pCurrentEvent && pCurrentEvent->GetEventType() == EVENT_DAMAGE)
                        currentEventAnimId = static_cast<CEventDamage*>(pCurrentEvent)->m_nAnimID;
                    switch (m_pedPieceType)
                    {
                    case PED_PIECE_LEFT_ARM:
                        if (m_ucDirection == 2)
                            m_nAnimID = ANIM_ID_DAM_ARML_FRMBK;
                        else if (m_ucDirection == 1)
                            m_nAnimID = ANIM_ID_DAM_ARML_FRMLT;
                        else
                            m_nAnimID = ANIM_ID_DAM_ARML_FRMFT;
                        if (m_nAnimID == currentEventAnimId) {
                            do {
                                m_nAnimID = CGeneral::RandomChoiceFromList({ANIM_ID_DAM_ARML_FRMBK, ANIM_ID_DAM_ARML_FRMFT, ANIM_ID_DAM_ARML_FRMLT});
                            } while (m_nAnimID == currentEventAnimId);
                        }
                        break;
                    case PED_PIECE_RIGHT_ARM:
                        if (m_ucDirection == 2)
                            m_nAnimID = ANIM_ID_DAM_ARMR_FRMBK;
                        else if (m_ucDirection == 3)
                            m_nAnimID = ANIM_ID_DAM_ARMR_FRMRT;
                        else
                            m_nAnimID = ANIM_ID_DAM_ARMR_FRMFT;
                        if (m_nAnimID == currentEventAnimId) {
                            do {
                                m_nAnimID = CGeneral::RandomChoiceFromList({ANIM_ID_DAM_ARMR_FRMBK, ANIM_ID_DAM_ARMR_FRMFT, ANIM_ID_DAM_ARMR_FRMRT});
                            } while (m_nAnimID == currentEventAnimId);
                        }
                        break;
                    case PED_PIECE_LEFT_LEG:
                        if (m_ucDirection == 2)
                            m_nAnimID = ANIM_ID_DAM_LEGL_FRMBK;
                        else if (m_ucDirection == 1)
                            m_nAnimID = ANIM_ID_DAM_LEGL_FRMLT;
                        else
                            m_nAnimID = ANIM_ID_DAM_LEGL_FRMFT;
                        if (m_nAnimID == currentEventAnimId) {
                            do {
                                m_nAnimID = CGeneral::RandomChoiceFromList({ANIM_ID_DAM_LEGL_FRMBK, ANIM_ID_DAM_LEGL_FRMFT, ANIM_ID_DAM_LEGL_FRMLT});
                            } while (m_nAnimID == currentEventAnimId);      
                        }                                                   
                        break;
                    case PED_PIECE_RIGHT_LEG:
                        if (m_ucDirection == 2)
                            m_nAnimID = ANIM_ID_DAM_LEGR_FRMBK;
                        else if (m_ucDirection == 3)
                            m_nAnimID = ANIM_ID_DAM_LEGR_FRMRT;
                        else
                            m_nAnimID = ANIM_ID_DAM_LEGR_FRMFT;
                        if (m_nAnimID == currentEventAnimId) {
                            do {
                                m_nAnimID = CGeneral::RandomChoiceFromList({ANIM_ID_DAM_LEGR_FRMBK, ANIM_ID_DAM_LEGR_FRMFT, ANIM_ID_DAM_LEGR_FRMRT});
                            } while (m_nAnimID == currentEventAnimId);
                        }
                        break;
                    case PED_PIECE_TORSO:
                    case PED_PIECE_ASS:
                        if (m_ucDirection) {
                            switch (m_ucDirection)
                            {
                            case 1:
                                m_nAnimID = ANIM_ID_DAM_STOMACH_FRMLT;
                                break;
                            case 2:
                                m_nAnimID = ANIM_ID_DAM_STOMACH_FRMBK;
                                break;
                            case 3:
                                m_nAnimID = ANIM_ID_DAM_STOMACH_FRMRT;
                                break;
                            }
                        }
                        else {
                            m_nAnimID = ANIM_ID_DAM_STOMACH_FRMFT;
                        }
                        if (m_nAnimID == currentEventAnimId) {
                            do {
                                m_nAnimID = CGeneral::RandomChoiceFromList({ANIM_ID_DAM_STOMACH_FRMBK, ANIM_ID_DAM_STOMACH_FRMFT, ANIM_ID_DAM_STOMACH_FRMLT, ANIM_ID_DAM_STOMACH_FRMRT});
                            } while (m_nAnimID == currentEventAnimId);
                        }
                        break;
                    default:
                        m_nAnimID = (AnimationId)(m_ucDirection + ANIM_ID_HIT_FRONT);
                        break;
                    }
                }
            }
        }
        else
        {
            m_nAnimGroup = ANIM_GROUP_DEFAULT;
            m_fAnimBlend = 8.0f;
            m_nAnimID = (AnimationId)(m_ucDirection + ANIM_ID_SHOT_PARTIAL);
            if (ped->IsPlayer()) {
                if (CTimer::GetTimeInMS() > ped->m_pPlayerData->m_nHitAnimDelayTimer && ped->m_nPedState != PEDSTATE_DRIVING) {
                    if (m_weaponType == WEAPON_M4)
                        ped->m_pPlayerData->m_nHitAnimDelayTimer = static_cast<uint32>(float(CTimer::GetTimeInMS()) + 2500.0f);
                    else
                        ped->m_pPlayerData->m_nHitAnimDelayTimer = static_cast<uint32>(float(CTimer::GetTimeInMS()) + 1500.0f);
                }
                else {
                    m_nAnimID = ANIM_ID_NO_ANIMATION_SET;
                }
            }
        }
    }
    if (bMakeActiveTaskAbortable) {
        CTask* activeTask = ped->GetTaskManager().GetActiveTask();
        if (activeTask && !activeTask->MakeAbortable(ped, ABORT_PRIORITY_URGENT, this)) {
            m_nAnimGroup = ANIM_GROUP_DEFAULT;
            m_nAnimID = ANIM_ID_DOOR_LHINGE_O;
            m_bFallDown = false;
        }
    }

    if (m_bFallDown && bNoComboHit) {
        m_nAnimID = (AnimationId)(m_ucDirection + ANIM_ID_KO_SKID_FRONT);
        if (fForceFactor > 0.0f) {
            ped->bWasStanding = false;
            ped->bIsStanding = false;
            CVector force = m_pSourceEntity->GetPosition() - ped->GetPosition();
            force.Normalise();
            force *= -fForceFactor;
            force.z = 5.0f;
            ped->ApplyMoveForce(force);
        }
    }
}

void CEventDamage::ComputeAnim(CPed* ped, bool bMakeActiveTaskAbortable) {
    if (ped->m_fHealth <= 0.f) {
        ComputeDeathAnim(ped, bMakeActiveTaskAbortable);
    } else {
        ComputeDeathAnim(ped, bMakeActiveTaskAbortable);
    }
}

// NOTSA
void CEventDamage::ComputeDamageResponseIfAffectsPed(CPed* ped, CPedDamageResponseCalculator calculator, bool bSpeak) {
    if (AffectsPed(ped)) {
        calculator.ComputeDamageResponse(ped, m_damageResponse, bSpeak);
    } else {
        m_damageResponse.SetDamageAsCalculated();
    }
}

// 0x4ADD60
bool CEventDamage::WasHeadShot() const {
    switch (m_weaponType) {
    case WEAPON_UNARMED:
    case WEAPON_BRASSKNUCKLE:
    case WEAPON_GOLFCLUB:
    case WEAPON_NIGHTSTICK:
    case WEAPON_KNIFE:
    case WEAPON_BASEBALLBAT:
    case WEAPON_SHOVEL:
    case WEAPON_POOL_CUE:
    case WEAPON_KATANA:
    case WEAPON_CHAINSAW:
    case WEAPON_DILDO1:
    case WEAPON_DILDO2:
    case WEAPON_VIBE1:
    case WEAPON_VIBE2:
    case WEAPON_FLOWERS:
    case WEAPON_CANE:
    case WEAPON_PARACHUTE:
        return m_damageResponse.m_bForceDeath;
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
    case WEAPON_UZI_DRIVEBY:
        return m_damageResponse.m_bForceDeath && m_pedPieceType == PED_PIECE_HEAD;
    }
    return false;
}
