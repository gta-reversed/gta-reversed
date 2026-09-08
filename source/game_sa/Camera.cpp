#include "StdInc.h"

#include "Camera.h"

#include "TaskSimpleGangDriveBy.h"
#include "TaskSimpleHoldEntity.h"
#include "TaskSimpleDuck.h"
#include "TaskSimpleSwim.h"
#include "PedModelInfo.h"
#include "Scene.h"
#include "Collision.h"
#include "WaterLevel.h"
#include "Hud.h"
#include "HandShaker.h"
#include "PostEffects.h"
#include "TaskComplexArrestPed.h"
#include "DummyObject.h"

auto& TheCamera = StaticRef<CCamera>(0xB6F028);
auto& gbModelViewer = StaticRef<bool>(0xBA6728);
auto& gbCineyCamMessageDisplayed = StaticRef<int8>(0x8CC381); // 2
auto& gCameraDirection = StaticRef<int32>(0x8CC384);         // 3
auto& gCameraMode = StaticRef<eCamMode>(0x8CC388);        // -1
auto& gLastTime2PlayerCameraWasOK = StaticRef<uint32>(0xB6EC24);    // 0
auto& gLastTime2PlayerCameraCollided = StaticRef<uint32>(0xB6EC28); // 0
auto& gPlayerPedVisible = StaticRef<bool>(0x8CC380); // true
auto& gCurCamColVars = StaticRef<uint8>(0x8CCB80);
auto& gCurDistForCam = StaticRef<float>(0x8CCB84);
auto& gpCamColVars = StaticRef<float*>(0xB6FE88);
auto& gCamColVars = StaticRef<float[28][6]>(0x8CC8E0);
static auto& gLastRadiusUsedInCollisionPreventionOfCamera = StaticRef<float>(0xB6EC6C);

CCam& CCamera::GetActiveCamera() {
    return TheCamera.m_aCams[TheCamera.m_nActiveCam];
}

void CCamera::InjectHooks() {
    RH_ScopedClass(CCamera);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(GetArrPosForVehicleType, 0x50AF00);
    RH_ScopedInstall(GetPositionAlongSpline, 0x50AF80);
    RH_ScopedInstall(GetRoughDistanceToGround, 0x516B00);
    RH_ScopedInstall(InitialiseCameraForDebugMode, 0x50AF90);
    RH_ScopedInstall(ProcessObbeCinemaCameraPed, 0x50B880);
    RH_ScopedInstall(ProcessWideScreenOn, 0x50B890);
    RH_ScopedInstall(RenderMotionBlur, 0x50B8F0);
    RH_ScopedInstall(SetCameraDirectlyBehindForFollowPed_CamOnAString, 0x50BD40);
    RH_ScopedInstall(SetCameraDirectlyInFrontForFollowPed_CamOnAString, 0x50BD70);
    RH_ScopedInstall(SetCamPositionForFixedMode, 0x50BEC0);
    RH_ScopedInstall(SetFadeColour, 0x50BF00);
    RH_ScopedInstall(SetMotionBlur, 0x50BF40);
    RH_ScopedInstall(SetMotionBlurAlpha, 0x50BF80);
    RH_ScopedInstall(SetNearClipScript, 0x50BF90);
    RH_ScopedInstall(SetNewPlayerWeaponMode, 0x50BFB0);
    RH_ScopedInstall(SetRwCamera, 0x50C100);
    RH_ScopedInstall(SetWideScreenOn, 0x50C140);
    RH_ScopedInstall(SetWideScreenOff, 0x50C150);
    RH_ScopedInstall(StartCooperativeCamMode, 0x50C260);
    RH_ScopedInstall(StopCooperativeCamMode, 0x50C270);
    RH_ScopedInstall(AllowShootingWith2PlayersInCar, 0x50C280);
    RH_ScopedInstall(StoreValuesDuringInterPol, 0x50C290);
    RH_ScopedInstall(ProcessScriptedCommands, 0x516AE0);
    RH_ScopedInstall(FinishCutscene, 0x514950);
    RH_ScopedInstall(LerpFOV, 0x50D280);
    RH_ScopedInstall(UpdateAimingCoors, 0x50CB10);
    RH_ScopedInstall(UpdateSoundDistances, 0x515BD0);
    RH_ScopedInstall(SetColVarsAimWeapon, 0x50CBF0);
    RH_ScopedInstall(ClearPlayerWeaponMode, 0x50AB10);
    RH_ScopedInstall(DontProcessObbeCinemaCamera, 0x50AB40);
    RH_ScopedInstall(Enable1rstPersonCamCntrlsScript, 0x50AC00);
    RH_ScopedInstall(FindCamFOV, 0x50AD20);
    RH_ScopedInstall(GetFading, 0x50ADE0);
    RH_ScopedInstall(GetFadingDirection, 0x50ADF0);
    RH_ScopedInstall(Get_Just_Switched_Status, 0x50AE10);
    RH_ScopedInstall(GetGameCamPosition, 0x50AE50);

    RH_ScopedInstall(Constructor, 0x51A450);
    RH_ScopedInstall(InitCameraVehicleTweaks, 0x50A3B0);
    RH_ScopedInstall(ApplyVehicleCameraTweaks, 0x50A480);
    RH_ScopedInstall(CamShake, 0x50A9F0);
    RH_ScopedInstall(GetScreenRect, 0x50AB50);
    RH_ScopedInstall(Enable1rstPersonWeaponsCamera, 0x50AC10);
    RH_ScopedInstall(Fade, 0x50AC20);
    RH_ScopedInstall(Find3rdPersonQuickAimPitch, 0x50AD40);
    RH_ScopedInstall(GetCutSceneFinishTime, 0x50AD90);
    RH_ScopedInstall(GetScreenFadeStatus, 0x50AE20);
    RH_ScopedInstall(GetLookingLRBFirstPerson, 0x50AE60);
    RH_ScopedInstall(GetLookDirection, 0x50AE90);
    RH_ScopedInstall(GetLookingForwardFirstPerson, 0x50AED0);
    RH_ScopedInstall(CopyCameraMatrixToRWCam, 0x50AFA0);
    RH_ScopedInstall(CalculateMirroredMatrix, 0x50B380);
    RH_ScopedInstall(DealWithMirrorBeforeConstructRenderList, 0x50B510);
    RH_ScopedInstall(ProcessFade, 0x50B5D0);
    RH_ScopedInstall(ProcessMusicFade, 0x50B6D0);
    RH_ScopedInstall(Restore, 0x50B930);
    RH_ScopedInstall(RestoreWithJumpCut, 0x50BAB0);
    RH_ScopedInstall(SetCamCutSceneOffSet, 0x50BD20);
    RH_ScopedInstall(SetCameraDirectlyBehindForFollowPed_ForAPed_CamOnAString, 0x50BDA0);
    RH_ScopedInstall(SetCameraDirectlyInFrontForFollowPed_ForAPed_CamOnAString, 0x50BE30);
    RH_ScopedInstall(Using1stPersonWeaponMode, 0x50BFF0);
    RH_ScopedInstall(SetParametersForScriptInterpolation, 0x50C030);
    RH_ScopedInstall(SetPercentAlongCutScene, 0x50C070);
    RH_ScopedInstall(SetZoomValueFollowPedScript, 0x50C160);
    RH_ScopedInstall(SetZoomValueCamStringScript, 0x50C1B0);
    RH_ScopedInstall(UpdateTargetEntity, 0x50C360);
    RH_ScopedInstall(TakeControl, 0x50C7C0);
    RH_ScopedInstall(TakeControlNoEntity, 0x50C8B0);
    RH_ScopedInstall(TakeControlAttachToEntity, 0x50C910);
    RH_ScopedInstall(TakeControlWithSpline, 0x50CAE0);
    RH_ScopedInstall(SetCamCollisionVarDataSet, 0x50CB60);
    RH_ScopedInstall(SetNearClipBasedOnPedCollision, 0x50CB90);
    RH_ScopedInstall(SetColVarsPed, 0x50CC50);
    RH_ScopedInstall(SetColVarsVehicle, 0x50CCA0);
    RH_ScopedInstall(StartTransitionWhenNotFinishedInter, 0x515BC0);
    RH_ScopedInstall(StartTransition, 0x515200);
    RH_ScopedInstall(CameraGenericModeSpecialCases, 0x50CD30);
    RH_ScopedInstall(CameraPedModeSpecialCases, 0x50CD80);
    RH_ScopedInstall(CameraPedAimModeSpecialCases, 0x50CDA0);
    RH_ScopedInstall(CameraVehicleModeSpecialCases, 0x50CDE0);
    RH_ScopedInstall(IsExtraEntityToIgnore, 0x50CE80);
    RH_ScopedInstall(ConsiderPedAsDucking, 0x50CEB0);
    RH_ScopedInstall(ResetDuckingSystem, 0x50CEF0);
    RH_ScopedInstall(HandleCameraMotionForDucking, 0x50CFA0);
    RH_ScopedInstall(HandleCameraMotionForDuckingDuringAim, 0x50D090);
    RH_ScopedInstall(VectorMoveLinear, 0x50D160);
    RH_ScopedInstall(VectorTrackLinear, 0x50D1D0);
    RH_ScopedInstall(AddShakeSimple, 0x50D240);
    RH_ScopedInstall(InitialiseScriptableComponents, 0x50D2D0);
    RH_ScopedInstall(DrawBordersForWideScreen, 0x514860);
    RH_ScopedInstall(Find3rdPersonCamTargetVector, 0x514970);
    RH_ScopedInstall(CalculateGroundHeight, 0x514B80);
    RH_ScopedInstall(AvoidTheGeometry, 0x514030);
    RH_ScopedInstall(CalculateFrustumPlanes, 0x514D60);
    RH_ScopedInstall(CalculateDerivedValues, 0x5150E0);
    RH_ScopedOverloadedInstall(IsSphereVisible, "Matrix", 0x420C40, bool(CCamera::*)(const CVector&, float, RwMatrix*));
    RH_ScopedInstall(ImproveNearClip, 0x516B20);
    RH_ScopedInstall(SetCameraUpForMirror, 0x51A560);
    RH_ScopedInstall(RestoreCameraAfterMirror, 0x51A5A0);
    RH_ScopedInstall(ConeCastCollisionResolve, 0x51A5D0);
    RH_ScopedInstall(TryToStartNewCamMode, 0x51E560, { .Reversed = false });
    RH_ScopedInstall(CameraColDetAndReact, 0x520190);
    RH_ScopedInstall(CamControl, 0x527FA0, { .Reversed = false });
    RH_ScopedInstall(Process, 0x52B730, { .Reversed = false });
    RH_ScopedInstall(DeleteCutSceneCamDataMemory, 0x5B24A0);
    RH_ScopedInstall(LoadPathSplines, 0x5B24D0);
    RH_ScopedInstall(Init, 0x5BC520);

    RH_ScopedOverloadedInstall(ProcessVectorTrackLinear, "0", 0x50D350, void(CCamera::*)(float));
    RH_ScopedOverloadedInstall(ProcessVectorTrackLinear, "1", 0x516440, void(CCamera::*)());
    RH_ScopedOverloadedInstall(ProcessVectorMoveLinear, "0", 0x50D430, void(CCamera::*)(float));
    RH_ScopedOverloadedInstall(ProcessVectorMoveLinear, "1", 0x5164A0, void(CCamera::*)());
    RH_ScopedOverloadedInstall(ProcessFOVLerp, "0", 0x50D510, void(CCamera::*)(float));
    RH_ScopedOverloadedInstall(ProcessFOVLerp, "1", 0x516500, void(CCamera::*)());
    RH_ScopedOverloadedInstall(ProcessShake, "Intensity", 0x516560, void(CCamera::*)(float));
    RH_ScopedOverloadedInstall(ProcessShake, "Timed", 0x51A6F0, void(CCamera::*)());

    RH_ScopedGlobalInstall(CamShakeNoPos, 0x50A970);
}

CCamera* CCamera::Constructor() { this->CCamera::CCamera(); return this; }

// 0x51A450
CCamera::CCamera() : CPlaceable() {
    m_nShakeType = 1;
    m_bMusicFadedOut = false;
    m_matrix = reinterpret_cast<CMatrixLink*>(&m_mCameraMatrix);
    m_fDuckCamMotionFactor = 0.0f;
    m_fDuckAimCamMotionFactor = 0.0f;

    InitialiseScriptableComponents();
}

// 0x50A870
CCamera::~CCamera() {
    m_matrix = nullptr;
}

// 0x5BC520
void CCamera::Init() {
    InitialiseScriptableComponents();
    
    for (auto& camera : m_aCams) {
        camera.Init();
    }

    {
        auto& cam = m_aCams[0];
        cam.m_nMode = MODE_FOLLOWPED;
        cam.m_fTargetCloseInDist = 2.0837801f - 1.85f;
        cam.m_fMinRealGroundDist = 1.85f;
        cam.m_fTargetZoomGroundOne = -0.55f;
        cam.m_fTargetZoomGroundTwo = 1.5f;
        cam.m_fTargetZoomGroundThree = 3.6f;
        cam.m_fTargetZoomOneZExtra = 0.06f;
        cam.m_fTargetZoomTwoZExtra = -0.1f;
        cam.m_fTargetZoomTwoInteriorZExtra = 0.0f;
        cam.m_fTargetZoomThreeZExtra = -0.07f;
        cam.m_fTargetZoomZCloseIn = 0.90040702f;
        cam.m_pCamTargetEntity = nullptr;
        cam.m_fCamBufferedHeight = 0.0f;
        cam.m_fCamBufferedHeightSpeed = 0.0f;
        cam.m_bCamLookingAtVector = false;
        cam.m_fPlayerVelocity = 0.0f;
    }

    {
        auto& cam = m_aCams[1];
        cam.m_nMode = MODE_FOLLOWPED;
        cam.m_pCamTargetEntity = nullptr;
        cam.m_fCamBufferedHeight = 0.0f;
        cam.m_fCamBufferedHeightSpeed = 0.0f;
        cam.m_bCamLookingAtVector = false;
        cam.m_fPlayerVelocity = 0.0f;
    }

    {
        auto& cam = m_aCams[2];
        cam.m_pCamTargetEntity = nullptr;
        cam.m_bCamLookingAtVector = false;
        cam.m_fPlayerVelocity = 0.0f;
    }

    ClearPlayerWeaponMode();

    m_pTargetEntity = FindPlayerEntity();
    CEntity::SafeRegisterRef(m_pTargetEntity);

    if (!FrontEndMenuManager.m_bStartGameLoading) {
        CDraw::FadeValue = 0;
        m_fMouseAccelVertical = notsa::IsFixBugs() ? m_fMouseAccelHorzntl * 0.6f : 0.0015f;
    }
    
    SetMotionBlur(255, 255, 255, 0, eMotionBlurType::NONE);

    m_f3rdPersonCHairMultX = 0.53f;
    m_f3rdPersonCHairMultY = 0.4f;
    gPlayerPedVisible = 1;
    m_bResetOldMatrix = true;
}

// 0x50A3B0
void CCamera::InitCameraVehicleTweaks() {
    m_fCurrentTweakDistance   = 1.0f;
    m_fCurrentTweakAltitude   = 1.0f;
    m_fCurrentTweakAngle      = 0.0f;
    m_nCurrentTweakModelIndex = -1;

    if (!m_bCameraVehicleTweaksInitialized) {
        for (auto& camTweak : m_aCamTweak) {
            camTweak.ModelID = -1;
            camTweak.Dist   = 1.0f;
            camTweak.Alt   = 1.0f;
            camTweak.Angle      = 0.0f;
        }

        m_aCamTweak[0].ModelID = MODEL_RCGOBLIN;
        m_aCamTweak[0].Dist = 1.0f;
        m_aCamTweak[0].Alt = 1.0f;
        m_aCamTweak[0].Angle    = 0.178997f; // todo: magic number

        m_bCameraVehicleTweaksInitialized = true;
    }
}

// 0x50D2D0
void CCamera::InitialiseScriptableComponents() {
    m_fTrackLinearStartTime    = -1.0f;
    m_fTrackLinearEndTime      = -1.0f;
    m_fStartShakeTime          = -1.0f;
    m_fEndShakeTime            = -1.0f;
    m_fEndZoomTime             = -1.0f;
    m_fStartZoomTime           = -1.0f;
    m_fZoomInFactor            = +0.0f;
    m_fZoomOutFactor           = +0.0f;
    m_bTrackLinearWithEase     = true;
    m_nZoomMode                = 1;
    m_bMoveLinearWithEase      = true;
    m_fMoveLinearStartTime     = -1.0f;
    m_fMoveLinearEndTime       = -1.0f;
    m_bBlockZoom               = false;
    m_bCameraPersistPosition   = false;
    m_bCameraPersistTrack      = false;
    m_bVecTrackLinearProcessed = false;
    m_bVecMoveLinearProcessed  = false;
    m_bFOVLerpProcessed        = false;
}

// 0x50AF90
void CCamera::InitialiseCameraForDebugMode() {
#ifndef FINAL
    if (auto* vehicle = FindPlayerVehicle()) {
        m_aCams[2].m_vecSource = vehicle->GetPosition();
    } else if (auto* player = FindPlayerPed()) {
        m_aCams[2].m_vecSource = player->GetPosition();
    }

    m_aCams[2].m_fTrueAlpha = 0.0f;
    m_aCams[2].m_fTrueBeta  = 0.0f;
    m_aCams[2].m_nMode = eCamMode::MODE_DEBUG;
#endif
}

// 0x50A480
void CCamera::ApplyVehicleCameraTweaks(CVehicle* vehicle) {
    if (vehicle->GetModelIndex() == m_nCurrentTweakModelIndex) {
        return;
    }

    InitCameraVehicleTweaks();
    for (auto& camTweak : m_aCamTweak) {
        if (camTweak.ModelID == vehicle->GetModelIndex()) {
            m_fCurrentTweakDistance = camTweak.Dist;
            m_fCurrentTweakAltitude = camTweak.Alt;
            m_fCurrentTweakAngle    = camTweak.Angle;
            return;
        }
    }
}

// 0x50A9F0
void CCamera::CamShake(float strength, CVector from) {
    auto dist = DistanceBetweenPoints(from, GetActiveCamera().m_vecSource);
    dist = std::clamp(dist, 0.0f, 100.0f);

    float percentShakeForce = 1.0f - dist / 100.f;
    float shakeForce = (m_fCamShakeForce - float(CTimer::GetTimeInMS() - m_nCamShakeStart) / 1000.f) * percentShakeForce;

    float toShakeForce = percentShakeForce * strength * 0.35f;
    if (toShakeForce > std::clamp(shakeForce, 0.0f, 2.0f)) {
        m_fCamShakeForce = toShakeForce;
        m_nCamShakeStart = CTimer::GetTimeInMS();
    }
}

// 0x50A970
void CamShakeNoPos(CCamera* camera, float strength) {
    float oldShake = camera->m_fCamShakeForce - float(CTimer::GetTimeInMS() - camera->m_nCamShakeStart) / 1000.f;

    if (strength > std::clamp(oldShake, 0.0f, 2.0f)) {
        camera->m_fCamShakeForce = strength;
        camera->m_nCamShakeStart = CTimer::GetTimeInMS();
    }
}

// 0x50AB10
void CCamera::ClearPlayerWeaponMode() {
    m_PlayerWeaponMode.m_nMode = 0;
    m_PlayerWeaponMode.m_nMaxZoom = 1;
    m_PlayerWeaponMode.m_nMinZoom = -1;
    m_PlayerWeaponMode.m_fDuration = 0.0f;
}

// 0x50AB40
void CCamera::DontProcessObbeCinemaCamera() {
    bDidWeProcessAnyCinemaCam = false;
}

// 0x50AC00
void CCamera::Enable1rstPersonCamCntrlsScript() {
    m_bEnable1rstPersonCamCntrlsScript = true;
}

// 0x50AC10
void CCamera::Enable1rstPersonWeaponsCamera() {
    m_bAllow1rstPersonWeaponsCamera = true;
}

// 0x50AC20
void CCamera::Fade(float duration, eFadeFlag direction) {
    m_fFadeDuration = duration;
    m_bFading = true;
    m_nFadeInOutFlag = direction;
    m_nFadeStartTime = CTimer::GetTimeInMS();

    if (m_bIgnoreFadingStuffForMusic && direction != eFadeFlag::FADE_OUT) {
        return;
    }
    m_bMusicFading           = true;
    m_nMusicFadingDirection  = direction;

    m_fTimeToFadeMusic       = std::min(std::max(duration * 0.3f, 0.3f), duration); //Can't use std::clamp there, duration can be bigger or smaller than 0.3f
    m_nFadeTimeStartedMusic  = CTimer::GetTimeInMS();
    m_fTimeToWaitToFadeMusic = direction == eFadeFlag::FADE_IN
        ? duration - m_fTimeToFadeMusic
        : 0.f;
    if (direction == eFadeFlag::FADE_IN) {
        m_fTimeToFadeMusic = std::max(m_fTimeToFadeMusic - 0.1f, 0.f);
    }
}

