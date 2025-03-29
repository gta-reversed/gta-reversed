/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#include "StdInc.h"

#include "TaskSimpleClimb.h"
#include "TaskSimpleClimb_models.h"


CColModel& ms_ClimbColModel    = StaticRef<CColModel>(0xC19518);
CColModel& ms_StandUpColModel  = StaticRef<CColModel>(0xC19548);
CColModel& ms_VaultColModel    = StaticRef<CColModel>(0xC19578);
CColModel& ms_FindEdgeColModel = StaticRef<CColModel>(0xC195A8);

float ms_fHangingOffsetHorz = -0.40f; // 0x8D2F1C
float ms_fHangingOffsetVert = -1.10f; // 0x8D2F20

float ms_fAtEdgeOffsetHorz = -0.40f; // 0x8D2F24
float ms_fAtEdgeOffsetVert = +0.00f; // 0xC18F78

float ms_fStandUpOffsetHorz = +0.15f; // 0x8D2F28
float ms_fStandUpOffsetVert = +1.00f; // 0x8D2F2C

float ms_fVaultOffsetHorz = +0.50f; // 0x8D2F30
float ms_fVaultOffsetVert = +0.00f; // 0xC18F7C

void CTaskSimpleClimb::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleClimb, 0x87059C, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(ScanToGrabSectorList<CPtrListSingleLink>, 0x67DE10);
    RH_ScopedInstall(ScanToGrab, 0x67FD30);
    RH_ScopedInstall(CreateColModel, 0x67A890);
    RH_ScopedInstall(TestForStandUp, 0x680570);
    RH_ScopedInstall(TestForVault, 0x6804D0);
    RH_ScopedInstall(TestForClimb, 0x6803A0);
    RH_ScopedInstall(StartAnim, 0x67DBE0);
    RH_ScopedInstall(StartSpeech, 0x67A320);
    RH_ScopedInstall(DeleteAnimCB, 0x67A380);
    RH_ScopedInstall(Shutdown, 0x67A250);
    RH_ScopedInstall(Constructor, 0x67A110);
    RH_ScopedInstall(GetCameraStickModifier, 0x67A5D0);
    RH_ScopedInstall(GetCameraTargetPos, 0x67A390);
    RH_ScopedVMTInstall(ProcessPed, 0x680DC0);
    RH_ScopedVMTInstall(MakeAbortable, 0x67A280);
}

// 0x67A110
CTaskSimpleClimb::CTaskSimpleClimb(CEntity* climbEntity, const CVector& vecTarget, float fHeading, eSurfaceType nSurfaceType, eClimbHeights nHeight, bool bForceClimb) :
    m_ClimbEntity{ climbEntity },
    m_HandholdPos{ vecTarget },
    m_HandholdHeading{ fHeading },
    m_SurfaceType{ nSurfaceType },
    m_HeightForAnim{ nHeight },
    m_HasToForceClimb{ bForceClimb }
{ }

// 0x67A1D0
CTaskSimpleClimb::~CTaskSimpleClimb() {
    if (const auto a = std::exchange(m_Anim, nullptr)) {
        a->SetDefaultDeleteCallback();
    }
}

