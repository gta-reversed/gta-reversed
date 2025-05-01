#include "StdInc.h"

#include "TaskSimpleSwim.h"
#include "TaskSimpleClimb.h"

void CTaskSimpleSwim::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleSwim, 0x870920, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x688930);
    RH_ScopedVMTInstall(ProcessPed, 0x68B1C0);
    RH_ScopedVMTInstall(MakeAbortable, 0x68B100);
    RH_ScopedInstall(ApplyRollAndPitch, 0x68A8E0);
    RH_ScopedInstall(ProcessSwimAnims, 0x6899F0);
    RH_ScopedInstall(ProcessSwimmingResistance, 0x68A1D0);
    RH_ScopedInstall(ProcessEffects, 0x68AA70);
    RH_ScopedInstall(ProcessControlAI, 0x689640);
    RH_ScopedInstall(ProcessControlInput, 0x688A90);
    RH_ScopedInstall(CreateFxSystem, 0x68A9F0);
    RH_ScopedInstall(DestroyFxSystem, 0x68AA50);
}
CTaskSimpleSwim* CTaskSimpleSwim::Constructor(CVector* pos, CPed* ped) { this->CTaskSimpleSwim::CTaskSimpleSwim(pos, ped); return this; }

// 0x688930
CTaskSimpleSwim::CTaskSimpleSwim(const CVector* pos, CPed* ped) :
    m_vecPos{pos ? *pos : CVector{}}
{
    m_bFinishedBlending = false;
    m_bAnimBlockRefAdded = false;
    m_fAnimSpeed = -1.0f;
    m_pPed = ped;
    m_fRotationX = 0.0f;
    m_fTurningRotationY = 0.0f;
    m_fUpperTorsoRotationX = 0.0f;
    m_fAimingRotation = 0.0f;
    m_fStateChanger = 0.0f;
    m_pEntity = nullptr;
    m_fSwimStopTime = 0.0f;
    m_nTimeStep = 0;
    m_nSwimState = SWIM_TREAD;
    m_AnimID = ANIM_ID_NO_ANIMATION_SET;

    CEntity::SafeRegisterRef(m_pPed);

    m_pFxSystem = nullptr;
    m_bTriggerWaterSplash = false;
    m_fRandomMoveBlendRatio = 0.0f;
}

// 0x688A00
CTaskSimpleSwim::~CTaskSimpleSwim() {
    if (m_bAnimBlockRefAdded) {
        CAnimManager::RemoveAnimBlockRef(CAnimManager::GetAnimationBlockIndex(ANIM_GROUP_SWIM));
    }

    CEntity::SafeCleanUpRef(m_pEntity); // todo: m_pEntity not registered CEntity::SafeRegisterRef() in ctor
    CEntity::SafeCleanUpRef(m_pPed);
}

// 0x68A9F0
void CTaskSimpleSwim::CreateFxSystem(CPed* ped, RwMatrix* mat) {
    CVector point{};
    m_pFxSystem = g_fxMan.CreateFxSystem("water_ripples", point, mat, false);
    if (m_pFxSystem) {
        m_pFxSystem->CopyParentMatrix();
        m_pFxSystem->Play();
    }
}

// 0x68AA50
void CTaskSimpleSwim::DestroyFxSystem() {
    FxSystem_c::SafeKillAndClear(m_pFxSystem);
}

// 0x68B100


bool CTaskSimpleSwim::MakeAbortable(class CPed* ped, eAbortPriority priority, const CEvent* event) {
    if (priority == ABORT_PRIORITY_IMMEDIATE) {
        CAnimManager::BlendAnimation(ped->m_pRwClump, ped->m_nAnimGroup, ANIM_ID_IDLE, 1000.0f);
        ped->m_nMoveState = PEDMOVE_STILL;
        ped->m_nSwimmingMoveState = PEDMOVE_STILL;

        if (m_AnimID != ANIM_ID_NO_ANIMATION_SET) {
            auto assoc = RpAnimBlendClumpGetAssociation(ped->m_pRwClump, m_AnimID);
            if (assoc) {
                assoc->m_BlendDelta = -1000.0f;
            }
        }

        ped->RestoreHeadingRate();

        DestroyFxSystem();
        return true;
    }

    if (!event) {
        return false;
    }
    const auto* dmgEvent = static_cast<const CEventDamage*>(event);
    if (event->GetEventPriority() >= 71 || event->GetEventType() == EVENT_DAMAGE && dmgEvent->m_damageResponse.m_bHealthZero && dmgEvent->m_bAddToEventGroup) {
        DestroyFxSystem();
        return true;
    }
    return false;
}

// 0x68B1C0