// 0x50AD20
float CCamera::FindCamFOV() const {
    return m_aCams[m_nActiveCam].m_fFOV;
}

/*!
* @addr 0x50AD40
* @return Rotation in radians at which the gun should point at, relative to the camera's vertical angle
*/
float CCamera::Find3rdPersonQuickAimPitch() const {
    const auto& cam = m_aCams[m_nActiveCam];

    // https://mathworld.wolfram.com/images/eps-svg/SOHCAHTOA_500.svg
    const auto adjacent = (0.5f - m_f3rdPersonCHairMultY) * 2.f;
    const auto opposite = std::tan(DegreesToRadians(cam.m_fFOV / 2.0f)) * adjacent;
    const auto relAngle = cam.m_fVerticalAngle + std::atan(opposite / CDraw::ms_fAspectRatio);
    return -relAngle; // Flip it
}

// 0x50AD90
uint32 CCamera::GetCutSceneFinishTime() {
    auto& cam = m_aCams[m_nActiveCam];
    if (cam.m_nMode == eCamMode::MODE_FLYBY) {
        return cam.m_nFinishTime;
    }

    cam = m_aCams[(m_nActiveCam + 1) % 2];
    if (cam.m_nMode == eCamMode::MODE_FLYBY) {
        return cam.m_nFinishTime;
    }

    return 0;
}

// 0x50ADE0
bool CCamera::GetFading() const {
    return m_bFading;
}

// TODO: eFadingDirection
// 0x50ADF0
int32 CCamera::GetFadingDirection() const {
    if (m_bFading)
        return m_nFadeInOutFlag == eFadeFlag::FADE_OUT;
    else
        return 2;
}

// 0x50AE10
bool CCamera::Get_Just_Switched_Status() const {
    return m_bJust_Switched;
}

// 0x50AE20
eNameState CCamera::GetScreenFadeStatus() const {
    if (m_fFadeAlpha == 0.0f) {
        return NAME_DONT_SHOW;
    }
    if (m_fFadeAlpha == 255.0f) {
        return NAME_FADE_IN;
    }

    return NAME_SHOW;
}

// 0x50AE50
CVector* CCamera::GetGameCamPosition() {
    return &m_vecGameCamPos;
}

// 0x50AE60
bool CCamera::GetLookingLRBFirstPerson() const {
    return m_aCams[m_nActiveCam].m_nMode == eCamMode::MODE_1STPERSON
        && m_aCams[m_nActiveCam].m_nDirectionWasLooking != LOOKING_DIRECTION_FORWARD;
}

// 0x50AED0
bool CCamera::GetLookingForwardFirstPerson() const {
    return m_aCams[m_nActiveCam].m_nMode == eCamMode::MODE_1STPERSON
        && m_aCams[m_nActiveCam].m_nDirectionWasLooking == LOOKING_DIRECTION_FORWARD;
}

// 0x50AE90
int32 CCamera::GetLookDirection() const {
    const auto& cam = m_aCams[m_nActiveCam];
    if (cam.m_nMode != eCamMode::MODE_CAM_ON_A_STRING &&
        cam.m_nMode != eCamMode::MODE_1STPERSON &&
        cam.m_nMode != eCamMode::MODE_BEHINDBOAT &&
        cam.m_nMode != eCamMode::MODE_FOLLOWPED ||
        (cam.m_nDirectionWasLooking == LOOKING_DIRECTION_FORWARD)
    ) {
        return LOOKING_DIRECTION_FORWARD;
    }

    return cam.m_nDirectionWasLooking; // todo: unsigned/signed
}

// 0x50AF00
bool CCamera::GetArrPosForVehicleType(eVehicleType type, int32& arrPos) {
    switch (type) {
    case VEHICLE_TYPE_MTRUCK:
        arrPos = 0;
        return true;
    case VEHICLE_TYPE_QUAD:
        arrPos = 1;
        return true;
    case VEHICLE_TYPE_HELI:
        arrPos = 2;
        return true;
    case VEHICLE_TYPE_PLANE:
        arrPos = 4;
        return true;
    case VEHICLE_TYPE_BOAT:
        arrPos = 3;
        return true;
    default:
        return false;
    }
}

// 0x50AF80
float CCamera::GetPositionAlongSpline() const {
    return m_fPositionAlongSpline;
}

// 0x516B00
float CCamera::GetRoughDistanceToGround() {
    return m_aCams[m_nActiveCam].m_vecSource.z - CalculateGroundHeight(eGroundHeightType::ENTITY_BB_BOTTOM);
}

// 0x50AFA0
void CCamera::CopyCameraMatrixToRWCam(bool bUpdateMatrix) {
    static auto& gPrevCamRight = StaticRef<CVector>(0xB6FF90);
    static auto& gPrevCamUp    = StaticRef<CVector>(0xB6FF9C);
    static auto& gPrevCamAt    = StaticRef<CVector>(0xB6FFA8);
    static auto& gPrevCamPos   = StaticRef<CVector>(0xB6FFB4);
    static auto& gPrevCamInit  = StaticRef<uint32>(0xB6FFC0);

    RwFrame*  frame  = RwCameraGetFrame(m_pRwCamera);
    RwMatrix* matrix = RwFrameGetMatrix(frame);

    if (!bUpdateMatrix) {
        m_mCameraMatrixOld.UpdateMatrix(matrix);
    }

    matrix->pos    = m_mCameraMatrix.GetPosition();
    matrix->at     = m_mCameraMatrix.GetForward();
    matrix->up     = m_mCameraMatrix.GetUp();
    matrix->right  = m_mCameraMatrix.GetRight();

    if (!(gPrevCamInit & 1)) {
        gPrevCamInit |= 1;
        gPrevCamPos = CVector(-99999.0f, -99999.0f, -99999.0f);
    }
    if (!(gPrevCamInit & 2)) {
        gPrevCamInit |= 2;
        gPrevCamAt = CVector(-99999.0f, -99999.0f, -99999.0f);
    }
    if (!(gPrevCamInit & 4)) {
        gPrevCamInit |= 4;
        gPrevCamUp = CVector(-99999.0f, -99999.0f, -99999.0f);
    }
    if (!(gPrevCamInit & 8)) {
        gPrevCamInit |= 8;
        gPrevCamRight = CVector(-99999.0f, -99999.0f, -99999.0f);
    }

    static auto& positionSnapDistance = StaticRef<float>(0x8CCC7C);
    static auto& orientationSnapDistance = StaticRef<float>(0x8CCC78);
    if ((gPrevCamPos - matrix->pos).SquaredMagnitude() < positionSnapDistance * positionSnapDistance) {
        matrix->pos = gPrevCamPos;
    }
    if ((gPrevCamAt - matrix->at).SquaredMagnitude() < orientationSnapDistance * orientationSnapDistance) {
        matrix->at = gPrevCamAt;
    }
    if ((gPrevCamUp - matrix->up).SquaredMagnitude() < orientationSnapDistance * orientationSnapDistance) {
        matrix->up = gPrevCamUp;
    }
    if ((gPrevCamRight - matrix->right).SquaredMagnitude() < orientationSnapDistance * orientationSnapDistance) {
        matrix->right = gPrevCamRight;
    }

    gPrevCamPos   = matrix->pos;
    gPrevCamAt    = matrix->at;
    gPrevCamUp    = matrix->up;
    gPrevCamRight = matrix->right;

    RwMatrixUpdate(matrix);
    RwFrameUpdateObjects(frame);
    RwFrameOrthoNormalize(frame);

    if (m_bResetOldMatrix && !bUpdateMatrix) {
        m_mCameraMatrixOld.UpdateMatrix(matrix);
        m_bResetOldMatrix = false;
    }
}

// 0x50B380
void CCamera::CalculateMirroredMatrix(CVector posn, float mirrorV, CMatrix *camMatrix, CMatrix* mirrorMatrix) {
    mirrorMatrix->GetPosition() = camMatrix->GetPosition() - posn * 2 * (DotProduct(posn, camMatrix->GetPosition()) - mirrorV);

    const CVector fwd = camMatrix->GetForward() - posn * 2 * DotProduct(posn, camMatrix->GetForward());
    mirrorMatrix->GetForward() = fwd;

    const CVector up = camMatrix->GetUp() - posn * 2 * DotProduct(posn, camMatrix->GetUp());
    mirrorMatrix->GetUp() = up;

    mirrorMatrix->GetRight() = CVector{
        up.y * fwd.z - up.z * fwd.y,
        up.z * fwd.x - up.x * fwd.z,
        up.x * fwd.y - up.y * fwd.x
    };
}

// 0x50B510
void CCamera::DealWithMirrorBeforeConstructRenderList(bool bActiveMirror, CVector mirrorNormal, float mirrorV, CMatrix* matMirror) {
    m_bMirrorActive = bActiveMirror;

    if (!bActiveMirror)
        return;

    if (matMirror)
        m_mMatMirror = *matMirror;
    else
        CalculateMirroredMatrix(mirrorNormal, mirrorV, &m_mCameraMatrix, &m_mMatMirror);

    m_mMatMirrorInverse = Invert(m_mMatMirror);
}

/// III/VC leftover
// 0x50B8F0
void CCamera::RenderMotionBlur() const {
    ZoneScoped;

    if (m_nBlurType != eMotionBlurType::NONE) {
        // CMBlur::MotionBlurRender(); // todo: Add CMBlur::MotionBlurRender is NOP, 0x71D700
    }
}

// 0x50B930
void CCamera::Restore() {
    m_bLookingAtPlayer = true;
    m_bLookingAtVector = false;
    m_nTypeOfSwitch = eSwitchType::INTERPOLATION;
    m_bUseNearClipScript = false;
    m_nModeObbeCamIsInForCar = 30;
    m_fPositionAlongSpline = 0.0f;
    m_bStartingSpline = false;
    m_bScriptParametersSetForInterp = false;
    m_nWhoIsInControlOfTheCamera = 0;

    CVehicle* vehicle = FindPlayerVehicle();
    CPlayerPed* player = FindPlayerPed();

    if (vehicle) {
        m_nModeToGoTo = MODE_CAM_ON_A_STRING;
        CEntity::SafeCleanUpRef(m_pTargetEntity);
        m_pTargetEntity = vehicle;
    } else {
        m_nModeToGoTo = MODE_FOLLOWPED;
        CEntity::SafeCleanUpRef(m_pTargetEntity);
        m_pTargetEntity = player;
    }
    CEntity::SafeRegisterRef(m_pTargetEntity);

    switch (player->m_nPedState) {
    case PEDSTATE_ENTER_CAR:
    case PEDSTATE_CARJACK:
    case PEDSTATE_OPEN_DOOR:
        m_nModeToGoTo = MODE_CAM_ON_A_STRING;
        break;
    }

    if (player->m_nPedState == PEDSTATE_EXIT_CAR) {
        m_nModeToGoTo = MODE_FOLLOWPED;

        CEntity::SafeCleanUpRef(m_pTargetEntity);
        m_pTargetEntity = player;
        CEntity::SafeRegisterRef(m_pTargetEntity);
    }

    CEntity::ClearReference(m_pAttachedEntity);

    m_bEnable1rstPersonCamCntrlsScript = false;
    m_bAllow1rstPersonWeaponsCamera = false;
    m_bUseScriptZoomValuePed = false;
    m_bUseScriptZoomValueCar = false;
    m_fAvoidTheGeometryProbsTimer = 0.0f;
    m_bStartInterScript = true;
    m_bCameraJustRestored = true;
}

// 0x50BAB0
void CCamera::RestoreWithJumpCut() {
    m_bRestoreByJumpCut = true;
    m_bLookingAtPlayer = true;
    m_bLookingAtVector = false;
    m_nTypeOfSwitch = eSwitchType::JUMPCUT;
    m_nWhoIsInControlOfTheCamera = 0;
    m_fPositionAlongSpline = 0.0f;
    m_bStartingSpline = false;
    m_bUseNearClipScript = false;
    m_nModeObbeCamIsInForCar = 30;
    m_bScriptParametersSetForInterp = false;

    CVehicle* vehicle = FindPlayerVehicle();
    CPlayerPed* player = FindPlayerPed();

    if (vehicle) {
        m_nModeToGoTo = MODE_CAM_ON_A_STRING;
        CEntity::SafeCleanUpRef(m_pTargetEntity);
        m_pTargetEntity = vehicle;
    } else {
        m_nModeToGoTo = MODE_FOLLOWPED;
        CEntity::SafeCleanUpRef(m_pTargetEntity);
        m_pTargetEntity = player;
    }
    CEntity::SafeRegisterRef(m_pTargetEntity);

    switch (player->m_nPedState) {
    case PEDSTATE_ENTER_CAR:
    case PEDSTATE_CARJACK:
    case PEDSTATE_OPEN_DOOR:
        m_nModeToGoTo = MODE_CAM_ON_A_STRING;
        break;
    }

    if (player->m_nPedState == PEDSTATE_EXIT_CAR) {
        m_nModeToGoTo = MODE_FOLLOWPED;

        CEntity::SafeCleanUpRef(m_pTargetEntity);
        m_pTargetEntity = player;
        CEntity::SafeRegisterRef(m_pTargetEntity);
    }

    if (!m_bCooperativeCamMode) {
        m_bUseScriptZoomValuePed = false;
        m_bUseScriptZoomValueCar = false;
        return;
    }

    CPlayerPed* player0 = FindPlayerPed(0);
    CPlayerPed* player1 = FindPlayerPed(1);

    if (!player0) {
        m_bUseScriptZoomValuePed = false;
        m_bUseScriptZoomValueCar = false;
        return;
    }

    if (!player1) {
        m_bUseScriptZoomValuePed = false;
        m_bUseScriptZoomValueCar = false;
        return;
    }

    CEntity::SafeCleanUpRef(m_pTargetEntity);

    if (!player0->IsInVehicle() || !player1->IsInVehicle()) {
        m_nModeToGoTo = m_nModeForTwoPlayersNotBothInCar;
        m_pTargetEntity = player0;
        CEntity::SafeRegisterRef(m_pTargetEntity);

        m_bUseScriptZoomValuePed = false;
        m_bUseScriptZoomValueCar = false;
        return;
    }

    if (player0->m_pVehicle == player1->m_pVehicle) {
        if (m_bAllowShootingWith2PlayersInCar) {
            m_nModeToGoTo = m_nModeForTwoPlayersSameCarShootingAllowed;
        } else {
            m_nModeToGoTo = m_nModeForTwoPlayersSameCarShootingNotAllowed;
        }
    } else {
        m_nModeToGoTo = m_nModeForTwoPlayersSeparateCars;
    }

    m_pTargetEntity = player0->m_pVehicle;
    CEntity::SafeRegisterRef(m_pTargetEntity);

    m_bUseScriptZoomValuePed = false;
    m_bUseScriptZoomValueCar = false;
}

// 0x50BD20
void CCamera::SetCamCutSceneOffSet(const CVector& offset) {
    m_vecCutSceneOffset = offset;
}

// 0x50BD40
void CCamera::SetCameraDirectlyBehindForFollowPed_CamOnAString() {
    m_bCamDirectlyBehind = true;
    CPed* player = FindPlayerPed();
    if (player) {
        m_fPedOrientForBehindOrInFront = CGeneral::GetATanOfXY(player->GetForward().x, player->GetForward().y);
    }
}

// 0x50BD70
void CCamera::SetCameraDirectlyInFrontForFollowPed_CamOnAString() {
    m_bCamDirectlyInFront = true;
    CPed* player = FindPlayerPed();
    if (player != nullptr) {
        m_fPedOrientForBehindOrInFront = CGeneral::GetATanOfXY(player->GetForward().x, player->GetForward().y);
    }
}

// unused
// 0x50BDA0
void CCamera::SetCameraDirectlyBehindForFollowPed_ForAPed_CamOnAString(CPed* targetPed) {
    if (!targetPed) {
        return;
    }

    m_bCamDirectlyBehind = true;
    m_bLookingAtPlayer = false;

    TheCamera.m_pTargetEntity = targetPed;
    CEntity::ChangeEntityReference(GetActiveCamera().m_pCamTargetEntity, targetPed);
    m_fPedOrientForBehindOrInFront = targetPed->GetHeading();
}

// 0x50BE30
void CCamera::SetCameraDirectlyInFrontForFollowPed_ForAPed_CamOnAString(CPed* targetPed) {
    if (!targetPed) {
        return;
    }

    m_bLookingAtPlayer = false;
    m_pTargetEntity = targetPed;

    CCam& camera = GetActiveCamera();
    CEntity::SafeCleanUpRef(camera.m_pCamTargetEntity);

    camera.m_pCamTargetEntity = targetPed;
    camera.m_pCamTargetEntity->RegisterReference(camera.m_pCamTargetEntity);

    m_bCamDirectlyInFront = true;
    m_fPedOrientForBehindOrInFront = CGeneral::GetATanOfXY(targetPed->GetForward().x, targetPed->GetForward().y);
}

// 0x50BEC0
void CCamera::SetCamPositionForFixedMode(const CVector& fixedModeSource, const CVector& fixedModeUpOffset) {
    m_vecFixedModeSource = fixedModeSource;
    m_vecFixedModeUpOffSet = fixedModeUpOffset;
    m_bGarageFixedCamPositionSet = false;
}

// 0x50BF00
void CCamera::SetFadeColour(uint8 red, uint8 green, uint8 blue) {
    m_bFadeTargetIsSplashScreen = false;
    if (red == 2 && green == 2 && blue == 2) {
        m_bFadeTargetIsSplashScreen = true;
    }

    CDraw::FadeRed   = red;
    CDraw::FadeGreen = green;
    CDraw::FadeBlue  = blue;
}

// 0x50BF40
void CCamera::SetMotionBlur(uint8 red, uint8 green, uint8 blue, int32 value, eMotionBlurType blurType) {
    m_nBlurRed    = red;
    m_nBlurGreen  = green;
    m_nBlurBlue   = blue;
    m_nBlurType   = blurType;
    m_nMotionBlur = value;
}

// 0x50BF80
void CCamera::SetMotionBlurAlpha(int32 alpha) {
    m_nMotionBlurAddAlpha = alpha;
}

// 0x50BF90
void CCamera::SetNearClipScript(float nearClip) {
    m_fNearClipScript = nearClip;
    m_bUseNearClipScript = true;
}

// 0x50BFB0
void CCamera::SetNewPlayerWeaponMode(eCamMode mode, int16 minZoom, int16 maxZoom) {
    m_PlayerWeaponMode.m_nMode     = mode;
    m_PlayerWeaponMode.m_nMinZoom  = minZoom;
    m_PlayerWeaponMode.m_nMaxZoom  = maxZoom;
    m_PlayerWeaponMode.m_fDuration = 0.0f;
}

// 0x50BFF0
bool CCamera::Using1stPersonWeaponMode() const {
    switch (m_PlayerWeaponMode.m_nMode) {
    case MODE_SNIPER:
    case MODE_M16_1STPERSON:
    case MODE_ROCKETLAUNCHER:
    case MODE_ROCKETLAUNCHER_HS:
    case MODE_HELICANNON_1STPERSON:
    case MODE_CAMERA:
    case MODE_AIMWEAPON_ATTACHED:
        return true;
    default:
        return false;
    }
}

// 0x50C030
void CCamera::SetParametersForScriptInterpolation(float interpolationToStopMoving, float interpolationToCatchUp, uint32 timeForInterpolation) {
    m_nScriptTimeForInterpolation = timeForInterpolation;
    m_bScriptParametersSetForInterp = true;
    m_fScriptPercentageInterToStopMoving = interpolationToStopMoving / 100.0f;
    m_fScriptPercentageInterToCatchUp = interpolationToCatchUp / 100.0f;
}

// 0x50C070
void CCamera::SetPercentAlongCutScene(float percent) {
    auto& cam = m_aCams[m_nActiveCam];
    if (cam.m_nMode == eCamMode::MODE_FLYBY) {
        cam.m_fTimeElapsedFloat = (float)cam.m_nFinishTime * percent / 100.0f;
        return;
    }

    cam = m_aCams[(m_nActiveCam + 1) % 2];
    if (cam.m_nMode == eCamMode::MODE_FLYBY) {
        cam.m_fTimeElapsedFloat = (float)cam.m_nFinishTime * percent / 100.0f;
        return;
    }
}

// 0x50C100
void CCamera::SetRwCamera(RwCamera* camera) {
    m_pRwCamera = camera;
    m_mViewMatrix.Attach(&camera->viewMatrix, false);
}

// 0x50C140
void CCamera::SetWideScreenOn() {
    m_bWideScreenOn = true;
    m_bWantsToSwitchWidescreenOff = false;
}

// 0x50C150
void CCamera::SetWideScreenOff() {
    m_bWantsToSwitchWidescreenOff = m_bWideScreenOn;
}

