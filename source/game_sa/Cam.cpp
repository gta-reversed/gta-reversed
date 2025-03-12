#include "StdInc.h"

#include "Cam.h"
#include "Camera.h"

bool& gbFirstPersonRunThisFrame = *reinterpret_cast<bool*>(0xB6EC20);

void CCam::InjectHooks() {
    RH_ScopedClass(CCam);
    RH_ScopedCategory("Camera");

    RH_ScopedInstall(Constructor, 0x517730);
    RH_ScopedInstall(Init, 0x50E490);
    RH_ScopedInstall(CacheLastSettingsDWCineyCam, 0x50D7A0, { .reversed = false });
    RH_ScopedInstall(DoCamBump, 0x50CB30);
    RH_ScopedInstall(Finalise_DW_CineyCams, 0x50DD70, { .reversed = false });
    RH_ScopedInstall(GetCoreDataForDWCineyCamMode, 0x517130, { .reversed = false });
    RH_ScopedInstall(GetLookFromLampPostPos, 0x5161A0, { .reversed = false });
    RH_ScopedInstall(GetVectorsReadyForRW, 0x509CE0, { .reversed = false });
    RH_ScopedInstall(Get_TwoPlayer_AimVector, 0x513E40);
    RH_ScopedInstall(IsTimeToExitThisDWCineyCamMode, 0x517400, { .reversed = false });
    RH_ScopedInstall(KeepTrackOfTheSpeed, 0x509DF0, { .reversed = false });
    RH_ScopedInstall(LookBehind, 0x520690, { .reversed = false });
    RH_ScopedInstall(LookRight, 0x520E40, { .reversed = false });
    RH_ScopedInstall(RotCamIfInFrontCar, 0x50A4F0, { .reversed = false });
    RH_ScopedInstall(Using3rdPersonMouseCam, 0x50A850, { .reversed = false });
    RH_ScopedInstall(Process, 0x526FC0, { .reversed = false });
    RH_ScopedInstall(ProcessArrestCamOne, 0x518500, { .reversed = false });
    RH_ScopedInstall(ProcessPedsDeadBaby, 0x519250);
    RH_ScopedInstall(Process_1rstPersonPedOnPC, 0x50EB70, { .reversed = false });
    RH_ScopedInstall(Process_1stPerson, 0x517EA0, { .reversed = false });
    RH_ScopedInstall(Process_AimWeapon, 0x521500, { .reversed = false });
    RH_ScopedInstall(Process_AttachedCam, 0x512B10, { .reversed = false });
    RH_ScopedInstall(Process_Cam_TwoPlayer, 0x525E50, { .reversed = false });
    RH_ScopedInstall(Process_Cam_TwoPlayer_InCarAndShooting, 0x519810, { .reversed = false });
    RH_ScopedInstall(Process_Cam_TwoPlayer_Separate_Cars, 0x513510, { .reversed = false });
    RH_ScopedInstall(Process_Cam_TwoPlayer_Separate_Cars_TopDown, 0x513BE0, { .reversed = false });
    RH_ScopedInstall(Process_DW_BirdyCam, 0x51B850, { .reversed = false });
    RH_ScopedInstall(Process_DW_CamManCam, 0x51B120, { .reversed = false });
    RH_ScopedInstall(Process_DW_HeliChaseCam, 0x51A740, { .reversed = false });
    RH_ScopedInstall(Process_DW_PlaneCam1, 0x51C760, { .reversed = false });
    RH_ScopedInstall(Process_DW_PlaneCam2, 0x51CC30, { .reversed = false });
    RH_ScopedInstall(Process_DW_PlaneCam3, 0x51D100, { .reversed = false });
    RH_ScopedInstall(Process_DW_PlaneSpotterCam, 0x51C250, { .reversed = false });
    RH_ScopedInstall(Process_Editor, 0x50F3F0, { .reversed = false });
    RH_ScopedInstall(Process_Fixed, 0x51D470, { .reversed = false });
    RH_ScopedInstall(Process_FlyBy, 0x5B25F0, { .reversed = false });
    RH_ScopedInstall(Process_FollowCar_SA, 0x5245B0, { .reversed = false });
    RH_ScopedInstall(Process_FollowPedWithMouse, 0x50F970, { .reversed = false });
    RH_ScopedInstall(Process_FollowPed_SA, 0x522D40, { .reversed = false });
    RH_ScopedInstall(Process_M16_1stPerson, 0x5105C0, { .reversed = false });
    RH_ScopedInstall(Process_Rocket, 0x511B50, { .reversed = false });
    RH_ScopedInstall(Process_SpecialFixedForSyphon, 0x517500, { .reversed = false });
    RH_ScopedInstall(Process_WheelCam, 0x512110, { .reversed = false });
}