// 0x680DC0
bool CTaskSimpleClimb::ProcessPed(CPed* ped) {
    if (m_HasFinished) {
        if (ped->m_pEntityIgnoredCollision == m_ClimbEntity) {
            ped->m_pEntityIgnoredCollision = nullptr;
        }
        return true;
    }

    if (!m_ClimbEntity || m_ClimbEntity->IsObject() && !m_ClimbEntity->IsStatic() && !m_ClimbEntity->AsPhysical()->physicalFlags.bDisableCollisionForce || m_ClimbEntity->IsVehicle() && m_ClimbEntity->AsVehicle()->IsSubTrain() && m_ClimbEntity->AsVehicle()->m_vecMoveSpeed.Magnitude() > 0.1F) {
        MakeAbortable(ped);
        return true;
    }

    if (m_ClimbEntity->IsVehicle()) {
        ped->m_pEntityIgnoredCollision = m_ClimbEntity;
    }

    if (!m_Anim || m_HasToChangeAnimation) {
        StartAnim(ped);
        StartSpeech(ped);
    }

    const auto [handPos, handAngle] = GetHandholdPosAndAngleForEntity(m_ClimbEntity, m_HandholdPos, m_HandholdHeading);

    const bool bNewHeightForPos = m_HasToChangePosition && m_Anim && m_Anim->m_BlendAmount == 1.0f;
    if (bNewHeightForPos) {
        m_HeightForPos   = m_HeightForAnim;
        m_HasToChangePosition = false;
    }

    if (m_HeightForPos > CLIMB_NOT_READY) {
        const auto offset = [&]() -> CVector2D {
            switch (m_HeightForPos) {
            case CLIMB_GRAB:
            case CLIMB_PULLUP: return { ms_fHangingOffsetHorz, ms_fHangingOffsetVert };
            case CLIMB_STANDUP:
            case CLIMB_VAULT: return { ms_fAtEdgeOffsetHorz, ms_fAtEdgeOffsetVert };
            case CLIMB_FINISHED: return { ms_fStandUpOffsetHorz, ms_fStandUpOffsetVert };
            case CLIMB_FINISHED_V: return { ms_fVaultOffsetHorz, ms_fVaultOffsetVert };
            default:
                NOTSA_UNREACHABLE();
            }
        }();
        const CVector targetPt = handPos + GetClimbOffset3D(offset, handAngle);

        CVector vecClimbEntSpeed{};
        CVector delta = targetPt - ped->GetPosition();
        if (!m_ClimbEntity->IsStatic() && m_ClimbEntity->IsPhysical()) {
            vecClimbEntSpeed = m_ClimbEntity->AsPhysical()->GetSpeed(targetPt - m_ClimbEntity->GetPosition());
        }

        if (bNewHeightForPos) {
            ped->SetPosn(targetPt);
            ped->m_vecMoveSpeed = vecClimbEntSpeed;
        } else if (delta.SquaredMagnitude() >= 0.1F) {
            ped->bIsStanding = false;
            delta *= 0.25f;
            ped->m_vecMoveSpeed = delta / CTimer::GetTimeStep();

            if (ped->m_vecMoveSpeed.Magnitude() > 0.2F) {
                ped->m_vecMoveSpeed *= 0.2f / ped->m_vecMoveSpeed.Magnitude();
            }

            ped->m_vecMoveSpeed += vecClimbEntSpeed;

            if (!(m_Anim->m_Flags & ANIMATION_IS_PLAYING) || m_Anim->m_AnimId == ANIM_ID_CLIMB_IDLE) {
                m_GetToPosCounter += (uint16)CTimer::GetTimeStepInMS();
                if (m_GetToPosCounter > 1'000 || m_Anim->m_AnimId == ANIM_ID_CLIMB_IDLE && m_GetToPosCounter > 500) {
                    m_IsInvalidClimb = true;
                    MakeAbortable(ped);
                    ped->ApplyMoveForce(ped->GetForward() * (ped->m_fMass * -0.1F));
                }
            }
        } else {
            ped->bIsStanding    = false;
            ped->m_vecMoveSpeed = delta / CTimer::GetTimeStep();
            ped->m_vecMoveSpeed += vecClimbEntSpeed;

            if (!(m_Anim->m_Flags & ANIMATION_IS_PLAYING) && m_HeightForAnim == CLIMB_STANDUP) {
                if (TestForVault(ped, handPos, handAngle)) {
                    m_HeightForAnim   = CLIMB_VAULT;
                    m_HasToChangeAnimation = true;
                } else {
                    m_Anim->m_Flags |= ANIMATION_IS_PLAYING;
                }
            }
        }
    }

    if (bNewHeightForPos) {
        if (notsa::contains({ CLIMB_FINISHED, CLIMB_FINISHED_V }, m_HeightForPos)) {
            m_HasFinished    = true;
            ped->bIsInTheAir = false;
            ped->bIsLanding  = false;

            if (m_HeightForPos == CLIMB_FINISHED_V) {
                ped->m_vecMoveSpeed = ped->GetForward() / 50.0f + CVector(0.0f, 0.0f, -0.05f);
                ped->bIsStanding    = false;
                ped->bWasStanding   = true;
            } else {
                ped->m_vecMoveSpeed = ped->GetForward() * 0.05f + CVector(0.0f, 0.0f, -0.01f);
                ped->bIsStanding    = true;
                ped->bWasStanding   = true;
            }

            if (ped->IsPlayer()) {
                GetEventGlobalGroup()->Add(CEventSoundQuiet{ ped, 50.0f, (uint32)(-1), CVector{} });
            }

            if (m_Anim) {
                m_Anim->SetBlendDelta(-8.0f);
            }

            if (ped->m_pEntityIgnoredCollision == m_ClimbEntity) {
                ped->m_pEntityIgnoredCollision = nullptr;
            }

            return true;
        }
    }

    const CPad* const pad = ped->IsPlayer()
        ? ped->AsPlayer()->GetPadFromPlayer()
        : nullptr;

    if (m_Anim && m_Anim->GetBlendAmount() == 1.0f) {
        const auto DoAdvanceAnimHeight = [&]() {
            m_HeightForAnim   = static_cast<eClimbHeights>(m_HeightForAnim + 1);
            m_HasToChangeAnimation = true;
        };
        switch (m_Anim->m_AnimId) {
        case ANIM_ID_CLIMB_JUMP:
            if (m_Anim->m_BlendHier->m_fTotalTime <= m_Anim->m_TimeStep + m_Anim->m_CurrentTime) {
                if (m_HasToForceClimb || pad && pad->GetJump()) {
                    DoAdvanceAnimHeight();
                }
                StartAnim(ped);
            }
            break;
        case ANIM_ID_CLIMB_IDLE:
            if (m_HasToForceClimb || !pad || pad->GetJump()) {
                DoAdvanceAnimHeight();
            }
            break;
        case ANIM_ID_CLIMB_PULL:
        case ANIM_ID_CLIMB_STAND:
            if (m_Anim->m_BlendHier->m_fTotalTime == m_Anim->m_CurrentTime) {
                DoAdvanceAnimHeight();
            }
            break;
        case ANIM_ID_CLIMB_JUMP_B:
            if (m_Anim->m_BlendHier->m_fTotalTime <= m_Anim->m_TimeStep + m_Anim->m_CurrentTime) {
                DoAdvanceAnimHeight();
            }
            break;
        default:
            if (m_Anim->m_BlendHier->m_fTotalTime == m_Anim->m_CurrentTime) {
                if (m_HasToForceClimb || pad && pad->JumpJustDown()) {
                    DoAdvanceAnimHeight();
                }
            }
            break;
        }
    }

    if (pad && pad->GetExitVehicle()) {
        MakeAbortable(ped);
    } else if (m_HeightForPos != CLIMB_STANDUP && m_HeightForPos != CLIMB_VAULT || !m_Anim || !(m_Anim->m_Flags & ANIMATION_IS_PLAYING)) {
        if (m_HeightForAnim == CLIMB_STANDUP && m_HeightForPos < CLIMB_STANDUP && TestForVault(ped, handPos, handAngle)) {
            m_HeightForAnim = CLIMB_VAULT;
        }
    } else if (!TestForStandUp(ped, handPos, handAngle)) {
        MakeAbortable(ped);
    }

    ped->m_fAimingRotation = ped->m_fCurrentRotation = handAngle;
    ped->SetOrientation(0.0f, 0.0f, handAngle);

    return false;
}

