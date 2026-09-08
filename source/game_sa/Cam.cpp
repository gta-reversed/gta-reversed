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
    RH_ScopedInstall(Process_1rstPersonPedOnPC, 0x50EB70, { .reversed = false });
    RH_ScopedInstall(Process_1stPerson, 0x517EA0);
    RH_ScopedInstall(Process_AimWeapon, 0x521500, { .reversed = false });
    RH_ScopedInstall(Process_AttachedCam, 0x512B10);
    RH_ScopedInstall(Process_Cam_TwoPlayer, 0x525E50, { .reversed = false });
    RH_ScopedInstall(Process_Cam_TwoPlayer_InCarAndShooting, 0x519810, { .reversed = false });
    RH_ScopedInstall(Process_Cam_TwoPlayer_Separate_Cars, 0x513510);
    RH_ScopedInstall(Process_Cam_TwoPlayer_Separate_Cars_TopDown, 0x513BE0);
    RH_ScopedInstall(Process_DW_BirdyCam, 0x51B850, { .reversed = false });
    RH_ScopedInstall(Process_DW_CamManCam, 0x51B120, { .reversed = false });
    RH_ScopedInstall(Process_DW_HeliChaseCam, 0x51A740, { .reversed = false });
    RH_ScopedInstall(Process_DW_PlaneCam1, 0x51C760);
    RH_ScopedInstall(Process_DW_PlaneCam2, 0x51CC30);
    RH_ScopedInstall(Process_DW_PlaneCam3, 0x51D100);
    RH_ScopedInstall(Process_DW_PlaneSpotterCam, 0x51C250);
    RH_ScopedInstall(Process_Editor, 0x50F3F0);
    RH_ScopedInstall(Process_Fixed, 0x51D470);
    RH_ScopedInstall(Process_FlyBy, 0x5B25F0, { .reversed = false });
    RH_ScopedInstall(Process_FollowCar_SA, 0x5245B0, { .reversed = false });
    RH_ScopedInstall(Process_FollowPedWithMouse, 0x50F970, { .reversed = false });
    RH_ScopedInstall(Process_FollowPed_SA, 0x522D40, { .reversed = false });
    RH_ScopedInstall(Process_M16_1stPerson, 0x5105C0, { .reversed = false });
    RH_ScopedInstall(Process_Rocket, 0x511B50);
    RH_ScopedInstall(Process_SpecialFixedForSyphon, 0x517500);
    RH_ScopedInstall(Process_WheelCam, 0x512110);

    RH_ScopedGlobalInstall(WellBufferMe, 0x509AE0);
    RH_ScopedGlobalInstall(WrapAngle, 0x509BE0);
    RH_ScopedGlobalInstall(FindSplinePathPositionFloat, 0x5B2330);
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
        const auto targetFwd = targetPed->GetForward().Normalized();
        const auto mag       = (pointIn - v3d_B6FFC4).Magnitude2D();

        m_vecSource = targetFwd * mag * 1.23f + targetPed->GetPosition() + CVector{ 0.0f, 0.0f, 0.59f };
    }

    CVector spinePos{};
    targetPed->GetTransformedBonePosition(spinePos, BONE_SPINE1, true);

    // TODO: Put in a function name e.g. 'HandleFreeMouseControl'?
    auto*      pad1   = CPad::GetPad(0);
    const auto fov    = m_fFOV / 80.0f;
    const auto amountMouseMoved = pad1->NewMouseControllerState.GetAmountMouseMoved();

    if (!amountMouseMoved.IsZero()) {
        m_fHorizontalAngle += -3.0f * amountMouseMoved.x * fov * CCamera::m_fMouseAccelHorzntl;
        m_fVerticalAngle += +4.0f * amountMouseMoved.y * fov * CCamera::m_fMouseAccelVertical;
    } else {
        const auto hv = (float)-pad1->LookAroundLeftRight(targetPed);
        const auto vv = (float)pad1->LookAroundUpDown(targetPed);

        m_fHorizontalAngle += sq(hv) / 10000.0f * fov / 17.5f * CTimer::GetTimeStep() * (hv < 0.0f ? -1.0f : 1.0f);
        m_fVerticalAngle += sq(vv) / 22500.0f * fov / 14.0f * CTimer::GetTimeStep() * (vv < 0.0f ? -1.0f : 1.0f);
    }
    ClipBeta();
    ClipAlpha();

    if (const auto* a = targetPed->m_pAttachedTo; targetPed->IsPlayer() && a) {
        // enum?
        switch (targetPed->m_fTurretAngleA) {
        case 0u:
            m_fHorizontalAngle -= a->GetHeading() + DegreesToRadians(90.0f);
            break;
        case 1u:
            m_fHorizontalAngle -= a->GetHeading() + DegreesToRadians(180.0f);
            break;
        case 2u:
            m_fHorizontalAngle -= a->GetHeading() + DegreesToRadians(-90.0f);
            break;
        case 3u:
            m_fHorizontalAngle -= a->GetHeading();
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
