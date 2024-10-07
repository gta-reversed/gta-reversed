/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Base.h"

#include "eCamMode.h"
#include "Vector.h"

/* http://code.google.com/p/mtasa-blue/source/browse/tags/1.3.4/MTA10/game_sa/CCamSA.h */

class CEntity;
class CPed;
class CVehicle;

extern bool& gbFirstPersonRunThisFrame;

class CCam {
public:
    bool      m_bBelowMinDist;
    bool      m_bBehindPlayerDesired;
    bool      m_bCamLookingAtVector;
    bool      m_bCollisionChecksOn;
    bool      m_bFixingBeta;
    bool      m_bTheHeightFixerVehicleIsATrain;
    bool      m_bLookBehindCamWasInFront;
    bool      m_bLookingBehind;
    bool      m_bLookingLeft;
    bool      m_bLookingRight;
    bool      m_bResetStatics;
    bool      m_bRotating;
    eCamMode  m_nMode;
    uint32    m_nFinishTime;
    uint32    m_nDoCollisionChecksOnFrameNum;
    uint32    m_nDoCollisionCheckEveryNumOfFrames;
    uint32    m_nFrameNumWereAt;
    uint32    m_nRunningVectorArrayPos;
    uint32    m_nRunningVectorCounter;
    uint32    m_nDirectionWasLooking;
    float     m_fMaxRoleAngle;
    float     m_fRoll;
    float     m_fRollSpeed;
    float     m_fSyphonModeTargetZOffSet;
    float     m_fAmountFractionObscured;
    float     m_fAlphaSpeedOverOneFrame;
    float     m_fBetaSpeedOverOneFrame;
    float     m_fBufferedTargetBeta;
    float     m_fBufferedTargetOrientation;
    float     m_fBufferedTargetOrientationSpeed;
    float     m_fCamBufferedHeight;
    float     m_fCamBufferedHeightSpeed;
    float     m_fCloseInPedHeightOffset;
    float     m_fCloseInPedHeightOffsetSpeed;
    float     m_fCloseInCarHeightOffset;
    float     m_fCloseInCarHeightOffsetSpeed;
    float     m_fDimensionOfHighestNearCar;
    float     m_fDistanceBeforeChanges;
    float     m_fFovSpeedOverOneFrame;
    float     m_fMinDistAwayFromCamWhenInterPolating;
    float     m_fPedBetweenCameraHeightOffset;
    float     m_fPlayerInFrontSyphonAngleOffSet;
    float     m_fRadiusForDead;
    float     m_fRealGroundDist;
    float     m_fTargetBeta;
    float     m_fTimeElapsedFloat;
    float     m_fTilt;
    float     m_fTiltSpeed;
    float     m_fTransitionBeta;
    float     m_fTrueBeta;
    float     m_fTrueAlpha;
    float     m_fInitialPlayerOrientation;
    float     m_fVerticalAngle;  ///< The radian angle ([-pi/2, pi/2]) relative to the entity the camera is locked on (the player usually)
    float     m_fAlphaSpeed;
    float     m_fFOV;
    float     m_fFOVSpeed;
    float     m_fHorizontalAngle;
    float     m_fBetaSpeed;
    float     m_fDistance;
    float     m_fDistanceSpeed;
    float     m_fCaMinDistance;
    float     m_fCaMaxDistance;
    float     m_fSpeedVar;
    float     m_fCameraHeightMultiplier;
    float     m_fTargetZoomGroundOne;
    float     m_fTargetZoomGroundTwo;
    float     m_fTargetZoomGroundThree;
    float     m_fTargetZoomOneZExtra;
    float     m_fTargetZoomTwoZExtra;
    float     m_fTargetZoomTwoInteriorZExtra;
    float     m_fTargetZoomThreeZExtra;
    float     m_fTargetZoomZCloseIn;
    float     m_fMinRealGroundDist;
    float     m_fTargetCloseInDist;
    float     m_fBeta_Targeting;
    float     m_fX_Targetting;
    float     m_fY_Targetting;
    CVehicle* m_pCarWeAreFocussingOn;
    CVehicle* m_pCarWeAreFocussingOnI;
    float     m_fCamBumpedHorz;
    float     m_fCamBumpedVert;
    uint32    m_nCamBumpedTime; // TODO: Probably float
    CVector   m_vecSourceSpeedOverOneFrame;
    CVector   m_vecTargetSpeedOverOneFrame;
    CVector   m_vecUpOverOneFrame;
    CVector   m_vecTargetCoorsForFudgeInter;
    CVector   m_vecCamFixedModeVector;
    CVector   m_vecCamFixedModeSource;
    CVector   m_vecCamFixedModeUpOffSet;
    CVector   m_vecLastAboveWaterCamPosition;
    CVector   m_vecBufferedPlayerBodyOffset;
    CVector   m_vecFront;
    CVector   m_vecSource;
    CVector   m_vecSourceBeforeLookBehind;
    CVector   m_vecUp;
    CVector   m_avecPreviousVectors[2];
    CVector   m_avecTargetHistoryPos[4];
    uint32    m_anTargetHistoryTime[4];
    uint32    m_nCurrentHistoryPoints;
    CEntity*  m_pCamTargetEntity;
    float     m_fCameraDistance;
    float     m_fIdealAlpha;
    float     m_fPlayerVelocity;
    CVehicle* m_pLastCarEntered;
    CPed*     m_pLastPedLookedAt;
    bool      m_bFirstPersonRunAboutActive;

public:
    static void InjectHooks();

