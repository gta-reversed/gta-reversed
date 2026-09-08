#include "StdInc.h"

#include "Cam.h"
#include "TimeCycle.h"
#include "Camera.h"
#include "Shadows.h"
#include "IdleCam.h"
#include "InterestingEvents.h"
#include "ModelIndices.h"
#include "HandShaker.h"
#include "General.h"
#include "CullZones.h"
#include "cHandlingDataMgr.h"
#include "ModelInfo.h"
#include "VehicleModelInfo.h"
#include "ControllerConfigManager.h"

auto& gbFirstPersonRunThisFrame = StaticRef<bool>(0xB6EC20);
auto& gLastFrameProcessedDWCineyCam = StaticRef<uint32>(0x8CCB9C);
static inline auto& gDWCineyCamSceneEndTime = StaticRef<uint32>(0x8CCBA4);

static inline auto& gbExitCam = StaticRef<std::array<bool, 9>>(0xB6EC5C);

static inline auto& DWCineyCamLastPos = StaticRef<CVector>(0xB6FE8C);
static inline auto& DWCineyCamLastUp = StaticRef<CVector>(0xB6FE98);
static inline auto& DWCineyCamLastRight = StaticRef<CVector>(0xB6FEA4);
static inline auto& DWCineyCamLastFwd = StaticRef<CVector>(0xB6FEB0);

static inline auto& DWCineyCamLastNearClip = StaticRef<float>(0xB6EC08);
static inline auto& DWCineyCamLastFov = StaticRef<float>(0xB6EC0C);

static bool IsLampPost(eModelID modelId);

struct DWHeliChaseState {
    CVector end;
    CVector start;
    float forwardDistance;
    float backwardDistance;
    float height;
    float sideDistance;
    float zoomInFraction;
    float wideFOV;
    float closeFOV;
    float lookAhead;
    float roll;
    float nearClip;
    bool targetFrozen;
    int32 frozenFrames;
    int32 maxFrozenFrames;
    CVector frozenTarget;
    int32 searchAttempts;
    bool sourceObstructed;
    int32 maxClearFrames;
    int32 clearFrames;
    float zoomDistanceStart;
    float zoomDistanceEnd;
    float distanceFOVReduction;
    float minimumDistance;
    float searchSphereRadius;
    float FOVRange;
    bool targetObstructed;
    bool skipZoomIn;
    float zoomOutStartFOV;
    bool zoomingOut;
    uint32 zoomOutStartTime;
    uint32 zoomOutEndTime;
    float zoomOutFraction;
    uint32 zoomOutDuration;

    // 0x50E180
    void SetDefaults() {
        forwardDistance = sideDistance = 50.0f;
        backwardDistance = 30.0f;
        height = 55.0f;
        zoomInFraction = 0.05f;
        wideFOV = 70.0f;
        closeFOV = 22.0f;
        lookAhead = 1.0f;
        roll = 0.0f;
        nearClip = 10.0f;
        maxFrozenFrames = 30;
        searchAttempts = 8;
        maxClearFrames = 60;
        zoomDistanceStart = 100.0f;
        zoomDistanceEnd = 110.0f;
        distanceFOVReduction = 10.0f;
        minimumDistance = 5.0f;
        searchSphereRadius = 12.0f;
        zoomOutFraction = 0.75f;
        zoomOutDuration = 4000;
        targetFrozen = sourceObstructed = targetObstructed = skipZoomIn = zoomingOut = false;
        FOVRange = wideFOV - closeFOV;
        clearFrames = maxClearFrames;
        frozenFrames = maxFrozenFrames;
    }

    // 0x50E090
    void Randomise() {
        const auto randomScale = StaticRef<float>(0x858C7C);
        forwardDistance *= (float)CGeneral::GetRandomNumber() * randomScale * 1.4f + 0.1f;
        backwardDistance *= (float)CGeneral::GetRandomNumber() * randomScale * 0.5f + 0.5f;
        height *= (float)CGeneral::GetRandomNumber() * randomScale * 0.5f + 0.5f;
        sideDistance *= (float)CGeneral::GetRandomNumber() * randomScale * 0.5f + 0.5f;
        zoomInFraction *= (float)CGeneral::GetRandomNumber() * randomScale * 1.5f + 0.5f;
    }
};

// 0x5B2330
static void FindSplinePathPositionFloat(float* output, const float* spline, float time, uint32* marker) {
    static auto& minimumSegmentTime = StaticRef<float>(0x8D0F80);
    const auto count = (uint32)spline[0];
    const float duration = (spline[*marker] - spline[*marker - 4]) * 1000.0f;
    const float endTime = spline[(count - 1) * 4 + 1] * 1000.0f;
    if (time < endTime) {
        if ((*marker - 1) / 4 > count) {
            *marker = (count - 1) * 4 + 1;
        } else if (duration <= minimumSegmentTime) {
            *marker += 4;
            if ((*marker - 1) / 4 > count) {
                *marker = (count - 1) * 4 + 1;
            }
        }
    }
    const auto index = *marker;
    float t = std::clamp((time - spline[index - 4] * 1000.0f) / ((spline[index] - spline[index - 4]) * 1000.0f), 0.0f, 1.0f);
    if (time > endTime) {
        t = 1.0f;
    }
    const float start = spline[index - 3];
    const float control = spline[index - 1];
    const float end = spline[index + 1];
    if (control == start) {
        *output = (end - start) * t + start;
    } else {
        const float s = 1.0f - t;
        *output = s * s * s * start + (control * s * s * 3.0f + (spline[index + 2] * s * 3.0f + end * t) * t) * t;
    }
}

// 0x5B2090
static void FindSplinePathPositionVector(CVector* output, const float* spline, float time, uint32* marker) {
    static auto& minimumSegmentTime = StaticRef<float>(0x8D0F80);
    const auto count = (uint32)spline[0];
    const float duration = (spline[*marker] - spline[*marker - 10]) * 1000.0f;
    const float endTime = spline[(count - 1) * 10 + 1] * 1000.0f;
    if (time < endTime) {
        if ((*marker - 1) / 10 > count) {
            *marker = (count - 1) * 10 + 1;
        } else if (duration <= minimumSegmentTime) {
            *marker += 10;
            if ((*marker - 1) / 10 > count) {
                *marker = (count - 1) * 10 + 1;
            }
        }
    }
    const auto index = *marker;
    // The vector evaluator retains the duration from before advancing the marker.
    float t = std::clamp((time - spline[index - 10] * 1000.0f) / duration, 0.0f, 1.0f);
    if (time > endTime) {
        t = 1.0f;
    }
    const CVector start{spline[index - 9], spline[index - 8], spline[index - 7]};
    const CVector end{spline[index + 1], spline[index + 2], spline[index + 3]};
    const CVector control1{spline[index - 3], spline[index - 2], spline[index - 1]};
    const CVector control2{spline[index + 4], spline[index + 5], spline[index + 6]};
    if (control1 == start) {
        *output = (end - start) * t + start;
    } else {
        const float s = 1.0f - t;
        *output = start * (s * s * s) + end * (t * t * t) + (control1 * (t * s * s) + control2 * (s * t * t)) * 3.0f;
    }
    *output += TheCamera.m_vecCutSceneOffset;
}

// 0x509AE0
void WellBufferMe(float target, float& valueToChange, float& speedSoFar, float topSpeed, float speedStep, bool isAnAngle) {
    const auto valueToTargetDiff = [&] {
        auto d = target - valueToChange;
        if (isAnAngle) {
            for (; d >= DegreesToRadians(180.0f); d -= DegreesToRadians(360.0f)) {
                ;
            }
            for (; d < DegreesToRadians(-180.0f); d += DegreesToRadians(360.0f)) {
                ;
            }
        }
        return d;
    }();

    const auto fullSpeedStep = valueToTargetDiff * topSpeed;
    speedSoFar += std::abs(std::abs(fullSpeedStep - speedSoFar) * CTimer::GetTimeStep() * speedStep);

    if (fullSpeedStep >= 0.0f || fullSpeedStep <= speedSoFar) {
        if (fullSpeedStep > 0.0f && fullSpeedStep < speedSoFar) {
            speedSoFar = fullSpeedStep;
        }
    } else {
        speedSoFar = fullSpeedStep;
    }

    valueToChange += std::min(CTimer::GetTimeStep(), 10.0f) * speedSoFar;
}

// 0x509BE0
static void WrapAngle(float& angle) {
    while (angle >= DegreesToRadians(180.0f)) {
        angle -= DegreesToRadians(360.0f);
    }
    while (angle < DegreesToRadians(-180.0f)) {
        angle += DegreesToRadians(360.0f);
    }
}

