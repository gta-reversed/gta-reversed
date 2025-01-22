/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#include "StdInc.h"

#include "TaskSimpleClimb.h"
#include "TaskSimpleClimb_models.h"

bool& CTaskSimpleClimb::tempMatrix_Initialized = *reinterpret_cast<bool*>(0xC19630);
CMatrix& CTaskSimpleClimb::tempMatrix = *reinterpret_cast<CMatrix*>(0xC195E8);

CColModel& CTaskSimpleClimb::ms_ClimbColModel = *reinterpret_cast<CColModel*>(0xC19518);
CColModel& CTaskSimpleClimb::ms_StandUpColModel = *reinterpret_cast<CColModel*>(0xC19548);
CColModel& CTaskSimpleClimb::ms_VaultColModel = *reinterpret_cast<CColModel*>(0xC19578);
CColModel& CTaskSimpleClimb::ms_FindEdgeColModel = *reinterpret_cast<CColModel*>(0xC195A8);

float CTaskSimpleClimb::ms_fHangingOffsetHorz = -0.40f; // 0x8D2F1C
float CTaskSimpleClimb::ms_fHangingOffsetVert = -1.10f; // 0x8D2F20

float CTaskSimpleClimb::ms_fAtEdgeOffsetHorz  = -0.40f; // 0x8D2F24
float CTaskSimpleClimb::ms_fAtEdgeOffsetVert  = +0.00f; // 0xC18F78

float CTaskSimpleClimb::ms_fStandUpOffsetHorz = +0.15f; // 0x8D2F28
float CTaskSimpleClimb::ms_fStandUpOffsetVert = +1.00f; // 0x8D2F2C

float CTaskSimpleClimb::ms_fVaultOffsetHorz   = +0.50f; // 0x8D2F30
float CTaskSimpleClimb::ms_fVaultOffsetVert   = +0.00f; // 0xC18F7C

float CTaskSimpleClimb::ms_fMinForStretchGrab = +1.40f; // 0x8D2F34

void CTaskSimpleClimb::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleClimb, 0x87059C, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(ScanToGrabSectorList, 0x67DE10);
    RH_ScopedInstall(ScanToGrab, 0x67FD30);
    RH_ScopedInstall(CreateColModel, 0x67A890);
    RH_ScopedInstall(TestForStandUp, 0x680570);
    RH_ScopedInstall(TestForVault, 0x6804D0);
    RH_ScopedInstall(TestForClimb, 0x6803A0);
    RH_ScopedInstall(StartAnim, 0x67DBE0, { .reversed = false });
    RH_ScopedInstall(StartSpeech, 0x67A320);
    RH_ScopedInstall(DeleteAnimCB, 0x67A380);
    RH_ScopedInstall(Shutdown, 0x67A250);
    RH_ScopedInstall(Constructor, 0x67A110);
    RH_ScopedInstall(GetCameraStickModifier, 0x67A5D0);
    RH_ScopedInstall(GetCameraTargetPos, 0x67A390);
    RH_ScopedVMTInstall(ProcessPed, 0x680DC0, { .reversed = false });
    RH_ScopedVMTInstall(MakeAbortable, 0x67A280);
}

CTaskSimpleClimb* CTaskSimpleClimb::Constructor(CEntity* pClimbEnt, const CVector& vecTarget, float fHeading, uint8 nSurfaceType, eClimbHeights nHeight, bool bForceClimb) {
    this->CTaskSimpleClimb::CTaskSimpleClimb(pClimbEnt, vecTarget, fHeading, nSurfaceType, nHeight, bForceClimb);
    return this;
}

// 0x67A110
CTaskSimpleClimb::CTaskSimpleClimb(CEntity* climbEntity, const CVector& vecTarget, float fHeading, uint8 nSurfaceType, eClimbHeights nHeight, bool bForceClimb) {
    m_bIsFinished = false;
    m_bChangeAnimation = false;
    m_bChangePosition = false;
    m_bInvalidClimb = false;
    m_pClimbEnt = climbEntity;
    m_pAnim = nullptr;
    m_vecHandholdPos = vecTarget;
    m_fHandholdHeading = fHeading;
    m_nSurfaceType = nSurfaceType;
    m_nHeightForAnim = nHeight;
    m_nHeightForPos = CLIMB_NOT_READY;
    m_nGetToPosCounter = 0;
    m_bForceClimb = bForceClimb;
    m_nFallAfterVault = -1;
    CEntity::SafeRegisterRef(m_pClimbEnt);
}

// 0x67A1D0
CTaskSimpleClimb::~CTaskSimpleClimb() {
    CEntity::SafeCleanUpRef(m_pClimbEnt);

    if (m_pAnim) {
        m_pAnim->SetDeleteCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
        m_pAnim = nullptr;
    }
}

