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
#include "GameLogic.h"
#include "TaskSimpleGangDriveBy.h"
#include "TaskSimpleArrestPed.h"
#include "TaskSimpleClimb.h"
#include "TaskSimpleUseGun.h"
#include "PedClothesDesc.h"
#include "TaskSimpleSwim.h"
#include "TaskComplexEnterCarAsDriver.h"
#include "TaskComplexProstituteSolicit.h"

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

// 0x515D80
static bool GetArrestCameraPosition(CEntity* target, CPed* cop, const CVector& lookAt, CVector& source) {
    if (!target || !cop) {
        return false;
    }
    auto position = cop->GetPosition();
    auto direction = lookAt - position;
    const auto side = CrossProduct(direction, CVector{0.0f, 0.0f, 1.0f}).Normalized();
    position += side * StaticRef<float>(0x8CC8CC);
    direction.Normalise();
    if (direction.z < -0.7071f) {
        direction.z = -0.7071f;
        const auto scale = direction.Magnitude2D() * StaticRef<float>(0x8631E4);
        if (scale > 0.0f) {
            direction.x /= scale;
            direction.y /= scale;
        }
        direction.Normalise();
    } else if (direction.z > 0.0f) {
        direction.z = 0.0f;
        direction.Normalise();
    }
    auto displacement = lookAt - (position - direction * StaticRef<float>(0x8CC8C8));
    const auto distance = displacement.Magnitude();
    const auto minimumDistance = StaticRef<float>(0x8CC8D4);
    if (distance > 0.0f && distance < minimumDistance) {
        displacement *= minimumDistance / distance;
    }
    source = lookAt - displacement;
    return true;
}

// 0x516010
static bool GetArrestCameraPositionOnGround(CEntity* target, CPed* cop, const CVector& lookAt, CVector& source) {
    if (!target || !cop) {
        return false;
    }
    auto direction = lookAt - cop->GetPosition();
    direction.z = 0.0f;
    direction.Normalise();
    source = lookAt + direction * StaticRef<float>(0x8CC7F0);
    source += CrossProduct(direction, CVector{0.0f, 0.0f, 1.0f}) * StaticRef<float>(0x8CC7F4);
    source.z = lookAt.z + 5.0f;
    bool foundGround{};
    const auto ground = CWorld::FindGroundZFor3DCoord(source.x, source.y, source.z, &foundGround, nullptr);
    if (foundGround) {
        source.z = ground + StaticRef<float>(0x8CC7F8);
    }
    return true;
}

// 0x513220
static bool CanSeeBothPlayers(CVector source) {
    gCurCamColVars = 5;
    CColPoint collision{};
    CEntity* hitEntity{};
    return !CWorld::ProcessLineOfSight(source, FindPlayerPed(PED_TYPE_PLAYER1)->GetPosition(), collision, hitEntity, true, false, false, false, false, true, true, false)
        && !CWorld::ProcessLineOfSight(source, FindPlayerPed(PED_TYPE_PLAYER2)->GetPosition(), collision, hitEntity, true, false, false, false, false, true, true, false);
}

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

// 0x50A0A0
static float RoundCamCoordinate(float value, int32 places) {
    const auto scaled = (long double)value * std::pow(10.0L, places + 1);
    const double biased = (scaled + (value >= 0.0f ? 5.0L : -5.0L)) * (long double)0.1f;
    return (float)(std::trunc(biased) / std::pow(10.0L, places));
}

// 0x50A120
static void RoundCamCoordinates(CVector& position, int32 places) {
    position.x = RoundCamCoordinate(position.x, places);
    position.y = RoundCamCoordinate(position.y, places);
    position.z = RoundCamCoordinate(position.z, places);
}

void CCam::InjectHooks() {
    RH_ScopedClass(CCam);
    RH_ScopedCategory("Camera");
    RH_ScopedGlobalInstall(CanSeeBothPlayers, 0x513220);
    RH_ScopedGlobalInstall(GetArrestCameraPosition, 0x515D80);
    RH_ScopedGlobalInstall(GetArrestCameraPositionOnGround, 0x516010);

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
    RH_ScopedInstall(Process, 0x526FC0);
    RH_ScopedInstall(ProcessArrestCamOne, 0x518500);
    RH_ScopedInstall(ProcessArrestCamFirstPerson, 0x512EF0);
    RH_ScopedInstall(ProcessPedsDeadBaby, 0x519250);
    RH_ScopedInstall(Process_1rstPersonPedOnPC, 0x50EB70);
    RH_ScopedInstall(Process_1stPerson, 0x517EA0);
    RH_ScopedInstall(Process_AimWeapon, 0x521500);
    RH_ScopedInstall(Process_AttachedCam, 0x512B10);
    RH_ScopedInstall(Process_Cam_TwoPlayer, 0x525E50);
    RH_ScopedInstall(Process_Cam_TwoPlayer_InCarAndShooting, 0x519810);
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
    RH_ScopedInstall(Process_FollowCar_SA, 0x5245B0);
    RH_ScopedInstall(Process_FollowPedWithMouse, 0x50F970);
    RH_ScopedInstall(Process_FollowPed_SA, 0x522D40);
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
    auto& idleTime = gIdleCam.m_IdleTickerFrames;
    auto& idleFlags = StaticRef<uint8>(0xC0B184);
    if ((float)idleTime <= gIdleCam.m_TimeControlsIdleForIdleToKickIn) {
        idleFlags &= ~1;
    }
    if (TheCamera.GetActiveCam().m_nMode != MODE_FOLLOWPED) {
        idleFlags &= ~1;
        idleTime = 0;
    }
    if (!m_pCamTargetEntity) {
        m_pCamTargetEntity = TheCamera.m_pTargetEntity;
        CEntity::RegisterReference(m_pCamTargetEntity);
    }
    if (gCrossHair[CrossHairId(0)].m_bClearImmediately) {
        auto* player = FindPlayerPed();
        if (!player || !player->m_pVehicle || player->m_pVehicle->m_nModelIndex != MODEL_HYDRA) {
            CWeaponEffects::ClearCrossHairImmediately(CrossHairId(0));
        }
    }
    if (++m_nFrameNumWereAt > m_nDoCollisionCheckEveryNumOfFrames) {
        m_nFrameNumWereAt = 1;
    }
    m_bCollisionChecksOn = m_nFrameNumWereAt == m_nDoCollisionChecksOnFrameNum;

    auto* pad = CPad::GetPad(0);
    CVector target;
    float orientation{}, speedWanted{};
    if (m_bCamLookingAtVector) {
        target = m_vecCamFixedModeVector;
    } else {
        auto* entity = m_pCamTargetEntity;
        target = entity->GetPosition();
        if (entity->IsVehicle()) {
            auto forward = entity->GetMatrix().GetForward();
            orientation = forward.x == 0.0f && forward.y == 0.0f ? 0.0f : CGeneral::GetATanOfXY(forward.x, forward.y);
            forward.z = 0.0f;
            forward.Normalise();
            const auto length = forward.Magnitude2D();
            if (length != 0.0f) {
                forward /= length;
            }
            const auto speed = entity->AsVehicle()->GetMoveSpeed();
            const float x = forward.x * speed.x, y = forward.y * speed.y;
            const auto magnitude = std::sqrt(x * x + y * y);
            speedWanted = x + y > 0.0f
                ? std::min(magnitude * StaticRef<float>(0x86325C), 1.0f)
                : -std::min(magnitude * StaticRef<float>(0x863258), 0.5f);
            m_fSpeedVar = m_fSpeedVar * StaticRef<float>(0x863250) + speedWanted * StaticRef<float>(0x863254);
            if (m_nDirectionWasLooking != 3 && (!pad->GetLookBehindForCar() || pad->GetLookLeft() || pad->GetLookRight())) {
                TheCamera.m_bCamDirectlyBehind = true;
            }
        } else {
            if (entity == FindPlayerPed()) {
                auto* player = FindPlayerPed();
                if (auto* climb = player->GetIntelligence()->GetTaskClimb()) {
                    climb->GetCameraTargetPos(player, target);
                }
                auto& previous = StaticRef<CVector>(0x8CCC3C);
                auto& velocity = StaticRef<CVector>(0xB6EC7C);
                const auto timeStep = CTimer::GetTimeStep();
                if ((previous - target).SquaredMagnitude() > 9.0f || timeStep < 0.2f || Using3rdPersonMouseCam()
                    || TheCamera.m_bCamDirectlyBehind || TheCamera.m_bCamDirectlyInFront) {
                    velocity.Reset();
                } else if (player->GetIntelligence()->GetTaskFighting() && m_nMode == MODE_AIMWEAPON) {
                    const auto damping = std::pow(StaticRef<float>(0x8CC39C), timeStep);
                    target = previous * damping + target * (1.0f - damping);
                    velocity.Reset();
                } else {
                    const auto positionDamping = std::pow(StaticRef<float>(0x8CC394), timeStep);
                    const auto velocityDamping = std::pow(StaticRef<float>(0x8CC398), timeStep);
                    const auto height = target.z;
                    target = target * (1.0f - positionDamping) + (previous + velocity * timeStep) * positionDamping;
                    target.z = height;
                    velocity = velocity * velocityDamping + (target - previous) * ((1.0f - velocityDamping) / std::max(1.0f, timeStep));
                }
                velocity.z = 0.0f;
                previous = target;
            }
            const auto& forward = entity->GetMatrix().GetForward();
            orientation = forward.x == 0.0f && forward.y == 0.0f ? 0.0f : CGeneral::GetATanOfXY(forward.x, forward.y);
            m_fSpeedVar = 0.0f;
        }
    }

    m_nDirectionWasLooking = gCameraDirection;
    gCameraDirection = 3;
    if (this == &TheCamera.GetActiveCam()) {
        if ((m_nMode == MODE_CAM_ON_A_STRING || m_nMode == MODE_1STPERSON || m_nMode == MODE_BEHINDBOAT || m_nMode == MODE_BEHINDCAR)
            && m_pCamTargetEntity->IsVehicle()) {
            const auto appearance = m_pCamTargetEntity->AsVehicle()->GetVehicleAppearance();
            const bool aircraft = appearance == VEHICLE_APPEARANCE_HELI || appearance == VEHICLE_APPEARANCE_PLANE;
            if (pad->GetLookBehindForCar()) {
                gCameraDirection = 0;
            } else if (pad->GetLookLeft() && !aircraft) {
                gCameraDirection = 1;
            } else if (pad->GetLookRight() && !aircraft) {
                gCameraDirection = 2;
            }
            if (gCameraDirection != 3) {
                TheCamera.m_bTransitionState = false;
                TheCamera.m_bDoingSpecialInterp = false;
                TheCamera.m_bWaitForInterpolToFinish = false;
            }
            if (m_nDirectionWasLooking != gCameraDirection) {
                TheCamera.m_bJust_Switched = true;
            }
        } else if (m_nMode == MODE_FOLLOWPED && m_pCamTargetEntity->IsPed() && pad->GetLookBehindForPed()) {
            gCameraDirection = 0;
            if (m_nDirectionWasLooking != 0 && !TheCamera.m_bTransitionState) {
                TheCamera.m_bJust_Switched = true;
            }
        } else if (m_nMode == MODE_FOLLOWPED || m_nMode == MODE_AIMWEAPON) {
            if (m_nDirectionWasLooking != 3) {
                StaticRef<float>(0x8CCB84) = 1.0f;
            }
        }
    }
    if (TheCamera.m_bJust_Switched) {
        StaticRef<float>(0x8CCB84) = 1.0f;
        TheCamera.m_bResetOldMatrix = true;
    }
    if (m_nMode != MODE_BEHINDCAR && m_nMode != MODE_CAM_ON_A_STRING && m_nMode != MODE_BEHINDBOAT
        && m_nMode != MODE_1STPERSON && m_nMode != MODE_TWOPLAYER_IN_CAR_AND_SHOOTING) {
        CPostEffects::m_bSpeedFXUserFlagCurrentFrame = false;
    }
    gbFirstPersonRunThisFrame = false;
    switch (m_nMode) {
    case MODE_BEHINDCAR:
    case MODE_CAM_ON_A_STRING:
    case MODE_BEHINDBOAT:
        Process_FollowCar_SA(target, orientation, m_fSpeedVar, speedWanted, false);
        break;
    case MODE_FOLLOWPED:
        if (!CCamera::m_bUseMouse3rdPerson || StaticRef<bool>(0x8CCF00)) {
            Process_FollowPed_SA(target, orientation, m_fSpeedVar, speedWanted, false);
        } else {
            Process_FollowPedWithMouse(target, orientation, m_fSpeedVar, speedWanted);
        }
        break;
    case MODE_SNIPER:
    case MODE_M16_1STPERSON:
    case MODE_HELICANNON_1STPERSON:
    case MODE_CAMERA:
        Process_M16_1stPerson(target, orientation, m_fSpeedVar, speedWanted);
        break;
    case MODE_ROCKETLAUNCHER:
    case MODE_ROCKETLAUNCHER_HS:
        Process_Rocket(target, orientation, m_fSpeedVar, speedWanted, m_nMode == MODE_ROCKETLAUNCHER_HS);
        break;
    case MODE_WHEELCAM:
        Process_WheelCam(target, orientation, m_fSpeedVar, speedWanted);
        break;
    case MODE_FIXED:
        Process_Fixed(target, orientation, m_fSpeedVar, speedWanted);
        break;
    case MODE_1STPERSON:
        Process_1stPerson(target, orientation, m_fSpeedVar, speedWanted);
        break;
    case MODE_FLYBY:
        Process_FlyBy(target, orientation, m_fSpeedVar, speedWanted);
        break;
    case MODE_PED_DEAD_BABY:
        ProcessPedsDeadBaby();
        TheCamera.m_bPlayerIsInGarage = false;
        TheCamera.m_bJustCameOutOfGarage = false;
        break;
    case MODE_ARRESTCAM_ONE:
        ProcessArrestCamOne();
        break;
    case MODE_ARRESTCAM_TWO:
        break;
    case MODE_SPECIAL_FIXED_FOR_SYPHON:
        Process_SpecialFixedForSyphon(target, orientation, m_fSpeedVar, speedWanted);
        break;
    case MODE_SNIPER_RUNABOUT:
    case MODE_ROCKETLAUNCHER_RUNABOUT:
    case MODE_1STPERSON_RUNABOUT:
    case MODE_M16_1STPERSON_RUNABOUT:
    case MODE_FIGHT_CAM_RUNABOUT:
    case MODE_ROCKETLAUNCHER_RUNABOUT_HS:
        Process_1rstPersonPedOnPC(target, orientation, m_fSpeedVar, speedWanted);
        break;
    case MODE_EDITOR:
        Process_Editor(target, orientation, m_fSpeedVar, speedWanted);
        break;
    case MODE_ATTACHCAM: Process_AttachedCam(); break;
    case MODE_TWOPLAYER: Process_Cam_TwoPlayer(); break;
    case MODE_TWOPLAYER_IN_CAR_AND_SHOOTING: Process_Cam_TwoPlayer_InCarAndShooting(); break;
    case MODE_TWOPLAYER_SEPARATE_CARS: Process_Cam_TwoPlayer_Separate_Cars(); break;
    case MODE_TWOPLAYER_SEPARATE_CARS_TOPDOWN: Process_Cam_TwoPlayer_Separate_Cars_TopDown(); break;
    case MODE_AIMWEAPON:
    case MODE_AIMWEAPON_FROMCAR:
    case MODE_AIMWEAPON_ATTACHED:
        Process_AimWeapon(target, orientation, m_fSpeedVar, speedWanted);
        break;
    case MODE_DW_HELI_CHASE: Process_DW_HeliChaseCam(false); break;
    case MODE_DW_CAM_MAN: Process_DW_CamManCam(false); break;
    case MODE_DW_BIRDY: Process_DW_BirdyCam(false); break;
    case MODE_DW_PLANE_SPOTTER: Process_DW_PlaneSpotterCam(false); break;
    case MODE_DW_DOG_FIGHT:
    case MODE_DW_FISH:
        TheCamera.m_bUseNearClipScript = false;
        break;
    case MODE_DW_PLANECAM1: Process_DW_PlaneCam1(false); break;
    case MODE_DW_PLANECAM2: Process_DW_PlaneCam2(false); break;
    case MODE_DW_PLANECAM3: Process_DW_PlaneCam3(false); break;
    default:
        m_vecSource.Reset();
        m_vecFront = {0.0f, 1.0f, 0.0f};
        m_vecUp = {0.0f, 0.0f, 1.0f};
        break;
    }
    if (m_nMode < MODE_DW_HELI_CHASE || m_nMode > MODE_DW_PLANECAM3) {
        StaticRef<int32>(0x8CC488) = -1;
    }
    gCameraMode = m_nMode;
    const auto delta = m_vecSource - m_vecTargetCoorsForFudgeInter;
    m_fTrueBeta = CGeneral::GetATanOfXY(delta.x, delta.y);
    m_fTrueAlpha = CGeneral::GetATanOfXY(delta.Magnitude2D(), delta.z);
    if (!TheCamera.m_bTransitionState) {
        KeepTrackOfTheSpeed(m_vecSource, m_vecTargetCoorsForFudgeInter, m_vecUp, m_fTrueAlpha, m_fTrueBeta, m_fFOV);
    }
    m_vecSourceBeforeLookBehind = m_vecSource;
    m_bLookingRight = m_bLookingLeft = m_bLookingBehind = false;
    if (this == &TheCamera.GetActiveCam()) {
        switch (gCameraDirection) {
        case 0: LookBehind(); break;
        case 1: LookRight(false); break;
        case 2: LookRight(true); break;
        }
        m_nDirectionWasLooking = gCameraDirection;
    }
    if (TheCamera.m_bFOVLerpProcessed) {
        m_fFOV = TheCamera.m_fFOVNew;
        TheCamera.m_bFOVLerpProcessed = false;
    }
    if (TheCamera.m_bVecMoveLinearProcessed) {
        m_vecSource = TheCamera.m_vecMoveLinear;
        TheCamera.m_bVecMoveLinearProcessed = false;
    }
    if (TheCamera.m_bVecTrackLinearProcessed) {
        m_vecFront = (TheCamera.m_vecTrackLinear - m_vecSource).Normalized();
        GetVectorsReadyForRW();
        TheCamera.m_bVecTrackLinearProcessed = false;
    }
}