    CCam();
    CCam* Constructor();

    void Init();

    void CacheLastSettingsDWCineyCam();
    void DoCamBump(float horizontal, float vertical);
    void Finalise_DW_CineyCams(CVector*, CVector*, float, float, float, float);
    void GetCoreDataForDWCineyCamMode(CEntity**, CVehicle**, CVector*, CVector*, CVector*, CVector*, CVector*, CVector*, float*, CVector*, float*, CColSphere*);
    void GetLookFromLampPostPos(CEntity*, CPed*, CVector&, CVector&);
    void GetVectorsReadyForRW();
    void Get_TwoPlayer_AimVector(CVector&);
    void IsTimeToExitThisDWCineyCamMode(int32, CVector*, CVector*, float, bool);
    void KeepTrackOfTheSpeed(const CVector&, const CVector&, const CVector&, const float&, const float&, const float&);
    void LookBehind();
    void LookRight(bool bLookRight);
    void RotCamIfInFrontCar(const CVector&, float);
    bool Using3rdPersonMouseCam();
    bool GetWeaponFirstPersonOn();

    bool Process();
    bool ProcessArrestCamOne();
    bool ProcessPedsDeadBaby();
    bool Process_1rstPersonPedOnPC(const CVector&, float, float, float);
    bool Process_1stPerson(const CVector&, float, float, float);
    bool Process_AimWeapon(const CVector&, float, float, float);
    bool Process_AttachedCam();
    bool Process_Cam_TwoPlayer();
    bool Process_Cam_TwoPlayer_InCarAndShooting();
    bool Process_Cam_TwoPlayer_Separate_Cars();
    bool Process_Cam_TwoPlayer_Separate_Cars_TopDown();
    bool Process_DW_BirdyCam(bool);
    bool Process_DW_CamManCam(bool);
    bool Process_DW_HeliChaseCam(bool);
    bool Process_DW_PlaneCam1(bool);
    bool Process_DW_PlaneCam2(bool);
    bool Process_DW_PlaneCam3(bool);
    bool Process_DW_PlaneSpotterCam(bool);
    bool Process_Editor(const CVector&, float, float, float);
    bool Process_Fixed(const CVector&, float, float, float);
    bool Process_FlyBy(const CVector&, float, float, float);
    bool Process_FollowCar_SA(const CVector&, float, float, float, bool);
    bool Process_FollowPedWithMouse(const CVector&, float, float, float);
    bool Process_FollowPed_SA(const CVector&, float, float, float, bool);
    bool Process_M16_1stPerson(const CVector&, float, float, float);
    bool Process_Rocket(const CVector&, float, float, float, bool);
    bool Process_SpecialFixedForSyphon(const CVector&, float, float, float);
    bool Process_WheelCam(const CVector&, float, float, float);
};

VALIDATE_SIZE(CCam, 0x238);