// 0x0
bool CTaskSimpleSwim::ProcessPed(CPed* ped) {
    if (m_pEntity) {
        CAnimManager::BlendAnimation(ped->m_pRwClump, ped->m_nAnimGroup, ANIM_ID_IDLE, 8.0f);
        ped->m_nMoveState = PEDMOVE_STILL;
        ped->m_nSwimmingMoveState = PEDMOVE_STILL;

        DestroyFxSystem();
        return true;
    }

    if (m_fSwimStopTime > SWIM_STOP_TIME || ped->bInVehicle) {
        CAnimBlendAssociation* assoc = nullptr;
        if (m_AnimID != ANIM_ID_NO_ANIMATION_SET) {
            assoc = RpAnimBlendClumpGetAssociation(ped->m_pRwClump, m_AnimID);
        }
        AnimationId animId = ANIM_ID_IDLE;
        ped->m_nSwimmingMoveState = PEDMOVE_STILL;
        ped->m_nMoveState = PEDMOVE_STILL;
        if (assoc) {
            if (assoc->m_AnimId == ANIM_ID_CLIMB_JUMP)
                assoc->m_Flags |= ANIMATION_IS_FINISH_AUTO_REMOVE;
            else
                assoc->m_BlendDelta = -4.0f;

            if (m_AnimID == ANIM_ID_SWIM_BREAST) {
                animId = ANIM_ID_WALK;
                ped->m_nSwimmingMoveState = PEDMOVE_WALK;
                ped->m_nMoveState = PEDMOVE_WALK;
            } else if (m_AnimID == ANIM_ID_SWIM_CRAWL) {
                animId = ANIM_ID_RUN;
                ped->m_nSwimmingMoveState = PEDMOVE_RUN;
                ped->m_nMoveState = PEDMOVE_RUN;
            }
        }
        CAnimManager::BlendAnimation(ped->m_pRwClump, ped->m_nAnimGroup, animId, 4.0f);
        ped->RestoreHeadingRate();
        DestroyFxSystem();
        return true;
    }

    ped->bIsInTheAir = false;

    if (ped->IsPlayer()) {
        if (m_nTimeStep && m_nSwimState != SWIM_UNDERWATER_SPRINTING) {
            ProcessControlAI(ped);
            auto swimmingTimeStep = static_cast<uint32>(CTimer::GetTimeStepInMS());
            if (m_nTimeStep <= swimmingTimeStep)
                m_nTimeStep = 0;
            else
                m_nTimeStep -= swimmingTimeStep;

            ped->m_pPlayerData->m_fMoveBlendRatio = DistanceBetweenPoints2D(ped->GetPosition(), m_vecPos);
            if (ped->m_pPlayerData->m_fMoveBlendRatio < 0.5f) {
                ped->m_pPlayerData->m_fMoveBlendRatio = 0.0f;
                CAnimManager::BlendAnimation(ped->m_pRwClump, ped->m_nAnimGroup, ANIM_ID_IDLE, 4.0f);
                ped->RestoreHeadingRate();
                DestroyFxSystem();
                return true;
            }
            ped->m_pPlayerData->m_fMoveBlendRatio = std::min(ped->m_pPlayerData->m_fMoveBlendRatio, 1.0f);
        } else {
            ProcessControlInput(ped->AsPlayer());
            m_nTimeStep = 0;
        }

        bool bDecreaseAir = false;
        float fDecreaseAirMult = 1.0f;
        if (m_nSwimState == SWIM_UNDERWATER_SPRINTING) {
            bDecreaseAir = true;
            CAnimBlendAssociation* assocUnder = RpAnimBlendClumpGetAssociation(ped->m_pRwClump, ANIM_ID_SWIM_UNDER);
            if (assocUnder) {
                fDecreaseAirMult = assocUnder->m_Speed * assocUnder->m_BlendAmount + 1.0f;
            }
        }

        ped->AsPlayer()->HandlePlayerBreath(bDecreaseAir, fDecreaseAirMult);
        if (m_pPed && m_nSwimState != SWIM_UNDERWATER_SPRINTING) {
            if (CStats::GetFatAndMuscleModifier(STAT_MOD_AIR_IN_LUNG) * 0.5f > ped->m_pPlayerData->m_fBreath) {
                ped->Say(CTX_GLOBAL_PAIN_CJ_SWIM_GASP);
            }
        }
        ped->SetMoveState(PEDMOVE_NONE);
    } else {
        ProcessControlAI(ped);

        if (m_nSwimState == SWIM_UNDERWATER_SPRINTING) {
            CPedDamageResponseCalculator damageCalculator(nullptr, CTimer::GetTimeStep(), WEAPON_DROWNING, PED_PIECE_TORSO, false);
            CEventDamage eventDamage(nullptr, CTimer::GetTimeInMS(), WEAPON_DROWNING, PED_PIECE_TORSO, 0, false, ped->bInVehicle);
            if (eventDamage.AffectsPed(ped))
                damageCalculator.ComputeDamageResponse(ped, eventDamage.m_damageResponse, true);
            else
                eventDamage.m_damageResponse.m_bDamageCalculated = true;
            ped->GetEventGroup().Add(&eventDamage, false);
        }
    }

    ProcessSwimAnims(ped);
    ProcessSwimmingResistance(ped);
    ProcessEffects(ped);
    return false;
}

// 0x68A8E0
void CTaskSimpleSwim::ApplyRollAndPitch(CPed* ped) const {
    LimbOrientation orientation(m_fAimingRotation, m_fUpperTorsoRotationX);
    ped->m_pedIK.RotateTorso(ped->m_apBones[PED_NODE_UPPER_TORSO], orientation, rwCOMBINEREPLACE);
    if (ped->m_pRwObject) {
        CMatrix pedMatrix(ped->GetModellingMatrix(), false);
        CMatrix rotationMatrix;
        rotationMatrix.SetTranslate(CVector{});
        rotationMatrix.RotateY(m_fTurningRotationY);
        rotationMatrix.RotateX(m_fRotationX);
        pedMatrix *= rotationMatrix;
        pedMatrix.UpdateRW();
        ped->UpdateRwFrame();
    }
}

