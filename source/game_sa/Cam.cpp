#include "StdInc.h"

#include "Cam.h"

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
    TheCamera.m_bCinemaCamera = false;
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
void CCam::Get_TwoPlayer_AimVector(CVector& out) {
    assert(0);
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
bool CCam::Using3rdPersonMouseCam() {
    return CCamera::m_bUseMouse3rdPerson && m_nMode == MODE_FOLLOWPED;
}

// 0x509DC0
bool CCam::GetWeaponFirstPersonOn() {
    return m_pCamTargetEntity && m_pCamTargetEntity->IsPed() && m_pCamTargetEntity->AsPed()->GetActiveWeapon().m_IsFirstPersonWeaponModeSelected;
}

// 0x526FC0
bool CCam::Process() {
    assert(0); return false;
}

// 0x518500
bool CCam::ProcessArrestCamOne() {
    assert(0); return false;
}

// 0x519250
bool CCam::ProcessPedsDeadBaby() {
    assert(0); return false;
}

// 0x50EB70
bool CCam::Process_1rstPersonPedOnPC(const CVector&, float, float, float) {
    assert(0); return false;
}

// 0x517EA0
bool CCam::Process_1stPerson(const CVector&, float, float, float) {
    assert(0); return false;
}

// 0x521500
bool CCam::Process_AimWeapon(const CVector&, float, float, float) {
    assert(0); return false;
}

// 0x512B10
bool CCam::Process_AttachedCam() {
    assert(0); return false;
}

// 0x525E50
bool CCam::Process_Cam_TwoPlayer() {
    assert(0); return false;
}

// 0x519810
bool CCam::Process_Cam_TwoPlayer_InCarAndShooting() {
    assert(0); return false;
}

// 0x513510
bool CCam::Process_Cam_TwoPlayer_Separate_Cars() {
    assert(0); return false;
}

// 0x513BE0
bool CCam::Process_Cam_TwoPlayer_Separate_Cars_TopDown() {
    assert(0); return false;
}

// 0x51B850
bool CCam::Process_DW_BirdyCam(bool) {
    assert(0); return false;
}

// 0x51B120
bool CCam::Process_DW_CamManCam(bool) {
    assert(0); return false;
}

// 0x51A740
bool CCam::Process_DW_HeliChaseCam(bool) {
    assert(0); return false;
}

// 0x51C760
bool CCam::Process_DW_PlaneCam1(bool) {
    assert(0); return false;
}

// 0x51CC30
bool CCam::Process_DW_PlaneCam2(bool) {
    assert(0); return false;
}

// 0x51D100
bool CCam::Process_DW_PlaneCam3(bool) {
    assert(0); return false;
}

// 0x51C250
bool CCam::Process_DW_PlaneSpotterCam(bool) {
    assert(0); return false;
}

// 0x50F3F0
bool CCam::Process_Editor(const CVector&, float, float, float) {
    assert(0); return false;
}

// 0x51D470
bool CCam::Process_Fixed(const CVector&, float, float, float) {
    assert(0); return false;
}

// 0x5B25F0
bool CCam::Process_FlyBy(const CVector&, float, float, float) {
    assert(0); return false;
}

// 0x5245B0
bool CCam::Process_FollowCar_SA(const CVector&, float, float, float, bool) {
    assert(0); return false;
}

// 0x50F970
bool CCam::Process_FollowPedWithMouse(const CVector&, float, float, float) {
    assert(0); return false;
}

// 0x522D40
bool CCam::Process_FollowPed_SA(const CVector&, float, float, float, bool) {
    assert(0); return false;
}

// 0x5105C0
bool CCam::Process_M16_1stPerson(const CVector&, float, float, float) {
    assert(0); return false;
}

// 0x511B50
bool CCam::Process_Rocket(const CVector&, float, float, float, bool) {
    assert(0); return false;
}

// 0x517500
bool CCam::Process_SpecialFixedForSyphon(const CVector&, float, float, float) {
    assert(0); return false;
}

// 0x512110
bool CCam::Process_WheelCam(const CVector&, float, float, float) {
    assert(0); return false;
}