// 0x50C160
void CCamera::SetZoomValueFollowPedScript(int16 zoomMode) {
    switch (zoomMode) {
    case 1:
        m_fPedZoomValueScript = 1.50f;
        break;
    case 2:
        m_fPedZoomValueScript = 2.90f;
        break;
    default:
        m_fPedZoomValueScript = 0.25f;
    }
    m_bUseScriptZoomValuePed = true;
}

// zoomMode : 0- ZOOM_ONE , 1- ZOOM_TWO , 2- ZOOM_THREE
// 0x50C1B0
void CCamera::SetZoomValueCamStringScript(int16 zoomMode) {
    auto entity = m_aCams[0].m_pCamTargetEntity;

    if (entity->GetStatus() == STATUS_SIMPLE) {
        int32 arrPos{};
        VERIFY(GetArrPosForVehicleType(static_cast<eVehicleType>(entity->AsVehicle()->GetVehicleAppearance()), arrPos));
        m_fCarZoomValueScript = [zoomMode]{
            switch (zoomMode) {
            case 0:
                return std::array{ -1.0f, -0.2f, -3.20f, 0.05f, -2.41f }; // 0x8CC3E0
            case 1:
                return std::array{ +1.0f, +1.4f, +0.65f, 1.90f, +6.49f }; // 0x8CC3F4
            case 2:
                return std::array{ +6.0f, +6.0f, +15.9f, 15.9f, +15.0f }; // 0x8CC408
            default:
                NOTSA_UNREACHABLE("Unexpected zoom mode: {}", zoomMode);
            }
        }()[arrPos];
    
        m_bUseScriptZoomValueCar = true;
    } else {
        SetZoomValueFollowPedScript(zoomMode);
    }
}

// 0x50C260
void CCamera::StartCooperativeCamMode() {
    m_bCooperativeCamMode = true;
    CGameLogic::n2PlayerPedInFocus = eFocusedPlayer::NONE;
}

// 0x50C270
void CCamera::StopCooperativeCamMode() {
    m_bCooperativeCamMode = false;
    CGameLogic::n2PlayerPedInFocus = eFocusedPlayer::NONE;
}

// 0x50C280
void CCamera::AllowShootingWith2PlayersInCar(bool bAllow) {
    m_bAllowShootingWith2PlayersInCar = bAllow;
}

// 0x50C290
void CCamera::StoreValuesDuringInterPol(CVector* sourceDuringInter, CVector* targetDuringInter, CVector* upDuringInter, float* FOVDuringInter) {
    m_vecSourceDuringInter = *sourceDuringInter;
    m_vecTargetDuringInter = *targetDuringInter;
    m_vecUpDuringInter     = *upDuringInter;
    m_fFOVDuringInter      = *FOVDuringInter;

    auto dist = *sourceDuringInter - m_vecTargetDuringInter;
    m_fBetaDuringInterPol = CGeneral::GetATanOfXY(dist.x, dist.y);

    float distOnGround = dist.Magnitude2D();
    m_fAlphaDuringInterPol = CGeneral::GetATanOfXY(distOnGround, dist.z);
}

// 0x50C360
void CCamera::UpdateTargetEntity() {
    m_bPlayerWasOnBike = m_pTargetEntity && m_pTargetEntity->GetIsTypeVehicle() && m_pTargetEntity->AsVehicle()->m_vecMoveSpeed.SquaredMagnitude() > 0.3f;

    const auto player = FindPlayerPed();
    assert(player);

    auto something{ true };
    if (m_nWhoIsInControlOfTheCamera == 2) {
        m_nModeObbeCamIsInForCar = m_nModeObbeCamIsInForCar;
        switch (m_nModeObbeCamIsInForCar) {
        case 8:
        case 7: {
            if (player->m_nPedState != PEDSTATE_ARRESTED) {
                something = false;
            }

            if (!FindPlayerVehicle()) {
                CEntity::ChangeEntityReference(m_pTargetEntity, player);
            }

            break;
        }
        }
    }

    if (!m_bLookingAtPlayer && !something || m_bTransitionState) {
        if (m_pTargetEntity) {
            if (!m_bTargetJustBeenOnTrain) {
                return;
            }
        }
        
    }

    bool playerDoingSomethingWhileDriveBy{};
    if ([&, this]() { // Check is player doing drive-by
        if (!FindPlayerVehicle()) {
            return true;
        }

        if (!CGameLogic::IsCoopGameGoingOn()) {
            if (player->GetTaskManager().GetSimplestActiveTaskAs<CTaskSimpleGangDriveBy>()) {
                return true;
            }
        }

        return false;
    }()) {
        CEntity::ChangeEntityReference(m_pTargetEntity, player);

        playerDoingSomethingWhileDriveBy = [this, player] {
            switch (player->m_nPedState) {
            case PEDSTATE_ENTER_CAR:
            case PEDSTATE_CARJACK:
            case PEDSTATE_OPEN_DOOR:
                return true;
            }
            return false;
        }();

        if (!playerDoingSomethingWhileDriveBy) {
            auto& cam = GetActiveCam();
            if (cam.m_pCamTargetEntity != m_pTargetEntity) {
                CEntity::ChangeEntityReference(cam.m_pCamTargetEntity, m_pTargetEntity);
            }
        }
    } else {
        CEntity::ChangeEntityReference(m_pTargetEntity, FindPlayerVehicle());
    }

    const auto canEnterCar = player && player->m_pVehicle && player->m_pVehicle->CanPedOpenLocks(player); // Inverted this variable

    if (canEnterCar && player->m_nPedState == PEDSTATE_ENTER_CAR && !playerDoingSomethingWhileDriveBy) {
        if (m_nCarZoom) {
            CEntity::ChangeEntityReference(m_pTargetEntity, FindPlayerEntity());
        }
    }

    if (canEnterCar) {
        switch (player->m_nPedState) {
        case PEDSTATE_CARJACK:
        case PEDSTATE_OPEN_DOOR: {
            if (!playerDoingSomethingWhileDriveBy) {
                if (m_nCarZoom) {
                    CEntity::ChangeEntityReference(m_pTargetEntity, FindPlayerEntity());
                }
            }

            if (!FindPlayerVehicle()) {
                CEntity::ChangeEntityReference(m_pTargetEntity, player);
            }
        }
        }
    }

    switch (player->m_nPedState) {
    case PEDSTATE_EXIT_CAR:
    case PEDSTATE_DRAGGED_FROM_CAR:
        CEntity::ChangeEntityReference(m_pTargetEntity, player);
    }

    if (m_pTargetEntity->GetIsTypeVehicle()) {
        if (m_nCarZoom == 0) {
            if (player->m_nPedState == PEDSTATE_ARRESTED) {
                CEntity::ChangeEntityReference(m_pTargetEntity, player);
            }
        }
    }
}

// 0x50C7C0
void CCamera::TakeControl(CEntity* target, eCamMode modeToGoTo, eSwitchType switchType, int32 whoIsInControlOfTheCamera) {
    if (!m_bCinemaCamera) {
        if (whoIsInControlOfTheCamera == 2 && m_nWhoIsInControlOfTheCamera == 1) {
            return;
        }
    }
    m_nWhoIsInControlOfTheCamera = whoIsInControlOfTheCamera;

    const auto [newGoToMode, newTargetEntity] = [&, this]() -> std::tuple<eCamMode, CEntity*>{
        if (target) {
            return {
                [&, this] {
                    if (modeToGoTo == MODE_NONE) {
                        switch (target->GetType()) {
                        case ENTITY_TYPE_PED:
                            return MODE_FOLLOWPED;
                        case ENTITY_TYPE_VEHICLE:
                            return MODE_CAM_ON_A_STRING;
                        }
                    }
                    return modeToGoTo;
                }(),
                target
            };
        }

        return { modeToGoTo, FindPlayerEntity() };
    }();

    CEntity::ChangeEntityReference(m_pTargetEntity, newTargetEntity);
    m_nModeToGoTo = newGoToMode;

    m_nTypeOfSwitch    = switchType;
    m_bLookingAtPlayer = m_bLookingAtVector = false;
    m_bStartInterScript = true;
}

// 0x50C8B0
void CCamera::TakeControlNoEntity(const CVector& fixedModeVector, eSwitchType switchType, int32 whoIsInControlOfTheCamera) {
    if (whoIsInControlOfTheCamera == 2 && m_nWhoIsInControlOfTheCamera == 1)
        return;

    m_nWhoIsInControlOfTheCamera = whoIsInControlOfTheCamera;
    m_bLookingAtVector           = true;
    m_nModeToGoTo                = MODE_FIXED;
    m_bLookingAtPlayer           = false;
    m_vecFixedModeVector         = fixedModeVector;
    m_nTypeOfSwitch              = switchType;
    m_bStartInterScript          = true;
}

// 0x50C910
void CCamera::TakeControlAttachToEntity(CEntity* target, CEntity* attached, CVector* attachedCamOffset, CVector* attachedCamLookAt, float tilt, eSwitchType switchType, int32 whoIsInControlOfTheCamera) {
    if (whoIsInControlOfTheCamera == 2 && m_nWhoIsInControlOfTheCamera == 1) {
        return;
    }
    m_nWhoIsInControlOfTheCamera = whoIsInControlOfTheCamera;
    if (!attached) {
        attached = FindPlayerVehicle();
        if (!attached) {
            attached = FindPlayerPed();
        }
    }
    if (target) {
        CEntity::ChangeEntityReference(m_pTargetEntity, target);
        m_bLookingAtVector = false;
    } else {
        m_bLookingAtVector = true;
        m_vecAttachedCamLookAt = *attachedCamLookAt != *attachedCamOffset ? *attachedCamLookAt : CVector{};
    }
    m_vecAttachedCamOffset = *attachedCamOffset == CVector{} ? CVector{0.0f, 0.0f, 2.0f} : *attachedCamOffset;
    m_fAttachedCamAngle = tilt;
    CEntity::ChangeEntityReference(m_pAttachedEntity, attached);
    m_nModeToGoTo = MODE_ATTACHCAM;
    m_nTypeOfSwitch = switchType;
    m_bLookingAtPlayer = false;
    m_bStartInterScript = true;
}

// 0x50CAE0
void CCamera::TakeControlWithSpline(eSwitchType switchType) {
    m_bLookingAtPlayer = false;
    m_bLookingAtVector = false;
    m_bCutsceneFinished = false;
    m_nModeToGoTo = MODE_FLYBY;
    m_nTypeOfSwitch = switchType;
    m_bStartInterScript = true;
}

// 0x50CB10
void CCamera::UpdateAimingCoors(const CVector& aimingTargetCoors) {
    m_vecAimingTargetCoors = aimingTargetCoors;
}

// 0x515BD0
void CCamera::UpdateSoundDistances() {
    const auto mode = m_aCams[m_nActiveCam].m_nMode;
    const bool firstPerson = notsa::contains({
        MODE_1STPERSON, MODE_SNIPER, MODE_SNIPER_RUNABOUT, MODE_ROCKETLAUNCHER_RUNABOUT,
        MODE_ROCKETLAUNCHER_RUNABOUT_HS, MODE_M16_1STPERSON_RUNABOUT, MODE_FIGHT_CAM_RUNABOUT,
        MODE_1STPERSON_RUNABOUT, MODE_HELICANNON_1STPERSON, MODE_CAMERA, MODE_M16_1STPERSON,
        MODE_ROCKETLAUNCHER, MODE_ROCKETLAUNCHER_HS
    }, mode);
    const auto source = m_mCameraMatrix.GetForward() * (firstPerson && m_pTargetEntity->IsPed() ? 0.5f : 5.0f) + GetPosition();
    const auto frame = CTimer::GetFrameCounter() % 12;
    if (frame == 0) {
        m_fSoundDistUpAsReadOld = m_fSoundDistUpAsRead;
        CColPoint collision{};
        CEntity* hitEntity{};
        m_fSoundDistUpAsRead = CWorld::ProcessVerticalLine(source, source.z + 20.0f, collision, hitEntity, true, false, false, false, true, false)
            ? collision.m_vecPoint.z - source.z
            : 20.0f;
    }
    const float blend = (float)(frame + 1) * (1.0f / 6.0f);
    m_fSoundDistUp = blend * m_fSoundDistUpAsRead + (1.0f - blend) * m_fSoundDistUpAsReadOld;
}

// unused
// 0x50CB90
void CCamera::SetNearClipBasedOnPedCollision(float arg2) {
    static auto& gSqrDistanceToNearestPed = StaticRef<float>(0xB6EC68);

    static auto& distanceScale = StaticRef<float>(0x8CCC84);
    static auto& maxClip = StaticRef<float>(0x8CCC80);

    const float minClip = gpCamColVars[4];
    float nearClip = std::sqrt(arg2) / gSqrDistanceToNearestPed * distanceScale * (maxClip - minClip) + minClip;
    if (nearClip < minClip) {
        nearClip = minClip;
    }
    RwCameraSetNearClipPlane(Scene.m_pRwCamera, nearClip);
}

// TODO: eAimingType
// 0x50CBF0
void CCamera::SetColVarsAimWeapon(int32 aimingType) {
    switch (aimingType) {
    case 0:
        CCamera::SetCamCollisionVarDataSet(0);
        break;
    case 1:
        CCamera::SetCamCollisionVarDataSet(1);
        break;
    case 2:
        CCamera::SetCamCollisionVarDataSet(2);
        break;
    case 3:
        CCamera::SetCamCollisionVarDataSet(3);
        break;
    default:
        return;
    }
}

// 0x50CC50
void CCamera::SetColVarsPed(ePedType pedType, int32 nCamPedZoom) {
    const int32 camColVars = [=] {
        switch (pedType) {
        case PED_TYPE_PLAYER1:
            return nCamPedZoom + 3;
        case PED_TYPE_PLAYER2:
            return nCamPedZoom + 6;
        default:
            return 0;
        }
    }();

    if (camColVars != gCurCamColVars) {
        gCurCamColVars = camColVars;
        gCurDistForCam = 1.0f;
        gpCamColVars = gCamColVars[camColVars];
    }
}

// 0x50CD30
void CCamera::CameraGenericModeSpecialCases(CPed* targetPed) {
    m_nExtraEntitiesCount = 0;

    if (!targetPed) {
        return;
    }

    auto* taskHold = static_cast<CTaskSimpleHoldEntity*>(targetPed->GetIntelligence()->GetTaskHold(false));
    if (!taskHold || !taskHold->m_pEntityToHold) {
        return;
    }

    m_pExtraEntity[m_nExtraEntitiesCount++] = targetPed;
}

// 0x50CD80
void CCamera::CameraPedModeSpecialCases() {
    CCollision::bCamCollideWithVehicles = true;
    CCollision::bCamCollideWithObjects  = true;
    CCollision::bCamCollideWithPeds     = true;
}

// 0x50CDA0
void CCamera::CameraPedAimModeSpecialCases(CPed* ped) {
    CameraPedModeSpecialCases();

    if (ped->IsInVehicle()) {
        m_pExtraEntity[m_nExtraEntitiesCount++] = ped->m_pVehicle;
    }
}

// 0x50CDE0
void CCamera::CameraVehicleModeSpecialCases(CVehicle* vehicle) {
    float speed = vehicle->m_vecMoveSpeed.Magnitude();

    const auto slow = speed <= 0.2f;
    CCollision::relVelCamCollisionVehiclesSqr = slow ? 0.1f : 1.0f;
    CCollision::bCamCollideWithVehicles = true;
    CCollision::bCamCollideWithPeds     = slow;
    CCollision::bCamCollideWithObjects  = slow;

    if (vehicle->m_pVehicleBeingTowed) {
        m_pExtraEntity[m_nExtraEntitiesCount++] = vehicle->m_pVehicleBeingTowed;
    }
}

// 0x50CE80
bool CCamera::IsExtraEntityToIgnore(CEntity* entity) {
    if (m_nExtraEntitiesCount <= 0) {
        return false;
    }
    return notsa::contains(m_pExtraEntity, entity);
}

// 0x420C40
bool CCamera::IsSphereVisible(const CVector& origin, float radius, RwMatrix* transformMatrix) {
    CVector point = origin;
    RwV3dTransformPoints(&point, &point, 1, transformMatrix);
    if (point.y + radius < CDraw::ms_fNearClipZ || point.y - radius > CDraw::ms_fFarClipZ) {
        return false;
    }
    for (size_t i = 0; i < 2; i++) {
        const auto& normal = m_avecFrustumNormals[i];
        if (point.x * normal.x + point.y * normal.y > radius) {
            return false;
        }
    }
    for (size_t i = 2; i < 4; i++) {
        const auto& normal = m_avecFrustumNormals[i];
        if (point.z * normal.z + point.y * normal.y > radius) {
            return false;
        }
    }
    return true;
}

// 0x420D40 - NOTE: Function has no hook
bool CCamera::IsSphereVisible(const CVector& origin, float radius) {
    return IsSphereVisible(origin, radius, (RwMatrix*)&m_mMatInverse)
        || (m_bMirrorActive && IsSphereVisible(origin, radius, (RwMatrix*)&m_mMatMirrorInverse));
}

// 0x50CEB0
bool CCamera::ConsiderPedAsDucking(CPed* ped) {
    auto task = ped->GetIntelligence()->GetTaskDuck(true);
    return task && ped->bIsDucking && !task->m_bIsAborting;
}

// 0x50CEF0
void CCamera::ResetDuckingSystem(CPed* ped) {
    m_fDuckCamMotionFactor    = 0.0f;
    m_fDuckAimCamMotionFactor = 0.0f;
    if (!ped)
        return;

    auto* task = ped->GetIntelligence()->GetTaskDuck(true);
    if (!task)
        return;

    if (!ped->bIsDucking || task->m_bIsAborting)
        return;

    float factor;
    if (ped->m_vecMoveSpeed.Magnitude() <= 0.000001f)
        factor = 0.3f - 1.0f;
    else
        factor = 0.3f - 0.5f;

    m_fDuckCamMotionFactor    = factor;
    m_fDuckAimCamMotionFactor = -0.35f;
}

// arg5 always used as false
// 0x50CFA0
void CCamera::HandleCameraMotionForDucking(CPed* ped, CVector* source, CVector* targPosn, bool arg5) {
    static auto& stationaryHeight = StaticRef<float>(0x8CCB94);
    static auto& movingHeight = StaticRef<float>(0x8CCB98);

    float targetFactor = 0.0f;
    if (ConsiderPedAsDucking(ped)) {
        targetFactor = ped->m_vecMoveSpeed.SquaredMagnitude() <= sq(0.001f)
            ? stationaryHeight - 1.0f
            : movingHeight - 0.5f;
    }
    if (!arg5) {
        m_fDuckCamMotionFactor += CTimer::GetTimeStep() * 0.1f * (targetFactor - m_fDuckCamMotionFactor);
    }
    if (source) {
        source->z += m_fDuckCamMotionFactor;
    }
    if (targPosn) {
        targPosn->z += m_fDuckCamMotionFactor;
    }
}

// arg5 always used as false
// 0x50D090
void CCamera::HandleCameraMotionForDuckingDuringAim(CPed* ped, CVector* source, CVector* targPosn, bool arg5) {
    const float targetFactor = ConsiderPedAsDucking(ped) ? -0.35f : 0.0f;
    if (!arg5) {
        m_fDuckAimCamMotionFactor += CTimer::GetTimeStep() * 0.13f * (targetFactor - m_fDuckAimCamMotionFactor);
    }
    if (source) {
        source->z += m_fDuckAimCamMotionFactor;
    }
    if (targPosn) {
        targPosn->z += m_fDuckAimCamMotionFactor;
    }
}

// 0x50D160
void CCamera::VectorMoveLinear(CVector* to, CVector* from, float duration, bool bMoveLinearWithEase) {
    const float time = (float)CTimer::GetTimeInMS();
    m_fMoveLinearStartTime = time;
    m_fMoveLinearEndTime   = time + duration;
    m_vecMoveLinearPosnStart = *from;
    m_vecMoveLinearPosnEnd   = *to;
    m_bMoveLinearWithEase    = bMoveLinearWithEase;
}

// 0x50D1D0
void CCamera::VectorTrackLinear(CVector* to, CVector* from, float duration, bool bEase) {
    const float time = (float)CTimer::GetTimeInMS();
    m_fTrackLinearStartTime = time;
    m_fTrackLinearEndTime   = time + duration;
    m_vecTrackLinearEndPoint   = *from;
    m_vecTrackLinearStartPoint = *to;
    m_bTrackLinearWithEase     = bEase;
}

// 0x516400
void CCamera::AddShake(float duration, float a2, float a3, float a4, float a5) {
    return AddShakeSimple(duration, 1, 1.0f);
}

// 0x50D240
void CCamera::AddShakeSimple(float durationMs, int32 type, float intensity) {
    m_fShakeIntensity = intensity;
    m_nShakeType = type;
    m_fStartShakeTime = static_cast<float>(CTimer::GetTimeInMS());
    m_fEndShakeTime = m_fStartShakeTime + durationMs;
}