// 0x6899F0
void CTaskSimpleSwim::ProcessSwimAnims(CPed* ped) {
    auto* player = ped->AsPlayer();
    const CVector& pedPos = player->GetPosition();
    CAnimBlendAssociation* assocTread = RpAnimBlendClumpGetAssociation(ped->m_pRwClump, ANIM_ID_SWIM_TREAD);
    if (m_bFinishedBlending) {
        if (assocTread && assocTread->m_BlendAmount < 1.0f && assocTread->m_BlendDelta <= 0.0f) {
            CAnimManager::BlendAnimation(player->m_pRwClump, ANIM_GROUP_DEFAULT, ANIM_ID_SWIM_TREAD, 8.0f);
        }
    } else {
        if (assocTread || (assocTread = CAnimManager::BlendAnimation(player->m_pRwClump, ANIM_GROUP_DEFAULT, ANIM_ID_SWIM_TREAD, 8.0f)) != nullptr) {
            if (assocTread->m_BlendAmount >= 1.0f) {
                m_bFinishedBlending = true;
            }
        }
        RpAnimBlendClumpSetBlendDeltas(player->m_pRwClump, 0x10, -8.0f); // todo: ANIMATION_IS_PARTIAL ?
        FxSystem_c::SafeKillAndClear(player->GetActiveWeapon().m_FxSystem); // Removes fire or something in water

        if (player->IsPlayer() && !m_nSwimState) {
            float waterLevel = 0.0f;
            if (CWaterLevel::GetWaterLevel(pedPos, waterLevel, true)) {
                if (waterLevel - 2.0f > pedPos.z) {
                    m_nSwimState = SWIM_UNDERWATER_SPRINTING;
                }
            }
        }
    }

    if (!m_bAnimBlockRefAdded) {
        CAnimBlock* animBlock = CAnimManager::GetAnimationBlock(ANIM_GROUP_SWIM);

        if (!animBlock) {
            const char* blockName = CAnimManager::GetAnimBlockName(ANIM_GROUP_SWIM);
            animBlock = CAnimManager::GetAnimationBlock(blockName);
        }

        const auto blockIndex = CAnimManager::GetAnimationBlockIndex(animBlock);
        if (animBlock->IsLoaded) {
            CAnimManager::AddAnimBlockRef(blockIndex);
            m_bAnimBlockRefAdded = true;
        } else {
            CStreaming::RequestModel(IFPToModelId(blockIndex), STREAMING_KEEP_IN_MEMORY);
        }
    }

    if (!m_bFinishedBlending || !m_bAnimBlockRefAdded)
        return;

    switch (m_nSwimState) {
    case SWIM_TREAD: {
        if (m_AnimID == ANIM_ID_SWIM_TREAD)
            break;

        constexpr struct { AnimationId anim; float delta; } mapping[] = {
            { ANIM_ID_SWIM_BREAST,      -1.0f },
            { ANIM_ID_SWIM_CRAWL,       -1.0f },
            { ANIM_ID_SWIM_DIVE_UNDER,  -4.0f },
            { ANIM_ID_SWIM_UNDER,       -2.0f },
            { ANIM_ID_SWIM_GLIDE,       -2.0f },
            { ANIM_ID_SWIM_JUMPOUT,     -4.0f },
            { ANIM_ID_CLIMB_JUMP,       -4.0f },
        };
        for (const auto& [anim, delta] : mapping) {
            if (auto* assoc = RpAnimBlendClumpGetAssociation(player->m_pRwClump, anim)) {
                assoc->m_BlendDelta = delta;
            }
        }
        m_AnimID = ANIM_ID_SWIM_TREAD;
        break;
    }
    case SWIM_SPRINT: {
        if (m_AnimID == ANIM_ID_SWIM_BREAST) {
            CAnimBlendAssociation* assoc = RpAnimBlendClumpGetAssociation(player->m_pRwClump, ANIM_ID_SWIM_BREAST);
            if (!assoc) {
                m_nSwimState = SWIM_TREAD;
                break;
            }
            if (player->IsPlayer()) {
                assoc->m_Speed = player->m_pPlayerData->m_fMoveBlendRatio;
            } else if (m_fAnimSpeed > 0.0f && m_fAnimSpeed <= 1.0f) {
                assoc->m_Speed = m_fAnimSpeed;
            }
        } else {
            CAnimManager::BlendAnimation(player->m_pRwClump, ANIM_GROUP_SWIM, ANIM_ID_SWIM_BREAST, 2.0f);
            m_AnimID = ANIM_ID_SWIM_BREAST;
        }
        break;
    }
    case SWIM_SPRINTING: {
        if (m_AnimID == ANIM_ID_SWIM_CRAWL) {
            CAnimBlendAssociation* assoc = RpAnimBlendClumpGetAssociation(player->m_pRwClump, ANIM_ID_SWIM_CRAWL);
            if (!assoc) {
                m_nSwimState = SWIM_TREAD;
                break;
            }
            if (player->m_pPlayerData) {
                float buttonSprintResults = player->GetButtonSprintResults(SPRINT_WATER);
                assoc->m_Speed = std::max(1.0f, buttonSprintResults);
            } else if (m_fAnimSpeed > 1.0f) {
                assoc->m_Speed = std::min(1.5f, m_fAnimSpeed - 1.0f);
            }
        } else {
            CAnimManager::BlendAnimation(player->m_pRwClump, ANIM_GROUP_SWIM, ANIM_ID_SWIM_CRAWL, 2.0f);
            m_AnimID = ANIM_ID_SWIM_CRAWL;
        }
        break;
    }
    case SWIM_DIVE_UNDERWATER: {
        if (m_AnimID == ANIM_ID_SWIM_DIVE_UNDER) {
            CAnimBlendAssociation* assocUnder = RpAnimBlendClumpGetAssociation(player->m_pRwClump, ANIM_ID_SWIM_DIVE_UNDER);
            if (!assocUnder) {
                m_nSwimState = SWIM_TREAD;
                break;
            }

            if (assocUnder->m_BlendHier->m_fTotalTime == assocUnder->m_CurrentTime) {
                m_nSwimState = SWIM_UNDERWATER_SPRINTING;
            }
        } else {
            CAnimManager::BlendAnimation(player->m_pRwClump, ANIM_GROUP_SWIM, ANIM_ID_SWIM_DIVE_UNDER, 8.0f);
            m_AnimID = ANIM_ID_SWIM_DIVE_UNDER;
        }
        break;
    }
    case SWIM_UNDERWATER_SPRINTING: {
        if ((m_AnimID == ANIM_ID_SWIM_UNDER || m_AnimID == ANIM_ID_SWIM_GLIDE) && m_fStateChanger >= 0.0f) {
            if (player->m_pPlayerData && player->GetButtonSprintResults(SPRINT_UNDERWATER) >= 1.0f) {
                CAnimBlendAssociation* assocUnder = RpAnimBlendClumpGetAssociation(player->m_pRwClump, ANIM_ID_SWIM_UNDER);
                if (!assocUnder || assocUnder->m_BlendDelta < 0.0f || assocUnder->m_BlendAmount == 0.0f) {
                    assocUnder = CAnimManager::BlendAnimation(player->m_pRwClump, ANIM_GROUP_SWIM, ANIM_ID_SWIM_UNDER, 4.0f);
                }
                if (assocUnder->m_BlendHier->m_fTotalTime == assocUnder->m_CurrentTime) {
                    assocUnder->Start(0.0f);
                    assocUnder->m_Speed = std::max(0.7f, player->GetButtonSprintResults(SPRINT_UNDERWATER));
                }
                m_AnimID = ANIM_ID_SWIM_UNDER;
            } else {
                CAnimBlendAssociation* assocGlide = RpAnimBlendClumpGetAssociation(player->m_pRwClump, ANIM_ID_SWIM_GLIDE);
                if (!assocGlide || assocGlide->m_BlendDelta < 0.0f || assocGlide->m_BlendAmount == 0.0f) {
                    CAnimManager::BlendAnimation(player->m_pRwClump, ANIM_GROUP_SWIM, ANIM_ID_SWIM_GLIDE, 4.0f);
                }
                m_AnimID = ANIM_ID_SWIM_GLIDE;
            }
        } else {
            CAnimBlendAssociation* assocUnder = RpAnimBlendClumpGetAssociation(player->m_pRwClump, ANIM_ID_SWIM_UNDER);
            if (assocUnder) {
                if (m_fStateChanger < 0.0f && assocUnder->m_BlendAmount >= 0.99f) {
                    if (m_fStateChanger > -2.0f) {
                        m_fStateChanger = 0.0f;
                        m_fRotationX = DegreesToRadians(CTaskSimpleSwim::SWIM_DIVE_UNDER_ANGLE);
                    } else {
                        m_fRotationX = DegreesToRadians(80.0f);
                        m_fStateChanger = 0.0f;
                    }
                }
            } else {
                CAnimManager::BlendAnimation(player->m_pRwClump, ANIM_GROUP_SWIM, ANIM_ID_SWIM_UNDER, 1000.0f);
                if (m_AnimID == ANIM_ID_SWIM_TREAD || m_AnimID == ANIM_ID_NO_ANIMATION_SET) {
                    m_fStateChanger = -2.0f;
                    m_AnimID = ANIM_ID_SWIM_UNDER;
                } else {
                    m_fStateChanger = -1.0f;
                    m_AnimID = ANIM_ID_SWIM_UNDER;
                }
            }
        }
        break;
    }
    case SWIM_BACK_TO_SURFACE: {
        if (m_AnimID == ANIM_ID_SWIM_JUMPOUT) {
            CAnimBlendAssociation* assocJumpOut = RpAnimBlendClumpGetAssociation(player->m_pRwClump, ANIM_ID_SWIM_JUMPOUT);
            if (assocJumpOut) {
                if (assocJumpOut->m_BlendHier->m_fTotalTime / 4.0f <= assocJumpOut->m_TimeStep + assocJumpOut->m_CurrentTime) {
                    assocJumpOut = CAnimManager::BlendAnimation(player->m_pRwClump, ANIM_GROUP_DEFAULT, ANIM_ID_CLIMB_JUMP, 8.0f);
                    assocJumpOut->m_Flags |= ANIMATION_IS_FINISH_AUTO_REMOVE;
                    m_AnimID = ANIM_ID_CLIMB_JUMP;
                }
                break;
            }
        } else {
            if (m_AnimID != ANIM_ID_CLIMB_JUMP) {
                CAnimManager::BlendAnimation(player->m_pRwClump, ANIM_GROUP_SWIM, ANIM_ID_SWIM_JUMPOUT, 8.0f);
                m_AnimID = ANIM_ID_SWIM_JUMPOUT;
                player->m_vecMoveSpeed.z = 8.0f / player->m_fMass;
                m_pEntity = CTaskSimpleClimb::TestForClimb(player, m_pClimbPos, m_fAngle, m_nSurfaceType, true);
                CEntity::SafeRegisterRef(m_pEntity);
                break;
            }
            if (RpAnimBlendClumpGetAssociation(player->m_pRwClump, ANIM_ID_CLIMB_JUMP)) { // 0x689C4B
                float waterLevel1 = 0.0f;
                if (CWaterLevel::GetWaterLevel(pedPos, waterLevel1, true)) {
                    if (pedPos.z + 0.5f < waterLevel1) {
                        return; // todo: check this
                    }
                    break;
                }
            }
        }
        m_nSwimState = SWIM_TREAD;
        break;
    }
    default: {
        break;
    }
    }

    if (player->m_pPlayerData && (player->m_pPlayerData->m_fMoveBlendRatio > 0.5f || m_nSwimState == SWIM_UNDERWATER_SPRINTING)) {
        CStats::UpdateStatsWhenSwimming(m_nSwimState == SWIM_UNDERWATER_SPRINTING, m_nSwimState == SWIM_SPRINTING);
    }
}