// 0x517730
CCam::CCam() {
    Init();
}

CCam* CCam::Constructor() {
    this->CCam::CCam();
    return this;
}

// 0x50E490
void CCam::Init() {
    m_vecFront = CVector(0, 0, -1);
    m_vecUp = CVector(0, 0, 1);
    m_nMode = eCamMode::MODE_FOLLOWPED;
    m_bRotating = false;
    m_nDoCollisionChecksOnFrameNum = 1;
    m_nDoCollisionCheckEveryNumOfFrames = 9;
    m_nFrameNumWereAt = 0;
    m_bCollisionChecksOn = true;
    m_fRealGroundDist = 0.0f;
    m_fBetaSpeed = 0.0f;
    m_fAlphaSpeed = 0.0f;
    m_fCameraHeightMultiplier = 0.75;
    m_fMaxRoleAngle = DegreesToRadians(20.0f);
    m_fDistance = 30.0f;
    m_fDistanceSpeed = 0.0f;
    m_pLastCarEntered = nullptr;
    m_pLastPedLookedAt = nullptr;
    m_bResetStatics = true;
    m_fHorizontalAngle = 0.0f;
    m_fTilt = 0.0f;
    m_fTiltSpeed = 0.0f;
    m_bFixingBeta = false;
    m_fCaMinDistance = 0.0f;
    m_fCaMaxDistance = 0.0f;
    m_bLookingBehind = false;
    m_bLookingLeft = false;
    m_bLookingRight = false;
    m_fPlayerInFrontSyphonAngleOffSet = DegreesToRadians(20.0f);
    m_fSyphonModeTargetZOffSet = 0.5f;
    m_fRadiusForDead = 1.5f;
    m_nDirectionWasLooking = 3; // TODO: enum
    m_bLookBehindCamWasInFront = 0;
    m_fRoll = 0.0f;
    m_fRollSpeed = 0.0f;
    m_fCloseInPedHeightOffset = 0.0f;
    m_fCloseInPedHeightOffsetSpeed = 0.0f;
    m_fCloseInCarHeightOffset = 0.0f;
    m_fCloseInCarHeightOffsetSpeed = 0.0f;
    m_fPedBetweenCameraHeightOffset = 0.0f;
    m_fTargetBeta = 0.0f;
    m_fBufferedTargetBeta = 0.0f;
    m_fBufferedTargetOrientation = 0.0f;
    m_fBufferedTargetOrientationSpeed = 0.0f;
    m_fDimensionOfHighestNearCar = 0.0;
    m_fBeta_Targeting = 0.0f;
    m_fX_Targetting = 0.0f;
    m_fY_Targetting = 0.0f;
    m_pCarWeAreFocussingOn = nullptr;
    m_pCarWeAreFocussingOnI = nullptr;
    m_fCamBumpedHorz = 1.0f;
    m_fCamBumpedVert = 0.0f;
    m_nCamBumpedTime = 0;
    for (int i = 0; i < 4; ++i) {
        m_anTargetHistoryTime[i] = 0;
        m_avecTargetHistoryPos[i] = CVector{};
    }
    m_nCurrentHistoryPoints = 0;
    gPlayerPedVisible = true;
    gbCineyCamMessageDisplayed = 2; // TODO: enum
    gCameraDirection = 3; // TODO: enum
    gCameraMode = (eCamMode)-1;
    gLastTime2PlayerCameraWasOK = 0;
    gLastTime2PlayerCameraCollided = 0;
    TheCamera.m_bForceCinemaCam = false;
}

// 0x50D7A0
void CCam::CacheLastSettingsDWCineyCam() {
    assert(0);
}

// 0x50CB30
void CCam::DoCamBump(float horizontal, float vertical) {
    m_fCamBumpedHorz = horizontal;
    m_fCamBumpedVert = vertical;
    m_nCamBumpedTime = CTimer::GetTimeInMS();
}

// 0x50DD70
void CCam::Finalise_DW_CineyCams(CVector*, CVector*, float, float, float, float) {
    assert(0);
}