// 0x50D280
void CCamera::LerpFOV(float zoomInFactor, float zoomOutFactor, float timeLimit, bool bEase) {
    m_fStartZoomTime = static_cast<float>(CTimer::GetTimeInMS());
    m_fEndZoomTime = static_cast<float>(CTimer::GetTimeInMS()) + timeLimit;

    m_nZoomMode = bEase; // TODO: Rename
    m_fZoomInFactor = zoomInFactor;
    m_fZoomOutFactor = zoomOutFactor;
}

// 0x50B5D0
void CCamera::ProcessFade() {
    ZoneScoped;

    if (!m_bFading) {
        return;
    }

    float fadeAlpha = 0.0f;

    if (m_nFadeInOutFlag == eFadeFlag::FADE_OUT) {
        m_fFadeDuration == 0.0f
            ? (m_fFadeAlpha += 0.0f)
            : (m_fFadeAlpha -= CTimer::GetTimeStepInSeconds() / m_fFadeDuration * 255.0f);

        if (m_fFadeAlpha > 0.0f) {
            CDraw::FadeValue = static_cast<uint8>(m_fFadeAlpha);
            return;
        }

        m_bFading = false;
    } else {
        if (m_nFadeInOutFlag == eFadeFlag::FADE_OUT) { // stupid, why not use a switch instead?
            CDraw::FadeValue = static_cast<uint8>(m_fFadeAlpha);
            return;
        }

        if (m_fFadeAlpha >= 255.0f) {
            m_bFading = false;
        }

        fadeAlpha = 255.0f;

        m_fFadeDuration == 0.0f
            ? (m_fFadeAlpha += 255.0f)
            : (m_fFadeAlpha += CTimer::GetTimeStepInSeconds() / m_fFadeDuration * 255.0f);

        if (m_fFadeAlpha < 255.0f) {
            CDraw::FadeValue = static_cast<uint8>(m_fFadeAlpha);
            return;
        }
    }

    m_fFadeAlpha = fadeAlpha;
    CDraw::FadeValue = static_cast<uint8>(m_fFadeAlpha);
}

// 0x50B6D0
void CCamera::ProcessMusicFade() {
    if (!m_bMusicFading)
        return;

    if (m_fTimeToWaitToFadeMusic <= 0.0f) {
        switch (m_nMusicFadingDirection) {
        case eFadeFlag::FADE_OUT: {
            m_fEffectsFaderScalingFactor = m_fTimeToFadeMusic > 0.0f
                ? CTimer::GetTimeStepInSeconds() / m_fTimeToFadeMusic + m_fEffectsFaderScalingFactor
                : 1.f;
            
            if (m_fEffectsFaderScalingFactor >= 1.0f) {
                m_bMusicFadedOut = false;
                m_bMusicFading = false;
                m_fEffectsFaderScalingFactor = 1.0f;
            }
            break;
        }
        case eFadeFlag::FADE_IN: {
            if (m_fEffectsFaderScalingFactor <= 0.0f) {
                m_bMusicFadedOut = true;
                m_bMusicFading = false;
                m_fEffectsFaderScalingFactor = 0.0f;
            }
            m_fEffectsFaderScalingFactor = m_fTimeToFadeMusic > 0.0f
                ? std::max(0.f, m_fEffectsFaderScalingFactor - CTimer::GetTimeStepInSeconds() / m_fTimeToFadeMusic)
                : 0.f;
            break;
        }
        }
    } else {
        m_fTimeToWaitToFadeMusic = m_fTimeToWaitToFadeMusic - CTimer::GetTimeStepInSeconds();
    }

    if (!AudioEngine.IsLoadingTuneActive()) {
        AudioEngine.SetMusicFaderScalingFactor(m_fEffectsFaderScalingFactor);
        AudioEngine.SetEffectsFaderScalingFactor(m_fEffectsFaderScalingFactor);
    }
}

// unused, empty
// 0x50B880
void CCamera::ProcessObbeCinemaCameraPed() {
    // NOP
}

//
void CCamera::ProcessObbeCinemaCameraPlane() {
    assert(0);
}

//
void CCamera::ProcessObbeCinemaCameraTrain() {
    assert(0);
}

// 0x50B890
void CCamera::ProcessWideScreenOn() {
    if (m_bWantsToSwitchWidescreenOff) {
        m_bWantsToSwitchWidescreenOff = false;
        m_bWideScreenOn = false;
        m_fWideScreenReductionAmount = 0.0f;
        m_fScreenReductionPercentage = 0.0f;
        m_fFOV_Wide_Screen = 0.0f;
    } else {
        m_fWideScreenReductionAmount = 1.0f;
        m_fScreenReductionPercentage = 30.0f;
        m_fFOV_Wide_Screen = m_aCams[m_nActiveCam].m_fFOV * 0.3f;
    }
}

// 0x516440
void CCamera::ProcessVectorTrackLinear() {
    const float now = (float)CTimer::GetTimeInMS();
    if (now <= m_fTrackLinearEndTime) {
        ProcessVectorTrackLinear(invLerp(m_fTrackLinearStartTime, m_fTrackLinearEndTime, now));
    } else if (m_bCameraPersistTrack) {
        m_bVecTrackLinearProcessed = true;
    }
}

// 0x50D350
void CCamera::ProcessVectorTrackLinear(float ratio) {
    m_bVecTrackLinearProcessed = true;
    const float t = m_bTrackLinearWithEase
        ? (std::sin(DegreesToRadians(270.0f - ratio * 180.0f)) + 1.0f) * 0.5f
        : ratio;
    m_vecTrackLinear = (m_vecTrackLinearStartPoint - m_vecTrackLinearEndPoint) * t + m_vecTrackLinearEndPoint;
}

//
void CCamera::ProcessObbeCinemaCameraBoat() {
    assert(0);
}

//
void CCamera::ProcessObbeCinemaCameraCar() {
    assert(0);
}

//
void CCamera::ProcessObbeCinemaCameraHeli() {
    assert(0);
}

// 0x50D430
void CCamera::ProcessVectorMoveLinear(float ratio) {
    m_bVecMoveLinearProcessed = true;
    const float t = m_bMoveLinearWithEase
        ? (std::sin(DegreesToRadians(270.0f - ratio * 180.0f)) + 1.0f) * 0.5f
        : ratio;
    m_vecMoveLinear = (m_vecMoveLinearPosnEnd - m_vecMoveLinearPosnStart) * t + m_vecMoveLinearPosnStart;
}

// 0x516500
void CCamera::ProcessFOVLerp() {
    if (const auto now = static_cast<float>(CTimer::GetTimeInMS()); now <= m_fEndZoomTime) { /* Check if still processing */
        ProcessFOVLerp(invLerp(m_fStartZoomTime, m_fEndZoomTime, now));
    } else if (m_bBlockZoom) { /* Finished */
        m_bFOVLerpProcessed = true;
    }
}

// 0x50D510
void CCamera::ProcessFOVLerp(float ratio) {
    m_bFOVLerpProcessed = true;
    const float t = m_nZoomMode != 0
        ? (std::sin(DegreesToRadians(270.0f - ratio * 180.0f)) + 1.0f) * 0.5f
        : ratio;
    m_fFOVNew = (m_fZoomOutFactor - m_fZoomInFactor) * t + m_fZoomInFactor;
}

// 0x5164A0
void CCamera::ProcessVectorMoveLinear() {
    const float now = (float)CTimer::GetTimeInMS();
    if (now <= m_fMoveLinearEndTime) {
        ProcessVectorMoveLinear(invLerp(m_fMoveLinearStartTime, m_fMoveLinearEndTime, now));
    } else if (m_bCameraPersistPosition) {
        m_bVecMoveLinearProcessed = true;
    }
}

// unused
// 0x51A6F0
void CCamera::ProcessShake() {
    const double now = (double)CTimer::GetTimeInMS();
    if (now <= m_fEndShakeTime) {
        ProcessShake((float)((now - m_fStartShakeTime) / ((double)m_fEndShakeTime - m_fStartShakeTime)));
    }
}

// shakeIntensity not used
// 0x516560
void CCamera::ProcessShake(float intensity) {
    static auto& initialized = StaticRef<bool>(0xB70048);
    auto& cam = m_aCams[m_nActiveCam];
    if (!initialized) {
        for (size_t i = 1; i < gHandShaker.size(); i++) {
            auto& shaker = gHandShaker[i];
            shaker.m_lim = {0.02f, 0.02f, i == 2 ? 0.04f : 0.01f};
            shaker.m_motion = {0.0002f, 0.0002f, 0.0001f};
            shaker.m_slow = {1.3f, 1.3f, 1.4f};
            shaker.m_scaleReactionMin = 0.3f;
            shaker.m_scaleReactionMax = 1.0f;
        }
        gHandShaker[1].m_twitchFreq = 15;
        gHandShaker[1].m_twitchVel = 0.001f;
        gHandShaker[2].m_twitchFreq = 20;
        gHandShaker[2].m_twitchVel = 0.001f;
        gHandShaker[3].m_twitchFreq = 10;
        gHandShaker[3].m_twitchVel = 0.0005f;
        gHandShaker[4].m_twitchFreq = 20;
        gHandShaker[4].m_twitchVel = 0.002f;
        gHandShaker[5].m_twitchFreq = 2;
        gHandShaker[5].m_twitchVel = 0.003f;
        initialized = true;
    }

    auto& shaker = gHandShaker[m_nShakeType];
    shaker.Process(m_fShakeIntensity);
    const float roll = shaker.m_ang.z * m_fShakeIntensity;
    cam.m_vecFront = shaker.m_resultMat.InverseTransformVector(cam.m_vecFront);
    cam.m_vecFront.Normalise();
    cam.m_vecUp = {std::sin(roll), 0.0f, std::cos(roll)};
    auto right = CrossProduct(cam.m_vecFront, cam.m_vecUp).Normalized();
    cam.m_vecUp = CrossProduct(right, cam.m_vecFront);
    if (cam.m_vecFront.x == 0.0f && cam.m_vecFront.y == 0.0f) {
        cam.m_vecFront.x = cam.m_vecFront.y = 0.0001f;
    }
    right = CrossProduct(cam.m_vecFront, cam.m_vecUp).Normalized();
    cam.m_vecUp = CrossProduct(right, cam.m_vecFront);
}

// inlined - 0x52B845
// 0x516AE0
void CCamera::ProcessScriptedCommands() {
    ProcessVectorMoveLinear();
    ProcessVectorTrackLinear();
    ProcessFOVLerp();
}

// 0x52B730
void CCamera::Process() {
    ZoneScoped;

    plugin::CallMethod<0x52B730, CCamera*>(this);
}

// 0x514860
void CCamera::DrawBordersForWideScreen() {
    CRect rect;
    GetScreenRect(&rect);
    if (m_nBlurType == eMotionBlurType::NONE || m_nBlurType == eMotionBlurType::LIGHT_SCENE) {
        m_nMotionBlurAddAlpha = 80;
    }
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RWRSTATE(NULL));
    CSprite2d::DrawRect({ -5.f, -5.f,     SCREEN_WIDTH + 5.f, rect.top         }, { 0, 0, 0, 255 });
    CSprite2d::DrawRect({ -5.f, rect.bottom, SCREEN_WIDTH + 5.f, SCREEN_HEIGHT + 5.f }, { 0, 0, 0, 255 });
}

// 0x4748A0
bool CCamera::VectorMoveRunning() const {
    return CTimer::m_snTimeInMilliseconds <= m_fMoveLinearEndTime;
}

// 0x474891
bool CCamera::VectorTrackRunning() const {
    return CTimer::m_snTimeInMilliseconds <= m_fTrackLinearEndTime;
}

// 0x514950
void CCamera::FinishCutscene() {
    SetPercentAlongCutScene(100.0f);
    m_fPositionAlongSpline = 1.0f;
    m_bCutsceneFinished = true;
}

// 0x514970
void CCamera::Find3rdPersonCamTargetVector(float range, CVector gunMuzzle, CVector& outSource, CVector& outTarget) {
    const auto pActiveCam = &m_aCams[m_nActiveCam];
    const float tanHalfFOV = std::tan(DegreesToRadians(pActiveCam->m_fFOV * 0.5f));
    const float aspectRatio = CDraw::ms_fAspectRatio;
    
    // Calculate aim target direction (This will be a unit vector)
    CVector dir = m_aCams[m_nActiveCam].m_vecFront;
    
    if (pActiveCam->m_nMode == eCamMode::MODE_TWOPLAYER_IN_CAR_AND_SHOOTING) {
        pActiveCam->Get_TwoPlayer_AimVector(dir);
    } else {
        // Vertical offset
        dir += pActiveCam->m_vecUp * (tanHalfFOV * ((0.5f - m_f3rdPersonCHairMultY) * 2.0f) / aspectRatio);

        // Horizontal offset
        const auto right = pActiveCam->m_vecFront.Cross(pActiveCam->m_vecUp);
        dir += right * (tanHalfFOV * ((m_f3rdPersonCHairMultX - 0.5f) * 2.0f));
        
        // Handle zero magnitude case
        if (dir.Magnitude() <= 0.0f) {
            dir = CVector(1.0f, 0.0f, 0.0f);
        } else {
            dir.Normalise();
        }
    }
    
    // Calculate intersection point with muzzle
    outSource = pActiveCam->m_vecSource;
    outSource += (gunMuzzle - outSource).ProjectOnToNormal(dir);

    // Apply final range to target 
    outTarget = outSource + dir * range;
}

// 0x514B80
float CCamera::CalculateGroundHeight(eGroundHeightType type) {
    static auto& lastCalcCamPos    = StaticRef<CVector>(0xB70034);
    static auto& exactGroundHeight = StaticRef<float>(0xB70030);
    static auto& bbTopZ            = StaticRef<float>(0xB7002C);
    static auto& bbBottomZ         = StaticRef<float>(0xB70028);

    const auto& camPos = GetPosition();

    // Possibly update the positions (If the camera has moved enough)
    const auto CheckDelta = [](float d) { return std::abs(d) > 20.f; };
    if (CheckDelta(lastCalcCamPos.x - camPos.x) || CheckDelta(lastCalcCamPos.y - camPos.y) || CheckDelta(lastCalcCamPos.z - camPos.z)) { // Check if there's enough of a delta
        CColPoint cp;
        CEntity* hitEntity;
        if (CWorld::ProcessVerticalLine({ camPos.x, camPos.y, 1000.f }, -1000.f, cp, hitEntity, true, false, false, false, true)) {
            const auto& hitEntPos = hitEntity->GetPosition();
            const auto& hitBB = hitEntity->GetColModel()->GetBoundingBox();

            exactGroundHeight = cp.m_vecPoint.z;

            bbTopZ = hitEntPos.z + hitBB.m_vecMax.z;

            const auto bbsz = hitBB.GetSize();
            bbBottomZ = std::max(
                0.f,
                bbsz.x > 120.f || bbsz.y > 120.f
                    ? exactGroundHeight
                    : hitEntPos.z + hitBB.m_vecMin.z
            );
        }
        lastCalcCamPos = camPos;
    }

    switch (type) {
    case eGroundHeightType::ENTITY_BB_TOP:       return bbTopZ;
    case eGroundHeightType::EXACT_GROUND_HEIGHT: return exactGroundHeight;
    case eGroundHeightType::ENTITY_BB_BOTTOM:    return bbBottomZ;
    default:                                     NOTSA_UNREACHABLE();
    }
}

// 0x514030
void CCamera::AvoidTheGeometry(const CVector* source, const CVector* target, CVector* output, float FOV) {
    static auto& checkOtherEntities = StaticRef<bool>(0xB6EC65);
    static auto& nearClipOffset = StaticRef<float>(0x8CC38C);
    static auto& minimumNearClip = StaticRef<float>(0x8CC390);
    static auto& timerDamping = StaticRef<float>(0x8CC81C);
    static auto& sphereScale = StaticRef<float>(0x8CC820);
    static auto& motionFactor = StaticRef<float>(0xB6EC38);
    static auto& motionSpeed = StaticRef<float>(0xB6EC3C);

    const auto delta = *target - *source;
    m_vecClearGeometryVec = {};
    const auto horizontalDistance = delta.Magnitude2D();
    const auto& forward = m_mCameraMatrix.GetForward();
    const float heading = delta.x == 0.0f && delta.y == 0.0f
        ? CGeneral::GetATanOfXY(forward.x, forward.y)
        : CGeneral::GetATanOfXY(delta.x, delta.y);
    const float pitch = horizontalDistance == 0.0f && delta.z == 0.0f
        ? 0.0f
        : CGeneral::GetATanOfXY(horizontalDistance, delta.z);
    CVector direction{std::cos(heading) * std::cos(pitch), std::sin(heading) * std::cos(pitch), std::sin(pitch)};
    *output = *target - direction * delta.Magnitude();
    direction.Normalise();

    CColPoint collision{};
    CEntity* hitEntity{};
    CWorld::pIgnoreEntity = m_pTargetEntity;
    if (CWorld::ProcessLineOfSight(*target, *output, collision, hitEntity, true, false, false, true, false, false, true, false)) {
        *output = collision.m_vecPoint;
        const auto firstCollision = *output;
        if (checkOtherEntities && CWorld::ProcessLineOfSight(*output, *target, collision, hitEntity, false, true, true, true, false, false, true, false)) {
            const float nearClip = RwCameraGetNearClipPlane(Scene.m_pRwCamera);
            if (DistanceBetweenPoints(*output, collision.m_vecPoint) < nearClip) {
                *output = collision.m_vecPoint;
            } else if (DistanceBetweenPoints(*output, firstCollision) < nearClip) {
                *output = firstCollision;
            }
        }
    }
    CWorld::pIgnoreEntity = nullptr;
    if (FindPlayerPed()) {
        const float nearClip = DistanceBetweenPoints(*target, *output) - nearClipOffset;
        if (nearClip < RwCameraGetNearClipPlane(Scene.m_pRwCamera)) {
            RwCameraSetNearClipPlane(Scene.m_pRwCamera, std::max(nearClip, minimumNearClip));
        }
    }

    const float nearClip = RwCameraGetNearClipPlane(Scene.m_pRwCamera);
    const float radius = nearClip * std::tan(DegreesToRadians(FOV) * 0.5f) * CDraw::ms_fAspectRatio * sphereScale;
    const auto sphereCenter = *output + direction * nearClip;
    float desiredMotion = 0.0f;
    if (CWorld::TestSphereAgainstWorld(sphereCenter, radius, nullptr, true, false, false, true, false, true)) {
        const auto& point = gaTempSphereColPoints[0];
        auto displacement = point.m_vecPoint - sphereCenter;
        const float collisionDistance = DotProduct(point.m_vecPoint - *output, direction);
        if (collisionDistance > minimumNearClip && collisionDistance < 0.9f) {
            if (collisionDistance < RwCameraGetNearClipPlane(Scene.m_pRwCamera)) {
                RwCameraSetNearClipPlane(Scene.m_pRwCamera, collisionDistance);
            }
        } else if (collisionDistance < minimumNearClip) {
            RwCameraSetNearClipPlane(Scene.m_pRwCamera, minimumNearClip);
        }
        const float penetration = radius - displacement.Magnitude();
        displacement.Normalise();
        auto normal = point.m_vecNormal.Normalized();
        if (DotProduct(normal, -displacement) < 0.0f) {
            normal = -normal;
        }
        desiredMotion = 1.0f;
        m_vecClearGeometryVec = normal * DotProduct(-displacement * penetration, normal);
        if (m_pTargetEntity && m_pTargetEntity->IsPed() && RwCameraGetNearClipPlane(Scene.m_pRwCamera) < minimumNearClip + minimumNearClip) {
            const float facing = DotProduct(normal, m_pTargetEntity->GetMatrix().GetForward());
            if (facing < 0.0f) {
                m_fAvoidTheGeometryProbsTimer = std::max(m_fAvoidTheGeometryProbsTimer, 0.0f) + CTimer::GetTimeStep();
            } else if (facing > 0.5f) {
                m_fAvoidTheGeometryProbsTimer = std::min(m_fAvoidTheGeometryProbsTimer, 0.0f) - CTimer::GetTimeStep();
            }
            if (m_nAvoidTheGeometryProbsDirn == 0) {
                m_nAvoidTheGeometryProbsDirn = CrossProduct(m_pTargetEntity->GetPosition() - *output, normal).z <= 0.0f ? 1 : (uint16)-1;
            }
        }
    }
    m_fAvoidTheGeometryProbsTimer *= std::pow(timerDamping, CTimer::GetTimeStep());
    WellBufferMe(desiredMotion, motionFactor, motionSpeed, 0.2f, 0.05f, false);
    m_vecClearGeometryVec *= motionFactor;
    m_bMoveCamToAvoidGeom = true;
}