// 0x68A1D0
void CTaskSimpleSwim::ProcessSwimmingResistance(CPed* ped) {
    float fSubmergeZ = -1.0f;
    CVector vecPedMoveSpeed{};

    switch (m_nSwimState) {
    case SWIM_TREAD:
    case SWIM_SPRINT:
    case SWIM_SPRINTING: {
        float fAnimBlendSum = 0.0f;
        float fAnimBlendDifference = 1.0f;

        CAnimBlendAssociation* animSwimBreast = RpAnimBlendClumpGetAssociation(ped->m_pRwClump, ANIM_ID_SWIM_BREAST);
        if (animSwimBreast) {
            fAnimBlendSum = 0.4f * animSwimBreast->m_BlendAmount;
            fAnimBlendDifference = 1.0f - animSwimBreast->m_BlendAmount;
        }

        CAnimBlendAssociation* animSwimCrawl = RpAnimBlendClumpGetAssociation(ped->m_pRwClump, ANIM_ID_SWIM_CRAWL);
        if (animSwimCrawl) {
            fAnimBlendSum += 0.2f * animSwimCrawl->m_BlendAmount;
            fAnimBlendDifference -= animSwimCrawl->m_BlendAmount;
        }
        if (fAnimBlendDifference < 0.0f) {
            fAnimBlendDifference = 0.0f;
        }

        fSubmergeZ = fAnimBlendDifference * 0.55f + fAnimBlendSum;

        vecPedMoveSpeed =  ped->m_vecAnimMovingShiftLocal.x * ped->GetRight();
        vecPedMoveSpeed += ped->m_vecAnimMovingShiftLocal.y * ped->GetForward();
        break;
    }
    case SWIM_DIVE_UNDERWATER: {
        vecPedMoveSpeed =  ped->m_vecAnimMovingShiftLocal.x * ped->GetRight();
        vecPedMoveSpeed += ped->m_vecAnimMovingShiftLocal.y * ped->GetForward();

        auto animSwimDiveUnder = RpAnimBlendClumpGetAssociation(ped->m_pRwClump, ANIM_ID_SWIM_DIVE_UNDER);
        if (animSwimDiveUnder) {
            vecPedMoveSpeed.z = animSwimDiveUnder->m_CurrentTime / animSwimDiveUnder->m_BlendHier->m_fTotalTime * -0.1f;
        }
        break;
    }
    case SWIM_UNDERWATER_SPRINTING: {
        vecPedMoveSpeed   =  ped->m_vecAnimMovingShiftLocal.x * ped->GetRight();
        vecPedMoveSpeed   += std::cos(m_fRotationX) * ped->m_vecAnimMovingShiftLocal.y * ped->GetForward();
        vecPedMoveSpeed.z += std::sin(m_fRotationX) * ped->m_vecAnimMovingShiftLocal.y + 0.01f;
        break;
    }
    case SWIM_BACK_TO_SURFACE: {
        auto animClimb = RpAnimBlendClumpGetAssociation(ped->m_pRwClump, ANIM_ID_CLIMB_JUMP);
        if (!animClimb)
            animClimb = RpAnimBlendClumpGetAssociation(ped->m_pRwClump, ANIM_ID_SWIM_JUMPOUT);

        if (animClimb) {
            if (animClimb->m_BlendHier->m_fTotalTime > animClimb->m_CurrentTime &&
                (animClimb->m_BlendAmount >= 1.0f || animClimb->m_BlendDelta > 0.0f)
            ) {
                float fMoveForceZ = CTimer::GetTimeStep() * ped->m_fMass * 0.3f * 0.008f;
                ped->ApplyMoveForce(0.0f, 0.0f, fMoveForceZ);
            }
        }
        return;
    }
    default: {
        return;
    }
    }

    float fTheTimeStep = std::pow(0.9f, CTimer::GetTimeStep());
    vecPedMoveSpeed *= 1.0f - fTheTimeStep;
    ped->m_vecMoveSpeed *= fTheTimeStep;
    ped->m_vecMoveSpeed += vecPedMoveSpeed;

    auto& pedPos = ped->GetPosition();
    bool bUpdateRotationX = true;
    CVector vecCheckWaterLevelPos = CTimer::GetTimeStep() * ped->m_vecMoveSpeed + pedPos;
    float fWaterLevel = 0.0f;
    if (!CWaterLevel::GetWaterLevel(vecCheckWaterLevelPos, fWaterLevel, true)) {
        fSubmergeZ = -1.0f;
        bUpdateRotationX = false;
    } else {
        if (m_nSwimState != SWIM_UNDERWATER_SPRINTING || m_fStateChanger < 0.0f) {
            bUpdateRotationX = false;
        } else {
            if (pedPos.z + 0.65f > fWaterLevel && m_fRotationX > 0.7854f) { // todo: magic number
                m_nSwimState = SWIM_TREAD;
                m_fStateChanger = 0.0f;
                bUpdateRotationX = false;
            }
        }
    }

    if (bUpdateRotationX) {
        if (m_fRotationX >= 0.0f) {
            if (pedPos.z + 0.65f <= fWaterLevel) {
                if (m_fStateChanger <= 0.001f)
                    m_fStateChanger = 0.0f;
                else
                    m_fStateChanger *= 0.95f;
            } else {
                float fMinimumSpeed = 0.05f * 0.5f;
                if (m_fStateChanger > fMinimumSpeed) {
                    m_fStateChanger *= 0.95f;
                }
                if (m_fStateChanger < fMinimumSpeed) {
                    m_fStateChanger += CTimer::GetTimeStepInSeconds() / 10.0f;
                    m_fStateChanger = std::min(fMinimumSpeed, m_fStateChanger);
                }
                m_fRotationX += CTimer::GetTimeStep() * m_fStateChanger;
                fSubmergeZ = (0.55f - 0.2f) * (m_fRotationX * 4.0f / PI) * 0.75f + 0.2f;
            }
        } else {
            if (pedPos.z - sin(m_fRotationX) + 0.65f <= fWaterLevel) {
                if (m_fStateChanger > 0.001f)
                    m_fStateChanger *= 0.95f;
                else
                    m_fStateChanger = 0.0f;
            } else {
                m_fStateChanger += CTimer::GetTimeStepInSeconds() / 10.0f;
                m_fStateChanger = std::min(m_fStateChanger, 0.05f);
            }
            m_fRotationX += CTimer::GetTimeStep() * m_fStateChanger;
        }
    }

    if (fSubmergeZ > 0.0f) {
        fWaterLevel -= fSubmergeZ + pedPos.z;
        float fTimeStepMoveSpeedZ = fWaterLevel / CTimer::GetTimeStep();
        float fTimeStep = CTimer::GetTimeStep() * 0.1f;
        fTimeStepMoveSpeedZ = std::clamp(fTimeStepMoveSpeedZ, -fTimeStep, fTimeStep);
        fTimeStepMoveSpeedZ -= ped->m_vecMoveSpeed.z;
        fTimeStep = CTimer::GetTimeStepInSeconds();
        fTimeStepMoveSpeedZ = std::clamp(fTimeStepMoveSpeedZ, -fTimeStep, fTimeStep);
        ped->m_vecMoveSpeed.z += fTimeStepMoveSpeedZ;
    }

    if (pedPos.z < -69.0f) {
        pedPos.z = -69.0f;
        ped->m_vecMoveSpeed.z = std::max(ped->m_vecMoveSpeed.z, 0.0f);
    }
}