// 0x517130
void CCam::GetCoreDataForDWCineyCamMode(CEntity**, CVehicle**, CVector*, CVector*, CVector*, CVector*, CVector*, CVector*, float*, CVector*, float*, CColSphere*) {
    assert(0);
}

// 0x5161A0
void CCam::GetLookFromLampPostPos(CEntity*, CPed*, CVector&, CVector&) {
    assert(0);
}

// 0x509CE0
void CCam::GetVectorsReadyForRW() {
    assert(0);
}

// 0x513E40
CEntity* CCam::Get_TwoPlayer_AimVector(CVector& vecFront) {
    constexpr int INCAR_2PLAYER_WEAPON_RANGE_MULTIPLIER = 2;

    // Are we targetting an entity at the moment?
    CPlayerPed* player = FindPlayerPed(PED_TYPE_PLAYER1);
    if (player && player->m_pVehicle && !player->m_pVehicle->IsDriver(player)) {
        player = FindPlayerPed(PED_TYPE_PLAYER2);
    }

    const auto weaponInfo    = player->GetActiveWeapon().GetWeaponInfo(player);
    CEntity*   pLockOnTarget = CWeapon::FindNearestTargetEntityWithScreenCoors(m_fX_Targetting, m_fY_Targetting, INCAR_2PLAYER_WEAPON_RANGE_MULTIPLIER * weaponInfo.GetWeaponRange(), player->GetPosition(), nullptr, nullptr);

    if (pLockOnTarget) {
        vecFront = pLockOnTarget->GetPosition() - m_vecSource;
    } else {
        CVector right = CrossProduct(m_vecFront, m_vecUp);
        vecFront      = m_vecFront + right * m_fX_Targetting * std::tan(m_fFOV * (PI / 180.0f) * 0.5f) - m_vecUp * (m_fY_Targetting * std::tan(m_fFOV * (PI / 180.0f) * 0.5f) / CDraw::GetAspectRatio());
    }
    vecFront.Normalise();

    return pLockOnTarget;
}

// 0x517400
void CCam::IsTimeToExitThisDWCineyCamMode(int32, CVector*, CVector*, float, bool) {
    assert(0);
}

// 0x509DF0
void CCam::KeepTrackOfTheSpeed(const CVector&, const CVector&, const CVector&, const float&, const float&, const float&) {
    assert(0);
}

// 0x520690
void CCam::LookBehind() {
    assert(0);
}

// 0x520E40
void CCam::LookRight(bool bLookRight) {
    assert(0);
}

// 0x50A4F0
void CCam::RotCamIfInFrontCar(const CVector&, float) {
    assert(0);
}

// 0x50A850
bool CCam::Using3rdPersonMouseCam() const {
    return CCamera::m_bUseMouse3rdPerson && m_nMode == MODE_FOLLOWPED;
}

// 0x509DC0
bool CCam::GetWeaponFirstPersonOn() {
    return m_pCamTargetEntity && m_pCamTargetEntity->IsPed() && m_pCamTargetEntity->AsPed()->GetActiveWeapon().m_IsFirstPersonWeaponModeSelected;
}

// 0x526FC0
bool CCam::Process() {
    assert(0);
    return false;
}

