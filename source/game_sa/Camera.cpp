#include "StdInc.h"

#include "Camera.h"

#include "TaskSimpleGangDriveBy.h"
#include "TaskSimpleHoldEntity.h"
#include "TaskSimpleDuck.h"
#include "Hud.h"

float& CCamera::m_f3rdPersonCHairMultY = *reinterpret_cast<float*>(0xB6EC10); ///< Where the player will be on the screen in relative coords when quick aiming
float& CCamera::m_f3rdPersonCHairMultX = *reinterpret_cast<float*>(0xB6EC14);
float& CCamera::m_fMouseAccelVertical = *reinterpret_cast<float*>(0xB6EC18);
float& CCamera::m_fMouseAccelHorzntl = *reinterpret_cast<float*>(0xB6EC1C);
bool& CCamera::m_bUseMouse3rdPerson = *reinterpret_cast<bool*>(0xB6EC2E);
bool& CCamera::bDidWeProcessAnyCinemaCam = *reinterpret_cast<bool*>(0xB6EC2D);

CCamera& TheCamera = *reinterpret_cast<CCamera*>(0xB6F028);
bool& gbModelViewer = *reinterpret_cast<bool*>(0xBA6728);
int8& gbCineyCamMessageDisplayed = *(int8*)0x8CC381; // 2
int32& gCameraDirection = *(int32*)0x8CC384;         // 3
eCamMode& gCameraMode = *(eCamMode*)0x8CC388;        // -1
uint32& gLastTime2PlayerCameraWasOK = *(uint32*)0xB6EC24;    // 0
uint32& gLastTime2PlayerCameraCollided = *(uint32*)0xB6EC28; // 0
bool& gPlayerPedVisible = *(bool*)0x8CC380; // true
uint8& gCurCamColVars = *(uint8*)0x8CCB80;
float& gCurDistForCam = *(float*)0x8CCB84;
float*& gpCamColVars = *(float**)0xB6FE88;
float (&gCamColVars)[28][6] = *(float (*)[28][6])0x8CC8E0;

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
    RH_ScopedInstall(CopyCameraMatrixToRWCam, 0x50AFA0, { .reversed = false });
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
    RH_ScopedInstall(SetNearClipBasedOnPedCollision, 0x50CB90, { .reversed = false });
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
    RH_ScopedInstall(HandleCameraMotionForDucking, 0x50CFA0, { .reversed = false });
    RH_ScopedInstall(HandleCameraMotionForDuckingDuringAim, 0x50D090, { .reversed = false });
    RH_ScopedInstall(VectorMoveLinear, 0x50D160, { .reversed = false });
    RH_ScopedInstall(VectorTrackLinear, 0x50D1D0, { .reversed = false });
    RH_ScopedInstall(AddShakeSimple, 0x50D240);
    RH_ScopedInstall(InitialiseScriptableComponents, 0x50D2D0);
    RH_ScopedInstall(DrawBordersForWideScreen, 0x514860);
    RH_ScopedInstall(Find3rdPersonCamTargetVector, 0x514970);
    RH_ScopedInstall(CalculateGroundHeight, 0x514B80);
    RH_ScopedInstall(CalculateFrustumPlanes, 0x514D60, { .reversed = false });
    RH_ScopedInstall(CalculateDerivedValues, 0x5150E0, { .reversed = false });
    RH_ScopedInstall(ImproveNearClip, 0x516B20, { .reversed = false });
    RH_ScopedInstall(SetCameraUpForMirror, 0x51A560);
    RH_ScopedInstall(RestoreCameraAfterMirror, 0x51A5A0);
    RH_ScopedInstall(ConeCastCollisionResolve, 0x51A5D0);
    RH_ScopedInstall(TryToStartNewCamMode, 0x51E560, { .reversed = false });
    RH_ScopedInstall(CameraColDetAndReact, 0x520190, { .reversed = false });
    RH_ScopedInstall(CamControl, 0x527FA0, { .reversed = false });
    RH_ScopedInstall(Process, 0x52B730, { .reversed = false });
    RH_ScopedInstall(DeleteCutSceneCamDataMemory, 0x5B24A0);
    RH_ScopedInstall(LoadPathSplines, 0x5B24D0, { .reversed = false });
    RH_ScopedInstall(Init, 0x5BC520, { .reversed = false });

    RH_ScopedOverloadedInstall(ProcessVectorTrackLinear, "0", 0x50D350, void(CCamera::*)(float), { .reversed = false });
    RH_ScopedOverloadedInstall(ProcessVectorTrackLinear, "1", 0x516440, void(CCamera::*)(), {.reversed = false});
    RH_ScopedOverloadedInstall(ProcessVectorMoveLinear, "0", 0x50D430, void(CCamera::*)(float), { .reversed = false });
    RH_ScopedOverloadedInstall(ProcessVectorMoveLinear, "1", 0x5164A0, void(CCamera::*)(), { .reversed = false });
    RH_ScopedOverloadedInstall(ProcessFOVLerp, "0", 0x50D510, void(CCamera::*)(float), { .reversed = false });
    RH_ScopedOverloadedInstall(ProcessFOVLerp, "1", 0x516500, void(CCamera::*)(), { .reversed = false });
    //RH_ScopedOverloadedInstall(ProcessJiggle, "0", 0x516560, { .reversed = false });

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
        m_fMouseAccelVertical = 0.0015f;
    }
    
    SetMotionBlur(255, 255, 255, 0, 0);

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
            camTweak.m_nModelIndex = -1;
            camTweak.m_fDistance   = 1.0f;
            camTweak.m_fAltitude   = 1.0f;
            camTweak.m_fAngle      = 0.0f;
        }

        m_aCamTweak[0].m_nModelIndex = MODEL_RCGOBLIN;
        m_aCamTweak[0].m_fDistance = 1.0f;
        m_aCamTweak[0].m_fAltitude = 1.0f;
        m_aCamTweak[0].m_fAngle    = 0.178997f; // todo: magic number

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
    if (vehicle->m_nModelIndex == m_nCurrentTweakModelIndex) {
        return;
    }

    InitCameraVehicleTweaks();
    for (auto& camTweak : m_aCamTweak) {
        if (camTweak.m_nModelIndex == (int32)vehicle->m_nModelIndex) { // todo: vehicle->m_nModelIndex -> int16?
            m_fCurrentTweakDistance = camTweak.m_fDistance;
            m_fCurrentTweakAltitude = camTweak.m_fAltitude;
            m_fCurrentTweakAngle    = camTweak.m_fAngle;
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
    const auto relAngle = cam.m_fVerticalAngle - std::atan(opposite / CDraw::ms_fAspectRatio);
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
    return plugin::CallMethod<0x50AFA0, CCamera*, bool>(this, bUpdateMatrix);
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

    if (m_nBlurType) {
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

// TODO: eBlurType
// 0x50BF40
void CCamera::SetMotionBlur(uint8 red, uint8 green, uint8 blue, int32 value, uint32 blurType) {
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
void CCamera::SetNewPlayerWeaponMode(eCamMode mode, int16 maxZoom, int16 minZoom) {
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

    if (entity->m_nStatus == STATUS_SIMPLE) {
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
    m_bPlayerWasOnBike = m_pTargetEntity && m_pTargetEntity->IsVehicle() && m_pTargetEntity->AsVehicle()->m_vecMoveSpeed.SquaredMagnitude() > 0.3f;

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

    if (m_pTargetEntity->IsVehicle()) {
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
                        switch (target->m_nType) {
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
void CCamera::TakeControlAttachToEntity(CEntity* target, CEntity* attached, CVector* attachedCamOffset, CVector* attachedCamLookAt, float attachedCamAngle, eSwitchType switchType, int32 whoIsInControlOfTheCamera) {
    plugin::CallMethod<0x50C910, CCamera*, CEntity*, CEntity*, CVector*, CVector*, float, eSwitchType, int32>(this, target, attached, attachedCamOffset, attachedCamLookAt, attachedCamAngle, switchType, whoIsInControlOfTheCamera);
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
    plugin::CallMethod<0x515BD0, CCamera*>(this);
}

// unused
// 0x50CB90
void CCamera::SetNearClipBasedOnPedCollision(float arg2) {
    plugin::CallMethod<0x50CB90, CCamera*, float>(this, arg2);
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

    if (vehicle->m_pTrailer) {
        m_pExtraEntity[m_nExtraEntitiesCount++] = vehicle->m_pTrailer;
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
    return plugin::CallMethodAndReturn<bool, 0x420C40, CCamera*, const CVector&, float, RwMatrix*>(this, origin, radius, transformMatrix);
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
    plugin::CallMethod<0x50CFA0, CCamera*, CPed*, CVector*, CVector*, bool>(this, ped, source, targPosn, arg5);
}

// arg5 always used as false
// 0x50D090
void CCamera::HandleCameraMotionForDuckingDuringAim(CPed* ped, CVector* source, CVector* targPosn, bool arg5) {
    plugin::CallMethod<0x50D090, CCamera*, CPed*, CVector*, CVector*, bool>(this, ped, source, targPosn, arg5);
}

// 0x50D160
void CCamera::VectorMoveLinear(CVector* moveLinearPosnEnd, CVector* moveLinearPosnStart, float duration, bool bMoveLinearWithEase) {
    plugin::CallMethod<0x50D160, CCamera*, CVector*, CVector*, float, bool>(this, moveLinearPosnEnd, moveLinearPosnStart, duration, bMoveLinearWithEase);
}

// 0x50D1D0
void CCamera::VectorTrackLinear(CVector* trackLinearStartPoint, CVector* trackLinearEndPoint, float duration, bool bEase) {
    plugin::CallMethod<0x50D1D0, CCamera*, CVector*, CVector*, float, bool>(this, trackLinearStartPoint, trackLinearEndPoint, duration, bEase);
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
    plugin::CallMethod<0x516440, CCamera*>(this);
}

// 0x50D350
void CCamera::ProcessVectorTrackLinear(float ratio) {
    plugin::CallMethod<0x50D350, CCamera*, float>(this, ratio);
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
    plugin::CallMethod<0x50D430, CCamera*, float>(this, ratio);
}

// 0x516500
void CCamera::ProcessFOVLerp() {
    plugin::CallMethod<0x516500, CCamera*>(this);
}

// 0x50D510
void CCamera::ProcessFOVLerp(float ratio) {
    plugin::CallMethod<0x50D510, CCamera*, float>(this, ratio);
}

// 0x5164A0
void CCamera::ProcessVectorMoveLinear() {
    plugin::CallMethod<0x5164A0, CCamera*>(this);
}

// unused
// 0x51A6F0
void CCamera::ProcessShake() {
    plugin::CallMethod<0x51A6F0, CCamera*>(this);
}

// shakeIntensity not used
// 0x516560
CVector* CCamera::ProcessShake(float intensity) {
    return plugin::CallMethodAndReturn<CVector*, 0x516560, CCamera*, float>(this, intensity);
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
    if (m_nBlurType == MOTION_BLUR_NONE  || m_nBlurType == MOTION_BLUR_LIGHT_SCENE) {
        m_nMotionBlurAddAlpha = 80;
    }
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RWRSTATE(NULL));
    CSprite2d::DrawRect({ -5.f, -5.f,     SCREEN_WIDTH + 5.f, rect.top         }, { 0, 0, 0, 255 });
    CSprite2d::DrawRect({ -5.f, rect.bottom, SCREEN_WIDTH + 5.f, SCREEN_HEIGHT + 5.f }, { 0, 0, 0, 255 });
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
    static auto& lastCalcCamPos    = StaticRef<CVector, 0xB70034>();
    static auto& exactGroundHeight = StaticRef<float, 0xB70030>();
    static auto& bbTopZ            = StaticRef<float, 0xB7002C>();
    static auto& bbBottomZ         = StaticRef<float, 0xB70028>();

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

// 0x514D60
void CCamera::CalculateFrustumPlanes(bool bForMirror) {
    plugin::CallMethod<0x514D60, CCamera*, bool>(this, bForMirror);
}

// 0x5150E0
void CCamera::CalculateDerivedValues(bool bForMirror, bool bOriented) {
    return plugin::CallMethod<0x5150E0, CCamera*, bool, bool>(this, bForMirror, bOriented);
}

// 0x516B20
void CCamera::ImproveNearClip(CVehicle* vehicle, CPed* ped, CVector* source, CVector* targPosn) {
    return plugin::CallMethod<0x516B20, CCamera*, CVehicle*, CPed*, CVector*, CVector*>(this, vehicle, ped, source, targPosn);
}

static CMatrix& preMirrorMat = *(CMatrix*)0xB6FE40;

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
    return plugin::CallMethodAndReturn<bool, 0x51E560, CCamera*, int32>(this, camSequence);
}

// 0x520190
void CCamera::CameraColDetAndReact(CVector* source, CVector* target) {
    plugin::CallMethod<0x520190, CCamera*, CVector*, CVector*>(this, source, target);
}

// 0x527FA0
void CCamera::CamControl() {
    plugin::CallMethod<0x527FA0, CCamera*>(this); // good luck warrior!
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
    plugin::CallMethod<0x5B24D0, CCamera*>(this);
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
void CCamera::StartTransitionWhenNotFinishedInter(eCamMode currentCamMode) {
    m_bDoingSpecialInterp = true;
    StartTransition(currentCamMode);
}

// 0x515200
void CCamera::StartTransition(eCamMode targetCamMode) {
    CCam*    activeCamera    = &this->m_aCams[this->m_nActiveCam];
    eCamMode previousCamMode = activeCamera->m_nMode;

    this->m_bItsOkToLookJustAtThePlayer = false;
    this->m_bUseTransitionBeta          = false;
    this->m_fFractionInterToStopMoving  = 0.25f;
    this->m_fFractionInterToStopCatchUp = 0.75f;

    // Handle special modes for rocket launcher etc
    if (this->m_pTargetEntity && this->m_pTargetEntity->IsPed() &&
        notsa::contains({
            MODE_SNIPER,
            MODE_ROCKETLAUNCHER,
            MODE_ROCKETLAUNCHER_HS,
            MODE_M16_1STPERSON,
            MODE_SNIPER_RUNABOUT,
            MODE_ROCKETLAUNCHER_RUNABOUT,
            MODE_ROCKETLAUNCHER_RUNABOUT_HS,
            MODE_M16_1STPERSON_RUNABOUT,
            MODE_FIGHT_CAM_RUNABOUT,
            MODE_HELICANNON_1STPERSON,
            MODE_CAMERA,
            MODE_1STPERSON_RUNABOUT
        }, activeCamera->m_nMode)
    ) {
        float targetRotation = CGeneral::GetATanOfXY(activeCamera->m_vecFront.x, activeCamera->m_vecFront.y) - HALF_PI;
        ((CPed*)this->m_pTargetEntity)->m_fCurrentRotation = targetRotation;
        ((CPed*)this->m_pTargetEntity)->m_fAimingRotation  = targetRotation;
    }


    // Copy fixed mode vectors and settings
    activeCamera->m_vecCamFixedModeVector = this->m_vecFixedModeVector;

    // Set up camera target entity
    CEntity::ChangeEntityReference(activeCamera->m_pCamTargetEntity, this->m_pTargetEntity);

    activeCamera->m_vecCamFixedModeSource   = this->m_vecFixedModeSource;
    activeCamera->m_vecCamFixedModeUpOffSet = this->m_vecFixedModeUpOffSet;
    activeCamera->m_bCamLookingAtVector     = this->m_bLookingAtVector;

    // Handle special transition cases
    switch (targetCamMode) {
    case MODE_BEHINDCAR:
    case MODE_BEHINDBOAT:
        activeCamera->m_fBetaSpeed = 0.0f;
        break;

    case MODE_FOLLOWPED: { // Please dont delete '{}'. It's important for the switch-case
        if (m_bJustCameOutOfGarage) {
            this->m_bUseTransitionBeta = true;
            if (activeCamera->m_vecFront.x != 0.0f || activeCamera->m_vecFront.y != 0.0f) {
                activeCamera->m_fTransitionBeta = CGeneral::GetATanOfXY(activeCamera->m_vecFront.x, activeCamera->m_vecFront.y) + PI;
            } else {
                activeCamera->m_fTransitionBeta = 0.0f;
            }
        }

        if (m_bTargetJustCameOffTrain) {
            this->m_bCamDirectlyInFront = true;
        }

        if (activeCamera->m_nMode != MODE_CAM_ON_A_STRING) {
            break;
        }

        this->m_bUseTransitionBeta = true;
        float angleXY              = CGeneral::GetATanOfXY(activeCamera->m_vecFront.x, activeCamera->m_vecFront.y);

        // Normalize angle between -PI and PI
        while (angleXY > PI) {
            angleXY -= TWO_PI;
        }
        while (angleXY < -PI) {
            angleXY += TWO_PI;
        }

        // Set transition angle based on current angle
        activeCamera->m_fTransitionBeta = angleXY + (std::fabs(angleXY) <= HALF_PI ? 4.1015239f : 0.95993114f);
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
        CEntity* vehicle   = FindPlayerVehicle();
        CMatrix* playerMat = vehicle ? &vehicle->GetMatrix() : &FindPlayerPed()->GetMatrix();

        activeCamera->m_fHorizontalAngle = CGeneral::GetATanOfXY(playerMat->GetForward().x, playerMat->GetForward().y);
        activeCamera->m_fVerticalAngle   = 0.0f;
        break;
    }
    case MODE_CAM_ON_A_STRING:
        if (this->m_bLookingAtPlayer && !this->m_bJustCameOutOfGarage) {
            this->m_bUseTransitionBeta      = true;
            float angle                     = CGeneral::GetATanOfXY(activeCamera->m_vecFront.y, activeCamera->m_vecFront.x);
            activeCamera->m_fTransitionBeta = angle;
        }
        break;
    case MODE_PED_DEAD_BABY:
        activeCamera->m_fVerticalAngle = DegreesToRadians(15.0f);
        break;
    }

    // Store horizontal angle before init
    float horizontalAngle = activeCamera->m_fHorizontalAngle;

    // Init camera with stored angle
    if (activeCamera->m_nMode == MODE_FOLLOWPED && targetCamMode == MODE_CAM_ON_A_STRING || activeCamera->m_nMode == MODE_CAM_ON_A_STRING && targetCamMode == MODE_FOLLOWPED) {
        activeCamera->m_nMode = targetCamMode;
    } else {
        activeCamera->Init();
        activeCamera->m_nMode            = targetCamMode;
        activeCamera->m_fHorizontalAngle = horizontalAngle;
    }

    bool isAimWeaponTransition  = false;
    this->m_nTransitionDuration = 1'350;

    if (previousCamMode == MODE_SYPHON && targetCamMode == MODE_SYPHON_CRIM_IN_FRONT) {
        this->m_nTransitionDuration = 1'800;
    } else if (previousCamMode == MODE_CAM_ON_A_STRING && notsa::contains({ MODE_SYPHON_CRIM_IN_FRONT, MODE_FOLLOWPED, MODE_SYPHON, MODE_SPECIAL_FIXED_FOR_SYPHON, MODE_AIMWEAPON }, targetCamMode)) {
        this->m_fFractionInterToStopMoving  = 0.1f;
        this->m_fFractionInterToStopCatchUp = 0.89999998f;
        this->m_nTransitionDuration         = 750;
    } else if (targetCamMode == MODE_SPECIAL_FIXED_FOR_SYPHON) {
        this->m_fFractionInterToStopMoving  = 0.25f;
        this->m_fFractionInterToStopCatchUp = 0.8f;
        this->m_nTransitionDuration         = 1'000;
    } else if (targetCamMode == MODE_FIXED) {
        this->m_fFractionInterToStopMoving  = 0.050000001f;
        this->m_fFractionInterToStopCatchUp = 0.94999999f;
    } else if (this->m_bPlayerWasOnBike
               && previousCamMode == MODE_FOLLOWPED
               && targetCamMode == MODE_CAM_ON_A_STRING) {
        this->m_nTransitionDuration         = 800;
        this->m_fFractionInterToStopMoving  = 0.02f;
        this->m_fFractionInterToStopCatchUp = 0.98000002f;
    } else if (notsa::contains({ MODE_CAM_ON_A_STRING, MODE_BEHINDBOAT }, previousCamMode)
               && notsa::contains({ MODE_SNIPER_RUNABOUT, MODE_ROCKETLAUNCHER_RUNABOUT, MODE_ROCKETLAUNCHER_RUNABOUT_HS, MODE_1STPERSON_RUNABOUT, MODE_M16_1STPERSON_RUNABOUT, MODE_FIGHT_CAM_RUNABOUT, MODE_CAMERA }, targetCamMode)) {
        this->m_fFractionInterToStopMoving  = 0.0f;
        this->m_fFractionInterToStopCatchUp = 1.0f;
        this->m_nTransitionDuration         = 1;
    } else if (previousCamMode == MODE_AIMWEAPON) {
        this->m_fFractionInterToStopMoving  = 0.0f;
        this->m_fFractionInterToStopCatchUp = 1.0f;
        this->m_nTransitionDuration         = 400;
        isAimWeaponTransition               = true;
    } else if (notsa::contains({ MODE_FOLLOWPED, MODE_SYPHON_CRIM_IN_FRONT, MODE_SYPHON, MODE_SPECIAL_FIXED_FOR_SYPHON }, previousCamMode)) {
        if (notsa::contains({ MODE_SYPHON_CRIM_IN_FRONT, MODE_FOLLOWPED, MODE_SYPHON, MODE_AIMWEAPON }, targetCamMode)) {
            this->m_fFractionInterToStopMoving  = 0.1f;
            this->m_fFractionInterToStopCatchUp = 0.89999998f;
            this->m_nTransitionDuration         = 350;
            isAimWeaponTransition               = true;
        } else {
            this->m_nTransitionDuration = 1'350;
        }
    }

    // Start transition
    this->m_bScriptParametersSetForInterp = this->m_bDoingSpecialInterp;
    this->m_bTransitionState              = true;
    this->m_nTimeTransitionStart          = CTimer::GetTimeInMS();
    this->m_bTransitionJUSTStarted        = true;

    // Store interpolation start values
    if (this->m_bScriptParametersSetForInterp) {
        this->m_vecStartingSourceForInterPol = this->m_vecSourceDuringInter;
        this->m_vecStartingTargetForInterPol = this->m_vecTargetDuringInter;
        this->m_vecStartingUpForInterPol     = this->m_vecUpDuringInter;
        this->m_fStartingAlphaForInterPol    = this->m_fAlphaDuringInterPol;
        this->m_fStartingBetaForInterPol     = this->m_fBetaDuringInterPol;
    } else {
        this->m_vecStartingSourceForInterPol = activeCamera->m_vecSource;
        this->m_vecStartingTargetForInterPol = activeCamera->m_vecTargetCoorsForFudgeInter;
        this->m_vecStartingUpForInterPol     = activeCamera->m_vecUp;
        this->m_fStartingAlphaForInterPol    = activeCamera->m_fTrueAlpha;
        this->m_fStartingBetaForInterPol     = activeCamera->m_fTrueBeta;
    }

    // Set up camera fixpoint vectors and target
    activeCamera->m_bCamLookingAtVector     = this->m_bLookingAtVector;
    activeCamera->m_vecCamFixedModeVector   = this->m_vecFixedModeVector;
    activeCamera->m_vecCamFixedModeSource   = this->m_vecFixedModeSource;
    activeCamera->m_vecCamFixedModeUpOffSet = this->m_vecFixedModeUpOffSet;
    activeCamera->m_nMode                   = targetCamMode;

    // Set up camera target entity
    CEntity::ChangeEntityReference(activeCamera->m_pCamTargetEntity, this->m_pTargetEntity);

    this->m_bTransitionState       = true;
    this->m_nTimeTransitionStart   = CTimer::GetTimeInMS();
    this->m_bTransitionJUSTStarted = true;

    // Store speeds at start of interpolation
    this->m_fStartingFOVForInterPol    = activeCamera->m_fFOV;
    this->m_vecSourceSpeedAtStartInter = activeCamera->m_vecSourceSpeedOverOneFrame;
    this->m_vecTargetSpeedAtStartInter = activeCamera->m_vecTargetSpeedOverOneFrame;
    this->m_vecUpSpeedAtStartInter     = activeCamera->m_vecUpOverOneFrame;
    this->m_fAlphaSpeedAtStartInter    = activeCamera->m_fAlphaSpeedOverOneFrame;
    this->m_fBetaSpeedAtStartInter     = activeCamera->m_fBetaSpeedOverOneFrame;
    this->m_fFOVSpeedAtStartInter      = activeCamera->m_fFovSpeedOverOneFrame;

    // Handle player transition parameters
    if (this->m_bLookingAtPlayer) {
        this->m_fFractionInterToStopMovingTarget  = 0.0f;
        this->m_fFractionInterToStopCatchUpTarget = 1.0f;
        this->m_nTransitionDurationTargetCoors    = isAimWeaponTransition ? 350 : 600;
    } else {
        if (this->m_bScriptParametersSetForInterp) {
            this->m_fFractionInterToStopMoving  = this->m_fScriptPercentageInterToStopMoving;
            this->m_fFractionInterToStopCatchUp = this->m_fScriptPercentageInterToCatchUp;
            this->m_nTransitionDuration         = this->m_nScriptTimeForInterpolation;
        }
        this->m_nTransitionDurationTargetCoors    = this->m_nTransitionDuration;
        this->m_fFractionInterToStopMovingTarget  = this->m_fFractionInterToStopMoving;
        this->m_fFractionInterToStopCatchUpTarget = this->m_fFractionInterToStopCatchUp;
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

    // That's it!
    return std::to_array(pts);
}
