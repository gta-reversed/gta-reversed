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
    RH_ScopedInstall(Process_FollowCar_SA, 0x5245B0);
    RH_ScopedInstall(Process_FollowPedWithMouse, 0x50F970, { .reversed = false });
    RH_ScopedInstall(Process_FollowPed_SA, 0x522D40, { .reversed = false });
    RH_ScopedInstall(Process_M16_1stPerson, 0x5105C0, { .reversed = false });
    RH_ScopedInstall(Process_Rocket, 0x511B50, { .reversed = false });
    RH_ScopedInstall(Process_SpecialFixedForSyphon, 0x517500, { .reversed = false });
    RH_ScopedInstall(Process_WheelCam, 0x512110, { .reversed = false });
    RH_ScopedGlobalInstall(FTrunc, 0x50A0A0);
    RH_ScopedGlobalInstall(VecTrunc, 0x50A120);
}


constexpr float CAR_FOV_START_SPEED = 0.4f;
constexpr float CAR_FOV_FADE_MULT = 0.98f;

CVector& gVecLookBehind = *reinterpret_cast<CVector*>(0xB6F018);

// Camera constants
constexpr float BOAT_1STPERSON_LR_OFFSETZ = 0.2f;
constexpr float BOAT_1STPERSON_L_OFFSETX = 0.7f;
constexpr float BOAT_1STPERSON_R_OFFSETX = 0.3f;
constexpr float AIMWEAPON_STICK_SENS = 0.007f;
constexpr float AIMWEAPON_FREETARGET_SENS = 0.1f;
constexpr float AIMWEAPON_DRIVE_SENS_MULT = 0.25f;
constexpr float AIMWEAPON_DRIVE_CLOSE_ENOUGH = 0.17453f;
constexpr float AIMWEAPON_RIFLE1_ZOOM = 50.0f;
constexpr float AIMWEAPON_RIFLE2_ZOOM = 35.0f;

// Camera zoom offset arrays
constexpr float ZmOneAlphaOffset[5] = { 0.08f, 0.08f, 0.15f, 0.08f, 0.08f };
constexpr float ZmTwoAlphaOffset[5] = { 0.07f, 0.08f, 0.3f, 0.08f, 0.08f };
constexpr float ZmThreeAlphaOffset[5] = { 0.055f, 0.05f, 0.15f, 0.06f, 0.08f };

constexpr float fTestShiftHeliCamTarget = 0.6f;

// 0x50A0A0
float FTrunc(float n, int digits) {
    long double v2;
    long double v3; 
    double v5;

    v5 = digits + 1;
    v2 = std::pow(10.0, (digits + 1)) * n;
    
    if (n >= 0.0) {
        v3 = v2 + 5.0;
    } else {
        v3 = v2 - 5.0;
    }
    
    std::modf(v3 * 0.1, &v5);
    return v5 / std::pow(10.0, digits);
}