// 0x518500
bool CCam::ProcessArrestCamOne() {
    /*
    constexpr int NUM_ARREST_CAMS = 6;

    bool bGotCameraPos = false;
    CVector ArrestCamPos;
    CVector PlayerPosition;
    CVector CopPosition;
    CVector CopToPlayer;

    CPed *pHandyPedPointer = nullptr;
    CVehicle *pHandyCarPointer = nullptr;
    CPed *pHandyCopPointer = nullptr;

    CEntity *pHitEntity = nullptr;
    CColPoint colPoint;
    m_fFOV = 45.0f;

    int32 aTryArrestCamList[NUM_ARREST_CAMS];
    rng::fill(aTryArrestCamList, -1);

    if (m_bResetStatics) {
        nUsingWhichCamera = 0;

        if (TheCamera.m_pTargetEntity->GetIsTypePed()) {
            pHandyPedPointer = (CPed *)TheCamera.m_pTargetEntity;
            pHandyPedPointer->PlayerPosition.GetBonePosition(BONE_SPINE1, true);
            if (FindPlayerPed() && FindPlayerPed()->GetPlayerData()->m_ArrestingOfficer)
                pHandyCopPointer = FindPlayerPed()->GetPlayerData()->m_ArrestingOfficer;

            aTryArrestCamList[0] = ARRESTCAM_DW;
            if (pHandyCopPointer && CGeneral::GetRandomNumberInRange(0.0f, 1.0f) > 0.5f) {
                aTryArrestCamList[1] = ARRESTCAM_OVERSHOULDER;
                aTryArrestCamList[2] = ARRESTCAM_ALONGGROUND;
                aTryArrestCamList[3] = ARRESTCAM_OVERSHOULDER;
                aTryArrestCamList[4] = ARRESTCAM_FROMLAMPPOST;
            } else {
                aTryArrestCamList[1] = ARRESTCAM_ALONGGROUND;
                aTryArrestCamList[2] = ARRESTCAM_OVERSHOULDER;
                aTryArrestCamList[3] = ARRESTCAM_FROMLAMPPOST;
            }
        } else if (TheCamera.m_pTargetEntity->GetIsTypeVehicle()) {
            pHandyCarPointer = (CVehicle *)TheCamera.m_pTargetEntity;
            if (pHandyCarPointer->m_pDriver && pHandyCarPointer->m_pDriver->IsPlayer()) {
                pHandyPedPointer = pHandyCarPointer->m_pDriver;
                pHandyPedPointer->PlayerPosition.GetBonePosition(BONE_SPINE1, true);
            } else {
                pHandyPedPointer = NULL;
                PlayerPosition = pHandyCarPointer->GetPosition();
            }

            if (FindPlayerPed() && FindPlayerPed()->GetPlayerData()->m_ArrestingOfficer)
                pHandyCopPointer = FindPlayerPed()->GetPlayerData()->m_ArrestingOfficer;

            if (pHandyCopPointer && CGeneral::GetRandomNumberInRange(0.0f, 1.0f) > 0.65f) {
                aTryArrestCamList[0] = ARRESTCAM_OVERSHOULDER;
                aTryArrestCamList[1] = ARRESTCAM_FROMLAMPPOST;
                aTryArrestCamList[2] = ARRESTCAM_ALONGGROUND;
                aTryArrestCamList[3] = ARRESTCAM_OVERSHOULDER;
            } else {
                aTryArrestCamList[0] = ARRESTCAM_FROMLAMPPOST;
                aTryArrestCamList[1] = ARRESTCAM_ALONGGROUND;
                aTryArrestCamList[2] = ARRESTCAM_OVERSHOULDER;
            }
        } else {
            return false;
        }

        if (!CHud::m_BigMessage[BIG_MESSAGE_WASTED][0]) {
            CMessages::AddBigMessage(TheText.Get("BUSTED"), 1'000 * 5, 2);
        }

        int32 nTryCam = 0;
        while (nUsingWhichCamera == 0 && nTryCam < NUM_ARREST_CAMS && aTryArrestCamList[nTryCam] > 0) {
            pStoredCopPed = nullptr;

            switch (aTryArrestCamList[nTryCam]) {
            case ARRESTCAM_DW: {
                gTimeDWBustedCamStarted = CTimer::GetTimeInMS();
                if (ProcessDWBustedCam1(pHandyCopPointer, true)) {
                    TheCamera.m_pTargetEntity->SetIsVisible(false);
                    nUsingWhichCamera = ARRESTCAM_DW;
                    m_bResetStatics = false;
                    return true;
                }
            } break;
            case ARRESTCAM_OVERSHOULDER:
                if (pHandyCopPointer) {
                    bGotCameraPos = GetLookOverShoulderPos(TheCamera.m_pTargetEntity, pHandyCopPointer, PlayerPosition, ArrestCamPos);
                    pStoredCopPed = pHandyCopPointer;
                    pHandyCopPointer = nullptr;
                } else if (pHandyPedPointer) {
                    CEntity **ppNearbyEntities = pHandyPedPointer->GetPedIntelligence()->GetNearbyPeds();
                    for (int i = 0; i < pHandyPedPointer->GetPedIntelligence()->GetMaxNumPedsInRange(); i++) {
                        CEntity *pNearbyEntity = ppNearbyEntities[i];
                        if (pNearbyEntity) {
                            assert(pNearbyEntity->GetType() == ENTITY_TYPE_PED);
                            CPed *pNearbyPed = (CPed *)pNearbyEntity;
                            CTaskSimpleArrestPed *pTaskArrest = (CTaskSimpleArrestPed *)pNearbyPed->GetPedIntelligence()->FindTaskActiveByType(CTaskTypes::TASK_SIMPLE_ARREST_PED);
                            if (pTaskArrest) {
                                if (FindPlayerPed() == pTaskArrest->GetTargetPed()) {
                                    bGotCameraPos = GetLookOverShoulderPos(TheCamera.m_pTargetEntity, pNearbyPed, PlayerPosition, ArrestCamPos);
                                    if (bGotCameraPos) {
                                        pStoredCopPed = pNearbyPed;
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
                break;

            case ARRESTCAM_ALONGGROUND:
                if (pHandyCopPointer) {
                    bGotCameraPos = GetLookAlongGroundPos(TheCamera.m_pTargetEntity, pHandyCopPointer, PlayerPosition, ArrestCamPos);
                    pStoredCopPed = pHandyCopPointer;
                    pHandyCopPointer = nullptr;
                } else if (pHandyPedPointer) {
                    CEntity **ppNearbyEntities = pHandyPedPointer->GetPedIntelligence()->GetNearbyPeds();
                    for (int i = 0; i < pHandyPedPointer->GetPedIntelligence()->GetMaxNumPedsInRange(); i++) {
                        CEntity *pNearbyEntity = ppNearbyEntities[i];
                        if (pNearbyEntity) {
                            assert(pNearbyEntity->GetType() == ENTITY_TYPE_PED);
                            CPed *pNearbyPed = (CPed *)pNearbyEntity;
                            CTaskSimpleArrestPed *pTaskArrest = (CTaskSimpleArrestPed *)pNearbyPed->GetPedIntelligence()->FindTaskActiveByType(CTaskTypes::TASK_SIMPLE_ARREST_PED);
                            if (pTaskArrest) {
                                if (FindPlayerPed() == pTaskArrest->GetTargetPed()) {
                                    bGotCameraPos = GetLookOverShoulderPos(TheCamera.m_pTargetEntity, pNearbyPed, PlayerPosition, ArrestCamPos);
                                    if (bGotCameraPos) {
                                        pStoredCopPed = pNearbyPed;
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
                break;

            case ARRESTCAM_FROMLAMPPOST:
                bGotCameraPos = GetLookFromLampPostPos(TheCamera.m_pTargetEntity, pHandyCopPointer, PlayerPosition, ArrestCamPos);
                break;
            }

            if (bGotCameraPos) {
                if (pStoredCopPed) {
                    CEntity::RegisterReference((CEntity **)&pStoredCopPed);
                }

                nUsingWhichCamera = aTryArrestCamList[nTryCam];

                if (nUsingWhichCamera == ARRESTCAM_ALONGGROUND) {
                    float fChooseCamRotation = CGeneral::GetRandomNumberInRange(0.0f, 5.0f);
                    if (fChooseCamRotation < 1.0f)
                        nUsingWhichCamera = ARRESTCAM_ALONGGROUND;
                    else if (fChooseCamRotation < 2.0f)
                        nUsingWhichCamera = ARRESTCAM_ALONGGROUND_RIGHT;
                    else if (fChooseCamRotation < 3.0f)
                        nUsingWhichCamera = ARRESTCAM_ALONGGROUND_RIGHT_UP;
                    else if (fChooseCamRotation < 4.0f)
                        nUsingWhichCamera = ARRESTCAM_ALONGGROUND_LEFT;
                    else
                        nUsingWhichCamera = ARRESTCAM_ALONGGROUND_LEFT_UP;
                }
            }

            nTryCam++;
        }

        m_vecSource = ArrestCamPos;
        CVector SourceBeforeChange = m_vecSource;
        TheCamera.AvoidTheGeometry(SourceBeforeChange, PlayerPosition, m_vecSource, m_fFOV);

        m_vecFront = PlayerPosition - m_vecSource;
        m_vecFront.Normalise();

        m_vecUp = CVector(0.0f, 0.0f, 1.0f);
        CVector TempRight = CrossProduct(m_vecFront, m_vecUp);
        TempRight.Normalise();
        m_vecUp = CrossProduct(TempRight, m_vecFront);

        if (nUsingWhichCamera != 0)
            m_bResetStatics = false;

        return true;
    } else {
        if (nUsingWhichCamera == ARRESTCAM_DW) {
            TheCamera.m_pTargetEntity->SetIsVisible(false);

            if (FindPlayerPed() && FindPlayerPed()->GetPlayerData()->m_ArrestingOfficer)
                pHandyCopPointer = FindPlayerPed()->GetPlayerData()->m_ArrestingOfficer;

            return ProcessDWBustedCam1(pHandyCopPointer, false) ? true : false;
        }

        if (TheCamera.m_pTargetEntity->GetIsTypePed()) {
            ((CPed *)TheCamera.m_pTargetEntity)->PlayerPosition.GetBonePosition(BONE_SPINE1, true);
        } else if (TheCamera.m_pTargetEntity->GetIsTypeVehicle()) {
            if (((CVehicle *)TheCamera.m_pTargetEntity)->m_pDriver && ((CVehicle *)TheCamera.m_pTargetEntity)->m_pDriver->IsPlayer()) {
                ((CVehicle *)TheCamera.m_pTargetEntity)->m_pDriver->PlayerPosition.GetBonePosition(BONE_SPINE1, true);
            } else
                PlayerPosition = TheCamera.m_pTargetEntity->GetPosition();
        } else
            return false;

        if (nUsingWhichCamera == ARRESTCAM_OVERSHOULDER && pStoredCopPed) {
            bGotCameraPos = GetLookOverShoulderPos(TheCamera.m_pTargetEntity, pStoredCopPed, PlayerPosition, ArrestCamPos);
            if (ArrestCamPos.z > m_vecSource.z + ARRESTCAM_S_ROTATION_UP * CTimer::GetTimeStep())
                ArrestCamPos.z = m_vecSource.z + ARRESTCAM_S_ROTATION_UP * CTimer::GetTimeStep();
        } else if (nUsingWhichCamera > ARRESTCAM_ALONGGROUND && nUsingWhichCamera <= ARRESTCAM_ALONGGROUND_LEFT_UP) {
            ArrestCamPos = m_vecSource;
            m_vecFront = PlayerPosition - m_vecSource;
            m_vecFront.Normalise();

            m_vecUp = CVector(0.0f, 0.0f, 1.0f);
            CVector TempRight = CrossProduct(m_vecFront, m_vecUp);
            TempRight.Normalise();

            if (nUsingWhichCamera == ARRESTCAM_ALONGGROUND_LEFT || nUsingWhichCamera == ARRESTCAM_ALONGGROUND_LEFT_UP)
                TempRight *= -1.0f;

            if (!CWorld::TestSphereAgainstWorld(m_vecSource + 0.5f * TempRight, 0.4f, TheCamera.m_pTargetEntity, true, true, false, true, false, true)) {
                bGotCameraPos = true;
                ArrestCamPos += TempRight * ARRESTCAM_ROTATION_SPEED * CTimer::GetTimeStep();
                if (nUsingWhichCamera == ARRESTCAM_ALONGGROUND_RIGHT_UP || nUsingWhichCamera == ARRESTCAM_ALONGGROUND_LEFT_UP) {
                    ArrestCamPos.z += ARRESTCAM_ROTATION_UP * CTimer::GetTimeStep();
                } else {
                    bool bGroundFound = false;
                    float fGroundZ = CWorld::FindGroundZFor3DCoord(ArrestCamPos, &bGroundFound);
                    if (bGroundFound)
                        ArrestCamPos.z = fGroundZ + ARRESTDIST_ABOVE_GROUND;
                }
            }
        } else if (nUsingWhichCamera == ARRESTCAM_FROMLAMPPOST) {
            ArrestCamPos = m_vecSource;
            m_vecFront = PlayerPosition - ArrestCamPos;
            m_vecFront.z = 0.0f;
            m_vecFront.Normalise();

            m_vecUp = CVector(0.0f, 0.0f, 1.0f);
            CVector TempRight = CrossProduct(m_vecFront, m_vecUp);
            TempRight.Normalise();

            m_vecFront = (PlayerPosition - ArrestCamPos) + TempRight * ARRESTCAM_LAMPPOST_ROTATEDIST;
            m_vecFront.z = 0.0f;
            m_vecFront.Normalise();

            if (!CWorld::TestSphereAgainstWorld(ArrestCamPos + 0.5f * m_vecFront, 0.4f, TheCamera.m_pTargetEntity, true, true, false, true, false, true)) {
                bGotCameraPos = true;
                ArrestCamPos += m_vecFront * ARRESTCAM_LAMPPOST_TRANSLATE * CTimer::GetTimeStep();
            }
        }

        if (bGotCameraPos) {
            m_vecSource = ArrestCamPos;
            CVector SourceBeforeChange = m_vecSource;
            TheCamera.AvoidTheGeometry(SourceBeforeChange, PlayerPosition, m_vecSource, m_fFOV);

            m_vecFront = PlayerPosition - m_vecSource;
            m_vecFront.Normalise();

            m_vecUp = CVector(0.0f, 0.0f, 1.0f);
            CVector TempRight = CrossProduct(m_vecFront, m_vecUp);
            TempRight.Normalise();
            m_vecUp = CrossProduct(TempRight, m_vecFront);
        } else {
            CVector SourceBeforeChange = m_vecSource;
            TheCamera.AvoidTheGeometry(SourceBeforeChange, PlayerPosition, m_vecSource, m_fFOV);
        }
    }
*/
    return true;
}