// 0x68AA70
void CTaskSimpleSwim::ProcessEffects(CPed* ped) {
    const auto& pedPos = ped->GetPosition();
    CVector particlePosition = ped->GetForwardVector();
    particlePosition *= 0.4f;
    particlePosition += pedPos;

    if (ped->m_pPlayerData)
        particlePosition.z = ped->m_pPlayerData->m_fWaterHeight;
    else
        particlePosition.z += 0.5f;

    if (m_nSwimState != SWIM_TREAD) {
        DestroyFxSystem();
    } else {
        RwMatrix* mat = RwMatrixCreate();
        mat->pos = *((RwV3d*)&particlePosition);
        RwMatrixUpdate(mat);
        if (!m_pFxSystem) {
            CreateFxSystem(ped, mat);
        }

        if (m_pFxSystem) {
            m_pFxSystem->SetMatrix(mat);
            m_pFxSystem->SetLocalParticles(true);
        }
        RwMatrixDestroy(mat);
    }

    if (m_nSwimState != SWIM_DIVE_UNDERWATER)
        m_bTriggerWaterSplash = false;

    switch (m_nSwimState) {
    case SWIM_SPRINT:
    case SWIM_SPRINTING: {
        const CVector& vecPedUp = ped->GetForward();

        float fRadianAngle = CGeneral::GetAngleBetweenPoints(vecPedUp.x, vecPedUp.y, 0.0f, 0.0f);
        float fLimitedRadianAngle = CGeneral::LimitAngle(fRadianAngle) + 180.0f;

        FxPrtMult_c fxPrtMult(1.0f, 1.0f, 1.0f, 0.2f, 0.4f, 0.0f, 0.5f);
        CVector particleVelocity;
        g_fx.m_Wake->AddParticle(&particlePosition, &particleVelocity, 0.0f, &fxPrtMult, fLimitedRadianAngle, 1.2f, 0.6f, 0);
        ped->GetAE().AddAudioEvent(AE_PED_SWIM_WAKE, 0.0f, 1.0f);

        if (m_nSwimState == SWIM_SPRINTING) {
            RpHAnimHierarchy* animHierarchy = GetAnimHierarchyFromSkinClump(ped->m_pRwClump); // todo: almost CPed::GetBoneMatrix
            const auto GetBonePos = [&](auto boneId) {
                auto index = RpHAnimIDGetIndex(animHierarchy, boneId);
                return &RpHAnimHierarchyGetMatrixArray(animHierarchy)[index].pos;
            };

            const auto AddAudioEvent = [&](RwV3d* bonePos) {
                if (std::fabs(bonePos->z - pedPos.z) < 0.05f) {
                    auto fx = g_fxMan.CreateFxSystem("water_swim", *bonePos, nullptr, false);
                    if (fx) {
                        fx->PlayAndKill();
                        ped->GetAE().AddAudioEvent(AE_PED_SWIM_STROKE_SPLASH, 0.0f, 1.0f);
                    }
                }
            };
            for (const auto& bone : { BONE_R_HAND, BONE_L_HAND, BONE_R_FOOT, BONE_L_FOOT }) {
                AddAudioEvent(GetBonePos(bone));
            }
        }
        break;
    }
    case SWIM_DIVE_UNDERWATER: {
        if (m_bTriggerWaterSplash) {
            return;
        }
        g_fx.TriggerWaterSplash(particlePosition);
        m_bTriggerWaterSplash = true;
        ped->GetAE().AddAudioEvent(AE_PED_SWIM_DIVE_SPLASH, 0.0f, 1.0f);
        break;
    }
    case SWIM_UNDERWATER_SPRINTING: {
        uint32 oxygen = 5;
        if (ped->IsPlayer()) {
            assert(ped->m_pPlayerData);
            oxygen = static_cast<uint32>(((100.0f - ped->m_pPlayerData->m_fBreath / CStats::GetFatAndMuscleModifier(STAT_MOD_AIR_IN_LUNG) * 100.0f) / 3.0f));
        }
        if ((unsigned)CGeneral::GetRandomNumberInRange(0, 100) < oxygen) {
            RpHAnimHierarchy* hier = GetAnimHierarchyFromSkinClump(ped->m_pRwClump); // todo: almost CPed::GetBoneMatrix
            CVector* bonePos = static_cast<CVector*>(&RpHAnimHierarchyGetMatrixArray(hier)[BONE_JAW].pos); // FIX_BUGS: Air bubbles from ass :D (BONE_SPINE1)

            static FxPrtMult_c fxPrtMult(1.0f, 1.0f, 1.0f, 0.25f, 0.3f, 0.0f, 0.5f);
            CVector vecParticleVelocity = { 0.0f, 0.0f, 2.0f };
            g_fx.m_Bubble->AddParticle(bonePos, &vecParticleVelocity, 0.0f, &fxPrtMult, -1.0f, 1.2f, 0.6f, false);
        }
        break;
    }
    }
}