// 0x50A120
void VecTrunc(CVector* pV, int digits = 4) {
    pV->x = FTrunc(pV->x, digits);
    pV->y = FTrunc(pV->y, digits);
    pV->z = FTrunc(pV->z, digits);
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
    m_fBeta = 0.0f;
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
        m_nTargetHistoryTime[i] = 0;
        m_aTargetHistoryPos[i] = CVector{};
    }
    m_nCurrentHistoryPoints = 0;
    gPlayerPedVisible = true;
    gbCineyCamMessageDisplayed = 2; // TODO: enum
    gDirectionIsLooking = 3; // TODO: enum
    gLastCamMode = (eCamMode)-1;
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
        } else if (TheCamera.m_pTargetEntity->IsVehicle()) {
            pHandyCarPointer = (CVehicle *)TheCamera.m_pTargetEntity;
            if (pHandyCarPointer->m_pDriver && pHandyCarPointer->m_pDriver->IsPlayer()) {
                pHandyPedPointer = pHandyCarPointer->m_pDriver;
                pHandyPedPointer->PlayerPosition.GetBonePosition(BONE_SPINE1, true);
            } else {
                pHandyPedPointer = nullptr;
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
                            CTaskSimpleArrestPed *pTaskArrest = (CTaskSimpleArrestPed *)pNearbyPed->GetPedIntelligence()->FindTaskActiveByType(eTaskType::TASK_SIMPLE_ARREST_PED);
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
                            CTaskSimpleArrestPed *pTaskArrest = (CTaskSimpleArrestPed *)pNearbyPed->GetPedIntelligence()->FindTaskActiveByType(eTaskType::TASK_SIMPLE_ARREST_PED);
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
        } else if (TheCamera.m_pTargetEntity->IsVehicle()) {
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
void CCam::Process_FollowCar_SA(const CVector &ThisCamsTarget, float TargetOrientation, float SpeedVar, float SpeedVarDesired, bool bScriptSetAngles) {

    struct CamFollowPedData {
        float fTargetOffsetZ;
        float fBaseCamDist;
        float fBaseCamZ;
        float fMinDist;
        float fMinFollowDist;
        float fDiffAlphaRate;
        float fDiffAlphaCap;
        float fDiffAlphaSwing;
        float fDiffBetaRate;
        float fDiffBetaCap;
        float fDiffBetaSwing;
        float fDiffBetaSwingCap;
        float fStickMult;
        float fUpLimit;
        float fDownLimit;
    };

    enum {
        FOLLOW_CAR_INCAR = 0,
        FOLLOW_CAR_ONBIKE,
        FOLLOW_CAR_INHELI,
        FOLLOW_CAR_INPLANE,
        FOLLOW_CAR_INBOAT,
        FOLLOW_CAR_RCCAR,
        FOLLOW_CAR_RCHELI,
        FOLLOW_CAR_MAX
    };
    
    CamFollowPedData CARCAM_SET[FOLLOW_CAR_MAX] = {
        {1.3f,	1.0f,	0.40f,	10.0f,	15.0f,	0.5f, 	1.0f, 	1.0f,	0.85f,	0.2f,	0.075,	0.05f,		0.80f,	DegreesToRadians(45.0f), DegreesToRadians(89.0f)},
        {1.1f,	1.0f,	0.10f,	10.0f,	11.0f,	0.5f, 	1.0f, 	1.0f,	0.85f,	0.2f,	0.075f,	0.05f,		0.75f,	DegreesToRadians(45.0f), DegreesToRadians(89.0f)},
        {1.1f,	1.0f,	0.20f,	10.0f,	15.0f,	0.05f, 	0.05f, 	0.0f,	0.9f,	0.05f,	0.01f,	0.05f,		1.0f,	DegreesToRadians(10.0f), DegreesToRadians(70.0f)},
        {1.1f,	3.5f,	0.20f,	10.0f,	25.0f,	0.5f, 	1.0f, 	1.0f,	0.75f,	0.1f,	0.005f,	0.20f,		1.0f,	DegreesToRadians(89.0f), DegreesToRadians(89.0f)},
        {1.3f,	1.0f,	0.40f,	10.0f,	15.0f,	0.5f, 	1.0f, 	0.0f,	0.9f,	0.05f,	0.005f,	0.05f,		1.0f,	DegreesToRadians(20.0f), DegreesToRadians(70.0f)},
        {1.1f,	1.0f,	0.20f,	10.0f,	5.0f,	0.5f, 	1.0f, 	1.0f,	0.75f,	0.1f,	0.005f,	0.20f,		1.0f,	DegreesToRadians(45.0f), DegreesToRadians(89.0f)},
        {1.1f,	1.0f,	0.20f,	10.0f,	5.0f,	0.5f, 	1.0f, 	1.0f,	0.75f,	0.1f,	0.005f,	0.20f,		1.0f,	DegreesToRadians(20.0f), DegreesToRadians(70.0f)}
    };

    if (!m_pCamTargetEntity->IsVehicle()) {
        return;
    }

    CVehicle *pTargetVehicle = m_pCamTargetEntity->AsVehicle();
    CVector cameraTargetPos = ThisCamsTarget;
    CPad *pPlayerPad = CPad::GetPad(0);
    if (pTargetVehicle->m_pDriver && pTargetVehicle->m_pDriver->m_nPedType == PED_TYPE_PLAYER2) {
        pPlayerPad = CPad::GetPad(1);
    }

    int32 vehicleCamType = FOLLOW_CAR_INCAR;

    TheCamera.ApplyVehicleCameraTweaks(pTargetVehicle);

    if (pTargetVehicle->GetModelIndex() == MODEL_RCBANDIT || pTargetVehicle->GetModelIndex() == MODEL_RCBARON || pTargetVehicle->GetModelIndex() == MODEL_RCTIGER || pTargetVehicle->GetModelIndex() == MODEL_RCCAM) {
        vehicleCamType = FOLLOW_CAR_RCCAR;
    } else if (pTargetVehicle->GetModelIndex() == MODEL_RCRAIDER || pTargetVehicle->GetModelIndex() == MODEL_RCGOBLIN) {
        vehicleCamType = FOLLOW_CAR_RCHELI;
    } else if (pTargetVehicle->IsBike() || pTargetVehicle->IsSubQuad()) {
        vehicleCamType = FOLLOW_CAR_ONBIKE;
    } else if (pTargetVehicle->IsSubHeli()) {
        vehicleCamType = FOLLOW_CAR_INHELI;
    } else if (pTargetVehicle->IsSubPlane()) {
        if (pTargetVehicle->GetModelIndex() == MODEL_HYDRA && pTargetVehicle->AsAutomobile()->m_wMiscComponentAngle >= CPlane::HARRIER_NOZZLE_SWITCH_LIMIT) {
            vehicleCamType = FOLLOW_CAR_INHELI;
        } else if (pTargetVehicle->GetModelIndex() == MODEL_VORTEX) {
            vehicleCamType = FOLLOW_CAR_INCAR;
        } else {
            vehicleCamType = FOLLOW_CAR_INPLANE;
        }
    } else if (pTargetVehicle->IsSubBoat()) {
        vehicleCamType = FOLLOW_CAR_INBOAT;
    }

    float cameraDistanceFromTarget = TheCamera.m_fCarZoomSmoothed + CARCAM_SET[vehicleCamType].fBaseCamDist;

    int vehicleTypeArrayIndex = 0;
    TheCamera.GetArrPosForVehicleType(pTargetVehicle->GetVehicleAppearance(), vehicleTypeArrayIndex);

    float cameraPitchAngle = 0.0f;
    if (pTargetVehicle->GetStatus() != STATUS_REMOTE_CONTROLLED) {
        switch (TheCamera.m_nCarZoom) {
        case ZOOM_ONE:
            cameraPitchAngle += ZmOneAlphaOffset[vehicleTypeArrayIndex];
            break;
        case ZOOM_TWO:
            cameraPitchAngle += ZmTwoAlphaOffset[vehicleTypeArrayIndex];
            break;
        case ZOOM_THREE:
            cameraPitchAngle += ZmThreeAlphaOffset[vehicleTypeArrayIndex];
            break;
        }
    } else {
        cameraPitchAngle += ZmTwoAlphaOffset[vehicleTypeArrayIndex];
    }

    float vehicleHeight = pTargetVehicle->GetColModel()->GetBoundBoxMax().z;
    float vehicleLength = 2.0f * std::abs(pTargetVehicle->GetColModel()->GetBoundBoxMin().y);

    static float sPositionBlendFactor = 0.0f;
    static float sPositionBlendRate = 0.02f;
    if (pTargetVehicle->m_pVehicleBeingTowed) {
        if (sPositionBlendFactor < 1.0f) {
            sPositionBlendFactor = std::min(1.0f, sPositionBlendFactor + sPositionBlendRate * CTimer::GetTimeStep());
        }

        static float TRAILER_CAMDIST_MULT = 0.5f;
        vehicleLength += sPositionBlendFactor * TRAILER_CAMDIST_MULT * (pTargetVehicle->m_pVehicleBeingTowed->GetColModel()->GetBoundBoxMax() - pTargetVehicle->m_pVehicleBeingTowed->GetColModel()->GetBoundBoxMin()).Magnitude();
        vehicleHeight += sPositionBlendFactor * (std::max(vehicleHeight, pTargetVehicle->m_pVehicleBeingTowed->GetColModel()->GetBoundBoxMax().z) - vehicleHeight);

        cameraTargetPos = (1.0f - 0.5f * sPositionBlendFactor) * cameraTargetPos + 0.5f * sPositionBlendFactor * pTargetVehicle->m_pVehicleBeingTowed->GetPosition();
    } else if (pTargetVehicle->IsSubBike() || pTargetVehicle->IsSubQuad()) {
        if (pTargetVehicle->m_apPassengers[0] != nullptr) {
            if (sPositionBlendFactor < 1.0f) {
                sPositionBlendFactor = std::min(1.0f, sPositionBlendFactor + sPositionBlendRate * CTimer::GetTimeStep());
            }
        } else if (sPositionBlendFactor > 0.0f) {
            sPositionBlendFactor = std::max(0.0f, sPositionBlendFactor - sPositionBlendRate * CTimer::GetTimeStep());
        }

        static float BIKE_WITH_PASSENGER_HEIGHT_ADD = 0.4f;
        vehicleHeight += sPositionBlendFactor * BIKE_WITH_PASSENGER_HEIGHT_ADD;
    } else {
        sPositionBlendFactor = 0.0f;
    }

    vehicleLength *= TheCamera.m_VehicleTweakLenMod;

    cameraDistanceFromTarget += vehicleLength;
    float minCameraDistance = CARCAM_SET[vehicleCamType].fMinDist * vehicleLength;

    if (pTargetVehicle->GetVehicleAppearance() == VEHICLE_APPEARANCE_HELI && pTargetVehicle->GetStatus() != STATUS_REMOTE_CONTROLLED) {
        cameraTargetPos += pTargetVehicle->GetMatrix().GetUp() * fTestShiftHeliCamTarget * vehicleHeight;
    } else {
        float targetZOffset = vehicleHeight * CARCAM_SET[vehicleCamType].fTargetOffsetZ - CARCAM_SET[vehicleCamType].fBaseCamZ;
        if (targetZOffset > 0.0f) {
            cameraTargetPos.z += targetZOffset;
            cameraDistanceFromTarget += targetZOffset;

            static float TEST_CAM_ALPHA_RAISE_MULT = 0.3f;
            cameraPitchAngle += TEST_CAM_ALPHA_RAISE_MULT * targetZOffset / cameraDistanceFromTarget;
        }
    }

    cameraTargetPos.z *= TheCamera.m_VehicleTweakTargetZMod;
    cameraPitchAngle += TheCamera.m_VehicleTweakPitchMod;

    float minFollowDistance = CARCAM_SET[vehicleCamType].fMinFollowDist;
    if (TheCamera.m_nCarZoom == ZOOM_ONE && (vehicleCamType == FOLLOW_CAR_INCAR || vehicleCamType == FOLLOW_CAR_ONBIKE)) {
        minFollowDistance *= 0.65f;
    }
    float followDistance = std::max(cameraDistanceFromTarget, minFollowDistance);

    m_fCaMaxDistance = cameraDistanceFromTarget;
    m_fCaMinDistance = 3.5f;

    if (m_bResetStatics) {
        m_fFOV = 70.f;
    } else {
        if ((pTargetVehicle->IsSubAutomobile() || pTargetVehicle->IsSubBike()) &&
            DotProduct(pTargetVehicle->GetMoveSpeed(), pTargetVehicle->GetMatrix().GetForward()) > CAR_FOV_START_SPEED) {
            m_fFOV += (DotProduct(pTargetVehicle->GetMoveSpeed(), pTargetVehicle->GetMatrix().GetForward()) - CAR_FOV_START_SPEED) * CTimer::GetTimeStep();
        }

        if (m_fFOV > 70.0f) {
            m_fFOV = 70.0f + (m_fFOV - 70.0f) * std::pow(CAR_FOV_FADE_MULT, CTimer::GetTimeStep());
        }

        if (m_fFOV > 100.0f) {
            m_fFOV = 100.0f;
        } else if (m_fFOV < 70.0f) {
            m_fFOV = 70.0f;
        }
    }

    if (m_bResetStatics || TheCamera.m_bCamDirectlyBehind || TheCamera.m_bCamDirectlyInFront) {
        m_bResetStatics = false;
        m_bRotating = false;
        m_bCollisionChecksOn = true;
        TheCamera.m_bResetOldMatrix = true;

        if (!TheCamera.m_bJustCameOutOfGarage && !bScriptSetAngles) {
            m_fAlpha = 0.0f;
            m_fBeta = pTargetVehicle->GetHeading() - HALF_PI;
            if (TheCamera.m_bCamDirectlyInFront) {
                m_fBeta += PI;
            }
        }
        m_fBetaSpeed = 0.0f;
        m_fAlphaSpeed = 0.0f;
        m_fDistance = 1000.0f;

        m_vecFront = CVector(-std::cos(m_fBeta) * std::cos(m_fAlpha), -std::sin(m_fBeta) * std::cos(m_fAlpha), std::sin(m_fAlpha));

        m_aTargetHistoryPos[0] = cameraTargetPos - followDistance * m_vecFront;
        m_nTargetHistoryTime[0] = CTimer::GetTimeInMS();
        m_aTargetHistoryPos[1] = cameraTargetPos - cameraDistanceFromTarget * m_vecFront;
        m_nCurrentHistoryPoints = 0;

        if (!TheCamera.m_bJustCameOutOfGarage && !bScriptSetAngles) {
            m_fAlpha = -cameraPitchAngle;
        }
    }

    m_vecFront = cameraTargetPos - m_aTargetHistoryPos[0];
    m_vecFront.Normalise();
    float distanceToTarget = (cameraTargetPos - m_aTargetHistoryPos[1]).Magnitude();

    float targetBeta, betaDiffMult, betaDiffCap;
    float targetBetaAngle = std::atan2(-m_vecFront.x, m_vecFront.y) - HALF_PI;
    if (targetBetaAngle < -PI) {
        targetBetaAngle += TWO_PI;
    }

    float vehicleHeadingBeta = 0.0f;
    if (pTargetVehicle->GetMoveSpeed().Magnitude2D() > 0.02f) {
        vehicleHeadingBeta = std::atan2(-pTargetVehicle->GetMoveSpeed().x, pTargetVehicle->GetMoveSpeed().y) - HALF_PI;
    } else {
        vehicleHeadingBeta = targetBetaAngle;
    }

    if (vehicleHeadingBeta > targetBetaAngle + PI) {
        vehicleHeadingBeta -= TWO_PI;
    } else if (vehicleHeadingBeta < targetBetaAngle - PI) {
        vehicleHeadingBeta += TWO_PI;
    }

    {
        betaDiffMult = CARCAM_SET[vehicleCamType].fDiffBetaSwing * CTimer::GetTimeStep();
        betaDiffCap = CARCAM_SET[vehicleCamType].fDiffBetaSwingCap * CTimer::GetTimeStep();

        float altSpeed = DotProduct(m_vecFront, pTargetVehicle->GetMoveSpeed());
        altSpeed = (pTargetVehicle->GetMoveSpeed() - altSpeed * m_vecFront).Magnitude();
        betaDiffMult = std::min(1.0f, altSpeed * betaDiffMult);

        targetBeta = betaDiffMult * (vehicleHeadingBeta - targetBetaAngle);
        if (targetBeta > betaDiffCap) {
            targetBeta = betaDiffCap;
        } else if (targetBeta < -betaDiffCap) {
            targetBeta = -betaDiffCap;
        }
    }

    targetBetaAngle += targetBeta;
    if (targetBetaAngle > m_fBeta + PI) {
        targetBetaAngle -= TWO_PI;
    } else if (targetBetaAngle < m_fBeta - PI) {
        targetBetaAngle += TWO_PI;
    }

    float camControlBetaSpeed = (targetBetaAngle - m_fBeta) / std::max(1.0f, CTimer::GetTimeStep());

    float targetAlphaAngle = std::asin(std::max(-1.0f, std::min(1.0f, m_vecFront.z)));

    if (distanceToTarget < cameraDistanceFromTarget && cameraDistanceFromTarget > minCameraDistance) {
        cameraDistanceFromTarget = std::max(minCameraDistance, distanceToTarget);
    }

    float alphaUpLimit = CARCAM_SET[vehicleCamType].fUpLimit;
    if (pTargetVehicle->GetMoveSpeed().SquaredMagnitude() < 0.4f && !(pTargetVehicle->IsBike() && ((CBike *)pTargetVehicle)->m_nNoOfContactWheels < 4) &&
        !(pTargetVehicle->IsSubHeli()) && !(pTargetVehicle->IsSubPlane() && ((CAutomobile *)pTargetVehicle)->m_nNoOfContactWheels == 0)) {
        CVector vecTempRight = CrossProduct(pTargetVehicle->GetMatrix().GetForward(), CVector(0.0f, 0.0f, 1.0f));
        vecTempRight.Normalise();
        CVector vecTempUp = CrossProduct(vecTempRight, pTargetVehicle->GetMatrix().GetForward());
        vecTempUp.Normalise();
        if (DotProduct(m_vecFront, vecTempUp) > 0.0f) {
            CColModel *vehCol = pTargetVehicle->GetColModel();
            float vehicleHeightAboveRoad = pTargetVehicle->GetHeightAboveRoad();
            vehicleHeightAboveRoad += cameraTargetPos.z - pTargetVehicle->GetPosition().z;
            float cornerAngle = std::atan2(vehCol->GetBoundBoxMax().x, -vehCol->GetBoundBoxMin().y);

            // difference between camera and vehicle's heading
            float betaDiff = std::abs(std::sin(m_fBeta - (pTargetVehicle->GetHeading() - HALF_PI)));
            betaDiff = std::asin(betaDiff);

            static float STICK_DOWN_LENGTH_ADD = 1.5f;
            static float STICK_DOWN_WIDTH_ADD = 1.2f;
            static float STICK_DOWN_DIST_LIMIT_MULT = 1.2f;
            float vehicleDist;
            if (betaDiff > cornerAngle) {
                vehicleDist = (vehCol->GetBoundBoxMax().x + STICK_DOWN_WIDTH_ADD) / std::cos(std::max(0.0f, HALF_PI - betaDiff));
            } else {
                vehicleDist = (-vehCol->GetBoundBoxMin().y + STICK_DOWN_LENGTH_ADD) / std::cos(betaDiff);
            }

            vehicleDist *= STICK_DOWN_DIST_LIMIT_MULT;
            float angleLimit = std::atan2(vehicleHeightAboveRoad, vehicleDist);

            alphaUpLimit = angleLimit;

            alphaUpLimit += std::cos(m_fBeta - (pTargetVehicle->GetHeading() - HALF_PI)) * std::atan2(pTargetVehicle->GetMatrix().GetForward().z, pTargetVehicle->GetMatrix().GetForward().Magnitude2D());

            if (pTargetVehicle->IsAutomobile() && ((CAutomobile *)pTargetVehicle)->m_nNoOfContactWheels > 1 &&
                std::abs(DotProduct(pTargetVehicle->GetTurnSpeed(), pTargetVehicle->GetMatrix().GetForward())) < 0.05f) {
                alphaUpLimit += std::cos(m_fBeta - (pTargetVehicle->GetHeading() - HALF_PI) + HALF_PI) * std::atan2(pTargetVehicle->GetMatrix().GetRight().z, pTargetVehicle->GetMatrix().GetRight().Magnitude2D());
            }
        }
    }

    targetAlphaAngle -= cameraPitchAngle;
    if (targetAlphaAngle > alphaUpLimit) {
        targetAlphaAngle = alphaUpLimit;
    } else if (targetAlphaAngle < -CARCAM_SET[vehicleCamType].fDownLimit) {
        targetAlphaAngle = -CARCAM_SET[vehicleCamType].fDownLimit;
    }

    betaDiffMult = std::pow(CARCAM_SET[vehicleCamType].fDiffAlphaRate, CTimer::GetTimeStep());
    betaDiffCap = CARCAM_SET[vehicleCamType].fDiffAlphaCap * CTimer::GetTimeStep();

    targetBeta = (1.0f - betaDiffMult) * (targetAlphaAngle - m_fAlpha);
    if (targetBeta > betaDiffCap) {
        targetBeta = betaDiffCap;
    } else if (targetBeta < -betaDiffCap) {
        targetBeta = -betaDiffCap;
    }

    float stickBetaOffset = -(pPlayerPad->AimWeaponLeftRight(nullptr));
    float stickAlphaOffset = pPlayerPad->AimWeaponUpDown(nullptr);

    if (TheCamera.m_bUseMouse3rdPerson) {
        stickAlphaOffset = 0.0f;
    }

    stickBetaOffset = AIMWEAPON_STICK_SENS * AIMWEAPON_STICK_SENS * std::abs(stickBetaOffset) * stickBetaOffset * (0.25f / 3.5f * (m_fFOV / 80.0f));
    stickAlphaOffset = AIMWEAPON_STICK_SENS * AIMWEAPON_STICK_SENS * std::abs(stickAlphaOffset) * stickAlphaOffset * (0.15f / 3.5f * (m_fFOV / 80.0f));

    bool fixAlphaAngle = true;
    if (pTargetVehicle->GetModelIndex() == MODEL_PACKER || pTargetVehicle->GetModelIndex() == MODEL_DOZER || pTargetVehicle->GetModelIndex() == MODEL_DUMPER || pTargetVehicle->GetModelIndex() == MODEL_CEMENT ||
        pTargetVehicle->GetModelIndex() == MODEL_ANDROM || pTargetVehicle->GetModelIndex() == MODEL_HYDRA || pTargetVehicle->GetModelIndex() == MODEL_TOWTRUCK || pTargetVehicle->GetModelIndex() == MODEL_FORKLIFT ||
        pTargetVehicle->GetModelIndex() == MODEL_TRACTOR) {
        stickAlphaOffset = 0.0f;
    } else if (pTargetVehicle->GetModelIndex() == MODEL_RCTIGER ) { //|| (pTargetVehicle->IsAutomobile() && ((CAutomobile *)pTargetVehicle)->hFlagsLocal & HF_HYDRAULICS_INSTALLED)) {
        stickAlphaOffset = 0.0f;
        stickBetaOffset = 0.0f;
    } else {
        fixAlphaAngle = false;
    }

    if (gDirectionIsLooking != LOOKING_FORWARD) {
        stickAlphaOffset = 0.0f;
        stickBetaOffset = 0.0f;
    }

    /*
    if (vehicleCamType == FOLLOW_CAR_INCAR && std::abs(pPlayerPad->GetSteeringUpDown()) > 120.0f && pTargetVehicle->m_pDriver && pTargetVehicle->m_pDriver->GetPedIntelligence()->GetTaskActive() &&
        pTargetVehicle->m_pDriver->GetPedIntelligence()->GetTaskActive()->GetTaskType() != eTaskType::TASK_COMPLEX_LEAVE_CAR) {
        float extraLeftStickMod = pPlayerPad->GetSteeringUpDown();
        extraLeftStickMod = 0.5f * AIMWEAPON_STICK_SENS * AIMWEAPON_STICK_SENS * std::abs(extraLeftStickMod) * extraLeftStickMod * (0.15f / 3.5f * (m_fFOV / 80.0f));
        stickAlphaOffset += extraLeftStickMod;
    }*/

    if (stickAlphaOffset > 0.0f) {
        stickAlphaOffset *= 0.5f;
    }

    /*
    bool usingMouse = false;
    if (TheCamera.m_bUseMouse3rdPerson && pPlayerPad->DisablePlayerControls == 0) {
        float mouseStickY = pPlayerPad->GetAmountMouseMoved().y * 2.0f;
        float mouseStickX = -pPlayerPad->GetAmountMouseMoved().x * 2.0f;

        static float MOUSE_INPUT_COUNTER = 0.0f;

        bool mouseSteeringOn = true;
        if (pTargetVehicle->IsSubPlane() || pTargetVehicle->IsSubHeli()) {
            mouseSteeringOn = CVehicle::m_bEnableMouseFlying;
        } else {
            mouseSteeringOn = CVehicle::m_bEnableMouseSteering;
        }

        if ((mouseStickX != 0.0f || mouseStickY != 0.0f) && (pPlayerPad->GetVehicleMouseLook() || !mouseSteeringOn)) {
            stickAlphaOffset = TheCamera.m_fMouseAccelHorzntl * mouseStickY * (m_fFOV / 80.0f);
            stickBetaOffset = TheCamera.m_fMouseAccelHorzntl * mouseStickX * (m_fFOV / 80.0f);
            m_fAlphaSpeed = m_fBetaSpeed = 0.0f;
            targetAlphaAngle = m_fAlpha;

            static float sMouseInputBufferTime = 1.0f;

            MOUSE_INPUT_COUNTER = FRAMES_PER_SECOND * sMouseInputBufferTime;
            usingMouse = true;
        } else if (MOUSE_INPUT_COUNTER > 0.0f) {
            stickAlphaOffset = 0.0f;
            stickBetaOffset = 0.0f;
            m_fAlphaSpeed = m_fBetaSpeed = 0.0f;
            targetAlphaAngle = m_fAlpha;

            MOUSE_INPUT_COUNTER = std::max(0.0f, MOUSE_INPUT_COUNTER - CTimer::GetTimeStep());
            usingMouse = true;
        }
    }

    if (pTargetVehicle->m_apPassengers[0] && pTargetVehicle->m_apPassengers[0]->GetPedIntelligence()->GetTaskActive() &&
        pTargetVehicle->m_apPassengers[0]->GetPedIntelligence()->GetTaskActive()->GetTaskType() == eTaskType::TASK_COMPLEX_PROSTITUTE_SOLICIT) {
        if (((CTaskComplexProstituteSolicit *)pTargetVehicle->m_apPassengers[0]->GetPedIntelligence()->GetTaskActive())->IsInSexCamMode()) {
            static float PROSTITUTE_CAM_ALPHA_ANGLE = 0.1f;
            static float PROSTITUTE_CAM_ALPHA_RATE = 0.0035f;
            if (m_fAlpha < alphaUpLimit - PROSTITUTE_CAM_ALPHA_ANGLE) {
                stickAlphaOffset = PROSTITUTE_CAM_ALPHA_RATE * CTimer::GetTimeStep();
            } else {
                stickAlphaOffset = 0.0f;
            }
        }
    }*/

    if (fixAlphaAngle) {
        static float gStickAlphaFix = 0.05f;
        static bool gAcquiredAlpha = false;

        if (gLastCamMode != MODE_CAM_ON_A_STRING) {
            gAcquiredAlpha = false;
        }

        if (!gAcquiredAlpha && std::abs(m_fAlpha + cameraPitchAngle) > 0.05f) {
            stickAlphaOffset = (-cameraPitchAngle - m_fAlpha) * gStickAlphaFix;
        } else {
            gAcquiredAlpha = true;
        }
    }

    stickAlphaOffset *= CARCAM_SET[vehicleCamType].fStickMult;
    stickBetaOffset *= CARCAM_SET[vehicleCamType].fStickMult;

    betaDiffMult = std::pow(CARCAM_SET[vehicleCamType].fDiffBetaRate, CTimer::GetTimeStep());
    betaDiffCap = CARCAM_SET[vehicleCamType].fDiffBetaCap;

    camControlBetaSpeed += stickBetaOffset;

    if (camControlBetaSpeed > betaDiffCap) {
        camControlBetaSpeed = betaDiffCap;
    } else if (camControlBetaSpeed < -betaDiffCap) {
        camControlBetaSpeed = -betaDiffCap;
    }

    m_fBetaSpeed = betaDiffMult * m_fBetaSpeed + (1.0f - betaDiffMult) * camControlBetaSpeed;

    static float gBetaSpeedTol = 0.0001f;
    if (std::abs(m_fBetaSpeed) < gBetaSpeedTol) {
        m_fBetaSpeed = 0.0f;
    }

    /*if (usingMouse) {
        m_fBeta += stickBetaOffset;
    } else {*/
        m_fBeta += m_fBetaSpeed * CTimer::GetTimeStep();
    //}

    if (TheCamera.m_bJustCameOutOfGarage) {
        m_fBeta = CGeneral::GetATanOfXY(m_vecFront.x, m_vecFront.y) + PI;
    }

    if (m_fBeta > PI) {
        m_fBeta -= TWO_PI;
    } else if (m_fBeta < -PI) {
        m_fBeta += TWO_PI;
    }

    if (vehicleCamType <= FOLLOW_CAR_ONBIKE && targetAlphaAngle < m_fAlpha && distanceToTarget >= cameraDistanceFromTarget) {
        int nWheelsOnGround = 0;
        if (pTargetVehicle->IsAutomobile()) {
            nWheelsOnGround = ((CAutomobile *)pTargetVehicle)->m_nNoOfContactWheels;
        } else if (pTargetVehicle->IsSubBike()) {
            nWheelsOnGround = ((CBike*)pTargetVehicle)->m_nNoOfContactWheels;
        }

        if (nWheelsOnGround > 1) {
            static float sCrestMultipler = 0.075f;
            stickAlphaOffset += (targetAlphaAngle - m_fAlpha) * sCrestMultipler;
        }
    }

    m_fAlphaSpeed = betaDiffMult * m_fAlphaSpeed + (1.0f - betaDiffMult) * stickAlphaOffset;
    if (stickAlphaOffset > 0.0f) {
        betaDiffCap *= 0.5f;
    }

    if (m_fAlphaSpeed > betaDiffCap) {
        m_fAlphaSpeed = betaDiffCap;
    } else if (m_fAlphaSpeed < -betaDiffCap) {
        m_fAlphaSpeed = -betaDiffCap;
    }

    static float gAlphaSpeedTol = 0.0001f;
    if (std::abs(m_fAlphaSpeed) < gAlphaSpeedTol) {
        m_fAlphaSpeed = 0.0f;
    }

    /*if (usingMouse) {
        targetAlphaAngle += stickAlphaOffset;
        m_fAlpha += stickAlphaOffset;
    } else {*/
        targetAlphaAngle += m_fAlphaSpeed * CTimer::GetTimeStep();
        m_fAlpha += targetBeta;
    //}

    if (m_fAlpha > alphaUpLimit) {
        m_fAlpha = alphaUpLimit;
        m_fAlphaSpeed = 0.0f;
    } else if (m_fAlpha < -CARCAM_SET[vehicleCamType].fDownLimit) {
        m_fAlpha = -CARCAM_SET[vehicleCamType].fDownLimit;
        m_fAlphaSpeed = 0.0f;
    }

    static float gOldAlpha = -9999.0f;
    static float gOldBeta = -9999.0f;
    static float gAlphaTol = 0.0001f;
    static float gBetaTol = 0.0001f;

    float dA = std::abs(gOldAlpha - m_fAlpha);
    if (dA < gAlphaTol) {
        m_fAlpha = gOldAlpha;
    }
    gOldAlpha = m_fAlpha;

    float dB = std::abs(gOldBeta - m_fBeta);
    if (dB < gBetaTol) {
        m_fBeta = gOldBeta;
    }
    gOldBeta = m_fBeta;

    m_vecFront = CVector(-std::cos(m_fBeta) * std::cos(m_fAlpha), -std::sin(m_fBeta) * std::cos(m_fAlpha), std::sin(m_fAlpha));

    VecTrunc(&m_vecSource, 4);

    GetVectorsReadyForRW();

    TheCamera.m_bCamDirectlyBehind = false;
    TheCamera.m_bCamDirectlyInFront = false;

    m_vecSource = cameraTargetPos - m_vecFront * cameraDistanceFromTarget;
    m_vecTargetCoorsForFudgeInter = cameraTargetPos;

    targetAlphaAngle += cameraPitchAngle;
    m_aTargetHistoryPos[2] = m_aTargetHistoryPos[0];
    m_aTargetHistoryPos[0] = cameraTargetPos - followDistance * CVector(-std::cos(m_fBeta) * std::cos(targetAlphaAngle), -std::sin(m_fBeta) * std::cos(targetAlphaAngle), std::sin(targetAlphaAngle));
    m_aTargetHistoryPos[1] = cameraTargetPos - cameraDistanceFromTarget * CVector(-std::cos(m_fBeta) * std::cos(targetAlphaAngle), -std::sin(m_fBeta) * std::cos(targetAlphaAngle), std::sin(targetAlphaAngle));

    TheCamera.SetColVarsVehicle((eVehicleType)vehicleCamType, TheCamera.m_nCarZoom);

    if (gDirectionIsLooking == LOOKING_FORWARD) {
        CWorld::pIgnoreEntity = pTargetVehicle;
        TheCamera.CameraGenericModeSpecialCases(nullptr);
        TheCamera.CameraVehicleModeSpecialCases(pTargetVehicle);

        // static bool gTopSphereCastTest;
        // if (pTargetVehicle->m_pHandlingData->m_bIsBig) {
        //     gTopSphereCastTest = true; // nop
        // }

        TheCamera.CameraColDetAndReact(&m_vecSource, &cameraTargetPos);

        TheCamera.ImproveNearClip(pTargetVehicle, nullptr, &m_vecSource, &cameraTargetPos);

        CWorld::pIgnoreEntity = nullptr;

        VecTrunc(&m_vecSource, 4);
    }

    TheCamera.m_bCamDirectlyBehind = false;
    TheCamera.m_bCamDirectlyInFront = false;

    VecTrunc(&m_vecSource);

    GetVectorsReadyForRW();

    gVecLookBehind = cameraTargetPos;
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