// 0x519250
void CCam::ProcessPedsDeadBaby() {
    constexpr float DEADCAM_HEIGHT_START   = 2.0f;
    constexpr float DEADCAM_HEIGHT_RATE    = 0.04f;
    constexpr float DEADCAM_WAFT_AMPLITUDE = 2.0f;
    constexpr float DEADCAM_WAFT_RATE      = 600.0f;
    constexpr float DEADCAM_WAFT_TILT_AMP  = -0.35f;

    CVector PlayerPosition;
    CVector DeadCamPos;
    CVector vecTempRight;

    // first find the target position
    if (TheCamera.m_pTargetEntity->IsPed()) {
        PlayerPosition = TheCamera.m_pTargetEntity->AsPed()->GetBonePosition(BONE_SPINE1, true);
    } else if (TheCamera.m_pTargetEntity->IsVehicle()) {
        PlayerPosition = TheCamera.m_pTargetEntity->GetPosition();
        PlayerPosition.z += TheCamera.m_pTargetEntity->GetColModel()->GetBoundingBox().m_vecMax.z;
    } else {
        return;
    }

    static float startTimeDWDeadCam = 0.0f;
    static float cameraRot          = 0.0f;

    if (m_bResetStatics) {
        startTimeDWDeadCam = (float)CTimer::GetTimeInMS();
        cameraRot          = 0.0f;

        TheCamera.m_nTimeLastChange = CTimer::GetTimeInMS();
        DeadCamPos                  = PlayerPosition;
        DeadCamPos.z += DEADCAM_HEIGHT_START;

        float fCamWaterLevel = 0.0f;
        if (CWaterLevel::GetWaterLevelNoWaves(DeadCamPos, &fCamWaterLevel)) {
            if (fCamWaterLevel + 1.5f > DeadCamPos.z) {
                DeadCamPos.z = fCamWaterLevel + 1.5f;
            }
        }

        vecTempRight   = CrossProduct(TheCamera.m_pTargetEntity->GetMatrix().GetForward(), CVector(0.0f, 0.0f, 1.0f));
        vecTempRight.z = 0.0f;
        vecTempRight.Normalise();

        m_vecFront = PlayerPosition - DeadCamPos;
        m_vecFront.Normalise();

        m_vecUp = CrossProduct(vecTempRight, m_vecFront);
        m_vecUp.Normalise();

        m_bResetStatics = false;
    } else {
        DeadCamPos = m_vecSource;

        if (!CWorld::TestSphereAgainstWorld(DeadCamPos + 0.2f * CVector(0.0f, 0.0f, 1.0f), 0.3f, TheCamera.m_pTargetEntity, true, true, false, true, false, true)) {
            DeadCamPos.z += DEADCAM_HEIGHT_RATE * CTimer::GetTimeStep();
        }

        vecTempRight   = CrossProduct(TheCamera.m_pTargetEntity->GetMatrix().GetForward(), CVector(0.0f, 0.0f, 1.0f));
        vecTempRight.z = 0.0f;
        vecTempRight.Normalise();

        float   fTimeOffset = (float)(CTimer::GetTimeInMS() - TheCamera.m_nTimeLastChange);
        float   fWaftOffset = (std::min(1000.0f, fTimeOffset) / 1000.0f) * std::sin(fTimeOffset / DEADCAM_WAFT_RATE);
        CVector vecWaftPos  = vecTempRight * DEADCAM_WAFT_AMPLITUDE * fWaftOffset;
        vecWaftPos.x += PlayerPosition.x;
        vecWaftPos.y += PlayerPosition.y;
        vecWaftPos.z = DeadCamPos.z;

        CVector vecTempTest = vecWaftPos - DeadCamPos;
        vecTempTest.Normalise();

        if (!CWorld::TestSphereAgainstWorld(DeadCamPos + 0.2f * vecTempTest, 0.3f, TheCamera.m_pTargetEntity, true, true, false, true, false, true)) {
            DeadCamPos = vecWaftPos;
        }

        m_vecFront = CVector(0.0f, 0.0f, -1.0f);
        m_vecFront += vecTempRight * DEADCAM_WAFT_TILT_AMP * (std::min(2000.0f, fTimeOffset) / 2000.0f) * std::cos(fTimeOffset / DEADCAM_WAFT_RATE);
        m_vecFront.Normalise();

        m_vecUp = CrossProduct(vecTempRight, m_vecFront);
        m_vecUp.Normalise();
    }

    m_vecSource                = DeadCamPos;
    CVector SourceBeforeChange = m_vecSource;
    TheCamera.AvoidTheGeometry(SourceBeforeChange, PlayerPosition, m_vecSource, m_fFOV);
    TheCamera.m_bMoveCamToAvoidGeom = false;
}

