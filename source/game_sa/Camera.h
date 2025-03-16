/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include <RenderWare.h>
#include <Vector.h>
#include <Vector2D.h>
#include <Cam.h>
#include "QueuedMode.h"
#include "CamPathSplines.h"
#include "eCamMode.h"
#include "eVehicleType.h"
#include "ePedType.h"
#include "Hud.h"

class CEntity;
class CVector;
class CVehicle;
class CMatrix;
class CPed;
class CSphere;
class CGarage;

enum class eFadeFlag : uint16 {
    FADE_IN,
    FADE_OUT
};

enum class eSwitchType : uint16 {
    NONE,
    INTERPOLATION,
    JUMPCUT
};

/* todo:
  LOOKING_BEHIND = 0x0,
  LOOKING_LEFT = 0x1,
  LOOKING_RIGHT = 0x2,
  LOOKING_FORWARD = 0x3,
*/
enum eLookingDirection {
    LOOKING_DIRECTION_UNKNOWN_1 = 0,
    LOOKING_DIRECTION_BEHIND    = 1,
    LOOKING_DIRECTION_UNKNOWN_3 = 2,
    LOOKING_DIRECTION_FORWARD   = 3,
};

enum class eGroundHeightType : int32 {
    ENTITY_BB_BOTTOM = 0,    // ground height + boundingBoxMin.z of colliding entity
    EXACT_GROUND_HEIGHT = 1, // ignores height of colliding entity at position
    ENTITY_BB_TOP = 2        // ground height + boundingBoxMax.z of colliding entity
};

enum {
    MOTION_BLUR_NONE = 0,
    MOTION_BLUR_SNIPER,
    MOTION_BLUR_LIGHT_SCENE,
    MOTION_BLUR_SECURITY_CAM,
    MOTION_BLUR_CUT_SCENE,
    MOTION_BLUR_INTRO,
    MOTION_BLUR_INTRO2,
    MOTION_BLUR_SNIPER_ZOOM,
    MOTION_BLUR_INTRO3,
    MOTION_BLUR_INTRO4,
};

struct CamTweak {
    int32 m_nModelIndex;
    float m_fDistance;
    float m_fAltitude;
    float m_fAngle;
};
VALIDATE_SIZE(CamTweak, 0x10);