// 0x512EF0
bool CCam::ProcessArrestCamFirstPerson(CPed* cop, bool firstFrame) {
    if (firstFrame && (float)CGeneral::GetRandomNumber() * StaticRef<float>(0x858C7C) > StaticRef<float>(0x858F50)) {
        return false;
    }
    auto* target = TheCamera.m_pTargetEntity;
    if (!target->IsPed() || !cop) {
        return false;
    }
    const auto t = std::clamp(((float)CTimer::GetTimeInMS() - StaticRef<float>(0xB6EC60) - StaticRef<float>(0xB70024)) / StaticRef<float>(0x8CCCB0), 0.0f, 1.0f);
    auto& moving = StaticRef<bool>(0xB70020);
    moving = t < 1.0f || !moving;
    auto& initialized = StaticRef<uint8>(0xB7001C);
    if (!(initialized & 1)) {
        initialized |= 1;
        StaticRef<CVector>(0xB70010).Reset();
    }
    if (!(initialized & 2)) {
        initialized |= 2;
        StaticRef<CVector>(0xB70004) = {0.0f, 0.0f, -0.5f};
    }
    const auto oldSource = m_vecSource;
    m_vecSource = target->GetPosition() + StaticRef<CVector>(0xB70004);
    m_fFOV = 100.0f;
    auto* hierarchy = GetAnimHierarchyFromSkinClump(cop->GetRpClump());
    const auto index = RpHAnimIDGetIndex(hierarchy, BONE_HEAD);
    const auto& matrix = RpHAnimHierarchyGetMatrixArray(hierarchy)[index];
    CVector lookAt = matrix.pos;
    lookAt.z += StaticRef<float>(0x8CCCAC) - StaticRef<float>(0x8CCCA8) * t;
    m_vecFront = (lookAt - m_vecSource).Normalized();
    const auto right = CrossProduct(m_vecFront, CVector{0.0f, 0.0f, 1.0f}).Normalized();
    m_vecUp = CrossProduct(right, m_vecFront);
    if (firstFrame) {
        if (!CWorld::GetIsLineOfSightClear(m_vecSource, lookAt, true, true, false, true, false, false, true)) {
            return false;
        }
        if (!CWorld::GetIsLineOfSightClear(m_vecSource, lookAt, true, false, false, true, false, false, false)) {
            m_vecSource = oldSource;
        }
    }
    target->SetIsVisible(false);
    return true;
}