// 0x50EB70
bool CCam::Process_1rstPersonPedOnPC(const CVector&, float, float, float) {
    assert(0);
    return false;
}

// 0x517EA0
bool CCam::Process_1stPerson(const CVector&, float, float, float) {
    assert(0);
    return false;
}

// 0x521500
bool CCam::Process_AimWeapon(const CVector&, float, float, float) {
    assert(0);
    return false;
}

// 0x512B10
bool CCam::Process_AttachedCam() {
    assert(0);
    return false;
}

// 0x525E50
bool CCam::Process_Cam_TwoPlayer() {
    assert(0);
    return false;
}

// 0x519810
bool CCam::Process_Cam_TwoPlayer_InCarAndShooting() {
    assert(0);
    return false;
}

// 0x513510
bool CCam::Process_Cam_TwoPlayer_Separate_Cars() {
    assert(0);
    return false;
}

// 0x513BE0
bool CCam::Process_Cam_TwoPlayer_Separate_Cars_TopDown() {
    assert(0);
    return false;
}

// 0x51B850
bool CCam::Process_DW_BirdyCam(bool) {
    assert(0);
    return false;
}

// 0x51B120
bool CCam::Process_DW_CamManCam(bool) {
    assert(0);
    return false;
}

// 0x51A740
bool CCam::Process_DW_HeliChaseCam(bool) {
    assert(0);
    return false;
}