// 0x689640
void CTaskSimpleSwim::ProcessControlAI(CPed* ped) {
    const auto& pedPos = ped->GetPosition();
    m_nSwimState = SWIM_TREAD;
    if (m_pPed) {
        const auto& taskPedPos = m_pPed->GetPosition();
        CVector vecPosition(taskPedPos - pedPos);
        vecPosition.Normalise();
        if (m_fRandomMoveBlendRatio == 0.0f) {
            m_fRandomMoveBlendRatio = float(CGeneral::GetRandomNumber() % 4) / 2.0f + 1.5f;
        }

        vecPosition *= m_fRandomMoveBlendRatio;
        m_vecPos = taskPedPos - vecPosition;
    }

    bool bPedGroupSet = false;
    CVector vecPosition(m_vecPos - pedPos);
    auto dist2d = vecPosition.Magnitude2D();
    if (m_vecPos != 0.0f) {
        ped->m_fAimingRotation = std::atan2(-vecPosition.x, vecPosition.y);

        float fMinimum2DDistanceBetweenPeds = 1.0f;
        if (ped->IsPlayer()) {
            fMinimum2DDistanceBetweenPeds = 0.5f;
        }

        if (dist2d > fMinimum2DDistanceBetweenPeds) {
            m_nSwimState = SWIM_SPRINT;
            if (m_pPed) {
                CPedGroup* pedGroup = CPedGroups::GetPedsGroup(ped);
                if (pedGroup && pedGroup->GetMembership().GetLeader() == m_pPed) {
                    bPedGroupSet = true;
                    auto swimTask = m_pPed->GetIntelligence()->GetTaskSwim();
                    if (swimTask &&
                        swimTask->m_nSwimState == SWIM_SPRINTING &&
                        vecPosition.Magnitude() > 5.0f
                    ) {
                        m_nSwimState = SWIM_SPRINTING;
                    }
                }
            } else if (m_fAnimSpeed > 1.0f && dist2d > 2.0f) {
                m_nSwimState = SWIM_SPRINTING;
            }
        }
    }

    if (m_pPed && m_pPed->bIsStanding && !m_pPed->physicalFlags.bSubmergedInWater ||
        ped->bIsDyingStuck && !m_pPed->physicalFlags.bSubmergedInWater && dist2d < 1.0f) {
        if (!((ped->m_nRandomSeedUpperByte + CTimer::GetFrameCounter() - 4) & 127)) {
            if (CTaskSimpleClimb::TestForClimb(ped, m_pClimbPos, m_fAngle, m_nSurfaceType, true)) {
                m_nSwimState = SWIM_BACK_TO_SURFACE;
            }
        }
    }
    m_fAimingRotation = 0.0f;
    m_fUpperTorsoRotationX = 0.0f;

    if (!bPedGroupSet && !ped->IsPlayer() && m_fAnimSpeed < 0.0f) {
        ped->m_vecAnimMovingShiftLocal /= 2.0f;
    }
}