// 0x680DC0
bool CTaskSimpleClimb::ProcessPed(CPed* ped) {
    if (m_bIsFinished) {
        if (ped->m_pEntityIgnoredCollision == m_pClimbEnt)
            ped->m_pEntityIgnoredCollision = nullptr;
        return true;
    }

    if (!m_pClimbEnt || m_pClimbEnt->IsObject() && !m_pClimbEnt->IsStatic() && !m_pClimbEnt->AsPhysical()->physicalFlags.bDisableCollisionForce ||
        m_pClimbEnt->IsVehicle() && m_pClimbEnt->AsVehicle()->IsSubTrain() && m_pClimbEnt->AsVehicle()->m_vecMoveSpeed.Magnitude() > 0.1F) {
        MakeAbortable(ped);
        return true;
    }

    if (m_pClimbEnt->IsVehicle())
        ped->m_pEntityIgnoredCollision = m_pClimbEnt;

    if (!m_pAnim || m_bChangeAnimation) {
        StartAnim(ped);
        StartSpeech(ped);
    }

    float fAngle = m_fHandholdHeading;
    CVector posn = m_vecHandholdPos;

    if (m_pClimbEnt->IsPhysical()) {
        posn = m_pClimbEnt->GetMatrix().TransformPoint(posn);
        fAngle += m_pClimbEnt->GetHeading();
    }

    bool bNewHeightForPos = false;
    if (m_bChangePosition && m_pAnim && m_pAnim->m_BlendAmount == 1.0F) {
        bNewHeightForPos = true;
        m_nHeightForPos = m_nHeightForAnim;
        m_bChangePosition = false;
    }

    if (m_nHeightForPos > 0) {
        auto [offsetHorz, offsetVert] = [&]{
            switch (m_nHeightForPos) {
            case CLIMB_GRAB:
            case CLIMB_PULLUP:
                return std::make_pair(ms_fHangingOffsetHorz, ms_fHangingOffsetVert);
            case CLIMB_STANDUP:
            case CLIMB_VAULT:
                return std::make_pair(ms_fAtEdgeOffsetHorz, ms_fAtEdgeOffsetVert);
            case CLIMB_FINISHED:
                return std::make_pair(ms_fStandUpOffsetHorz, ms_fStandUpOffsetVert);
            case CLIMB_FINISHED_V:
                return std::make_pair(ms_fVaultOffsetHorz, ms_fVaultOffsetVert);
            default:
                NOTSA_UNREACHABLE();
            }
        }();

        CVector targetPoint = posn + CVector(-std::sin(fAngle) * offsetHorz, std::cos(fAngle) * offsetHorz, offsetVert);
        CVector vecClimbEntSpeed{};
        CVector relPosn = targetPoint - ped->GetPosition();

        if (!m_pClimbEnt->IsStatic() && m_pClimbEnt->IsPhysical())
            vecClimbEntSpeed = m_pClimbEnt->AsPhysical()->GetSpeed(targetPoint - m_pClimbEnt->GetPosition());

        if (bNewHeightForPos) {
            ped->SetPosn(targetPoint);
            ped->m_vecMoveSpeed = vecClimbEntSpeed;
        } else {
            if (relPosn.SquaredMagnitude() >= 0.1F) {
                ped->bIsStanding = false;
                relPosn *= 0.25f;
                ped->m_vecMoveSpeed = relPosn / CTimer::GetTimeStep();

                if (ped->m_vecMoveSpeed.Magnitude() > 0.2F)
                    ped->m_vecMoveSpeed *= 0.2f / ped->m_vecMoveSpeed.Magnitude();

                ped->m_vecMoveSpeed += vecClimbEntSpeed;

                if (!(m_pAnim->m_Flags & ANIMATION_IS_PLAYING) || m_pAnim->m_AnimId == ANIM_ID_CLIMB_IDLE) {
                    m_nGetToPosCounter += (uint16)CTimer::GetTimeStepInMS();
                    if (m_nGetToPosCounter > 1000 || m_pAnim->m_AnimId == ANIM_ID_CLIMB_IDLE && m_nGetToPosCounter > 500) {
                        m_bInvalidClimb = true;
                        MakeAbortable(ped);
                        ped->ApplyMoveForce(ped->GetForward() * (ped->m_fMass * -0.1F));
                    }
                }
            } else {
                ped->bIsStanding = false;
                ped->m_vecMoveSpeed = relPosn / CTimer::GetTimeStep();
                ped->m_vecMoveSpeed += vecClimbEntSpeed;

                if (!(m_pAnim->m_Flags & ANIMATION_IS_PLAYING) && m_nHeightForAnim == CLIMB_STANDUP) {
                    if (TestForVault(ped, &posn, fAngle)) {
                        m_nHeightForAnim = CLIMB_VAULT;
                        m_bChangeAnimation = true;
                    } else
                        m_pAnim->m_Flags |= ANIMATION_IS_PLAYING;
                }
            }
        }
    }

    if (bNewHeightForPos) {
        if (m_nHeightForPos == CLIMB_FINISHED || m_nHeightForPos == CLIMB_FINISHED_V) {
            m_bIsFinished = true;
            ped->bIsInTheAir = false;
            ped->bIsLanding = false;

            if (m_nHeightForPos == CLIMB_FINISHED_V) {
                ped->m_vecMoveSpeed = ped->GetForward() / 50.0f + CVector(0.0f, 0.0f, -0.05f);
                ped->bIsStanding = false;
                ped->bWasStanding = true;
            } else {
                ped->m_vecMoveSpeed = ped->GetForward() * 0.05f + CVector(0.0f, 0.0f, -0.01f);
                ped->bIsStanding = true;
                ped->bWasStanding = true;
            }

            if (ped->IsPlayer()) {
                CVector empty{};
                CEventSoundQuiet event(ped, 50.0f, -1, empty);
                GetEventGlobalGroup()->Add(&event, false);
            }

            if (m_pAnim)
                m_pAnim->m_BlendDelta = -8.0f;

            if (ped->m_pEntityIgnoredCollision == m_pClimbEnt)
                ped->m_pEntityIgnoredCollision = nullptr;

            return true;
        }
    }

    CPad* pad = ped->IsPlayer() ? ped->AsPlayer()->GetPadFromPlayer() : nullptr;

    if (m_pAnim && m_pAnim->m_BlendAmount == 1.0F) {
        switch (m_pAnim->m_AnimId) {
        case ANIM_ID_CLIMB_JUMP:
            if (m_pAnim->m_BlendHier->m_fTotalTime <= m_pAnim->m_TimeStep + m_pAnim->m_CurrentTime) {
                if (m_bForceClimb || pad && pad->GetJump()) {
                    m_nHeightForAnim++;
                    m_bChangeAnimation = true;
                }

                StartAnim(ped);
            }
            break;
        case ANIM_ID_CLIMB_IDLE:
            if (m_bForceClimb || !pad || pad->GetJump()) {
                m_nHeightForAnim++;
                m_bChangeAnimation = true;
            }
            break;
        case ANIM_ID_CLIMB_PULL:
        case ANIM_ID_CLIMB_STAND:
            if (m_pAnim->m_BlendHier->m_fTotalTime == m_pAnim->m_CurrentTime) {
                m_nHeightForAnim++;
                m_bChangeAnimation = true;
            }
            break;
        case ANIM_ID_CLIMB_JUMP_B:
            if (m_pAnim->m_BlendHier->m_fTotalTime <= m_pAnim->m_TimeStep + m_pAnim->m_CurrentTime) {
                m_nHeightForAnim++;
                m_bChangeAnimation = true;
            }
            break;
        default:
            if (m_pAnim->m_BlendHier->m_fTotalTime == m_pAnim->m_CurrentTime)
                if (m_bForceClimb || pad && pad->JumpJustDown()) {
                    m_nHeightForAnim++;
                    m_bChangeAnimation = true;
                }
            break;
        }
    }

    if (pad && pad->GetExitVehicle()) {
        MakeAbortable(ped);
    } else if (m_nHeightForPos != CLIMB_STANDUP && m_nHeightForPos != CLIMB_VAULT || !m_pAnim || !(m_pAnim->m_Flags & ANIMATION_IS_PLAYING)) {
        if (m_nHeightForAnim == CLIMB_STANDUP && m_nHeightForPos < CLIMB_STANDUP && TestForVault(ped, &posn, fAngle)) {
            m_nHeightForAnim = CLIMB_VAULT;
        }
    } else if (!TestForStandUp(ped, &posn, fAngle)) {
        MakeAbortable(ped);
    }

    ped->m_fAimingRotation = fAngle;
    ped->m_fCurrentRotation = fAngle;
    ped->SetOrientation(0.0f, 0.0f, fAngle);

    return false;
}