void CCam::InjectHooks() {
    RH_ScopedClass(CCam);
    RH_ScopedCategory("Camera");

    RH_ScopedInstall(Constructor, 0x517730);
    RH_ScopedInstall(Init, 0x50E490);
    RH_ScopedInstall(CacheLastSettingsDWCineyCam, 0x50D7A0);
    RH_ScopedInstall(DoCamBump, 0x50CB30);
    RH_ScopedInstall(Finalise_DW_CineyCams, 0x50DD70);
    RH_ScopedInstall(GetCoreDataForDWCineyCamMode, 0x517130);
    RH_ScopedInstall(GetTwoPlayerCameraPosition, 0x5132D0);
    RH_ScopedInstall(GetLookFromLampPostPos, 0x5161A0);
    RH_ScopedInstall(GetVectorsReadyForRW, 0x509CE0);
    RH_ScopedInstall(GetBoatHandlingCamHeight, 0x509CA0);
    RH_ScopedInstall(Get_TwoPlayer_AimVector, 0x513E40);
    RH_ScopedInstall(IsTimeToExitThisDWCineyCamMode, 0x517400);
    RH_ScopedInstall(KeepTrackOfTheSpeed, 0x509DF0);
    RH_ScopedInstall(LookBehind, 0x520690);
    RH_ScopedInstall(LookRight, 0x520E40);
    RH_ScopedInstall(RotCamIfInFrontCar, 0x50A4F0);
    RH_ScopedInstall(Using3rdPersonMouseCam, 0x50A850);
    RH_ScopedInstall(Process, 0x526FC0, { .reversed = false });
    RH_ScopedInstall(ProcessArrestCamOne, 0x518500, { .reversed = false });
    RH_ScopedInstall(ProcessPedsDeadBaby, 0x519250);
    RH_ScopedInstall(Process_1rstPersonPedOnPC, 0x50EB70);
    RH_ScopedInstall(Process_1stPerson, 0x517EA0);
    RH_ScopedInstall(Process_AimWeapon, 0x521500, { .reversed = false });
    RH_ScopedInstall(Process_AttachedCam, 0x512B10);
    RH_ScopedInstall(Process_Cam_TwoPlayer, 0x525E50, { .reversed = false });
    RH_ScopedInstall(Process_Cam_TwoPlayer_InCarAndShooting, 0x519810, { .reversed = false });
    RH_ScopedInstall(Process_Cam_TwoPlayer_Separate_Cars, 0x513510);
    RH_ScopedInstall(Process_Cam_TwoPlayer_Separate_Cars_TopDown, 0x513BE0);
    RH_ScopedInstall(Process_DW_BirdyCam, 0x51B850);
    RH_ScopedInstall(Process_DW_CamManCam, 0x51B120);
    RH_ScopedInstall(Process_DW_HeliChaseCam, 0x51A740);
    RH_ScopedInstall(Process_DW_PlaneCam1, 0x51C760);
    RH_ScopedInstall(Process_DW_PlaneCam2, 0x51CC30);
    RH_ScopedInstall(Process_DW_PlaneCam3, 0x51D100);
    RH_ScopedInstall(Process_DW_PlaneSpotterCam, 0x51C250);
    RH_ScopedInstall(Process_Editor, 0x50F3F0);
    RH_ScopedInstall(Process_Fixed, 0x51D470);
    RH_ScopedInstall(Process_FlyBy, 0x5B25F0);
    RH_ScopedInstall(Process_FollowCar_SA, 0x5245B0, { .reversed = false });
    RH_ScopedInstall(Process_FollowPedWithMouse, 0x50F970);
    RH_ScopedInstall(Process_FollowPed_SA, 0x522D40, { .reversed = false });
    RH_ScopedInstall(Process_M16_1stPerson, 0x5105C0);
    RH_ScopedInstall(Process_Rocket, 0x511B50);
    RH_ScopedInstall(Process_SpecialFixedForSyphon, 0x517500);
    RH_ScopedInstall(Process_WheelCam, 0x512110);

    RH_ScopedGlobalInstall(WellBufferMe, 0x509AE0);
    RH_ScopedGlobalInstall(WrapAngle, 0x509BE0);
    RH_ScopedGlobalInstall(FindSplinePathPositionFloat, 0x5B2330);
    RH_ScopedGlobalInstall(FindSplinePathPositionVector, 0x5B2090);
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
void CCam::Finalise_DW_CineyCams(const CVector& src, const CVector& dest, float roll, float fov, float nearClip, float shakeDegree) {
    m_vecFront  = (dest - src).Normalized();
    m_vecSource = src;

    // What is this thing?
    {
        auto rightDir = m_vecFront.Cross({ std::sin(roll), 0.0f, std::cos(roll) }).Normalized();
        m_vecUp       = rightDir.Cross(m_vecFront);
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
    m_vecFront.Normalise();

    {
        auto rightDir = m_vecFront.Cross({ std::sin(roll), 0.0f, std::cos(roll) }).Normalized();
        m_vecUp       = rightDir.Cross(m_vecFront);
        if (m_vecFront.x == 0.0f && m_vecFront.y == 0.0f) {
            m_vecFront.x = m_vecFront.y = 0.0001f;
        }
        rightDir = m_vecFront.Cross(m_vecUp).Normalized();
        m_vecUp  = rightDir.Cross(m_vecFront);
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
bool CCam::GetLookFromLampPostPos(CEntity* target, CPed* cop, const CVector& vecTarget, CVector& outPos) {
    int16    count{};
    CEntity* entities[16];
    CWorld::FindObjectsInRange(vecTarget, 30.0f, true, &count, 0xF, entities, false, false, false, true, true);

    CEntity* winner{};
    float    closestDistDiff = 10000.0f;
    for (int32 i = 0; i < count; i++) {
        CEntity* entity = entities[i];
        if (!entity->m_bIsStatic && !entity->m_bIsStaticWaitingForCollision) {
            continue;
        }
        if (!entity->m_matrix) {
            entity->AllocateMatrix();
            entity->m_placement.UpdateMatrix(entity->m_matrix);
        }
        if (entity->GetUp().z <= 0.9f) {
            continue;
        }
        if (!IsLampPost((eModelID)entity->GetModelIndex())) {
            continue;
        }

        const auto dist = DistanceBetweenPoints2D(entity->GetPosition(), vecTarget);
        if (dist <= 5.0f || std::abs(17.0f - dist) >= closestDistDiff) {
            continue;
        }

        const CVector topPos = entity->GetMatrix().TransformPoint(entity->GetColModel()->m_boundBox.m_vecMax);
        CVector      dir     = topPos - vecTarget;
        dir.Normalise();
        if (!CWorld::GetIsLineOfSightClear(topPos, dir + vecTarget, true, false, false, false, false, true, true)) {
            continue;
        }

        winner          = entity;
        outPos          = topPos;
        closestDistDiff = std::abs(17.0f - dist);
    }
    return winner != nullptr;
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

// 0x513E40 -- not tested
void CCam::Get_TwoPlayer_AimVector(CVector& out) {
    const auto player = [&] {
        auto* p1 = FindPlayerPed(PED_TYPE_PLAYER1);
        if (p1->m_pVehicle && !p1->m_pVehicle->IsDriver(p1)) {
            return FindPlayerPed(PED_TYPE_PLAYER2);
        }
        return p1;
    }();

    const auto weaponInfo = player->GetActiveWeapon().GetWeaponInfo(player);
    const auto nearestTargetEntityInScreen = CWeapon::FindNearestTargetEntityWithScreenCoors(
        m_fX_Targetting,
        m_fY_Targetting,
        2 * weaponInfo.m_fWeaponRange,
        player->GetPosition()
    );

    if (nearestTargetEntityInScreen) {
        out = nearestTargetEntityInScreen->GetPosition() - m_vecSource;
    } else {
        const auto right  = CrossProduct(m_vecFront, m_vecUp);
        const auto tanFov = std::tan(m_fFOV * PI / 360.0f);

        out = m_fX_Targetting * m_fY_Targetting * tanFov * right + m_vecFront - tanFov / CDraw::ms_fAspectRatio * m_vecUp;
    }
    out.Normalise();
}

// 0x517400
bool CCam::IsTimeToExitThisDWCineyCamMode(int32 camId, const CVector& src, const CVector& dst, float t, bool lineOfSightCheck) {
    if (gbExitCam[camId]) {
        return true;
    }

    static constexpr float aMinDists[MODE_SYPHON_CRIM_IN_FRONT - MODE_FOLLOW_PED_WITH_BIND + 1] = { 3.0f, 3.0f, 1.0f, 3.0f, 5.0f, 3.0f, 3.0f, 3.0f, 3.0f };
    static constexpr float aMaxDists[MODE_SYPHON_CRIM_IN_FRONT - MODE_FOLLOW_PED_WITH_BIND + 1] = { 185.0f, 100.0f, 100.0f, 100.0f, 30.0f, 30.0f, 100.0f, 100.0f, 100.0f };

    const auto dist         = (dst - src).Magnitude();
    const bool isWithinBand = dist >= aMinDists[camId - MODE_FOLLOW_PED_WITH_BIND]
                           && dist <= aMaxDists[camId - MODE_FOLLOW_PED_WITH_BIND];

    bool isLosClear = true;
    if (lineOfSightCheck) {
        CWorld::pIgnoreEntity = m_pCamTargetEntity;
        CColPoint colPoint{};
        CEntity*  hitEntity{};
        isLosClear = !CWorld::ProcessLineOfSight(dst, src, colPoint, hitEntity, true, true, false, false, false, false, false, false);
        CWorld::pIgnoreEntity = nullptr;
    }

    if (camId >= MODE_FOLLOW_PED_WITH_BIND && camId <= MODE_SYPHON_CRIM_IN_FRONT) {
        if (!isWithinBand || !isLosClear || CTimer::GetTimeInMS() > gDWCineyCamSceneEndTime) {
            return true;
        }
    }
    return false;
}

// 0x509DF0
void CCam::KeepTrackOfTheSpeed(const CVector& source, const CVector& target, const CVector& up, const float& alpha, const float& beta, const float& fov) {
    static CVector prevSource;
    static CVector prevTarget;
    static CVector prevUp;
    static float  prevBeta;
    static float  prevAlpha;
    static float  prevFov;
    static uint32 staticsInitialized;

    if ((staticsInitialized & 1) == 0) {
        prevSource = source;
        staticsInitialized |= 1;
    }
    if ((staticsInitialized & 2) == 0) {
        prevTarget = target;
        staticsInitialized |= 2;
    }
    if ((staticsInitialized & 4) == 0) {
        prevUp = up;
        staticsInitialized |= 4;
    }

    float prevBetaVal = prevBeta;
    if ((staticsInitialized & 8) == 0) {
        staticsInitialized |= 8;
        prevBetaVal = beta;
    }
    if ((staticsInitialized & 0x10) == 0) {
        prevAlpha = alpha;
        staticsInitialized |= 0x10;
    }
    float prevFovVal = prevFov;
    if ((staticsInitialized & 0x20) == 0) {
        staticsInitialized |= 0x20;
        prevFovVal = fov;
    }

    if (TheCamera.m_bJust_Switched) {
        prevSource = source;
        prevTarget = target;
        prevUp     = up;
    }

    m_vecSourceSpeedOverOneFrame = source - prevSource;
    m_vecTargetSpeedOverOneFrame = target - prevTarget;
    m_vecUpOverOneFrame          = up - prevUp;

    m_fFovSpeedOverOneFrame = fov - prevFovVal;
    m_fBetaSpeedOverOneFrame = beta - prevBetaVal;
    WrapAngle(m_fBetaSpeedOverOneFrame);
    m_fAlphaSpeedOverOneFrame = alpha - prevAlpha;
    WrapAngle(m_fAlphaSpeedOverOneFrame);

    prevSource = source;
    prevTarget = target;
    prevUp     = up;
    prevBeta   = beta;
    prevAlpha  = alpha;
    prevFov    = fov;
}

// 0x509CA0
bool CCam::GetBoatHandlingCamHeight(float* outCamHeight) {
    if (!m_pCamTargetEntity) {
        return false;
    }

    const auto* boatHandling = gHandlingDataMgr.GetBoatPointer(
        CModelInfo::GetVehicleModelInfo(m_pCamTargetEntity->m_nModelIndex)->m_nHandlingId
    );
    if (!boatHandling) {
        return false;
    }

    *outCamHeight = boatHandling->m_fLookLRBehindCamHeight;
    return true;
}

// 0x520690
void CCam::LookBehind() {
    static auto& gStoredTargetPos                    = StaticRef<CVector>(0xB6F018);
    static auto& gNumEntitiesRegisteredForCollision  = StaticRef<uint32>(0xB6FC70);
    static auto& g_aEntitiesRegisteredForCollision   = StaticRef<CEntity*[16]>(0xB6FC74);
    static auto& gDistOverOneFrame                   = StaticRef<float>(0xB6F0FC);

    CEntity* const entity     = m_pCamTargetEntity;
    const auto     entityType = entity->GetType();

    const bool isBehindCamVehicle =
        (m_nMode == MODE_CAM_ON_A_STRING || m_nMode == MODE_BEHINDBOAT || m_nMode == MODE_BEHINDCAR)
        && entityType == ENTITY_TYPE_VEHICLE;
    const bool isFirstPersonVehicle = m_nMode == MODE_1STPERSON && entityType == ENTITY_TYPE_VEHICLE;

    if (!isBehindCamVehicle && !isFirstPersonVehicle && entityType != ENTITY_TYPE_PED) {
        return;
    }

    const CVector entityPos = entity->GetPosition();
    m_vecSourceBeforeLookBehind = m_vecSource;
    m_vecFront = CVector(entityPos - m_vecSource);

    if (isBehindCamVehicle) {
        m_vecSource = gStoredTargetPos;
        m_bLookingBehind = true;

        const float distAway =
            (m_nMode == MODE_CAM_ON_A_STRING) ? m_fCaMaxDistance : 15.5f;

        if (!entity->m_matrix) {
            entity->AllocateMatrix();
            entity->m_placement.UpdateMatrix(entity->m_matrix);
        }
        m_vecSource.x = entity->GetMatrix().GetForward().x;
        m_vecSource.y = entity->GetMatrix().GetForward().y;
        m_vecSource.z = entity->GetMatrix().GetForward().z + 0.2f;

        m_vecSource.x = gStoredTargetPos.x + distAway * m_vecSource.x;
        m_vecSource.y = gStoredTargetPos.y + distAway * m_vecSource.y;
        m_vecSource.z = gStoredTargetPos.z + distAway * m_vecSource.z;

        CWorld::pIgnoreEntity = (CEntity*)entity;
        gNumEntitiesRegisteredForCollision = 0;
        TheCamera.CameraVehicleModeSpecialCases((CVehicle*)entity);
        TheCamera.CameraColDetAndReact(&m_vecSource, &gStoredTargetPos);

        m_vecFront = CVector(entity->GetPosition() - m_vecSource);
        GetVectorsReadyForRW();
        TheCamera.ImproveNearClip((CVehicle*)entity, nullptr, &m_vecSource, &gStoredTargetPos);
        CWorld::pIgnoreEntity = nullptr;
    }

    if (isFirstPersonVehicle) {
        m_bLookingBehind = true;
        RwCameraSetNearClipPlane(Scene.m_pRwCamera, 0.05f);

        if (!entity->m_matrix) {
            entity->AllocateMatrix();
            entity->m_placement.UpdateMatrix(entity->m_matrix);
        }
        m_vecFront = entity->GetMatrix().GetForward();
        m_vecFront.Normalise();

        if (entity->AsVehicle()->m_nVehicleType == VEHICLE_TYPE_BOAT) {
            m_vecSourceBeforeLookBehind.z -= 2.0f;
        }

        const auto appearance = entity->AsVehicle()->GetVehicleAppearance();
        if (appearance == VEHICLE_APPEARANCE_BIKE) {
            m_vecSource += m_vecFront * 2.3f;
            m_vecFront = -m_vecFront;
            GetVectorsReadyForRW();
        } else if (appearance == VEHICLE_APPEARANCE_HELI) {
            if (!entity->m_matrix) {
                entity->AllocateMatrix();
                entity->m_placement.UpdateMatrix(entity->m_matrix);
            }
            m_vecFront = entity->GetMatrix().GetUp() * -1.0f;
            m_vecUp    = entity->GetMatrix().GetForward();
            m_vecSource += m_vecFront * 0.25f;
        } else {
            m_vecSource += m_vecFront * 0.25f;
            m_vecFront = -m_vecFront;
        }
    }

    if (entityType == ENTITY_TYPE_PED) {
        static auto& gZoomAddSourceZBack      = StaticRef<float[4]>(0x8CCE3C);
        static auto& gZoomAddTargetZBack      = StaticRef<float[4]>(0x8CCE30);
        static auto& gZoomLerpBackSwim        = StaticRef<float[4]>(0x8CCE24);
        static auto& gZoomAddSourceZBackSwim  = StaticRef<float[4]>(0x8CCE18);

        CVector target = entityPos;

        m_vecSource.Set(-std::cos(m_fHorizontalAngle), -std::sin(m_fHorizontalAngle), 0.0f);
        m_vecSource.z = 0.3f - m_vecSource.Dot(entity->AsPed()->field_578);
        m_vecSource.Normalise();

        float dist = 2.0f + gDistOverOneFrame;
        if (dist < 0.6f) {
            dist = 0.6f;
        }
        m_vecSource = target + m_vecSource * dist;

        const float      srcAddZ = gZoomAddSourceZBack[TheCamera.m_nPedZoom];
        target.z += gZoomAddTargetZBack[TheCamera.m_nPedZoom];

        if (entity->AsPed()->m_pIntelligence->GetTaskSwim()) {
            m_vecSource = target + (target - m_vecSource) * gZoomLerpBackSwim[TheCamera.m_nPedZoom];
            m_vecSource.z += gZoomAddSourceZBackSwim[TheCamera.m_nPedZoom];
        }
        m_vecSource.z += srcAddZ;

        TheCamera.HandleCameraMotionForDucking((CPed*)entity, &m_vecSource, &target, false);
        gNumEntitiesRegisteredForCollision = 0;

        if (m_pCamTargetEntity) {
            const auto* holdTask = entity->AsPed()->m_pIntelligence->GetTaskHold(false);
            if (holdTask && holdTask->m_pEntityToHold) {
                g_aEntitiesRegisteredForCollision[gNumEntitiesRegisteredForCollision] = holdTask->m_pEntityToHold;
                gNumEntitiesRegisteredForCollision++;
            }
        }

        CCollision::bCamCollideWithVehicles = true;
        CCollision::bCamCollideWithObjects  = true;
        CCollision::bCamCollideWithPeds     = true;

        TheCamera.CameraColDetAndReact(&m_vecSource, &target);
        m_vecFront = target - m_vecSource;
        GetVectorsReadyForRW();
        TheCamera.ImproveNearClip(nullptr, (CPed*)entity, &m_vecSource, &target);

        if (TheCamera.m_nPedZoom == 1 && RwCameraGetNearClipPlane(Scene.m_pRwCamera) > 0.05f) {
            RwCameraSetNearClipPlane(Scene.m_pRwCamera, 0.05f);
        }
    }

    GetVectorsReadyForRW();
}

// 0x520E40
void CCam::LookRight(bool bLookRight) {
    static auto& gNumEntitiesRegisteredForCollision = StaticRef<uint32>(0xB6FC70);

    CEntity* const entity     = m_pCamTargetEntity;
    const auto     entityType = entity->GetType();

    const bool isBehindCamVehicle =
        (m_nMode == MODE_CAM_ON_A_STRING || m_nMode == MODE_BEHINDBOAT || m_nMode == MODE_BEHINDCAR)
        && entityType == ENTITY_TYPE_VEHICLE;
    const bool isFirstPersonVehicle = m_nMode == MODE_1STPERSON && entityType == ENTITY_TYPE_VEHICLE;

    float sideMult = 1.0f;
    if (!bLookRight) {
        m_bLookingLeft  = true;
        sideMult = -1.0f;
    } else {
        m_bLookingRight = true;
    }

    if (isBehindCamVehicle) {
        CVector targetPos = entity->GetPosition();

        float dist = 9.0f;
        if (m_nMode == MODE_CAM_ON_A_STRING) {
            dist = m_fCaMaxDistance;
        } else if (m_nMode == MODE_BEHINDBOAT) {
            float boatCamHeight = 0.0f;
            if (GetBoatHandlingCamHeight(&boatCamHeight) && !CCullZones::Cam1stPersonForPlayer()) {
                m_vecSource.z = targetPos.z + boatCamHeight;
            }
        }

        if (!entity->m_matrix) {
            entity->AllocateMatrix();
            entity->m_placement.UpdateMatrix(entity->m_matrix);
        }
        CVector fwd = entity->GetMatrix().GetForward();
        fwd.Normalise();

        const float halfPi = 1.5707964f;
        const float angle  = CGeneral::GetATanOfXY(fwd.x, fwd.y) + sideMult * halfPi;
        m_vecSource.x = targetPos.x + dist * std::cos(angle);
        m_vecSource.y = targetPos.y + dist * std::sin(angle);

        const auto* colModel = entity->GetColModel();
        const float sourceZBeforeCollision = m_vecSource.z;

        CWorld::pIgnoreEntity              = entity;
        gNumEntitiesRegisteredForCollision = 0;
        TheCamera.CameraVehicleModeSpecialCases(entity->AsVehicle());
        TheCamera.CameraColDetAndReact(&m_vecSource, &targetPos);
        CWorld::pIgnoreEntity = nullptr;

        targetPos = entity->GetPosition();

        const float sideZ = !bLookRight
            ? colModel->m_boundBox.m_vecMax.x * entity->GetMatrix().GetRight().z
            : entity->GetMatrix().GetRight().z * colModel->m_boundBox.m_vecMin.x;
        float z = targetPos.z + sideZ + colModel->m_boundBox.m_vecMax.z * entity->GetMatrix().GetUp().z;

        const float zMax = std::max(z, m_vecTargetCoorsForFudgeInter.z);
        float newZ = sourceZBeforeCollision;
        if (zMax + 0.1f <= sourceZBeforeCollision) {
            newZ = zMax + 0.1f;
        }
        if (newZ <= m_vecSource.z) {
            newZ = m_vecSource.z;
        }
        m_vecSource.z = newZ;

        m_vecFront = targetPos - m_vecSource;
        m_vecFront.z += 1.1f;
        if (m_nMode == MODE_BEHINDBOAT) {
            m_vecFront.z += 1.2f;
        }
        GetVectorsReadyForRW();
        return;
    }

    if (isFirstPersonVehicle) {
        RwCameraSetNearClipPlane(Scene.m_pRwCamera, 0.05f);

        if (!entity->m_matrix) {
            entity->AllocateMatrix();
            entity->m_placement.UpdateMatrix(entity->m_matrix);
        }

        if (entity->AsVehicle()->m_nVehicleType == VEHICLE_TYPE_BOAT) {
            if (auto* driver = entity->AsVehicle()->m_pDriver) {
                CVector neckPos{};
                driver->SetPedPositionInCar();
                driver->UpdateRwMatrix();
                driver->UpdateRwFrame();
                driver->UpdateRpHAnim();
                driver->GetBonePosition(&neckPos, BONE_NECK, true);
                neckPos += entity->GetMatrix().GetRight() * (bLookRight ? 0.7f : 0.3f);
                neckPos += entity->GetMatrix().GetUp() * 0.2f;
                m_vecSource = neckPos;
            } else {
                m_vecSource.z -= 0.5f;
            }
        }

        if (entity->AsVehicle()->m_nVehicleType != VEHICLE_TYPE_BIKE) {
            m_vecSource -= entity->GetMatrix().GetRight() * 0.35f;
        }

        m_vecUp = entity->GetMatrix().GetUp();
        m_vecUp.Normalise();

        m_vecFront = entity->GetMatrix().GetForward();
        m_vecFront.Normalise();

        m_vecFront = !bLookRight ? CrossProduct(m_vecUp, m_vecFront)
                                 : CrossProduct(m_vecFront, m_vecUp);
        m_vecFront.Normalise();

        if (entity->AsVehicle()->GetVehicleAppearance() == VEHICLE_APPEARANCE_BIKE) {
            m_vecSource -= m_vecFront * 1.45f;
        }
    }
}

// 0x50A4F0
void CCam::RotCamIfInFrontCar(const CVector& target, float orientation) {
    auto* ent = m_pCamTargetEntity;
    if (ent->GetType() != ENTITY_TYPE_VEHICLE) {
        return;
    }
    auto* veh = static_cast<CVehicle*>(ent);

    const auto& speed = veh->GetMoveSpeed();
    if (speed.SquaredMagnitude() > 0.0036f) {
        orientation = CGeneral::GetATanOfXY(-speed.x, speed.y) - DegreesToRadians(90.0f);
    }

    const float dist = DistanceBetweenPoints2D(m_vecSource, target);
    float delta = orientation - m_fHorizontalAngle;
    while (delta > DegreesToRadians(180.0f)) {
        delta -= DegreesToRadians(360.0f);
    }
    while (delta < -DegreesToRadians(180.0f)) {
        delta += DegreesToRadians(360.0f);
    }
    if (std::fabs(delta) > DegreesToRadians(20.0f)
        && DotProduct(veh->GetMatrix().GetForward(), speed) > 0.1f
        && !TheCamera.m_bTransitionState) {
        m_bFixingBeta = true;
    }

    const auto pad = CPad::GetPad(0);
    if (!pad->GetLookBehindForCar() && !pad->GetLookBehindForPed() && !pad->GetLookLeft() && !pad->GetLookRight() && m_nDirectionWasLooking != 3) {
        TheCamera.m_bCamDirectlyBehind = true;
    }

    if (!m_bFixingBeta && !TheCamera.m_bUseTransitionBeta && !TheCamera.m_bCamDirectlyBehind && !TheCamera.m_bCamDirectlyInFront) {
        return;
    }

    bool wasRequested = false;
    if ((TheCamera.m_bCamDirectlyBehind || TheCamera.m_bCamDirectlyInFront || TheCamera.m_bUseTransitionBeta) && &TheCamera.GetActiveCam() == this) {
        wasRequested = true;
    }
    if (m_bFixingBeta || wasRequested) {
        WellBufferMe(orientation, m_fHorizontalAngle, m_fBetaSpeed, 0.1f, 0.003f, true);
        if (TheCamera.m_bCamDirectlyBehind && &TheCamera.GetActiveCam() == this) {
            m_fHorizontalAngle = orientation;
        }
        if (TheCamera.m_bCamDirectlyInFront && &TheCamera.GetActiveCam() == this) {
            m_fHorizontalAngle = orientation + DegreesToRadians(180.0f);
        }
        if (TheCamera.m_bUseTransitionBeta && &TheCamera.GetActiveCam() == this) {
            m_fHorizontalAngle = m_fTransitionBeta;
        }
        m_vecSource.x = target.x + std::cos(m_fHorizontalAngle) * dist;
        m_vecSource.y = target.y + std::sin(m_fHorizontalAngle) * dist;

        delta = orientation - m_fHorizontalAngle;
        while (delta > DegreesToRadians(180.0f)) {
            delta -= DegreesToRadians(360.0f);
        }
        while (delta < -DegreesToRadians(180.0f)) {
            delta += DegreesToRadians(360.0f);
        }
        if (std::fabs(delta) < DegreesToRadians(2.0f)) {
            m_bFixingBeta = false;
        }
    }
    TheCamera.m_bCamDirectlyBehind = false;
    TheCamera.m_bCamDirectlyInFront  = false;
}

// 0x50A850
bool CCam::Using3rdPersonMouseCam() const {
    return CCamera::m_bUseMouse3rdPerson && m_nMode == MODE_FOLLOWPED;
}

// 0x509DC0
bool CCam::GetWeaponFirstPersonOn() {
    return m_pCamTargetEntity && m_pCamTargetEntity->GetIsTypePed() && m_pCamTargetEntity->AsPed()->GetActiveWeapon().m_IsFirstPersonWeaponModeSelected;
}

// inlined -- alpha = vertical angle
void CCam::ClipAlpha() {
    m_fVerticalAngle = std::clamp(
        m_fVerticalAngle,
        DegreesToRadians(-85.5f),
        DegreesToRadians(+60.0f)
    );
}

// 0x509C50 -- beta = horizontal angle
void CCam::ClipBeta() {
    if (m_fHorizontalAngle < DegreesToRadians(-180.0f)) {
        m_fHorizontalAngle += DegreesToRadians(360.0f);
    } else {
        m_fHorizontalAngle -= DegreesToRadians(360.0f);
    }
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
    static auto& initialHeight = StaticRef<float>(0x8CC804);
    static auto& riseSpeed = StaticRef<float>(0x8CC808);
    static auto& swayDistance = StaticRef<float>(0x8CC80C);
    static auto& swayPeriod = StaticRef<float>(0x8CC810);
    static auto& swayAngle = StaticRef<float>(0x8CC814);
    static auto& startTime = StaticRef<uint32>(0xB6F094);
    static auto& unusedTime = StaticRef<float>(0xB70054);
    static auto& unusedValue = StaticRef<float>(0xB70050);

    auto* entity = TheCamera.m_pTargetEntity;
    CVector target;
    if (entity->IsPed()) {
        entity->AsPed()->GetBonePosition(&target, BONE_SPINE1, true);
    } else if (entity->IsVehicle()) {
        target = entity->GetPosition();
        target.z += entity->GetColModel()->GetBoundingBox().m_vecMax.z;
    } else {
        return;
    }

    CVector source;
    if (m_bResetStatics) {
        unusedTime = (float)CTimer::GetTimeInMS();
        startTime = CTimer::GetTimeInMS();
        unusedValue = 0.0f;
        source = target;
        source.z += initialHeight;
        float waterLevel{};
        if (CWaterLevel::GetWaterLevelNoWaves(source, &waterLevel) && source.z < waterLevel + 1.5f) {
            source.z = waterLevel + 1.5f;
        }
        auto right = CrossProduct(entity->GetMatrix().GetForward(), CVector{0.0f, 0.0f, 1.0f});
        right.z = 0.0f;
        right.Normalise();
        m_vecFront = (target - source).Normalized();
        m_vecUp = CrossProduct(right, m_vecFront).Normalized();
        m_bResetStatics = false;
    } else {
        source = m_vecSource;
        if (!CWorld::TestSphereAgainstWorld(source + CVector{0.0f, 0.0f, 0.2f}, 0.3f, entity, true, true, false, true, false, true)) {
            source.z += CTimer::GetTimeStep() * riseSpeed;
        }
        auto right = CrossProduct(entity->GetMatrix().GetForward(), CVector{0.0f, 0.0f, 1.0f});
        right.z = 0.0f;
        right.Normalise();
        const float elapsed = (float)(CTimer::GetTimeInMS() - startTime);
        auto desiredSource = target + right * swayDistance * (std::min(elapsed, 1000.0f) * 0.001f * std::sin(elapsed / swayPeriod));
        desiredSource.z = source.z;
        const auto movement = (desiredSource - source).Normalized();
        if (!CWorld::TestSphereAgainstWorld(source + movement * 0.2f, 0.3f, entity, true, true, false, true, false, true)) {
            source = desiredSource;
        }
        m_vecFront = CVector{0.0f, 0.0f, -1.0f} + right * swayAngle * (std::min(elapsed, 2000.0f) * 0.0005f) * std::cos(elapsed / swayPeriod);
        m_vecFront.Normalise();
        m_vecUp = CrossProduct(right, m_vecFront).Normalized();
    }
    m_vecSource = source;
    TheCamera.AvoidTheGeometry(&source, &target, &m_vecSource, m_fFOV);
    TheCamera.m_bMoveCamToAvoidGeom = false;
}

// 0x50EB70
void CCam::Process_1rstPersonPedOnPC(const CVector& target, float orientation, float speedVar, float speedVarWanted) {
    static auto& v3d_8CCC54   = StaticRef<CVector>(0x8CCC54);
    static auto& byte_B6FFDC  = StaticRef<bool>(0xB6FFDC);
    static auto& v3d_B6FFC4   = StaticRef<CVector>(0xB6FFC4);
    static auto& v3d_B6FFD0   = StaticRef<CVector>(0xB6FFD0);

    StaticRef<uint32>(0xB6FFE0) |= 1;
    TheCamera.m_b1rstPersonRunCloseToAWall = false;

    if (m_nMode != MODE_SNIPER_RUNABOUT) {
        m_fFOV = 70.0f;
    }

    if (!m_pCamTargetEntity->GetRwObject()) {
        return;
    }

    if (!m_pCamTargetEntity->GetIsTypePed()) {
        m_bResetStatics = false;
        RwCameraSetNearClipPlane(Scene.m_pRwCamera, 0.05f);
        return;
    }

    const auto hier = GetAnimHierarchyFromSkinClump(m_pCamTargetEntity->GetRpClump());
    const auto aIdx = RpHAnimIDGetIndex(hier, ConvertPedNode2BoneTag(2)); // todo: enum
    auto&      aMat = RpHAnimHierarchyGetMatrixArray(hier)[aIdx];
    auto*      targetPed = m_pCamTargetEntity->AsPed();

    CVector pointIn = v3d_8CCC54;
    RwV3dTransformPoint(&pointIn, &pointIn, &aMat);
    RwV3d v3dZero{ 0.0f };
    RwMatrixScale(&aMat, &v3dZero, rwCOMBINEPRECONCAT);

    if (m_bResetStatics) {
        // unnecessary entity ped check
        m_fVerticalAngle = 0.0f;
        byte_B6FFDC      = false;
        v3d_B6FFD0.Reset();
        m_fHorizontalAngle            = targetPed->m_fCurrentRotation + DegreesToRadians(90.0f);
        m_bCollisionChecksOn          = true;
        m_fInitialPlayerOrientation   = m_fHorizontalAngle;
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
        auto targetFwd = targetPed->GetForward();
        targetFwd.z = 0.0f;
        targetFwd.Normalise();
        const auto mag       = (pointIn - v3d_B6FFC4).Magnitude2D();

        m_vecSource = targetFwd * mag * 1.23f + targetPed->GetPosition() + CVector{ 0.0f, 0.0f, 0.59f };
    }

    CVector spinePos{};
    targetPed->GetTransformedBonePosition(spinePos, BONE_SPINE1, true);

    // TODO: Put in a function name e.g. 'HandleFreeMouseControl'?
    auto*      pad1   = CPad::GetPad(0);
    const auto fov    = m_fFOV / 80.0f;
    const auto amountMouseMoved = pad1->NewMouseControllerState.GetAmountMouseMoved();
    float verticalInput = amountMouseMoved.y * 4.0f;

    if (!amountMouseMoved.IsZero()) {
        m_fHorizontalAngle += -3.0f * amountMouseMoved.x * fov * CCamera::m_fMouseAccelHorzntl;
        m_fVerticalAngle += +4.0f * amountMouseMoved.y * fov * CCamera::m_fMouseAccelVertical;
    } else {
        const auto hv = (float)-pad1->LookAroundLeftRightOnPC();
        const auto vv = (float)pad1->LookAroundUpDownOnPC();
        verticalInput = vv;

        m_fHorizontalAngle += sq(hv) / 10000.0f * fov / 17.5f * CTimer::GetTimeStep() * (hv < 0.0f ? -1.0f : 1.0f);
        m_fVerticalAngle += sq(vv) / 22500.0f * fov / 14.0f * CTimer::GetTimeStep() * (vv < 0.0f ? -1.0f : 1.0f);
    }
    if (m_fHorizontalAngle > PI) {
        m_fHorizontalAngle -= TWO_PI;
    } else if (m_fHorizontalAngle < -PI) {
        m_fHorizontalAngle += TWO_PI;
    }
    m_fVerticalAngle = std::clamp(m_fVerticalAngle, DegreesToRadians(-89.5f), DegreesToRadians(60.0f));

    if (const auto* a = targetPed->m_pAttachedTo; targetPed->IsPlayer() && a) {
        float baseHeading = verticalInput;
        switch (targetPed->m_fTurretAngleA) {
        case 0u:
            baseHeading = a->GetHeading() + DegreesToRadians(90.0f);
            break;
        case 1u:
            baseHeading = a->GetHeading() + DegreesToRadians(180.0f);
            break;
        case 2u:
            baseHeading = a->GetHeading() + DegreesToRadians(-90.0f);
            break;
        case 3u:
            baseHeading = a->GetHeading();
            break;
        default:
            break;
        }

        auto relativeHeading = m_fHorizontalAngle - baseHeading;
        if (relativeHeading > PI) {
            relativeHeading -= TWO_PI;
        } else if (relativeHeading < -PI) {
            relativeHeading += TWO_PI;
        }
        m_fHorizontalAngle = baseHeading + std::clamp(relativeHeading, -targetPed->m_fTurretAngleB, targetPed->m_fTurretAngleB);
    }

    const CVector lookAt = m_vecSource + CVector{
        std::cos(m_fHorizontalAngle) * std::cos(m_fVerticalAngle),
        std::sin(m_fHorizontalAngle) * std::cos(m_fVerticalAngle),
        std::sin(m_fVerticalAngle)
    } * 3.0f;
    m_vecFront = (lookAt - m_vecSource).Normalized();
    m_vecSource += m_vecFront * 0.4f;
    TheCamera.m_fAlphaForPlayerAnim1rstPerson = m_fVerticalAngle;
    GetVectorsReadyForRW();

    auto* player = TheCamera.m_pTargetEntity->AsPed();
    player->m_fCurrentRotation = player->m_fAimingRotation = std::atan2(-m_vecFront.x, m_vecFront.y);
    player->SetHeading(player->m_fCurrentRotation);
    player->UpdateRwMatrix();

    if (m_nMode == MODE_SNIPER_RUNABOUT) {
        const auto zoomFactor = (CTimer::GetTimeStep() * 255.0f + 10000.0f) * 0.0001f;
        if (pad1->SniperZoomOut()) {
            m_fFOV *= zoomFactor;
        } else if (pad1->SniperZoomIn()) {
            m_fFOV /= zoomFactor;
        }
        TheCamera.SetMotionBlur(180, 255, 180, 120, eMotionBlurType::SNIPER);
        m_fFOV = std::clamp(m_fFOV, 15.0f, 70.0f);
    }
    m_bResetStatics = false;
    RwCameraSetNearClipPlane(Scene.m_pRwCamera, 0.05f);
}

// 0x517EA0
void CCam::Process_1stPerson(const CVector& target, float orientation, float speedVar, float speedVarWanted) {
    static auto& s_LastWheelieTime = StaticRef<float>(0x8CCD14);
    // Making sure player doesn't see below ground when flipped.
    // Name is made up cuz I found it funny to name it like that.
    static auto& s_GroundFaultProtection = StaticRef<float>(0xB7004C);

    gbFirstPersonRunThisFrame = true;

    m_fFOV = 70.0f;
    if (!m_pCamTargetEntity->GetRwObject()) {
        return;
    }

    if (m_bResetStatics) {
        m_fVerticalAngle   = 0.0f;
        m_fHorizontalAngle = [&] {
            if (m_pCamTargetEntity->GetIsTypePed()) {
                return m_pCamTargetEntity->AsPed()->m_fCurrentRotation + DegreesToRadians(90.0f);
            } else {
                return orientation;
            }
        }();
        m_fInitialPlayerOrientation = m_fHorizontalAngle;

        s_GroundFaultProtection                 = 0.0f;
        TheCamera.m_fAvoidTheGeometryProbsTimer = 0.0f;
    }

    if (m_pCamTargetEntity->GetIsTypePed()) {
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
    m_fFOV = 70.0f;
    const float tilt = DegreesToRadians(TheCamera.m_fAttachedCamAngle);
    auto* attached = TheCamera.m_pAttachedEntity;
    m_vecSource = attached->GetMatrix().TransformVector(TheCamera.m_vecAttachedCamOffset) + attached->GetPosition();
    if (TheCamera.m_bLookingAtVector) {
        m_vecFront = attached->GetMatrix().TransformVector(TheCamera.m_vecAttachedCamLookAt) + attached->GetPosition() - m_vecSource;
    } else {
        m_vecFront = TheCamera.m_pTargetEntity->GetPosition() - m_vecSource;
    }
    m_vecFront.Normalise();
    const auto right = CrossProduct(m_vecFront, CVector{0.0f, 0.0f, 1.0f}).Normalized();
    const auto up = CrossProduct(right, m_vecFront).Normalized();

    if (float waterLevel{}; CWaterLevel::GetWaterLevel(m_vecSource, waterLevel, true) && m_vecSource.z < waterLevel - 0.3f) {
        ApplyUnderwaterMotionBlur();
    }
    m_vecUp = up * std::cos(tilt) + right * std::sin(tilt);
    CWorld::pIgnoreEntity = nullptr;
}

// 0x5132D0
void CCam::GetTwoPlayerCameraPosition(float beta, CVector& source, CVector& front, CVector& target) {
    front = {-std::cos(beta) * std::cos(m_fVerticalAngle), -std::sin(beta) * std::cos(m_fVerticalAngle), std::sin(m_fVerticalAngle)};
    const auto horizontalFront = CVector{front.x, front.y, 0.0f}.Normalized();
    const auto& first = FindPlayerPed(PED_TYPE_PLAYER1)->GetPosition();
    const auto& second = FindPlayerPed(PED_TYPE_PLAYER2)->GetPosition();
    const auto distance = (first - second).Magnitude() * 0.67f + 7.0f;
    const auto separation = CVector{first.x - second.x, first.y - second.y, 0.0f}.Normalized();
    const auto weight = 0.5f - DotProduct(separation, horizontalFront) * 0.25f;
    target = first * weight + second * (1.0f - weight);
    source = target - front * distance;
    source.z += distance * 0.1f;
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
    static auto& blendSpeed = StaticRef<float>(0x8CCCB4);
    m_fFOV = 80.0f;
    auto* car1 = FindPlayerPed(0)->m_pVehicle;
    auto* car2 = FindPlayerPed(1)->m_pVehicle;
    const auto& position1 = car1->GetPosition();
    const auto& position2 = car2->GetPosition();
    const auto direction = (position2 - position1).Normalized();
    auto source1 = position1 - direction * 6.0f;
    auto source2 = position2 + direction * 6.0f;
    source1.z += car1->GetModelInfo()->GetColModel()->GetBoundingBox().m_vecMax.z + 1.0f;
    source2.z += car2->GetModelInfo()->GetColModel()->GetBoundingBox().m_vecMax.z + 1.0f;
    const auto front1 = (position2 - source1).Normalized();
    const auto front2 = (position1 - source2).Normalized();
    CVector side{source1.y - source2.y, source2.x - source1.x, 0.0f};
    side.Normalise();
    side.z = -0.1f;
    side.Normalise();

    const float angle = m_fTwoPlayerFocusBlend * PI;
    const float sine = std::sin(angle);
    const float blend = (std::cos(angle) + 1.0f) * 0.5f;
    const float distance = DistanceBetweenPoints(position1, position2);
    m_vecSource = source1 * blend + source2 * (1.0f - blend) - side * sine * distance * 0.75f;
    m_vecFront = ((front1 * blend + front2 * (1.0f - blend)) * (1.0f - sine) + side * sine).Normalized();
    m_vecTargetCoorsForFudgeInter = m_nTwoPlayerFocus == 0 ? position2 : position1;
    m_vecUp = {0.0f, 0.0f, 1.0f};
    m_vecFront.Normalise();
    const auto left = CrossProduct(m_vecUp, m_vecFront).Normalized();
    m_vecUp = CrossProduct(m_vecFront, left).Normalized();

    auto* focusedCar = FindPlayerPed(m_nTwoPlayerFocus)->m_pVehicle;
    auto* otherCar = FindPlayerPed((m_nTwoPlayerFocus - 1) & 1)->m_pVehicle;
    const auto horizontalFront = CVector{m_vecFront.x, m_vecFront.y, 0.0f}.Normalized();
    const float focusedSpeed = DotProduct(horizontalFront, focusedCar->m_vecMoveSpeed);
    if (focusedSpeed < -0.13f && focusedSpeed < DotProduct(-horizontalFront, otherCar->m_vecMoveSpeed)) {
        m_nTwoPlayerFocus = (m_nTwoPlayerFocus - 1) & 1;
    }
    if (m_nTwoPlayerFocus == 0) {
        m_fTwoPlayerFocusBlend = std::max(m_fTwoPlayerFocusBlend - CTimer::GetTimeStep() * blendSpeed, 0.0f);
    } else {
        m_fTwoPlayerFocusBlend = std::min(m_fTwoPlayerFocusBlend + CTimer::GetTimeStep() * blendSpeed, 1.0f);
    }
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
bool CCam::Process_DW_BirdyCam(bool) {
    auto& lastCamMode = StaticRef<int32>(0x8CC488);
    auto& sceneStartTime = StaticRef<uint32>(0x8CCBA0);
    auto& clearFrames = StaticRef<int32>(0xB7007C);
    auto& positions = StaticRef<std::array<CVector, 2>>(0xB70080);
    auto& initialized = StaticRef<uint8>(0xB70098);
    auto& exitCam = StaticRef<bool>(0xB6EC72);
    const auto maxClearFrames = StaticRef<int32>(0x8CCD74);

    TheCamera.m_bUseNearClipScript = false;
    if (!m_pCamTargetEntity || !m_pCamTargetEntity->IsVehicle()) {
        return false;
    }
    CEntity* entity{};
    CVehicle* vehicle{};
    CVector target, source, up, right, forward, velocity, angularVelocity;
    float speed{}, angularSpeed{};
    CColSphere sphere{};
    GetCoreDataForDWCineyCamMode(entity, vehicle, target, source, up, right, forward, velocity, speed, angularVelocity, angularSpeed, sphere);
    const auto now = CTimer::GetTimeInMS();
    initialized |= 1;
    if (!(initialized & 2)) {
        initialized |= 2;
        clearFrames = maxClearFrames;
    }

    if (lastCamMode != MODE_DW_BIRDY || gLastFrameProcessedDWCineyCam < CTimer::GetFrameCounter() - 1u) {
        lastCamMode = MODE_DW_BIRDY;
        gDWCineyCamSceneEndTime = now + StaticRef<uint32>(0x8CCBB0);
        sceneStartTime = now;
        clearFrames = maxClearFrames;
        exitCam = false;
        gHandShaker[0].Reset();

        CEntity* ahead[128];
        CEntity* behind[128];
        int16 aheadCount{}, behindCount{};
        const auto aheadRadius = StaticRef<float>(0x8CCD70);
        const auto behindRadius = StaticRef<float>(0x8CCD68);
        const auto aheadCentre = target + forward * aheadRadius * StaticRef<float>(0x8CCD6C);
        const auto behindCentre = target - forward * behindRadius * StaticRef<float>(0x8CCD64);
        CWorld::FindObjectsInRange(aheadCentre, aheadRadius, true, &aheadCount, 127, ahead, false, false, false, true, true);
        CWorld::FindObjectsInRange(behindCentre, behindRadius, true, &behindCount, 127, behind, false, false, false, true, true);

        const auto isSuitable = [](CEntity* candidate) {
            return (candidate->m_bIsStatic || candidate->m_bIsStaticWaitingForCollision)
                && candidate->GetMatrix().GetUp().z > 0.9f
                && IsLampPost((eModelID)candidate->GetModelIndex());
        };
        CEntity* aheadPosts[128];
        CEntity* behindPosts[128];
        int16 aheadPostCount{}, behindPostCount{};
        for (int16 i = 0; i < aheadCount; i++) {
            if (isSuitable(ahead[i])) {
                aheadPosts[aheadPostCount++] = ahead[i];
            }
        }
        for (int16 i = 0; i < behindCount; i++) {
            if (isSuitable(behind[i])) {
                behindPosts[behindPostCount++] = behind[i];
            }
        }
        const auto getPosition = [](CEntity* post) {
            const auto& bounds = post->GetColModel()->m_boundBox;
            auto position = post->GetMatrix().TransformPoint(bounds.m_vecMax);
            const auto maxOffset = bounds.m_vecMax.z - bounds.m_vecMin.z * 0.5f;
            position.z -= 1.0f + (maxOffset - 1.0f) * (float)CGeneral::GetRandomNumber() * StaticRef<float>(0x858C7C);
            return position;
        };
        bool found = false;
        for (int16 i = 0; i < aheadPostCount && !found; i++) {
            if (!isSuitable(aheadPosts[i])) {
                continue;
            }
            const auto start = getPosition(aheadPosts[i]);
            const auto nearTarget = target + (start - target).Normalized();
            if (std::abs(StaticRef<double>(0x859EF8)) >= StaticRef<float>(0x8CCD60)) {
                continue;
            }
            // The inner search starts at the outer candidate's index in the original.
            for (int16 j = i; j < behindPostCount && !found; j++) {
                if (!isSuitable(behindPosts[j])) {
                    continue;
                }
                const auto end = getPosition(behindPosts[j]);
                if (CWorld::GetIsLineOfSightClear(start, end, true, false, false, false, false, true, true)
                    && CWorld::GetIsLineOfSightClear(end, nearTarget, true, false, false, false, false, true, true)) {
                    source = positions[0] = start;
                    positions[1] = end;
                    found = true;
                }
            }
        }
        if (!found) {
            exitCam = true;
            return false;
        }
    }

    const auto t = (float)(int32)(now - sceneStartTime) / (float)(int32)(gDWCineyCamSceneEndTime - sceneStartTime);
    const auto offset = (positions[1] - positions[0]).Normalized() * StaticRef<float>(0x8CCD5C);
    const auto start = positions[0] + offset * 2.0f;
    const auto end = positions[1] - offset;
    if (!exitCam) {
        const auto blend = (1.0f + std::sin(DegreesToRadians(270.0f - t * 180.0f))) * 0.5f;
        source = end + (start - end) * blend;
    }
    if (IsTimeToExitThisDWCineyCamMode(MODE_BEHINDBOAT, source, target, t, false)) {
        exitCam = true;
        return false;
    }

    CColPoint collision{};
    CEntity* hitEntity{};
    CWorld::pIgnoreEntity = entity;
    const auto obstructed = CWorld::ProcessLineOfSight(target, source, collision, hitEntity, true, true, false, false, false, false, false, false);
    CWorld::pIgnoreEntity = nullptr;
    if (obstructed) {
        if (clearFrames-- == 0) {
            exitCam = true;
            return false;
        }
    } else if (clearFrames++ > maxClearFrames) {
        clearFrames = maxClearFrames;
    }
    Finalise_DW_CineyCams(source, target, 0.0f, 70.0f, 0.3f, 0.0f);
    return true;
}

// 0x51B120
bool CCam::Process_DW_CamManCam(bool) {
    auto& lastCamMode = StaticRef<int32>(0x8CC488);
    auto& sceneStartTime = StaticRef<uint32>(0x8CCBA0);
    auto& cameraPosition = StaticRef<CVector>(0xB70068);
    auto& clearFrames = StaticRef<int32>(0xB70074);
    auto& initialized = StaticRef<uint32>(0xB70078);
    auto& exitCam = StaticRef<bool>(0xB6EC71);
    const auto maxClearFrames = StaticRef<int32>(0x8CCD58);

    TheCamera.m_bUseNearClipScript = false;
    if (!m_pCamTargetEntity || !m_pCamTargetEntity->IsVehicle()) {
        return false;
    }
    CEntity* entity{};
    CVehicle* vehicle{};
    CVector target, source, up, right, forward, velocity, angularVelocity;
    float speed{}, angularSpeed{};
    CColSphere sphere{};
    GetCoreDataForDWCineyCamMode(entity, vehicle, target, source, up, right, forward, velocity, speed, angularVelocity, angularSpeed, sphere);
    const auto now = CTimer::GetTimeInMS();
    initialized |= 1;
    if (!(initialized & 2)) {
        initialized |= 2;
        clearFrames = maxClearFrames;
    }
    if (lastCamMode != MODE_DW_CAM_MAN || gLastFrameProcessedDWCineyCam < CTimer::GetFrameCounter() - 1u) {
        lastCamMode = MODE_DW_CAM_MAN;
        gDWCineyCamSceneEndTime = now + StaticRef<uint32>(0x8CCBAC);
        sceneStartTime = now;
        clearFrames = maxClearFrames;
        exitCam = false;
        gHandShaker[0].Reset();

        const auto searchRadius = StaticRef<float>(0x8CCD54);
        const auto searchCentre = target + forward * searchRadius;
        CEntity* candidates[16];
        int16 count{};
        CWorld::FindObjectsInRange(searchCentre, searchRadius, true, &count, 15, candidates, false, false, false, true, true);
        bool found = false;
        float closestDistance = 10000.0f;
        for (int16 i = 0; i < count; i++) {
            auto* candidate = candidates[i];
            if (!candidate->m_bIsStatic && !candidate->m_bIsStaticWaitingForCollision) {
                continue;
            }
            if (candidate->GetMatrix().GetUp().z <= 0.9f || !IsLampPost((eModelID)candidate->GetModelIndex())) {
                continue;
            }
            const auto distance = (candidate->GetPosition() - target).Magnitude2D();
            if (distance >= closestDistance || distance <= StaticRef<float>(0x8CCD50)) {
                continue;
            }
            const auto& bounds = candidate->GetColModel()->m_boundBox;
            auto position = candidate->GetMatrix().TransformPoint(bounds.m_vecMax);
            position.z -= bounds.m_vecMax.z;
            position.z += bounds.m_vecMin.z * 0.5f;
            const auto nearTarget = target + (position - target).Normalized();
            // The executable compares a constant here, rather than the direction's height.
            if (std::abs(StaticRef<double>(0x859EF8)) >= StaticRef<float>(0x8CCD4C)
                || !CWorld::GetIsLineOfSightClear(position, nearTarget, true, false, false, false, false, true, true)) {
                continue;
            }
            found = true;
            closestDistance = distance;
            source = cameraPosition = position;
        }
        if (!found) {
            exitCam = true;
            return false;
        }
    }

    const auto t = (float)(int32)(now - sceneStartTime) / (float)(int32)(gDWCineyCamSceneEndTime - sceneStartTime);
    if (!exitCam) {
        source = cameraPosition + (target - cameraPosition).Normalized() * StaticRef<float>(0x8CCD48);
    }
    const auto distanceFraction = std::clamp((target - source).Magnitude() / StaticRef<float>(0x8CCD44), 0.0f, 1.0f);
    const auto distanceBlend = (1.0f + std::sin(DegreesToRadians(270.0f - distanceFraction * 180.0f))) * 0.5f;
    auto fov = lerp(StaticRef<float>(0x8CCD3C), StaticRef<float>(0x8CCD40), distanceBlend);
    const auto introDuration = StaticRef<float>(0x8CCD38);
    if (t < introDuration) {
        const auto introFraction = std::clamp(t / introDuration, 0.0f, 1.0f);
        const auto introBlend = (1.0f + std::sin(DegreesToRadians(270.0f - introFraction * 180.0f))) * 0.5f;
        fov = lerp(StaticRef<float>(0x8CCD34), fov, introBlend);
    }
    if (IsTimeToExitThisDWCineyCamMode(MODE_CHRIS, source, target, t, false)) {
        exitCam = true;
        return false;
    }

    CColPoint collision{};
    CEntity* hitEntity{};
    CWorld::pIgnoreEntity = entity;
    const auto obstructed = CWorld::ProcessLineOfSight(target, source, collision, hitEntity, true, true, false, false, false, false, false, false);
    CWorld::pIgnoreEntity = nullptr;
    if (obstructed) {
        if (clearFrames-- == 0) {
            exitCam = true;
            return false;
        }
    } else if (clearFrames++ > maxClearFrames) {
        clearFrames = maxClearFrames;
    }
    const auto shake = std::clamp(std::max(velocity.Magnitude() * StaticRef<float>(0x8CCD30), StaticRef<float>(0x8CCD2C)), 0.0f, 1.0f);
    Finalise_DW_CineyCams(source, target, 0.0f, fov, 10.0f - fov * (1.0f / 70.0f) * 9.7f, shake);
    return true;
}

// 0x51A740
bool CCam::Process_DW_HeliChaseCam(bool) {
    auto& state = StaticRef<DWHeliChaseState>(0xB6FEC0);
    auto& lastMode = StaticRef<int32>(0x8CC488);
    auto& startTime = StaticRef<uint32>(0x8CCBA0);
    auto& exitCam = StaticRef<bool>(0xB6EC70);
    auto& obstructionPosition = StaticRef<CVector>(0xB70058);
    auto& obstructionFrames = StaticRef<int32>(0x8CCD24);
    TheCamera.m_bUseNearClipScript = false;
    // The original consumes a random number to select from a single configuration.
    CGeneral::GetRandomNumber();
    if (!m_pCamTargetEntity || !m_pCamTargetEntity->IsVehicle()) {
        return false;
    }
    CEntity* entity{};
    CVehicle* vehicle{};
    CVector target, source, up, right, forward, velocity, angularVelocity;
    float speed{}, angularSpeed{};
    CColSphere sphere{};
    GetCoreDataForDWCineyCamMode(entity, vehicle, target, source, up, right, forward, velocity, speed, angularVelocity, angularSpeed, sphere);
    const auto now = CTimer::GetTimeInMS();
    if (lastMode != MODE_DW_HELI_CHASE || gLastFrameProcessedDWCineyCam < CTimer::GetFrameCounter() - 1u) {
        lastMode = MODE_DW_HELI_CHASE;
        startTime = now;
        gDWCineyCamSceneEndTime = now + StaticRef<uint32>(0x8CCBA8);
        exitCam = false;
        state.SetDefaults();
        state.Randomise();
        gHandShaker[0].Reset();

        bool found = false;
        for (int32 i = 0; i < state.searchAttempts; i++) {
            state.start = target - forward * state.backwardDistance;
            state.end = target + forward * state.forwardDistance;
            state.start.z += state.height;
            state.end.z += state.height;
            const auto startSide = CGeneral::GetRandomNumber() < 0x3FFF ? -1.0f : 1.0f;
            const auto endSide = CGeneral::GetRandomNumber() < 0x3FFF ? -1.0f : 1.0f;
            const CVector horizontalRight{right.x, right.y, 0.0f};
            state.start += horizontalRight * state.sideDistance * startSide;
            state.end += horizontalRight * state.sideDistance * endSide;
            if (CWorld::TestSphereAgainstWorld(state.start, state.searchSphereRadius, nullptr, true, true, false, false, false, false)) {
                continue;
            }
            CColPoint collision{};
            CEntity* hitEntity{};
            CWorld::pIgnoreEntity = entity;
            const auto obstructed = CWorld::ProcessLineOfSight(target, state.start, collision, hitEntity, true, true, false, false, false, false, false, false);
            CWorld::pIgnoreEntity = nullptr;
            if (!obstructed) {
                state.skipZoomIn = CGeneral::GetRandomNumber() < 0x3FFF;
                state.zoomingOut = CGeneral::GetRandomNumber() < 0x3FFF;
                found = true;
                break;
            }
        }
        if (!found) {
            state.targetFrozen = state.sourceObstructed = state.targetObstructed = state.skipZoomIn = state.zoomingOut = false;
            state.FOVRange = state.wideFOV - state.closeFOV;
            state.clearFrames = state.maxClearFrames;
            state.frozenFrames = state.maxFrozenFrames;
            exitCam = true;
            return false;
        }
    }
    if (exitCam) {
        return false;
    }

    const auto t = (float)(int32)(now - startTime) / (float)(int32)(gDWCineyCamSceneEndTime - startTime);
    source = state.start + (state.end - state.start) * t;
    target += forward * (speed * state.lookAhead) + forward;
    const auto distance2D = (target - source).Magnitude2D();
    if (distance2D < state.minimumDistance) {
        const auto direction = (target - source) / distance2D;
        source.x = target.x - direction.x * state.minimumDistance;
        source.y = target.y - direction.y * state.minimumDistance;
    }
    const auto smoothFraction = [](float fraction) {
        return (1.0f + std::sin(DegreesToRadians(270.0f - fraction * 180.0f))) * 0.5f;
    };
    auto fov = state.closeFOV;
    if (t < state.zoomInFraction && !state.skipZoomIn) {
        fov = lerp(state.wideFOV, state.closeFOV, smoothFraction(t / state.zoomInFraction));
    }
    const auto distance = (source - target).Magnitude();
    if (distance > state.zoomDistanceStart) {
        const auto fraction = std::clamp((distance - state.zoomDistanceStart) / (state.zoomDistanceEnd - state.zoomDistanceStart), 0.0f, 1.0f);
        fov -= smoothFraction(fraction) * state.distanceFOVReduction;
    }
    const auto roll = t * state.roll;

    if (state.sourceObstructed || CWorld::TestSphereAgainstWorld(source, StaticRef<float>(0x8CCD28), nullptr, true, true, false, false, false, false)) {
        StaticRef<uint32>(0xB70064) |= 1;
        if (!state.sourceObstructed) {
            obstructionPosition = source;
            state.sourceObstructed = true;
            obstructionFrames = 100;
        }
        if (obstructionFrames < 0) {
            --obstructionFrames;
            exitCam = true;
            return false;
        }
        source = obstructionPosition + (source - obstructionPosition) * StaticRef<float>(0x8CCD20);
        --obstructionFrames;
    }

    if (!state.targetFrozen) {
        CColPoint collision{};
        CEntity* hitEntity{};
        CWorld::pIgnoreEntity = entity;
        const auto obstructed = CWorld::ProcessLineOfSight(target, source, collision, hitEntity, true, true, false, false, false, false, false, false);
        CWorld::pIgnoreEntity = nullptr;
        if (obstructed) {
            state.targetObstructed = true;
            if (!state.zoomingOut && state.clearFrames < state.maxClearFrames / 4) {
                state.zoomOutStartFOV = fov;
                state.zoomingOut = true;
                state.zoomOutStartTime = now;
                state.zoomOutEndTime = now + state.zoomOutDuration;
            }
            if (state.clearFrames-- == 0) {
                state.frozenTarget = target;
                state.targetFrozen = true;
            }
        } else {
            state.clearFrames = std::min(state.clearFrames + 1, state.maxClearFrames);
        }
    } else {
        target = state.frozenTarget;
        if (state.frozenFrames-- == 0) {
            exitCam = true;
            return false;
        }
    }
    if (!state.zoomingOut && t >= state.zoomOutFraction) {
        state.zoomOutStartFOV = fov;
        state.zoomingOut = true;
        state.zoomOutStartTime = now;
        state.zoomOutEndTime = now + state.zoomOutDuration;
    }
    if (state.zoomingOut) {
        const auto fraction = std::clamp(
            ((float)(int32)now - (float)(int32)state.zoomOutStartTime) / ((float)(int32)state.zoomOutEndTime - (float)(int32)state.zoomOutStartTime),
            0.0f, 1.0f
        );
        fov = lerp(state.zoomOutStartFOV, state.wideFOV, smoothFraction(fraction));
    }
    if (IsTimeToExitThisDWCineyCamMode(MODE_FOLLOW_PED_WITH_BIND, source, target, t, false)) {
        exitCam = true;
        return false;
    }
    Finalise_DW_CineyCams(source, target, roll, fov, state.nearClip, 1.0f);
    return true;
}

// 0x51C760
bool CCam::Process_DW_PlaneCam1(bool) {
    static auto& lastCamMode       = StaticRef<int32>(0x8CC488);
    static auto& sceneStartTime    = StaticRef<uint32>(0x8CCBA0);
    static auto& sceneDuration     = StaticRef<uint32>(0x8CCBC0);
    static auto& maxClearFrames    = StaticRef<int32>(0x8CCD98);
    static auto& heightOffset      = StaticRef<float>(0x8CCDA8);
    static auto& forwardOffset     = StaticRef<float>(0x8CCDB0);
    static auto& minHeight         = StaticRef<float>(0x8CCDBC);
    static auto& exitCam           = StaticRef<bool>(0xB6EC76);
    static auto& clearFrames       = StaticRef<int32>(0xB700B8);
    static auto& staticsInitialized = StaticRef<uint32>(0xB700BC);

    static auto& waveAmplitude = StaticRef<float>(0x8CCD9C);
    static auto& waveFrequency = StaticRef<float>(0x8CCDA0);
    static auto& sideOffset = StaticRef<float>(0x8CCDAC);
    static auto& sideSign = StaticRef<float>(0x8CCDB4);
    static auto& heightSign = StaticRef<float>(0xB700C0);
    static auto& heightTravel = StaticRef<float>(0x8CCDA4);
    static auto& minGroundDistance = StaticRef<float>(0x8CCDB8);

    TheCamera.m_bUseNearClipScript = false;
    if (!m_pCamTargetEntity || !m_pCamTargetEntity->IsVehicle()) {
        return false;
    }

    CEntity* entity{};
    CVehicle* vehicle{};
    CVector dst, src, targetUp, targetRight, targetFwd, targetVel, targetAngVel;
    float targetSpeed{}, targetAngSpeed{};
    CColSphere colSphere{};
    GetCoreDataForDWCineyCamMode(
        entity, vehicle, dst, src, targetUp, targetRight, targetFwd,
        targetVel, targetSpeed, targetAngVel, targetAngSpeed, colSphere
    );

    if (dst.z < minHeight) {
        exitCam = true;
        return false;
    }

    const auto now = CTimer::GetTimeInMS();
    if (lastCamMode != MODE_DW_PLANECAM1 || gLastFrameProcessedDWCineyCam < CTimer::GetFrameCounter() - 1u) {
        lastCamMode = MODE_DW_PLANECAM1;
        gDWCineyCamSceneEndTime = now + sceneDuration;
        exitCam = false;
        sceneStartTime = now;

        CColPoint colPoint{};
        CEntity* hitEntity{};
        CWorld::pIgnoreEntity = entity;
        const auto obstructed = CWorld::ProcessLineOfSight(dst, src, colPoint, hitEntity, true, true, false, false, false, false, false, false);
        CWorld::pIgnoreEntity = nullptr;
        if (obstructed) {
            exitCam = true;
            return false;
        }
        heightSign = CGeneral::GetRandomNumber() < 0x3FFF ? -1.0f : 1.0f;
        if (CGeneral::GetRandomNumber() < 0x3FFF) {
            sideSign = -1.0f;
        }
    } else if (TheCamera.GetRoughDistanceToGround() < minGroundDistance) {
        exitCam = true;
        return false;
    }

    const auto t = static_cast<float>(static_cast<int32>(now - sceneStartTime))
                 / static_cast<float>(static_cast<int32>(gDWCineyCamSceneEndTime - sceneStartTime));
    const auto forward = targetFwd.Normalized();
    const auto right = targetRight.Normalized() * sideSign;
    src = dst + forward * forwardOffset + right * sideOffset + targetUp * heightOffset;
    src += targetUp * heightTravel * (t - 0.5f) * heightSign;
    const auto waveT = (static_cast<double>(now) - sceneStartTime)
                    / (static_cast<double>(gDWCineyCamSceneEndTime) - sceneStartTime);
    const auto wave = std::sin(waveFrequency * waveT * 360.0 * 0.0174532924f);
    src += targetUp * waveAmplitude * static_cast<float>(wave);

    CColPoint colPoint{};
    CEntity* hitEntity{};
    CWorld::pIgnoreEntity = entity;
    const auto obstructed = CWorld::ProcessLineOfSight(dst, src, colPoint, hitEntity, true, true, false, false, false, false, false, false);
    CWorld::pIgnoreEntity = nullptr;

    if (!(staticsInitialized & 1)) {
        staticsInitialized |= 1;
        clearFrames = maxClearFrames;
    }
    if (obstructed) {
        if (clearFrames-- == 0) {
            exitCam = true;
            return false;
        }
    } else if (clearFrames++ > maxClearFrames) {
        // The original compares before incrementing, allowing maxClearFrames + 1.
        clearFrames = maxClearFrames;
    }

    if (IsTimeToExitThisDWCineyCamMode(MODE_BLOOD_ON_THE_TRACKS, src, dst, t, false)) {
        exitCam = true;
        return false;
    }
    Finalise_DW_CineyCams(src, dst, 0.0f, 70.0f, 5.0f, 1.0f);
    return true;
}

// 0x51CC30
bool CCam::Process_DW_PlaneCam2(bool) {
    static auto& lastCamMode       = StaticRef<int32>(0x8CC488);
    static auto& sceneStartTime    = StaticRef<uint32>(0x8CCBA0);
    static auto& sceneDuration     = StaticRef<uint32>(0x8CCBC4);
    static auto& maxClearFrames    = StaticRef<int32>(0x8CCDC0);
    static auto& heightOffset      = StaticRef<float>(0x8CCDCC);
    static auto& forwardOffset     = StaticRef<float>(0x8CCDD4);
    static auto& minHeight         = StaticRef<float>(0x8CCDDC);
    static auto& exitCam           = StaticRef<bool>(0xB6EC77);
    static auto& clearFrames       = StaticRef<int32>(0xB700C4);
    static auto& staticsInitialized = StaticRef<uint32>(0xB700C8);

    static auto& waveAmplitude = StaticRef<float>(0x8CCDC4);
    static auto& waveFrequency = StaticRef<float>(0x8CCDC8);
    static auto& sideOffset = StaticRef<float>(0x8CCDD0);
    static auto& sideSign = StaticRef<float>(0x8CCDD8);
    static auto& forwardSign = StaticRef<float>(0xB700CC);
    static auto& unusedSign = StaticRef<float>(0xB700D0);

    TheCamera.m_bUseNearClipScript = false;
    if (!m_pCamTargetEntity || !m_pCamTargetEntity->IsVehicle()) {
        return false;
    }

    CEntity* entity{};
    CVehicle* vehicle{};
    CVector dst, src, targetUp, targetRight, targetFwd, targetVel, targetAngVel;
    float targetSpeed{}, targetAngSpeed{};
    CColSphere colSphere{};
    GetCoreDataForDWCineyCamMode(
        entity, vehicle, dst, src, targetUp, targetRight, targetFwd,
        targetVel, targetSpeed, targetAngVel, targetAngSpeed, colSphere
    );

    if (dst.z < minHeight) {
        exitCam = true;
        return false;
    }

    const auto now = CTimer::GetTimeInMS();
    if (lastCamMode != MODE_DW_PLANECAM2 || gLastFrameProcessedDWCineyCam < CTimer::GetFrameCounter() - 1u) {
        lastCamMode = MODE_DW_PLANECAM2;
        gDWCineyCamSceneEndTime = now + sceneDuration;
        exitCam = false;
        sceneStartTime = now;

        CColPoint colPoint{};
        CEntity* hitEntity{};
        CWorld::pIgnoreEntity = entity;
        const auto obstructed = CWorld::ProcessLineOfSight(dst, src, colPoint, hitEntity, true, true, false, false, false, false, false, false);
        CWorld::pIgnoreEntity = nullptr;
        if (obstructed) {
            exitCam = true;
            return false;
        }
        unusedSign = CGeneral::GetRandomNumber() < 0x3FFF ? -1.0f : 1.0f;
        forwardSign = CGeneral::GetRandomNumber() < 0x3FFF ? -1.0f : 1.0f;
        if (CGeneral::GetRandomNumber() < 0x3FFF) {
            sideSign = -1.0f;
        }
    }

    const auto t = static_cast<float>(static_cast<int32>(now - sceneStartTime))
                 / static_cast<float>(static_cast<int32>(gDWCineyCamSceneEndTime - sceneStartTime));
    const auto forward = targetFwd.Normalized() * (1.0f - (t + t)) * forwardSign;
    const auto right = targetRight.Normalized() * (1.0f - t) * sideSign;
    src = dst + forward * forwardOffset + right * sideOffset + targetUp * heightOffset;
    const auto waveT = (static_cast<double>(now) - sceneStartTime)
                    / (static_cast<double>(gDWCineyCamSceneEndTime) - sceneStartTime);
    const auto wave = std::sin(waveFrequency * waveT * 360.0 * 0.0174532924f);
    src += targetUp * waveAmplitude * static_cast<float>(wave);

    CColPoint colPoint{};
    CEntity* hitEntity{};
    CWorld::pIgnoreEntity = entity;
    const auto obstructed = CWorld::ProcessLineOfSight(dst, src, colPoint, hitEntity, true, true, false, false, false, false, false, false);
    CWorld::pIgnoreEntity = nullptr;

    if (!(staticsInitialized & 1)) {
        staticsInitialized |= 1;
        clearFrames = maxClearFrames;
    }
    if (obstructed) {
        if (clearFrames-- == 0) {
            exitCam = true;
            return false;
        }
    } else if (clearFrames++ > maxClearFrames) {
        // The original compares before incrementing, allowing maxClearFrames + 1.
        clearFrames = maxClearFrames;
    }

    if (IsTimeToExitThisDWCineyCamMode(MODE_IM_THE_PASSENGER_WOOWOO, src, dst, t, false)) {
        exitCam = true;
        return false;
    }
    Finalise_DW_CineyCams(src, dst, 0.0f, 70.0f, 5.0f, 1.0f);
    return true;
}

// 0x51D100
bool CCam::Process_DW_PlaneCam3(bool) {
    static auto& lastCamMode       = StaticRef<int32>(0x8CC488);
    static auto& sceneStartTime    = StaticRef<uint32>(0x8CCBA0);
    static auto& sceneDuration     = StaticRef<uint32>(0x8CCBC8);
    static auto& maxClearFrames    = StaticRef<int32>(0x8CCDE0);
    static auto& heightOffset      = StaticRef<float>(0x8CCDE4);
    static auto& forwardOffset     = StaticRef<float>(0x8CCDE8);
    static auto& minHeight         = StaticRef<float>(0x8CCDEC);
    static auto& exitCam           = StaticRef<bool>(0xB6EC78);
    static auto& clearFrames       = StaticRef<int32>(0xB700D4);
    static auto& staticsInitialized = StaticRef<uint32>(0xB700D8);

    TheCamera.m_bUseNearClipScript = false;
    if (!m_pCamTargetEntity || !m_pCamTargetEntity->IsVehicle()) {
        return false;
    }

    CEntity* entity{};
    CVehicle* vehicle{};
    CVector dst, src, targetUp, targetRight, targetFwd, targetVel, targetAngVel;
    float targetSpeed{}, targetAngSpeed{};
    CColSphere colSphere{};
    GetCoreDataForDWCineyCamMode(
        entity, vehicle, dst, src, targetUp, targetRight, targetFwd,
        targetVel, targetSpeed, targetAngVel, targetAngSpeed, colSphere
    );

    if (dst.z < minHeight) {
        exitCam = true;
        return false;
    }

    const auto now = CTimer::GetTimeInMS();
    if (lastCamMode != MODE_DW_PLANECAM3 || gLastFrameProcessedDWCineyCam < CTimer::GetFrameCounter() - 1u) {
        lastCamMode = MODE_DW_PLANECAM3;
        gDWCineyCamSceneEndTime = now + sceneDuration;
        exitCam = false;
        sceneStartTime = now;

        CColPoint colPoint{};
        CEntity* hitEntity{};
        CWorld::pIgnoreEntity = entity;
        const auto obstructed = CWorld::ProcessLineOfSight(dst, src, colPoint, hitEntity, true, true, false, false, false, false, false, false);
        CWorld::pIgnoreEntity = nullptr;
        if (obstructed) {
            exitCam = true;
            return false;
        }
    }

    const auto t = static_cast<float>(static_cast<int32>(now - sceneStartTime))
                 / static_cast<float>(static_cast<int32>(gDWCineyCamSceneEndTime - sceneStartTime));
    const auto& bounds = entity->GetColModel()->GetBoundingBox();
    forwardOffset = (bounds.m_vecMax.y - bounds.m_vecMin.y) * 0.5f;
    forwardOffset += forwardOffset;
    src = dst + targetFwd * forwardOffset + targetUp * heightOffset;

    CColPoint colPoint{};
    CEntity* hitEntity{};
    CWorld::pIgnoreEntity = entity;
    const auto obstructed = CWorld::ProcessLineOfSight(dst, src, colPoint, hitEntity, true, true, false, false, false, false, false, false);
    CWorld::pIgnoreEntity = nullptr;

    if (!(staticsInitialized & 1)) {
        staticsInitialized |= 1;
        clearFrames = maxClearFrames;
    }
    if (obstructed) {
        if (clearFrames-- == 0) {
            exitCam = true;
            return false;
        }
    } else if (clearFrames++ > maxClearFrames) {
        // The original compares before incrementing, allowing maxClearFrames + 1.
        clearFrames = maxClearFrames;
    }

    if (IsTimeToExitThisDWCineyCamMode(MODE_SYPHON_CRIM_IN_FRONT, src, dst, t, false)) {
        exitCam = true;
        return false;
    }
    Finalise_DW_CineyCams(src, dst, 0.0f, 70.0f, 5.0f, 1.0f);
    return true;
}

// 0x51C250
bool CCam::Process_DW_PlaneSpotterCam(bool) {
    static auto& lastCamMode = StaticRef<int32>(0x8CC488);
    static auto& sceneStartTime = StaticRef<uint32>(0x8CCBA0);
    static auto& sceneDuration = StaticRef<uint32>(0x8CCBB4);
    static auto& maxClearFrames = StaticRef<int32>(0x8CCD78);
    static auto& zoomStartFraction = StaticRef<float>(0x8CCD7C);
    static auto& initialFov = StaticRef<float>(0x8CCD80);
    static auto& distantFov = StaticRef<float>(0x8CCD84);
    static auto& zoomDistance = StaticRef<float>(0x8CCD88);
    static auto& searchDistance = StaticRef<float>(0x8CCD8C);
    static auto& searchDepth = StaticRef<float>(0x8CCD90);
    static auto& searchAttempts = StaticRef<int32>(0x8CCD94);
    static auto& clearFrames = StaticRef<int32>(0xB7009C);
    static auto& startingFov = StaticRef<float>(0xB700A0);
    static auto& useZoom = StaticRef<bool>(0xB700A4);
    static auto& cameraPosition = StaticRef<CVector>(0xB700A8);
    static auto& initialized = StaticRef<uint32>(0xB700B4);
    static auto& exitCam = StaticRef<bool>(0xB6EC73);

    TheCamera.m_bUseNearClipScript = false;
    if (!m_pCamTargetEntity || !m_pCamTargetEntity->IsVehicle()) {
        return false;
    }
    CEntity* entity{};
    CVehicle* vehicle{};
    CVector target, source, up, right, forward, velocity, angularVelocity;
    float speed{}, angularSpeed{};
    CColSphere sphere{};
    GetCoreDataForDWCineyCamMode(entity, vehicle, target, source, up, right, forward, velocity, speed, angularVelocity, angularSpeed, sphere);
    const auto now = CTimer::GetTimeInMS();
    initialized |= 1;
    if (lastCamMode != MODE_DW_PLANE_SPOTTER || gLastFrameProcessedDWCineyCam < CTimer::GetFrameCounter() - 1u) {
        lastCamMode = MODE_DW_PLANE_SPOTTER;
        gDWCineyCamSceneEndTime = now + sceneDuration;
        exitCam = false;
        sceneStartTime = now;
        if (searchAttempts > 0) {
            bool found = false;
            for (int32 i = 0; i < searchAttempts; i++) {
                source = target;
                source.z -= searchDepth;
                source.x += CGeneral::GetRandomNumberInRange(searchDistance * 0.5f, searchDistance);
                source.y += CGeneral::GetRandomNumberInRange(searchDistance * 0.5f, searchDistance);
                CColPoint collision{};
                CEntity* hitEntity{};
                CWorld::pIgnoreEntity = entity;
                found = CWorld::ProcessLineOfSight(target, source, collision, hitEntity, true, true, false, false, false, false, false, false);
                CWorld::pIgnoreEntity = nullptr;
                if (found) {
                    cameraPosition = collision.m_vecPoint;
                    cameraPosition.z += 2.0f;
                    break;
                }
            }
            if (!found) {
                exitCam = true;
                return false;
            }
        }
        useZoom = CGeneral::GetRandomNumber() < 0x3FFF;
    }

    const float t = (float)(int32)(now - sceneStartTime) / (float)(int32)(gDWCineyCamSceneEndTime - sceneStartTime);
    source = cameraPosition;
    if ((source - target).Magnitude2D() < 5.0f) {
        exitCam = true;
        return false;
    }
    const auto ease = [](float ratio) {
        return (std::sin(DegreesToRadians(270.0f - std::clamp(ratio, 0.0f, 1.0f) * 180.0f)) + 1.0f) * 0.5f;
    };
    float fov = 70.0f;
    if (useZoom) {
        fov = (distantFov - initialFov) * ease(DistanceBetweenPoints(target, source) / zoomDistance) + initialFov;
        if (t < zoomStartFraction) {
            if (!(initialized & 2)) {
                initialized |= 2;
                startingFov = initialFov;
            }
            fov = (fov - startingFov) * ease(t / zoomStartFraction) + startingFov;
        }
    }
    CColPoint collision{};
    CEntity* hitEntity{};
    CWorld::pIgnoreEntity = entity;
    const bool obstructed = CWorld::ProcessLineOfSight(target, source, collision, hitEntity, true, true, false, false, false, false, false, false);
    CWorld::pIgnoreEntity = nullptr;
    if (!(initialized & 4)) {
        initialized |= 4;
        clearFrames = maxClearFrames;
    }
    if (obstructed) {
        if (clearFrames-- == 0) {
            exitCam = true;
            return false;
        }
    } else if (clearFrames++ > maxClearFrames) {
        clearFrames = maxClearFrames;
    }
    if (IsTimeToExitThisDWCineyCamMode(MODE_PLAYER_FALLEN_WATER, source, target, t, false)) {
        exitCam = true;
        return false;
    }
    Finalise_DW_CineyCams(source, target, 0.0f, fov, 10.0f - fov * (1.0f / 70.0f) * 9.7f, 1.0f);
    return true;
}

// 0x50F3F0 - debug
void CCam::Process_Editor(const CVector& target, float orientation, float speedVar, float speedVarWanted) {
    static auto& s_LookAtAngle     = StaticRef<float>(0xB6FFE4);
    static auto& s_DoRenderShadows = StaticRef<bool>(0xB7295A);

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

    m_fVerticalAngle = std::max(m_fVerticalAngle, DegreesToRadians(85.0f));
    if (m_fVerticalAngle >= DegreesToRadians(-85.0f)) {
        if (pad->IsSquareDown()) {
            s_LookAtAngle += 0.1f;
        } else if (pad->IsCrossDown()) {
            s_LookAtAngle -= 0.1f;
        } else {
            s_LookAtAngle = 0.0f;
        }
    } else {
        m_fVerticalAngle = DegreesToRadians(-85.0f);
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
    static auto& fov = StaticRef<float>(0xBC4074);
    static auto& firstFov = StaticRef<float>(0xBC4078);
    static auto& targetMarker = StaticRef<uint32>(0xBC407C);
    static auto& sourceMarker = StaticRef<uint32>(0xBC4080);
    static auto& fovMarker = StaticRef<uint32>(0xBC4084);
    static auto& upMarker = StaticRef<uint32>(0xBC4088);

    if (TheCamera.m_bCutsceneFinished) {
        return;
    }
    const auto* fovPath = TheCamera.m_aPathArray[0].m_pArrPathData;
    const auto* upPath = TheCamera.m_aPathArray[1].m_pArrPathData;
    const auto* sourcePath = TheCamera.m_aPathArray[2].m_pArrPathData;
    const auto* targetPath = TheCamera.m_aPathArray[3].m_pArrPathData;
    m_vecUp = {0.0f, 0.0f, 1.0f};
    if (!TheCamera.m_bStartingSpline) {
        m_fTimeElapsedFloat = 0.0f;
        m_nFinishTime = (uint32)(sourcePath[((int32)sourcePath[0] - 1) * 10 + 1] * 1000.0f);
        upMarker = fovMarker = 5;
        sourceMarker = targetMarker = 11;
        TheCamera.m_bStartingSpline = true;
        firstFov = fov = fovPath[2];
    } else {
        m_fTimeElapsedFloat += CTimer::GetTimeStepNonClipped() * 0.02f * 1000.0f;
    }
    const float time = (float)(uint32)m_fTimeElapsedFloat;
    const bool finished = time >= (float)m_nFinishTime;
    if (finished) {
        sourceMarker = ((uint32)sourcePath[0] - 1) * 10 + 1;
        targetMarker = ((uint32)targetPath[0] - 1) * 10 + 1;
        upMarker = ((uint32)upPath[0] - 1) * 4 + 1;
        fovMarker = ((uint32)fovPath[0] - 1) * 4 + 1;
    } else {
        TheCamera.m_fPositionAlongSpline = time / (float)m_nFinishTime;
        while (time >= (sourcePath[sourceMarker] - sourcePath[1]) * 1000.0f) {
            sourceMarker += 10;
        }
    }
    FindSplinePathPositionVector(&m_vecSource, sourcePath, time, &sourceMarker);
    if (!finished) {
        while (time >= (targetPath[targetMarker] - targetPath[1]) * 1000.0f) {
            targetMarker += 10;
        }
    }
    FindSplinePathPositionVector(&m_vecFront, targetPath, time, &targetMarker);
    if (!finished) {
        while (time >= (upPath[upMarker] - upPath[1]) * 1000.0f) {
            upMarker += 4;
        }
    }
    float upAngle{};
    FindSplinePathPositionFloat(&upAngle, upPath, time, &upMarker);
    upAngle = DegreesToRadians(upAngle) + HALF_PI;
    m_vecUp.x = std::cos(upAngle);
    m_vecUp.z = std::sin(upAngle);
    if (!finished) {
        while (time >= (fovPath[fovMarker] - fovPath[1]) * 1000.0f) {
            fovMarker += 4;
        }
    }
    FindSplinePathPositionFloat(&fov, fovPath, time, &fovMarker);
    if (finished) {
        TheCamera.m_fPositionAlongSpline = 1.0f;
        sourceMarker = targetMarker = upMarker = fovMarker = 0;
    }
    m_vecTargetCoorsForFudgeInter = m_vecFront;
    m_vecFront -= m_vecSource;
    m_vecFront.Normalise();
    const auto left = CrossProduct(m_vecUp, m_vecFront);
    m_vecUp = CrossProduct(m_vecFront, left).Normalized();
    m_fFOV = fov;
}

// 0x5245B0
void CCam::Process_FollowCar_SA(const CVector&, float, float, float, bool) {
    NOTSA_UNREACHABLE();
}

// 0x50F970
void CCam::Process_FollowPedWithMouse(const CVector& target, float orientation, float, float) {
    m_fFOV = 70.0f;
    if (!m_pCamTargetEntity->IsPed()) {
        return;
    }
    auto* pad = CPad::GetPad(0);
    if (m_bResetStatics) {
        m_bRotating = false;
        m_bCollisionChecksOn = true;
        pad->ClearMouseHistory();
        m_bResetStatics = false;
    }
    const auto* vehicle = FindPlayerVehicle();
    const auto onTrain = vehicle && vehicle->IsTrain();
    const auto cameraTarget = target + CVector{0.0f, 0.0f, StaticRef<float>(0x8CC7D0)};
    float horizontal{}, vertical{};
    if (pad->bPlayerSafe) {
        const auto direction = (m_vecSource - cameraTarget).Normalized();
        horizontal = (direction.z >= -0.9f ? std::atan2(direction.y, direction.x) : orientation + PI) - m_fHorizontalAngle;
    } else {
        const auto mouse = CPad::NewMouseControllerState.GetAmountMouseMoved();
        const auto fovScale = m_fFOV / 80.0f;
        if (mouse.IsZero() || pad->DisablePlayerControls) {
            horizontal = fovScale / 14.0f * CTimer::GetTimeStep() * StaticRef<float>(0x8CC7CC) * -(float)pad->LookAroundLeftRightOnPC();
            vertical = fovScale * (3.0f / 70.0f) * CTimer::GetTimeStep() * (float)pad->LookAroundUpDownOnPC() * StaticRef<float>(0x8CC7CC);
        } else {
            horizontal = CCamera::m_fMouseAccelHorzntl * fovScale * mouse.x * -2.5f;
            vertical = fovScale * mouse.y * 4.0f * CCamera::m_fMouseAccelVertical;
        }
    }
    const auto fadingOut = TheCamera.m_bFading && TheCamera.m_nFadeInOutFlag == eFadeFlag::FADE_OUT;
    if ((fadingOut && CDraw::FadeValue > StaticRef<uint32>(0x8CC7D4)) || CDraw::FadeValue > 200 || pad->bPlayerSafe) {
        vertical = std::clamp(StaticRef<float>(0x8CC7D8) - m_fVerticalAngle, -0.05f, 0.05f);
    }
    m_fHorizontalAngle += horizontal;
    m_fVerticalAngle += vertical;
    if (m_fHorizontalAngle > PI) {
        m_fHorizontalAngle -= TWO_PI;
    } else if (m_fHorizontalAngle < -PI) {
        m_fHorizontalAngle += TWO_PI;
    }
    m_fVerticalAngle = std::clamp(m_fVerticalAngle, DegreesToRadians(-89.5f), DegreesToRadians(45.0f));
    const auto distanceAngle = m_fVerticalAngle <= 0.0f
        ? m_fVerticalAngle
        : std::min(StaticRef<float>(0x8CC7C8) * m_fVerticalAngle, HALF_PI);
    const auto desiredDistance = std::cos(distanceAngle) * StaticRef<float>(0x8CC7C4) + StaticRef<float>(0x8CC7C0);
    if (TheCamera.m_bUseTransitionBeta) {
        m_fHorizontalAngle = m_fTransitionBeta;
    }
    if (TheCamera.m_bCamDirectlyBehind) {
        m_fHorizontalAngle = TheCamera.m_fPedOrientForBehindOrInFront + PI;
    }
    if (TheCamera.m_bCamDirectlyInFront) {
        m_fHorizontalAngle = TheCamera.m_fPedOrientForBehindOrInFront;
    }
    if (onTrain) {
        m_fHorizontalAngle = orientation;
    }
    m_vecFront = {
        -std::cos(m_fHorizontalAngle) * std::cos(m_fVerticalAngle),
        -std::sin(m_fHorizontalAngle) * std::cos(m_fVerticalAngle),
        std::sin(m_fVerticalAngle)
    };
    m_vecSource = cameraTarget - m_vecFront * desiredDistance;
    m_vecTargetCoorsForFudgeInter = cameraTarget;

    CColPoint collision{};
    CEntity* hitEntity{};
    CWorld::pIgnoreEntity = m_pCamTargetEntity;
    if (CWorld::ProcessLineOfSight(cameraTarget, m_vecSource, collision, hitEntity, true, true, true, true, false, false, true, false)) {
        const auto hitDistance = (cameraTarget - collision.m_vecPoint).Magnitude();
        if (!hitEntity->IsPed() || desiredDistance - hitDistance <= 0.4f) {
            m_vecSource = collision.m_vecPoint;
            if (hitDistance < 0.6f) {
                RwCameraSetNearClipPlane(Scene.m_pRwCamera, std::max(hitDistance - 0.3f, 0.05f));
            }
        } else {
            const auto hitPosition = collision.m_vecPoint;
            if (CWorld::ProcessLineOfSight(hitPosition, m_vecSource, collision, hitEntity, true, true, true, true, false, false, true, false)) {
                const auto nextHitDistance = (cameraTarget - collision.m_vecPoint).Magnitude();
                m_vecSource = collision.m_vecPoint;
                if (nextHitDistance < 0.6f) {
                    RwCameraSetNearClipPlane(Scene.m_pRwCamera, std::max(nextHitDistance - 0.3f, 0.05f));
                }
            } else {
                RwCameraSetNearClipPlane(Scene.m_pRwCamera, std::min(desiredDistance - hitDistance - 0.35f, 0.9f));
            }
        }
    }
    CWorld::pIgnoreEntity = nullptr;

    const auto radiusScale = std::tan(DegreesToRadians(m_fFOV) * 0.5f) * CDraw::ms_fAspectRatio * 1.1f;
    auto nearClip = RwCameraGetNearClipPlane(Scene.m_pRwCamera);
    auto* obstruction = CWorld::TestSphereAgainstWorld(m_vecSource + m_vecFront * nearClip, nearClip * radiusScale, nullptr, true, true, false, true, false, false);
    for (int32 i = 0; i < 6 && obstruction; i++) {
        const auto displacement = gaTempSphereColPoints[0].m_vecPoint - m_vecSource;
        const auto perpendicular = displacement - m_vecFront * DotProduct(displacement, m_vecFront);
        const auto newClip = std::max(std::min(perpendicular.Magnitude() / radiusScale, nearClip), 0.1f);
        if (newClip < nearClip) {
            RwCameraSetNearClipPlane(Scene.m_pRwCamera, newClip);
        }
        if (newClip == 0.1f) {
            m_vecSource += (cameraTarget - m_vecSource) * 0.3f;
        }
        nearClip = RwCameraGetNearClipPlane(Scene.m_pRwCamera);
        obstruction = CWorld::TestSphereAgainstWorld(m_vecSource + m_vecFront * nearClip, nearClip * radiusScale, nullptr, true, true, false, true, false, false);
    }

    const auto distance = (cameraTarget - m_vecSource).Magnitude();
    if (m_fDistance > distance) {
        m_fDistance = distance;
    } else {
        const auto damping = std::pow(0.92f, CTimer::GetTimeStep());
        m_fDistance = damping * m_fDistance + (1.0f - damping) * distance;
        if (distance > 0.05f) {
            m_vecSource = cameraTarget + (m_vecSource - cameraTarget) * (m_fDistance / distance);
        }
        const auto maxNearClip = m_fDistance - StaticRef<float>(0x8CC38C);
        if (maxNearClip < RwCameraGetNearClipPlane(Scene.m_pRwCamera)) {
            RwCameraSetNearClipPlane(Scene.m_pRwCamera, std::max(maxNearClip, 0.1f));
        }
    }
    TheCamera.m_bCamDirectlyBehind = false;
    TheCamera.m_bCamDirectlyInFront = false;
    GetVectorsReadyForRW();
    if (fadingOut && CDraw::FadeValue > 128) {
        auto* player = TheCamera.m_pTargetEntity->AsPed();
        player->m_fCurrentRotation = player->m_fAimingRotation = std::atan2(-m_vecFront.x, m_vecFront.y);
        player->SetHeading(player->m_fCurrentRotation);
        player->UpdateRwMatrix();
    }
}

// 0x522D40
void CCam::Process_FollowPed_SA(const CVector&, float, float, float, bool) {
    NOTSA_UNREACHABLE();
}

// 0x5105C0
void CCam::Process_M16_1stPerson(const CVector&, float, float, float) {
    if (!m_pCamTargetEntity->IsPed()) {
        return;
    }
    auto* ped = m_pCamTargetEntity->AsPed();
    auto* pad = CPad::GetPad(0);
    const auto attached = ped->IsPlayer() && ped->m_pAttachedTo;
    auto& targetFOV = StaticRef<float>(0xB6FFE8);
    auto& blocked = StaticRef<bool>(0xB6FFF4);
    auto& attachedHeading = CTheScripts::fCameraHeadingWhenPlayerIsAttached;
    auto& headingStep = CTheScripts::fCameraHeadingStepWhenPlayerIsAttached;
    const auto timeStep = CTimer::GetTimeStep();

    if (m_bResetStatics) {
        if (!CCamera::m_bUseMouse3rdPerson || ped->m_pPlayerData || attached) {
            m_fHorizontalAngle = attached ? attachedHeading : ped->m_fCurrentRotation - HALF_PI;
            m_fVerticalAngle = 0.0f;
        }
        targetFOV = m_fFOV;
        m_fInitialPlayerOrientation = ped->m_fCurrentRotation - HALF_PI;
        m_bResetStatics = false;
        blocked = false;
        StaticRef<float>(0xB6FFEC) = 0.0f;
        StaticRef<float>(0xB6FFF0) = 0.0f;
        m_bCollisionChecksOn = true;
        m_fFOVSpeed = m_fAlphaSpeed = m_fBetaSpeed = 0.0f;
    }

    if (m_nMode == MODE_SNIPER || m_nMode == MODE_CAMERA) {
        const auto zoomInButton = ControlsManager.GetMouseButtonAssociatedWithAction(eControllerAction::PED_SNIPER_ZOOM_IN);
        const auto zoomOutButton = ControlsManager.GetMouseButtonAssociatedWithAction(eControllerAction::PED_SNIPER_ZOOM_OUT);
        const auto wheel = CPad::NewMouseControllerState.m_fWheelMoved;
        const auto wheelFactor = (std::abs(wheel) * 7.0f + 10000.0f) * 0.0001f;
        bool wheelZoom = false;
        if ((wheel > 0.0f && (int32)zoomOutButton == 4) || (wheel < 0.0f && (int32)zoomOutButton == 5)) {
            targetFOV *= wheelFactor;
            wheelZoom = true;
        } else if ((wheel > 0.0f && (int32)zoomInButton == 4) || (wheel < 0.0f && (int32)zoomInButton == 5)) {
            targetFOV /= wheelFactor;
            wheelZoom = true;
        }
        const auto zoomFactor = (timeStep * 255.0f + 10000.0f) * 0.0001f;
        if (pad->SniperZoomOut() && !wheelZoom) {
            targetFOV = m_fFOV *= zoomFactor;
            m_fFOVSpeed = 0.0f;
        } else if (pad->SniperZoomIn() && !wheelZoom) {
            targetFOV = m_fFOV /= zoomFactor;
            m_fFOVSpeed = 0.0f;
        } else if (std::abs(targetFOV - m_fFOV) > 0.5f) {
            WellBufferMe(targetFOV, m_fFOV, m_fFOVSpeed, 0.5f, 0.25f, false);
        } else {
            m_fFOVSpeed = 0.0f;
        }
        m_fFOV = std::min(m_fFOV, 70.0f);
        if (targetFOV > 70.0f) {
            targetFOV = 70.0f;
        } else {
            const auto minFOV = m_nMode == MODE_CAMERA ? 3.0f : 15.0f;
            m_fFOV = std::max(m_fFOV, minFOV);
            targetFOV = std::max(targetFOV, minFOV);
        }
        TheCamera.SetMotionBlur(180, 255, 180, 120, eMotionBlurType::SNIPER);
    } else {
        m_fFOV = 70.0f;
    }

    if (attached && headingStep > (float)StaticRef<double>(0x859EF8)) {
        auto delta = m_fHorizontalAngle - attachedHeading;
        if (delta < 0.0f) {
            delta += TWO_PI;
        }
        if (delta < headingStep || TWO_PI - delta < headingStep) {
            m_fHorizontalAngle = attachedHeading;
            headingStep = 0.0f;
        } else {
            m_fHorizontalAngle += delta <= TWO_PI - delta ? -headingStep : headingStep;
        }
    }

    const auto mouse = CPad::NewMouseControllerState.GetAmountMouseMoved();
    const auto fovScale = m_fFOV / 80.0f;
    float horizontal{}, vertical{};
    if (mouse.IsZero()) {
        const auto h = -(float)pad->LookAroundLeftRight(ped);
        const auto v = (float)pad->LookAroundUpDown(ped);
        if (attached) {
            horizontal = fovScale * 0.04f * std::abs(h / 128.0f) * timeStep * (h / 128.0f);
            vertical = fovScale * StaticRef<float>(0x8631C0) * std::abs(v / 128.0f) * timeStep * (v / 128.0f);
        } else {
            horizontal = fovScale / 17.5f * h * h * 0.0001f * timeStep * (h < 0.0f ? -1.0f : 1.0f);
            vertical = fovScale / 14.0f * v * v / 22500.0f * timeStep * (v < 0.0f ? -1.0f : 1.0f);
        }
        const auto dampingBase = std::abs(h) < 2.0f && std::abs(v) < 2.0f ? StaticRef<float>(0x8CCC94) : StaticRef<float>(0x8CCC98);
        const auto damping = std::pow(dampingBase, timeStep);
        horizontal = m_fBetaSpeed = damping * m_fBetaSpeed + (1.0f - damping) * horizontal;
        vertical = m_fAlphaSpeed = damping * m_fAlphaSpeed + (1.0f - damping) * vertical;
    } else {
        if (!pad->DisablePlayerControls && !pad->JustOutOfFrontEnd && timeStep > 0.0f) {
            horizontal = fovScale * CCamera::m_fMouseAccelHorzntl * mouse.x * -3.0f;
            vertical = fovScale * CCamera::m_fMouseAccelVertical * mouse.y * 3.0f;
        }
        m_fAlphaSpeed = m_fBetaSpeed = 0.0f;
    }
    m_fHorizontalAngle += horizontal;
    m_fVerticalAngle += vertical;
    if (m_fHorizontalAngle > PI) {
        m_fHorizontalAngle -= TWO_PI;
    } else if (m_fHorizontalAngle < -PI) {
        m_fHorizontalAngle += TWO_PI;
    }

    const auto now = CTimer::GetTimeInMS();
    if ((int32)m_nCamBumpedTime > 0) {
        const auto wave = std::cos((float)(now - m_nCamBumpedTime) / (float)StaticRef<int32>(0x8CC474) * TWO_PI);
        m_fHorizontalAngle += StaticRef<float>(0x8CC480) * wave * m_fCamBumpedHorz;
        m_fVerticalAngle += wave * m_fCamBumpedVert * StaticRef<float>(0x8CC480);
        const auto damping = std::pow(StaticRef<float>(0x8CC47C), timeStep);
        m_fCamBumpedHorz *= damping;
        m_fCamBumpedVert *= damping;
        if (now > m_nCamBumpedTime + StaticRef<uint32>(0x8CC478)) {
            m_nCamBumpedTime = 0;
        }
    }

    auto forwardOffset = ped->bIsDucking ? 0.8f : 0.3f;
    if (attached) {
        auto* parent = ped->m_pAttachedTo;
        const auto& matrix = parent->GetMatrix();
        auto baseHeading = parent->GetHeading();
        float pitchComponent{};
        switch (ped->m_fTurretAngleA) {
        case 1:
            pitchComponent = -matrix.GetRight().z;
            break;
        case 2:
            pitchComponent = -matrix.GetForward().z;
            baseHeading += HALF_PI;
            break;
        case 3:
            pitchComponent = matrix.GetRight().z;
            baseHeading -= PI;
            break;
        default:
            pitchComponent = matrix.GetForward().z;
            baseHeading -= HALF_PI;
            break;
        }
        const auto basePitch = std::asin(std::clamp(pitchComponent, -1.0f, 1.0f));
        ped->PositionAttachedPed();
        ped->UpdateRwMatrix();
        ped->UpdateRwFrame();
        ped->UpdateRpHAnim();

        auto upOffset = 0.1f;
        auto upperLimit = DegreesToRadians(60.0f);
        auto lowerLimit = DegreesToRadians(85.5f);
        if (parent->IsVehicle() && parent->AsVehicle()->IsBike()) {
            upOffset = forwardOffset = 0.0f;
            upperLimit = lowerLimit = ped->m_nTurretPosnMode;
        }
        CVector head{};
        ped->GetTransformedBonePosition(head, BONE_HEAD, true);
        m_vecSource = head + ped->GetMatrix().GetUp() * upOffset - ped->GetMatrix().GetForward() * forwardOffset;

        auto delta = baseHeading - m_fHorizontalAngle;
        if (delta > PI) {
            baseHeading -= TWO_PI;
        } else if (delta < -PI) {
            baseHeading += TWO_PI;
        }
        delta = baseHeading - m_fHorizontalAngle;
        const auto softLimit = ped->m_fTurretAngleB * 0.75f;
        const auto margin = ped->m_fTurretAngleB * 0.25f;
        if (delta > softLimit || delta < -softLimit) {
            const auto excess = delta > softLimit ? delta - softLimit : delta + softLimit;
            const auto correction = std::max(timeStep * 0.05f * std::abs(excess), std::abs(excess) - margin);
            if (parent->IsVehicle() && parent->AsVehicle()->IsSubHeli()) {
                auto* heli = parent->AsVehicle()->AsHeli();
                if (heli->m_fForcedOrientation > 0.0f) {
                    heli->SetHeliOrientation(heli->m_fForcedOrientation - timeStep * excess * 0.1f);
                }
            }
            m_fHorizontalAngle += excess < 0.0f ? -correction : correction;
        }
        if (m_fVerticalAngle > basePitch + upperLimit * 0.75f) {
            const auto excess = m_fVerticalAngle - basePitch - upperLimit * 0.75f;
            m_fVerticalAngle -= std::max(timeStep * 0.05f * excess, excess - upperLimit * 0.25f);
        }
        if (m_fVerticalAngle < basePitch - lowerLimit * 0.75f) {
            const auto excess = basePitch - m_fVerticalAngle - lowerLimit * 0.75f;
            m_fVerticalAngle += std::max(timeStep * 0.05f * excess, excess - lowerLimit * 0.25f);
        }
    } else {
        m_fVerticalAngle = std::clamp(m_fVerticalAngle, DegreesToRadians(-85.5f), DegreesToRadians(60.0f));
        ped->UpdateRwMatrix();
        ped->UpdateRwFrame();
        ped->UpdateRpHAnim();
        CVector head{};
        ped->GetTransformedBonePosition(head, BONE_HEAD, true);
        m_vecSource = head + CVector{0.0f, 0.0f, 0.1f};
        const auto& matrix = ped->GetMatrix();
        const auto offset = ped->bIsDucking
            ? matrix.GetForward() * StaticRef<float>(0x8CC7BC) + matrix.GetRight() * StaticRef<float>(0x8CC7B8)
            : matrix.GetForward() * forwardOffset;
        m_vecSource.x -= offset.x;
        m_vecSource.y -= offset.y;
    }

    const auto pitchLimit = StaticRef<float>(0x8CCC90);
    m_fVerticalAngle = std::clamp(m_fVerticalAngle, -pitchLimit, pitchLimit);
    m_vecFront = {
        -std::cos(m_fHorizontalAngle) * std::cos(m_fVerticalAngle),
        -std::sin(m_fHorizontalAngle) * std::cos(m_fVerticalAngle),
        std::sin(m_fVerticalAngle)
    };
    const auto lookAt = m_vecSource + m_vecFront * 3.0f;
    m_vecSource += m_vecFront * 0.4f;
    if (m_bCollisionChecksOn) {
        blocked = !CWorld::GetIsLineOfSightClear(lookAt, m_vecSource, true, true, false, true, false, true, true);
        for (const auto side : {1.0f, -1.0f}) {
            if (blocked) {
                break;
            }
            const auto beta = m_fHorizontalAngle + side * StaticRef<float>(0x8631B8);
            const auto alpha = m_fVerticalAngle - StaticRef<float>(0x8631B4);
            const auto probe = m_vecSource + CVector{
                std::cos(beta) * std::cos(alpha),
                std::sin(beta) * std::cos(alpha),
                std::sin(alpha)
            } * 3.0f;
            blocked = !CWorld::GetIsLineOfSightClear(probe, m_vecSource, true, true, false, true, false, true, true);
        }
        if (blocked) {
            RwCameraSetNearClipPlane(Scene.m_pRwCamera, 0.3f);
        }
    }
    if (!blocked && m_nMode == MODE_CAMERA) {
        RwCameraSetNearClipPlane(Scene.m_pRwCamera, ((15.0f - std::min(m_fFOV, 15.0f)) * 0.15f + 1.0f) * 0.3f);
    }
    m_vecSource -= m_vecFront * 0.4f;
    GetVectorsReadyForRW();
    auto* player = TheCamera.m_pTargetEntity->AsPed();
    player->m_fCurrentRotation = player->m_fAimingRotation = std::atan2(-m_vecFront.x, m_vecFront.y);
}

// 0x511B50
void CCam::Process_Rocket(const CVector& target, float orientation, float speedVar, float speedVarWanted, bool isHeatSeeking) {
    static auto& dword_B6FFF8 = StaticRef<uint32>(0xB6FFF8);
    static auto& dword_B6FFFC = StaticRef<uint32>(0xB6FFFC);
    static auto& byte_B70000  = StaticRef<bool>(0xB70000);

    if (!m_pCamTargetEntity->GetIsTypePed()) {
        return;
    }

    auto* targetPed = m_pCamTargetEntity->AsPed();
    m_fFOV = 70.0f;
    if (m_bResetStatics) {
        if (!CCamera::m_bUseMouse3rdPerson || targetPed->m_pTargetedObject) {
            m_fVerticalAngle = 0.0f;
            m_fHorizontalAngle = targetPed->m_fCurrentRotation - DegreesToRadians(90.0f);
        }
        m_fInitialPlayerOrientation = m_fHorizontalAngle;
        m_bResetStatics             = 0;
        m_bCollisionChecksOn        = true;
        byte_B70000                 = 0;
        dword_B6FFFC                = 0;
        dword_B6FFF8                = 0;
    }
    m_pCamTargetEntity->UpdateRwMatrix();
    m_pCamTargetEntity->UpdateRwFrame();
    CVector headPosition{};
    targetPed->GetTransformedBonePosition(headPosition, eBoneTag::BONE_HEAD, true);
    m_vecSource = headPosition + CVector{0.0f, 0.0f, 0.1f};

    auto*      pad1   = CPad::GetPad(0);
    const auto fov    = m_fFOV / 80.0f;
    const auto amountMouseMoved = pad1->NewMouseControllerState.GetAmountMouseMoved();
    
    if (!amountMouseMoved.IsZero()) {
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
        -(std::cos(m_fHorizontalAngle) * std::cos(m_fVerticalAngle)),
        -(std::sin(m_fHorizontalAngle) * std::cos(m_fVerticalAngle)),
        std::sin(m_fVerticalAngle)
    );
    GetVectorsReadyForRW();

    const auto heading = CGeneral::GetATanOfXY(m_vecFront.x, m_vecFront.y) - DegreesToRadians(90.0f);
    TheCamera.m_pTargetEntity->AsPed()->m_fCurrentRotation = heading;
    TheCamera.m_pTargetEntity->AsPed()->m_fAimingRotation  = heading;

    if (isHeatSeeking) {
        auto* player     = FindPlayerPed();
        auto* playerData = player->GetPlayerData();
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

            const auto targetUsesCollision = hsTarget->GetUsesCollision();
            const auto playerUsesCollision = player->GetUsesCollision();
            hsTarget->SetUsesCollision(false);
            player->SetUsesCollision(false);

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
            player->SetUsesCollision(playerUsesCollision);
            hsTarget->SetUsesCollision(targetUsesCollision);
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
void CCam::Process_SpecialFixedForSyphon(const CVector& target, float, float, float) {
    m_vecSource = m_vecCamFixedModeSource;
    m_vecTargetCoorsForFudgeInter = target;
    m_vecTargetCoorsForFudgeInter.z += m_fSyphonModeTargetZOffSet;
    m_vecFront = target - m_vecSource;
    const auto fixedSource = m_vecCamFixedModeSource;
    TheCamera.AvoidTheGeometry(&fixedSource, &m_vecTargetCoorsForFudgeInter, &m_vecSource, m_fFOV);
    m_vecFront.z += m_fSyphonModeTargetZOffSet;
    GetVectorsReadyForRW();
    m_vecUp += m_vecCamFixedModeUpOffSet;
    m_vecUp.Normalise();
    const auto right = CrossProduct(m_vecUp, m_vecFront).Normalized();
    m_vecFront = CrossProduct(right, m_vecUp).Normalized();
    m_fFOV = 70.0f;

    if (m_pCamTargetEntity && m_pCamTargetEntity->IsPed()) {
        auto* ped = m_pCamTargetEntity->AsPed();
        if (ped->m_pTargetedObject) {
            const auto* weapon = CWeaponInfo::GetWeaponInfo(ped->GetActiveWeapon().m_Type, ped->GetWeaponSkill());
            if (weapon && (!weapon->flags.bAimWithArm || ped->bIsDucking) && (int32)weapon->m_nWeaponFire != 0) {
                const auto direction = ped->m_pTargetedObject->GetPosition() - ped->GetPosition();
                const float heading = std::atan2(-direction.x, direction.y);
                ped->m_fAimingRotation = ped->m_fCurrentRotation = heading;
                ped->SetHeading(heading);
                ped->UpdateRwMatrix();
            }
        }
    }
}

// 0x512110
bool CCam::Process_WheelCam(const CVector&, float, float, float) {
    static auto& sideOffset = StaticRef<float>(0x8CC7DC);
    static auto& bikeSideOffset = StaticRef<float>(0x8CC7E0);
    static auto& boatSideOffset = StaticRef<float>(0x8CCC60);
    static auto& boatForwardOffset = StaticRef<float>(0x8CCC64);
    static auto& boatHeightOffset = StaticRef<float>(0x8CCC68);
    static auto& predatorSideOffset = StaticRef<float>(0x8CCC6C);
    static auto& predatorForwardOffset = StaticRef<float>(0x8CCC70);
    static auto& predatorHeightOffset = StaticRef<float>(0x8CCC74);
    static auto& boatUpScale = StaticRef<float>(0x8CCCA0);
    static auto& boatForwardScale = StaticRef<float>(0x8CCCA4);
    static auto& rollFrequency = StaticRef<float>(0x8631C4);

    m_fFOV = 70.0f;
    auto* entity = m_pCamTargetEntity;
    CVector offset;
    if (entity->IsPed()) {
        m_vecSource = entity->GetMatrix().TransformVector({-0.3f, -0.5f, 0.1f}) + entity->GetPosition();
        m_vecFront = {1.0f, 0.0f, 0.0f};
    } else {
        offset = {entity->GetColModel()->GetBoundingBox().m_vecMin.x - sideOffset, -2.3f, 0.3f};
        m_vecSource = entity->GetMatrix().TransformPoint(offset);
        m_vecFront = entity->GetMatrix().GetForward();
    }

    CVector right, up;
    auto* vehicle = entity->IsVehicle() ? entity->AsVehicle() : nullptr;
    if (vehicle && (vehicle->GetVehicleAppearance() == VEHICLE_APPEARANCE_HELI || vehicle->GetVehicleAppearance() == VEHICLE_APPEARANCE_PLANE)) {
        right = entity->GetMatrix().GetRight();
        up = entity->GetMatrix().GetUp();
        offset.x = -1.55f;
        m_vecSource = entity->GetMatrix().TransformPoint(offset);
    } else if (vehicle && vehicle->IsBoat()) {
        right = CrossProduct(m_vecFront, CVector{0.0f, 0.0f, 1.0f}).Normalized();
        up = CrossProduct(right, m_vecFront).Normalized();
        if (vehicle->m_pDriver) {
            CVector head{};
            vehicle->m_pDriver->GetBonePosition(&head, BONE_HEAD, true);
            head += vehicle->m_vecMoveSpeed * CTimer::GetTimeStep() + right * boatSideOffset;
            head += vehicle->GetMatrix().GetForward() * boatForwardOffset;
            head.z += boatHeightOffset;
            if (vehicle->m_nModelIndex == MODEL_PREDATOR) {
                head += right * predatorSideOffset;
                head += vehicle->GetMatrix().GetForward() * boatForwardScale * predatorForwardOffset;
                head.z += boatUpScale * predatorHeightOffset;
            }
            m_vecSource = head;
        } else {
            m_vecSource.z += boatHeightOffset + boatHeightOffset;
        }
    } else if (vehicle && vehicle->IsBike()) {
        right = entity->GetMatrix().GetRight();
        up = {0.0f, 0.0f, 1.0f};
        m_vecFront = CrossProduct(m_vecUp, right).Normalized();
        offset.x += sideOffset - bikeSideOffset;
        m_vecSource = entity->GetPosition() + right * offset.x + m_vecFront * offset.y + m_vecUp * offset.z;
    } else {
        if (vehicle && vehicle->IsTrain() && DotProduct(vehicle->m_vecMoveSpeed, m_vecFront) < 0.0f) {
            m_vecFront = -m_vecFront;
        }
        right = CrossProduct(m_vecFront, CVector{0.0f, 0.0f, 1.0f}).Normalized();
        up = CrossProduct(right, m_vecFront).Normalized();
    }

    if (float waterLevel{}; CWaterLevel::GetWaterLevel(m_vecSource, waterLevel, true) && m_vecSource.z < waterLevel - 0.3f) {
        ApplyUnderwaterMotionBlur();
    }
    const float roll = std::cos((CTimer::GetTimeInMS() & 0x1FFFF) * rollFrequency) * 0.4f;
    m_vecUp = up * std::cos(roll) + right * std::sin(roll);
    m_vecFront.Normalise();
    m_vecUp.Normalise();

    CColPoint collision{};
    CEntity* hitEntity{};
    CWorld::pIgnoreEntity = entity;
    const bool obstructed = CWorld::ProcessLineOfSight(m_vecSource, entity->GetPosition(), collision, hitEntity, true, false, false, true, false, false, true, false);
    CWorld::pIgnoreEntity = nullptr;
    return !obstructed;
}

// based on 0x51847C - 0x5184EC
void CCam::ApplyUnderwaterMotionBlur() {
    static constexpr uint32 UNDERWATER_CAM_BLUR      = 20;    // 0x8CC7A4
    static constexpr float  UNDERWATER_CAM_MAG_LIMIT = 10.0f; // 0x8CC7A8

    const auto colorMag = std::sqrt(
        sq(CTimeCycle::GetWaterRed()) +
        sq(CTimeCycle::GetWaterGreen()) +
        sq(CTimeCycle::GetWaterBlue())
    );

    const auto factor = (colorMag <= UNDERWATER_CAM_MAG_LIMIT) ? 1.0f : UNDERWATER_CAM_MAG_LIMIT / colorMag;

    TheCamera.SetMotionBlur(
        static_cast<uint32>(factor * CTimeCycle::GetWaterRed()),
        static_cast<uint32>(factor * CTimeCycle::GetWaterGreen()),
        static_cast<uint32>(factor * CTimeCycle::GetWaterBlue()),
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