class CCamera : public CPlaceable {
public:
    bool            m_bAboveGroundTrainNodesLoaded{};
    bool            m_bBelowGroundTrainNodesLoaded{};
    bool            m_bCamDirectlyBehind{};
    bool            m_bCamDirectlyInFront{};
    bool            m_bCameraJustRestored{};
    bool            m_bCutsceneFinished{};
    bool            m_bCullZoneChecksOn{};
    bool            m_bFirstPersonBeingUsed{};
    bool            m_bJustJumpedOutOf1stPersonBecauseOfTarget{};
    bool            m_bIdleOn{};
    bool            m_bInATunnelAndABigVehicle{};
    bool            m_bInitialNodeFound{};
    bool            m_bInitialNoNodeStaticsSet{};
    bool            m_bIgnoreFadingStuffForMusic{};
    bool            m_bPlayerIsInGarage{};
    bool            m_bPlayerWasOnBike{};
    bool            m_bJustCameOutOfGarage{};
    bool            m_bJustInitialized{true};
    bool            m_bJust_Switched{};
    bool            m_bLookingAtPlayer{true};
    bool            m_bLookingAtVector{};
    bool            m_bMoveCamToAvoidGeom{};
    bool            m_bObbeCinematicPedCamOn{};
    bool            m_bObbeCinematicCarCamOn{};
    bool            m_bRestoreByJumpCut{};
    bool            m_bUseNearClipScript{};
    bool            m_bStartInterScript{};
    bool            m_bStartingSpline{};
    bool            m_bTargetJustBeenOnTrain{};
    bool            m_bTargetJustCameOffTrain{};
    bool            m_bUseSpecialFovTrain{};
    bool            m_bUseTransitionBeta{};
    bool            m_bUseScriptZoomValuePed{};
    bool            m_bUseScriptZoomValueCar{};
    bool            m_bWaitForInterpolToFinish{};
    bool            m_bItsOkToLookJustAtThePlayer{};
    bool            m_bWantsToSwitchWidescreenOff{};
    bool            m_bWideScreenOn{};
    bool            m_b1rstPersonRunCloseToAWall{};
    bool            m_bHeadBob{};
    bool            m_bVehicleSuspenHigh{};
    bool            m_bEnable1rstPersonCamCntrlsScript{};
    bool            m_bAllow1rstPersonWeaponsCamera{};
    bool            m_bCooperativeCamMode{};
    bool            m_bAllowShootingWith2PlayersInCar{true};
    bool            m_bDisableFirstPersonInCar{};
    eCamMode        m_nModeForTwoPlayersSeparateCars{ MODE_TWOPLAYER_SEPARATE_CARS };
    eCamMode        m_nModeForTwoPlayersSameCarShootingAllowed{ MODE_TWOPLAYER_IN_CAR_AND_SHOOTING };
    eCamMode        m_nModeForTwoPlayersSameCarShootingNotAllowed{ MODE_BEHINDCAR };
    eCamMode        m_nModeForTwoPlayersNotBothInCar{ MODE_TWOPLAYER };
    bool            m_bGarageFixedCamPositionSet{};
    bool            m_bDoingSpecialInterp{};
    bool            m_bScriptParametersSetForInterp{};
    bool            m_bFading{};
    bool            m_bMusicFading{};
    bool            m_bMusicFadedOut{};
    bool            m_bFailedCullZoneTestPreviously{};
    bool            m_bFadeTargetIsSplashScreen{};
    bool            m_bWorldViewerBeingUsed{};
    bool            m_bTransitionJUSTStarted{};
    bool            m_bTransitionState{};
    uint8           m_nActiveCam{};
    uint32          m_nCamShakeStart{};
    uint32          m_nFirstPersonCamLastInputTime{};
    uint32          m_nLongestTimeInMill{ 5000 };
    uint32          m_nNumberOfTrainCamNodes{};
    uint32          m_nTimeLastChange{};
    uint32          m_nTimeWeLeftIdle_StillNoInput{};
    uint32          m_nTimeWeEnteredIdle{};
    uint32          m_nTimeTransitionStart{};
    uint32          m_nTransitionDuration{};
    uint32          m_nTransitionDurationTargetCoors{};
    uint32          m_nBlurBlue{};
    uint32          m_nBlurGreen{};
    uint32          m_nBlurRed{};
    uint32          m_nBlurType{};
    uint32          m_nWorkOutSpeedThisNumFrames{4};
    uint32          m_nNumFramesSoFar{};
    uint32          m_nCurrentTrainCamNode{};
    uint32          m_nMotionBlur{};
    uint32          m_nMotionBlurAddAlpha{};
    uint32          m_nCheckCullZoneThisNumFrames{6};
    uint32          m_nZoneCullFrameNumWereAt{};
    uint32          m_nWhoIsInControlOfTheCamera{};
    uint32          m_nCarZoom{2};
    float           m_fCarZoomBase{};
    float           m_fCarZoomTotal{};
    float           m_fCarZoomSmoothed{};
    float           m_fCarZoomValueScript{};
    uint32          m_nPedZoom{2};
    float           m_fPedZoomBase{};
    float           m_fPedZoomTotal{};
    float           m_fPedZoomSmoothed{};
    float           m_fPedZoomValueScript{};
    float           m_fCamFrontXNorm{};
    float           m_fCamFrontYNorm{};
    float           m_fDistanceToWater{};
    float           m_fHeightOfNearestWater{};
    float           m_fFOVDuringInter{};
    float           m_fLODDistMultiplier{1.f};
    float           m_fGenerationDistMultiplier{};
    float           m_fAlphaSpeedAtStartInter{};
    float           m_fAlphaWhenInterPol{};
    float           m_fAlphaDuringInterPol{};
    float           m_fBetaDuringInterPol{};
    float           m_fBetaSpeedAtStartInter{};
    float           m_fBetaWhenInterPol{};
    float           m_fFOVWhenInterPol{};
    float           m_fFOVSpeedAtStartInter{};
    float           m_fStartingBetaForInterPol{};
    float           m_fStartingAlphaForInterPol{};
    float           m_fPedOrientForBehindOrInFront{};
    float           m_fCameraAverageSpeed{};
    float           m_fCameraSpeedSoFar{};
    float           m_fCamShakeForce{};
    float           m_fFovForTrain{70.f};
    float           m_fFOV_Wide_Screen{};
    float           m_fNearClipScript{ 0.9f };
    float           m_fOldBetaDiff{};
    float           m_fPositionAlongSpline{};
    float           m_fScreenReductionPercentage{};
    float           m_fScreenReductionSpeed{};
    float           m_fAlphaForPlayerAnim1rstPerson{};
    float           m_fOrientation{};
    float           m_fPlayerExhaustion{1.f};
    float           m_fSoundDistUp{};
    float           m_fSoundDistUpAsRead{};
    float           m_fSoundDistUpAsReadOld{};
    float           m_fAvoidTheGeometryProbsTimer{};
    uint16          m_nAvoidTheGeometryProbsDirn{};
    float           m_fWideScreenReductionAmount{};
    float           m_fStartingFOVForInterPol{};
    CCam            m_aCams[3]{};
    CGarage*        m_pToGarageWeAreIn{};
    CGarage*        m_pToGarageWeAreInForHackAvoidFirstPerson{};
    CQueuedMode     m_PlayerMode{};
    CQueuedMode     m_PlayerWeaponMode{};
    CVector         m_vecPreviousCameraPosition{};
    CVector         m_vecRealPreviousCameraPosition{};
    CVector         m_vecAimingTargetCoors{};
    CVector         m_vecFixedModeVector{};
    CVector         m_vecFixedModeSource{};
    CVector         m_vecFixedModeUpOffSet{};
    CVector         m_vecCutSceneOffset{};
    CVector         m_vecStartingSourceForInterPol{};
    CVector         m_vecStartingTargetForInterPol{};
    CVector         m_vecStartingUpForInterPol{};
    CVector         m_vecSourceSpeedAtStartInter{};
    CVector         m_vecTargetSpeedAtStartInter{};
    CVector         m_vecUpSpeedAtStartInter{};
    CVector         m_vecSourceWhenInterPol{};
    CVector         m_vecTargetWhenInterPol{};
    CVector         m_vecUpWhenInterPol{};
    CVector         m_vecClearGeometryVec{};
    CVector         m_vecGameCamPos{};
    CVector         m_vecSourceDuringInter{};
    CVector         m_vecTargetDuringInter{};
    CVector         m_vecUpDuringInter{};
    CVector         m_vecAttachedCamOffset{};
    CVector         m_vecAttachedCamLookAt{};
    float           m_fAttachedCamAngle{};
    RwCamera*       m_pRwCamera{};
    CEntity*        m_pTargetEntity{};
    CEntity*        m_pAttachedEntity{};
    CCamPathSplines m_aPathArray[4]{};
    bool            m_bMirrorActive{};
    bool            m_bResetOldMatrix{};
    CMatrix         m_mCameraMatrix{ CMatrix::Identity() };
    CMatrix         m_mCameraMatrixOld{};
    CMatrix         m_mViewMatrix{};
    CMatrix         m_mMatInverse{};
    CMatrix         m_mMatMirrorInverse{};
    CMatrix         m_mMatMirror{};
    CVector         m_avecFrustumNormals[4]{};
    CVector         m_avecFrustumWorldNormals[4]{};
    CVector         m_avecFrustumWorldNormals_Mirror[4]{};
    float           m_fFrustumPlaneOffsets[4]{};
    float           m_fFrustumPlaneOffsets_Mirror[4]{};
    CVector         m_vecRightFrustumNormal{};  //!< unused?
    CVector         m_vecBottomFrustumNormal{}; //!< unused?
    CVector         m_vecTopFrustumNormal{};    //!< unused?
    float           field_BF8{};                //!< unused?
    float           m_fFadeAlpha{};
    float           m_fEffectsFaderScalingFactor{};
    float           m_fFadeDuration{};
    float           m_fTimeToFadeMusic{};
    float           m_fTimeToWaitToFadeMusic{};
    float           m_fFractionInterToStopMoving{0.25f};
    float           m_fFractionInterToStopCatchUp{0.75f};
    float           m_fFractionInterToStopMovingTarget{};
    float           m_fFractionInterToStopCatchUpTarget{};
    float           m_fGaitSwayBuffer{0.85f};
    float           m_fScriptPercentageInterToStopMoving{};
    float           m_fScriptPercentageInterToCatchUp{};
    uint32          m_nScriptTimeForInterpolation{};
    eFadeFlag       m_nFadeInOutFlag{};
    int32           m_nModeObbeCamIsInForCar{30};
    eCamMode        m_nModeToGoTo{ MODE_FOLLOWPED };
    eFadeFlag       m_nMusicFadingDirection{};
    eSwitchType     m_nTypeOfSwitch{ eSwitchType::INTERPOLATION };
    char            _alignC40[2]{};
    uint32          m_nFadeStartTime{};
    uint32          m_nFadeTimeStartedMusic{};
    int32           m_nExtraEntitiesCount{};
    CEntity*        m_pExtraEntity[2]{};
    float           m_fDuckCamMotionFactor{};
    float           m_fDuckAimCamMotionFactor{};
    float           m_fTrackLinearStartTime{};
    float           m_fTrackLinearEndTime{};
    CVector         m_vecTrackLinearEndPoint{};
    CVector         m_vecTrackLinearStartPoint{};
    bool            m_bTrackLinearWithEase{};
    CVector         m_vecTrackLinear{};
    bool            m_bVecTrackLinearProcessed{};
    float           m_fShakeIntensity{};
    float           m_fStartShakeTime{}; ///< In MS [Obtained from `CTimer::GetTimeInMS()`]
    float           m_fEndShakeTime{};
    int32           field_C9C{};
    int32           m_nShakeType{};
    float           m_fStartZoomTime{};
    float           m_fEndZoomTime{};
    float           m_fZoomInFactor{};
    float           m_fZoomOutFactor{};
    uint8           m_nZoomMode{};
    bool            m_bFOVLerpProcessed{};
    float           m_fFOVNew{};
    float           m_fMoveLinearStartTime{};
    float           m_fMoveLinearEndTime{};
    CVector         m_vecMoveLinearPosnStart{};
    CVector         m_vecMoveLinearPosnEnd{};
    bool            m_bMoveLinearWithEase{};
    CVector         m_vecMoveLinear{};
    bool            m_bVecMoveLinearProcessed{};
    bool            m_bBlockZoom{};
    bool            m_bCameraPersistPosition{};
    bool            m_bCameraPersistTrack{};
    bool            m_bCinemaCamera{};
    CamTweak        m_aCamTweak[5]{};
    bool            m_bCameraVehicleTweaksInitialized{};
    float           m_fCurrentTweakDistance{};
    float           m_fCurrentTweakAltitude{};
    float           m_fCurrentTweakAngle{};
    int32           m_nCurrentTweakModelIndex{};
    // the following are unused?
    int32           field_D58{};
    int32           field_D5C{};
    int32           field_D60{};
    int32           field_D64{};
    int32           field_D68{};
    int32           field_D6C{};
    int32           field_D70{};
    int32           field_D74{};