// 0x67A280
bool CTaskSimpleClimb::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    if (event && event->GetEventPriority() < 71 && ped->m_fHealth > 0.0F)
        return false;

    if (m_pAnim) {
        m_pAnim->m_BlendDelta = -4.0f;
        m_pAnim->SetDeleteCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
        m_pAnim = nullptr;
    }

    ped->m_vecMoveSpeed.Set(0.0f, 0.0f, -0.05f);
    m_bIsFinished = true;
    return true;
}

// 0x6803A0
CEntity* CTaskSimpleClimb::TestForClimb(CPed* ped, CVector& outClimbPos, float& outClimbHeading, uint8& outSurfaceType, bool bLaunch) {
    auto entity = (CEntity*)ScanToGrab(ped, outClimbPos, outClimbHeading, outSurfaceType, bLaunch, false, false, nullptr);

    if (entity) {
        CVector point = outClimbPos;
        float angle = outClimbHeading;

        if (entity->IsPhysical()) {
            point = entity->GetMatrix().TransformPoint(point);
            angle += entity->GetHeading();
        }

        point += CVector(-ms_fAtEdgeOffsetHorz * std::sin(angle), ms_fAtEdgeOffsetHorz * std::cos(angle), ms_fAtEdgeOffsetVert);

        CVector v;
        uint8 surfaceType2;
        if (!ScanToGrab(ped, v, angle, surfaceType2, false, true, false, &point)) {
            return entity;
        }
        outClimbHeading = -9999.9f;
    }

    return nullptr;
}