// 0x514D60
void CCamera::CalculateFrustumPlanes(bool bForMirror) {
    // The executable uses this approximate half-degree conversion constant.
    const float angle = CDraw::ms_fFOV * 0.00872638915f;
    const float cosine = std::cos(angle);
    const float sine = std::sin(angle);
    const float aspect = (float)RsGlobal.maximumHeight / (float)RsGlobal.maximumWidth;
    m_avecFrustumNormals[0] = {cosine, -sine, 0.0f};
    m_avecFrustumNormals[1] = {-cosine, -sine, 0.0f};
    m_avecFrustumNormals[2] = {0.0f, -(aspect * sine), -(aspect * cosine)};
    m_avecFrustumNormals[3] = {0.0f, -(aspect * sine), aspect * cosine};

    auto& normals = bForMirror ? m_avecFrustumWorldNormals_Mirror : m_avecFrustumWorldNormals;
    auto& offsets = bForMirror ? m_fFrustumPlaneOffsets_Mirror : m_fFrustumPlaneOffsets;
    TransformVectors(normals.data(), (int32)normals.size(), m_mCameraMatrix, m_avecFrustumNormals.data());
    const auto& position = GetPosition();
    for (size_t i = 0; i < normals.size(); i++) {
        const auto& normal = normals[i];
        offsets[i] = normal.y * position.y + normal.z * position.z + normal.x * position.x;
    }
}

// 0x5150E0
void CCamera::CalculateDerivedValues(bool bForMirror, bool bOriented) {
    m_mMatInverse = Invert(m_mCameraMatrix);
    CalculateFrustumPlanes(bForMirror);

    auto& forward = m_mCameraMatrix.GetForward();
    if (forward.x == 0.0f && forward.y == 0.0f) {
        forward.x = 0.0001f;
    } else if (bOriented) {
        m_fOrientation = std::atan2(forward.x, forward.y);
    }

    m_fCamFrontXNorm = forward.x;
    m_fCamFrontYNorm = forward.y;
    const float length = std::sqrt(sq(forward.x) + sq(forward.y));
    if (length == 0.0f) {
        m_fCamFrontXNorm = 1.0f;
    } else {
        const float inverseLength = 1.0f / length;
        m_fCamFrontXNorm *= inverseLength;
        m_fCamFrontYNorm *= inverseLength;
    }
}

// 0x516B20
void CCamera::ImproveNearClip(CVehicle* vehicle, CPed* ped, CVector* source, CVector* targPosn) {
    static auto& longDistance = StaticRef<float>(0x8CCD08);
    static auto& longDistanceScale = StaticRef<float>(0x8CCD04);
    static auto& aircraftCollisionThreshold = StaticRef<float>(0x8CCD00);
    static auto& groundDistanceThreshold = StaticRef<float>(0x8CCCFC);
    static auto& aircraftCollisionScale = StaticRef<float>(0x8CCCF8);
    static auto& aircraftDistanceScale = StaticRef<float>(0x8CCCF4);
    static auto& heliNearClip = StaticRef<float>(0x8CCCF0);
    static auto& waterDistanceThreshold = StaticRef<float>(0x8CCCEC);
    static auto& waterNearClip = StaticRef<float>(0x8CCCE8);
    static auto& underwaterNearClip = StaticRef<float>(0x8CCCE4);
    static auto& flyingPedCollisionScale = StaticRef<float>(0x8CCCE0);
    static auto& flyingPedDistanceScale = StaticRef<float>(0x8CCCDC);
    static auto& specialPieceRadiusScale = StaticRef<float>(0x8CCCD8);
    static auto& minPedNearClip = StaticRef<float>(0x8CCCD4);
    static auto& maxPedNearClip = StaticRef<float>(0x8CCCD0);

    const float distSrcToTarg = (*source - *targPosn).Magnitude();
    if (distSrcToTarg > longDistance) {
        if (const float nearClip = longDistanceScale * gCurDistForCam; RwCameraGetNearClipPlane(Scene.m_pRwCamera) < nearClip) {
            RwCameraSetNearClipPlane(Scene.m_pRwCamera, nearClip);
        }
    }

    if (vehicle) {
        if (vehicle->IsSubHeli() || vehicle->IsSubPlane()) {
            if (gCurDistForCam <= aircraftCollisionThreshold) {
                if (vehicle->IsSubHeli()) {
                    RwCameraSetNearClipPlane(Scene.m_pRwCamera, heliNearClip);
                }
            } else if (GetRoughDistanceToGround() > groundDistanceThreshold) {
                if (const float nearClip = std::min(distSrcToTarg * aircraftDistanceScale, aircraftCollisionScale * gCurDistForCam);
                    RwCameraGetNearClipPlane(Scene.m_pRwCamera) < nearClip
                ) {
                    RwCameraSetNearClipPlane(Scene.m_pRwCamera, nearClip);
                }
            }
        }
    } else if (ped) {
        if (ped->bIsStanding) {
            const float collisionLimit = std::sin(DegreesToRadians(90.0f - TheCamera.GetActiveCam().m_fFOV * 0.5f))
                * gLastRadiusUsedInCollisionPreventionOfCamera;
            auto* const modelInfo = ped->GetModelInfo()->AsPedModelInfoPtr();
            auto* const colModel  = modelInfo->AnimatePedColModelSkinnedWorld(ped->GetRpClump());
            const auto& cam       = TheCamera.m_aCams[m_nActiveCam];
            const auto& front     = cam.m_vecFront;
            const float planeDist = front.Dot(cam.m_vecSource);

            float minDist = 999999.0f;
            for (const auto& sphere : std::span{ colModel->GetData()->m_pSpheres, CPedModelInfo::NUM_PED_COL_NODE_INFOS }) {
                float dist = sphere.m_vecCenter.Dot(front) - planeDist - sphere.m_fRadius;
                if (sphere.m_Surface.m_nPiece == 9) {
                    dist -= specialPieceRadiusScale * sphere.m_fRadius;
                }
                if (dist < minDist) {
                    minDist = dist;
                }
            }
            if (minDist > collisionLimit) {
                minDist = collisionLimit;
            } else if (minDist < minPedNearClip) {
                minDist = minPedNearClip;
            }
            if (minDist > maxPedNearClip) {
                minDist = maxPedNearClip;
            }
            RwCameraSetNearClipPlane(Scene.m_pRwCamera, (float)(int32)(minDist * 100.0f) * 0.01f);
        } else {
            const bool usingParachute = ped->GetIntelligence()->GetUsingParachute();
            if (const auto* swimTask = ped->GetIntelligence()->GetTaskSwim()) {
                const auto swimState = swimTask->m_nSwimState;
                float waterLevel = 0.0f;
                const bool hasWaterLevel = CWaterLevel::GetWaterLevel(source->x, source->y, source->z, waterLevel, false, nullptr);
                if (hasWaterLevel && std::fabs(waterLevel - source->z) < waterDistanceThreshold) {
                    RwCameraSetNearClipPlane(Scene.m_pRwCamera, waterNearClip);
                } else if (swimState == SWIM_UNDERWATER_SPRINTING && TheCamera.m_nPedZoom == 1) {
                    RwCameraSetNearClipPlane(Scene.m_pRwCamera, underwaterNearClip);
                }
            } else if (usingParachute || ped->GetIntelligence()->GetTaskJetPack()) {
                if (GetRoughDistanceToGround() > groundDistanceThreshold) {
                    if (const float nearClip = std::min(distSrcToTarg * flyingPedDistanceScale, flyingPedCollisionScale * gCurDistForCam);
                        nearClip > RwCameraGetNearClipPlane(Scene.m_pRwCamera)
                    ) {
                        RwCameraSetNearClipPlane(Scene.m_pRwCamera, nearClip);
                    }
                }
            }
        }
    }

    float nearest = 999999.0f;
    CCollision::CheckPeds(*source, m_aCams[m_nActiveCam].m_vecFront, nearest);
}

static auto& preMirrorMat = StaticRef<CMatrix>(0xB6FE40);

// 0x51A560
void CCamera::SetCameraUpForMirror() {
    preMirrorMat = m_mCameraMatrix;
    m_mCameraMatrix = m_mMatMirror;
    CopyCameraMatrixToRWCam(true);
    CalculateDerivedValues(true, false);
}

// 0x51A5A0
void CCamera::RestoreCameraAfterMirror() {
    SetMatrix(preMirrorMat);
    CopyCameraMatrixToRWCam(true);
    CalculateDerivedValues(false, false);
}

// 0x51A5D0
bool CCamera::ConeCastCollisionResolve(const CVector& pos, const CVector& lookAt, CVector& outDest, float radius, float minDist, float& outDist) {
    if (pos == lookAt) {
        return false;
    }

    if (CCollision::CameraConeCastVsWorldCollision(CSphere{ lookAt, radius }, CSphere{ pos, radius }, outDist, minDist)) {
        outDest = lerp(lookAt, pos, outDist);
        return true;
    } else {
        outDest = pos;
        outDist = 1.f;
        return false;
    }
}

// 0x51E560
bool CCamera::TryToStartNewCamMode(int32 camSequence) {
    if (camSequence < 0) {
        return false;
    }

    const auto player = FindPlayerPed();
    const auto playerEntity = FindPlayerEntity();
    const auto playerVehicle = FindPlayerVehicle();
    const auto playerPos = [&] {
        return FindPlayerCoors();
    };
    const auto playerSpeed = [&] {
        auto speed = FindPlayerSpeed();
        speed.z = 0.0f;
        speed.Normalise();
        return speed;
    };
    const auto isBoatExceptSkimmer = [&] {
        return playerVehicle && playerVehicle->IsBoat() && m_pTargetEntity->GetModelIndex() != MODEL_SKIMMER;
    };
    const auto isUnderwater = [] {
        const auto& source = TheCamera.GetActiveCam().m_vecSource;
        float level{};
        return CWaterLevel::GetWaterLevel(source, level, true) && level >= source.z;
    };
    const auto setFixed = [&](const CVector& source) {
        SetCamPositionForFixedMode(source, CVector{});
        TakeControl(playerEntity, MODE_FIXED, eSwitchType::JUMPCUT, 2);
        return !isUnderwater();
    };
    const auto findGroundOrRoof = [](CVector& position, float offset) {
        bool found{};
        auto height = CWorld::FindGroundZFor3DCoord(CVector{position.x, position.y, position.z + 5.0f}, &found);
        if (!found) {
            height = CWorld::FindRoofZFor3DCoord(position.x, position.y, position.z - 5.0f, &found);
        }
        if (found) {
            position.z = height + offset;
        }
    };
    const auto dotXY = [](const CVector& lhs, const CVector& rhs) {
        return lhs.x * rhs.x + lhs.y * rhs.y;
    };

    switch (camSequence) {
    case 0: { // Wheel camera
        if (!playerVehicle || (isBoatExceptSkimmer() || playerVehicle->GetModelIndex() == MODEL_RHINO)) {
            return false;
        }
        const auto target = playerVehicle->GetMatrix().TransformVector(CVector{-1.4f, -2.3f, 0.3f}) + playerVehicle->GetPosition();
        if (!CWorld::GetIsLineOfSightClear(playerVehicle->GetPosition(), target, true, false, false, false, false, false, false)) {
            return false;
        }
        TakeControl(playerVehicle, MODE_WHEELCAM, eSwitchType::JUMPCUT, 2);
        return true;
    }
    case 1:
    case 2:
    case 3:
    case 5: {
        if ((camSequence == 1 || camSequence == 2) && isBoatExceptSkimmer()) {
            return false;
        }

        const auto speed = playerSpeed();
        auto cameraPosition = playerPos();
        const auto distance = camSequence == 1 ? 20.0f : camSequence == 2 ? 16.0f : 30.0f;
        const auto side = camSequence == 1 ? 3.0f : camSequence == 2 ? 2.5f : camSequence == 3 ? 8.0f : -6.0f;
        cameraPosition += speed * distance;
        cameraPosition += CVector{speed.y, -speed.x, 0.0f} * side;

        if (camSequence == 3) {
            cameraPosition.z += 16.0f;
        } else {
            findGroundOrRoof(cameraPosition, camSequence == 1 ? 1.5f : camSequence == 2 ? 0.5f : 3.5f);
        }
        if (!CWorld::GetIsLineOfSightClear(playerPos(), cameraPosition, true, false, false, false, false, false, false)) {
            return false;
        }

        auto forward = playerPos() - cameraPosition;
        forward.z = 0.0f;
        const auto horizontalDistance = forward.Magnitude();
        const auto maxDistance = camSequence == 1 ? 40.0f : camSequence == 2 ? 29.0f : 0.0f;
        if (maxDistance > 0.0f && horizontalDistance > maxDistance && DotProduct(FindPlayerSpeed(), forward) > 0.0f) {
            return false;
        }
        const auto minDistance = camSequence == 1 ? 4.5f : camSequence == 2 ? 2.0f : 0.0f;
        if (minDistance > 0.0f && horizontalDistance < minDistance) {
            return false;
        }
        const auto started = setFixed(cameraPosition);
        if (camSequence == 1 || camSequence == 2 || (camSequence == 3 && started)) {
            RwCameraSetNearClipPlane(Scene.m_pRwCamera, 0.15f);
        }
        return started;
    }
    case 6:
        TakeControl(playerEntity, MODE_1STPERSON, eSwitchType::JUMPCUT, 2);
        return true;
    case 7:
    case 8: { // Cop-car chase and wheel chase
        if (!player || !playerVehicle || isBoatExceptSkimmer() || player->m_pWanted->GetWantedLevel() < 1) {
            return false;
        }

        const auto playerPosition = playerPos();
        auto* pool = GetVehiclePool();
        for (auto i = pool->GetSize() - 1; i >= 0; --i) {
            auto* candidate = pool->GetAt(i);
            if (!candidate) {
                continue;
            }
            auto& vehicle = *candidate;
            if ((camSequence == 7 && vehicle.GetStatus() != STATUS_PHYSICS) || &vehicle == playerVehicle || !vehicle.IsAutomobile() || !vehicle.vehicleFlags.bIsLawEnforcer) {
                continue;
            }
            const auto delta = vehicle.GetPosition() - playerPosition;
            if (delta.Magnitude() >= 30.0f) {
                continue;
            }
            if (dotXY(delta, playerVehicle->GetForward()) >= 0.0f
                || dotXY(vehicle.GetForward(), playerVehicle->GetForward()) <= 0.8f) {
                continue;
            }
            if (camSequence == 8) {
                const auto wheelTarget = vehicle.GetMatrix().TransformVector(CVector{-1.4f, -2.3f, 0.3f}) + vehicle.GetPosition();
                if (!CWorld::GetIsLineOfSightClear(vehicle.GetPosition(), wheelTarget, true, false, false, false, false, false, false)) {
                    return false;
                }
                TakeControl(&vehicle, MODE_WHEELCAM, eSwitchType::JUMPCUT, 2);
            } else {
                TakeControl(&vehicle, MODE_CAM_ON_A_STRING, eSwitchType::JUMPCUT, 2);
            }
            if (!isUnderwater()) {
                return true;
            }
        }
        return false;
    }
    case 0xF: {
        if (!playerVehicle) {
            return false;
        }
        auto cameraPosition = playerPos() + playerSpeed() * 34.0f;
        cameraPosition.z = playerPos().z + 0.5f + (playerVehicle->IsBoat() ? 1.0f : 0.0f);
        if (!CWorld::GetIsLineOfSightClear(playerPos(), cameraPosition, true, false, false, false, false, false, false)) {
            return false;
        }
        const auto distance = (playerPos() - cameraPosition).Magnitude();
        if (distance > 44.0f && dotXY(FindPlayerSpeed(), playerPos() - cameraPosition) > 0.0f) {
            return false;
        }
        if (distance < 3.0f) {
            return false;
        }
        return setFixed(cameraPosition);
    }
    case 0x10:
    case 0x11:
    case 0x12:
    case 0x13: {
        if ((camSequence == 0x10 || camSequence == 0x11) && !playerVehicle) {
            return false;
        }
        auto speed = FindPlayerSpeed();
        speed.z = 0.0f;
        if (camSequence == 0x10 || camSequence == 0x11) {
            speed.Normalise();
        }
        const auto angle = CGeneral::GetATanOfXY(speed.x, speed.y)
            + DegreesToRadians(camSequence == 0x10 ? 60.0f : camSequence == 0x11 ? 190.0f : camSequence == 0x12 ? 145.0f : 28.0f);
        speed += CVector{std::cos(angle), std::sin(angle), 0.0f};
        speed.Normalise();

        auto cameraPosition = playerPos();
        cameraPosition += speed * (camSequence == 0x10 ? 30.0f : camSequence == 0x11 ? 25.0f : camSequence == 0x12 ? 15.0f : 12.5f);
        if (camSequence == 0x12) {
            cameraPosition.z += playerVehicle && playerVehicle->IsBoat() ? 23.0f : -23.0f;
        } else if (camSequence == 0x13) {
            cameraPosition.z += playerVehicle && playerVehicle->IsBoat() ? 4.0f : -1.0f;
        } else {
            cameraPosition.z += camSequence == 0x10 ? -5.5f : -1.0f;
        }

        bool foundGround{};
        if (camSequence == 0x10 || camSequence == 0x11) {
            const auto ground = CWorld::FindRoofZFor3DCoord(cameraPosition.x, cameraPosition.y, cameraPosition.z, &foundGround);
            if (foundGround) {
                cameraPosition.z = ground + 0.5f;
            }
        } else {
            const auto ground = CWorld::FindGroundZFor3DCoord(cameraPosition, &foundGround);
            foundGround = ground == 1.0f;
            if (foundGround && cameraPosition.z < ground) {
                cameraPosition.z = ground + 0.5f;
            }
        }
        if (!foundGround) {
            float water{};
            const auto offset = StaticRef<float>(playerVehicle && playerVehicle->IsBoat() ? 0x8CC8C4 : 0x8CC8C0);
            if (CWaterLevel::GetWaterLevelNoWaves(cameraPosition, &water) && cameraPosition.z < water + offset) {
                cameraPosition.z = water + offset;
            }
        }
        if (!CWorld::GetIsLineOfSightClear(playerPos(), cameraPosition, true, false, false, false, false, false, false)) {
            return false;
        }

        const auto distance = (playerPos() - cameraPosition).Magnitude();
        const auto maxDistance = camSequence == 0x10 || camSequence == 0x11 ? 50.0f : camSequence == 0x12 ? 57.0f : 36.0f;
        if (distance > maxDistance && (camSequence != 0x11 || dotXY(FindPlayerSpeed(), playerPos() - cameraPosition) > 0.0f)) {
            return false;
        }
        const auto minDistance = camSequence == 0x10 ? 3.0f : camSequence == 0x11 ? 2.0f : camSequence == 0x12 ? 1.0f : 2.0f;
        if (distance < minDistance) {
            return false;
        }
        return setFixed(cameraPosition);
    }
    case 0x14:
    case 0x15:
    case 0x16:
    case 0x17:
    case 0x1A:
    case 0x1B:
    case 0x1C: {
        auto& cam = GetActiveCam();
        const auto started = [&] {
            switch (camSequence) {
            case 0x14: return cam.Process_DW_HeliChaseCam(true);
            case 0x15: return cam.Process_DW_CamManCam(true);
            case 0x16: return cam.Process_DW_BirdyCam(true);
            case 0x17: return cam.Process_DW_PlaneSpotterCam(true);
            case 0x1A: return cam.Process_DW_PlaneCam1(true);
            case 0x1B: return cam.Process_DW_PlaneCam2(true);
            default:   return cam.Process_DW_PlaneCam3(true);
            }
        }();
        if (!started) {
            return false;
        }
        TakeControl(playerEntity, static_cast<eCamMode>(camSequence + 0x24), eSwitchType::JUMPCUT, 2);
        return !isUnderwater();
    }
    case 0x18:
    case 0x19:
        TheCamera.m_bUseNearClipScript = false;
        return false;
    case 0x1D:
        TakeControl(playerEntity, MODE_CAM_ON_A_STRING, eSwitchType::JUMPCUT, 2);
        return true;
    default:
        return false;
    }
}