    static float &m_f3rdPersonCHairMultY;
    static float &m_f3rdPersonCHairMultX;
    static float &m_fMouseAccelVertical;
    static float &m_fMouseAccelHorzntl;
    static bool &m_bUseMouse3rdPerson;
    static bool &bDidWeProcessAnyCinemaCam;

public:
    static void InjectHooks();

    CCamera();
    ~CCamera() override;
    CCamera* Constructor();
    CCamera* Destructor();

    void Init();
    void InitCameraVehicleTweaks();
    void InitialiseScriptableComponents();
    void InitialiseCameraForDebugMode();

    void LoadPathSplines(FILE* file);

    bool IsTargetingActive();
    bool IsExtraEntityToIgnore(CEntity *entity);
    bool IsItTimeForNewCamera(int32 camSequence, int32 startTime); // IsItTimeForNewcam
    bool IsSphereVisible(const CVector& origin, float radius, RwMatrix* transformMatrix);
    bool IsSphereVisible(const CVector& origin, float radius);
    bool IsSphereVisible(const CSphere& sphere) { return IsSphereVisible(sphere.m_vecCenter, sphere.m_fRadius); }
    void LerpFOV(float zoomInFactor, float zoomOutFactor, float timeLimit, bool bEase);

    void Process();
    void ProcessWideScreenOn();
    void ProcessFOVLerp(float ratio);
    void ProcessFOVLerp();
    void ProcessFade();
    void ProcessMusicFade();
    void ProcessScriptedCommands();
    void ProcessShake();
    CVector* ProcessShake(float intensity);
    void ProcessVectorMoveLinear();
    void ProcessVectorMoveLinear(float ratio);
    void ProcessVectorTrackLinear();
    void ProcessVectorTrackLinear(float ratio);
    void ProcessObbeCinemaCameraBoat();
    void ProcessObbeCinemaCameraCar();
    void ProcessObbeCinemaCameraHeli();
    void ProcessObbeCinemaCameraPed();
    void ProcessObbeCinemaCameraPlane();
    void ProcessObbeCinemaCameraTrain();
    static void DontProcessObbeCinemaCamera();