//returns bool or CEntity*
// 0x67DE10
void* CTaskSimpleClimb::ScanToGrabSectorList(CPtrList* sectorList, CPed* ped, CVector& targetPos, float& fAngle, uint8& nSurfaceType, bool flag1, bool bStandUp, bool bVault) {
    if (!tempMatrix_Initialized) {
        tempMatrix_Initialized = true;
        tempMatrix = CMatrix();
    }

    CColModel* cm = &ms_ClimbColModel;
    if (bStandUp)
        cm = &ms_StandUpColModel;
    if (bVault)
        cm = &ms_VaultColModel;

    CEntity* collidedEntity = nullptr;

    for (auto node = sectorList->GetNode(); node; node = node->m_next) {
        auto* entity = reinterpret_cast<CEntity*>(node->m_item);

        if (entity->IsScanCodeCurrent())
            continue;

        entity->SetCurrentScanCode();

        if (!entity->m_bUsesCollision)
            continue;

        if (entity->IsBuilding()
            || (
                entity->IsObject()
                && (entity->IsStatic() || entity->AsObject()->physicalFlags.bDisableCollisionForce)
                && !entity->AsObject()->physicalFlags.bInfiniteMass
                )
            || (
                entity->IsVehicle()
                && (bStandUp || bVault || (ped->GetIntelligence()->GetTaskSwim() && entity->AsVehicle()->IsSubBoat()) || (entity->AsVehicle()->IsSubTrain() && entity->AsVehicle()->m_vecMoveSpeed.Magnitude2D() < 0.1F))
                )
            )
        {
            if (DistanceBetweenPoints(entity->GetBoundCentre(), ped->GetMatrix().TransformPoint(cm->GetBoundCenter())) >= entity->GetModelInfo()->GetColModel()->GetBoundRadius() + cm->GetBoundRadius())
                continue;

            int32 numSpheres = -1;
            if (entity->IsVehicle() && entity->AsVehicle()->IsSubBoat()) {
                numSpheres = entity->GetColModel()->m_pColData->m_nNumSpheres;
                entity->GetColModel()->m_pColData->m_nNumSpheres = 0;
            }

            if (CCollision::ProcessColModels(*ped->m_matrix, *cm, entity->GetMatrix(), *entity->GetColModel(), CWorld::m_aTempColPts, nullptr, nullptr, false) > 0) {
                if (bStandUp || bVault) {
                    if (entity->IsVehicle() && numSpheres > -1)
                        entity->GetColModel()->m_pColData->m_nNumSpheres = numSpheres;

                    return entity;
                }
                uint8 nColSphereIndex = CWorld::m_aTempColPts[0].m_nPieceTypeA;

                if (nColSphereIndex == 0 ||
                    nColSphereIndex == 1 ||
                    nColSphereIndex == 2 ||
                    nColSphereIndex == 3 ||
                    nColSphereIndex == 4 ||
                    nColSphereIndex == 10
                ) {
                    if (entity->IsVehicle() && numSpheres > -1)
                        entity->GetColModel()->m_pColData->m_nNumSpheres = numSpheres;
                    return (void*)1;
                }

                auto relPosn = CWorld::m_aTempColPts[0].m_vecPoint - targetPos;
                if (nColSphereIndex == 16 || CWorld::m_aTempColPts[0].m_vecPoint.z <= targetPos.z && DotProduct(relPosn, ped->GetForward()) >= 0.0f ||
                    !g_surfaceInfos.CanClimb(CWorld::m_aTempColPts[0].m_nSurfaceTypeB)) {
                    if (entity->IsVehicle() && numSpheres > -1)
                        entity->GetColModel()->m_pColData->m_nNumSpheres = numSpheres;
                    continue;
                }

                int32 fatModifier = ped->m_pPlayerData && !ped->GetIntelligence()->GetTaskSwim() ? (int32)CStats::GetFatAndMuscleModifier(STAT_MOD_0) : 0;

                if (!flag1 || fatModifier >= 1) {
                    if (nColSphereIndex == 5 || nColSphereIndex == 11 || nColSphereIndex == 17) {
                        if (entity->IsVehicle() && numSpheres > -1)
                            entity->GetColModel()->m_pColData->m_nNumSpheres = numSpheres;
                        continue;
                    }

                    if (fatModifier >= 1 && (!flag1 || fatModifier >= 2)) {
                        if (nColSphereIndex == 6 || nColSphereIndex == 12 || nColSphereIndex == 18) {
                            if (entity->IsVehicle() && numSpheres > -1)
                                entity->GetColModel()->m_pColData->m_nNumSpheres = numSpheres;
                            continue;
                        }
                    }
                }

                if (fabsf(CWorld::m_aTempColPts[0].m_vecNormal.x) <= 0.05f && fabsf(CWorld::m_aTempColPts[0].m_vecNormal.y) <= 0.05F) {
                    targetPos = CWorld::m_aTempColPts[0].m_vecPoint;
                    fAngle = ped->m_fCurrentRotation;
                    nSurfaceType = CWorld::m_aTempColPts[0].m_nSurfaceTypeB;
                    collidedEntity = entity;
                } else {
                    CVector vecNormal = CWorld::m_aTempColPts[0].m_vecNormal;

                    if (DotProduct2D(vecNormal, CWorld::m_aTempColPts[0].m_vecPoint - ped->GetPosition()) < 0.0F)
                        vecNormal = -vecNormal;

                    if (DotProduct(vecNormal, ped->GetForward()) > 0.3F) {
                        targetPos = CWorld::m_aTempColPts[0].m_vecPoint;
                        fAngle = std::atan2f(-vecNormal.x, vecNormal.y);
                        nSurfaceType = CWorld::m_aTempColPts[0].m_nSurfaceTypeB;
                        collidedEntity = entity;
                    }
                }

                tempMatrix = *ped->m_matrix;
                tempMatrix.SetTranslateOnly(CWorld::m_aTempColPts[0].m_vecPoint);

                if (CCollision::ProcessColModels(tempMatrix, ms_FindEdgeColModel, entity->GetMatrix(), *entity->GetColModel(), CWorld::m_aTempColPts, nullptr, nullptr, false) > 0) {
                    if (std::fabsf(CWorld::m_aTempColPts[0].m_vecNormal.x) <= 0.05f && std::fabsf(CWorld::m_aTempColPts[0].m_vecNormal.y) <= 0.05F) {
                        targetPos = CWorld::m_aTempColPts[0].m_vecPoint;
                        nSurfaceType = CWorld::m_aTempColPts[0].m_nSurfaceTypeB;
                    } else {
                        CVector vecNormal = CWorld::m_aTempColPts[0].m_vecNormal;

                        if (DotProduct2D(vecNormal, CWorld::m_aTempColPts[0].m_vecPoint - ped->GetPosition()) < 0.0F) {
                            vecNormal = -vecNormal;
                        }

                        if (DotProduct(vecNormal, ped->GetForward()) > 0.3F) {
                            targetPos = CWorld::m_aTempColPts[0].m_vecPoint;
                            fAngle = std::atan2f(-vecNormal.x, vecNormal.y);
                            nSurfaceType = CWorld::m_aTempColPts[0].m_nSurfaceTypeB;
                            collidedEntity = entity;
                        }
                    }
                }

                if (entity->IsVehicle() && numSpheres > -1) {
                    entity->GetColModel()->m_pColData->m_nNumSpheres = numSpheres;
                }
            } else if (entity->IsVehicle() && numSpheres > -1) {
                entity->GetColModel()->m_pColData->m_nNumSpheres = numSpheres;
            }
        }
    }

    return collidedEntity;
}