// 0x518500
bool CCam::ProcessArrestCamOne() {
    static auto& mode = StaticRef<int32>(0xB6EC58);
    // These globals share storage with the cinematic camera exit flags.
    static auto& cameraCop = StaticRef<CPed*>(0xB6EC5C);
    static auto& firstPersonStartTime = StaticRef<float>(0xB6EC60);

    m_fFOV = 45.0f;
    auto* target = TheCamera.m_pTargetEntity;
    const auto GetArrestingCop = []() -> CPed* {
        auto* player = FindPlayerPed();
        return player ? player->GetPlayerData()->m_pArrestingCop : nullptr;
    };
    if (!m_bResetStatics && mode == 1) {
        target->SetIsVisible(false);
        return ProcessArrestCamFirstPerson(GetArrestingCop(), false);
    }
    if (m_bResetStatics) {
        mode = 0;
    }

    CPed* ped{};
    CVector lookAt;
    if (target->IsPed()) {
        ped = target->AsPed();
    } else if (target->IsVehicle()) {
        auto* driver = target->AsVehicle()->m_pDriver;
        if (driver && driver->IsPlayer()) {
            ped = driver;
        }
    } else {
        return false;
    }
    if (ped) {
        ped->GetBonePosition(&lookAt, BONE_SPINE1, true);
    } else {
        lookAt = target->GetPosition();
    }

    const auto Finalise = [&](const CVector& source) {
        m_vecSource = source;
        const auto beforeCollision = m_vecSource;
        TheCamera.AvoidTheGeometry(&beforeCollision, &lookAt, &m_vecSource, m_fFOV);
        m_vecFront = (lookAt - m_vecSource).Normalized();
        m_vecUp = {0.0f, 0.0f, 1.0f};
        const auto right = CrossProduct(m_vecFront, m_vecUp).Normalized();
        m_vecUp = CrossProduct(right, m_vecFront);
    };
    CVector source{};
    if (m_bResetStatics) {
        auto* cop = GetArrestingCop();
        std::array<int32, 6> choices{-1, -1, -1, -1, -1, -1};
        const auto randomScale = StaticRef<float>(0x858C7C);
        if (target->IsPed()) {
            choices = cop && (float)CGeneral::GetRandomNumber() * randomScale > 0.5f
                ? std::array<int32, 6>{1, 2, 3, 2, 8, -1}
                : std::array<int32, 6>{1, 3, 2, 8, -1, -1};
        } else {
            choices = cop && (float)CGeneral::GetRandomNumber() * randomScale > 0.65f
                ? std::array<int32, 6>{2, 8, 3, 2, -1, -1}
                : std::array<int32, 6>{8, 3, 2, -1, -1, -1};
        }
        if (!StaticRef<bool>(0xBAADC0)) {
            CMessages::AddBigMessage(TheText.Get("BUSTED"), 5000, static_cast<eMessageStyle>(2));
        }
        bool found{};
        for (const auto choice : choices) {
            if (mode || choice < 1) {
                break;
            }
            cameraCop = nullptr;
            switch (choice) {
            case 1:
                firstPersonStartTime = (float)CTimer::GetTimeInMS();
                if (ProcessArrestCamFirstPerson(cop, true)) {
                    target->SetIsVisible(false);
                    mode = 1;
                    m_bResetStatics = false;
                    return true;
                }
                break;
            case 2:
            case 3:
                if (cop) {
                    found = choice == 2
                        ? GetArrestCameraPosition(target, cop, lookAt, source)
                        : GetArrestCameraPositionOnGround(target, cop, lookAt, source);
                    cameraCop = cop;
                    cop = nullptr;
                } else if (ped) {
                    for (auto* entity : ped->GetIntelligence()->GetPedScanner().m_apEntities) {
                        if (!entity) {
                            continue;
                        }
                        auto* candidate = entity->AsPed();
                        auto* task = static_cast<CTaskSimpleArrestPed*>(candidate->GetTaskManager().FindActiveTaskByType(TASK_SIMPLE_ARREST_PED));
                        if (task && task->m_Ped == FindPlayerPed()) {
                            // Both choices use the standing helper for a scanned cop.
                            found = GetArrestCameraPosition(target, candidate, lookAt, source);
                            if (found) {
                                cameraCop = candidate;
                                break;
                            }
                        }
                    }
                }
                break;
            case 8:
                found = GetLookFromLampPostPos(target, cop, lookAt, source);
                break;
            }
            if (found) {
                if (cameraCop) {
                    CEntity::RegisterReference(cameraCop);
                }
                mode = choice;
                if (mode == 3) {
                    const auto random = (float)CGeneral::GetRandomNumber() * randomScale * 5.0f;
                    mode = random < 1.0f ? 3 : random < 2.0f ? 4 : random < 3.0f ? 5 : random < 4.0f ? 6 : 7;
                }
            }
        }
        Finalise(source);
        if (mode) {
            m_bResetStatics = false;
        }
        return true;
    }

    bool moved{};
    if (mode == 2 && cameraCop) {
        moved = GetArrestCameraPosition(target, cameraCop, lookAt, source);
        source.z = std::min(source.z, m_vecSource.z + CTimer::GetTimeStep() * StaticRef<float>(0x8CC7EC));
    } else if (mode >= 4 && mode <= 7) {
        source = m_vecSource;
        m_vecFront = (lookAt - source).Normalized();
        m_vecUp = {0.0f, 0.0f, 1.0f};
        auto side = CrossProduct(m_vecFront, m_vecUp).Normalized();
        if (mode == 6 || mode == 7) {
            side = -side;
        }
        if (!CWorld::TestSphereAgainstWorld(source + side * 0.5f, 0.4f, target, true, true, false, true, false, true)) {
            source += side * StaticRef<float>(0x8CC7E4) * CTimer::GetTimeStep();
            if (mode == 5 || mode == 7) {
                source.z += CTimer::GetTimeStep() * StaticRef<float>(0x8CC7E8);
            } else {
                bool foundGround{};
                const auto ground = CWorld::FindGroundZFor3DCoord(source.x, source.y, source.z, &foundGround, nullptr);
                if (foundGround) {
                    source.z = ground + StaticRef<float>(0x8CC7F8);
                }
            }
            moved = true;
        }
    } else if (mode == 8) {
        source = m_vecSource;
        m_vecFront = lookAt - source;
        m_vecFront.z = 0.0f;
        m_vecFront.Normalise();
        m_vecUp = {0.0f, 0.0f, 1.0f};
        const auto side = CrossProduct(m_vecFront, m_vecUp).Normalized();
        m_vecFront = lookAt - source + side * StaticRef<float>(0x8CC7FC);
        m_vecFront.z = 0.0f;
        m_vecFront.Normalise();
        if (!CWorld::TestSphereAgainstWorld(source + m_vecFront * 0.5f, 0.4f, target, true, true, false, true, false, true)) {
            source += m_vecFront * StaticRef<float>(0x8CC800) * CTimer::GetTimeStep();
            moved = true;
        }
    }
    if (moved) {
        Finalise(source);
    } else {
        const auto beforeCollision = m_vecSource;
        TheCamera.AvoidTheGeometry(&beforeCollision, &lookAt, &m_vecSource, m_fFOV);
    }
    return true;
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
void CCam::Process_AimWeapon(const CVector& target, float, float, float) {
    static auto& initialized = StaticRef<uint8>(0xB70110);
    static auto& bufferedTarget = StaticRef<CVector>(0xB70104);
    static auto& meleePitch = StaticRef<float>(0xB70100);
    static auto& meleeYaw = StaticRef<float>(0xB700FC);
    static auto& meleeSideTimer = StaticRef<float>(0xB700F8);
    static auto& meleeTargetBlend = StaticRef<float>(0xB700F4);
    static auto& manualRotation = StaticRef<bool>(0xB6EC44);
    static auto& timeWithoutInput = StaticRef<int32>(0xB6EC48);
    static auto& lastTargetPressed = StaticRef<uint32>(0xB6EC4C);
    static auto& recenterHeading = StaticRef<float>(0x8CC530);
    if (!(initialized & 1)) {
        initialized |= 1;
        bufferedTarget.Reset();
    }
    if (!m_pCamTargetEntity->IsPed() || !m_pCamTargetEntity->AsPed()->IsPlayer()) {
        return;
    }
    auto* ped = m_pCamTargetEntity->AsPed();
    auto* intelligence = ped->GetIntelligence();
    auto* gunTask = intelligence->GetTaskUseGun();
    auto* weapon = gunTask ? gunTask->m_WeaponInfo : CWeaponInfo::GetWeaponInfo(ped->GetActiveWeapon().m_Type, eWeaponSkill::STD);
    auto* vehicle = ped->bInVehicle ? ped->m_pVehicle : nullptr;
    const bool driver = vehicle && vehicle->m_pDriver == ped;
    int32 aimingType{};
    if (ped->bInVehicle) {
        aimingType = vehicle && (vehicle->m_nVehicleType == VEHICLE_TYPE_BIKE || vehicle->m_nVehicleSubType == VEHICLE_TYPE_QUAD) ? 1 : 2;
    } else if (intelligence->GetTaskJetPack()) {
        aimingType = 1;
    } else if (ped->GetActiveWeapon().IsTypeMelee()) {
        aimingType = 3;
    }
    const auto& settings = StaticRef<std::array<std::array<float, 7>, 4>>(0x8CC4C0)[aimingType];
    const auto timeStep = CTimer::GetTimeStep();
    const auto weaponType = ped->GetActiveWeapon().m_Type;
    auto wantedFOV = 70.0f;
    if (weaponType == WEAPON_AK47 || weaponType == WEAPON_M4) {
        wantedFOV = StaticRef<float>(0x8CC4B4);
    } else if (weaponType == WEAPON_COUNTRYRIFLE) {
        wantedFOV = StaticRef<float>(0x8CC4B8);
    }
    if (!TheCamera.m_bTransitionState) {
        if (m_bResetStatics && weaponType != WEAPON_COUNTRYRIFLE) {
            m_fFOV = wantedFOV;
        } else {
            const auto step = timeStep * StaticRef<float>(0x862F1C);
            m_fFOV = m_fFOV + step < wantedFOV ? m_fFOV + step : std::max(m_fFOV - step, wantedFOV);
        }
    }
    const bool melee = weapon->m_nWeaponFire == WEAPON_FIRE_MELEE;
    float pitchOffset, yawOffset;
    if (melee) {
        if (!(initialized & 2)) {
            initialized |= 2;
            meleePitch = StaticRef<float>(0x862F20);
        }
        if (!(initialized & 4)) {
            initialized |= 4;
            meleeYaw = StaticRef<float>(0x862F24);
        }
        auto pitch = StaticRef<float>(0x862F20);
        auto yaw = StaticRef<float>(0x862F24);
        float blend{};
        if (intelligence->GetTaskFighting() && ped->m_nMoveState < PEDMOVE_WALK && ped->m_pTargetedObject) {
            if (meleeSideTimer > timeStep) {
                meleeSideTimer -= timeStep;
            } else if (meleeSideTimer < -timeStep) {
                meleeSideTimer += timeStep;
            } else {
                const auto origin = ped->GetPosition() + CVector{0.0f, 0.0f, 0.75f};
                auto side = CrossProduct(ped->m_pTargetedObject->GetPosition() - ped->GetPosition(), CVector{0.0f, 0.0f, 1.0f});
                side *= 2.0f / std::max(0.7f, side.Magnitude());
                meleeSideTimer = CWorld::GetIsLineOfSightClear(origin, origin + side, true, true, false, true, false, true, true) ? 100.0f : -100.0f;
            }
            if (meleeSideTimer >= 0.0f) {
                pitch = StaticRef<float>(0x862F28);
                yaw = StaticRef<float>(0x862F2C);
                blend = 1.0f;
            }
        }
        if (m_bResetStatics) {
            meleePitch = pitch;
            meleeYaw = yaw;
            meleeTargetBlend = 0.0f;
        } else if (!TheCamera.m_bTransitionState) {
            const auto damping = std::pow(StaticRef<float>(0x862F30), timeStep);
            meleePitch = meleePitch * damping + pitch * (1.0f - damping);
            meleeYaw = meleeYaw * damping + yaw * (1.0f - damping);
            meleeTargetBlend = meleeTargetBlend * damping + blend * (1.0f - damping);
        }
        pitchOffset = DegreesToRadians(meleePitch);
        yawOffset = DegreesToRadians(meleeYaw);
    } else {
        meleeTargetBlend = 0.0f;
        const auto halfFOV = std::tan(DegreesToRadians(m_fFOV * 0.5f));
        yawOffset = std::atan((CCamera::m_f3rdPersonCHairMultX - 0.5f) * 2.0f * halfFOV);
        pitchOffset = std::atan((0.5f - CCamera::m_f3rdPersonCHairMultY) * 2.0f * halfFOV / CDraw::ms_fAspectRatio);
    }
    if (m_bResetStatics) {
        TheCamera.ResetDuckingSystem(ped);
        m_bRotating = false;
        m_bCollisionChecksOn = true;
        m_fAlphaSpeed = m_fBetaSpeed = 0.0f;
        manualRotation = true;
        timeWithoutInput = 60000;
        recenterHeading = -1001.0f;
        lastTargetPressed = 0;
        if (!CCamera::m_bUseMouse3rdPerson || ped->m_pTargetedObject) {
            m_fVerticalAngle = settings[3];
            if (vehicle) {
                m_fHorizontalAngle = ped->m_fCurrentRotation - HALF_PI - yawOffset;
                m_fVerticalAngle += std::asin(std::clamp(ped->GetMatrix().GetForward().z, -1.0f, 1.0f));
            } else if (!ped->m_pTargetedObject) {
                m_fHorizontalAngle = ped->m_fCurrentRotation - HALF_PI + yawOffset;
                if (ped->bIsStanding) {
                    m_fVerticalAngle -= std::asin(std::clamp(DotProduct(ped->field_578, ped->GetMatrix().GetForward()), -1.0f, 1.0f));
                    if (weaponType == WEAPON_EXTINGUISHER) {
                        m_fVerticalAngle += StaticRef<float>(0x8D610C);
                    }
                }
            }
        }
    }
    auto& forcedStep = StaticRef<float>(0xA44498);
    const auto forcedHeading = StaticRef<float>(0xA4449C);
    if (forcedStep > 0.0f) {
        auto delta = m_fHorizontalAngle - forcedHeading;
        if (delta < 0.0f) {
            delta += TWO_PI;
        }
        if (delta < forcedStep || TWO_PI - delta < forcedStep) {
            m_fHorizontalAngle = forcedHeading;
            forcedStep = 0.0f;
        } else {
            m_fHorizontalAngle += delta <= TWO_PI - delta ? -forcedStep : forcedStep;
        }
    }

    auto lookAt = target;
    ped->UpdateRpHAnim();
    lookAt.z = ped->GetPosition().z + 0.5f + settings[4];
    if (m_fFOV < 70.0f) {
        lookAt.z += std::min((70.0f - m_fFOV) / (70.0f - StaticRef<float>(0x8CC4B4)), 1.0f) * StaticRef<float>(0x858B1C);
    }
    const auto heightOffset = lookAt.z - target.z;
    auto sideDistance = 0.2f;
    if (!weapon->flags.bAimWithArm && ped->GetPlayerData()->m_pPedClothesDesc->HasVisibleNewHairCut(1)) {
        sideDistance = 0.3f;
    } else if (m_fFOV < 70.0f) {
        sideDistance += std::min((70.0f - m_fFOV) / (70.0f - StaticRef<float>(0x8CC4B8)), 1.0f) * StaticRef<float>(0x858B1C);
    }
    if (StaticRef<bool>(0x8CCE64)) {
        const auto right = CrossProduct(m_vecFront, m_vecUp);
        const auto alignment = std::clamp(DotProduct(right, ped->GetMatrix().GetRight()), 0.0f, 1.0f);
        lookAt += right * ((1.0f - std::acos(alignment) * StaticRef<float>(0x858FB8)) * sideDistance);
    } else {
        lookAt += ped->GetMatrix().GetRight() * sideDistance;
    }
    if (auto* locked = ped->m_pTargetedObject) {
        CVector position;
        if (locked->IsPed() && !melee) {
            locked->AsPed()->GetBonePosition(&position, BONE_SPINE1, true);
        } else {
            position = locked->GetPosition();
        }
        if (melee) {
            position.z += heightOffset * StaticRef<float>(0x8CCE60);
        }
        if (!m_bResetStatics && intelligence->GetTaskFighting()) {
            const auto damping = std::pow(StaticRef<float>(0x8CC39C), timeStep);
            bufferedTarget = bufferedTarget * damping + position * (1.0f - damping);
        } else {
            bufferedTarget = position;
        }
    }

    auto* pad = CPad::GetPad(0);
    if (ped->m_pTargetedObject) {
        const auto delta = bufferedTarget - lookAt;
        auto pitch = std::atan2(delta.z, delta.Magnitude2D());
        if (melee) {
            pitch *= std::cos(yawOffset);
        } else {
            const auto distance = std::min((lookAt - m_vecSource).Magnitude(), settings[0]);
            const auto scale = (delta.Magnitude() + distance) / delta.Magnitude();
            pitchOffset *= scale;
            yawOffset *= scale;
        }
        auto yaw = std::atan2(-delta.x, delta.y) - HALF_PI + yawOffset;
        pitch -= pitchOffset;
        if (pitch < -PI) {
            pitch += TWO_PI;
        } else if (pitch > PI) {
            pitch -= TWO_PI;
        }
        const auto step = m_bResetStatics ? 1000.0f : timeStep * StaticRef<float>(0x8CC4A4);
        m_fVerticalAngle += std::clamp(pitch - m_fVerticalAngle, -step, step);
        auto yawDelta = yaw - m_fHorizontalAngle;
        if (yawDelta > PI) {
            yaw -= TWO_PI;
        } else if (yawDelta < -PI) {
            yaw += TWO_PI;
        }
        m_fHorizontalAngle += std::clamp(yaw - m_fHorizontalAngle, -step, step);
        m_fAlphaSpeed = m_fBetaSpeed = 0.0f;
    } else {
        const auto mouse = pad->NewMouseControllerState.GetAmountMouseMoved();
        const auto fovScale = m_fFOV / 80.0f;
        if (CCamera::m_bUseMouse3rdPerson && !pad->DisablePlayerControls && !mouse.IsZero()) {
            m_fHorizontalAngle += mouse.x * -3.0f * fovScale * CCamera::m_fMouseAccelHorzntl;
            m_fVerticalAngle += mouse.y * 4.0f * fovScale * CCamera::m_fMouseAccelVertical;
            m_fAlphaSpeed = m_fBetaSpeed = 0.0f;
        } else {
            const auto h = (float)-pad->LookAroundLeftRight(ped);
            const auto v = (float)pad->LookAroundUpDown(ped);
            const auto sensitivity = sq(StaticRef<float>(0x8CC4A0));
            auto horizontal = fovScale * StaticRef<float>(0x859B50) * std::abs(h) * h * timeStep * sensitivity;
            auto vertical = fovScale * StaticRef<float>(0x8631AC) * std::abs(v) * v * timeStep * sensitivity;
            const auto damping = std::pow(StaticRef<float>(std::abs(h) < 2.0f && std::abs(v) < 2.0f ? 0x8CCE58 : 0x8CCE5C), timeStep);
            horizontal = m_fBetaSpeed = horizontal * (1.0f - damping) + m_fBetaSpeed * damping;
            vertical = m_fAlphaSpeed = vertical * (1.0f - damping) + m_fAlphaSpeed * damping;
            if (vehicle && !driver && pad->GetEnterTargeting()) {
                const auto now = CTimer::GetTimeInMS();
                if ((float)(now - lastTargetPressed) < StaticRef<float>(0x8CCE54)) {
                    horizontal = PI;
                    vertical = 0.0f;
                } else {
                    lastTargetPressed = now;
                }
            } else if (driver) {
                if (h == 0.0f && v == 0.0f) {
                    if (!pad->GetWeapon(ped)) {
                        timeWithoutInput += (int32)(timeStep * StaticRef<float>(0x858B38) * StaticRef<float>(0x858C4C));
                    }
                } else {
                    timeWithoutInput = 0;
                }
                if (timeWithoutInput > StaticRef<int32>(0x8CC534)) {
                    manualRotation = false;
                    recenterHeading = ped->m_fCurrentRotation - HALF_PI + yawOffset;
                } else if (timeWithoutInput > StaticRef<int32>(0x8CC538)) {
                    auto delta = ped->m_fCurrentRotation - HALF_PI - yawOffset - m_fHorizontalAngle;
                    if (delta > TWO_PI) {
                        delta -= TWO_PI;
                    } else if (delta < StaticRef<float>(0x863234)) {
                        delta += TWO_PI;
                    }
                    if (delta < StaticRef<float>(0x858F20)) {
                        recenterHeading = ped->m_fCurrentRotation - HALF_PI + yawOffset;
                        timeWithoutInput = StaticRef<int32>(0x8CC534) + 1;
                        manualRotation = false;
                    } else {
                        manualRotation = true;
                    }
                } else {
                    manualRotation = true;
                    if (pad->GetWeapon(ped)) {
                        timeWithoutInput = 0;
                    }
                }
            }
            if (manualRotation) {
                recenterHeading = -1001.0f;
                m_fHorizontalAngle += horizontal;
                m_fVerticalAngle += vertical;
            } else {
                if (recenterHeading < StaticRef<float>(0x859948)) {
                    recenterHeading = ped->m_fCurrentRotation - HALF_PI;
                }
                if (!weapon->flags.bAimWithArm && !melee) {
                    const auto heading = recenterHeading + HALF_PI;
                    ped->m_fAimingRotation = ped->m_fCurrentRotation = heading;
                    ped->SetHeading(heading);
                    ped->UpdateRwMatrix();
                }
                auto step = timeStep * StaticRef<float>(0x8CC4A8);
                auto deadZone = 0.0f;
                if (driver) {
                    step *= StaticRef<float>(0x8CC4AC);
                    deadZone = StaticRef<float>(0x8CC4B0);
                }
                const auto GetCorrection = [&](float desired, float current) {
                    auto delta = desired - current;
                    if (delta > PI) {
                        delta -= TWO_PI;
                    } else if (delta < -PI) {
                        delta += TWO_PI;
                    }
                    return delta > deadZone ? delta - deadZone : delta < -deadZone ? delta + deadZone : 0.0f;
                };
                const auto correction = GetCorrection(recenterHeading - yawOffset, m_fHorizontalAngle);
                if (std::abs(correction) < step) {
                    manualRotation = true;
                }
                m_fHorizontalAngle += std::clamp(correction, -step, step);
                if (driver) {
                    const auto pitch = std::asin(std::clamp(ped->GetMatrix().GetForward().z, -1.0f, 1.0f)) + settings[3];
                    m_fVerticalAngle += std::clamp(GetCorrection(pitch, m_fVerticalAngle), -step, step);
                } else {
                    m_fVerticalAngle += vertical;
                }
            }
        }
    }
    ClipBeta();
    m_fVerticalAngle = std::clamp(m_fVerticalAngle, -settings[6], settings[5]);
    const auto distancePitch = m_fVerticalAngle > 0.0f ? std::min(settings[2] * m_fVerticalAngle, HALF_PI) : m_fVerticalAngle;
    const auto distance = std::cos(distancePitch) * settings[1] + settings[0];
    m_vecFront = {-std::cos(m_fHorizontalAngle) * std::cos(m_fVerticalAngle), -std::sin(m_fHorizontalAngle) * std::cos(m_fVerticalAngle), std::sin(m_fVerticalAngle)};
    m_vecSource = lookAt - m_vecFront * distance;
    TheCamera.HandleCameraMotionForDuckingDuringAim(ped, &m_vecSource, &lookAt, false);
    m_vecTargetCoorsForFudgeInter = lookAt;
    CCamera::SetColVarsAimWeapon(aimingType);
    if (gCameraDirection == 3) {
        TheCamera.CameraGenericModeSpecialCases(ped);
        TheCamera.CameraPedAimModeSpecialCases(ped);
        TheCamera.CameraColDetAndReact(&m_vecSource, &lookAt);
        TheCamera.ImproveNearClip(nullptr, ped, &m_vecSource, &lookAt);
    }
    TheCamera.m_bCamDirectlyBehind = TheCamera.m_bCamDirectlyInFront = false;
    if (meleeTargetBlend > 0.0f && ped->m_pTargetedObject) {
        const auto blend = meleeTargetBlend * 0.5f;
        m_vecFront = (lookAt * (1.0f - blend) + bufferedTarget * blend - m_vecSource).Normalized();
    }
    GetVectorsReadyForRW();
    if ((!weapon->flags.bAimWithArm || ped->bIsDucking) && !melee && !ped->bInVehicle) {
        float heading = -1000.0f;
        if (weaponType == WEAPON_SPRAYCAN) {
            heading = std::atan2(-m_vecFront.x, m_vecFront.y) - yawOffset;
        } else if (ped->m_pTargetedObject) {
            const auto delta = ped->m_pTargetedObject->GetPosition() - ped->GetPosition();
            heading = std::atan2(-delta.x, delta.y);
        } else if (manualRotation) {
            heading = std::atan2(-m_vecFront.x, m_vecFront.y) - yawOffset;
        }
        if (heading > -100.0f) {
            ped->m_fCurrentRotation = ped->m_fAimingRotation = heading + StaticRef<float>(0x862F18);
            ped->SetHeading(heading);
            ped->UpdateRwMatrix();
        }
        TheCamera.m_pTargetEntity->AsPed()->GetPlayerData()->m_fLookPitch = TheCamera.Find3rdPersonQuickAimPitch();
    }
    m_bResetStatics = false;
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
    auto& focus = CGameLogic::n2PlayerPedInFocus;
    auto* first = FindPlayerPed(PED_TYPE_PLAYER1);
    auto* second = FindPlayerPed(PED_TYPE_PLAYER2);
    if (CPad::GetPad(0)->CycleCameraModeJustDown()) {
        focus = focus == eFocusedPlayer::PLAYER1 ? eFocusedPlayer::NONE : eFocusedPlayer::PLAYER1;
    } else if (CPad::GetPad(1)->CycleCameraModeJustDown()) {
        focus = focus == eFocusedPlayer::PLAYER2 ? eFocusedPlayer::NONE : eFocusedPlayer::PLAYER2;
    }
    if (focus == eFocusedPlayer::PLAYER1 || focus == eFocusedPlayer::PLAYER2) {
        auto* player = focus == eFocusedPlayer::PLAYER1 ? first : second;
        if (player->bInVehicle && player->m_pVehicle) {
            m_pCamTargetEntity = player->m_pVehicle;
            Process_FollowCar_SA(player->m_pVehicle->GetPosition(), 0.0f, 0.0f, 0.0f, false);
            m_pCamTargetEntity = first;
        } else {
            if (player == second) {
                m_pCamTargetEntity = second;
            }
            Process_FollowPed_SA(player->GetPosition(), 0.0f, 0.0f, 0.0f, false);
            if (player == second) {
                m_pCamTargetEntity = first;
            }
        }
        m_bResetStatics = false;
        return;
    }

    auto& lastClearTime = StaticRef<uint32>(0xB6EC24);
    auto& lastObstructionTime = StaticRef<uint32>(0xB6EC28);
    const auto now = CTimer::GetTimeInMS();
    const auto timeStep = CTimer::GetTimeStep();
    m_fVerticalAngle = DegreesToRadians(-30.0f);
    m_fAlphaSpeed = 0.0f;
    CVector candidateSource, candidateFront;
    float candidateBeta{};
    int32 candidate = 0;
    for (; candidate < 21; candidate++) {
        const auto offset = (float)((candidate + 1) / 2) * 0.15f;
        candidateBeta = m_fHorizontalAngle + (candidate & 1 ? offset : -offset);
        GetTwoPlayerCameraPosition(candidateBeta, candidateSource, candidateFront, m_vecTargetCoorsForFudgeInter);
        if (CanSeeBothPlayers(candidateSource)) {
            lastClearTime = now;
            break;
        }
    }
    if (candidate == 21) {
        candidateBeta = m_fHorizontalAngle;
    }
    if (candidate > 0) {
        lastObstructionTime = now;
    }
    if (m_bResetStatics) {
        m_fHorizontalAngle = candidateBeta;
    }
    const auto relativeAngle = [](float angle, float reference) {
        if (angle > reference + PI) {
            return angle - TWO_PI;
        }
        if (angle < reference - PI) {
            return angle + TWO_PI;
        }
        return angle;
    };
    const auto desiredBeta = relativeAngle(candidateBeta, m_fHorizontalAngle);
    const auto correctedBeta = m_fHorizontalAngle + std::clamp((desiredBeta - m_fHorizontalAngle) * 0.2f, -0.1f, 0.1f);
    float movementRotation{};
    if (candidate == 0 && now >= lastObstructionTime + 1000) {
        const auto movement = first->GetMoveSpeed() + second->GetMoveSpeed();
        if (movement.SquaredMagnitude() > 0.01f) {
            const auto movementHeading = relativeAngle(std::atan2(-movement.x, movement.y) - HALF_PI, correctedBeta);
            const auto blend = std::min(movement.Magnitude() * StaticRef<float>(0x8CC5E8) * timeStep, 1.0f);
            const auto maxRotation = StaticRef<float>(0x8CC5EC) * timeStep;
            movementRotation = std::clamp((movementHeading - correctedBeta) * blend, -maxRotation, maxRotation);
            if (movementRotation > 0.01f) {
                candidateBeta += 0.15f;
            } else if (movementRotation < 0.01f) {
                candidateBeta -= 0.15f;
            }
        } else {
            candidateBeta -= 0.15f;
        }
        if (std::abs(movementRotation) > 0.01f) {
            GetTwoPlayerCameraPosition(candidateBeta, candidateSource, candidateFront, m_vecTargetCoorsForFudgeInter);
            if (!CanSeeBothPlayers(candidateSource)) {
                if (movementRotation * m_fBetaSpeed > 0.0f) {
                    m_fBetaSpeed = 0.0f;
                }
                lastObstructionTime = now;
                movementRotation = 0.0f;
            }
        }
    }
    auto desiredSpeed = (relativeAngle(correctedBeta + movementRotation, m_fHorizontalAngle) - m_fHorizontalAngle) / std::max(timeStep, 1.0f);
    const auto damping = std::pow(StaticRef<float>(0x8CC5E0), timeStep);
    if (candidate == 0 && now >= lastObstructionTime + 1000) {
        const auto input = std::clamp(
            -(float)CPad::GetPad(0)->AimWeaponLeftRight(first) - (float)CPad::GetPad(1)->AimWeaponLeftRight(second),
            StaticRef<float>(0x85F3C4), StaticRef<float>(0x858BF4)
        );
        const auto inputScale = StaticRef<float>(0x8CC4A0);
        auto rotation = m_fFOV / 80.0f / 14.0f * std::abs(input) * inputScale * inputScale * input;
        if (rotation > 0.01f) {
            candidateBeta += 0.15f;
        } else if (rotation < 0.01f) {
            candidateBeta -= 0.15f;
        }
        if (std::abs(rotation) > 0.01f) {
            GetTwoPlayerCameraPosition(candidateBeta, candidateSource, candidateFront, m_vecTargetCoorsForFudgeInter);
            if (!CanSeeBothPlayers(candidateSource)) {
                if (rotation * m_fBetaSpeed > 0.0f) {
                    m_fBetaSpeed = 0.0f;
                }
                lastObstructionTime = now;
                rotation = 0.0f;
            }
        }
        desiredSpeed += rotation;
    }
    const auto maxSpeed = StaticRef<float>(0x8CC5E4);
    m_fBetaSpeed = damping * m_fBetaSpeed + (1.0f - damping) * std::clamp(desiredSpeed, -maxSpeed, maxSpeed);
    m_fHorizontalAngle += m_fBetaSpeed * timeStep;
    GetTwoPlayerCameraPosition(m_fHorizontalAngle, m_vecSource, m_vecFront, m_vecTargetCoorsForFudgeInter);
    if (candidate == 21 && now - lastClearTime > 500) {
        gCurCamColVars = 5;
        CColPoint collision{};
        CEntity* hitEntity{};
        if (CWorld::ProcessLineOfSight(m_vecTargetCoorsForFudgeInter, m_vecSource, collision, hitEntity, true, false, false, false, false, true, true, false)) {
            m_vecSource = collision.m_vecPoint;
        }
        if ((uint32)CGameLogic::nPrintFocusHelpTimer < now && CGameLogic::nPrintFocusHelpCounter < 6) {
            CHud::SetHelpMessage(TheText.Get("WRN2_2P"), false, false, false);
            CGameLogic::nPrintFocusHelpTimer = now + 60000;
            ++CGameLogic::nPrintFocusHelpCounter;
        }
    }
    m_vecFront.Normalise();
    const auto right = CrossProduct(CVector{0.0f, 0.0f, 1.0f}, m_vecFront).Normalized();
    m_vecUp = CrossProduct(m_vecFront, right).Normalized();
    m_fFOV = 70.0f;
    m_bResetStatics = false;
}

// 0x519810
void CCam::Process_Cam_TwoPlayer_InCarAndShooting() {
    if (!m_pCamTargetEntity->IsVehicle()) {
        return;
    }
    auto* first = FindPlayerPed(PED_TYPE_PLAYER1);
    auto* vehicle = first->m_pVehicle;
    auto target = vehicle->GetPosition();
    const auto heading = vehicle->GetHeading() - HALF_PI;
    const auto firstDriving = vehicle->m_pDriver == first;
    auto* shooter = firstDriving ? FindPlayerPed(PED_TYPE_PLAYER2) : first;
    auto* pad = CPad::GetPad(firstDriving ? 1 : 0);
    const auto timeStep = CTimer::GetTimeStep();
    const auto forwardSpeed = DotProduct(vehicle->GetMoveSpeed(), vehicle->GetMatrix().GetForward());
    if ((vehicle->IsSubAutomobile() || vehicle->IsSubBike()) && forwardSpeed > StaticRef<float>(0x8CC540)) {
        m_fFOV += (forwardSpeed - StaticRef<float>(0x8CC540)) * timeStep;
    }
    if (m_fFOV > 70.0f) {
        m_fFOV = (m_fFOV - 70.0f) * std::pow(StaticRef<float>(0x8CC544), timeStep) + 70.0f;
    }
    m_fFOV = std::clamp(m_fFOV, 70.0f, 100.0f);

    const auto horizontal = (float)pad->AimWeaponLeftRight(shooter);
    const auto vertical = -(float)pad->AimWeaponUpDown(shooter);
    const auto inputScale = StaticRef<float>(0x862F8C);
    const auto fovScale = m_fFOV / 80.0f;
    m_fX_Targetting += fovScale / 14.0f * std::abs(horizontal) * timeStep * inputScale * inputScale * horizontal;
    m_fY_Targetting += fovScale * (3.0f / 70.0f) * std::abs(vertical) * timeStep * inputScale * inputScale * vertical;
    auto& weapon = shooter->GetActiveWeapon();
    const auto& weaponInfo = weapon.GetWeaponInfo(shooter);
    float targetX{}, targetY{};
    auto* aimEntity = CWeapon::FindNearestTargetEntityWithScreenCoors(m_fX_Targetting, m_fY_Targetting, weaponInfo.m_fWeaponRange * 2.0f, shooter->GetPosition(), &targetX, &targetY);
    if (aimEntity && std::abs(horizontal) < StaticRef<float>(0x858BB0) && std::abs(vertical) < StaticRef<float>(0x858BB0)) {
        const auto blend = 1.0f - std::pow(StaticRef<float>(0x862F90), timeStep);
        const auto maxStep = timeStep * StaticRef<float>(0x862F94);
        m_fX_Targetting += std::clamp((targetX - m_fX_Targetting) * blend, -maxStep, maxStep);
        m_fY_Targetting += std::clamp((targetY - m_fY_Targetting) * blend, -maxStep, maxStep);
    }
    const auto horizontalOverflow = m_fX_Targetting - std::clamp(m_fX_Targetting, -0.9f, 0.9f);
    m_fX_Targetting = std::clamp(m_fX_Targetting, -0.9f, 0.9f);
    const auto verticalOverflow = m_fY_Targetting - std::clamp(m_fY_Targetting, -0.9f, 0.9f);
    m_fY_Targetting = std::clamp(m_fY_Targetting, -0.9f, 0.9f);
    m_fVerticalAngle -= verticalOverflow * timeStep * StaticRef<float>(0x862F6C);
    if (std::abs(horizontal) < 1.0f && std::abs(vertical) < 1.0f && !aimEntity) {
        const CVector2D displacement{m_fX_Targetting, m_fY_Targetting + 0.4f};
        const auto distance = displacement.Magnitude();
        const auto step = timeStep * StaticRef<float>(0x858F44);
        if (step > distance) {
            m_fX_Targetting = 0.0f;
            m_fY_Targetting = -0.4f;
        } else {
            m_fX_Targetting -= step / distance * displacement.x;
            m_fY_Targetting -= step / distance * displacement.y;
        }
    }

    const auto& bounds = vehicle->GetColModel()->m_boundBox;
    auto maxDistance = StaticRef<float>(0x862F60) + std::abs(bounds.m_vecMin.y) * 2.0f;
    auto desiredPitch = StaticRef<float>(0x862F68);
    if (vehicle->GetVehicleAppearance() == VEHICLE_APPEARANCE_HELI && vehicle->GetStatus() != STATUS_REMOTE_CONTROLLED) {
        target += vehicle->GetMatrix().GetUp() * StaticRef<float>(0x8CC53C) * bounds.m_vecMax.z;
    } else {
        const auto height = StaticRef<float>(0x8CC600) * bounds.m_vecMax.z - StaticRef<float>(0x8CC608);
        if (height > 0.0f) {
            target.z += height;
            maxDistance += height;
            desiredPitch += StaticRef<float>(0x8CCD1C) / maxDistance * height;
        }
    }
    m_fCaMinDistance = maxDistance * 0.9f;
    maxDistance += StaticRef<float>(0x862F64);
    m_fCaMaxDistance = maxDistance;
    const auto displacement = m_vecSource - target;
    auto distance = displacement.Magnitude2D();
    m_fDistanceBeforeChanges = distance;
    if (distance < (float)StaticRef<double>(0x8631F8)) {
        distance = StaticRef<float>(0x858F44);
    }
    m_fHorizontalAngle = std::atan2(displacement.x, -displacement.y) - HALF_PI;
    if (distance > maxDistance || distance < m_fCaMinDistance) {
        const auto scale = (distance > maxDistance ? maxDistance : m_fCaMinDistance) / distance;
        m_vecSource.x = target.x + displacement.x * scale;
        m_vecSource.y = target.y + displacement.y * scale;
    }
    const auto& velocity = vehicle->GetMoveSpeed();
    if (velocity.SquaredMagnitude() > 0.0001f) {
        const auto pitch = std::atan2(velocity.z, velocity.Magnitude2D());
        const auto speedScale = vehicle->IsSubHeli() ? StaticRef<float>(0x862F88) : StaticRef<float>(0x862F7C);
        desiredPitch += std::min((velocity.Magnitude() - 0.01f) * speedScale, 1.0f) * pitch;
        const auto damping = std::pow(StaticRef<float>(0x862F78), timeStep);
        m_fVerticalAngle = damping * m_fVerticalAngle + (1.0f - damping) * desiredPitch;
    }
    const auto minPitch = vehicle->IsSubHeli() ? StaticRef<float>(0x862F80) : StaticRef<float>(0x862F70);
    const auto maxPitch = vehicle->IsSubHeli() ? StaticRef<float>(0x862F84) : StaticRef<float>(0x862F74);
    m_fVerticalAngle = std::clamp(m_fVerticalAngle, minPitch, maxPitch);
    m_vecSource.z = target.z - std::sin(m_fVerticalAngle) * maxDistance;
    RotCamIfInFrontCar(target, heading);
    m_vecTargetCoorsForFudgeInter = target;
    auto source = m_vecSource;
    TheCamera.AvoidTheGeometry(&source, &m_vecTargetCoorsForFudgeInter, &m_vecSource, m_fFOV);
    const auto offset = m_vecSource - target;
    m_vecSource.x = target.x + std::cos(horizontalOverflow) * offset.x + std::sin(horizontalOverflow) * offset.y;
    m_vecSource.y = target.y + std::cos(horizontalOverflow) * offset.y - std::sin(horizontalOverflow) * offset.x;
    m_vecFront = target - m_vecSource;
    m_bResetStatics = false;
    GetVectorsReadyForRW();

    if (!vehicle->CanPedLeanOut(shooter)) {
        weapon.Update(shooter);
    }
    if (!pad->GetCarGunFired() || vehicle->CanPedLeanOut(shooter) || weapon.IsTypeMelee() || weapon.GetState() != WEAPONSTATE_READY) {
        return;
    }
    CVector aimPosition;
    if (aimEntity) {
        aimPosition = aimEntity->GetPosition();
    } else {
        const auto right = CrossProduct(m_vecFront, m_vecUp);
        const auto tanFOV = std::tan(DegreesToRadians(m_fFOV) * 0.5f);
        aimPosition = m_vecSource + (m_vecFront + right * (m_fX_Targetting * tanFOV) - m_vecUp * (tanFOV / CDraw::ms_fAspectRatio * m_fY_Targetting)) * (weaponInfo.m_fWeaponRange * 3.0f);
    }
    const auto aimDirection = aimPosition - m_vecSource;
    auto relativeHeading = std::atan2(-aimDirection.x, aimDirection.y) - vehicle->GetHeading();
    if (relativeHeading > PI) {
        relativeHeading -= TWO_PI;
    } else if (relativeHeading < -PI) {
        relativeHeading += TWO_PI;
    }
    relativeHeading += DegreesToRadians(45.0f);
    if (relativeHeading < 0.0f) {
        relativeHeading += TWO_PI;
    }
    CTaskSimpleGangDriveBy driveBy{nullptr, nullptr, 100.0f, 100, eDrivebyStyle::AI_ALL_DIRN, shooter != vehicle->m_apPassengers[1]};
    driveBy.m_pWeaponInfo = &weapon.GetWeaponInfo(shooter);
    driveBy.m_nFakeShootDirn = (int8)(relativeHeading * StaticRef<float>(0x858FB8));
    // CTaskSimpleGangDriveBy::FireGun is implemented in the original task code.
    plugin::CallMethod<0x627CC0, CTaskSimpleGangDriveBy*, CPed*>(&driveBy, shooter);
    CamShakeNoPos(&TheCamera, StaticRef<float>(0x8CCD18));
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
void CCam::Process_FollowCar_SA(const CVector& target, float, float, float, bool preserveAngles) {
    if (!m_pCamTargetEntity->IsVehicle()) {
        return;
    }
    auto* vehicle = m_pCamTargetEntity->AsVehicle();
    auto* pad = CPad::GetPad(vehicle->m_pDriver && vehicle->m_pDriver->m_nPedType == PED_TYPE_PLAYER2 ? 1 : 0);
    auto lookAt = target;
    TheCamera.ApplyVehicleCameraTweaks(vehicle);
    const auto model = vehicle->m_nModelIndex;
    const auto type = vehicle->m_nVehicleType;
    const auto subtype = vehicle->m_nVehicleSubType;
    const auto appearance = vehicle->GetVehicleAppearance();
    const bool remoteControlled = vehicle->GetStatus() == STATUS_REMOTE_CONTROLLED;
    uint32 category{};
    if (model == MODEL_RCBANDIT || model == MODEL_RCBARON || model == MODEL_RCTIGER || model == MODEL_RCCAM) {
        category = 5;
    } else if (model == MODEL_RCRAIDER || model == MODEL_RCGOBLIN) {
        category = 6;
    } else if (type == VEHICLE_TYPE_BIKE || subtype == VEHICLE_TYPE_QUAD) {
        category = 1;
    } else if (subtype == VEHICLE_TYPE_HELI) {
        category = 2;
    } else if (subtype == VEHICLE_TYPE_PLANE) {
        if (model == MODEL_HYDRA && vehicle->AsAutomobile()->m_wMiscComponentAngle >= StaticRef<int32>(0x8D33C8)) {
            category = 2;
        } else {
            category = model == MODEL_VORTEX ? 0 : 3;
        }
    } else if (subtype == VEHICLE_TYPE_BOAT) {
        category = 4;
    }
    const auto& settings = StaticRef<std::array<std::array<float, 15>, 7>>(0x8CC600)[category];
    const auto timeStep = CTimer::GetTimeStep();
    auto distance = TheCamera.m_fCarZoomSmoothed + settings[1];
    int32 zoomIndex{};
    TheCamera.GetArrPosForVehicleType(static_cast<eVehicleType>(appearance), zoomIndex);
    float pitchOffset{};
    if (remoteControlled || TheCamera.m_nCarZoom == 2) {
        pitchOffset = StaticRef<std::array<float, 5>>(0x8CC430)[zoomIndex];
    } else if (TheCamera.m_nCarZoom == 1) {
        pitchOffset = StaticRef<std::array<float, 5>>(0x8CC41C)[zoomIndex];
    } else if (TheCamera.m_nCarZoom == 3) {
        pitchOffset = StaticRef<std::array<float, 5>>(0x8CC444)[zoomIndex];
    }
    const auto& bounds = vehicle->GetColModel()->GetBoundingBox();
    auto height = bounds.m_vecMax.z;
    auto length = std::abs(bounds.m_vecMin.y) * 2.0f;
    auto& passengerBlend = StaticRef<float>(0xB7011C);
    if (auto* trailer = vehicle->m_pVehicleBeingTowed) {
        if (passengerBlend < 1.0f) {
            passengerBlend = std::min(passengerBlend + timeStep * StaticRef<float>(0x8CCEE8), 1.0f);
        }
        const auto& trailerBounds = trailer->GetColModel()->GetBoundingBox();
        length += (trailerBounds.m_vecMax - trailerBounds.m_vecMin).Magnitude() * StaticRef<float>(0x8CCEE4) * passengerBlend;
        height += (std::max(trailerBounds.m_vecMax.z, height) - height) * passengerBlend;
        const auto blend = passengerBlend * 0.5f;
        lookAt = lookAt * (1.0f - blend) + trailer->GetPosition() * blend;
    } else if (subtype == VEHICLE_TYPE_BIKE || subtype == VEHICLE_TYPE_QUAD) {
        if (vehicle->m_apPassengers[0]) {
            if (passengerBlend < 1.0f) {
                passengerBlend = std::min(passengerBlend + timeStep * StaticRef<float>(0x8CCEE8), 1.0f);
            }
        } else if (passengerBlend > 0.0f) {
            passengerBlend = std::max(passengerBlend - timeStep * StaticRef<float>(0x8CCEE8), 0.0f);
        }
        height += StaticRef<float>(0x8CCEE0) * passengerBlend;
    } else {
        passengerBlend = 0.0f;
    }
    distance += TheCamera.m_fCurrentTweakDistance * length;
    const auto minimumDistance = TheCamera.m_fCurrentTweakDistance * length * settings[3];
    const auto& matrix = vehicle->GetMatrix();
    if (appearance == VEHICLE_APPEARANCE_HELI && !remoteControlled) {
        lookAt += matrix.GetUp() * StaticRef<float>(0x8CC53C) * height;
    } else {
        const auto heightOffset = height * settings[0] - settings[2];
        if (heightOffset > 0.0f) {
            lookAt.z += heightOffset;
            distance += heightOffset;
            pitchOffset += StaticRef<float>(0x8CCEDC) / distance * heightOffset;
        }
    }
    lookAt.z *= TheCamera.m_fCurrentTweakAltitude;
    pitchOffset += TheCamera.m_fCurrentTweakAngle;
    auto historyDistance = settings[4];
    if (TheCamera.m_nCarZoom == 1 && category <= 1) {
        historyDistance *= 0.65f;
    }
    historyDistance = std::max(historyDistance, distance);
    m_fCaMaxDistance = distance;
    m_fCaMinDistance = 3.5f;
    const auto speed = vehicle->GetMoveSpeed();
    if (m_bResetStatics) {
        m_fFOV = 70.0f;
    } else {
        const auto forwardSpeed = DotProduct(speed, matrix.GetForward());
        if ((subtype == VEHICLE_TYPE_AUTOMOBILE || subtype == VEHICLE_TYPE_BIKE) && forwardSpeed > StaticRef<float>(0x8CC540)) {
            m_fFOV += (forwardSpeed - StaticRef<float>(0x8CC540)) * timeStep;
        }
        if (m_fFOV > 70.0f) {
            m_fFOV = (m_fFOV - 70.0f) * std::pow(StaticRef<float>(0x8CC544), timeStep) + 70.0f;
        }
        m_fFOV = std::clamp(m_fFOV, 70.0f, 100.0f);
    }
    if (m_bResetStatics || TheCamera.m_bCamDirectlyBehind || TheCamera.m_bCamDirectlyInFront) {
        m_bResetStatics = m_bRotating = false;
        m_bCollisionChecksOn = true;
        TheCamera.m_bResetOldMatrix = true;
        if (!TheCamera.m_bJustCameOutOfGarage && !preserveAngles) {
            m_fVerticalAngle = 0.0f;
            m_fHorizontalAngle = vehicle->GetHeading() - HALF_PI;
            if (TheCamera.m_bCamDirectlyInFront) {
                m_fHorizontalAngle += PI;
            }
        }
        m_fBetaSpeed = m_fAlphaSpeed = 0.0f;
        m_fDistance = 1000.0f;
        m_vecFront = {-std::cos(m_fHorizontalAngle) * std::cos(m_fVerticalAngle), -std::sin(m_fHorizontalAngle) * std::cos(m_fVerticalAngle), std::sin(m_fVerticalAngle)};
        m_avecTargetHistoryPos[0] = lookAt - m_vecFront * historyDistance;
        m_anTargetHistoryTime[0] = CTimer::GetTimeInMS();
        m_avecTargetHistoryPos[1] = lookAt - m_vecFront * distance;
        m_nCurrentHistoryPoints = 0;
        if (!TheCamera.m_bJustCameOutOfGarage && !preserveAngles) {
            m_fVerticalAngle = -pitchOffset;
        }
    }
    m_vecFront = (lookAt - m_avecTargetHistoryPos[0]).Normalized();
    const auto previousLength = (lookAt - m_avecTargetHistoryPos[1]).Magnitude();
    auto heading = std::atan2(-m_vecFront.x, m_vecFront.y) - HALF_PI;
    if (heading < -PI) {
        heading += TWO_PI;
    }
    auto movementHeading = heading;
    if (speed.Magnitude2D() > StaticRef<float>(0x858B38)) {
        movementHeading = std::atan2(-speed.x, speed.y) - HALF_PI;
    }
    if (movementHeading > heading + PI) {
        movementHeading -= TWO_PI;
    } else if (movementHeading < heading - PI) {
        movementHeading += TWO_PI;
    }
    const auto lateralSpeed = speed - m_vecFront * DotProduct(speed, m_vecFront);
    const auto turnFraction = std::min(timeStep * settings[10] * lateralSpeed.Magnitude(), 1.0f);
    const auto turnStep = timeStep * settings[11];
    heading += std::clamp((movementHeading - heading) * turnFraction, -turnStep, turnStep);
    if (heading > m_fHorizontalAngle + PI) {
        heading -= TWO_PI;
    } else if (heading < m_fHorizontalAngle - PI) {
        heading += TWO_PI;
    }
    const auto headingVelocity = (heading - m_fHorizontalAngle) / std::max(timeStep, 1.0f);
    const auto pitch = std::asin(std::clamp(m_vecFront.z, -1.0f, 1.0f));
    if (previousLength < distance && minimumDistance < distance) {
        distance = std::max(minimumDistance, previousLength);
    }
    auto upperLimit = settings[13];
    const auto lowerLimit = settings[14];
    // The contact counters belong to the bike and automobile layouts respectively.
    const auto contactWheels = [&]() -> uint8 {
        if (type == VEHICLE_TYPE_BIKE) {
            return vehicle->AsBike()->m_nNoOfContactWheels;
        }
        if (type == VEHICLE_TYPE_AUTOMOBILE || subtype == VEHICLE_TYPE_PLANE) {
            return vehicle->AsAutomobile()->m_nNumContactWheels;
        }
        return 0;
    }();
    if (speed.SquaredMagnitude() < StaticRef<float>(0x863244) && (type != VEHICLE_TYPE_BIKE || contactWheels >= 4)
        && subtype != VEHICLE_TYPE_HELI && (subtype != VEHICLE_TYPE_PLANE || contactWheels != 0)) {
        const auto side = CrossProduct(matrix.GetForward(), CVector{0.0f, 0.0f, 1.0f}).Normalized();
        const auto up = CrossProduct(side, matrix.GetForward()).Normalized();
        if (DotProduct(up, m_vecFront) > 0.0f) {
            const auto heightAboveRoad = lookAt.z - vehicle->GetPosition().z + vehicle->GetHeightAboveRoad();
            const auto relativeHeading = m_fHorizontalAngle - (vehicle->GetHeading() - HALF_PI);
            const auto angle = std::asin(std::abs(std::sin(relativeHeading)));
            const auto cornerAngle = std::atan2(bounds.m_vecMax.x, -bounds.m_vecMin.y);
            const auto clearance = angle > cornerAngle
                ? (StaticRef<float>(0x8CCED8) + bounds.m_vecMax.x) / std::cos(std::max(0.0f, HALF_PI - angle))
                : (StaticRef<float>(0x8CCED4) - bounds.m_vecMin.y) / std::cos(angle);
            upperLimit = std::atan2(heightAboveRoad, clearance * StaticRef<float>(0x8CCED0))
                + std::atan2(matrix.GetForward().z, matrix.GetForward().Magnitude2D()) * std::cos(relativeHeading);
            if (type == VEHICLE_TYPE_AUTOMOBILE && contactWheels > 1 && std::abs(DotProduct(vehicle->GetTurnSpeed(), matrix.GetForward())) < 0.05f) {
                upperLimit += std::atan2(matrix.GetRight().z, matrix.GetRight().Magnitude2D()) * std::cos(relativeHeading + HALF_PI);
            }
        }
    }
    auto desiredPitch = std::clamp(pitch - pitchOffset, -lowerLimit, upperLimit);
    const auto pitchStep = timeStep * settings[6];
    const auto pitchCorrection = std::clamp((desiredPitch - m_fVerticalAngle) * (1.0f - std::pow(settings[5], timeStep)), -pitchStep, pitchStep);
    auto horizontal = (float)-pad->LookAroundLeftRight(nullptr);
    auto vertical = CCamera::m_bUseMouse3rdPerson ? 0.0f : (float)pad->LookAroundUpDown(nullptr);
    const auto fovScale = m_fFOV / 80.0f;
    const auto sensitivity = sq(StaticRef<float>(0x8CC4A0));
    horizontal *= fovScale * StaticRef<float>(0x859B50) * std::abs(horizontal) * sensitivity;
    vertical *= fovScale * StaticRef<float>(0x8631AC) * std::abs(vertical) * sensitivity;
    bool fixPitch = true;
    switch (model) {
    case MODEL_PACKER:
    case MODEL_DOZER:
    case MODEL_DUMPER:
    case MODEL_CEMENT:
    case MODEL_ANDROM:
    case MODEL_HYDRA:
    case MODEL_TOWTRUCK:
    case MODEL_FORKLIFT:
    case MODEL_TRACTOR:
        vertical = 0.0f;
        break;
    default:
        if (model == MODEL_RCTIGER || (type == VEHICLE_TYPE_AUTOMOBILE && vehicle->handlingFlags.bHydraulicInst)) {
            horizontal = vertical = 0.0f;
        } else {
            fixPitch = false;
        }
        break;
    }
    if (gCameraDirection != 3) {
        horizontal = vertical = 0.0f;
    }
    if (category == 0 && std::abs((float)pad->GetSteeringUpDown()) > StaticRef<float>(0x858BB0) && vehicle->m_pDriver) {
        auto* task = vehicle->m_pDriver->GetTaskManager().GetActiveTask();
        if (task && task->GetTaskType() != TASK_COMPLEX_LEAVE_CAR) {
            const auto input = (float)pad->GetSteeringUpDown();
            vertical += fovScale * StaticRef<float>(0x8631AC) * std::abs(input) * input * sensitivity * 0.5f;
        }
    }
    if (vertical > 0.0f) {
        vertical *= 0.5f;
    }
    auto& mouseTimer = StaticRef<float>(0xB70118);
    bool mouseControls{};
    if (CCamera::m_bUseMouse3rdPerson && !pad->DisablePlayerControls) {
        const auto mouse = pad->NewMouseControllerState.GetAmountMouseMoved();
        const auto mouseVertical = mouse.y * 2.0f;
        const auto mouseHorizontal = mouse.x * StaticRef<float>(0x858B18);
        const auto mouseSteering = subtype == VEHICLE_TYPE_PLANE || subtype == VEHICLE_TYPE_HELI ? CVehicle::m_bEnableMouseFlying : CVehicle::m_bEnableMouseSteering;
        if ((mouseHorizontal != 0.0f || mouseVertical != 0.0f) && (pad->NewState.m_bVehicleMouseLook || !mouseSteering)) {
            vertical = mouseVertical * fovScale * CCamera::m_fMouseAccelHorzntl;
            horizontal = mouseHorizontal * fovScale * CCamera::m_fMouseAccelHorzntl;
            m_fBetaSpeed = m_fAlphaSpeed = 0.0f;
            desiredPitch = m_fVerticalAngle;
            mouseTimer = StaticRef<float>(0x8CCECC) * StaticRef<float>(0x858B40);
            mouseControls = true;
        } else if (mouseTimer > 0.0f) {
            desiredPitch = m_fVerticalAngle;
            m_fBetaSpeed = m_fAlphaSpeed = 0.0f;
            horizontal = vertical = 0.0f;
            mouseTimer = std::max(0.0f, mouseTimer - timeStep);
            mouseControls = true;
        }
    }
    if (auto* passenger = vehicle->m_apPassengers[0]) {
        auto* task = passenger->GetTaskManager().GetActiveTask();
        if (task && task->GetTaskType() == TASK_COMPLEX_PROSTITUTE_SOLICIT && static_cast<CTaskComplexProstituteSolicit*>(task)->bMoveCameraDown) {
            vertical = m_fVerticalAngle < upperLimit - StaticRef<float>(0x8CCEC8) ? timeStep * StaticRef<float>(0x8CCEC4) : 0.0f;
        }
    }
    auto& pitchFixed = StaticRef<bool>(0xB70114);
    if (fixPitch) {
        if (gCameraMode != MODE_CAM_ON_A_STRING) {
            pitchFixed = false;
        }
        if (!pitchFixed && std::abs(pitchOffset + m_fVerticalAngle) > 0.05f) {
            vertical = (-pitchOffset - m_fVerticalAngle) * StaticRef<float>(0x8CCEC0);
        } else {
            pitchFixed = true;
        }
    }
    horizontal *= settings[12];
    vertical *= settings[12];
    const auto damping = std::pow(settings[8], timeStep);
    m_fBetaSpeed = damping * m_fBetaSpeed + (1.0f - damping) * std::clamp(horizontal + headingVelocity, -settings[9], settings[9]);
    if (std::abs(m_fBetaSpeed) < StaticRef<float>(0x8CCEBC)) {
        m_fBetaSpeed = 0.0f;
    }
    if (!mouseControls) {
        horizontal = timeStep * m_fBetaSpeed;
    }
    m_fHorizontalAngle += horizontal;
    if (TheCamera.m_bJustCameOutOfGarage) {
        m_fHorizontalAngle = CGeneral::GetATanOfXY(m_vecFront.x, m_vecFront.y) + PI;
    }
    ClipBeta();
    if (category < 2 && desiredPitch < m_fVerticalAngle && distance <= previousLength
        && (type == VEHICLE_TYPE_AUTOMOBILE || type == VEHICLE_TYPE_BIKE) && contactWheels > 1) {
        vertical += (desiredPitch - m_fVerticalAngle) * StaticRef<float>(0x8CCEB8);
    }
    m_fAlphaSpeed = damping * m_fAlphaSpeed + (1.0f - damping) * vertical;
    const auto maxVerticalSpeed = vertical > 0.0f ? settings[9] * 0.5f : settings[9];
    m_fAlphaSpeed = std::clamp(m_fAlphaSpeed, -maxVerticalSpeed, maxVerticalSpeed);
    if (std::abs(m_fAlphaSpeed) < StaticRef<float>(0x8CCEB4)) {
        m_fAlphaSpeed = 0.0f;
    }
    auto correction = vertical;
    if (!mouseControls) {
        vertical = timeStep * m_fAlphaSpeed;
        correction = pitchCorrection;
    }
    desiredPitch += vertical;
    m_fVerticalAngle += correction;
    if (m_fVerticalAngle > upperLimit || m_fVerticalAngle < -lowerLimit) {
        m_fVerticalAngle = std::clamp(m_fVerticalAngle, -lowerLimit, upperLimit);
        m_fAlphaSpeed = 0.0f;
    }
    auto& previousPitch = StaticRef<float>(0x8CCEB0);
    auto& previousHeading = StaticRef<float>(0x8CCEA8);
    if (std::abs(previousPitch - m_fVerticalAngle) < StaticRef<float>(0x8CCEAC)) {
        m_fVerticalAngle = previousPitch;
    }
    previousPitch = m_fVerticalAngle;
    if (std::abs(previousHeading - m_fHorizontalAngle) < StaticRef<float>(0x8CCEA4)) {
        m_fHorizontalAngle = previousHeading;
    }
    previousHeading = m_fHorizontalAngle;
    m_vecFront = {-std::cos(m_fHorizontalAngle) * std::cos(m_fVerticalAngle), -std::sin(m_fHorizontalAngle) * std::cos(m_fVerticalAngle), std::sin(m_fVerticalAngle)};
    RoundCamCoordinates(m_vecSource, 4);
    GetVectorsReadyForRW();
    TheCamera.m_bCamDirectlyBehind = TheCamera.m_bCamDirectlyInFront = false;
    m_vecSource = lookAt - m_vecFront * distance;
    m_vecTargetCoorsForFudgeInter = lookAt;
    m_avecTargetHistoryPos[2] = m_avecTargetHistoryPos[0];
    const auto historyPitch = desiredPitch + pitchOffset;
    const CVector historyFront{-std::cos(m_fHorizontalAngle) * std::cos(historyPitch), -std::sin(m_fHorizontalAngle) * std::cos(historyPitch), std::sin(historyPitch)};
    m_avecTargetHistoryPos[0] = lookAt - historyFront * historyDistance;
    m_avecTargetHistoryPos[1] = lookAt - historyFront * distance;
    CCamera::SetColVarsVehicle(static_cast<eVehicleType>(category), TheCamera.m_nCarZoom);
    if (gCameraDirection == 3) {
        TheCamera.m_nExtraEntitiesCount = 0;
        CWorld::pIgnoreEntity = vehicle;
        TheCamera.CameraVehicleModeSpecialCases(vehicle);
        if (vehicle->bIsBig) {
            StaticRef<bool>(0x9655E5) = true;
        }
        TheCamera.CameraColDetAndReact(&m_vecSource, &lookAt);
        TheCamera.ImproveNearClip(vehicle, nullptr, &m_vecSource, &lookAt);
        CWorld::pIgnoreEntity = nullptr;
        RoundCamCoordinates(m_vecSource, 4);
    }
    TheCamera.m_bCamDirectlyBehind = TheCamera.m_bCamDirectlyInFront = false;
    RoundCamCoordinates(m_vecSource, 4);
    GetVectorsReadyForRW();
    StaticRef<CVector>(0xB6F018) = lookAt;
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
void CCam::Process_FollowPed_SA(const CVector& target, float, float, float, bool preserveAngles) {
    if (!m_pCamTargetEntity->IsPed() || !m_pCamTargetEntity->AsPed()->IsPlayer()) {
        return;
    }
    auto* ped = m_pCamTargetEntity->AsPed();
    auto* intelligence = ped->GetIntelligence();
    auto* pad = CPad::GetPad(ped->m_nPedType == PED_TYPE_PLAYER2 ? 1 : 0);
    const auto interior = CGame::currArea != 0;
    const auto& settings = StaticRef<std::array<std::array<float, 15>, 2>>(0x8CC548)[interior];
    const auto timeStep = CTimer::GetTimeStep();
    auto lookAt = target;
    auto distanceOffset = settings[1];
    if (!ped->bIsStanding && TheCamera.m_nPedZoom == 3 && intelligence->GetUsingParachute()) {
        distanceOffset *= 2.0f;
    }
    auto distance = TheCamera.m_fPedZoomSmoothed + distanceOffset;
    auto minimumDistance = settings[3];
    auto& previousDistance = StaticRef<float>(0xB6EC50);
    if (previousDistance < distance) {
        minimumDistance = distance;
    }
    previousDistance = distance;
    auto pitchOffset = settings[2];
    switch (TheCamera.m_nPedZoom) {
    case 1: pitchOffset += m_fTargetZoomOneZExtra; break;
    case 2: pitchOffset += interior ? m_fTargetZoomTwoInteriorZExtra : m_fTargetZoomTwoZExtra; break;
    case 3: pitchOffset += m_fTargetZoomThreeZExtra; break;
    }
    const auto upperLimit = settings[13], lowerLimit = settings[14];
    auto* swim = intelligence->GetTaskSwim();
    auto* jetpack = intelligence->GetTaskJetPack();
    float headingWeight{}, pitchWeight{};
    if (swim) {
        headingWeight = 1.0f;
        if (swim->m_nSwimState != SWIM_UNDERWATER_SPRINTING) {
            pitchWeight = StaticRef<float>(0x862F34);
        }
    } else if (jetpack) {
        headingWeight = 0.5f;
        if (!ped->bIsStanding) {
            pitchWeight = StaticRef<float>(0x862F3C);
        }
    }
    if (!TheCamera.m_bTransitionState) {
        if (m_bResetStatics) {
            m_fFOV = 70.0f;
        } else {
            const auto step = timeStep * StaticRef<float>(0x862F1C);
            m_fFOV = m_fFOV + step < 70.0f ? m_fFOV + step : std::max(m_fFOV - step, 70.0f);
        }
    }
    lookAt.z += settings[0];
    const auto historyDistance = std::max(distance, settings[4]);
    auto& forceBehind = StaticRef<bool>(0xB6EC54);
    const bool reset = m_bResetStatics || TheCamera.m_bCamDirectlyBehind || TheCamera.m_bCamDirectlyInFront || preserveAngles;
    if (reset) {
        if (preserveAngles) {
            StaticRef<CVector>(0x8CCC3C) = ped->GetPosition();
            StaticRef<CVector>(0xB6EC7C).Reset();
            lookAt = ped->GetPosition() + CVector{0.0f, 0.0f, settings[0]};
        }
        TheCamera.ResetDuckingSystem(ped);
        m_bRotating = false;
        forceBehind = false;
        m_bCollisionChecksOn = true;
        if (!TheCamera.m_bJustCameOutOfGarage && !preserveAngles) {
            m_fHorizontalAngle = ped->GetHeading() - HALF_PI;
            if (TheCamera.m_bCamDirectlyInFront) {
                m_fHorizontalAngle += PI;
            }
        }
        m_fBetaSpeed = m_fAlphaSpeed = 0.0f;
        m_fDistance = 1000.0f;
        if (distance == TheCamera.m_fPedZoomBase) {
            distance = TheCamera.m_fPedZoomSmoothed = TheCamera.m_fPedZoomTotal;
        }
        if (!TheCamera.m_bJustCameOutOfGarage && !preserveAngles) {
            m_fVerticalAngle = ped->bIsStanding ? -std::asin(std::clamp(DotProduct(ped->field_578, ped->GetMatrix().GetForward()), -1.0f, 1.0f)) : 0.0f;
        }
        m_vecFront = {-std::cos(m_fHorizontalAngle) * std::cos(m_fVerticalAngle), -std::sin(m_fHorizontalAngle) * std::cos(m_fVerticalAngle), std::sin(m_fVerticalAngle)};
        m_avecTargetHistoryPos[0] = lookAt - m_vecFront * historyDistance;
        m_anTargetHistoryTime[0] = CTimer::GetTimeInMS();
        m_avecTargetHistoryPos[1] = lookAt - m_vecFront * distance;
        m_nCurrentHistoryPoints = 0;
        if (!TheCamera.m_bJustCameOutOfGarage && !preserveAngles) {
            m_fVerticalAngle = -pitchOffset;
        }
        if (swim && swim->m_nSwimState != SWIM_UNDERWATER_SPRINTING) {
            m_fVerticalAngle += StaticRef<float>(0x862F38);
        } else if (jetpack) {
            m_fVerticalAngle += StaticRef<float>(0x862F40);
        }
        CPad::GetPad(0)->ClearMouseHistory();
    } else if (auto* standingOn = ped->m_standingOnEntity) {
        auto* physical = standingOn->AsPhysical();
        const auto IsTrain = [](CEntity* entity) {
            return entity && entity->IsVehicle() && entity->AsVehicle()->IsSubTrain();
        };
        if (IsTrain(standingOn) || IsTrain(physical->m_pAttachedTo)) {
            const auto speed = physical->GetMoveSpeed().Magnitude();
            const auto threshold = StaticRef<float>(0x8CCEA0);
            const auto fraction = std::max(0.0f, speed - threshold) / std::max(threshold, speed);
            const auto displacement = physical->GetMoveSpeed() * fraction * timeStep;
            m_avecTargetHistoryPos[0] += displacement;
            m_avecTargetHistoryPos[1] += displacement;
        }
    }
    m_vecFront = (lookAt - m_avecTargetHistoryPos[0]).Normalized();
    const auto previousLength = (lookAt - m_avecTargetHistoryPos[1]).Magnitude();
    if (previousLength < distance && settings[3] < distance) {
        distance = std::max(previousLength, minimumDistance);
    }
    auto heading = std::atan2(-m_vecFront.x, m_vecFront.y) - HALF_PI;
    if (heading < -PI) {
        heading += TWO_PI;
    }
    auto pedHeading = ped->GetHeading() - HALF_PI;
    if (pedHeading - heading > PI) {
        pedHeading -= TWO_PI;
    } else if (pedHeading - heading < -PI) {
        pedHeading += TWO_PI;
    }
    if (pad->GetForceCameraBehindPlayer()) {
        forceBehind = true;
    } else if (forceBehind && (ped->GetMoveSpeed().SquaredMagnitude() > StaticRef<float>(0x858CDC)
        || std::abs(pedHeading - heading) < StaticRef<float>(0x858C58)
        || pad->LookAroundLeftRight(ped) || pad->LookAroundUpDown(ped))) {
        forceBehind = false;
    }
    float rotationFraction{}, rotationStep{};
    if (std::abs(pedHeading - heading) < StaticRef<float>(0x8CCE9C) || forceBehind || headingWeight != 0.0f) {
        rotationFraction = timeStep * settings[10];
        rotationStep = timeStep * settings[11];
        if (!forceBehind && headingWeight == 0.0f) {
            const auto relativeSpeed = ped->m_standingOnEntity ? ped->GetMoveSpeed() - ped->m_standingOnEntity->AsPhysical()->GetMoveSpeed() : ped->GetMoveSpeed();
            rotationFraction = std::min(relativeSpeed.Magnitude() * rotationFraction, 1.0f);
        } else {
            rotationFraction = std::min(rotationFraction * StaticRef<float>(0x8CCE98), 1.0f);
            rotationStep *= StaticRef<float>(0x8CCE94);
            if (headingWeight != 0.0f) {
                rotationFraction *= headingWeight;
                rotationStep *= headingWeight;
            }
        }
        heading += std::clamp((pedHeading - heading) * rotationFraction, -rotationStep, rotationStep);
    }
    if (heading > m_fHorizontalAngle + PI) {
        heading -= TWO_PI;
    } else if (heading < m_fHorizontalAngle - PI) {
        heading += TWO_PI;
    }
    const auto headingVelocity = (heading - m_fHorizontalAngle) / std::max(timeStep, 1.0f);
    auto pitch = std::asin(std::clamp(m_vecFront.z, -1.0f, 1.0f));
    const auto headingDifference = std::abs(pedHeading - heading);
    if (headingDifference > StaticRef<float>(0x8CCE90) && ped->GetMoveSpeed().SquaredMagnitude() > StaticRef<float>(0x858F44)) {
        const auto fraction = std::min((headingDifference - StaticRef<float>(0x8CCE90)) * StaticRef<float>(0x858F08) / (PI - StaticRef<float>(0x8CCE90)), 1.0f);
        const auto limit = HALF_PI - (HALF_PI - StaticRef<float>(0x8CCE8C)) * fraction;
        const auto damping = std::pow(StaticRef<float>(0x8CCE88), timeStep);
        if (pitch > limit) {
            pitch = pitch * damping + limit * (1.0f - damping);
        } else if (pitch < -limit) {
            pitch = pitch * damping - limit * (1.0f - damping);
        }
    }
    if (pitchWeight != 0.0f || (forceBehind && ped->bIsStanding)) {
        float desired{};
        if (jetpack) {
            desired = StaticRef<float>(0x862F40);
        } else if (swim) {
            desired = StaticRef<float>(0x862F38);
        } else if (ped->bIsStanding) {
            desired = -std::asin(std::clamp(DotProduct(ped->field_578, ped->GetMatrix().GetForward()), -1.0f, 1.0f));
        }
        auto fraction = std::min(rotationFraction * StaticRef<float>(0x8CCE84), 1.0f);
        auto step = rotationStep * StaticRef<float>(0x8CCE80);
        if (pitchWeight != 0.0f) {
            fraction *= pitchWeight;
            step *= pitchWeight;
        }
        pitch += std::clamp((desired - pitch) * fraction, -step, step);
    }
    pitch = std::clamp(pitch - pitchOffset, -lowerLimit, upperLimit);
    const auto pitchStep = timeStep * settings[6];
    const auto pitchCorrection = std::clamp((pitch - m_fVerticalAngle) * (1.0f - std::pow(settings[5], timeStep)), -pitchStep, pitchStep);
    auto horizontal = (float)-pad->LookAroundLeftRight(ped);
    auto vertical = (float)pad->LookAroundUpDown(ped);
    if (auto* gun = intelligence->GetTaskUseGun(); gun && (gun->m_LastCmd == eGunCommand::FIRE || gun->m_LastCmd == eGunCommand::FIREBURST)
        && gun->m_WeaponInfo && !gun->m_WeaponInfo->flags.bAimWithArm) {
        if (std::abs(horizontal) < std::abs((float)pad->GetPedWalkLeftRight())) {
            horizontal = (float)-pad->GetPedWalkLeftRight();
        }
    }
    if ((vertical != 0.0f || horizontal != 0.0f) && !pad->GetPedWalkLeftRight() && !pad->GetPedWalkUpDown()) {
        auto* player = FindPlayerPed(PED_TYPE_PLAYER1);
        const auto forward = TheCamera.GetForwardVector();
        if (DotProduct(forward, player->GetMatrix().GetForward()) > StaticRef<float>(0x858C24)) {
            auto position = player->GetPosition() + forward * 5.0f;
            g_ikChainMan.LookAt("FollowPedSA", player, nullptr, 1500, static_cast<eBoneTag32>(-1), &position, false, 0.25f, 500, 3, false);
        }
    }
    const auto scale = m_fFOV / 80.0f * sq(StaticRef<float>(0x8CC4A0));
    horizontal *= scale * StaticRef<float>(0x859B50) * std::abs(horizontal);
    vertical *= scale * StaticRef<float>(0x8631AC) * std::abs(vertical);
    if (auto* climb = intelligence->GetTaskClimb()) {
        climb->GetCameraStickModifier(ped, m_fVerticalAngle, m_fHorizontalAngle, vertical, horizontal);
    } else if (auto* task = ped->GetTaskManager().GetActiveTask(); task && task->GetTaskType() == TASK_COMPLEX_ENTER_CAR_AS_DRIVER) {
        // CTaskComplexEnterCar::GetCameraStickModifier is not yet exposed by the task class.
        plugin::CallMethod<0x63A380, CTask*, CPed*, float, float*, float*, float*, float*>(task, ped, distance, &m_fVerticalAngle, &m_fHorizontalAngle, &vertical, &horizontal);
    }
    const auto damping = std::pow(settings[8], timeStep);
    m_fBetaSpeed = damping * m_fBetaSpeed + (1.0f - damping) * std::clamp(horizontal + headingVelocity, -settings[9], settings[9]);
    if (std::abs(m_fBetaSpeed) < StaticRef<float>(0x8CCE7C)) {
        m_fBetaSpeed = 0.0f;
    }
    const bool mouseControls = CCamera::m_bUseMouse3rdPerson && !pad->DisablePlayerControls;
    const auto mouse = pad->NewMouseControllerState.GetAmountMouseMoved();
    if (mouseControls) {
        horizontal = m_fFOV / 80.0f * mouse.x * -3.0f * CCamera::m_fMouseAccelHorzntl;
        m_fBetaSpeed = 0.0f;
    } else {
        horizontal = timeStep * m_fBetaSpeed;
    }
    m_fHorizontalAngle += horizontal;
    ClipBeta();
    // The original uses the opposite damping weights for vertical input here.
    m_fAlphaSpeed = std::clamp(vertical * damping + m_fAlphaSpeed * (1.0f - damping), -settings[9], settings[9]);
    if (std::abs(m_fAlphaSpeed) < StaticRef<float>(0x8CCE78)) {
        m_fAlphaSpeed = 0.0f;
    }
    const auto verticalMotion = timeStep * m_fAlphaSpeed;
    auto correction = pitchCorrection;
    if (mouseControls) {
        correction = m_fFOV / 80.0f * mouse.y * StaticRef<float>(0x858FA0) * CCamera::m_fMouseAccelHorzntl;
        if ((TheCamera.m_bFading && TheCamera.GetFadingDirection() == 1 && CDraw::FadeValue > 45) || CDraw::FadeValue > 200) {
            correction = std::clamp(-pitchOffset - m_fVerticalAngle, -0.05f, 0.05f);
        }
        m_fAlphaSpeed = 0.0f;
    }
    m_fVerticalAngle += correction;
    if (m_fVerticalAngle > upperLimit || m_fVerticalAngle < -lowerLimit) {
        m_fVerticalAngle = std::clamp(m_fVerticalAngle, -lowerLimit, upperLimit);
        m_fAlphaSpeed = 0.0f;
    }
    auto& previousPitch = StaticRef<float>(0x8CCE74);
    auto& previousHeading = StaticRef<float>(0x8CCE6C);
    if (std::abs(previousPitch - m_fVerticalAngle) < StaticRef<float>(0x8CCE70)) {
        m_fVerticalAngle = previousPitch;
    }
    previousPitch = m_fVerticalAngle;
    if (std::abs(previousHeading - m_fHorizontalAngle) < StaticRef<float>(0x8CCE68)) {
        m_fHorizontalAngle = previousHeading;
    }
    previousHeading = m_fHorizontalAngle;
    m_vecFront = {-std::cos(m_fHorizontalAngle) * std::cos(m_fVerticalAngle), -std::sin(m_fHorizontalAngle) * std::cos(m_fVerticalAngle), std::sin(m_fVerticalAngle)};
    GetVectorsReadyForRW();
    TheCamera.m_bCamDirectlyBehind = TheCamera.m_bCamDirectlyInFront = false;
    m_vecSource = lookAt - m_vecFront * distance;
    RoundCamCoordinates(m_vecSource, 4);
    const auto historyPitch = verticalMotion + pitch + pitchOffset;
    const CVector historyFront{-std::cos(m_fHorizontalAngle) * std::cos(historyPitch), -std::sin(m_fHorizontalAngle) * std::cos(historyPitch), std::sin(historyPitch)};
    m_avecTargetHistoryPos[0] = lookAt - historyFront * historyDistance;
    m_avecTargetHistoryPos[1] = lookAt - historyFront * distance;
    if (pad->GetForceCameraBehindPlayer() && pad->LookAroundLeftRight(nullptr)) {
        auto delta = m_fHorizontalAngle - (ped->m_fCurrentRotation - HALF_PI);
        if (delta > PI) {
            delta -= TWO_PI;
        } else if (delta < -PI) {
            delta += TWO_PI;
        }
        if (std::abs(delta) < timeStep * StaticRef<float>(0x858B1C)) {
            ped->m_fAimingRotation = m_fHorizontalAngle + HALF_PI;
        }
    }
    TheCamera.HandleCameraMotionForDucking(ped, &m_vecSource, &lookAt, false);
    m_vecTargetCoorsForFudgeInter = lookAt;
    RoundCamCoordinates(m_vecSource, 4);
    CCamera::SetColVarsPed(interior ? PED_TYPE_PLAYER2 : PED_TYPE_PLAYER1, TheCamera.m_nPedZoom);
    if (gCameraDirection == 3) {
        TheCamera.CameraGenericModeSpecialCases(ped);
        TheCamera.CameraPedModeSpecialCases();
        TheCamera.CameraColDetAndReact(&m_vecSource, &lookAt);
        TheCamera.ImproveNearClip(nullptr, ped, &m_vecSource, &lookAt);
        RoundCamCoordinates(m_vecSource, 4);
    }
    TheCamera.m_bCamDirectlyBehind = TheCamera.m_bCamDirectlyInFront = false;
    RoundCamCoordinates(m_vecSource, 4);
    GetVectorsReadyForRW();
    if (!interior && TheCamera.m_nWhoIsInControlOfTheCamera != 1 && ped->bIsStanding && !CGameLogic::IsCoopGameGoingOn()
        && !TheCamera.m_bFOVLerpProcessed && !TheCamera.m_bVecMoveLinearProcessed && !TheCamera.m_bVecTrackLinearProcessed
        && ped->GetMoveSpeed().SquaredMagnitude() < StaticRef<float>(0x858FC4)) {
        gIdleCam.Process();
    } else {
        gIdleCam.m_IdleTickerFrames = 0;
    }
    m_bResetStatics = false;
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