// 0x51C760
bool CCam::Process_DW_PlaneCam1(bool) {
    assert(0);
    return false;
}

// 0x51CC30
bool CCam::Process_DW_PlaneCam2(bool) {
    assert(0);
    return false;
}

// 0x51D100
bool CCam::Process_DW_PlaneCam3(bool) {
    assert(0);
    return false;
}

// 0x51C250
bool CCam::Process_DW_PlaneSpotterCam(bool) {
    assert(0);
    return false;
}

// 0x50F3F0
bool CCam::Process_Editor(const CVector&, float, float, float) {
    assert(0);
    return false;
}

// 0x51D470
bool CCam::Process_Fixed(const CVector&, float, float, float) {
    assert(0);
    return false;
}

// 0x5B25F0
bool CCam::Process_FlyBy(const CVector&, float, float, float) {
    assert(0);
    return false;
}

// 0x5245B0
bool CCam::Process_FollowCar_SA(const CVector&, float, float, float, bool) {
    assert(0);
    return false;
}

// 0x50F970
bool CCam::Process_FollowPedWithMouse(const CVector&, float, float, float) {
    assert(0);
    return false;
}

// 0x522D40
bool CCam::Process_FollowPed_SA(const CVector&, float, float, float, bool) {
    assert(0);
    return false;
}

// 0x5105C0
bool CCam::Process_M16_1stPerson(const CVector&, float, float, float) {
    assert(0);
    return false;
}

// 0x511B50
bool CCam::Process_Rocket(const CVector&, float, float, float, bool) {
    assert(0);
    return false;
}

// 0x517500
bool CCam::Process_SpecialFixedForSyphon(const CVector&, float, float, float) {
    assert(0);
    return false;
}

// 0x512110
bool CCam::Process_WheelCam(const CVector&, float, float, float) {
    assert(0);
    return false;
}