// 0x67FD30
CEntity* CTaskSimpleClimb::ScanToGrab(CPed* ped, CVector& climbPos, float& fAngle, uint8& pSurfaceType, bool flag1, bool bStandUp, bool bVault, CVector* pedPosition) {
    if (!ms_ClimbColModel.m_pColData) {
        CreateColModel();
    }

    CEntity* collidedEntity = nullptr;

    CVector originalPedPosition = ped->GetPosition();

    if (pedPosition)
        ped->SetPosn(*pedPosition);

    climbPos = ped->GetPosition() + ped->GetForward() * 10.0f;

    auto outPoint = ped->m_matrix->TransformPoint(ms_ClimbColModel.GetBoundCenter());

    int32 startSectorX = CWorld::GetSectorX(outPoint.x - ms_ClimbColModel.GetBoundRadius());
    int32 startSectorY = CWorld::GetSectorY(outPoint.y - ms_ClimbColModel.GetBoundRadius());
    int32 endSectorX   = CWorld::GetSectorX(outPoint.x + ms_ClimbColModel.GetBoundRadius());
    int32 endSectorY   = CWorld::GetSectorY(outPoint.y + ms_ClimbColModel.GetBoundRadius());

    CWorld::IncrementCurrentScanCode();

    for (int32 y = startSectorY; y <= endSectorY; y++) {
        for (int32 x = startSectorX; x <= endSectorX; x++) {
            const auto ScanToGrab_ = [&](CPtrList& ptrList) -> CEntity* {
                return static_cast<CEntity*>(ScanToGrabSectorList(&ptrList, ped, climbPos, fAngle, pSurfaceType, flag1, bStandUp, bVault));
            };
            auto scanResult1 = ScanToGrab_(GetSector(x, y)->m_buildings);
            auto scanResult2 = ScanToGrab_(GetRepeatSector(x, y)->GetList(REPEATSECTOR_OBJECTS));
            if (!scanResult2)
                scanResult2 = ScanToGrab_(GetRepeatSector(x, y)->GetList(REPEATSECTOR_VEHICLES));

            if ((int32)(scanResult1) == 1 || (int32)(scanResult2) == 1)
                return nullptr;

            if (auto entity = scanResult2 ? scanResult2 : scanResult1) {
                if (bStandUp || bVault) {
                    if (pedPosition)
                        ped->SetPosn(originalPedPosition);

                    return entity;
                } else {
                    collidedEntity = entity;
                }
            }
        }
    }

    if (pedPosition)
        ped->SetPosn(originalPedPosition);

    if (collidedEntity) {
        if (collidedEntity->IsPhysical()) {
            climbPos = Invert(collidedEntity->GetMatrix()).TransformPoint(climbPos);
            fAngle -= collidedEntity->GetHeading();
        }
    }

    return collidedEntity;
}