// 0x520190
bool CCamera::CameraColDetAndReact(CVector* source, CVector* target) {
    static auto& radiusScale = StaticRef<float>(0x8CCB90);
    static auto& minRadius = StaticRef<float>(0x8CCE18);
    static auto& pedMinDistance = StaticRef<float>(0x8CCE10);
    static auto& aimMinDistance = StaticRef<float>(0x8CCE14);
    static auto& bikeMinDistance = StaticRef<float>(0x8CCE0C);
    static auto& sourceMotionThreshold = StaticRef<float>(0x8CCE08);
    static auto& maxDistanceStep = StaticRef<float>(0x8CCE04);
    static auto& bikeCollisionThreshold = StaticRef<float>(0x8CCE00);
    static auto& bikeNearClip = StaticRef<float>(0x8CCDFC);

    static auto& gCamColLastSrcPos   = StaticRef<CVector>(0xB700DC);
    static auto& gCamColStateFlags   = StaticRef<uint32>(0xB700E8);
    static auto& gCamColMinExtent    = StaticRef<float>(0xB700EC);
    static auto& gCamColCachedModel  = StaticRef<int32>(0xB700F0);

    const CVector delta = *source - *target;
    const float   dist  = delta.Magnitude();
    float         radius     = dist * gpCamColVars[0] * radiusScale;

    const auto    ignoreEntity   = CWorld::pIgnoreEntity;
    const auto*   ignoreVehicle  = (ignoreEntity && ignoreEntity->GetIsTypeVehicle())
        ? ignoreEntity->AsVehicle()
        : nullptr;
    const bool    isIgnoredBike  = gCurCamColVars >= 10 && ignoreVehicle && ignoreVehicle->IsBike();

    if (ignoreEntity && gCurCamColVars >= 10) {
        float fVar;
        if (ignoreVehicle && ignoreVehicle->IsSubAutomobile()) {
            if (gCamColCachedModel != ignoreEntity->GetModelIndex()) {
                gCamColMinExtent = 100.0f;
                if (const auto* colData = ignoreEntity->GetColModel()->GetData()) {
                    for (int32 i = 0; i < colData->m_nNumSpheres; i++) {
                        const auto& sphere = colData->m_pSpheres[i];
                        gCamColMinExtent   = std::min(gCamColMinExtent, sphere.m_vecCenter.z - sphere.m_fRadius);
                    }
                }
                gCamColCachedModel = ignoreEntity->GetModelIndex();
            }
            if (!ignoreEntity->m_matrix) {
                ignoreEntity->AllocateMatrix();
                ignoreEntity->m_placement.UpdateMatrix(ignoreEntity->m_matrix);
            }
            const auto& entityMat = ignoreEntity->GetMatrix();
            fVar = (*target - entityMat.GetPosition()).Dot(entityMat.GetUp()) - gCamColMinExtent;
            if (fVar < 0.2f) {
                fVar = 0.2f;
            }
        } else {
            const auto& boundingBox = ignoreEntity->GetColModel()->m_boundBox;
            fVar = std::min({ (boundingBox.m_vecMax.x - boundingBox.m_vecMin.x) * 0.5f,
                              (boundingBox.m_vecMax.y - boundingBox.m_vecMin.y) * 0.5f,
                              (boundingBox.m_vecMax.z - boundingBox.m_vecMin.z) * 0.5f });
        }
        radius = (fVar > gpCamColVars[1]) ? std::min(radius, gpCamColVars[1])
                                          : std::min(radius, fVar);
    }
    radius = std::max(std::min(radius, gpCamColVars[1]), minRadius);

    float minDist = gpCamColVars[2];
    if (gCurCamColVars < 10) {
        minDist = (gCurCamColVars < 4 ? pedMinDistance : aimMinDistance) / dist;
    }
    if (isIgnoredBike) {
        minDist = bikeMinDistance;
    }
    CVector solvePos{};

    gLastRadiusUsedInCollisionPreventionOfCamera = radius;

    float outDist = 1.0f;
    const bool collided = ConeCastCollisionResolve(*source, *target, solvePos, radius, minDist, outDist);
    if (collided) {
        if (outDist <= gpCamColVars[3]) {
            RwCameraSetNearClipPlane(Scene.m_pRwCamera, gpCamColVars[4]);
        }
    }

    if (outDist >= gCurDistForCam) {
        if (!(gCamColStateFlags & 1)) {
            gCamColStateFlags |= 1;
            gCamColLastSrcPos = CVector{};
        }
        if ((*source - gCamColLastSrcPos).SquaredMagnitude() > sourceMotionThreshold * sourceMotionThreshold) {
            gCurDistForCam += std::min(CTimer::GetTimeStep() * gpCamColVars[5] * (outDist - gCurDistForCam), maxDistanceStep);
        }
        gCamColLastSrcPos = *source;
    } else {
        gCurDistForCam = outDist;
    }
    if (gCurDistForCam > 1.0f) {
        gCurDistForCam = 1.0f;
    }

    *source = *target + delta * gCurDistForCam;

    if (isIgnoredBike && gCurDistForCam < bikeCollisionThreshold) {
        RwCameraSetNearClipPlane(Scene.m_pRwCamera, bikeNearClip);
    }
    return collided;
}