// 0x67A280
bool CTaskSimpleClimb::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    if (event && event->GetEventPriority() < 71 && ped->m_fHealth > 0.0F) {
        return false;
    }

    if (m_Anim) {
        m_Anim->m_BlendDelta = -4.0f;
        m_Anim->SetDeleteCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
        m_Anim = nullptr;
    }

    ped->m_vecMoveSpeed.Set(0.0f, 0.0f, -0.05f);
    m_HasFinished = true;
    return true;
}

// 0x6803A0
CEntity* CTaskSimpleClimb::TestForClimb(CPed* ped, CVector& outClimbPos, float& outClimbHeading, eSurfaceType& outSurfaceType, bool bLaunch) {
    if (const auto entity = ScanToGrab(ped, outClimbPos, outClimbHeading, outSurfaceType, bLaunch, false, false, nullptr)) {
        auto [pt, angle] = GetHandholdPosAndAngleForEntity(entity, outClimbPos, outClimbHeading);
        pt += GetClimbOffset3D({ ms_fHangingOffsetHorz, ms_fHangingOffsetVert }, angle);

        CVector      grabPos;
        eSurfaceType grabSurface;
        if (!ScanToGrab(ped, grabPos, angle, grabSurface, false, true, false, &pt)) {
            return entity;
        }
        outClimbHeading = -9999.9f;
    }
    return nullptr;
}