    void Restore();
    void RestoreCameraAfterMirror();
    void RestoreWithJumpCut();
    void RenderMotionBlur() const;
    void ResetDuckingSystem(CPed *ped);

    void SetCamCutSceneOffSet(const CVector& offset);
    void SetCamPositionForFixedMode(const CVector& fixedModeSource, const CVector& fixedModeUpOffset);
    void SetCameraDirectlyBehindForFollowPed_CamOnAString();
    void SetCameraDirectlyInFrontForFollowPed_CamOnAString();
    void SetCameraDirectlyBehindForFollowPed_ForAPed_CamOnAString(CPed* targetPed);
    void SetCameraDirectlyInFrontForFollowPed_ForAPed_CamOnAString(CPed* targetPed);
    void SetCameraUpForMirror();
    void SetFadeColour(uint8 red, uint8 green, uint8 blue);
    void SetMotionBlur(uint8 red, uint8 green, uint8 blue, int32 value, uint32 blurType);
    void SetMotionBlurAlpha(int32 alpha);
    void SetNearClipBasedOnPedCollision(float arg2);
    void SetNearClipScript(float nearClip);
    void SetNewPlayerWeaponMode(eCamMode mode, int16 maxZoom = 0, int16 minZoom = 0);
    void SetParametersForScriptInterpolation(float interpolationToStopMoving, float interpolationToCatchUp, uint32 timeForInterpolation);
    void SetPercentAlongCutScene(float percent);
    void SetRwCamera(RwCamera* camera);
    void SetWideScreenOff();
    void SetWideScreenOn();
    void SetZoomValueCamStringScript(int16 zoomMode);
    void SetZoomValueFollowPedScript(int16 zoomMode);