// 0x527FA0
void CCamera::CamControl() {
    auto& requestedMode = StaticRef<eCamMode>(0xB70140);
    auto& cinematicProcessed = StaticRef<bool>(0xB6EC34);
    auto& activeCam = GetActiveCam();
    auto& otherCam = m_aCams[(m_nActiveCam + 1) % 2];
    const auto previousMode = activeCam.m_nMode;
    auto* pad = CPad::GetPad(0);
    auto* player = FindPlayerPed();
    bool jumpCut = false;
    bool stairs = false;
    bool cameOutOfArrest = false;
    m_bObbeCinematicPedCamOn = false;
    m_bObbeCinematicCarCamOn = false;
    m_bUseSpecialFovTrain = false;
    m_bUseTransitionBeta = false;
    m_bJustCameOutOfGarage = false;
    m_bTargetJustCameOffTrain = false;
    m_bInATunnelAndABigVehicle = false;
    m_bJustJumpedOutOf1stPersonBecauseOfTarget = false;
    cinematicProcessed = false;
    if (!activeCam.m_pCamTargetEntity && !m_pTargetEntity) {
        CEntity::ChangeEntityReference(m_pTargetEntity, FindPlayerPed());
    }
    if (++m_nZoneCullFrameNumWereAt > m_nCheckCullZoneThisNumFrames) {
        m_nZoneCullFrameNumWereAt = 1;
    }
    m_bCullZoneChecksOn = m_nZoneCullFrameNumWereAt == m_nCheckCullZoneThisNumFrames;
    if (m_bCullZoneChecksOn) {
        m_bFailedCullZoneTestPreviously = CCullZones::CamCloseInForPlayer();
    }
    if (m_bLookingAtPlayer) {
        pad->DisablePlayerControls &= ~1u;
        player->bIsVisible = true;
    }
    const auto containsMode = [](eCamMode mode, std::initializer_list<eCamMode> modes) {
        return std::find(modes.begin(), modes.end(), mode) != modes.end();
    };
    const auto approach = [](float value, float target) {
        const float step = CTimer::GetTimeStep() * 0.12f;
        return value < target ? std::min(value + step, target) : std::max(value - step, target);
    };
    const auto setCam = [&](eCamMode mode, bool lookingAtVector) {
        auto& cam = GetActiveCam();
        cam.m_nMode = mode;
        cam.m_bResetStatics = true;
        cam.m_vecCamFixedModeVector = m_vecFixedModeVector;
        CEntity::ChangeEntityReference(cam.m_pCamTargetEntity, m_pTargetEntity);
        cam.m_vecCamFixedModeSource = m_vecFixedModeSource;
        cam.m_vecCamFixedModeUpOffSet = m_vecFixedModeUpOffSet;
        cam.m_bCamLookingAtVector = lookingAtVector;
        cam.m_vecLastAboveWaterCamPosition = otherCam.m_vecLastAboveWaterCamPosition;
        m_bJust_Switched = true;
        m_fCarZoomSmoothed = m_fCarZoomBase;
        m_fPedZoomSmoothed = m_fPedZoomBase;
    };
    const auto garageCamera = [&](bool vehicle, CAttributeZone* zone) {
        const auto position = m_pTargetEntity->GetPosition();
        const bool inGarageZone = CGarages::IsPointInAGarageCameraZone(position);
        auto* garage = m_pToGarageWeAreIn;
        const bool canSet = vehicle ? ((!m_bGarageFixedCamPositionSet && m_bLookingAtPlayer) || m_nWhoIsInControlOfTheCamera == 2)
                                   : (!m_bGarageFixedCamPositionSet && m_bLookingAtPlayer);
        if (!(inGarageZone || zone)) {
            if (m_bPlayerIsInGarage) {
                m_bJustCameOutOfGarage = true;
                m_bPlayerIsInGarage = false;
            }
            m_bGarageFixedCamPositionSet = false;
            if (vehicle) {
                requestedMode = MODE_CAM_ON_A_STRING;
            }
            return;
        }
        if (canSet && (garage || zone)) {
            CObject* firstDoor{};
            CObject* secondDoor{};
            CVector center{};
            CVector source = activeCam.m_vecSource;
            CVector direction{};
            if (garage) {
                garage->FindDoorsWithGarage(&firstDoor, &secondDoor);
                center = {(garage->m_fLeftCoord + garage->m_fRightCoord) * 0.5f,
                          (garage->m_fFrontCoord + garage->m_fBackCoord) * 0.5f, 0.0f};
                const auto door = firstDoor ? firstDoor : secondDoor;
                direction = (door ? door->GetPosition() : position) - center;
                direction.z = 0.0f;
                direction.Normalise();
            } else {
                const auto& bounds = zone->zoneDef;
                center = {static_cast<float>(bounds.m_cornerX) + (bounds.m_vec1X + bounds.m_vec2X) * 0.5f,
                          static_cast<float>(bounds.m_cornerY) + (bounds.m_vec1Y + bounds.m_vec2Y) * 0.5f, 0.0f};
                if (vehicle) {
                    center = position;
                }
                direction = position - center;
                direction.z = 0.0f;
                direction.Normalise();
                if (!vehicle || (position - source).Magnitude2D() > 15.0f) {
                    const auto extent = std::max(std::abs(bounds.m_vec1X) + std::abs(bounds.m_vec2X),
                                                 std::abs(bounds.m_vec1Y) + std::abs(bounds.m_vec2Y));
                    auto candidate = position + direction * (2.0f * extent);
                    if (CWorld::GetIsLineOfSightClear(position, candidate, true, false, false, false, false, false, true)) {
                        source = candidate;
                    } else {
                        candidate = position - direction * (2.0f * extent);
                        if (CWorld::GetIsLineOfSightClear(position, candidate, true, false, false, false, false, false, true)) {
                            source = candidate;
                        }
                    }
                }
            }
            if (vehicle) {
                CVector base = position;
                if (firstDoor && secondDoor) {
                    base = (firstDoor->m_pDummyObject->GetPosition() + secondDoor->m_pDummyObject->GetPosition()) * 0.5f;
                    direction = secondDoor->m_pDummyObject->GetMatrix().GetRight();
                } else if (firstDoor || secondDoor) {
                    base = (firstDoor ? firstDoor : secondDoor)->m_pDummyObject->GetPosition();
                    direction = (firstDoor ? firstDoor : secondDoor)->m_pDummyObject->GetMatrix().GetRight();
                } else {
                    direction = position - center;
                    direction.z = 0.0f;
                    direction.Normalise();
                }
                source = base + direction * StaticRef<float>(0x8CCF1C);
                source.z += StaticRef<float>(0x8CCF18);
                SetCamPositionForFixedMode(source, CVector{});
            } else {
                auto fromCenter = source - center;
                fromCenter.z = 0.0f;
                fromCenter.Normalise();
                if (garage) {
                    const auto door = firstDoor ? firstDoor : secondDoor;
                    source = (door ? door->GetPosition() : position) + direction * 13.0f;
                } else {
                    const auto& bounds = zone->zoneDef;
                    const auto extent = std::max(std::abs(bounds.m_vec1X) + std::abs(bounds.m_vec2X),
                                                 std::abs(bounds.m_vec1Y) + std::abs(bounds.m_vec2Y));
                    source = center + fromCenter * (extent * 0.7f + 3.75f);
                }
                bool found{};
                auto ground = CWorld::FindGroundZFor3DCoord(position, &found);
                if (!found) {
                    ground = position.z - 0.2f;
                }
                if (m_nPedZoom != 4 || zone) {
                    source.z = ground + 3.1f;
                } else {
                    source = center;
                    source.z = std::min(position.z + center.z + 2.1f, garage->m_fRightCoord);
                }
                SetCamPositionForFixedMode(source, CVector{});
                if (garage) {
                    CVector base = position;
                    if (firstDoor && secondDoor) {
                        base = (firstDoor->m_pDummyObject->GetPosition() + secondDoor->m_pDummyObject->GetPosition()) * 0.5f;
                        direction = secondDoor->m_pDummyObject->GetMatrix().GetRight();
                    } else if (firstDoor || secondDoor) {
                        base = (firstDoor ? firstDoor : secondDoor)->m_pDummyObject->GetPosition();
                        direction = (firstDoor ? firstDoor : secondDoor)->m_pDummyObject->GetMatrix().GetRight();
                    } else {
                        direction = position - center;
                        direction.z = 0.0f;
                        direction.Normalise();
                    }
                    source = base + direction * StaticRef<float>(0x8CCF10);
                    source.z += StaticRef<float>(0x8CCF0C);
                    SetCamPositionForFixedMode(source, CVector{});
                }
            }
            m_bGarageFixedCamPositionSet = true;
        }
        const bool controlled = vehicle ? (m_bLookingAtPlayer || m_nWhoIsInControlOfTheCamera == 2) : m_bLookingAtPlayer;
        if ((CGarages::CameraShouldBeOutside() || zone) && m_bGarageFixedCamPositionSet && controlled) {
            if (garage || zone) {
                requestedMode = MODE_FIXED;
                m_bPlayerIsInGarage = true;
            }
        } else {
            if (m_bPlayerIsInGarage) {
                m_bJustCameOutOfGarage = true;
                m_bPlayerIsInGarage = false;
            }
            requestedMode = vehicle ? MODE_CAM_ON_A_STRING : MODE_FOLLOWPED;
        }
    };
    if (!CTimer::GetIsPaused() && !m_bIdleOn) {
        if (m_bTargetJustBeenOnTrain && (!m_pTargetEntity->IsVehicle() || !m_pTargetEntity->AsVehicle()->IsTrain())) {
            Restore();
            m_bTargetJustBeenOnTrain = false;
            m_bTargetJustCameOffTrain = true;
            m_bWantsToSwitchWidescreenOff = m_bWideScreenOn;
        }
        if (m_pTargetEntity->IsVehicle()) {
            auto* vehicle = m_pTargetEntity->AsVehicle();
            auto& forcedMode = StaticRef<int32>(0x8CC824);
            if (forcedMode > 0) {
                requestedMode = static_cast<eCamMode>(forcedMode);
                forcedMode = -1;
            }
            if (vehicle->IsTrain()) {
                requestedMode = MODE_BEHINDCAR;
            } else {
                const bool cycleUp = pad->CycleCameraModeJustDown();
                if ((cycleUp || pad->sub_5404F0()) && CReplay::Mode != static_cast<eReplayMode>(1) && !m_bWideScreenOn
                    && !m_bFailedCullZoneTestPreviously && (m_bLookingAtPlayer || m_nWhoIsInControlOfTheCamera == 2)
                    && !CGameLogic::IsCoopGameGoingOn()) {
                    int32 zoom = static_cast<int32>(m_nCarZoom) + (cycleUp ? -1 : 1);
                    if (zoom > 5) zoom = 0;
                    if (zoom < 0) zoom = 5;
                    if (zoom == 4) zoom = cycleUp ? 3 : 5;
                    else if (zoom == 0 && m_bDisableFirstPersonInCar) zoom = cycleUp ? 5 : 1;
                    m_nCarZoom = zoom;
                }
                if (m_bFailedCullZoneTestPreviously && m_nCarZoom != 4 && m_nCarZoom != 0) {
                    requestedMode = MODE_CAM_ON_A_STRING;
                }
                const auto type = vehicle->m_nVehicleType;
                if (type == VEHICLE_TYPE_BOAT && vehicle->GetModelIndex() != MODEL_SKIMMER) {
                    requestedMode = MODE_BEHINDBOAT;
                } else if (type == VEHICLE_TYPE_AUTOMOBILE || type == VEHICLE_TYPE_BIKE || vehicle->GetModelIndex() == MODEL_SKIMMER) {
                    auto* zone = type == VEHICLE_TYPE_BIKE && CCullZones::CamStairsForPlayer()
                        ? CCullZones::FindZoneWithStairsAttributeForPlayer() : nullptr;
                    stairs = zone != nullptr;
                    garageCamera(true, zone);
                }
                int32 arrayIndex{};
                GetArrPosForVehicleType(static_cast<eVehicleType>(vehicle->GetVehicleAppearance()), arrayIndex);
                if (m_nCarZoom == 0 && !m_bPlayerIsInGarage) {
                    requestedMode = MODE_1STPERSON;
                    m_fCarZoomBase = 0.0f;
                } else if (m_nCarZoom >= 1 && m_nCarZoom <= 3) {
                    m_fCarZoomBase = StaticRef<float[5]>(0x8CC3E0 + (m_nCarZoom - 1) * 20)[arrayIndex];
                }
                if (m_nCarZoom == 4 && !m_bPlayerIsInGarage) {
                    m_fCarZoomBase = 1.0f;
                }
                if (m_fCarZoomTotal == 0.0f) {
                    m_fCarZoomTotal = m_fCarZoomBase;
                }
                float closeIn = 0.0f;
                if (m_bUseScriptZoomValueCar) {
                    m_fCarZoomSmoothed = approach(m_fCarZoomSmoothed, m_fCarZoomValueScript);
                } else if (m_bFailedCullZoneTestPreviously) {
                    closeIn = 0.65f;
                    m_fCarZoomSmoothed = approach(m_fCarZoomSmoothed, -0.65f);
                } else {
                    m_fCarZoomSmoothed = approach(m_fCarZoomSmoothed, m_fCarZoomBase);
                    if (m_nCarZoom == 3 && m_fCarZoomBase == 0.0f) m_fCarZoomSmoothed = m_fCarZoomBase;
                }
                WellBufferMe(closeIn, activeCam.m_fCloseInCarHeightOffset, activeCam.m_fCloseInCarHeightOffsetSpeed, 0.1f, 0.25f, false);
            }
        } else if (m_pTargetEntity->IsPed()) {
            if ((pad->CycleCameraModeJustDown() || pad->sub_5404F0()) && CReplay::Mode != static_cast<eReplayMode>(1)
                && !m_bWideScreenOn && !m_bFailedCullZoneTestPreviously && !m_bFirstPersonBeingUsed
                && (m_bLookingAtPlayer || m_nWhoIsInControlOfTheCamera == 2) && !CGameLogic::IsCoopGameGoingOn()) {
                int32 zoom = static_cast<int32>(m_nPedZoom) + (pad->CycleCameraModeJustDown() ? -1 : 1);
                m_nPedZoom = zoom > 3 ? 1 : zoom < 1 ? 3 : zoom;
            }
            requestedMode = MODE_FOLLOWPED;
            if ((m_bLookingAtPlayer || m_bEnable1rstPersonCamCntrlsScript) && (!m_bWideScreenOn || m_bEnable1rstPersonCamCntrlsScript)) {
                if (m_aCams[0].Using3rdPersonMouseCam()) {
                    m_bFirstPersonBeingUsed = false;
                } else {
                    if (!player->GetTaskManager().GetTaskSecondary(TASK_SECONDARY_ATTACK) && !pad->LookAroundLeftRightOnPC()) {
                        pad->LookAroundUpDownOnPC();
                    }
                    if (m_bFirstPersonBeingUsed) {
                        const auto& state = pad->NewState;
                        if (pad->GetPedWalkLeftRight() || pad->GetPedWalkUpDown() || state.ButtonSquare || state.ButtonTriangle
                            || state.ButtonCross || state.ButtonCircle || state.Select
                            || static_cast<double>(CTimer::GetTimeInMS() - m_nFirstPersonCamLastInputTime) > 2850.0) {
                            m_bFirstPersonBeingUsed = false;
                        } else if (pad->GetEnterTargeting()) {
                            m_bJustJumpedOutOf1stPersonBecauseOfTarget = true;
                            m_bFirstPersonBeingUsed = false;
                        }
                    }
                }
            } else {
                m_bFirstPersonBeingUsed = false;
            }
            if (!player->IsPedInControl() || player->GetPlayerData()->m_fMoveBlendRatio > 0.0f) {
                m_bFirstPersonBeingUsed = false;
            }
            if (m_bFirstPersonBeingUsed) {
                requestedMode = MODE_1STPERSON;
                pad->DisablePlayerControls |= 1;
            }
            m_fPedZoomBase = m_nPedZoom == 1 ? activeCam.m_fTargetZoomGroundOne
                : m_nPedZoom == 3 ? activeCam.m_fTargetZoomGroundThree : activeCam.m_fTargetZoomGroundTwo;
            float closeIn = 0.0f;
            if (m_bUseScriptZoomValuePed) {
                m_fPedZoomSmoothed = approach(m_fPedZoomSmoothed, m_fPedZoomValueScript);
            } else if (m_bFailedCullZoneTestPreviously) {
                closeIn = 0.7f;
                m_fPedZoomSmoothed = approach(m_fPedZoomSmoothed, StaticRef<float>(0x8CCF14));
            } else {
                m_fPedZoomSmoothed = approach(m_fPedZoomSmoothed, m_fPedZoomBase);
                if (m_nPedZoom == 3 && m_fPedZoomBase == 0.0f) m_fPedZoomSmoothed = m_fPedZoomBase;
            }
            WellBufferMe(closeIn, activeCam.m_fCloseInPedHeightOffset, activeCam.m_fCloseInPedHeightOffsetSpeed, 0.1f, 0.025f, false);
            auto* zone = CCullZones::CamStairsForPlayer() ? CCullZones::FindZoneWithStairsAttributeForPlayer() : nullptr;
            stairs = zone != nullptr;
            garageCamera(false, zone);
            const auto weaponMode = static_cast<eCamMode>(m_PlayerWeaponMode.m_nMode);
            if (!pad->GetTarget() && weaponMode != MODE_NONE && !containsMode(weaponMode, {MODE_HELICANNON_1STPERSON, MODE_AIMWEAPON_FROMCAR, MODE_AIMWEAPON_ATTACHED})
                && (weaponMode != MODE_CAMERA || !player->m_pAttachedTo)) {
                ClearPlayerWeaponMode();
            }
            if (m_PlayerMode.m_nMode) {
                requestedMode = static_cast<eCamMode>(m_PlayerMode.m_nMode);
            }
            const auto currentWeaponMode = static_cast<eCamMode>(m_PlayerWeaponMode.m_nMode);
            if (currentWeaponMode != MODE_NONE && !stairs) {
                const bool mouse = activeCam.GetWeaponFirstPersonOn();
                const bool direct = containsMode(currentWeaponMode, {MODE_SNIPER, MODE_ROCKETLAUNCHER, MODE_ROCKETLAUNCHER_HS, MODE_M16_1STPERSON, MODE_HELICANNON_1STPERSON, MODE_CAMERA});
                if (direct || mouse) {
                    requestedMode = player->m_nPedState != PEDSTATE_SEEK_CAR || requestedMode == MODE_TOP_DOWN_PED || mouse
                        ? currentWeaponMode : MODE_FOLLOWPED;
                } else if (requestedMode != MODE_TOP_DOWN_PED && (player->m_pTargetedObject || player->GetPlayerData()->m_bFreeAiming)) {
                    auto& fixedAim = StaticRef<bool>(0xB7013D);
                    bool dyingTarget = player->m_pTargetedObject && player->m_pTargetedObject->IsPed()
                        && (player->m_pTargetedObject->AsPed()->m_nPedState == PEDSTATE_DEAD
                            || player->m_pTargetedObject->AsPed()->m_nPedState == PEDSTATE_DIE);
                    const auto delta = m_vecAimingTargetCoors - m_pTargetEntity->GetPosition();
                    const float beta = CGeneral::GetATanOfXY(activeCam.m_vecSource.x - m_pTargetEntity->GetPosition().x,
                                                            activeCam.m_vecSource.y - m_pTargetEntity->GetPosition().y);
                    requestedMode = currentWeaponMode;
                    float distance = 0.0f;
                    if (currentWeaponMode == MODE_AIMWEAPON && dyingTarget && player->m_pTargetedObject
                        && (!m_bTransitionState || activeCam.m_nMode == MODE_SPECIAL_FIXED_FOR_SYPHON)) {
                        const float threshold = activeCam.m_nMode == MODE_SPECIAL_FIXED_FOR_SYPHON && player->m_pTargetedObject->IsPed()
                            ? StaticRef<float>(0x8CCF04) : StaticRef<float>(0x8CCF08);
                        if (delta.Magnitude2D() < threshold) {
                            requestedMode = MODE_SPECIAL_FIXED_FOR_SYPHON;
                            distance = 5.6f;
                        }
                    }
                    if (requestedMode == MODE_SPECIAL_FIXED_FOR_SYPHON) {
                        if (!fixedAim) {
                            auto source = m_pTargetEntity->GetPosition() + CVector{std::cos(beta) * distance, std::sin(beta) * distance, 1.15f};
                            CColPoint collision{};
                            CEntity* hit{};
                            if (CWorld::ProcessLineOfSight(m_pTargetEntity->GetPosition(), source, collision, hit, true, false, false, true, false, true, true, false)) {
                                source = collision.m_vecPoint;
                            }
                            SetCamPositionForFixedMode(source, CVector{});
                            fixedAim = true;
                        }
                    } else {
                        fixedAim = false;
                    }
                }
            }
        }
    }
    if (m_bCooperativeCamMode) {
        auto* first = FindPlayerPed(0);
        auto* second = FindPlayerPed(1);
        if (first && second) {
            if (first->bInVehicle && second->bInVehicle && first->m_pVehicle && second->m_pVehicle) {
                m_pTargetEntity = first->m_pVehicle;
                requestedMode = first->m_pVehicle != second->m_pVehicle ? m_nModeForTwoPlayersSeparateCars
                    : m_bAllowShootingWith2PlayersInCar ? m_nModeForTwoPlayersSameCarShootingAllowed : m_nModeForTwoPlayersSameCarShootingNotAllowed;
            } else {
                requestedMode = m_nModeForTwoPlayersNotBothInCar;
            }
        }
    }
    auto& wasArrested = StaticRef<bool>(0xB7013C);
    auto& lastPedState = StaticRef<ePedState>(0xB70138);
    auto& arrestMode = StaticRef<eCamMode>(0xB70134);
    const auto pedState = player->m_nPedState;
    if (pedState == PEDSTATE_ARRESTED) {
        wasArrested = true;
    } else if (wasArrested) {
        cameOutOfArrest = true;
        wasArrested = false;
    }
    const bool enteredArrest = lastPedState != PEDSTATE_ARRESTED && pedState == PEDSTATE_ARRESTED
        && (m_nCarZoom != 0 || !m_pTargetEntity->IsVehicle());
    lastPedState = pedState;
    if (enteredArrest) {
        requestedMode = arrestMode = MODE_ARRESTCAM_ONE;
        activeCam.m_bResetStatics = true;
    } else if (pedState == PEDSTATE_ARRESTED) {
        requestedMode = arrestMode;
    }
    if (pedState == PEDSTATE_DEAD) {
        m_bObbeCinematicCarCamOn = false;
        if (activeCam.m_nMode == MODE_PED_DEAD_BABY || activeCam.m_nMode == MODE_ARRESTCAM_ONE) {
            requestedMode = activeCam.m_nMode;
        } else {
            requestedMode = MODE_PED_DEAD_BABY;
            if (m_pTargetEntity->IsPed()) {
                auto* target = m_pTargetEntity->AsPed();
                for (auto* nearby : target->GetIntelligence()->m_pedScanner.m_apEntities) {
                    if (!nearby) continue;
                    auto* task = nearby->AsPed()->GetTaskManager().FindActiveTaskByType(TASK_COMPLEX_ARREST_PED);
                    if (task && static_cast<CTaskComplexArrestPed*>(task)->m_Ped == player
                        && (nearby->GetPosition() - target->GetPosition()).Magnitude() < 4.0f) {
                        requestedMode = MODE_ARRESTCAM_ONE;
                        break;
                    }
                }
            }
            activeCam.m_bResetStatics = true;
        }
    }
    if (m_bRestoreByJumpCut) {
        if (!containsMode(requestedMode, {MODE_FOLLOWPED, MODE_M16_1STPERSON, MODE_SNIPER, MODE_ROCKETLAUNCHER, MODE_ROCKETLAUNCHER_HS,
                MODE_CAMERA, MODE_SYPHON, MODE_SYPHON_CRIM_IN_FRONT, MODE_SPECIAL_FIXED_FOR_SYPHON, MODE_CAM_ON_A_STRING, MODE_BEHINDCAR})
            && !m_bUseMouse3rdPerson) {
            SetCameraDirectlyBehindForFollowPed_CamOnAString();
        }
        requestedMode = m_nModeToGoTo;
        setCam(requestedMode, false);
        m_bRestoreByJumpCut = false;
        m_bTransitionState = false;
        m_bDoingSpecialInterp = false;
    }
    if (gbModelViewer) requestedMode = MODE_MODELVIEW;
    if (m_pTargetEntity) {
        if (m_pTargetEntity->IsVehicle()) {
            if (m_nCarZoom == 5) m_bObbeCinematicCarCamOn = true;
        } else if (m_nPedZoom == 5) {
            m_bObbeCinematicPedCamOn = true;
        }
    }
    if (const auto vehicle = FindPlayerVehicle(); vehicle && vehicle->IsTrain()) {
        m_bObbeCinematicCarCamOn = true;
    }
    bool cinematicAllowed = true;
    if (m_pTargetEntity && m_pTargetEntity->IsVehicle()) {
        if (pedState == PEDSTATE_ARRESTED || pedState == PEDSTATE_DEAD) {
            m_bObbeCinematicPedCamOn = false;
            cinematicAllowed = false;
            requestedMode = pedState == PEDSTATE_ARRESTED ? MODE_ARRESTCAM_ONE : MODE_PED_DEAD_BABY;
        }
    }
    if (m_bTargetJustBeenOnTrain || containsMode(requestedMode, {MODE_PED_DEAD_BABY, MODE_PLAYER_FALLEN_WATER, MODE_SYPHON_CRIM_IN_FRONT,
            MODE_SYPHON, MODE_SNIPER, MODE_SPECIAL_FIXED_FOR_SYPHON, MODE_ROCKETLAUNCHER, MODE_ROCKETLAUNCHER_HS,
            MODE_ARRESTCAM_ONE, MODE_ARRESTCAM_TWO, MODE_M16_1STPERSON, MODE_FIGHT_CAM, MODE_SNIPER_RUNABOUT,
            MODE_ROCKETLAUNCHER_RUNABOUT, MODE_ROCKETLAUNCHER_RUNABOUT_HS, MODE_M16_1STPERSON_RUNABOUT,
            MODE_FIGHT_CAM_RUNABOUT, MODE_1STPERSON_RUNABOUT, MODE_HELICANNON_1STPERSON, MODE_CAMERA})
        || m_nWhoIsInControlOfTheCamera == 1 || m_bJustCameOutOfGarage || m_bPlayerIsInGarage || activeCam.m_nMode == MODE_PED_DEAD_BABY) {
        cinematicAllowed = false;
    }
    if (m_bCinemaCamera) {
        m_bObbeCinematicCarCamOn = true;
        cinematicAllowed = true;
    }
    bool cinema = cinematicAllowed && (m_bObbeCinematicPedCamOn || m_bObbeCinematicCarCamOn);
    if (!cinema) {
        jumpCut |= m_bPlayerIsInGarage && m_bObbeCinematicCarCamOn;
        bDidWeProcessAnyCinemaCam = false;
    } else if (!m_bObbeCinematicPedCamOn) {
        CPostEffects::m_bSpeedFXUserFlagCurrentFrame = false;
        if (m_pTargetEntity->IsVehicle()) {
            auto* vehicle = m_pTargetEntity->AsVehicle();
            // These cinematic selectors have not yet been reversed.
            if (vehicle->m_nVehicleSubType == VEHICLE_TYPE_PLANE) {
                plugin::CallMethod<0x526C80, CCamera*>(this);
            } else if (vehicle->GetVehicleAppearance() == VEHICLE_APPEARANCE_HELI) {
                plugin::CallMethod<0x526AE0, CCamera*>(this);
            } else if (vehicle->m_nVehicleSubType == VEHICLE_TYPE_BOAT) {
                plugin::CallMethod<0x526E20, CCamera*>(this);
            } else if (vehicle->m_nVehicleSubType == VEHICLE_TYPE_TRAIN) {
                plugin::CallMethod<0x526950, CCamera*>(this);
            } else {
                plugin::CallMethod<0x5267C0, CCamera*>(this);
            }
        }
    }
    const auto oldMode = activeCam.m_nMode;
    const auto firstPersonModes = {MODE_1STPERSON, MODE_SNIPER, MODE_M16_1STPERSON, MODE_ROCKETLAUNCHER, MODE_ROCKETLAUNCHER_HS,
        MODE_SNIPER_RUNABOUT, MODE_ROCKETLAUNCHER_RUNABOUT, MODE_ROCKETLAUNCHER_RUNABOUT_HS, MODE_M16_1STPERSON_RUNABOUT,
        MODE_FIGHT_CAM_RUNABOUT, MODE_1STPERSON_RUNABOUT, MODE_HELICANNON_1STPERSON, MODE_CAMERA};
    if (!m_bLookingAtPlayer) {
        bool forceScriptCut = false;
        bool weaponCut = false;
        if (m_bEnable1rstPersonCamCntrlsScript || m_bAllow1rstPersonWeaponsCamera) {
            if (requestedMode != MODE_1STPERSON) {
                const auto weaponMode = static_cast<eCamMode>(m_PlayerWeaponMode.m_nMode);
                if (containsMode(weaponMode, {MODE_SNIPER, MODE_1STPERSON, MODE_ROCKETLAUNCHER, MODE_ROCKETLAUNCHER_HS})
                    && pad->GetTarget() && m_bAllow1rstPersonWeaponsCamera) {
                    forceScriptCut = weaponCut = true;
                } else if (oldMode != m_nModeToGoTo) {
                    m_bStartInterScript = true;
                    m_nTypeOfSwitch = eSwitchType::JUMPCUT;
                    pad->DisablePlayerControls &= ~1u;
                }
            } else if (oldMode != requestedMode) {
                forceScriptCut = true;
            }
        }
        if (m_bStartInterScript && m_nTypeOfSwitch == eSwitchType::INTERPOLATION) {
            requestedMode = m_nModeToGoTo;
            if (m_bTransitionState) m_bDoingSpecialInterp = true;
            StartTransition(requestedMode);
        } else if ((m_bStartInterScript && m_nTypeOfSwitch == eSwitchType::JUMPCUT) || forceScriptCut) {
            m_bTransitionState = false;
            m_bDoingSpecialInterp = false;
            setCam(m_bEnable1rstPersonCamCntrlsScript && requestedMode == MODE_1STPERSON ? requestedMode
                : weaponCut ? static_cast<eCamMode>(m_PlayerWeaponMode.m_nMode) : m_nModeToGoTo, m_bLookingAtVector);
        }
    } else {
        jumpCut |= containsMode(requestedMode, {MODE_TOPDOWN, MODE_1STPERSON, MODE_TOP_DOWN_PED});
        if (containsMode(requestedMode, {MODE_CAM_ON_A_STRING, MODE_BEHINDBOAT})
            && containsMode(oldMode, {MODE_TOPDOWN, MODE_1STPERSON, MODE_TOP_DOWN_PED})) jumpCut = true;
        if (requestedMode == MODE_FIXED && oldMode == MODE_TOPDOWN) jumpCut = true;
        if (containsMode(requestedMode, {MODE_AIMWEAPON, MODE_AIMWEAPON_FROMCAR, MODE_AIMWEAPON_ATTACHED})
            && m_pTargetEntity && m_pTargetEntity->IsPed()) {
            auto* ped = m_pTargetEntity->AsPed();
            const bool jetpack = ped->GetIntelligence()->GetTaskJetPack() != nullptr;
            if (requestedMode == MODE_AIMWEAPON && oldMode == MODE_FOLLOWPED && !jetpack) {
                float heading = ped->GetHeading();
                if (ped->m_pTargetedObject) {
                    const auto offset = ped->m_pTargetedObject->GetPosition() - ped->GetPosition();
                    heading = std::atan2(offset.y, -offset.x);
                }
                heading -= HALF_PI;
                if (heading > activeCam.m_fHorizontalAngle + PI) heading -= TWO_PI;
                else if (heading < activeCam.m_fHorizontalAngle - PI) heading += TWO_PI;
                if (std::abs(heading - activeCam.m_fHorizontalAngle) > DegreesToRadians(StaticRef<float>(0x8CC46C))
                    || (ped->GetPosition() - m_mCameraMatrix.GetPosition()).Magnitude() > (TheCamera.m_fPedZoomSmoothed + 2.0f) * 1.5f) {
                    jumpCut = true;
                }
                if (m_bUseMouse3rdPerson) jumpCut = false;
            } else {
                jumpCut = true;
            }
        }
        if ((requestedMode == MODE_TWOPLAYER && oldMode != MODE_TWOPLAYER_IN_CAR_AND_SHOOTING)
            || (requestedMode == MODE_TWOPLAYER_IN_CAR_AND_SHOOTING && oldMode != MODE_TWOPLAYER)
            || (oldMode == MODE_TWOPLAYER && requestedMode != MODE_TWOPLAYER_IN_CAR_AND_SHOOTING)
            || (oldMode == MODE_TWOPLAYER_IN_CAR_AND_SHOOTING && requestedMode != MODE_TWOPLAYER)) jumpCut = true;
        if ((requestedMode == MODE_TOPDOWN && oldMode == MODE_TOP_DOWN_PED)
            || (requestedMode == MODE_TOP_DOWN_PED && oldMode == MODE_TOPDOWN) || oldMode == MODE_PED_DEAD_BABY) {
            jumpCut = false;
            if (oldMode == MODE_PED_DEAD_BABY) jumpCut = true;
        } else if ((containsMode(requestedMode, firstPersonModes)
                   || containsMode(requestedMode, {MODE_ARRESTCAM_ONE, MODE_ARRESTCAM_TWO})) && m_pTargetEntity->IsPed()) {
            jumpCut = true;
        } else if (requestedMode == MODE_FIXED && m_bPlayerIsInGarage) {
            if ((containsMode(oldMode, firstPersonModes) || oldMode == MODE_TOP_DOWN_PED || stairs) && m_pTargetEntity && !m_pTargetEntity->IsVehicle()) {
                jumpCut = true;
            }
        } else if (requestedMode == MODE_FOLLOWPED) {
            bool aimCut = false;
            if (oldMode == MODE_AIMWEAPON && m_pTargetEntity->IsPed()) {
                auto* ped = m_pTargetEntity->AsPed();
                if (ped->CanWeRunAndFireWithWeapon() && !ped->bIsDucking) {
                    auto heading = ped->GetHeading() - HALF_PI;
                    if (heading > activeCam.m_fHorizontalAngle + PI) heading -= TWO_PI;
                    else if (heading < activeCam.m_fHorizontalAngle - PI) heading += TWO_PI;
                    aimCut = std::abs(heading - activeCam.m_fHorizontalAngle) > DegreesToRadians(StaticRef<float>(0x8CC46C)) || !ped->bIsStanding;
                    if (m_bUseMouse3rdPerson) {
                        aimCut = false;
                        m_bJustCameOutOfGarage = true;
                    }
                }
            }
            if ((containsMode(oldMode, firstPersonModes) || containsMode(oldMode, {MODE_PED_DEAD_BABY, MODE_ARRESTCAM_ONE,
                    MODE_ARRESTCAM_TWO, MODE_PILLOWS_PAPS, MODE_TOPDOWN, MODE_TOP_DOWN_PED}) || aimCut || cameOutOfArrest)
                && !m_bJustCameOutOfGarage) {
                if (containsMode(oldMode, firstPersonModes)) {
                    auto* ped = m_pTargetEntity->AsPed();
                    ped->m_fCurrentRotation = ped->m_fAimingRotation = CGeneral::GetATanOfXY(activeCam.m_vecFront.x, activeCam.m_vecFront.y) - HALF_PI;
                }
                m_bUseTransitionBeta = true;
                jumpCut = true;
                activeCam.m_fTransitionBeta = oldMode == MODE_TOP_DOWN_PED ? CGeneral::GetATanOfXY(0.001f, 1.0f)
                    : CGeneral::GetATanOfXY(activeCam.m_vecFront.x, activeCam.m_vecFront.y) + PI;
            }
        } else if (containsMode(requestedMode, {MODE_LIGHTHOUSE, MODE_ARRESTCAM_ONE, MODE_ARRESTCAM_TWO, MODE_PED_DEAD_BABY})) {
            jumpCut = true;
        }
        if (requestedMode != oldMode && !activeCam.m_pCamTargetEntity) jumpCut = true;
        if (m_bPlayerIsInGarage) {
            if (m_pToGarageWeAreIn && static_cast<int32>(m_pToGarageWeAreIn->m_nType) >= 2 && static_cast<int32>(m_pToGarageWeAreIn->m_nType) <= 4
                && m_pTargetEntity->IsVehicle() && m_pTargetEntity->GetModelIndex() == MODEL_YANKEE && requestedMode != oldMode) jumpCut = true;
            if (activeCam.m_pCamTargetEntity) {
                const auto target = activeCam.m_pCamTargetEntity->GetPosition();
                if (DotProduct(target - m_vecFixedModeSource, target - activeCam.m_vecSource) < 0.0f) jumpCut = true;
            }
        }
        if (requestedMode != oldMode && jumpCut) {
            if ((!m_bPlayerIsInGarage || m_bJustCameOutOfGarage)
                && !containsMode(requestedMode, {MODE_FOLLOWPED, MODE_M16_1STPERSON, MODE_SNIPER, MODE_ROCKETLAUNCHER,
                    MODE_ROCKETLAUNCHER_HS, MODE_CAMERA, MODE_SYPHON, MODE_1STPERSON, MODE_SYPHON_CRIM_IN_FRONT, MODE_SPECIAL_FIXED_FOR_SYPHON})
                && !m_bUseMouse3rdPerson) SetCameraDirectlyBehindForFollowPed_CamOnAString();
            setCam(requestedMode, m_bLookingAtVector);
            m_bTransitionState = false;
            m_bDoingSpecialInterp = false;
            m_bStartInterScript = false;
        } else if (requestedMode != oldMode && m_bTransitionState) {
            if (!m_bWaitForInterpolToFinish && m_bLookingAtPlayer && m_pTargetEntity && m_pTargetEntity->IsPed()
                && (player->GetPosition() - m_mCameraMatrix.GetPosition()).Magnitude() > 17.5f
                && containsMode(requestedMode, {MODE_SYPHON, MODE_SYPHON_CRIM_IN_FRONT})) m_bWaitForInterpolToFinish = true;
            if (!m_bWaitForInterpolToFinish) {
                m_bDoingSpecialInterp = true;
                StartTransition(requestedMode);
            }
        } else if (requestedMode != oldMode && !m_bWaitForInterpolToFinish) {
            StartTransition(requestedMode);
        } else if (requestedMode == MODE_FIXED && m_pTargetEntity != activeCam.m_pCamTargetEntity && m_bPlayerIsInGarage) {
            if (m_bTransitionState) m_bDoingSpecialInterp = true;
            StartTransition(MODE_FIXED);
        }
    }
    m_bStartInterScript = false;
    if (!GetActiveCam().m_pCamTargetEntity) {
        CEntity::ChangeEntityReference(GetActiveCam().m_pCamTargetEntity, m_pTargetEntity);
    }
    const auto finalMode = GetActiveCam().m_nMode;
    if (finalMode == MODE_FLYBY || (m_pTargetEntity->IsPed() && containsMode(finalMode, {MODE_1STPERSON, MODE_SNIPER, MODE_M16_1STPERSON,
            MODE_CAMERA, MODE_HELICANNON_1STPERSON, MODE_ROCKETLAUNCHER, MODE_ROCKETLAUNCHER_HS}))) {
        if (player->bIsVisible) {
            player->bIsVisible = false;
            if (auto* hold = player->GetIntelligence()->GetTaskHold(false); hold && hold->m_pEntityToHold) hold->m_pEntityToHold->bIsVisible = false;
        }
    } else {
        player->bIsVisible = true;
    }
    if (finalMode == MODE_FIXED) player->bIsVisible = gPlayerPedVisible;
    bool restoredCinema = false;
    if (!cinema && m_nWhoIsInControlOfTheCamera == 2) {
        RestoreWithJumpCut();
        restoredCinema = true;
        m_bCamDirectlyBehind = true;
        if (player) m_fPedOrientForBehindOrInFront = CGeneral::GetATanOfXY(player->GetForward().x, player->GetForward().y);
    }
    if ((previousMode != finalMode || restoredCinema || finalMode == MODE_FOLLOWPED || finalMode == MODE_CAM_ON_A_STRING)
        && pad->sub_540530() && CReplay::Mode != static_cast<eReplayMode>(1)
        && (m_bLookingAtPlayer || m_nWhoIsInControlOfTheCamera == 2) && !m_bWideScreenOn
        && (m_nWhoIsInControlOfTheCamera != 2 || (cinematicProcessed && !pad->DisablePlayerControls))) {
        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_DISPLAY_INFO, 0.0f, 1.0f);
    }
}

