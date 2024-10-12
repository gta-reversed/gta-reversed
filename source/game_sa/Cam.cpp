#include "StdInc.h"

#include "Cam.h"
#include "TimeCycle.h"
#include "Camera.h"
#include "Shadows.h"

bool& gbFirstPersonRunThisFrame = *reinterpret_cast<bool*>(0xB6EC20);

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
    RH_ScopedInstall(Finalise_DW_CineyCams, 0x50DD70, { .reversed = false });
    RH_ScopedInstall(GetCoreDataForDWCineyCamMode, 0x517130, { .reversed = false });
    RH_ScopedInstall(GetLookFromLampPostPos, 0x5161A0, { .reversed = false });
    RH_ScopedInstall(GetVectorsReadyForRW, 0x509CE0);
    RH_ScopedInstall(Get_TwoPlayer_AimVector, 0x513E40, { .reversed = false });
    RH_ScopedInstall(IsTimeToExitThisDWCineyCamMode, 0x517400, { .reversed = false });
    RH_ScopedInstall(KeepTrackOfTheSpeed, 0x509DF0, { .reversed = false });
    RH_ScopedInstall(LookBehind, 0x520690, { .reversed = false });
    RH_ScopedInstall(LookRight, 0x520E40, { .reversed = false });
    RH_ScopedInstall(RotCamIfInFrontCar, 0x50A4F0, { .reversed = false });
    RH_ScopedInstall(Using3rdPersonMouseCam, 0x50A850, { .reversed = false });
    RH_ScopedInstall(Process, 0x526FC0, { .reversed = false });
    RH_ScopedInstall(ProcessArrestCamOne, 0x518500, { .reversed = false });
    RH_ScopedInstall(ProcessPedsDeadBaby, 0x519250, { .reversed = false });
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
    RH_ScopedInstall(Process_Editor, 0x50F3F0);
    RH_ScopedInstall(Process_Fixed, 0x51D470);
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
    TheCamera.m_bCinemaCamera = false;
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
void CCam::Finalise_DW_CineyCams(CVector*, CVector*, float, float, float, float) {
    NOTSA_UNREACHABLE();
}

// 0x517130
void CCam::GetCoreDataForDWCineyCamMode(CEntity**, CVehicle**, CVector*, CVector*, CVector*, CVector*, CVector*, CVector*, float*, CVector*, float*, CColSphere*) {
    NOTSA_UNREACHABLE();
}

// 0x5161A0
void CCam::GetLookFromLampPostPos(CEntity*, CPed*, CVector&, CVector&) {
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
void CCam::Get_TwoPlayer_AimVector(CVector& out) {
    NOTSA_UNREACHABLE();
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
bool CCam::Using3rdPersonMouseCam() {
    return CCamera::m_bUseMouse3rdPerson && m_nMode == MODE_FOLLOWPED;
}

// 0x509DC0
bool CCam::GetWeaponFirstPersonOn() {
    return m_pCamTargetEntity && m_pCamTargetEntity->IsPed() && m_pCamTargetEntity->AsPed()->GetActiveWeapon().m_IsFirstPersonWeaponModeSelected;
}

// 0x526FC0
void CCam::Process() {
    NOTSA_UNREACHABLE();
}

// 0x518500
void CCam::ProcessArrestCamOne() {
    NOTSA_UNREACHABLE();
}

// 0x519250
void CCam::ProcessPedsDeadBaby() {
    NOTSA_UNREACHABLE();
}

// 0x50EB70
void CCam::Process_1rstPersonPedOnPC(const CVector&, float, float, float) {
    NOTSA_UNREACHABLE();
}

// 0x517EA0
void CCam::Process_1stPerson(const CVector&, float, float, float) {
    NOTSA_UNREACHABLE();
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
    NOTSA_UNREACHABLE();
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
void CCam::Process_Editor(const CVector&, float, float, float) {
    static float& LOOKAT_ANGLE   = StaticRef<float>(0xB6FFE4);
    static bool&  RENDER_SHADOWS = StaticRef<bool>(0xB7295A);

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
    m_fHorizontalAngle += pad->GetLeftStickX() * _90DEG_PER_HOUR_IN_RAD_PER_MIN / 19.0f;
    m_fVerticalAngle   += DegreesToRadians(static_cast<float>(pad->GetLeftStickY())) / 50.0f;

    const auto lookAtPos = [&] {
        return m_pCamTargetEntity ? m_pCamTargetEntity->GetPosition() : m_vecSource;
    }();

    m_fVerticalAngle = std::max(m_fVerticalAngle, DegreesToRadians(85.0f));
    if (m_fVerticalAngle >= DegreesToRadians(-85.0f)) {
        if (pad->IsSquareDown()) {
            LOOKAT_ANGLE += 0.1f;
        } else if (pad->IsCrossDown()) {
            LOOKAT_ANGLE -= 0.1f;
        } else {
            LOOKAT_ANGLE = 0.0f;
        }
    } else {
        m_fVerticalAngle = DegreesToRadians(-85.0f);
    }
    LOOKAT_ANGLE = std::clamp(LOOKAT_ANGLE, -70.0f, 70.0f);

    m_vecFront = (lookAtPos - m_vecSource).Normalized();
    m_vecSource += LOOKAT_ANGLE * m_vecFront;
    m_vecSource.z = std::min(m_vecSource.z, -450.0f);

    if (pad->IsRightShoulder2Pressed()) {
        if (auto* veh = FindPlayerVehicle()) {
            veh->Teleport(m_vecSource, false);
        } else {
            FindPlayerPed()->Teleport(m_vecSource, false);
        }
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

    if (!pad->IsLeftShockPressed() && RENDER_SHADOWS) {
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
    m_vecUp = CrossProduct(
        CrossProduct(
            m_vecFront,
            (m_vecCamFixedModeUpOffSet + CVector{ 0.0f, 0.0f, 1.0f }).Normalized()
        ).Normalized(),
        m_vecFront
    );
    m_fFOV = 70.0f;

    float waterLevel{};
    if (CWaterLevel::GetWaterLevel(m_vecSource, waterLevel, true) && m_vecSource.z < waterLevel) {
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
void CCam::Process_FollowCar_SA(const CVector&, float, float, float, bool) {
    NOTSA_UNREACHABLE();
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
void CCam::Process_Rocket(const CVector&, float, float, float, bool) {
    NOTSA_UNREACHABLE();
}

// 0x517500
void CCam::Process_SpecialFixedForSyphon(const CVector&, float, float, float) {
    NOTSA_UNREACHABLE();
}

// 0x512110
void CCam::Process_WheelCam(const CVector&, float, float, float) {
    NOTSA_UNREACHABLE();
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