    static void SetCamCollisionVarDataSet(int32 index);
    static void SetColVarsAimWeapon(int32 aimingType);
    static void SetColVarsPed(ePedType pedType, int32 nCamPedZoom);
    static void SetColVarsVehicle(eVehicleType vehicleType, int32 camVehicleZoom);

    void StartCooperativeCamMode();
    void StopCooperativeCamMode();
    void StartTransition(eCamMode newCamMode);
    void StartTransitionWhenNotFinishedInter(eCamMode newCamMode);

    void StoreValuesDuringInterPol(CVector *sourceDuringInter, CVector *targetDuringInter, CVector *upDuringInter, float *FOVDuringInter);

    void TakeControl(CEntity *target, eCamMode modeToGoTo, eSwitchType switchType, int32 whoIsInControlOfTheCamera);
    void TakeControlNoEntity(const CVector& fixedModeVector, eSwitchType switchType, int32 whoIsInControlOfTheCamera);
    void TakeControlAttachToEntity(CEntity* target, CEntity* attached, CVector* attachedCamOffset, CVector* attachedCamLookAt, float attachedCamAngle, eSwitchType switchType, int32 whoIsInControlOfTheCamera);
    void TakeControlWithSpline(eSwitchType switchType);

    bool TryToStartNewCamMode(int32 camSequence);

    void UpdateAimingCoors(const CVector& aimingTargetCoors);
    void UpdateSoundDistances();
    void UpdateTargetEntity();
    bool Using1stPersonWeaponMode() const;

    bool VectorMoveRunning();
    void VectorMoveLinear(CVector* moveLinearPosnEnd, CVector* moveLinearPosnStart, float duration, bool bMoveLinearWithEase);
    bool VectorTrackRunning();
    void VectorTrackLinear(CVector* trackLinearStartPoint, CVector* trackLinearEndPoint, float duration, bool bEase);

    void AllowShootingWith2PlayersInCar(bool bAllow);
    void ApplyVehicleCameraTweaks(CVehicle* vehicle);
    void AvoidTheGeometry(const CVector* arg2, const CVector* arg3, CVector* arg4, float FOV);

    void CalculateDerivedValues(bool bForMirror, bool bOriented);
    void CalculateFrustumPlanes(bool bForMirror);
    float CalculateGroundHeight(eGroundHeightType type);
    void CalculateMirroredMatrix(CVector posn, float mirrorV, CMatrix* camMatrix, CMatrix* mirrorMatrix);
    void CamControl();