// 0x67A890
bool CTaskSimpleClimb::CreateColModel() {
    ms_ClimbColModel.AllocateData((int32)std::size(ClimbColModelSpheres), 0, 0, 0, 0, false);
    ms_ClimbColModel.m_pColData->SetSpheres(ClimbColModelSpheres);
    ms_ClimbColModel.m_boundSphere.Set(2.02f, { 0.0f, 0.75f, 1.65f });
    ms_ClimbColModel.GetBoundingBox().Set({ -0.4f, -0.46f, 0.0f }, { 0.4f, 1.9f, 3.3f });
    ms_ClimbColModel.m_nColSlot = 0;

    ms_StandUpColModel.AllocateData((int32)std::size(StandUpColModelSpheres), 0, 0, 0, 0, false);
    ms_StandUpColModel.m_pColData->SetSpheres(StandUpColModelSpheres);
    ms_StandUpColModel.m_boundSphere.Set(1.5f, {0.0f, 0.3f, 0.7f });
    ms_StandUpColModel.GetBoundingBox().Set({ -0.35f, -0.35f, -0.65f }, { 0.35f, 0.95f, 2.1f });
    ms_StandUpColModel.m_nColSlot = 0;

    ms_VaultColModel.AllocateData((int32)std::size(VaultColModelSpheres), 0, 0, 0, 0, false);
    ms_VaultColModel.m_pColData->SetSpheres(VaultColModelSpheres);
    ms_VaultColModel.m_boundSphere.Set(1.15f, {0.0f, 0.5f, 0.1f });
    ms_VaultColModel.GetBoundingBox().Set({-0.35f, -0.35f, -0.6f }, { 0.35f, 1.3f, 0.85f });
    ms_VaultColModel.m_nColSlot = 0;

    ms_FindEdgeColModel.AllocateData((int32)std::size(FindEdgeColModelSpheres), 0, 0, 0, 0, false);
    ms_FindEdgeColModel.m_pColData->SetSpheres(FindEdgeColModelSpheres);
    ms_FindEdgeColModel.m_boundSphere.Set(0.9f, { 0.0f, -0.2f, 0.2f });
    ms_FindEdgeColModel.GetBoundingBox().Set({ -0.3f, -0.8f, -0.4f }, { 0.3f, 0.4f, 0.8f });
    ms_FindEdgeColModel.m_nColSlot = 0;

    return true;
}

// 0x67A250
void CTaskSimpleClimb::Shutdown() {
    ms_ClimbColModel.RemoveCollisionVolumes();
    ms_StandUpColModel.RemoveCollisionVolumes();
    ms_VaultColModel.RemoveCollisionVolumes();
    ms_FindEdgeColModel.RemoveCollisionVolumes();
}

// 0x680570
bool CTaskSimpleClimb::TestForStandUp(CPed* ped, CVector* point, float fAngle) {
    CVector v;
    float angle;
    uint8 nSurfaceType;
    CVector pedPos = *point + CVector(-ms_fAtEdgeOffsetHorz * std::sin(fAngle), ms_fAtEdgeOffsetHorz * std::cos(fAngle), ms_fAtEdgeOffsetVert);
    return !ScanToGrab(ped, v, angle, nSurfaceType, false, true, false, &pedPos);
}

// 0x6804D0
bool CTaskSimpleClimb::TestForVault(CPed* ped, CVector* point, float fAngle) {
    CVector v;
    float angle;
    uint8 nSurfaceType;
    CVector pedPos = *point + CVector(-ms_fAtEdgeOffsetHorz * std::sin(fAngle), ms_fAtEdgeOffsetHorz * std::cos(fAngle), ms_fAtEdgeOffsetVert);
    return !m_pClimbEnt->IsVehicle() && !ScanToGrab(ped, v, angle, nSurfaceType, false, false, true, &pedPos);
}