// 0x688A90
void CTaskSimpleSwim::ProcessControlInput(CPlayerPed* ped) {
    CPlayerPedData* playerData = ped->m_pPlayerData;
    if (!m_bFinishedBlending || !m_bAnimBlockRefAdded) {
        playerData->m_fMoveBlendRatio = 0.0f;
        return;
    }

    CPad* pad = ped->GetPadFromPlayer();
    CVector vecPedWalk{
        (float)pad->GetPedWalkLeftRight() / 128.0f,
        (float)pad->GetPedWalkUpDown()    / 128.0f,
        0.0f
    };
    float fWalkMagnitude = vecPedWalk.Magnitude2D();
    if (m_nSwimState < SWIM_SPRINTING) {
        if (pad->JumpJustDown()) {
            m_nSwimState = SWIM_BACK_TO_SURFACE;
        } else if (pad->WeaponJustDown(ped)) {
            m_nSwimState = SWIM_DIVE_UNDERWATER;
            playerData->m_fMoveBlendRatio = 0.0f;
        }
    }

    switch (m_nSwimState) {
    case SWIM_TREAD:
    case SWIM_SPRINT:
    case SWIM_SPRINTING: {
        float pedWalkX = 0.0f;
        bool bPlayerUse2PlayerControls = false;
        if (CGameLogic::IsPlayerUse2PlayerControls(ped)) {
            bPlayerUse2PlayerControls = true;
            pedWalkX = vecPedWalk.x;
            if (fWalkMagnitude > 0) {
                float negativePedWalkX = -pedWalkX;
                float fRadianAngle = CGeneral::GetRadianAngleBetweenPoints(0.0f, 0.0f, negativePedWalkX, vecPedWalk.y) - TheCamera.m_fOrientation;
                float angle = CGeneral::LimitRadianAngle(fRadianAngle);

                CVector vecPedWalkDirection(0.0f, -std::sin(angle), std::cos(angle));
                if (angle <= ped->m_fCurrentRotation + DegreesToRadians(180.0f)) {
                    if (angle < ped->m_fCurrentRotation - DegreesToRadians(180.0f)) {
                        angle += DegreesToRadians(360.0f);
                    }
                } else {
                    angle -= DegreesToRadians(360.0f);
                }

                float fCurrenRotation = angle - ped->m_fCurrentRotation;
                fCurrenRotation = std::clamp(fCurrenRotation, -1.0f, 1.0f);

                ped->m_fAimingRotation = fCurrenRotation * (CTimer::GetTimeStep() * 0.08f) + ped->m_fCurrentRotation;
                if (ped->m_fAimingRotation <= DegreesToRadians(180.0f)) {
                    if (ped->m_fAimingRotation < -DegreesToRadians(180.0f)) {
                        ped->m_fAimingRotation += DegreesToRadians(360.0f);
                    }
                } else {
                    ped->m_fAimingRotation -= DegreesToRadians(360.0f);
                }
                if (CGameLogic::IsPlayerAllowedToGoInThisDirection(ped, vecPedWalkDirection, 0.0f)) {
                    pedWalkX = (
                        vecPedWalkDirection.x * ped->m_matrix->GetRight().x +
                        vecPedWalkDirection.y * ped->m_matrix->GetRight().y +
                        0.0f                  * ped->m_matrix->GetRight().z
                    );
                    pedWalkX *= fWalkMagnitude;
                    vecPedWalk.y = -((vecPedWalkDirection.y * ped->m_matrix->GetForward().y + ped->m_matrix->GetForward().z * 0.0f + vecPedWalkDirection.x * ped->m_matrix->GetForward().x) * fWalkMagnitude);
                } else {
                    pedWalkX = 0.0f;
                    fWalkMagnitude = 0.0f;
                    vecPedWalk.y = 0.0f;
                }
            }
        } else
        {
            pedWalkX = vecPedWalk.x;
        }

        float fRotation = pedWalkX;

        if (!CCamera::m_bUseMouse3rdPerson) {
            if (fWalkMagnitude <= 0.0f) {
                if (playerData->m_fMoveBlendRatio > 0.0f) {
                    playerData->m_fMoveBlendRatio -= CTimer::GetTimeStep() * 0.05f;
                    if (playerData->m_fMoveBlendRatio < 0.0f) {
                        playerData->m_fMoveBlendRatio = 0.0f;
                    }
                }
            } else {
                bool bUpdateMoveBlendRatio = false;
                if (!bPlayerUse2PlayerControls) {
                    ped->m_fAimingRotation += CTimer::GetTimeStep() * -0.03f * pedWalkX;
                    if (ped->m_fAimingRotation <= DegreesToRadians(180.0f)) {
                        if (ped->m_fAimingRotation < -DegreesToRadians(180.0f)) {
                            ped->m_fAimingRotation += DegreesToRadians(360.0f);
                        }
                    } else {
                        ped->m_fAimingRotation -= DegreesToRadians(360.0f);
                    }

                    if (m_nSwimState) {
                        if (vecPedWalk.y > 0.0f) {
                            fWalkMagnitude -= vecPedWalk.y;
                        }
                    } else {
                        fWalkMagnitude = -vecPedWalk.y;
                    }
                    fWalkMagnitude = std::max(fWalkMagnitude, 0.0f);

                    bUpdateMoveBlendRatio = true;
                }

                if (bPlayerUse2PlayerControls || bUpdateMoveBlendRatio) {
                    float fTimeStep = CTimer::GetTimeStep() * 0.07f;
                    if (fWalkMagnitude - playerData->m_fMoveBlendRatio <= fTimeStep) {
                        playerData->m_fMoveBlendRatio = fWalkMagnitude;
                    } else {
                        playerData->m_fMoveBlendRatio += fTimeStep;
                    }
                }
            }
        } else {
            const auto& camFront = CCamera::GetActiveCamera().m_vecFront;
            ped->m_fAimingRotation = std::atan2(-camFront.x, camFront.y); // heading
            if (TheCamera.GetLookDirection() != LOOKING_DIRECTION_FORWARD) {
                ped->m_fAimingRotation += DegreesToRadians(180.0f);
                if (ped->m_fAimingRotation > DegreesToRadians(180.0f)) {
                    ped->m_fAimingRotation -= DegreesToRadians(360.0f);
                }
            }

            // 0x6891C9
            float negativePedWalkY = -vecPedWalk.y;
            float fTimeStep = CTimer::GetTimeStep() * 0.07f;
            if (negativePedWalkY - playerData->m_fMoveBlendRatio > fTimeStep) {
                playerData->m_fMoveBlendRatio += fTimeStep;
            } else {
                playerData->m_fMoveBlendRatio = negativePedWalkY;
            }

            fRotation = -(ped->m_fAimingRotation - ped->m_fCurrentRotation);
            if (fRotation <= DegreesToRadians(180.0f)) {
                if (fRotation < -DegreesToRadians(180.0f)) {
                    fRotation = DegreesToRadians(360.0f) - ped->m_fAimingRotation - ped->m_fCurrentRotation;
                }
            } else {
                fRotation -= DegreesToRadians(360.0f);
            }
            fRotation = std::clamp(fRotation * 10.0f, -1.0f, 1.0f);
        }

        m_fAimingRotation += CTimer::GetTimeStep() * 0.08f * fRotation;

        if (m_nSwimState == SWIM_SPRINTING) {
            m_fTurningRotationY += CTimer::GetTimeStep() * 0.04f * fRotation;
        } else if (m_nSwimState == SWIM_SPRINT) {
            m_fUpperTorsoRotationX += fabs(pedWalkX) * CTimer::GetTimeStep() * 0.04f;
        }

        if (m_nSwimState == SWIM_SPRINTING) {
            float fWaterLevel1{}, fWaterLevel2{};
            const auto& pedPos = ped->GetPosition();
            CVector distance = pedPos - ped->GetForward();
            CVector position = pedPos + ped->GetForward();
            if (CWaterLevel::GetWaterLevel(position, fWaterLevel1, true) && CWaterLevel::GetWaterLevel(distance, fWaterLevel2, true)) {
                m_fRotationX = std::atan2(fWaterLevel1 - fWaterLevel2, 2.0f) * 1.0f;
            }
        }

        if (ped->ControlButtonSprint(SPRINT_WATER) < 1.0f) {
            m_nSwimState = playerData->m_fMoveBlendRatio > 0.5f ? SWIM_SPRINT : SWIM_TREAD;
        } else {
            m_nSwimState = SWIM_SPRINTING;
        }
        break;
    }
    case SWIM_DIVE_UNDERWATER: {
        if (m_fStateChanger > 0.0f) {
            m_fStateChanger = 0.0f;
        }
        break;
    }
    case SWIM_UNDERWATER_SPRINTING: {
        float rotX{};
        if (CCamera::m_bUseMouse3rdPerson) {
            CVector vecActiveCamFront = CCamera::GetActiveCamera().m_vecFront;
            if (TheCamera.GetLookDirection() != LOOKING_DIRECTION_FORWARD) {
                vecActiveCamFront.x *= -1.0f;
                vecActiveCamFront.y *= -1.0f;
                vecActiveCamFront.z = 0.0f;
            }

            ped->m_fAimingRotation = std::atan2(-vecActiveCamFront.x, vecActiveCamFront.y);
            float fRotation = -(ped->m_fAimingRotation - ped->m_fCurrentRotation);
            if (fRotation <= DegreesToRadians(180.0f)) {
                if (fRotation < -DegreesToRadians(180.0f))
                    fRotation += DegreesToRadians(360.0f);
            } else {
                fRotation -= DegreesToRadians(360.0f);
            }
            fRotation = std::clamp(fRotation * 10.0f, -1.0f, 1.0f);

            m_fTurningRotationY += CTimer::GetTimeStep() * 0.04f * fRotation;
            m_fAimingRotation   += CTimer::GetTimeStep() * 0.08f * fRotation;

            float fRotationX = (std::asin(vecActiveCamFront.z) - m_fRotationX) * 10.0f;
            fRotationX = std::clamp(fRotationX, -1.0f, 1.0f); // originally min/max used
            if (m_fStateChanger == 0.0f || fRotationX > 0.0f) {
                m_fRotationX += CTimer::GetTimeStepInSeconds() * fRotationX;
            }
            rotX = m_fTurningRotationY / 0.5f;
            rotX = std::clamp(rotX, -1.0f, 1.0f);
            rotX = fRotationX + rotX * -0.08f * fRotation;
        } else if (fWalkMagnitude > 0.0f) {
            float fNormalizedWalkMagnitude = 1.0f / fWalkMagnitude;
            vecPedWalk.x   = vecPedWalk.x * fNormalizedWalkMagnitude;
            float pedWalkY = vecPedWalk.y * fNormalizedWalkMagnitude;
            ped->m_fAimingRotation += CTimer::GetTimeStep() * -0.03f * vecPedWalk.x;

            if (ped->m_fAimingRotation <= DegreesToRadians(180.0f)) {
                if (ped->m_fAimingRotation < -DegreesToRadians(180.0f)) {
                    ped->m_fAimingRotation += DegreesToRadians(360.0f);
                }
            } else {
                ped->m_fAimingRotation -= DegreesToRadians(360.0f);
            }

            m_fTurningRotationY += CTimer::GetTimeStep() * 0.04f * vecPedWalk.x;
            m_fAimingRotation   += CTimer::GetTimeStep() * 0.08f * vecPedWalk.x;
            if (m_fStateChanger == 0.0f || pedWalkY > 0.0f) {
                m_fRotationX += CTimer::GetTimeStepInSeconds() * pedWalkY;
            }
            rotX = m_fTurningRotationY / 0.5f;
            rotX = std::clamp(rotX, -1.0f, 1.0f);
            rotX = pedWalkY + rotX * -0.08f * vecPedWalk.x;
        }
        m_fUpperTorsoRotationX += CTimer::GetTimeStep() * -0.08f * std::clamp(rotX, -1.0f, 1.0f);
        m_fRotationX += CTimer::GetTimeStep() / 1000.0f;
        m_fRotationX = std::clamp(m_fRotationX, -DegreesToRadians(80.0f), DegreesToRadians(80.0f));
        // BUG: it should be m_fTimeCanRun <= 0.1f
        if (playerData->m_fTimeCanRun <= 0.0f) {
            playerData->m_fTimeCanRun = 0.1f;
        }
        ped->ControlButtonSprint(SPRINT_UNDERWATER);
        break;
    }
    }

    float fRotation = m_nSwimState == SWIM_UNDERWATER_SPRINTING || m_nSwimState == SWIM_SPRINTING ? 0.95f : 0.95f; // same value like in OG

    if (m_fTurningRotationY > 0.01f || m_fTurningRotationY < -0.01f) {
        m_fTurningRotationY *= std::pow(fRotation, CTimer::GetTimeStep());
    } else {
        m_fTurningRotationY = 0.0f;
    }

    if (m_nSwimState != SWIM_UNDERWATER_SPRINTING && m_nSwimState != SWIM_SPRINTING) {
        if (m_fRotationX > 0.01f || m_fRotationX < -0.01f) {
            m_fRotationX *= std::pow(fRotation, CTimer::GetTimeStep());
        } else {
            m_fRotationX = 0.0f;
        }
    }

    fRotation = m_nSwimState == SWIM_DIVE_UNDERWATER || m_nSwimState == SWIM_BACK_TO_SURFACE ? 0.95f : 0.92f;

    if (m_fAimingRotation > 0.01f || m_fAimingRotation < -0.01f || m_fUpperTorsoRotationX > 0.01f || m_fUpperTorsoRotationX < -0.01f) {
        float fTimeStepRotation = std::pow(fRotation, CTimer::GetTimeStep());
        m_fAimingRotation      *= fTimeStepRotation;
        m_fUpperTorsoRotationX *= fTimeStepRotation;
    } else {
        m_fAimingRotation = 0.0f;
        m_fUpperTorsoRotationX = 0.0f;
    }

    if (m_nSwimState == SWIM_SPRINT)
        ped->HandleSprintEnergy(false, 0.5f);
    else if (m_nSwimState != SWIM_SPRINTING)
        ped->HandleSprintEnergy(false, 1.0f);
}