    void AddShake(float duration, float a2, float a3, float a4, float a5);
    void AddShakeSimple(float duration, int32 type, float intensity);
    void CamShake(float strength, CVector from);
    void CameraColDetAndReact(CVector* source, CVector* target);
    void CameraGenericModeSpecialCases(CPed* targetPed);
    void CameraPedAimModeSpecialCases(CPed* ped);
    void CameraPedModeSpecialCases();
    void CameraVehicleModeSpecialCases(CVehicle* vehicle);
    void ClearPlayerWeaponMode();
    bool ConeCastCollisionResolve(const CVector& pos, const CVector& lookAt, CVector& outDest, float rad, float minDist, float& outDist);
    bool ConsiderPedAsDucking(CPed* ped);
    void CopyCameraMatrixToRWCam(bool bUpdateMatrix);
    void DealWithMirrorBeforeConstructRenderList(bool bActiveMirror, CVector mirrorNormal, float mirrorV, CMatrix* matMirror);
    void DeleteCutSceneCamDataMemory();
    void DrawBordersForWideScreen();

    void Enable1rstPersonCamCntrlsScript();
    void Enable1rstPersonWeaponsCamera();

    void Fade(float duration, eFadeFlag direction);
    void Find3rdPersonCamTargetVector(float range, CVector vecGunMuzzle, CVector& outSource, CVector& outTarget);
    float Find3rdPersonQuickAimPitch() const;
    float FindCamFOV() const;
    void FinishCutscene();

    bool GetArrPosForVehicleType(eVehicleType type, int32& arrPos);
    uint32 GetCutSceneFinishTime();
    [[nodiscard]] bool GetFading() const;
    [[nodiscard]] int32 GetFadingDirection() const;
    CVector* GetGameCamPosition();
    int32 GetLookDirection() const;
    bool GetLookingForwardFirstPerson() const;
    bool GetLookingLRBFirstPerson() const;
    [[nodiscard]] float GetPositionAlongSpline() const;
    float GetRoughDistanceToGround();
    [[nodiscard]] enum eNameState GetScreenFadeStatus() const;
    void GetScreenRect(CRect* rect) const;
    [[nodiscard]] bool Get_Just_Switched_Status() const;

    void HandleCameraMotionForDucking(CPed* ped, CVector* source, CVector* targPosn, bool arg5);
    void HandleCameraMotionForDuckingDuringAim(CPed* ped, CVector* source, CVector* targPosn, bool arg5);
    void ImproveNearClip(CVehicle* vehicle, CPed* ped, CVector* source, CVector* targPosn);

    bool ShouldPedControlsBeRelative();
    void SetToSphereMap(float);
    float GetCutsceneBarHeight();
    int32 GetCamDirectlyBehind();

    auto& GetActiveCam() { return m_aCams[m_nActiveCam]; }
    
    /*!
    * @addr notsa
    * @brief Get frustum points of the camera in world space: top left, top right, bottom right, bottom left + the center (0, 0, 0)
    */
    auto GetFrustumPoints() -> std::array<CVector, 5>;

    //! Get the camera's front normal (Whatever that is)
    auto GetFrontNormal2D() const { return CVector2D{ m_fCamFrontXNorm, m_fCamFrontYNorm }; }

public:
    static CCam& GetActiveCamera(); // TODO: Replace this with `TheCamera.GetActiveCam()`

    RwMatrix* GetRwMatrix() { return RwFrameGetMatrix(RwCameraGetFrame(m_pRwCamera)); }
    CMatrix& GetViewMatrix() { return m_mViewMatrix; }

    bool IsSphereVisibleInMirror(const CVector& origin, float radius) {
        return m_bMirrorActive && IsSphereVisible(origin, radius, (RwMatrix*)&m_mMatMirrorInverse);
    }
};
VALIDATE_SIZE(CCamera, 0xD78);

extern CCamera& TheCamera;
extern bool& gbModelViewer;
extern int8& gbCineyCamMessageDisplayed;
extern bool& gPlayerPedVisible;
extern uint8& gCurCamColVars;
extern int32& gCameraDirection;
extern eCamMode& gCameraMode;
extern uint32& gLastTime2PlayerCameraWasOK;
extern uint32& gLastTime2PlayerCameraCollided;
extern float*& gpCamColVars;
extern float (&gCamColVars)[28][6];
static inline auto& gpMadeInvisibleEntities = StaticRef<std::array<CEntity*, 10>, 0x9655A0>();
static inline auto& gNumEntitiesSetInvisible = StaticRef<uint32, 0x9655DC>();

void CamShakeNoPos(CCamera* camera, float strength);