// 0x67DBE0
void CTaskSimpleClimb::StartAnim(CPed* ped) {
    switch (m_nHeightForAnim) {
    case CLIMB_GRAB:
        if (m_pAnim) {
            m_pAnim->SetDeleteCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
            m_pAnim = CAnimManager::BlendAnimation(ped->m_pRwClump, ANIM_GROUP_DEFAULT, ANIM_ID_CLIMB_IDLE, 4.0f);
        } else {
            m_pAnim = CAnimManager::BlendAnimation(ped->m_pRwClump, ANIM_GROUP_DEFAULT, ANIM_ID_CLIMB_JUMP, 8.0f);
        }
        m_nHeightForPos = CLIMB_GRAB;
        m_bChangePosition = false;
        m_bChangeAnimation = false;
        break;
    case CLIMB_PULLUP:
        if (m_nHeightForPos == CLIMB_NOT_READY) {
            m_nHeightForAnim = CLIMB_STANDUP;
            m_nHeightForPos = CLIMB_STANDUP;
            m_pAnim = CAnimManager::BlendAnimation(ped->m_pRwClump, ANIM_GROUP_DEFAULT, ANIM_ID_CLIMB_STAND, 4.0f);
            m_pAnim->m_Flags &= ~ANIMATION_IS_PLAYING;
        } else {
            m_pAnim->SetDeleteCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
            m_pAnim = CAnimManager::BlendAnimation(ped->m_pRwClump, ANIM_GROUP_DEFAULT, ANIM_ID_CLIMB_PULL, 1000.0f);
            m_nHeightForPos = CLIMB_PULLUP;
        }
        m_bChangePosition = false;
        m_bChangeAnimation = false;
        break;
    case CLIMB_STANDUP:
        if (m_pAnim) {
            m_pAnim->SetDeleteCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
        }
        m_pAnim = CAnimManager::BlendAnimation(ped->m_pRwClump, ANIM_GROUP_DEFAULT, ANIM_ID_CLIMB_STAND, 1000.0f);
        m_bChangePosition = true;
        m_bChangeAnimation = false;
        break;
    case CLIMB_FINISHED:
    case CLIMB_FINISHED_V:
        CAnimManager::BlendAnimation(ped->m_pRwClump, ped->m_nAnimGroup, ANIM_ID_IDLE, 1000.0f);
        ped->SetMoveState(PEDMOVE_STILL);
        ped->SetMoveAnim();
        if (ped->m_pPlayerData) {
            ped->m_pPlayerData->m_fMoveBlendRatio = 0.0f;
        }
        if (m_pAnim) {
            m_pAnim->SetDeleteCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
        }
        m_pAnim = CAnimManager::BlendAnimation(
            ped->m_pRwClump, ANIM_GROUP_DEFAULT,
            m_nHeightForAnim == CLIMB_FINISHED_V ? ANIM_ID_CLIMB_JUMP2FALL : ANIM_ID_CLIMB_STAND_FINISH,
            1000.0F
        );
        m_bChangePosition = true;
        m_bChangeAnimation = false;
        break;
    case CLIMB_VAULT:
        if (m_pAnim) {
            m_pAnim->SetDeleteCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
        }
        m_pAnim = CAnimManager::BlendAnimation(ped->m_pRwClump, ANIM_GROUP_DEFAULT, ANIM_ID_CLIMB_JUMP_B, m_pAnim && m_pAnim->m_AnimId == ANIM_ID_CLIMB_STAND ? 16.0f : 1000.0f);
        m_bChangePosition = true;
        m_bChangeAnimation = false;
        break;
    default:
        break;
    }

    if (m_pAnim) {
        m_pAnim->SetDeleteCallback(DeleteAnimCB, this);
        if (ped->m_pPlayerData && m_pAnim->m_Flags & ANIMATION_IS_PLAYING &&
            (m_pAnim->m_AnimId == ANIM_ID_CLIMB_PULL || m_pAnim->m_AnimId == ANIM_ID_CLIMB_STAND || m_pAnim->m_AnimId == ANIM_ID_CLIMB_JUMP_B)) {
            m_pAnim->m_Speed = CStats::GetFatAndMuscleModifier(STAT_MOD_1);
        }
    }
}

// 0x67A320
void CTaskSimpleClimb::StartSpeech(CPed* ped) {
    if (ped->IsPlayer()) {
        if (m_nHeightForAnim == CLIMB_PULLUP)
            ped->Say(CTX_GLOBAL_PAIN_CJ_STRAIN);
        else if (m_nHeightForAnim == CLIMB_STANDUP)
            ped->Say(CTX_GLOBAL_PAIN_CJ_STRAIN_EXHALE);
    }
}