// 0x67DE10
template<typename PtrListType>
CEntity* CTaskSimpleClimb::ScanToGrabSectorList(PtrListType* sectorList, CPed* ped, CVector& outTargetPos, float& outAngle, eSurfaceType& outSurfaceType, bool isLaunch, bool hasToTestStandup, bool hasToTestDropOtherSide) {
    const auto cm = hasToTestDropOtherSide
        ? &ms_VaultColModel
        : hasToTestStandup
            ? &ms_StandUpColModel
            : &ms_ClimbColModel;
    CEntity* collidedEntity = nullptr;

    for (auto node = sectorList->GetNode(); node; node = node->m_next) {
        auto* entity = reinterpret_cast<CEntity*>(node->m_item);

        if (entity->IsScanCodeCurrent()) {
            continue;
        }

        entity->SetCurrentScanCode();

        if (!entity->m_bUsesCollision) {
            continue;
        }

        if (entity->IsBuilding()
            || (entity->IsObject()
                && (entity->IsStatic() || entity->AsObject()->physicalFlags.bDisableCollisionForce)
                && !entity->AsObject()->physicalFlags.bInfiniteMass
            )
            || (entity->IsVehicle()
                && (hasToTestStandup || hasToTestDropOtherSide || (ped->GetIntelligence()->GetTaskSwim() && entity->AsVehicle()->IsSubBoat()) || (entity->AsVehicle()->IsSubTrain() && entity->AsVehicle()->m_vecMoveSpeed.Magnitude2D() < 0.1F))
            )) {
            if (DistanceBetweenPoints(entity->GetBoundCentre(), ped->GetMatrix().TransformPoint(cm->GetBoundCenter())) >= entity->GetModelInfo()->GetColModel()->GetBoundRadius() + cm->GetBoundRadius()) {
                continue;
            }

            int32 numSpheres = -1;
            if (entity->IsVehicle() && entity->AsVehicle()->IsSubBoat()) {
                numSpheres                                       = entity->GetColModel()->m_pColData->m_nNumSpheres;
                entity->GetColModel()->m_pColData->m_nNumSpheres = 0;
            }

            if (CCollision::ProcessColModels(*ped->m_matrix, *cm, entity->GetMatrix(), *entity->GetColModel(), CWorld::m_aTempColPts, nullptr, nullptr, false) > 0) {
                if (hasToTestStandup || hasToTestDropOtherSide) {
                    if (entity->IsVehicle() && numSpheres > -1) {
                        entity->GetColModel()->m_pColData->m_nNumSpheres = numSpheres;
                    }

                    return entity;
                }
                uint8 nColSphereIndex = CWorld::m_aTempColPts[0].m_nPieceTypeA;

                if (nColSphereIndex == 0 || nColSphereIndex == 1 || nColSphereIndex == 2 || nColSphereIndex == 3 || nColSphereIndex == 4 || nColSphereIndex == 10) {
                    if (entity->IsVehicle() && numSpheres > -1) {
                        entity->GetColModel()->m_pColData->m_nNumSpheres = numSpheres;
                    }
                    return (CEntity*)(1);
                }

                auto relPosn = CWorld::m_aTempColPts[0].m_vecPoint - outTargetPos;
                if (nColSphereIndex == 16 || CWorld::m_aTempColPts[0].m_vecPoint.z <= outTargetPos.z && DotProduct(relPosn, ped->GetForward()) >= 0.0f || !g_surfaceInfos.CanClimb(CWorld::m_aTempColPts[0].m_nSurfaceTypeB)) {
                    if (entity->IsVehicle() && numSpheres > -1) {
                        entity->GetColModel()->m_pColData->m_nNumSpheres = numSpheres;
                    }
                    continue;
                }

                int32 fatModifier = ped->m_pPlayerData && !ped->GetIntelligence()->GetTaskSwim() ? (int32)CStats::GetFatAndMuscleModifier(STAT_MOD_0) : 0;

                if (!isLaunch || fatModifier >= 1) {
                    if (nColSphereIndex == 5 || nColSphereIndex == 11 || nColSphereIndex == 17) {
                        if (entity->IsVehicle() && numSpheres > -1) {
                            entity->GetColModel()->m_pColData->m_nNumSpheres = numSpheres;
                        }
                        continue;
                    }

                    if (fatModifier >= 1 && (!isLaunch || fatModifier >= 2)) {
                        if (nColSphereIndex == 6 || nColSphereIndex == 12 || nColSphereIndex == 18) {
                            if (entity->IsVehicle() && numSpheres > -1) {
                                entity->GetColModel()->m_pColData->m_nNumSpheres = numSpheres;
                            }
                            continue;
                        }
                    }
                }

                if (fabsf(CWorld::m_aTempColPts[0].m_vecNormal.x) <= 0.05f && fabsf(CWorld::m_aTempColPts[0].m_vecNormal.y) <= 0.05F) {
                    outTargetPos      = CWorld::m_aTempColPts[0].m_vecPoint;
                    outAngle         = ped->m_fCurrentRotation;
                    outSurfaceType   = CWorld::m_aTempColPts[0].m_nSurfaceTypeB;
                    collidedEntity = entity;
                } else {
                    CVector vecNormal = CWorld::m_aTempColPts[0].m_vecNormal;

                    if (DotProduct2D(vecNormal, CWorld::m_aTempColPts[0].m_vecPoint - ped->GetPosition()) < 0.0F) {
                        vecNormal = -vecNormal;
                    }

                    if (DotProduct(vecNormal, ped->GetForward()) > 0.3F) {
                        outTargetPos      = CWorld::m_aTempColPts[0].m_vecPoint;
                        outAngle         = std::atan2f(-vecNormal.x, vecNormal.y);
                        outSurfaceType   = CWorld::m_aTempColPts[0].m_nSurfaceTypeB;
                        collidedEntity = entity;
                    }
                }

                CMatrix matrix{ped->GetMatrix()};
                matrix.SetTranslateOnly(CWorld::m_aTempColPts[0].m_vecPoint);
                if (CCollision::ProcessColModels(matrix, ms_FindEdgeColModel, entity->GetMatrix(), *entity->GetColModel(), CWorld::m_aTempColPts, nullptr, nullptr, false) > 0) {
                    if (std::fabsf(CWorld::m_aTempColPts[0].m_vecNormal.x) <= 0.05f && std::fabsf(CWorld::m_aTempColPts[0].m_vecNormal.y) <= 0.05F) {
                        outTargetPos    = CWorld::m_aTempColPts[0].m_vecPoint;
                        outSurfaceType = CWorld::m_aTempColPts[0].m_nSurfaceTypeB;
                    } else {
                        CVector vecNormal = CWorld::m_aTempColPts[0].m_vecNormal;

                        if (DotProduct2D(vecNormal, CWorld::m_aTempColPts[0].m_vecPoint - ped->GetPosition()) < 0.0F) {
                            vecNormal = -vecNormal;
                        }

                        if (DotProduct(vecNormal, ped->GetForward()) > 0.3F) {
                            outTargetPos      = CWorld::m_aTempColPts[0].m_vecPoint;
                            outAngle         = std::atan2f(-vecNormal.x, vecNormal.y);
                            outSurfaceType   = CWorld::m_aTempColPts[0].m_nSurfaceTypeB;
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
CEntity* CTaskSimpleClimb::ScanToGrab(CPed* ped, CVector& outClimbPos, float& outClimbAngle, eSurfaceType& pSurfaceType, bool flag1, bool bStandUp, bool bVault, CVector* pedPosition) {
    if (!ms_ClimbColModel.m_pColData) {
        CreateColModel();
    }

    CEntity* collidedEntity = nullptr;

    CVector originalPedPosition = ped->GetPosition();

    if (pedPosition) {
        ped->SetPosn(*pedPosition);
    }

    outClimbPos = ped->GetPosition() + ped->GetForward() * 10.0f;

    auto outPoint = ped->m_matrix->TransformPoint(ms_ClimbColModel.GetBoundCenter());

    int32 startSectorX = CWorld::GetSectorX(outPoint.x - ms_ClimbColModel.GetBoundRadius());
    int32 startSectorY = CWorld::GetSectorY(outPoint.y - ms_ClimbColModel.GetBoundRadius());
    int32 endSectorX   = CWorld::GetSectorX(outPoint.x + ms_ClimbColModel.GetBoundRadius());
    int32 endSectorY   = CWorld::GetSectorY(outPoint.y + ms_ClimbColModel.GetBoundRadius());

    CWorld::IncrementCurrentScanCode();

    for (int32 y = startSectorY; y <= endSectorY; y++) {
        for (int32 x = startSectorX; x <= endSectorX; x++) {
            const auto ScanToGrabSector = [&]<typename PtrListType>(PtrListType& ptrList) -> CEntity* {
                return static_cast<CEntity*>(ScanToGrabSectorList(&ptrList, ped, outClimbPos, outClimbAngle, pSurfaceType, flag1, bStandUp, bVault));
            };
            auto scanResult1 = ScanToGrabSector(GetSector(x, y)->m_buildings);
            auto scanResult2 = ScanToGrabSector(GetRepeatSector(x, y)->Objects);
            if (!scanResult2) {
                scanResult2 = ScanToGrabSector(GetRepeatSector(x, y)->Vehicles);
            }

            if (scanResult1 == (CEntity*)(1) || scanResult2 == (CEntity*)(1)) {
                return nullptr;
            }

            if (auto entity = scanResult2 ? scanResult2 : scanResult1) {
                if (bStandUp || bVault) {
                    if (pedPosition) {
                        ped->SetPosn(originalPedPosition);
                    }

                    return entity;
                } else {
                    collidedEntity = entity;
                }
            }
        }
    }

    if (pedPosition) {
        ped->SetPosn(originalPedPosition);
    }

    if (collidedEntity) {
        if (collidedEntity->IsPhysical()) {
            outClimbPos = Invert(collidedEntity->GetMatrix()).TransformPoint(outClimbPos);
            outClimbAngle -= collidedEntity->GetHeading();
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
    ms_StandUpColModel.m_boundSphere.Set(1.5f, { 0.0f, 0.3f, 0.7f });
    ms_StandUpColModel.GetBoundingBox().Set({ -0.35f, -0.35f, -0.65f }, { 0.35f, 0.95f, 2.1f });
    ms_StandUpColModel.m_nColSlot = 0;

    ms_VaultColModel.AllocateData((int32)std::size(VaultColModelSpheres), 0, 0, 0, 0, false);
    ms_VaultColModel.m_pColData->SetSpheres(VaultColModelSpheres);
    ms_VaultColModel.m_boundSphere.Set(1.15f, { 0.0f, 0.5f, 0.1f });
    ms_VaultColModel.GetBoundingBox().Set({ -0.35f, -0.35f, -0.6f }, { 0.35f, 1.3f, 0.85f });
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
bool CTaskSimpleClimb::TestForStandUp(CPed* ped, const CVector& point, float fAngle) {
    CVector      climbPos;
    float        angle;
    eSurfaceType surfaceType;
    CVector      pedPos = point + GetClimbOffset3D({ ms_fAtEdgeOffsetHorz, ms_fAtEdgeOffsetVert }, fAngle);
    return !ScanToGrab(ped, climbPos, angle, surfaceType, false, true, false, &pedPos);
}

// 0x6804D0
bool CTaskSimpleClimb::TestForVault(CPed* ped, const CVector& point, float fAngle) {
    CVector      climbPos;
    float        angle;
    eSurfaceType surfaceType;
    CVector      pedPos = point + GetClimbOffset3D({ ms_fAtEdgeOffsetHorz, ms_fAtEdgeOffsetVert }, fAngle);
    return !m_ClimbEntity->IsVehicle()
        && !ScanToGrab(ped, climbPos, angle, surfaceType, false, false, true, &pedPos);
}

// 0x67DBE0
void CTaskSimpleClimb::StartAnim(CPed* ped) {
    switch (m_HeightForAnim) {
    case CLIMB_GRAB:
        if (m_Anim) {
            m_Anim->SetDeleteCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
            m_Anim = CAnimManager::BlendAnimation(ped->m_pRwClump, ANIM_GROUP_DEFAULT, ANIM_ID_CLIMB_IDLE, 4.0f);
        } else {
            m_Anim = CAnimManager::BlendAnimation(ped->m_pRwClump, ANIM_GROUP_DEFAULT, ANIM_ID_CLIMB_JUMP, 8.0f);
        }
        m_HeightForPos    = CLIMB_GRAB;
        m_HasToChangePosition  = false;
        m_HasToChangeAnimation = false;
        break;
    case CLIMB_PULLUP:
        if (m_HeightForPos == CLIMB_NOT_READY) {
            m_HeightForAnim = CLIMB_STANDUP;
            m_HeightForPos  = CLIMB_STANDUP;
            m_Anim          = CAnimManager::BlendAnimation(ped->m_pRwClump, ANIM_GROUP_DEFAULT, ANIM_ID_CLIMB_STAND, 4.0f);
            m_Anim->m_Flags &= ~ANIMATION_IS_PLAYING;
        } else {
            m_Anim->SetDeleteCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
            m_Anim         = CAnimManager::BlendAnimation(ped->m_pRwClump, ANIM_GROUP_DEFAULT, ANIM_ID_CLIMB_PULL, 1000.0f);
            m_HeightForPos = CLIMB_PULLUP;
        }
        m_HasToChangePosition  = false;
        m_HasToChangeAnimation = false;
        break;
    case CLIMB_STANDUP:
        if (m_Anim) {
            m_Anim->SetDeleteCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
        }
        m_Anim            = CAnimManager::BlendAnimation(ped->m_pRwClump, ANIM_GROUP_DEFAULT, ANIM_ID_CLIMB_STAND, 1000.0f);
        m_HasToChangePosition  = true;
        m_HasToChangeAnimation = false;
        break;
    case CLIMB_FINISHED:
    case CLIMB_FINISHED_V:
        CAnimManager::BlendAnimation(ped->m_pRwClump, ped->m_nAnimGroup, ANIM_ID_IDLE, 1000.0f);
        ped->SetMoveState(PEDMOVE_STILL);
        ped->SetMoveAnim();
        if (ped->m_pPlayerData) {
            ped->m_pPlayerData->m_fMoveBlendRatio = 0.0f;
        }
        if (m_Anim) {
            m_Anim->SetDeleteCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
        }
        m_Anim = CAnimManager::BlendAnimation(
            ped->m_pRwClump, ANIM_GROUP_DEFAULT, m_HeightForAnim == CLIMB_FINISHED_V ? ANIM_ID_CLIMB_JUMP2FALL : ANIM_ID_CLIMB_STAND_FINISH, 1000.0F
        );
        m_HasToChangePosition  = true;
        m_HasToChangeAnimation = false;
        break;
    case CLIMB_VAULT:
        if (m_Anim) {
            m_Anim->SetDeleteCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
        }
        m_Anim            = CAnimManager::BlendAnimation(ped->m_pRwClump, ANIM_GROUP_DEFAULT, ANIM_ID_CLIMB_JUMP_B, m_Anim && m_Anim->m_AnimId == ANIM_ID_CLIMB_STAND ? 16.0f : 1000.0f);
        m_HasToChangePosition  = true;
        m_HasToChangeAnimation = false;
        break;
    default:
        break;
    }

    if (m_Anim) {
        m_Anim->SetDeleteCallback(DeleteAnimCB, this);
        if (ped->m_pPlayerData && m_Anim->m_Flags & ANIMATION_IS_PLAYING && (m_Anim->m_AnimId == ANIM_ID_CLIMB_PULL || m_Anim->m_AnimId == ANIM_ID_CLIMB_STAND || m_Anim->m_AnimId == ANIM_ID_CLIMB_JUMP_B)) {
            m_Anim->m_Speed = CStats::GetFatAndMuscleModifier(STAT_MOD_1);
        }
    }
}

// 0x67A320
void CTaskSimpleClimb::StartSpeech(CPed* ped) const {
    if (ped->IsPlayer()) {
        if (m_HeightForAnim == CLIMB_PULLUP) {
            ped->Say(CTX_GLOBAL_PAIN_CJ_STRAIN);
        } else if (m_HeightForAnim == CLIMB_STANDUP) {
            ped->Say(CTX_GLOBAL_PAIN_CJ_STRAIN_EXHALE);
        }
    }
}

// 0x67A380
void CTaskSimpleClimb::DeleteAnimCB(CAnimBlendAssociation* anim, void* data) {
    const auto self = notsa::cast<CTaskSimpleClimb>(static_cast<CTask*>(data));

    self->m_Anim = nullptr;
}

// 0x67A5D0
void CTaskSimpleClimb::GetCameraStickModifier(CEntity* entity, float& outAlpha, float& outBeta, float& outStickAlpha, float& outStickBeta) {
    if (!m_Anim) {
        return;
    }

    if (m_Anim->m_AnimId == ANIM_ID_CLIMB_JUMP_B) {
        auto [pos, angle] = GetHandholdPosAndAngleForEntity(m_ClimbEntity, m_HandholdPos, m_HandholdHeading);
        pos += GetClimbOffset3D({ ms_fAtEdgeOffsetHorz, ms_fAtEdgeOffsetVert }, angle);

        CColPoint colPoint{};
        CEntity*  colEntity{};
        m_FallAfterVault = CWorld::ProcessVerticalLine(pos, pos.z - 3.0f, colPoint, colEntity, true, true, false, true)
            ? (int8)(std::max(pos.z - colPoint.m_vecPoint.z - 1.0f, 0.0F) * 10.0f)
            : 20;

        const float d = m_FallAfterVault > 8 ? -0.6f : -0.1f;
        if (outAlpha > d) {
            outStickAlpha = std::max((outAlpha - d) * -0.05f, -0.05f);
        }
    } else if (m_Anim->m_AnimId == ANIM_ID_CLIMB_IDLE) {
        float fHeading = m_ClimbEntity->GetHeading() - HALF_PI;
        if (fHeading > outBeta + PI) {
            fHeading -= TWO_PI;
        } else if (fHeading < outBeta - PI) {
            fHeading += TWO_PI;
        }

        if (fHeading > outBeta + HALF_PI + 0.2F) {
            outStickBeta = 0.02f;
        } else if (fHeading > outBeta + HALF_PI && outStickBeta < 0.0F) {
            outStickBeta = 0.0f;
        } else if (fHeading < outBeta - HALF_PI - 0.2F) {
            outStickBeta = -0.02f;
        } else if (fHeading < outBeta - HALF_PI && outStickBeta > 0.0F) {
            outStickBeta = 0.0f;
        }
    }
}

// 0x67A390
void CTaskSimpleClimb::GetCameraTargetPos(CPed* ped, CVector& outTargetPt) {
    outTargetPt = ped->GetPosition();

    if (!m_Anim) {
        return;
    }

    // Calculate offset
    CVector2D offset{};
    switch (m_Anim->m_AnimId) {
    case ANIM_ID_FALL_GLIDE:
    case ANIM_ID_CLIMB_JUMP2FALL:    offset = { ms_fVaultOffsetHorz, ms_fVaultOffsetVert };                                                                                                  break;
    case ANIM_ID_CLIMB_PULL:         offset = lerp<CVector2D>({ ms_fHangingOffsetHorz, ms_fAtEdgeOffsetHorz }, { ms_fHangingOffsetVert, ms_fAtEdgeOffsetVert }, m_Anim->GetTimeProgress()); break;
    case ANIM_ID_CLIMB_STAND:        offset = lerp<CVector2D>({ ms_fAtEdgeOffsetHorz, ms_fStandUpOffsetHorz }, { ms_fAtEdgeOffsetVert, ms_fStandUpOffsetVert }, m_Anim->GetTimeProgress()); break;
    case ANIM_ID_CLIMB_STAND_FINISH: offset = { ms_fStandUpOffsetHorz, ms_fStandUpOffsetVert };                                                                                              break;
    case ANIM_ID_CLIMB_JUMP_B:       offset = lerp<CVector2D>({ ms_fAtEdgeOffsetHorz, ms_fVaultOffsetHorz }, { ms_fAtEdgeOffsetVert, ms_fVaultOffsetVert }, m_Anim->GetTimeProgress());     break;
    default:                         return;
    }

    // Calculate final target position
    const auto [point, angle] = GetHandholdPosAndAngleForEntity(m_ClimbEntity, m_HandholdPos, m_HandholdHeading);
    outTargetPt = point + GetClimbOffset3D(offset, angle);
}

CVector CTaskSimpleClimb::GetClimbOffset3D(CVector2D offset2D, float angle) {
    return CVector{
        (CVector2D{ std::cos(angle), std::sin(angle) } * offset2D.x).GetPerpLeft(),
        offset2D.y
    };
}

std::pair<CVector, float> CTaskSimpleClimb::GetHandholdPosAndAngleForEntity(CEntity* entity, const CVector& handPos, float handAngle) {
    if (entity->IsPhysical()) {
        return { entity->GetMatrix().TransformPoint(handPos), handAngle + entity->GetHeading() };
    }
    return { handPos, handAngle };
}
