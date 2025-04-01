#include "StdInc.h"

#include "Cam.h"
#include "TimeCycle.h"
#include "Camera.h"
#include "Shadows.h"
#include "IdleCam.h"
#include "InterestingEvents.h"
#include "ModelIndices.h"
#include "HandShaker.h"
#include "TaskComplexProstituteSolicit.h "

bool& gbFirstPersonRunThisFrame = StaticRef<bool>(0xB6EC20);
uint32& gLastFrameProcessedDWCineyCam = StaticRef<uint32>(0x8CCB9C);

static inline std::array<bool, 9> gbExitCam = StaticRef<std::array<bool, 9>>(0xB6EC5C);

static inline CVector& DWCineyCamLastPos = StaticRef<CVector>(0xB6FE8C);
static inline CVector& DWCineyCamLastUp = StaticRef<CVector>(0xB6FE98);
static inline CVector& DWCineyCamLastRight = StaticRef<CVector>(0xB6FEA4);
static inline CVector& DWCineyCamLastFwd = StaticRef<CVector>(0xB6FEB0);

static inline float& DWCineyCamLastNearClip = StaticRef<float>(0xB6EC08);
static inline float& DWCineyCamLastFov = StaticRef<float>(0xB6EC0C);

void CCam::InjectHooks() {
    RH_ScopedClass(CCam);
    RH_ScopedCategory("Camera");

    RH_ScopedInstall(Constructor, 0x517730);
    RH_ScopedInstall(Init, 0x50E490);
    RH_ScopedInstall(CacheLastSettingsDWCineyCam, 0x50D7A0);
    RH_ScopedInstall(DoCamBump, 0x50CB30);
    RH_ScopedInstall(Finalise_DW_CineyCams, 0x50DD70);
    RH_ScopedInstall(GetCoreDataForDWCineyCamMode, 0x517130);
    RH_ScopedInstall(GetLookFromLampPostPos, 0x5161A0, { .reversed = false });
    RH_ScopedInstall(GetVectorsReadyForRW, 0x509CE0);
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
    RH_ScopedInstall(Process_1stPerson, 0x517EA0);
    RH_ScopedInstall(Process_AimWeapon, 0x521500, { .reversed = false });
    RH_ScopedInstall(Process_AttachedCam, 0x512B10, { .reversed = false });
    RH_ScopedInstall(Process_Cam_TwoPlayer, 0x525E50, { .reversed = false });
    RH_ScopedInstall(Process_Cam_TwoPlayer_InCarAndShooting, 0x519810, { .reversed = false });
    RH_ScopedInstall(Process_Cam_TwoPlayer_Separate_Cars, 0x513510, { .reversed = false });
    RH_ScopedInstall(Process_Cam_TwoPlayer_Separate_Cars_TopDown, 0x513BE0);
    RH_ScopedInstall(Process_DW_BirdyCam, 0x51B850, { .reversed = false });
    RH_ScopedInstall(Process_DW_CamManCam, 0x51B120, { .reversed = false });
    RH_ScopedInstall(Process_DW_HeliChaseCam, 0x51A740, { .reversed = false });
    RH_ScopedInstall(Process_DW_PlaneCam1, 0x51C760, { .reversed = false });
    RH_ScopedInstall(Process_DW_PlaneCam2, 0x51CC30, { .reversed = false });
    RH_ScopedInstall(Process_DW_PlaneCam3, 0x51D100, { .reversed = false });
    RH_ScopedInstall(Process_DW_PlaneSpotterCam, 0x51C250, { .reversed = false });
    RH_ScopedInstall(Process_Editor, 0x50F3F0);
    RH_ScopedInstall(Process_Fixed, 0x51D470);
    RH_ScopedInstall(Process_FlyBy, 0x5B25F0, { .reversed = false });
    RH_ScopedInstall(Process_FollowCar_SA, 0x5245B0);
    RH_ScopedInstall(Process_FollowPedWithMouse, 0x50F970, { .reversed = false });
    RH_ScopedInstall(Process_FollowPed_SA, 0x522D40, { .reversed = false });
    RH_ScopedInstall(Process_M16_1stPerson, 0x5105C0, { .reversed = false });
    RH_ScopedInstall(Process_Rocket, 0x511B50);
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
// High precision truncation function
// Clearly the person who made the function did not understand the difference between rounding and truncating.
// The legacy func must do truncation, not rounding, but actually it does rounding.
// Now that is fixed for trunc like it should be.
float FTrunc(float n, int digits) {
    const double factor = std::pow(10.0, digits);
    return std::trunc(n * factor) / factor;
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
    DWCineyCamLastUp       = m_vecUp;
    DWCineyCamLastFwd      = m_vecFront;
    DWCineyCamLastRight    = CrossProduct(m_vecFront, m_vecUp);
    DWCineyCamLastFov      = m_fFOV;
    DWCineyCamLastNearClip = RwCameraGetNearClipPlane(Scene.m_pRwCamera);
    DWCineyCamLastPos      = m_vecSource;
}

// 0x50CB30
void CCam::DoCamBump(float horizontal, float vertical) {
    m_fCamBumpedHorz = horizontal;
    m_fCamBumpedVert = vertical;
    m_nCamBumpedTime = CTimer::GetTimeInMS();
}

// 0x50DD70
void CCam::Finalise_DW_CineyCams(const CVector& src, const CVector& dest, float roll, float fov, float nearClip, float shakeDegree) {
    m_vecFront  = (src - dest).Normalized();
    m_vecSource = src;

    // What is this thing?
    {
        auto rightDir = CrossProduct(m_vecFront, { std::sin(roll), 0.0f, std::cos(roll) }).Normalized();
        m_vecUp       = CrossProduct(rightDir, m_vecFront);
        if (m_vecFront.x == 0.0f && m_vecFront.y == 0.0f) {
            m_vecFront.x = m_vecFront.y = 0.0001f;
        }
        rightDir = CrossProduct(m_vecFront, m_vecUp).Normalized();
        m_vecUp  = CrossProduct(rightDir, m_vecFront);
    }

    m_fFOV = fov;
    RwCameraSetNearClipPlane(Scene.m_pRwCamera, 0.4f); // meant to use nearClip here?
    CacheLastSettingsDWCineyCam();
    gLastFrameProcessedDWCineyCam = CTimer::GetFrameCounter();
    gHandShaker[0].Process(shakeDegree);

    m_vecFront = gHandShaker[0].m_resultMat.TransformVector(m_vecFront);

    {
        auto rightDir = CrossProduct(m_vecFront, { std::sin(roll), 0.0f, std::cos(roll) }).Normalized();
        m_vecUp       = CrossProduct(rightDir, m_vecFront);
        if (m_vecFront.x == 0.0f && m_vecFront.y == 0.0f) {
            m_vecFront.x = m_vecFront.y = 0.0001f;
        }
        rightDir = CrossProduct(m_vecFront, m_vecUp).Normalized();
        m_vecUp  = CrossProduct(rightDir, m_vecFront);
    }
}

// 0x517130
void CCam::GetCoreDataForDWCineyCamMode(
    CEntity*& entity,
    CVehicle*& vehicle,
    CVector& dest,
    CVector& src,
    CVector& targetUp,
    CVector& targetRight,
    CVector& targetFwd,
    CVector& targetVel,
    float& targetSpeed,
    CVector& targetAngVel,
    float& targetAngSpeed,
    CColSphere& colSphere
) {
    entity         = m_pCamTargetEntity;
    vehicle        = entity->AsVehicle();
    dest           = entity->GetPosition();
    src            = DWCineyCamLastPos;
    targetUp       = entity->GetUpVector();
    targetRight    = entity->GetRightVector();
    targetFwd      = entity->GetForwardVector();
    targetVel      = entity->AsPhysical()->GetMoveSpeed();
    targetSpeed    = targetVel.Magnitude();
    targetAngVel   = entity->AsPhysical()->GetTurnSpeed();
    targetAngSpeed = targetAngVel.Magnitude();

    colSphere.Set(
        entity->GetModelInfo()->GetColModel()->GetBoundRadius(),
        entity->GetBoundCentre(),
        eSurfaceType::SURFACE_DEFAULT
    );
}

// 0x5161A0
void CCam::GetLookFromLampPostPos(CEntity* target, CPed* cop, const CVector& vecTarget, const CVector& vecSource) {
    NOTSA_UNREACHABLE();
}

// 0x509CE0
void CCam::GetVectorsReadyForRW() {
    m_vecFront.Normalise();
    if (m_vecFront.x == 0.0f && m_vecFront.y == 0.0f) {
        m_vecFront.x = m_vecFront.y = 0.0001f;
    }
    const auto a = CrossProduct(m_vecFront, { 0.0f, 0.0f, 1.0f }).Normalized();
    m_vecUp = CrossProduct(a, m_vecFront);
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
bool CCam::IsTimeToExitThisDWCineyCamMode(int32 camId, const CVector& src, const CVector& dst, float t, bool lineOfSightCheck) {
    NOTSA_UNREACHABLE();
    return false;
}

// 0x509DF0
void CCam::KeepTrackOfTheSpeed(const CVector&, const CVector&, const CVector&, const float&, const float&, const float&) {
    NOTSA_UNREACHABLE();
}

// 0x520690
void CCam::LookBehind() {
    NOTSA_UNREACHABLE();
}

// 0x520E40
void CCam::LookRight(bool bLookRight) {
    NOTSA_UNREACHABLE();
}

// 0x50A4F0
void CCam::RotCamIfInFrontCar(const CVector&, float) {
    NOTSA_UNREACHABLE();
}

// 0x50A850
bool CCam::Using3rdPersonMouseCam() const {
    return CCamera::m_bUseMouse3rdPerson && m_nMode == MODE_FOLLOWPED;
}

// 0x509DC0
bool CCam::GetWeaponFirstPersonOn() {
return m_pCamTargetEntity && m_pCamTargetEntity->IsPed() && m_pCamTargetEntity->AsPed()->GetActiveWeapon().m_IsFirstPersonWeaponModeSelected;
}

// inlined -- alpha = vertical angle
void CCam::ClipAlpha() {
    m_fAlpha = std::clamp(
        m_fAlpha,
        DegreesToRadians(-85.5f),
        DegreesToRadians(+60.0f)
    );
}

// 0x509C50 -- beta = horizontal angle
void CCam::ClipBeta() {
    if (m_fBeta < DegreesToRadians(-180.0f)) {
        m_fBeta += DegreesToRadians(360.0f);
    } else {
        m_fBeta -= DegreesToRadians(360.0f);
    }
}

// 0x526FC0
void CCam::Process() {
    assert(0);
}

// 0x518500
void CCam::ProcessArrestCamOne() {
    NOTSA_UNREACHABLE();
}

// 0x519250
void CCam::ProcessPedsDeadBaby() {
    constexpr float DEADCAM_HEIGHT_START   = 2.0f;
    constexpr float DEADCAM_HEIGHT_RATE    = 0.04f;
    constexpr float DEADCAM_WAFT_AMPLITUDE = 2.0f;
    constexpr float DEADCAM_WAFT_RATE      = 600.0f;
    constexpr float DEADCAM_WAFT_TILT_AMP  = -0.35f;
    static float startTimeDWDeadCam = 0.0f;
    static float cameraRot          = 0.0f;

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
void CCam::Process_1rstPersonPedOnPC(const CVector& target, float orientation, float speedVar, float speedVarWanted) {
    static CVector& v3d_8CCC54   = StaticRef<CVector>(0x8CCC54);
    static bool&    byte_B6FFDC  = StaticRef<bool>(0xB6FFDC);
    static CVector& v3d_B6FFC4   = StaticRef<CVector>(0xB6FFC4);
    static CVector& v3d_B6FFD0   = StaticRef<CVector>(0xB6FFD0);

    if (m_nMode != MODE_SNIPER_RUNABOUT) {
        m_fFOV = 70.0f;
    }

    if (!m_pCamTargetEntity->m_pRwObject) {
        return;
    }

    if (!m_pCamTargetEntity->IsPed()) {
        m_bResetStatics = false;
        RwCameraSetNearClipPlane(Scene.m_pRwCamera, 0.05f);
        return;
    }

    const auto hier = GetAnimHierarchyFromSkinClump(m_pCamTargetEntity->m_pRwClump);
    const auto aIdx = RpHAnimIDGetIndex(hier, ConvertPedNode2BoneTag(2)); // todo: enum
    auto&      aMat = RpHAnimHierarchyGetMatrixArray(hier)[aIdx];
    auto*      targetPed = m_pCamTargetEntity->AsPed();

    CVector pointIn = v3d_8CCC54;
    RwV3dTransformPoint(&pointIn, &pointIn, &aMat);
    RwV3d v3dZero{ 0.0f };
    RwMatrixScale(&aMat, &v3dZero, rwCOMBINEPRECONCAT);

    if (m_bResetStatics) {
        // unnecessary entity ped check
        m_fAlpha = 0.0f;
        byte_B6FFDC      = false;
        v3d_B6FFD0.Reset();
        m_fBeta            = targetPed->m_fCurrentRotation + DegreesToRadians(90.0f);
        m_bCollisionChecksOn          = true;
        m_fInitialPlayerOrientation   = m_fBeta;
        m_vecBufferedPlayerBodyOffset = v3d_B6FFC4 = pointIn;
    }
    m_vecBufferedPlayerBodyOffset.y = pointIn.y;

    if (TheCamera.m_bHeadBob) {
        m_vecBufferedPlayerBodyOffset.x = lerp(
            pointIn.x,
            m_vecBufferedPlayerBodyOffset.x,
            TheCamera.m_fScriptPercentageInterToCatchUp
        );

        m_vecBufferedPlayerBodyOffset.z = lerp(
            pointIn.z,
            m_vecBufferedPlayerBodyOffset.z,
            TheCamera.m_fScriptPercentageInterToCatchUp
        );

        m_vecSource = targetPed->GetMatrix().TransformPoint(m_vecBufferedPlayerBodyOffset);
    } else {
        const auto targetFwd = targetPed->GetForward().Normalized();
        const auto mag       = (pointIn - v3d_B6FFC4).Magnitude2D();

        m_vecSource = targetFwd * mag * 1.23f + targetPed->GetPosition() + CVector{ 0.0f, 0.0f, 0.59f };
    }

    CVector spinePos{};
    targetPed->GetTransformedBonePosition(spinePos, BONE_SPINE1, true);

    const auto fov    = m_fFOV / 80.0f;
    const auto amountMouseMoved = CPad::GetPad(0)->NewMouseControllerState.GetAmountMouseMoved();

    if (amountMouseMoved.x != 0.0f || amountMouseMoved.y != 0.0f) {
        m_fBeta += -3.0f * amountMouseMoved.x * fov * CCamera::m_fMouseAccelHorzntl;
        m_fAlpha += +4.0f * amountMouseMoved.y * fov * CCamera::m_fMouseAccelVertical;
    } else {
        const auto hv = (float)-CPad::GetPad(0)->LookAroundLeftRight(targetPed);
        const auto vv = (float)CPad::GetPad(0)->LookAroundUpDown(targetPed);

        m_fBeta += sq(hv) / 10000.0f * fov / 17.5f * CTimer::GetTimeStep() * (hv < 0.0f ? -1.0f : 1.0f);
        m_fAlpha += sq(vv) / 22500.0f * fov / 14.0f * CTimer::GetTimeStep() * (vv < 0.0f ? -1.0f : 1.0f);
    }
    ClipBeta();
    ClipAlpha();

    if (const auto* a = targetPed->m_pAttachedTo; targetPed->IsPlayer() && a) {
        // enum?
        switch (targetPed->m_fTurretAngleA) {
        case 0u:
            m_fBeta -= a->GetHeading() + DegreesToRadians(90.0f);
            break;
        case 1u:
            m_fBeta -= a->GetHeading() + DegreesToRadians(180.0f);
            break;
        case 2u:
            m_fBeta -= a->GetHeading() + DegreesToRadians(-90.0f);
            break;
        case 3u:
            m_fBeta -= a->GetHeading();
            break;
        default:
            // NOTE(yukani): If this is fired, gimme a call. 0x50F0ED
            NOTSA_UNREACHABLE();
            break;
        }

        // ...
    }
}

// 0x517EA0
void CCam::Process_1stPerson(const CVector& target, float orientation, float speedVar, float speedVarWanted) {
    static float& s_LastWheelieTime = StaticRef<float>(0x8CCD14);
    // Making sure player doesn't see below ground when flipped.
    // Name is made up cuz I found it funny to name it like that.
    static float& s_GroundFaultProtection = StaticRef<float>(0xB7004C);

    gbFirstPersonRunThisFrame = true;

    m_fFOV = 70.0f;
    if (!m_pCamTargetEntity->m_pRwObject) {
        return;
    }

    if (m_bResetStatics) {
        m_fAlpha   = 0.0f;
        m_fBeta = [&] {
            if (m_pCamTargetEntity->IsPed()) {
                return m_pCamTargetEntity->AsPed()->m_fCurrentRotation + DegreesToRadians(90.0f);
            } else {
                return orientation;
            }
        }();
        m_fInitialPlayerOrientation = m_fBeta;

        s_GroundFaultProtection                 = 0.0f;
        TheCamera.m_fAvoidTheGeometryProbsTimer = 0.0f;
    }

    if (m_pCamTargetEntity->IsPed()) {
        m_bResetStatics = false;
        return;
    }

    const auto wheelieTime = static_cast<float>(CTimer::GetTimeInMS());
    if (s_LastWheelieTime > wheelieTime) {
        s_LastWheelieTime = 0.0f;
    }

    auto* targetVeh = m_pCamTargetEntity->AsVehicle();
    if (targetVeh->IsBike() && targetVeh->AsBike()->bikeFlags.bWheelieForCamera || TheCamera.m_fAvoidTheGeometryProbsTimer > 0.0f) {
        if (wheelieTime - s_LastWheelieTime >= 3000.0f) {
            s_LastWheelieTime = static_cast<float>(CTimer::GetTimeInMS());
        }

        const auto pad1 = CPad::GetPad();
        if (!pad1->NewState.LeftShoulder2 && !pad1->NewState.RightShoulder2) {
            auto* targetBike = targetVeh->AsBike();
            if (Process_WheelCam(target, orientation, speedVar, speedVarWanted)) {
                if (targetBike->bikeFlags.bWheelieForCamera) {
                    TheCamera.m_fAvoidTheGeometryProbsTimer = 50.0f;
                } else {
                    TheCamera.m_fAvoidTheGeometryProbsTimer -= CTimer::GetTimeStep();
                    targetBike->bikeFlags.bWheelieForCamera = true;
                }
                return;
            }
            TheCamera.m_fAvoidTheGeometryProbsTimer = 0.0f;
            targetBike->bikeFlags.bWheelieForCamera = false;

            s_LastWheelieTime = 0.0f;
        }
    }

    const auto& entityWorldMat = [&] {
        if (auto* t = targetVeh->AsBike(); t->IsBike()) {
            t->CalculateLeanMatrix();
            return t->m_mLeanMatrix;
        } else {
            return targetVeh->GetMatrix();
        }
    }();

    const auto dummyPos = [&] {
        const auto* vehStruct = targetVeh->GetVehicleModelInfo()->GetVehicleStruct();
        return vehStruct->m_avDummyPos[targetVeh->IsBoat() ? DUMMY_LIGHT_FRONT_MAIN : DUMMY_SEAT_FRONT] * CVector{0.0f, 1.0f, 1.0f}; // ignore x
    }() + CVector{ 0.0f, 0.08f, 0.62f };

    m_fFOV = 60.0f;
    m_vecSource = entityWorldMat.TransformVector(dummyPos);
    m_vecSource += targetVeh->GetPosition();

    if (targetVeh->IsBike() && targetVeh->m_pDriver) {
        auto*   targetBike = targetVeh->AsBike();
        CVector neckPos{};

        targetVeh->m_pDriver->GetTransformedBonePosition(neckPos, BONE_NECK, true);
        neckPos += targetBike->GetMoveSpeed() * CTimer::GetTimeStep();

        constexpr auto BIKE_1ST_PERSON_ZOFFSET = 0.15f; // 0x8CC7B4
        m_vecSource.z = neckPos.z + BIKE_1ST_PERSON_ZOFFSET;

        const auto right = CrossProduct(m_vecFront, m_vecUp);
        // right *= flt_8CCD0C; (=1.0f)

        if (!CWorld::GetIsLineOfSightClear(
            CrossProduct(m_vecSource, m_vecSource + right),
            CrossProduct(m_vecSource, m_vecSource - right),
            true,
            false,
            false,
            false
        )) {
            m_vecSource = targetBike->GetPosition();
            m_vecSource.z = neckPos.z + BIKE_1ST_PERSON_ZOFFSET + 0.62f;
        }
    } else if (targetVeh->IsBoat()) {
        m_vecSource.z += 0.5f;
    }

    // todo: refactor
    if (targetVeh->IsUpsideDown()) {
        if (s_GroundFaultProtection >= 0.5f) {
            s_GroundFaultProtection = 0.5f;
        } else {
            s_GroundFaultProtection += 0.03f;
        }
    } else if (s_GroundFaultProtection >= 0.0f) {
        s_GroundFaultProtection = 0.0f;
    } else {
        s_GroundFaultProtection -= 0.03f;
    }
    m_vecSource.z += s_GroundFaultProtection;

    m_vecFront = entityWorldMat.GetForward().Normalized();
    m_vecUp    = entityWorldMat.GetUp().Normalized();
    const auto a = CrossProduct(m_vecFront, m_vecUp).Normalized();
    m_vecUp = CrossProduct(a, m_vecFront).Normalized();

    if (float wl{}; CWaterLevel::GetWaterLevel(m_vecSource, wl, true) && m_vecSource.z < wl - 0.3f) {
        ApplyUnderwaterMotionBlur();
    }
    m_bResetStatics = false;
}

// 0x521500
void CCam::Process_AimWeapon(const CVector&, float, float, float) {
    NOTSA_UNREACHABLE();
}

// 0x512B10
void CCam::Process_AttachedCam() {
    NOTSA_UNREACHABLE();
}

// 0x525E50
void CCam::Process_Cam_TwoPlayer() {
    NOTSA_UNREACHABLE();
}

// 0x519810
void CCam::Process_Cam_TwoPlayer_InCarAndShooting() {
    NOTSA_UNREACHABLE();
}

// 0x513510
void CCam::Process_Cam_TwoPlayer_Separate_Cars() {
    NOTSA_UNREACHABLE();
}

// 0x513BE0
void CCam::Process_Cam_TwoPlayer_Separate_Cars_TopDown() {
    m_fFOV = 80.0f;
    const auto p1 = FindPlayerEntity(PED_TYPE_PLAYER1), p2 = FindPlayerEntity(PED_TYPE_PLAYER2);

    const auto p1p2Centroid = (p1->GetPosition() + p2->GetPosition()) / 2.0f;
    const auto p1p2Distance = DistanceBetweenPoints(p1->GetPosition(), p2->GetPosition());

    auto camHeightMult = std::abs([&] {
        if (FindPlayerVehicle(PED_TYPE_PLAYER1)) {
            return FindPlayerVehicle(PED_TYPE_PLAYER2) ? 1.0f : 0.75f;
        }
        return FindPlayerVehicle(PED_TYPE_PLAYER2) ? 0.75f : 0.45f;
    }() - m_fCameraHeightMultiplier);

    if (const auto s = CTimer::GetTimeStep() / 200.0f; camHeightMult >= s) {
        camHeightMult = (camHeightMult >= 0.0f ? s : -s) + m_fCameraHeightMultiplier;
    }
    m_fCameraHeightMultiplier = camHeightMult;

    const auto v21 = std::max(p1p2Distance + 10.0f, 30.0f);
    m_vecSource.Set(
        p1p2Centroid.x,
        p1p2Centroid.y - v21 * std::sin(0.4f),
        p1p2Centroid.z - v21 * -std::cos(0.4f)
    );
    m_vecFront.Set(0.0f, std::sin(0.4f), -std::cos(0.4f));
    m_vecUp.Set(0.0f, m_vecSource.y, std::sin(0.4f));
    m_vecTargetCoorsForFudgeInter = m_vecSource;
}

// 0x51B850
void CCam::Process_DW_BirdyCam(bool) {
    NOTSA_UNREACHABLE();
}

// 0x51B120
void CCam::Process_DW_CamManCam(bool) {
    NOTSA_UNREACHABLE();
}

// 0x51A740
void CCam::Process_DW_HeliChaseCam(bool) {
    NOTSA_UNREACHABLE();
}

// 0x51C760
void CCam::Process_DW_PlaneCam1(bool) {
    NOTSA_UNREACHABLE();
}

// 0x51CC30
void CCam::Process_DW_PlaneCam2(bool) {
    NOTSA_UNREACHABLE();
}

// 0x51D100
void CCam::Process_DW_PlaneCam3(bool) {
    NOTSA_UNREACHABLE();
}

// 0x51C250
void CCam::Process_DW_PlaneSpotterCam(bool) {
    NOTSA_UNREACHABLE();
}

// 0x50F3F0 - debug
void CCam::Process_Editor(const CVector& target, float orientation, float speedVar, float speedVarWanted) {
    static float& s_LookAtAngle     = StaticRef<float>(0xB6FFE4);
    static bool&  s_DoRenderShadows = StaticRef<bool>(0xB7295A);

    if (m_bResetStatics) {
        m_vecSource.Set(796.0f, -937.0f, 40.0f);
        CEntity::SafeCleanUpRef(m_pCamTargetEntity);
        m_pCamTargetEntity = nullptr;
        m_bResetStatics    = false;
    }
    RwCameraSetNearClipPlane(Scene.m_pRwCamera, 0.3f);
    m_fFOV = 70.0f;

    static constexpr float _90DEG_PER_HOUR_IN_RAD_PER_MIN = 0.02617994f;
    const auto* pad = CPad::GetPad(1);
    m_fBeta += pad->GetLeftStickX() * _90DEG_PER_HOUR_IN_RAD_PER_MIN / 19.0f;
    m_fAlpha   += DegreesToRadians(static_cast<float>(pad->GetLeftStickY())) / 50.0f;

    m_fAlpha = std::max(m_fAlpha, DegreesToRadians(85.0f));
    if (m_fAlpha >= DegreesToRadians(-85.0f)) {
        if (pad->IsSquareDown()) {
            s_LookAtAngle += 0.1f;
        } else if (pad->IsCrossDown()) {
            s_LookAtAngle -= 0.1f;
        } else {
            s_LookAtAngle = 0.0f;
        }
    } else {
        m_fAlpha = DegreesToRadians(-85.0f);
    }
    s_LookAtAngle = std::clamp(s_LookAtAngle, -70.0f, 70.0f);

    m_vecFront = (m_pCamTargetEntity ? m_pCamTargetEntity->GetPosition() : m_vecSource - m_vecSource).Normalized();
    m_vecSource += s_LookAtAngle * m_vecFront;
    m_vecSource.z = std::min(m_vecSource.z, -450.0f);

    if (pad->IsRightShoulder2Pressed()) {
        FindPlayerEntity()->Teleport(m_vecSource, false);
    }

    const auto ClampByLoop = [](float& value, float min, float max) {
        while (value > max) {
            value -= 1.0f;
        }
        while (value < min) {
            value += 1.0f;
        }
    };
    ClampByLoop(m_vecSource.x, 5.0f, 115.0f);
    ClampByLoop(m_vecSource.y, 5.0f, 115.0f);

    GetVectorsReadyForRW();

    if (!pad->IsLeftShockPressed() && s_DoRenderShadows) {
        CShadows::StoreShadowToBeRendered(
            eShadowType::SHADOW_ADDITIVE,
            gpShadowExplosionTex,
            m_vecSource,
            {12.0f, 0.0f},
            {0.0f, -12.0f},
            128,
            128,
            128,
            128,
            1000.0f,
            false,
            1.0f,
            nullptr,
            false
        );
    }

    if (CHud::m_Wants_To_Draw_Hud) {
        NOTSA_LOG_DEBUG("CamX: {:0.3f} CamY: {:0.3f}  CamZ:  {:0.3f}", m_vecSource.x, m_vecSource.y, m_vecSource.z);
        NOTSA_LOG_DEBUG("Frontx: {:0.3f}, Fronty: {:0.3f}, Frontz: {:0.3f} ", m_vecFront.x, m_vecFront.y, m_vecFront.z);
        NOTSA_LOG_DEBUG("LookAT: {:0.3f}, LookAT: {:0.3f}, LookAT: {:0.3f} ", m_vecSource.x + m_vecFront.x, m_vecSource.y + m_vecFront.y, m_vecSource.z + m_vecFront.z);
    }
}

// 0x51D470
void CCam::Process_Fixed(const CVector& target, float orientation, float speedVar, float speedVarWanted) {
    if (m_nDirectionWasLooking != 3) {
        m_nDirectionWasLooking = 3; // todo: enum
    }

    m_vecSource = m_vecCamFixedModeSource;
    m_vecFront = (target - m_vecSource).Normalized();
    m_vecTargetCoorsForFudgeInter = target;
    GetVectorsReadyForRW();

    // inlined?
    const auto a = CrossProduct(
        m_vecFront,
        (m_vecCamFixedModeUpOffSet + CVector{ 0.0f, 0.0f, 1.0f }).Normalized()
    ).Normalized();
    m_vecUp = CrossProduct(
        a,
        m_vecFront
    );
    m_fFOV = 70.0f;

    if (float wl{}; CWaterLevel::GetWaterLevel(m_vecSource, wl, true) && m_vecSource.z < wl) {
        ApplyUnderwaterMotionBlur();
    }

    if (gAllowScriptedFixedCameraCollision) {
        const auto savedIgnoreEntity = CWorld::pIgnoreEntity;

        CWorld::pIgnoreEntity = FindPlayerVehicle();
        CVector out{};
        float   outDist{1.0f};
        if (TheCamera.ConeCastCollisionResolve(m_vecSource, target, out, 2.0f, 0.1f, outDist)) {
            m_vecSource.y = out.y;
            m_vecSource.z = out.z;
        }

        CWorld::pIgnoreEntity = savedIgnoreEntity;
    }
}

// 0x5B25F0
void CCam::Process_FlyBy(const CVector&, float, float, float) {
    NOTSA_UNREACHABLE();
}

// 0x5245B0
void CCam::Process_FollowCar_SA(const CVector &ThisCamsTarget, float TargetOrientation, float SpeedVar, float SpeedVarDesired, bool bScriptSetAngles) {

    static float gOldAlpha = -9999.0f;
    static float gOldBeta = -9999.0f;
    static float gAlphaTol = 0.0001f;
    static float gBetaTol = 0.0001f;
    float targetBeta;
    float betaDiffMult;
    float betaDiffCap;
    bool usingMouse = false;
    static float PROSTITUTE_CAM_ALPHA_ANGLE = 0.1f;
    static float PROSTITUTE_CAM_ALPHA_RATE  = 0.0035f;

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
    } else if (pTargetVehicle->GetModelIndex() == MODEL_RCTIGER || (pTargetVehicle->IsAutomobile() && pTargetVehicle->AsAutomobile()->handlingFlags.bHydraulicInst)) {
        stickAlphaOffset = 0.0f;
        stickBetaOffset = 0.0f;
    } else {
        fixAlphaAngle = false;
    }

    if (gDirectionIsLooking != LOOKING_FORWARD) {
        stickAlphaOffset = 0.0f;
        stickBetaOffset = 0.0f;
    }

    if (vehicleCamType == FOLLOW_CAR_INCAR && std::abs(pPlayerPad->GetSteeringUpDown()) > 120.0f && pTargetVehicle->m_pDriver && pTargetVehicle->m_pDriver->GetIntelligence()->GetActivePrimaryTask() && pTargetVehicle->m_pDriver->GetIntelligence()->GetActivePrimaryTask()->GetTaskType() != eTaskType::TASK_COMPLEX_LEAVE_CAR) {
        float extraLeftStickMod = pPlayerPad->GetSteeringUpDown();
        extraLeftStickMod = 0.5f * AIMWEAPON_STICK_SENS * AIMWEAPON_STICK_SENS * std::abs(extraLeftStickMod) * extraLeftStickMod * (0.15f / 3.5f * (m_fFOV / 80.0f));
        stickAlphaOffset += extraLeftStickMod;
    }

    if (stickAlphaOffset > 0.0f) {
        stickAlphaOffset *= 0.5f;
    }

    if (TheCamera.m_bUseMouse3rdPerson && pPlayerPad->DisablePlayerControls == 0) {
        float mouseStickY = pPlayerPad->CPad::NewMouseControllerState.m_AmountMoved.y * 2.0f;
        float mouseStickX = -pPlayerPad->CPad::NewMouseControllerState.m_AmountMoved.x * 2.0f;

        static float MOUSE_INPUT_COUNTER = 0.0f;

        bool mouseSteeringOn = true;
        if (pTargetVehicle->IsSubPlane() || pTargetVehicle->IsSubHeli()) {
            mouseSteeringOn = CVehicle::m_bEnableMouseFlying;
        } else {
            mouseSteeringOn = CVehicle::m_bEnableMouseSteering;
        }

        if ((mouseStickX != 0.0f || mouseStickY != 0.0f) && (pPlayerPad->NewState.m_bVehicleMouseLook || !mouseSteeringOn)) {
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

    if (pTargetVehicle->m_apPassengers[0] && pTargetVehicle->m_apPassengers[0]->GetIntelligence()->GetActivePrimaryTask() && pTargetVehicle->m_apPassengers[0]->GetIntelligence()->GetActivePrimaryTask()->GetTaskType() == eTaskType::TASK_COMPLEX_PROSTITUTE_SOLICIT) {
        if (((CTaskComplexProstituteSolicit*)pTargetVehicle->m_apPassengers[0]->GetIntelligence()->GetActivePrimaryTask())->bSexProcessStarted) {
            if (m_fAlpha < alphaUpLimit - PROSTITUTE_CAM_ALPHA_ANGLE) {
                stickAlphaOffset = PROSTITUTE_CAM_ALPHA_RATE * CTimer::GetTimeStep();
            } else {
                stickAlphaOffset = 0.0f;
            }
        }
    }

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

    if (usingMouse) {
        m_fBeta += stickBetaOffset;
    } else {
        m_fBeta += m_fBetaSpeed * CTimer::GetTimeStep();
    }

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

    if (usingMouse) {
        targetAlphaAngle += stickAlphaOffset;
        m_fAlpha += stickAlphaOffset;
    } else {
        targetAlphaAngle += m_fAlphaSpeed * CTimer::GetTimeStep();
        m_fAlpha += targetBeta;
    }

    if (m_fAlpha > alphaUpLimit) {
        m_fAlpha = alphaUpLimit;
        m_fAlphaSpeed = 0.0f;
    } else if (m_fAlpha < -CARCAM_SET[vehicleCamType].fDownLimit) {
        m_fAlpha = -CARCAM_SET[vehicleCamType].fDownLimit;
        m_fAlphaSpeed = 0.0f;
    }

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
        TheCamera.CameraColDetAndReact(&m_vecSource, &cameraTargetPos);
        TheCamera.ImproveNearClip(pTargetVehicle, nullptr, &m_vecSource, &cameraTargetPos);
        CWorld::pIgnoreEntity = nullptr;
        VecTrunc(&m_vecSource, 4);
    }

    TheCamera.m_bCamDirectlyBehind = false;
    TheCamera.m_bCamDirectlyInFront = false;

    VecTrunc(&m_vecSource, 4);

    GetVectorsReadyForRW();

    gVecLookBehind = cameraTargetPos;
}

// 0x50F970
void CCam::Process_FollowPedWithMouse(const CVector&, float, float, float) {
    NOTSA_UNREACHABLE();
}

// 0x522D40
void CCam::Process_FollowPed_SA(const CVector&, float, float, float, bool) {
    NOTSA_UNREACHABLE();
}

// 0x5105C0
void CCam::Process_M16_1stPerson(const CVector&, float, float, float) {
    NOTSA_UNREACHABLE();
}

// 0x511B50
void CCam::Process_Rocket(const CVector& target, float orientation, float speedVar, float speedVarWanted, bool isHeatSeeking) {
    static uint32 dword_B6FFF8 = StaticRef<uint32>(0xB6FFF8);
    static uint32 dword_B6FFFC = StaticRef<uint32>(0xB6FFFC);
    static bool   byte_B70000  = StaticRef<bool>(0xB70000);

    if (!m_pCamTargetEntity->IsPed()) {
        return;
    }

    auto* targetPed = m_pCamTargetEntity->AsPed();
    m_fFOV = 70.0f;
    if (m_bResetStatics) {
        if (!CCamera::m_bUseMouse3rdPerson || targetPed->m_pTargetedObject) {
            m_fAlpha = 0.0f;
            m_fBeta = targetPed->m_fCurrentRotation - DegreesToRadians(90.0f);
        }
        m_fInitialPlayerOrientation = m_fBeta;
        m_bResetStatics             = 0;
        m_bCollisionChecksOn        = true;
        byte_B70000                 = 0;
        dword_B6FFFC                = 0;
        dword_B6FFF8                = 0;
    }
    m_pCamTargetEntity->UpdateRW();
    m_pCamTargetEntity->UpdateRwFrame();
    CVector headPosition{};
    targetPed->GetTransformedBonePosition(headPosition, eBoneTag::BONE_HEAD, true);
    m_vecSource = headPosition + CVector{0.0f, 0.0f, 0.1f};

    auto*      pad1   = CPad::GetPad(0);
    const auto fov    = m_fFOV / 80.0f;
    const auto mouseX = pad1->NewMouseControllerState.X, mouseY = pad1->NewMouseControllerState.Y;
    
    if (amountMouseMoved.x != 0.0f || amountMouseMoved.y != 0.0f) {
        m_fHorizontalAngle += -3.0f * amountMouseMoved.x * fov * CCamera::m_fMouseAccelHorzntl;
        m_fVerticalAngle += +4.0f * amountMouseMoved.y * fov * CCamera::m_fMouseAccelVertical;
    } else {
        const auto hv  = (float)-pad1->LookAroundLeftRight(targetPed);
        const auto vv  = (float)pad1->LookAroundUpDown(targetPed);

        m_fHorizontalAngle += sq(hv) / 10000.0f * fov / 17.5f * CTimer::GetTimeStep() * (hv < 0.0f ? -1.0f : 1.0f);
        m_fVerticalAngle   += sq(vv) / 22500.0f * fov / 14.0f * CTimer::GetTimeStep() * (vv < 0.0f ? -1.0f : 1.0f);
    }
    ClipBeta();
    ClipAlpha();

    m_vecFront.Set(
        -(std::cos(m_fBeta) * std::cos(m_fAlpha)),
        -(std::sin(m_fBeta) * std::cos(m_fAlpha)),
        std::sin(m_fAlpha)
    );
    GetVectorsReadyForRW();

    const auto heading = CGeneral::GetATanOfXY(m_vecFront.x, m_vecFront.y) - DegreesToRadians(90.0f);
    TheCamera.m_pTargetEntity->AsPed()->m_fCurrentRotation = heading;
    TheCamera.m_pTargetEntity->AsPed()->m_fAimingRotation  = heading;

    if (isHeatSeeking) {
        auto* player     = FindPlayerPed();
        auto* playerData = player->m_pPlayerData;
        if (!playerData->m_nFireHSMissilePressedTime) {
            playerData->m_nFireHSMissilePressedTime = CTimer::GetTimeInMS();
        }

        const auto hsTarget = CWeapon::PickTargetForHeatSeekingMissile(
            m_vecSource,
            m_vecFront,
            1.2f,
            player,
            false,
            playerData->m_LastHSMissileTarget
        );

        // NOTE: not sure about the second one
        if (hsTarget && CTimer::GetTimeInMS() - playerData->m_nLastHSMissileLOSTime > 1'000) {
            playerData->m_nLastHSMissileLOSTime = CTimer::GetTimeInMS();

            const auto targetUsesCollision = hsTarget->m_bUsesCollision;
            const auto playerUsesCollision = player->m_bUsesCollision;
            hsTarget->m_bUsesCollision     = false;
            player->m_bUsesCollision       = false;

            const auto isClear = CWorld::GetIsLineOfSightClear(
                player->GetPosition(),
                hsTarget->GetPosition(),
                true,
                true,
                false,
                true,
                false,
                true
            );
            player->m_bUsesCollision        = playerUsesCollision;
            hsTarget->m_bUsesCollision      = targetUsesCollision;
            playerData->m_bLastHSMissileLOS = isClear;
        }

        if (!playerData->m_bLastHSMissileLOS || !hsTarget || hsTarget != playerData->m_LastHSMissileTarget) {
            playerData->m_nFireHSMissilePressedTime = CTimer::GetTimeInMS();
        }

        if (hsTarget) {
            CWeaponEffects::MarkTarget(
                CrossHairId(0),
                hsTarget->GetPosition(),
                255,
                255,
                255,
                100,
                1.3f,
                true
            );
        }

        auto& crosshair = gCrossHair[CrossHairId(0)];
        const auto time = CTimer::GetTimeInMS() - playerData->m_nFireHSMissilePressedTime;

        crosshair.m_nTimeWhenToDeactivate = 0;
        crosshair.m_color.Set(
            255,
            time <= 1'500 ? 255 : 0,
            time <= 1'500 ? 255 : 0
        );
        crosshair.m_fRotation = time <= 1'500 ? 0.0f : 1.0f;
        playerData->m_LastHSMissileTarget = hsTarget;
    }

    constexpr auto ROCKET_CAM_NEARCLIP_PLANE = 0.15f; // 0x8CCC9C
    RwCameraSetNearClipPlane(Scene.m_pRwCamera, ROCKET_CAM_NEARCLIP_PLANE);
}

// 0x517500
void CCam::Process_SpecialFixedForSyphon(const CVector&, float, float, float) {
    NOTSA_UNREACHABLE();
}

// 0x512110
bool CCam::Process_WheelCam(const CVector&, float, float, float) {
    NOTSA_UNREACHABLE();
    return false;
}

void CCam::ApplyUnderwaterMotionBlur() {
    static constexpr uint32 UNDERWATER_CAM_BLUR      = 20;    // 0x8CC7A4
    static constexpr float  UNDERWATER_CAM_MAG_LIMIT = 10.0f; // 0x8CC7A8

    const auto colorMag = std::sqrt(
        sq(CTimeCycle::m_CurrentColours.m_fWaterRed) +
        sq(CTimeCycle::m_CurrentColours.m_fWaterGreen) +
        sq(CTimeCycle::m_CurrentColours.m_fWaterBlue)
    );

    const auto factor = (colorMag <= UNDERWATER_CAM_MAG_LIMIT) ? 1.0f : UNDERWATER_CAM_MAG_LIMIT / colorMag;

    TheCamera.SetMotionBlur(
        static_cast<uint32>(factor * CTimeCycle::m_CurrentColours.m_fWaterRed),
        static_cast<uint32>(factor * CTimeCycle::m_CurrentColours.m_fWaterGreen),
        static_cast<uint32>(factor * CTimeCycle::m_CurrentColours.m_fWaterBlue),
        UNDERWATER_CAM_BLUR,
        eMotionBlurType::LIGHT_SCENE
    );
}

// 0x4D58A0
int32 ConvertPedNode2BoneTag(int32 simpleId) {
    const auto map = notsa::make_mapping<int32, int32>({
        { 1,  3 },
        { 2,  5 },
        { 3,  32},
        { 4,  22},
        { 5,  34},
        { 6,  24},
        { 7,  41},
        { 8,  51},
        { 9,  43},
        { 10, 53},
        { 11, 52},
        { 12, 42},
        { 13, 33},
        { 14, 23},
        { 15, 31},
        { 16, 21},
        { 17, 4 },
        { 18, 8 },
    });
    return notsa::find_value_or(map, simpleId, -1);
}

// 0x509A30
bool IsLampPost(eModelID modelId) {
    using namespace ModelIndices;

    return notsa::contains<eModelID>(
        {
            MI_SINGLESTREETLIGHTS1,
            MI_SINGLESTREETLIGHTS2,
            MI_SINGLESTREETLIGHTS3,
            MI_BOLLARDLIGHT,
            MI_MLAMPPOST,
            MI_STREETLAMP1,
            MI_STREETLAMP2,
            MI_TELPOLE02,
            MI_TRAFFICLIGHTS_MIAMI,
            MI_TRAFFICLIGHTS_TWOVERTICAL,
            MI_TRAFFICLIGHTS_3,
            MI_TRAFFICLIGHTS_4,
            MI_TRAFFICLIGHTS_GAY,
            MI_TRAFFICLIGHTS_5,
        },
        modelId
    );
}