// 0x67A380
void CTaskSimpleClimb::DeleteAnimCB(CAnimBlendAssociation* anim, void* data) {
    reinterpret_cast<CTaskSimpleClimb*>(data)->m_pAnim = nullptr;
}

// 0x67A5D0
void CTaskSimpleClimb::GetCameraStickModifier(CEntity* entity, float& fVerticalAngle, float& fHorizontalAngle, float& a5, float& a6) {
    if (!m_pAnim)
        return;

    if (m_pAnim->m_AnimId == ANIM_ID_CLIMB_JUMP_B) {
        CVector vec = m_vecHandholdPos;
        float fAngle = m_fHandholdHeading;

        if (m_pClimbEnt->IsPhysical()) {
            vec = m_pClimbEnt->m_matrix->TransformPoint(vec);
            fAngle += m_pClimbEnt->GetHeading();
        }

        vec += CVector(-ms_fVaultOffsetHorz * std::sin(fAngle), ms_fVaultOffsetHorz * std::cos(fAngle), ms_fVaultOffsetVert);

        CColPoint colPoint{};
        CEntity* colEntity{};
        if (CWorld::ProcessVerticalLine(vec, vec.z - 3.0f, colPoint, colEntity, true, true, false, true)) {
            m_nFallAfterVault = (int8)(std::max(vec.z - colPoint.m_vecPoint.z - 1.0f, 0.0F) * 10.0f);
        } else {
            m_nFallAfterVault = 20;
        }

        float v20 = m_nFallAfterVault > 8 ? -0.6f : -0.1f;
        if (fVerticalAngle > v20) {
            a5 = std::max((fVerticalAngle - v20) * -0.05f, -0.05f);
        }
    } else if (m_pAnim->m_AnimId == ANIM_ID_CLIMB_IDLE) {
        float fHeading = m_pClimbEnt->GetHeading() - HALF_PI;
        if (fHeading > fHorizontalAngle + PI)
            fHeading -= TWO_PI;
        else if (fHeading < fHorizontalAngle - PI)
            fHeading += TWO_PI;

        if (fHeading > fHorizontalAngle + HALF_PI + 0.2F)
            a6 = 0.02f;
        else if (fHeading > fHorizontalAngle + HALF_PI && a6 < 0.0F)
            a6 = 0.0f;
        else if (fHeading < fHorizontalAngle - HALF_PI - 0.2F)
            a6 = -0.02f;
        else if (fHeading < fHorizontalAngle - HALF_PI && a6 > 0.0F)
            a6 = 0.0f;
    }
}

// 0x67A390
void CTaskSimpleClimb::GetCameraTargetPos(CPed* ped, CVector& vecTarget) {
    vecTarget = ped->GetPosition();

    if (!m_pAnim)
        return;

    float fProgress = m_pAnim->m_CurrentTime / m_pAnim->m_BlendHier->m_fTotalTime;
    float offsetHorz, offsetVert;

    switch (m_pAnim->m_AnimId) {
    case ANIM_ID_FALL_GLIDE:
    case ANIM_ID_CLIMB_JUMP2FALL:
        offsetHorz = ms_fVaultOffsetHorz;
        offsetVert = ms_fVaultOffsetVert;
        break;
    case ANIM_ID_CLIMB_PULL:
        offsetHorz = lerp(ms_fHangingOffsetHorz, ms_fAtEdgeOffsetHorz, fProgress);
        offsetVert = lerp(ms_fHangingOffsetVert, ms_fAtEdgeOffsetVert, fProgress);
        break;
    case ANIM_ID_CLIMB_STAND:
        offsetHorz = lerp(ms_fAtEdgeOffsetHorz, ms_fStandUpOffsetHorz, fProgress);
        offsetVert = lerp(ms_fAtEdgeOffsetVert, ms_fStandUpOffsetVert, fProgress);
        break;
    case ANIM_ID_CLIMB_STAND_FINISH:
        offsetHorz = ms_fStandUpOffsetHorz;
        offsetVert = ms_fStandUpOffsetVert;
        break;
    case ANIM_ID_CLIMB_JUMP_B:
        offsetHorz = lerp(ms_fAtEdgeOffsetHorz, ms_fVaultOffsetHorz, fProgress);
        offsetVert = lerp(ms_fAtEdgeOffsetVert, ms_fVaultOffsetVert, fProgress);
        break;
    default:
        return;
    }

    CVector point = m_vecHandholdPos;
    float fAngle = m_fHandholdHeading;
    if (m_pClimbEnt->IsPhysical()) {
        point = m_pClimbEnt->GetMatrix().TransformPoint(point);
        fAngle += m_pClimbEnt->GetHeading();
    }

    vecTarget = point + CVector(-offsetHorz * std::sin(fAngle), offsetHorz * cos(fAngle), offsetVert);
}