// 0x5B24A0
void CCamera::DeleteCutSceneCamDataMemory() {
    for (auto& splines : m_aPathArray) {
        delete splines.m_pArrPathData;
        splines.m_pArrPathData = nullptr;
    }
}

// 0x5B24D0
void CCamera::LoadPathSplines(FILE* file) {
    DeleteCutSceneCamDataMemory();
    int32 pathIndex = -1;
    int32 linesRemaining = 0;
    bool expectCount = true;
    float* output = nullptr;
    for (auto* line = CFileLoader::LoadLine(file); line; line = CFileLoader::LoadLine(file)) {
        if (*line == '#' || *line == '\0') {
            continue;
        }
        if (linesRemaining != 0) {
            --linesRemaining;
            for (auto* token = std::strtok(line, ", \t"); token; token = std::strtok(nullptr, ", \t")) {
                *output++ = (float)std::atof(token);
            }
        } else if (expectCount) {
            if (++pathIndex >= (int32)m_aPathArray.size()) {
                return;
            }
            sscanf(line, "%d", &linesRemaining);
            const auto floatsPerLine = pathIndex < 2 ? 4 : 10;
            auto*& data = m_aPathArray[pathIndex].m_pArrPathData;
            data = static_cast<float*>(::operator new((linesRemaining * floatsPerLine + 1) * sizeof(float)));
            data[0] = (float)linesRemaining;
            output = data + 1;
            expectCount = false;
        } else if (*line == ';') {
            expectCount = true;
        }
    }
}

// 0x50AB50
void CCamera::GetScreenRect(CRect* rect) const {
    rect->left  = 0.0f;
    rect->right = SCREEN_WIDTH;

    if (m_bWideScreenOn) {
        rect->top    = (float)(RsGlobal.maximumHeight / 2) * m_fScreenReductionPercentage / 100.f - SCREEN_SCALE_Y(22.0f);
        rect->bottom = SCREEN_HEIGHT - (RsGlobal.maximumHeight / 2) * m_fScreenReductionPercentage / 100.f - SCREEN_SCALE_Y(14.0f);
    } else {
        rect->top    = 0.0f;
        rect->bottom = SCREEN_HEIGHT;
    }
}

// 0x50CB60
void CCamera::SetCamCollisionVarDataSet(int32 index) {
    if (index == gCurCamColVars) {
        return;
    }

    gCurCamColVars = index;
    gCurDistForCam = 1.0f;
    gpCamColVars   = gCamColVars[index];
}

// 0x50CCA0
void CCamera::SetColVarsVehicle(eVehicleType vehicleType, int32 camVehicleZoom) {
    switch (vehicleType) {
        case VEHICLE_TYPE_AUTOMOBILE:
        case VEHICLE_TYPE_PLANE:
            SetCamCollisionVarDataSet(camVehicleZoom + 9);
            return;
        case VEHICLE_TYPE_MTRUCK:
            SetCamCollisionVarDataSet(camVehicleZoom + 12);
            return;
        case VEHICLE_TYPE_QUAD:
            SetCamCollisionVarDataSet(camVehicleZoom + 15);
            return;
        case VEHICLE_TYPE_HELI:
            SetCamCollisionVarDataSet(camVehicleZoom + 18);
            return;
        case VEHICLE_TYPE_BOAT:
            SetCamCollisionVarDataSet(camVehicleZoom + 21);
            return;
        case VEHICLE_TYPE_TRAIN:
            SetCamCollisionVarDataSet(camVehicleZoom + 24);
            return;
    }
}

// 0x515BC0
void CCamera::StartTransitionWhenNotFinishedInter(eCamMode newCamMode) {
    m_bDoingSpecialInterp = true;
    StartTransition(newCamMode);
}

// 0x515200
/**
 * @brief Initiates a camera transition to a new camera mode.
 * 
 * This function handles the transition between different camera modes, setting up all necessary parameters
 * for a smooth camera movement. It manages aspects such as:
 * - Camera rotation and positioning
 * - Transition timing and interpolation fractions
 * - Special handling for weapon modes
 * - Entity references and target updates
 * 
 * The transition process includes:
 * 1. Setting up default transition values
 * 2. Handling player rotation for weapon modes
 * 3. Setting up the new camera parameters
 * 4. Managing specific camera mode transitions
 * 5. Initializing transition state and interpolation values
 * 6. Storing starting speeds and final transition parameters
 * 
 * @param newCamMode The camera mode to transition to (type eCamMode)
 * 
 * @note This function is central to the game's camera system and affects how the camera behaves
 * when switching between different views (e.g., from following a ped to aiming a weapon).
 * 
 * @see eCamMode
 * @see CCam
 */
void CCamera::StartTransition(eCamMode newCamMode) {
    CCam& activeCam             = m_aCams[m_nActiveCam];
    const auto activeCamMode    = activeCam.m_nMode;

    // Unused flag, not used in the game.
    // In GTA III/VC it was used for the Colt Python.
    m_bItsOkToLookJustAtThePlayer = false;

    // Default values
    m_bUseTransitionBeta          = false;
    m_fFractionInterToStopMoving  = 0.25f;
    m_fFractionInterToStopCatchUp = 0.75f;

    // Handle player rotation for weapon modes
    if (m_pTargetEntity && m_pTargetEntity->GetIsTypePed() && notsa::contains({ MODE_SNIPER, MODE_ROCKETLAUNCHER, MODE_ROCKETLAUNCHER_HS, MODE_M16_1STPERSON, MODE_SNIPER_RUNABOUT, MODE_ROCKETLAUNCHER_RUNABOUT, MODE_ROCKETLAUNCHER_RUNABOUT_HS, MODE_M16_1STPERSON_RUNABOUT, MODE_FIGHT_CAM_RUNABOUT, MODE_HELICANNON_1STPERSON, MODE_CAMERA, MODE_1STPERSON_RUNABOUT }, activeCamMode)) {
        const float angle                            = CGeneral::GetATanOfXY(activeCam.m_vecFront.x, activeCam.m_vecFront.y) - HALF_PI;
        m_pTargetEntity->AsPed()->m_fCurrentRotation = angle;
        m_pTargetEntity->AsPed()->m_fAimingRotation  = angle;
    }

    // Setup new camera
    activeCam.m_vecCamFixedModeVector = m_vecFixedModeVector;
    CEntity::ChangeEntityReference(activeCam.m_pCamTargetEntity, m_pTargetEntity);

    activeCam.m_vecCamFixedModeSource   = m_vecFixedModeSource;
    activeCam.m_vecCamFixedModeUpOffSet = m_vecFixedModeUpOffSet;
    activeCam.m_bCamLookingAtVector     = m_bLookingAtVector;
    if (m_bItsOkToLookJustAtThePlayer) {
        activeCam.m_nMode = newCamMode;
    }

    // Handle specific camera mode transitions
    switch (newCamMode) {
    case MODE_BEHINDCAR:
    case MODE_BEHINDBOAT:
        activeCam.m_fBetaSpeed = 0.0f;
        break;
    case MODE_FOLLOWPED: {
        if (m_bJustCameOutOfGarage) {
            activeCam.m_fHorizontalAngle = CGeneral::GetATanOfXY(activeCam.m_vecFront.x, activeCam.m_vecFront.y) + PI;
            activeCam.m_fTransitionBeta  = 0.0f;
        }

        m_bCamDirectlyInFront |= m_bTargetJustCameOffTrain;

        if (activeCamMode == MODE_CAM_ON_A_STRING) {
            m_bUseTransitionBeta        = true;
            const float angle           = CGeneral::GetATanOfXY(activeCam.m_vecFront.x, activeCam.m_vecFront.y);
            activeCam.m_fTransitionBeta = angle + (fabs(angle) <= HALF_PI ? DegreesToRadians(235.0f) : DegreesToRadians(55.0f));
        }
        break;
    }
    case MODE_SNIPER:
    case MODE_ROCKETLAUNCHER:
    case MODE_M16_1STPERSON:
    case MODE_SNIPER_RUNABOUT:
    case MODE_ROCKETLAUNCHER_RUNABOUT:
    case MODE_1STPERSON_RUNABOUT:
    case MODE_M16_1STPERSON_RUNABOUT:
    case MODE_FIGHT_CAM_RUNABOUT:
    case MODE_HELICANNON_1STPERSON:
    case MODE_CAMERA:
    case MODE_ROCKETLAUNCHER_HS:
    case MODE_ROCKETLAUNCHER_RUNABOUT_HS: {
        CEntity* vehicle             = FindPlayerVehicle();
        CMatrix* playerMat           = vehicle ? &vehicle->GetMatrix() : &FindPlayerPed()->GetMatrix();
        activeCam.m_fHorizontalAngle = CGeneral::GetATanOfXY(playerMat->GetForward().x, playerMat->GetForward().y);
        activeCam.m_fVerticalAngle   = 0.0f;
        break;
    }
    case MODE_CAM_ON_A_STRING: {
        if (m_bLookingAtPlayer && !m_bJustCameOutOfGarage) {
            m_bUseTransitionBeta = true;
            const float angle    = CGeneral::GetATanOfXY(activeCam.m_vecFront.x, activeCam.m_vecFront.y);
            if (activeCamMode == MODE_FIXED) { // Ghidra
                activeCam.m_fTransitionBeta = angle;
                break;
            }

            // Reconstruced + android simplified
            activeCam.m_fTransitionBeta = angle + (fabs(angle) <= HALF_PI ? DegreesToRadians(235.0f) : DegreesToRadians(55.0f));
        }
        break;
    }
    case MODE_PED_DEAD_BABY:
        activeCam.m_fVerticalAngle = DegreesToRadians(15.0f);
        break;
    }

    // Backup horizontal angle before Init.
    const float horizAngle = activeCam.m_fHorizontalAngle;

    int targetCoorsDuration = 600; // Like android version instead bool.
    m_nTransitionDuration   = 1'350;

    // Switch active camera
    if (activeCamMode == MODE_FOLLOWPED && newCamMode == MODE_CAM_ON_A_STRING
        || activeCamMode == MODE_CAM_ON_A_STRING && newCamMode == MODE_FOLLOWPED) {
        activeCam.m_nMode = newCamMode;
    } else {
        activeCam.Init();
        activeCam.m_nMode            = newCamMode;
        activeCam.m_fHorizontalAngle = horizAngle;
    }

    [&]() -> const void {
        if (newCamMode == MODE_CAM_ON_A_STRING && notsa::contains({ MODE_SYPHON_CRIM_IN_FRONT, MODE_FOLLOWPED, MODE_SYPHON, MODE_SPECIAL_FIXED_FOR_SYPHON, MODE_AIMWEAPON }, activeCamMode)) {
            m_fFractionInterToStopMoving  = 0.1f;
            m_fFractionInterToStopCatchUp = 0.9f;
            m_nTransitionDuration         = 750;
            return;
        }

        switch (activeCamMode) {
        case MODE_SYPHON_CRIM_IN_FRONT:
            if (newCamMode == MODE_SYPHON) {
                m_nTransitionDuration = 1'800;
                return;
            }
            break;
        case MODE_SPECIAL_FIXED_FOR_SYPHON:
            m_fFractionInterToStopMoving  = 0.2f;  // dword_8CCCCC
            m_fFractionInterToStopCatchUp = 0.8f;  // *&dword_8CCCC8
            m_nTransitionDuration         = 1'000; // dword_8CCCC4
            return;
        case MODE_FIXED:
            m_fFractionInterToStopMoving  = 0.05f;
            m_fFractionInterToStopCatchUp = 0.95f;
            return;
        }

        if (m_bPlayerWasOnBike && newCamMode == MODE_FOLLOWPED) {
            if (activeCamMode == MODE_CAM_ON_A_STRING) {
                m_nTransitionDuration         = 800;
                m_fFractionInterToStopMoving  = 0.02f;
                m_fFractionInterToStopCatchUp = 0.98f;
                return;
            }
        } else {
            switch (newCamMode) {
            case MODE_CAM_ON_A_STRING:
            case MODE_BEHINDBOAT:
                if (notsa::contains({ MODE_SNIPER_RUNABOUT, MODE_ROCKETLAUNCHER_RUNABOUT, MODE_ROCKETLAUNCHER_RUNABOUT_HS, MODE_1STPERSON_RUNABOUT, MODE_M16_1STPERSON_RUNABOUT, MODE_FIGHT_CAM_RUNABOUT, MODE_CAMERA }, activeCamMode)) {
                    m_fFractionInterToStopMoving  = 0.0f;
                    m_fFractionInterToStopCatchUp = 1.0f;
                    m_nTransitionDuration         = 1;
                    return;
                }
                break;
            case MODE_AIMWEAPON:
                m_fFractionInterToStopMoving  = 0.0f; // dword_B70044 ?
                m_fFractionInterToStopCatchUp = 1.0f; // *&dword_8CCCC0
                m_nTransitionDuration         = 400;  // dword_8CCCBC
                targetCoorsDuration           = 350;
                return;
            }

            if (!notsa::contains({ MODE_FOLLOWPED, MODE_SYPHON_CRIM_IN_FRONT, MODE_SYPHON, MODE_SPECIAL_FIXED_FOR_SYPHON }, newCamMode)) {
                m_nTransitionDuration = 1'350;
                return;
            }
        }
        if (!notsa::contains({ MODE_SYPHON_CRIM_IN_FRONT, MODE_FOLLOWPED, MODE_SYPHON, MODE_AIMWEAPON }, activeCamMode)) {
            m_nTransitionDuration = 1'350;
            return;
        }
        m_fFractionInterToStopMoving  = 0.1f;
        m_fFractionInterToStopCatchUp = 0.9f;
        m_nTransitionDuration         = 350;
        targetCoorsDuration           = 350;
    }();

    // Initialize transition state
    m_bTransitionState       = true;
    m_nTimeTransitionStart   = CTimer::GetTimeInMS();
    m_bTransitionJUSTStarted = true;

    // Store starting interpolation values
    if (m_bDoingSpecialInterp) {
        m_vecStartingSourceForInterPol = m_vecSourceDuringInter;
        m_vecStartingTargetForInterPol = m_vecTargetDuringInter;
        m_vecStartingUpForInterPol     = m_vecUpDuringInter;
        m_fStartingAlphaForInterPol    = m_fAlphaDuringInterPol;
        m_fStartingBetaForInterPol     = m_fBetaDuringInterPol;
    } else {
        m_vecStartingSourceForInterPol = activeCam.m_vecSource;
        m_vecStartingTargetForInterPol = activeCam.m_vecTargetCoorsForFudgeInter;
        m_vecStartingUpForInterPol     = activeCam.m_vecUp;
        m_fStartingAlphaForInterPol    = activeCam.m_fTrueAlpha;
        m_fStartingBetaForInterPol     = activeCam.m_fTrueBeta;
    }

    // Update active camera parameters
    activeCam.m_bCamLookingAtVector     = m_bLookingAtVector;
    activeCam.m_vecCamFixedModeVector   = m_vecFixedModeVector;
    activeCam.m_vecCamFixedModeSource   = m_vecFixedModeSource;
    activeCam.m_vecCamFixedModeUpOffSet = m_vecFixedModeUpOffSet;
    activeCam.m_nMode                   = newCamMode;
    CEntity::ChangeEntityReference(activeCam.m_pCamTargetEntity, m_pTargetEntity);

    // Store starting speeds
    m_fStartingFOVForInterPol    = activeCam.m_fFOV;
    m_vecSourceSpeedAtStartInter = activeCam.m_vecSourceSpeedOverOneFrame;
    m_vecTargetSpeedAtStartInter = activeCam.m_vecTargetSpeedOverOneFrame;
    m_vecUpSpeedAtStartInter     = activeCam.m_vecUpOverOneFrame;
    m_fAlphaSpeedAtStartInter    = activeCam.m_fAlphaSpeedOverOneFrame;
    m_fBetaSpeedAtStartInter     = activeCam.m_fBetaSpeedOverOneFrame;
    m_fFOVSpeedAtStartInter      = activeCam.m_fFovSpeedOverOneFrame;

    // Setup final transition parameters
    if (m_bLookingAtPlayer) {
        m_fFractionInterToStopMovingTarget  = 0.0f;
        m_fFractionInterToStopCatchUpTarget = 1.0f;
        m_nTransitionDurationTargetCoors    = targetCoorsDuration;
    } else {
        if (m_bScriptParametersSetForInterp) {
            m_fFractionInterToStopMoving  = m_fScriptPercentageInterToStopMoving;
            m_fFractionInterToStopCatchUp = m_fScriptPercentageInterToCatchUp;
            m_nTransitionDuration         = m_nScriptTimeForInterpolation;
        }
        m_nTransitionDurationTargetCoors    = m_nTransitionDuration;
        m_fFractionInterToStopMovingTarget  = m_fFractionInterToStopMoving;
        m_fFractionInterToStopCatchUpTarget = m_fFractionInterToStopCatchUp;
    }
}

auto CCamera::GetFrustumPoints() -> std::array<CVector, 5> {
    CVector pts[5]{};

    // First, the corners
    const auto farPlane  = RwCameraGetFarClipPlane(m_pRwCamera);
    const auto farVWSize = CVector2D{ *RwCameraGetViewWindow(m_pRwCamera) } * farPlane;
    const auto corners   = CRect{ -farVWSize, farVWSize }.GetCorners3D(farPlane);

    // Copy it into pts
    rng::copy(corners, pts);

    // Last is the center point
    pts[4] = CVector{ 0.f, 0.f, 0.f };

    // Transform them to world space
    RwV3dTransformPoints(pts, pts, 5, GetRwMatrix());

    // top left, top right, bottom right, bottom left, center
    return std::to_array(pts);
}
